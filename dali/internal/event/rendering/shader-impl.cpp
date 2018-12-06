/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/rendering/shader-impl.h> // Dali::Internal::Shader

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name             |type    |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "program",       MAP,     true,     false,     false,  Dali::Shader::Property::PROGRAM )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX, ShaderDefaultProperties )

Dali::Scripting::StringEnum ShaderHintsTable[] =
  { { "NONE",                     Dali::Shader::Hint::NONE},
    { "OUTPUT_IS_TRANSPARENT",    Dali::Shader::Hint::OUTPUT_IS_TRANSPARENT},
    { "MODIFIES_GEOMETRY",        Dali::Shader::Hint::MODIFIES_GEOMETRY}
  };

const uint32_t ShaderHintsTableSize = static_cast<uint32_t>( sizeof( ShaderHintsTable ) / sizeof( ShaderHintsTable[0] ) );

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType( typeid( Dali::Shader ), typeid( Dali::Handle ), Create, ShaderDefaultProperties );

#define TOKEN_STRING(x) (#x)

void AppendString(std::string& to, const std::string& append)
{
  if(to.size())
  {
    to += ",";
  }
  to += append;
}

Property::Value HintString(const Dali::Shader::Hint::Value& hints)
{
  std::string s;

  if(hints == Dali::Shader::Hint::NONE)
  {
    s = "NONE";
  }

  if(hints & Dali::Shader::Hint::OUTPUT_IS_TRANSPARENT)
  {
    AppendString(s, "OUTPUT_IS_TRANSPARENT");
  }

  if(hints & Dali::Shader::Hint::MODIFIES_GEOMETRY)
  {
    AppendString(s, "MODIFIES_GEOMETRY");
  }

  return Property::Value(s);
}

} // unnamed namespace

ShaderPtr Shader::New( const std::string& vertexShader,
                       const std::string& fragmentShader,
                       Dali::Shader::Hint::Value hints )
{
  // create scene object first so it's guaranteed to exist for the event side
  auto sceneObject = new SceneGraph::Shader( hints );
  OwnerPointer< SceneGraph::Shader > transferOwnership( sceneObject );
  // pass the pointer to base for message passing
  ShaderPtr shader( new Shader( sceneObject ) );
  // transfer scene object ownership to update manager
  auto&& services = shader->GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = services.GetUpdateManager();
  AddShaderMessage( updateManager, transferOwnership );

  services.RegisterObject( shader.Get() );
  shader->SetShader( vertexShader, fragmentShader, hints );

  return shader;
}

const SceneGraph::Shader& Shader::GetShaderSceneObject() const
{
  return static_cast<const SceneGraph::Shader&>( GetSceneObject() );
}

void Shader::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  switch(index)
  {
    case Dali::Shader::Property::PROGRAM:
    {
      if( propertyValue.GetType() == Property::MAP )
      {
        Dali::Property::Map* map = propertyValue.GetMap();
        if( map )
        {
          std::string vertex;
          std::string fragment;
          Dali::Shader::Hint::Value hints(Dali::Shader::Hint::NONE);

          if( Property::Value* value = map->Find("vertex") )
          {
            vertex = value->Get<std::string>();
          }

          if( Property::Value* value = map->Find("fragment") )
          {
            fragment = value->Get<std::string>();
          }

          if( Property::Value* value = map->Find("hints") )
          {
            static_cast<void>( // ignore return
              Scripting::GetEnumeration< Dali::Shader::Hint::Value >(value->Get<std::string>().c_str(),
                                                                     ShaderHintsTable, ShaderHintsTableSize, hints)
              );
          }

          SetShader( vertex, fragment, hints );
        }
      }
      else
      {
        DALI_LOG_WARNING( "Shader program property should be a map\n" );
      }
      break;
    }
  }
}

Property::Value Shader::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  switch(index)
  {
    case Dali::Shader::Property::PROGRAM:
    {
      Dali::Property::Map map;
      if( mShaderData )
      {
        map["vertex"] = Property::Value(mShaderData->GetVertexShader());
        map["fragment"] = Property::Value(mShaderData->GetFragmentShader());
        map["hints"] = HintString(mShaderData->GetHints());
      }
      value = map;
      break;
    }
  }

  return value;
}

Property::Value Shader::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  return GetDefaultProperty( index ); // Event-side only properties
}

Shader::Shader( const SceneGraph::Shader* sceneObject )
: Object( sceneObject ),
  mShaderData( nullptr )
{
}

void Shader::SetShader( const std::string& vertexSource,
                        const std::string& fragmentSource,
                        Dali::Shader::Hint::Value hints )
{
  // Try to load a pre-compiled shader binary for the source pair:
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  ShaderFactory& shaderFactory = tls.GetShaderFactory();
  size_t shaderHash;
  mShaderData = shaderFactory.Load( vertexSource, fragmentSource, hints, shaderHash );

  // Add shader program to scene-object using a message to the UpdateManager
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
  SetShaderProgramMessage( updateManager, GetShaderSceneObject(), mShaderData, (hints & Dali::Shader::Hint::MODIFIES_GEOMETRY) != 0x0 );
}

Shader::~Shader()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveShaderMessage( updateManager, &GetShaderSceneObject() );

    eventThreadServices.UnregisterObject( this );
  }
}


} // namespace Internal
} // namespace Dali
