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
#include <dali/public-api/actors/renderable-actor.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/renderable-actor-impl.h>
#include <dali/internal/event/effects/shader-effect-impl.h>

namespace Dali
{

const BlendingMode::Type RenderableActor::DEFAULT_BLENDING_MODE = BlendingMode::AUTO;

RenderableActor::RenderableActor()
{
}

RenderableActor RenderableActor::DownCast( BaseHandle handle )
{
  return RenderableActor( dynamic_cast<Dali::Internal::RenderableActor*>(handle.GetObjectPtr()) );
}

RenderableActor::~RenderableActor()
{
}

RenderableActor::RenderableActor(const RenderableActor& copy)
: Actor(copy)
{
}

RenderableActor& RenderableActor::operator=(const RenderableActor& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

RenderableActor& RenderableActor::operator=(BaseHandle::NullType* rhs)
{
  DALI_ASSERT_ALWAYS( (rhs == NULL) && "Can only assign NULL pointer to handle");
  Reset();
  return *this;
}

void RenderableActor::SetSortModifier(float modifier)
{
  GetImplementation(*this).SetSortModifier(modifier);
}

float RenderableActor::GetSortModifier() const
{
  return GetImplementation(*this).GetSortModifier();
}

void RenderableActor::SetCullFace(const CullFaceMode mode)
{
  GetImplementation(*this).SetCullFace(mode);
}

CullFaceMode RenderableActor::GetCullFace() const
{
  return GetImplementation(*this).GetCullFace();
}

void RenderableActor::SetBlendMode( BlendingMode::Type mode )
{
  GetImplementation(*this).SetBlendMode( mode );
}

BlendingMode::Type RenderableActor::GetBlendMode() const
{
  return GetImplementation(*this).GetBlendMode();
}

void RenderableActor::SetBlendFunc( BlendingFactor::Type srcFactorRgba, BlendingFactor::Type destFactorRgba )
{
  GetImplementation(*this).SetBlendFunc( srcFactorRgba, destFactorRgba );
}

void RenderableActor::SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                                    BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha )
{
  GetImplementation(*this).SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void RenderableActor::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                                    BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const
{
  GetImplementation(*this).GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void RenderableActor::SetBlendEquation( BlendingEquation::Type equationRgba )
{
  GetImplementation(*this).SetBlendEquation( equationRgba );
}

void RenderableActor::SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha )
{
  GetImplementation(*this).SetBlendEquation( equationRgb, equationAlpha );
}

void RenderableActor::GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const
{
  GetImplementation(*this).GetBlendEquation( equationRgb, equationAlpha );
}

void RenderableActor::SetBlendColor( const Vector4& color )
{
  GetImplementation(*this).SetBlendColor( color );
}

const Vector4& RenderableActor::GetBlendColor() const
{
  return GetImplementation(*this).GetBlendColor();
}

void RenderableActor::SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter )
{
  GetImplementation(*this).SetFilterMode( minFilter, magFilter );
}

void RenderableActor::GetFilterMode( FilterMode::Type& minFilter, FilterMode::Type& magFilter ) const
{
  GetImplementation(*this).GetFilterMode( minFilter, magFilter );
}

void RenderableActor::SetShaderEffect(ShaderEffect effect)
{
  GetImplementation(*this).SetShaderEffect(GetImplementation(effect));
}

ShaderEffect RenderableActor::GetShaderEffect() const
{
  Internal::ShaderEffectPtr internal = GetImplementation(*this).GetShaderEffect();

  return ShaderEffect(internal.Get());
}

void RenderableActor::RemoveShaderEffect()
{
  GetImplementation(*this).RemoveShaderEffect();
}

RenderableActor::RenderableActor(Internal::RenderableActor* internal)
: Actor(internal)
{
}

void SetShaderEffectRecursively( Actor actor, ShaderEffect effect )
{
  // only do something if the actor and effect are valid
  if( actor && effect )
  {
    // first remove from this actor
    RenderableActor renderable = RenderableActor::DownCast( actor );
    if( renderable )
    {
      renderable.SetShaderEffect( effect );
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
    RenderableActor renderable = RenderableActor::DownCast( actor );
    if( renderable )
    {
      renderable.RemoveShaderEffect();
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
