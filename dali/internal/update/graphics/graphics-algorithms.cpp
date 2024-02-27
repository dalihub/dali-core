/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-command-buffer.h>
#include <dali/graphics-api/graphics-framebuffer.h>

// INTERNAL INCLUDES
#include <dali/devel-api/common/hash.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/rendering/render-instruction-container.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-shader.h>
#include "../../../graphics-api/graphics-types.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
// Size of uniform buffer page used when resizing
constexpr uint32_t UBO_PAGE_SIZE = 8192u;

// UBO allocation threshold below which the UBO will shrink
constexpr auto UBO_SHRINK_THRESHOLD = 0.75f;

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

constexpr Graphics::BlendFactor ConvertBlendFactor( BlendFactor::Type blendFactor )
{
  switch( blendFactor )
  {
    case BlendFactor::ZERO:
      return Graphics::BlendFactor::ZERO;
    case BlendFactor::ONE:
      return Graphics::BlendFactor::ONE;
    case BlendFactor::SRC_COLOR:
      return Graphics::BlendFactor::SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_SRC_COLOR;
    case BlendFactor::SRC_ALPHA:
      return Graphics::BlendFactor::SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DST_ALPHA:
      return Graphics::BlendFactor::DST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_DST_ALPHA;
    case BlendFactor::DST_COLOR:
      return Graphics::BlendFactor::DST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_DST_COLOR;
    case BlendFactor::SRC_ALPHA_SATURATE:
      return Graphics::BlendFactor::SRC_ALPHA_SATURATE;
    case BlendFactor::CONSTANT_COLOR:
      return Graphics::BlendFactor::CONSTANT_COLOR;
    case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::CONSTANT_ALPHA:
      return Graphics::BlendFactor::CONSTANT_ALPHA;
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_CONSTANT_ALPHA;
  }
  return Graphics::BlendFactor{};
}

constexpr Graphics::BlendOp ConvertBlendEquation( BlendEquation::Type blendEquation )
{
  switch( blendEquation )
  {
    case BlendEquation::ADD:
      return Graphics::BlendOp::ADD;
    case BlendEquation::SUBTRACT:
      return Graphics::BlendOp::SUBTRACT;
    case BlendEquation::REVERSE_SUBTRACT:
      return Graphics::BlendOp::REVERSE_SUBTRACT;
  }
  return Graphics::BlendOp{};
}


struct HashedName
{
  std::string name;
  size_t hash;
};

constexpr Graphics::StencilOp ConvertStencilOp( StencilOperation::Type stencilOp )
{
  switch( stencilOp )
  {
    case StencilOperation::ZERO: return Graphics::StencilOp::ZERO;
    case StencilOperation::DECREMENT: return Graphics::StencilOp::DECREMENT_AND_CLAMP;
    case StencilOperation::DECREMENT_WRAP: return Graphics::StencilOp::DECREMENT_AND_WRAP;
    case StencilOperation::INCREMENT: return Graphics::StencilOp::INCREMENT_AND_CLAMP;
    case StencilOperation::INCREMENT_WRAP: return Graphics::StencilOp::INCREMENT_AND_WRAP;
    case StencilOperation::INVERT: return Graphics::StencilOp::INVERT;
    case StencilOperation::KEEP: return Graphics::StencilOp::KEEP;
    case StencilOperation::REPLACE: return Graphics::StencilOp::REPLACE;
  }
  return {};
}

constexpr Graphics::CompareOp ConvertStencilFunc( StencilFunction::Type stencilFunc )
{
  switch( stencilFunc )
  {
    case StencilFunction::NEVER: return Graphics::CompareOp::NEVER;
    case StencilFunction::LESS: return Graphics::CompareOp::LESS;
    case StencilFunction::EQUAL: return Graphics::CompareOp::EQUAL;
    case StencilFunction::LESS_EQUAL: return Graphics::CompareOp::LESS_OR_EQUAL;
    case StencilFunction::GREATER: return Graphics::CompareOp::GREATER;
    case StencilFunction::NOT_EQUAL: return Graphics::CompareOp::NOT_EQUAL;
    case StencilFunction::GREATER_EQUAL: return Graphics::CompareOp::GREATER_OR_EQUAL;
    case StencilFunction::ALWAYS: return Graphics::CompareOp::ALWAYS;
  }
  return {};
}

