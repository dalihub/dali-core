/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use *this file except in compliance with the License.
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
#include <dali/public-api/rendering/geometry.h> // Dali::Geometry

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/geometry-impl.h> // Dali::Internal::Geometry

namespace Dali
{
Geometry Geometry::New()
{
  Internal::GeometryPtr geometry = Internal::Geometry::New();
  return Geometry(geometry.Get());
}

Geometry::Geometry() = default;

Geometry::~Geometry() = default;

Geometry::Geometry(const Geometry& handle) = default;

Geometry Geometry::DownCast(BaseHandle handle)
{
  return Geometry(dynamic_cast<Dali::Internal::Geometry*>(handle.GetObjectPtr()));
}

Geometry& Geometry::operator=(const Geometry& handle) = default;

Geometry::Geometry(Geometry&& rhs) noexcept = default;

Geometry& Geometry::operator=(Geometry&& rhs) noexcept = default;

std::size_t Geometry::AddVertexBuffer(VertexBuffer& vertexBuffer)
{
  DALI_ASSERT_ALWAYS(vertexBuffer && "VertexBuffer is not initialized ");
  return GetImplementation(*this).AddVertexBuffer(GetImplementation(vertexBuffer));
}

std::size_t Geometry::GetNumberOfVertexBuffers() const
{
  return GetImplementation(*this).GetNumberOfVertexBuffers();
}

void Geometry::RemoveVertexBuffer(std::size_t index)
{
  GetImplementation(*this).RemoveVertexBuffer(static_cast<uint32_t>(index));
}

void Geometry::SetIndexBuffer(const uint16_t* indices, size_t count)
{
  GetImplementation(*this).SetIndexBuffer(indices, static_cast<uint32_t>(count));
}

void Geometry::SetIndexBuffer(const uint32_t* indices, size_t count)
{
  GetImplementation(*this).SetIndexBuffer(indices, static_cast<uint32_t>(count));
}

void Geometry::SetType(Type geometryType)
{
  GetImplementation(*this).SetType(geometryType);
}

Geometry::Type Geometry::GetType() const
{
  return GetImplementation(*this).GetType();
}

Geometry::Geometry(Internal::Geometry* pointer)
: BaseHandle(pointer)
{
}

} //namespace Dali
