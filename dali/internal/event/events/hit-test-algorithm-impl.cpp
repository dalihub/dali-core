/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

namespace Dali::Internal::HitTestAlgorithm
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

  bool ActorRequiresHitResultCheck(Actor* actor, Integration::Point point, Vector2 hitPointLocal, uint32_t timeStamp, bool isGeometry) override
  {
    // Geometry way does not require Hittest from the client.
    if(!isGeometry)
    {
      return actor->EmitHitTestResultSignal(point, hitPointLocal, timeStamp);
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

  bool ActorRequiresHitResultCheck(Actor* actor, Integration::Point point, Vector2 hitPointLocal, uint32_t timeStamp, bool isGeometry) override
  {
    // The Geometry way behaves like AllowedOnlyOwnTouch is enabled.
    if(point.GetState() != PointState::STARTED && (isGeometry || actor->IsAllowedOnlyOwnTouch()) && ownActor != actor)
    {
      return false;
    }
    // Geometry way does not require Hittest from the client.
    if(!isGeometry)
    {
      return actor->EmitHitTestResultSignal(point, hitPointLocal, timeStamp);
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
 * Checks if actor or anyone of it's parents are an overlay, until either the currentActor is reached or the root actor
 * @param actor The child-actor and it's parents to check
 * @param currentActor The top actor of this current branch which we should not go above
 * @return True if the actor or a parent is an overlay, false otherwise
 */
inline bool IsOnOverlay(Actor* actor, Actor* currentActor)
{
  while(actor && actor != currentActor)
  {
    if(actor->IsOverlay())
    {
      return true;
    }
    actor = actor->GetParent();
  }
  return false;
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
                  const RayTest&            rayTest,
                  const Integration::Point& point,
                  const uint32_t            eventTime,
                  bool                      clippingActor,
                  bool                      overlayedActor,
                  Actor&                    actor,
                  bool&                     overlayHit,
                  HitActor&                 hit,
                  bool                      isGeometry)
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

      // Finally, perform a more accurate ray test to see if our ray actually hits the actor.
      if(rayTest.ActorTest(actor, rayOrigin, rayDir, hitPointLocal, distance))
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
            if(hitCheck.ActorRequiresHitResultCheck(&actor, point, hitPointLocal, eventTime, isGeometry))
            {
              hit.actor       = &actor;
              hit.hitPosition = hitPointLocal;
              hit.distance    = distance;
              hit.depth       = actor.GetSortingDepth();

              if(actor.GetRendererCount() > 0)
              {
                // Get renderer with maximum depth
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
                            const RayTest&                             rayTest,
                            const Integration::Point&                  point,
                            const uint32_t                             eventTime,
                            std::list<Dali::Internal::Actor*>&         actorLists,
                            bool                                       isGeometry)
{
  HitActor hit;

  if(IsActorExclusiveToAnotherRenderTask(actor, renderTask, exclusives))
  {
    return hit;
  }

  // For clipping, regardless of whether we have hit this actor or not.
  // This is used later to ensure all nested clipped children have hit
  // all clipping actors also for them to be counted as hit.
  const ClippingMode::Type clippingMode   = actor.GetClippingMode();
  bool                     clippingActor  = clippingMode != ClippingMode::DISABLED;
  bool                     overlayedActor = overlayed || actor.IsOverlay();

  // If we are a clipping actor or hittable...
  HitTestActor(renderTask, rayOrigin, rayDir, nearClippingPlane, farClippingPlane, hitCheck, rayTest, point, eventTime, clippingActor, overlayedActor, actor, overlayHit, hit, isGeometry);

  // If current actor is clipping, and hit failed, We should not checkup child actors. Fast return
  // Only do this if we're using CLIP_CHILDREN though, as children whose drawing mode is OVERLAY_2D are not clipped when CLIP_TO_BOUNDING_BOX is selected.
  if(clippingActor && !(hit.actor) && (clippingMode == ClippingMode::CLIP_CHILDREN))
  {
    return hit;
  }
  else if(isGeometry && hit.actor)
  {
    // Saves the actors that can be hit as a list
    actorLists.push_back(hit.actor);
  }

  // Find a child hit, until we run out of actors in the current layer.
  HitActor childHit;
  if(actor.GetChildCount() > 0)
  {
    // If the child touches outside the parent's size boundary, it should not be hit.
    if(isGeometry && !actor.IsLayer())
    {
      Vector2 hitPointLocal;
      float   distance;
      if(!(rayTest.SphereTest(actor, rayOrigin, rayDir) &&
           rayTest.ActorTest(actor, rayOrigin, rayDir, hitPointLocal, distance)))
      {
        return hit;
      }
    }

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
                                               eventTime,
                                               actorLists,
                                               isGeometry));
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

  if(childHit.actor)
  {
    // If child has been hit & current actor is clipping to bounding box...
    if(clippingMode == ClippingMode::CLIP_TO_BOUNDING_BOX)
    {
      // ...then make sure the clipping actor has actually been hit unless the child hit actor is on a child overlay.
      if(hit.actor || IsOnOverlay(childHit.actor, &actor))
      {
        // Only then should we return the child hit in this scenario.
        return childHit;
      }
    }
    else
    {
      // no clipping concerns, return child hit.
      return childHit;
    }
  }

  return hit;
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

    // Ensure that we can descend into the layer's (or any of its parent's) hierarchy.
    while(actor && hittable)
    {
      if(!hitCheck.DescendActorHierarchy(actor))
      {
        hittable = false;
        break;
      }
      actor = actor->GetParent();
    }
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

void GeoHitTestRenderTask(const RenderTaskList::ExclusivesContainer& exclusives,
                          const Vector2&                             sceneSize,
                          LayerList&                                 layers,
                          RenderTask&                                renderTask,
                          Vector2                                    screenCoordinates,
                          Results&                                   results,
                          HitTestInterface&                          hitCheck,
                          const RayTest&                             rayTest)
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
      return;
    }

    float nearClippingPlane, farClippingPlane;
    GetCameraClippingPlane(renderTask, nearClippingPlane, farClippingPlane);

    // Determine the layer depth of the source actor
    Actor* sourceActor(renderTask.GetSourceActor());
    if(sourceActor)
    {
      Dali::Layer sourceLayer(sourceActor->GetLayer());
      if(sourceLayer)
      {
        const uint32_t sourceActorDepth(sourceLayer.GetProperty<bool>(Dali::Layer::Property::DEPTH));
        CameraActor*   cameraActor     = renderTask.GetCameraActor();
        bool           pickingPossible = cameraActor->BuildPickingRay(screenCoordinates,
                                                            viewport,
                                                            results.rayOrigin,
                                                            results.rayDirection);
        if(!pickingPossible)
        {
          return;
        }

        // Hit test starting with the top layer, working towards the bottom layer.
        bool overlayHit = false;

        for(uint32_t i = 0; i < layers.GetLayerCount(); ++i)
        {
          Layer* layer(layers.GetLayer(i));
          overlayHit = false;
          HitActor hit;

          // Ensure layer is touchable (also checks whether ancestors are also touchable)
          if(IsActuallyHittable(*layer, screenCoordinates, sceneSize, hitCheck))
          {
            // Always hit-test the source actor; otherwise test whether the layer is below the source actor in the hierarchy
            if(sourceActorDepth == i)
            {
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
                                       results.eventTime,
                                       results.actorLists,
                                       true);
            }
            else if(IsWithinSourceActors(*sourceActor, *layer))
            {
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
                                       results.eventTime,
                                       results.actorLists,
                                       true);
            }
          }

          if(hit.actor)
          {
            results.renderTask       = RenderTaskPtr(&renderTask);
            results.actor            = Dali::Actor(hit.actor);
            results.actorCoordinates = hit.hitPosition;
          }
        }
      }
    }
  }
  return;
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
                       const RayTest&                             rayTest)
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
    if(sourceActor)
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
                                       results.eventTime,
                                       results.actorLists,
                                       false);
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
                                       results.eventTime,
                                       results.actorLists,
                                       false);
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
          return ret;
        }
      }
    }
  }
  return false;
}

