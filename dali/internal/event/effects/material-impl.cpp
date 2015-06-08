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
#include <dali/internal/event/effects/material-impl.h> // Dali::Internal::Material

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/material.h> // Dali::Internal::Material
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/effects/scene-graph-material.h>
#include <dali/internal/update/effects/scene-graph-sampler.h>
#include <dali/internal/update/manager/update-manager.h>

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
DALI_PROPERTY( "color",                           VECTOR4,  true, true,   true, Dali::Material::Property::COLOR )
DALI_PROPERTY( "face-culling-mode",               STRING,   true, false,  false, Dali::Material::Property::FACE_CULLING_MODE )
DALI_PROPERTY( "blending-mode",                   STRING,   true, false,  false, Dali::Material::Property::BLENDING_MODE )
DALI_PROPERTY( "blend-equation-rgb",                  STRING,   true, false,  false, Dali::Material::Property::BLEND_EQUATION_RGB )
DALI_PROPERTY( "blend-equation-alpha",                  STRING,   true, false,  false, Dali::Material::Property::BLEND_EQUATION_ALPHA )
DALI_PROPERTY( "source-blend-factor-rgb",         STRING,   true, false,  false, Dali::Material::Property::BLENDING_SRC_FACTOR_RGB )
DALI_PROPERTY( "destination-blend-factor-rgb",    STRING,   true, false,  false, Dali::Material::Property::BLENDING_DEST_FACTOR_RGB )
DALI_PROPERTY( "source-blend-factor-alpha",       STRING,   true, false,  false, Dali::Material::Property::BLENDING_SRC_FACTOR_ALPHA )
DALI_PROPERTY( "destination-blend-factor-alpha",  STRING,   true, false,  false, Dali::Material::Property::BLENDING_DEST_FACTOR_ALPHA )
DALI_PROPERTY( "blend-color",                     VECTOR4,  true, true,   true, Dali::Material::Property::BLEND_COLOR )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> MATERIAL_IMPL = { DEFAULT_PROPERTY_DETAILS };

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
  mShaderConnector.Set( shader, OnStage() );

  const SceneGraph::Shader& sceneGraphShader = dynamic_cast<const SceneGraph::Shader&>( *shader.GetSceneObject() );
  SceneGraph::SetShaderMessage( GetEventThreadServices(), *mSceneObject, sceneGraphShader );
}

Shader* Material::GetShader() const
{
  return mShaderConnector.Get().Get();
}

void Material::AddSampler( Sampler& sampler )
{
  SamplerConnector connector;
  connector.Set( sampler, OnStage() );
  mSamplerConnectors.push_back( connector );

  const SceneGraph::Sampler& sceneGraphSampler = dynamic_cast<const SceneGraph::Sampler&>( *sampler.GetSceneObject() );
  SceneGraph::AddSamplerMessage( GetEventThreadServices(), *mSceneObject, sceneGraphSampler );
}

std::size_t Material::GetNumberOfSamplers() const
{
  return mSamplerConnectors.size();
}

void Material::RemoveSampler( std::size_t index )
{
  mSamplerConnectors.erase( mSamplerConnectors.begin() + index );
}

Sampler* Material::GetSamplerAt( unsigned int index ) const
{
  return mSamplerConnectors[index].Get().Get();
}

void Material::SetFaceCullingMode( Dali::Material::FaceCullingMode cullingMode )
{
  if( NULL != mSceneObject )
  {
    SceneGraph::DoubleBufferedPropertyMessage<int>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mFaceCullingMode, &SceneGraph::DoubleBufferedProperty<int>::Set, static_cast<int>(cullingMode) );
  }
}

void Material::SetBlendMode( BlendingMode::Type mode )
{
  mBlendingMode = mode;

  if( NULL != mSceneObject )
  {
    SceneGraph::DoubleBufferedPropertyMessage<int>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mBlendingMode, &SceneGraph::DoubleBufferedProperty<int>::Set, static_cast<int>(mode) );
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
  if( mSceneObject )
  {
    SceneGraph::AnimatablePropertyMessage<Vector4>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mBlendColor, &SceneGraph::AnimatableProperty<Vector4>::Bake, color );
  }
}

