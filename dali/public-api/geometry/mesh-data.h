#ifndef __DALI_MESH_DATA_H__
#define __DALI_MESH_DATA_H__

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


// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/modeling/bone.h>
#include <dali/public-api/modeling/material.h>


// DECLARATION FILES


namespace Dali DALI_IMPORT_API
{
class Matrix;

/**
 * @brief The MeshData class encompasses all the data required to describe and
 * render a 3D mesh.
 *
 * The mesh can have one of three geometry types: Points, Lines or Triangles.
 * The Point type draws each vertex. The shader can control the point size.
 * The Line type draws single pixel width lines between each specified vertex.
 * The Triangles type draws solid color or texture between each specified
 * vertex.
 *
 * When using the Points or Lines geometry type, normals aren't used.
 *
 * If the HasColor property is set, the vertex color is used to draw the points,
 * lines or triangles, otherwise the material's diffuse color or texture is
 * used. The material's diffuse alpha value is always used.
 *
 * When using the Triangles geometry type, the normals are mutually exclusive
 * with vertex color. This means that shading cannot be performed if vertex
 * color is being used.
 *
 * It is possible to deform the mesh using skeletal deformation. To acheive
 * this, a number of bones can be supplied during creation. These are named
 * actors that can be anywhere in the actor tree. Each bone has an offset
 * matrix. A vertex in the mesh can reference up to 4 bones in the bone
 * container with an associated weighting.
 *
 * When the mesh is added to the scene graph, the node for each named actor is
 * added to an internal container. Each Update frame after the node update pass,
 * the node's world matrix is multiplied by the associated bone's offset matrix.
 * When rendering the mesh, these transform matrices are multiplied by the view
 * matrix, their inverse transforms calculated and passed onto the shaders.
 *
 * The mesh vertex shader then applies up to 4 weighted transforms to each
 * vertex and vertex normal.
 */
class MeshData
{
public:
  static const unsigned int MAX_NUMBER_OF_BONES = 12; ///< Maximum number of bones that can affect this mesh.

  struct Vertex;
  typedef std::vector<Vertex>                 VertexContainer;    ///< Collection of vertices
  typedef VertexContainer::iterator           VertexIter;         ///< Iterator for @ref Dali::MeshData::VertexContainer
  typedef VertexContainer::const_iterator     VertexConstIter;    ///< Const iterator for @ref Dali::MeshData::VertexContainer

  typedef unsigned short                      FaceIndex;          ///< index of one point of a polygonal face
  typedef std::vector<FaceIndex>              FaceIndices;        ///< Collection of FaceIndex items ( The span should match the geometry type: Lines - span is 2; Triangles - span is 3 ).
  typedef FaceIndices::iterator               FaceIndexIter;      ///< Iterator for @ref Dali::MeshData::FaceIndices
  typedef FaceIndices::const_iterator         FaceIndexConstIter; ///< Const iterator for @ref Dali::MeshData::FaceIndices

  /**
   * @brief The type of geometry to draw
   */
  enum VertexGeometryType
  {
    POINTS,    ///< Draw only points at each vertex
    LINES,     ///< Draw lines between specified vertices
    TRIANGLES  ///< Draw filled triangles between specified vertices
  };

public: // construction, destruction and initialisation

  /**
   * @brief Create a new mesh.
   */
  MeshData( );

  /**
   * @brief Copy constructor.
   *
   * @param[in] meshData object to copy
   */
  MeshData( const MeshData& meshData );

  /**
   * @brief Assignment operator.
   *
   * @param[in] rhs MeshData object to copy data from
   * @return A reference to this
   */
  MeshData& operator=(const MeshData& rhs);

  /**
   * @brief Sets the vertex coords, the face indices, the bones affecting this mesh and a default
   * material.
   *
   * @param[in] vertices The Vertex data (coordinates of each vertex)
   * @param[in] faceIndices The index into the vertices buffer for each corner of each triangular face.
   * @param[in] bones A container of Bones affecting this mesh.
   * @param[in] material A handle to a material object.
   */
  void SetData( const VertexContainer& vertices,
                const FaceIndices&     faceIndices,
                const BoneContainer&   bones,
                const Material         material );

  /**
   * @brief Set the vertex coords and end points of each line.
   *
   * @param[in] vertices The vertex data (coords & color of each vertex)
   * @param[in] lineIndices A list of vertex indices for the start & end of each line.
   * @param[in] material A handle to a material object.
   */
  void SetLineData( const VertexContainer& vertices,
                    const FaceIndices&    lineIndices,
                    const Material        material );

  /**
   * @brief Set the vertex coords for each point.
   *
   * @param[in] vertices The vertex data (coords & color of each vertex)
   * @param[in] material A handle to a material object.
   */
  void SetPointData( const VertexContainer& vertices,
                     const Material         material );

  /**
   * @brief Set the vertex coords for each point.
   *
   * @param[in] vertices The vertex data (coords & color of each vertex)
   */
  void SetVertices( const VertexContainer& vertices );

  /**
   * @brief Sets the face indices.
   *
   * @param[in] faceIndices The index into the vertices buffer for each corner of each triangular face.or for the start and end of each line.
   */
  void SetFaceIndices( const FaceIndices& faceIndices );

  /**
   * @brief Add the mesh to the bounding volume.
   *
   * Expands a bounding volume to include the mesh
   * @param[in,out] min   Lower bounds
   * @param[in,out] max   Upper bounds
   * @param[in] transform transform the mesh vertices
   */
  void AddToBoundingVolume(Vector4& min, Vector4& max, const Matrix& transform);

  /**
   * @brief Get the geometry type.
   *
   * A mesh defaults to triangles if no data has been set.
   * @return the geometry type;
   */
  VertexGeometryType GetVertexGeometryType() const;

