/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/geometry/mesh.h>

// INTERNAL INCLUDES
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/public-api/geometry/mesh-factory.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace
{

} // namespace

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

Mesh Mesh::New( const MeshData& meshData )
{
  // Convert material handle to intrusive pointer
  Dali::Material material( meshData.GetMaterial() );
  Internal::MaterialIPtr materialPtr( &GetImplementation( material ) );

  Internal::MeshIPtr meshPtr = Internal::Mesh::New( meshData, true /* discardable */, false /* scaling not required */ );
  return Mesh( meshPtr.Get() );
}

Mesh Mesh::NewPlane(const float width, const float height, const int xSteps, const int ySteps, const Rect<float>& textureCoordinates)
{
  MeshData meshData( MeshFactory::NewPlane(width, height, xSteps, ySteps, textureCoordinates) );
  return New(meshData);
}

Mesh Mesh::DownCast( BaseHandle handle )
{
  return Mesh( dynamic_cast<Dali::Internal::Mesh*>(handle.GetObjectPtr()) );
}

void Mesh::UpdateMeshData( const MeshData& meshData )
{
  GetImplementation(*this).UpdateMeshData( meshData );
}

Mesh::Mesh(Internal::Mesh* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
