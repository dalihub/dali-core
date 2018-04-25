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
#include <dali/internal/event/rendering/frame-buffer-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/rendering/scene-graph-frame-buffer.h>

namespace Dali
{
namespace Internal
{

FrameBufferPtr FrameBuffer::New( unsigned int width, unsigned int height, unsigned int attachments  )
{
  FrameBufferPtr frameBuffer( new FrameBuffer( width, height, attachments ) );
  frameBuffer->Initialize();
  return frameBuffer;
}


SceneGraph::FrameBuffer* FrameBuffer::GetRenderObject() const
{
  return mRenderObject;
}

FrameBuffer::FrameBuffer( unsigned int width, unsigned int height, unsigned int attachments )
: mEventThreadServices( *Stage::GetCurrent() ),
  mRenderObject( NULL ),
  mColor( NULL ),
  mWidth( width ),
  mHeight( height ),
  mAttachments( attachments )
{
}

void FrameBuffer::Initialize()
{
  mRenderObject = new SceneGraph::FrameBuffer( mWidth, mHeight, mAttachments );
  AddFrameBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject );
}

void FrameBuffer::AttachColorTexture( TexturePtr texture, unsigned int mipmapLevel, unsigned int layer )
{
  if( ( texture->GetWidth() / ( 1u << mipmapLevel ) == mWidth ) &&
      ( texture->GetHeight() / ( 1u << mipmapLevel ) == mHeight ) )
  {
    mColor = texture;
    AttachColorTextureToFrameBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject, texture->GetRenderObject(), mipmapLevel, layer );
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
    RemoveFrameBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject );
  }
}


} // namespace Internal
} // namespace Dali
