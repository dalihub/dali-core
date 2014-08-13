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
#include <dali/internal/event/actors/actor-impl.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <algorithm>

// INTERNAL INCLUDES

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/scripting/scripting.h>

#include <dali/internal/common/internal-constants.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/common/property-index-ranges.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/actor-attachments/actor-attachment-impl.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/effects/shader-effect-impl.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/internal/update/nodes/node-messages.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/internal/update/animation/scene-graph-constraint.h>
#include <dali/internal/event/events/actor-gesture-data.h>
#include <dali/internal/common/message.h>
#include <dali/integration-api/debug.h>

#ifdef DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-body-config-impl.h>
#include <dali/internal/event/dynamics/dynamics-body-impl.h>
#include <dali/internal/event/dynamics/dynamics-joint-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#endif

using Dali::Internal::SceneGraph::Node;
using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::PropertyBase;

using namespace std;

namespace Dali
{

const Property::Index Actor::PARENT_ORIGIN              = 0;
const Property::Index Actor::PARENT_ORIGIN_X            = 1;
const Property::Index Actor::PARENT_ORIGIN_Y            = 2;
const Property::Index Actor::PARENT_ORIGIN_Z            = 3;
const Property::Index Actor::ANCHOR_POINT               = 4;
const Property::Index Actor::ANCHOR_POINT_X             = 5;
const Property::Index Actor::ANCHOR_POINT_Y             = 6;
const Property::Index Actor::ANCHOR_POINT_Z             = 7;
const Property::Index Actor::SIZE                       = 8;
const Property::Index Actor::SIZE_WIDTH                 = 9;
const Property::Index Actor::SIZE_HEIGHT                = 10;
const Property::Index Actor::SIZE_DEPTH                 = 11;
const Property::Index Actor::POSITION                   = 12;
const Property::Index Actor::POSITION_X                 = 13;
const Property::Index Actor::POSITION_Y                 = 14;
const Property::Index Actor::POSITION_Z                 = 15;
const Property::Index Actor::WORLD_POSITION             = 16;
const Property::Index Actor::WORLD_POSITION_X           = 17;
const Property::Index Actor::WORLD_POSITION_Y           = 18;
const Property::Index Actor::WORLD_POSITION_Z           = 19;
const Property::Index Actor::ROTATION                   = 20;
const Property::Index Actor::WORLD_ROTATION             = 21;
const Property::Index Actor::SCALE                      = 22;
const Property::Index Actor::SCALE_X                    = 23;
const Property::Index Actor::SCALE_Y                    = 24;
const Property::Index Actor::SCALE_Z                    = 25;
const Property::Index Actor::WORLD_SCALE                = 26;
const Property::Index Actor::VISIBLE                    = 27;
const Property::Index Actor::COLOR                      = 28;
const Property::Index Actor::COLOR_RED                  = 29;
const Property::Index Actor::COLOR_GREEN                = 30;
const Property::Index Actor::COLOR_BLUE                 = 31;
const Property::Index Actor::COLOR_ALPHA                = 32;
const Property::Index Actor::WORLD_COLOR                = 33;
const Property::Index Actor::WORLD_MATRIX               = 34;
const Property::Index Actor::NAME                       = 35;
const Property::Index Actor::SENSITIVE                  = 36;
const Property::Index Actor::LEAVE_REQUIRED             = 37;
const Property::Index Actor::INHERIT_ROTATION           = 38;
const Property::Index Actor::INHERIT_SCALE              = 39;
const Property::Index Actor::COLOR_MODE                 = 40;
const Property::Index Actor::POSITION_INHERITANCE       = 41;
const Property::Index Actor::DRAW_MODE                  = 42;

namespace // unnamed namespace
{

/**
 * We want to discourage the use of property strings (minimize string comparisons),
 * particularly for the default properties.
 */
const Internal::PropertyDetails DEFAULT_PROPERTY_DETAILS[] =
{
  // Name                     Type              writable animatable constraint-input
  { "parent-origin",          Property::VECTOR3,  true,    false,   true  },  // PARENT_ORIGIN
  { "parent-origin-x",        Property::FLOAT,    true,    false,   true  },  // PARENT_ORIGIN_X
  { "parent-origin-y",        Property::FLOAT,    true,    false,   true  },  // PARENT_ORIGIN_Y
  { "parent-origin-z",        Property::FLOAT,    true,    false,   true  },  // PARENT_ORIGIN_Z
  { "anchor-point",           Property::VECTOR3,  true,    false,   true  },  // ANCHOR_POINT
  { "anchor-point-x",         Property::FLOAT,    true,    false,   true  },  // ANCHOR_POINT_X
  { "anchor-point-y",         Property::FLOAT,    true,    false,   true  },  // ANCHOR_POINT_Y
  { "anchor-point-z",         Property::FLOAT,    true,    false,   true  },  // ANCHOR_POINT_Z
  { "size",                   Property::VECTOR3,  true,    true,    true  },  // SIZE
  { "size-width",             Property::FLOAT,    true,    true,    true  },  // SIZE_WIDTH
  { "size-height",            Property::FLOAT,    true,    true,    true  },  // SIZE_HEIGHT
  { "size-depth",             Property::FLOAT,    true,    true,    true  },  // SIZE_DEPTH
  { "position",               Property::VECTOR3,  true,    true,    true  },  // POSITION
  { "position-x",             Property::FLOAT,    true,    true,    true  },  // POSITION_X
  { "position-y",             Property::FLOAT,    true,    true,    true  },  // POSITION_Y
  { "position-z",             Property::FLOAT,    true,    true,    true  },  // POSITION_Z
  { "world-position",         Property::VECTOR3,  false,   false,   true  },  // WORLD_POSITION
  { "world-position-x",       Property::FLOAT,    false,   false,   true  },  // WORLD_POSITION_X
  { "world-position-y",       Property::FLOAT,    false,   false,   true  },  // WORLD_POSITION_Y
  { "world-position-z",       Property::FLOAT,    false,   false,   true  },  // WORLD_POSITION_Z
  { "rotation",               Property::ROTATION, true,    true,    true  },  // ROTATION
  { "world-rotation",         Property::ROTATION, false,   false,   true  },  // WORLD_ROTATION
  { "scale",                  Property::VECTOR3,  true,    true,    true  },  // SCALE
  { "scale-x",                Property::FLOAT,    true,    true,    true  },  // SCALE_X
  { "scale-y",                Property::FLOAT,    true,    true,    true  },  // SCALE_Y
  { "scale-z",                Property::FLOAT,    true,    true,    true  },  // SCALE_Z
  { "world-scale",            Property::VECTOR3,  false,   false,   true  },  // WORLD_SCALE
  { "visible",                Property::BOOLEAN,  true,    true,    true  },  // VISIBLE
  { "color",                  Property::VECTOR4,  true,    true,    true  },  // COLOR
  { "color-red",              Property::FLOAT,    true,    true,    true  },  // COLOR_RED
  { "color-green",            Property::FLOAT,    true,    true,    true  },  // COLOR_GREEN
  { "color-blue",             Property::FLOAT,    true,    true,    true  },  // COLOR_BLUE
  { "color-alpha",            Property::FLOAT,    true,    true,    true  },  // COLOR_ALPHA
  { "world-color",            Property::VECTOR4,  false,   false,   true  },  // WORLD_COLOR
  { "world-matrix",           Property::MATRIX,   false,   false,   true  },  // WORLD_MATRIX
  { "name",                   Property::STRING,   true,    false,   false },  // NAME
  { "sensitive",              Property::BOOLEAN,  true,    false,   false },  // SENSITIVE
  { "leave-required",         Property::BOOLEAN,  true,    false,   false },  // LEAVE_REQUIRED
  { "inherit-rotation",       Property::BOOLEAN,  true,    false,   false },  // INHERIT_ROTATION
  { "inherit-scale",          Property::BOOLEAN,  true,    false,   false },  // INHERIT_SCALE
  { "color-mode",             Property::STRING,   true,    false,   false },  // COLOR_MODE
  { "position-inheritance",   Property::STRING,   true,    false,   false },  // POSITION_INHERITANCE
  { "draw-mode",              Property::STRING,   true,    false,   false },  // DRAW_MODE
};
const int DEFAULT_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_DETAILS ) / sizeof( Internal::PropertyDetails );

} // unnamed namespace

namespace Internal
{

unsigned int Actor::mActorCounter = 0;
ActorContainer Actor::mNullChildren;

#ifdef DYNAMICS_SUPPORT

// Encapsulate actor related dynamics data
struct DynamicsData
{
  DynamicsData( Actor* slotOwner )
  : slotDelegate( slotOwner )
  {
  }

  typedef std::map<Actor*, DynamicsJointPtr> JointContainer;
  typedef std::vector<DynamicsJointPtr>      ReferencedJointContainer;

  DynamicsBodyPtr          body;
  JointContainer           joints;
  ReferencedJointContainer referencedJoints;

