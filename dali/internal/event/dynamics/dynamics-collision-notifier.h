#ifndef __DALI_INTERNAL_EVENTS_DYNAMICS_COLLISION_NOTIFIER_H__
#define __DALI_INTERNAL_EVENTS_DYNAMICS_COLLISION_NOTIFIER_H__

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
#include <dali/internal/common/message.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
struct DynamicsCollisionData;
} // namespace SceneGraph

/**
 * Interface used by the update-thread to trigger collision event signals.
 */
class DynamicsCollisionNotifier
{
public:

  /**
   * Virtual destructor.
   */
  virtual ~DynamicsCollisionNotifier()
  {
  }

  /**
   * Invoked when the simulation detects a new collision between two bodies.
   * @param[in] collisionData Contains information about the colliding bodies, their
   *                          points of contact, and impact magnitude
   */
  virtual void CollisionImpact( SceneGraph::DynamicsCollisionData* collisionData );

  /**
   * Invoked when the simulation detects two bodies already in collision scraping against each other.
   * @param[in] collisionData Contains information about the colliding bodies, their
   *                          points of contact, and impact magnitude
   */
  virtual void CollisionScrape( SceneGraph::DynamicsCollisionData* collisionData );

  /**
   * Invoked when the simulation detects two previously colliding bodies moving apart.
   * @param[in] collisionData Contains information about the colliding bodies.
   */
  virtual void CollisionDisperse( SceneGraph::DynamicsCollisionData* collisionData );
};

inline MessageBase* CollisionImpactMessage( DynamicsCollisionNotifier& dynamicsNotifier, SceneGraph::DynamicsCollisionData* collisionData )
{
  return new MessageValue1< DynamicsWorld, SceneGraph::DynamicsCollisionData* >( &dynamicsNotifier, &DynamicsCollisionNotifier::CollisionImpact, collisionData );
}

inline MessageBase* CollisionScrapeMessage( DynamicsCollisionNotifier& dynamicsNotifier, SceneGraph::DynamicsCollisionData* collisionData )
{
  return new MessageValue1< DynamicsWorld, SceneGraph::DynamicsCollisionData* >( &dynamicsNotifier, &DynamicsCollisionNotifier::CollisionScrape, collisionData );
}

inline MessageBase* CollisionDisperseMessage( DynamicsCollisionNotifier& dynamicsNotifier, SceneGraph::DynamicsCollisionData* collisionData )
{
  return new MessageValue1< DynamicsWorld, SceneGraph::DynamicsCollisionData* >( &dynamicsNotifier, &DynamicsCollisionNotifier::CollisionDisperse, collisionData );
}

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_EVENTS_DYNAMICS_COLLISION_NOTIFIER_H__

