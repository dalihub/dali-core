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
#include <dali/internal/event/modeling/model-logger.h>

// EXTERNAL INCLUDES
#include <iostream>
#include <sstream>
#include <iomanip>

// INTERNAL INCLUDES
#include <dali/public-api/modeling/bone.h>
#include <dali/public-api/modeling/entity.h>
#include <dali/public-api/modeling/material.h>
#include <dali/public-api/modeling/model-animation-map.h>
#include <dali/public-api/modeling/entity-animator-map.h>

#include <dali/integration-api/debug.h>

#include <dali/internal/event/modeling/light-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/event/animation/key-frames-impl.h>
#include <dali/internal/event/modeling/model-impl.h>

using namespace std;

namespace Dali
{
using Internal::MaterialIPtr;
using Internal::MeshIPtr;
using Dali::MeshData;

namespace Internal
{

void ModelLogger::WriteMeshes()
{
  cout << "Meshes: " << endl;

  for(unsigned int meshIdx = 0; meshIdx < mModel.NumberOfMeshes(); ++meshIdx)
  {
    const Dali::MeshData& meshData = mModel.GetMesh(meshIdx);

    cout << endl;
    cout << "Mesh[" << meshIdx << "]  NumVertices: " << meshData.GetVertexCount()
         << "  NumFaces: " << meshData.GetFaceCount()
         << "  HasTexCoords: " << (meshData.HasTextureCoords()?"T":"F")
         << "  HasNormals: " << (meshData.HasNormals()?"T":"F")
         << "  Material: " << meshData.GetMaterial().GetName() << endl
         << "  Bounding box: " << meshData.GetBoundingBoxMin() << " - " << meshData.GetBoundingBoxMax() << endl;

    if (meshData.GetBoneCount())
    {
      const BoneContainer& bones = meshData.GetBones();
      for(size_t boneIdx = 0; boneIdx < bones.size(); ++boneIdx)
      {
        cout << "   Bones[" << boneIdx << "] " << bones.at(boneIdx).GetName() << endl;
      }
    }


    if(mWriteVertices)
    {
      // create temporary pointer to the vertices
      const Dali::MeshData::VertexContainer& vertices = meshData.GetVertices();
      for(unsigned int vertexIdx = 0; vertexIdx < vertices.size(); ++vertexIdx)
      {
        const MeshData::Vertex& vertex = vertices[vertexIdx];
        cout << "Vertex[" << vertexIdx << "] Pos<"<<vertex.x<<","<<vertex.y<<","<<vertex.z<<">";
        if(meshData.HasTextureCoords())
        {
          cout << " TexC<"<<vertex.u<<","<<vertex.v<<">";
        }
        if(meshData.HasNormals())
        {
          cout << " Normal<"<<vertex.nX<<","<<vertex.nY<<","<<vertex.nZ<<">";
        }
        cout << endl;
        if(meshData.HasBones())
        {
          cout << "    BoneIndices[] = ";
          for(size_t i=0; i<MeshData::Vertex::MAX_BONE_INFLUENCE; i++)
          {
            cout << (int)vertex.boneIndices[i] << ", ";
          }
          cout << endl << "    BoneWeights[] = ";
          for (size_t i=0; i< MeshData::Vertex::MAX_BONE_INFLUENCE; i++)
          {
            cout << vertex.boneWeights[i] << ", ";
          }
          cout << endl;
        }
      }
      cout << endl;

      const Dali::MeshData::FaceIndices& faces = meshData.GetFaces();
      for(unsigned int faceIdx = 0; faceIdx < meshData.GetFaceCount(); ++faceIdx)
      {
        const FaceIndex* face = &(faces)[faceIdx * 3];
        cout << "(" << face[0] <<"," << face[1] << "," << face[2] << ") ";
        cout << endl;
      }

      cout << endl;
    }

  }
  cout << endl;
}

void ModelLogger::WriteMaterials()
{
  cout << "Materials:" << endl;

  for(unsigned int matIdx = 0; matIdx < mModel.NumberOfMaterials(); ++matIdx)
  {
    Dali::Material mat = mModel.GetMaterial(matIdx);
    cout << "Material["   << matIdx << "] " << mat.GetName();
    cout << "  Opacity: "   << mat.GetOpacity();
    cout << "  Shininess: " << mat.GetShininess();
    cout << "  Ambient: "   << Debug::ColorToString(mat.GetAmbientColor());
    cout << "  Diffuse: "   << Debug::ColorToString(mat.GetDiffuseColor());
    cout << "  Specular: "  << Debug::ColorToString(mat.GetSpecularColor());
    cout << "  Emissive: "  << Debug::ColorToString(mat.GetEmissiveColor()) << endl;

    // TODO: Add
  }
  cout << endl;
}

void ModelLogger::WriteLights()
{
  cout << "Lights:" << endl;
  for(unsigned int lightIdx = 0; lightIdx < mModel.NumberOfLights(); ++lightIdx)
  {
    Dali::Light light = mModel.GetLight(lightIdx);
    cout << "Light[" << lightIdx << "] " << light.GetName()
         << " type(" << light.GetType()
         << ") falloff(" << light.GetFallOff().x << ", " << light.GetFallOff().y
         << ") spotangle(" << light.GetSpotAngle().x << ", " << light.GetSpotAngle().y
         << ") ambient(" << light.GetAmbientColor().r << ", " << light.GetAmbientColor().g << ", " << light.GetAmbientColor().b
         << ") diffuse(" << light.GetDiffuseColor().r << ", " << light.GetDiffuseColor().g << ", " << light.GetDiffuseColor().b
         << ") specular(" << light.GetSpecularColor().r << ", " << light.GetSpecularColor().g << ", " << light.GetSpecularColor().b
         << ")" << endl;
  }
  cout << endl;
}

void ModelLogger::WriteEntity(Dali::Entity entity, int level)
{
  if (entity)
  {
    string indent = string(level*2, ' ');
    cout << indent << "Entity " << "Type:<";
    switch(entity.GetType())
    {
      case Dali::Entity::OBJECT: cout<<"OBJECT"; break;
      case Dali::Entity::CAMERA: cout<<"CAMERA"; break;
      case Dali::Entity::LIGHT:  cout<<"LIGHT";  break;
    }
    cout << "> Name: " << entity.GetName() << endl;
    cout << indent << "  Meshes(" << entity.NumberOfMeshes() << ") = [";
    for (int i=0; i<entity.NumberOfMeshes(); i++)
    {
      cout << entity.GetMeshByIndex(i) << ", ";
    }
    cout << "]" << endl;

    cout << indent << "  LowerBounds: " << Debug::Vector3ToString(entity.GetLowerBounds()) << endl;
    cout << indent << "  UpperBounds: " << Debug::Vector3ToString(entity.GetUpperBounds()) << endl;
    cout << indent << "  TransformMatrix: " << endl;
    cout << Debug::MatrixToString(entity.GetTransformMatrix(), 3, level*2+4) << endl;

    if(entity.HasChildren())
    {
      ++level;
      const EntityContainer& children = entity.GetChildren();
      for(EntityConstIter iter = children.begin(); iter != children.end(); ++iter)
      {
        WriteEntity(*iter, level);
      }
    }
  }
  cout << endl;
}

void ModelLogger::WriteAnimations()
{
  cout << "Animations (:" << mModel.NumberOfAnimationMaps() << ")" << endl;
  for(unsigned int idx = 0; idx < mModel.NumberOfAnimationMaps(); ++idx)
  {
    const ModelAnimationMap* animation( mModel.GetAnimationMap(idx) );
    cout << "Animation ["<< idx << "] " << endl;
    if( animation != NULL )
    {
      cout << "  Duration:  " << animation->duration << endl;
      cout << "  Repeats:   " << animation->repeats << endl;

      size_t animatorIdx=0;

      for(EntityAnimatorMapIter iter = animation->animators.begin(); iter != animation->animators.end(); ++iter)
      {
        const EntityAnimatorMap& animator(*iter);
        std::string entityName = animator.GetEntityName();

        cout << "  Animator ["<<animatorIdx<<"] " << entityName << "  "
             << "Duration: " << animator.GetDuration() << endl;

        const KeyFrameVector3* positionKeyFrames;
        GetSpecialization(GetImplementation(animator.GetPositionKeyFrames()), positionKeyFrames);
        const KeyFrameVector3* scaleKeyFrames;
        GetSpecialization(GetImplementation(animator.GetScaleKeyFrames()), scaleKeyFrames);
        const KeyFrameQuaternion* rotationKeyFrames;
        GetSpecialization(GetImplementation(animator.GetRotationKeyFrames()), rotationKeyFrames);

        if(positionKeyFrames->GetNumberOfKeyFrames() > 0)
        {
          cout << "    Channel: Position" << endl;

          for(size_t kfIndex = 0; kfIndex < positionKeyFrames->GetNumberOfKeyFrames(); kfIndex++)
          {
            float progress;
            Vector3 position;

            positionKeyFrames->GetKeyFrame(kfIndex, progress, position);
            cout << "  [" << kfIndex << "] " << progress << position << endl;
          }
        }

        if(scaleKeyFrames->GetNumberOfKeyFrames() > 0)
        {
          cout << "    Channel: Scale" << endl;
          for(size_t kfIndex = 0; kfIndex < scaleKeyFrames->GetNumberOfKeyFrames(); kfIndex++)
          {
            float progress;
            Vector3 scale;
            scaleKeyFrames->GetKeyFrame(kfIndex, progress, scale);
            cout << "  [" << kfIndex << "] " << progress << scale << endl;
          }
        }

        if(rotationKeyFrames->GetNumberOfKeyFrames() > 0)
        {
          cout << "    Channel: Rotation" << endl;
          for(size_t kfIndex = 0; kfIndex < rotationKeyFrames->GetNumberOfKeyFrames(); kfIndex++)
          {
            float progress;
            Quaternion rotation;
            rotationKeyFrames->GetKeyFrame(kfIndex, progress, rotation);
            cout << "  [" << kfIndex << "] " << progress << rotation << endl;
          }
        }
      }
    }
    else
    {
      cout << "Not found";
    }
  }
  cout << endl;
}


void ModelLogger::Write()
{
  cout << "Model name : " << mModel.GetName() << endl;
  WriteMeshes();
  WriteMaterials();
  WriteLights();
  cout << "Entities:" << endl;
  WriteEntity(mModel.GetRootEntity(), 0);
  WriteAnimations();
}


} // Internal
} // Dali