  SlotDelegate< Actor > slotDelegate;
};

#endif // DYNAMICS_SUPPORT

namespace
{

using namespace Dali;

BaseHandle CreateActor()
{
  return Dali::Actor::New();
}

TypeRegistration mType( typeid(Dali::Actor), typeid(Dali::Handle), CreateActor );

SignalConnectorType signalConnector1(mType, Dali::Actor::SIGNAL_TOUCHED,    &Actor::DoConnectSignal);
SignalConnectorType signalConnector2(mType, Dali::Actor::SIGNAL_SET_SIZE,   &Actor::DoConnectSignal);
SignalConnectorType signalConnector3(mType, Dali::Actor::SIGNAL_ON_STAGE,   &Actor::DoConnectSignal);
SignalConnectorType signalConnector4(mType, Dali::Actor::SIGNAL_OFF_STAGE,  &Actor::DoConnectSignal);

TypeAction a1(mType, Dali::Actor::ACTION_SHOW, &Actor::DoAction);
TypeAction a2(mType, Dali::Actor::ACTION_HIDE, &Actor::DoAction);

}

Actor::DefaultPropertyLookup* Actor::mDefaultPropertyLookup = NULL;

ActorPtr Actor::New()
{
  ActorPtr actor( new Actor( BASIC ) );

  // Second-phase construction
  actor->Initialize();

  return actor;
}

const std::string& Actor::GetName() const
{
  return mName;
}

void Actor::SetName(const std::string& name)
{
  mName = name;

  if( NULL != mNode )
  {
    // ATTENTION: string for debug purposes is not thread safe.
    DALI_LOG_SET_OBJECT_STRING( const_cast< SceneGraph::Node* >( mNode ), name );
  }
}

unsigned int Actor::GetId() const
{
  return mId;
}

void Actor::Attach( ActorAttachment& attachment )
{
  DALI_ASSERT_DEBUG( !mAttachment && "An Actor can only have one attachment" );

  if( OnStage() )
  {
    attachment.Connect();
  }

  mAttachment = ActorAttachmentPtr(&attachment);
}

ActorAttachmentPtr Actor::GetAttachment()
{
  return mAttachment;
}

bool Actor::OnStage() const
{
  return mIsOnStage;
}

Dali::Layer Actor::GetLayer()
{
  Dali::Layer layer;

  // Short-circuit for Layer derived actors
  if( mIsLayer )
  {
    layer = Dali::Layer( static_cast< Dali::Internal::Layer* >( this ) ); // static cast as we trust the flag
  }

  // Find the immediate Layer parent
  for (Actor* parent = mParent; !layer && parent != NULL; parent = parent->GetParent())
  {
    if( parent->IsLayer() )
    {
      layer = Dali::Layer( static_cast< Dali::Internal::Layer* >( parent ) ); // static cast as we trust the flag
    }
  }

  return layer;
}

void Actor::Add(Actor& child)
{
  DALI_ASSERT_ALWAYS( this != &child && "Cannot add actor to itself" );
  DALI_ASSERT_ALWAYS( !child.IsRoot() && "Cannot add root actor" );

  if( !mChildren )
  {
    mChildren = new ActorContainer;
  }

  Actor* const oldParent( child.mParent );

  // child might already be ours
  if( this != oldParent )
  {
    // if we already have parent, unparent us first
    if( oldParent )
    {
      oldParent->Remove( child ); // This causes OnChildRemove callback
    }

    // Guard against Add() during previous OnChildRemove callback
    if ( !child.mParent )
    {
      // Do this first, since user callbacks from within SetParent() may need to remove child
      mChildren->push_back(Dali::Actor(&child));

      // SetParent asserts that child can be added
      child.SetParent(this);

      // Notification for derived classes
      OnChildAdd(child);
    }
  }
}

void Actor::Remove(Actor& child)
{
  DALI_ASSERT_ALWAYS( this != &child && "Cannot remove actor from itself" );

  Dali::Actor removed;

  if( !mChildren )
  {
    // no children
    return;
  }

  // Find the child in mChildren, and unparent it
  ActorIter end = mChildren->end();
  for( ActorIter iter = mChildren->begin(); iter != end; ++iter )
  {
    Actor& actor = GetImplementation(*iter);

    if( &actor == &child )
    {
      // Keep handle for OnChildRemove notification
      removed = Dali::Actor( &actor );

      // Do this first, since user callbacks from within SetParent() may need to add the child
      mChildren->erase(iter);

      DALI_ASSERT_DEBUG( actor.GetParent() == this );
      actor.SetParent( NULL );

      break;
    }
  }

  if ( removed )
  {
    // Notification for derived classes
    OnChildRemove( GetImplementation(removed) );
  }
}

void Actor::Unparent()
{
  if( mParent )
  {
    mParent->Remove( *this );
  }
}

unsigned int Actor::GetChildCount() const
{
  return ( NULL != mChildren ) ? mChildren->size() : 0;
}

Dali::Actor Actor::GetChildAt(unsigned int index) const
{
  DALI_ASSERT_ALWAYS( index < GetChildCount() );

  return ( ( mChildren ) ? (*mChildren)[index] : Dali::Actor() );
}

ActorContainer Actor::GetChildren()
{
  if( NULL != mChildren )
  {
    return *mChildren;
  }

  // return copy of mNullChildren
  return mNullChildren;
}

const ActorContainer& Actor::GetChildren() const
{
  if( NULL != mChildren )
  {
    return *mChildren;
  }

  // return const reference to mNullChildren
  return mNullChildren;
}

ActorPtr Actor::FindChildByName(const std::string& actorName)
{
  ActorPtr child=0;
  if (actorName == mName)
  {
    child = this;
  }
  else if( mChildren )
  {
    ActorIter end = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != end; ++iter )
    {
      child = GetImplementation(*iter).FindChildByName(actorName);

      if (child)
      {
        break;
      }
    }
  }
  return child;
}

Dali::Actor Actor::FindChildByAlias(const std::string& actorAlias)
{
  Dali::Actor child = DoGetChildByAlias(actorAlias);

  // If not found then search by name.
  if (!child)
  {
    Internal::ActorPtr child_ptr = FindChildByName(actorAlias);
    if (child_ptr)
    {
      child = Dali::Actor(child_ptr.Get());
    }
  }

  return child;
}

Dali::Actor Actor::DoGetChildByAlias(const std::string& actorAlias)
{
  Dali::Actor child = GetChildByAlias(actorAlias);

  if (!child && mChildren )
  {
    ActorIter end = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != end; ++iter )
    {
      child = GetImplementation(*iter).DoGetChildByAlias(actorAlias);

      if (child)
      {
        break;
      }
    }
  }

  return child;
}

ActorPtr Actor::FindChildById(const unsigned int id)
{
  ActorPtr child = 0;
  if (id == mId)
  {
    child = this;
  }
  else if( mChildren )
  {
    ActorIter end = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != end; ++iter )
    {
      child = GetImplementation(*iter).FindChildById(id);

      if (child)
      {
        break;
      }
    }
  }
  return child;
}

void Actor::SetParentOrigin( const Vector3& origin )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SetParentOriginMessage( mStage->GetUpdateInterface(), *mNode, origin );
  }

  // Cache for event-thread access
  if( !mParentOrigin )
  {
    // not allocated, check if different from default
    if( ParentOrigin::DEFAULT != origin )
    {
      mParentOrigin = new Vector3( origin );
    }
  }
  else
  {
    // check if different from current costs more than just set
    *mParentOrigin = origin;
  }
}

void Actor::SetParentOriginX( float x )
{
  const Vector3& current = GetCurrentParentOrigin();

  SetParentOrigin( Vector3( x, current.y, current.z ) );
}

void Actor::SetParentOriginY( float y )
{
  const Vector3& current = GetCurrentParentOrigin();

  SetParentOrigin( Vector3( current.x, y, current.z ) );
}

void Actor::SetParentOriginZ( float z )
{
  const Vector3& current = GetCurrentParentOrigin();

  SetParentOrigin( Vector3( current.x, current.y, z ) );
}

const Vector3& Actor::GetCurrentParentOrigin() const
{
  // Cached for event-thread access
  return ( mParentOrigin ) ? *mParentOrigin : ParentOrigin::DEFAULT;
}

void Actor::SetAnchorPoint(const Vector3& anchor)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SetAnchorPointMessage( mStage->GetUpdateInterface(), *mNode, anchor );
  }

  // Cache for event-thread access
  if( !mAnchorPoint )
  {
    // not allocated, check if different from default
    if( AnchorPoint::DEFAULT != anchor )
    {
      mAnchorPoint = new Vector3( anchor );
    }
  }
  else
  {
    // check if different from current costs more than just set
    *mAnchorPoint = anchor;
  }
}

void Actor::SetAnchorPointX( float x )
{
  const Vector3& current = GetCurrentAnchorPoint();

  SetAnchorPoint( Vector3( x, current.y, current.z ) );
}

void Actor::SetAnchorPointY( float y )
{
  const Vector3& current = GetCurrentAnchorPoint();

  SetAnchorPoint( Vector3( current.x, y, current.z ) );
}

void Actor::SetAnchorPointZ( float z )
{
  const Vector3& current = GetCurrentAnchorPoint();

  SetAnchorPoint( Vector3( current.x, current.y, z ) );
}

const Vector3& Actor::GetCurrentAnchorPoint() const
{
  // Cached for event-thread access
  return ( mAnchorPoint ) ? *mAnchorPoint : AnchorPoint::DEFAULT;
}

void Actor::SetPosition(float x, float y)
{
  SetPosition(Vector3(x, y, 0.0f));
}

void Actor::SetPosition(float x, float y, float z)
{
  SetPosition(Vector3(x, y, z));
}

void Actor::SetPosition(const Vector3& position)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::Bake, position );
  }
}

void Actor::SetX(float x)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::BakeX, x );
  }
}

void Actor::SetY(float y)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::BakeY, y );
  }
}

void Actor::SetZ(float z)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::BakeZ, z );
  }
}

void Actor::MoveBy(const Vector3& distance)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::BakeRelative, distance );
  }
}

const Vector3& Actor::GetCurrentPosition() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetPosition(mStage->GetEventBufferIndex());
  }

  return Vector3::ZERO;
}

const Vector3& Actor::GetCurrentWorldPosition() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetWorldPosition( mStage->GetEventBufferIndex() );
  }

  return Vector3::ZERO;
}

void Actor::SetPositionInheritanceMode( PositionInheritanceMode mode )
{
  // this flag is not animatable so keep the value
  mPositionInheritanceMode = mode;
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetPositionInheritanceModeMessage( mStage->GetUpdateInterface(), *mNode, mode );
  }
}

PositionInheritanceMode Actor::GetPositionInheritanceMode() const
{
  // Cached for event-thread access
  return mPositionInheritanceMode;
}

void Actor::SetRotation(const Radian& angle, const Vector3& axis)
{
  Vector4 normalizedAxis(axis.x, axis.y, axis.z, 0.0f);
  normalizedAxis.Normalize();

  Quaternion rotation(Quaternion::FromAxisAngle(normalizedAxis, angle));

  SetRotation(rotation);
}

void Actor::SetRotation(const Quaternion& rotation)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Quaternion>::Send( mStage->GetUpdateManager(), mNode, &mNode->mRotation, &AnimatableProperty<Quaternion>::Bake, rotation );
  }
}

void Actor::RotateBy(const Radian& angle, const Vector3& axis)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Quaternion>::Send( mStage->GetUpdateManager(), mNode, &mNode->mRotation, &AnimatableProperty<Quaternion>::BakeRelative, Quaternion(angle, axis) );
  }
}

void Actor::RotateBy(const Quaternion& relativeRotation)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Quaternion>::Send( mStage->GetUpdateManager(), mNode, &mNode->mRotation, &AnimatableProperty<Quaternion>::BakeRelative, relativeRotation );
  }
}

