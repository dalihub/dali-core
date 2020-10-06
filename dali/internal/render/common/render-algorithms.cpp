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
#include <dali/internal/render/common/render-algorithms.h>

// INTERNAL INCLUDES
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/render-list.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>

using Dali::Internal::SceneGraph::RenderItem;
using Dali::Internal::SceneGraph::RenderList;
using Dali::Internal::SceneGraph::RenderListContainer;
using Dali::Internal::SceneGraph::RenderInstruction;

namespace Dali
{

namespace Internal
{

namespace Render
{

namespace
{

// Table for fast look-up of Dali::DepthFunction enum to a GL depth function.
// Note: These MUST be in the same order as Dali::DepthFunction enum.
const int DaliDepthToGLDepthTable[]  = { GL_NEVER, GL_ALWAYS, GL_LESS, GL_GREATER, GL_EQUAL, GL_NOTEQUAL, GL_LEQUAL, GL_GEQUAL };

// Table for fast look-up of Dali::StencilFunction enum to a GL stencil function.
// Note: These MUST be in the same order as Dali::StencilFunction enum.
const int DaliStencilFunctionToGL[]  = { GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS };

// Table for fast look-up of Dali::StencilOperation enum to a GL stencil operation.
// Note: These MUST be in the same order as Dali::StencilOperation enum.
const int DaliStencilOperationToGL[] = { GL_ZERO, GL_KEEP, GL_REPLACE, GL_INCR, GL_DECR, GL_INVERT, GL_INCR_WRAP, GL_DECR_WRAP };

/**
 * @brief Find the intersection of two AABB rectangles.
 * This is a logical AND operation. IE. The intersection is the area overlapped by both rectangles.
 * @param[in]     aabbA                  Rectangle A
 * @param[in]     aabbB                  Rectangle B
 * @return                               The intersection of rectangle A & B (result is a rectangle)
 */
inline ClippingBox IntersectAABB( const ClippingBox& aabbA, const ClippingBox& aabbB )
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

/**
 * @brief Set up the stencil and color buffer for automatic clipping (StencilMode::AUTO).
 * @param[in]     item                     The current RenderItem about to be rendered
 * @param[in]     context                  The context
 * @param[in/out] lastClippingDepth        The stencil depth of the last renderer drawn.
 * @param[in/out] lastClippingId           The clipping ID of the last renderer drawn.
 */
inline void SetupStencilClipping( const RenderItem& item, Context& context, uint32_t& lastClippingDepth, uint32_t& lastClippingId )
{
  const Dali::Internal::SceneGraph::Node* node = item.mNode;
  const uint32_t clippingId = node->GetClippingId();
  // If there is no clipping Id, then either we haven't reached a clipping Node yet, or there aren't any.
  // Either way we can skip clipping setup for this renderer.
  if( clippingId == 0u )
  {
    // Exit immediately if there are no clipping actions to perform (EG. we have not yet hit a clipping node).
    context.EnableStencilBuffer( false );
    return;
  }

  context.EnableStencilBuffer( true );

  const uint32_t clippingDepth = node->GetClippingDepth();

  // Pre-calculate a mask which has all bits set up to and including the current clipping depth.
  // EG. If depth is 3, the mask would be "111" in binary.
  const uint32_t currentDepthMask = ( 1u << clippingDepth ) - 1u;

  // Are we are writing to the stencil buffer?
  if( item.mNode->GetClippingMode() == Dali::ClippingMode::CLIP_CHILDREN )
  {
    // We are writing to the stencil buffer.
    // If clipping Id is 1, this is the first clipping renderer within this render-list.
    if( clippingId == 1u )
    {
      // We are enabling the stencil-buffer for the first time within this render list.
      // Clear the buffer at this point.
      context.StencilMask( 0xff );
      context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
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

      context.StencilMask( stencilClearMask );
      context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
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

    context.StencilFunc( GL_EQUAL, currentDepthMask, testMask ); // Test against existing stencil bit-planes. All must match up to (but not including) this depth.
    context.StencilMask( currentDepthMask );                     // Write to the new stencil bit-plane (the other previous bit-planes are also written to).
    context.StencilOp( GL_KEEP, GL_REPLACE, GL_REPLACE );
  }
  else
  {
    // We are reading from the stencil buffer. Set up the stencil accordingly
    // This calculation sets all the bits up to the current depth bit.
    // This has the effect of testing that the pixel being written to exists in every bit-plane up to the current depth.
    context.StencilFunc( GL_EQUAL, currentDepthMask, 0xff );
    context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
  }
}

/**
 * @brief Sets up the depth buffer for reading and writing based on the current render item.
 * The items read and write mode are used if specified.
 *  - If AUTO is selected for reading, the decision will be based on the Layer Behavior.
 *  - If AUTO is selected for writing, the decision will be based on the items opacity.
 * @param[in]     item                The RenderItem to set up the depth buffer for.
 * @param[in]     context             The context used to execute GL commands.
 * @param[in]     depthTestEnabled    True if depth testing has been enabled.
 * @param[in/out] firstDepthBufferUse Initialize to true on the first call, this method will set it to false afterwards.
 */
inline void SetupDepthBuffer( const RenderItem& item, Context& context, bool depthTestEnabled, bool& firstDepthBufferUse )
{
  // Set up whether or not to write to the depth buffer.
  const DepthWriteMode::Type depthWriteMode = item.mRenderer->GetDepthWriteMode();
  // Most common mode (AUTO) is tested first.
  const bool enableDepthWrite = ( ( depthWriteMode == DepthWriteMode::AUTO ) && depthTestEnabled && item.mIsOpaque ) ||
                                ( depthWriteMode == DepthWriteMode::ON );

  // Set up whether or not to read from (test) the depth buffer.
  const DepthTestMode::Type depthTestMode = item.mRenderer->GetDepthTestMode();
  // Most common mode (AUTO) is tested first.
  const bool enableDepthTest = ( ( depthTestMode == DepthTestMode::AUTO ) && depthTestEnabled ) ||
                               ( depthTestMode == DepthTestMode::ON );

  // Is the depth buffer in use?
  if( enableDepthWrite || enableDepthTest )
  {
    // The depth buffer must be enabled if either reading or writing.
    context.EnableDepthBuffer( true );

    // Look-up the GL depth function from the Dali::DepthFunction enum, and set it.
    context.DepthFunc( DaliDepthToGLDepthTable[ item.mRenderer->GetDepthFunction() ] );

    // If this is the first use of the depth buffer this RenderTask, perform a clear.
    // Note: We could do this at the beginning of the RenderTask and rely on the
    // context cache to ignore the clear if not required, but, we would have to enable
    // the depth buffer to do so, which could be a redundant enable.
    if( DALI_UNLIKELY( firstDepthBufferUse ) )
    {
      // This is the first time the depth buffer is being written to or read.
      firstDepthBufferUse = false;

      // Note: The buffer will only be cleared if written to since a previous clear.
      context.DepthMask( true );
      context.Clear( GL_DEPTH_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
    }

    // Set up the depth mask based on our depth write setting.
    context.DepthMask( enableDepthWrite );
  }
  else
  {
    // The depth buffer is not being used by this renderer, so we must disable it to stop it being tested.
    context.EnableDepthBuffer( false );
  }
}

} // Unnamed namespace


/**
 * @brief This method is responsible for making decisions on when to apply and unapply scissor clipping, and what rectangular dimensions should be used.
 * A stack of scissor clips at each depth of clipping is maintained, so it can be applied and unapplied.
 * As the clips are hierarchical, this RenderItems AABB is clipped against the current "active" scissor bounds via an intersection operation.
 * @param[in]     item                     The current RenderItem about to be rendered
 * @param[in]     context                  The context
 */
inline void RenderAlgorithms::SetupScissorClipping( const RenderItem& item, Context& context, int orientation )
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

      // Get the AABB bounding box for the current render item.
      const ClippingBox scissorBox( item.CalculateViewportSpaceAABB( item.mSize, mViewportRectangle.width, mViewportRectangle.height ) );

      // Get the AABB for the parent item that we must intersect with.
      const ClippingBox& parentBox( mScissorStack.back() );

      // We must reduce the clipping area based on the parents area to allow nested clips. This is a set intersection function.
      // We add the new scissor box to the stack so we can return to it if needed.
      mScissorStack.emplace_back( IntersectAABB( parentBox, scissorBox ) );
    }

