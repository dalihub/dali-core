/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/hit-test-algorithm-impl.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-coords.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/ray-test.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/rendering/renderer-impl.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

namespace Dali::Internal::HitTestAlgorithm
{
namespace
{
enum HitType
{
  HIT_ACTOR,
  HIT_MAPPING_ACTOR
};

struct Ray
{
  Vector4 origin;
  Vector4 direction;
};

struct HitResult
{
  HitResult(Actor* actor, Vector2 hitPosition, float distance, HitType hitType, Ray ray)
  : mActor(actor),
    mHitPosition(hitPosition),
    mDistance(distance),
    mHitType(hitType),
    mRay(ray)
  {
  }

  Actor*  mActor;       ///< The actor hit (if actor is hit, then this is initialised).
  Vector2 mHitPosition; ///< Position of hit (only valid if actor valid).
  float   mDistance;
  HitType mHitType;
  Ray     mRay;
};

struct HitCommonInformation
{
  const RenderTaskList::ExclusivesContainer&  exclusives;
  std::unordered_map<uint32_t, RenderTaskPtr> mappingActors;
  LayerList&                                  layers;
};

/**
 * Creates an Actor handle so that a HitTestFunction provided via the public API can be called.
 */
struct HitTestFunctionWrapper : public HitTestInterface
{
  /**
   * Constructor
   *
   * @param[in] func HitTestFunction to call with an Actor handle.
   */
  HitTestFunctionWrapper(Dali::HitTestAlgorithm::HitTestFunction func)
  : mFunc(func)
  {
  }

  bool IsActorHittable(Actor* actor) override
  {
    return mFunc(Dali::Actor(actor), Dali::HitTestAlgorithm::CHECK_ACTOR);
  }

  bool DescendActorHierarchy(Actor* actor) override
  {
    return mFunc(Dali::Actor(actor), Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE);
  }

  bool DoesLayerConsumeHit(Layer* layer) override
  {
    // Layer::IsTouchConsumed() focuses on touch only. Here we are a wrapper for the public-api
    // where the caller may want to check for something completely different.
    // TODO: Should provide a means to let caller decide. For now do not allow layers to consume
    return false;
  }

  bool ActorRequiresHitResultCheck(Actor* actor, Vector2 hitPointLocal) override
  {
    // Geometry way does not require Hittest from the client.
    if(GetPropagationType() == Integration::Scene::TouchPropagationType::PARENT)
    {
      return actor->EmitHitTestResultSignal(GetPoint(), hitPointLocal, GetTimeStamp());
    }
    return true;
  }

  Dali::HitTestAlgorithm::HitTestFunction mFunc;
};

/**
 * Used in the hit-test algorithm to check whether the actor is touchable.
 * It is used by the touch event processor.
 */
struct ActorTouchableCheck : public HitTestInterface
{
  bool IsActorHittable(Actor* actor) override
  {
    return (actor->GetTouchRequired() || actor->GetInterceptTouchRequired() || actor->IsTouchFocusable()) && // Does the Application or derived actor type require a touch event or a intercept touch event? or focusable by touch?
           actor->IsHittable();                                                                              // Is actor sensitive, visible and on the scene?
  }

  bool DescendActorHierarchy(Actor* actor) override
  {
    return actor->IsVisible() && // Actor is visible, if not visible then none of its children are visible.
           actor->IsSensitive(); // Actor is sensitive, if insensitive none of its children should be hittable either.
  }

  bool DoesLayerConsumeHit(Layer* layer) override
  {
    return layer->IsTouchConsumed();
  }

  bool ActorRequiresHitResultCheck(Actor* actor, Vector2 hitPointLocal) override
  {
    // The Geometry way behaves like AllowedOnlyOwnTouch is enabled.
    if(GetPoint().GetState() != PointState::STARTED && (GetPropagationType() == Integration::Scene::TouchPropagationType::GEOMETRY || actor->IsAllowedOnlyOwnTouch()) && ownActor != actor)
    {
      return false;
    }
    // Geometry way does not require Hittest from the client.
    if(GetPropagationType() == Integration::Scene::TouchPropagationType::PARENT)
    {
      return actor->EmitHitTestResultSignal(GetPoint(), hitPointLocal, GetTimeStamp());
    }
    return true;
  }

