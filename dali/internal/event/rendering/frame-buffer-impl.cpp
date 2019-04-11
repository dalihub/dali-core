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
#include <dali/internal/event/rendering/frame-buffer-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-frame-buffer.h>
#include <dali/integration-api/render-surface.h>

namespace Dali
{
namespace Internal
{

FrameBufferPtr FrameBuffer::New( uint32_t width, uint32_t height, Mask attachments  )
{
  FrameBufferPtr frameBuffer( new FrameBuffer( width, height, attachments ) );
  frameBuffer->Initialize();
  return frameBuffer;
}

FrameBufferPtr FrameBuffer::New( Dali::Integration::RenderSurface& renderSurface, Mask attachments )
{
  Dali::PositionSize positionSize = renderSurface.GetPositionSize();
  FrameBufferPtr frameBuffer( new FrameBuffer( positionSize.width, positionSize.height, attachments ) );
  frameBuffer->Initialize( &renderSurface );
  return frameBuffer;
}

SceneGraph::FrameBuffer* FrameBuffer::GetRenderObject() const
{
  return mRenderObject;
}

FrameBuffer::FrameBuffer( uint32_t width, uint32_t height, Mask attachments )
: mEventThreadServices( EventThreadServices::Get() ),
  mRenderObject( NULL ),
  mColor( NULL ),
  mWidth( width ),
  mHeight( height ),
  mAttachments( attachments )
{
}

void FrameBuffer::Initialize( Integration::RenderSurface* renderSurface )
{
  mRenderObject = new SceneGraph::FrameBuffer( mWidth, mHeight, mAttachments );
  OwnerPointer< SceneGraph::FrameBuffer > transferOwnership( mRenderObject );
  AddFrameBufferMessage( mEventThreadServices.GetUpdateManager(), transferOwnership );
  //AddFrameBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject );
}

void FrameBuffer::AttachColorTexture( TexturePtr texture, uint32_t mipmapLevel, uint32_t layer )
{
  if( ( texture->GetWidth() / ( 1u << mipmapLevel ) == mWidth ) &&
      ( texture->GetHeight() / ( 1u << mipmapLevel ) == mHeight ) )
  {
    mColor = texture;
    AttachColorTextureMessage( mEventThreadServices, *mRenderObject, texture->GetRenderObject(), mipmapLevel, layer );
  }
  else
  {
    DALI_LOG_ERROR( "Failed to attach color texture to FrameBuffer: Size mismatch \n" );
  }
}

Texture* FrameBuffer::GetColorTexture()
{
  return mColor.Get();
}

FrameBuffer::~FrameBuffer()
{
  if( EventThreadServices::IsCoreRunning() && mRenderObject )
  {
    RemoveFrameBufferMessage( mEventThreadServices.GetUpdateManager(), *mRenderObject );
  }
}


} // namespace Internal
} // namespace Dali
