/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/geometry.h>  // Dali::Geometry

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/geometry-impl.h> // Dali::Internal::Geometry

namespace Dali
{

Geometry Geometry::New()
{
  Internal::GeometryPtr geometry = Internal::Geometry::New();
  return Geometry( geometry.Get() );
}

Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

Geometry::Geometry( const Geometry& handle )
: Handle( handle )
{
}

Geometry Geometry::DownCast( BaseHandle handle )
{
  return Geometry( dynamic_cast<Dali::Internal::Geometry*>(handle.GetObjectPtr()));
}

Geometry& Geometry::operator=( const Geometry& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

std::size_t Geometry::AddVertexBuffer( PropertyBuffer& vertexBuffer )
{
  DALI_ASSERT_ALWAYS( vertexBuffer && "VertexBuffer is not initialized ");
  return GetImplementation(*this).AddVertexBuffer( GetImplementation( vertexBuffer ) );
}

std::size_t Geometry::GetNumberOfVertexBuffers() const
{
  return GetImplementation(*this).GetNumberOfVertexBuffers();
}

void Geometry::RemoveVertexBuffer( std::size_t index )
{
  GetImplementation(*this).RemoveVertexBuffer( index );
}

void Geometry::SetIndexBuffer( PropertyBuffer& indexBuffer )
{
  DALI_ASSERT_ALWAYS( indexBuffer && "indexBuffer is not initialized ");
  GetImplementation(*this).SetIndexBuffer( GetImplementation( indexBuffer ) );
}

void Geometry::SetGeometryType( GeometryType geometryType )
{
  GetImplementation(*this).SetGeometryType( geometryType );
}

Geometry::GeometryType Geometry::GetGeometryType() const
{
  return GetImplementation(*this).GetGeometryType();
}

void Geometry::SetRequiresDepthTesting( bool requiresDepthTest )
{
  GetImplementation(*this).SetRequiresDepthTesting( requiresDepthTest );
}

bool Geometry::GetRequiresDepthTesting() const
{
  return GetImplementation(*this).GetRequiresDepthTesting();
}

Geometry::Geometry( Internal::Geometry* pointer )
: Handle( pointer )
{
}

} //namespace Dali
