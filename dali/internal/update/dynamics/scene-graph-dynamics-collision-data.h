#ifndef __SCENE_GRAPH_DYNAMICS_COLLISION_DATA_H__
#define __SCENE_GRAPH_DYNAMICS_COLLISION_DATA_H__

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

// BASE CLASS HEADER

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/devel-api/dynamics/dynamics-world.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

struct DynamicsCollisionData
{
public:
  DynamicsCollisionData(DynamicsBody* bodyA, DynamicsBody* bodyB, const Vector3& pointOnA, const Vector3& pointOnB, const Vector3& normal, const float impact)
  : mBodyA(bodyA),
    mBodyB(bodyB),
    mPointOnA(pointOnA),
    mPointOnB(pointOnB),
    mNormal(normal),
    mImpact(impact)
  {
  }

  DynamicsCollisionData(const DynamicsCollisionData& rhs)
  : mBodyA(rhs.mBodyA),
    mBodyB(rhs.mBodyB),
    mPointOnA(rhs.mPointOnA),
    mPointOnB(rhs.mPointOnB),
    mNormal(rhs.mNormal),
    mImpact(rhs.mImpact)
  {
  }

  DynamicsCollisionData()
  : mBodyA(NULL),
    mBodyB(NULL),
    mImpact(0.0f)
  {
  }

  ~DynamicsCollisionData()
  {
  }

  DynamicsCollisionData& operator=(const DynamicsCollisionData& rhs)
  {
    if( this != &rhs )
    {
      mBodyA = rhs.mBodyA;
      mBodyB = rhs.mBodyB;
      mPointOnA = rhs.mPointOnA;
      mPointOnB = rhs.mPointOnB;
      mNormal = rhs.mNormal;
      mImpact = rhs.mImpact;
    }
    return *this;
  }

  DynamicsBody* mBodyA;
  DynamicsBody* mBodyB;
  Vector3       mPointOnA;
  Vector3       mPointOnB;
  Vector3       mNormal;
  float         mImpact;
}; // DynamicsCollisionData

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __SCENE_GRAPH_DYNAMICS_COLLISION_DATA_H__
