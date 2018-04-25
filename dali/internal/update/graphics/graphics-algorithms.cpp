/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include <dali/internal/update/graphics/graphics-algorithms.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <glm/glm.hpp>
// EXTERNAL INCLUDES

#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-frame.h>
#include <dali/graphics-api/graphics-api-render-command.h>


// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-instruction.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
Graphics::API::RenderCommand& BuildRenderCommand(
  Graphics::API::Controller&           graphics,
  Graphics::API::Frame&                frame,
  BufferIndex                          bufferIndex,

  Matrix                               viewProjection,
  const RenderList&                    renderItemList,
  Graphics::API::RenderCommand&        command
)
{
  /*
  // per item/renderer
  //const auto& renderer = renderItemList.GetRenderer(0);
  const auto& item = renderItemList.GetItem(0);

  // scenegraph renderer should have complete data
  const auto sgRenderer = item.mNode->GetRendererAt(0);


  const auto& renderer = *item.mRenderer;
  const auto& dataProviderConst = renderer.GetRenderDataProvider();
  auto& dataProvider = const_cast<RenderDataProvider&>( dataProviderConst );
  const auto& uniformMap = dataProvider.GetUniformMap().GetUniformMap( bufferIndex );

  // get resources
  const auto& shader = sgRenderer->GetShader();
  const auto& textures = sgRenderer->GetTextures();
  // todo: samplers
  //const auto& samplers = sgRenderer->Get;


  // prepare vertex buffers
  const auto& vertexBuffers = sgRenderer->GetGeometry()->GetVertexBuffers();
  std::vector<Graphics::API::RenderCommand::VertexAttributeBufferBinding> vertexAttributeBindings;
  auto attribLocation = 0u;
  auto bindingIndex = 0u;

  for( auto&& vertexBuffer : vertexBuffers )
  {
    auto attributeCountInForBuffer = vertexBuffer->GetAttributeCount();

    for( auto i = 0u; i < attributeCountInForBuffer; ++i )
    {
      // create binding per attribute
      auto binding = Graphics::API::RenderCommand::VertexAttributeBufferBinding{}
        .SetOffset( (vertexBuffer->GetFormat()->components[i]).offset )
        .SetBinding( bindingIndex )
        .SetBuffer( vertexBuffer->GetGfxObject() )
        .SetInputAttributeRate( Graphics::API::RenderCommand::InputAttributeRate::PER_VERTEX )
        .SetLocation( attribLocation + i )
        .SetStride( vertexBuffer->GetFormat()->size );

      vertexAttributeBindings.emplace_back( std::move(binding) );
    }
  }

  // prepare uniforms
  auto gfxShader = shader.GetGfxObject();

  // find mapped uniforms
*/
  return command;
}

static constexpr float CLIP_MATRIX_DATA[] = {
  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f
};
static const Matrix CLIP_MATRIX(CLIP_MATRIX_DATA);


void SubmitRenderItemList( Graphics::API::Controller&           graphics,
                           Graphics::API::Frame&                frame,
                           BufferIndex                          bufferIndex,
                           Graphics::API::RenderCommandBuilder& commandBuilder,
                           Matrix                               viewProjection,
                           RenderInstruction&                   instruction,
                           const RenderList&                    renderItemList )
{
  // TODO: @todo Set shaders and other properties
  //commandBuilder.Set( );

  // TODO: @todo Clipping...
  //using InternalTextureSet = Dali::Internal::SceneGraph::TextureSet;
  auto numberOfRenderItems = renderItemList.Count();

  const auto viewMatrix = instruction.GetViewMatrix( bufferIndex );
  const auto projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );

  Matrix vulkanProjectionMatrix;
  Matrix::Multiply( vulkanProjectionMatrix, *projectionMatrix, CLIP_MATRIX );

  std::vector<Graphics::API::RenderCommand*> commandList;
  for( auto i = 0u; i < numberOfRenderItems; ++i )
  {
    auto& item = renderItemList.GetItem( i );
    auto sgRenderer = item.mNode->GetRendererAt(0);
    auto& cmd = sgRenderer->GetGfxRenderCommand();

    Matrix mvp;
    Matrix::Multiply( mvp, item.mModelMatrix, viewProjection );
    sgRenderer->WriteUniform( "uModelMatrix", item.mModelMatrix );
    sgRenderer->WriteUniform( "uMvpMatrix", mvp );
    sgRenderer->WriteUniform( "uViewMatrix", *viewMatrix );
    sgRenderer->WriteUniform( "uModelViewMatrix", item.mModelViewMatrix );
    sgRenderer->WriteUniform( "uProjection", vulkanProjectionMatrix );
    sgRenderer->WriteUniform( "uSize", item.mSize );
    sgRenderer->WriteUniform( "uColor", item.mNode->GetWorldColor( bufferIndex ));
    commandList.push_back( &cmd  );
  }

  graphics.SubmitCommands( std::move(commandList) );
}

void SubmitInstruction( Graphics::API::Controller& graphics,
                        Graphics::API::Frame&      frame,
                        BufferIndex                bufferIndex,
                        RenderInstruction&         instruction )
{
  using namespace Graphics::API;

  // Create constant buffer with static uniforms: view matrix, projection matrix

  // TODO: @todo: buffer for constant uniforms
  /*
  auto contantUniforms = BuildBuffer<ProjectionMatrix, ViewMatrix>();
  constantUniforms["uProjection"] = projectionMatrix;
  constantUniforms["uViewMatrix"] = viewMatrix;
  */

  const Matrix* viewMatrix       = instruction.GetViewMatrix( bufferIndex );
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );
  Matrix        viewProjection;
  Matrix::Multiply( viewProjection, *viewMatrix, *projectionMatrix );

  auto commandBuilder = RenderCommandBuilder{};

  auto numberOfRenderLists = instruction.RenderListCount();
  for( auto i = 0u; i < numberOfRenderLists; ++i )
  {
    SubmitRenderItemList(
      graphics, frame, bufferIndex, commandBuilder, viewProjection, instruction, *instruction.GetRenderList( i ) );
  }
}
} // namespace

void SubmitRenderInstructions( Graphics::API::Controller&  graphics,
                               RenderInstructionContainer& renderInstructions,
                               BufferIndex                 bufferIndex )
{
  auto frame = Graphics::API::Frame{};
  graphics.BeginFrame();


  auto numberOfInstructions = renderInstructions.Count( bufferIndex );
  for( size_t i = 0; i < numberOfInstructions; ++i )
  {
    RenderInstruction& instruction = renderInstructions.At( bufferIndex, i );

    SubmitInstruction( graphics, frame, bufferIndex, instruction );
  }

  graphics.EndFrame( );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#pragma GCC diagnostic pop
