/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/uniform-buffer-view-pool.h>
#include <dali/internal/render/renderers/uniform-buffer-view.h>
#include <dali/internal/render/renderers/uniform-buffer.h>

#include <dali/graphics-api/graphics-buffer-create-info.h>
#include <dali/graphics-api/graphics-buffer.h>

#include <cstring>
#include <memory>

namespace Dali::Internal::Render
{
UniformBufferManager::UniformBufferManager(Dali::Graphics::Controller* controller)
: mController(controller)
{
}

UniformBufferManager::~UniformBufferManager() = default;

Graphics::UniquePtr<UniformBuffer> UniformBufferManager::AllocateUniformBuffer(uint32_t size, uint32_t alignment)
{
  // TODO : Current code only assume CPU_ALLOCATED uniform buffer now
  return Graphics::UniquePtr<UniformBuffer>(
    new UniformBuffer(mController,
                      size,
                      alignment,
                      Dali::Graphics::BufferUsageFlags{0u} | Dali::Graphics::BufferUsage::TRANSFER_DST | Dali::Graphics::BufferUsage::UNIFORM_BUFFER,
                      Dali::Graphics::BufferPropertiesFlags{0u} | Dali::Graphics::BufferPropertiesFlagBit::CPU_ALLOCATED));
}

Graphics::UniquePtr<UniformBufferView> UniformBufferManager::CreateUniformBufferView(UniformBuffer* uniformBuffer, uint32_t offset, uint32_t size)
{
  // Allocate offset of given UBO (allocation strategy may reuse memory)
  return Graphics::UniquePtr<UniformBufferView>(new UniformBufferView(*uniformBuffer, offset, size));
}

Graphics::UniquePtr<UniformBufferViewPool> UniformBufferManager::CreateUniformBufferViewPool()
{
  return Graphics::UniquePtr<UniformBufferViewPool>(
    new UniformBufferViewPool(*this, 1));
}

[[nodiscard]] UniformBufferViewPool* UniformBufferManager::GetUniformBufferViewPool(uint32_t bufferIndex)
{
  if(!mUniformBufferPoolStorage[bufferIndex])
  {
    // create new uniform buffer view pool with default (initial) capacity
    mUniformBufferPoolStorage[bufferIndex] = CreateUniformBufferViewPool();
  }
  return mUniformBufferPoolStorage[bufferIndex].get();
}

void UniformBufferManager::ReadyToLockUniformBuffer(uint32_t bufferIndex)
{
  GetUniformBufferViewPool(bufferIndex)->ReadyToLockUniformBuffer();
}

void UniformBufferManager::UnlockUniformBuffer(uint32_t bufferIndex)
{
  GetUniformBufferViewPool(bufferIndex)->UnlockUniformBuffer();
}

} // namespace Dali::Internal::Render
