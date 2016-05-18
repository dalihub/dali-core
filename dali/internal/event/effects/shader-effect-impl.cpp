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

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/internal/event/actors/image-actor-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/images/image-impl.h>
#include "dali-shaders.h"

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

std::string GetStringProperty(const std::string& field, const Property::Value& property)
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

Dali::Shader::ShaderHints ConvertHints( Dali::ShaderEffect::GeometryHints hints)
{
  int convertedHints = Dali::Shader::HINT_NONE;

  if( hints & Dali::ShaderEffect::HINT_BLENDING )
  {
    convertedHints |= Dali::Shader::HINT_OUTPUT_IS_TRANSPARENT;
  }
  if( !(hints & Dali::ShaderEffect::HINT_DOESNT_MODIFY_GEOMETRY) )
  {
    convertedHints |= Dali::Shader::HINT_MODIFIES_GEOMETRY;
  }

  return Dali::Shader::ShaderHints( convertedHints );
}

} // unnamed namespace

ShaderEffectPtr ShaderEffect::New( Dali::ShaderEffect::GeometryHints hints )
{
  ShaderEffectPtr shaderEffect( new ShaderEffect( hints ) );
  shaderEffect->RegisterObject();
  return shaderEffect;
}

ShaderEffect::ShaderEffect( Dali::ShaderEffect::GeometryHints hints )
: mGridDensity( Dali::ShaderEffect::DEFAULT_GRID_DENSITY ),
  mGeometryHints( hints )
{
}

ShaderEffect::~ShaderEffect()
{
  // Guard to allow handle destruction after Core has been destroyed
  UnregisterObject();
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
  mShader->RegisterProperty( name, value );
}

void ShaderEffect::SetPrograms( const string& vertexSource, const string& fragmentSource )
{
  SetPrograms( "", "", vertexSource, fragmentSource );
}

void ShaderEffect::SetPrograms( const std::string& vertexPrefix, const std::string& fragmentPrefix,
                                const std::string& vertexSource, const std::string& fragmentSource )
{
  mShader = Shader::New( WrapVertexShader( vertexPrefix, vertexSource ),
                         WrapFragmentShader( fragmentPrefix, fragmentSource ),
                         ConvertHints( mGeometryHints ) );
}

Vector2 ShaderEffect::GetGridSize( const Vector2& size )
{
  Vector2 gridSize( 1.f, 1.f );

  if( mGridDensity > 0 )
  {
    if( ( mGeometryHints & Dali::ShaderEffect::HINT_GRID_X ) )
    {
      gridSize.x = ceil( size.width / mGridDensity );
    }
    if( ( mGeometryHints & Dali::ShaderEffect::HINT_GRID_Y ) )
    {
      gridSize.y = ceil( size.height / mGridDensity );
    }
  }

  return gridSize;
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

unsigned int ShaderEffect::GetPropertyCount() const
{
  return GetDefaultPropertyCount() + mShader->GetPropertyCount();
}

std::string ShaderEffect::GetPropertyName( Property::Index index ) const
{
  if ( index < DEFAULT_PROPERTY_COUNT )
  {
    return GetDefaultPropertyName( index );
  }
  else
  {
    return mShader->GetPropertyName( index );
  }
}

Property::Index ShaderEffect::GetPropertyIndex( const std::string& name ) const
{
  Property::Index index = GetDefaultPropertyIndex( name );
  if( index == Property::INVALID_INDEX )
  {
    return mShader->GetPropertyIndex( name );
  }
  else
  {
    return index;
  }
}

bool ShaderEffect::IsPropertyWritable( Property::Index index ) const
{
  if ( index < DEFAULT_PROPERTY_COUNT )
  {
    return IsDefaultPropertyWritable( index );
  }
  else
  {
    return mShader->IsPropertyWritable( index );
  }
}

bool ShaderEffect::IsPropertyAnimatable( Property::Index index ) const
{
  if ( index < DEFAULT_PROPERTY_COUNT )
  {
    return IsDefaultPropertyAnimatable( index );
  }
  else
  {
    return mShader->IsPropertyAnimatable( index );
  }
}

bool ShaderEffect::IsPropertyAConstraintInput( Property::Index index ) const
{
  if ( index < DEFAULT_PROPERTY_COUNT )
  {
    return IsDefaultPropertyAConstraintInput( index );
  }
  else
  {
    return mShader->IsPropertyAConstraintInput( index );
  }
}

Property::Type ShaderEffect::GetPropertyType( Property::Index index ) const
{
  if ( index < DEFAULT_PROPERTY_COUNT )
  {
    return GetDefaultPropertyType( index );
  }
  return mShader->GetPropertyType( index );
}

void ShaderEffect::SetProperty( Property::Index index, const Property::Value& propertyValue )
{
  if ( index < DEFAULT_PROPERTY_COUNT )
  {
    SetDefaultProperty( index, propertyValue );
  }
  else
  {
    mShader->SetProperty( index, propertyValue );
  }
}

Property::Value ShaderEffect::GetProperty( Property::Index index ) const
{
  if ( index < DEFAULT_PROPERTY_COUNT )
  {
    return GetDefaultProperty( index );
  }
  return mShader->GetProperty( index );
}

void ShaderEffect::GetPropertyIndices( Property::IndexContainer& indices ) const
{
  mShader->GetPropertyIndices( indices );
  GetDefaultPropertyIndices( indices );
}

Property::Index ShaderEffect::RegisterProperty( const std::string& name, const Property::Value& propertyValue )
{
  return mShader->RegisterProperty( name, propertyValue );
}

Property::Index ShaderEffect::RegisterProperty( const std::string& name, const Property::Value& propertyValue, Property::AccessMode accessMode )
{
  return mShader->RegisterProperty( name, propertyValue, accessMode );
}

Dali::PropertyNotification ShaderEffect::AddPropertyNotification( Property::Index index,
                                                                  int componentIndex,
                                                                  const Dali::PropertyCondition& condition )
{
  return mShader->AddPropertyNotification( index, componentIndex, condition );
}

void ShaderEffect::RemovePropertyNotification( Dali::PropertyNotification propertyNotification )
{
  mShader->RemovePropertyNotification( propertyNotification );
}

void ShaderEffect::RemovePropertyNotifications()
{
  mShader->RemovePropertyNotifications();
}

unsigned int ShaderEffect::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void ShaderEffect::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.Reserve( indices.Size() + DEFAULT_PROPERTY_COUNT );

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
      std::string vertexPrefix   = GetStringProperty("vertexPrefix",  propertyValue);
      std::string fragmentPrefix = GetStringProperty("fragmentPrefix",  propertyValue);
      std::string vertex         = GetStringProperty("vertex", propertyValue);
      std::string fragment       = GetStringProperty("fragment", propertyValue);

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
  return mShader->GetSceneObject();
}

const SceneGraph::PropertyBase* ShaderEffect::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  return mShader->GetSceneObjectAnimatableProperty( index );
}

const PropertyInputImpl* ShaderEffect::GetSceneObjectInputProperty( Property::Index index ) const
{
  return mShader->GetSceneObjectInputProperty( index );
}

int ShaderEffect::GetPropertyComponentIndex( Property::Index index ) const
{
  return mShader->GetPropertyComponentIndex( index );
}

} // namespace Internal

} // namespace Dali