const Quaternion& Actor::GetCurrentRotation() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetRotation(mStage->GetEventBufferIndex());
  }

  return Quaternion::IDENTITY;
}

const Quaternion& Actor::GetCurrentWorldRotation() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetWorldRotation( mStage->GetEventBufferIndex() );
  }

  return Quaternion::IDENTITY;
}

void Actor::SetScale(float scale)
{
  SetScale(Vector3(scale, scale, scale));
}

void Actor::SetScale(float x, float y, float z)
{
  SetScale(Vector3(x, y, z));
}

void Actor::SetScale(const Vector3& scale)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mScale, &AnimatableProperty<Vector3>::Bake, scale );
  }
}

void Actor::SetScaleX( float x )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mScale, &AnimatableProperty<Vector3>::BakeX, x );
  }
}

void Actor::SetScaleY( float y )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mScale, &AnimatableProperty<Vector3>::BakeY, y );
  }
}

void Actor::SetScaleZ( float z )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mScale, &AnimatableProperty<Vector3>::BakeZ, z );
  }
}

void Actor::SetInitialVolume(const Vector3& volume)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetInitialVolumeMessage( mStage->GetUpdateInterface(), *mNode, volume );
  }
}

void Actor::SetTransmitGeometryScaling(bool transmitGeometryScaling)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetTransmitGeometryScalingMessage( mStage->GetUpdateInterface(), *mNode, transmitGeometryScaling );
  }
}

bool Actor::GetTransmitGeometryScaling() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetTransmitGeometryScaling();
  }

  return false;
}

void Actor::ScaleBy(const Vector3& relativeScale)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mScale, &AnimatableProperty<Vector3>::BakeRelativeMultiply, relativeScale );
  }
}

const Vector3& Actor::GetCurrentScale() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetScale(mStage->GetEventBufferIndex());
  }

  return Vector3::ONE;
}

const Vector3& Actor::GetCurrentWorldScale() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetWorldScale( mStage->GetEventBufferIndex() );
  }

  return Vector3::ONE;
}

void Actor::SetInheritScale( bool inherit )
{
  // non animateable so keep local copy
  mInheritScale = inherit;
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetInheritScaleMessage( mStage->GetUpdateInterface(), *mNode, inherit );
  }
}

bool Actor::IsScaleInherited() const
{
  return mInheritScale;
}

Matrix Actor::GetCurrentWorldMatrix() const
{
  if( NULL != mNode )
  {
    // World matrix is no longer updated unless there is something observing the node.
    // Need to calculate it from node's world position, rotation and scale:
    BufferIndex updateBufferIndex = mStage->GetEventBufferIndex();
    Matrix worldMatrix(false);
    worldMatrix.SetTransformComponents( mNode->GetWorldScale( updateBufferIndex ),
                                        mNode->GetWorldRotation( updateBufferIndex ),
                                        mNode->GetWorldPosition( updateBufferIndex ) );
    return worldMatrix;
  }

  return Matrix::IDENTITY;
}

void Actor::SetVisible(bool visible)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<bool>::Send( mStage->GetUpdateManager(), mNode, &mNode->mVisible, &AnimatableProperty<bool>::Bake, visible );
  }
}

bool Actor::IsVisible() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->IsVisible( mStage->GetEventBufferIndex() );
  }

  return true;
}

void Actor::SetOpacity(float opacity)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( mStage->GetUpdateManager(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeW, opacity );
  }
}

void Actor::OpacityBy(float relativeOpacity)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( mStage->GetUpdateManager(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeWRelative, relativeOpacity );
  }
}

float Actor::GetCurrentOpacity() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetOpacity(mStage->GetEventBufferIndex());
  }

  return 1.0f;
}

const Vector4& Actor::GetCurrentWorldColor() const
{
  if( NULL != mNode )
  {
    return mNode->GetWorldColor( mStage->GetEventBufferIndex() );
  }

  return Color::WHITE;
}

void Actor::SetColor(const Vector4& color)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector4>::Send( mStage->GetUpdateManager(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::Bake, color );
  }
}

void Actor::SetColorRed( float red )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( mStage->GetUpdateManager(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeX, red );
  }
}

void Actor::SetColorGreen( float green )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( mStage->GetUpdateManager(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeY, green );
  }
}

void Actor::SetColorBlue( float blue )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( mStage->GetUpdateManager(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeZ, blue );
  }
}

void Actor::ColorBy(const Vector4& relativeColor)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector4>::Send( mStage->GetUpdateManager(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeRelative, relativeColor );
  }
}

const Vector4& Actor::GetCurrentColor() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetColor(mStage->GetEventBufferIndex());
  }

  return Color::WHITE;
}

void Actor::SetInheritRotation(bool inherit)
{
  // non animateable so keep local copy
  mInheritRotation = inherit;
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetInheritRotationMessage( mStage->GetUpdateInterface(), *mNode, inherit );
  }
}

bool Actor::IsRotationInherited() const
{
  return mInheritRotation;
}

void Actor::SetColorMode(ColorMode colorMode)
{
  // non animateable so keep local copy
  mColorMode = colorMode;
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetColorModeMessage( mStage->GetUpdateInterface(), *mNode, colorMode );
  }
}

ColorMode Actor::GetColorMode() const
{
  // we have cached copy
  return mColorMode;
}

void Actor::SetSize(float width, float height)
{
  SetSize( Vector2( width, height ) );
}

void Actor::SetSize(float width, float height, float depth)
{
  SetSize( Vector3( width, height, depth ) );
}

void Actor::SetSize(const Vector2& size)
{
  Vector3 volume( size );
  volume.z = std::min( size.width, size.height );
  SetSize( volume );
}

void Actor::SetSize(const Vector3& size)
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mSize, &AnimatableProperty<Vector3>::Bake, size );

    // Notification for derived classes
    OnSizeSet(size);

    // Emit signal for application developer

    if( !mSetSizeSignalV2.Empty() )
    {
      Dali::Actor handle( this );
      mSetSizeSignalV2.Emit( handle, size );
    }
  }
}

void Actor::SetWidth( float width )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mSize, &AnimatableProperty<Vector3>::BakeX, width );
  }
}

void Actor::SetHeight( float height )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mSize, &AnimatableProperty<Vector3>::BakeY, height );
  }
}

void Actor::SetDepth( float depth )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mSize, &AnimatableProperty<Vector3>::BakeZ, depth );
  }
}

const Vector3& Actor::GetCurrentSize() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetSize( mStage->GetEventBufferIndex() );
  }

  return Vector3::ZERO;
}

void Actor::SetShaderEffect(ShaderEffect& effect)
{
  // no-op on an Actor
}

ShaderEffectPtr Actor::GetShaderEffect() const
{
  return ShaderEffectPtr();
}

void Actor::RemoveShaderEffect()
{
}

#ifdef DYNAMICS_SUPPORT

//--------------- Dynamics ---------------

void Actor::DisableDynamics()
{
  if( NULL != mDynamicsData )
  {
    DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s- (\"%s\")\n", __PRETTY_FUNCTION__, mName.c_str());

    // ensure dynamics object are disconnected from scene
    DisconnectDynamics();

    // delete joint owned by this actor
    while( !mDynamicsData->joints.empty() )
    {
      RemoveDynamicsJoint( mDynamicsData->joints.begin()->second );
    }

    // delete other joints referencing this actor
    while( !mDynamicsData->referencedJoints.empty() )
    {
      DynamicsJointPtr joint( *(mDynamicsData->referencedJoints.begin()) );
      ActorPtr jointOwner( joint->GetActor( true ) );
      if( jointOwner )
      {
        jointOwner->RemoveDynamicsJoint( joint );
      }
      else
      {
        mDynamicsData->referencedJoints.erase( mDynamicsData->referencedJoints.begin() );
      }
    }
    // delete the DynamicsBody object
    mDynamicsData->body.Reset();

    // Discard Dynamics data structure
    delete mDynamicsData;
    mDynamicsData = NULL;
  }
}

DynamicsBodyPtr Actor::GetDynamicsBody() const
{
  DynamicsBodyPtr body;

  if( NULL != mDynamicsData )
  {
    body = mDynamicsData->body;
  }

  return body;
}

DynamicsBodyPtr Actor::EnableDynamics(DynamicsBodyConfigPtr bodyConfig)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s- (\"%s\")\n", __PRETTY_FUNCTION__, mName.c_str());

  if( NULL == mDynamicsData )
  {
    mDynamicsData = new DynamicsData( this );
  }

  if( !mDynamicsData->body )
  {
    mDynamicsData->body = new DynamicsBody(mName, bodyConfig, *this, *(const_cast<SceneGraph::Node*>(mNode)) );

    if( OnStage() )
    {
      DynamicsWorldPtr world( mStage->GetDynamicsWorld() );
      if( world )
      {
        if( mParent == world->GetRootActor().Get() )
        {
          mDynamicsData->body->Connect(*mStage);
        }
      }
    }
  }

  return mDynamicsData->body;
}

DynamicsJointPtr Actor::AddDynamicsJoint( ActorPtr attachedActor, const Vector3& offset )
{
  DALI_ASSERT_ALWAYS( attachedActor && "'attachedActor' must be initialized!" );
  return AddDynamicsJoint( attachedActor, offset, ( GetCurrentPosition() + offset ) - attachedActor->GetCurrentPosition() );
}

DynamicsJointPtr Actor::AddDynamicsJoint( ActorPtr attachedActor, const Vector3& offsetA, const Vector3& offsetB )
{
  DALI_ASSERT_ALWAYS( attachedActor && "'attachedActor' must be initialized!" );
  DALI_ASSERT_ALWAYS( this != attachedActor.Get() && "Cannot create a joint to oneself!" );

  DynamicsJointPtr joint;

  DynamicsWorldPtr world( mStage->GetDynamicsWorld() );

  if( world )
  {
    if( NULL != mDynamicsData )
    {
      DynamicsData::JointContainer::iterator it( mDynamicsData->joints.find( attachedActor.Get() ) );

      if( mDynamicsData->joints.end() != it )
      {
        // use existing joint
        joint = it->second;
      }

      if( !joint )
      {
        DynamicsBodyPtr bodyA( GetDynamicsBody() );
        DynamicsBodyPtr bodyB( attachedActor->GetDynamicsBody() );

        if( !bodyA )
        {
          bodyA = EnableDynamics( new DynamicsBodyConfig );
        }

        if( !bodyB )
        {
          bodyB = attachedActor->EnableDynamics( new DynamicsBodyConfig );
        }

        joint = new DynamicsJoint(world, bodyA, bodyB, offsetA, offsetB);
        mDynamicsData->joints[ attachedActor.Get() ] = joint;

        if( OnStage() && attachedActor->OnStage() )
        {
          joint->Connect(*mStage);
        }

        attachedActor->ReferenceJoint( joint );

        attachedActor->OnStageSignal().Connect( mDynamicsData->slotDelegate, &Actor::AttachedActorOnStage );
        attachedActor->OffStageSignal().Connect( mDynamicsData->slotDelegate, &Actor::AttachedActorOffStage );
      }
    }
  }
  return joint;
}

