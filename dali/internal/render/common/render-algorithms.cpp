/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

    // Set up the depth mask based on our depth write setting.
    context.DepthMask( enableDepthWrite );

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
      context.Clear( GL_DEPTH_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
    }
  }
  else
  {
    // The depth buffer is not being used by this renderer, so we must disable it to stop it being tested.
    context.EnableDepthBuffer( false );
  }
}

} // Unnamed namespace


inline ClippingBox RenderAlgorithms::CalculateScreenSpaceAABB( const SceneGraph::RenderItem& item )
{
  // Calculate extent vector of the AABB:
  const Vector3& actorSize = item.mSize;
  const float halfActorX = actorSize.x * 0.5f;
  const float halfActorY = actorSize.y * 0.5f;

  // Transform to absolute oriented bounding box.
  const Matrix& worldMatrix = item.mModelViewMatrix;

  // To transform the actor bounds to screen-space, We do a fast, 2D version of a matrix multiply optimized for 2D quads.
  // This reduces float multiplications from 64 (16 * 4) to 12 (4 * 3).
  // We create an array of 4 corners and directly initialize the first 3 with the matrix multiplication result of the respective corner.
  // This causes the construction of the vector arrays contents in-place for optimization.
  // We skip the 4th corner here as we can calculate that from the other 3, bypassing matrix multiplication.
  // Note: The below * operators trigger a fast (2D) matrix multiply (only 4 multiplications are done).
  Vector2 corners[4]{ worldMatrix * Vector2( -halfActorX, -halfActorY ),
                      worldMatrix * Vector2(  halfActorX, -halfActorY ),
                      worldMatrix * Vector2(  halfActorX,  halfActorY ) };

  // As we are dealing with a rectangle, we can do a fast calculation to get the 4th corner from knowing the other 3 (even if rotated).
  corners[3] = Vector2( corners[0] + ( corners[2] - corners[1] ) );

  // Calculate the AABB:
  // We use knowledge that opposite corners will be the max/min of each other. Doing this reduces the normal 12 branching comparisons to 3.
  // The standard equivalent min/max code of the below would be:
  //       Vector2 AABBmax( std::max( corners[0].x, std::max( corners[1].x, std::max( corners[3].x, corners[2].x ) ) ),
  //                        std::max( corners[0].y, std::max( corners[1].y, std::max( corners[3].y, corners[2].y ) ) ) );
  //       Vector2 AABBmin( std::min( corners[0].x, std::min( corners[1].x, std::min( corners[3].x, corners[2].x ) ) ),
  //                        std::min( corners[0].y, std::min( corners[1].y, std::min( corners[3].y, corners[2].y ) ) ) );
  unsigned int smallestX = 0u;
  // Loop 3 times to find the index of the smallest X value.
  // Note: We deliberately do NOT unroll the code here as this hampers the compilers output.
  for( unsigned int i = 1u; i < 4u; ++i )
  {
    if( corners[i].x < corners[smallestX].x )
    {
      smallestX = i;
    }
  }

  // As we are dealing with a rectangle, we can assume opposite corners are the largest.
  // So without doing min/max branching, we can fetch the min/max values of all the remaining X/Y coords from this one index.
  Vector4 aabb( corners[smallestX].x, corners[( smallestX + 3u ) % 4].y, corners[( smallestX + 2u ) % 4].x, corners[( smallestX + 1u ) % 4].y );

  // Convert maximums to extents.
  aabb.z -= aabb.x;
  aabb.w -= aabb.y;

  // Return the AABB in screen-space pixels (x, y, width, height).
  // Note: This is a algebraic simplification of: ( viewport.x - aabb.width ) / 2 - ( ( aabb.width / 2 ) + aabb.x ) per axis.
  return ClippingBox( ( mViewportRectangle.width / 2 ) - aabb.z - aabb.x, ( mViewportRectangle.height / 2 ) - aabb.w - aabb.y, aabb.z, aabb.w );
}

inline void RenderAlgorithms::SetupScissorClipping( const RenderItem& item, Context& context )
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

  // If we are on a clipping node, or we have traveled up the tree and gone back past a clipping node, may need to apply a new scissor clip.
  if( clippingNode || traversedUpTree )
  {
    // First, check if we are a clipping node.
    if( clippingNode )
    {
      // This is a clipping node. We generate the AABB for this node and intersect it with the previous intersection further up the tree.

      // Get the AABB bounding box for the current render item.
      const ClippingBox scissorBox( CalculateScreenSpaceAABB( item ) );
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
      context.Scissor( useScissorBox.x, useScissorBox.y, useScissorBox.width, useScissorBox.height );
    }
  }
}

