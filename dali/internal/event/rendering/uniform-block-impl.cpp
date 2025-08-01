/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali/internal/event/rendering/uniform-block-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/renderer-impl.h>
#include <dali/internal/event/rendering/shader-impl.h>
#include <dali/internal/render/renderers/render-uniform-block.h>
#include <dali/internal/update/manager/update-manager.h> ///< for AddUniformBlockMessage

namespace Dali::Internal
{
UniformBlockPtr UniformBlock::New(std::string&& name)
{
  auto sceneObject = new Render::UniformBlock(std::move(name)); // In Update/Render side, only 1 object.

  OwnerPointer<Render::UniformBlock> transferOwnership(sceneObject);
  UniformBlockPtr                    uniformBlock(new UniformBlock(sceneObject));

  auto&& services = uniformBlock->GetEventThreadServices();
  AddUniformBlockMessage(services.GetUpdateManager(), transferOwnership);
  services.RegisterObject(uniformBlock.Get());

  return uniformBlock;
}

const Render::UniformBlock& UniformBlock::GetUniformBlockSceneObject() const
{
  return static_cast<const Render::UniformBlock&>(GetSceneObject());
}

bool UniformBlock::ConnectToShader(Shader* shader, bool strongConnection, bool programCacheCleanRequired)
{
  if(shader != nullptr)
  {
    if(mShaderContainer.find(shader) == mShaderContainer.end())
    {
      mShaderContainer.insert({shader, strongConnection});
      if(!strongConnection)
      {
        AddObserver(*shader);
      }
      shader->ConnectUniformBlock(*this, strongConnection, programCacheCleanRequired);
      return true;
    }
  }
  return false;
}

void UniformBlock::DisconnectFromShader(Shader* shader)
{
  if(shader != nullptr)
  {
    auto iter = mShaderContainer.find(shader);
    if(iter != mShaderContainer.end())
    {
      if(!iter->second)
      {
        RemoveObserver(*shader);
      }
      mShaderContainer.erase(iter);
      shader->DisconnectUniformBlock(*this);
    }
  }
}

void UniformBlock::ObjectDestroyed(Object& object)
{
  auto* shader = dynamic_cast<Shader*>(&object);
  if(shader)
  {
    DisconnectFromShader(shader);
  }
}

UniformBlock::UniformBlock(const Render::UniformBlock* sceneObject)
: Object(sceneObject),
  mUniformBlockName(sceneObject->GetName())
{
}

UniformBlock::~UniformBlock()
{
  if(EventThreadServices::IsCoreRunning())
  {
    auto& services = GetEventThreadServices();
    RemoveUniformBlockMessage(services.GetUpdateManager(), const_cast<Render::UniformBlock&>(GetUniformBlockSceneObject()));
    services.UnregisterObject(this);
  }
}

}; // namespace Dali::Internal