  void SetOwnActor(const Actor* actor)
  {
    ownActor = actor;
  }
  const Actor* ownActor;
};

/**
 * Return true if actor is sourceActor or a descendent of sourceActor
 */
bool IsWithinSourceActors(const Actor& sourceActor, const Actor& actor)
{
  if(&sourceActor == &actor)
  {
    return true;
  }

  Actor* parent = actor.GetParent();
  if(parent)
  {
    return IsWithinSourceActors(sourceActor, *parent);
  }

  // Not within source actors
  return false;
}

/**
 * Returns true if the actor and all of the actor's parents are hittable.
 */
bool IsActorActuallyHittable(Actor* actor, HitTestInterface& hitCheck)
{
  Actor* currentActor = actor;
  // Ensure that we can descend into the layer's (or any of its parent's) hierarchy.
  while(currentActor)
  {
    if(!hitCheck.DescendActorHierarchy(currentActor))
    {
      return false;
    }
    currentActor = currentActor->GetParent();
  }

  return true;
}

/**
 * Gets the near and far clipping planes of the camera from which the scene is viewed in the render task.
 */
void GetCameraClippingPlane(RenderTask& renderTask, float& nearClippingPlane, float& farClippingPlane)
{
  CameraActor* cameraActor = renderTask.GetCameraActor();
  nearClippingPlane        = cameraActor->GetNearClippingPlane();
  farClippingPlane         = cameraActor->GetFarClippingPlane();
}

RenderTask* curRenderTask;

bool HitTestRenderTask(std::vector<std::shared_ptr<HitResult>>& hitResultList,
                       RenderTask&                              renderTask,
                       HitCommonInformation&                    hitCommonInformation,
                       const Vector2&                           screenSize,
                       Vector2                                  screenCoordinates,
                       HitTestInterface&                        hitCheck);

bool IsActorPickable(const Ray&   ray,
                     const float& projectedNearClippingDistance,
                     const float& projectedFarClippingDistance,
                     const Actor& actor)
{
  Vector2 hitPointLocal;
  float   distance;
  if(!RayTest::ActorTest(actor, ray.origin, ray.direction, hitPointLocal, distance))
  {
    return false;
  }

  if(distance < projectedNearClippingDistance || distance > projectedFarClippingDistance)
  {
    return false;
  }
  return true;
}

/**
 * Hit tests the given actor and updates the in/out variables appropriately
 */
std::shared_ptr<HitResult> HitTestActor(const Ray&            ray,
                                        const float&          projectedNearClippingDistance,
                                        const float&          projectedFarClippingDistance,
                                        HitTestInterface&     hitCheck,
                                        Actor&                actor,
                                        Dali::Layer::Behavior layerBehavior)
{
  if(!hitCheck.IsActorHittable(&actor) && !actor.IsRenderTaskMappingActor())
  {
    return nullptr;
  }

  Vector3 size = actor.GetCurrentSize();
  if(size.x <= 0.0f || size.y < 0.0f)
  {
    return nullptr;
  }

  Vector2 hitPointLocal;
  float   distance;
  if(layerBehavior == Dali::Layer::Behavior::LAYER_3D)
  {
    Vector3 hitPointLocalVector3;
    bool    hitSucceeded = RayTest::ActorBoundingBoxTest(actor, ray.origin, ray.direction, hitPointLocalVector3, distance);
    hitPointLocal        = Vector2(hitPointLocalVector3);
    if(!hitSucceeded)
    {
      return nullptr;
    }
  }
  else
  {
    if(!RayTest::SphereTest(actor, ray.origin, ray.direction))
    {
      return nullptr;
    }

    if(!RayTest::ActorTest(actor, ray.origin, ray.direction, hitPointLocal, distance))
    {
      return nullptr;
    }
  }

  if(distance < projectedNearClippingDistance || distance > projectedFarClippingDistance)
  {
    return nullptr;
  }

  if(!hitCheck.ActorRequiresHitResultCheck(&actor, hitPointLocal))
  {
    return nullptr;
  }

  std::shared_ptr<HitResult> hitResult = std::make_shared<HitResult>(&actor, hitPointLocal, distance, (actor.IsRenderTaskMappingActor()) ? HitType::HIT_MAPPING_ACTOR : HitType::HIT_ACTOR, ray);

  return hitResult;
}

bool IsActorExclusive(const Actor&                               actor,
                      const RenderTaskList::ExclusivesContainer& exclusives)

{
  auto result = std::find_if(exclusives.begin(), exclusives.end(), [&actor](const RenderTaskList::Exclusive& exclusive) { return exclusive.actor.GetActor() == &actor; });
  return (result != exclusives.end());
}

inline bool IsActorValid(Actor&                                     actor,
                         const RenderTaskList::ExclusivesContainer& exclusives,
                         HitTestInterface&                          hitCheck)
{
  if(IsActorExclusive(actor, exclusives))
  {
    return false;
  }

  if(actor.IsLayer())
  {
    return false;
  }

  if(!hitCheck.DescendActorHierarchy(&actor))
  {
    return false;
  }

  return true;
}

bool HitTestActorRecursively(std::vector<std::shared_ptr<HitResult>>& hitResultList,
                             Actor&                                   currentActor,
                             HitCommonInformation&                    hitCommonInformation,
                             const Ray&                               ray,
                             const float&                             projectedNearClippingDistance,
                             const float&                             projectedFarClippingDistance,
                             HitTestInterface&                        hitCheck,
                             Dali::Layer::Behavior                    layerBehavior,
                             bool                                     isKeepingHitTestRequired,
                             bool                                     isOverlay)
{
  if(!isOverlay && currentActor.IsOverlay())
  {
    return false;
  }

  std::shared_ptr<HitResult> hitResultOfThisActor;
  bool                       isClippingRequired = (layerBehavior != Dali::Layer::LAYER_3D) && ((currentActor.GetClippingMode() != ClippingMode::DISABLED) || (hitCheck.GetPropagationType() == Integration::Scene::TouchPropagationType::GEOMETRY));

  if(isClippingRequired)
  {
    if(!currentActor.IsLayer() && !IsActorPickable(ray, projectedNearClippingDistance, projectedFarClippingDistance, currentActor))
    {
      return false;
    }
  }

  if(currentActor.GetChildCount() > 0)
  {
    ActorContainer&                  children = currentActor.GetChildrenInternal();
    ActorContainer::reverse_iterator endIter  = children.rend();
    for(ActorContainer::reverse_iterator iter = children.rbegin(); endIter != iter; ++iter)
    {
      Actor& childActor = *((*iter).Get());
      if(!IsActorValid(childActor, hitCommonInformation.exclusives, hitCheck))
      {
        continue;
      }

      bool isHit = HitTestActorRecursively(hitResultList, childActor, hitCommonInformation, ray, projectedNearClippingDistance, projectedFarClippingDistance, hitCheck, layerBehavior, isKeepingHitTestRequired, isOverlay);
      if(isKeepingHitTestRequired)
      {
        continue;
      }

      if(isHit)
      {
        return true;
      }
    }
  }

  hitResultOfThisActor = HitTestActor(ray, projectedNearClippingDistance, projectedFarClippingDistance, hitCheck, currentActor, layerBehavior);
  if(!hitResultOfThisActor)
  {
    return false;
  }

  RenderTaskPtr fboRenderTask = nullptr;
  if(hitResultOfThisActor->mHitType == HitType::HIT_MAPPING_ACTOR)
  {
    auto iter = hitCommonInformation.mappingActors.find(hitResultOfThisActor->mActor->GetId());
    if(iter != hitCommonInformation.mappingActors.end())
    {
      fboRenderTask = iter->second;
    }
    else
    {
      hitResultOfThisActor->mHitType = HitType::HIT_ACTOR;
    }
  }

  if(fboRenderTask)
  {
    std::vector<std::shared_ptr<HitResult>> frameBufferHitResultList;
    Vector2                                 hitPosition = hitResultOfThisActor->mHitPosition;
    Vector2                                 screenSize  = hitResultOfThisActor->mActor->GetCurrentSize().GetVectorXY();
    if(!HitTestRenderTask(frameBufferHitResultList, *fboRenderTask, hitCommonInformation, screenSize, hitPosition, hitCheck))
    {
      return false;
    }

    // Every FBO hit result should have same distance of this camera.
    for(auto&& hitResult : frameBufferHitResultList)
    {
      hitResult->mDistance = hitResultOfThisActor->mDistance;
      hitResultList.push_back(hitResult);
    }
  }
  else
  {
    hitResultList.push_back(hitResultOfThisActor);
  }

  return true;
}

void RetrieveValidActorTrees(std::vector<Actor*>&                       validActorRoots,
                             Actor&                                     parentActor,
                             const RenderTaskList::ExclusivesContainer& exclusives,
                             HitTestInterface&                          hitCheck,
                             const Ray&                                 ray,
                             const float&                               projectedNearClippingDistance,
                             const float&                               projectedFarClippingDistance)
{
  if(parentActor.GetChildCount() == 0)
  {
    return;
  }

  if((parentActor.GetClippingMode() == ClippingMode::CLIP_CHILDREN) &&
     !IsActorPickable(ray, projectedNearClippingDistance, projectedFarClippingDistance, parentActor))
  {
    return;
  }

  ActorContainer&          children = parentActor.GetChildrenInternal();
  ActorContainer::iterator endIter  = children.end();
  for(ActorContainer::iterator iter = children.begin(); endIter != iter; ++iter)
  {
    Actor& childActor = *((*iter).Get());
    if(childActor.IsLayer())
    {
      continue;
    }

    if(childActor.IsOverlay())
    {
      bool   valid       = true;
      Actor* currentNode = &childActor;
      // Should not make valid to false for scene root layer.
      while(currentNode && currentNode->GetParent())
      {
        if(!IsActorValid(*currentNode, exclusives, hitCheck))
        {
          valid = false;
          break;
        }
        currentNode = currentNode->GetParent();
      }
      if(valid)
      {
        validActorRoots.push_back(&childActor);
      }
    }
    else
    {
      RetrieveValidActorTrees(validActorRoots, childActor, exclusives, hitCheck, ray, projectedNearClippingDistance, projectedFarClippingDistance);
    }
  }
}

bool HitTestWithinSubTree(std::vector<std::shared_ptr<HitResult>>& hitResultList,
                          Actor&                                   actor,
                          HitCommonInformation&                    hitCommonInformation,
                          const Ray&                               ray,
                          const float&                             projectedNearClippingDistance,
                          const float&                             projectedFarClippingDistance,
                          HitTestInterface&                        hitCheck,
                          Dali::Layer::Behavior                    layerBehavior)
{
  std::vector<Actor*> validActorRoots;
  validActorRoots.push_back(&actor);

  if(actor.GetScene().HasOverlayContent())
  {
    RetrieveValidActorTrees(validActorRoots, actor, hitCommonInformation.exclusives, hitCheck, ray, projectedNearClippingDistance, projectedFarClippingDistance);
  }

  bool isKeepingHitTestRequired = (hitCheck.GetPropagationType() == Integration::Scene::TouchPropagationType::GEOMETRY) || (layerBehavior == Dali::Layer::Behavior::LAYER_3D);

  auto endIter = validActorRoots.rend();
  for(auto iter = validActorRoots.rbegin(); endIter != iter; ++iter)
  {
    if(HitTestActorRecursively(hitResultList, *(*iter), hitCommonInformation, ray, projectedNearClippingDistance, projectedFarClippingDistance, hitCheck, layerBehavior, isKeepingHitTestRequired, (*iter)->IsOverlay()))
    {
      break;
    }
  }

  if(hitResultList.empty())
  {
    return false;
  }

  if(layerBehavior == Dali::Layer::Behavior::LAYER_3D)
  {
    std::stable_sort(hitResultList.begin(), hitResultList.end(), [](std::shared_ptr<HitResult> first, std::shared_ptr<HitResult> second) {
                        if(std::abs(first->mDistance - second->mDistance) < Dali::Epsilon<1000>::value)
                        {
                          return first->mActor->GetSortingDepth() > second->mActor->GetSortingDepth();
                        }
                        else
                        {
                          return first->mDistance < second->mDistance;
                        } });
  }

  if(hitCheck.GetPropagationType() == Integration::Scene::TouchPropagationType::PARENT)
  {
    hitResultList.resize(1);
  }

  return true;
}

/**
 * Hit test a RenderTask
 */
bool HitTestRenderTask(std::vector<std::shared_ptr<HitResult>>& hitResultList,
                       RenderTask&                              renderTask,
                       HitCommonInformation&                    hitCommonInformation,
                       const Vector2&                           screenSize,
                       Vector2                                  screenCoordinates,
                       HitTestInterface&                        hitCheck)
{
  curRenderTask = &renderTask;
  if(!renderTask.IsInputAvailable())
  {
    return false;
  }

  Actor* sourceActor(renderTask.GetSourceActor());
  if(!sourceActor)
  {
    return false;
  }

  Dali::Layer sourceLayer(sourceActor->GetLayer());
  if(!sourceLayer)
  {
    return false;
  }

  Ray ray;
  if(!renderTask.GetCameraActor()->BuildPickingRay(screenCoordinates, screenSize, ray.origin, ray.direction, !!renderTask.GetFrameBuffer()))
  {
    return false;
  }

  float nearClippingPlane, farClippingPlane;
  GetCameraClippingPlane(renderTask, nearClippingPlane, farClippingPlane);

  // Recompute near and far clipping plane distance with the picking ray.
  Vector3 centerDirection = renderTask.GetCameraActor()->GetCurrentWorldOrientation().Rotate(Dali::Vector3::ZAXIS);
  centerDirection.Normalize();
  float projectionFactor              = centerDirection.Dot(Vector3(ray.direction));
  float projectedNearClippingDistance = nearClippingPlane / projectionFactor;
  float projectedFarClippingDistance  = farClippingPlane / projectionFactor;

  Vector2 hitPosition;
  for(int32_t i = hitCommonInformation.layers.GetLayerCount() - 1; i >= 0; --i)
  {
    Layer* layer(hitCommonInformation.layers.GetLayer(i));
    Actor* testRootActor = (sourceLayer == layer) ? sourceActor : ((IsWithinSourceActors(*sourceActor, *layer)) ? layer : nullptr);
    if(!testRootActor)
    {
      continue;
    }

    // If source Actor is been exclusive by other RenderTask, skip it.
    if(IsActorExclusive(*testRootActor, hitCommonInformation.exclusives) && (!renderTask.IsExclusive() || testRootActor != sourceActor))
    {
      continue;
    }

    if(!IsActorActuallyHittable(testRootActor, hitCheck))
    {
      continue;
    }

    std::vector<std::shared_ptr<HitResult>> subTreeHitResultList;
    bool                                    isHit = HitTestWithinSubTree(subTreeHitResultList,
                                      *testRootActor,
                                      hitCommonInformation,
                                      ray,
                                      projectedNearClippingDistance,
                                      projectedFarClippingDistance,
                                      hitCheck,
                                      layer->GetBehavior());

    if(!isHit && hitCheck.DoesLayerConsumeHit(layer))
    {
      hitResultList.push_back(std::make_shared<HitResult>(layer, Vector2(0.0f, 0.0f), 0.0f, HitType::HIT_ACTOR, ray));
      break;
    }

    hitResultList.insert(hitResultList.end(), subTreeHitResultList.begin(), subTreeHitResultList.end());

    if(isHit && hitCheck.GetPropagationType() == Integration::Scene::TouchPropagationType::PARENT)
    {
      break;
    }
  }

  if(hitResultList.empty())
  {
    return false;
  }

  return true;
}

bool ConvertScreenCoordinates(Vector2& convertedScreenCoordinates, const Vector2& screenCoordinates, const RenderTask& targetRenderTask)
{
  Viewport viewport;
  targetRenderTask.GetViewport(viewport);

  if(screenCoordinates.x < static_cast<float>(viewport.x) ||
     screenCoordinates.x > static_cast<float>(viewport.x + viewport.width) ||
     screenCoordinates.y < static_cast<float>(viewport.y) ||
     screenCoordinates.y > static_cast<float>(viewport.y + viewport.height))
  {
    // The screen coordinate is outside the viewport of render task. The viewport clips all layers.
    return false;
  }
  convertedScreenCoordinates = screenCoordinates - Vector2(static_cast<float>(viewport.x), static_cast<float>(viewport.y));

  return true;
}

void CollectMappingActors(RenderTaskList::RenderTaskContainer& tasks, std::unordered_map<uint32_t, RenderTaskPtr>& mappingActors)
{
  RenderTaskList::RenderTaskContainer::reverse_iterator endIter = tasks.rend();
  for(RenderTaskList::RenderTaskContainer::reverse_iterator iter = tasks.rbegin(); endIter != iter; ++iter)
  {
    RenderTask& renderTask = *iter->Get();
    if(renderTask.GetFrameBuffer() && renderTask.GetScreenToFrameBufferMappingActor())
    {
      uint32_t mappingActorId       = renderTask.GetScreenToFrameBufferMappingActor().GetProperty<int32_t>(Dali::Actor::Property::ID);
      mappingActors[mappingActorId] = RenderTaskPtr(&renderTask);
    }
  }
}

/**
 * Iterate through the RenderTaskList and perform hit testing.
 *
 * @param[in] sceneSize The scene size the tests will be performed in
 * @param[in] layers The list of layers to test
 * @param[in] taskList The list of render tasks
 * @param[out] results Ray information calculated by the camera
 * @param[in] hitCheck The hit testing interface object to use
 * @param[in] propagationType Whether the scene using geometry event propagation touch and hover events.
 * @return True if we have a hit, false otherwise
 */
bool HitTestRenderTaskList(const Vector2&                                 sceneSize,
                           RenderTaskList&                                taskList,
                           LayerList&                                     layers,
                           const Vector2&                                 screenCoordinates,
                           HitTestInterface&                              hitCheck,
                           const Integration::Scene::TouchPropagationType propagationType,
                           Results&                                       results)
{
  bool                                 isHitSucceeded = false;
  RenderTaskList::RenderTaskContainer& tasks          = taskList.GetTasks();
  const auto&                          exclusives     = taskList.GetExclusivesList();

  HitCommonInformation hitCommonInformation{exclusives, std::unordered_map<uint32_t, RenderTaskPtr>(), layers};
  CollectMappingActors(tasks, hitCommonInformation.mappingActors);

  RenderTaskList::RenderTaskContainer::reverse_iterator endIter = tasks.rend();
  for(RenderTaskList::RenderTaskContainer::reverse_iterator iter = tasks.rbegin(); endIter != iter; ++iter)
  {
    RenderTask& renderTask = *iter->Get();
    if(renderTask.GetFrameBuffer() && renderTask.GetScreenToFrameBufferMappingActor())
    {
      continue;
    }

    // Don't need to consider a RenderTask draws on window and its source actor is inside FBO.
    // Can't support it.
    Viewport                                viewport;
    std::vector<std::shared_ptr<HitResult>> hitResults;
    Vector2                                 convertedScreenCoordinates;
    renderTask.GetViewport(viewport);
    if(ConvertScreenCoordinates(convertedScreenCoordinates, screenCoordinates, renderTask) &&
       HitTestRenderTask(hitResults, renderTask, hitCommonInformation, Vector2(viewport.width, viewport.height), convertedScreenCoordinates, hitCheck))
    {
      results.actor            = Dali::Actor(hitResults.front()->mActor);
      results.renderTask       = RenderTaskPtr(&renderTask);
      results.actorCoordinates = hitResults.front()->mHitPosition;
      results.rayOrigin        = hitResults.front()->mRay.origin;
      results.rayDirection     = hitResults.front()->mRay.direction;

      if(hitCheck.GetPropagationType() == Integration::Scene::TouchPropagationType::GEOMETRY)
      {
        std::vector<std::shared_ptr<HitResult>>::reverse_iterator endIter = hitResults.rend();
        for(std::vector<std::shared_ptr<HitResult>>::reverse_iterator iter = hitResults.rbegin(); endIter != iter; ++iter)
        {
          results.actorLists.push_back(iter->get()->mActor);
        }
      }
      isHitSucceeded = true;
      break;
    }
  }
  return isHitSucceeded;
}

} // unnamed namespace

HitTestInterface::~HitTestInterface() = default;

bool HitTest(const Vector2& sceneSize, RenderTaskList& renderTaskList, LayerList& layerList, const Vector2& screenCoordinates, Dali::HitTestAlgorithm::Results& results, Dali::HitTestAlgorithm::HitTestFunction func, const Integration::Scene::TouchPropagationType propagationType)
{
  bool wasHit(false);
  // Hit-test the regular on-scene actors
  Results hitTestResults;
  hitTestResults.eventTime = 0u; ///< Unused

  HitTestFunctionWrapper hitTestFunctionWrapper(func);
  // TODO : It looks better to do this before hit test is required.
  hitTestFunctionWrapper.SetPoint(hitTestResults.point);
  hitTestFunctionWrapper.SetTimeStamp(hitTestResults.eventTime);
  hitTestFunctionWrapper.SetPropagationType(propagationType);
  if(HitTestRenderTaskList(sceneSize, renderTaskList, layerList, screenCoordinates, hitTestFunctionWrapper, propagationType, hitTestResults))
  {
    results.actor            = hitTestResults.actor;
    results.actorCoordinates = hitTestResults.actorCoordinates;
    wasHit                   = true;
  }
  return wasHit;
}

bool HitTest(const Vector2& sceneSize, RenderTaskList& renderTaskList, LayerList& layerList, const Vector2& screenCoordinates, Results& results, HitTestInterface& hitTestInterface, const Integration::Scene::TouchPropagationType propagationType)
{
  // TODO : It looks better to do this before hit test is required.
  hitTestInterface.SetPoint(results.point);
  hitTestInterface.SetTimeStamp(results.eventTime);
  hitTestInterface.SetPropagationType(propagationType);

  // Hit-test the regular on-scene actors
  return HitTestRenderTaskList(sceneSize, renderTaskList, layerList, screenCoordinates, hitTestInterface, propagationType, results);
}

bool HitTest(const Vector2& sceneSize, RenderTaskList& renderTaskList, LayerList& layerList, const Vector2& screenCoordinates, Results& results, const Actor* ownActor, const Integration::Scene::TouchPropagationType propagationType)
{
  ActorTouchableCheck actorTouchableCheck;
  actorTouchableCheck.SetOwnActor(ownActor);
  return HitTest(sceneSize, renderTaskList, layerList, screenCoordinates, results, actorTouchableCheck, propagationType);
}

} // namespace Dali::Internal::HitTestAlgorithm
