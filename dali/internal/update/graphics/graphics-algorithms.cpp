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
#include <dali/graphics-api/graphics-api-framebuffer.h>

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/rendering/render-instruction-container.h>
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
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDERER" );
#endif

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

}

ClippingBox IntersectAABB( const ClippingBox& aabbA, const ClippingBox& aabbB )
{
  ClippingBox intersectionBox;

  // First calculate the largest starting positions in X and Y.
  intersectionBox.x = std::max( aabbA.x, aabbB.x );
  intersectionBox.y = std::max( aabbA.y, aabbB.y );

  // Now calculate the smallest ending positions, and take the largest starting
  // positions from the result, to get the width and height respectively.
  // If the two boxes do not intersect at all, then we need a 0 width and height clipping area.
  // We use max here to clamp both width and height to >= 0 for this use-case.
  intersectionBox.width =  std::max( std::min( aabbA.x + aabbA.width,  aabbB.x + aabbB.width  ) - intersectionBox.x, 0 );
  intersectionBox.height = std::max( std::min( aabbA.y + aabbA.height, aabbB.y + aabbB.height ) - intersectionBox.y, 0 );

  return intersectionBox;
}

bool GraphicsAlgorithms::SetupScissorClipping( const RenderItem& item)
{
  // Get the number of child scissors in the stack (do not include layer or root box).
  size_t childStackDepth = mScissorStack.size() - 1u;
  const uint32_t scissorDepth = item.mNode->GetScissorDepth();
  const bool clippingNode = item.mNode->GetClippingMode() == Dali::ClippingMode::CLIP_TO_BOUNDING_BOX;
  bool traversedUpTree = false;

  // If we are using scissor clipping and we are at the same depth (or less), we need to undo previous clips.
  // We do this by traversing up the scissor clip stack and then apply the appropriate clip for the current render item.
  // To know this, we use clippingDepth. This value is set on *every* node, but only increased as clipping nodes are hit depth-wise.
  // So we know if we are at depth 4 and the stackDepth is 5, that we have gone up.
  // If the depth is the same then we are effectively part of a different sub-tree from the parent, we must also remove the current clip.
  // Note: Stack depth must always be at least 1, as we will have the layer or stage size as the root value.
  if( ( childStackDepth > 0u ) && ( scissorDepth < childStackDepth ) )
  {
    while( scissorDepth < childStackDepth )
    {
      mScissorStack.pop_back();
      --childStackDepth;
    }

    // We traversed up the tree, we need to apply a new scissor rectangle (unless we are at the root).
    traversedUpTree = true;
  }
  if( clippingNode && childStackDepth > 0u && childStackDepth == scissorDepth ) // case of sibling clip area
  {
    mScissorStack.pop_back();
    --childStackDepth;
  }

  // If we are on a clipping node, or we have traveled up the tree and gone back past a clipping node, may need to apply a new scissor clip.
  if( clippingNode || traversedUpTree )
  {
    // First, check if we are a clipping node.
    if( clippingNode )
    {
      // This is a clipping node. We generate the AABB for this node and intersect it with the previous intersection further up the tree.
      // If the viewport hasn't been set, and we're rendering to a framebuffer, then
      // set the size of the viewport to that of the framebuffer.
      // Get the AABB bounding box for the current render item.
      const ClippingBox scissorBox( item.CalculateViewportSpaceAABB( mScissorStack[0].width, mScissorStack[0].height ) );

      // Get the AABB for the parent item that we must intersect with.
      const ClippingBox& parentBox( mScissorStack.back() );

      // We must reduce the clipping area based on the parents area to allow nested clips. This is a set intersection function.
      // We add the new scissor box to the stack so we can return to it if needed.
      mScissorStack.emplace_back( IntersectAABB( parentBox, scissorBox ) );
    }
  }

  return ( mScissorStack.size() > 0u );
}