/**
 * Iterate through the RenderTaskList and perform hit testing.
 *
 * @param[in] sceneSize The scene size the tests will be performed in
 * @param[in] layers The list of layers to test
 * @param[in] taskList The list of render tasks
 * @param[out] results Ray information calculated by the camera
 * @param[in] hitCheck The hit testing interface object to use
 * @param[in] isGeometry Whether the scene using geometry event propagation touch and hover events.
 * @return True if we have a hit, false otherwise
 */
bool HitTestRenderTaskList(const Vector2&    sceneSize,
                           LayerList&        layers,
                           RenderTaskList&   taskList,
                           const Vector2&    screenCoordinates,
                           Results&          results,
                           HitTestInterface& hitCheck,
                           bool              isGeometry)
{
  if(isGeometry)
  {
    RenderTaskList::RenderTaskContainer&          tasks      = taskList.GetTasks();
    RenderTaskList::RenderTaskContainer::iterator endIter    = tasks.end();
    const auto&                                   exclusives = taskList.GetExclusivesList();
    RayTest                                       rayTest;

    // Hit test order should be of draw order
    for(RenderTaskList::RenderTaskContainer::iterator iter = tasks.begin(); endIter != iter; ++iter)
    {
      RenderTask& renderTask = *iter->Get();
      GeoHitTestRenderTask(exclusives, sceneSize, layers, renderTask, screenCoordinates, results, hitCheck, rayTest);
    }

    return !results.actorLists.empty();
  }
  else
  {
    RenderTaskList::RenderTaskContainer&                  tasks      = taskList.GetTasks();
    RenderTaskList::RenderTaskContainer::reverse_iterator endIter    = tasks.rend();
    const auto&                                           exclusives = taskList.GetExclusivesList();
    RayTest                                               rayTest;

    // Hit test order should be reverse of draw order
    for(RenderTaskList::RenderTaskContainer::reverse_iterator iter = tasks.rbegin(); endIter != iter; ++iter)
    {
      RenderTask& renderTask = *iter->Get();
      if(HitTestRenderTask(exclusives, sceneSize, layers, renderTask, screenCoordinates, results, hitCheck, rayTest))
      {
        // Return true when an actor is hit (or layer in our render-task consumes the hit)
        return true;
      }
    }
    return false;
  }
}

