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
#include <dali/public-api/rendering/vertex-buffer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/vertex-buffer-impl.h> // Dali::Internal::VertexBuffer
#include <dali/public-api/object/property-map.h>              // Dali::Property::Map

namespace Dali
{
struct VertexBufferUpdateCallback::Impl
{
  Impl(CallbackBase* callback)
  : mCallback(callback)
  {
  }

  uint32_t Invoke(void* data, size_t size)
  {
    return CallbackBase::ExecuteReturn<uint32_t>(*mCallback, data, size);
  }

  std::unique_ptr<CallbackBase> mCallback;
};

VertexBufferUpdateCallback::~VertexBufferUpdateCallback() = default;

std::unique_ptr<VertexBufferUpdateCallback> VertexBufferUpdateCallback::New(CallbackBase* callbackBase)
{
  std::unique_ptr<VertexBufferUpdateCallback> retval;
  auto                                        impl = std::make_unique<Impl>(callbackBase);
  retval.reset(new VertexBufferUpdateCallback(std::move(impl)));
  return retval;
}

VertexBufferUpdateCallback::VertexBufferUpdateCallback(std::unique_ptr<VertexBufferUpdateCallback::Impl>&& impl)
: mImpl(std::move(impl))
{
}

uint32_t VertexBufferUpdateCallback::Invoke(void* data, size_t size)
{
  return mImpl->Invoke(data, size);
}

} // namespace Dali

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

VertexBuffer::VertexBuffer(VertexBuffer&& rhs) noexcept = default;

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& rhs) noexcept = default;

void VertexBuffer::SetData(const void* data, std::size_t size)
{
  GetImplementation(*this).SetData(data, static_cast<uint32_t>(size)); // only support 4,294,967,295 bytes
}

std::size_t VertexBuffer::GetSize() const
{
  return GetImplementation(*this).GetSize();
}

void VertexBuffer::SetDivisor(uint32_t divisor)
{
  GetImplementation(*this).SetDivisor(divisor);
}

uint32_t VertexBuffer::GetDivisor() const
{
  return GetImplementation(*this).GetDivisor();
}

void VertexBuffer::SetVertexBufferUpdateCallback(std::unique_ptr<VertexBufferUpdateCallback>&& updateCallback)
{
  GetImplementation(*this).SetVertexBufferUpdateCallback(*updateCallback.release());
}

VertexBuffer::VertexBuffer(Internal::VertexBuffer* pointer)
: BaseHandle(pointer)
{
}

} // namespace Dali
