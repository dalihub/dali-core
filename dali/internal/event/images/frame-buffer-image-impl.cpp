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
#include <dali/internal/event/images/frame-buffer-image-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{

namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::FrameBufferImage ), typeid( Dali::Image ), NULL );

const int RenderBufferFormatToFrameBufferAttachments[]  = { Dali::FrameBuffer::Attachment::NONE,
                                                            Dali::FrameBuffer::Attachment::DEPTH,
                                                            Dali::FrameBuffer::Attachment::STENCIL,
                                                            Dali::FrameBuffer::Attachment::DEPTH_STENCIL
                                                          };

} // unnamed namespace

FrameBufferImagePtr FrameBufferImage::New( unsigned int width,
                                           unsigned int height,
                                           Pixel::Format pixelFormat,
                                           RenderBuffer::Format bufferformat )
{
  FrameBufferImagePtr image = new FrameBufferImage( width, height );
  image->Initialize();

  image->mTexture = Texture::New( Dali::TextureType::TEXTURE_2D, pixelFormat, width, height );
  image->mFrameBufferObject = FrameBuffer::New( width, height, RenderBufferFormatToFrameBufferAttachments[bufferformat] );
  image->mFrameBufferObject->AttachColorTexture( image->mTexture, 0u, 0u );

  return image;
}

FrameBufferImagePtr FrameBufferImage::New( NativeImageInterface& nativeImage )
{
  FrameBufferImagePtr image = new FrameBufferImage( nativeImage );
  image->Initialize();

  image->mTexture = Texture::New( nativeImage );
  image->mFrameBufferObject = FrameBuffer::New( image->mTexture->GetWidth(), image->mTexture->GetHeight(), Dali::FrameBuffer::Attachment::NONE );
  image->mFrameBufferObject->AttachColorTexture( image->mTexture, 0u, 0u );

  return image;
}

FrameBufferImage::FrameBufferImage( unsigned int width, unsigned int height)
: Image(),
  mNativeImage(0),
  mIsNativeFbo( false )
{
  mWidth  = width;
  mHeight = height;
}

FrameBufferImage::FrameBufferImage( NativeImageInterface& nativeImage )
: Image(),
  mNativeImage( &nativeImage ),
  mIsNativeFbo( true )
{
  mWidth = nativeImage.GetWidth();
  mHeight = nativeImage.GetHeight();
}


bool FrameBufferImage::IsNativeFbo() const
{
  return mIsNativeFbo;
}

FrameBufferImage::~FrameBufferImage()
{
}

} // namespace Internal

} // namespace Dali
