/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/frame-buffer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/frame-buffer-impl.h> // Dali::Internal::FrameBuffer


namespace Dali
{

FrameBuffer FrameBuffer::New( uint32_t width, uint32_t height, uint32_t attachments )
{
  Internal::FrameBufferPtr frameBuffer = Internal::FrameBuffer::New( width, height, attachments );
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

void FrameBuffer::AttachColorTexture( Texture& texture )
{
  AttachColorTexture( texture, 0u, 0u );
}

void FrameBuffer::AttachColorTexture( Texture& texture, uint32_t mipmapLevel, uint32_t layer )
{
  if( texture )
  {
    Internal::TexturePtr texturePtr( &GetImplementation( texture ) );
    GetImplementation(*this).AttachColorTexture( texturePtr, mipmapLevel, layer );
  }
}

Texture FrameBuffer::GetColorTexture()
{
  Internal::Texture* texturePtr = GetImplementation(*this).GetColorTexture();
  return Dali::Texture( texturePtr );
}

} //namespace Dali