const int Actor::GetNumberOfJoints() const
{
  return static_cast<int>( NULL != mDynamicsData ? mDynamicsData->joints.size() : 0 );
}

DynamicsJointPtr Actor::GetDynamicsJointByIndex( const int index ) const
{
  DynamicsJointPtr joint;

  if( NULL != mDynamicsData )
  {
    if( index >= 0 && index < static_cast<int>(mDynamicsData->joints.size()) )
    {
      DynamicsData::JointContainer::const_iterator it( mDynamicsData->joints.begin() );

      for( int i = 0; i < index; ++i  )
      {
        ++it;
      }

      joint = it->second;
    }
  }

  return joint;
}

DynamicsJointPtr Actor::GetDynamicsJoint( ActorPtr attachedActor ) const
{
  DynamicsJointPtr joint;

  if( NULL != mDynamicsData )
  {
    DynamicsData::JointContainer::const_iterator it( mDynamicsData->joints.find( attachedActor.Get() ) );

    if( mDynamicsData->joints.end() != it )
    {
      // use existing joint
      joint = it->second;
    }
  }

  return joint;
}

void Actor::RemoveDynamicsJoint( DynamicsJointPtr joint )
{
  if( NULL != mDynamicsData )
  {
    DynamicsData::JointContainer::iterator it( mDynamicsData->joints.begin() );
    DynamicsData::JointContainer::iterator endIt( mDynamicsData->joints.end() );

    for( ; it != endIt; ++it )
    {
      if( it->second == joint.Get() )
      {
        ActorPtr attachedActor( it->first );

        if( OnStage() && attachedActor && attachedActor->OnStage() )
        {
          joint->Disconnect(*mStage);
        }

        if( attachedActor )
        {
          attachedActor->ReleaseJoint( joint );
          attachedActor->OnStageSignal().Disconnect( mDynamicsData->slotDelegate, &Actor::AttachedActorOnStage );
          attachedActor->OffStageSignal().Disconnect( mDynamicsData->slotDelegate, &Actor::AttachedActorOffStage );
        }

        mDynamicsData->joints.erase(it);
        break;
      }
    }
  }
}

void Actor::ReferenceJoint( DynamicsJointPtr joint )
{
  DALI_ASSERT_DEBUG( NULL != mDynamicsData && "Dynamics not enabled on this actor!" );

  if( NULL != mDynamicsData )
  {
    mDynamicsData->referencedJoints.push_back(joint);
  }
}

void Actor::ReleaseJoint( DynamicsJointPtr joint )
{
  DALI_ASSERT_DEBUG( NULL != mDynamicsData && "Dynamics not enabled on this actor!" );

  if( NULL != mDynamicsData )
  {
    DynamicsData::ReferencedJointContainer::iterator it( std::find( mDynamicsData->referencedJoints.begin(), mDynamicsData->referencedJoints.end(), joint ) );

    if( it != mDynamicsData->referencedJoints.end() )
    {
      mDynamicsData->referencedJoints.erase( it );
    }
  }
}

void Actor::SetDynamicsRoot(bool flag)
{
  if( mIsDynamicsRoot != flag )
  {
    mIsDynamicsRoot = flag;

    if( OnStage() && mChildren )
    {
      // walk the children connecting or disconnecting any dynamics enabled child from the dynamics simulation
      ActorIter end = mChildren->end();
      for( ActorIter iter = mChildren->begin(); iter != end; ++iter )
      {
        Actor& child = GetImplementation(*iter);

        if( child.GetDynamicsBody() )
        {
          if( mIsDynamicsRoot )
          {
            child.ConnectDynamics();
          }
          else
          {
            child.DisconnectDynamics();
          }
        }
      }
    }
  }
}

bool Actor::IsDynamicsRoot() const
{
  return mIsDynamicsRoot;
}

void Actor::AttachedActorOnStage( Dali::Actor actor )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  if( OnStage() )
  {
    ActorPtr attachedActor( &GetImplementation(actor) );

    DALI_ASSERT_DEBUG( NULL != mDynamicsData && "Dynamics not enabled on this actor!" );
    if( NULL != mDynamicsData )
    {
      DynamicsData::JointContainer::iterator it( mDynamicsData->joints.find(  attachedActor.Get() ) );
      if( mDynamicsData->joints.end() != it )
      {
        DynamicsJointPtr joint( it->second );
        joint->Connect(*mStage);
      }
    }
  }
}

void Actor::AttachedActorOffStage( Dali::Actor actor )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  if( OnStage() )
  {
    ActorPtr attachedActor( &GetImplementation(actor) );

    DALI_ASSERT_DEBUG( NULL != mDynamicsData && "Dynamics not enabled on this actor!" );
    if( NULL != mDynamicsData )
    {
      DynamicsData::JointContainer::iterator it( mDynamicsData->joints.find(  attachedActor.Get() ) );
      if( mDynamicsData->joints.end() != it )
      {
        DynamicsJointPtr joint( it->second );
        joint->Disconnect(*mStage);
      }
    }
  }
}

void Actor::ConnectDynamics()
{
  if( NULL != mDynamicsData && mDynamicsData->body )
  {
    if( OnStage() && mParent && mParent->IsDynamicsRoot() )
    {
      mDynamicsData->body->Connect(*mStage);

      // Connect all joints where attachedActor is also on stage
      if( !mDynamicsData->joints.empty() )
      {
        DynamicsData::JointContainer::iterator it( mDynamicsData->joints.begin() );
        DynamicsData::JointContainer::iterator endIt( mDynamicsData->joints.end() );

        for( ; it != endIt; ++it )
        {
          Actor* attachedActor( it->first );
          if( NULL != attachedActor && attachedActor->OnStage() )
          {
            DynamicsJointPtr joint( it->second );

            joint->Connect(*mStage);
          }
        }
      }
    }
  }
}

void Actor::DisconnectDynamics()
{
  if( NULL != mDynamicsData && mDynamicsData->body )
  {
    if( OnStage() )
    {
      mDynamicsData->body->Disconnect(*mStage);

      // Disconnect all joints
      if( !mDynamicsData->joints.empty() )
      {
        DynamicsData::JointContainer::iterator it( mDynamicsData->joints.begin() );
        DynamicsData::JointContainer::iterator endIt( mDynamicsData->joints.end() );

        for( ; it != endIt; ++it )
        {
          DynamicsJointPtr joint( it->second );

          joint->Disconnect(*mStage);
        }
      }
    }
  }
}

#endif // DYNAMICS_SUPPORT

void Actor::SetOverlay(bool enable)
{
  // Setting STENCIL will override OVERLAY
  if( DrawMode::STENCIL != mDrawMode )
  {
    SetDrawMode( enable ? DrawMode::OVERLAY : DrawMode::NORMAL );
  }
}

bool Actor::IsOverlay() const
{
  return ( DrawMode::OVERLAY == mDrawMode );
}

void Actor::SetDrawMode( DrawMode::Type drawMode )
{
  // this flag is not animatable so keep the value
  mDrawMode = drawMode;
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetDrawModeMessage( mStage->GetUpdateInterface(), *mNode, drawMode );
  }
}

DrawMode::Type Actor::GetDrawMode() const
{
  return mDrawMode;
}

bool Actor::ScreenToLocal( float& localX,
                           float& localY,
                           float screenX,
                           float screenY ) const
{
  // only valid when on-stage
  if ( OnStage() )
  {
    const RenderTaskList& taskList = mStage->GetRenderTaskList();

    Vector2 converted( screenX, screenY );

    // do a reverse traversal of all lists (as the default onscreen one is typically the last one)
    const int taskCount = taskList.GetTaskCount();
    for( int i = taskCount - 1; i >= 0; --i )
    {
      Dali::RenderTask task = taskList.GetTask( i );
      if( ScreenToLocal( Dali::GetImplementation( task ), localX, localY, screenX, screenY ) )
      {
        // found a task where this conversion was ok so return
        return true;
      }
    }
  }
  return false;
}

bool Actor::ScreenToLocal( RenderTask& renderTask,
                           float& localX,
                           float& localY,
                           float screenX,
                           float screenY ) const
{
  bool retval = false;
  // only valid when on-stage
  if ( OnStage() )
  {
    CameraActor* camera = renderTask.GetCameraActor();
    if( camera )
    {
      Viewport viewport;
      renderTask.GetViewport( viewport );

      // need to translate coordinates to render tasks coordinate space
      Vector2 converted( screenX, screenY );
      if( renderTask.TranslateCoordinates( converted ) )
      {
        retval = ScreenToLocal( camera->GetViewMatrix(), camera->GetProjectionMatrix(), viewport, localX, localY, converted.x, converted.y );
      }
    }
  }
  return retval;
}

