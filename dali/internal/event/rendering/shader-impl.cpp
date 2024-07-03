/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/scripting/scripting.h>
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/public-api/object/type-registry.h>

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
DALI_PROPERTY("program", MAP, true, false, false, Dali::Shader::Property::PROGRAM)
DALI_PROPERTY_TABLE_END(DEFAULT_ACTOR_PROPERTY_START_INDEX, ShaderDefaultProperties)

Dali::Scripting::StringEnum ShaderHintsTable[] =
  {{"NONE", Dali::Shader::Hint::NONE},
   {"OUTPUT_IS_TRANSPARENT", Dali::Shader::Hint::OUTPUT_IS_TRANSPARENT},
   {"MODIFIES_GEOMETRY", Dali::Shader::Hint::MODIFIES_GEOMETRY}};

const uint32_t ShaderHintsTableSize = static_cast<uint32_t>(sizeof(ShaderHintsTable) / sizeof(ShaderHintsTable[0]));

static constexpr uint32_t         DEFAULT_RENDER_PASS_TAG = 0u;
static constexpr std::string_view DEFAULT_SHADER_NAME     = "";

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType(typeid(Dali::Shader), typeid(Dali::Handle), Create, ShaderDefaultProperties);

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

void GetShaderData(const Property::Map& shaderMap, std::string& vertexShader, std::string& fragmentShader, uint32_t& renderPassTag, Dali::Shader::Hint::Value& hints, std::string& name)
{
  hints         = Dali::Shader::Hint::NONE;
  renderPassTag = DEFAULT_RENDER_PASS_TAG;
  name          = DEFAULT_SHADER_NAME;

  if(Property::Value* value = shaderMap.Find("vertex"))
  {
    vertexShader = value->Get<std::string>();
  }

  if(Property::Value* value = shaderMap.Find("fragment"))
  {
    fragmentShader = value->Get<std::string>();
  }

  if(Property::Value* value = shaderMap.Find("renderPassTag"))
  {
    renderPassTag = static_cast<uint32_t>(value->Get<int32_t>());
  }

  if(Property::Value* value = shaderMap.Find("name"))
  {
    name = value->Get<std::string>();
  }

  if(Property::Value* value = shaderMap.Find("hints"))
  {
    int32_t hintInteger = 0;
    if(value->Get(hintInteger))
    {
      hints = static_cast<Dali::Shader::Hint::Value>(hintInteger);
    }
    else
    {
      static_cast<void>( // ignore return
        Scripting::GetEnumeration<Dali::Shader::Hint::Value>(value->Get<std::string>().c_str(),
                                                             ShaderHintsTable,
                                                             ShaderHintsTableSize,
                                                             hints));
    }
  }
}

} // unnamed namespace

ShaderPtr Shader::New(std::string_view          vertexShader,
                      std::string_view          fragmentShader,
                      Dali::Shader::Hint::Value hints,
                      std::string_view          shaderName)
{
  // create scene object first so it's guaranteed to exist for the event side
  auto                             sceneObject = new SceneGraph::Shader();
  OwnerPointer<SceneGraph::Shader> transferOwnership(sceneObject);
  // pass the pointer to base for message passing
  ShaderPtr shader(new Shader(sceneObject));
  // transfer scene object ownership to update manager
  auto&&                     services      = shader->GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = services.GetUpdateManager();
  AddShaderMessage(updateManager, transferOwnership);

  services.RegisterObject(shader.Get());
  shader->UpdateShaderData(vertexShader, fragmentShader, DEFAULT_RENDER_PASS_TAG, hints, shaderName);

  return shader;
}

ShaderPtr Shader::New(Dali::Property::Value shaderMap)
{
  // create scene object first so it's guaranteed to exist for the event side
  auto                             sceneObject = new SceneGraph::Shader();
  OwnerPointer<SceneGraph::Shader> transferOwnership(sceneObject);
  // pass the pointer to base for message passing
  ShaderPtr shader(new Shader(sceneObject));
  // transfer scene object ownership to update manager
  auto&&                     services      = shader->GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = services.GetUpdateManager();
  AddShaderMessage(updateManager, transferOwnership);

  services.RegisterObject(shader.Get());
  shader->SetShaderProperty(shaderMap);

  return shader;
}

const SceneGraph::Shader& Shader::GetShaderSceneObject() const
{
  return static_cast<const SceneGraph::Shader&>(GetSceneObject());
}

void Shader::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  switch(index)
  {
    case Dali::Shader::Property::PROGRAM:
    {
      SetShaderProperty(propertyValue);
      break;
    }
  }
}

