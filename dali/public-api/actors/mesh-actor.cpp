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
#include <dali/public-api/actors/mesh-actor.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/mesh-actor-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/event/modeling/animatable-mesh-impl.h>

namespace Dali
{

MeshActor::MeshActor()
{
}

MeshActor MeshActor::New()
{
  Internal::MeshActorPtr internal = Internal::MeshActor::New();
  return MeshActor(internal.Get());
}

MeshActor MeshActor::New( Mesh mesh)
{
  Internal::MeshActorPtr internal = Internal::MeshActor::New(mesh);
  return MeshActor(internal.Get());
}

MeshActor MeshActor::New( AnimatableMesh mesh)
{
  Internal::MeshActorPtr internal = Internal::MeshActor::New(mesh);
  return MeshActor(internal.Get());
}

MeshActor MeshActor::DownCast( BaseHandle handle )
{
  return MeshActor( dynamic_cast<Dali::Internal::MeshActor*>(handle.GetObjectPtr()) );
}

MeshActor::~MeshActor()
{
}

MeshActor::MeshActor(const MeshActor& copy)
: RenderableActor(copy)
{
}

MeshActor& MeshActor::operator=(const MeshActor& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void MeshActor::SetMaterial(
  Actor               actor,
  const std::string&  actorName,
  Material            material)
{
  Actor child = actor.FindChildByName(actorName);
  if(child)
  {
    Internal::MeshActor* meshActor = dynamic_cast<Internal::MeshActor*>(&GetImplementation(child));
    if(meshActor)
    {
      meshActor->SetMaterial(material);
    }
  }
  else
  {
    DALI_LOG_ERROR("MeshActor::SetMaterial() - Can't find mesh actor\n");
  }
}

void MeshActor::SetMaterial(Material material)
{
  GetImplementation(*this).SetMaterial(material);
}

Material MeshActor::GetMaterial() const
{
  return GetImplementation(*this).GetMaterial();
}

void MeshActor::BindBonesToMesh(Actor rootActor)
{
  Internal::ActorPtr root(&GetImplementation(rootActor));

  GetImplementation(*this).BindBonesToMesh(root);
}

MeshActor::MeshActor(Internal::MeshActor* internal)
  : RenderableActor(internal)
{
}

} // namespace Dali
