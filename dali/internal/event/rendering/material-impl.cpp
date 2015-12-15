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
#include <dali/internal/event/rendering/material-impl.h> // Dali::Internal::Material

//EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/rendering/material.h> // Dali::Internal::Material
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-material.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name                              |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "faceCullingMode",                 INTEGER,   true, false,  false, Dali::Material::Property::FACE_CULLING_MODE )
DALI_PROPERTY( "blendingMode",                    INTEGER,   true, false,  false, Dali::Material::Property::BLENDING_MODE )
DALI_PROPERTY( "blendEquationRgb",                INTEGER,   true, false,  false, Dali::Material::Property::BLEND_EQUATION_RGB )
DALI_PROPERTY( "blendEquationAlpha",              INTEGER,   true, false,  false, Dali::Material::Property::BLEND_EQUATION_ALPHA )
DALI_PROPERTY( "sourceBlendFactorRgb",            INTEGER,   true, false,  false, Dali::Material::Property::BLENDING_SRC_FACTOR_RGB )
DALI_PROPERTY( "destinationBlendFactorRgb",       INTEGER,   true, false,  false, Dali::Material::Property::BLENDING_DEST_FACTOR_RGB )
DALI_PROPERTY( "sourceBlendFactorAlpha",          INTEGER,   true, false,  false, Dali::Material::Property::BLENDING_SRC_FACTOR_ALPHA )
DALI_PROPERTY( "destinationBlendFactorAlpha",     INTEGER,   true, false,  false, Dali::Material::Property::BLENDING_DEST_FACTOR_ALPHA )
DALI_PROPERTY( "blendColor",                      VECTOR4,   true, false,  false, Dali::Material::Property::BLEND_COLOR )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> MATERIAL_IMPL = { DEFAULT_PROPERTY_DETAILS };

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType( typeid( Dali::Material ), typeid( Dali::Handle ), Create );

} // unnamed namespace

MaterialPtr Material::New()
{
  MaterialPtr material( new Material() );
  material->Initialize();
  return material;
}

void Material::SetShader( Shader& shader )
{
  DALI_ASSERT_DEBUG( mSceneObject )
  mShader = &shader;

  SceneGraph::Shader& sceneGraphShader = *shader.GetShaderSceneObject();
  SceneGraph::SetShaderMessage( GetEventThreadServices(), *mSceneObject, sceneGraphShader );
}

Shader* Material::GetShader() const
{
  return mShader.Get();
}

size_t Material::AddTexture( ImagePtr image, const std::string& uniformName, SamplerPtr sampler )
{
  size_t index = mTextures.size();
  mTextures.push_back( Texture( uniformName, image, sampler ) );

  Render::Sampler* renderSampler(0);
  if( sampler )
  {
    renderSampler = sampler->GetSamplerRenderObject();
  }

  if( mOnStage )
  {
    image->Connect();
  }

  SceneGraph::AddTextureMessage( GetEventThreadServices(), *mSceneObject, uniformName, image->GetResourceId(), renderSampler );
  return index;
}

void Material::RemoveTexture( size_t index )
{
  if( index < GetNumberOfTextures() )
  {
    mTextures.erase( mTextures.begin() + index );
    SceneGraph::RemoveTextureMessage( GetEventThreadServices(), *mSceneObject, index );
  }
}

void Material::SetTextureImage( size_t index, Image* image )
{
  if( index < GetNumberOfTextures() )
  {
    if( mTextures[index].mImage && mOnStage )
    {
      mTextures[index].mImage->Disconnect();

      if( image )
      {
        image->Connect();
      }
    }

    mTextures[index].mImage.Reset(image);
    SceneGraph::SetTextureImageMessage( GetEventThreadServices(), *mSceneObject, index, mTextures[index].mImage.Get()->GetResourceId() );
  }
}

void Material::SetTextureSampler( size_t index, Sampler* sampler )
{
  if( index < GetNumberOfTextures() )
  {
    mTextures[index].mSampler.Reset(sampler);

    Render::Sampler* renderSampler(0);
    if( sampler )
    {
      renderSampler = sampler->GetSamplerRenderObject();
    }
    SceneGraph::SetTextureSamplerMessage( GetEventThreadServices(), *mSceneObject, index,  renderSampler );
  }
}

Sampler* Material::GetTextureSampler( size_t index ) const
{
  if( index < GetNumberOfTextures() )
  {
    return mTextures[index].mSampler.Get();
  }

  return NULL;
}