bool Actor::ScreenToLocal( const Matrix& viewMatrix,
                           const Matrix& projectionMatrix,
                           const Viewport& viewport,
                           float& localX,
                           float& localY,
                           float screenX,
                           float screenY ) const
{
  // Early-out if mNode is NULL
  if( !OnStage() )
  {
    return false;
  }

  BufferIndex bufferIndex( mStage->GetEventBufferIndex() );

  // Calculate the ModelView matrix
  Matrix modelView(false/*don't init*/);
  // need to use the components as world matrix is only updated for actors that need it
  modelView.SetTransformComponents( mNode->GetWorldScale(bufferIndex), mNode->GetWorldRotation(bufferIndex), mNode->GetWorldPosition(bufferIndex) );
  Matrix::Multiply(modelView, modelView, viewMatrix);

  // Calculate the inverted ModelViewProjection matrix; this will be used for 2 unprojects
  Matrix invertedMvp(false/*don't init*/);
  Matrix::Multiply(invertedMvp, modelView, projectionMatrix);
  bool success = invertedMvp.Invert();

  // Convert to GL coordinates
  Vector4 screenPos( screenX - viewport.x, viewport.height - (screenY - viewport.y), 0.f, 1.f );

  Vector4 nearPos;
  if (success)
  {
    success = Unproject(screenPos, invertedMvp, viewport.width, viewport.height, nearPos);
  }

  Vector4 farPos;
  if (success)
  {
    screenPos.z = 1.0f;
    success = Unproject(screenPos, invertedMvp, viewport.width, viewport.height, farPos);
  }

  if (success)
  {
    Vector4 local;
    if (XyPlaneIntersect(nearPos, farPos, local))
    {
      Vector3 size = GetCurrentSize();
      localX = local.x + size.x * 0.5f;
      localY = local.y + size.y * 0.5f;
    }
    else
    {
      success = false;
    }
  }

  return success;
}

bool Actor::RaySphereTest( const Vector4& rayOrigin, const Vector4& rayDir ) const
{
  /*
    http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection

    Mathematical Formulation

    Given the above mentioned sphere, a point 'p' lies on the surface of the sphere if

    ( p - c ) dot ( p - c ) = r^2

    Given a ray with a point of origin 'o', and a direction vector 'd':

    ray(t) = o + td, t >= 0

    we can find the t at which the ray intersects the sphere by setting ray(t) equal to 'p'

    (o + td - c ) dot ( o + td - c ) = r^2

    To solve for t we first expand the above into a more recognisable quadratic equation form

    ( d dot d )t^2 + 2( o - c ) dot dt + ( o - c ) dot ( o - c ) - r^2 = 0

    or

    At2 + Bt + C = 0

    where

    A = d dot d
    B = 2( o - c ) dot d
    C = ( o - c ) dot ( o - c ) - r^2

    which can be solved using a standard quadratic formula.

    Note that in the absence of positive, real, roots, the ray does not intersect the sphere.

    Practical Simplification

    In a renderer, we often differentiate between world space and object space. In the object space
    of a sphere it is centred at origin, meaning that if we first transform the ray from world space
    into object space, the mathematical solution presented above can be simplified significantly.

    If a sphere is centred at origin, a point 'p' lies on a sphere of radius r2 if

    p dot p = r^2

    and we can find the t at which the (transformed) ray intersects the sphere by

    ( o + td ) dot ( o + td ) = r^2

    According to the reasoning above, we expand the above quadratic equation into the general form

    At2 + Bt + C = 0

    which now has coefficients:

    A = d dot d
    B = 2( d dot o )
    C = o dot o - r^2
   */

  // Early out if mNode is NULL
  if( !mNode )
  {
    return false;
  }

  BufferIndex bufferIndex( mStage->GetEventBufferIndex() );

  // Transforms the ray to the local reference system. As the test is against a sphere, only the translation and scale are needed.
  const Vector3& translation( mNode->GetWorldPosition( bufferIndex ) );
  Vector3 rayOriginLocal(rayOrigin.x - translation.x,
                         rayOrigin.y - translation.y,
                         rayOrigin.z - translation.z);

  // Compute the radius is not needed, square radius it's enough.
  const Vector3& size( mNode->GetSize( bufferIndex ) );

  // Scale the sphere.
  const Vector3& scale( mNode->GetWorldScale( bufferIndex ) );

  const float width = size.width * scale.width;
  const float height = size.height * scale.height;

  float squareSphereRadius = 0.5f * ( width * width + height * height );

  float a = rayDir.Dot( rayDir );                                       // a
  float b2 = rayDir.Dot( rayOriginLocal );                              // b/2
  float c = rayOriginLocal.Dot( rayOriginLocal ) - squareSphereRadius;  // c

  return ( b2*b2 - a*c ) >= 0.f;
}

bool Actor::RayActorTest( const Vector4& rayOrigin, const Vector4& rayDir, Vector4& hitPointLocal, float& distance ) const
{
  bool hit = false;

  if( OnStage() &&
      NULL != mNode )
  {
    BufferIndex bufferIndex( mStage->GetEventBufferIndex() );

    // Transforms the ray to the local reference system.

    // Calculate the inverse of Model matrix
    Matrix invModelMatrix(false/*don't init*/);
    // need to use the components as world matrix is only updated for actors that need it
    invModelMatrix.SetInverseTransformComponents( mNode->GetWorldScale(bufferIndex), mNode->GetWorldRotation(bufferIndex), mNode->GetWorldPosition(bufferIndex) );

    Vector4 rayOriginLocal(invModelMatrix * rayOrigin);
    Vector4 rayDirLocal(invModelMatrix * rayDir - invModelMatrix.GetTranslation());

    // Test with the actor's XY plane (Normal = 0 0 1 1).

    float a = -rayOriginLocal.z;
    float b = rayDirLocal.z;

    if( fabsf( b ) > Math::MACHINE_EPSILON_1 )
    {
      // Ray travels distance * rayDirLocal to intersect with plane.
      distance = a / b;

      const Vector3& size = mNode->GetSize( bufferIndex );

      hitPointLocal.x = rayOriginLocal.x + rayDirLocal.x * distance + size.x * 0.5f;
      hitPointLocal.y = rayOriginLocal.y + rayDirLocal.y * distance + size.y * 0.5f;

      // Test with the actor's geometry.
      hit = ( hitPointLocal.x >= 0.f ) && ( hitPointLocal.x <= size.x ) && ( hitPointLocal.y >= 0.f ) && ( hitPointLocal.y <= size.y );
    }
  }

  return hit;
}

void Actor::SetLeaveRequired(bool required)
{
  mLeaveRequired = required;
}

bool Actor::GetLeaveRequired() const
{
  return mLeaveRequired;
}

void Actor::SetKeyboardFocusable( bool focusable )
{
  mKeyboardFocusable = focusable;
}

bool Actor::IsKeyboardFocusable() const
{
  return mKeyboardFocusable;
}

bool Actor::GetTouchRequired() const
{
  return !mTouchedSignalV2.Empty() || mDerivedRequiresTouch;
}

bool Actor::GetMouseWheelEventRequired() const
{
  return !mMouseWheelEventSignalV2.Empty() || mDerivedRequiresMouseWheelEvent;
}

bool Actor::IsHittable() const
{
  return IsSensitive() &&
         IsVisible() &&
         ( GetCurrentWorldColor().a > FULLY_TRANSPARENT ) &&
         IsNodeConnected();
}

ActorGestureData& Actor::GetGestureData()
{
  // Likely scenario is that once gesture-data is created for this actor, the actor will require
  // that gesture for its entire life-time so no need to destroy it until the actor is destroyed
  if ( NULL == mGestureData )
  {
    mGestureData = new ActorGestureData;
  }
  return *mGestureData;
}

bool Actor::IsGestureRequred( Gesture::Type type ) const
{
  return mGestureData && mGestureData->IsGestureRequred( type );
}

bool Actor::EmitTouchEventSignal(const TouchEvent& event)
{
  bool consumed = false;

  if ( !mTouchedSignalV2.Empty() )
  {
    Dali::Actor handle( this );
    consumed = mTouchedSignalV2.Emit( handle, event );
  }

  if (!consumed)
  {
    // Notification for derived classes
    consumed = OnTouchEvent( event );
  }

  return consumed;
}

bool Actor::EmitMouseWheelEventSignal(const MouseWheelEvent& event)
{
  bool consumed = false;

  if ( !mMouseWheelEventSignalV2.Empty() )
  {
    Dali::Actor handle( this );
    consumed = mMouseWheelEventSignalV2.Emit( handle, event );
  }

  if (!consumed)
  {
    // Notification for derived classes
    consumed = OnMouseWheelEvent(event);
  }

  return consumed;
}

Dali::Actor::TouchSignalV2& Actor::TouchedSignal()
{
  return mTouchedSignalV2;
}

Dali::Actor::MouseWheelEventSignalV2& Actor::MouseWheelEventSignal()
{
  return mMouseWheelEventSignalV2;
}

Dali::Actor::SetSizeSignalV2& Actor::SetSizeSignal()
{
  return mSetSizeSignalV2;
}

Dali::Actor::OnStageSignalV2& Actor::OnStageSignal()
{
  return mOnStageSignalV2;
}

Dali::Actor::OffStageSignalV2& Actor::OffStageSignal()
{
  return mOffStageSignalV2;
}

