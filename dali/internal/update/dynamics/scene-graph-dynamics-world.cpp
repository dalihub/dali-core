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

// CLASS HEADER
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>

// EXTERNAL HEADERS
#include <algorithm>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-collision-data.h>
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>
#include <dali/integration-api/dynamics/dynamics-world-intf.h>
#include <dali/integration-api/dynamics/dynamics-world-settings.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/event/dynamics/dynamics-notifier.h>
#include <dali/internal/event/dynamics/dynamics-body-config-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-config-impl.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-body.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-joint.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-shape.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

DynamicsWorld::DynamicsWorld(DynamicsNotifier& dynamicsNotifier,
                             NotificationManager& notificationManager,
                             Integration::DynamicsFactory& dynamicsFactory )
: mDynamicsNotifier(dynamicsNotifier),
  mNotificationManager(notificationManager),
  mSceneController(NULL),
  mBuffers(NULL),
  mDynamicsFactory(dynamicsFactory),
  mDynamicsWorld(NULL),
  mSettings(NULL)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

DynamicsWorld::~DynamicsWorld()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  { // clean up joints
    mDiscardedJoints.Clear();
    mInactiveJoints.Clear();

    DynamicsJointContainer::Iterator it( mJoints.Begin() );
    DynamicsJointContainer::Iterator endIt( mJoints.End() );
    for( ; it != endIt; ++it)
    {
      // remove from simulation
      mDynamicsWorld->RemoveJoint( (*it)->GetJoint() );
    }
    mJoints.Clear();
  }

  { // clean up bodies
    mDiscardedBodies.Clear();
    mInactiveBodies.Clear();
    DynamicsBodyContainer::Iterator it( mBodies.Begin() );
    DynamicsBodyContainer::Iterator endIt( mBodies.End() );
    for( ; it != endIt; ++it)
    {
      // remove from simulation
      mDynamicsWorld->RemoveBody( (*it)->GetBody() );
    }
    mBodies.Clear();
  }

  { // clean up shapes
    mDiscardedShapes.Clear();
    mShapes.Clear();
  }

  // Destroy simulation
  delete mDynamicsWorld;
  delete mSettings;

  // Unload shared object library
  mDynamicsFactory.TerminateDynamics();
}

void DynamicsWorld::Initialize(SceneController* sceneController, Integration::DynamicsWorldSettings* worldSettings, const SceneGraphBuffers* buffers )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::General, "%s\n", __PRETTY_FUNCTION__);

  mSettings = worldSettings;              // take ownership of the DynamicsWorldSettings object

  mDynamicsWorld = mDynamicsFactory.CreateDynamicsWorld();
  mDynamicsWorld->Initialize( *mSettings );

  mSceneController = sceneController;
  mBuffers = buffers;
}

void DynamicsWorld::AddBody(DynamicsBody& body)
{
  // Assert to catch multiple adds
  DALI_ASSERT_DEBUG(std::find(mBodies.Begin(), mBodies.End(), &body) == mBodies.End());

  // remove from inactive bodies container
  DynamicsBodyContainer::Iterator iter = std::find(mInactiveBodies.Begin(), mInactiveBodies.End(), &body);
  if( iter != mInactiveBodies.End() )
  {
    mInactiveBodies.Release(iter);
  }

  mDynamicsWorld->AddBody( body.GetBody() );

  mBodies.PushBack(&body);

  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::General, "%s - (Bodies: %d)\n", __PRETTY_FUNCTION__, (int)(mBodies.Count() + mInactiveBodies.Count()) );
}

void DynamicsWorld::RemoveBody(DynamicsBody& body)
{
  DynamicsBodyContainer::Iterator iter = std::find(mBodies.Begin(), mBodies.End(), &body);
  DALI_ASSERT_DEBUG(iter != mBodies.End());

  mDynamicsWorld->RemoveBody( body.GetBody() );

  // Add to inactive container
  mInactiveBodies.PushBack(*iter);

  // Remove from active container
  mBodies.Release(iter);

  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::General, "%s - (Bodies: %d)\n", __PRETTY_FUNCTION__, (int)(mBodies.Count() + mInactiveBodies.Count()) );
}

