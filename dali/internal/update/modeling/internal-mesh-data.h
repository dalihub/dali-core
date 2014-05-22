#ifndef __DALI_INTERNAL_MESH_DATA_H__
#define __DALI_INTERNAL_MESH_DATA_H__

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

#include <dali/public-api/geometry/mesh-data.h>

namespace Dali
{

namespace Internal
{

/**
 * Internal MeshData class to hold the data required to describe and
 * render a 3D mesh.
 *
 * This class is the internal equivalent of the public api MeshData.
 *
 * For more details about the data it holds look at Dali::MeshData.
 */

class MeshData
{
public:

  // Reuse the types as Dali::MeshData
  typedef Dali::MeshData::Vertex Vertex;
  typedef Dali::MeshData::VertexContainer VertexContainer;
  typedef Dali::MeshData::FaceIndices FaceIndices;
  typedef Dali::BoneContainer BoneContainer;

  /**
   * Create a new MeshData from a Dali::MeshData object.
   * This constructor is explicit to prevent accidental copies.
   * @param[in] meshData object to copy
   * @param[in] discardable if true, the vertex and index data will be discarded after they are uploaded to gl
   * @param[in] scalingRequired - True if this Mesh should be scaled to fit actor size
   */
  explicit MeshData( const Dali::MeshData& meshData, bool discardable, bool scalingRequired );

  /**
   * @copydoc Dali::MeshData::GetVertexCount
   */
  size_t GetVertexCount() const;

  /**
   * @copydoc Dali::MeshData::GetFaceCount
   */
  size_t GetFaceCount() const;

  /**
   * @copydoc Dali::MeshData::GetFaceIndexCount
   */
  size_t GetFaceIndexCount() const;

  /**
   * @copydoc Dali::MeshData::GetBoneCount
   */
  size_t GetBoneCount() const;

  /**
   * @copydoc Dali::MeshData::GetVertexGeometryType
   */
  Dali::MeshData::VertexGeometryType GetVertexGeometryType() const;

  /**
   * @copydoc Dali::MeshData::GetVertices
   */
  const VertexContainer& GetVertices() const;

  /**
   * @copydoc Dali::MeshData::GetVertices
   */
  VertexContainer& GetVertices();

  /**
   * @copydoc Dali::MeshData::GetVertices
   */
  void SetVertices( const VertexContainer& vertices );

  /**
   * @copydoc Dali::MeshData::GetFaces
   */
  const FaceIndices& GetFaces() const;

  /**
   * @copydoc Dali::MeshData::GetBones
   */
  const Dali::BoneContainer& GetBones() const;

  /**
   * @copydoc Dali::MeshData::HasNormals
   */
  bool HasNormals() const;

  /**
   * @copydoc Dali::MeshData::HasColor
   */
  bool HasColor() const;

  /**
   * Discard the geometrical data.
   */
  void Discard();

  /**
   * Query whether this Mesh should be scaled to fit actor size.
   * @return True if scaling is required.
   */
  bool IsScalingRequired() const;

private:
  VertexContainer mVertices;
  FaceIndices     mFaces;
  BoneContainer   mBones;

  size_t mNumberOfVertices;
  size_t mNumberOfFaceIndices;

  Dali::MeshData::VertexGeometryType mGeometryType;

  bool mHasNormals : 1;
  bool mHasColor   : 1;

  bool mDiscardable : 1;
  bool mDiscarded   : 1;

  bool mScalingRequired : 1;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_MESH_DATA_H__
