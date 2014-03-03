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

// CLASS HEADER
#include <dali/public-api/geometry/mesh.h>

// INTERNAL INCLUDES
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace
{

Vector3 CalculateNormal(const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
  // Calculate normal...
  const Vector3 e0 = v1 - v0;
  const Vector3 e1 = v2 - v1;
  Vector3 normal = e0.Cross(e1);
  normal.Normalize();
  return normal;
}

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
  DALI_ASSERT_DEBUG( xSteps > 1 && ySteps > 1 );

  const int vertexCount = xSteps * ySteps;

  // vertices
  MeshData::VertexContainer vertices(vertexCount);
  const float xSpacing = width / ( xSteps - 1 );
  const float ySpacing = height / (ySteps - 1 );
  const float xOffset = -xSpacing * (0.5f * xSteps) + (0.5f * xSpacing);      // origin at (width / 2, height / 2)
  const float yOffset = -ySpacing * (0.5f * ySteps) + (0.5f * ySpacing);
  const float xSpacingUV = textureCoordinates.width / (xSteps - 1);
  const float ySpacingUV = textureCoordinates.height / (ySteps - 1);
  int vertexIndex = 0;
  for( int y = 0; y < ySteps; ++y )
  {
    for( int x = 0; x < xSteps; ++x )
    {
      MeshData::Vertex& vertex = vertices[vertexIndex];
      vertex.x = xOffset + (xSpacing * x);
      vertex.y = yOffset + (ySpacing * y);
      vertex.z = 0.0f;

      vertex.nX = 0.0f;
      vertex.nY = 0.0f;
      vertex.nZ = 1.0f;

      vertex.u = textureCoordinates.x + (xSpacingUV * x);
      vertex.v = textureCoordinates.y + (ySpacingUV * y);
      ++vertexIndex;
    }
  }

  // faces
  const int faceCount = 2 * ((ySteps - 1) * (xSteps - 1));
  MeshData::FaceIndices faces( faceCount * 3 );
  unsigned short* pIndex = &(faces)[0];
  unsigned short index0 = 0;
  unsigned short index1 = 0;
  unsigned short index2 = 0;

  for( int y = 0; y < ySteps - 1; ++y )
  {
    for( int x = 0; x < xSteps - 1; ++x )
    {
      index0 = (y * xSteps) + x;
      index1 = ((y + 1) * xSteps) + x;
      index2 = ((y + 1) * xSteps) + (x + 1);
      *pIndex++ = index0;
      *pIndex++ = index1;
      *pIndex++ = index2;

      index0 = ((y + 1) * xSteps) + (x + 1);
      index1 = (y * xSteps) + (x + 1);
      index2 = (y * xSteps) + x;
      *pIndex++ = index0;
      *pIndex++ = index1;
      *pIndex++ = index2;
    }
  }

  BoneContainer bones;

  MeshData meshData;
  meshData.SetHasNormals(true);
  meshData.SetHasTextureCoords(true);
  meshData.SetData( vertices, faces, bones, Material::New("PlaneMat"));

  Vector4 vMin;
  Vector4 vMax;
  meshData.AddToBoundingVolume(vMin, vMax, Matrix::IDENTITY);

  meshData.SetBoundingBoxMin(vMin);
  meshData.SetBoundingBoxMax(vMax);

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