inline void RenderAlgorithms::SetupClipping( const RenderItem& item, Context& context, bool& usedStencilBuffer, uint32_t& lastClippingDepth, uint32_t& lastClippingId )
{
  const Renderer *renderer = item.mRenderer;

  // Setup the stencil using either the automatic clipping feature, or, the manual per-renderer stencil API.
  // Note: This switch is in order of most likely value first.
  RenderMode::Type renderMode = renderer->GetRenderMode();
  switch( renderMode )
  {
    case RenderMode::AUTO:
    {
      // Turn the color buffer on as we always want to render this renderer, regardless of clipping hierarchy.
      context.ColorMask( true );

      // The automatic clipping feature will manage the scissor and stencil functions.
      // As both scissor and stencil clips can be nested, we may be simultaneously traversing up the scissor tree, requiring a scissor to be un-done. Whilst simultaneously adding a new stencil clip.
      // We process both based on our current and old clipping depths for each mode.
      // Both methods with return rapidly if there is nothing to be done for that type of clipping.
      SetupScissorClipping( item, context );
      SetupStencilClipping( item, context, lastClippingDepth, lastClippingId );
      break;
    }

    case RenderMode::NONE:
    case RenderMode::COLOR:
    {
      // No clipping is performed for these modes.
      // Note: We do not turn off scissor clipping as it may be used for the whole layer.
      // The stencil buffer will not be used at all.
      context.EnableStencilBuffer( false );

      // Setup the color buffer based on the RenderMode.
      context.ColorMask( renderMode == RenderMode::COLOR );
      break;
    }

    case RenderMode::STENCIL:
    case RenderMode::COLOR_STENCIL:
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
      break;
    }
  }
}

inline void RenderAlgorithms::ProcessRenderList(
  const RenderList& renderList,
  Context& context,
  BufferIndex bufferIndex,
  const Matrix& viewMatrix,
  const Matrix& projectionMatrix )
{
  DALI_PRINT_RENDER_LIST( renderList );

  // Note: The depth buffer is enabled or disabled on a per-renderer basis.
  // Here we pre-calculate the value to use if these modes are set to AUTO.
  const bool autoDepthTestMode( !( renderList.GetSourceLayer()->IsDepthTestDisabled() ) && renderList.HasColorRenderItems() );
  const std::size_t count = renderList.Count();
  uint32_t lastClippingDepth( 0u );
  uint32_t lastClippingId( 0u );
  bool usedStencilBuffer( false );
  bool firstDepthBufferUse( true );
  mViewportRectangle = context.GetViewport();
  mHasLayerScissor = false;

  // Setup Scissor testing (for both viewport and per-node scissor)
  mScissorStack.clear();
  if( renderList.IsClipping() )
  {
    context.SetScissorTest( true );
    const ClippingBox& layerScissorBox = renderList.GetClippingBox();
    context.Scissor( layerScissorBox.x, layerScissorBox.y, layerScissorBox.width, layerScissorBox.height );
    mScissorStack.push_back( layerScissorBox );
    mHasLayerScissor = true;
  }
  else
  {
    // We are not performing a layer clip. Add the viewport as the root scissor rectangle.
    context.SetScissorTest( false );
    mScissorStack.push_back( mViewportRectangle );
  }

  for( size_t index( 0u ); index < count; ++index )
  {
    const RenderItem& item = renderList.GetItem( index );
    DALI_PRINT_RENDER_ITEM( item );

    // Set up the depth buffer based on per-renderer flags.
    // If the per renderer flags are set to "ON" or "OFF", they will always override any Layer depth mode or
    // draw-mode state, such as Overlays.
    // If the flags are set to "AUTO", the behavior then depends on the type of renderer. Overlay Renderers will always
    // disable depth testing and writing. Color Renderers will enable them if the Layer does.
    SetupDepthBuffer( item, context, autoDepthTestMode, firstDepthBufferUse );

    // Set up clipping based on both the Renderer and Actor APIs.
    // The Renderer API will be used if specified. If AUTO, the Actors automatic clipping feature will be used.
    SetupClipping( item, context, usedStencilBuffer, lastClippingDepth, lastClippingId );

    // Render the item (we skip rendering for bounding box clips).
    if( item.mNode->GetClippingMode() != ClippingMode::CLIP_TO_BOUNDING_BOX )
    {
      item.mRenderer->Render( context, bufferIndex, *item.mNode, item.mModelMatrix, item.mModelViewMatrix,
                              viewMatrix, projectionMatrix, item.mSize, !item.mIsOpaque );
    }
  }
}

RenderAlgorithms::RenderAlgorithms()
  : mViewportRectangle(),
    mHasLayerScissor( false )
{
}

void RenderAlgorithms::ProcessRenderInstruction( const RenderInstruction& instruction, Context& context, BufferIndex bufferIndex )
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
        ProcessRenderList( *renderList, context, bufferIndex, *viewMatrix, *projectionMatrix );
      }
    }
  }
}


} // namespace Render

} // namespace Internal

} // namespace Dali