bool Actor::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  Actor* actor = dynamic_cast<Actor*>(object);

  if(Dali::Actor::SIGNAL_TOUCHED == signalName)
  {
    actor->TouchedSignal().Connect( tracker, functor );
  }
  else if(Dali::Actor::SIGNAL_MOUSE_WHEEL_EVENT == signalName)
  {
    actor->MouseWheelEventSignal().Connect( tracker, functor );
  }
  else if(Dali::Actor::SIGNAL_SET_SIZE == signalName)
  {
    actor->SetSizeSignal().Connect( tracker, functor );
  }
  else if(Dali::Actor::SIGNAL_ON_STAGE == signalName)
  {
    actor->OnStageSignal().Connect( tracker, functor );
  }
  else if(Dali::Actor::SIGNAL_OFF_STAGE == signalName)
  {
    actor->OffStageSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

Actor::Actor( DerivedType derivedType )
: mStage( NULL ),
  mParent( NULL ),
  mChildren( NULL ),
  mNode( NULL ),
  mParentOrigin( NULL ),
  mAnchorPoint( NULL ),
#ifdef DYNAMICS_SUPPORT
  mDynamicsData( NULL ),
#endif
  mGestureData( NULL ),
  mAttachment(),
  mName(),
  mId( ++mActorCounter ), // actor ID is initialised to start from 1, and 0 is reserved
  mIsRoot( ROOT_LAYER == derivedType ),
  mIsRenderable( RENDERABLE == derivedType ),
  mIsLayer( LAYER == derivedType || ROOT_LAYER == derivedType ),
  mIsOnStage( false ),
  mIsDynamicsRoot(false),
  mSensitive( true ),
  mLeaveRequired( false ),
  mKeyboardFocusable( false ),
  mDerivedRequiresTouch( false ),
  mDerivedRequiresMouseWheelEvent( false ),
  mOnStageSignalled( false ),
  mInheritRotation( true ),
  mInheritScale( true ),
  mDrawMode( DrawMode::NORMAL ),
  mPositionInheritanceMode( Node::DEFAULT_POSITION_INHERITANCE_MODE ),
  mColorMode( Node::DEFAULT_COLOR_MODE )
{
}

void Actor::Initialize()
{
  mStage = Stage::GetCurrent();

  // Node creation
  SceneGraph::Node* node = CreateNode();

  AddNodeMessage( mStage->GetUpdateManager(), *node ); // Pass ownership to scene-graph
  mNode = node; // Keep raw-pointer to Node

  if(!mDefaultPropertyLookup)
  {
    mDefaultPropertyLookup = new DefaultPropertyLookup();

    for (int i=0; i<DEFAULT_PROPERTY_COUNT; ++i)
    {
      (*mDefaultPropertyLookup)[DEFAULT_PROPERTY_DETAILS[i].name] = i;
    }
  }

  OnInitialize();

  RegisterObject();
}

Actor::~Actor()
{
  // Remove mParent pointers from children even if we're destroying core,
  // to guard against GetParent() & Unparent() calls from CustomActor destructors.
  if( mChildren )
  {
    ActorConstIter endIter = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != endIter; ++iter )
    {
      Actor& actor = GetImplementation( *iter );
      actor.SetParent( NULL );
    }
  }
  delete mChildren;

  // Guard to allow handle destruction after Core has been destroyed
  if( Stage::IsInstalled() )
  {
    if( NULL != mNode )
    {
      DestroyNodeMessage( mStage->GetUpdateManager(), *mNode );
      mNode = NULL; // Node is about to be destroyed
    }

    UnregisterObject();
  }

#ifdef DYNAMICS_SUPPORT
  // Cleanup dynamics
  delete mDynamicsData;
#endif

  // Cleanup optional gesture data
  delete mGestureData;

  // Cleanup optional parent origin and anchor
  delete mParentOrigin;
  delete mAnchorPoint;
}

void Actor::ConnectToStage( Stage& stage )
{
  // This container is used instead of walking the Actor hierachy.
  // It protects us when the Actor hierachy is modified during OnStageConnectionExternal callbacks.
  ActorContainer connectionList;

  // This stage is atomic i.e. not interrupted by user callbacks
  RecursiveConnectToStage( stage, connectionList );

  // Notify applications about the newly connected actors.
  const ActorIter endIter = connectionList.end();
  for( ActorIter iter = connectionList.begin(); iter != endIter; ++iter )
  {
    Actor& actor = GetImplementation(*iter);
    actor.NotifyStageConnection();
  }
}

void Actor::RecursiveConnectToStage( Stage& stage, ActorContainer& connectionList )
{
  DALI_ASSERT_ALWAYS( !OnStage() );

  mIsOnStage = true;

  ConnectToSceneGraph();

  // Notification for internal derived classes
  OnStageConnectionInternal();

  // This stage is atomic; avoid emitting callbacks until all Actors are connected
  connectionList.push_back( Dali::Actor(this) );

  // Recursively connect children
  if( mChildren )
  {
    ActorConstIter endIter = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != endIter; ++iter )
    {
      Actor& actor = GetImplementation( *iter );
      actor.RecursiveConnectToStage( stage, connectionList );
    }
  }
}

/**
 * This method is called when the Actor is connected to the Stage.
 * The parent must have added its Node to the scene-graph.
 * The child must connect its Node to the parent's Node.
 * This is resursive; the child calls ConnectToStage() for its children.
 */
void Actor::ConnectToSceneGraph()
{
  DALI_ASSERT_DEBUG( mNode != NULL);
  DALI_ASSERT_DEBUG( mParent != NULL);
  DALI_ASSERT_DEBUG( mParent->mNode != NULL );

  if( NULL != mNode )
  {
    // Reparent Node in next Update
    ConnectNodeMessage( mStage->GetUpdateManager(), *(mParent->mNode), *mNode );
  }

  // Notify attachment
  if (mAttachment)
  {
    mAttachment->Connect();
  }

#ifdef DYNAMICS_SUPPORT
  // Notify dynamics
  if( NULL != mDynamicsData )
  {
    ConnectDynamics();
  }
#endif

  // Notification for ProxyObject::Observers
  OnSceneObjectAdd();
}

void Actor::NotifyStageConnection()
{
  // Actors can be removed (in a callback), before the on-stage stage is reported.
  // The actor may also have been reparented, in which case mOnStageSignalled will be true.
  if ( OnStage() && !mOnStageSignalled )
  {
    // Notification for external (CustomActor) derived classes
    OnStageConnectionExternal();

    if ( !mOnStageSignalV2.Empty() )
    {
      Dali::Actor handle( this );
      mOnStageSignalV2.Emit( handle );
    }

    // Guard against Remove during callbacks
    if ( OnStage()  )
    {
      mOnStageSignalled = true; // signal required next time Actor is removed
    }
  }
}

void Actor::DisconnectFromStage()
{
  // This container is used instead of walking the Actor hierachy.
  // It protects us when the Actor hierachy is modified during OnStageDisconnectionExternal callbacks.
  ActorContainer disconnectionList;

  // This stage is atomic i.e. not interrupted by user callbacks
  RecursiveDisconnectFromStage( disconnectionList );

  // Notify applications about the newly disconnected actors.
  const ActorIter endIter = disconnectionList.end();
  for( ActorIter iter = disconnectionList.begin(); iter != endIter; ++iter )
  {
    Actor& actor = GetImplementation(*iter);
    actor.NotifyStageDisconnection();
  }
}

void Actor::RecursiveDisconnectFromStage( ActorContainer& disconnectionList )
{
  DALI_ASSERT_ALWAYS( OnStage() );

  // Recursively disconnect children
  if( mChildren )
  {
    ActorConstIter endIter = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != endIter; ++iter )
    {
      Actor& actor = GetImplementation( *iter );
      actor.RecursiveDisconnectFromStage( disconnectionList );
    }
  }

  // This stage is atomic; avoid emitting callbacks until all Actors are disconnected
  disconnectionList.push_back( Dali::Actor(this) );

  // Notification for internal derived classes
  OnStageDisconnectionInternal();

  DisconnectFromSceneGraph();

  mIsOnStage = false;
}

/**
 * This method is called by an actor or its parent, before a node removal message is sent.
 * This is recursive; the child calls DisconnectFromStage() for its children.
 */
void Actor::DisconnectFromSceneGraph()
{
  // Notification for ProxyObject::Observers
  OnSceneObjectRemove();

  // Notify attachment
  if (mAttachment)
  {
    mAttachment->Disconnect();
  }

#ifdef DYNAMICS_SUPPORT
  // Notify dynamics
  if( NULL != mDynamicsData )
  {
    DisconnectDynamics();
  }
#endif
}

void Actor::NotifyStageDisconnection()
{
  // Actors can be added (in a callback), before the off-stage state is reported.
  // Also if the actor was added & removed before mOnStageSignalled was set, then we don't notify here.
  // only do this step if there is a stage, i.e. Core is not being shut down
  if ( Stage::IsInstalled() && !OnStage() && mOnStageSignalled )
  {
    // Notification for external (CustomeActor) derived classes
    OnStageDisconnectionExternal();

    if( !mOffStageSignalV2.Empty() )
    {
      Dali::Actor handle( this );
      mOffStageSignalV2.Emit( handle );
    }

    // Guard against Add during callbacks
    if ( !OnStage()  )
    {
      mOnStageSignalled = false; // signal required next time Actor is added
    }
  }
}

bool Actor::IsNodeConnected() const
{
  bool connected( false );

  if( OnStage() &&
      NULL != mNode )
  {
    if( mNode->IsRoot() || mNode->GetParent() )
    {
      connected = true;
    }
  }

  return connected;
}

bool Actor::IsSceneObjectRemovable() const
{
  return false;
}

unsigned int Actor::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void Actor::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const std::string& Actor::GetDefaultPropertyName( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }
  else
  {
    // index out of range..return empty string
    return String::EMPTY;
  }
}

Property::Index Actor::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  DALI_ASSERT_DEBUG( NULL != mDefaultPropertyLookup );

  // Look for name in default properties
  DefaultPropertyLookup::const_iterator result = mDefaultPropertyLookup->find( name );
  if ( mDefaultPropertyLookup->end() != result )
  {
    index = result->second;
  }

  return index;
}

bool Actor::IsDefaultPropertyWritable(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].writable;
  }
  else
  {
    return false;
  }
}

bool Actor::IsDefaultPropertyAnimatable(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].animatable;
  }
  else
  {
    return false;
  }
}

bool Actor::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].constraintInput;
  }
  else
  {
    return false;
  }
}

Property::Type Actor::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }
  else
  {
    // index out of range...return Property::NONE
    return Property::NONE;
  }
}

