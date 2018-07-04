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
#include <dali/internal/update/nodes/scene-graph-layer.h>

// EXTERNAL INCLUDES
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
#include "../../../graphics-api/graphics-api-types.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{

static constexpr float CLIP_MATRIX_DATA[] = {
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, -1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, -0.5f, 0.0f,
  0.0f, 0.0f, 0.5f, 1.0f
};
static const Matrix CLIP_MATRIX(CLIP_MATRIX_DATA);



constexpr Graphics::API::BlendFactor ConvertBlendFactor( BlendFactor::Type blendFactor )
{
  switch( blendFactor )
  {
    case BlendFactor::ZERO:
      return Graphics::API::BlendFactor::ZERO;
    case BlendFactor::ONE:
      return Graphics::API::BlendFactor::ONE;
    case BlendFactor::SRC_COLOR:
      return Graphics::API::BlendFactor::SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
      return Graphics::API::BlendFactor::ONE_MINUS_SRC_COLOR;
    case BlendFactor::SRC_ALPHA:
      return Graphics::API::BlendFactor::SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
      return Graphics::API::BlendFactor::ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DST_ALPHA:
      return Graphics::API::BlendFactor::DST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
      return Graphics::API::BlendFactor::ONE_MINUS_DST_ALPHA;
    case BlendFactor::DST_COLOR:
      return Graphics::API::BlendFactor::DST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOR:
      return Graphics::API::BlendFactor::ONE_MINUS_DST_COLOR;
    case BlendFactor::SRC_ALPHA_SATURATE:
      return Graphics::API::BlendFactor::SRC_ALPHA_SATURATE;
    case BlendFactor::CONSTANT_COLOR:
      return Graphics::API::BlendFactor::CONSTANT_COLOR;
    case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
      return Graphics::API::BlendFactor::ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::CONSTANT_ALPHA:
      return Graphics::API::BlendFactor::CONSTANT_ALPHA;
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
      return Graphics::API::BlendFactor::ONE_MINUS_CONSTANT_ALPHA;
  }
  return Graphics::API::BlendFactor{};
}

constexpr Graphics::API::BlendOp ConvertBlendEquation( BlendEquation::Type blendEquation )
{
  switch( blendEquation )
  {
    case BlendEquation::ADD:
      return Graphics::API::BlendOp::ADD;
    case BlendEquation::SUBTRACT:
      return Graphics::API::BlendOp::SUBTRACT;
    case BlendEquation::REVERSE_SUBTRACT:
      return Graphics::API::BlendOp::REVERSE_SUBTRACT;
  }
  return Graphics::API::BlendOp{};
}

