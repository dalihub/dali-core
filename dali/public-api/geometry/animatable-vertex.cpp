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

#include <dali/public-api/geometry/animatable-vertex.h>
#include <dali/public-api/geometry/animatable-mesh.h>
#include <dali/internal/event/modeling/animatable-mesh-impl.h>

namespace Dali
{

AnimatableVertex::AnimatableVertex( unsigned int vertex, AnimatableMesh mesh)
  : mVertex(vertex),
    mMesh(GetImplementation(mesh))
{
  DALI_ASSERT_ALWAYS( mesh && "Mesh handle is empty" );
  DALI_ASSERT_ALWAYS( vertex < mesh.GetNumberOfVertices() && "vertex is out of bounds" );
}

AnimatableVertex::~AnimatableVertex()
{
}

void AnimatableVertex::SetPosition(const Vector3& position)
{
  mMesh.SetProperty( mMesh.GetVertexPropertyIndex(mVertex, POSITION), Property::Value(position) );
}

void AnimatableVertex::SetColor(const Vector4& color)
{
  mMesh.SetProperty( mMesh.GetVertexPropertyIndex(mVertex, COLOR), Property::Value(color) );
}

void AnimatableVertex::SetTextureCoords(const Vector2& textureCoords)
{
  mMesh.SetProperty( mMesh.GetVertexPropertyIndex(mVertex, TEXTURE_COORDS), Property::Value(textureCoords) );
}

Vector3 AnimatableVertex::GetCurrentPosition()
{
  Vector3 position;
  mMesh.GetProperty( mMesh.GetVertexPropertyIndex(mVertex, POSITION) ).Get(position);
  return position;
}

Vector4 AnimatableVertex::GetCurrentColor()
{
  Vector4 color;
  mMesh.GetProperty( mMesh.GetVertexPropertyIndex(mVertex, COLOR) ).Get(color);
  return color;
}

Vector2 AnimatableVertex::GetCurrentTextureCoords()
{
  Vector2 textureCoords;
  mMesh.GetProperty( mMesh.GetVertexPropertyIndex(mVertex, TEXTURE_COORDS) ).Get(textureCoords);
  return textureCoords;
}

}//Dali