  /**
   * @brief Get the number of vertices.
   *
   * @return The number of vertices
   */
  size_t GetVertexCount() const;

  /**
   * @brief Get the vertex array.
   *
   * @return The vertex array
   */
  const VertexContainer& GetVertices() const;

  /**
   * @brief Get the number of points, lines or faces (note this is not the same as the number of face indices!).
   *
   * depending on the geometry type;
   * @return Number of points, lines or faces
   */
  size_t GetFaceCount() const;

  /**
   * @brief Get the face index array.
   *
   * @return The face index array
   */
  const FaceIndices& GetFaces() const;

  /**
   * @brief Sets if the mesh has texture coordinates.
   *
   * @param hasTexCoords - True if the mesh has texture coordinates.
   */
  void SetHasTextureCoords(bool hasTexCoords);

  /**
   * @brief Checks if the mesh is textured.
   *
   * @return true if the mesh is texture mapped.
   */
  bool HasTextureCoords() const;

  /**
   * @brief Sets if the mesh has normals.
   *
   * Mutually exclusive with HasColor. Setting this to true will force the
   * HasColor property to be set to false.
   * @param hasNormals - True if the mesh has normals
   */
  void SetHasNormals(bool hasNormals);

  /**
   * @brief Checks if the mesh has normals.
   *
   * @return true if the mesh contains normals.
   */
  bool HasNormals() const;

  /**
   * @brief Sets if the mesh vertices have color.
   *
   * Mutually exclusive with HasNormals. Setting this to true will force the
   * HasNormals property to be set to false.
   * @param hasColor - True if the mesh vertices have color.
   */
  void SetHasColor(bool hasColor);

  /**
   * @brief Checks if the mesh vertices have color.
   *
   * @return true if the mesh contains colored vertices.
   */
  bool HasColor() const;

  /**
   * @brief Get the original material associated with this mesh.
   *
   * @return Handle to the material
   */
  Material GetMaterial() const;

  /**
   * @brief Set the default material associated with this mesh.
   */
  void SetMaterial( Material material );

  /**
   * @brief Get the number of bones affecting this mesh.
   *
   * @return The number of bones affecting this mesh
   */
  size_t GetBoneCount() const;

  /**
   * @brief Does this mesh have bones?.
   *
   * @return true if this mesh has bones.
   */
  bool HasBones() const;

  /**
   * @brief Get the bone container.
   *
   * @return the bones
   */
  const BoneContainer& GetBones() const;

  /**
   * @brief Get the lower bounds of the bounding box containing the vertices.
   *
   * @return the lower bounds
   */
  const Vector4& GetBoundingBoxMin() const;

  /**
   * @brief Set the lower bounds of the bounding box containing the vertices.
   *
   * @param bounds The lower bounds
   */
  void SetBoundingBoxMin(const Vector4& bounds);

  /**
   * @brief Get the upper bounds of the bounding box containing the vertices.
   *
   * @return the upper bounds
   */
  const Vector4& GetBoundingBoxMax() const;

  /**
   * @brief Set the upper bounds of the bounding box containing the vertices.
   *
   * @param bounds The upper bounds
   */
  void SetBoundingBoxMax(const Vector4& bounds);

  /**
   * @brief Destructor.
   */
  ~MeshData();

private:
  VertexContainer         mVertices;          ///< The vertex data
  FaceIndices             mFaces;             ///< Indices of triangle faces or line terminators
  VertexGeometryType      mGeometryType;      ///< The type of geometry to draw
  bool                    mUseTextureCoords;  ///< Whether the vertex data contains texture coords
  bool                    mUseNormals;        ///< Whether the vertex data contains normals
  bool                    mUseColor;          ///< Whether the vertex data contains vertex color
  BoneContainer           mBones;             ///< Bones for skeletal deformation

  Material                mMaterial;          ///< Material of the mesh

  Vector4                 mMin;               ///< The lower bounds of the bounding box
  Vector4                 mMax;               ///< The upper bounds of the bounding box

}; // class MeshData

/**
 * @brief A vertex within a mesh, with the corresponding texture coordinate, normal and up to 4 bone influences.
 */
struct MeshData::Vertex
{
  static const unsigned int MAX_BONE_INFLUENCE = 4; ///< Maximum number of bones that can influence this particular vertex.

  Vertex()
  : x(0.0f),  y(0.0f),  z(0.0f),
    u(0.0f),  v(0.0f),
    nX(0.0f), nY(0.0f), nZ(0.0f)
  {
  }

  /**
   * @brief Constructor.
   *
   * @param[in] position The vertex position
   * @param[in] textureCoordinates The texture coordinates
   * @param[in] normal The surface normal
   */
  Vertex( const Vector3& position, const Vector2& textureCoordinates, const Vector3& normal )
  : x(position.x),  y(position.y),  z(position.z),
    u(textureCoordinates.x),  v(textureCoordinates.y),
    nX(normal.x), nY(normal.y), nZ(normal.z)
  {
  }

  // Vertices
  float x;
  float y;
  float z;

  // Texture coordinates
  float u;
  float v;

  // Normals / vertex colours
  union
  {
    float nX;
    float vertexR;
  };
  union
  {
    float nY;
    float vertexG;
  };
  union
  {
    float nZ;
    float vertexB;
  };

  /**
   * @brief Bone indices specify which bones have an influence over the vertex (if any).
   */
  unsigned char boneIndices[MAX_BONE_INFLUENCE];

  /**
   * @brief Bone weights determine the strength of the influence of each bone.
   */
  float boneWeights[MAX_BONE_INFLUENCE];
};

} // namespace Dali

#endif // __DALI_MESH_DATA_H__
