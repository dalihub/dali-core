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
#include <dali/internal/event/events/hit-test-algorithm-impl.h>

// INTERNAL INCLUDES
#include <dali/integration-api/system-overlay.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/actors/image-actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/actors/renderable-actor-impl.h>
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
    x( 0 ),
    y( 0 ),
    distance( std::numeric_limits<float>::max() ),
    depth( std::numeric_limits<int>::min() )
  {
  }

  Actor *actor;                         ///< the actor hit. (if actor hit, then initialised)
  float x;                              ///< x position of hit (only valid if actor valid)
  float y;                              ///< y position of hit (only valid if actor valid)
  float distance;                       ///< distance from ray origin to hit actor
  int depth;                            ///< depth index of this actor

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
  if ( exclusives.Size() )
  {
    for ( Vector< RenderTaskList::Exclusive >::Iterator exclusiveIt = exclusives.Begin(); exclusives.End() != exclusiveIt; ++exclusiveIt )
    {
      if ( exclusiveIt->renderTaskPtr != &renderTask )
      {
        if ( exclusiveIt->actorPtr == &actor )
        {
          return true;
        }
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
                             bool& stencilOnLayer,
                             bool& stencilHit,
                             bool parentIsStencil )
{
  HitActor hit;

  if ( IsActorExclusiveToAnotherRenderTask( actor, renderTask, exclusives ) )
  {
    return hit;
  }

  // Children should inherit the stencil draw mode
  bool isStencil = parentIsStencil;

  if ( actor.GetDrawMode() == DrawMode::STENCIL && actor.IsVisible() )
  {
    isStencil = true;
    stencilOnLayer = true;
  }

  // If we are a stencil or hittable...
  if ( isStencil || hitCheck.IsActorHittable( &actor ) )
  {
    Vector3 size( actor.GetCurrentSize() );

    if ( size.x > 0.0f && size.y > 0.0f &&          // Ensure the actor has a valid size.
         actor.RaySphereTest( rayOrigin, rayDir ) ) // Perform quicker ray sphere test to see if our ray is close to the actor.
    {
      Vector4 hitPointLocal;
      float distance;

      // Finally, perform a more accurate ray test to see if our ray actually hits the actor.
      if( actor.RayActorTest( rayOrigin, rayDir, hitPointLocal, distance ) )
      {
        if( distance >= nearClippingPlane && distance <= farClippingPlane )
        {
          // If the hit has happened on a stencil then register, but don't record as hit result
          if ( isStencil )
          {
            stencilHit = true;
          }
          else
          {
            hit.actor = &actor;
            hit.x = hitPointLocal.x;
            hit.y = hitPointLocal.y;
            hit.distance = distance;

            // Is this actor an Image Actor or contains a renderer?
            if ( ImageActor* imageActor = dynamic_cast< ImageActor* >( &actor ) )
            {
              hit.depth = imageActor->GetDepthIndex();
            }
            else
            {
              if ( actor.GetRendererCount() )
              {
                hit.depth = actor.GetRendererAt( 0 ).GetDepthIndex();
              }
              else
              {
                hit.depth = 0;
              }
            }
          }
        }
      }
    }
  }

  // If we are a stencil (or a child of a stencil) and we have already ascertained that the stencil has been hit then there is no need to hit-test the children of this stencil-actor
  if ( isStencil && stencilHit  )
  {
    return hit;
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
      if ( !iter->IsLayer() &&    // Child is NOT a layer, hit testing current layer only or Child is not a layer and we've inherited the stencil draw mode
           ( isStencil || hitCheck.DescendActorHierarchy( &GetImplementation( *iter ) ) ) ) // We are a stencil OR we can descend into child hierarchy
      {
        HitActor currentHit( HitTestWithinLayer(  GetImplementation(*iter),
                                                  renderTask,
                                                  exclusives,
                                                  rayOrigin,
                                                  rayDir,
                                                  nearClippingPlane,
                                                  farClippingPlane,
                                                  hitCheck,
                                                  stencilOnLayer,
                                                  stencilHit,
                                                  isStencil ) );

        bool updateChildHit = false;
        // If our ray casting hit, then check then if the hit actor's depth is greater that the favorite, it will be preferred
        if ( currentHit.distance >= 0.0f )
        {
          if ( currentHit.depth > childHit.depth )
          {
            updateChildHit = true;
          }

          // If the hit actor's depth is equal to current favorite, then we check the distance and prefer the closer
          else if ( currentHit.depth == childHit.depth )
          {
            if ( currentHit.distance < childHit.distance )
            {
              updateChildHit = true;
            }
          }
        }

        if ( updateChildHit )
        {
          if( !parentIsRenderable || currentHit.depth > hit.depth ||
            ( currentHit.depth == hit.depth && currentHit.distance < hit.distance ) )
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
  else
  {
    Actor* parent = actor.GetParent();
    if ( parent )
    {
      return IsWithinSourceActors( sourceActor, *parent );
    }
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

  if(layer.IsClipping())
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

  if(hittable)
  {
    Actor* actor( &layer );

    // Ensure that we can descend into the layer's (or any of its parent's) hierarchy.
    while ( actor && hittable )
    {
      if ( ! hitCheck.DescendActorHierarchy( actor ) )
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
    GetCameraClippingPlane(renderTask, nearClippingPlane, farClippingPlane);

    // Determine the layer depth of the source actor
    Actor* sourceActor( renderTask.GetSourceActor() );
    if ( sourceActor )
    {
      Dali::Layer layer( sourceActor->GetLayer() );
      if ( layer )
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
        bool stencilOnLayer = false;
        bool stencilHit = false;
        bool layerConsumesHit = false;
        const Vector2& stageSize = Stage::GetCurrent()->GetSize();

        for (int i=layers.GetLayerCount()-1; i>=0 && !(hit.actor); --i)
        {
          Layer* layer( layers.GetLayer(i) );
          HitActor previousHit = hit;
          stencilOnLayer = false;
          stencilHit = false;

          // Ensure layer is touchable (also checks whether ancestors are also touchable)
          if ( IsActuallyHittable ( *layer, screenCoordinates, stageSize, hitCheck ) )
          {
            // Always hit-test the source actor; otherwise test whether the layer is below the source actor in the hierarchy
            if ( sourceActorDepth == static_cast<unsigned int>(i) )
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
                                        stencilOnLayer,
                                        stencilHit,
                                        false );
            }
            else if ( IsWithinSourceActors( *sourceActor, *layer ) )
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
                                        stencilOnLayer,
                                        stencilHit,
                                        false );
            }

            // If a stencil on this layer hasn't been hit, then discard hit results for this layer if our current hit actor is renderable
            if ( stencilOnLayer && !stencilHit &&
                 hit.actor && hit.actor->IsRenderable() )
            {
              hit = previousHit;
            }

            // If this layer is set to consume the hit, then do not check any layers behind it
            if ( hitCheck.DoesLayerConsumeHit( layer ) )
            {
              layerConsumesHit = true;
              break;
            }
          }
        }
        if ( hit.actor )
        {
          results.renderTask = Dali::RenderTask(&renderTask);
          results.actor = Dali::Actor(hit.actor);
          results.actorCoordinates.x = hit.x;
          results.actorCoordinates.y = hit.y;
          return true; // Success
        }
        else if ( layerConsumesHit )
        {
          return true; // Also success if layer is consuming the hit
        }
      }
    }
  }
  return false;
}

/**
 * Iterate through RenderTaskList and perform hit test.
 *
 * @return true if we have a hit, false otherwise
 */
bool HitTestForEachRenderTask( LayerList& layers,
                               RenderTaskList& taskList,
                               const Vector2& screenCoordinates,
                               Results& results,
                               HitTestInterface& hitCheck )
{
  RenderTaskList::RenderTaskContainer& tasks = taskList.GetTasks();
  RenderTaskList::RenderTaskContainer::reverse_iterator endIter = tasks.rend();

  const Vector< RenderTaskList::Exclusive >& exclusives = taskList.GetExclusivesList();

  // Check onscreen tasks before offscreen ones, hit test order should be reverse of draw order (see ProcessRenderTasks() where offscreen tasks are drawn first).

  // on screen
  for ( RenderTaskList::RenderTaskContainer::reverse_iterator iter = tasks.rbegin(); endIter != iter; ++iter )
  {
    RenderTask& renderTask = GetImplementation( *iter );
    Dali::FrameBufferImage frameBufferImage = renderTask.GetTargetFrameBuffer();

    // Note that if frameBufferImage is NULL we are using the default (on screen) render target
    if(frameBufferImage)
    {
      ResourceId id = GetImplementation(frameBufferImage).GetResourceId();

      // on screen only
      if(0 != id)
      {
        // Skip to next task
        continue;
      }
    }

    if ( HitTestRenderTask( exclusives, layers, renderTask, screenCoordinates, results, hitCheck ) )
    {
      // Return true when an actor is hit (or layer in our render-task consumes the hit)
      return true; // don't bother checking off screen tasks
    }
  }

  // off screen
  for ( RenderTaskList::RenderTaskContainer::reverse_iterator iter = tasks.rbegin(); endIter != iter; ++iter )
  {
    RenderTask& renderTask = GetImplementation( *iter );
    Dali::FrameBufferImage frameBufferImage = renderTask.GetTargetFrameBuffer();

    // Note that if frameBufferImage is NULL we are using the default (on screen) render target
    if(frameBufferImage)
    {
      ResourceId id = GetImplementation(frameBufferImage).GetResourceId();

      // off screen only
      if(0 == id)
      {
        // Skip to next task
        continue;
      }

      if ( HitTestRenderTask( exclusives, layers, renderTask, screenCoordinates, results, hitCheck ) )
      {
        // Return true when an actor is hit (or a layer in our render-task consumes the hit)
        return true;
      }
    }
  }
  return false;
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
  if (  HitTestForEachRenderTask( layerList, taskList, screenCoordinates, hitTestResults, hitTestFunctionWrapper ) )
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

  if ( systemOverlay )
  {
    RenderTaskList& overlayTaskList = systemOverlay->GetOverlayRenderTasks();
    LayerList& overlayLayerList = systemOverlay->GetLayerList();

    wasHit = HitTestForEachRenderTask( overlayLayerList, overlayTaskList, screenCoordinates, results, hitTestInterface );
  }

  // Hit-test the regular on-stage actors
  if ( !wasHit )
  {
    RenderTaskList& taskList = stage.GetRenderTaskList();
    LayerList& layerList = stage.GetLayerList();

    wasHit = HitTestForEachRenderTask( layerList, taskList, screenCoordinates, results, hitTestInterface );
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

  const Vector< RenderTaskList::Exclusive >& exclusives = Stage::GetCurrent()->GetRenderTaskList().GetExclusivesList();
  HitTestFunctionWrapper hitTestFunctionWrapper( func );
  if ( HitTestRenderTask( exclusives, stage.GetLayerList(), renderTask, screenCoordinates, hitTestResults, hitTestFunctionWrapper ) )
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
