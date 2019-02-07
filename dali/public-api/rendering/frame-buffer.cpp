/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <type_traits>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h> // DALI_LOG_WARNING_NOFN
#include <dali/internal/event/rendering/frame-buffer-impl.h> // Dali::Internal::FrameBuffer
#include <dali/internal/event/rendering/texture-impl.h> // Dali::Internal::Texture

namespace Dali
{

namespace
{
/// Bool operator for FrameBuffer::Attachment::Mask.
/// in cpp as only used in this file
bool operator&( FrameBuffer::Attachment::Mask lhs, FrameBuffer::Attachment::Mask rhs )
{
  using UnderlyingType = typename std::underlying_type< FrameBuffer::Attachment::Mask >::type;
  return static_cast<bool>( static_cast<UnderlyingType>( lhs ) & static_cast<UnderlyingType>( rhs ) );
}

} // unnamed namespace

FrameBuffer FrameBuffer::New( uint32_t width, uint32_t height )
{
  return New( width, height, FrameBuffer::Attachment::COLOR );
}

FrameBuffer FrameBuffer::New( uint32_t width, uint32_t height, Attachment::Mask attachments )
{
  Internal::FrameBufferPtr frameBuffer = Internal::FrameBuffer::New( width, height, attachments );
  if( attachments & FrameBuffer::Attachment::COLOR )
  {
    Internal::TexturePtr texture = Internal::Texture::New( Dali::TextureType::TEXTURE_2D, Pixel::RGB888, width, height );
    frameBuffer->AttachColorTexture( texture, 0u, 0u );
  }
  return FrameBuffer( frameBuffer.Get() );
}

FrameBuffer FrameBuffer::New( uint32_t width, uint32_t height, uint32_t attachments )
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: FrameBuffer::New(uint32_t,uint32_t,uint32_t) is deprecated and will be removed from next release. use New(uint32_t, uint32_t,Attachment::Mask) instead.\n" );
  // have to static cast, which according to standard since C++11 is undefined behaviour, hence this variant is deprecated
  Internal::FrameBufferPtr frameBuffer = Internal::FrameBuffer::New( width, height, static_cast<Attachment::Mask>( attachments ) );
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