Property::Value Shader::GetDefaultProperty(Property::Index index) const
{
  Property::Value value;

  switch(index)
  {
    case Dali::Shader::Property::PROGRAM:
    {
      if(mShaderDataList.size() == 1u)
      {
        Dali::Property::Map map;
        map["vertex"]        = Property::Value(mShaderDataList.front()->GetVertexShader());
        map["fragment"]      = Property::Value(mShaderDataList.front()->GetFragmentShader());
        map["renderPassTag"] = Property::Value(static_cast<int32_t>(mShaderDataList.front()->GetRenderPassTag()));
        map["hints"]         = HintString(mShaderDataList.front()->GetHints());
        map["name"]          = Property::Value(mShaderDataList.front()->GetName());
        value                = map;
      }
      else
      {
        Dali::Property::Array array;
        for(auto&& shaderData : mShaderDataList)
        {
          if(shaderData)
          {
            Dali::Property::Map map;
            map["vertex"]        = Property::Value(shaderData->GetVertexShader());
            map["fragment"]      = Property::Value(shaderData->GetFragmentShader());
            map["renderPassTag"] = Property::Value(static_cast<int32_t>(shaderData->GetRenderPassTag()));
            map["hints"]         = HintString(shaderData->GetHints());
            map["name"]          = Property::Value(shaderData->GetName());
            array.PushBack(map);
          }
        }
        value = array;
      }
      break;
    }
  }

  return value;
}

Property::Value Shader::GetDefaultPropertyCurrentValue(Property::Index index) const
{
  return GetDefaultProperty(index); // Event-side only properties
}

Shader::Shader(const SceneGraph::Shader* sceneObject)
: Object(sceneObject)
{
}

void Shader::UpdateShaderData(std::string_view          vertexSource,
                              std::string_view          fragmentSource,
                              uint32_t                  renderPassTag,
                              Dali::Shader::Hint::Value hints,
                              std::string_view          name)
{
  // Try to load a pre-compiled shader binary for the source pair:
  ThreadLocalStorage&     tls           = ThreadLocalStorage::Get();
  ShaderFactory&          shaderFactory = tls.GetShaderFactory();
  size_t                  shaderHash;
  Internal::ShaderDataPtr shaderData = shaderFactory.Load(vertexSource, fragmentSource, hints, renderPassTag, name, shaderHash);

  std::vector<Internal::ShaderDataPtr>::iterator shaderDataIterator = std::find_if(mShaderDataList.begin(), mShaderDataList.end(), [&shaderData](const Internal::ShaderDataPtr& shaderDataItem) { return shaderDataItem->GetRenderPassTag() == shaderData->GetRenderPassTag(); });
  if(shaderDataIterator != mShaderDataList.end())
  {
    *shaderDataIterator = shaderData;
  }
  else
  {
    mShaderDataList.push_back(shaderData);
  }

  // Add shader data to scene-object
  SceneGraph::UpdateShaderDataMessage(GetEventThreadServices(), GetShaderSceneObject(), shaderData);
}

void Shader::SetShaderProperty(const Dali::Property::Value& shaderMap)
{
  if(shaderMap.GetType() == Property::MAP)
  {
    const Dali::Property::Map* map = shaderMap.GetMap();
    if(map)
    {
      std::string               vertex;
      std::string               fragment;
      uint32_t                  renderPassTag{DEFAULT_RENDER_PASS_TAG};
      Dali::Shader::Hint::Value hints(Dali::Shader::Hint::NONE);
      std::string               name(DEFAULT_SHADER_NAME);
      GetShaderData(*map, vertex, fragment, renderPassTag, hints, name);

      UpdateShaderData(vertex, fragment, renderPassTag, hints, name);
    }
  }
  else if(shaderMap.GetType() == Property::ARRAY)
  {
    const Dali::Property::Array* array = shaderMap.GetArray();
    if(array)
    {
      uint32_t arraySize = array->Count();
      for(uint32_t i = 0; i < arraySize; ++i)
      {
        const Dali::Property::Map* map = array->GetElementAt(i).GetMap();
        if(map)
        {
          std::string               vertex;
          std::string               fragment;
          uint32_t                  renderPassTag{DEFAULT_RENDER_PASS_TAG};
          Dali::Shader::Hint::Value hints(Dali::Shader::Hint::NONE);
          std::string               name(DEFAULT_SHADER_NAME);
          GetShaderData(*map, vertex, fragment, renderPassTag, hints, name);

          UpdateShaderData(vertex, fragment, renderPassTag, hints, name);
        }
      }
    }
  }
  else
  {
    DALI_LOG_ERROR("Shader program property should be a map or array of map.\n");
  }
}

Shader::~Shader()
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("~Shader[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }

  if(DALI_LIKELY(EventThreadServices::IsCoreRunning()))
  {
    EventThreadServices&       eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager       = eventThreadServices.GetUpdateManager();
    RemoveShaderMessage(updateManager, &GetShaderSceneObject());

    eventThreadServices.UnregisterObject(this);
  }
}

uint32_t Shader::GetShaderLanguageVersion()
{
  Dali::Internal::ThreadLocalStorage& tls = Dali::Internal::ThreadLocalStorage::Get();
  return tls.GetShaderLanguageVersion();
}

std::string Shader::GetShaderVersionPrefix()
{
  Dali::Internal::ThreadLocalStorage& tls = Dali::Internal::ThreadLocalStorage::Get();
  return tls.GetShaderVersionPrefix();
}

std::string Shader::GetVertexShaderPrefix()
{
  Dali::Internal::ThreadLocalStorage& tls = Dali::Internal::ThreadLocalStorage::Get();
  return tls.GetVertexShaderPrefix();
}

std::string Shader::GetFragmentShaderPrefix()
{
  Dali::Internal::ThreadLocalStorage& tls = Dali::Internal::ThreadLocalStorage::Get();
  return tls.GetFragmentShaderPrefix();
}

} // namespace Internal
} // namespace Dali