void Actor::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  switch ( index )
  {
    case Dali::Actor::PARENT_ORIGIN:
    {
      SetParentOrigin( property.Get<Vector3>() );
      break;
    }

    case Dali::Actor::PARENT_ORIGIN_X:
    {
      SetParentOriginX( property.Get<float>() );
      break;
    }

    case Dali::Actor::PARENT_ORIGIN_Y:
    {
      SetParentOriginY( property.Get<float>() );
      break;
    }

    case Dali::Actor::PARENT_ORIGIN_Z:
    {
      SetParentOriginZ( property.Get<float>() );
      break;
    }

    case Dali::Actor::ANCHOR_POINT:
    {
      SetAnchorPoint( property.Get<Vector3>() );
      break;
    }

    case Dali::Actor::ANCHOR_POINT_X:
    {
      SetAnchorPointX( property.Get<float>() );
      break;
    }

    case Dali::Actor::ANCHOR_POINT_Y:
    {
      SetAnchorPointY( property.Get<float>() );
      break;
    }

    case Dali::Actor::ANCHOR_POINT_Z:
    {
      SetAnchorPointZ( property.Get<float>() );
      break;
    }

    case Dali::Actor::SIZE:
    {
      SetSize( property.Get<Vector3>() );
      break;
    }

    case Dali::Actor::SIZE_WIDTH:
    {
      SetWidth( property.Get<float>() );
      break;
    }

    case Dali::Actor::SIZE_HEIGHT:
    {
      SetHeight( property.Get<float>() );
      break;
    }

    case Dali::Actor::SIZE_DEPTH:
    {
      SetDepth( property.Get<float>() );
      break;
    }

    case Dali::Actor::POSITION:
    {
      SetPosition( property.Get<Vector3>() );
      break;
    }

    case Dali::Actor::POSITION_X:
    {
      SetX( property.Get<float>() );
      break;
    }

    case Dali::Actor::POSITION_Y:
    {
      SetY( property.Get<float>() );
      break;
    }

    case Dali::Actor::POSITION_Z:
    {
      SetZ( property.Get<float>() );
      break;
    }

    case Dali::Actor::ROTATION:
    {
      SetRotation( property.Get<Quaternion>() );
      break;
    }

    case Dali::Actor::SCALE:
    {
      SetScale( property.Get<Vector3>() );
      break;
    }

    case Dali::Actor::SCALE_X:
    {
      SetScaleX( property.Get<float>() );
      break;
    }

    case Dali::Actor::SCALE_Y:
    {
      SetScaleY( property.Get<float>() );
      break;
    }

    case Dali::Actor::SCALE_Z:
    {
      SetScaleZ( property.Get<float>() );
      break;
    }

    case Dali::Actor::VISIBLE:
    {
      SetVisible( property.Get<bool>() );
      break;
    }

    case Dali::Actor::COLOR:
    {
      SetColor( property.Get<Vector4>() );
      break;
    }

    case Dali::Actor::COLOR_RED:
    {
      SetColorRed( property.Get<float>() );
      break;
    }

    case Dali::Actor::COLOR_GREEN:
    {
      SetColorGreen( property.Get<float>() );
      break;
    }

    case Dali::Actor::COLOR_BLUE:
    {
      SetColorBlue( property.Get<float>() );
      break;
    }

    case Dali::Actor::COLOR_ALPHA:
    {
      SetOpacity( property.Get<float>() );
      break;
    }

    case Dali::Actor::NAME:
    {
      SetName( property.Get<std::string>() );
      break;
    }

    case Dali::Actor::SENSITIVE:
    {
      SetSensitive( property.Get<bool>() );
      break;
    }

    case Dali::Actor::LEAVE_REQUIRED:
    {
      SetLeaveRequired( property.Get<bool>() );
      break;
    }

    case Dali::Actor::INHERIT_ROTATION:
    {
      SetInheritRotation( property.Get<bool>() );
      break;
    }

    case Dali::Actor::INHERIT_SCALE:
    {
      SetInheritScale( property.Get<bool>() );
      break;
    }

    case Dali::Actor::COLOR_MODE:
    {
      SetColorMode( Scripting::GetColorMode( property.Get<std::string>() ) );
      break;
    }

    case Dali::Actor::POSITION_INHERITANCE:
    {
      SetPositionInheritanceMode( Scripting::GetPositionInheritanceMode( property.Get<std::string>() ) );
      break;
    }

    case Dali::Actor::DRAW_MODE:
    {
      SetDrawMode( Scripting::GetDrawMode( property.Get<std::string>() ) );
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS(false && "Actor::Property is out of bounds"); // should not come here
      break;
    }
  }
}

void Actor::SetCustomProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value )
{
  // TODO: This should be deprecated
  OnPropertySet(index, value);

  if(entry.IsAnimatable())
  {
    // TODO: ADD MATRIX & MATRIX3 types

    switch ( entry.type )
    {
      case Property::BOOLEAN:
      {
        AnimatableProperty<bool>* property = dynamic_cast< AnimatableProperty<bool>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::NodePropertyMessage<bool>::Send( mStage->GetUpdateManager(), mNode, property, &AnimatableProperty<bool>::Bake, value.Get<bool>() );

        break;
      }

      case Property::FLOAT:
      {
        AnimatableProperty<float>* property = dynamic_cast< AnimatableProperty<float>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::NodePropertyMessage<float>::Send( mStage->GetUpdateManager(), mNode, property, &AnimatableProperty<float>::Bake, value.Get<float>() );

        break;
      }

      case Property::INTEGER:
      {
        AnimatableProperty<int>* property = dynamic_cast< AnimatableProperty<int>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::NodePropertyMessage<int>::Send( mStage->GetUpdateManager(), mNode, property, &AnimatableProperty<int>::Bake, value.Get<int>() );

        break;
      }

      case Property::VECTOR2:
      {
        AnimatableProperty<Vector2>* property = dynamic_cast< AnimatableProperty<Vector2>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::NodePropertyMessage<Vector2>::Send( mStage->GetUpdateManager(), mNode, property, &AnimatableProperty<Vector2>::Bake, value.Get<Vector2>() );

        break;
      }

      case Property::VECTOR3:
      {
        AnimatableProperty<Vector3>* property = dynamic_cast< AnimatableProperty<Vector3>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::NodePropertyMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, property, &AnimatableProperty<Vector3>::Bake, value.Get<Vector3>() );

        break;
      }

      case Property::VECTOR4:
      {
        AnimatableProperty<Vector4>* property = dynamic_cast< AnimatableProperty<Vector4>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::NodePropertyMessage<Vector4>::Send( mStage->GetUpdateManager(), mNode, property, &AnimatableProperty<Vector4>::Bake, value.Get<Vector4>() );

        break;
      }

      case Property::ROTATION:
      {
        AnimatableProperty<Quaternion>* property = dynamic_cast< AnimatableProperty<Quaternion>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::NodePropertyMessage<Quaternion>::Send( mStage->GetUpdateManager(), mNode, property,&AnimatableProperty<Quaternion>::Bake,  value.Get<Quaternion>() );

        break;
      }

      default:
      {
        DALI_ASSERT_ALWAYS( false && "Property type enumeration out of bounds" ); // should not come here
        break;
      }
    }
  }
}

Property::Value Actor::GetDefaultProperty(Property::Index index) const
{
  Property::Value value;

  switch ( index )
  {
    case Dali::Actor::PARENT_ORIGIN:
    {
      value = GetCurrentParentOrigin();
      break;
    }

    case Dali::Actor::PARENT_ORIGIN_X:
    {
      value = GetCurrentParentOrigin().x;
      break;
    }

    case Dali::Actor::PARENT_ORIGIN_Y:
    {
      value = GetCurrentParentOrigin().y;
      break;
    }

    case Dali::Actor::PARENT_ORIGIN_Z:
    {
      value = GetCurrentParentOrigin().z;
      break;
    }

    case Dali::Actor::ANCHOR_POINT:
    {
      value = GetCurrentAnchorPoint();
      break;
    }

    case Dali::Actor::ANCHOR_POINT_X:
    {
      value = GetCurrentAnchorPoint().x;
      break;
    }

    case Dali::Actor::ANCHOR_POINT_Y:
    {
      value = GetCurrentAnchorPoint().y;
      break;
    }

    case Dali::Actor::ANCHOR_POINT_Z:
    {
      value = GetCurrentAnchorPoint().z;
      break;
    }

    case Dali::Actor::SIZE:
    {
      value = GetCurrentSize();
      break;
    }

    case Dali::Actor::SIZE_WIDTH:
    {
      value = GetCurrentSize().width;
      break;
    }

    case Dali::Actor::SIZE_HEIGHT:
    {
      value = GetCurrentSize().height;
      break;
    }

    case Dali::Actor::SIZE_DEPTH:
    {
      value = GetCurrentSize().depth;
      break;
    }

    case Dali::Actor::POSITION:
    {
      value = GetCurrentPosition();
      break;
    }

    case Dali::Actor::POSITION_X:
    {
      value = GetCurrentPosition().x;
      break;
    }

    case Dali::Actor::POSITION_Y:
    {
      value = GetCurrentPosition().y;
      break;
    }

    case Dali::Actor::POSITION_Z:
    {
      value = GetCurrentPosition().z;
      break;
    }

    case Dali::Actor::WORLD_POSITION:
    {
      value = GetCurrentWorldPosition();
      break;
    }

    case Dali::Actor::WORLD_POSITION_X:
    {
      value = GetCurrentWorldPosition().x;
      break;
    }

    case Dali::Actor::WORLD_POSITION_Y:
    {
      value = GetCurrentWorldPosition().y;
      break;
    }

    case Dali::Actor::WORLD_POSITION_Z:
    {
      value = GetCurrentWorldPosition().z;
      break;
    }

    case Dali::Actor::ROTATION:
    {
      value = GetCurrentRotation();
      break;
    }

    case Dali::Actor::WORLD_ROTATION:
    {
      value = GetCurrentWorldRotation();
      break;
    }

    case Dali::Actor::SCALE:
    {
      value = GetCurrentScale();
      break;
    }

    case Dali::Actor::SCALE_X:
    {
      value = GetCurrentScale().x;
      break;
    }

    case Dali::Actor::SCALE_Y:
    {
      value = GetCurrentScale().y;
      break;
    }

    case Dali::Actor::SCALE_Z:
    {
      value = GetCurrentScale().z;
      break;
    }

    case Dali::Actor::WORLD_SCALE:
    {
      value = GetCurrentWorldScale();
      break;
    }

    case Dali::Actor::VISIBLE:
    {
      value = IsVisible();
      break;
    }

    case Dali::Actor::COLOR:
    {
      value = GetCurrentColor();
      break;
    }

    case Dali::Actor::COLOR_RED:
    {
      value = GetCurrentColor().r;
      break;
    }

    case Dali::Actor::COLOR_GREEN:
    {
      value = GetCurrentColor().g;
      break;
    }

    case Dali::Actor::COLOR_BLUE:
    {
      value = GetCurrentColor().b;
      break;
    }

    case Dali::Actor::COLOR_ALPHA:
    {
      value = GetCurrentColor().a;
      break;
    }

    case Dali::Actor::WORLD_COLOR:
    {
      value = GetCurrentWorldColor();
      break;
    }

    case Dali::Actor::WORLD_MATRIX:
    {
      value = GetCurrentWorldMatrix();
      break;
    }

    case Dali::Actor::NAME:
    {
      value = GetName();
      break;
    }

    case Dali::Actor::SENSITIVE:
    {
      value = IsSensitive();
      break;
    }

    case Dali::Actor::LEAVE_REQUIRED:
    {
      value = GetLeaveRequired();
      break;
    }

    case Dali::Actor::INHERIT_ROTATION:
    {
      value = IsRotationInherited();
      break;
    }

    case Dali::Actor::INHERIT_SCALE:
    {
      value = IsScaleInherited();
      break;
    }

    case Dali::Actor::COLOR_MODE:
    {
      value = Scripting::GetColorMode( GetColorMode() );
      break;
    }

    case Dali::Actor::POSITION_INHERITANCE:
    {
      value = Scripting::GetPositionInheritanceMode( GetPositionInheritanceMode() );
      break;
    }

    case Dali::Actor::DRAW_MODE:
    {
      value = Scripting::GetDrawMode( GetDrawMode() );
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS(false && "Actor Property index invalid" ); // should not come here
      break;
    }
  }

  return value;
}