    // The scissor test is enabled if we have any children on the stack, OR, if there are none but it is a user specified layer scissor box.
    // IE. It is not enabled if we are at the top of the stack and the layer does not have a specified clipping box.
    const bool scissorEnabled = ( mScissorStack.size() > 0u ) || mHasLayerScissor;

    // Enable the scissor test based on the above calculation
    context.SetScissorTest( scissorEnabled );

    // If scissor is enabled, we use the calculated screen-space coordinates (now in the stack).
    if( scissorEnabled )
    {
      ClippingBox useScissorBox( mScissorStack.back() );
      GLint x = useScissorBox.x;
      GLint y = useScissorBox.y;
      if( orientation == 90 )
      {
        x = mViewportRectangle.height - (useScissorBox.y + useScissorBox.height);
        y = useScissorBox.x;
        context.Scissor( x, y, useScissorBox.height, useScissorBox.width );
      }
      else if( orientation == 180 )
      {
        x = mViewportRectangle.width - (useScissorBox.x + useScissorBox.width);
        y = mViewportRectangle.height - (useScissorBox.y + useScissorBox.height);
        context.Scissor( x, y, useScissorBox.width, useScissorBox.height );
      }
      else if( orientation == 270 )
      {
        x = useScissorBox.y;
        y = mViewportRectangle.width - (useScissorBox.x + useScissorBox.width);
        context.Scissor( x, y, useScissorBox.height, useScissorBox.width );
      }
      else
      {
        context.Scissor( x, y, useScissorBox.width, useScissorBox.height );
      }
    }
  }
}

inline void RenderAlgorithms::SetupClipping( const RenderItem& item,
                                             Context& context,
                                             bool& usedStencilBuffer,
                                             uint32_t& lastClippingDepth,
                                             uint32_t& lastClippingId,
                                             Integration::StencilBufferAvailable stencilBufferAvailable,
                                             int orientation )
{
  RenderMode::Type renderMode = RenderMode::AUTO;
  const Renderer *renderer = item.mRenderer;
  if( renderer )
  {
    renderMode = renderer->GetRenderMode();
  }

  // Setup the stencil using either the automatic clipping feature, or, the manual per-renderer stencil API.
  // Note: This switch is in order of most likely value first.
  switch( renderMode )
  {
    case RenderMode::AUTO:
    {
      // Turn the color buffer on as we always want to render this renderer, regardless of clipping hierarchy.
      context.ColorMask( true );

      // The automatic clipping feature will manage the scissor and stencil functions, only if stencil buffer is available for the latter.
      // As both scissor and stencil clips can be nested, we may be simultaneously traversing up the scissor tree, requiring a scissor to be un-done. Whilst simultaneously adding a new stencil clip.
      // We process both based on our current and old clipping depths for each mode.
      // Both methods with return rapidly if there is nothing to be done for that type of clipping.
      SetupScissorClipping( item, context, orientation );

      if( stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE )
      {
        SetupStencilClipping( item, context, lastClippingDepth, lastClippingId );
      }
      break;
    }

    case RenderMode::NONE:
    case RenderMode::COLOR:
    {
      // No clipping is performed for these modes.
      // Note: We do not turn off scissor clipping as it may be used for the whole layer.
      // The stencil buffer will not be used at all, but we only need to disable it if it's available.
      if( stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE )
      {
        context.EnableStencilBuffer( false );
      }

      // Setup the color buffer based on the RenderMode.
      context.ColorMask( renderMode == RenderMode::COLOR );
      break;
    }

    case RenderMode::STENCIL:
    case RenderMode::COLOR_STENCIL:
    {
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
      break;
    }
  }
}

inline void RenderAlgorithms::ProcessRenderList( const RenderList& renderList,
                                                 Context& context,
                                                 BufferIndex bufferIndex,
                                                 const Matrix& viewMatrix,
                                                 const Matrix& projectionMatrix,
                                                 Integration::DepthBufferAvailable depthBufferAvailable,
                                                 Integration::StencilBufferAvailable stencilBufferAvailable,
                                                 Vector<GLuint>& boundTextures,
                                                 const RenderInstruction& instruction,
                                                 const Rect<int>& rootClippingRect,
                                                 int orientation )
{
  DALI_PRINT_RENDER_LIST( renderList );

  // Note: The depth buffer is enabled or disabled on a per-renderer basis.
  // Here we pre-calculate the value to use if these modes are set to AUTO.
  const bool autoDepthTestMode(  ( depthBufferAvailable == Integration::DepthBufferAvailable::TRUE ) &&
                                !( renderList.GetSourceLayer()->IsDepthTestDisabled() ) &&
                                 renderList.HasColorRenderItems() );
  const std::size_t count = renderList.Count();
  uint32_t lastClippingDepth( 0u );
  uint32_t lastClippingId( 0u );
  bool usedStencilBuffer( false );
  bool firstDepthBufferUse( true );
  mViewportRectangle = context.GetViewport();
  mHasLayerScissor = false;

  if( orientation == 90 || orientation == 270 )
  {
    int temp = mViewportRectangle.width;
    mViewportRectangle.width = mViewportRectangle.height;
    mViewportRectangle.height = temp;
  }

  // Setup Scissor testing (for both viewport and per-node scissor)
  mScissorStack.clear();

  // Add root clipping rect (set manually for Render function ny partial update for example)
  // on the bottom of the stack
  if (!rootClippingRect.IsEmpty())
  {
    context.SetScissorTest( true );
    context.Scissor( rootClippingRect.x, rootClippingRect.y, rootClippingRect.width, rootClippingRect.height );
    mScissorStack.push_back( rootClippingRect );
  }
  // We are not performing a layer clip and no clipping rect set. Add the viewport as the root scissor rectangle.
  else if (!renderList.IsClipping())
  {
    context.SetScissorTest( false );
    mScissorStack.push_back( mViewportRectangle );
  }

  if( renderList.IsClipping() )
  {
    context.SetScissorTest( true );
    const ClippingBox& layerScissorBox = renderList.GetClippingBox();
    GLint x = layerScissorBox.x;
    GLint y = layerScissorBox.y;

    if( orientation == 90 )
    {
      x = mViewportRectangle.height - (layerScissorBox.y + layerScissorBox.height);
      y = layerScissorBox.x;
      context.Scissor( x, y, layerScissorBox.height, layerScissorBox.width );
    }
    else if( orientation == 180 )
    {
      x = mViewportRectangle.width - (layerScissorBox.x + layerScissorBox.width);
      y = mViewportRectangle.height - (layerScissorBox.y + layerScissorBox.height);
      context.Scissor( x, y, layerScissorBox.width, layerScissorBox.height );
    }
    else if( orientation == 270 )
    {
      x = layerScissorBox.y;
      y = mViewportRectangle.width - (layerScissorBox.x + layerScissorBox.width);
      context.Scissor( x, y, layerScissorBox.height, layerScissorBox.width );
    }
    else
    {
      context.Scissor( x, y, layerScissorBox.width, layerScissorBox.height );
    }

    mScissorStack.push_back( layerScissorBox );
    mHasLayerScissor = true;
  }

  // Loop through all RenderList in the RenderList, set up any prerequisites to render them, then perform the render.
  for( uint32_t index = 0u; index < count; ++index )
  {
    const RenderItem& item = renderList.GetItem( index );

    DALI_PRINT_RENDER_ITEM( item );

    // Set up clipping based on both the Renderer and Actor APIs.
    // The Renderer API will be used if specified. If AUTO, the Actors automatic clipping feature will be used.
    SetupClipping( item, context, usedStencilBuffer, lastClippingDepth, lastClippingId, stencilBufferAvailable, orientation );

    if( DALI_LIKELY( item.mRenderer ) )
    {
      // Set up the depth buffer based on per-renderer flags if depth buffer is available
      // If the per renderer flags are set to "ON" or "OFF", they will always override any Layer depth mode or
      // draw-mode state, such as Overlays.
      // If the flags are set to "AUTO", the behavior then depends on the type of renderer. Overlay Renderers will always
      // disable depth testing and writing. Color Renderers will enable them if the Layer does.
      if( depthBufferAvailable == Integration::DepthBufferAvailable::TRUE )
      {
        SetupDepthBuffer( item, context, autoDepthTestMode, firstDepthBufferUse );
      }

      // Depending on whether the renderer has draw commands attached or not the rendering process will
      // iterate through all the render queues. If there are no draw commands attached, only one
      // iteration must be done and the default behaviour of the renderer will be executed.
      // The queues allow to iterate over the same renderer multiple times changing the state of the renderer.
      // It is similar to the multi-pass rendering.
      auto const MAX_QUEUE = item.mRenderer->GetDrawCommands().empty() ? 1 : DevelRenderer::RENDER_QUEUE_MAX;
      for( auto queue = 0u; queue < MAX_QUEUE; ++queue )
      {
        // Render the item.
        item.mRenderer->Render(context, bufferIndex, *item.mNode, item.mModelMatrix, item.mModelViewMatrix,
                               viewMatrix, projectionMatrix, item.mSize, !item.mIsOpaque, boundTextures, instruction, queue);
      }
    }
  }
}

RenderAlgorithms::RenderAlgorithms()
  : mViewportRectangle(),
    mHasLayerScissor( false )
{
}

void RenderAlgorithms::ProcessRenderInstruction( const RenderInstruction& instruction,
                                                 Context& context,
                                                 BufferIndex bufferIndex,
                                                 Integration::DepthBufferAvailable depthBufferAvailable,
                                                 Integration::StencilBufferAvailable stencilBufferAvailable,
                                                 Vector<GLuint>& boundTextures,
                                                 const Rect<int>& rootClippingRect,
                                                 int orientation )
{
  DALI_PRINT_RENDER_INSTRUCTION( instruction, bufferIndex );

  const Matrix* viewMatrix       = instruction.GetViewMatrix( bufferIndex );
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );

  DALI_ASSERT_DEBUG( viewMatrix );
  DALI_ASSERT_DEBUG( projectionMatrix );

  if( viewMatrix && projectionMatrix )
  {
    const RenderListContainer::SizeType count = instruction.RenderListCount();

    // Iterate through each render list in order. If a pair of render lists
    // are marked as interleaved, then process them together.
    for( RenderListContainer::SizeType index = 0; index < count; ++index )
    {
      const RenderList* renderList = instruction.GetRenderList( index );

      if( renderList && !renderList->IsEmpty() )
      {
        ProcessRenderList( *renderList,
                            context,
                            bufferIndex,
                           *viewMatrix,
                           *projectionMatrix,
                            depthBufferAvailable,
                            stencilBufferAvailable,
                            boundTextures,
                            instruction, //added for reflection effect
                            rootClippingRect,
                            orientation );
      }
    }
  }
}


} // namespace Render

} // namespace Internal

} // namespace Dali
