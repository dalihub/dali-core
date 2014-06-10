#ifndef __DALI_DYNAMICS_COLLISION_IMPL_H__
#define __DALI_DYNAMICS_COLLISION_IMPL_H__

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

// BASE CLASS HEADER
#include <dali/public-api/object/base-object.h>

// INTERNAL HEADERS
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/public-api/dynamics/dynamics-collision.h>

namespace Dali
{

struct Vector3;

namespace Internal
{

/**
 * Contains information about a collision between two actors
 */
class DynamicsCollision : public BaseObject
{
public:
  /**
   * Constructor.
   */
  DynamicsCollision();

protected:
  /**
   * Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsCollision();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsCollision(const DynamicsCollision&);
  DynamicsCollision& operator=(const DynamicsCollision&);

  // Methods
public:
  /// @copydoc Dali::DynamicsCollision::GetActorA
  ActorPtr GetActorA();

  /**
   * Set first actor in collision
   * @param[in] actor First Actor in collision
   */
  void SetActorA(ActorPtr actor);

  /// @copydoc Dali::DynamicsCollision::GetActorB
  ActorPtr GetActorB();

  /**
   * Set second actor in collision
   * @param[in] actor Second Actor in collision
   */
  void SetActorB(ActorPtr actor);

  /// @copydoc Dali::DynamicsCollision::GetImpactForce
  float GetImpactForce() const;

  /**
   * Set impact force
   * @param[in] impact The impact force (0 on dispersal)
   */
  void SetImpactForce(const float impact);

  /// @copydoc Dali::DynamicsCollision::GetPointOnA
  Vector3 GetPointOnA() const;

  /**
   * Set the contact point on first Actor
   * @param[in] point The contact point on the first Actor
   */
  void SetPointOnA(const Vector3 point);

  /// @copydoc Dali::DynamicsCollision::GetPointOnB
  Vector3 GetPointOnB() const;

  /**
   * Set the contact point on second Actor
   * @param[in] point The contact point on the second Actor
   */
  void SetPointOnB(const Vector3 point);

  /// @copydoc Dali::DynamicsCollision::GetNormal
  Vector3 GetNormal() const;

  /**
   * Set the contact normal
   * @param[in] normal The contact normal
   */
  void SetNormal(const Vector3 normal);

private:
  ActorPtr  mActorA;
  ActorPtr  mActorB;
  Vector3   mPointA;
  Vector3   mPointB;
  Vector3   mNormal;
  float     mImpact;
}; // class DynamicsCollision

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::DynamicsCollision& GetImplementation(DynamicsCollision& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsCollision object is uninitialized!");

  BaseObject& handle = object.GetBaseObject();

  return static_cast<Internal::DynamicsCollision&>(handle);
}

inline const Internal::DynamicsCollision& GetImplementation(const DynamicsCollision& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsCollision object is uninitialized!");

  const BaseObject& handle = object.GetBaseObject();

  return static_cast<const Internal::DynamicsCollision&>(handle);
}

} // namespace Dali

#endif /* __DALI_DYNAMICS_COLLISION_IMPL_H__ */
