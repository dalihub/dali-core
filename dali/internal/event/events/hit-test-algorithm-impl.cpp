/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/system-overlay.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/common/system-overlay-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/images/frame-buffer-image-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>

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
  : actor( NULL ),
    distance( std::numeric_limits<float>::max() ),
    depth( std::numeric_limits<int>::min() )
  {
  }

  Actor *actor;                         ///< The actor hit (if actor is hit, then this is initialised).
  Vector2 hitPosition;                  ///< Position of hit (only valid if actor valid).
  float distance;                       ///< Distance from ray origin to hit actor.
  int depth;                            ///< Depth index of this actor.
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
  HitTestFunctionWrapper( Dali::HitTestAlgorithm::HitTestFunction func )
  : mFunc( func )
  {
  }

  virtual bool IsActorHittable( Actor* actor )
  {
    return mFunc( Dali::Actor( actor ), Dali::HitTestAlgorithm::CHECK_ACTOR );
  }

  virtual bool DescendActorHierarchy( Actor* actor )
  {
    return mFunc( Dali::Actor( actor ), Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE );
  }

  virtual bool DoesLayerConsumeHit( Layer* layer )
  {
    // Layer::IsTouchConsumed() focuses on touch only. Here we are a wrapper for the public-api
    // where the caller may want to check for something completely different.
    // TODO: Should provide a means to let caller decide. For now do not allow layers to consume
    return false;
  }

  Dali::HitTestAlgorithm::HitTestFunction mFunc;
};

/**
 * Used in the hit-test algorithm to check whether the actor is touchable.
 * It is used by the touch event processor.
 */
struct ActorTouchableCheck : public HitTestInterface
{
  virtual bool IsActorHittable( Actor* actor )
  {
    return actor->GetTouchRequired() && // Does the Application or derived actor type require a touch event?
           actor->IsHittable();         // Is actor sensitive, visible and on the scene?
  }

  virtual bool DescendActorHierarchy( Actor* actor )
  {
    return actor->IsVisible() && // Actor is visible, if not visible then none of its children are visible.
           actor->IsSensitive(); // Actor is sensitive, if insensitive none of its children should be hittable either.
  }

  virtual bool DoesLayerConsumeHit( Layer* layer )
  {
    return layer->IsTouchConsumed();
  }
};

/**
 * Check to see if the actor we're about to hit test is exclusively owned by another rendertask?
 */
bool IsActorExclusiveToAnotherRenderTask( const Actor& actor,
                                          const RenderTask& renderTask,
                                          const Vector< RenderTaskList::Exclusive >& exclusives )

