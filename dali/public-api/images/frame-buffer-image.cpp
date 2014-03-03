//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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

FrameBufferImage FrameBufferImage::New(unsigned int width, unsigned int height, Pixel::Format pixelformat)
{
  Dali::Vector2 stageSize = Stage::GetCurrent().GetSize();
  return FrameBufferImage(new Internal::FrameBufferImage((0 == width) ? stageSize.width : width,
                                                         (0 == height) ? stageSize.height : height,
                                                         pixelformat,
                                                         Dali::Image::Never));
}

FrameBufferImage FrameBufferImage::New(unsigned int width, unsigned int height, Pixel::Format pixelformat, ReleasePolicy releasePolicy)
{
  Dali::Vector2 stageSize = Stage::GetCurrent().GetSize();
  return FrameBufferImage(new Internal::FrameBufferImage((0 == width) ? stageSize.width : width,
                                                         (0 == height) ? stageSize.height : height,
                                                         pixelformat,
                                                         releasePolicy));
}

FrameBufferImage FrameBufferImage::New( NativeImage& image, ReleasePolicy releasePolicy )
{
  return FrameBufferImage(new Internal::FrameBufferImage( image, releasePolicy ));
}

FrameBufferImage FrameBufferImage::New( NativeImage& image )
{
  return FrameBufferImage(new Internal::FrameBufferImage( image ));
}


FrameBufferImage FrameBufferImage::DownCast( BaseHandle handle )
{
  return FrameBufferImage( dynamic_cast<Dali::Internal::FrameBufferImage*>(handle.GetObjectPtr()) );
}

} // namespace Dali
