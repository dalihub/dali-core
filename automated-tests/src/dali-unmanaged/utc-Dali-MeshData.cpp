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

#include <iostream>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

#include "mesh-builder.h"



int UtcDaliMeshDataNew(void)
{
  TestApplication application;
  MeshData meshData;

  DALI_TEST_EQUALS(meshData.HasNormals(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(meshData.HasTextureCoords(), false, TEST_LOCATION);
  END_TEST;
}


int UtcDaliMeshDataSetData(void)
{
  TestApplication application;

  MeshData meshData;
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 60);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);

  DALI_TEST_GREATER(meshData.GetVertexCount(), 0u, TEST_LOCATION);
  DALI_TEST_GREATER(meshData.GetFaceCount(), 0u, TEST_LOCATION);

  const MeshData::FaceIndices& faces2 = meshData.GetFaces();
  const MeshData::VertexContainer& verts2 = meshData.GetVertices();
  DALI_TEST_EQUALS(faces.at(0), faces2.at(0), TEST_LOCATION);
  DALI_TEST_EQUALS(vertices.at(1).y, verts2.at(1).y, TEST_LOCATION);
  DALI_TEST_EQUALS(meshData.GetBoneCount(), static_cast<size_t>(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeshDataAddToBoundingVolume(void)
{
  TestApplication application;

  float sz=40.0f;

  MeshData meshData;
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, sz);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);

  Vector4 upper(-1e10f, -1e10f, -1e10f, 0.0f);
  Vector4 lower(1e10f, 1e10f, 1e10f, 0.0f);
  Matrix f(false);
  f.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  meshData.AddToBoundingVolume(lower, upper, f);

  Vector4 min(-sz*0.5f, -sz,      -sz*0.7f, 0.0f);
  Vector4 max( sz*0.5f,  sz*0.3f,  sz*0.5f, 0.0f);

  // Test that upper and lower bounds are set and transformed
  DALI_TEST_EQUALS(lower, min*2.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(upper, max*2.0f, 0.001, TEST_LOCATION);

  // Test that mesh's upper and lower bounds are set and not transformed
  DALI_TEST_EQUALS(meshData.GetBoundingBoxMin(), min, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(meshData.GetBoundingBoxMax(), max, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeshDataBoundingBox(void)
{
  TestApplication application;

  float sz=40.0f;
  MeshData meshData;
  Vector4 min(-1.0f, -2.0f, -3.0f, 0.0f);
  Vector4 max(1.0f, 2.0f, 3.0f, 0.0f);
  meshData.SetBoundingBoxMin(min);
  meshData.SetBoundingBoxMax(max);
  DALI_TEST_EQUALS(meshData.GetBoundingBoxMin(), min, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(meshData.GetBoundingBoxMax(), max, 0.001, TEST_LOCATION);

  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, sz);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);

  // Check bounding box hasn't changed
  DALI_TEST_EQUALS(meshData.GetBoundingBoxMin(), min, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(meshData.GetBoundingBoxMax(), max, 0.001, TEST_LOCATION);

  Vector4 upper(-1e10f, -1e10f, -1e10f, 0.0f);
  Vector4 lower(1e10f, 1e10f, 1e10f, 0.0f);
  meshData.AddToBoundingVolume(lower, upper, Matrix::IDENTITY);

  // Bounding box should have been update
  Vector4 bbMin(-sz*0.5f, -sz,      -sz*0.7f, 0.0f);
  Vector4 bbMax( sz*0.5f,  sz*0.3f,  sz*0.5f, 0.0f);

  // Test that upper and lower bounds are set and transformed
  DALI_TEST_EQUALS(lower, bbMin, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(upper, bbMax, 0.001, TEST_LOCATION);

  // Test that mesh's upper and lower bounds are set and not transformed
  DALI_TEST_EQUALS(meshData.GetBoundingBoxMin(), bbMin, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(meshData.GetBoundingBoxMax(), bbMax, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeshDataGetVertexCount(void)
{
  TestApplication application;

  MeshData meshData;
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 30);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();

  DALI_TEST_EQUALS(meshData.GetVertexCount(), static_cast<size_t>(0), TEST_LOCATION);

  meshData.SetData(vertices, faces, bones, customMaterial);
  DALI_TEST_EQUALS(meshData.GetVertexCount(), vertices.size(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliMeshDataGetVertices(void)
{
  TestApplication application;
  MeshData meshData;
  const Dali::MeshData::VertexContainer& verts1 = meshData.GetVertices();
  DALI_TEST_CHECK(verts1.size() == 0);

  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 30);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);

  const Dali::MeshData::VertexContainer& verts2 = meshData.GetVertices();
  DALI_TEST_CHECK(verts2.size() != 0);
  DALI_TEST_CHECK(verts2.size() == meshData.GetVertexCount());
  END_TEST;
}

int UtcDaliMeshDataGetFaceCount(void)
{
  TestApplication application;
  MeshData meshData;
  DALI_TEST_EQUALS(meshData.GetFaceCount(), static_cast<size_t>(0), TEST_LOCATION);

  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 30);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);

  DALI_TEST_EQUALS(meshData.GetFaceCount(), faces.size() / 3, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeshDataGetFaces(void)
{
  TestApplication application;
  MeshData meshData;
  const Dali::MeshData::FaceIndices& faces1 = meshData.GetFaces();
  DALI_TEST_CHECK(faces1.size() == 0);

  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 30);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);

  const Dali::MeshData::FaceIndices& faces2 = meshData.GetFaces();
  DALI_TEST_CHECK(faces2.size() != 0);
  END_TEST;
}

int UtcDaliMeshDataTextureCoords(void)
{
  TestApplication application;
  MeshData meshData;
  DALI_TEST_EQUALS(meshData.HasTextureCoords(), false, TEST_LOCATION);
  meshData.SetHasTextureCoords(true);
  DALI_TEST_EQUALS(meshData.HasTextureCoords(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeshDataNormals(void)
{
  TestApplication application;
  MeshData meshData;
  DALI_TEST_EQUALS(meshData.HasNormals(), false, TEST_LOCATION);
  meshData.SetHasNormals(true);
  DALI_TEST_EQUALS(meshData.HasNormals(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeshDataGetMaterial(void)
{
  TestApplication application;
  MeshData meshData;
  Material aMat = meshData.GetMaterial();
  DALI_TEST_CHECK(!aMat);

  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 30);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);

  aMat = meshData.GetMaterial();
  DALI_TEST_CHECK(aMat);
  END_TEST;
}

int UtcDaliMeshDataSetMaterial(void)
{
  TestApplication application;
  MeshData meshData;

  Material aMat = meshData.GetMaterial();
  DALI_TEST_CHECK(!aMat);

  Material mat1 = ConstructMaterial();
  meshData.SetMaterial(mat1);
  aMat = meshData.GetMaterial();
  DALI_TEST_CHECK(mat1 == aMat);

  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 30);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);

  aMat = meshData.GetMaterial();

  DALI_TEST_CHECK(aMat == customMaterial);
  DALI_TEST_CHECK(aMat != mat1);

  END_TEST;
}

int UtcDaliMeshDataGetBoneCount(void)
{
  TestApplication application;
  MeshData meshData;
  DALI_TEST_EQUALS(meshData.GetBoneCount(), static_cast<size_t>(0), TEST_LOCATION);
  DALI_TEST_EQUALS(meshData.HasBones(), false, TEST_LOCATION);

  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 30);
  ConstructFaces(vertices, faces);
  ConstructBones(bones);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);

  DALI_TEST_EQUALS(meshData.GetBoneCount(), static_cast<size_t>(3), TEST_LOCATION);
  DALI_TEST_EQUALS(meshData.HasBones(), true, TEST_LOCATION);
  END_TEST;
}


int UtcDaliMeshDataGetBones(void)
{
  TestApplication application;
  MeshData meshData;
  DALI_TEST_EQUALS(meshData.GetBoneCount(), static_cast<size_t>(0), TEST_LOCATION);
  const BoneContainer& bones1 = meshData.GetBones();
  DALI_TEST_CHECK(bones1.empty());

  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 30);
  ConstructFaces(vertices, faces);
  ConstructBones(bones);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);
  const BoneContainer& bones3 = meshData.GetBones();
  DALI_TEST_CHECK( ! bones3.empty() );
  END_TEST;
}
