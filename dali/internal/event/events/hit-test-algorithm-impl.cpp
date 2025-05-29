/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/events/ray-test.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/rendering/renderer-impl.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Internal
{
namespace HitTestAlgorithm
{
namespace
{
struct HitActor
{
  HitActor()
  : actor(nullptr),
    distance(std::numeric_limits<float>::max()),
    depth(std::numeric_limits<int>::min())
  {
  }

  Actor*  actor;       ///< The actor hit (if actor is hit, then this is initialised).
  Vector2 hitPosition; ///< Position of hit (only valid if actor valid).
  float   distance;    ///< Distance from ray origin to hit actor.
  int32_t depth;       ///< Depth index of this actor.
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

  bool ActorRequiresHitResultCheck(Actor* actor, Integration::Point point, Vector2 hitPointLocal, uint32_t timeStamp) override
  {
    return actor->EmitHitTestResultSignal(point, hitPointLocal, timeStamp);
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

  bool ActorRequiresHitResultCheck(Actor* actor, Integration::Point point, Vector2 hitPointLocal, uint32_t timeStamp) override
  {
    if(point.GetState() != PointState::STARTED && actor->IsAllowedOnlyOwnTouch() && ownActor != actor)
    {
      return false;
    }
    return actor->EmitHitTestResultSignal(point, hitPointLocal, timeStamp);
  }

  void SetOwnActor(const Actor* actor)
  {
    ownActor = actor;
  }
  const Actor* ownActor;
};

/**
 * Check to see if the actor we're about to hit test is exclusively owned by another rendertask?
 */
bool IsActorExclusiveToAnotherRenderTask(const Actor&                               actor,
                                         const RenderTask&                          renderTask,
                                         const RenderTaskList::ExclusivesContainer& exclusives)

{
  bool exclusiveByOtherTask = false;
  if(exclusives.size())
  {
    for(const auto& exclusive : exclusives)
    {
      if(exclusive.actor.GetActor() == &actor)
      {
        if(exclusive.renderTaskPtr != &renderTask)
        {
          exclusiveByOtherTask = true;
        }
        else
        {
          // Fast-out if render task is itself
          return false;
        }
      }
    }
  }
  return exclusiveByOtherTask;
}

/**
 * Hit tests the given actor and updates the in/out variables appropriately
 */
void HitTestActor(const RenderTask&         renderTask,
                  const Vector4&            rayOrigin,
                  const Vector4&            rayDir,
                  const float&              nearClippingPlane,
                  const float&              farClippingPlane,
                  HitTestInterface&         hitCheck,
                  RayTest&                  rayTest,
                  const Integration::Point& point,
                  const uint32_t            eventTime,
                  bool                      clippingActor,
                  bool                      overlayedActor,
                  Actor&                    actor,
                  bool&                     overlayHit,
                  HitActor&                 hit)
{
  if(clippingActor || hitCheck.IsActorHittable(&actor))
  {
    Vector3 size(actor.GetCurrentSize());

    // Ensure the actor has a valid size.
    // If so, perform a quick ray sphere test to see if our ray is close to the actor.
    if(size.x > 0.0f && size.y > 0.0f && rayTest.SphereTest(actor, rayOrigin, rayDir))
    {
      Vector2 hitPointLocal;
      float   distance;

      Vector3 hitPointLocal3D;
      Dali::Layer::Behavior layerBehavior = actor.GetLayer().GetProperty<Dali::Layer::Behavior>(Dali::Layer::Property::BEHAVIOR);

      bool isHitted = false;
      if(layerBehavior == Dali::Layer::Behavior::LAYER_3D)
      {
        isHitted = rayTest.ActorBoundingBoxTest(actor, rayOrigin, rayDir, hitPointLocal3D, distance);
      }
      else
      {
        isHitted = rayTest.ActorTest(actor, rayOrigin, rayDir, hitPointLocal, distance);
      }

      // Finally, perform a more accurate ray test to see if our ray actually hits the actor.
      if(isHitted)
      {
        // Calculate z coordinate value in Camera Space.
        const Matrix&  viewMatrix          = renderTask.GetCameraActor()->GetViewMatrix();
        const Vector4& hitDir              = Vector4(rayDir.x * distance, rayDir.y * distance, rayDir.z * distance, 0.0f);
        const float    cameraDepthDistance = (viewMatrix * hitDir).z;

        // Check if cameraDepthDistance is between clipping plane
        if(cameraDepthDistance >= nearClippingPlane && cameraDepthDistance <= farClippingPlane)
        {
          if(overlayHit && !overlayedActor)
          {
            // If we have already hit an overlay and current actor is not an overlay ignore current actor.
          }
          else
          {
            if(overlayedActor)
            {
              overlayHit = true;
            }

            // If the hit actor does not want to hit, the hit-test continues.
            if(hitCheck.ActorRequiresHitResultCheck(&actor, point, hitPointLocal, eventTime))
            {
              hit.actor       = &actor;
              hit.hitPosition = hitPointLocal;
              hit.distance    = distance;
              hit.depth       = actor.GetSortingDepth();

              if(actor.GetRendererCount() > 0)
              {
                //Get renderer with maximum depth
                int rendererMaxDepth(actor.GetRendererAt(0).Get()->GetDepthIndex());
                for(uint32_t i(1); i < actor.GetRendererCount(); ++i)
                {
                  int depth = actor.GetRendererAt(i).Get()->GetDepthIndex();
                  if(depth > rendererMaxDepth)
                  {
                    rendererMaxDepth = depth;
                  }
                }
                hit.depth += rendererMaxDepth;
              }
            }
          }
        }
      }
    }
  }
}

/**
 * When iterating through the children of an actor, this method updates the child-hit-data.
 */
void UpdateChildHitData(const HitActor& hit, const HitActor& currentHit, const bool layerIs3d, const bool parentIsRenderable, HitActor& childHit)
{
  bool updateChildHit = false;
  if(currentHit.distance >= 0.0f)
  {
    if(layerIs3d)
    {
      updateChildHit = ((currentHit.depth > childHit.depth) ||
                        ((currentHit.depth == childHit.depth) && (currentHit.distance < childHit.distance)));
    }
    else
    {
      updateChildHit = currentHit.depth >= childHit.depth;
    }
  }

  if(updateChildHit)
  {
    if(!parentIsRenderable || currentHit.depth > hit.depth ||
       (layerIs3d && (currentHit.depth == hit.depth && currentHit.distance < hit.distance)))
    {
      childHit = currentHit;
    }
  }
}

/**
 * Recursively hit test all the actors, without crossing into other layers.
 * This algorithm performs a Depth-First-Search (DFS) on all Actors within Layer.
 * Hit-Testing each Actor, noting the distance from the Ray-Origin (3D origin
 * of touch vector). The closest Hit-Tested Actor is that which is returned.
 * Exceptions to this rule are:
 * - When comparing against renderable parents, if Actor is the same distance
 * or closer than it's renderable parent, then it takes priority.
 */
HitActor HitTestWithinLayer(Actor&                                     actor,
                            const RenderTask&                          renderTask,
                            const RenderTaskList::ExclusivesContainer& exclusives,
                            const Vector4&                             rayOrigin,
                            const Vector4&                             rayDir,
                            const float&                               nearClippingPlane,
                            const float&                               farClippingPlane,
                            HitTestInterface&                          hitCheck,
                            const bool&                                overlayed,
                            bool&                                      overlayHit,
                            bool                                       layerIs3d,
                            RayTest&                                   rayTest,
                            const Integration::Point&                  point,
                            const uint32_t                             eventTime)
{
  HitActor hit;

  if(IsActorExclusiveToAnotherRenderTask(actor, renderTask, exclusives))
  {
    return hit;
  }

  // For clipping, regardless of whether we have hit this actor or not.
  // This is used later to ensure all nested clipped children have hit
  // all clipping actors also for them to be counted as hit.
  bool clippingActor  = actor.GetClippingMode() != ClippingMode::DISABLED;
  bool overlayedActor = overlayed || actor.IsOverlay();

  // If we are a clipping actor or hittable...
  HitTestActor(renderTask, rayOrigin, rayDir, nearClippingPlane, farClippingPlane, hitCheck, rayTest, point, eventTime, clippingActor, overlayedActor, actor, overlayHit, hit);

  // If current actor is clipping, and hit failed, We should not checkup child actors. Fast return
  if(clippingActor && !(hit.actor))
  {
    return hit;
  }

  // Find a child hit, until we run out of actors in the current layer.
  HitActor childHit;
  if(actor.GetChildCount() > 0)
  {
    childHit.distance        = std::numeric_limits<float>::max();
    childHit.depth           = std::numeric_limits<int32_t>::min();
    ActorContainer& children = actor.GetChildrenInternal();

    // Hit test ALL children and calculate their distance.
    bool parentIsRenderable = actor.IsRenderable();

    for(ActorIter iter = children.begin(), endIter = children.end(); iter != endIter; ++iter)
    {
      // Descend tree only if...
      if(!(*iter)->IsLayer() &&                           // Child is NOT a layer, hit testing current layer only
         (hitCheck.DescendActorHierarchy((*iter).Get()))) // We can descend into child hierarchy
      {
        HitActor currentHit(HitTestWithinLayer((*iter->Get()),
                                               renderTask,
                                               exclusives,
                                               rayOrigin,
                                               rayDir,
                                               nearClippingPlane,
                                               farClippingPlane,
                                               hitCheck,
                                               overlayedActor,
                                               overlayHit,
                                               layerIs3d,
                                               rayTest,
                                               point,
                                               eventTime));

        // Make sure the set hit actor is actually hittable. This is usually required when we have some
        // clipping as we need to hit-test all actors as we descend the tree regardless of whether they
        // are hittable or not.
        if(currentHit.actor && (!hitCheck.IsActorHittable(currentHit.actor)))
        {
          continue;
        }

        UpdateChildHitData(hit, currentHit, layerIs3d, parentIsRenderable, childHit);
      }
    }
  }

  return (childHit.actor) ? childHit : hit;
}

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
 * Returns true if the actor and all of the actor's parents are visible and sensitive.
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
 * Returns true if the layer and all of the layer's parents are visible and sensitive.
 */
inline bool IsActuallyHittable(Layer& layer, const Vector2& screenCoordinates, const Vector2& stageSize, HitTestInterface& hitCheck)
{
  bool hittable(true);

  if(layer.IsClipping())
  {
    ClippingBox box = layer.GetClippingBox();

    if(screenCoordinates.x < static_cast<float>(box.x) ||
       screenCoordinates.x > static_cast<float>(box.x + box.width) ||
       screenCoordinates.y < stageSize.y - static_cast<float>(box.y + box.height) ||
       screenCoordinates.y > stageSize.y - static_cast<float>(box.y))
    {
      // Not touchable if clipping is enabled in the layer and the screen coordinate is outside the clip region.
      hittable = false;
    }
  }

  if(hittable)
  {
    Actor* actor(&layer);
    hittable = IsActorActuallyHittable(actor, hitCheck);
  }

  return hittable;
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

/**
 * Hit test a RenderTask
 */
bool HitTestRenderTask(const RenderTaskList::ExclusivesContainer& exclusives,
                       const Vector2&                             sceneSize,
                       LayerList&                                 layers,
                       RenderTask&                                renderTask,
                       Vector2                                    screenCoordinates,
                       Results&                                   results,
                       HitTestInterface&                          hitCheck,
                       RayTest&                                   rayTest)
{
  if(renderTask.IsHittable(screenCoordinates))
  {
    Viewport viewport;
    renderTask.GetHittableViewport(viewport);

    if(screenCoordinates.x < static_cast<float>(viewport.x) ||
       screenCoordinates.x > static_cast<float>(viewport.x + viewport.width) ||
       screenCoordinates.y < static_cast<float>(viewport.y) ||
       screenCoordinates.y > static_cast<float>(viewport.y + viewport.height))
    {
      // The screen coordinate is outside the viewport of render task. The viewport clips all layers.
      return false;
    }

    float nearClippingPlane, farClippingPlane;
    GetCameraClippingPlane(renderTask, nearClippingPlane, farClippingPlane);

    // Determine the layer depth of the source actor
    Actor* sourceActor(renderTask.GetSourceActor());

    // Check the source actor is actually hittable or not.
    if(sourceActor && IsActorActuallyHittable(sourceActor, hitCheck))
    {
      Dali::Layer sourceLayer(sourceActor->GetLayer());
      if(sourceLayer)
      {
        const uint32_t sourceActorDepth(sourceLayer.GetProperty<bool>(Dali::Layer::Property::DEPTH));

        CameraActor* cameraActor     = renderTask.GetCameraActor();
        bool         pickingPossible = cameraActor->BuildPickingRay(
          screenCoordinates,
          viewport,
          results.rayOrigin,
          results.rayDirection);
        if(!pickingPossible)
        {
          return false;
        }

        // Hit test starting with the top layer, working towards the bottom layer.
        HitActor hit;
        bool     overlayHit       = false;
        bool     layerConsumesHit = false;

        // Be used when we decide to consume layer.
        // We should not consume hit if sourceLayer is above on consumable layer. Otherwise, we should consume. So just initialize it as 0.
        // sourceLayerIndex can be a relative value to calculate the relationship with the layer.
        // If the layer is consumed first, sourceLayerIndex is not the actual index, but it must be guaranteed to have an index smaller than the layer.
        // If there is a sourceLayer above the consumable layer, the sourceLayerIndex is determined and the index of the consumable layer is also determined.
        // Then we can calculate the relationship between the two layers.
        bool    IsHitTestWithinLayer = false;
        int32_t sourceLayerIndex     = 0;
        int32_t consumedLayerIndex   = -1;

        for(int32_t i = layers.GetLayerCount() - 1; i >= 0 && !(hit.actor); --i)
        {
          Layer* layer(layers.GetLayer(i));
          overlayHit           = false;
          IsHitTestWithinLayer = false;

          if(sourceLayer == layer)
          {
            sourceLayerIndex = i;
          }

          // Ensure layer is touchable (also checks whether ancestors are also touchable)
          if(IsActuallyHittable(*layer, screenCoordinates, sceneSize, hitCheck))
          {
            // Always hit-test the source actor; otherwise test whether the layer is below the source actor in the hierarchy
            if(sourceActorDepth == static_cast<uint32_t>(i))
            {
              IsHitTestWithinLayer = true;
              // Recursively hit test the source actor & children, without crossing into other layers.
              hit = HitTestWithinLayer(*sourceActor,
                                       renderTask,
                                       exclusives,
                                       results.rayOrigin,
                                       results.rayDirection,
                                       nearClippingPlane,
                                       farClippingPlane,
                                       hitCheck,
                                       overlayHit,
                                       overlayHit,
                                       layer->GetBehavior() == Dali::Layer::LAYER_3D,
                                       rayTest,
                                       results.point,
                                       results.eventTime);
            }
            else if(IsWithinSourceActors(*sourceActor, *layer))
            {
              IsHitTestWithinLayer = true;
              // Recursively hit test all the actors, without crossing into other layers.
              hit = HitTestWithinLayer(*layer,
                                       renderTask,
                                       exclusives,
                                       results.rayOrigin,
                                       results.rayDirection,
                                       nearClippingPlane,
                                       farClippingPlane,
                                       hitCheck,
                                       overlayHit,
                                       overlayHit,
                                       layer->GetBehavior() == Dali::Layer::LAYER_3D,
                                       rayTest,
                                       results.point,
                                       results.eventTime);
            }

            // If this layer is set to consume the hit, then do not check any layers behind it
            if(IsHitTestWithinLayer && hitCheck.DoesLayerConsumeHit(layer))
            {
              consumedLayerIndex = i;
              layerConsumesHit   = true;
              break;
            }
          }
        }

        if(hit.actor)
        {
          results.renderTask       = RenderTaskPtr(&renderTask);
          results.actor            = Dali::Actor(hit.actor);
          results.actorCoordinates = hit.hitPosition;

          return true; // Success
        }

        if(layerConsumesHit)
        {
          // Consumes if the hitted layer is above the SourceActor's layer.
          bool ret = sourceLayerIndex <= consumedLayerIndex;
          if(ret)
          {
            DALI_LOG_RELEASE_INFO("layer is set to consume the hit\n");
            results.renderTask = RenderTaskPtr(&renderTask);
            results.actor      = Dali::Layer(layers.GetLayer(consumedLayerIndex));
          }
          return ret; // Also success if layer is consuming the hit
        }
      }
    }
  }
  return false;
}

Dali::Actor FindPriorActorInLayer(Dali::Actor rootActor, Dali::Actor firstActor, Dali::Actor secondActor)
{
  Dali::Actor priorActor;
  uint32_t childCount = rootActor.GetChildCount();
  if(childCount > 0)
  {
    for(int32_t i = childCount - 1; i >= 0; --i)
    {
      Dali::Actor child = rootActor.GetChildAt(i);
      if(GetImplementation(child).IsLayer())
      {
        continue;
      }
      priorActor = FindPriorActorInLayer(child, firstActor, secondActor);
      if(priorActor)
      {
        break;
      }
    }
  }

  if(!priorActor)
  {
    if(rootActor == firstActor)
    {
      priorActor = firstActor;
    }

    if(rootActor == secondActor)
    {
      priorActor = secondActor;
    }
  }

  return priorActor;
}

Dali::Actor FindPriorActorInLayers(LayerList& layers, Dali::Actor rootActor, Dali::Actor firstActor, Dali::Actor secondActor)
{
  Dali::Layer sourceLayer = rootActor.GetLayer();
  const uint32_t sourceActorDepth(sourceLayer.GetProperty<int>(Dali::Layer::Property::DEPTH));

  Dali::Actor priorActor;
  uint32_t layerCount = layers.GetLayerCount();
  if(layerCount > 0)
  {
    for(int32_t i = layerCount - 1; i >= 0; --i)
    {
      Layer* layer(layers.GetLayer(i));
      if(sourceActorDepth == static_cast<uint32_t>(i))
      {
        priorActor = FindPriorActorInLayer(rootActor, firstActor, secondActor);
      }
      else if(IsWithinSourceActors(GetImplementation(rootActor), *layer))
      {
        Dali::Actor layerRoot = Dali::Actor(layer);
        priorActor            = FindPriorActorInLayer(layerRoot, firstActor, secondActor);
      }

      if(priorActor)
      {
        break;
      }
    }
  }
  return priorActor;
}

/**
 * Iterate through the RenderTaskList and perform hit testing.
 *
 * @param[in] sceneSize The scene size the tests will be performed in
 * @param[in] layers The list of layers to test
 * @param[in] taskList The list of render tasks
 * @param[out] results Ray information calculated by the camera
 * @param[in] hitCheck The hit testing interface object to use
 * @return True if we have a hit, false otherwise
 */
bool HitTestRenderTaskList(const Vector2&    sceneSize,
                           LayerList&        layers,
                           RenderTaskList&   taskList,
                           const Vector2&    screenCoordinates,
                           Results&          results,
                           HitTestInterface& hitCheck)
{
  RenderTaskList::RenderTaskContainer&                  tasks      = taskList.GetTasks();
  RenderTaskList::RenderTaskContainer::reverse_iterator endIter    = tasks.rend();
  const auto&                                           exclusives = taskList.GetExclusivesList();
  RayTest                                               rayTest;

  Results storedResults = results;
  std::vector<std::pair<Dali::Actor, Results>> hitResults;
  // Hit test order should be reverse of draw order
  for(RenderTaskList::RenderTaskContainer::reverse_iterator iter = tasks.rbegin(); endIter != iter; ++iter)
  {
    RenderTask& renderTask = *iter->Get();
    if(HitTestRenderTask(exclusives, sceneSize, layers, renderTask, screenCoordinates, results, hitCheck, rayTest))
    {
      if(renderTask.GetFrameBuffer())
      {
        Results result = results;
        hitResults.push_back(std::make_pair(renderTask.GetScreenToFrameBufferMappingActor(), result));
        continue;
      }
      else
      {
        Actor* sourceActor(renderTask.GetSourceActor());
        for(auto&& pair : hitResults)
        {
          Dali::Actor mappingActor = pair.first;
          if(!IsWithinSourceActors(*sourceActor, GetImplementation(mappingActor)))
          {
            continue;
          }

          bool mappingActorInsideHitConsumingLayer = false;
          if(GetImplementation(results.actor).IsLayer())
          {
            Dali::Layer resultLayer = Dali::Layer::DownCast(results.actor);
            // Check the resultLayer is consuming hit even though the layer is not hittable.
            // And check the resultLayer is the layer of mappingActor too.
            if(hitCheck.DoesLayerConsumeHit(&GetImplementation(resultLayer)) && !hitCheck.IsActorHittable(&GetImplementation(results.actor)) && results.actor == mappingActor.GetLayer())
            {
              mappingActorInsideHitConsumingLayer = true;
            }
          }
          if(mappingActorInsideHitConsumingLayer || mappingActor == FindPriorActorInLayers(layers, Dali::Actor(sourceActor), mappingActor, results.actor))
          {
            results = pair.second;
            break;
          }
        }
      }
      // Return true when an actor is hit (or layer in our render-task consumes the hit)
      return true;
    }
  }

  // When nothing is hitted in OnScreen RenderTask but there is hit results from OffScreen RenderTask,
  // then returns the result those mapping Actor is placed on top.
  if(!hitResults.empty())
  {
    Dali::Actor topMappingActor = hitResults.front().first;
    results                     = hitResults.front().second;
    Dali::Actor rootActor       = Dali::Actor(tasks.front().Get()->GetSourceActor());
    for(uint32_t i = 1; i < hitResults.size(); ++i)
    {
      Dali::Actor priorActor = FindPriorActorInLayers(layers, rootActor, topMappingActor, hitResults[i].first);
      results                = (priorActor == topMappingActor) ? results : hitResults[i].second;
      topMappingActor        = priorActor;
    }
    return true;
  }

  results = storedResults;
  return false;
}

/**
 * Iterate through the RenderTaskList and perform hit testing for both on-screen and off-screen.
 *
 * @param[in] sceneSize The scene size the tests will be performed in
 * @param[in] layers The list of layers to test
 * @param[in] taskList The list of render tasks
 * @param[out] results Ray information calculated by the camera
 * @param[in] hitCheck The hit testing interface object to use
 * @return True if we have a hit, false otherwise
 */
bool HitTestForEachRenderTask(const Vector2&    sceneSize,
                              LayerList&        layers,
                              RenderTaskList&   taskList,
                              const Vector2&    screenCoordinates,
                              Results&          results,
                              HitTestInterface& hitCheck)
{
  bool result = false;

  if(HitTestRenderTaskList(sceneSize, layers, taskList, screenCoordinates, results, hitCheck))
  {
    // Found hit.
    result = true;
  }

  return result;
}

} // unnamed namespace

HitTestInterface::~HitTestInterface() = default;

bool HitTest(const Vector2& sceneSize, RenderTaskList& taskList, LayerList& layerList, const Vector2& screenCoordinates, Dali::HitTestAlgorithm::Results& results, Dali::HitTestAlgorithm::HitTestFunction func)
{
  bool wasHit(false);
  // Hit-test the regular on-scene actors
  Results                hitTestResults;
  HitTestFunctionWrapper hitTestFunctionWrapper(func);
  if(HitTestForEachRenderTask(sceneSize, layerList, taskList, screenCoordinates, hitTestResults, hitTestFunctionWrapper))
  {
    results.actor            = hitTestResults.actor;
    results.actorCoordinates = hitTestResults.actorCoordinates;
    wasHit                   = true;
  }
  return wasHit;
}

bool HitTest(const Vector2& sceneSize, RenderTaskList& renderTaskList, LayerList& layerList, const Vector2& screenCoordinates, Results& results, HitTestInterface& hitTestInterface)
{
  bool wasHit(false);

  // Hit-test the regular on-scene actors
  if(!wasHit)
  {
    wasHit = HitTestForEachRenderTask(sceneSize, layerList, renderTaskList, screenCoordinates, results, hitTestInterface);
  }
  return wasHit;
}

bool HitTest(const Vector2& sceneSize, RenderTaskList& renderTaskList, LayerList& layerList, const Vector2& screenCoordinates, Results& results, const Actor* ownActor)
{
  ActorTouchableCheck actorTouchableCheck;
  actorTouchableCheck.SetOwnActor(ownActor);
  return HitTest(sceneSize, renderTaskList, layerList, screenCoordinates, results, actorTouchableCheck);
}

} // namespace HitTestAlgorithm

} // namespace Internal

} // namespace Dali