const Vector4& Material::GetBlendColor() const
{
  return mSceneObject->mBlendColor[ GetEventThreadServices().GetEventBufferIndex() ];
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
    case Dali::Material::Property::COLOR:
    {
      SceneGraph::AnimatablePropertyMessage<Vector4>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mColor, &SceneGraph::AnimatableProperty<Vector4>::Bake, propertyValue.Get<Vector4>() );
      break;
    }
    case Dali::Material::Property::FACE_CULLING_MODE:
    {
      SceneGraph::DoubleBufferedPropertyMessage<int>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mFaceCullingMode, &SceneGraph::DoubleBufferedProperty<int>::Set, propertyValue.Get<int>() );
      break;
    }
    case Dali::Material::Property::BLENDING_MODE:
    {
      if( mSceneObject )
      {
        SceneGraph::DoubleBufferedPropertyMessage<int>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mBlendingMode, &SceneGraph::DoubleBufferedProperty<int>::Set, propertyValue.Get<int>() );
      }
      break;
    }
    case Dali::Material::Property::BLEND_EQUATION_RGB:
    {
      BlendingEquation::Type alphaEquation = mBlendingOptions.GetBlendEquationAlpha();
      mBlendingOptions.SetBlendEquation( static_cast<BlendingEquation::Type>(propertyValue.Get<int>()), alphaEquation );
      break;
    }
    case Dali::Material::Property::BLEND_EQUATION_ALPHA:
    {
      BlendingEquation::Type rgbEquation = mBlendingOptions.GetBlendEquationRgb();
      mBlendingOptions.SetBlendEquation( rgbEquation, static_cast<BlendingEquation::Type>(propertyValue.Get<int>()) );
      break;
    }
    case Dali::Material::Property::BLENDING_SRC_FACTOR_RGB:
    {
      BlendingFactor::Type srcFactorRgb;
      BlendingFactor::Type destFactorRgb;
      BlendingFactor::Type srcFactorAlpha;
      BlendingFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      SetBlendFunc( static_cast<BlendingFactor::Type>(propertyValue.Get<int>()),
                    destFactorRgb,
                    srcFactorAlpha,
                    destFactorAlpha );
      break;
    }
    case Dali::Material::Property::BLENDING_DEST_FACTOR_RGB:
    {
      BlendingFactor::Type srcFactorRgb;
      BlendingFactor::Type destFactorRgb;
      BlendingFactor::Type srcFactorAlpha;
      BlendingFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      SetBlendFunc( srcFactorRgb,
                    static_cast<BlendingFactor::Type>(propertyValue.Get<int>()),
                    srcFactorAlpha,
                    destFactorAlpha );
      break;
    }
    case Dali::Material::Property::BLENDING_SRC_FACTOR_ALPHA:
    {
      BlendingFactor::Type srcFactorRgb;
      BlendingFactor::Type destFactorRgb;
      BlendingFactor::Type srcFactorAlpha;
      BlendingFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      SetBlendFunc( srcFactorRgb,
                    destFactorRgb,
                    static_cast<BlendingFactor::Type>(propertyValue.Get<int>()),
                    destFactorAlpha );
      break;
    }
    case Dali::Material::Property::BLENDING_DEST_FACTOR_ALPHA:
    {
      BlendingFactor::Type srcFactorRgb;
      BlendingFactor::Type destFactorRgb;
      BlendingFactor::Type srcFactorAlpha;
      BlendingFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      SetBlendFunc( srcFactorRgb,
                    destFactorRgb,
                    srcFactorAlpha,
                    static_cast<BlendingFactor::Type>(propertyValue.Get<int>()) );
      break;
    }
    case Dali::Material::Property::BLEND_COLOR:
    {
      SceneGraph::AnimatablePropertyMessage<Vector4>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mBlendColor, &SceneGraph::AnimatableProperty<Vector4>::Bake, propertyValue.Get<Vector4>() );
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
  BufferIndex bufferIndex = GetEventThreadServices().GetEventBufferIndex();
  Property::Value value;

  switch( index )
  {
    case Dali::Material::Property::COLOR:
    {
      if( mSceneObject )
      {
        value = mSceneObject->mColor[bufferIndex];
      }
      break;
    }
    case Dali::Material::Property::FACE_CULLING_MODE:
    {
      if( mSceneObject )
      {
        value = mSceneObject->mFaceCullingMode[bufferIndex];
      }
      break;
    }
    case Dali::Material::Property::BLENDING_MODE:
    {
      if( mSceneObject )
      {
        value = mSceneObject->mBlendingMode[bufferIndex];
      }
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
      if( mSceneObject )
      {
        value = mSceneObject->mBlendColor[bufferIndex];
      }
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
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable( index ) && "Property is not animatable" );

  const SceneGraph::PropertyBase* property = NULL;

  if( OnStage() )
  {
    property = MATERIAL_IMPL.GetRegisteredSceneGraphProperty( this,
                                                              &Material::FindAnimatableProperty,
                                                              &Material::FindCustomProperty,
                                                              index );

    if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
    {
      switch(index)
      {
        case Dali::Material::Property::COLOR:
        {
          property = &mSceneObject->mColor;
          break;
        }
        case Dali::Material::Property::BLEND_COLOR:
        {
          property = &mSceneObject->mBlendColor;
          break;
        }
        default:
        {
          DALI_ASSERT_ALWAYS( 0 && "Property is not animatable");
          break;
        }
      }
    }
  }

  return property;
}