void Material::SetTextureUniformName( size_t index, const std::string& uniformName )
{
  if( index < GetNumberOfTextures() )
  {
    mTextures[index].mUniformName = uniformName;
    SceneGraph::SetTextureUniformNameMessage( GetEventThreadServices(), *mSceneObject, index,  uniformName );
  }
}

int Material::GetTextureIndex( const std::string& uniformName ) const
{
  size_t textureCount(GetNumberOfTextures());
  for( size_t i(0); i<textureCount; ++i )
  {
    if( uniformName.compare( mTextures[i].mUniformName ) == 0 )
    {
      return i;
    }
  }

  return -1;
}

Image* Material::GetTexture( const std::string& uniformName ) const
{
  int textureId = GetTextureIndex( uniformName );
  if( textureId != -1 )
  {
    return GetTexture( textureId );
  }

  return NULL;
}

Image* Material::GetTexture( size_t index ) const
{
  if( index < GetNumberOfTextures() )
  {
    return mTextures[ index ].mImage.Get();
  }

  return NULL;
}

size_t Material::GetNumberOfTextures() const
{
  return mTextures.size();
}

void Material::SetFaceCullingMode( Dali::Material::FaceCullingMode cullingMode )
{
  if( mFaceCullingMode != cullingMode )
  {
    mFaceCullingMode = cullingMode;

    SetFaceCullingModeMessage( GetEventThreadServices(), *mSceneObject, mFaceCullingMode );
  }
}

Dali::Material::FaceCullingMode Material::GetFaceCullingMode()
{
  return mFaceCullingMode;
}

void Material::SetBlendMode( BlendingMode::Type mode )
{
  if( mBlendingMode != mode )
  {
    mBlendingMode = mode;

    SetBlendingModeMessage( GetEventThreadServices(), *mSceneObject, mBlendingMode );
  }
}

BlendingMode::Type Material::GetBlendMode() const
{
  return mBlendingMode;
}

void Material::SetBlendFunc( BlendingFactor::Type srcFactorRgba, BlendingFactor::Type destFactorRgba )
{
  mBlendingOptions.SetBlendFunc( srcFactorRgba, destFactorRgba, srcFactorRgba, destFactorRgba );
  SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
}

void Material::SetBlendFunc( BlendingFactor::Type srcFactorRgb,
                             BlendingFactor::Type destFactorRgb,
                             BlendingFactor::Type srcFactorAlpha,
                             BlendingFactor::Type destFactorAlpha )
{
  mBlendingOptions.SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
  SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
}

void Material::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,
                             BlendingFactor::Type& destFactorRgb,
                             BlendingFactor::Type& srcFactorAlpha,
                             BlendingFactor::Type& destFactorAlpha ) const
{
  srcFactorRgb    = mBlendingOptions.GetBlendSrcFactorRgb();
  destFactorRgb   = mBlendingOptions.GetBlendDestFactorRgb();
  srcFactorAlpha  = mBlendingOptions.GetBlendSrcFactorAlpha();
  destFactorAlpha = mBlendingOptions.GetBlendDestFactorAlpha();
}

void Material::SetBlendEquation( BlendingEquation::Type equationRgba )
{
  mBlendingOptions.SetBlendEquation( equationRgba, equationRgba );
  SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
}

void Material::SetBlendEquation( BlendingEquation::Type equationRgb,
                                 BlendingEquation::Type equationAlpha )
{
  mBlendingOptions.SetBlendEquation( equationRgb, equationAlpha );
  SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
}

void Material::GetBlendEquation( BlendingEquation::Type& equationRgb,
                                 BlendingEquation::Type& equationAlpha ) const
{
  // These are not animatable, the cached values are up-to-date.
  equationRgb   = mBlendingOptions.GetBlendEquationRgb();
  equationAlpha = mBlendingOptions.GetBlendEquationAlpha();
}

void Material::SetBlendColor( const Vector4& color )
{
  if( !mBlendColor )
  {
    mBlendColor = new Vector4();
  }
  if( *mBlendColor != color )
  {
    *mBlendColor = color;
    SetBlendColorMessage( GetEventThreadServices(), *mSceneObject, *mBlendColor );
  }
}

Vector4 Material::GetBlendColor() const
{
  if( mBlendColor )
  {
    return *mBlendColor;
  }
  return Color::TRANSPARENT; // GL default
}

const SceneGraph::Material* Material::GetMaterialSceneObject() const
{
  return mSceneObject;
}

unsigned int Material::GetDefaultPropertyCount() const
{
  return MATERIAL_IMPL.GetDefaultPropertyCount();
}

void Material::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  MATERIAL_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Material::GetDefaultPropertyName(Property::Index index) const
{
  return MATERIAL_IMPL.GetDefaultPropertyName( index );
}