void SubmitRenderItemList( Graphics::API::Controller&           graphics,
                           BufferIndex                          bufferIndex,
                           Matrix                               viewProjection,
                           RenderInstruction&                   instruction,
                           const RenderList&                    renderItemList )
{
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
    auto color = item.mNode->GetWorldColor( bufferIndex );
    auto renderer = item.mRenderer;

    auto &cmd = renderer->GetGfxRenderCommand();
    if (cmd.GetVertexBufferBindings()
           .empty())
    {
      continue;
    }
    cmd.BindRenderTarget(renderTargetBinding);

    auto opacity = renderer->GetOpacity( bufferIndex );

    if( renderer->IsPreMultipliedAlphaEnabled() )
    {
      float alpha = color.a * opacity;
      color = Vector4( color.r * alpha, color.g * alpha, color.b * alpha, alpha );
    }
    else
    {
      color.a *= opacity;
    }

    Matrix mvp, mvp2;
    Matrix::Multiply(mvp, item.mModelMatrix, viewProjection);
    Matrix::Multiply(mvp2, mvp, CLIP_MATRIX);
    renderer->WriteUniform("uModelMatrix", item.mModelMatrix);
    renderer->WriteUniform("uMvpMatrix", mvp2);
    renderer->WriteUniform("uViewMatrix", *viewMatrix);
    renderer->WriteUniform("uModelView", item.mModelViewMatrix);

    Matrix3 uNormalMatrix( item.mModelViewMatrix );
    uNormalMatrix.Invert();
    uNormalMatrix.Transpose();

    renderer->WriteUniform("uNormalMatrix", uNormalMatrix);
    renderer->WriteUniform("uProjection", vulkanProjectionMatrix);
    renderer->WriteUniform("uSize", item.mSize);
    renderer->WriteUniform("uColor", color );

    commandList.push_back(&cmd);
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



bool PrepareGraphicsPipeline( Graphics::API::Controller& controller,
                              RenderInstruction& instruction,
                              const RenderList* renderList,
                              RenderItem& item,
                              BufferIndex bufferIndex )
{
  using namespace Dali::Graphics::API;

  // for each renderer within node
  // vertex input state
  VertexInputState vi{};

  auto *renderer = item.mRenderer;
  auto *geometry = renderer->GetGeometry();
  auto gfxShader = renderer->GetShader()
                           .GetGfxObject();

  if( !gfxShader )
  {
    return false;
  }
  /**
   * Prepare vertex attribute buffer bindings
   */
  uint32_t                                                    bindingIndex{0u};
  std::vector<Graphics::API::Buffer*> vertexBuffers{};

  for (auto &&vertexBuffer : geometry->GetVertexBuffers())
  {
    vertexBuffers.push_back(vertexBuffer->GetGfxObject());
    auto attributeCountInForBuffer = vertexBuffer->GetAttributeCount();

    // update vertex buffer if necessary
    vertexBuffer->Update(controller);

    // store buffer binding
    vi.bufferBindings
      .emplace_back(vertexBuffer->GetFormat()
                                ->size, VertexInputRate::PER_VERTEX);

    for (auto i = 0u; i < attributeCountInForBuffer; ++i)
    {
      // create attribute description
      vi.attributes
        .emplace_back(
          gfxShader->GetVertexAttributeLocation(vertexBuffer->GetAttributeName(i)),
          bindingIndex, (vertexBuffer->GetFormat()
                                     ->components[i]).offset,
          VertexInputFormat::UNDEFINED);

    }
    bindingIndex++;
  }

  // set optional index buffer
  auto topology         = PrimitiveTopology::TRIANGLE_STRIP;
  auto geometryTopology = geometry->GetType();
  switch (geometryTopology)
  {
    case Dali::Geometry::Type::TRIANGLE_STRIP:
    {
      topology = PrimitiveTopology::TRIANGLE_STRIP;
      break;
    }
    default:
    {
      topology = PrimitiveTopology::TRIANGLE_LIST;
    }
  }

  /**
   * 1. DEPTH MDOE
   */
  // use correct depth mode
  DepthStencilState depthStencilState;
  depthStencilState.SetDepthCompareOp( CompareOp::GREATER );

  const bool depthTestEnable( !renderList->GetSourceLayer()->IsDepthTestDisabled() && renderList->HasColorRenderItems() );
  depthStencilState.SetDepthTestEnable( depthTestEnable );

  const bool enableDepthWrite = ( ( renderer->GetDepthWriteMode() == DepthWriteMode::AUTO )
                                  && depthTestEnable && item.mIsOpaque ) ||
                                ( renderer->GetDepthWriteMode() == DepthWriteMode::ON );

  depthStencilState.SetDepthWriteEnable( enableDepthWrite );

  /**
   * 2. BLENDING
   */
  ColorBlendState colorBlendState{};
  colorBlendState.SetBlendEnable(false);
  if( renderer->GetBlendMode() != BlendMode::OFF)
  {
    colorBlendState.SetBlendEnable(true);
    const auto& options = renderer->GetBlendingOptions();
    colorBlendState
      .SetSrcColorBlendFactor(ConvertBlendFactor(options.GetBlendSrcFactorRgb()))
      .SetSrcAlphaBlendFactor(ConvertBlendFactor(options.GetBlendSrcFactorAlpha()))
      .SetDstColorBlendFactor(ConvertBlendFactor(options.GetBlendDestFactorRgb()))
      .SetDstAlphaBlendFactor(ConvertBlendFactor(options.GetBlendDestFactorAlpha()))
      .SetColorBlendOp(ConvertBlendEquation(options.GetBlendEquationRgb()))
      .SetAlphaBlendOp(ConvertBlendEquation(options.GetBlendEquationAlpha()));
  }

  /**
   * 3. VIEWPORT
   */
  ViewportState viewportState{};
  if (instruction.mIsViewportSet)
  {
    viewportState.SetViewport({float(instruction.mViewport
                                                .x), float(instruction.mViewport
                                                                      .y),
                                float(instruction.mViewport
                                                 .width), float(instruction.mViewport
                                                                           .height),
                                0.0, 1.0});
  }
  else
  {
    viewportState.SetViewport({0.0, 0.0, 0.0, 0.0,
                                0.0, 1.0});
  }


  // create pipeline
  auto pipeline = controller.CreatePipeline(controller.GetPipelineFactory()

              // vertex input
            .SetVertexInputState(vi)

              // shaders
            .SetShaderState(ShaderState()
                              .SetShaderProgram(*gfxShader))

              // input assembly
            .SetInputAssemblyState(InputAssemblyState()
                                     .SetTopology(topology)
                                     .SetPrimitiveRestartEnable(true))

              // viewport ( if zeroes then framebuffer size used )
            .SetViewportState(viewportState)

              // depth stencil
            .SetDepthStencilState(depthStencilState)


              // color blend
            .SetColorBlendState(colorBlendState
                                  .SetColorComponentsWriteBits(0xff)
                                  .SetLogicOpEnable(false))

              // rasterization
            .SetRasterizationState(RasterizationState()
                                     .SetCullMode(CullMode::BACK)
                                     .SetPolygonMode(PolygonMode::FILL)
                                     .SetFrontFace(FrontFace::COUNTER_CLOCKWISE)));

  // bind pipeline to the renderer
  renderer->BindPipeline(std::move(pipeline));

  return true;
}

void PrepareRendererPipelines( Graphics::API::Controller& controller,
                               RenderInstructionContainer& renderInstructions,
                               BufferIndex bufferIndex )
{
  for( auto i = 0u; i < renderInstructions.Count( bufferIndex ); ++i )
  {
    RenderInstruction &ri = renderInstructions.At(bufferIndex, i);
    for (auto renderListIndex = 0u; renderListIndex < ri.RenderListCount(); ++renderListIndex)
    {
      const auto *renderList = ri.GetRenderList(renderListIndex);
      for (auto renderItemIndex = 0u; renderItemIndex < renderList->Count(); ++renderItemIndex)
      {
        auto &item = renderList->GetItem(renderItemIndex);
        PrepareGraphicsPipeline(controller, ri, renderList, item, bufferIndex);
      }
    }
  }
}

void SubmitRenderInstructions( Graphics::API::Controller&  controller,
                               RenderInstructionContainer& renderInstructions,
                               BufferIndex                 bufferIndex )
{
  PrepareRendererPipelines( controller, renderInstructions, bufferIndex );

  auto numberOfInstructions = renderInstructions.Count( bufferIndex );

  controller.BeginFrame();

  for( size_t i = 0; i < numberOfInstructions; ++i )
  {
    RenderInstruction& instruction = renderInstructions.At( bufferIndex, i );

    SubmitInstruction( controller, bufferIndex, instruction );
  }

  controller.EndFrame();
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

