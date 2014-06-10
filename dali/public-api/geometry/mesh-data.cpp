//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// Class header
#include <dali/public-api/geometry/mesh-data.h>

// Internal headers
#include <dali/public-api/math/matrix.h>
#include <dali/integration-api/debug.h>

using namespace std;

namespace Dali
{

namespace // unnamed namespace
{
static const float kBoundsDefault = 1e10f;
} // unnamed namespace

using Dali::Vector4;


MeshData::MeshData( )
: mGeometryType(TRIANGLES),
  mUseTextureCoords(false),
  mUseNormals(false),
  mUseColor(false),
  mMin( kBoundsDefault,  kBoundsDefault,  kBoundsDefault, 0.0f),
  mMax(-kBoundsDefault, -kBoundsDefault, -kBoundsDefault, 0.0f)
{
}

MeshData::MeshData( const MeshData& meshData )
: mVertices( meshData.mVertices ),
  mFaces( meshData.mFaces ),
  mGeometryType( meshData.mGeometryType ),
  mUseTextureCoords( meshData.mUseTextureCoords ),
  mUseNormals( meshData.mUseNormals ),
  mUseColor( meshData.mUseColor ),
  mBones( meshData.mBones ),
  mMaterial( meshData.mMaterial ),
  mMin( meshData.mMin ),
  mMax( meshData.mMax )
{
}

MeshData& MeshData::operator=(const MeshData& rhs)
{
  mVertices = rhs.mVertices;
  mFaces = rhs.mFaces;
  mGeometryType = rhs.mGeometryType;
  mUseTextureCoords = rhs.mUseTextureCoords;
  mUseNormals = rhs.mUseNormals;
  mUseColor = rhs.mUseColor;
  mBones = rhs.mBones;
  mMaterial = rhs.mMaterial;
  mMin = rhs.mMin;
  mMax = rhs.mMax;

  return *this;
}

MeshData::~MeshData()
{
}

void MeshData::SetData(
    const VertexContainer& vertices,
    const FaceIndices&     faceIndices,
    const BoneContainer&   bones,
    Material               material )
{
  DALI_ASSERT_ALWAYS( !vertices.empty() && "VertexContainer is empty" );
  DALI_ASSERT_ALWAYS( !faceIndices.empty() && "FaceIndices is empty" );
  DALI_ASSERT_ALWAYS( material && "Material handle is empty" );

  mGeometryType = TRIANGLES;
  mVertices = vertices;
  mFaces = faceIndices;
  mMaterial = material;
  mBones = bones;
}

void MeshData::SetLineData(
    const VertexContainer& vertices,
    const FaceIndices&     lineIndices,
    Material               material )
{
  DALI_ASSERT_ALWAYS( !vertices.empty() && "VertexContainer is empty" );
  DALI_ASSERT_ALWAYS( !lineIndices.empty() && "FaceIndices is empty" );
  DALI_ASSERT_ALWAYS( material && "Material handle is empty" );

  mGeometryType = LINES;
  mVertices = vertices;
  mFaces = lineIndices;
  mMaterial = material;
}

void MeshData::SetPointData(
    const VertexContainer& vertices,
    Material               material )
{
  DALI_ASSERT_ALWAYS( !vertices.empty() && "VertexContainer is empty" );
  DALI_ASSERT_ALWAYS( material && "Material handle is empty" );

  mGeometryType = POINTS;
  mVertices = vertices;
  mMaterial = material;
}

void MeshData::SetVertices( const VertexContainer& vertices )
{
  DALI_ASSERT_ALWAYS( !vertices.empty() && "VertexContainer is empty" );

  mVertices = vertices;
}

void MeshData::SetFaceIndices( const FaceIndices& faceIndices )
{
  DALI_ASSERT_ALWAYS( !faceIndices.empty() && "FaceIndices is empty" );

  mFaces = faceIndices;
}

void MeshData::AddToBoundingVolume(Vector4& min, Vector4& max, const Dali::Matrix& transform)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  for (VertexConstIter iter = mVertices.begin(); iter != mVertices.end(); ++iter)
  {
    const Vertex& vertex = *iter;
    Vector4 point = Vector4(vertex.x, vertex.y, vertex.z, 0.0f);

    mMin = Min(mMin, point);
    mMax = Max(mMax, point);
  }
  // mMin/mMax are accessed through public API, so have chosen to set w to zero
  // ( They should really be Vector3's )
  mMin.w=0.0f;
  mMax.w=0.0f;

  // For the matrix multiplication below to work correctly, the w needs to be 1.
  Vector4 tmpMin(mMin);
  Vector4 tmpMax(mMax);
  tmpMin.w = 1.0f;
  tmpMax.w = 1.0f;

  min = Min(min, transform * tmpMin);
  max = Max(max, transform * tmpMax);

  // Ensure the bounding volume out parameters also have w=0. (They should also
  // be Vector3's )
  min.w = 0.0f;
  max.w = 0.0f;
}

MeshData::VertexGeometryType MeshData::GetVertexGeometryType() const
{
  return mGeometryType;
}

size_t MeshData::GetVertexCount() const
{
  return mVertices.size();
}

const MeshData::VertexContainer& MeshData::GetVertices() const
{
  return mVertices;
}

size_t MeshData::GetFaceCount() const
{
  size_t faceCount = 0;

  switch( mGeometryType )
  {
    case POINTS:
      faceCount = mVertices.size();
      break;
    case LINES:
      faceCount = mFaces.size() / 2;
      break;
    case TRIANGLES:
      faceCount = mFaces.size() / 3;
      break;
  }
  return faceCount;
}

const MeshData::FaceIndices& MeshData::GetFaces() const
{
  return mFaces;
}

void MeshData::SetHasTextureCoords(bool hasTexCoords)
{
  mUseTextureCoords = hasTexCoords;
}

bool MeshData::HasTextureCoords() const
{
  return mUseTextureCoords;
}

void MeshData::SetHasNormals(bool hasNormals)
{
  mUseNormals = hasNormals;
  if( hasNormals )
  {
    // Enforce mutual exclusivity
    mUseColor = false;
  }
}

bool MeshData::HasNormals() const
{
  return mUseNormals;
}

void MeshData::SetHasColor(bool hasColor)
{
  mUseColor = hasColor;
  if( hasColor )
  {
    // Enforce mutual exclusivity
    mUseNormals = false;
  }
}

bool MeshData::HasColor() const
{
  return mUseColor;
}

Material MeshData::GetMaterial() const
{
  return mMaterial;
}

void MeshData::SetMaterial(Material material)
{
  mMaterial = material;
}

size_t MeshData::GetBoneCount() const
{
  return mBones.size();
}

bool MeshData::HasBones() const
{
  return mBones.size() > 0;
}

const BoneContainer& MeshData::GetBones() const
{
  return mBones;
}

const Vector4& MeshData::GetBoundingBoxMin() const
{
  return mMin;
}

void MeshData::SetBoundingBoxMin(const Vector4& bounds)
{
  mMin = bounds;
}

const Vector4& MeshData::GetBoundingBoxMax() const
{
  return mMax;
}


void MeshData::SetBoundingBoxMax(const Vector4& bounds)
{
  mMax = bounds;
}

} // namespace Dali

