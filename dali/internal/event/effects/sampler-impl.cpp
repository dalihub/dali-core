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
#include <dali/internal/event/effects/sampler-impl.h> // Dali::Internal::Sampler

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/sampler.h> // Dali::Internal::Sampler
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/effects/scene-graph-sampler.h> // Dali::Internal::SceneGraph::Sampler
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name                    |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "minification-filter",   STRING,   true, false,  true, Dali::Sampler::Property::MINIFICATION_FILTER )
DALI_PROPERTY( "magnification-filter",  STRING,   true, false,  true, Dali::Sampler::Property::MAGNIGICATION_FILTER )
DALI_PROPERTY( "u-wrap",                STRING,   true, false,  true, Dali::Sampler::Property::U_WRAP )
DALI_PROPERTY( "v-wrap",                STRING,   true, false,  true, Dali::Sampler::Property::V_WRAP )
DALI_PROPERTY( "affects-transparency",  BOOLEAN,  true, false,  true, Dali::Sampler::Property::AFFECTS_TRANSPARENCY )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> SAMPLER_IMPL = { DEFAULT_PROPERTY_DETAILS };

} // unnamed namespace

SamplerPtr Sampler::New( const std::string& textureUnitUniformName )
{
  SamplerPtr sampler( new Sampler() );
  sampler->Initialize( textureUnitUniformName );
  return sampler;
}

void Sampler::SetUniformName( const std::string& name )
{
// TODO: MESH_REWORK - change method to SetUnitName (or SetTextureUnitUniformName)
  SetUnitNameMessage( GetEventThreadServices(), *mSceneObject, name);
}

void Sampler::SetImage( Image& image )
{
  // TODO: MESH_REWORK - should probably review this comment

  unsigned int resourceId = image.GetResourceId();

  // sceneObject is being used in a separate thread; queue a message to set
  SetTextureMessage( GetEventThreadServices(), *mSceneObject, resourceId );
}

void Sampler::SetFilterMode( Dali::Sampler::FilterMode minFilter, Dali::Sampler::FilterMode magFilter )
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

void Sampler::SetWrapMode( Dali::Sampler::WrapMode uWrap, Dali::Sampler::WrapMode vWrap )
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

void Sampler::SetAffectsTransparency( bool affectsTransparency )
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

const SceneGraph::Sampler* Sampler::GetSamplerSceneObject() const
{
  return mSceneObject;
}

unsigned int Sampler::GetDefaultPropertyCount() const
{
  return SAMPLER_IMPL.GetDefaultPropertyCount();
}

void Sampler::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  SAMPLER_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Sampler::GetDefaultPropertyName(Property::Index index) const
{
  return SAMPLER_IMPL.GetDefaultPropertyName( index );
}

Property::Index Sampler::GetDefaultPropertyIndex( const std::string& name ) const
{
  return SAMPLER_IMPL.GetDefaultPropertyIndex( name );
}

bool Sampler::IsDefaultPropertyWritable( Property::Index index ) const
{
  return SAMPLER_IMPL.IsDefaultPropertyWritable( index );
}

bool Sampler::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return SAMPLER_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Sampler::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return SAMPLER_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Sampler::GetDefaultPropertyType( Property::Index index ) const
{
  return SAMPLER_IMPL.GetDefaultPropertyType( index );
}

void Sampler::SetDefaultProperty( Property::Index index,
                                  const Property::Value& propertyValue )
{
  switch( index )
  {
    case Dali::Sampler::Property::MINIFICATION_FILTER:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
    case Dali::Sampler::Property::MAGNIGICATION_FILTER:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
    case Dali::Sampler::Property::U_WRAP:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
    case Dali::Sampler::Property::V_WRAP:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
    case Dali::Sampler::Property::AFFECTS_TRANSPARENCY:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
  }
}

void Sampler::SetSceneGraphProperty( Property::Index index,
                                     const PropertyMetadata& entry,
                                     const Property::Value& value )
{
  SAMPLER_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
  OnPropertySet(index, value);
}

Property::Value Sampler::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  switch( index )
  {
    case Dali::Sampler::Property::MINIFICATION_FILTER:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
    case Dali::Sampler::Property::MAGNIGICATION_FILTER:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
    case Dali::Sampler::Property::U_WRAP:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
    case Dali::Sampler::Property::V_WRAP:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
    case Dali::Sampler::Property::AFFECTS_TRANSPARENCY:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
  }
  return value;
}

const SceneGraph::PropertyOwner* Sampler::GetPropertyOwner() const
{
  return mSceneObject;
}

const SceneGraph::PropertyOwner* Sampler::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* Sampler::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable( index ) && "Property is not animatable" );

  const SceneGraph::PropertyBase* property = NULL;

  if( OnStage() )
  {
    property = SAMPLER_IMPL.GetRegisteredSceneGraphProperty( this,
                                                             &Sampler::FindAnimatableProperty,
                                                             &Sampler::FindCustomProperty,
                                                             index );

    if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
    {
      // No animatable default props
      DALI_ASSERT_ALWAYS( 0 && "Property is not animatable" );
    }
  }

  return property;
}

const PropertyInputImpl* Sampler::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = NULL;

  if( OnStage() )
  {
    const SceneGraph::PropertyBase* baseProperty =
      SAMPLER_IMPL.GetRegisteredSceneGraphProperty( this,
                                                    &Sampler::FindAnimatableProperty,
                                                    &Sampler::FindCustomProperty,
                                                    index );
    property = static_cast<const PropertyInputImpl*>( baseProperty );

    if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
    {
      switch( index )
      {
        case Dali::Sampler::Property::MINIFICATION_FILTER:
        {
          DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
          break;
        }
        case Dali::Sampler::Property::MAGNIGICATION_FILTER:
        {
          DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
          break;
        }
        case Dali::Sampler::Property::U_WRAP:
        {
          DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
          break;
        }
        case Dali::Sampler::Property::V_WRAP:
        {
          DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
          break;
        }
        case Dali::Sampler::Property::AFFECTS_TRANSPARENCY:
        {
          DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
          break;
        }
      }
    }
  }

  return property;
}

int Sampler::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
}

bool Sampler::OnStage() const
{
  return mOnStage;
}

void Sampler::Connect()
{
  mOnStage = true;

  mImageConnector.OnStageConnect();
}

void Sampler::Disconnect()
{
  mOnStage = false;

  mImageConnector.OnStageDisconnect();
}

Sampler::Sampler()
: mSceneObject( NULL ),
  mOnStage( false )
{
}

void Sampler::Initialize( const std::string& textureUnitUniformName )
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  DALI_ASSERT_ALWAYS( EventThreadServices::IsCoreRunning() && "Core is not running" );

  mSceneObject = new SceneGraph::Sampler( textureUnitUniformName );
  AddMessage( updateManager, updateManager.GetSamplerOwner(), *mSceneObject );
}

Sampler::~Sampler()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveMessage( updateManager, updateManager.GetSamplerOwner(), *mSceneObject );
  }
}

} // namespace Internal
} // namespace Dali
