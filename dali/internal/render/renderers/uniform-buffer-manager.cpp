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
 *
 */

// CLASS HEADER
#include <dali/internal/render/renderers/uniform-buffer-manager.h>

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/uniform-buffer-view.h>
#include <dali/internal/render/renderers/uniform-buffer.h>

#include <dali/internal/update/common/scene-graph-scene.h>

#include <dali/graphics-api/graphics-buffer-create-info.h>
#include <dali/graphics-api/graphics-buffer.h>

#include <cstring>
#include <memory>

namespace
{
uint32_t CPU_MEMORY_ALIGNMENT{256};
}

namespace Dali::Internal::Render
{
UniformBufferManager::UniformBufferManager(Dali::Graphics::Controller* controller)
: mController(controller)
{
}

UniformBufferManager::~UniformBufferManager() = default;

void UniformBufferManager::ContextDestroyed()
{
  mUBOMap.clear();
  mCurrentUBOSet = nullptr;
  mCurrentScene  = nullptr;
}

void UniformBufferManager::SetCurrentSceneRenderInfo(SceneGraph::Scene* scene, bool offscreen)
{
  mCurrentSceneOffscreen = offscreen;
  mCurrentScene          = scene;

  // Only works if RegisterScene has been called...
  mCurrentUBOSet = FindSetForScene(scene);
}

Graphics::UniquePtr<UniformBufferView> UniformBufferManager::CreateUniformBufferView(UniformBufferView*& oldView, uint32_t size, bool emulated)
{
  // Allocate offset of given UBO (allocation strategy may reuse memory)

  DALI_ASSERT_DEBUG(mCurrentUBOSet && "UBO set should exist when creating view");
  if(!mCurrentUBOSet)
  {
    return Graphics::UniquePtr<UniformBufferView>(nullptr);
  }

  UBOSet::BufferType                    bufferType = UBOSet::GetBufferType(mCurrentSceneOffscreen, emulated);
  Graphics::UniquePtr<UniformBufferV2>& ubo        = mCurrentUBOSet->GetBuffer(bufferType);

  // Use current offset and increment it after
  auto offset = ubo->GetCurrentOffset();
  auto retval = Graphics::UniquePtr<UniformBufferView>(UniformBufferView::TryRecycle(oldView, *ubo.get(), offset));

  ubo->IncrementOffsetBy(size);
  return retval;
}

void UniformBufferManager::RegisterScene(SceneGraph::Scene* scene)
{
  auto iter = mUBOMap.find(scene);
  if(iter == mUBOMap.end())
  {
    // Create new UBO set
    UBOSet uboSet;

    uint32_t cpuAlignment = GetUniformBlockAlignment(true);
    uint32_t gpuAlignment = GetUniformBlockAlignment(false);

    // Create all buffers per scene
    uboSet.cpuBufferOnScreen  = Render::UniformBufferV2::New(mController, true, cpuAlignment);
    uboSet.gpuBufferOnScreen  = Render::UniformBufferV2::New(mController, false, gpuAlignment);
    uboSet.cpuBufferOffScreen = Render::UniformBufferV2::New(mController, true, cpuAlignment);
    uboSet.gpuBufferOffScreen = Render::UniformBufferV2::New(mController, false, gpuAlignment);

    mUBOMap.emplace(std::pair(scene, std::move(uboSet)));
  }
}

void UniformBufferManager::UnregisterScene(SceneGraph::Scene* scene)
{
  auto iter = mUBOMap.find(scene);
  if(iter != mUBOMap.end())
  {
    mUBOMap.erase(iter);
  }
}

UniformBufferV2* UniformBufferManager::GetUniformBufferForScene(SceneGraph::Scene* scene, bool offscreen, bool emulated)
{
  auto* uboSet = GetUBOSetForScene(scene);
  if(!uboSet)
  {
    return nullptr;
  }
  DALI_ASSERT_DEBUG(mCurrentScene == scene && "Scene should match current cache");
  return uboSet->GetBuffer(UBOSet::GetBufferType(offscreen, emulated)).get();
}

void UniformBufferManager::Rollback(SceneGraph::Scene* scene, bool offscreen)
{
  auto* uboSet = GetUBOSetForScene(scene);
  if(uboSet)
  {
    if(offscreen)
    {
      uboSet->cpuBufferOffScreen->Rollback();
      uboSet->gpuBufferOffScreen->Rollback();
    }
    else
    {
      uboSet->cpuBufferOnScreen->Rollback();
      uboSet->gpuBufferOnScreen->Rollback();
    }
  }
}

void UniformBufferManager::Flush(SceneGraph::Scene* scene, bool offscreen)
{
  auto* uboSet = GetUBOSetForScene(scene);
  if(uboSet)
  {
    if(offscreen)
    {
      uboSet->cpuBufferOffScreen->Flush();
      uboSet->gpuBufferOffScreen->Flush();
    }
    else
    {
      uboSet->cpuBufferOnScreen->Flush();
      uboSet->gpuBufferOnScreen->Flush();
    }
  }
}

uint32_t UniformBufferManager::GetUniformBlockAlignment(bool emulated)
{
  if(emulated)
  {
    return CPU_MEMORY_ALIGNMENT;
  }

  if(mCachedUniformBlockAlignment == 0)
  {
    mCachedUniformBlockAlignment = mController->GetDeviceLimitation(Graphics::DeviceCapability::MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT);
  }

  return mCachedUniformBlockAlignment;
}

UniformBufferManager::UBOSet* UniformBufferManager::GetUBOSetForScene(SceneGraph::Scene* scene)
{
  if(mCurrentScene == scene)
  {
    return mCurrentUBOSet;
  }
  mCurrentUBOSet = FindSetForScene(scene);
  return mCurrentUBOSet;
}

// Find efficiently without caching
UniformBufferManager::UBOSet* UniformBufferManager::FindSetForScene(SceneGraph::Scene* scene)
{
  if(mUBOMap.size() == 1 && mUBOMap.begin()->first == scene)
  {
    return &mUBOMap.begin()->second;
  }
  else
  {
    auto iter = mUBOMap.find(scene);
    if(iter != mUBOMap.end())
    {
      return &iter->second;
    }
  }
  return nullptr;
}

UniformBufferManager::UBOSet::UBOSet(UniformBufferManager::UBOSet&& rhs)
{
  cpuBufferOnScreen.swap(rhs.cpuBufferOnScreen);
  gpuBufferOnScreen.swap(rhs.gpuBufferOnScreen);
  cpuBufferOffScreen.swap(rhs.cpuBufferOffScreen);
  gpuBufferOffScreen.swap(rhs.gpuBufferOffScreen);
}

} // namespace Dali::Internal::Render
