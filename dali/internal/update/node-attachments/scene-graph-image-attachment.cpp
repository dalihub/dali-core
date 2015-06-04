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
#include <dali/internal/update/node-attachments/scene-graph-image-attachment.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/internal-constants.h>

#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/resources/complete-status-manager.h>

#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/nodes/node.h>

#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/vertex.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/renderers/scene-graph-image-renderer.h>
#include <dali/internal/render/renderers/scene-graph-renderer-declarations.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>

#include <dali/internal/update/node-attachments/scene-graph-image-attachment-debug.h>

namespace Dali
{

namespace Internal
{

// value types used by messages
template <> struct ParameterType< SceneGraph::ImageRenderer::MeshType >
: public BasicType< SceneGraph::ImageRenderer::MeshType > {};

namespace SceneGraph
{

ImageAttachment* ImageAttachment::New( unsigned int textureId )
{
  return new ImageAttachment( textureId );
}

ImageAttachment::ImageAttachment( unsigned int textureId )
: RenderableAttachment( false ), // no scaling
  mImageRenderer( NULL ),
  mTextureId( textureId ),
  mRefreshMeshData( true ),
  mIsPixelAreaSet( false ),
  mPreviousRefreshHints( 0 ),
  mStyle( Dali::ImageActor::STYLE_QUAD ),
  mCullFaceMode( CullNone )
{
}

void ImageAttachment::Initialize2( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( NULL != mSceneController );

  // Create main renderer, passing ownership to the render-thread
  mImageRenderer = ImageRenderer::New( *mParent );

  mSceneController->GetRenderMessageDispatcher().AddRenderer( *mImageRenderer );

  ATTACHMENT_LOG_FMT(Debug::General, " renderer: %p\n", mImageRenderer);

  if( mTextureId != 0 )
  {
    typedef MessageValue1< ImageRenderer, ResourceId > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( mImageRenderer, &ImageRenderer::SetTextureId, mTextureId );
  }

  // After derived classes have (potentially) created their renderer
  Renderer& renderer = GetRenderer();
  renderer.SetCullFace( mCullFaceMode );

  // set the default shader here as well
  renderer.SetShader( mShader );
}

void ImageAttachment::OnDestroy2()
{
  DALI_ASSERT_DEBUG( NULL != mSceneController );

  // Request deletion in the next Render
  mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mImageRenderer );
  mImageRenderer = NULL;
}

void ImageAttachment::ConnectedToSceneGraph()
{
  // Do nothing
}

void ImageAttachment::DisconnectedFromSceneGraph()
{
  // Do nothing
}

ImageAttachment::~ImageAttachment()
{
}

Renderer& ImageAttachment::GetRenderer()
{
  return *mImageRenderer;
}

const Renderer& ImageAttachment::GetRenderer() const
{
  return *mImageRenderer;
}

void ImageAttachment::SetTextureId( BufferIndex updateBufferIndex, unsigned int textureId )
{
  DALI_ASSERT_DEBUG(mSceneController);
  ATTACHMENT_LOG_FMT(Debug::General, " textureid: %d)\n", textureId);

  mTextureId = textureId;

  // Loading is essentially finished if we don't have a resource ID
  mFinishedResourceAcquisition = ( 0 == mTextureId );

  if( mImageRenderer )
  {
    typedef MessageValue1< ImageRenderer, ResourceId > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( mImageRenderer, &ImageRenderer::SetTextureId, mTextureId );
  }
}

void ImageAttachment::SetPixelArea( BufferIndex updateBufferIndex, const PixelArea& pixelArea )
{
  // update the pixel area information
  mIsPixelAreaSet = true;

  {
    typedef MessageValue1< ImageRenderer, ImageRenderer::PixelArea > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( mImageRenderer, &ImageRenderer::SetPixelArea, pixelArea );
  }

  // we rely on attachment to not call us unless it actually did change
  mRefreshMeshData = true;
}

void ImageAttachment::ClearPixelArea()
{
  mIsPixelAreaSet = false;
  mRefreshMeshData = true;
}

void ImageAttachment::SetStyle( Dali::ImageActor::Style style )
{
  if (mStyle != style)
  {
    mStyle = style;
    mRefreshMeshData = true;
  }
}

void ImageAttachment::SetBorder( BufferIndex updateBufferIndex, const Vector4& border, bool inPixels )
{
  // update the 9 patch border information

  typedef MessageValue2< ImageRenderer, Vector4, bool > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( mImageRenderer, &ImageRenderer::SetNinePatchBorder, border, inPixels );

  if (mStyle != Dali::ImageActor::STYLE_QUAD)
  {
    mRefreshMeshData = true;
  }
}

void ImageAttachment::SetBlendingOptions( BufferIndex updateBufferIndex, unsigned int options )
{
  // Blending options are forwarded to renderer in render-thread
  typedef MessageValue1< Renderer, unsigned int > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &GetRenderer(), &Renderer::SetBlendingOptions, options );
}

void ImageAttachment::SetBlendColor( BufferIndex updateBufferIndex, const Vector4& color )
{
  // Blend color is forwarded to renderer in render-thread
  typedef MessageValue1< Renderer, Vector4 > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &GetRenderer(), &Renderer::SetBlendColor, color );
}

