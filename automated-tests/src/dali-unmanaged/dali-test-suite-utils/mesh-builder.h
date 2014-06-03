#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H
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

#include <dali/dali.h>
#include <dali/public-api/dali-core.h>

namespace Dali
{

void AddVertex( MeshData::VertexContainer& verts, Vector3 V, Vector2 UV );
void SetNormal( MeshData::VertexContainer& verts, size_t vertIdx, Vector3 normal );
void SetBone( MeshData::VertexContainer& verts, size_t vertIdx, size_t index, size_t boneIndex, float weight);
void SetBones(MeshData::VertexContainer& verts);
void ConstructBones(BoneContainer& bones);
void CopyVertex( MeshData::Vertex& vert, Vector3& vector );
void AddTriangle( MeshData::VertexContainer& verts,
                  MeshData::FaceIndices& faces,
                  size_t v0, size_t v1, size_t v2 );
void ConstructVertices( MeshData::VertexContainer& vertices, float sz );
void ConstructFaces(MeshData::VertexContainer& vertices, MeshData::FaceIndices& faces);
Material ConstructMaterial();
Mesh ConstructMesh( float sz );
void AddBone(Dali::BoneContainer& bones, const std::string& name, const Dali::Matrix& offsetMatrix);
void CreateMeshData(MeshData& meshData);
Dali::ModelData BuildTreeModel();

}

#endif // MESH_BUILDER_H