bool GraphicsAlgorithms::SetupPipelineViewportState( Graphics::API::ViewportState& outViewportState )
{
  // The scissor test is enabled if we have any children on the stack, OR, if there are none but it is a user specified layer scissor box.
  // IE. It is not enabled if we are at the top of the stack and the layer does not have a specified clipping box.

  auto scissorEnabled = ( mScissorStack.size() > 0u );// || mHasLayerScissor;
  // If scissor is enabled, we use the calculated screen-space coordinates (now in the stack).
  if( scissorEnabled )
  {
    ClippingBox useScissorBox( mScissorStack.back() );
    outViewportState.SetScissorTestEnable( true  );
    outViewportState.SetScissor( { useScissorBox.x,
                                int32_t(outViewportState.viewport.height - (useScissorBox.y + useScissorBox.height) ),
                                uint32_t(useScissorBox.width),
                                uint32_t(useScissorBox.height) } );
  }
  else
  {
    outViewportState.SetScissorTestEnable( false );
    outViewportState.SetScissor({});
  }
  return scissorEnabled;
}

void GraphicsAlgorithms::SubmitRenderItemList(
  Graphics::API::Controller&           graphics,
  BufferIndex                          bufferIndex,
  Graphics::API::RenderCommand::RenderTargetBinding& renderTargetBinding,
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


  for( auto i = 0u; i < numberOfRenderItems; ++i )
  {
    auto& item = renderItemList.GetItem( i );
    auto renderer = item.mRenderer;
    if( !renderer )
    {
      continue;
    }
    auto color = item.mNode->GetWorldColor( bufferIndex );

    auto &cmd = renderer->GetGfxRenderCommand( bufferIndex );
    if (cmd.GetVertexBufferBindings()
           .empty())
    {
      continue;
    }
    cmd.BindRenderTarget(renderTargetBinding);

    float width = instruction.mViewport.width;
    float height = instruction.mViewport.height;

    // If the viewport hasn't been set, and we're rendering to a framebuffer, then
    // set the size of the viewport to that of the framebuffer.
    if( !instruction.mIsViewportSet && renderTargetBinding.framebuffer != nullptr )
    {
      width = renderTargetBinding.framebufferWidth;
      height = renderTargetBinding.framebufferHeight;
    }
    cmd.mDrawCommand.SetViewport( { float( instruction.mViewport.x ),
                                    float( instruction.mViewport.y ),
                                    width,
                                    height,
                                    0.0f , 1.0f } )
                    .SetViewportEnable( true );
    // Could set to false if we know that we can use the Pipeline viewport rather than the
    // dynamic viewport.

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
    renderer->WriteUniform( bufferIndex, "uModelMatrix", item.mModelMatrix);
    renderer->WriteUniform( bufferIndex, "uMvpMatrix", mvp2);
    renderer->WriteUniform( bufferIndex, "uViewMatrix", *viewMatrix);
    renderer->WriteUniform( bufferIndex, "uModelView", item.mModelViewMatrix);

    Matrix3 uNormalMatrix( item.mModelViewMatrix );
    uNormalMatrix.Invert();
    uNormalMatrix.Transpose();

    renderer->WriteUniform( bufferIndex, "uNormalMatrix", uNormalMatrix);
    renderer->WriteUniform( bufferIndex, "uProjection", vulkanProjectionMatrix);
    renderer->WriteUniform( bufferIndex, "uSize", item.mSize);
    renderer->WriteUniform( bufferIndex, "uColor", color );

    commandList.push_back(&cmd);
  }

  graphics.SubmitCommands( std::move(commandList) );
}

