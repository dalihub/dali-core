#ifndef __DYNAMICS_WORLD_IMPL_H__
#define __DYNAMICS_WORLD_IMPL_H__

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

// INTERNAL HEADERS
#include <dali/public-api/object/base-object.h>
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/dynamics/dynamics-notifier.h>
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/devel-api/dynamics/dynamics-world.h>
#include <dali/public-api/signals/slot-delegate.h>


namespace Dali
{

namespace Integration
{

struct DynamicsCollisionData;
class  DynamicsBody;
class  DynamicsFactory;

} // namespace Integration

namespace Internal
{

class Stage;

namespace SceneGraph
{
struct DynamicsCollisionData;
class DynamicsBody;
class DynamicsWorld;
} // namespace SceneGraph

/// @copydoc Dali::DynamicsWorld
class DynamicsWorld : public BaseObject, public Dali::Internal::DynamicsNotifier
{
public:
  static DynamicsWorldPtr New();

public:
  /**
   * Constructor.
   * @param[in] name    A name for the world
   */
  DynamicsWorld(const std::string& name);

protected:
  /**
   * Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsWorld();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsWorld(const DynamicsWorld&);
  DynamicsWorld& operator=(const DynamicsWorld&);

public:

  /**
   * @copydoc Dali::DynamicsWorld::GetInstance()
   */
  static DynamicsWorldPtr GetInstance( DynamicsWorldConfigPtr configuration );

  /**
   * @copydoc Dali::DynamicsWorld::Get()
   */
  static DynamicsWorldPtr Get();

  /**
   * @copydoc Dali::DynamicsWorld::DestroyInstance()
   */
  static void DestroyInstance();

  /**
   * Gets the instance of the Dynamics Notifier, if created with GetInstance().
   *
   * @return The DynamicsNotifier instance.
   */
  static DynamicsNotifier& GetNotifier();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  /**
   * Initializes the simulation
   * @param[in] stage           A reference to the stage.
   * @param[in] dynamicsFactory A reference to the factory object which creates instances of dynamics objects
   * @param[in] config          Configuration parameters for the world
   */
  void Initialize(Stage& stage, Integration::DynamicsFactory& dynamicsFactory, DynamicsWorldConfigPtr config);

  /**
   * Terminate the simulation, sends a message to UpdateManager
   * @param[in] stage   A reference to the stage.
   */
  void Terminate(Stage& stage);

  // Methods
public:
  /// @copydoc Dali::SetGravity
  void SetGravity(const Vector3& gravity);

  /// @copydoc Dali::GetGravity
  const Vector3& GetGravity() const;

  /// @copydoc Dali::DynamicsWorld::CollisionSignal
  Dali::DynamicsWorld::CollisionSignalType& CollisionSignal();

  /// @copydoc Dali::DynamicsWorld::GetDebugDrawMode
  int GetDebugDrawMode() const;

  /// @copydoc Dali::DynamicsWorld::SetDebugDrawMode
  void SetDebugDrawMode(const int mode);

  /// @copydoc Dali::DynamicsWorld::SetRootActor
  void SetRootActor(ActorPtr actor);

  /// @copydoc Dali::DynamicsWorld::GetRootActor
  ActorPtr GetRootActor() const;

  SceneGraph::DynamicsWorld* GetSceneObject() const
  {
    return mDynamicsWorld;
  }

private:
  /**
   * Callback, invoked when the Actor set in SetWorldRoot() is added to the scene graph
   * @param actor A handle to the root actor.
   */
  void RootOnStage( Dali::Actor actor );

  /**
   * Callback, invoked when the Actor set in SetWorldRoot() is removed from the scene graph
   * @param actor A handle to the root actor.
   */
  void RootOffStage( Dali::Actor actor );

public:

  // From DynamicsCollisionNotifier

  /**
   * @copydoc DynamicsNotifier::CollisionImpact
   */
  void CollisionImpact( Integration::DynamicsCollisionData* collisionData );

  /**
   * @copydoc DynamicsNotifier::CollisionScrape
   */
  void CollisionScrape( Integration::DynamicsCollisionData* collisionData );

  /**
   * @copydoc DynamicsNotifier::CollisionDisperse
   */
  void CollisionDisperse( Integration::DynamicsCollisionData* collisionData );

  void MapActor(SceneGraph::DynamicsBody* sceneObject, Actor& actor);
  void UnmapActor(SceneGraph::DynamicsBody* sceneObject);
  ActorPtr GetMappedActor(SceneGraph::DynamicsBody* sceneObject) const;

private:
  typedef std::map< SceneGraph::DynamicsBody*, Actor* > BodyContainer;

  int           mDebugMode;
  SceneGraph::DynamicsWorld* mDynamicsWorld;
  Vector3       mGravity; ///< World gravity
  float         mUnit;
  BodyContainer mBodies;
  ActorPtr      mRootActor;

  Dali::DynamicsWorld::CollisionSignalType mCollisionSignal;

  SlotDelegate< DynamicsWorld > mSlotDelegate;
};

// Message helpers

inline MessageBase* CollisionImpactMessage( DynamicsWorld& dynamicsWorld, Integration::DynamicsCollisionData* collisionData )
{
  return new MessageValue1< DynamicsWorld, Integration::DynamicsCollisionData* >( &dynamicsWorld, &DynamicsWorld::CollisionImpact, collisionData );
}

inline MessageBase* CollisionScrapeMessage( DynamicsWorld& dynamicsWorld, Integration::DynamicsCollisionData* collisionData )
{
  return new MessageValue1< DynamicsWorld, Integration::DynamicsCollisionData* >( &dynamicsWorld, &DynamicsWorld::CollisionScrape, collisionData );
}

inline MessageBase* CollisionDisperseMessage( DynamicsWorld& dynamicsWorld, Integration::DynamicsCollisionData* collisionData )
{
  return new MessageValue1< DynamicsWorld, Integration::DynamicsCollisionData* >( &dynamicsWorld, &DynamicsWorld::CollisionDisperse, collisionData );
}

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::DynamicsWorld& GetImplementation(DynamicsWorld& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsWorld object is uninitialized!");

  Dali::RefObject& handle = object.GetBaseObject();

  return static_cast<Internal::DynamicsWorld&>(handle);
}

inline const Internal::DynamicsWorld& GetImplementation(const DynamicsWorld& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsWorld object is uninitialized!");

  const Dali::RefObject& handle = object.GetBaseObject();

  return static_cast<const Internal::DynamicsWorld&>(handle);
}

} // namespace Dali

#endif // __DYNAMICS_WORLD_IMPL_H__
