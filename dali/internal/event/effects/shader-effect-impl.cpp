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
#include <dali/internal/event/effects/shader-effect-impl.h>

// EXTERNAL INCLUDES
#include <fstream>

// INTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/scripting/scripting.h>
#include "dali-shaders.h"
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/render/shaders/shader.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>

using Dali::Internal::SceneGraph::UpdateManager;
using Dali::Internal::SceneGraph::UniformMeta;
using Dali::Internal::SceneGraph::Shader;
using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::PropertyBase;
using Dali::Internal::SceneGraph::PropertyBase;
using Dali::Internal::SceneGraph::RenderQueue;

namespace Dali
{

const Property::Index ShaderEffect::GRID_DENSITY        = 0;
const Property::Index ShaderEffect::IMAGE               = 1;
const Property::Index ShaderEffect::PROGRAM             = 2;
const Property::Index ShaderEffect::GEOMETRY_HINTS      = 3;

namespace Internal
{

ShaderEffect::DefaultPropertyLookup* ShaderEffect::mDefaultPropertyLookup = NULL;

namespace
{

BaseHandle Create()
{
  Internal::ShaderEffectPtr internal = Internal::ShaderEffect::New( );

  ShaderFactory::LoadTextSubtypeShaders(internal);

  return Dali::ShaderEffect(internal.Get());
}

TypeRegistration mType( typeid(Dali::ShaderEffect), typeid(Dali::Handle), Create );

const std::string DEFAULT_PROPERTY_NAMES[] =
{
  "grid-density",
  "image",
  "program",
  "geometry-hints",
};
const int DEFAULT_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_NAMES ) / sizeof( std::string );

const Property::Type DEFAULT_PROPERTY_TYPES[DEFAULT_PROPERTY_COUNT] =
{
  Property::FLOAT,    // "grid-density",
  Property::MAP,      // "image",
  Property::MAP,      // "program",
  Property::INTEGER,  // "geometry-hints",
};

void SetShaderProgram( const std::string& imageVertexSnippet, const std::string& imageFragmentSnippet,
                       const std::string& textVertexSnippet, const std::string& textFragmentSnippet,
                       const std::string& texturedMeshVertexSnippet, const std::string& texturedMeshFragmentSnippet,
                       const std::string& meshVertexSnippet, const std::string& meshFragmentSnippet,
                       const std::string& vertexShaderPrefix, const std::string& fragmentShaderPrefix,
                       ShaderEffectPtr shaderEffect )
{
  DALI_ASSERT_DEBUG(shaderEffect);

  // create complete shader program strings for GEOMETRY_TYPE_IMAGE
  std::string vertexSource = vertexShaderPrefix + CustomImagePrefixVertex;
  std::string fragmentSource = fragmentShaderPrefix + CustomImagePrefixFragment;

  // Append the custom vertex shader code if supplied, otherwise append the default
  if ( imageVertexSnippet.length() > 0 )
  {
    vertexSource.append( imageVertexSnippet );
  }
  else
  {
    vertexSource.append( CustomImagePostfixVertex );
  }

  // Append the custom fragment shader code if supplied, otherwise append the default
  if ( imageFragmentSnippet.length() > 0 )
  {
    fragmentSource.append( imageFragmentSnippet );
  }
  else
  {
    fragmentSource.append( CustomImagePostfixFragment );
  }

  // Add the program
  shaderEffect->SetProgram( GEOMETRY_TYPE_IMAGE, SHADER_SUBTYPE_ALL, vertexSource, fragmentSource );


  // create complete shader program strings for GEOMETRY_TYPE_TEXT
  vertexSource = CustomFontPrefixVertex;
  fragmentSource = CustomFontPrefixFragment;

  // Append the custom vertex shader code if supplied, otherwise append the default
  if ( textVertexSnippet.length() > 0 )
  {
    vertexSource.append( textVertexSnippet );
  }
  else
  {
    vertexSource.append( CustomFontPostfixVertex );
  }

  // Append the custom fragment shader code if supplied, otherwise append the default
  if ( textFragmentSnippet.length() > 0 )
  {
    fragmentSource.append( textFragmentSnippet );
  }
  else
  {
    fragmentSource.append( CustomFontPostfixFragment );
  }

  // Add Program. Note, we only change the default program here, and leave the other sub-types as-is
  shaderEffect->SetProgram(GEOMETRY_TYPE_TEXT, SHADER_DEFAULT, vertexSource, fragmentSource);

  // create complete shader program strings for GEOMETRY_TYPE_TEXTURED_MESH
  if( texturedMeshVertexSnippet.length() > 0 )
  {
    vertexSource = CustomMeshPrefixVertex;
    vertexSource.append( texturedMeshVertexSnippet );
  }
  else
  {
    vertexSource = MeshVertex;
  }

  if( texturedMeshFragmentSnippet.length() > 0 )
  {
    fragmentSource = CustomMeshPrefixFragment;
    fragmentSource.append( texturedMeshFragmentSnippet );
  }
  else
  {
    fragmentSource = MeshFragment;
  }

  // Add Program
  shaderEffect->SetProgram( GEOMETRY_TYPE_TEXTURED_MESH, SHADER_SUBTYPE_ALL, vertexSource, fragmentSource );

  // create complete shader program strings for GEOMETRY_TYPE_MESH
  if( meshVertexSnippet.length() > 0 )
  {
    vertexSource = CustomMeshPrefixVertex;
    vertexSource.append( meshVertexSnippet );
  }
  else
  {
    vertexSource = MeshVertex;
  }

  if( meshFragmentSnippet.length() > 0 )
  {
    fragmentSource = CustomMeshPrefixFragment;
    fragmentSource.append( meshFragmentSnippet );
  }
  else
  {
    fragmentSource = MeshFragment;
  }

  // Add Program
  shaderEffect->SetProgram( GEOMETRY_TYPE_MESH, SHADER_SUBTYPE_ALL, vertexSource, fragmentSource );
}

void SetShaderProgram( const std::string& vertexShaderPrefix, const std::string& vertexShader,
                       const std::string& fragmentShaderPrefix, const std::string& fragmentShader,
                       GeometryType geometryTypes,
                       ShaderEffectPtr shaderEffect )
{
  DALI_ASSERT_DEBUG(shaderEffect);

  if( geometryTypes & GEOMETRY_TYPE_IMAGE )
  {
    shaderEffect->SetProgram( GEOMETRY_TYPE_IMAGE, SHADER_SUBTYPE_ALL,
                              vertexShaderPrefix + CustomImagePrefixVertex + ( !vertexShader.empty() ? vertexShader : CustomImagePostfixVertex ),
                              fragmentShaderPrefix + CustomImagePrefixFragment + ( !fragmentShader.empty() ? fragmentShader : CustomImagePostfixFragment ) );
  }
  else
  {
    shaderEffect->SetProgram( GEOMETRY_TYPE_IMAGE, SHADER_SUBTYPE_ALL,
                              CustomImagePrefixVertex + CustomImagePostfixVertex,
                              CustomImagePrefixFragment + CustomImagePostfixFragment );
  }

  if( geometryTypes & GEOMETRY_TYPE_TEXT )
  {
    shaderEffect->SetProgram( GEOMETRY_TYPE_TEXT, SHADER_DEFAULT,
                              vertexShaderPrefix + CustomFontPrefixVertex + ( !vertexShader.empty() ? vertexShader : CustomFontPostfixVertex ),
                              fragmentShaderPrefix + CustomFontPrefixFragment + ( !fragmentShader.empty() ? fragmentShader : CustomFontPostfixFragment ) );
  }
  else
  {
    shaderEffect->SetProgram( GEOMETRY_TYPE_TEXT, SHADER_DEFAULT,
                              CustomFontPrefixVertex + CustomFontPostfixVertex,
                              CustomFontPrefixFragment + CustomFontPostfixFragment );
  }

  if( geometryTypes & GEOMETRY_TYPE_TEXTURED_MESH )
  {
    shaderEffect->SetProgram( GEOMETRY_TYPE_TEXTURED_MESH, SHADER_SUBTYPE_ALL,
                              vertexShaderPrefix + ( !vertexShader.empty() ? CustomMeshPrefixVertex + vertexShader : MeshVertex ),
                              fragmentShaderPrefix + ( !fragmentShader.empty() ? CustomMeshPrefixFragment + fragmentShader : MeshFragment ) );
  }
  else
  {
    shaderEffect->SetProgram( GEOMETRY_TYPE_TEXTURED_MESH, SHADER_SUBTYPE_ALL, MeshVertex, MeshFragment );
  }

  if( geometryTypes & GEOMETRY_TYPE_MESH )
  {
    shaderEffect->SetProgram( GEOMETRY_TYPE_MESH, SHADER_SUBTYPE_ALL,
                              vertexShaderPrefix + ( !vertexShader.empty() ? CustomMeshPrefixVertex + vertexShader : MeshColorNoTextureVertex ),
                              fragmentShaderPrefix + ( !fragmentShader.empty() ? CustomMeshPrefixFragment + fragmentShader : MeshColorNoTextureFragment ) );
  }
  else
  {
    shaderEffect->SetProgram( GEOMETRY_TYPE_MESH, SHADER_SUBTYPE_ALL, MeshColorNoTextureVertex, MeshColorNoTextureFragment );
  }
}

std::string GetFileContents( const std::string& filename )
{
  std::ifstream input( filename.c_str() );
  return std::string( std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() );
}

std::string GetShader(const std::string& field, const Property::Value& property)
{
  if( property.HasKey(field) )
  {
    DALI_ASSERT_ALWAYS(property.GetValue(field).GetType() == Property::STRING && "Shader property is not a string" );

    // we could also check here for an array of strings as convenience for json not having multi line strings
    return property.GetValue(field).Get<std::string>();
  }
  else
  {
    // convention of shader field appended with -filename signifies a file
    std::string filenameKey(std::string(field) + std::string("-filename"));

    if( property.HasKey( filenameKey ) )
    {
      DALI_ASSERT_ALWAYS(property.GetValue(filenameKey).GetType() == Property::STRING && "Shader filename property is not a string" );
      // this should eventually be done by an adaptor interface
      return GetFileContents( property.GetValue(filenameKey).Get<std::string>() );
    }
  }

  // if we got here
  return std::string();
}

} // anon namespace


ShaderEffectPtr ShaderEffect::New( Dali::ShaderEffect::GeometryHints hints )
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  UpdateManager& updateManager = tls.GetUpdateManager();
  ShaderFactory& shaderFactory = tls.GetShaderFactory();

