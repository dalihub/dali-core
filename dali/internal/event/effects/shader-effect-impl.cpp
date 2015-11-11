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
#include <dali/internal/event/effects/shader-effect-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/internal/event/actors/image-actor-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/shaders/uniform-meta.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/manager/update-manager.h>
#include "dali-shaders.h"

using Dali::Internal::SceneGraph::UpdateManager;
using Dali::Internal::SceneGraph::UniformMeta;
using Dali::Internal::SceneGraph::Shader;
using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::PropertyBase;
using Dali::Internal::SceneGraph::RenderQueue;
using std::string;

namespace Dali
{

namespace Internal
{

namespace
{

// Properties

//              Name             Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "gridDensity",    FLOAT,   true,    false,   false,   Dali::ShaderEffect::Property::GRID_DENSITY   )
DALI_PROPERTY( "image",          MAP,     true,    false,   false,   Dali::ShaderEffect::Property::IMAGE          )
DALI_PROPERTY( "program",        MAP,     true,    false,   false,   Dali::ShaderEffect::Property::PROGRAM        )
DALI_PROPERTY( "geometryHints",  STRING,  true,    false,   false,   Dali::ShaderEffect::Property::GEOMETRY_HINTS )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

BaseHandle Create()
{
  Internal::ShaderEffectPtr internal = Internal::ShaderEffect::New();

  return Dali::ShaderEffect(internal.Get());
}

TypeRegistration mType( typeid(Dali::ShaderEffect), typeid(Dali::Handle), Create );

struct WrapperStrings
{
  const char* vertexShaderPrefix;
  const char* fragmentShaderPrefix;
  const char* vertexShaderPostfix;
  const char* fragmentShaderPostfix;
};

WrapperStrings customImageShaderWrappers =
{
  CustomImagePrefixVertex, CustomImagePrefixFragment,
  CustomImagePostfixVertex, CustomImagePostfixFragment
};

/**
 * Helper to wrap the program with our default pre and postfix if needed
 * @param[in] vertexPrefix from application
 * @param[in] vertexBody from application
 */
std::string WrapVertexShader( const std::string& vertexPrefix, const std::string& vertexBody )
{
  std::string vertexSource = vertexPrefix + customImageShaderWrappers.vertexShaderPrefix;

  // Append the custom vertex shader code if supplied, otherwise append the default
  if ( vertexBody.length() > 0 )
  {
    vertexSource.append( vertexBody );
  }
  else
  {
    vertexSource.append( customImageShaderWrappers.vertexShaderPostfix );
  }

  return vertexSource;
}

/**
 * Helper to wrap the program with our default pre and postfix if needed
 * @param[in] fragmentPrefix from application
 * @param[in] fragmentBody from application
 */
std::string WrapFragmentShader( const std::string& fragmentPrefix, const std::string& fragmentBody )
{
  std::string fragmentSource = fragmentPrefix + customImageShaderWrappers.fragmentShaderPrefix;

  // Append the custom fragment shader code if supplied, otherwise append the default
  if ( fragmentBody.length() > 0 )
  {
    fragmentSource.append( fragmentBody );
  }
  else
  {
    fragmentSource.append( customImageShaderWrappers.fragmentShaderPostfix );
  }

  return fragmentSource;
}

std::string GetShader(const std::string& field, const Property::Value& property)
{
  std::string retval;
  const Property::Map* map = property.GetMap();
  if( map )
  {
    const Property::Value* value = map->Find( field );
    if( value )
    {
      value->Get( retval );
    }
  }

  return retval;
}

} // unnamed namespace

ShaderEffectPtr ShaderEffect::New( Dali::ShaderEffect::GeometryHints hints )
{
  Stage* stage = Stage::GetCurrent();

  if( stage )
  {
    ShaderEffectPtr shaderEffect( new ShaderEffect( *stage, hints ) );
    shaderEffect->RegisterObject();
    return shaderEffect;
  }
  else
  {
    return NULL;
  }
}

ShaderEffect::ShaderEffect( EventThreadServices& eventThreadServices, Dali::ShaderEffect::GeometryHints hints )
: mEventThreadServices( eventThreadServices ),
  mGeometryHints( hints ),
  mGridDensity( Dali::ShaderEffect::DEFAULT_GRID_DENSITY )
{
  mSceneObject = new SceneGraph::Shader( hints );
  DALI_ASSERT_DEBUG( NULL != mSceneObject );

  // Transfer shader ownership to a scene message
  AddShaderMessage( eventThreadServices.GetUpdateManager(), *mSceneObject );
}

ShaderEffect::~ShaderEffect()
{
  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    // Remove scene-object using a message to the UpdateManager
    if( mSceneObject )
    {
      RemoveShaderMessage( mEventThreadServices.GetUpdateManager(), *mSceneObject );
    }
    UnregisterObject();
  }
}

void ShaderEffect::SetEffectImage( Dali::Image image )
{
  // if images are the same, do nothing
  if ( mEffectImage == image )
  {
    return;
  }

  if ( mEffectImage && mConnectedActors.size() > 0 )
  {
    // unset previous image
    GetImplementation( mEffectImage ).Disconnect();
  }

  // in case image is empty this will reset our image handle
  mEffectImage = image;

  if( image )
  {
    // tell image that we're using it
    if (mConnectedActors.size() > 0)
    {
      GetImplementation( mEffectImage ).Connect();
    }
  }

  //inform connected actors the image has been unset
  for(std::vector< ActorPtr >::iterator it = mConnectedActors.begin(); it != mConnectedActors.end(); ++it )
  {
    ImageActor* imageActor = dynamic_cast< ImageActor* >( it->Get() );
    if( imageActor )
    {
      imageActor->EffectImageUpdated();
    }
  }
}

void ShaderEffect::SetUniform( const std::string& name, Property::Value value, UniformCoordinateType uniformCoordinateType )
{
  // Register the property if it does not exist
  Property::Index index = RegisterProperty( name, value );

  Uniform uniform = { name, index, value };
  mUniforms.push_back( uniform );

  //inform any connected actors
  for(std::vector< ActorPtr >::iterator it = mConnectedActors.begin(); it != mConnectedActors.end(); ++it)
  {
    ImageActor* imageActor = dynamic_cast< ImageActor* >( (*it).Get() );
    if( imageActor )
    {
      imageActor->EffectUniformUpdated( uniform );
    }
  }
}

void ShaderEffect::SetPrograms( const string& vertexSource, const string& fragmentSource )
{
  SetPrograms( "", "", vertexSource, fragmentSource );
}

void ShaderEffect::SetPrograms( const std::string& vertexPrefix, const std::string& fragmentPrefix,
                                const std::string& vertexSource, const std::string& fragmentSource )
{
  mVertexSource = WrapVertexShader( vertexPrefix, vertexSource );
  mFragmentSource = WrapFragmentShader( fragmentPrefix, fragmentSource );
}

void ShaderEffect::Connect( ActorPtr actor )
{
  if( !actor )
  {
    return;
  }

  std::vector< ActorPtr >::const_iterator it = std::find( mConnectedActors.begin(), mConnectedActors.end(), actor );
  if( it == mConnectedActors.end() )
  {
    mConnectedActors.push_back( actor );
  }

  if( mEffectImage && mConnectedActors.size() == 1 )
  {
    GetImplementation( mEffectImage ).Connect();
  }
}
void ShaderEffect::Disconnect( ActorPtr actor )
{
  if( !actor )
  {
    return;
  }

  DALI_ASSERT_DEBUG(mConnectedActors.size() > 0);
  std::vector< ActorPtr >::iterator match( std::remove( mConnectedActors.begin(), mConnectedActors.end(), actor ) );
  mConnectedActors.erase( match, mConnectedActors.end() );

  if (mEffectImage && mConnectedActors.size() == 0)
  {
     GetImplementation(mEffectImage).Disconnect();
  }
}

unsigned int ShaderEffect::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void ShaderEffect::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.Reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.PushBack( i );
  }
}