/**
 * Helper function writing data to the DALi shader default uniforms
 */
template<class T>
bool WriteDefaultUniform( Renderer* renderer, GraphicsBuffer& ubo, const std::vector<Graphics::UniformBufferBinding>& bindings, Shader::DefaultUniformIndex defaultUniformIndex, const T& data )
{
  auto info = renderer->GetShader().GetDefaultUniform( defaultUniformIndex );
  if( info )
  {
    renderer->WriteUniform( ubo, bindings, *info, data );
    return true;
  }
  return false;
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

GraphicsAlgorithms::GraphicsAlgorithms( Graphics::Controller& controller )
: mGraphicsController(controller)
{
  mUniformBufferManager.reset( new UniformBufferManager( &controller ) );
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

bool GraphicsAlgorithms::SetupStencilClipping( const RenderItem& item, uint32_t& lastClippingDepth, uint32_t& lastClippingId )
{
  const Dali::Internal::SceneGraph::Node* node = item.mNode;
  const uint32_t clippingId = node->GetClippingId();
  // If there is no clipping Id, then either we haven't reached a clipping Node yet, or there aren't any.
  // Either way we can skip clipping setup for this renderer.
  if( clippingId == 0u )
  {
    mCurrentStencilState = {};
    mCurrentStencilState.stencilTestEnable = false;
    // Exit immediately if there are no clipping actions to perform (EG. we have not yet hit a clipping node).
    return false;
  }

  mCurrentStencilState.stencilTestEnable = true;

  const uint32_t clippingDepth = node->GetClippingDepth();

  // Pre-calculate a mask which has all bits set up to and including the current clipping depth.
  // EG. If depth is 3, the mask would be "111" in binary.
  const uint32_t currentDepthMask = ( 1u << clippingDepth ) - 1u;

  //@todo clear support between draw calls
  //item.mClearStencilEnabled = false;

  // Are we are writing to the stencil buffer?
  if( item.mNode->GetClippingMode() == Dali::ClippingMode::CLIP_CHILDREN )
  {
    // We are writing to the stencil buffer.
    // If clipping Id is 1, this is the first clipping renderer within this render-list.
    if( clippingId == 1u )
    {
      // We are enabling the stencil-buffer for the first time within this render list.
      // Clear the buffer at this point.
      mCurrentStencilState.front.writeMask = 0xff;
      mCurrentStencilState.back.writeMask = 0xff;

      // tell render item to clear stencil with specified mask
      // using clear command
      //@todo clear support between draw calls
      //item.mClearStencilValue = 0xff;
      //item.mClearStencilEnabled = true;
    }
    else if( ( clippingDepth < lastClippingDepth ) ||
             ( ( clippingDepth == lastClippingDepth ) && ( clippingId > lastClippingId ) ) )
    {
      // The above if() statement tests if we need to clear some (not all) stencil bit-planes.
      // We need to do this if either of the following are true:
      //   1) We traverse up the scene-graph to a previous stencil depth
      //   2) We are at the same stencil depth but the clipping Id has increased.
      //
      // This calculation takes the new depth to move to, and creates an inverse-mask of that number of consecutive bits.
      // This has the effect of clearing everything except the bit-planes up to (and including) our current depth.
      const uint32_t stencilClearMask = ( currentDepthMask >> 1u ) ^ 0xff;

      mCurrentStencilState.front.writeMask = stencilClearMask;
      mCurrentStencilState.back.writeMask = stencilClearMask;

      //@todo clear support between draw calls
      //item.mClearStencilValue = stencilClearMask;
      //item.mClearStencilEnabled = true;
    }

    // We keep track of the last clipping Id and depth so we can determine when we are
    // moving back up the scene graph and require some of the stencil bit-planes to be deleted.
    lastClippingDepth = clippingDepth;
    lastClippingId = clippingId;

    // We only ever write to bit-planes up to the current depth as we may need
    // to erase individual bit-planes and revert to a previous clipping area.
    // Our reference value for testing (in StencilFunc) is written to to the buffer, but we actually
    // want to test a different value. IE. All the bit-planes up to but not including the current depth.
    // So we use the Mask parameter of StencilFunc to mask off the top bit-plane when testing.
    // Here we create our test mask to innore the top bit of the reference test value.
    // As the mask is made up of contiguous "1" values, we can do this quickly with a bit-shift.
    const uint32_t testMask = currentDepthMask >> 1u;

    mCurrentStencilState.front.compareOp = Graphics::CompareOp::EQUAL;
    mCurrentStencilState.front.reference = currentDepthMask;
    mCurrentStencilState.front.compareMask = testMask;

    mCurrentStencilState.front.failOp = Graphics::StencilOp::KEEP;
    mCurrentStencilState.front.depthFailOp = Graphics::StencilOp::REPLACE;
    mCurrentStencilState.front.passOp = Graphics::StencilOp::REPLACE;

    mCurrentStencilState.back = mCurrentStencilState.front;
  }
  else
  {
    // We are reading from the stencil buffer. Set up the stencil accordingly
    // This calculation sets all the bits up to the current depth bit.
    // This has the effect of testing that the pixel being written to exists in every bit-plane up to the current depth.

    mCurrentStencilState.front.compareOp = Graphics::CompareOp::EQUAL;
    mCurrentStencilState.front.reference = currentDepthMask;
    mCurrentStencilState.front.compareMask = 0xff;

    mCurrentStencilState.front.failOp = Graphics::StencilOp::KEEP;
    mCurrentStencilState.front.depthFailOp = Graphics::StencilOp::KEEP;
    mCurrentStencilState.front.passOp = Graphics::StencilOp::KEEP;

    mCurrentStencilState.back = mCurrentStencilState.front;
  }

  return true;
}

void GraphicsAlgorithms::SetupClipping( const RenderItem& item,
                                        bool& usedStencilBuffer,
                                        uint32_t& lastClippingDepth,
                                        uint32_t& lastClippingId )
{
  RenderMode::Type renderMode = RenderMode::AUTO;
  const Renderer *renderer = item.mRenderer;
  if( renderer )
  {
    renderMode = renderer->GetStencilParameters().renderMode;
  }

  // Setup the stencil using either the automatic clipping feature, or, the manual per-renderer stencil API.
  // Note: This switch is in order of most likely value first.
  switch( renderMode )
  {
    case RenderMode::AUTO:
    {
      // Turn the color buffer on as we always want to render this renderer, regardless of clipping hierarchy.

      // The automatic clipping feature will manage the scissor and stencil functions, only if stencil buffer is available for the latter.
      // As both scissor and stencil clips can be nested, we may be simultaneously traversing up the scissor tree, requiring a scissor to be un-done. Whilst simultaneously adding a new stencil clip.
      // We process both based on our current and old clipping depths for each mode.
      // Both methods with return rapidly if there is nothing to be done for that type of clipping.
      SetupScissorClipping( item );

      //if( stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE )
      {
        SetupStencilClipping( item, lastClippingDepth, lastClippingId );
      }
      break;
    }
    case RenderMode::NONE:
    case RenderMode::COLOR:
    {
#if 0
      // No clipping is performed for these modes.
      // Note: We do not turn off scissor clipping as it may be used for the whole layer.
      // The stencil buffer will not be used at all, but we only need to disable it if it's available.
      if( stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE )
      {
        context.EnableStencilBuffer( false );
      }

      // Setup the color buffer based on the RenderMode.
      context.ColorMask( renderMode == RenderMode::COLOR );
#endif
      break;
    }

    case RenderMode::STENCIL:
    case RenderMode::COLOR_STENCIL:
    {
#if 0
      if( stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE )
      {
        // We are using the low-level Renderer Stencil API.
        // The stencil buffer must be enabled for every renderer with stencil mode on, as renderers in between can disable it.
        // Note: As the command state is cached, it is only sent when needed.
        context.EnableStencilBuffer( true );

        // Setup the color buffer based on the RenderMode.
        context.ColorMask( renderMode == RenderMode::COLOR_STENCIL );

        // If this is the first use of the stencil buffer within this RenderList, clear it (this avoids unnecessary clears).
        if( !usedStencilBuffer )
        {
          context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
          usedStencilBuffer = true;
        }

        // Setup the stencil buffer based on the renderers properties.
        context.StencilFunc( DaliStencilFunctionToGL[ renderer->GetStencilFunction() ],
                             renderer->GetStencilFunctionReference(),
                             renderer->GetStencilFunctionMask() );
        context.StencilOp( DaliStencilOperationToGL[ renderer->GetStencilOperationOnFail() ],
                           DaliStencilOperationToGL[ renderer->GetStencilOperationOnZFail() ],
                           DaliStencilOperationToGL[ renderer->GetStencilOperationOnZPass() ] );
        context.StencilMask( renderer->GetStencilMask() );
      }
#endif
      break;
    }
  }
}


bool GraphicsAlgorithms::SetupPipelineViewportState( Graphics::ViewportState& outViewportState )
{
  // The scissor test is enabled if we have more than one child (the first entry is always the viewport)
  // on the stack, or the stack is empty and there is a layer clipping box.
  // IE. It is not enabled if we are at the top of the stack and the layer does not have a specified clipping box.

  auto scissorEnabled = ( mScissorStack.size() > 1u );// || mHasLayerScissor;
  // If scissor is enabled, we use the calculated screen-space coordinates (now in the stack).
  if( scissorEnabled )
  {
    ClippingBox useScissorBox( mScissorStack.back() );
    outViewportState.SetScissorTestEnable( true  );
    outViewportState.SetScissor( { useScissorBox.x,
                                int32_t(outViewportState.viewport.height - float(useScissorBox.y + useScissorBox.height) ),
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

void GraphicsAlgorithms::RecordRenderItemList(
  BufferIndex bufferIndex,
  Graphics::RenderTargetBinding &renderTargetBinding,
  Matrix viewProjection,
  RenderInstruction &instruction,
  const RenderList &renderItemList,
  std::vector<Graphics::CommandBuffer *> &commandList)
{
  auto numberOfRenderItems = renderItemList.Count();

  const auto viewMatrix = instruction.GetViewMatrix( bufferIndex );
  const auto projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );

  Matrix vulkanProjectionMatrix;
  Matrix::Multiply( vulkanProjectionMatrix, *projectionMatrix, CLIP_MATRIX );

  for( auto i = 0u; i < numberOfRenderItems; ++i )
  {
    auto& item = renderItemList.GetItem( i );
    auto renderer = item.mRenderer;
    if( !renderer )
    {
      continue;
    }

    // @todo Should create commandBuffers similarly to current gfx impl
    auto &renderCmd = renderer->GetRenderCommand( &instruction, bufferIndex );
    auto &cmd = renderCmd.GetGfxRenderCommand( bufferIndex );

    auto color = item.mNode->GetWorldColor( bufferIndex );

    if (cmd.GetVertexBufferBindings()
           .empty())
    {
      continue;
    }
    //cmd.BindRenderTarget( renderTargetBinding ); //@todo Should now be BeginrenderPass

    auto width = float(instruction.mViewport.width);
    auto height = float(instruction.mViewport.height);

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

    // update the uniform buffer
    // pass shared UBO and offset, return new offset for next item to be used
    // don't process bindings if there are no uniform buffers allocated
    auto shader = renderer->GetShader().GetGraphicsObject();
    auto ubo = mUniformBuffer[bufferIndex].get();
    if( ubo && shader )
    {
      std::vector<Graphics::UniformBufferBinding>* bindings{ nullptr };
      if( renderer->UpdateUniformBuffers( instruction, *ubo, bindings, mUboOffset, bufferIndex ) )
      {
        cmd.BindUniformBuffers( bindings );
      }

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

      // we know bindings for this render item, so we can use 'offset' and write additional
      // uniforms
      Matrix mvp, mvp2;
      Matrix::Multiply(mvp, item.mModelMatrix, viewProjection);
      Matrix::Multiply(mvp2, mvp, CLIP_MATRIX);

      WriteDefaultUniform( renderer, *ubo, *bindings, Shader::DefaultUniformIndex::MODEL_MATRIX, item.mModelMatrix );
      WriteDefaultUniform( renderer, *ubo, *bindings, Shader::DefaultUniformIndex::MVP_MATRIX, mvp2 );
      WriteDefaultUniform( renderer, *ubo, *bindings, Shader::DefaultUniformIndex::VIEW_MATRIX, *viewMatrix );
      WriteDefaultUniform( renderer, *ubo, *bindings, Shader::DefaultUniformIndex::MODEL_VIEW_MATRIX, item.mModelViewMatrix );
      WriteDefaultUniform( renderer, *ubo, *bindings, Shader::DefaultUniformIndex::PROJECTION_MATRIX, vulkanProjectionMatrix );
      WriteDefaultUniform( renderer, *ubo, *bindings, Shader::DefaultUniformIndex::SIZE, item.mSize );
      WriteDefaultUniform( renderer, *ubo, *bindings, Shader::DefaultUniformIndex::COLOR, color );

      // Update normal matrix only when used in the shader
      if( renderer->GetShader().GetDefaultUniform( Shader::DefaultUniformIndex::NORMAL_MATRIX ) )
      {
        Matrix3 uNormalMatrix( item.mModelViewMatrix );
        uNormalMatrix.Invert();
        uNormalMatrix.Transpose();
        WriteDefaultUniform( renderer, *ubo, *bindings, Shader::DefaultUniformIndex::NORMAL_MATRIX, uNormalMatrix );
      }
    }

    commandList.push_back(&cmd);
  }
}

void GraphicsAlgorithms::RecordInstruction(
  BufferIndex bufferIndex,
  RenderInstruction& instruction,
  std::vector<Graphics::RenderCommand*>& commandList,
  bool renderToFbo)
{
  using namespace Graphics;

  // Create constant buffer with static uniforms: view matrix, projection matrix
  const Matrix* viewMatrix       = instruction.GetViewMatrix( bufferIndex );
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );
  Matrix        viewProjection;
  Matrix::Multiply( viewProjection, *viewMatrix, *projectionMatrix );


  // @todo change to BeginRenderPass
  // auto renderTargetBinding = Graphics::RenderTargetBinding{}
  // .SetClearColors( {{ instruction.mClearColor.r,
  //                   instruction.mClearColor.g,
  //                   instruction.mClearColor.b,
  //                   instruction.mClearColor.a }} );

  if( !instruction.mIgnoreRenderToFbo )
  {
    if( instruction.mFrameBuffer != 0 )
    {
      renderTargetBinding.SetFramebuffer( instruction.mFrameBuffer->GetGraphicsObject());
      // Store the size of the framebuffer in case the viewport isn't set.
      renderTargetBinding.framebufferWidth = float(instruction.mFrameBuffer->GetWidth());
      renderTargetBinding.framebufferHeight = float(instruction.mFrameBuffer->GetHeight());
    }
  }

  auto numberOfRenderLists = instruction.RenderListCount();
  for( auto i = 0u; i < numberOfRenderLists; ++i )
  {
    RecordRenderItemList( graphics, bufferIndex, renderTargetBinding,
                          viewProjection, instruction, *instruction.GetRenderList(i),
                          commandList );
  }
}

bool GraphicsAlgorithms::PrepareGraphicsPipeline(
  RenderInstruction& instruction,
  const RenderList* renderList,
  RenderItem& item,
  bool& usesDepth,
  bool& usesStencil,
  BufferIndex bufferIndex )
{
  using namespace Dali::Graphics;

  const uint32_t UBO_ALIGNMENT = 256u;

  // vertex input state
  VertexInputState vi{};

  auto *renderer = item.mRenderer;
  auto &renderCmd = renderer->GetRenderCommand( &instruction, bufferIndex );
  auto &cmd = renderCmd.GetGfxRenderCommand( bufferIndex );
  auto *geometry = renderer->GetGeometry();
  auto graphicsProgram = renderer->GetShader().GetGraphicsObject();

  if( !graphicsProgram )
  {
    return false;
  }
  auto& reflection = mGraphicsController.GetProgramReflection(graphicsProgram);

  // Update allocation requirements
  mUniformBlockAllocationCount += reflection->GetUniformBlockCount();
  for( auto i = 0u; i < reflection->GetUniformBlockCount(); ++i )
  {
    auto size = reflection->GetUniformBlockSize( i );
    auto blockSize = (( size / UBO_ALIGNMENT ) + ( ( size % UBO_ALIGNMENT ) ? 1 : 0 )) * UBO_ALIGNMENT;
    if( mUniformBlockMaxSize < blockSize )
    {
      mUniformBlockMaxSize = blockSize;
    }
    mUniformBlockAllocationBytes += blockSize;
  }

  /**
   * Prepare vertex attribute buffer bindings
   */
  uint32_t bindingIndex{0u};
  std::vector<const Graphics::Buffer*> vertexBuffers{};

  for (auto &&vertexBuffer : geometry->GetVertexBuffers())
  {
    auto gpuBuffer = vertexBuffer->GetGpuBuffer();
    auto graphicsBuffer = gpuBuffer->GetGraphicsObject();
    vertexBuffers.push_back(graphicsBuffer);
    auto attributeCountInForBuffer = vertexBuffer->GetAttributeCount();

    // update vertex buffer if necessary
    vertexBuffer->Update(mGraphicsController);

    // store buffer binding
    vi.bufferBindings
      .emplace_back(vertexBuffer->GetFormat()
                                ->size, VertexInputRate::PER_VERTEX);

    for (auto i = 0u; i < attributeCountInForBuffer; ++i)
    {
      // create attribute description
      vi.attributes
        .emplace_back(
          reflection->GetVertexAttributeLocation(vertexBuffer->GetAttributeName(i)),
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

  if( !usesDepth && (enableDepthTest || enableDepthWrite) )
  {
    usesDepth = true; // set out-value to indicate at least 1 pipeline uses depth buffer
  }

  // Stencil setup
  bool stencilEnabled = mCurrentStencilState.stencilTestEnable;

  if( !usesStencil && stencilEnabled )
  {
    usesStencil = true; // set out-value to indicate at least 1 pipeline uses stencil buffer
  }

  if( stencilEnabled)
  {
    depthStencilState
      .SetStencilTestEnable( mCurrentStencilState.stencilTestEnable )
      .SetFront( mCurrentStencilState.front )
      .SetBack( mCurrentStencilState.back );
  }

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
  if( !cmd.GetDrawCommand().viewportEnable && instruction.mIsViewportSet )
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
    framebufferState.SetFramebuffer( *instruction.mFrameBuffer->GetGraphicsObject() );
  }

  /**
   * Scissor test is represented only by the dynamic state as it can be transformed
   * any time.
   */
  Graphics::PipelineDynamicStateMask dynamicStateMask{ 0u };

  if( SetupPipelineViewportState( viewportState) )
  {
    cmd.mDrawCommand.SetScissor( viewportState.scissor );
    cmd.mDrawCommand.SetScissorTestEnable( true );
    dynamicStateMask = Graphics::PipelineDynamicStateBits::SCISSOR_BIT;
  }
  else
  {
    cmd.mDrawCommand.SetScissorTestEnable( false );
  }

  // todo: make it possible to decide earlier whether we want dynamic or static viewport
  dynamicStateMask |= Graphics::PipelineDynamicStateBits::VIEWPORT_BIT;

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

  Graphics::ProgramState programState{};
  programState.SetProgram(graphicsProgram);

  colorBlendState
    .SetColorComponentsWriteBits(0xff)
    .SetLogicOpEnable(false);

  Graphics::InputAssemblyState inputAssembly{};
  inputAssembly
    .SetTopology(topology)
    .SetPrimitiveRestartEnable(false);

  Graphics::RasterizationState rasterizationState{};
  rasterizationState
    .SetCullMode(cullMode)
    .SetPolygonMode(PolygonMode::FILL)
    .SetFrontFace(FrontFace::COUNTER_CLOCKWISE);

  Graphics::PipelineCreateInfo createInfo;
  createInfo
    .SetProgramState(&programState)
    .SetColorBlendState(&colorBlendState)
    .SetRasterizationState(&rasterizationState)
    .SetVertexInputState(&vi)
    .SetInputAssemblyState(&inputAssembly);

    //.SetFramebufferState( framebufferState )   // Does not exist in modern API
    //.SetDepthStencilState( depthStencilState ) // Not used in modern impl
    //.SetViewportState( &viewportState )        // Not used in modern impl
    //.SetDynamicStateMask( dynamicStateMask );  // Not used in modern impl

  // create pipeline
  auto pipeline = mGraphicsController.CreatePipeline( createInfo, nullptr /*renderer->ReleaseGraphicsPipeline() */ );

  // bind pipeline to the render command
  renderer->BindPipeline( std::move(pipeline), bufferIndex, &instruction );
  return true;
}


void GraphicsAlgorithms::PrepareRendererPipelines(
  RenderInstructionContainer& renderInstructions,
  bool& usesDepth,
  bool& usesStencil,
  BufferIndex bufferIndex )
{
  mUniformBlockAllocationCount = 0u;
  mUniformBlockAllocationBytes = 0u;
  mUniformBlockMaxSize = 0u;

  for( auto i = 0u; i < renderInstructions.Count( bufferIndex ); ++i )
  {
    RenderInstruction &ri = renderInstructions.At(bufferIndex, i);

    for (auto renderListIndex = 0u; renderListIndex < ri.RenderListCount(); ++renderListIndex)
    {
      const auto *renderList = ri.GetRenderList(renderListIndex);

      // Reset scissor stack
      mScissorStack.clear();
      mScissorStack.push_back( ri.mViewport );

      // Reset stencil state
      mCurrentStencilState = {};

      uint32_t lastClippingDepth( 0u );
      uint32_t lastClippingId( 0u );
      bool usedStencilBuffer( false );

      for (auto renderItemIndex = 0u; renderItemIndex < renderList->Count(); ++renderItemIndex)
      {
        auto &item = renderList->GetItem(renderItemIndex);

        // setup clipping for item
        SetupClipping( item, usedStencilBuffer, lastClippingDepth, lastClippingId );

        if( item.mRenderer )
        {
          PrepareGraphicsPipeline( controller, ri, renderList, item, usesDepth, usesStencil, bufferIndex );
        }
      }
    }
  }
}
void GraphicsAlgorithms::ResetCommandBuffer()
{
  // Reset main command buffer
  if(!mGraphicsCommandBuffer)
  {
    mGraphicsCommandBuffer = mGraphicsController.CreateCommandBuffer(
      Graphics::CommandBufferCreateInfo()
        .SetLevel(Graphics::CommandBufferLevel::PRIMARY),
      nullptr);
  }
  else
  {
    mGraphicsCommandBuffer->Reset();
  }
}

void GraphicsAlgorithms::SubmitRenderInstructions(
  RenderInstructionContainer& renderInstructions,
  BufferIndex                 bufferIndex )
{
  // ComputeUniformBufferRequirements

  // BeginFrame
  // PrepareRendererPipelines
  // foreach RenderInstruction, // records fbos first
  //   RecordInstruction
  // SubmitCommands
  // EndFrame


  bool usesDepth = false;
  bool usesStencil = false;

  PrepareRendererPipelines( renderInstructions, usesDepth, usesStencil, bufferIndex );

  // If state of depth/stencil has changed between frames then the pipelines must be
  // prepared again. Note, this stage does not recompile shader but collects necessary
  // data to compile pipelines at the further stage.
  if( mGraphicsController.EnableDepthStencilBuffer( usesDepth, usesStencil ) )
  {
    PrepareRendererPipelines( mGraphicsController, renderInstructions, usesDepth, usesStencil, bufferIndex );
  }

  auto numberOfInstructions = renderInstructions.Count( bufferIndex );

  // Prepare uniform buffers
  if( !mUniformBufferManager )
  {
    mUniformBufferManager.reset( new UniformBufferManager( &mGraphicsController ) );
  }

  mGraphicsController.BeginFrame();

  auto pagedAllocation = ( ( mUniformBlockAllocationBytes / UBO_PAGE_SIZE + 1u ) ) * UBO_PAGE_SIZE;

  // Allocate twice memory as required by the uniform buffers
  // todo: memory usage backlog to use optimal allocation
  if( mUniformBlockAllocationBytes && !mUniformBuffer[bufferIndex] )
  {
    mUniformBuffer[bufferIndex] = std::move( mGraphicsBufferManager->AllocateUniformBuffer( pagedAllocation ) );
  }
  else if( mUniformBlockAllocationBytes && (
    mUniformBuffer[bufferIndex]->GetSize() < pagedAllocation ||
    (pagedAllocation < uint32_t(float(mUniformBuffer[bufferIndex]->GetSize()) * UBO_SHRINK_THRESHOLD ))))
  {
    mUniformBuffer[bufferIndex]->Reserve( pagedAllocation, true );
  }

  // Clear UBO
  if( mUniformBuffer[bufferIndex] )
  {
    mUniformBuffer[bufferIndex]->Fill( 0, 0u, 0u );
  }

  mUboOffset = 0u;

  std::vector<Graphics::RenderCommand*> commandList{};

  // First, record each framebuffer
  for( uint32_t i = 0; i < numberOfInstructions; ++i )
  {
    RenderInstruction& instruction = renderInstructions.At( bufferIndex, i );
    if(instruction.mFrameBuffer)
    {
      RecordInstruction(bufferIndex, instruction, commandList, true);
    }
  }
  // Then, record all other instructions to the surface.
  for( uint32_t i = 0; i < numberOfInstructions; ++i )
  {
    RenderInstruction& instruction = renderInstructions.At( bufferIndex, i );
    if(!instruction.mFrameBuffer)
    {
      RecordInstruction(bufferIndex, instruction, commandList, false);
    }
  }

  // Submit all render commands in one go
  mGraphicsController.SubmitCommands( std::move(commandList) );

  if( mUniformBlockAllocationBytes && mUniformBuffer[bufferIndex] )
  {
    mUniformBuffer[bufferIndex]->Flush();
  }

  mGraphicsController.EndFrame();

  mCurrentFrameIndex++;
}

void GraphicsAlgorithms::DiscardUnusedResources( Graphics::Controller& controller )
{
  // wait for queues to be idle
  controller.WaitIdle();

  // Destroy Uniform buffers
  for( auto& ubo : mUniformBuffer )
  {
    ubo.reset( nullptr );
  }

  // Discard unused resources
  controller.DiscardUnusedResources();
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