  // Create a new scene-object, temporarily owned
  Shader* sceneObject = new Shader( hints );
  DALI_ASSERT_DEBUG( NULL != sceneObject );

  ShaderEffectPtr shaderEffect( new ShaderEffect( updateManager, shaderFactory, *sceneObject ) );
  shaderEffect->RegisterObject();

  return shaderEffect;
}

ShaderEffectPtr ShaderEffect::New( const std::string& vertexShader,
                                   const std::string& fragmentShader,
                                   GeometryType geometryType,
                                   Dali::ShaderEffect::GeometryHints hints )
{
  return NewWithPrefix( "", vertexShader, "", fragmentShader, geometryType, hints);
}

ShaderEffectPtr ShaderEffect::NewWithPrefix( const std::string& vertexShaderPrefix,
                                             const std::string& vertexShader,
                                             const std::string& fragmentShaderPrefix,
                                             const std::string& fragmentShader,
                                             GeometryType geometryTypes,
                                             Dali::ShaderEffect::GeometryHints hints )
{
  ShaderEffectPtr shaderEffect( New(hints) );

  ShaderFactory::LoadTextSubtypeShaders(shaderEffect);

  SetShaderProgram( vertexShaderPrefix, vertexShader,
                    fragmentShaderPrefix, fragmentShader,
                    geometryTypes,
                    shaderEffect );

  return shaderEffect;
}

