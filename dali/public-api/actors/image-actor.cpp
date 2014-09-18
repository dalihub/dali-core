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
#include <dali/public-api/actors/image-actor.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/image-actor-impl.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

ImageActor::ImageActor()
{
}

ImageActor ImageActor::New()
{
  Internal::ImageActorPtr internal = Internal::ImageActor::New();
  return ImageActor( internal.Get() );
}

ImageActor ImageActor::New(Image image)
{
  ImageActor actor = ImageActor::New();
  actor.SetImage( image );

  return actor;
}

ImageActor ImageActor::New(Image image, PixelArea pixelArea)
{
  ImageActor actor = ImageActor::New();
  actor.SetImage( image );
  actor.SetPixelArea( pixelArea );

  return actor;
}

ImageActor ImageActor::DownCast( BaseHandle handle )
{
  return ImageActor( dynamic_cast<Dali::Internal::ImageActor*>(handle.GetObjectPtr()) );
}

ImageActor::~ImageActor()
{
}

ImageActor::ImageActor(const ImageActor& copy)
: RenderableActor(copy)
{
}

ImageActor& ImageActor::operator=(const ImageActor& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

ImageActor& ImageActor::operator=(BaseHandle::NullType* rhs)
{
  DALI_ASSERT_ALWAYS( (rhs == NULL) && "Can only assign NULL pointer to handle");
  Reset();
  return *this;
}

void ImageActor::SetImage(Image image)
{
  Internal::ImagePtr imagePtr;
  if( image )
  {
    imagePtr = &GetImplementation(image);
  }
  GetImplementation(*this).SetImage( imagePtr );
}

Image ImageActor::GetImage()
{
  Internal::ImagePtr imagePtr( GetImplementation(*this).GetImage() );
  return Dali::Image( imagePtr.Get() );
}

void ImageActor::SetToNaturalSize()
{
  GetImplementation(*this).SetToNaturalSize();
}

void ImageActor::SetPixelArea(const PixelArea& pixelArea)
{
  GetImplementation(*this).SetPixelArea(pixelArea);
}

ImageActor::PixelArea ImageActor::GetPixelArea() const
{
  return GetImplementation(*this).GetPixelArea();
}

bool ImageActor::IsPixelAreaSet() const
{
  return GetImplementation(*this).IsPixelAreaSet();
}

void ImageActor::ClearPixelArea()
{
  GetImplementation(*this).ClearPixelArea();
}

void ImageActor::SetStyle(Style style)
{
  GetImplementation(*this).SetStyle(style);
}

ImageActor::Style ImageActor::GetStyle() const
{
  return GetImplementation(*this).GetStyle();
}

void ImageActor::SetNinePatchBorder(const Vector4& border)
{
  GetImplementation(*this).SetNinePatchBorder(border, true/*in pixels*/);
}

Vector4 ImageActor::GetNinePatchBorder() const
{
  return GetImplementation(*this).GetNinePatchBorder();
}

ImageActor::ImageActor(Internal::ImageActor* internal)
: RenderableActor(internal)
{
}

} // namespace Dali
