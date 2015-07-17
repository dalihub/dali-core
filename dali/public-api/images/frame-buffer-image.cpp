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
#include <dali/public-api/images/frame-buffer-image.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/images/frame-buffer-image-impl.h>

namespace Dali
{

FrameBufferImage::FrameBufferImage()
{
}

FrameBufferImage::FrameBufferImage(Internal::FrameBufferImage* internal)
: Image(internal)
{
}

FrameBufferImage::~FrameBufferImage()
{
}

FrameBufferImage::FrameBufferImage(const FrameBufferImage& handle)
: Image(handle)
{
}

FrameBufferImage& FrameBufferImage::operator=(const FrameBufferImage& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

FrameBufferImage FrameBufferImage::New(unsigned int width, unsigned int height, Pixel::Format pixelformat, RenderBuffer::Format bufferformat)
{
  Dali::Vector2 stageSize = Stage::GetCurrent().GetSize();
  Internal::FrameBufferImagePtr internal = Internal::FrameBufferImage::New(
    (0 == width) ? stageSize.width : width,
    (0 == height) ? stageSize.height : height,
    pixelformat,
    Dali::Image::NEVER,
    bufferformat);

  return FrameBufferImage(internal.Get());
}

FrameBufferImage FrameBufferImage::New(unsigned int width, unsigned int height, Pixel::Format pixelformat, ReleasePolicy releasePolicy, RenderBuffer::Format bufferformat)
{
  Dali::Vector2 stageSize = Stage::GetCurrent().GetSize();
  Internal::FrameBufferImagePtr internal = Internal::FrameBufferImage::New(
    (0 == width) ? stageSize.width : width,
    (0 == height) ? stageSize.height : height,
    pixelformat,
    releasePolicy,
    bufferformat);

  return FrameBufferImage(internal.Get());
}

FrameBufferImage FrameBufferImage::New( NativeImageInterface& image, ReleasePolicy releasePolicy )
{
  Internal::FrameBufferImagePtr internal = Internal::FrameBufferImage::New( image, releasePolicy );
  return FrameBufferImage(internal.Get());
}

FrameBufferImage FrameBufferImage::New( NativeImageInterface& image )
{
  Internal::FrameBufferImagePtr internal = Internal::FrameBufferImage::New( image );
  return FrameBufferImage(internal.Get());
}

FrameBufferImage FrameBufferImage::DownCast( BaseHandle handle )
{
  return FrameBufferImage( dynamic_cast<Dali::Internal::FrameBufferImage*>(handle.GetObjectPtr()) );
}

} // namespace Dali
