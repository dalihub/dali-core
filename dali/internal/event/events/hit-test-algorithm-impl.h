#ifndef DALI_INTERNAL_HIT_TEST_ALGORITHM_H
#define DALI_INTERNAL_HIT_TEST_ALGORITHM_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/actors/actor.h>

namespace Dali
{
namespace Internal
{
class Layer;
class LayerList;

/**
 * This namespace is provided for application developers to do hit test for the actors.
 */
namespace HitTestAlgorithm
{
struct Results
{
  RenderTaskPtr      renderTask;                ///< The render-task displaying the actor.
  Dali::Actor        actor;                     ///< The hit actor.
  Vector2            actorCoordinates;          ///< The actor coordinates.
  Vector4            rayOrigin;                 ///< The point of origin of the ray.
  Vector4            rayDirection;              ///< The direction vector of the ray.
  Integration::Point point;                     ///< The point of event touched.
  uint32_t           eventTime;                 ///< The time the event occurred.
  std::list<Dali::Internal::Actor*> actorLists; ///< If the geometry hittest way is used, a list of actors that can be hit is stored.
};

/**
 * Interface used by the hit-test-algorithm to determine whether the actor is hittable or whether
 * we walk down its hierarchy.
 */
struct HitTestInterface
{
  /**
   * Called by the hit-test algorithm to determine whether the actor is hittable or not.
   *
   * @param[in] actor Raw pointer to an Actor object.
   *
   * @return true if actor is hittable, false otherwise.
   */
  virtual bool IsActorHittable(Actor* actor) = 0;

  /**
   * Called by the hit-test algorithm to determine whether the algorithm should descend the actor's
   * hierarchy (and hit-test its children as well).
   *
   * @param[in] actor Raw pointer to an Actor object.
   *
   * @return true if we should descend the actor's hierarchy, false otherwise.
   */
  virtual bool DescendActorHierarchy(Actor* actor) = 0;

  /**
   * Called by the hit-test algorithm to determine whether the layer specified consumes the hit
   * regardless of whether an actor in the layer requires it or not.
   *
   * @note If true is returned, then no layers behind this layer will be hit-test.
   *
   * @param[in] layer Raw pointer to a Layer object.
   *
   * @return true if the layer should consume the hit, false otherwise.
   */
  virtual bool DoesLayerConsumeHit(Layer* layer) = 0;

  /**
   * Called by the hit-test algorithm to determine whether the actor will be hit or not.
   *
   * @note If true is returned, then this actor will be hit.
   *       If false is returend, then this actor passes the hit-test and the next actor performs the hit-test.
   *
   * @param[in] actor The hit actor.
   * @param[in] hitPointLocal The hit point in the Actor's local reference system.
   * @param[in] propagationType If Integration::Scene::TouchPropagationType::GEOMETRY, hittest works in a geometry way.
   *
   * @return true if the actor should be the hit, false otherwise.
   */
  virtual bool ActorRequiresHitResultCheck(Actor* actor, Vector2 hitPointLocal) = 0;

  void SetPoint(const Integration::Point& point)
  {
    mPoint = point;
  }

  const Integration::Point& GetPoint() const
  {
    return mPoint;
  }

  void SetTimeStamp(uint32_t timeStamp)
  {
    mTimeStamp = timeStamp;
  }

  uint32_t GetTimeStamp() const
  {
    return mTimeStamp;
  }

  void SetPropagationType(const Integration::Scene::TouchPropagationType propagationtType)
  {
    mPropagationType = propagationtType;
  }

  Integration::Scene::TouchPropagationType GetPropagationType() const
  {
    return mPropagationType;
  }

protected:
  /**
   * Virtual destructor, no deletion through this interface
   */
  virtual ~HitTestInterface();

private:
  Integration::Point                       mPoint;
  uint32_t                                 mTimeStamp{0u};
  Integration::Scene::TouchPropagationType mPropagationType{Integration::Scene::TouchPropagationType::PARENT};
};

/**
 * Hit test specific to a given scene.
 *
 * @param[in] sceneSize The size of the scene.
 * @param[in] renderTaskList The render task list of the scene.
 * @param[in] layerList The layer list of the scene.
 * @param[in] screenCoordinates The screen coordinates.
 * @param[out] results The results of the hit-test.
 * @param[in] func The function to use in the hit-test algorithm.
 * @param[in] propagationType If Integration::Scene::TouchPropagationType::GEOMETRY, hittest works in a geometry way.
 * @return true if something was hit
 *
 * @see HitTest(Stage&, const Vector2&, Results&, HitTestInterface&)
 */
bool HitTest(const Vector2& sceneSize, RenderTaskList& renderTaskList, LayerList& layerList, const Vector2& screenCoordinates, Dali::HitTestAlgorithm::Results& results, Dali::HitTestAlgorithm::HitTestFunction func, const Integration::Scene::TouchPropagationType propagationType = Integration::Scene::TouchPropagationType::PARENT);

/**
 * Given screen coordinates, this method returns the hit actor & the local coordinates relative to the actor etc.
 * @param[in] sceneSize The size of the scene.
 * @param[in] renderTaskList The render task list of the scene.
 * @param[in] layerList The layer list of the scene.
 * @param[in] screenCoordinates The screen coordinates.
 * @param[out] results The results of the hit-test.
 * @param[in] hitTestInterface Used to determine whether the actor is hit or whether we walk down its hierarchy
 * @param[in] propagationType If Integration::Scene::TouchPropagationType::GEOMETRY, hittest works in a geometry way.
 * @return true if something was hit
 *
 * <h3>Hit Test Algorithm:</h3>
 *
 * - The regular RenderTaskList is used to hit test the on scene actors.
 * - The bulk of the hit test algorithm is described in Dali::Actor.
 * - In each RenderTask's its viewing parameters (the view and projection matrices, and the viewport)
 *   are used to build a picking ray into the scene which is used for our ray tests when hit testing
 *   an actor within each layer.
 * - If an actor is deemed to be hittable, then a quicker ray sphere test on the actor is performed
 *   first to determine if the ray is in the actor's proximity.
 * - If this is also successful, then a more accurate ray test is performed to see if we have a hit.
 *
 * @note Currently, we prefer a child hit over a parent (regardless of the distance from the
 *       camera) unless the parent is a RenderableActor but this is subject to change.
 */
bool HitTest(const Vector2& sceneSize, RenderTaskList& renderTaskList, LayerList& layerList, const Vector2& screenCoordinates, Results& results, HitTestInterface& hitTestInterface, const Integration::Scene::TouchPropagationType propagationType = Integration::Scene::TouchPropagationType::PARENT);

/**
 * Default HitTest where we check if a touch is required.
 *
 * @param[in] sceneSize The size of the scene.
 * @param[in] renderTaskList The render task list of the scene.
 * @param[in] layerList The layer list of the scene.
 * @param[in] screenCoordinates The screen coordinates.
 * @param[out] results The results of the hit-test.
 * @param[in] ownActor The actor from which the touch down was started.
 * @param[in] propagationType If Integration::Scene::TouchPropagationType::GEOMETRY, hittest works in a geometry way.
 * @return true if something was hit
 *
 * @see HitTest(Stage&, const Vector2&, Results&, HitTestInterface&)
 */
bool HitTest(const Vector2& sceneSize, RenderTaskList& renderTaskList, LayerList& layerList, const Vector2& screenCoordinates, Results& results, const Actor* ownActor = nullptr, const Integration::Scene::TouchPropagationType propagationType = Integration::Scene::TouchPropagationType::PARENT);

} // namespace HitTestAlgorithm

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_HIT_TEST_ALGORITHM_H
