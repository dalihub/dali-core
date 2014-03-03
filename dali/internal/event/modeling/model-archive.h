#if !defined(__DALI_INTERNAL_MODEL_ARCHIVE_H__)
#define __DALI_INTERNAL_MODEL_ARCHIVE_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/internal/event/modeling/model-data-impl.h>
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/internal/event/resources/archive.h>

namespace Dali
{
class MeshData;

/**
 * Vertex element types.
 * Defines the shape of a vertex
 */
enum VertexElementType
{
  VET_Position,             ///< Vector3
  VET_BoneWeights,          ///< Array of 4 floats
  VET_BoneIndices,          ///< Array of 4 uint8_t
  VET_Normal,               ///< Vector3
  VET_Diffuse,              ///< Vertex color - Vector4
  VET_Specular,             ///< Vertex color - Vector4
  VET_UV,                   ///< Texture coordinate - Vector2
  VET_Bitangent,            ///< Bitangent (positive Y texture axis) - Vector3
  VET_Tangent,              ///< Tangent  (positive X texture axis) - Vector3
  VET_Custom                ///< User defined 1 float
}; // VertexElementType

namespace Internal
{
class Material;
class MaterialProperties;
class ModelData;

namespace Serialize
{

//---------- MaterialProperties ----------//
Archive& operator<< (Archive& ar, const MaterialProperties& t);
Archive& operator>> (Archive& ar, MaterialProperties& t);
//---------- Material ----------//
Archive& operator<< (Archive& ar, const Material& t);
Archive& operator>> (Archive& ar, Material& t);
/*---- Vertex ----*/
Archive& operator<< (Archive&, const Dali::MeshData::Vertex&);
Archive& operator>> (Archive&, Dali::MeshData::Vertex&);
/*---- Light ----*/
Archive& operator<< (Archive&, const Dali::Light&);
Archive& operator>> (Archive&, Dali::Light&);
/*---- Bone ----*/
Archive& operator<< (Archive&, const Dali::Bone&);
Archive& operator>> (Archive&, Dali::Bone&);
/*---- MeshData ----*/
Archive& operator<< (Archive&, const Dali::MeshData&);
Archive& operator>> (Archive&, MeshData&);
/*---- EntityAnimatorMap ----*/
Archive& operator<< (Archive&, const Dali::EntityAnimatorMap&);
Archive& operator>> (Archive&, Dali::EntityAnimatorMap&);
/*---- ModelAnimationMap ----*/
Archive& operator<< (Archive&, const Dali::ModelAnimationMap&);
Archive& operator>> (Archive&, ModelAnimationMap&);
/*---- Entity ----*/
Archive& operator<< (Archive&, const Dali::Entity&);
Archive& operator>> (Archive&, Dali::Entity&);
/*---- ModelData ----*/
Archive& operator<< (Archive&, const ModelData&);
Archive& operator>> (Archive&, ModelData&);

} // namespace Serialize

} // Internal

} // Dali

#endif // !defined(__DALI_INTERNAL_MODEL_ARCHIVE_H__)