void GraphicsAlgorithms::SubmitInstruction( Graphics::API::Controller& graphics,
                        BufferIndex                bufferIndex,
                        RenderInstruction&         instruction )
{
  using namespace Graphics::API;

  // Create constant buffer with static uniforms: view matrix, projection matrix
  const Matrix* viewMatrix       = instruction.GetViewMatrix( bufferIndex );
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );
  Matrix        viewProjection;
  Matrix::Multiply( viewProjection, *viewMatrix, *projectionMatrix );


  auto renderTargetBinding = Graphics::API::RenderCommand::RenderTargetBinding{}
  .SetClearColors( {{ instruction.mClearColor.r,
                    instruction.mClearColor.g,
                    instruction.mClearColor.b,
                    instruction.mClearColor.a }} );

  if( !instruction.mIgnoreRenderToFbo )
  {
    if( instruction.mFrameBuffer != 0 )
    {
      renderTargetBinding.SetFramebuffer( instruction.mFrameBuffer->GetGfxObject());
    // Store the size of the framebuffer in case the viewport isn't set.
      renderTargetBinding.framebufferWidth = instruction.mFrameBuffer->GetWidth();
      renderTargetBinding.framebufferHeight = instruction.mFrameBuffer->GetHeight();
    }
  }

  auto numberOfRenderLists = instruction.RenderListCount();
  for( auto i = 0u; i < numberOfRenderLists; ++i )
  {
    SubmitRenderItemList( graphics, bufferIndex, renderTargetBinding,
                          viewProjection, instruction, *instruction.GetRenderList( i ) );
  }
}

