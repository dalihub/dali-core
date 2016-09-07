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
#include <dali/internal/update/manager/geometry-batcher.h>

using Dali::Internal::SceneGraph::RenderItem;
using Dali::Internal::SceneGraph::RenderList;
using Dali::Internal::SceneGraph::RenderListContainer;
using Dali::Internal::SceneGraph::RenderInstruction;
using Dali::Internal::SceneGraph::GeometryBatcher;

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
    context.Scissor(clip.x, clip.y, clip.width, clip.height);
  }
  else
  {
    context.SetScissorTest( false );
  }
}

/**
 * Sets the render flags for the stencil buffer and clears all required buffers (depth and stencil if required).
 * @param[in] renderList The render list from which to get the render flags
 * @param[in] context The context
 * @param[in] depthTestEnabled True if depth test is enabled for the layer
 */
inline void SetRenderFlags( const RenderList& renderList, Context& context, bool depthTestEnabled )
{
  const unsigned int renderFlags = renderList.GetFlags();
  GLbitfield clearMask = 0u;

  // Stencil enabled, writing, and clearing...
  const bool enableStencilBuffer( renderFlags & RenderList::STENCIL_BUFFER_ENABLED );
  const bool enableStencilWrite( renderFlags & RenderList::STENCIL_WRITE );
  context.EnableStencilBuffer( enableStencilBuffer );
  if( enableStencilBuffer )
  {
    context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
    context.StencilOp( GL_KEEP, GL_REPLACE, GL_REPLACE );

    // Write to stencil buffer or color buffer, but not both.
    // These should only be set if the Actor::DrawMode is managing the stencil (and color) buffer.
    context.StencilMask( enableStencilWrite ? 0xFF : 0x00 );
    context.ColorMask( !enableStencilWrite );

    clearMask |= ( renderFlags & RenderList::STENCIL_CLEAR ) ? GL_STENCIL_BUFFER_BIT : 0u;
  }

  // Enable and Clear the depth buffer if required.
  if( depthTestEnabled )
  {
    clearMask |= GL_DEPTH_BUFFER_BIT;

    // We need to enable the depth buffer to clear it.
    // Subsequently it is enabled and disabled on a per-RenderItem basis.
    context.EnableDepthBuffer( true );
  }
  else
  {
    context.EnableDepthBuffer( false );
  }

  // Clear Depth and/or stencil buffers as required.
  // Note: The buffers will only be cleared if written to since a previous clear.
  context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
}

/**
 * @brief This method sets up the stencil and color buffer based on the current Renderers flags.
 * @param[in]     item                     The current RenderItem about to be rendered
 * @param[in]     context                  The context
 * @param[in/out] usedStencilBuffer        True if the stencil buffer has been used so far within this RenderList
 * @param[in]     stencilManagedByDrawMode True if the stencil and color buffer is being managed by DrawMode::STENCIL
 */
