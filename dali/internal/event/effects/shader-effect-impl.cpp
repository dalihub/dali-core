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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/scripting/scripting.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/render/shaders/shader.h>
#include <dali/internal/render/shaders/uniform-meta.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
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

const Property::Index ShaderEffect::GRID_DENSITY        = 0;
const Property::Index ShaderEffect::IMAGE               = 1;
const Property::Index ShaderEffect::PROGRAM             = 2;
const Property::Index ShaderEffect::GEOMETRY_HINTS      = 3;

namespace Internal
{

namespace
{
const PropertyDetails DEFAULT_PROPERTY_DETAILS[] =
{
 // Name               Type            writable animatable constraint-input
 { "grid-density",   Property::FLOAT,   true,    false,   false  }, // GRID_DENSITY
 { "image",          Property::MAP,     true,    false,   false  }, // IMAGE
 { "program",        Property::MAP,     true,    false,   false  }, // PROGRAM
 { "geometry-hints", Property::INTEGER, true,    false,   false  }, // GEOMETRY_HINTS
};

const int DEFAULT_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_DETAILS ) / sizeof( DEFAULT_PROPERTY_DETAILS[0] );

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

WrapperStrings customShaderWrappers [] =
{
  {
    CustomImagePrefixVertex, CustomImagePrefixFragment,
    CustomImagePostfixVertex, CustomImagePostfixFragment
  },
  {
    CustomTextDistanceFieldPrefixVertex, CustomTextDistanceFieldPrefixFragment,
    CustomTextDistanceFieldPostfixVertex, CustomTextDistanceFieldPostfixFragment
  },
  {
    CustomUntexturedMeshPrefixVertex, CustomUntexturedMeshPrefixFragment,
    CustomUntexturedMeshPostfixVertex, CustomUntexturedMeshPostfixFragment
  },
  {
    CustomTexturedMeshPrefixVertex, CustomTexturedMeshPrefixFragment,
    CustomTexturedMeshPostfixVertex, CustomTexturedMeshPostfixFragment
  }
};

/**
 * Helper to wrap the program with our default pre and postfix if needed and then send it to update/render thread
 * @param[in] effect of the shader
 * @param[in] actualGeometryType of the shader
 * @param[in] expectedGeometryType of the shader
 * @param[in] vertexPrefix from application
 * @param[in] fragmentPrefix from application
 * @param[in] vertexBody from application
 * @param[in] fragmentBody from application
 * @param[in] modifiesGeometry based on flags and vertex shader
 */
void WrapAndSetProgram( Internal::ShaderEffect& effect,
                        GeometryType actualGeometryType, GeometryType expectedGeometryType,
                        const std::string& vertexPrefix, const std::string& fragmentPrefix,
                        const std::string& vertexBody, const std::string& fragmentBody,
                        bool modifiesGeometry )
{
  // if geometry type matches and there is some real data in the strings
  if( ( actualGeometryType & expectedGeometryType )&&
      ( ( vertexPrefix.length() > 0   )||
        ( fragmentPrefix.length() > 0 )||
        ( vertexBody.length() > 0     )||
        ( fragmentBody.length() > 0   ) ) )
  {
    std::string vertexSource = vertexPrefix;
    std::string fragmentSource = fragmentPrefix;

    // create complete shader program strings for the given geometry type
    unsigned int index = 0;
    switch( expectedGeometryType )
    {
      case GEOMETRY_TYPE_IMAGE:
      {
        index = 0;
        break;
      }
      case GEOMETRY_TYPE_TEXT:
      {
        index = 1;
        break;
      }
      case GEOMETRY_TYPE_UNTEXTURED_MESH:
      {
        index = 2;
        break;
      }
      case GEOMETRY_TYPE_TEXTURED_MESH:
      {
        index = 3;
        break;
      }
      case GEOMETRY_TYPE_LAST:
      {
        DALI_ASSERT_DEBUG(0 && "Wrong geometry type");
        break;
      }
    }

    vertexSource += customShaderWrappers[index].vertexShaderPrefix;
    // Append the custom vertex shader code if supplied, otherwise append the default
    if ( vertexBody.length() > 0 )
    {
      vertexSource.append( vertexBody );
    }
    else
    {
      vertexSource.append( customShaderWrappers[index].vertexShaderPostfix );
    }

    fragmentSource += customShaderWrappers[index].fragmentShaderPrefix;
    // Append the custom fragment shader code if supplied, otherwise append the default
    if ( fragmentBody.length() > 0 )
    {
      fragmentSource.append( fragmentBody );
    }
    else
    {
      fragmentSource.append( customShaderWrappers[index].fragmentShaderPostfix );
    }

    effect.SendProgramMessage( expectedGeometryType, SHADER_SUBTYPE_ALL, vertexSource, fragmentSource, modifiesGeometry );
  }
}

std::string GetShader(const std::string& field, const Property::Value& property)
{
  std::string value;
  if( property.HasKey(field) )
  {
    DALI_ASSERT_ALWAYS(property.GetValue(field).GetType() == Property::STRING && "Shader property is not a string" );

    // we could also check here for an array of strings as convenience for json not having multi line strings
    value = property.GetValue(field).Get<std::string>();
  }

  return value;
}

} // unnamed namespace