void ImageAttachment::SetCullFace( BufferIndex updateBufferIndex, CullFaceMode mode )
{
  DALI_ASSERT_DEBUG(mSceneController);
  DALI_ASSERT_DEBUG(mode >= CullNone && mode <= CullFrontAndBack);

  mCullFaceMode = mode;

  typedef MessageValue1< Renderer, CullFaceMode > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &GetRenderer(), &Renderer::SetCullFace, mode );
}

void ImageAttachment::SetSampler( BufferIndex updateBufferIndex, unsigned int samplerBitfield )
{
  DALI_ASSERT_DEBUG(mSceneController);

  typedef MessageValue1< Renderer, unsigned int > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &GetRenderer(), &Renderer::SetSampler, samplerBitfield );
}

void ImageAttachment::ApplyShader( BufferIndex updateBufferIndex, Shader* shader )
{
  mShader = shader;

  // send the message to renderer
  SendShaderChangeMessage( updateBufferIndex );

  // tell derived class to do something
  ShaderChanged( updateBufferIndex );
}

void ImageAttachment::RemoveShader( BufferIndex updateBufferIndex )
{
  // return to default shader
  mShader = NULL;

  // send the message to renderer
  SendShaderChangeMessage( updateBufferIndex );

  // tell derived class to do something
  ShaderChanged( updateBufferIndex );
}


void ImageAttachment::ShaderChanged( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( mSceneController );

  int hints = 0; // default shader hints are 0
  if( mShader )
  {
    hints = mShader->GetGeometryHints();
  }
  if ( hints != mPreviousRefreshHints )
  {
    mPreviousRefreshHints = hints;
    // (may) need to change geometry
    mRefreshMeshData = true;
  }
}

void ImageAttachment::SizeChanged( BufferIndex updateBufferIndex )
{
  // avoid regenerating geometry if the size did not actually change (visibility change can accumulate a false SizeChanged)
  const Vector3& actorSize = GetParent().GetSize( updateBufferIndex );
  if( fabsf( actorSize.width  - mGeometrySize.width  ) > Math::MACHINE_EPSILON_1 ||
      fabsf( actorSize.height - mGeometrySize.height ) > Math::MACHINE_EPSILON_1 )
  {
    mRefreshMeshData = true;
  }
}

bool ImageAttachment::DoPrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager )
{
  DALI_LOG_TRACE_METHOD_FMT(gImageAttachmentLogFilter, "this:%p", this);
  bool ready = false;

  if( 0 != mTextureId )
  {
    // The metadata is used by IsFullyOpaque(), below.
    mBitmapMetadata = resourceManager.GetBitmapMetadata( mTextureId );

    CompleteStatusManager& completeStatusManager = mSceneController->GetCompleteStatusManager();
    CompleteStatusManager::CompleteState status = completeStatusManager.GetStatus( mTextureId );

    switch( status )
    {
      case CompleteStatusManager::NOT_READY:
      {
        ready = false;

        if( mBitmapMetadata.GetIsFramebuffer() )
        {
          ready = true;
        }
        mFinishedResourceAcquisition = false;
        FollowTracker( mTextureId );
      }
      break;

      case CompleteStatusManager::COMPLETE:
      {
        ready = true;
        mFinishedResourceAcquisition = true;
      }
      break;

      case CompleteStatusManager::NEVER:
      {
        ready = false;
        mFinishedResourceAcquisition = true;
      }
      break;
    }
  }
  else
  {
    // Loading is essentially finished if we don't have a resource ID
    mFinishedResourceAcquisition = true;
  }

  ATTACHMENT_LOG_FMT(Debug::General, " ObjName:%s finished:%s ready:%s \n",
                     DALI_LOG_GET_OBJECT_C_STR(mParent),
                     mFinishedResourceAcquisition?"T":"F", ready?"T":"F");

  return ready;
}

