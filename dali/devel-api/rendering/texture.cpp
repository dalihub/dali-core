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
#include <dali/devel-api/rendering/texture.h>  // Dali::Texture

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/texture-impl.h> // Dali::Internal::Texture


namespace Dali
{

Texture Texture::New( TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height )
{
  Internal::NewTexturePtr texture = Internal::NewTexture::New(type, format, width, height );
  return Texture( texture.Get() );
}

Texture::Texture()
{
}

Texture::~Texture()
{
}

Texture::Texture( const Texture& handle )
: BaseHandle( handle )
{
}

Texture Texture::DownCast( BaseHandle handle )
{
  return Texture( dynamic_cast<Dali::Internal::NewTexture*>(handle.GetObjectPtr()));
}

Texture& Texture::operator=( const Texture& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

void Texture::Upload( Vector<unsigned char>& buffer, const TextureUploadParams& params )
{
  return GetImplementation(*this).Upload( buffer, params );
}

void Texture::Upload( PixelData pixelData, const TextureUploadParams& params )
{
  //TODO: Implement this method once PixelData uses handle/body pattern
  DALI_ASSERT_ALWAYS( false && "Not implemented yet");
}

void Texture::GenerateMipmaps()
{
  return GetImplementation(*this).GenerateMipmaps();
}

unsigned int Texture::GetWidth() const
{
  return GetImplementation(*this).GetWidth();
}

unsigned int Texture::GetHeight() const
{
  return GetImplementation(*this).GetHeight();
}

Texture::Texture( Internal::NewTexture* pointer )
: BaseHandle( pointer )
{
}

} //namespace Dali
