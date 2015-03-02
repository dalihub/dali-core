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
#include <dali/internal/event/actor-attachments/image-attachment-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/update/node-attachments/scene-graph-image-attachment.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/effects/shader-effect-impl.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>

namespace Dali
{

namespace Internal
{

ImageAttachmentPtr ImageAttachment::New( Stage& stage, const SceneGraph::Node& parentNode )
{
  ImageAttachmentPtr attachment( new ImageAttachment( stage ) );

  // Transfer object ownership of scene-object to message
  SceneGraph::ImageAttachment* sceneObject = CreateSceneObject();
  AttachToNodeMessage( stage.GetUpdateManager(), parentNode, sceneObject );

  // Keep raw pointer for message passing
  attachment->mSceneObject = sceneObject;

  return attachment;
}

ImageAttachment::ImageAttachment( Stage& stage )
: RenderableAttachment(stage),
  mSceneObject(NULL),
  mPixelArea(0,0,0,0),
  mStyle(Dali::ImageActor::STYLE_QUAD),
  mBorder(0.45,0.45,0.1,0.1),
  mIsPixelAreaSet(false),
  mBorderInPixels(false),


  mBlendingOptions(),
  mSamplerBitfield( ImageSampler::PackBitfield( FilterMode::DEFAULT, FilterMode::DEFAULT ) ),
  mSortModifier( 0.0f ),
  mCullFaceMode( CullNone ),
  mBlendingMode( BlendingMode::AUTO ),
  mShaderEffect()
{
  mImageConnectable.Set( NULL, false );
}

ImageAttachment::~ImageAttachment()
{
}

void ImageAttachment::SetImage( ImagePtr& image )
{
  bool onStage = OnStage();
  // keep a reference to Image object
  mImageConnectable.Set( image, onStage );

  // Wait until the scene-graph attachment is connected, before providing resource ID
  if ( OnStage() )
  {
    unsigned int resourceId = (image) ? image->GetResourceId() : 0u;

    // sceneObject is being used in a separate thread; queue a message to set
    SetTextureIdMessage( mStage->GetUpdateInterface(), *mSceneObject, resourceId );
  }
}

ImagePtr ImageAttachment::GetImage()
{
  return mImageConnectable.Get();
}

void ImageAttachment::SetPixelArea(const PixelArea& pixelArea)
{
  // check to see if pixel area is actually different, using rect::operator==
  if( pixelArea != mPixelArea )
  {
    // Cache for public getters
    mPixelArea = pixelArea;
    mIsPixelAreaSet = true;

    // sceneObject is being used in a separate thread; queue a message to set
    SetPixelAreaMessage( mStage->GetUpdateInterface(), *mSceneObject, mPixelArea );
  }
}

void ImageAttachment::ClearPixelArea()
{
  // Cache for public getters
  mIsPixelAreaSet = false;

  // sceneObject is being used in a separate thread; queue a message to set
  ClearPixelAreaMessage( mStage->GetUpdateInterface(), *mSceneObject );
}

void ImageAttachment::SetStyle(Style style)
{
  // Cache for public getters
  mStyle = style;

  // sceneObject is being used in a separate thread; queue a message to set
  SetStyleMessage( mStage->GetUpdateInterface(), *mSceneObject, style );
}

void ImageAttachment::SetNinePatchBorder(const Vector4& border, bool inPixels)
{
  // Cache for public getters
  mBorder = border;
  mBorderInPixels = inPixels;

  // sceneObject is being used in a separate thread; queue a message to set
  SetNinePatchBorderMessage( mStage->GetUpdateInterface(), *mSceneObject, border, inPixels );
}

SceneGraph::ImageAttachment* ImageAttachment::CreateSceneObject()
{
  return SceneGraph::ImageAttachment::New( 0u );
}

const SceneGraph::RenderableAttachment& ImageAttachment::GetSceneObject() const
{
  DALI_ASSERT_DEBUG( mSceneObject != NULL );
  return *mSceneObject;
}


void ImageAttachment::SetSortModifier(float modifier)
{
  // Cache for actor-side getters
  mSortModifier = modifier;

  // attachment is being used in a separate thread; queue a message to set the value & base value
  SetSortModifierMessage( mStage->GetUpdateInterface(), GetSceneObject(), modifier );
}

float ImageAttachment::GetSortModifier() const
{
  // mSortModifier is not animatable; this is the most up-to-date value.
  return mSortModifier;
}

void ImageAttachment::SetCullFace( CullFaceMode mode )
{
  // Cache for actor-side getters
  mCullFaceMode = mode;

  // attachment is being used in a separate thread; queue a message to set the value
  SetCullFaceMessage( mStage->GetUpdateInterface(), GetSceneObject(), mode );
}

CullFaceMode ImageAttachment::GetCullFace() const
{
  // mCullFaceMode is not animatable; this is the most up-to-date value.
  return mCullFaceMode;
}

void ImageAttachment::SetBlendMode( BlendingMode::Type mode )
{
  mBlendingMode = mode;

  // attachment is being used in a separate thread; queue a message to set the value
  SetBlendingModeMessage( mStage->GetUpdateInterface(), GetSceneObject(), mode );
}

BlendingMode::Type ImageAttachment::GetBlendMode() const
{
  return mBlendingMode;
}

void ImageAttachment::SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                                         BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha )
{
  // Cache for actor-side getters
  mBlendingOptions.SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );

