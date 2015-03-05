/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 */

#include "scene-graph-geometry.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Geometry::Geometry()
: mGeometryType(Dali::Geometry::TRIANGLES)
{
}

Geometry::~Geometry()
{
}

void Geometry::AddVertexBuffer( PropertyBuffer vertexBuffer )
{
}

void Geometry::RemoveVertexBuffer( PropertyBuffer vertexBuffer )
{
}

void Geometry::SetIndexBuffer( PropertyBuffer indexBuffer )
{
}

void Geometry::SetGeometryType( Geometry::GeometryType geometryType )
{
}

const Geometry::VertexBuffers& Geometry::GetVertexBuffers()
{
  return mVertexBuffers;
}

const PropertyBuffer& Geometry::GetIndexBuffer()
{
  return mIndexBuffer;
}

Geometry::GeometryType Geometry::GetGeometryType( )
{
  return mGeometryType;
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
