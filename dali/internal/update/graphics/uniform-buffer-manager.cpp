/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/graphics/uniform-buffer-manager.h>

// INTERNAL INCLUDES
#include <dali/internal/update/graphics/uniform-buffer-view.h>
#include <dali/internal/update/graphics/uniform-buffer.h>

#include <dali/graphics-api/graphics-buffer-create-info.h>
#include <dali/graphics-api/graphics-buffer.h>

#include <cstring>
#include <memory>

namespace
{
uint32_t CPU_MEMORY_ALIGNMENT{256};
}

namespace Dali::Internal::SceneGraph
{
UniformBufferManager::UniformBufferManager(Dali::Graphics::Controller* controller)
: mController(controller)
{
}

UniformBufferManager::~UniformBufferManager() = default;

void UniformBufferManager::SetCurrentSceneRenderInfo(/*SceneGraph::Scene* scene, */bool offscreen)
{
  mCurrentSceneOffscreen = offscreen;
  //mCurrentScene          = scene;

  // Only works if RegisterScene has been called...
  mCurrentUBOSet = FindSetForScene(/*scene*/);
}

Graphics::UniquePtr<UniformBufferView> UniformBufferManager::CreateUniformBufferView(uint32_t size, bool emulated)
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
  auto retval = Graphics::UniquePtr<UniformBufferView>(new UniformBufferView(*ubo.get(), offset, size));

  // make sure new offset will meet alignment requirements
  uint32_t alignedSize = ubo->AlignSize(size);
  ubo->IncrementOffsetBy(alignedSize);
  return retval;
}

void UniformBufferManager::RegisterScene(/*SceneGraph::Scene* scene*/)
{
  if(mUBO == nullptr)
  {
    // Create new UBO set
    mUBO = std::make_unique<UBOSet>();

    uint32_t cpuAlignment = GetUniformBlockAlignment(true);
    uint32_t gpuAlignment = GetUniformBlockAlignment(false);

    // Create all buffers per scene
    mUBO->cpuBufferOnScreen  = UniformBufferV2::New(mController, true, cpuAlignment);
    mUBO->gpuBufferOnScreen  = UniformBufferV2::New(mController, false, gpuAlignment);
    mUBO->cpuBufferOffScreen = UniformBufferV2::New(mController, true, cpuAlignment);
    mUBO->gpuBufferOffScreen = UniformBufferV2::New(mController, false, gpuAlignment);
  }
}

void UniformBufferManager::UnregisterScene(/*SceneGraph::Scene* scene*/)
{
  mUBO.reset();
}

UniformBufferV2* UniformBufferManager::GetUniformBufferForScene(/*SceneGraph::Scene* scene, */bool offscreen, bool emulated)
{
  auto* uboSet = GetUBOSetForScene(/*scene*/);
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
    /* Getting the block alignment. This is a little complicated for the following reasons:
     *
     * On GL, this is as simple as calling glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT).
     *
     * On Vulkan, this needs a test buffer to be allocated with the right usage flags,
     * and GetBufferMemoryRequirements will return alignment that would then be common
     * to all similar buffers.
     *
     * We have generally copied the Vulkan api into our GraphicsAPI, with some changes.
     * So, we have to use Graphics::GetBufferMemoryRequirements() which we call on a test buffer.
     *
     * Note, this doesn't change during the graphics context lifetime, so can be cached.
     */

    Graphics::BufferPropertiesFlags flags = 0u;

    const uint32_t TEST_BUFFER_ALLOCATION{256};
    auto           createInfo = Graphics::BufferCreateInfo()
                        .SetSize(TEST_BUFFER_ALLOCATION)
                        .SetBufferPropertiesFlags(flags)
                        .SetUsage(0u | Graphics::BufferUsage::UNIFORM_BUFFER);

    Graphics::UniquePtr<Graphics::Buffer> graphicsBuffer = mController->CreateBuffer(createInfo, nullptr);
    Graphics::MemoryRequirements          requirements   = mController->GetBufferMemoryRequirements(*graphicsBuffer.get());
    mCachedUniformBlockAlignment                         = requirements.alignment;
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