void Actor::InstallSceneObjectProperty( PropertyBase& newProperty, const std::string& name, unsigned int index )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to add the property
    InstallCustomPropertyMessage( mStage->GetUpdateInterface(), *mNode, newProperty ); // Message takes ownership
  }
}

const SceneGraph::PropertyOwner* Actor::GetSceneObject() const
{
  // This method should only return an object connected to the scene-graph
  return OnStage() ? mNode : NULL;
}

const PropertyBase* Actor::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable(index) && "Property is not animatable" );

  const PropertyBase* property( NULL );

  // This method should only return a property of an object connected to the scene-graph
  if ( !OnStage() )
  {
    return property;
  }

  if ( static_cast<unsigned int>(index) >= DEFAULT_PROPERTY_MAX_COUNT )
  {
    CustomPropertyLookup::const_iterator entry = GetCustomPropertyLookup().find( index );

    DALI_ASSERT_ALWAYS( GetCustomPropertyLookup().end() != entry && "index is invalid" );

    property = dynamic_cast<const PropertyBase*>( entry->second.GetSceneGraphProperty() );
  }
  else if( NULL != mNode )
  {
    switch ( index )
    {
      case Dali::Actor::SIZE:
        property = &mNode->mSize;
        break;

      case Dali::Actor::SIZE_WIDTH:
        property = &mNode->mSize;
        break;

      case Dali::Actor::SIZE_HEIGHT:
        property = &mNode->mSize;
        break;

      case Dali::Actor::SIZE_DEPTH:
        property = &mNode->mSize;
        break;

      case Dali::Actor::POSITION:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::POSITION_X:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::POSITION_Y:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::POSITION_Z:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::ROTATION:
        property = &mNode->mRotation;
        break;

      case Dali::Actor::SCALE:
        property = &mNode->mScale;
        break;

      case Dali::Actor::SCALE_X:
        property = &mNode->mScale;
        break;

      case Dali::Actor::SCALE_Y:
        property = &mNode->mScale;
        break;

      case Dali::Actor::SCALE_Z:
        property = &mNode->mScale;
        break;

      case Dali::Actor::VISIBLE:
        property = &mNode->mVisible;
        break;

      case Dali::Actor::COLOR:
        property = &mNode->mColor;
        break;

      case Dali::Actor::COLOR_RED:
        property = &mNode->mColor;
        break;

      case Dali::Actor::COLOR_GREEN:
        property = &mNode->mColor;
        break;

      case Dali::Actor::COLOR_BLUE:
        property = &mNode->mColor;
        break;

      case Dali::Actor::COLOR_ALPHA:
        property = &mNode->mColor;
        break;

      default:
        break;
    }
  }

  return property;
}

const PropertyInputImpl* Actor::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property( NULL );

  // This method should only return a property of an object connected to the scene-graph
  if ( !OnStage() )
  {
    return property;
  }

  if ( index >= DEFAULT_PROPERTY_MAX_COUNT )
  {
    CustomPropertyLookup::const_iterator entry = GetCustomPropertyLookup().find( index );

    DALI_ASSERT_ALWAYS( GetCustomPropertyLookup().end() != entry && "property index is invalid" );

    property = entry->second.GetSceneGraphProperty();
  }
  else if( NULL != mNode )
  {
    switch ( index )
    {
      case Dali::Actor::PARENT_ORIGIN:
        property = &mNode->mParentOrigin;
        break;

      case Dali::Actor::PARENT_ORIGIN_X:
        property = &mNode->mParentOrigin;
        break;

      case Dali::Actor::PARENT_ORIGIN_Y:
        property = &mNode->mParentOrigin;
        break;

      case Dali::Actor::PARENT_ORIGIN_Z:
        property = &mNode->mParentOrigin;
        break;

      case Dali::Actor::ANCHOR_POINT:
        property = &mNode->mAnchorPoint;
        break;

      case Dali::Actor::ANCHOR_POINT_X:
        property = &mNode->mAnchorPoint;
        break;

      case Dali::Actor::ANCHOR_POINT_Y:
        property = &mNode->mAnchorPoint;
        break;

      case Dali::Actor::ANCHOR_POINT_Z:
        property = &mNode->mAnchorPoint;
        break;

      case Dali::Actor::SIZE:
        property = &mNode->mSize;
        break;

      case Dali::Actor::SIZE_WIDTH:
        property = &mNode->mSize;
        break;

      case Dali::Actor::SIZE_HEIGHT:
        property = &mNode->mSize;
        break;

      case Dali::Actor::SIZE_DEPTH:
        property = &mNode->mSize;
        break;

      case Dali::Actor::POSITION:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::POSITION_X:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::POSITION_Y:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::POSITION_Z:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::WORLD_POSITION:
        property = &mNode->mWorldPosition;
        break;

      case Dali::Actor::WORLD_POSITION_X:
        property = &mNode->mWorldPosition;
        break;

      case Dali::Actor::WORLD_POSITION_Y:
        property = &mNode->mWorldPosition;
        break;

      case Dali::Actor::WORLD_POSITION_Z:
        property = &mNode->mWorldPosition;
        break;

      case Dali::Actor::ROTATION:
        property = &mNode->mRotation;
        break;

      case Dali::Actor::WORLD_ROTATION:
        property = &mNode->mWorldRotation;
        break;

      case Dali::Actor::SCALE:
        property = &mNode->mScale;
        break;

      case Dali::Actor::SCALE_X:
        property = &mNode->mScale;
        break;

      case Dali::Actor::SCALE_Y:
        property = &mNode->mScale;
        break;

      case Dali::Actor::SCALE_Z:
        property = &mNode->mScale;
        break;

      case Dali::Actor::WORLD_SCALE:
        property = &mNode->mWorldScale;
        break;

      case Dali::Actor::VISIBLE:
        property = &mNode->mVisible;
        break;

      case Dali::Actor::COLOR:
        property = &mNode->mColor;
        break;

      case Dali::Actor::COLOR_RED:
        property = &mNode->mColor;
        break;

      case Dali::Actor::COLOR_GREEN:
        property = &mNode->mColor;
        break;

      case Dali::Actor::COLOR_BLUE:
        property = &mNode->mColor;
        break;

      case Dali::Actor::COLOR_ALPHA:
        property = &mNode->mColor;
        break;

      case Dali::Actor::WORLD_COLOR:
        property = &mNode->mWorldColor;
        break;

      case Dali::Actor::WORLD_MATRIX:
        property = &mNode->mWorldMatrix;
        break;

      default:
        break;
    }
  }

  return property;
}

int Actor::GetPropertyComponentIndex( Property::Index index ) const
{
  int componentIndex( Property::INVALID_COMPONENT_INDEX );

  switch ( index )
  {
    case Dali::Actor::PARENT_ORIGIN_X:
    case Dali::Actor::ANCHOR_POINT_X:
    case Dali::Actor::SIZE_WIDTH:
    case Dali::Actor::POSITION_X:
    case Dali::Actor::SCALE_X:
    case Dali::Actor::COLOR_RED:
    case Dali::Actor::WORLD_POSITION_X:
    {
      componentIndex = 0;
      break;
    }

    case Dali::Actor::PARENT_ORIGIN_Y:
    case Dali::Actor::ANCHOR_POINT_Y:
    case Dali::Actor::SIZE_HEIGHT:
    case Dali::Actor::POSITION_Y:
    case Dali::Actor::SCALE_Y:
    case Dali::Actor::COLOR_GREEN:
    case Dali::Actor::WORLD_POSITION_Y:
    {
      componentIndex = 1;
      break;
    }

    case Dali::Actor::PARENT_ORIGIN_Z:
    case Dali::Actor::ANCHOR_POINT_Z:
    case Dali::Actor::SIZE_DEPTH:
    case Dali::Actor::POSITION_Z:
    case Dali::Actor::SCALE_Z:
    case Dali::Actor::COLOR_BLUE:
    case Dali::Actor::WORLD_POSITION_Z:
    {
      componentIndex = 2;
      break;
    }

    case Dali::Actor::COLOR_ALPHA:
    {
      componentIndex = 3;
      break;
    }

    default:
    {
      // Do nothing
      break;
    }
  }

  return componentIndex;
}

void Actor::SetParent(Actor* parent)
{
  if( parent )
  {
    DALI_ASSERT_ALWAYS( !mParent && "Actor cannot have 2 parents" );

    mParent = parent;

    if ( Stage::IsInstalled() && // Don't emit signals or send messages during Core destruction
         parent->OnStage() )
    {
      StagePtr stage = parent->mStage;

      // Instruct each actor to create a corresponding node in the scene graph
      ConnectToStage(*stage);
    }
  }
  else // parent being set to NULL
  {
    DALI_ASSERT_ALWAYS( mParent != NULL && "Actor should have a parent" );

    mParent = NULL;

    if ( Stage::IsInstalled() && // Don't emit signals or send messages during Core destruction
         OnStage() )
    {
      DALI_ASSERT_ALWAYS(mNode != NULL);

      if( NULL != mNode )
      {
        // Disconnect the Node & its children from the scene-graph.
        DisconnectNodeMessage( mStage->GetUpdateManager(), *mNode );
      }

      // Instruct each actor to discard pointers to the scene-graph
      DisconnectFromStage();
    }
  }
}

SceneGraph::Node* Actor::CreateNode() const
{
  return Node::New();
}

bool Actor::DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes)
{
  bool done = false;
  Actor* actor = dynamic_cast<Actor*>(object);

  if( actor )
  {
    if(Dali::Actor::ACTION_SHOW == actionName)
    {
      actor->SetVisible(true);
      done = true;
    }
    else if(Dali::Actor::ACTION_HIDE == actionName)
    {
      actor->SetVisible(false);
      done = true;
    }
  }

  return done;
}

} // namespace Internal

} // namespace Dali