inline void SetupPerRendererFlags( const RenderItem& item, Context& context, bool& usedStencilBuffer, bool stencilManagedByDrawMode )
{
  // DrawMode::STENCIL is deprecated, however to support it we must not set
  // flags based on the renderer properties if it is in use.
  if( stencilManagedByDrawMode )
  {
    return;
  }

  // Setup the color buffer based on the renderers properties.
  Renderer *renderer = item.mRenderer;
  RenderMode::Type renderMode = renderer->GetRenderMode();
  const bool writeToColorBuffer = ( renderMode == RenderMode::AUTO ) || ( renderMode == RenderMode::COLOR ) || ( renderMode == RenderMode::COLOR_STENCIL );
  context.ColorMask( writeToColorBuffer );

  // If the stencil buffer is disabled for this renderer, exit now to save unnecessary value setting.
  if( ( renderMode != RenderMode::STENCIL ) && ( renderMode != RenderMode::COLOR_STENCIL ) )
  {
    // No per-renderer stencil setup, exit.
    context.EnableStencilBuffer( false );
    return;
  }

  // At this point, the stencil buffer is enabled.
  context.EnableStencilBuffer( true );

  // If this is the first use of the stencil buffer within this RenderList, clear it now.
  // This avoids unnecessary clears.
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

/**
 * Sets up the depth buffer for reading and writing based on the current render item.
 * The items read and write mode are used if specified.
 * If AUTO is selected for reading, the decision will be based on the Layer Behavior.
 * If AUTO is selected for writing, the decision will be based on the items opacity.
 * @param item The RenderItem to set up the depth buffer for
 * @param context The context used to execute GL commands.
 * @param depthTestEnabled True if depth testing has been enabled.
 */
inline void SetupDepthBuffer( const RenderItem& item, Context& context, bool depthTestEnabled )
{
  // Set up whether or not to write to the depth buffer.
  const DepthWriteMode::Type depthWriteMode = item.mRenderer->GetDepthWriteMode();
  // Most common mode (AUTO) is tested first.
  bool enableDepthWrite = ( ( depthWriteMode == DepthWriteMode::AUTO ) && depthTestEnabled && item.mIsOpaque ) ||
                          ( depthWriteMode == DepthWriteMode::ON );
  context.DepthMask( enableDepthWrite );

  // Set up whether or not to read from (test) the depth buffer.
  const DepthTestMode::Type depthTestMode = item.mRenderer->GetDepthTestMode();
  // Most common mode (AUTO) is tested first.
  bool enableDepthTest = ( ( depthTestMode == DepthTestMode::AUTO ) && depthTestEnabled ) ||
                         ( depthTestMode == DepthTestMode::ON );
  // Look-up the GL depth function from the Dali::DepthFunction enum, and set it.
  context.DepthFunc( DaliDepthToGLDepthTable[ item.mRenderer->GetDepthFunction() ] );

  // The depth buffer must be enabled if either reading or writing.
  context.EnableDepthBuffer( enableDepthWrite || enableDepthTest );
}

/**
 * Process a render-list.
 * @param[in] renderList The render-list to process.
 * @param[in] context The GL context.
 * @param[in] defaultShader The default shader to use.
 * @param[in] buffer The current render buffer index (previous update buffer)
 * @param[in] viewMatrix The view matrix from the appropriate camera.
 * @param[in] projectionMatrix The projection matrix from the appropriate camera.
 * @param[in] geometryBatcher The instance of the geometry batcher
 */
inline void ProcessRenderList(
  const RenderList& renderList,
  Context& context,
  SceneGraph::TextureCache& textureCache,
  SceneGraph::Shader& defaultShader,
  BufferIndex bufferIndex,
  const Matrix& viewMatrix,
  const Matrix& projectionMatrix,
  GeometryBatcher* geometryBatcher )
{
  DALI_PRINT_RENDER_LIST( renderList );

  bool autoDepthTestMode = !( renderList.GetSourceLayer()->IsDepthTestDisabled() ) && renderList.HasColorRenderItems();
  bool usedStencilBuffer = false;
  bool stencilManagedByDrawMode = renderList.GetFlags() & RenderList::STENCIL_BUFFER_ENABLED;

  SetScissorTest( renderList, context );
  SetRenderFlags( renderList, context, autoDepthTestMode );

  const std::size_t count = renderList.Count();
  bool skip( false );
  for ( size_t index = 0; index < count; ++index )
  {
    const RenderItem& item = renderList.GetItem( index );
    DALI_PRINT_RENDER_ITEM( item );

    // Set up the depth buffer based on per-renderer flags.
    // If the per renderer flags are set to "ON" or "OFF", they will always override any Layer depth mode or
    // draw-mode state, such as Overlays.
    // If the flags are set to "AUTO", the behaviour then depends on the type of renderer. Overlay Renderers will always
    // disable depth testing and writing. Color Renderers will enable them if the Layer does.
    SetupDepthBuffer( item, context, autoDepthTestMode );
    SetupPerRendererFlags( item, context, usedStencilBuffer, stencilManagedByDrawMode );

    // Check if the node has a valid batch index value ( set previously by
    // GeometryBatcher ). If so, then it queries the geometry object for this particular batch.
    // If not, it still checks if the batch parent is set as it is possible, batching may
    // fail ( for example if vertex format or buffers are not set ). In that case we need
    // to skip rendering, otherwise unwanted GPU buffers will get uploaded. This is very rare case.
    uint32_t batchIndex = item.mNode->mBatchIndex;
    if( batchIndex != BATCH_NULL_HANDLE )
    {
      item.mBatchRenderGeometry = geometryBatcher->GetGeometry( batchIndex );
    }
    else
    {
      skip = item.mNode->GetBatchParent();
      item.mBatchRenderGeometry = NULL;
    }
    if( DALI_LIKELY( !skip ) )
    {
      item.mRenderer->Render( context, textureCache, bufferIndex, *item.mNode, defaultShader,
                            item.mModelMatrix, item.mModelViewMatrix, viewMatrix, projectionMatrix, item.mSize, item.mBatchRenderGeometry, !item.mIsOpaque );
    }
  }
}

void ProcessRenderInstruction( const RenderInstruction& instruction,
                               Context& context,
                               SceneGraph::TextureCache& textureCache,
                               SceneGraph::Shader& defaultShader,
                               GeometryBatcher& geometryBatcher,
                               BufferIndex bufferIndex )
{
  DALI_PRINT_RENDER_INSTRUCTION( instruction, bufferIndex );

  const Matrix* viewMatrix       = instruction.GetViewMatrix( bufferIndex );
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );

  DALI_ASSERT_DEBUG( NULL != viewMatrix );
  DALI_ASSERT_DEBUG( NULL != projectionMatrix );

  if( NULL != viewMatrix &&
      NULL != projectionMatrix )
  {
    const RenderListContainer::SizeType count = instruction.RenderListCount();

    // Iterate through each render list in order. If a pair of render lists
    // are marked as interleaved, then process them together.
    for( RenderListContainer::SizeType index = 0; index < count; ++index )
    {
      const RenderList* renderList = instruction.GetRenderList( index );

      if(  renderList &&
          !renderList->IsEmpty() )
      {
        ProcessRenderList( *renderList, context, textureCache, defaultShader, bufferIndex, *viewMatrix, *projectionMatrix, &geometryBatcher );
      }
    }
  }
}

} // namespace Render

} // namespace Internal

} // namespace Dali