ShaderEffectPtr ShaderEffect::New( const std::string& imageVertexShader,
                                   const std::string& imageFragmentShader,
                                   const std::string& textVertexShader,
                                   const std::string& textFragmentShader,
                                   const std::string& texturedMeshVertexShader,
                                   const std::string& texturedMeshFragmentShader,
                                   const std::string& meshVertexShader,
                                   const std::string& meshFragmentShader,
                                   Dali::ShaderEffect::GeometryHints hints )
{
  ShaderEffectPtr shaderEffect( New(hints) );

  ShaderFactory::LoadTextSubtypeShaders(shaderEffect);

  SetShaderProgram( imageVertexShader, imageFragmentShader,
                    textVertexShader,  textFragmentShader,
                    texturedMeshVertexShader, texturedMeshFragmentShader,
                    meshVertexShader,  meshFragmentShader,
                    "", "",
                    shaderEffect );

  return shaderEffect;
}

ShaderEffect::~ShaderEffect()
{
  DALI_ASSERT_DEBUG( mSceneObject != NULL );

  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    // Remove scene-object using a message to the UpdateManager
    RemoveShaderMessage( mUpdateManager, *mSceneObject );

    UnregisterObject();
  }
}

ShaderEffect::ShaderEffect( UpdateManager& updateManager, ShaderFactory& shaderFactory, Shader& sceneObject )
: mUpdateManager( updateManager ),
  mShaderFactory( shaderFactory ),
  mSceneObject( &sceneObject ),
  mConnectionCount (0)
{
  // Transfer shader ownership to a scene message
  AddShaderMessage( mUpdateManager, *mSceneObject );
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

  SetCoordinateTypeMessage( mUpdateManager.GetEventToUpdate(), *mCustomMetadata[index], uniformCoordinateType );
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

void ShaderEffect::SetProgram( GeometryType geometryType, ShaderSubTypes subType,
                               const std::string& vertexSource, const std::string& fragmentSource )
{
  // Load done asynchronously in update thread. SetProgram message below must be processed afterwards.
  // Therefore, resource manager cannot farm out the loading to the adaptor resource threads,
  // but must instead use synchronous loading via PlatformAbstraction::LoadFile()
  size_t shaderHash;
  ResourceTicketPtr ticket( mShaderFactory.Load(vertexSource, fragmentSource, shaderHash) );

  DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, "ShaderEffect: SetProgram(geometryType %d subType:%d ticket.id:%d)\n", geometryType, subType, ticket->GetId() );

  // Add shader program to scene-object using a message to the UpdateManager
  SetShaderProgramMessage( mUpdateManager, *mSceneObject, geometryType, subType, ticket->GetId(), shaderHash );

  mTickets.push_back(ticket);       // add ticket to collection to keep it alive.
}


