/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/uniform-buffer-view-pool.h>

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/uniform-buffer-manager.h>
#include <dali/internal/render/renderers/uniform-buffer.h>
#include <dali/internal/render/renderers/uniform-buffer-view.h>
#include <dali/graphics-api/graphics-buffer-create-info.h>
#include <dali/graphics-api/graphics-buffer.h>

namespace Dali::Internal::Render
{
namespace
{
// Default UBO page size set to 32kb
const uint32_t DEFAULT_UBO_PAGE_SIZE = 32768;
}

UniformBufferViewPool::UniformBufferViewPool( UniformBufferManager& manager, uint32_t alignment ) :
  mUboManager(manager)
{
  // Create initial UBO
  mUniformBufferStorage = mUboManager.AllocateUniformBuffer( DEFAULT_UBO_PAGE_SIZE, alignment );
  mAlignment = alignment;
  mCurrentOffset = 0;
}

UniformBufferViewPool::~UniformBufferViewPool() = default;

void UniformBufferViewPool::Rollback()
{
  // Rollback offset
  mCurrentOffset = 0;

  // turn buffer into single allocation by resizing it
  // to current size with invalidation
  auto currentSize = mUniformBufferStorage->GetSize();
  mUniformBufferStorage->Resize( currentSize ? currentSize : DEFAULT_UBO_PAGE_SIZE, true );
}

Graphics::UniquePtr<UniformBufferView> UniformBufferViewPool::CreateUniformBufferView( size_t size )
{
  // find new offset
  auto newOffset = ((mCurrentOffset + size) / mAlignment) * mAlignment;
  if(mAlignment > 1 && newOffset < mCurrentOffset + size)
  {
    newOffset += mAlignment;
  }

  // resize Ubo if needed
  if( newOffset >= mUniformBufferStorage->GetSize())
  {
    // move offset to the new buffer
    mCurrentOffset = mUniformBufferStorage->GetSize();
    newOffset = ((mCurrentOffset + size) / mAlignment) * mAlignment;

    // Adjust current offset so the view doesn't intersect multiple buffers
    auto prevSize = mUniformBufferStorage->GetSize();
    if(mCurrentOffset + size >= prevSize )
    {
      mCurrentOffset = prevSize;
      newOffset = ((mCurrentOffset + size) / mAlignment) * mAlignment;
    }
    mUniformBufferStorage->Resize( mUniformBufferStorage->GetSize()+DEFAULT_UBO_PAGE_SIZE, false );
  }

  // create buffer view from
  Graphics::UniquePtr<UniformBufferView> uboView = mUboManager.CreateUniformBufferView(mUniformBufferStorage.get(), mCurrentOffset, size);

  // adjust offset
  mCurrentOffset = newOffset;

  return uboView;
}

} // namespace Dali::Internal::Render