{
  if( exclusives.Size() )
  {
    for( Vector< RenderTaskList::Exclusive >::Iterator exclusiveIt = exclusives.Begin(); exclusives.End() != exclusiveIt; ++exclusiveIt )
    {
      if( ( exclusiveIt->renderTaskPtr != &renderTask ) && ( exclusiveIt->actorPtr == &actor ) )
      {
        return true;
      }
    }
  }
  return false;
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
HitActor HitTestWithinLayer( Actor& actor,
                             const RenderTask& renderTask,
                             const Vector< RenderTaskList::Exclusive >& exclusives,
                             const Vector4& rayOrigin,
                             const Vector4& rayDir,
                             float& nearClippingPlane,
                             float& farClippingPlane,
                             HitTestInterface& hitCheck,
                             bool& overlayHit,
                             bool layerIs3d,
                             unsigned int clippingDepth,
                             unsigned int clippingBitPlaneMask )
{
  HitActor hit;

  if( IsActorExclusiveToAnotherRenderTask( actor, renderTask, exclusives ) )
  {
    return hit;
  }

  // For clipping, regardless of whether we have hit this actor or not,
  // we increase the clipping depth if we have hit a clipping actor.
  // This is used later to ensure all nested clipped children have hit
  // all clipping actors also for them to be counted as hit.
  unsigned int newClippingDepth = clippingDepth;
  bool clippingActor = actor.GetClippingMode() != ClippingMode::DISABLED;
  if( clippingActor )
  {
    ++newClippingDepth;
  }

  // If we are a clipping actor or hittable...
  if( clippingActor || hitCheck.IsActorHittable( &actor ) )
  {
    Vector3 size( actor.GetCurrentSize() );

    // Ensure the actor has a valid size.
    // If so, perform a quick ray sphere test to see if our ray is close to the actor.
    if( size.x > 0.0f && size.y > 0.0f && actor.RaySphereTest( rayOrigin, rayDir ) )
    {
      Vector2 hitPointLocal;
      float distance;

      // Finally, perform a more accurate ray test to see if our ray actually hits the actor.
      if( actor.RayActorTest( rayOrigin, rayDir, hitPointLocal, distance ) )
      {
        if( distance >= nearClippingPlane && distance <= farClippingPlane )
        {
          // If the hit has happened on a clipping actor, then add this clipping depth to the mask of hit clipping depths.
          // This mask shows all the actors that have been hit at different clipping depths.
          if( clippingActor )
          {
            clippingBitPlaneMask |= 1u << clippingDepth;
          }

          if( overlayHit && !actor.IsOverlay() )
          {
            // If we have already hit an overlay and current actor is not an overlay ignore current actor.
          }
          else
          {
            if( actor.IsOverlay() )
            {
              overlayHit = true;
            }

            // At this point we have hit an actor.
            // Now perform checks for clipping.
            // Assume we have hit the actor first as if it is not clipped this would be the case.
            bool haveHitActor = true;

            // Check if we are performing clipping. IE. if any actors so far have clipping enabled - not necessarily this one.
            // We can do this by checking the clipping depth has a value 1 or above.
            if( newClippingDepth >= 1u )
            {
              // Now for us to count this actor as hit, we must have also hit
              // all CLIPPING actors up to this point in the hierarchy as well.
              // This information is stored in the clippingBitPlaneMask we updated above.
              // Here we calculate a comparison mask by setting all the bits up to the current depth value.
              // EG. a depth of 4 (10000 binary) = a mask of 1111 binary.
              // This allows us a fast way of comparing all bits are set up to this depth.
              // Note: If the current Actor has clipping, that is included in the depth mask too.
              unsigned int clippingDepthMask = ( 1u << newClippingDepth ) - 1u;

              // The two masks must be equal to be a hit, as we are already assuming a hit
              // (for non-clipping mode) then they must be not-equal to disqualify the hit.
              if( clippingBitPlaneMask != clippingDepthMask )
              {
                haveHitActor = false;
              }
            }

            if( haveHitActor )
            {
              hit.actor = &actor;
              hit.hitPosition = hitPointLocal;
              hit.distance = distance;
              hit.depth = actor.GetSortingDepth() ;

              if( actor.GetRendererCount() > 0 )
              {
                //Get renderer with maximum depth
                int rendererMaxDepth(actor.GetRendererAt( 0 ).Get()->GetDepthIndex());
                for( unsigned int i(1); i < actor.GetRendererCount(); ++i )
                {
                  int depth = actor.GetRendererAt( i ).Get()->GetDepthIndex();
                  if( depth > rendererMaxDepth )
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

  // Find a child hit, until we run out of actors in the current layer.
  HitActor childHit;
  if( actor.GetChildCount() > 0 )
  {
    childHit.distance = std::numeric_limits<float>::max();
    childHit.depth = std::numeric_limits<int>::min();
    ActorContainer& children = actor.GetChildrenInternal();

    // Hit test ALL children and calculate their distance.
    bool parentIsRenderable = actor.IsRenderable();

    for( ActorIter iter = children.begin(), endIter = children.end(); iter != endIter; ++iter )
    {
      // Descend tree only if...
      if ( !( *iter )->IsLayer() &&                                 // Child is NOT a layer, hit testing current layer only
            ( hitCheck.DescendActorHierarchy( ( *iter ).Get() ) ) ) // We can descend into child hierarchy
      {
        HitActor currentHit( HitTestWithinLayer( ( *iter->Get() ),
                                                 renderTask,
                                                 exclusives,
                                                 rayOrigin,
                                                 rayDir,
                                                 nearClippingPlane,
                                                 farClippingPlane,
                                                 hitCheck,
                                                 overlayHit,
                                                 layerIs3d,
                                                 newClippingDepth,
                                                 clippingBitPlaneMask ) );

        bool updateChildHit = false;
        if( currentHit.distance >= 0.0f )
        {
          if( layerIs3d )
          {
            updateChildHit = ( ( currentHit.depth > childHit.depth ) ||
                               ( ( currentHit.depth == childHit.depth ) && ( currentHit.distance < childHit.distance ) ) );
          }
          else
          {
            updateChildHit = currentHit.depth >= childHit.depth;
          }
        }

        if( updateChildHit )
        {
          if( !parentIsRenderable || currentHit.depth > hit.depth ||
            ( layerIs3d && ( currentHit.depth == hit.depth && currentHit.distance < hit.distance )) )
          {
            childHit = currentHit;
          }
        }
      }
    }
  }

  return ( childHit.actor ) ? childHit : hit;
}

/**
 * Return true if actor is sourceActor or a descendent of sourceActor
 */
bool IsWithinSourceActors( const Actor& sourceActor, const Actor& actor )
{
  if ( &sourceActor == &actor )
  {
    return true;
  }

  Actor* parent = actor.GetParent();
  if ( parent )
  {
    return IsWithinSourceActors( sourceActor, *parent );
  }

  // Not within source actors
  return false;
}

/**
 * Returns true if the layer and all of the layer's parents are visible and sensitive.
 */
inline bool IsActuallyHittable( Layer& layer, const Vector2& screenCoordinates, const Vector2& stageSize, HitTestInterface& hitCheck )
{
  bool hittable( true );

  if( layer.IsClipping() )
  {
    ClippingBox box = layer.GetClippingBox();

    if( screenCoordinates.x < box.x ||
        screenCoordinates.x > box.x + box.width ||
        screenCoordinates.y < stageSize.y - (box.y + box.height) ||
        screenCoordinates.y > stageSize.y - box.y)
    {
      // Not touchable if clipping is enabled in the layer and the screen coordinate is outside the clip region.
      hittable = false;
    }
  }

  if( hittable )
  {
    Actor* actor( &layer );

    // Ensure that we can descend into the layer's (or any of its parent's) hierarchy.
    while( actor && hittable )
    {
      if( ! hitCheck.DescendActorHierarchy( actor ) )
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
void GetCameraClippingPlane( RenderTask& renderTask, float& nearClippingPlane, float& farClippingPlane )
{
  CameraActor* cameraActor = renderTask.GetCameraActor();
  nearClippingPlane = cameraActor->GetNearClippingPlane();
  farClippingPlane = cameraActor->GetFarClippingPlane();
}

/**
 * Hit test a RenderTask
 */
bool HitTestRenderTask( const Vector< RenderTaskList::Exclusive >& exclusives,
                        Stage& stage,
                        LayerList& layers,
                        RenderTask& renderTask,
                        Vector2 screenCoordinates,
                        Results& results,
                        HitTestInterface& hitCheck )
{
  if ( renderTask.IsHittable( screenCoordinates ) )
  {
    Viewport viewport;
    renderTask.GetViewport( viewport );
    if( screenCoordinates.x < viewport.x ||
        screenCoordinates.x > viewport.x + viewport.width ||
        screenCoordinates.y < viewport.y ||
        screenCoordinates.y > viewport.y + viewport.height )
    {
      // The screen coordinate is outside the viewport of render task. The viewport clips all layers.
      return false;
    }

    float nearClippingPlane, farClippingPlane;
    GetCameraClippingPlane( renderTask, nearClippingPlane, farClippingPlane );

    // Determine the layer depth of the source actor
    Actor* sourceActor( renderTask.GetSourceActor() );
    if( sourceActor )
    {
      Dali::Layer layer( sourceActor->GetLayer() );
      if( layer )
      {
        const unsigned int sourceActorDepth( layer.GetDepth() );

        CameraActor* cameraActor = renderTask.GetCameraActor();
        bool pickingPossible = cameraActor->BuildPickingRay(
            screenCoordinates,
            viewport,
            results.rayOrigin,
            results.rayDirection );
        if( !pickingPossible )
        {
          return false;
        }

        // Hit test starting with the top layer, working towards the bottom layer.
        HitActor hit;
        bool overlayHit = false;
        bool layerConsumesHit = false;
        const Vector2& stageSize = stage.GetSize();

        for( int i = layers.GetLayerCount() - 1; i >= 0 && !( hit.actor ); --i )
        {
          Layer* layer( layers.GetLayer( i ) );
          overlayHit = false;

          // Ensure layer is touchable (also checks whether ancestors are also touchable)
          if( IsActuallyHittable( *layer, screenCoordinates, stageSize, hitCheck ) )
          {
            // Always hit-test the source actor; otherwise test whether the layer is below the source actor in the hierarchy
            if( sourceActorDepth == static_cast<unsigned int>( i ) )
            {
              // Recursively hit test the source actor & children, without crossing into other layers.
              hit = HitTestWithinLayer( *sourceActor,
                                        renderTask,
                                        exclusives,
                                        results.rayOrigin,
                                        results.rayDirection,
                                        nearClippingPlane,
                                        farClippingPlane,
                                        hitCheck,
                                        overlayHit,
                                        layer->GetBehavior() == Dali::Layer::LAYER_3D,
                                        0u,
                                        0u );
            }
            else if( IsWithinSourceActors( *sourceActor, *layer ) )
            {
              // Recursively hit test all the actors, without crossing into other layers.
              hit = HitTestWithinLayer( *layer,
                                        renderTask,
                                        exclusives,
                                        results.rayOrigin,
                                        results.rayDirection,
                                        nearClippingPlane,
                                        farClippingPlane,
                                        hitCheck,
                                        overlayHit,
                                        layer->GetBehavior() == Dali::Layer::LAYER_3D,
                                        0u,
                                        0u );
            }

            // If this layer is set to consume the hit, then do not check any layers behind it
            if( hitCheck.DoesLayerConsumeHit( layer ) )
            {
              layerConsumesHit = true;
              break;
            }
          }
        }

        if( hit.actor )
        {
          results.renderTask = Dali::RenderTask( &renderTask );
          results.actor = Dali::Actor( hit.actor );
          results.actorCoordinates = hit.hitPosition;

          return true; // Success
        }

        if( layerConsumesHit )
        {
          return true; // Also success if layer is consuming the hit
        }
      }
    }
  }
  return false;
}

/**
 * Iterate through the RenderTaskList and perform hit testing.
 *
 * @param[in] stage The stage the tests will be performed in
 * @param[in] layers The list of layers to test
 * @param[in] taskList The list of render tasks
 * @param[out] results Ray information calculated by the camera
 * @param[in] hitCheck The hit testing interface object to use
 * @param[in] onScreen True to test on-screen, false to test off-screen
 * @return True if we have a hit, false otherwise
 */
bool HitTestRenderTaskList( Stage& stage,
                            LayerList& layers,
                            RenderTaskList& taskList,
                            const Vector2& screenCoordinates,
                            Results& results,
                            HitTestInterface& hitCheck,
                            bool onScreen )
{
  RenderTaskList::RenderTaskContainer& tasks = taskList.GetTasks();
  RenderTaskList::RenderTaskContainer::reverse_iterator endIter = tasks.rend();
  const Vector< RenderTaskList::Exclusive >& exclusives = taskList.GetExclusivesList();

  for( RenderTaskList::RenderTaskContainer::reverse_iterator iter = tasks.rbegin(); endIter != iter; ++iter )
  {
    RenderTask& renderTask = GetImplementation( *iter );
    bool isOffscreenRenderTask = ( iter->GetTargetFrameBuffer() || iter->GetFrameBuffer() );
    if( (onScreen && isOffscreenRenderTask) || (!onScreen && !isOffscreenRenderTask) )
    {
      // Skip to next task
      continue;
    }

    if( HitTestRenderTask( exclusives, stage, layers, renderTask, screenCoordinates, results, hitCheck ) )
    {
      // Return true when an actor is hit (or layer in our render-task consumes the hit)
      return true; // don't bother checking off screen tasks
    }
  }

  return false;
}

/**
 * Iterate through the RenderTaskList and perform hit testing for both on-screen and off-screen.
 *
 * @param[in] stage The stage the tests will be performed in
 * @param[in] layers The list of layers to test
 * @param[in] taskList The list of render tasks
 * @param[out] results Ray information calculated by the camera
 * @param[in] hitCheck The hit testing interface object to use
 * @param[in] onScreen True to test on-screen, false to test off-screen
 * @return True if we have a hit, false otherwise
 */
bool HitTestForEachRenderTask( Stage& stage,
                               LayerList& layers,
                               RenderTaskList& taskList,
                               const Vector2& screenCoordinates,
                               Results& results,
                               HitTestInterface& hitCheck )
{
  bool result = false;

  // Check on-screen tasks before off-screen ones.
  // Hit test order should be reverse of draw order (see ProcessRenderTasks() where off-screen tasks are drawn first).
  if( HitTestRenderTaskList( stage, layers, taskList, screenCoordinates, results, hitCheck, true  ) ||
      HitTestRenderTaskList( stage, layers, taskList, screenCoordinates, results, hitCheck, false ) )
  {
    // Found hit.
    result = true;
  }

  return result;
}

} // unnamed namespace

bool HitTest( Stage& stage, const Vector2& screenCoordinates, Dali::HitTestAlgorithm::Results& results, Dali::HitTestAlgorithm::HitTestFunction func )
{
  bool wasHit( false );
  // Hit-test the regular on-stage actors
  RenderTaskList& taskList = stage.GetRenderTaskList();
  LayerList& layerList = stage.GetLayerList();

  Results hitTestResults;
  HitTestFunctionWrapper hitTestFunctionWrapper( func );
  if( HitTestForEachRenderTask( stage, layerList, taskList, screenCoordinates, hitTestResults, hitTestFunctionWrapper ) )
  {
    results.actor = hitTestResults.actor;
    results.actorCoordinates = hitTestResults.actorCoordinates;
    wasHit = true;
  }
  return wasHit;
}

bool HitTest( Stage& stage, const Vector2& screenCoordinates, Results& results, HitTestInterface& hitTestInterface )
{
  bool wasHit( false );

  // Hit-test the system-overlay actors first
  SystemOverlay* systemOverlay = stage.GetSystemOverlayInternal();

  if( systemOverlay )
  {
    RenderTaskList& overlayTaskList = systemOverlay->GetOverlayRenderTasks();
    LayerList& overlayLayerList = systemOverlay->GetLayerList();

    wasHit = HitTestForEachRenderTask( stage, overlayLayerList, overlayTaskList, screenCoordinates, results, hitTestInterface );
  }

  // Hit-test the regular on-stage actors
  if( !wasHit )
  {
    RenderTaskList& taskList = stage.GetRenderTaskList();
    LayerList& layerList = stage.GetLayerList();

    wasHit = HitTestForEachRenderTask( stage, layerList, taskList, screenCoordinates, results, hitTestInterface );
  }
  return wasHit;
}

bool HitTest( Stage& stage, const Vector2& screenCoordinates, Results& results )
{
  ActorTouchableCheck actorTouchableCheck;
  return HitTest( stage, screenCoordinates, results, actorTouchableCheck );
}

bool HitTest( Stage& stage, RenderTask& renderTask, const Vector2& screenCoordinates,
              Dali::HitTestAlgorithm::Results& results, Dali::HitTestAlgorithm::HitTestFunction func )
{
  bool wasHit( false );
  Results hitTestResults;

  const Vector< RenderTaskList::Exclusive >& exclusives = stage.GetRenderTaskList().GetExclusivesList();
  HitTestFunctionWrapper hitTestFunctionWrapper( func );
  if( HitTestRenderTask( exclusives, stage, stage.GetLayerList(), renderTask, screenCoordinates, hitTestResults, hitTestFunctionWrapper ) )
  {
    results.actor = hitTestResults.actor;
    results.actorCoordinates = hitTestResults.actorCoordinates;
    wasHit = true;
  }
  return wasHit;
}


} // namespace HitTestAlgorithm

} // namespace Internal

} // namespace Dali
