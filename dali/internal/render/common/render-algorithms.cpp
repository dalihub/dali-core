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
const short DaliDepthToGLDepthTable[] = { GL_NEVER, GL_ALWAYS, GL_LESS, GL_GREATER, GL_EQUAL, GL_NOTEQUAL, GL_LEQUAL, GL_GEQUAL };
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
 * @param[in] isLayer3D True if the layer is a 3D layer
 */
inline void SetRenderFlags( const RenderList& renderList, Context& context, bool depthTestEnabled, bool isLayer3D )
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

    clearMask |= ( renderFlags & RenderList::STENCIL_CLEAR ) ? GL_STENCIL_BUFFER_BIT : 0u;
  }

  // Write to stencil buffer or color buffer, but not both
  context.StencilMask( enableStencilWrite ? 0xFF : 0x00 );
  context.ColorMask( !enableStencilWrite );

  // Enable and Clear the depth buffer if required.
  // DepthTest must be enabled for the layer, else testing is turned off.
  if( !depthTestEnabled )
  {
    context.EnableDepthBuffer( false );
  }
  else if( renderList.HasColorRenderItems() || isLayer3D ) // Also, within the context of this if(), depth test is enabled.
  {

    clearMask |= GL_DEPTH_BUFFER_BIT;
    // We need to enable the depth buffer to clear it.
    // Subsequently it is enabled and disabled on a per-RenderItem basis.
    // If we do not have color renderers, this is only done for 3D layers.
    context.EnableDepthBuffer( true );
  }

  // Clear Depth and/or stencil buffers as required.
  // Note: The buffers will only be cleared if written to since a previous clear.
  context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
}

/**
 * Sets up the depth buffer for reading and writing based on the current render item.
 * The items read and write mode are used if specified.
 * If AUTO is selected for reading, the decision will be based on the Layer Behavior.
 * If AUTO is selected for writing, the decision will be based on the items opacity.
 * @param item The RenderItem to set up the depth buffer for
 * @param context The context used to execute GL commands.
 * @param isLayer3D True if the layer behavior is set to LAYER_3D
 */
inline void SetupDepthBuffer( const RenderItem& item, Context& context, bool isLayer3D )
{
  // Set up whether or not to write to the depth buffer.
  const DepthWriteMode::Type depthWriteMode = item.mRenderer->GetDepthWriteMode();
  // Most common mode (AUTO) is tested first.
  bool enableDepthWrite = ( ( depthWriteMode == DepthWriteMode::AUTO ) && item.mIsOpaque ) ||
                          ( depthWriteMode == DepthWriteMode::ON );
  context.DepthMask( enableDepthWrite );

  // Set up whether or not to read from (test) the depth buffer.
  const DepthTestMode::Type depthTestMode = item.mRenderer->GetDepthTestMode();
  // Most common mode (AUTO) is tested first.
  bool enableDepthTest = ( ( depthTestMode == DepthTestMode::AUTO ) && isLayer3D ) ||
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
 */
inline void ProcessRenderList(
  const RenderList& renderList,
  Context& context,
  SceneGraph::TextureCache& textureCache,
  SceneGraph::Shader& defaultShader,
  BufferIndex bufferIndex,
  const Matrix& viewMatrix,
  const Matrix& projectionMatrix )
{
  DALI_PRINT_RENDER_LIST( renderList );

  bool depthTestEnabled = !( renderList.GetSourceLayer()->IsDepthTestDisabled() );
  bool isLayer3D = renderList.GetSourceLayer()->GetBehavior() == Dali::Layer::LAYER_3D;

  SetScissorTest( renderList, context );
  SetRenderFlags( renderList, context, depthTestEnabled, isLayer3D );

  // The Layers depth enabled flag overrides the per-renderer depth flags.
  // So if depth test is disabled at the layer level, we ignore per-render flags.
  // Note: Overlay renderers will not read or write from the depth buffer.
  if( DALI_LIKELY( !renderList.HasColorRenderItems() || !depthTestEnabled ) )
  {
    size_t count = renderList.Count();
    for ( size_t index = 0; index < count; ++index )
    {
      const RenderItem& item = renderList.GetItem( index );
      DALI_PRINT_RENDER_ITEM( item );

      item.mRenderer->Render( context, textureCache, bufferIndex, *item.mNode, defaultShader,
                              item.mModelMatrix, item.mModelViewMatrix, viewMatrix, projectionMatrix, item.mSize, !item.mIsOpaque );
    }
  }
  else
  {
    size_t count = renderList.Count();
    for ( size_t index = 0; index < count; ++index )
    {
      const RenderItem& item = renderList.GetItem( index );
      DALI_PRINT_RENDER_ITEM( item );

      // Set up the depth buffer based on per-renderer flags.
      SetupDepthBuffer( item, context, isLayer3D );

      item.mRenderer->Render( context, textureCache, bufferIndex, *item.mNode, defaultShader,
                              item.mModelMatrix, item.mModelViewMatrix, viewMatrix, projectionMatrix, item.mSize, !item.mIsOpaque );
    }
  }
}

void ProcessRenderInstruction( const RenderInstruction& instruction,
                               Context& context,
                               SceneGraph::TextureCache& textureCache,
                               SceneGraph::Shader& defaultShader,
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
        ProcessRenderList( *renderList, context, textureCache, defaultShader, bufferIndex, *viewMatrix, *projectionMatrix );
      }
    }
  }
}

} // namespace Render

} // namespace Internal

} // namespace Dali