  // attachment is being used in a separate thread; queue a message to set the value
  SetBlendingOptionsMessage( mStage->GetUpdateInterface(), GetSceneObject(), mBlendingOptions.GetBitmask() );
}

void ImageAttachment::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                                         BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const
{
  // These are not animatable, the cached values are up-to-date.
  srcFactorRgb    = mBlendingOptions.GetBlendSrcFactorRgb();
  destFactorRgb   = mBlendingOptions.GetBlendDestFactorRgb();
  srcFactorAlpha  = mBlendingOptions.GetBlendSrcFactorAlpha();
  destFactorAlpha = mBlendingOptions.GetBlendDestFactorAlpha();
}

void ImageAttachment::SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha )
{
  mBlendingOptions.SetBlendEquation( equationRgb, equationAlpha );

  // attachment is being used in a separate thread; queue a message to set the value
  SetBlendingOptionsMessage( mStage->GetUpdateInterface(), GetSceneObject(), mBlendingOptions.GetBitmask() );
}

void ImageAttachment::GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const
{
  // These are not animatable, the cached values are up-to-date.
  equationRgb   = mBlendingOptions.GetBlendEquationRgb();
  equationAlpha = mBlendingOptions.GetBlendEquationAlpha();
}

void ImageAttachment::SetBlendColor( const Vector4& color )
{
  if( mBlendingOptions.SetBlendColor( color ) )
  {
    // attachment is being used in a separate thread; queue a message to set the value
    SetBlendColorMessage( mStage->GetUpdateInterface(), GetSceneObject(), color );
  }
}

const Vector4& ImageAttachment::GetBlendColor() const
{
  const Vector4* optionalColor = mBlendingOptions.GetBlendColor();
  if( optionalColor )
  {
    return *optionalColor;
  }

  return Vector4::ZERO;
}

void ImageAttachment::SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter )
{
  mSamplerBitfield = ImageSampler::PackBitfield( minFilter, magFilter );

  SetSamplerMessage( mStage->GetUpdateInterface(), GetSceneObject(), mSamplerBitfield );
}

void ImageAttachment::GetFilterMode( FilterMode::Type& minFilter, FilterMode::Type& magFilter ) const
{
  minFilter = ImageSampler::GetMinifyFilterMode( mSamplerBitfield );
  magFilter = ImageSampler::GetMagnifyFilterMode( mSamplerBitfield );
}


void ImageAttachment::SetShaderEffect(ShaderEffect& effect)
{
  if ( OnStage() )
  {
    if ( mShaderEffect )
    {
      mShaderEffect->Disconnect();
    }

    mShaderEffect.Reset( &effect );

    const SceneGraph::Shader& shader = dynamic_cast<const SceneGraph::Shader&>( *mShaderEffect->GetSceneObject() );

    ApplyShaderMessage( mStage->GetUpdateInterface(), GetSceneObject(), shader );

    mShaderEffect->Connect();
  }
  else
  {
    mShaderEffect = ShaderEffectPtr(&effect);
  }
  // Effects can only be applied when the Node is connected to scene-graph
}

ShaderEffectPtr ImageAttachment::GetShaderEffect() const
{
  return mShaderEffect;
}

void ImageAttachment::RemoveShaderEffect()
{
  if ( OnStage() )
  {
    RemoveShaderMessage( mStage->GetUpdateInterface(), GetSceneObject() );

    // Notify shader effect
    if (mShaderEffect)
    {
      mShaderEffect->Disconnect();
    }
  }

  mShaderEffect.Reset();
}


void ImageAttachment::OnStageConnection2()
{
  if ( mShaderEffect )
  {
    const SceneGraph::Shader& shader = dynamic_cast<const SceneGraph::Shader&>( *mShaderEffect->GetSceneObject() );

    ApplyShaderMessage( mStage->GetUpdateInterface(), GetSceneObject(), shader );

    // Notify shader effect
    mShaderEffect->Connect();
  }

  mImageConnectable.OnStageConnect();

  // Provide resource ID when scene-graph attachment is connected
  ImagePtr image = mImageConnectable.Get();
  unsigned int resourceId = (image) ? image->GetResourceId() : 0u;
  if ( 0u != resourceId )
  {
    SetTextureIdMessage( mStage->GetUpdateInterface(), *mSceneObject, resourceId );
  }
}

void ImageAttachment::OnStageDisconnection2()
{
  // Notify shader effect
  if ( mShaderEffect )
  {
    mShaderEffect->Disconnect();
  }

  // Remove resource ID when scene-graph attachment is disconnected
  SetTextureIdMessage( mStage->GetUpdateInterface(), *mSceneObject, 0u );

  mImageConnectable.OnStageDisconnect();
}



} // namespace Internal

} // namespace Dali
