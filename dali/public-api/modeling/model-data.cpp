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

// CLASS HEADER
#include <dali/public-api/modeling/model-data.h>

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali/internal/event/modeling/model-data-impl.h>

namespace Dali
{

ModelData::ModelData()
{
}

ModelData::ModelData(Internal::ModelData* modelData) : BaseHandle(modelData)
{
}

ModelData ModelData::New(const std::string& name)
{
  Internal::ModelDataPtr internal = Internal::ModelData::New(name);

  return ModelData(internal.Get());
}

ModelData ModelData::DownCast( BaseHandle handle )
{
  return ModelData( dynamic_cast<Dali::Internal::ModelData*>(handle.GetObjectPtr()) );
}

ModelData::~ModelData()
{
}

const std::string& ModelData::GetName() const
{
  return GetImplementation(*this).GetName();
}

void ModelData::SetRootEntity(Entity root)
{
  GetImplementation(*this).SetRootEntity(root);
}

Entity ModelData::GetRootEntity() const
{
  return GetImplementation(*this).GetRootEntity();
}

void ModelData::AddMesh(Dali::MeshData& meshPtr)
{
  GetImplementation(*this).AddMesh(meshPtr);
}

const Dali::MeshData& ModelData::GetMesh(unsigned int index) const
{
  const Internal::ModelData& modelData( GetImplementation(*this) );
  return modelData.GetMesh(index);
}

Dali::MeshData& ModelData::GetMesh(unsigned int index)
{
  Internal::ModelData& modelData( GetImplementation(*this) );
  return modelData.GetMesh(index);
}

unsigned int ModelData::NumberOfMeshes() const
{
  return GetImplementation(*this).NumberOfMeshes();
}

void ModelData::AddMaterial(Material material)
{
  GetImplementation(*this).AddMaterial(material);
}

Dali::Material ModelData::GetMaterial(unsigned int index) const
{
  return GetImplementation(*this).GetMaterial(index);
}

unsigned int ModelData::NumberOfMaterials() const
{
  return GetImplementation(*this).NumberOfMaterials();
}

ModelAnimationMapContainer& ModelData::GetAnimationMapContainer()
{
  return GetImplementation(*this).GetAnimationMapContainer();
}

const ModelAnimationMap* ModelData::GetAnimationMap (unsigned int index) const
{
  return GetImplementation(*this).GetAnimationMap(index);
}

const ModelAnimationMap* ModelData::GetAnimationMap (const std::string& name) const
{
  return GetImplementation(*this).GetAnimationMap(name);
}

bool ModelData::FindAnimation (const std::string& name, unsigned int& index) const
{
  return GetImplementation(*this).FindAnimation(name, index);
}

unsigned int ModelData::NumberOfAnimationMaps() const
{
  return GetImplementation(*this).NumberOfAnimationMaps();
}

void ModelData::AddLight(Dali::Light light)
{
  GetImplementation(*this).AddLight(light);
}

Dali::Light ModelData::GetLight(unsigned int index) const
{
  return GetImplementation(*this).GetLight(index);
}

unsigned int ModelData::NumberOfLights() const
{
  return GetImplementation(*this).NumberOfLights();
}

bool ModelData::Read(std::streambuf& buf)
{
  return GetImplementation(*this).Read(buf);
}

bool ModelData::Write(std::streambuf& buf) const
{
  return GetImplementation(*this).Write(buf);
}

} //namespace Dali
