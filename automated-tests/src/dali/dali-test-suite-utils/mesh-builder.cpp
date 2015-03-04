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
#include "mesh-builder.h"

namespace Dali
{

void AddVertex( MeshData::VertexContainer& verts, Vector3 V, Vector2 UV )
{
  MeshData::Vertex meshVertex;
  meshVertex.x = V.x;
  meshVertex.y = V.y;
  meshVertex.z = V.z;
  meshVertex.u = UV.x;
  meshVertex.v = UV.y;
  verts.push_back(meshVertex);
}

void SetNormal( MeshData::VertexContainer& verts, size_t vertIdx, Vector3 normal )
{
  verts[vertIdx].nX = normal.x;
  verts[vertIdx].nY = normal.y;
  verts[vertIdx].nZ = normal.z;
}

void SetBone( MeshData::VertexContainer& verts, size_t vertIdx, size_t index, size_t boneIndex, float weight)
{
  verts[vertIdx].boneIndices[index] = boneIndex;
  verts[vertIdx].boneWeights[index] = weight;
}

void SetBones(MeshData::VertexContainer& verts)
{
  // Set all verts in one corner to be affected fully by bone 0
  SetBone(verts, 0, 0, 0, 1.0f);
  SetBone(verts, 1, 0, 0, 1.0f);
  SetBone(verts, 2, 0, 0, 1.0f);

  // Set all verts in next corner to be affected by bone 1 and bone 2 equally
  SetBone(verts, 3, 0, 1, 0.5f);
  SetBone(verts, 4, 0, 1, 0.5f);
  SetBone(verts, 5, 0, 1, 0.5f);

  SetBone(verts, 3, 1, 2, 0.5f);
  SetBone(verts, 4, 1, 2, 0.5f);
  SetBone(verts, 5, 1, 2, 0.5f);
}

void ConstructBones(BoneContainer& bones)
{
  bones.push_back(Bone("Bone1", Matrix::IDENTITY));
  bones.push_back(Bone("Bone2", Matrix::IDENTITY));
  bones.push_back(Bone("Bone3", Matrix::IDENTITY));
}

void CopyVertex( MeshData::Vertex& vert, Vector3& vector )
{
  vector.x = vert.x;
  vector.y = vert.y;
  vector.z = vert.z;
}

void AddTriangle( MeshData::VertexContainer& verts,
                  MeshData::FaceIndices& faces,
                  size_t v0, size_t v1, size_t v2 )
{
  faces.push_back(v0);
  faces.push_back(v1);
  faces.push_back(v2);

  // Calculate normal...
  Vector3 vert0, vert1, vert2;
  CopyVertex(verts[v0], vert0);
  CopyVertex(verts[v1], vert1);
  CopyVertex(verts[v2], vert2);
  Vector3 e0 = vert1 - vert0;
  Vector3 e1 = vert2 - vert1;
  Vector3 normal = e0.Cross(e1);
  normal.Normalize();
  SetNormal(verts, v0, normal);
  SetNormal(verts, v1, normal);
  SetNormal(verts, v2, normal);
}

void ConstructVertices( MeshData::VertexContainer& vertices, float sz )
{
  // back
  AddVertex(vertices, Vector3( 0.0f,  -sz, 0.0f), Vector2(0.50f, 0.50f));        // 0a  0
  AddVertex(vertices, Vector3( 0.0f,  -sz, 0.0f), Vector2(0.50f, 0.50f));        // 0b  1
  AddVertex(vertices, Vector3( 0.0f,  -sz, 0.0f), Vector2(0.50f, 0.50f));        // 0c  2

  // left
  AddVertex(vertices, Vector3(-sz*0.5f, sz*0.3f, sz*0.5f), Vector2(0.25f, 0.50f));  // 1a  3
  AddVertex(vertices, Vector3(-sz*0.5f, sz*0.3f, sz*0.5f), Vector2(0.25f, 0.50f));  // 1b  4
  AddVertex(vertices, Vector3(-sz*0.5f, sz*0.3f, sz*0.5f), Vector2(0.25f, 0.50f));  // 1c  5

  // right
  AddVertex(vertices, Vector3( sz*0.5f, sz*0.3f, sz*0.5f), Vector2(0.50f, 0.25f));  // 2a  6
  AddVertex(vertices, Vector3( sz*0.5f, sz*0.3f, sz*0.5f), Vector2(0.50f, 0.25f));  // 2b  7
  AddVertex(vertices, Vector3( sz*0.5f, sz*0.3f, sz*0.5f), Vector2(0.50f, 0.25f));  // 2c  8

  // top
  AddVertex(vertices, Vector3( 0.0f,   sz*0.3f, -sz*0.7f), Vector2(0.25f, 0.25f)); // 3a  9
  AddVertex(vertices, Vector3( 0.0f,   sz*0.3f, -sz*0.7f), Vector2(0.25f, 0.25f)); // 3b 10
  AddVertex(vertices, Vector3( 0.0f,   sz*0.3f, -sz*0.7f), Vector2(0.25f, 0.25f)); // 3c 11
}

void ConstructFaces(MeshData::VertexContainer& vertices, MeshData::FaceIndices& faces)
{
  AddTriangle(vertices, faces,  0, 6,  3); // 0, 2, 1  back, right, left (ac)
  AddTriangle(vertices, faces,  1, 9,  7); // 0, 3, 2  back, top , right (ac)
  AddTriangle(vertices, faces,  2, 4, 10); // 0, 1, 3  back, left, top   (ac)
  AddTriangle(vertices, faces, 11, 5,  8); // 3, 1, 2  top, left, right  (ac)
}

Material ConstructMaterial()
{
  Material customMaterial = Material::New("CustomMaterial");
  customMaterial.SetOpacity(.76f);
  customMaterial.SetDiffuseColor(Vector4(0.8f, 0.0f, 0.4f, 1.0f));
  customMaterial.SetAmbientColor(Vector4(0.2f, 1.0f, 0.6f, 1.0f));
  customMaterial.SetSpecularColor(Vector4(0.5f, 0.6f, 0.7f, 1.0f));
  return customMaterial;
}


Mesh ConstructMesh( float sz )
{
  MeshData::VertexContainer vertices;
  MeshData::FaceIndices     faces;
  ConstructVertices( vertices, sz );
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();

  MeshData meshData;
  BoneContainer bones;
  meshData.SetData(vertices, faces, bones, customMaterial);
  meshData.SetHasNormals(true);
  meshData.SetHasTextureCoords(true);

  Mesh mesh = Mesh::New(meshData);
  return mesh;
}


void AddBone(Dali::BoneContainer& bones, const std::string& name, const Dali::Matrix& offsetMatrix)
{
  bones.push_back(Bone(name, offsetMatrix));
}

void CreateMeshData(MeshData& meshData)
{
  MeshData::VertexContainer vertices;
  MeshData::FaceIndices faces;
  Dali::BoneContainer bones;
  AddBone(bones, "trunk",  Matrix::IDENTITY);
  AddBone(bones, "branch", Matrix::IDENTITY);
  AddBone(bones, "twig",   Matrix::IDENTITY);
  ConstructVertices( vertices, 50 );
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);
  meshData.SetHasNormals(true);
  meshData.SetHasTextureCoords(true);
}


} // namespace Dali