ShaderEffectPtr ShaderEffect::New( Dali::ShaderEffect::GeometryHints hints )
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  UpdateManager& updateManager = tls.GetUpdateManager();

  ShaderEffectPtr shaderEffect( new ShaderEffect( updateManager, hints ) );
  shaderEffect->RegisterObject();

  return shaderEffect;
}

ShaderEffect::ShaderEffect( UpdateManager& updateManager, Dali::ShaderEffect::GeometryHints hints )
: mUpdateManager( updateManager ),
  mConnectionCount (0),
  mGeometryHints( hints )
{
  mSceneObject = new Shader( hints );
  DALI_ASSERT_DEBUG( NULL != mSceneObject );

  // Transfer shader ownership to a scene message
  AddShaderMessage( mUpdateManager, *mSceneObject );
}

ShaderEffect::~ShaderEffect()
{
  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    // Remove scene-object using a message to the UpdateManager
    if( mSceneObject )
    {
      RemoveShaderMessage( mUpdateManager, *mSceneObject );
    }
    UnregisterObject();
  }
}

void ShaderEffect::SetEffectImage( Dali::Image image )
{
  // if images are the same, do nothing
  if (mImage == image)
  {
    return;
  }

  if (mImage && mConnectionCount > 0)
  {
    // unset previous image
    GetImplementation(mImage).Disconnect();
  }

  // in case image is empty this will reset our image handle
  mImage = image;

  if (!image)
  {
    // mSceneShader can be in a separate thread; queue a setter message
    SetTextureIdMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, 0 );
  }
  else
  {
    // tell image that we're using it
    if (mConnectionCount > 0)
    {
      GetImplementation(mImage).Connect();
    }
    // mSceneShader can be in a separate thread; queue a setter message
    SetTextureIdMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, GetImplementation(mImage).GetResourceId() );
  }
}

void ShaderEffect::SetUniform( const std::string& name, Property::Value value, UniformCoordinateType uniformCoordinateType )
{
  // Register the property if it does not exist
  Property::Index index = GetPropertyIndex( name );
  if ( Property::INVALID_INDEX == index )
  {
    index = RegisterProperty( name, value );
  }

  SetProperty( index, value );

  // RegisterProperty guarantees a positive value as index
  DALI_ASSERT_DEBUG( static_cast<unsigned int>(index) >= CustomPropertyStartIndex() );
  unsigned int metaIndex = index - CustomPropertyStartIndex();
  // check if there's space in cache
  if( mCoordinateTypes.Count() < (metaIndex + 1) )
  {
    mCoordinateTypes.Resize( metaIndex + 1 );
  }
  // only send message if the value is different than current, initial value is COORDINATE_TYPE_DEFAULT (0)
  if( uniformCoordinateType != mCoordinateTypes[ metaIndex ] )
  {
    mCoordinateTypes[ metaIndex ] = uniformCoordinateType;
    SetCoordinateTypeMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, metaIndex, uniformCoordinateType );
  }
}

void ShaderEffect::AttachExtension( Dali::ShaderEffect::Extension *extension )
{
  DALI_ASSERT_ALWAYS( extension != NULL && "Attaching uninitialized extension" );
  mExtension = IntrusivePtr<Dali::ShaderEffect::Extension>( extension );
}

