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


#include <dali/public-api/geometry/animatable-mesh.h>
#include <dali/internal/event/modeling/animatable-mesh-impl.h>

namespace Dali
{

AnimatableMesh::AnimatableMesh()
{
}

AnimatableMesh AnimatableMesh::New(
  unsigned int numVertices,
  const AnimatableMesh::Faces& faceIndices,
  Dali::Material material )
{
  Internal::AnimatableMeshPtr internal = Internal::AnimatableMesh::New( numVertices,
                                                                        faceIndices,
                                                                        material );
  return AnimatableMesh(internal.Get());
}

AnimatableMesh AnimatableMesh::New( unsigned int numVertices,
                                    const Faces& faceIndices )
{

  Internal::AnimatableMeshPtr internal = Internal::AnimatableMesh::New( numVertices, faceIndices );
  return AnimatableMesh(internal.Get());
}

AnimatableMesh AnimatableMesh::DownCast( BaseHandle handle )
{
  return AnimatableMesh( dynamic_cast<Dali::Internal::AnimatableMesh*>(handle.GetObjectPtr()) );
}

AnimatableMesh::~AnimatableMesh()
{
}

AnimatableMesh::AnimatableMesh(const AnimatableMesh& handle)
: Constrainable(handle)
{
}

AnimatableMesh& AnimatableMesh::operator=(const AnimatableMesh& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

unsigned int AnimatableMesh::GetNumberOfVertices() const
{
  return GetImplementation(*this).GetNumberOfVertices();
}

AnimatableVertex AnimatableMesh::operator[](const unsigned int index) const
{
  DALI_ASSERT_ALWAYS( index < GetNumberOfVertices() && "Index out of bounds" );

  return AnimatableVertex(index, *this);
}

Property::Index AnimatableMesh::GetPropertyIndex(
  unsigned int index,
  Property::Index property ) const
{
  DALI_ASSERT_ALWAYS( index < GetNumberOfVertices() && "Index out of bounds" );

  return GetImplementation(*this).GetVertexPropertyIndex( index, property );
}

Property AnimatableMesh::GetVertexProperty( unsigned int vertex, Property::Index property)
{
  return Property( *this, GetPropertyIndex( vertex, property ) );
}


AnimatableMesh::AnimatableMesh( Internal::AnimatableMesh* internal )
: Constrainable(internal)
{
}

} // Dali