/**
 * Iterate through the RenderTaskList and perform hit testing for both on-screen and off-screen.
 *
 * @param[in] sceneSize The scene size the tests will be performed in
 * @param[in] layers The list of layers to test
 * @param[in] taskList The list of render tasks
 * @param[out] results Ray information calculated by the camera
 * @param[in] hitCheck The hit testing interface object to use
 * @param[in] isGeometry Whether the scene using geometry event propagation touch and hover events.
 * @return True if we have a hit, false otherwise
 */
bool HitTestForEachRenderTask(const Vector2&    sceneSize,
                              LayerList&        layers,
                              RenderTaskList&   taskList,
                              const Vector2&    screenCoordinates,
                              Results&          results,
                              HitTestInterface& hitCheck,
                              bool              isGeometry)
{
  bool result = false;

  if(HitTestRenderTaskList(sceneSize, layers, taskList, screenCoordinates, results, hitCheck, isGeometry))
  {
    // Found hit.
    result = true;
  }

  return result;
}

} // unnamed namespace

HitTestInterface::~HitTestInterface() = default;

bool HitTest(const Vector2& sceneSize, RenderTaskList& taskList, LayerList& layerList, const Vector2& screenCoordinates, Dali::HitTestAlgorithm::Results& results, Dali::HitTestAlgorithm::HitTestFunction func, bool isGeometry)
{
  bool wasHit(false);
  // Hit-test the regular on-scene actors
  Results                hitTestResults;
  HitTestFunctionWrapper hitTestFunctionWrapper(func);
  if(HitTestForEachRenderTask(sceneSize, layerList, taskList, screenCoordinates, hitTestResults, hitTestFunctionWrapper, isGeometry))
  {
    results.actor            = hitTestResults.actor;
    results.actorCoordinates = hitTestResults.actorCoordinates;
    wasHit                   = true;
  }
  return wasHit;
}

bool HitTest(const Vector2& sceneSize, RenderTaskList& renderTaskList, LayerList& layerList, const Vector2& screenCoordinates, Results& results, HitTestInterface& hitTestInterface, bool isGeometry)
{
  bool wasHit(false);

  // Hit-test the regular on-scene actors
  if(!wasHit)
  {
    wasHit = HitTestForEachRenderTask(sceneSize, layerList, renderTaskList, screenCoordinates, results, hitTestInterface, isGeometry);
  }
  return wasHit;
}

bool HitTest(const Vector2& sceneSize, RenderTaskList& renderTaskList, LayerList& layerList, const Vector2& screenCoordinates, Results& results, const Actor* ownActor, bool isGeometry)
{
  ActorTouchableCheck actorTouchableCheck;
  actorTouchableCheck.SetOwnActor(ownActor);
  return HitTest(sceneSize, renderTaskList, layerList, screenCoordinates, results, actorTouchableCheck, isGeometry);
}

} // namespace Dali::Internal::HitTestAlgorithm
