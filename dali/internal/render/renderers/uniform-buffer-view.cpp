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
#include <dali/internal/render/renderers/uniform-buffer-view.h>

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/uniform-buffer.h>

namespace Dali::Internal::Render
{
UniformBufferView::UniformBufferView( UniformBuffer& ubo, uint32_t offset, size_t size ) :
  mUniformBuffer(&ubo),
  mOffset(offset),
  mSize (size)
{
}

UniformBufferView::~UniformBufferView() = default;

void UniformBufferView::Write(const void* data, uint32_t size, uint32_t offset)
{
  // Write into mapped buffer
  mUniformBuffer->Write( data, size, offset );
}

Graphics::Buffer* UniformBufferView::GetBuffer( uint32_t* relativeOffset )
{
  auto buffer = mUniformBuffer->GetBufferByOffset( mOffset, relativeOffset, nullptr );
  return buffer ? buffer->buffer.get() : nullptr;
}
} // Namespace Dali::Internal::Render