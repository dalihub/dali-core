/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

} // Unnamed namespace

/**
 * Sets up the scissor test if required.
 * @param[in] renderList The render list from which to get the clipping flag
 * @param[in] context The context
 */
inline void SetScissorTest( const RenderList& renderList, Context& context )
{
  // Scissor testing
  if( renderList.IsClipping() )
  {
    context.SetScissorTest( true );

    const Dali::ClippingBox& clip = renderList.GetClippingBox();
    context.Scissor( clip.x, clip.y, clip.width, clip.height );
  }
  else
  {
    context.SetScissorTest( false );
  }
}

/**
 * @brief Set up the stencil and color buffer for automatic clipping (StencilMode::AUTO).
 * @param[in]     item                     The current RenderItem about to be rendered
 * @param[in]     context                  The context
 * @param[in/out] lastStencilDepth         The stencil depth of the last renderer drawn.
 * @param[in/out] lastClippingId           The clipping ID of the last renderer drawn.
 */
inline void SetupClipping( const RenderItem& item, Context& context, uint32_t& lastStencilDepth, uint32_t& lastClippingId )
{
  const Dali::Internal::SceneGraph::Node* node = item.mNode;
  const uint32_t clippingId = node->GetClippingId();

  // Turn the color buffer on as we always want to render this renderer, regardless of clipping hierarchy.
  context.ColorMask( true );

  // If there is no clipping Id, then either we haven't reached a clipping Node yet, or there aren't any.
  // Either way we can skip clipping setup for this renderer.
  if( clippingId == 0u )
  {
    // Exit immediately if there are no clipping actions to perform (EG. we have not yet hit a clipping node).
    context.EnableStencilBuffer( false );
    return;
  }

  const ClippingMode::Type clippingMode( node->GetClippingMode() );
  const uint32_t currentStencilDepth( node->GetClippingDepth() );

  context.EnableStencilBuffer( true );

  // Pre-calculate a mask which has all bits set up to and including the current clipping depth.
  // EG. If depth is 3, the mask would be "111" in binary.
  const uint32_t currentDepthMask = ( 1u << currentStencilDepth ) - 1u;

  // If we have a clipping mode specified, we are writing to the stencil buffer.
  if( clippingMode != ClippingMode::DISABLED )
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
    else if( ( currentStencilDepth < lastStencilDepth ) || ( clippingId != lastClippingId ) )
    {
      // The above if() statement tests if we need to clear some (not all) stencil bit-planes.
      // We need to do this if either of the following are true:
      //   1) We traverse up the scene-graph to a previous stencil
      //   2) We are at the same stencil depth but the clipping Id has changed.
      //
      // This calculation takes the new depth to move to, and creates an inverse-mask of that number of consecutive bits.
      // This has the effect of clearing everything except the bit-planes up to (and including) our current depth.
      const uint32_t stencilClearMask = ( currentDepthMask >> 1u ) ^ 0xff;

      context.StencilMask( stencilClearMask );
      context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
    }

    // We keep track of the last clipping Id and depth so we can determine when we are
    // moving back up the scene graph and require some of the stencil bit-planes to be deleted.
    lastStencilDepth = currentStencilDepth;
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
 * @brief Set up the stencil and color buffer based on the current Renderers properties.
 * @param[in]     item                     The current RenderItem about to be rendered
 * @param[in]     context                  The context
 * @param[in/out] usedStencilBuffer        True if the stencil buffer has been used so far within this RenderList. Used by StencilMode::ON.
 * @param[in/out] lastStencilDepth         The stencil depth of the last renderer drawn. Used by the clipping feature.
 * @param[in/out] lastClippingId           The clipping ID of the last renderer drawn.   Used by the clipping feature.
 */
inline void SetupStencilBuffer( const RenderItem& item, Context& context, bool& usedStencilBuffer, uint32_t& lastStencilDepth, uint32_t& lastClippingId )
{
  const Renderer *renderer = item.mRenderer;

  // Setup the stencil using either the automatic clipping feature, or, the manual per-renderer stencil API.
  // Note: This switch is in order of most likely value first.
  RenderMode::Type renderMode = renderer->GetRenderMode();
  switch( renderMode )
  {
    case RenderMode::AUTO:
    {
      // The automatic clipping feature will manage the stencil functions and color buffer mask.
      SetupClipping( item, context, lastStencilDepth, lastClippingId );
      break;
    }

    case RenderMode::NONE:
    case RenderMode::COLOR:
    {
      // The stencil buffer will not be used at all.
      context.EnableStencilBuffer( false );

      // Setup the color buffer based on the RenderMode.
      context.ColorMask( renderMode == RenderMode::COLOR );
      return;
      break; // Break statement for consistency (although return will be called instead).
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

/**
 * @brief Sets up the depth buffer for reading and writing based on the current render item.
 * The items read and write mode are used if specified.
 *  - If AUTO is selected for reading, the decision will be based on the Layer Behavior.
 *  - If AUTO is selected for writing, the decision will be based on the items opacity.
 * @param[in]     item                The RenderItem to set up the depth buffer for.
 * @param[in]     context             The context used to execute GL commands.
 * @param[in]     depthTestEnabled    True if depth testing has been enabled.
 * @param[in/out] firstDepthBufferUse Initialise to true on the first call, this method will set it to false afterwards.
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

/**
 * @brief Process a render-list.
 * @param[in] renderList       The render-list to process.
 * @param[in] context          The GL context.
 * @param[in] defaultShader    The default shader to use.
 * @param[in] buffer           The current render buffer index (previous update buffer)
 * @param[in] viewMatrix       The view matrix from the appropriate camera.
 * @param[in] projectionMatrix The projection matrix from the appropriate camera.
 */
inline void ProcessRenderList(
  const RenderList& renderList,
  Context& context,
  SceneGraph::Shader& defaultShader,
  BufferIndex bufferIndex,
  const Matrix& viewMatrix,
  const Matrix& projectionMatrix )
{
  DALI_PRINT_RENDER_LIST( renderList );

  SetScissorTest( renderList, context );

  // Note: The depth buffer is enabled or disabled on a per-renderer basis.
  // Here we pre-calculate the value to use if these modes are set to AUTO.
  const bool autoDepthTestMode( !( renderList.GetSourceLayer()->IsDepthTestDisabled() ) && renderList.HasColorRenderItems() );
  const std::size_t count = renderList.Count();
  uint32_t lastStencilDepth( 0u );
  uint32_t lastClippingId( 0u );
  bool usedStencilBuffer( false );
  bool firstDepthBufferUse( true );

  for( size_t index( 0u ); index < count; ++index )
  {
    const RenderItem& item = renderList.GetItem( index );
    DALI_PRINT_RENDER_ITEM( item );

    // Set up the depth buffer based on per-renderer flags.
    // If the per renderer flags are set to "ON" or "OFF", they will always override any Layer depth mode or
    // draw-mode state, such as Overlays.
    // If the flags are set to "AUTO", the behaviour then depends on the type of renderer. Overlay Renderers will always
    // disable depth testing and writing. Color Renderers will enable them if the Layer does.
    SetupDepthBuffer( item, context, autoDepthTestMode, firstDepthBufferUse );

    // Set up the stencil buffer based on both the Renderer and Actor APIs.
    // The Renderer API will be used if specified. If AUTO, the Actors automatic clipping feature will be used.
    SetupStencilBuffer( item, context, usedStencilBuffer, lastStencilDepth, lastClippingId );

    // Render the item
    item.mRenderer->Render( context,
                            bufferIndex,
                            *item.mNode,
                            defaultShader,
                            item.mModelMatrix,
                            item.mModelViewMatrix,
                            viewMatrix,
                            projectionMatrix,
                            item.mSize,
                            !item.mIsOpaque );
  }
}

void ProcessRenderInstruction( const RenderInstruction& instruction,
                               Context& context,
                               SceneGraph::Shader& defaultShader,
                               BufferIndex bufferIndex )
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
                           defaultShader,
                           bufferIndex,
                           *viewMatrix,
                           *projectionMatrix );
      }
    }
  }
}

} // namespace Render

} // namespace Internal

} // namespace Dali
