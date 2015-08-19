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
#include <dali/public-api/actors/image-actor.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/image-actor-impl.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/effects/shader-effect-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

const BlendingMode::Type ImageActor::DEFAULT_BLENDING_MODE = BlendingMode::AUTO;

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
: Actor(copy)
{
}

ImageActor& ImageActor::operator=(const ImageActor& rhs)
{
  BaseHandle::operator=(rhs);
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

void ImageActor::SetPixelArea(const PixelArea& pixelArea)
{
  GetImplementation(*this).SetPixelArea(pixelArea);
}

ImageActor::PixelArea ImageActor::GetPixelArea() const
{
  return GetImplementation(*this).GetPixelArea();
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

void ImageActor::SetSortModifier(float modifier)
{
  GetImplementation(*this).SetSortModifier(modifier);
}

float ImageActor::GetSortModifier() const
{
  return GetImplementation(*this).GetSortModifier();
}

void ImageActor::SetBlendMode( BlendingMode::Type mode )
{
  GetImplementation(*this).SetBlendMode( mode );
}

BlendingMode::Type ImageActor::GetBlendMode() const
{
  return GetImplementation(*this).GetBlendMode();
}

void ImageActor::SetBlendFunc( BlendingFactor::Type srcFactorRgba, BlendingFactor::Type destFactorRgba )
{
  GetImplementation(*this).SetBlendFunc( srcFactorRgba, destFactorRgba );
}

void ImageActor::SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                                    BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha )
{
  GetImplementation(*this).SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void ImageActor::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                                    BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const
{
  GetImplementation(*this).GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void ImageActor::SetBlendEquation( BlendingEquation::Type equationRgba )
{
  GetImplementation(*this).SetBlendEquation( equationRgba );
}

void ImageActor::SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha )
{
  GetImplementation(*this).SetBlendEquation( equationRgb, equationAlpha );
}

void ImageActor::GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const
{
  GetImplementation(*this).GetBlendEquation( equationRgb, equationAlpha );
}

void ImageActor::SetBlendColor( const Vector4& color )
{
  GetImplementation(*this).SetBlendColor( color );
}

const Vector4& ImageActor::GetBlendColor() const
{
  return GetImplementation(*this).GetBlendColor();
}

void ImageActor::SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter )
{
  GetImplementation(*this).SetFilterMode( minFilter, magFilter );
}

void ImageActor::GetFilterMode( FilterMode::Type& minFilter, FilterMode::Type& magFilter ) const
{
  GetImplementation(*this).GetFilterMode( minFilter, magFilter );
}

void ImageActor::SetShaderEffect(ShaderEffect effect)
{
  GetImplementation(*this).SetShaderEffect(GetImplementation(effect));
}

ShaderEffect ImageActor::GetShaderEffect() const
{
  Internal::ShaderEffectPtr internal = GetImplementation(*this).GetShaderEffect();

  return ShaderEffect(internal.Get());
}

void ImageActor::RemoveShaderEffect()
{
  GetImplementation(*this).RemoveShaderEffect();
}


ImageActor::ImageActor(Internal::ImageActor* internal)
: Actor(internal)
{
}

void SetShaderEffectRecursively( Actor actor, ShaderEffect effect )
{
  // only do something if the actor and effect are valid
  if( actor && effect )
  {
    // first remove from this actor
    ImageActor imageActor = ImageActor::DownCast( actor );
    if( imageActor )
    {
      imageActor.SetShaderEffect( effect );
    }
    // then all children recursively
    const unsigned int count = actor.GetChildCount();
    for( unsigned int index = 0; index < count; ++index )
    {
      Actor child( actor.GetChildAt( index ) );
      SetShaderEffectRecursively( child, effect );
    }
  }
}

void RemoveShaderEffectRecursively( Actor actor )
{
  // only do something if the actor is valid
  if( actor )
  {
    // first remove from this actor
    ImageActor imageActor = ImageActor::DownCast( actor );
    if( imageActor )
    {
      imageActor.RemoveShaderEffect();
    }
    // then all children recursively
    const unsigned int count = actor.GetChildCount();
    for( unsigned int index = 0; index < count; ++index )
    {
      Actor child( actor.GetChildAt( index ) );
      RemoveShaderEffectRecursively( child );
    }
  }
}

} // namespace Dali
