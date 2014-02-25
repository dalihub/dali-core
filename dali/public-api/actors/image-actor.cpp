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
  // empty image
  Internal::ImageActorPtr internal = Internal::ImageActor::New(NULL);
  return ImageActor(internal.Get());
}

ImageActor ImageActor::New(Image image)
{
  Internal::ImageActorPtr internal;
  if (image)
  {
    internal = Internal::ImageActor::New(&GetImplementation(image));
  }
  else
  {
    // empty image
    internal = Internal::ImageActor::New(NULL);
  }

  return ImageActor(internal.Get());
}

ImageActor ImageActor::New(Image image, PixelArea pixelArea)
{
  Internal::ImageActorPtr internal;
  if (image)
  {
    internal = Internal::ImageActor::New(&GetImplementation(image), pixelArea);
  }
  else
  {
    // empty image
    internal = Internal::ImageActor::New(NULL, pixelArea);
  }

  return ImageActor(internal.Get());
}

ImageActor ImageActor::DownCast( BaseHandle handle )
{
  return ImageActor( dynamic_cast<Dali::Internal::ImageActor*>(handle.GetObjectPtr()) );
}

ImageActor::~ImageActor()
{
}

void ImageActor::SetImage(Image image)
{
  if (image)
  {
    GetImplementation(*this).SetImage(&GetImplementation(image));
  }
  else
  {
    GetImplementation(*this).SetImage(NULL);
  }
}

Image ImageActor::GetImage()
{
  return GetImplementation(*this).GetImage();
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

void ImageActor::SetFadeIn(bool enableFade)
{
  GetImplementation(*this).SetFadeIn(enableFade);
}

bool ImageActor::GetFadeIn() const
{
  return GetImplementation(*this).GetFadeIn();
}

void ImageActor::SetFadeInDuration(float durationSeconds)
{
  GetImplementation(*this).SetFadeInDuration(durationSeconds);
}

float ImageActor::GetFadeInDuration() const
{
  return GetImplementation(*this).GetFadeInDuration();
}

Vector2 ImageActor::GetCurrentImageSize() const
{
  return GetImplementation(*this).GetCurrentImageSize();
}

ImageActor::ImageActor(Internal::ImageActor* internal)
: RenderableActor(internal)
{
}

} // namespace Dali
