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

#include <dali/internal/update/modeling/internal-mesh-data.h>

namespace Dali
{

namespace Internal
{

MeshData::MeshData( const Dali::MeshData& meshData, bool discardable, bool scalingRequired )
: mVertices( meshData.GetVertices() ),
  mFaces( meshData.GetFaces() ),
  mBones( meshData.GetBones() ),
  mNumberOfVertices( mVertices.size() ),
  mNumberOfFaceIndices( mFaces.size() ),
  mGeometryType( meshData.GetVertexGeometryType() ),
  mHasNormals( meshData.HasNormals() ),
  mHasColor( meshData.HasColor() ),
  mDiscardable( discardable ),
  mDiscarded( false ),
  mScalingRequired( scalingRequired )
{
}

size_t MeshData::GetVertexCount() const
{
  return mNumberOfVertices;
}

size_t MeshData::GetFaceCount() const
{
  size_t faceCount = 0;

  switch( mGeometryType )
  {
    case Dali::MeshData::POINTS:
      faceCount = mNumberOfVertices;
      break;
    case Dali::MeshData::LINES:
      faceCount = mNumberOfFaceIndices / 2;
      break;
    case Dali::MeshData::TRIANGLES:
      faceCount = mNumberOfFaceIndices / 3;
      break;
  }
  return faceCount;
}

size_t MeshData::GetFaceIndexCount() const
{
  return mNumberOfFaceIndices;
}

size_t MeshData::GetBoneCount() const
{
  return mBones.size();
}

Dali::MeshData::VertexGeometryType MeshData::GetVertexGeometryType() const
{
  return mGeometryType;
}

const MeshData::VertexContainer& MeshData::GetVertices() const
{
  DALI_ASSERT_DEBUG( !mVertices.empty() );
  return mVertices;
}

MeshData::VertexContainer& MeshData::GetVertices()
{
  DALI_ASSERT_DEBUG( !mVertices.empty() );
  return mVertices;
}

void MeshData::SetVertices( const MeshData::VertexContainer& vertices )
{
  mVertices = vertices;
  mNumberOfVertices = vertices.size();
}

const MeshData::FaceIndices& MeshData::GetFaces() const
{
  DALI_ASSERT_DEBUG( !mFaces.empty() );
  return mFaces;
}

const Dali::BoneContainer& MeshData::GetBones() const
{
  return mBones;
}

bool MeshData::HasNormals() const
{
  return mHasNormals;
}

bool MeshData::HasColor() const
{
  return mHasColor;
}

void MeshData::Discard()
{
  if (mDiscardable)
  {
    DALI_ASSERT_DEBUG( !mDiscarded );
    VertexContainer().swap(mVertices);  // this will enforce releasing the memory
    FaceIndices().swap(mFaces);         // this will enforce releasing the memory
    mDiscarded = true;
  }
}

bool MeshData::IsScalingRequired() const
{
  return mScalingRequired;
}

} // namespace Internal

} // namespace Dali