Property::Index Material::GetDefaultPropertyIndex( const std::string& name ) const
{
  return MATERIAL_IMPL.GetDefaultPropertyIndex( name );
}

bool Material::IsDefaultPropertyWritable( Property::Index index ) const
{
  return MATERIAL_IMPL.IsDefaultPropertyWritable( index );
}

bool Material::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return MATERIAL_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Material::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return MATERIAL_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Material::GetDefaultPropertyType( Property::Index index ) const
{
  return MATERIAL_IMPL.GetDefaultPropertyType( index );
}

void Material::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
  switch( index )
  {
    case Dali::Material::Property::FACE_CULLING_MODE:
    {
      int faceCullingMode;
      if( propertyValue.Get( faceCullingMode ) )
      {
        SetFaceCullingMode( Dali::Material::FaceCullingMode( faceCullingMode ) );
      }
      break;
    }
    case Dali::Material::Property::BLENDING_MODE:
    {
      int blendingMode;
      if( propertyValue.Get( blendingMode ) )
      {
        SetBlendMode( BlendingMode::Type( blendingMode ) );
      }
      break;
    }
    case Dali::Material::Property::BLEND_EQUATION_RGB:
    {
      int blendingEquation;
      if( propertyValue.Get( blendingEquation ) )
      {
        BlendingEquation::Type alphaEquation = mBlendingOptions.GetBlendEquationAlpha();
        mBlendingOptions.SetBlendEquation( static_cast<BlendingEquation::Type>( blendingEquation ), alphaEquation );
        SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
      }
      break;
    }
    case Dali::Material::Property::BLEND_EQUATION_ALPHA:
    {
      int blendingEquation;
      if( propertyValue.Get( blendingEquation ) )
      {
        BlendingEquation::Type rgbEquation = mBlendingOptions.GetBlendEquationRgb();
        mBlendingOptions.SetBlendEquation( rgbEquation, static_cast<BlendingEquation::Type>( blendingEquation ) );
        SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
      }
      break;
    }
    case Dali::Material::Property::BLENDING_SRC_FACTOR_RGB:
    {
      int blendingFactor;
      if( propertyValue.Get( blendingFactor ) )
      {
        BlendingFactor::Type srcFactorRgb;
        BlendingFactor::Type destFactorRgb;
        BlendingFactor::Type srcFactorAlpha;
        BlendingFactor::Type destFactorAlpha;
        GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
        SetBlendFunc( static_cast<BlendingFactor::Type>( blendingFactor ),
                      destFactorRgb,
                      srcFactorAlpha,
                      destFactorAlpha );
      }
      break;
    }
    case Dali::Material::Property::BLENDING_DEST_FACTOR_RGB:
    {
      int blendingFactor;
      if( propertyValue.Get( blendingFactor ) )
      {
        BlendingFactor::Type srcFactorRgb;
        BlendingFactor::Type destFactorRgb;
        BlendingFactor::Type srcFactorAlpha;
        BlendingFactor::Type destFactorAlpha;
        GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
        SetBlendFunc( srcFactorRgb,
                      static_cast<BlendingFactor::Type>( blendingFactor ),
                      srcFactorAlpha,
                      destFactorAlpha );
      }
      break;
    }
    case Dali::Material::Property::BLENDING_SRC_FACTOR_ALPHA:
    {
      int blendingFactor;
      if( propertyValue.Get( blendingFactor ) )
      {
        BlendingFactor::Type srcFactorRgb;
        BlendingFactor::Type destFactorRgb;
        BlendingFactor::Type srcFactorAlpha;
        BlendingFactor::Type destFactorAlpha;
        GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
        SetBlendFunc( srcFactorRgb,
                      destFactorRgb,
                      static_cast<BlendingFactor::Type>( blendingFactor ),
                      destFactorAlpha );
      }
      break;
    }
    case Dali::Material::Property::BLENDING_DEST_FACTOR_ALPHA:
    {
      int blendingFactor;
      if( propertyValue.Get( blendingFactor ) )
      {
        BlendingFactor::Type srcFactorRgb;
        BlendingFactor::Type destFactorRgb;
        BlendingFactor::Type srcFactorAlpha;
        BlendingFactor::Type destFactorAlpha;
        GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
        SetBlendFunc( srcFactorRgb,
                      destFactorRgb,
                      srcFactorAlpha,
                      static_cast<BlendingFactor::Type>( blendingFactor ) );
      }
      break;
    }
    case Dali::Material::Property::BLEND_COLOR:
    {
      Vector4 blendColor;
      if( propertyValue.Get( blendColor ) )
      {
        SetBlendColor( blendColor );
      }
      break;
    }
  }
}

