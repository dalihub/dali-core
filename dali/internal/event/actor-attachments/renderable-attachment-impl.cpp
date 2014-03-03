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
#include <dali/internal/event/actor-attachments/renderable-attachment-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>

namespace Dali
{

namespace Internal
{

RenderableAttachment::RenderableAttachment( Stage& stage )
: ActorAttachment( stage ),
  mSortModifier( 0.0f ),
  mCullFaceMode( CullNone ),
  mBlendingMode( BlendingMode::AUTO )
{
}

RenderableAttachment::~RenderableAttachment()
{
}

void RenderableAttachment::SetSortModifier(float modifier)
{
  // Cache for actor-side getters
  mSortModifier = modifier;

  // attachment is being used in a separate thread; queue a message to set the value & base value
  SetSortModifierMessage( mStage->GetUpdateInterface(), GetSceneObject(), modifier );
}

float RenderableAttachment::GetSortModifier() const
{
  // mSortModifier is not animatable; this is the most up-to-date value.
  return mSortModifier;
}

void RenderableAttachment::SetCullFace( CullFaceMode mode )
{
  // Cache for actor-side getters
  mCullFaceMode = mode;

  // attachment is being used in a separate thread; queue a message to set the value
  SetCullFaceMessage( mStage->GetUpdateInterface(), GetSceneObject(), mode );
}

CullFaceMode RenderableAttachment::GetCullFace() const
{
  // mCullFaceMode is not animatable; this is the most up-to-date value.
  return mCullFaceMode;
}

void RenderableAttachment::SetBlendMode( BlendingMode::Type mode )
{
  mBlendingMode = mode;

  // attachment is being used in a separate thread; queue a message to set the value
  SetBlendingModeMessage( mStage->GetUpdateInterface(), GetSceneObject(), mode );
}

BlendingMode::Type RenderableAttachment::GetBlendMode() const
{
  return mBlendingMode;
}

void RenderableAttachment::SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                                         BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha )
{
  // Cache for actor-side getters
  mBlendingOptions.SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );

  // attachment is being used in a separate thread; queue a message to set the value
  SetBlendingOptionsMessage( mStage->GetUpdateInterface(), GetSceneObject(), mBlendingOptions.GetBitmask() );
}

void RenderableAttachment::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                                         BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const
{
  // These are not animatable, the cached values are up-to-date.
  srcFactorRgb    = mBlendingOptions.GetBlendSrcFactorRgb();
  destFactorRgb   = mBlendingOptions.GetBlendDestFactorRgb();
  srcFactorAlpha  = mBlendingOptions.GetBlendSrcFactorAlpha();
  destFactorAlpha = mBlendingOptions.GetBlendDestFactorAlpha();
}

void RenderableAttachment::SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha )
{
  mBlendingOptions.SetBlendEquation( equationRgb, equationAlpha );

  // attachment is being used in a separate thread; queue a message to set the value
  SetBlendingOptionsMessage( mStage->GetUpdateInterface(), GetSceneObject(), mBlendingOptions.GetBitmask() );
}

void RenderableAttachment::GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const
{
  // These are not animatable, the cached values are up-to-date.
  equationRgb   = mBlendingOptions.GetBlendEquationRgb();
  equationAlpha = mBlendingOptions.GetBlendEquationAlpha();
}

void RenderableAttachment::SetBlendColor( const Vector4& color )
{
  if( mBlendingOptions.SetBlendColor( color ) )
  {
    // attachment is being used in a separate thread; queue a message to set the value
    SetBlendColorMessage( mStage->GetUpdateInterface(), GetSceneObject(), color );
  }
}

const Vector4& RenderableAttachment::GetBlendColor() const
{
  const Vector4* optionalColor = mBlendingOptions.GetBlendColor();
  if( optionalColor )
  {
    return *optionalColor;
  }

  return Vector4::ZERO;
}

void RenderableAttachment::OnStageConnection()
{
  // For derived classes
  OnStageConnection2();
}

void RenderableAttachment::OnStageDisconnection()
{
  // For derived classes
  OnStageDisconnection2();
}

} // namespace Internal

} // namespace Dali
