#ifndef __DALI_INTERNAL_HIT_TEST_ALGORITHM_H__
#define __DALI_INTERNAL_HIT_TEST_ALGORITHM_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/events/hit-test-algorithm.h>
#include <dali/public-api/render-tasks/render-task.h>

namespace Dali
{

namespace Internal
{

/**
 * This namespace is provided for application developers to do hit test for the actors.
 */
namespace HitTestAlgorithm
{

struct Results
{
  Dali::RenderTask renderTask;       ///< The render-task displaying the actor.
  Dali::Actor      actor;            ///< The hit actor.
  Vector2          actorCoordinates; ///< The actor coordinates.
  Vector4          rayOrigin;        ///< The point of origin of the ray.
  Vector4          rayDirection;     ///< The direction vector of the ray.
};

/**
 * @copydoc Dali::HitTestAlgorithm::HitTest(Stage stage, const Vector2& screenCoordinates, Results& results, HitTestFunction func )
 */
void HitTest( Stage& stage, const Vector2& screenCoordinates, Dali::HitTestAlgorithm::Results& results, Dali::HitTestAlgorithm::HitTestFunction func );

/**
 * Given screen coordinates, this method returns the hit actor & the local coordinates relative to the actor etc.
 * @param[in] stage The stage.
 * @param[in] screenCoordinates The screen coordinates.
 * @param[out] results The results of the hit-test.
 *
 * <h3>Hit Test Algorithm:</h3>
 *
 * - The system overlay RenderTaskList is hit-tested first.
 * - If no hit then the regular RenderTaskList is used to hit test the on stage actors.
 * - The bulk of the hit test algorithm is described in Dali::Actor.
 * - In each RenderTask's its viewing parameters (the view and projection matrices, and the viewport)
 *   are used to build a picking ray into the scene which is used for our ray tests when hit testing
 *   an actor within each layer.
 * - If an actor is deemed to be hittable, then a quicker ray sphere test on the actor is performed
 *   first to determine if the ray is in the actor's proximity.
 * - If this is also successful, then a more accurate ray test is performed to see if we have a hit.
 *
 * - NOTE: Currently, we prefer a child hit over a parent (regardless of the distance from the
 *   camera) unless the parent is a RenderableActor but this is subject to change.
 */
void HitTest( Stage& stage, const Vector2& screenCoordinates, Results& results );

/**
 * Hit test specific to a given RenderTask
 * @param[in] stage The stage.
 * @param[in] renderTask The render task for hit test
 * @param[in] screenCoordinates The screen coordinates.
 * @param[out] results The results of the hit-test.
 * @param[in] func The function to use in the hit-test algorithm.
 */
void HitTest( Stage& stage, RenderTask& renderTask, const Vector2& screenCoordinates,
              Dali::HitTestAlgorithm::Results& results, Dali::HitTestAlgorithm::HitTestFunction func );

} // namespace HitTestAlgorithm

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_HIT_TEST_ALGORITHM_H__