void Material::SetSceneGraphProperty( Property::Index index,
                                      const PropertyMetadata& entry,
                                      const Property::Value& value )
{
  MATERIAL_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
  OnPropertySet(index, value);
}

Property::Value Material::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  switch( index )
  {
    case Dali::Material::Property::FACE_CULLING_MODE:
    {
      value = mFaceCullingMode;
      break;
    }
    case Dali::Material::Property::BLENDING_MODE:
    {
      value = mBlendingMode;
      break;
    }
    case Dali::Material::Property::BLEND_EQUATION_RGB:
    {
      value = static_cast<int>( mBlendingOptions.GetBlendEquationRgb() );
      break;
    }
    case Dali::Material::Property::BLEND_EQUATION_ALPHA:
    {
      value = static_cast<int>( mBlendingOptions.GetBlendEquationAlpha() );
      break;
    }
    case Dali::Material::Property::BLENDING_SRC_FACTOR_RGB:
    {
      BlendingFactor::Type srcFactorRgb;
      BlendingFactor::Type destFactorRgb;
      BlendingFactor::Type srcFactorAlpha;
      BlendingFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int>( srcFactorRgb );
      break;
    }
    case Dali::Material::Property::BLENDING_DEST_FACTOR_RGB:
    {
      BlendingFactor::Type srcFactorRgb;
      BlendingFactor::Type destFactorRgb;
      BlendingFactor::Type srcFactorAlpha;
      BlendingFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int>( destFactorRgb );
      break;
    }
    case Dali::Material::Property::BLENDING_SRC_FACTOR_ALPHA:
    {
      BlendingFactor::Type srcFactorRgb;
      BlendingFactor::Type destFactorRgb;
      BlendingFactor::Type srcFactorAlpha;
      BlendingFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int>( srcFactorAlpha );
      break;
    }
    case Dali::Material::Property::BLENDING_DEST_FACTOR_ALPHA:
    {
      BlendingFactor::Type srcFactorRgb;
      BlendingFactor::Type destFactorRgb;
      BlendingFactor::Type srcFactorAlpha;
      BlendingFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int>( destFactorAlpha );
      break;
    }
    case Dali::Material::Property::BLEND_COLOR:
    {
      value = mBlendColor;
      break;
    }
  }

  return value;
}

const SceneGraph::PropertyOwner* Material::GetPropertyOwner() const
{
  return mSceneObject;
}

const SceneGraph::PropertyOwner* Material::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* Material::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  PropertyMetadata* property = index >= PROPERTY_CUSTOM_START_INDEX ? static_cast<PropertyMetadata*>(FindCustomProperty( index )) : static_cast<PropertyMetadata*>(FindAnimatableProperty( index ));
  DALI_ASSERT_ALWAYS( property && "Property index is invalid" );
  return property->GetSceneGraphProperty();
}

const PropertyInputImpl* Material::GetSceneObjectInputProperty( Property::Index index ) const
{
  return GetSceneObjectAnimatableProperty( index );
}

int Material::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
}

bool Material::OnStage() const
{
  return mOnStage;
}

void Material::Connect()
{
  mOnStage = true;

  for( size_t i(0); i<mTextures.size(); ++i )
  {
    if( mTextures[i].mImage )
    {
      mTextures[i].mImage->Connect();
      if( mTextures[i].mImage->GetResourceId() != 0 )
      {
        SceneGraph::SetTextureImageMessage( GetEventThreadServices(), *mSceneObject, i, mTextures[i].mImage->GetResourceId() );
      }
    }
  }
}

void Material::Disconnect()
{
  for( size_t i(0); i<mTextures.size(); ++i )
  {
    if( mTextures[i].mImage )
    {
      mTextures[i].mImage->Disconnect();
    }
  }

  mOnStage = false;
}

Material::Material()
: mSceneObject( NULL ),
  mShader( NULL ),
  mTextures(),
  mFaceCullingMode( Dali::Material::NONE ),
  mBlendingMode( Dali::BlendingMode::AUTO ),
  mBlendingOptions(), // initialises to defaults
  mBlendColor( NULL ),
  mOnStage( false )
{
}

void Material::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  mSceneObject = SceneGraph::Material::New();
  AddMessage( updateManager, updateManager.GetMaterialOwner(), *mSceneObject );

  eventThreadServices.RegisterObject( this );
}

Material::~Material()
{
  delete mBlendColor;
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveMessage( updateManager, updateManager.GetMaterialOwner(), *mSceneObject );

    eventThreadServices.UnregisterObject( this );
  }
}

} // namespace Internal
} // namespace Dali
