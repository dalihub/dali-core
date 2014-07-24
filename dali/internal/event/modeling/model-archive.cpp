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

// CLASS HEADER
#include <dali/internal/event/modeling/model-archive.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/internal/event/animation/key-frames-impl.h>
#include <dali/internal/event/modeling/entity-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/integration-api/debug.h>

using std::string;

#define FOURCC(a,b,c,d)   \
  ((((a) & 0xff)      ) | \
   (((b) & 0xff) <<  8) | \
   (((c) & 0xff) << 16) | \
   (((d) & 0xff) << 24))

namespace Dali
{

namespace Internal
{

namespace Serialize
{
/*------ serialization overrides of operator<< () and operator>> () ------*/

//---------- MaterialProperties ----------//
Archive& operator<< (Archive& ar, const MaterialProperties& t)
{
  ar << t.mOpacity;
  ar << t.mShininess;
  ar << t.mAmbientColor;
  ar << t.mDiffuseColor;
  ar << t.mSpecularColor;
  ar << t.mEmissiveColor;
  ar << t.mMapU;
  ar << t.mMapV;
  ar << t.mDiffuseUVIndex;
  ar << t.mOpacityUVIndex;
  ar << t.mNormalUVIndex;
  ar << t.mHasHeightMap;

  return ar;
}

Archive& operator>> (Archive& ar, MaterialProperties& t)
{
  ar >> t.mOpacity;
  ar >> t.mShininess;
  ar >> t.mAmbientColor;
  ar >> t.mDiffuseColor;
  ar >> t.mSpecularColor;
  ar >> t.mEmissiveColor;
  ar >> t.mMapU;
  ar >> t.mMapV;
  ar >> t.mDiffuseUVIndex;
  ar >> t.mOpacityUVIndex;
  ar >> t.mNormalUVIndex;
  ar >> t.mHasHeightMap;

  return ar;
}

//---------- Material ----------//
Archive& operator<< (Archive& ar, const Material& t)
{
  ar.OpenChunk(FOURCC('M', 'A', 'T', '_'));

  // Material's name
  ar << t.GetName();

  // Diffuse texture file name
  ar << t.GetDiffuseTextureFileName();

  // Opacity texture file name
  ar << t.GetOpacityTextureFileName();

  // Normal map file name
  ar << t.GetNormalMapFileName();

  // Material properties
  ar << t.GetProperties();

  ar.CloseChunk(); // MAT_

  return ar;
}

Archive& operator>> (Archive& ar, Material& t)
{
  string name;

  if( ar.GetResult() &&
      ar.OpenChunk(FOURCC('M', 'A', 'T', '_')) )
  {
    // Material's name
    ar >> name;
    t.SetName(name);

    // Diffuse texture file name
    ar >> name;
    t.SetDiffuseTextureFileName(name);

    // Opacity texture file name
    ar >> name;
    t.SetOpacityTextureFileName(name);

    // Normal map file name
    ar >> name;
    t.SetNormalMapFileName(name);

    // Material properties
    MaterialProperties properties;
    ar >> properties;
    t.SetProperties(properties);

    ar.CloseChunk(); // MAT_
  }

  return ar;
}

/*---- Vertex ----*/

Archive& operator<< (Archive& ar, const Dali::MeshData::Vertex& t)
{
  // Position
  ar << t.x << t.y << t.z;

  // Texture coordinates
  ar << t.u << t.v;

  // Normals
  ar << t.nX << t.nY << t.nZ;

  // Bones
  for( unsigned int i = 0; i < Dali::MeshData::Vertex::MAX_BONE_INFLUENCE; ++i)
  {
    ar << t.boneIndices[i];
  }

  for( unsigned int i = 0; i < Dali::MeshData::Vertex::MAX_BONE_INFLUENCE; ++i)
  {
    ar << t.boneWeights[i];
  }

  return ar;
}

Archive& operator>> (Archive& ar, MeshData::Vertex& t)
{
  // Position
  ar >> t.x >> t.y >> t.z;

  // Texture coordinates
  ar >> t.u >> t.v;

  // Normals
  ar >> t.nX >> t.nY >> t.nZ;

  // Bones
  for( unsigned int i = 0; i < Dali::MeshData::Vertex::MAX_BONE_INFLUENCE; ++i)
  {
    ar >> t.boneIndices[i];
  }

  for( unsigned int i = 0; i < Dali::MeshData::Vertex::MAX_BONE_INFLUENCE; ++i)
  {
    ar >> t.boneWeights[i];
  }

  return ar;
}

/*---- Light ----*/

Archive& operator<< (Archive& ar, const Dali::Light& t)
{
  ar.OpenChunk(FOURCC('L', 'I', 'T', '_'));

  ar << t.GetName();
  ar << static_cast<unsigned int>(t.GetType());
  ar << t.GetFallOff();
  ar << t.GetSpotAngle();
  ar << t.GetAmbientColor();
  ar << t.GetDiffuseColor();
  ar << t.GetSpecularColor();
  ar << t.GetDirection();

  ar.CloseChunk(); // LIT_
  return ar;
}

Archive& operator>> (Archive& ar, Dali::Light& t)
{
  std::string name;
  Vector2 v2Value;
  Vector3 v3Value;

  if( ar.GetResult() &&
      ar.OpenChunk(FOURCC('L', 'I', 'T', '_')) )
  {

    ar >> name;
    t.SetName(name);

    ar >> v2Value;
    t.SetFallOff(v2Value);
    ar >> v2Value;
    t.SetSpotAngle(v2Value);
    ar >> v3Value;
    t.SetAmbientColor(v3Value);
    ar >> v3Value;
    t.SetDiffuseColor(v3Value);
    ar >> v3Value;
    t.SetSpecularColor(v3Value);
    ar >> v3Value;
    t.SetDirection(v3Value);

    ar.CloseChunk(); // LIT_
  }

  return ar;
}

/*---- Bone ----*/

Archive& operator<< (Archive& ar, const Dali::Bone& t)
{
  ar.OpenChunk(FOURCC('B', 'O', 'N', 'E'));

  ar << t.GetName();
  ar << t.GetOffsetMatrix();

  ar.CloseChunk(); // BONE
  return ar;
}

Archive& operator>> (Archive& ar, Bone& t)
{
  if( ar.GetResult() &&
      ar.OpenChunk(FOURCC('B', 'O', 'N', 'E')) )
  {
    std::string name;
    Matrix offsetMatrix;
    ar >> name;
    ar >> offsetMatrix;
    t = Bone( name, offsetMatrix );
    ar.CloseChunk(); // BONE
  }
  return ar;
}

/*---- MeshData ----*/

Archive& operator<< (Archive& ar, const Dali::MeshData& meshData)
{
  ar.OpenChunk(FOURCC('M', 'E', 'S', 'H'));

  unsigned int count;

  // write vertices
  ar.OpenChunk(FOURCC('V', 'R', 'T', 'S'));
  // definition of a vertex
  ar.OpenChunk(FOURCC('D', 'E', 'F', '_'));
  ar << static_cast<unsigned char>(VET_Position);
  ar << static_cast<unsigned char>(VET_UV);
  ar << static_cast<unsigned char>(VET_Normal);
  ar << static_cast<unsigned char>(VET_BoneIndices);
  ar << static_cast<unsigned char>(VET_BoneWeights);
  ar.CloseChunk(); // DEF_
  count = meshData.GetVertexCount();
  ar << count;
  if( count )
  {
    const Dali::MeshData::VertexContainer& vertices(meshData.GetVertices());
    for( unsigned int i = 0; i < count; ++i)
    {
      ar << vertices[i];
    }
  }
  ar.CloseChunk(); // VRTS

  // write faces
  ar.OpenChunk(FOURCC('F', 'C', 'E', 'S'));
  count = meshData.GetFaceCount();
  count *= 3;  // 3 elements per triangular face
  ar << count;
  if( count )
  {
    const Dali::MeshData::FaceIndices& faces(meshData.GetFaces());
    for( unsigned int i = 0; i < count; ++i)
    {
      ar << faces[i];
    }
  }
  ar.CloseChunk(); // FCES

  // write bones
  ar.OpenChunk(FOURCC('B', 'N', 'E', 'S'));
  count = meshData.GetBoneCount();
  ar << count;
  if( count )
  {
    const Dali::BoneContainer& bones(meshData.GetBones());
    for( unsigned int i = 0; i < count; ++i)
    {
      ar << bones.at(i);
    }
  }
  ar.CloseChunk(); // BNES

  // write material (just write the material's name)
  ar << meshData.GetMaterial().GetName();

  // write texturing options
  ar << meshData.HasTextureCoords();
  ar << meshData.HasNormals();

  // write AABB
  ar << meshData.GetBoundingBoxMin();
  ar << meshData.GetBoundingBoxMax();

  ar.CloseChunk(); // MESH
  return ar;
}

Archive& operator>> (Archive& ar, MeshData& meshData)
{
  Dali::MeshData::VertexContainer vertices;
  Dali::MeshData::FaceIndices faces;
  Dali::BoneContainer bones;
  Dali::Material material;
  Dali::MeshData::Vertex vertex;
  Dali::MeshData::FaceIndex faceIndex;
  Dali::Bone bone;
  std::string name;
  unsigned int count = 0;

  if( ar.GetResult() &&
      ar.OpenChunk(FOURCC('M', 'E', 'S', 'H')) )
  {
    // read vertices
    if( ar.OpenChunk(FOURCC('V', 'R', 'T', 'S')) )
    {
      // Version 1 has fixed vertex elements, so skip definition
      ar.SkipChunk(FOURCC('D', 'E', 'F', '_'));
      ar >> count;
      if( count )
      {
        vertices.reserve(count);

        for( unsigned int i = 0; i < count; ++i )
        {
          ar >> vertex;
          vertices.push_back(vertex);
        }
      }
      ar.CloseChunk(); // VRTS
    }

    // read faces
    if( ar.GetResult() &&
        ar.OpenChunk(FOURCC('F', 'C', 'E', 'S')) )
    {
      ar >> count;
      if( count )
      {
        faces.reserve(count);

        for( unsigned int i = 0; i < count; ++i )
        {
          ar >> faceIndex;
          faces.push_back(faceIndex);
        }
      }
      ar.CloseChunk(); // FCES
    }

    // read bones
    if( ar.GetResult() &&
        ar.OpenChunk(FOURCC('B', 'N', 'E', 'S')) )
    {
      ar >> count;
      if( count )
      {
        bones.reserve(count);

        for( unsigned int i = 0; i < count; ++i )
        {
          ar >> bone;
          bones.push_back(bone);
        }
      }
      ar.CloseChunk(); // BNES
    }

    // read material
    ar >> name;
    material = Dali::Material::New(name);

    meshData.SetData(vertices, faces, bones, material);

    // write texturing options
    bool option = false;
    ar >> option;
    meshData.SetHasTextureCoords(option);
    ar >> option;
    meshData.SetHasNormals(option);

    // write AABB
    Vector4 bounds;
    ar >> bounds;
    bounds.w = 0.0f;
    meshData.SetBoundingBoxMin(bounds);
    ar >> bounds;
    bounds.w = 0.0f;
    meshData.SetBoundingBoxMax(bounds);

    ar.CloseChunk(); // MESH
  }
  return ar;
}

/*---- EntityAnimatorMap ----*/
Archive& operator<< (Archive& ar, const Dali::EntityAnimatorMap& t)
{
  ar.OpenChunk(FOURCC('E', 'A', 'N', 'I'));

  ar << t.GetEntityName();
  ar << t.GetDuration();

  const KeyFrameVector3* positionKeyFrames = NULL;
  GetSpecialization(GetImplementation(t.GetPositionKeyFrames()), positionKeyFrames);
  unsigned int count = positionKeyFrames->GetNumberOfKeyFrames();
  ar << count;
  if( count )
  {
    for( unsigned int i = 0; i < count; ++i)
    {
      float progress;
      Vector3 position;
      positionKeyFrames->GetKeyFrame(i, progress, position);

      ar << progress << position;
    }
  }

  const KeyFrameVector3* scaleKeyFrames = NULL;
  GetSpecialization(GetImplementation(t.GetScaleKeyFrames()), scaleKeyFrames);
  count = scaleKeyFrames->GetNumberOfKeyFrames();
  ar << count;
  if( count )
  {
    for( unsigned int i = 0; i < count; ++i)
    {
      float progress;
      Vector3 scale;
      scaleKeyFrames->GetKeyFrame(i, progress, scale);

      ar << progress << scale;
    }
  }

  const KeyFrameQuaternion* rotationKeyFrames = NULL;
  GetSpecialization(GetImplementation(t.GetRotationKeyFrames()), rotationKeyFrames);
  count = rotationKeyFrames->GetNumberOfKeyFrames();
  ar << count;
  if( count )
  {
    for( unsigned int i = 0; i < count; ++i)
    {
      float progress;
      Quaternion rotation;
      rotationKeyFrames->GetKeyFrame(i, progress, rotation);

      ar << progress << rotation;
    }
  }

  ar.CloseChunk(); // EANI
  return ar;
}

Archive& operator>> (Archive& ar, Dali::EntityAnimatorMap& t)
{
  std::string name;
  unsigned int count = 0;
  float floatValue = 0.0f;
  Vector3 vec3Value;
  Quaternion quatValue;

  if( ar.GetResult() &&
      ar.OpenChunk(FOURCC('E', 'A', 'N', 'I')) )
  {
    ar >> name;
    t.SetEntityName(name);
    ar >> floatValue;
    t.SetDuration(floatValue);


    Dali::KeyFrames positionKeyFrames = Dali::KeyFrames::New();
    ar >> count;
    if( count )
    {
      for( unsigned int i = 0; i < count; ++i )
      {
        ar >> floatValue;
        ar >> vec3Value;
        Property::Value value(vec3Value);
        positionKeyFrames.Add(floatValue, value);
      }
      t.SetPositionKeyFrames(positionKeyFrames);
    }

    Dali::KeyFrames scaleKeyFrames = Dali::KeyFrames::New();
    ar >> count;
    if( count )
    {
      for( unsigned int i = 0; i < count; ++i )
      {
        ar >> floatValue;
        ar >> vec3Value;
        Property::Value value(vec3Value);
        scaleKeyFrames.Add(floatValue, value);
      }
      t.SetScaleKeyFrames(scaleKeyFrames);
    }

    Dali::KeyFrames rotationKeyFrames = Dali::KeyFrames::New();
    ar >> count;
    if( count )
    {
      for( unsigned int i = 0; i < count; ++i )
      {
        ar >> floatValue;
        ar >> quatValue;
        Property::Value value(quatValue);
        rotationKeyFrames.Add(floatValue, value);
      }
      t.SetRotationKeyFrames(rotationKeyFrames);
    }

    ar.CloseChunk(); // EANI
  }

  return ar;
}

/*---- ModelAnimationMap ----*/
Archive& operator<< (Archive& ar, const Dali::ModelAnimationMap& t)
{
  ar.OpenChunk(FOURCC('A', 'N', 'I', 'M'));

  ar << t.name;

  // animators
  unsigned int count = t.animators.size();
  ar << count;
  if( count )
  {
    for( unsigned int i = 0; i < count; ++i)
    {
      const EntityAnimatorMap& entityAnimatorMap(t.animators[i]);

      ar << entityAnimatorMap;
    }
  }

  ar << t.duration;
  ar << t.repeats;

  ar.CloseChunk(); // ANIM
  return ar;
}

Archive& operator>> (Archive& ar, ModelAnimationMap& t)
{
  std::string name;
  unsigned int count = 0;

  if( ar.GetResult() &&
      ar.OpenChunk(FOURCC('A', 'N', 'I', 'M')) )
  {
    ar >> t.name;

    // animators
    ar >> count;
    if( count )
    {
      t.animators.reserve(count);
      for( unsigned int i = 0; i < count; ++i)
      {
        EntityAnimatorMap entityAnimator("");
        ar >> entityAnimator;
        t.animators.push_back(entityAnimator);
      }
    }

    ar >> t.duration;
    ar >> t.repeats;

    ar.CloseChunk(); // ANIM
  }

  return ar;
}

/*---- Entity ----*/
Archive& operator<< (Archive& ar, const Dali::Entity& t)
{
  ar.OpenChunk(FOURCC('E', 'N', 'T', 'Y'));

  unsigned int count;

  ar << t.GetName();

  // meshes
  count = t.NumberOfMeshes();
  ar << count;
  if( count )
  {
    const EntityMeshIndices& meshes = t.GetMeshes();
    for( unsigned int i = 0; i < count; ++i)
    {
      ar << meshes[i];
    }
  }

  // matrix
  ar << t.GetTransformMatrix();

  // bounds
  ar << t.GetLowerBounds();
  ar << t.GetUpperBounds();

  // type
  ar << static_cast<unsigned int>(t.GetType());

  // children
  count = t.NumberOfChildren();
  ar << count;
  if( count )
  {
    const EntityContainer& entities = t.GetChildren();
    for( unsigned int i = 0; i < count; ++i )
    {
      ar << entities[i];
    }
  }

  ar.CloseChunk(); // ENTY
  return ar;
}

Archive& operator>> (Archive& ar, Dali::Entity& t)
{
  unsigned int count = 0;
  unsigned int uintValue = 0;
  Matrix mat4Value;
  Vector3 vec3Value;
  std::string name;

  if( ar.GetResult() &&
      ar.OpenChunk(FOURCC('E', 'N', 'T', 'Y')) )
  {
    ar >> name;
    t.SetName(name);

    // meshes
    ar >> count;
    if( count )
    {
      t.SetMeshCapacity(count);
      for( unsigned int i = 0; i < count; ++i)
      {
        ar >> uintValue;
        t.AddMeshIndex(uintValue);
      }
    }

    // matrix
    ar >> mat4Value;
    t.SetTransformMatrix(mat4Value);

    // bounds
    ar >> vec3Value;
    GetImplementation(t).SetLowerBounds(vec3Value);
    ar >> vec3Value;
    GetImplementation(t).SetUpperBounds(vec3Value);

    // type
    ar >> uintValue;
    t.SetType(static_cast<Dali::Entity::EntityType>(uintValue));

    // children
    ar >> count;
    if( count )
    {
      for( unsigned int i = 0; i < count; ++i )
      {
        Dali::Entity child(Dali::Entity::New(""));
        ar >> child;

        t.Add(child);
        t.AddToBounds(child);
      }
    }

    ar.CloseChunk(); // ENTY
  }

  return ar;
}

/*---- ModelData ----*/

Archive& operator<< (Archive& ar, const ModelData& t)
{
  unsigned int count;

  ar.OpenChunk(FOURCC('D', 'A', 'L', 'I'));

  ar.OpenChunk(FOURCC('T', 'Y', 'P', 'E'));
  std::string info("Dali Binary Model");
  ar << info;
  ar.CloseChunk(); // TYPE

  ar.OpenChunk(FOURCC('V', 'E', 'R', 'S'));
  ar << ar.GetVersion();
  ar.CloseChunk(); // VERS

  ar.OpenChunk(FOURCC('N', 'A', 'M', 'E'));
  ar << t.GetName();
  ar.CloseChunk(); // NAME

  // Materials
  ar.OpenChunk(FOURCC('M', 'A', 'T', 'S'));
  count = t.NumberOfMaterials();
  ar << count;
  for( unsigned int i = 0; i < count; ++i)
  {
    Dali::Material material = t.GetMaterial(i);
    ar << static_cast<const Material&>(material.GetBaseObject());
  }
  ar.CloseChunk(); // MATS

  // Meshes
  ar.OpenChunk(FOURCC('M', 'S', 'H', 'S'));
  count = t.NumberOfMeshes();
  ar << count;
  for( unsigned int i = 0; i < count; ++i)
  {
    const Dali::MeshData& mesh(t.GetMesh(i));

    ar << mesh;
  }
  ar.CloseChunk(); // MSHS

  // Lights
  ar.OpenChunk(FOURCC('L', 'I', 'T', 'S'));
  count = t.NumberOfLights();
  ar << count;
  for( unsigned int i = 0; i < count; ++i)
  {
    ar << t.GetLight(i);
  }
  ar.CloseChunk(); // LITS

  // Entities
  ar.OpenChunk(FOURCC('E', 'N', 'T', 'S'));
  ar << t.GetRootEntity();
  ar.CloseChunk(); // ENTS

  // Animations
  ar.OpenChunk(FOURCC('A', 'N', 'I', 'S'));
  count = t.NumberOfAnimationMaps();
  ar << count;
  if( count )
  {
    for( unsigned int i = 0; i < count; ++i)
    {
      ar << *t.GetAnimationMap(i);
    }
  }
  ar.CloseChunk(); // ANIS

  ar.CloseChunk(); // DALI
  return ar;
}

Archive& operator>> (Archive& ar, ModelData& t)
{
  unsigned int count = 0;
  unsigned int uintValue = 0;

  if( ar.GetResult() &&
      ar.OpenChunk(FOURCC('D', 'A', 'L', 'I')) )
  {
    DALI_ASSERT_DEBUG( ar.PeekChunk() == FOURCC('T', 'Y', 'P', 'E') );
    //  ar.OpenChunk(FOURCC('T', 'Y', 'P', 'E'));
    ar.SkipChunk(FOURCC('T', 'Y', 'P', 'E'));

    if( ar.GetResult() &&
        ar.OpenChunk(FOURCC('V', 'E', 'R', 'S')) )
    {
      ar >> uintValue;
      static_cast<InputArchive&>(ar).SetFileVersion(uintValue);
      ar.CloseChunk();
      // Set archive failure if there is a version mismatch
      if( uintValue != ar.GetVersion() )
      {
        ar.SetResultFailed();
      }
    }

    //  ar.OpenChunk(FOURCC('N', 'A', 'M', 'E'));
    ar.SkipChunk(FOURCC('N', 'A', 'M', 'E'));

    // Materials
    if( ar.GetResult() &&
        ar.OpenChunk(FOURCC('M', 'A', 'T', 'S')) )
    {
      ar >> count;
      for( unsigned int i = 0; i < count; ++i)
      {
        Dali::Material material = Dali::Material::New("");
        ar >> static_cast<Material&>(material.GetBaseObject());
        t.AddMaterial(material);
      }
      ar.CloseChunk();
    }

    // Meshes
    if( ar.GetResult() &&
        ar.OpenChunk(FOURCC('M', 'S', 'H', 'S')) )
    {
      ar >> count;
      for( unsigned int i = 0; i < count; ++i)
      {
        Dali::MeshData meshData;
        ar >> meshData;

        for( unsigned j = 0; j < t.NumberOfMaterials(); ++j)
        {
          Dali::Material material = t.GetMaterial(j);
          if( material.GetName() == meshData.GetMaterial().GetName() )
          {
            meshData.SetMaterial(material);
          }
        }
        t.AddMesh(meshData);
      }
      ar.CloseChunk(); // MSHS
    }

    if( ar.GetResult() &&
        ar.OpenChunk(FOURCC('L', 'I', 'T', 'S')) )
    {
      ar >> count;
      if( count )
      {
        for( unsigned int i = 0; i < count; ++i )
        {
          Dali::Light light(Dali::Light::New(""));
          ar >> light;
          t.AddLight(light);
        }
      }
      ar.CloseChunk(); // LITS
    }

    // Entities
    if( ar.GetResult() &&
        ar.OpenChunk(FOURCC('E', 'N', 'T', 'S')) )
    {
      Dali::Entity root(Dali::Entity::New(""));
      ar >> root;
      t.SetRootEntity(root);
      ar.CloseChunk(); // ENTS
    }

    // Animations
    if( ar.GetResult() &&
        ar.OpenChunk(FOURCC('A', 'N', 'I', 'S')) )
    {
      ar >> count;
      if( count )
      {
        ModelAnimationMapContainer& mapContainer( t.GetAnimationMapContainer() );
        mapContainer.reserve(count);
        for( unsigned int i = 0; i < count; ++i)
        {
          ModelAnimationMap animation;
          ar >> animation;
          mapContainer.push_back(animation);
        }
      }
      ar.CloseChunk(); // ANIS
    }

    ar.CloseChunk(); // DALI
  }
  return ar;
}

} // namespace Serialize

} // Internal

} // Dali
