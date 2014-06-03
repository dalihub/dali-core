#ifndef __DYNAMICS_BODY_IMPL_H__
#define __DYNAMICS_BODY_IMPL_H__

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
#include <dali/public-api/object/base-object.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/public-api/dynamics/dynamics-body.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class DynamicsBody;
class Node;

} // namespace SceneGraph

class Stage;
class Actor;

typedef IntrusivePtr<Actor> ActorPtr;

/// @copydoc Dali::DynamicsBody
class DynamicsBody : public BaseObject
{
public:
  /**
   * Constructor.
   * @param[in] name  The name for this body/
   * @param[in] config DynamicsBody configuration data
   * @param[in] actor The actor for this DynamicsBody
   */
  DynamicsBody(const std::string& name, DynamicsBodyConfigPtr config, Actor& actor, SceneGraph::Node& node );

protected:
  /**
   * Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsBody();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsBody(const DynamicsBody&);
  DynamicsBody& operator=(const DynamicsBody&);

  // Methods
public:
  /// @copydoc Dali::DynamicsBody::GetName
  const std::string& GetName() const;

  /// Get the actor for this DynamicsBody
  Actor& GetActor() const;

  /// @copydoc Dali::DynamicsBody::GetMass
  float GetMass() const;

  /**
   * Set the mass
   * @param[in] mass The mass
   */
  void SetMass(const float mass);

  /// @copydoc Dali::DynamicsBody::GetElasticity
  float GetElasticity() const;

  /**
   * Set the elasticity
   * @param[in] elasticity The elasticity
   */
  void SetElasticity(const float elasticity);

  /// @copydoc Dali::DynamicsBody::SetKinematic
  void SetKinematic( const bool flag );

  /// @copydoc Dali::DynamicsBody::IsKinematic
  bool IsKinematic() const;

  /// @copydoc Dali::DynamicsBody::SetSleepEnabled
  void SetSleepEnabled( const bool flag );

  /// @copydoc Dali::DynamicsBody::GetSleepEnabled
  bool GetSleepEnabled() const;

  /// @copydoc Dali::DynamicsBody::WakeUp
  void WakeUp();

  /// @copydoc Dali::DynamicsBody::SetLinearVelocity
  void SetLinearVelocity(const Vector3& velocity);

  /// @copydoc Dali::DynamicsBody::GetCurrentLinearVelocity
  Vector3 GetCurrentLinearVelocity() const;

  /// @copydoc Dali::DynamicsBody::SetAngularVelocity
  void SetAngularVelocity(const Vector3& velocity);

  /// @copydoc Dali::DynamicsBody::GetCurrentAngularVelocity
  Vector3 GetCurrentAngularVelocity() const;

  /// @copydoc Dali::DynamicsBody::AddAnchor
  void AddAnchor(const unsigned int index, DynamicsBodyPtr body, const bool collisions);

  /// @copydoc Dali::DynamicsBody::ConserveVolume
  void ConserveVolume(const bool flag);

  /// @copydoc Dali::DynamicsBody::ConserveShape
  void ConserveShape(const bool flag);

  /**
   * Called when the associated actor is added to the stage
   * @param[in] stage Reference to the stage
   */
  void Connect(Stage& stage);

  /**
   * Called when the associated actor is removed from the stage
   * @param[in] stage Reference to the stage
   */
  void Disconnect(Stage& stage);

  SceneGraph::DynamicsBody* GetSceneObject()
  {
    return mDynamicsBody;
  }

  const SceneGraph::DynamicsBody* GetSceneObject() const
  {
    return mDynamicsBody;
  }

private:
  SceneGraph::DynamicsBody* mDynamicsBody;

  std::string       mName;
  float             mMass;
  float             mElasticity;
  bool              mKinematic;
  bool              mSleepEnabled;
  DynamicsShapePtr  mShape;
  Actor&            mActor;
}; // class DynamicsBody

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::DynamicsBody& GetImplementation(DynamicsBody& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsBody object is uninitialized!");

  BaseObject& handle = object.GetBaseObject();

  return static_cast<Internal::DynamicsBody&>(handle);
}

inline const Internal::DynamicsBody& GetImplementation(const DynamicsBody& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsBody object is uninitialized!");

  const BaseObject& handle = object.GetBaseObject();

  return static_cast<const Internal::DynamicsBody&>(handle);
}

} // namespace Dali

#endif // __DYNAMICS_BODY_IMPL_H__
