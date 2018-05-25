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

// EXTERNAL INCLUDES
#include <glm/glm.hpp>
#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-render-command.h>

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/rendering/render-instruction-container.h>
#include <dali/internal/update/rendering/render-instruction.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-shader.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{

static constexpr float CLIP_MATRIX_DATA[] = {
  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f
};
static const Matrix CLIP_MATRIX(CLIP_MATRIX_DATA);


void SubmitRenderItemList( Graphics::API::Controller&           graphics,
                           BufferIndex                          bufferIndex,
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

  // @todo: this should be probably separated ???
  auto renderTargetBinding = Graphics::API::RenderCommand::RenderTargetBinding{}
  .SetClearColors( {
                     { instruction.mClearColor.r,
                       instruction.mClearColor.g,
                       instruction.mClearColor.b,
                       instruction.mClearColor.a
                     }});


  for( auto i = 0u; i < numberOfRenderItems; ++i )
  {
    auto& item = renderItemList.GetItem( i );
    auto sgRenderer = item.mNode->GetRendererAt(0);
    auto& cmd = sgRenderer->GetGfxRenderCommand();
    if(cmd.GetVertexBufferBindings().empty())
    {
      continue;
    }
    cmd.BindRenderTarget( renderTargetBinding );
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
                        BufferIndex                bufferIndex,
                        RenderInstruction&         instruction )
{
  using namespace Graphics::API;

  // Create constant buffer with static uniforms: view matrix, projection matrix
  const Matrix* viewMatrix       = instruction.GetViewMatrix( bufferIndex );
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );
  Matrix        viewProjection;
  Matrix::Multiply( viewProjection, *viewMatrix, *projectionMatrix );

  auto numberOfRenderLists = instruction.RenderListCount();
  for( auto i = 0u; i < numberOfRenderLists; ++i )
  {
    SubmitRenderItemList(
      graphics, bufferIndex, viewProjection, instruction, *instruction.GetRenderList( i ) );
  }
}
} // namespace

void SubmitRenderInstructions( Graphics::API::Controller&  graphics,
                               RenderInstructionContainer& renderInstructions,
                               BufferIndex                 bufferIndex )
{
  graphics.BeginFrame();


  auto numberOfInstructions = renderInstructions.Count( bufferIndex );
  for( size_t i = 0; i < numberOfInstructions; ++i )
  {
    RenderInstruction& instruction = renderInstructions.At( bufferIndex, i );

    SubmitInstruction( graphics, bufferIndex, instruction );
  }

  graphics.EndFrame( );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