const char* ShaderEffect::GetDefaultPropertyName(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }

  return NULL;
}

Property::Index ShaderEffect::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in default properties
  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_PROPERTY_DETAILS[ i ];
    if( 0 == strcmp( name.c_str(), property->name ) ) // dont want to convert rhs to string
    {
      index = i;
      break;
    }
  }

  return index;
}

bool ShaderEffect::IsDefaultPropertyWritable(Property::Index index) const
{
  return DEFAULT_PROPERTY_DETAILS[ index ].writable;
}

bool ShaderEffect::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return DEFAULT_PROPERTY_DETAILS[ index ].animatable;
}

bool ShaderEffect::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return DEFAULT_PROPERTY_DETAILS[ index ].constraintInput;
}

Property::Type ShaderEffect::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }

  // index out of range...return Property::NONE
  return Property::NONE;
}

void ShaderEffect::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  switch ( index )
  {
    case Dali::ShaderEffect::Property::GRID_DENSITY:
    {
      propertyValue.Get( mGridDensity );
      if( ( mGeometryHints & Dali::ShaderEffect::HINT_GRID_X ) ||
          ( mGeometryHints & Dali::ShaderEffect::HINT_GRID_Y ) )
      {
        //inform all the connected actors
        for(std::vector< ActorPtr >::iterator it = mConnectedActors.begin(); it != mConnectedActors.end(); ++it )
        {
          (*it)->RelayoutRequest();
        }
      }
      break;
    }

    case Dali::ShaderEffect::Property::IMAGE:
    {
      Dali::Image img(Scripting::NewImage( propertyValue ));
      if(img)
      {
        SetEffectImage( img );
      }
      else
      {
        DALI_LOG_WARNING("Cannot create image from property value for ShaderEffect image\n");
      }
      break;
    }

    case Dali::ShaderEffect::Property::PROGRAM:
    {
      std::string vertexPrefix   = GetShader("vertexPrefix",  propertyValue);
      std::string fragmentPrefix = GetShader("fragmentPrefix",  propertyValue);
      std::string vertex         = GetShader("vertex", propertyValue);
      std::string fragment       = GetShader("fragment", propertyValue);

      SetPrograms( vertexPrefix, fragmentPrefix, vertex, fragment );
      break;
    }

    case Dali::ShaderEffect::Property::GEOMETRY_HINTS:
    {
      mGeometryHints = Dali::ShaderEffect::HINT_NONE;
      std::string s = propertyValue.Get<std::string>();
      if(s == "HINT_NONE")
      {
        mGeometryHints = Dali::ShaderEffect::HINT_NONE;
      }
      else if(s == "HINT_GRID_X")
      {
        mGeometryHints = Dali::ShaderEffect::HINT_GRID_X;
      }
      else if(s == "HINT_GRID_Y")
      {
        mGeometryHints = Dali::ShaderEffect::HINT_GRID_Y;
      }
      else if(s == "HINT_GRID")
      {
        mGeometryHints = Dali::ShaderEffect::HINT_GRID;
      }
      else if(s == "HINT_DEPTH_BUFFER")
      {
        mGeometryHints = Dali::ShaderEffect::HINT_DEPTH_BUFFER;
      }
      else if(s == "HINT_BLENDING")
      {
        mGeometryHints = Dali::ShaderEffect::HINT_BLENDING;
      }
      else if(s == "HINT_DOESNT_MODIFY_GEOMETRY")
      {
        mGeometryHints = Dali::ShaderEffect::HINT_DOESNT_MODIFY_GEOMETRY;
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Geometry hint unknown" );
      }
      break;
    }

    default:
    {
      // nothing to do
      break;
    }
  }
}

Property::Value ShaderEffect::GetDefaultProperty(Property::Index /*index*/) const
{
  // none of our properties are readable so return empty
  return Property::Value();
}

const SceneGraph::PropertyOwner* ShaderEffect::GetSceneObject() const
{
  return mSceneObject;
}

const PropertyBase* ShaderEffect::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  PropertyMetadata* property = index >= PROPERTY_CUSTOM_START_INDEX ? static_cast<PropertyMetadata*>(FindCustomProperty( index )) : static_cast<PropertyMetadata*>(FindAnimatableProperty( index ));
  DALI_ASSERT_ALWAYS( property && "Property index is invalid" );
  return property->GetSceneGraphProperty();
}

const PropertyInputImpl* ShaderEffect::GetSceneObjectInputProperty( Property::Index index ) const
{
  return GetSceneObjectAnimatableProperty( index );
}

} // namespace Internal

} // namespace Dali
