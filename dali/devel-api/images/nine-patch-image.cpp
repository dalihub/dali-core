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
#include <dali/devel-api/images/nine-patch-image.h>

// INTERNAL INCLUDES
#include <dali/internal/common/image-attributes.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/images/nine-patch-image-impl.h>


namespace Dali
{

NinePatchImage::NinePatchImage()
{
}

NinePatchImage::NinePatchImage(Internal::NinePatchImage* internal)
: ResourceImage(internal)
{
}

NinePatchImage::~NinePatchImage()
{
}

NinePatchImage::NinePatchImage(const NinePatchImage& handle)
: ResourceImage(handle)
{
}

NinePatchImage& NinePatchImage::operator=(const NinePatchImage& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

NinePatchImage NinePatchImage::New( const std::string& filename )
{
  Internal::NinePatchImagePtr internal = Internal::NinePatchImage::New( filename );
  return NinePatchImage(internal.Get());
}

NinePatchImage NinePatchImage::DownCast( BaseHandle handle )
{
  return NinePatchImage( dynamic_cast<Dali::Internal::NinePatchImage*>(handle.GetObjectPtr()) );
}

Vector4 NinePatchImage::GetStretchBorders()
{
  Vector4 border;

  const NinePatchImage::StretchRanges& stretchPixelsX = GetStretchPixelsX();
  const NinePatchImage::StretchRanges& stretchPixelsY = GetStretchPixelsY();

  if( stretchPixelsX.Size() > 0 && stretchPixelsY.Size() > 0 )
  {
    //The NinePatchImage stretch pixels are in the cropped image space, inset by 1 to get it to uncropped image space
    border.x = stretchPixelsX[ 0 ].GetX() + 1;
    border.y = stretchPixelsY[ 0 ].GetX() + 1;
    border.z = GetWidth() - stretchPixelsX[ 0 ].GetY() - 1;
    border.w = GetHeight() - stretchPixelsY[ 0 ].GetY() - 1;
  }

  return border;
}

const NinePatchImage::StretchRanges& NinePatchImage::GetStretchPixelsX()
{
  return GetImplementation(*this).GetStretchPixelsX();
}

const NinePatchImage::StretchRanges& NinePatchImage::GetStretchPixelsY()
{
  return GetImplementation(*this).GetStretchPixelsY();
}

Rect<int> NinePatchImage::GetChildRectangle()
{
  return GetImplementation(*this).GetChildRectangle();
}

BufferImage NinePatchImage::CreateCroppedBufferImage()
{
  Internal::BufferImagePtr internal = GetImplementation(*this).CreateCroppedBufferImage();
  return BufferImage(internal.Get());
}

bool NinePatchImage::IsNinePatchUrl( const std::string& url )
{
  return Internal::NinePatchImage::IsNinePatchUrl( url );
}

} // namespace Dali
