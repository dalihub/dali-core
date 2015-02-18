/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/nine-patch-image.h>

// INTERNAL INCLUDES
#include <dali/public-api/images/image-attributes.h>
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
  ImageAttributes defaultAttrs;

  Internal::NinePatchImagePtr internal = Internal::NinePatchImage::New( filename, defaultAttrs, Image::NEVER );
  return NinePatchImage(internal.Get());
}

NinePatchImage NinePatchImage::DownCast( BaseHandle handle )
{
  return NinePatchImage( dynamic_cast<Dali::Internal::NinePatchImage*>(handle.GetObjectPtr()) );
}

Vector4 NinePatchImage::GetStretchBorders()
{
  return GetImplementation(*this).GetStretchBorders();
}

Rect<int> NinePatchImage::GetChildRectangle()
{
  return GetImplementation(*this).GetChildRectangle();
}

BitmapImage NinePatchImage::CreateCroppedBitmapImage()
{
  Internal::BitmapImagePtr internal = GetImplementation(*this).CreateCroppedBitmapImage();
  return BitmapImage(internal.Get());
}


} // namespace Dali