Dali::ShaderEffect::Extension& ShaderEffect::GetExtension()
{
  DALI_ASSERT_ALWAYS( mExtension && "Getting uninitialized extension" );
  return *mExtension;
}

const Dali::ShaderEffect::Extension& ShaderEffect::GetExtension() const
{
  DALI_ASSERT_ALWAYS( mExtension && "Getting uninitialized extension" );
  return *mExtension;
}

void ShaderEffect::SetPrograms( GeometryType geometryType, const string& vertexSource, const string& fragmentSource )
{
  SetPrograms( geometryType, "", "", vertexSource, fragmentSource );
}

void ShaderEffect::SetPrograms( GeometryType geometryType,
                                const std::string& vertexPrefix, const std::string& fragmentPrefix,
                                const std::string& vertexSource, const std::string& fragmentSource )
{
  bool modifiesGeometry = true;
  // check if the vertex shader is empty (means it cannot modify geometry)
  if( (vertexPrefix.length() == 0 )&&( vertexSource.length() == 0 ) )
  {
    modifiesGeometry = false;
  }
  // check the hint second
  if( (mGeometryHints & Dali::ShaderEffect::HINT_DOESNT_MODIFY_GEOMETRY ) != 0 )
  {
    modifiesGeometry = false;
  }

  WrapAndSetProgram( *this, geometryType, GEOMETRY_TYPE_IMAGE, vertexPrefix, fragmentPrefix, vertexSource, fragmentSource, modifiesGeometry );
  WrapAndSetProgram( *this, geometryType, GEOMETRY_TYPE_TEXT, vertexPrefix, fragmentPrefix, vertexSource, fragmentSource, modifiesGeometry );
  WrapAndSetProgram( *this, geometryType, GEOMETRY_TYPE_TEXTURED_MESH, vertexPrefix, fragmentPrefix, vertexSource, fragmentSource, modifiesGeometry );
  WrapAndSetProgram( *this, geometryType, GEOMETRY_TYPE_UNTEXTURED_MESH, vertexPrefix, fragmentPrefix, vertexSource, fragmentSource, modifiesGeometry );
}

void ShaderEffect::SendProgramMessage( GeometryType geometryType, ShaderSubTypes subType,
                                       const string& vertexSource, const string& fragmentSource,
                                       bool modifiesGeometry )
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  ShaderFactory& shaderFactory = tls.GetShaderFactory();
  size_t shaderHash;

  ResourceTicketPtr ticket( shaderFactory.Load(vertexSource, fragmentSource, shaderHash) );

  DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, "ShaderEffect: SetProgram(geometryType %d subType:%d ticket.id:%d)\n", geometryType, subType, ticket->GetId() );

  // Add shader program to scene-object using a message to the UpdateManager
  SetShaderProgramMessage( mUpdateManager, *mSceneObject, geometryType, subType, ticket->GetId(), shaderHash, modifiesGeometry );

  mTickets.push_back(ticket);       // add ticket to collection to keep it alive.
}

void ShaderEffect::Connect()
{
  ++mConnectionCount;

  if (mImage && mConnectionCount == 1)
  {
    GetImplementation(mImage).Connect();

    // Image may have changed resource due to load/release policy. Ensure correct texture ID is set on scene graph object
    SetTextureIdMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, GetImplementation(mImage).GetResourceId() );
  }
}

void ShaderEffect::Disconnect()
{
  DALI_ASSERT_DEBUG(mConnectionCount > 0);
  --mConnectionCount;

  if (mImage && mConnectionCount == 0)
  {
     GetImplementation(mImage).Disconnect();
  }
}

unsigned int ShaderEffect::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void ShaderEffect::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const char* ShaderEffect::GetDefaultPropertyName(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }
  else
  {
    return NULL;
  }
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
  return true; // all properties are writable
}

bool ShaderEffect::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return false; // all properties are non animatable
}

bool ShaderEffect::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return false; // all properties cannot be used as constraint input
}

Property::Type ShaderEffect::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }
  else
  {
    // index out of range...return Property::NONE
    return Property::NONE;
  }
}