void DynamicsWorld::DeleteBody(DynamicsBody& body)
{
  // Assert the body is not in the active bodies container
  DALI_ASSERT_DEBUG(std::find(mBodies.Begin(), mBodies.End(), &body) == mBodies.End());

  // Assert the body is in the inactive bodies container
  DynamicsBodyContainer::Iterator iter = std::find(mInactiveBodies.Begin(), mInactiveBodies.End(), &body);
  DALI_ASSERT_DEBUG(iter != mInactiveBodies.End());

  // Add body to be discarded container
  mDiscardedBodies.PushBack(&body);

  // Remove from inactive container
  mInactiveBodies.Release(iter);

  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::General, "%s - (Bodies: %d)\n", __PRETTY_FUNCTION__, (int)(mBodies.Count() + mInactiveBodies.Count()) );
}

void DynamicsWorld::AddJoint(DynamicsJoint& joint)
{
  // Assert to catch multiple adds
  DALI_ASSERT_DEBUG(std::find(mJoints.Begin(), mJoints.End(), &joint) == mJoints.End());

  // remove from inactive joint container
  DynamicsJointContainer::Iterator iter = std::find(mInactiveJoints.Begin(), mInactiveJoints.End(), &joint);
  if( iter != mInactiveJoints.End() )
  {
    mInactiveJoints.Release(iter);
  }

  // Add to simulation
  mDynamicsWorld->AddJoint( joint.GetJoint() );

  // Add to active container
  mJoints.PushBack(&joint);

  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::General, "%s - (Joints: %d)\n", __PRETTY_FUNCTION__, (int)(mJoints.Count() + mInactiveJoints.Count()));
}

void DynamicsWorld::RemoveJoint(DynamicsJoint& joint)
{
  // Assert joint is in active queue
  DynamicsJointContainer::Iterator iter = std::find(mJoints.Begin(), mJoints.End(), &joint);
  DALI_ASSERT_DEBUG(iter != mJoints.End());

  // remove from simulation
  mDynamicsWorld->RemoveJoint( joint.GetJoint() );

  // Add to inactive container
  mInactiveJoints.PushBack(*iter);

  // Remove from active container
  mJoints.Release(iter);

  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::General, "%s - (Joints: %d)\n", __PRETTY_FUNCTION__, (int)(mJoints.Count() + mInactiveJoints.Count()));
}

void DynamicsWorld::DeleteJoint(DynamicsJoint& joint)
{
  // Assert the joint is not in the active joints container
  DALI_ASSERT_DEBUG(std::find(mJoints.Begin(), mJoints.End(), &joint) == mJoints.End());

  // Assert the joint is in the inactive container
  DynamicsJointContainer::Iterator iter = std::find(mInactiveJoints.Begin(), mInactiveJoints.End(), &joint);
  DALI_ASSERT_DEBUG(iter != mInactiveJoints.End());

  // add to discarded container
  mDiscardedJoints.PushBack(&joint);

  // remove from inactive container
  mInactiveJoints.Release(iter);

  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::General, "%s - (Joints: %d)\n", __PRETTY_FUNCTION__, (int)(mJoints.Count() + mInactiveJoints.Count()));
}

void DynamicsWorld::AddShape(DynamicsShape& shape)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  // Assert to catch multiple adds
  DALI_ASSERT_DEBUG(std::find(mShapes.Begin(), mShapes.End(), &shape) == mShapes.End());

  // Add to active container
  mShapes.PushBack(&shape);
}

void DynamicsWorld::DeleteShape(DynamicsShape& shape)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  // Assert the shape is in the container
  DynamicsShapeContainer::Iterator iter = std::find(mShapes.Begin(), mShapes.End(), &shape);
  DALI_ASSERT_DEBUG(iter != mShapes.End());

  // add to discarded container
  mDiscardedShapes.PushBack(&shape);

  // remove from container
  mShapes.Release(iter);
}

void DynamicsWorld::SetGravity( const Vector3& gravity )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - gravity(%f %f %f)\n", __PRETTY_FUNCTION__,
                gravity.x, gravity.y, gravity.z);

  mDynamicsWorld->SetGravity( gravity );
}

