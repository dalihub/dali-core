#ifndef __SCENE_GRAPH_DYNAMICS_MOTION_STATE_H__
#define __SCENE_GRAPH_DYNAMICS_MOTION_STATE_H__

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

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class DynamicsBody;

/**
 * The DynamicsMotionState class allows the dynamics engine to synchronize and interpolate
 * the updated world transforms with Dali.
 * Only really useful for kinematic objects.
 */
class DynamicsMotionState
{
public:
  /**
   * Constructor.
   * @param[in] dynamicsBody  The DynamicsBody synchronized with this MotionState
   */
  DynamicsMotionState(DynamicsBody& dynamicsBody);

  /**
   * Destructor
   */
  virtual ~DynamicsMotionState();

  /**
   * Called by the Dynamics Engine to update it's copy of the body's position/orientation
   * @param[out] transform  The matrix the body's position/orientation will be copied to.
   */
  virtual void getWorldTransform(btTransform& transform) const;

  /**
   * Called by the Dynamics Engine to update our copy the body's position/orientation
   * @param[in] transform  The body's position/orientation as calculated by the Dynamics engine.
   */
  virtual void setWorldTransform(const btTransform& transform);

protected:
  DynamicsBody& mDynamicsBody;  ///< The DynamicsBody
}; // class DynamicsMotionState

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __SCENE_GRAPH_DYNAMICS_MOTION_STATE_H__
