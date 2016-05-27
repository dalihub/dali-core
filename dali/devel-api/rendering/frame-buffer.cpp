/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use *this file except in compliance with the License.
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
#include <dali/devel-api/rendering/frame-buffer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/frame-buffer-impl.h> // Dali::Internal::FrameBuffer


namespace Dali
{

FrameBuffer FrameBuffer::New( unsigned int width, unsigned int height, Format format )
{
  Internal::FrameBufferPtr frameBuffer = Internal::FrameBuffer::New( width, height, format );
  return FrameBuffer( frameBuffer.Get() );
}

FrameBuffer::FrameBuffer()
{
}

FrameBuffer::~FrameBuffer()
{
}

FrameBuffer::FrameBuffer( const FrameBuffer& handle )
: BaseHandle( handle )
{
}

FrameBuffer FrameBuffer::DownCast( BaseHandle handle )
{
  return FrameBuffer( dynamic_cast<Dali::Internal::FrameBuffer*>(handle.GetObjectPtr()));
}

FrameBuffer& FrameBuffer::operator=( const FrameBuffer& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

FrameBuffer::FrameBuffer( Internal::FrameBuffer* pointer )
: BaseHandle( pointer )
{
}

void FrameBuffer::AttachColorTexture( Texture& texture, unsigned int mipmapLevel, unsigned int layer )
{
  if( texture )
  {
    Internal::NewTexturePtr texturePtr( &GetImplementation( texture ) );
    GetImplementation(*this).AttachColorTexture( texturePtr, mipmapLevel, layer );
  }
}

Texture FrameBuffer::GetColorTexture()
{
  Internal::NewTexture* texturePtr = GetImplementation(*this).GetColorTexture();
  return Dali::Texture( texturePtr );
}

} //namespace Dali
