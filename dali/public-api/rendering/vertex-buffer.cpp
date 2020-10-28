/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/vertex-buffer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/vertex-buffer-impl.h> // Dali::Internal::VertexBuffer
#include <dali/public-api/object/property-map.h>              // Dali::Property::Map

namespace Dali
{
VertexBuffer VertexBuffer::New(Dali::Property::Map& bufferFormat)
{
  Internal::VertexBufferPtr vertexBuffer = Internal::VertexBuffer::New(bufferFormat);

  return VertexBuffer(vertexBuffer.Get());
}

VertexBuffer::VertexBuffer() = default;

VertexBuffer::~VertexBuffer() = default;

VertexBuffer::VertexBuffer(const VertexBuffer& handle) = default;

VertexBuffer VertexBuffer::DownCast(BaseHandle handle)
{
  return VertexBuffer(dynamic_cast<Dali::Internal::VertexBuffer*>(handle.GetObjectPtr()));
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& handle) = default;

VertexBuffer::VertexBuffer(VertexBuffer&& rhs) = default;

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& rhs) = default;

void VertexBuffer::SetData(const void* data, std::size_t size)
{
  GetImplementation(*this).SetData(data, static_cast<uint32_t>(size)); // only support 4,294,967,295 bytes
}

std::size_t VertexBuffer::GetSize() const
{
  return GetImplementation(*this).GetSize();
}

VertexBuffer::VertexBuffer(Internal::VertexBuffer* pointer)
: BaseHandle(pointer)
{
}

} // namespace Dali