const PropertyInputImpl* Material::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = NULL;

  if( OnStage() )
  {
    const SceneGraph::PropertyBase* baseProperty =
      MATERIAL_IMPL.GetRegisteredSceneGraphProperty( this,
                                                     &Material::FindAnimatableProperty,
                                                     &Material::FindCustomProperty,
                                                     index );
    property = static_cast<const PropertyInputImpl*>( baseProperty );

    if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
    {
      switch(index)
      {
        case Dali::Material::Property::COLOR:
        {
          property = &mSceneObject->mColor;
          break;
        }
        case Dali::Material::Property::FACE_CULLING_MODE:
        {
          property = &mSceneObject->mFaceCullingMode;
          break;
        }
        case Dali::Material::Property::BLEND_COLOR:
        {
          property = &mSceneObject->mBlendColor;
          break;
        }
        default:
        {
          DALI_ASSERT_ALWAYS( 0 && "Property cannot be a constraint input");
          break;
        }
      }
    }
  }

  return property;
}

int Material::GetPropertyComponentIndex( Property::Index index ) const
{
  // @todo MESH_REWORK - Change this if component properties are added for color/blend-color
  return Property::INVALID_COMPONENT_INDEX;
}

bool Material::OnStage() const
{
  return mOnStage;
}

void Material::Connect()
{
  mOnStage = true;

  SamplerConnectorContainer::const_iterator end = mSamplerConnectors.end();
  for( SamplerConnectorContainer::iterator it = mSamplerConnectors.begin();
       it < end;
       ++it )
  {
    it->OnStageConnect();
  }
  mShaderConnector.OnStageConnect();
}

void Material::Disconnect()
{
  mOnStage = false;

  SamplerConnectorContainer::const_iterator end = mSamplerConnectors.end();
  for( SamplerConnectorContainer::iterator it = mSamplerConnectors.begin();
       it < end;
       ++it )
  {
    it->OnStageDisconnect();
  }
  mShaderConnector.OnStageDisconnect();
}

Material::Material()
: mSceneObject( NULL ),
  mOnStage( false )
{
}

void Material::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  DALI_ASSERT_ALWAYS( EventThreadServices::IsCoreRunning() && "Core is not running" );

  mSceneObject = new SceneGraph::Material();
  AddMessage( updateManager, updateManager.GetMaterialOwner(), *mSceneObject );
}

Material::~Material()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveMessage( updateManager, updateManager.GetMaterialOwner(), *mSceneObject );
  }
}

} // namespace Internal
} // namespace Dali
