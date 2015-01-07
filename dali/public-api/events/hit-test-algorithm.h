#ifndef  __DALI_HIT_TEST_ALGORITHM_H__
#define  __DALI_HIT_TEST_ALGORITHM_H__

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

#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/stage.h>


namespace Dali
{

class RenderTask;

/**
 * @brief This namespace is provided for application developers to do hit-test for the actors.
 *
 * <h3>Hit Test Algorithm:</h3>
 *
 * Hit testing is dependent on the camera used, which is specific to each RenderTask. For each RenderTask,
 * hit testing starts from the top-most layer and we go through all the layers until we have a hit or there
 * are none left. Before we perform a hit test within a layer, we check if all the layer's parents meet the
 * conditions defined by the function ((e.g. whether it is visible)). If they are not, we skip hit testing
 * the actors in that layer altogether. Otherwise, we walk through the actor tree within a layer to check
 * whether the actors within the actor tree should be hit-tested.
 *
 * The following pseudocode gives an example of what the function can typically check, which should normally
 * be provided by the application code:
 *
 *   @code
 *   HIT-TEST-FUNCTION( ACTOR, TRAVERSE-TYPE )
 *   {
 *     if( TRAVERSE-TYPE == CHECK_ACTOR ) // Check whether current actor should be hit-tested
 *     {
 *       if( ACTOR-IS-VISIBLE &&
 *           ACTOR-WORLD-COLOR-IS-NOT-TRANSPARENT )
 *       {
 *         HITTABLE = TRUE
 *       }
 *     }
 *     else if( TRAVERSE-TYPE == DESCEND_ACTOR_TREE ) ///< Check whether the actor tree should be descended to hit-test its children.
 *     {
 *       if( ACTOR-IS-VISIBLE )
 *       {
 *         HITTABLE = TRUE
 *       }
 *     }
 *   }
 *   @endcode
 *
 * The following pseudocode explains how the algorithm performs the hit-test with the above functor:
 *
 *   @code
 *   HIT-TEST-WITHIN-LAYER( ACTOR )
 *   {
 *     // Depth-first traversal within current layer, visiting parent first
 *
 *     // Check whether current actor should be hit-tested
 *     IF ( HIT-TEST-FUNCTION( ACTOR, CHECK_ACTOR ) &&
 *          ACTOR-HAS-NON-ZERO-SIZE )
 *     {
 *       // Hit-test current actor
 *       IF ( ACTOR-HIT )
 *       {
 *         IF ( DISTANCE-TO-ACTOR < DISTANCE-TO-LAST-HIT-ACTOR )
 *         {
 *           // The current actor is the closest actor that was underneath the touch
 *           LAST-HIT-ACTOR = CURRENT-ACTOR
 *         }
 *       }
 *     }
 *
 *     // Keep checking children, in case we hit something closer
 *     FOR-EACH CHILD (in order)
 *     {
 *       IF ( HIT-TEST-FUNCTION( ACTOR, DESCEND_ACTOR_TREE ) &&
 *            ACTOR-IS-NOT-A-LAYER )
 *       {
 *         // Continue traversal for this child's sub-tree
 *         HIT-TEST-WITHIN-LAYER ( CHILD )
 *       }
 *       // else we skip the sub-tree with from this child
 *     }
 *   }
 *   @endcode
 */
namespace HitTestAlgorithm
{

/**
 * @brief How the actor tree should be traversed.
 */
enum TraverseType
{
  CHECK_ACTOR,            ///< Hit test the given actor.
  DESCEND_ACTOR_TREE      ///< Check whether the actor tree should be descended to hit-test its children.
};

/**
 * @brief Results structure containing the hit actor and where it was hit.
 */
struct Results
{
  Actor      actor;            ///< The hit actor.
  Vector2    actorCoordinates; ///< The actor coordinates.
};

/**
 * @brief Definition of a hit-test function to use in HitTest() method to check if the actor is hittable (e.g. touchable or focusable).
 *
 * @return true, if the actor is hittable, false otherwise.
 */
typedef bool (*HitTestFunction)(Actor actor, TraverseType type);

/**
 * @brief Given screen coordinates, this method returns the hit actor & the local coordinates relative to
 * the top-left (0.0f, 0.0f, 0.5f) of the actor.
 *
 * An actor is only hittable if the actor meets all the conditions
 * defined by the given function (see HitTestAlgorithm).
 *
 * Typically, if an actor has a zero size or its world color is fully transparent, it should not be
 * hittable; and if an actor's visibility flag is unset, its children should not be hittable either.
 *
 * @param[in] stage The stage.
 * @param[in] screenCoordinates The screen coordinates.
 * @param[out] results The results of the hit-test, only modified if something is hit
 * @param[in] func The function to use in the hit-test algorithm.
 * @return true if something was hit
 */
DALI_IMPORT_API bool HitTest( Stage stage, const Vector2& screenCoordinates, Results& results, HitTestFunction func );

/**
 * @brief Hit test specific to a given RenderTask.
 *
 * @param[in] renderTask The render task for hit test
 * @param[in] screenCoordinates The screen coordinates.
 * @param[out] results The results of the hit-test, only modified if something is hit
 * @param[in] func The function to use in the hit-test algorithm.
 * @return true if something was hit
 */
DALI_IMPORT_API bool HitTest( RenderTask& renderTask, const Vector2& screenCoordinates, Results& results, HitTestFunction func );

} // namespace HitTestAlgorithm

} // namespace Dali

#endif // __DALI_HIT_TEST_ALGORITHM_H__
