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
#include <dali/internal/event/actors/mesh-actor-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/modeling/model-impl.h>
#include <dali/internal/event/modeling/animatable-mesh-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>

namespace Dali
{

using Internal::MeshIter;
using Internal::MeshIPtr;
using Internal::MaterialIPtr;

namespace Internal
{


namespace
{

BaseHandle Create()
{
  return Dali::MeshActor::New();
}

TypeRegistration mType( typeid(Dali::MeshActor), typeid(Dali::RenderableActor), Create );

}

MeshActorPtr MeshActor::New()
{
  MeshActorPtr actor(new MeshActor());

  // Second-phase construction
  actor->Initialize();

  // Create the attachment
  actor->mMeshAttachment = MeshAttachment::New( *actor->mNode );
  actor->Attach(*actor->mMeshAttachment);
  actor->SetCullFace( Dali::CullBack );

  return actor;
}

MeshActorPtr MeshActor::New(Dali::Mesh mesh)
{
  MeshActorPtr actor = MeshActor::New();

  if(actor)
  {
    actor->SetMesh(mesh);
  }

  return actor;
}

MeshActorPtr MeshActor::New(Dali::AnimatableMesh mesh)
{
  MeshActorPtr actor = MeshActor::New();

  if(actor)
  {
    actor->SetMesh(mesh);
  }

  return actor;
}


MeshActorPtr MeshActor::New(ModelDataPtr modelData, Dali::Entity entity)
{
  MeshActorPtr actor(new MeshActor());
  // Second-phase construction
  actor->Initialize();

  // Create the attachment
  actor->mMeshAttachment = MeshAttachment::New( *actor->mNode );
  actor->Attach(*actor->mMeshAttachment);
  actor->SetName(entity.GetName());
  actor->SetCullFace( Dali::CullBack );

  DALI_ASSERT_ALWAYS(entity.NumberOfMeshes() == 1 && "Dali does not support multiple meshes per node in the model");

  actor->SetMesh(modelData, entity.GetMeshByIndex(0)); // only use the first mesh

  Matrix transform(entity.GetTransformMatrix());
  Vector3 position;
  Quaternion rotation;
  Vector3 scale;
  transform.GetTransformComponents(position, rotation, scale);
  actor->SetPosition(position);
  actor->SetRotation(rotation);
  actor->SetScale(scale);

  return actor;
}

MeshActor::MeshActor()
: RenderableActor()
{
}

MeshActor::~MeshActor()
{
}

void MeshActor::SetMesh(Dali::Mesh mesh)
{
  SetMesh( &GetImplementation(mesh) );
}

void MeshActor::SetMesh(Dali::AnimatableMesh mesh)
{
  SetMesh( GetImplementation(mesh).GetMesh() );

  SetInitialVolume(Vector3(1.0f, 1.0f, 1.0f));
}

void MeshActor::SetMesh( MeshIPtr meshPtr )
{
  mMeshAttachment->SetMesh( meshPtr, meshPtr->GetResourceId(), meshPtr->GetBones(),  meshPtr->GetMaterial() );
}

// Used by Internal::ModelActorFactory
void MeshActor::SetMesh(ModelDataPtr modelData, unsigned int meshIndex)
{
  ResourceTicketPtr meshTicket = modelData->GetMeshTicket(meshIndex);
  const Dali::MeshData& meshData = modelData->GetMesh(meshIndex);
  Dali::Material material = meshData.GetMaterial();
  DALI_ASSERT_ALWAYS( material && "No material found" );
  MaterialIPtr matPtr = &GetImplementation(material);

  mMeshAttachment->SetMesh(meshTicket, meshData.GetBones(), matPtr);
}

void MeshActor::SetMaterial(const Dali::Material material)
{
  MaterialIPtr materialPtr = const_cast<Internal::Material*>(&GetImplementation(material));
  mMeshAttachment->SetMaterial(materialPtr);
}

Dali::Material MeshActor::GetMaterial() const
{
  Dali::Material material;

  Internal::MaterialIPtr internalPtr = mMeshAttachment->GetMaterial( );

  if(internalPtr)
  {
    material = Dali::Material(internalPtr.Get());
  }

  return material;
}

void MeshActor::SetAffectedByLighting(bool affectedByLighting)
{
  mMeshAttachment->SetAffectedByLighting( affectedByLighting );
}

bool MeshActor::IsAffectedByLighting()
{
  return mMeshAttachment->IsAffectedByLighting();
}

void MeshActor::BindBonesToMesh(Internal::ActorPtr rootActor)
{
  mMeshAttachment->BindBonesToMesh(rootActor);
}

} // namespace Internal

} // namespace Dali