void ShaderEffect::SetProgram( GeometryType geometryType, ShaderSubTypes subType,
                               const std::string& vertexPrefix, const std::string& fragmentPrefix,
                               const std::string& vertexSource, const std::string& fragmentSource )
{
  const std::string vertex( vertexPrefix + vertexSource );
  const std::string fragment( fragmentPrefix + fragmentSource );
  SetProgram( geometryType, subType, vertex, fragment );
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

bool ShaderEffect::IsSceneObjectRemovable() const
{
  return false; // The Shader is not removed during this proxy's lifetime
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

const std::string& ShaderEffect::GetDefaultPropertyName(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_NAMES[index];
  }
  else
  {
    // index out of range..return empty string
    static const std::string INVALID_PROPERTY_NAME;
    return INVALID_PROPERTY_NAME;
  }
}

Property::Index ShaderEffect::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Lazy initialization of static mDefaultPropertyLookup
  if (!mDefaultPropertyLookup)
  {
    mDefaultPropertyLookup = new DefaultPropertyLookup();

    for (int i=0; i<DEFAULT_PROPERTY_COUNT; ++i)
    {
      (*mDefaultPropertyLookup)[DEFAULT_PROPERTY_NAMES[i]] = i;
    }
  }
  DALI_ASSERT_DEBUG( NULL != mDefaultPropertyLookup );

  // Look for name in default properties
  DefaultPropertyLookup::const_iterator result = mDefaultPropertyLookup->find( name );
  if ( mDefaultPropertyLookup->end() != result )
  {
    index = result->second;
  }

  return index;
}

bool ShaderEffect::IsDefaultPropertyWritable(Property::Index index) const
{
  return true;
}

bool ShaderEffect::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return false;
}

Property::Type ShaderEffect::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_TYPES[index];
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
        else if( s == "GEOMETRY_TYPE_MESH")
        {
          geometryType  = GEOMETRY_TYPE_MESH;
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
      SetShaderProgram( vertexPrefix, vertex, fragmentPrefix, fragment, geometryType, ShaderEffectPtr(this) );
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
      else
      {
        DALI_ASSERT_ALWAYS(!"Geometry hint unknown" );
      }

      SetHintsMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, hint );

      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS(false && "ShaderEffect property enumeration out of range"); // should not come here
      break;
    }
  }
}

Property::Value ShaderEffect::GetDefaultProperty(Property::Index /*index*/) const
{
  // none of our properties are readable so return empty
  return Property::Value();
}

void ShaderEffect::InstallSceneObjectProperty( PropertyBase& newProperty, const std::string& name, unsigned int index )
{
  // Warning - the property is added to the Shader object in the Update thread and the meta-data is added in the Render thread (through a secondary message)

  // mSceneObject is being used in a separate thread; queue a message to add the property
  InstallCustomPropertyMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, newProperty ); // Message takes ownership

  // mSceneObject requires metadata for each custom property (uniform)
  UniformMeta* meta = UniformMeta::New( name, newProperty, Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
  // mSceneObject is being used in a separate thread; queue a message to add the property
  InstallUniformMetaMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, *meta ); // Message takes ownership

  // Add entry to the metadata lookup
  mCustomMetadata[index] = meta;
}

const SceneGraph::PropertyOwner* ShaderEffect::GetSceneObject() const
{
  return mSceneObject;
}

const PropertyBase* ShaderEffect::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  CustomPropertyLookup::const_iterator entry = GetCustomPropertyLookup().find( index );

  DALI_ASSERT_ALWAYS( GetCustomPropertyLookup().end() != entry && "Property index is invalid" );

  DALI_ASSERT_ALWAYS( entry->second.IsAnimatable() && "shader effect has only animatable properties" );

  return dynamic_cast<const PropertyBase*>( entry->second.GetSceneGraphProperty() );
}

const PropertyInputImpl* ShaderEffect::GetSceneObjectInputProperty( Property::Index index ) const
{
  CustomPropertyLookup::const_iterator entry = GetCustomPropertyLookup().find( index );

  DALI_ASSERT_ALWAYS( GetCustomPropertyLookup().end() != entry && "Property index is invalid" );

  DALI_ASSERT_ALWAYS( entry->second.IsAnimatable() && "shader effect has only animatable properties" );

  return entry->second.GetSceneGraphProperty();
}

} // namespace Internal

} // namespace Dali