void ShaderEffect::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  switch ( index )
  {
    case Dali::ShaderEffect::GRID_DENSITY:
    {
      SetGridDensityMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, propertyValue.Get<float>() );
      break;
    }

    case Dali::ShaderEffect::IMAGE:
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

    case Dali::ShaderEffect::PROGRAM:
    {
      std::string vertexPrefix   = GetShader("vertex-prefix", propertyValue);
      std::string fragmentPrefix = GetShader("fragment-prefix", propertyValue);
      std::string vertex         = GetShader("vertex", propertyValue);
      std::string fragment       = GetShader("fragment", propertyValue);

      GeometryType geometryType      = GEOMETRY_TYPE_IMAGE;

      if( propertyValue.HasKey("geometry-type") )
      {
        Property::Value geometryValue  = propertyValue.GetValue("geometry-type");
        DALI_ASSERT_ALWAYS(geometryValue.GetType() == Property::STRING && "Geometry type is not a string" );

        std::string s = geometryValue.Get<std::string>();
        if(s == "GEOMETRY_TYPE_IMAGE")
        {
          geometryType  = GEOMETRY_TYPE_IMAGE;
        }
        else if (s == "GEOMETRY_TYPE_TEXT")
        {
          geometryType  = GEOMETRY_TYPE_TEXT;
        }
        else if( s == "GEOMETRY_TYPE_UNTEXTURED_MESH")
        {
          geometryType  = GEOMETRY_TYPE_UNTEXTURED_MESH;
        }
        else if( s == "GEOMETRY_TYPE_TEXTURED_MESH")
        {
          geometryType  = GEOMETRY_TYPE_TEXTURED_MESH;
        }
        else
        {
          DALI_ASSERT_ALWAYS(!"Geometry type unknown" );
        }
      }
      SetPrograms( geometryType, vertexPrefix, fragmentPrefix, vertex, fragment );
      break;
    }

    case Dali::ShaderEffect::GEOMETRY_HINTS:
    {
      Dali::ShaderEffect::GeometryHints hint = Dali::ShaderEffect::HINT_NONE;
      Property::Value geometryHintsValue   = propertyValue.GetValue("geometry-hints");

      std::string s = geometryHintsValue.Get<std::string>();
      if(s == "HINT_NONE")
      {
        hint = Dali::ShaderEffect::HINT_NONE;
      }
      else if(s == "HINT_GRID_X")
      {
        hint = Dali::ShaderEffect::HINT_GRID_X;
      }
      else if(s == "HINT_GRID_Y")
      {
        hint = Dali::ShaderEffect::HINT_GRID_Y;
      }
      else if(s == "HINT_GRID")
      {
        hint = Dali::ShaderEffect::HINT_GRID;
      }
      else if(s == "HINT_DEPTH_BUFFER")
      {
        hint = Dali::ShaderEffect::HINT_DEPTH_BUFFER;
      }
      else if(s == "HINT_BLENDING")
      {
        hint = Dali::ShaderEffect::HINT_BLENDING;
      }
      else if(s == "HINT_DOESNT_MODIFY_GEOMETRY")
      {
        hint = Dali::ShaderEffect::HINT_DOESNT_MODIFY_GEOMETRY;
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Geometry hint unknown" );
      }

      SetHintsMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, hint );

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

void ShaderEffect::NotifyScenePropertyInstalled( const SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index )
{
  // Warning - the property is added to the Shader object in the Update thread and the meta-data is added in the Render thread (through a secondary message)

  // mSceneObject requires metadata for each custom property (uniform)
  UniformMeta* meta = UniformMeta::New( name, newProperty, Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
  // mSceneObject is being used in a separate thread; queue a message to add the property
  InstallUniformMetaMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, *meta ); // Message takes ownership
}

const SceneGraph::PropertyOwner* ShaderEffect::GetSceneObject() const
{
  return mSceneObject;
}

const PropertyBase* ShaderEffect::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  CustomProperty* custom = FindCustomProperty( index );
  DALI_ASSERT_ALWAYS( custom && "Property index is invalid" );
  return custom->GetSceneGraphProperty();
}

const PropertyInputImpl* ShaderEffect::GetSceneObjectInputProperty( Property::Index index ) const
{
  return GetSceneObjectAnimatableProperty( index );
}

} // namespace Internal

} // namespace Dali