void ImageAttachment::DoPrepareRender( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( mSceneController && mImageRenderer );

  ATTACHMENT_LOG_FMT(Debug::General, "ObjName:%s textureId:%d\n",
                     DALI_LOG_GET_OBJECT_C_STR(mParent),
                     mTextureId);

  // Check whether we need to refresh the vertex buffer.
  if ( mRefreshMeshData )
  {
    const Vector3& actorSize = GetParent().GetSize( updateBufferIndex );
    mGeometrySize.x = actorSize.x;
    mGeometrySize.y = actorSize.y;

    ImageRenderer::MeshType meshType = ImageRenderer::GRID_QUAD;

    if ( !PreviousHintEnabled( Dali::ShaderEffect::HINT_GRID ) )
    {
      if ( mStyle == Dali::ImageActor::STYLE_NINE_PATCH )
      {
        meshType = ImageRenderer::NINE_PATCH;
      }
      else if ( mStyle == Dali::ImageActor::STYLE_NINE_PATCH_NO_CENTER )
      {
        meshType = ImageRenderer::NINE_PATCH_NO_CENTER;
      }
      else
      {
        meshType = ImageRenderer::QUAD;
      }
    }
    else
    {
      if ( mStyle == Dali::ImageActor::STYLE_NINE_PATCH )
      {
        meshType = ImageRenderer::GRID_NINE_PATCH;
      }
      else if ( mStyle == Dali::ImageActor::STYLE_NINE_PATCH_NO_CENTER )
      {
        meshType = ImageRenderer::GRID_NINE_PATCH_NO_CENTER;
      }
      else
      {
        meshType = ImageRenderer::GRID_QUAD;
      }
    }

    // Recalculate the mesh data in the next render
    {
      typedef MessageValue3< ImageRenderer, ImageRenderer::MeshType, Vector2, bool > DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( mImageRenderer, &ImageRenderer::CalculateMeshData, meshType, mGeometrySize, mIsPixelAreaSet );
    }

    mRefreshMeshData = false;
  }
}

void RenderableAttachment::SetBlendingMode( BlendingMode::Type mode )
{
  mBlendingMode = mode;
}

BlendingMode::Type RenderableAttachment::GetBlendingMode() const
{
  return mBlendingMode;
}

bool ImageAttachment::IsFullyOpaque( BufferIndex updateBufferIndex )
{
  bool fullyOpaque = true;

  switch( mBlendingMode )
  {
    case BlendingMode::OFF:
    {
      fullyOpaque = true;
      break;
    }
    case BlendingMode::ON:
    {
      // Blending always.
      fullyOpaque = false;
      break;
    }
    case BlendingMode::AUTO:
    {
      /**
       * Fully opaque when...
       *   1) not using the alpha channel from the image data
       *   2) the inherited color is not transparent nor semi-transparent
       *   3) the shader doesn't require blending
       */
      fullyOpaque = mBitmapMetadata.IsFullyOpaque();

      if ( fullyOpaque && mParent )
      {
        fullyOpaque = ( mParent->GetWorldColor(updateBufferIndex).a >= FULLY_OPAQUE );

        if ( fullyOpaque && mShader != NULL )
        {
          fullyOpaque = !PreviousHintEnabled( Dali::ShaderEffect::HINT_BLENDING );
        }
      }
    }
  }
  return fullyOpaque;
}

void ImageAttachment::SendShaderChangeMessage( BufferIndex updateBufferIndex )
{
  typedef MessageValue1< Renderer, Shader* > DerivedType;
  // Reserve memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
  // Construct message in the mRenderer queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &GetRenderer(), &Renderer::SetShader, mShader );
}



} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
