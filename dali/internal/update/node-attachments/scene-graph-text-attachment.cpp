/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/node-attachments/scene-graph-text-attachment.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/text-parameters.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/vertex.h>
#include <dali/internal/render/renderers/scene-graph-text-renderer.h>
#include <dali/internal/render/renderers/scene-graph-renderer-declarations.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/integration-api/debug.h>

#if defined(DEBUG_ENABLED)
namespace
{
Debug::Filter* gTextFilter = Debug::Filter::New(Debug::Concise, false, "LOG_SCENE_GRAPH_TEXT_ATTACHMENT");
}
#endif

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

TextAttachment* TextAttachment::New( )
{
  return new TextAttachment;
}

TextAttachment::TextAttachment()
: RenderableAttachment( true ), // scale enabled
  mTextRenderer( NULL ),
  mGeometrySize(), // Vector2::ZERO
  mGeometryScaling(), // Vector2::ZERO
  mAtlasId(0)
{
  DALI_LOG_INFO(gTextFilter, Debug::General, "TextAttachment::constructor(this=%p)\n", this);
}

void TextAttachment::ConnectToSceneGraph2( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( NULL != mSceneController );

  // Create the renderer, passing ownership to the render-thread
  mTextRenderer = TextRenderer::New( *mParent );
  mTextRenderer->SetUseBlend( true ); // Text renderer always requires blending

  mSceneController->GetRenderMessageDispatcher().AddRenderer( *mTextRenderer );
}

void TextAttachment::OnDestroy2()
{
  DALI_ASSERT_DEBUG( NULL != mSceneController );

  // Request deletion in the next Render
  mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mTextRenderer );
  mTextRenderer = NULL;
}

TextAttachment::~TextAttachment()
{
}

Renderer& TextAttachment::GetRenderer()
{
  return *mTextRenderer;
}

const Renderer& TextAttachment::GetRenderer() const
{
  return *mTextRenderer;
}

void TextAttachment::SetTextVertexBuffer( BufferIndex updateBufferIndex, TextVertexBuffer* vertexBuffer )
{
  DALI_LOG_INFO( gTextFilter, Debug::Verbose, "TextAttachment::SetTextVertexBuffer( vertexBuffer:%p\n", vertexBuffer );

  if( vertexBuffer )
  {
    mGeometrySize = vertexBuffer->mVertexMax;
    SetRecalculateScaleForSize(); // update cached scaling if geometry is changed
    mAtlasId = vertexBuffer->mTextureId;

    typedef MessageValue1< TextRenderer, OwnerPointer< TextVertexBuffer> > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( mTextRenderer, &TextRenderer::SetVertexData, vertexBuffer );
  }
}

void TextAttachment::SetTextFontSize( BufferIndex updateBufferIndex, float pixelSize )
{
  typedef MessageValue1< TextRenderer, float > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( mTextRenderer, &TextRenderer::SetFontSize, pixelSize );
}

void TextAttachment::SetGradient( BufferIndex updateBufferIndex, const Vector4& color, const Vector2& startPoint, const Vector2& endPoint )
{
  DALI_ASSERT_DEBUG(mSceneController);

  typedef MessageValue3< TextRenderer, Vector4, Vector2, Vector2 > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( mTextRenderer, &TextRenderer::SetGradient, color, startPoint, endPoint );
}

void TextAttachment::SetTextColor( BufferIndex updateBufferIndex, const Vector4& color )
{
  DALI_ASSERT_DEBUG(mSceneController);

  typedef MessageValue1< TextRenderer, Vector4 > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( mTextRenderer, &TextRenderer::SetTextColor, color );
}

void TextAttachment::SetOutline( BufferIndex updateBufferIndex, const bool enable, const Vector4& color, const Vector2& params )
{
  DALI_ASSERT_DEBUG(mSceneController);

  typedef MessageValue3< TextRenderer, bool, Vector4, Vector2 > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( mTextRenderer, &TextRenderer::SetOutline, enable, color, params );
}

void TextAttachment::SetGlow( BufferIndex updateBufferIndex, const bool enable, const Vector4& color, const float params )
{
  DALI_ASSERT_DEBUG(mSceneController);

  typedef MessageValue3< TextRenderer, bool, Vector4, float > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( mTextRenderer, &TextRenderer::SetGlow, enable, color, params );
}

void TextAttachment::SetDropShadow( BufferIndex updateBufferIndex, const bool enable, const Vector4& color, const Vector2& offset, const float size )
{
  DALI_ASSERT_DEBUG(mSceneController);

  typedef MessageValue4< TextRenderer, bool, Vector4, Vector2, float > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( mTextRenderer, &TextRenderer::SetDropShadow, enable, color, offset, size );
}

void TextAttachment::SetSmoothEdge( BufferIndex updateBufferIndex, const float params )
{
  DALI_ASSERT_DEBUG(mSceneController);

  typedef MessageValue1< TextRenderer, float > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( mTextRenderer, &TextRenderer::SetSmoothEdge, params );
}

void TextAttachment::ShaderChanged( BufferIndex updateBufferIndex )
{
  // nothing to do
}

void TextAttachment::SizeChanged( BufferIndex updateBufferIndex )
{
  SetRecalculateScaleForSize();
}

bool TextAttachment::DoPrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager )
{
  bool resourcesAvailable = false;
  mFinishedResourceAcquisition = false;

  if( mAtlasId > 0)
  {
    if( resourceManager.IsResourceLoaded( mAtlasId ) )
    {
      // Check if currently loading glyph sets have finished writing to atlas
      resourcesAvailable = true;

      Integration::LoadStatus status = resourceManager.GetAtlasLoadStatus( mAtlasId );

      if( status == Integration::RESOURCE_COMPLETELY_LOADED)
      {
        // all glyphs are loaded with high quality
        mFinishedResourceAcquisition = true;
      }
    }
  }

  DALI_LOG_INFO(gTextFilter, Debug::Verbose, "TextAttachment::DoPrepareResources(this=%p) == %d  Complete:%d\n", this, resourcesAvailable, mFinishedResourceAcquisition);

  return resourcesAvailable;
}

void TextAttachment::DoPrepareRender( BufferIndex updateBufferIndex )
{
}

void TextAttachment::DoGetScaleForSize( const Vector3& nodeSize, Vector3& scaling )
{
  // update cached scale for size if needed
  if( IsScaleForSizeDirty() )
  {
    // scale the geometry to fill the actor
    if( mGeometrySize.width > 0.0f )
    {
      mGeometryScaling.width = nodeSize.width / mGeometrySize.width;
    }
    if( mGeometrySize.height > 0.0f )
    {
      mGeometryScaling.height = nodeSize.height / mGeometrySize.height;
    }
  }
  // use the already calculated value
  scaling.width  = mGeometryScaling.width;
  scaling.height = mGeometryScaling.height;
  scaling.depth  = 1.0f;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
