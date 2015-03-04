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

#include "mesh-factory.h"

#include <dali/public-api/geometry/mesh-data.h>
#include <dali/public-api/modeling/material.h>

namespace Dali
{
namespace MeshFactory
{

Dali::MeshData NewPlane(const float width, const float height, const int xSteps, const int ySteps, const Rect<float>& textureCoordinates)
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

  Dali::MeshData meshData;

  meshData.SetHasNormals(true);
  meshData.SetHasTextureCoords(true);
  meshData.SetData( vertices, faces, bones, Dali::Material::New("PlaneMat"));

  Vector4 vMin;
  Vector4 vMax;
  meshData.AddToBoundingVolume(vMin, vMax, Matrix::IDENTITY);

  meshData.SetBoundingBoxMin(vMin);
  meshData.SetBoundingBoxMax(vMax);

  return meshData;
}

Dali::MeshData NewPath( Dali::Path path, const unsigned int resolution )
{
  MeshData meshData;
  meshData.SetHasNormals(false);
  meshData.SetHasTextureCoords(false);

  if( resolution != 0 )
  {
    size_t vertexCount(resolution+1);
    MeshData::VertexContainer vertex(vertexCount);

    float sampleDelta = 1.0f/(float)resolution;
    Vector3 tangent;
    for( size_t i(0); i!=vertexCount; ++i )
    {
      //Sample path to get the vertex position
      Vector3* vertexPosition = reinterpret_cast<Vector3*>(&vertex[i].x);
      path.Sample( i*sampleDelta, *vertexPosition, tangent );
    }

    //Generate indices. Each vertex is connected to the next
    size_t indexCount(resolution*2);
    MeshData::FaceIndices index(indexCount);
    unsigned int nIndex = 0;
    for( size_t i(0); i!=indexCount; i+=2 )
    {
      index[i] = nIndex;
      index[i+1] = ++nIndex;
    }

    meshData.SetLineData(vertex, index, Dali::Material::New("PathMat"));
  }

  return meshData;
}

} // MeshFactory
} // Dali
