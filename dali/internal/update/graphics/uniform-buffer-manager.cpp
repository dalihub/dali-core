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

void UniformBufferManager::SetCurrentSceneRenderInfo(const SceneGraph::Scene* scene, bool offscreen)
{
  mCurrentSceneOffscreen = offscreen;
}

Graphics::UniquePtr<UniformBufferView> UniformBufferManager::CreateUniformBufferView(uint32_t size, bool emulated)
{
  // Allocate offset of given UBO (allocation strategy may reuse memory)
  if(!mUBOSet)
  {
    return Graphics::UniquePtr<UniformBufferView>(nullptr);
  }

  UBOSet::BufferType                    bufferType = UBOSet::GetBufferType(mCurrentSceneOffscreen, emulated);
  Graphics::UniquePtr<UniformBufferV2>& ubo        = mUBOSet->GetBuffer(bufferType);

  // Use current offset and increment it after
  auto offset = ubo->GetCurrentOffset();
  auto retval = Graphics::UniquePtr<UniformBufferView>(new UniformBufferView(*ubo.get(), offset, size));

  // make sure new offset will meet alignment requirements
  uint32_t alignedSize = ubo->AlignSize(size);
  ubo->IncrementOffsetBy(alignedSize);
  return retval;
}

void UniformBufferManager::RegisterScene(const SceneGraph::Scene* scene)
{
  if(mUBOSet == nullptr)
  {
    // Create new UBO set
    mUBOSet = std::make_unique<UBOSet>();

    uint32_t cpuAlignment = GetUniformBlockAlignment(true);
    uint32_t gpuAlignment = GetUniformBlockAlignment(false);

    // Create all buffers per scene
    mUBOSet->cpuBufferOnScreen  = UniformBufferV2::New(mController, true, cpuAlignment);
    mUBOSet->gpuBufferOnScreen  = UniformBufferV2::New(mController, false, gpuAlignment);
    mUBOSet->cpuBufferOffScreen = UniformBufferV2::New(mController, true, cpuAlignment);
    mUBOSet->gpuBufferOffScreen = UniformBufferV2::New(mController, false, gpuAlignment);
  }
}

void UniformBufferManager::UnregisterScene(const SceneGraph::Scene* scene)
{
  mUBOSet.reset();
}

UniformBufferV2* UniformBufferManager::GetUniformBufferForScene(const SceneGraph::Scene* scene, bool offscreen, bool emulated)
{
  return mUBOSet->GetBuffer(UBOSet::GetBufferType(offscreen, emulated)).get();
}

void UniformBufferManager::Rollback(const SceneGraph::Scene* scene, bool offscreen)
{
  if(offscreen)
  {
    mUBOSet->cpuBufferOffScreen->Rollback();
    mUBOSet->gpuBufferOffScreen->Rollback();
  }
  else
  {
    mUBOSet->cpuBufferOnScreen->Rollback();
    mUBOSet->gpuBufferOnScreen->Rollback();
  }
}

void UniformBufferManager::Flush(const SceneGraph::Scene* scene, bool offscreen)
{
  if(offscreen)
  {
    mUBOSet->cpuBufferOffScreen->Flush();
    mUBOSet->gpuBufferOffScreen->Flush();
  }
  else
  {
    mUBOSet->cpuBufferOnScreen->Flush();
    mUBOSet->gpuBufferOnScreen->Flush();
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


UniformBufferManager::UBOSet::UBOSet(UniformBufferManager::UBOSet&& rhs)
 noexcept {
  cpuBufferOnScreen.swap(rhs.cpuBufferOnScreen);
  gpuBufferOnScreen.swap(rhs.gpuBufferOnScreen);
  cpuBufferOffScreen.swap(rhs.cpuBufferOffScreen);
  gpuBufferOffScreen.swap(rhs.gpuBufferOffScreen);
}

} // namespace Dali::Internal::Render
