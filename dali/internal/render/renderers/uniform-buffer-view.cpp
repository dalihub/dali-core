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
#include <dali/internal/render/renderers/uniform-buffer-view.h>

// INTERNAL INCLUDES
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/renderers/uniform-buffer.h>
#include <dali/internal/update/common/scene-graph-memory-pool-collection.h>

namespace Dali::Internal::Render
{
namespace
{

Dali::Internal::SceneGraph::MemoryPoolCollection*                                 gMemoryPoolCollection = nullptr;
static constexpr Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType gMemoryPoolType       = Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType::RENDER_UBO_VIEW;
} // namespace

UniformBufferView* UniformBufferView::New(UniformBufferV2& ubo, uint32_t offset)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "UniformBufferView::RegisterMemoryPoolCollection not called!");
  return new(gMemoryPoolCollection->AllocateRaw(gMemoryPoolType)) UniformBufferView(ubo, offset);
}

UniformBufferView* UniformBufferView::TryRecycle(UniformBufferView*& oldView, UniformBufferV2& ubo, uint32_t offset)
{
  auto* ptr = oldView;
  if(ptr)
  {
    oldView             = nullptr;
    ptr->mUniformBuffer = &ubo;
    ptr->mOffset        = offset;
    return ptr;
  }
  return UniformBufferView::New(ubo, offset);
}

void UniformBufferView::RegisterMemoryPoolCollection(SceneGraph::MemoryPoolCollection& memoryPoolCollection)
{
  gMemoryPoolCollection = &memoryPoolCollection;
}

void UniformBufferView::UnregisterMemoryPoolCollection()
{
  gMemoryPoolCollection = nullptr;
}

UniformBufferView::UniformBufferView(UniformBufferV2& ubo, uint32_t offset)
: mUniformBuffer(&ubo),
  mOffset(offset)
{
}

UniformBufferView::~UniformBufferView() = default;

void UniformBufferView::operator delete(void* ptr)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "UniformBufferView::RegisterMemoryPoolCollection not called!");
  gMemoryPoolCollection->Free(gMemoryPoolType, ptr);
}

void UniformBufferView::Write(const void* data, uint32_t size, uint32_t offset)
{
  // Write into mapped buffer
  mUniformBuffer->Write(data, size, offset + mOffset);
}

Graphics::Buffer* UniformBufferView::GetBuffer() const
{
  return mUniformBuffer->GetGraphicsBuffer();
}

} // Namespace Dali::Internal::Render