bool GraphicsAlgorithms::PrepareGraphicsPipeline( Graphics::API::Controller& controller,
                              RenderInstruction& instruction,
                              const RenderList* renderList,
                              RenderItem& item,
                              BufferIndex bufferIndex )
{
  using namespace Dali::Graphics::API;

  // vertex input state
  VertexInputState vi{};

  auto *renderer = item.mRenderer;
  auto *geometry = renderer->GetGeometry();
  auto gfxShader = renderer->GetShader().GetGfxObject();

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
    case Dali::Geometry::Type::LINE_LOOP:
    case Dali::Geometry::Type::LINE_STRIP:
    {
      topology = PrimitiveTopology::LINE_STRIP;
      break;
    }
    case Dali::Geometry::Type::LINES:
    {
      topology = PrimitiveTopology::LINE_LIST;
      break;
    }
    case Dali::Geometry::Type::TRIANGLE_FAN:
    {
      topology = PrimitiveTopology::TRIANGLE_FAN;
      break;
    }
    case Dali::Geometry::Type::POINTS:
    {
      topology = PrimitiveTopology::POINT_LIST;
      break;
    }
    case Dali::Geometry::Type::TRIANGLES:
    {
      topology = PrimitiveTopology::TRIANGLE_LIST;
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

  const bool enableDepthWrite = ( ( renderer->GetDepthWriteMode() == DepthWriteMode::AUTO )
                                  && depthTestEnable && item.mIsOpaque ) ||
                                ( renderer->GetDepthWriteMode() == DepthWriteMode::ON );

  // Set up whether or not to read from (test) the depth buffer.
  const DepthTestMode::Type depthTestMode = item.mRenderer->GetDepthTestMode();

  // Most common mode (AUTO) is tested first.
  const bool enableDepthTest = ( ( depthTestMode == DepthTestMode::AUTO ) && depthTestEnable ) ||
                               ( depthTestMode == DepthTestMode::ON );

  depthStencilState.SetDepthTestEnable( enableDepthTest );
  depthStencilState.SetDepthWriteEnable( enableDepthWrite );

  /**
   * 2. BLENDING
   */
  ColorBlendState colorBlendState{};
  colorBlendState.SetBlendEnable(false);

  if( !item.mIsOpaque )
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

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "PrepareGraphicsPipeline(renderer=%p), opaque:%s, blendMode:%d  => Blending:%s\n", item.mRenderer, item.mIsOpaque?"T":"F", renderer->GetBlendMode(), colorBlendState.blendEnable?"T":"F" );
  /**
   * 3. VIEWPORT
   */
  ViewportState viewportState{};

  // Set viewport only when not using dynamic viewport state
  if( !renderer->GetGfxRenderCommand( bufferIndex ).GetDrawCommand().viewportEnable && instruction.mIsViewportSet )
  {
    // scissor test only when we have viewport
    viewportState.SetViewport({ float(instruction.mViewport.x), float(instruction.mViewport.y),
                                float(instruction.mViewport.width), float(instruction.mViewport.height),
                                0.0, 1.0});
  }
  else
  {
    // Use zero-size viewport for dynamic viewport or viewport-less state
    viewportState.SetViewport( { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 } );
  }

  FramebufferState framebufferState{};
  if( instruction.mFrameBuffer )
  {
    framebufferState.SetFramebuffer( *instruction.mFrameBuffer->GetGfxObject() );
  }

  /**
   * Scissor test is represented only by the dynamic state as it can be transformed
   * any time.
   */
  Graphics::API::PipelineDynamicStateMask dynamicStateMask{ 0u };

  if( SetupPipelineViewportState( viewportState) )
  {
    renderer->GetGfxRenderCommand( bufferIndex ).mDrawCommand.SetScissor( viewportState.scissor );
    renderer->GetGfxRenderCommand( bufferIndex ).mDrawCommand.SetScissorTestEnable( true );
    dynamicStateMask = Graphics::API::PipelineDynamicStateBits::SCISSOR_BIT;
  }
  else
  {
    renderer->GetGfxRenderCommand( bufferIndex ).mDrawCommand.SetScissorTestEnable( false );
  }

  // todo: make it possible to decide earlier whether we want dynamic or static viewport
  dynamicStateMask |= Graphics::API::PipelineDynamicStateBits::VIEWPORT_BIT;

  // reset pipeline's viewport to prevent hashing function changing due to animated values.
  viewportState.SetViewport({0.0, 0.0, 0.0, 0.0, 0.0, 1.0});

  // disable scissors per-pipeline
  viewportState.SetScissorTestEnable( false );
  viewportState.SetScissor( {} );

  // set face culling
  auto cullMode { CullMode::NONE };
  switch( renderer->GetFaceCullingMode() )
  {
    case FaceCullingMode::BACK:
    {
      cullMode = CullMode::BACK;
      break;
    }
    case FaceCullingMode::FRONT:
    {
      cullMode = CullMode::FRONT;
      break;
    }
    case FaceCullingMode::FRONT_AND_BACK:
    {
      cullMode = CullMode::FRONT_AND_BACK;
      break;
    }
    case FaceCullingMode::NONE:
    {
      cullMode = CullMode::NONE;
    }
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
                                     .SetPrimitiveRestartEnable(false))

            // viewport ( if zeroes then framebuffer size used )
            .SetViewportState(viewportState)

            .SetFramebufferState( framebufferState )

            // depth stencil
            .SetDepthStencilState(depthStencilState)

            // color blend
            .SetColorBlendState(colorBlendState
                                  .SetColorComponentsWriteBits(0xff)
                                  .SetLogicOpEnable(false))

            // rasterization
            .SetRasterizationState(RasterizationState()
                                     .SetCullMode(cullMode)
                                     .SetPolygonMode(PolygonMode::FILL)
                                     .SetFrontFace(FrontFace::COUNTER_CLOCKWISE))

            // dynamic state mask
            .SetDynamicStateMask( dynamicStateMask )
  );


  // bind pipeline to the render command
  renderer->BindPipeline( bufferIndex, std::move(pipeline) );
  return true;
}

void GraphicsAlgorithms::PrepareRendererPipelines( Graphics::API::Controller& controller,
                               RenderInstructionContainer& renderInstructions,
                               BufferIndex bufferIndex )
{
  for( auto i = 0u; i < renderInstructions.Count( bufferIndex ); ++i )
  {
    RenderInstruction &ri = renderInstructions.At(bufferIndex, i);

    for (auto renderListIndex = 0u; renderListIndex < ri.RenderListCount(); ++renderListIndex)
    {
      const auto *renderList = ri.GetRenderList(renderListIndex);
      mScissorStack.clear();
      mScissorStack.push_back( ri.mViewport );
      for (auto renderItemIndex = 0u; renderItemIndex < renderList->Count(); ++renderItemIndex)
      {
        auto &item = renderList->GetItem(renderItemIndex);

        // setup clipping for item
        SetupScissorClipping( item );

        if( item.mRenderer )
        {
          PrepareGraphicsPipeline( controller, ri, renderList, item, bufferIndex );
        }
      }
    }
  }
}

void GraphicsAlgorithms::SubmitRenderInstructions( Graphics::API::Controller&  controller,
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
