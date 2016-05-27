/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-frame-buffer.h>

namespace Dali
{
namespace Internal
{

FrameBufferPtr FrameBuffer::New( unsigned int width, unsigned int height, Format format  )
{
  FrameBufferPtr frameBuffer( new FrameBuffer( width, height, format ) );
  frameBuffer->Initialize();
  return frameBuffer;
}


Render::FrameBuffer* FrameBuffer::GetRenderObject() const
{
  return mRenderObject;
}

FrameBuffer::FrameBuffer( unsigned int width, unsigned int height, Format format )
: mEventThreadServices( *Stage::GetCurrent() ),
  mRenderObject( NULL ),
  mColor(NULL),
  mWidth( width ),
  mHeight( height ),
  mFormat( format )
{
}

void FrameBuffer::Initialize()
{
  mRenderObject = new Render::FrameBuffer( mWidth, mHeight, mFormat );
  AddFrameBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject );
}

void FrameBuffer::AttachColorTexture( NewTexturePtr texture, unsigned int mipmapLevel, unsigned int layer )
{
  if( (unsigned int)(texture->GetWidth() / (1<<mipmapLevel)) != mWidth ||
      (unsigned int)(texture->GetHeight() / (1<<mipmapLevel)) != mHeight )
  {
    DALI_LOG_ERROR( "Failed to attach color texture to FrameBuffer: Size mismatch \n" );
  }
  else
  {
    mColor = texture;
    AttachColorTextureToFrameBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject, texture->GetRenderObject(), mipmapLevel, layer );
  }
}

NewTexture* FrameBuffer::GetColorTexture()
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
