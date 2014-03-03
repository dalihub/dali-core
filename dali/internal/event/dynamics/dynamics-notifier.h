#ifndef __DALI_INTERNAL_EVENT_DYNAMICS_NOTIFIER_H__
#define __DALI_INTERNAL_EVENT_DYNAMICS_NOTIFIER_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>

namespace Dali
{

namespace Integration
{

struct DynamicsCollisionData;

} // namespace Integration

namespace Internal
{

namespace SceneGraph
{

class DynamicsBody;

} // namespace SceneGraph

/**
 * Interface used by the update-thread to trigger collision event signals.
 */
class DynamicsNotifier
{
public:
  static DynamicsNotifier* New();

  DynamicsNotifier();

  /**
   * Virtual destructor.
   */
  virtual ~DynamicsNotifier();

  /**
   * Invoked when the simulation detects a new collision between two bodies.
   * @param[in] collisionData Contains information about the colliding bodies, their
   *                          points of contact, and impact magnitude
   */
  virtual void CollisionImpact( Integration::DynamicsCollisionData* collisionData );

  /**
   * Invoked when the simulation detects two bodies already in collision scraping against each other.
   * @param[in] collisionData Contains information about the colliding bodies, their
   *                          points of contact, and impact magnitude
   */
  virtual void CollisionScrape( Integration::DynamicsCollisionData* collisionData );

  /**
   * Invoked when the simulation detects two previously colliding bodies moving apart.
   * @param[in] collisionData Contains information about the colliding bodies.
   */
  virtual void CollisionDisperse( Integration::DynamicsCollisionData* collisionData );
}; // class DynamicsNotifier

inline MessageBase* CollisionImpactMessage( DynamicsNotifier& dynamicsNotifier, Integration::DynamicsCollisionData* collisionData )
{
  return new MessageValue1< DynamicsNotifier, Integration::DynamicsCollisionData* >( &dynamicsNotifier, &DynamicsNotifier::CollisionImpact, collisionData );
}

inline MessageBase* CollisionScrapeMessage( DynamicsNotifier& dynamicsNotifier, Integration::DynamicsCollisionData* collisionData )
{
  return new MessageValue1< DynamicsNotifier, Integration::DynamicsCollisionData* >( &dynamicsNotifier, &DynamicsNotifier::CollisionScrape, collisionData );
}

inline MessageBase* CollisionDisperseMessage( DynamicsNotifier& dynamicsNotifier, Integration::DynamicsCollisionData* collisionData )
{
  return new MessageValue1< DynamicsNotifier, Integration::DynamicsCollisionData* >( &dynamicsNotifier, &DynamicsNotifier::CollisionDisperse, collisionData );
}

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_EVENT_DYNAMICS_NOTIFIER_H__
