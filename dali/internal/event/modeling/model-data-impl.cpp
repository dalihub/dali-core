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
#include <dali/internal/event/modeling/model-data-impl.h>

// EXTERNAL INCLUDES
#include <stdio.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/common/light.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/modeling/entity-impl.h>
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/event/modeling/model-archive.h>
#include <dali/internal/update/modeling/internal-mesh-data.h>

namespace
{
const unsigned int ModelDataFormatVersion = 0x10004;
}

namespace Dali
{

namespace Internal
{

using Dali::Vector4;
using namespace Serialize;

ModelData::ModelData()
: mRoot(),
  mUnpacked( false )
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);
}

ModelDataPtr ModelData::New(const std::string& name)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  ModelDataPtr modelData(new ModelData());
  modelData->SetName(name);
  return modelData;
}

ModelData::~ModelData()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  mMeshes.Clear();
  mMaterials.clear();
  mAnimationMaps.clear();
  mLights.clear();
}

void ModelData::SetName(const std::string& name)
{
  mName = name;
}

const std::string& ModelData::GetName() const
{
  return mName;
}

void ModelData::SetRootEntity(Dali::Entity root)
{
  mRoot = root;
}

Dali::Entity ModelData::GetRootEntity() const
{
  return mRoot;
}

void ModelData::AddMesh(const Dali::MeshData& mesh)
{
  mMeshes.PushBack( new Dali::MeshData( mesh ) );
}

void ModelData::AddMeshTicket(ResourceTicketPtr ticket)
{
  mMeshTickets.push_back(ticket);
}

const Dali::MeshData& ModelData::GetMesh(unsigned int index) const
{
  DALI_ASSERT_ALWAYS( index < mMeshes.Size() && "Mesh index out of bounds" );
  return *mMeshes[index];
}

Dali::MeshData& ModelData::GetMesh(unsigned int index)
{
  DALI_ASSERT_ALWAYS( index < mMeshes.Size() && "Mesh index out of bounds" );
  return *mMeshes[index];
}

const ResourceTicketPtr ModelData::GetMeshTicket(unsigned int index) const
{
  DALI_ASSERT_ALWAYS( index < mMeshTickets.size() && "Mesh index out of bounds" );
  return mMeshTickets[index];
}

unsigned int ModelData::NumberOfMeshes() const
{
  return mMeshes.Size();
}

void ModelData::AddMaterial(Dali::Material material)
{
  mMaterials.push_back(material);
}

Dali::Material ModelData::GetMaterial(unsigned int index) const
{
  DALI_ASSERT_DEBUG(index < mMaterials.size());

  return mMaterials[index];
}

unsigned int ModelData::NumberOfMaterials() const
{
  return mMaterials.size();
}

ModelAnimationMapContainer& ModelData::GetAnimationMapContainer()
{
  return mAnimationMaps;
}

const ModelAnimationMap* ModelData::GetAnimationMap (unsigned int index) const
{
  const ModelAnimationMap* found(NULL);
  if( index < mAnimationMaps.size() )
  {
    found = &mAnimationMaps[index];
  }
  return found;
}

const ModelAnimationMap* ModelData::GetAnimationMap (const std::string& name) const
{
  const ModelAnimationMap* found(NULL);

  for(ModelAnimationMapContainer::const_iterator iter=mAnimationMaps.begin();
      iter != mAnimationMaps.end();
      iter++)
  {
    const ModelAnimationMap& animData = *iter;
    if(animData.name == name)
    {
      found = &(*iter);
      break;
    }
  }

  return found;
}

bool ModelData::FindAnimation (const std::string& name, unsigned int& index) const
{
  bool found = false;

  index = 0;
  for(ModelAnimationMapContainer::const_iterator iter=mAnimationMaps.begin();
      iter != mAnimationMaps.end();
      iter++, index++)
  {
    const ModelAnimationMap& animData(*iter);
    if(animData.name == name)
    {
      found = true;
      break;
    }
  }
  return found;
}

float ModelData::GetAnimationDuration(size_t animationIndex) const
{
  DALI_ASSERT_DEBUG (animationIndex < mAnimationMaps.size ());
  const ModelAnimationMap& animData( mAnimationMaps[animationIndex] );
  return animData.duration;
}

unsigned int ModelData::NumberOfAnimationMaps() const
{
  return mAnimationMaps.size();
}

void ModelData::AddLight(Dali::Light light)
{
  mLights.push_back(light);
}

Dali::Light ModelData::GetLight(unsigned int index) const
{
  Dali::Light light;
  if (index < mLights.size())
  {
    light = mLights[index];
  }
  return light;
}

unsigned int ModelData::NumberOfLights() const
{
  return mLights.size();
}

void ModelData::Unpack( ResourceClient& resourceClient )
{
  // Only unpack once
  if ( !mUnpacked )
  {
    // Ensure that mesh tickets are in same order as the meshes they represent
    for ( size_t meshIdx = 0; meshIdx < NumberOfMeshes(); meshIdx++ )
    {
      // Copy the mesh-data into an internal structure, and pass ownership to the resourceClient
      OwnerPointer<MeshData> meshDataPtr( new MeshData( GetMesh( meshIdx ), true /* discardable */, false /* scaling is transmitted through parent Node */ ) );
      ResourceTicketPtr meshTicket = resourceClient.AllocateMesh( meshDataPtr );

      AddMeshTicket( meshTicket );
    }

    // TODO: Update to use image tickets directly. Would need access to ImageFactory.
    for ( size_t materialIdx = 0; materialIdx < NumberOfMaterials(); materialIdx++ )
    {
      Dali::Material material = GetMaterial( materialIdx );
      const std::string& diffuseFileName = material.GetDiffuseFileName();
      if( ! diffuseFileName.empty() )
      {
        material.SetDiffuseTexture( Dali::Image::New( diffuseFileName ) );
      }

      const std::string& opacityFileName = material.GetOpacityTextureFileName();
      if( ! opacityFileName.empty() )
      {
        material.SetOpacityTexture( Dali::Image::New( opacityFileName ) );
      }

      const std::string& normalMapFileName = material.GetNormalMapFileName();
      if( ! normalMapFileName.empty() )
      {
        material.SetNormalMap( Dali::Image::New( normalMapFileName ) );
      }
    }

    mUnpacked = true;
  }
}

bool ModelData::Read(std::streambuf& buf)
{
  InputArchive ar(buf, ModelDataFormatVersion);
  ar >> *this;
  return ar.GetResult();
}

bool ModelData::Write(std::streambuf& buf) const
{
  OutputArchive ar(buf, ModelDataFormatVersion);
  ar << *this;
  return ar.GetResult();
}

} // namespace Internal

} // namespace Dali
