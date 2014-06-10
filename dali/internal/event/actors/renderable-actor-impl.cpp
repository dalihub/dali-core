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
#include <dali/internal/event/actors/renderable-actor-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/actor-attachments/renderable-attachment-impl.h>

namespace // unnamed namespace
{

using namespace Dali;

/*
 * This may look like a no-op but maintains TypeRegistry chain of classes.
 * ie if a child actor declares its base as RenderableActor, RenderableActor must exist
 * in TypeRegistry otherwise it doesnt know that child is related to Actor.
 */
BaseHandle Create()
{
  return BaseHandle();
}

TypeRegistration mType( typeid(Dali::RenderableActor), typeid(Dali::Actor), Create );

} // unnamed namespace

namespace Dali
{

namespace Internal
{

void RenderableActor::SetSortModifier(float modifier)
{
  GetRenderableAttachment().SetSortModifier(modifier);
}

float RenderableActor::GetSortModifier() const
{
  return GetRenderableAttachment().GetSortModifier();
}

void RenderableActor::SetCullFace(CullFaceMode mode)
{
  GetRenderableAttachment().SetCullFace( mode );
}

CullFaceMode RenderableActor::GetCullFace() const
{
  return GetRenderableAttachment().GetCullFace();
}

void RenderableActor::SetBlendMode( BlendingMode::Type mode )
{
  GetRenderableAttachment().SetBlendMode( mode );
}

BlendingMode::Type RenderableActor::GetBlendMode() const
{
  return GetRenderableAttachment().GetBlendMode();
}

void RenderableActor::SetBlendFunc( BlendingFactor::Type srcFactorRgba,   BlendingFactor::Type destFactorRgba )
{
  GetRenderableAttachment().SetBlendFunc( srcFactorRgba, destFactorRgba, srcFactorRgba, destFactorRgba );
}

void RenderableActor::SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                                    BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha )
{
  GetRenderableAttachment().SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void RenderableActor::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                                    BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const
{
  GetRenderableAttachment().GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void RenderableActor::SetBlendEquation( BlendingEquation::Type equationRgba )
{
  GetRenderableAttachment().SetBlendEquation( equationRgba, equationRgba );
}

void RenderableActor::SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha )
{
  GetRenderableAttachment().SetBlendEquation( equationRgb, equationAlpha );
}

void RenderableActor::GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const
{
  GetRenderableAttachment().GetBlendEquation( equationRgb, equationAlpha );
}

void RenderableActor::SetBlendColor( const Vector4& color )
{
  GetRenderableAttachment().SetBlendColor( color );
}

const Vector4& RenderableActor::GetBlendColor() const
{
  return GetRenderableAttachment().GetBlendColor();
}

RenderableActor::RenderableActor()
: Actor( Actor::RENDERABLE )
{
}

RenderableActor::~RenderableActor()
{
}

} // namespace Internal

} // namespace Dali