BufferIndex DynamicsWorld::GetBufferIndex() const
{
  return mBuffers->GetUpdateBufferIndex();
}

Integration::DynamicsFactory& DynamicsWorld::GetDynamicsFactory()
{
  return mDynamicsFactory;
}

Integration::DynamicsWorld& DynamicsWorld::GetDynamicsWorld()
{
  return *mDynamicsWorld;
}

bool DynamicsWorld::Update( float elapsedSeconds )
{
  bool anyPositionChanged( false );

  if( mNode )
  {
    PreSimulationStep();

    mDynamicsWorld->Update( elapsedSeconds );

    anyPositionChanged = PostSimulationStep();

    if( anyPositionChanged )
    {
      CheckForCollisions();
    }
  }

  return anyPositionChanged;
}

void DynamicsWorld::PreSimulationStep()
{
  DynamicsBodyContainer::Iterator it = mBodies.Begin();
  DynamicsBodyContainer::Iterator end_it = mBodies.End();

  for( ; it != end_it; ++it )
  {
    DynamicsBody* body(*it);
    if( Dali::DynamicsBodyConfig::RIGID == body->GetType() )
    {
      if( body->IsKinematic() )
      {
        body->RefreshDynamics();
      }
    }
  }
}

bool DynamicsWorld::PostSimulationStep()
{
  bool anyPositionChanged(false);

  DynamicsBodyContainer::Iterator it = mBodies.Begin();
  DynamicsBodyContainer::Iterator end_it = mBodies.End();

  for( ; it != end_it; ++it )
  {
    DynamicsBody* body(*it);
    anyPositionChanged |= body->RefreshNode( GetBufferIndex() );
  }

  // clean up discarded objects
  mDiscardedJoints.Clear();
  mDiscardedBodies.Clear();
  mDiscardedShapes.Clear();

  return anyPositionChanged;
}


// TODO: This needs to be totally rewritten and rationalized
// 1) Keep track of contact points between objects
// 2) Signal new collision between objects (impact)
// 3) Signal added contact points between already colliding objects (crumple)
// 4) Signal contact points that are moving (scraping)
// 5) Signal objects that are no longer colliding (disperse)
void DynamicsWorld::CheckForCollisions()
{
  Integration::CollisionDataContainer newContacts;

  mDynamicsWorld->CheckForCollisions( newContacts );

  const float worldScale( GetWorldScale());

  /* Check for added contacts ... */
  Integration::CollisionDataContainer::iterator it;
  if( !newContacts.empty() )
  {
    for( it = newContacts.begin(); it != newContacts.end(); ++it )
    {
      if( mContacts.find( (*it).first ) == mContacts.end() )
      {
        // signal new collision
        Integration::DynamicsCollisionData& data = (*it).second;

        DALI_ASSERT_DEBUG(NULL != data.mBodyA);
        DALI_ASSERT_DEBUG(NULL != data.mBodyB);

        data.mPointOnA *= worldScale;
        data.mPointOnB *= worldScale;

        mNotificationManager.QueueMessage( CollisionImpactMessage( mDynamicsNotifier, new Integration::DynamicsCollisionData(data) ) );
      }
      else
      {
        // Remove to filter no more active contacts
        mContacts.erase( (*it).first );
      }
    }
  }

  /* ... and removed contacts */
  if (!mContacts.empty())
  {
    for (it = mContacts.begin(); it != mContacts.end(); ++it)
    {
      // signal end of collision
      Integration::DynamicsCollisionData& data = (*it).second;

      DALI_ASSERT_DEBUG(NULL != data.mBodyA);
      DALI_ASSERT_DEBUG(NULL != data.mBodyB);

      mNotificationManager.QueueMessage( CollisionDisperseMessage( mDynamicsNotifier, new Integration::DynamicsCollisionData(data) ) );
    }
    mContacts.clear();
  }

  // store current/active contacts
  mContacts = newContacts;
}

void DynamicsWorld::SetNode(Node* node)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s (node:%p)\n", __PRETTY_FUNCTION__, node);
  mNode = node;
}

/// @copydoc Dali::DynamicsWorld::GetWorldScale
float DynamicsWorld::GetWorldScale() const
{
  return 1.0f / mSettings->worldScale;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
