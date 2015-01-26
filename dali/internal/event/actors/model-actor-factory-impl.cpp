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
#include <dali/internal/event/actors/model-actor-factory-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/modeling/entity.h>
#include <dali/internal/event/actors/mesh-actor-impl.h>
#include <dali/internal/event/modeling/model-impl.h>
#include <dali/internal/event/modeling/model-data-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/event/animation/animator-connector.h>


namespace Dali
{
using Dali::Entity;
using Internal::MeshIter;
using Internal::ActorPtr;

namespace Internal
{

ActorPtr ModelActorFactory::BuildActorTree(Dali::Model& model, const std::string& entityName)
{
  Dali::Entity entity;
  ActorPtr actorPtr;

  Internal::ModelDataPtr modelData(GetImplementation(model).GetModelData());

  if (modelData)
  {
    Dali::Entity root = modelData->GetRootEntity();
    if (entityName.empty())
    {
      entity = root;
    }
    else
    {
      entity = root.Find(entityName);
    }
  }

  if(entity)
  {
    actorPtr = RecurseNew(modelData, entity);
  }
  else
  {
    DALI_LOG_INFO(Debug::Filter::gModel, Debug::General, "Entity %s not found\n", entityName.c_str());
  }

  if(actorPtr)
  {
    Vector4 bounds(entity.GetUpperBounds() - entity.GetLowerBounds());
    Vector3 initialVolume(bounds.x, bounds.y, bounds.z);

    actorPtr->SetInitialVolume(initialVolume);
    actorPtr->SetTransmitGeometryScaling(true);

    BindBonesToMeshActors(actorPtr, actorPtr);
  }
  return actorPtr;
}


ActorPtr ModelActorFactory::RecurseNew(ModelDataPtr modelData, Dali::Entity entity)
{
  ActorPtr actorPtr;
  if(entity.HasMeshes())
  {
    actorPtr = Internal::MeshActor::New(modelData, entity);
  }
  else
  {
    // Root with no mesh, or bone/joint actor (with no mesh)
    actorPtr = Internal::Actor::New();
    actorPtr->SetName(entity.GetName());
    Matrix transform(entity.GetTransformMatrix());
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;
    transform.GetTransformComponents(position, rotation, scale);
    actorPtr->SetPosition(position);
    actorPtr->SetRotation(rotation);
    actorPtr->SetScale(scale);
  }

  actorPtr->SetParentOrigin(ParentOrigin::CENTER);
  actorPtr->SetAnchorPoint(AnchorPoint::CENTER);

  if (entity.HasChildren())
  {
    for (EntityConstIter iter = entity.GetChildren().begin(); iter != entity.GetChildren().end(); ++iter)
    {
      Dali::Entity childEntity = (*iter);
      ActorPtr child = RecurseNew(modelData, childEntity);
      actorPtr->Add(*child.Get());
    }
  }

  return actorPtr;
}

void ModelActorFactory::BindBonesToMeshActors(ActorPtr rootActor, ActorPtr actorPtr)
{
  MeshActor* meshActor = dynamic_cast<MeshActor*>(actorPtr.Get());
  if(meshActor)
  {
    meshActor->BindBonesToMesh(rootActor);
  }

  // Descend to all child actors, not just mesh actors
  const ActorContainer& children = actorPtr->GetChildren();
  for ( ActorConstIter iter = children.begin(); iter != children.end(); ++iter)
  {
    ActorPtr childActor = const_cast<Actor*>(&GetImplementation(*iter));
    BindBonesToMeshActors(rootActor, childActor);
  }
}



AnimationPtr ModelActorFactory::BuildAnimation(
  Model& model,
  Actor& rootActor,
  size_t index)
{
  AnimationPtr animation;
  Internal::ModelDataPtr modelData(model.GetModelData());

  if (modelData)
  {
    if (index >= modelData->NumberOfAnimationMaps())
    {
      DALI_LOG_ERROR("Invalid animation index\n");
    }
    else
    {
      const ModelAnimationMap* animationData(modelData->GetAnimationMap(index));
      if( animationData != NULL )
      {
        animation = CreateAnimation(rootActor, animationData, AlphaFunctions::Linear, animationData->duration);
      }
    }
  }
  return animation;
}

AnimationPtr ModelActorFactory::BuildAnimation(
  Model& model,
  Actor& rootActor,
  size_t index,
  float durationSeconds)
{
  AnimationPtr animation;
  Internal::ModelDataPtr modelData(model.GetModelData());

  if (modelData)
  {
    if (index >= modelData->NumberOfAnimationMaps())
    {
      DALI_LOG_ERROR("Invalid animation index\n");
    }
    else
    {
      const ModelAnimationMap* animationData(modelData->GetAnimationMap(index));
      if( animationData != NULL )
      {
        animation = CreateAnimation(rootActor, animationData, AlphaFunctions::Linear, durationSeconds);
      }
    }
  }
  return animation;
}

AnimationPtr ModelActorFactory::BuildAnimation(
  Model& model,
  Actor& rootActor,
  size_t index,
  AlphaFunction alpha)
{
  AnimationPtr animation;
  Internal::ModelDataPtr modelData(model.GetModelData());

  if (modelData)
  {
    if (index >= modelData->NumberOfAnimationMaps())
    {
      DALI_LOG_ERROR("Invalid animation index\n");
    }
    else
    {
      const ModelAnimationMap* animationData(modelData->GetAnimationMap(index));
      if( animationData != NULL )
      {
        animation = CreateAnimation(rootActor, animationData, alpha, animationData->duration);
      }
    }
  }
  return animation;
}


AnimationPtr ModelActorFactory::BuildAnimation(
  Model& model,
  Actor& rootActor,
  size_t index,
  AlphaFunction alpha,
  float durationSeconds)
{
  AnimationPtr animation;
  Internal::ModelDataPtr modelData(model.GetModelData());

  if (modelData)
  {
    if (index >= modelData->NumberOfAnimationMaps())
    {
      DALI_LOG_ERROR("Invalid animation index\n");
    }
    else
    {
      const ModelAnimationMap* animationData(modelData->GetAnimationMap(index));
      if( animationData != NULL )
      {
        animation = CreateAnimation(rootActor, animationData, alpha, durationSeconds);
      }
    }
  }
  return animation;
}




AnimationPtr ModelActorFactory::CreateAnimation(
  Actor& rootActor,
  const ModelAnimationMap* animationData,
  AlphaFunction alpha,
  float durationSeconds)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  AnimationPtr animation(Animation::New(durationSeconds));
  animation->SetDefaultAlphaFunction(alpha);

