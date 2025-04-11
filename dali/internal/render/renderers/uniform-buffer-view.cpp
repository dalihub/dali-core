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

namespace Dali::Internal::Render
{
namespace
{
// Memory pool used to allocate new uniform buffer view. Memory used by this pool will be released when process dies
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::Render::UniformBufferView>& GetUboViewMemoryPool()
{
  static Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::Render::UniformBufferView> gUboViewMemoryPool;
  return gUboViewMemoryPool;
}
} // namespace

UniformBufferView* UniformBufferView::New(UniformBufferV2& ubo, uint32_t offset)
{
  return new(GetUboViewMemoryPool().AllocateRaw()) UniformBufferView(ubo, offset);
}

void UniformBufferView::ResetMemoryPool()
{
  GetUboViewMemoryPool().ResetMemoryPool();
}

UniformBufferView::UniformBufferView(UniformBufferV2& ubo, uint32_t offset)
: mUniformBuffer(&ubo),
  mOffset(offset)
{
}

UniformBufferView::~UniformBufferView() = default;

void UniformBufferView::operator delete(void* ptr)
{
  GetUboViewMemoryPool().Free(static_cast<UniformBufferView*>(ptr));
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