  for(EntityAnimatorMapIter it = animationData->animators.begin(); it != animationData->animators.end(); ++it)
  {
    const EntityAnimatorMap& entityAnimator(*it);

    // find actor for this animator
    ActorPtr animatedActor = rootActor.FindChildByName(entityAnimator.GetEntityName());
    if (!animatedActor)
    {
      // If we can't find the actor, it may not have been instantiated, may
      // be a sibling or parent of rootActor or may have been removed.
      continue;
    }

    Dali::Actor targetActor(animatedActor.Get());

    Dali::KeyFrames posKFHandle = entityAnimator.GetPositionKeyFrames();
    if(posKFHandle)
    {
      const KeyFrames& positionKeyFrames = GetImplementation(posKFHandle);
      if(positionKeyFrames.GetKeyFramesBase()->GetNumberOfKeyFrames() > 0)
      {
        animation->AnimateBetween(Property(targetActor, Dali::Actor::POSITION),
                                  positionKeyFrames, alpha, durationSeconds);
      }
    }

    Dali::KeyFrames scaleKFHandle = entityAnimator.GetScaleKeyFrames();
    if(scaleKFHandle)
    {
      const KeyFrames& scaleKeyFrames    = GetImplementation(scaleKFHandle);
      if(scaleKeyFrames.GetKeyFramesBase()->GetNumberOfKeyFrames() > 0)
      {
        animation->AnimateBetween(Property(targetActor, Dali::Actor::SCALE),    scaleKeyFrames, alpha, durationSeconds);
      }
    }

    Dali::KeyFrames rotationKFHandle = entityAnimator.GetRotationKeyFrames();
    if(rotationKFHandle)
    {
      const KeyFrames& rotationKeyFrames = GetImplementation(rotationKFHandle);
      if(rotationKeyFrames.GetKeyFramesBase()->GetNumberOfKeyFrames() > 0)
      {
        animation->AnimateBetween(Property(targetActor, Dali::Actor::ROTATION), rotationKeyFrames, alpha, durationSeconds);
      }
    }
  }
  return animation;
}



} // namespace Internal
} // namespace Dali
