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
#include <cfloat>
#include <cstring> // for strcmp

// INTERNAL INCLUDES

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/scripting/scripting.h>

#include <dali/internal/common/internal-constants.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/type-info-impl.h>
#include <dali/internal/event/actor-attachments/actor-attachment-impl.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>
#include <dali/internal/update/common/animatable-property.h>
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

namespace Dali
{
namespace ResizePolicy
{

namespace
{
DALI_ENUM_TO_STRING_TABLE_BEGIN( Type )DALI_ENUM_TO_STRING( FIXED )
DALI_ENUM_TO_STRING( USE_NATURAL_SIZE )
DALI_ENUM_TO_STRING( FILL_TO_PARENT )
DALI_ENUM_TO_STRING( SIZE_RELATIVE_TO_PARENT )
DALI_ENUM_TO_STRING( SIZE_FIXED_OFFSET_FROM_PARENT )
DALI_ENUM_TO_STRING( FIT_TO_CHILDREN )
DALI_ENUM_TO_STRING( DIMENSION_DEPENDENCY )
DALI_ENUM_TO_STRING( USE_ASSIGNED_SIZE )
DALI_ENUM_TO_STRING_TABLE_END( Type )

} // unnamed namespace
} // ResizePolicy

namespace SizeScalePolicy
{
namespace
{
// Enumeration to / from string conversion tables
DALI_ENUM_TO_STRING_TABLE_BEGIN( Type )DALI_ENUM_TO_STRING( USE_SIZE_SET )
DALI_ENUM_TO_STRING( FIT_WITH_ASPECT_RATIO )
DALI_ENUM_TO_STRING( FILL_WITH_ASPECT_RATIO )
DALI_ENUM_TO_STRING_TABLE_END( Type )
} // unnamed namespace
} // SizeScalePolicy

namespace Internal
{

unsigned int Actor::mActorCounter = 0;
ActorContainer Actor::mNullChildren;

/**
 * Struct to collect relayout variables
 */
struct Actor::RelayoutData
{
  RelayoutData()
      : sizeModeFactor( Vector3::ONE ), preferredSize( Vector2::ZERO ), sizeSetPolicy( SizeScalePolicy::USE_SIZE_SET ), relayoutEnabled( false ), insideRelayout( false )
  {
    // Set size negotiation defaults
    for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      resizePolicies[ i ] = ResizePolicy::FIXED;
      negotiatedDimensions[ i ] = 0.0f;
      dimensionNegotiated[ i ] = false;
      dimensionDirty[ i ] = false;
      dimensionDependencies[ i ] = Dimension::ALL_DIMENSIONS;
      dimensionPadding[ i ] = Vector2( 0.0f, 0.0f );
      minimumSize[ i ] = 0.0f;
      maximumSize[ i ] = FLT_MAX;
    }
  }

  ResizePolicy::Type resizePolicies[ Dimension::DIMENSION_COUNT ];      ///< Resize policies

  Dimension::Type dimensionDependencies[ Dimension::DIMENSION_COUNT ];  ///< A list of dimension dependencies

  Vector2 dimensionPadding[ Dimension::DIMENSION_COUNT ];         ///< Padding for each dimension. X = start (e.g. left, bottom), y = end (e.g. right, top)

  float negotiatedDimensions[ Dimension::DIMENSION_COUNT ];       ///< Storage for when a dimension is negotiated but before set on actor

  float minimumSize[ Dimension::DIMENSION_COUNT ];                ///< The minimum size an actor can be
  float maximumSize[ Dimension::DIMENSION_COUNT ];                ///< The maximum size an actor can be

  bool dimensionNegotiated[ Dimension::DIMENSION_COUNT ];         ///< Has the dimension been negotiated
  bool dimensionDirty[ Dimension::DIMENSION_COUNT ];              ///< Flags indicating whether the layout dimension is dirty or not

  Vector3 sizeModeFactor;                              ///< Factor of size used for certain SizeModes

  Vector2 preferredSize;                               ///< The preferred size of the actor

  SizeScalePolicy::Type sizeSetPolicy :3;            ///< Policy to apply when setting size. Enough room for the enum

  bool relayoutEnabled :1;                   ///< Flag to specify if this actor should be included in size negotiation or not (defaults to true)
  bool insideRelayout :1;                    ///< Locking flag to prevent recursive relayouts on size set
};

#ifdef DYNAMICS_SUPPORT

// Encapsulate actor related dynamics data
struct DynamicsData
{
  DynamicsData( Actor* slotOwner )
  : slotDelegate( slotOwner )
  {
  }

  typedef std::map<Actor*, DynamicsJointPtr> JointContainer;
  typedef std::vector<DynamicsJointPtr> ReferencedJointContainer;

  DynamicsBodyPtr body;
  JointContainer joints;
  ReferencedJointContainer referencedJoints;

  SlotDelegate< Actor > slotDelegate;
};

#endif // DYNAMICS_SUPPORT

namespace // unnamed namespace
{

// Properties

/**
 * We want to discourage the use of property strings (minimize string comparisons),
 * particularly for the default properties.
 *              Name                   Type   writable animatable constraint-input  enum for index-checking
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "parent-origin", VECTOR3, true, false, true, Dali::Actor::Property::PARENT_ORIGIN )
DALI_PROPERTY( "parent-origin-x", FLOAT, true, false, true, Dali::Actor::Property::PARENT_ORIGIN_X )
DALI_PROPERTY( "parent-origin-y", FLOAT, true, false, true, Dali::Actor::Property::PARENT_ORIGIN_Y )
DALI_PROPERTY( "parent-origin-z", FLOAT, true, false, true, Dali::Actor::Property::PARENT_ORIGIN_Z )
DALI_PROPERTY( "anchor-point", VECTOR3, true, false, true, Dali::Actor::Property::ANCHOR_POINT )
DALI_PROPERTY( "anchor-point-x", FLOAT, true, false, true, Dali::Actor::Property::ANCHOR_POINT_X )
DALI_PROPERTY( "anchor-point-y", FLOAT, true, false, true, Dali::Actor::Property::ANCHOR_POINT_Y )
DALI_PROPERTY( "anchor-point-z", FLOAT, true, false, true, Dali::Actor::Property::ANCHOR_POINT_Z )
DALI_PROPERTY( "size", VECTOR3, true, true, true, Dali::Actor::Property::SIZE )
DALI_PROPERTY( "size-width", FLOAT, true, true, true, Dali::Actor::Property::SIZE_WIDTH )
DALI_PROPERTY( "size-height", FLOAT, true, true, true, Dali::Actor::Property::SIZE_HEIGHT )
DALI_PROPERTY( "size-depth", FLOAT, true, true, true, Dali::Actor::Property::SIZE_DEPTH )
DALI_PROPERTY( "position", VECTOR3, true, true, true, Dali::Actor::Property::POSITION )
DALI_PROPERTY( "position-x", FLOAT, true, true, true, Dali::Actor::Property::POSITION_X )
DALI_PROPERTY( "position-y", FLOAT, true, true, true, Dali::Actor::Property::POSITION_Y )
DALI_PROPERTY( "position-z", FLOAT, true, true, true, Dali::Actor::Property::POSITION_Z )
DALI_PROPERTY( "world-position", VECTOR3, false, false, true, Dali::Actor::Property::WORLD_POSITION )
DALI_PROPERTY( "world-position-x", FLOAT, false, false, true, Dali::Actor::Property::WORLD_POSITION_X )
DALI_PROPERTY( "world-position-y", FLOAT, false, false, true, Dali::Actor::Property::WORLD_POSITION_Y )
DALI_PROPERTY( "world-position-z", FLOAT, false, false, true, Dali::Actor::Property::WORLD_POSITION_Z )
DALI_PROPERTY( "orientation", ROTATION, true, true, true, Dali::Actor::Property::ORIENTATION )
DALI_PROPERTY( "world-orientation", ROTATION, false, false, true, Dali::Actor::Property::WORLD_ORIENTATION )
DALI_PROPERTY( "scale", VECTOR3, true, true, true, Dali::Actor::Property::SCALE )
DALI_PROPERTY( "scale-x", FLOAT, true, true, true, Dali::Actor::Property::SCALE_X )
DALI_PROPERTY( "scale-y", FLOAT, true, true, true, Dali::Actor::Property::SCALE_Y )
DALI_PROPERTY( "scale-z", FLOAT, true, true, true, Dali::Actor::Property::SCALE_Z )
DALI_PROPERTY( "world-scale", VECTOR3, false, false, true, Dali::Actor::Property::WORLD_SCALE )
DALI_PROPERTY( "visible", BOOLEAN, true, true, true, Dali::Actor::Property::VISIBLE )
DALI_PROPERTY( "color", VECTOR4, true, true, true, Dali::Actor::Property::COLOR )
DALI_PROPERTY( "color-red", FLOAT, true, true, true, Dali::Actor::Property::COLOR_RED )
DALI_PROPERTY( "color-green", FLOAT, true, true, true, Dali::Actor::Property::COLOR_GREEN )
DALI_PROPERTY( "color-blue", FLOAT, true, true, true, Dali::Actor::Property::COLOR_BLUE )
DALI_PROPERTY( "color-alpha", FLOAT, true, true, true, Dali::Actor::Property::COLOR_ALPHA )
DALI_PROPERTY( "world-color", VECTOR4, false, false, true, Dali::Actor::Property::WORLD_COLOR )
DALI_PROPERTY( "world-matrix", MATRIX, false, false, true, Dali::Actor::Property::WORLD_MATRIX )
DALI_PROPERTY( "name", STRING, true, false, false, Dali::Actor::Property::NAME )
DALI_PROPERTY( "sensitive", BOOLEAN, true, false, false, Dali::Actor::Property::SENSITIVE )
DALI_PROPERTY( "leave-required", BOOLEAN, true, false, false, Dali::Actor::Property::LEAVE_REQUIRED )
DALI_PROPERTY( "inherit-orientation", BOOLEAN, true, false, false, Dali::Actor::Property::INHERIT_ORIENTATION )
DALI_PROPERTY( "inherit-scale", BOOLEAN, true, false, false, Dali::Actor::Property::INHERIT_SCALE )
DALI_PROPERTY( "color-mode", STRING, true, false, false, Dali::Actor::Property::COLOR_MODE )
DALI_PROPERTY( "position-inheritance", STRING, true, false, false, Dali::Actor::Property::POSITION_INHERITANCE )
DALI_PROPERTY( "draw-mode", STRING, true, false, false, Dali::Actor::Property::DRAW_MODE )
DALI_PROPERTY( "size-mode-factor", VECTOR3, true, false, false, Dali::Actor::Property::SIZE_MODE_FACTOR )
DALI_PROPERTY( "width-resize-policy", STRING, true, false, false, Dali::Actor::Property::WIDTH_RESIZE_POLICY )
DALI_PROPERTY( "height-resize-policy", STRING, true, false, false, Dali::Actor::Property::HEIGHT_RESIZE_POLICY )
DALI_PROPERTY( "size-scale-policy", STRING, true, false, false, Dali::Actor::Property::SIZE_SCALE_POLICY )
DALI_PROPERTY( "width-for-height", BOOLEAN, true, false, false, Dali::Actor::Property::WIDTH_FOR_HEIGHT )
DALI_PROPERTY( "height-for-width", BOOLEAN, true, false, false, Dali::Actor::Property::HEIGHT_FOR_WIDTH )
DALI_PROPERTY( "padding", VECTOR4, true, false, false, Dali::Actor::Property::PADDING )
DALI_PROPERTY( "minimum-size", VECTOR2, true, false, false, Dali::Actor::Property::MINIMUM_SIZE )
DALI_PROPERTY( "maximum-size", VECTOR2, true, false, false, Dali::Actor::Property::MAXIMUM_SIZE )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

// Signals

const char* const SIGNAL_TOUCHED = "touched";
const char* const SIGNAL_HOVERED = "hovered";
const char* const SIGNAL_MOUSE_WHEEL_EVENT = "mouse-wheel-event";
const char* const SIGNAL_ON_STAGE = "on-stage";
const char* const SIGNAL_OFF_STAGE = "off-stage";

// Actions

const char* const ACTION_SHOW = "show";
const char* const ACTION_HIDE = "hide";

BaseHandle CreateActor()
{
  return Dali::Actor::New();
}

TypeRegistration mType( typeid(Dali::Actor), typeid(Dali::Handle), CreateActor );

SignalConnectorType signalConnector1( mType, SIGNAL_TOUCHED, &Actor::DoConnectSignal );
SignalConnectorType signalConnector2( mType, SIGNAL_HOVERED, &Actor::DoConnectSignal );
SignalConnectorType signalConnector3( mType, SIGNAL_ON_STAGE, &Actor::DoConnectSignal );
SignalConnectorType signalConnector4( mType, SIGNAL_OFF_STAGE, &Actor::DoConnectSignal );

TypeAction a1( mType, ACTION_SHOW, &Actor::DoAction );
TypeAction a2( mType, ACTION_HIDE, &Actor::DoAction );

/**
 * @brief Extract a given dimension from a Vector2
 *
 * @param[in] values The values to extract from
 * @param[in] dimension The dimension to extract
 * @return Return the value for the dimension
 */
float GetDimensionValue( const Vector2& values, Dimension::Type dimension )
{
  switch( dimension )
  {
    case Dimension::WIDTH:
    {
      return values.width;
    }

    case Dimension::HEIGHT:
    {
      return values.height;
    }

    default:
    {
      break;
    }
  }

  return 0.0f;
}

/**
 * @brief Extract a given dimension from a Vector3
 *
 * @param[in] values The values to extract from
 * @param[in] dimension The dimension to extract
 * @return Return the value for the dimension
 */
float GetDimensionValue( const Vector3& values, Dimension::Type dimension )
{
  return GetDimensionValue( values.GetVectorXY(), dimension );
}

} // unnamed namespace

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

void Actor::SetName( const std::string& name )
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

  mAttachment = ActorAttachmentPtr( &attachment );
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
  for( Actor* parent = mParent; !layer && parent != NULL; parent = parent->GetParent() )
  {
    if( parent->IsLayer() )
    {
      layer = Dali::Layer( static_cast< Dali::Internal::Layer* >( parent ) ); // static cast as we trust the flag
    }
  }

  return layer;
}

void Actor::Add( Actor& child )
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

      // Old parent may need to readjust to missing child
      if( oldParent->RelayoutDependentOnChildren() )
      {
        oldParent->RelayoutRequest();
      }
    }

    // Guard against Add() during previous OnChildRemove callback
    if( !child.mParent )
    {
      // Do this first, since user callbacks from within SetParent() may need to remove child
      mChildren->push_back( Dali::Actor( &child ) );

      // SetParent asserts that child can be added
      child.SetParent( this );

      // Notification for derived classes
      OnChildAdd( child );

      // Only put in a relayout request if there is a suitable dependency
      if( RelayoutDependentOnChildren() )
      {
        RelayoutRequest();
      }
    }
  }
}

void Actor::Insert( unsigned int index, Actor& child )
{
  DALI_ASSERT_ALWAYS( this != &child && "Cannot add actor to itself" );
  DALI_ASSERT_ALWAYS( !child.IsRoot() && "Cannot add root actor" );

  if( !mChildren )
  {
    mChildren = new ActorContainer;
  }

  Actor* const oldParent( child.mParent );

  // since an explicit position has been given, always insert, even if already a child
  if( oldParent )
  {
    oldParent->Remove( child ); // This causes OnChildRemove callback

    // Old parent may need to readjust to missing child
    if( oldParent->RelayoutDependentOnChildren() )
    {
      oldParent->RelayoutRequest();
    }
  }

  // Guard against Add() during previous OnChildRemove callback
  if( !child.mParent )
  {
    // Do this first, since user callbacks from within SetParent() may need to remove child
    if( index < GetChildCount() )
    {
      ActorIter it = mChildren->begin();
      std::advance( it, index );
      mChildren->insert( it, Dali::Actor( &child ) );
    }
    else
    {
      mChildren->push_back( Dali::Actor( &child ) );
    }
    // SetParent asserts that child can be added
    child.SetParent( this, index );

    // Notification for derived classes
    OnChildAdd( child );

    // Only put in a relayout request if there is a suitable dependency
    if( RelayoutDependentOnChildren() )
    {
      RelayoutRequest();
    }

    if( child.RelayoutDependentOnParent() )
    {
      child.RelayoutRequest();
    }
  }
}

void Actor::Remove( Actor& child )
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
    Actor& actor = GetImplementation( *iter );

    if( &actor == &child )
    {
      // Keep handle for OnChildRemove notification
      removed = Dali::Actor( &actor );

      // Do this first, since user callbacks from within SetParent() may need to add the child
      mChildren->erase( iter );

      DALI_ASSERT_DEBUG( actor.GetParent() == this );
      actor.SetParent( NULL );

      break;
    }
  }

  if( removed )
  {
    // Notification for derived classes
    OnChildRemove( GetImplementation( removed ) );

    // Only put in a relayout request if there is a suitable dependency
    if( RelayoutDependentOnChildren() )
    {
      RelayoutRequest();
    }
  }
}

void Actor::Unparent()
{
  if( mParent )
  {
    // Remove this actor from the parent. The remove will put a relayout request in for
    // the parent if required
    mParent->Remove( *this );
    // mParent is now NULL!
  }
}

unsigned int Actor::GetChildCount() const
{
  return ( NULL != mChildren ) ? mChildren->size() : 0;
}

Dali::Actor Actor::GetChildAt( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < GetChildCount() );

  return ( ( mChildren ) ? ( *mChildren )[ index ] : Dali::Actor() );
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

ActorPtr Actor::FindChildByName( const std::string& actorName )
{
  ActorPtr child = 0;
  if( actorName == mName )
  {
    child = this;
  }
  else if( mChildren )
  {
    ActorIter end = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != end; ++iter )
    {
      child = GetImplementation( *iter ).FindChildByName( actorName );

      if( child )
      {
        break;
      }
    }
  }
  return child;
}

ActorPtr Actor::FindChildById( const unsigned int id )
{
  ActorPtr child = 0;
  if( id == mId )
  {
    child = this;
  }
  else if( mChildren )
  {
    ActorIter end = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != end; ++iter )
    {
      child = GetImplementation( *iter ).FindChildById( id );

      if( child )
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
    SetParentOriginMessage( GetEventThreadServices(), *mNode, origin );
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

void Actor::SetAnchorPoint( const Vector3& anchor )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SetAnchorPointMessage( GetEventThreadServices(), *mNode, anchor );
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

void Actor::SetPosition( float x, float y )
{
  SetPosition( Vector3( x, y, 0.0f ) );
}

void Actor::SetPosition( float x, float y, float z )
{
  SetPosition( Vector3( x, y, z ) );
}

void Actor::SetPosition( const Vector3& position )
{
  mTargetPosition = position;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::Bake, position );
  }
}

void Actor::SetX( float x )
{
  mTargetPosition.x = x;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::BakeX, x );
  }
}

void Actor::SetY( float y )
{
  mTargetPosition.y = y;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::BakeY, y );
  }
}

void Actor::SetZ( float z )
{
  mTargetPosition.z = z;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::BakeZ, z );
  }
}

void Actor::TranslateBy( const Vector3& distance )
{
  mTargetPosition += distance;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::BakeRelative, distance );
  }
}

const Vector3& Actor::GetCurrentPosition() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetPosition(GetEventThreadServices().GetEventBufferIndex());
  }

  return Vector3::ZERO;
}

const Vector3& Actor::GetTargetPosition() const
{
  return mTargetPosition;
}

const Vector3& Actor::GetCurrentWorldPosition() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetWorldPosition( GetEventThreadServices().GetEventBufferIndex() );
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
    SetPositionInheritanceModeMessage( GetEventThreadServices(), *mNode, mode );
  }
}

PositionInheritanceMode Actor::GetPositionInheritanceMode() const
{
  // Cached for event-thread access
  return mPositionInheritanceMode;
}

void Actor::SetOrientation( const Radian& angle, const Vector3& axis )
{
  Vector3 normalizedAxis( axis.x, axis.y, axis.z );
  normalizedAxis.Normalize();

  Quaternion orientation( angle, normalizedAxis );

  SetOrientation( orientation );
}

void Actor::SetOrientation( const Quaternion& orientation )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Quaternion>::Send( GetEventThreadServices(), mNode, &mNode->mOrientation, &AnimatableProperty<Quaternion>::Bake, orientation );
  }
}

void Actor::RotateBy( const Radian& angle, const Vector3& axis )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Quaternion>::Send( GetEventThreadServices(), mNode, &mNode->mOrientation, &AnimatableProperty<Quaternion>::BakeRelative, Quaternion(angle, axis) );
  }
}

void Actor::RotateBy( const Quaternion& relativeRotation )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Quaternion>::Send( GetEventThreadServices(), mNode, &mNode->mOrientation, &AnimatableProperty<Quaternion>::BakeRelative, relativeRotation );
  }
}

const Quaternion& Actor::GetCurrentOrientation() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetOrientation(GetEventThreadServices().GetEventBufferIndex());
  }

  return Quaternion::IDENTITY;
}

const Quaternion& Actor::GetCurrentWorldOrientation() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetWorldOrientation( GetEventThreadServices().GetEventBufferIndex() );
  }

  return Quaternion::IDENTITY;
}

void Actor::SetScale( float scale )
{
  SetScale( Vector3( scale, scale, scale ) );
}

void Actor::SetScale( float x, float y, float z )
{
  SetScale( Vector3( x, y, z ) );
}

void Actor::SetScale( const Vector3& scale )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mScale, &AnimatableProperty<Vector3>::Bake, scale );
  }
}

void Actor::SetScaleX( float x )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mScale, &AnimatableProperty<Vector3>::BakeX, x );
  }
}

void Actor::SetScaleY( float y )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mScale, &AnimatableProperty<Vector3>::BakeY, y );
  }
}

void Actor::SetScaleZ( float z )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mScale, &AnimatableProperty<Vector3>::BakeZ, z );
  }
}

void Actor::SetInitialVolume( const Vector3& volume )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetInitialVolumeMessage( GetEventThreadServices(), *mNode, volume );
  }
}

void Actor::SetTransmitGeometryScaling( bool transmitGeometryScaling )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetTransmitGeometryScalingMessage( GetEventThreadServices(), *mNode, transmitGeometryScaling );
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

void Actor::ScaleBy( const Vector3& relativeScale )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mScale, &AnimatableProperty<Vector3>::BakeRelativeMultiply, relativeScale );
  }
}

const Vector3& Actor::GetCurrentScale() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetScale(GetEventThreadServices().GetEventBufferIndex());
  }

  return Vector3::ONE;
}

const Vector3& Actor::GetCurrentWorldScale() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetWorldScale( GetEventThreadServices().GetEventBufferIndex() );
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
    SetInheritScaleMessage( GetEventThreadServices(), *mNode, inherit );
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
    // Need to calculate it from node's world position, orientation and scale:
    BufferIndex updateBufferIndex = GetEventThreadServices().GetEventBufferIndex();
    Matrix worldMatrix(false);
    worldMatrix.SetTransformComponents( mNode->GetWorldScale( updateBufferIndex ),
                                        mNode->GetWorldOrientation( updateBufferIndex ),
                                        mNode->GetWorldPosition( updateBufferIndex ) );
    return worldMatrix;
  }

  return Matrix::IDENTITY;
}

void Actor::SetVisible( bool visible )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<bool>::Send( GetEventThreadServices(), mNode, &mNode->mVisible, &AnimatableProperty<bool>::Bake, visible );
  }
}

bool Actor::IsVisible() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->IsVisible( GetEventThreadServices().GetEventBufferIndex() );
  }

  return true;
}

void Actor::SetOpacity( float opacity )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeW, opacity );
  }
}

float Actor::GetCurrentOpacity() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetOpacity(GetEventThreadServices().GetEventBufferIndex());
  }

  return 1.0f;
}

const Vector4& Actor::GetCurrentWorldColor() const
{
  if( NULL != mNode )
  {
    return mNode->GetWorldColor( GetEventThreadServices().GetEventBufferIndex() );
  }

  return Color::WHITE;
}

void Actor::SetColor( const Vector4& color )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector4>::Send( GetEventThreadServices(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::Bake, color );
  }
}

void Actor::SetColorRed( float red )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeX, red );
  }
}

void Actor::SetColorGreen( float green )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeY, green );
  }
}

void Actor::SetColorBlue( float blue )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeZ, blue );
  }
}

const Vector4& Actor::GetCurrentColor() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetColor(GetEventThreadServices().GetEventBufferIndex());
  }

  return Color::WHITE;
}

void Actor::SetInheritOrientation( bool inherit )
{
  // non animateable so keep local copy
  mInheritOrientation = inherit;
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetInheritOrientationMessage( GetEventThreadServices(), *mNode, inherit );
  }
}

bool Actor::IsOrientationInherited() const
{
  return mInheritOrientation;
}

void Actor::SetSizeModeFactor( const Vector3& factor )
{
  EnsureRelayoutData();

  mRelayoutData->sizeModeFactor = factor;
}

const Vector3& Actor::GetSizeModeFactor() const
{
  EnsureRelayoutData();

  return mRelayoutData->sizeModeFactor;
}

void Actor::SetColorMode( ColorMode colorMode )
{
  // non animateable so keep local copy
  mColorMode = colorMode;
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetColorModeMessage( GetEventThreadServices(), *mNode, colorMode );
  }
}

ColorMode Actor::GetColorMode() const
{
  // we have cached copy
  return mColorMode;
}

void Actor::SetSize( float width, float height )
{
  SetSize( Vector2( width, height ) );
}

void Actor::SetSize( float width, float height, float depth )
{
  SetSize( Vector3( width, height, depth ) );
}

void Actor::SetSize( const Vector2& size )
{
  SetSize( Vector3( size.width, size.height, CalculateSizeZ( size ) ) );
}

void Actor::SetSizeInternal( const Vector2& size )
{
  SetSizeInternal( Vector3( size.width, size.height, CalculateSizeZ( size ) ) );
}

float Actor::CalculateSizeZ( const Vector2& size ) const
{
  return std::min( size.width, size.height );
}

void Actor::SetSize( const Vector3& size )
{
  if( IsRelayoutEnabled() && !mRelayoutData->insideRelayout )
  {
    SetPreferredSize( size.GetVectorXY() );
  }
  else
  {
    SetSizeInternal( size );
  }
}

void Actor::SetSizeInternal( const Vector3& size )
{
  if( NULL != mNode )
  {
    mTargetSize = size;

    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mSize, &AnimatableProperty<Vector3>::Bake, mTargetSize );

    // Notification for derived classes
    OnSizeSet( mTargetSize );

    // Raise a relayout request if the flag is not locked
    if( mRelayoutData && !mRelayoutData->insideRelayout )
    {
      RelayoutRequest();
    }
  }
}

void Actor::NotifySizeAnimation( Animation& animation, const Vector3& targetSize )
{
  mTargetSize = targetSize;

  // Notify deriving classes
  OnSizeAnimation( animation, targetSize );
}

void Actor::SetWidth( float width )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mSize, &AnimatableProperty<Vector3>::BakeX, width );
  }
}

void Actor::SetHeight( float height )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mSize, &AnimatableProperty<Vector3>::BakeY, height );
  }
}

void Actor::SetDepth( float depth )
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mSize, &AnimatableProperty<Vector3>::BakeZ, depth );
  }
}

const Vector3& Actor::GetTargetSize() const
{
  return mTargetSize;
}

const Vector3& Actor::GetCurrentSize() const
{
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; copy the value from the previous update
    return mNode->GetSize( GetEventThreadServices().GetEventBufferIndex() );
  }

  return Vector3::ZERO;
}

Vector3 Actor::GetNaturalSize() const
{
  // It is up to deriving classes to return the appropriate natural size
  return Vector3( 0.0f, 0.0f, 0.0f );
}

void Actor::SetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
{
  EnsureRelayoutData();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      mRelayoutData->resizePolicies[ i ] = policy;
    }
  }

  if( policy == ResizePolicy::DIMENSION_DEPENDENCY )
  {
    if( dimension & Dimension::WIDTH )
    {
      SetDimensionDependency( Dimension::WIDTH, Dimension::HEIGHT );
    }

    if( dimension & Dimension::HEIGHT )
    {
      SetDimensionDependency( Dimension::HEIGHT, Dimension::WIDTH );
    }
  }

  // If calling SetResizePolicy, assume we want relayout enabled
  SetRelayoutEnabled( true );

  OnSetResizePolicy( policy, dimension );

  // Trigger relayout on this control
  RelayoutRequest();
}

ResizePolicy::Type Actor::GetResizePolicy( Dimension::Type dimension ) const
{
  EnsureRelayoutData();

  // If more than one dimension is requested, just return the first one found
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( ( dimension & ( 1 << i ) ) )
    {
      return mRelayoutData->resizePolicies[ i ];
    }
  }

  return ResizePolicy::FIXED;   // Default
}

void Actor::SetSizeScalePolicy( SizeScalePolicy::Type policy )
{
  EnsureRelayoutData();

  mRelayoutData->sizeSetPolicy = policy;
}

SizeScalePolicy::Type Actor::GetSizeScalePolicy() const
{
  EnsureRelayoutData();

  return mRelayoutData->sizeSetPolicy;
}

void Actor::SetDimensionDependency( Dimension::Type dimension, Dimension::Type dependency )
{
  EnsureRelayoutData();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      mRelayoutData->dimensionDependencies[ i ] = dependency;
    }
  }
}

Dimension::Type Actor::GetDimensionDependency( Dimension::Type dimension ) const
{
  EnsureRelayoutData();

  // If more than one dimension is requested, just return the first one found
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( ( dimension & ( 1 << i ) ) )
    {
      return mRelayoutData->dimensionDependencies[ i ];
    }
  }

  return Dimension::ALL_DIMENSIONS;   // Default
}

void Actor::SetRelayoutEnabled( bool relayoutEnabled )
{
  // If relayout data has not been allocated yet and the client is requesting
  // to disable it, do nothing
  if( mRelayoutData || relayoutEnabled )
  {
    EnsureRelayoutData();

    mRelayoutData->relayoutEnabled = relayoutEnabled;
  }
}

bool Actor::IsRelayoutEnabled() const
{
  // Assume that if relayout data has not been allocated yet then
  // relayout is disabled
  return mRelayoutData && mRelayoutData->relayoutEnabled;
}

void Actor::SetLayoutDirty( bool dirty, Dimension::Type dimension )
{
  EnsureRelayoutData();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      mRelayoutData->dimensionDirty[ i ] = dirty;
    }
  }
}

bool Actor::IsLayoutDirty( Dimension::Type dimension ) const
{
  EnsureRelayoutData();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( ( dimension & ( 1 << i ) ) && mRelayoutData->dimensionDirty[ i ] )
    {
      return true;
    }
  }

  return false;
}

bool Actor::RelayoutPossible( Dimension::Type dimension ) const
{
  EnsureRelayoutData();

  return mRelayoutData->relayoutEnabled && !IsLayoutDirty( dimension );
}

bool Actor::RelayoutRequired( Dimension::Type dimension ) const
{
  EnsureRelayoutData();

  return mRelayoutData->relayoutEnabled && IsLayoutDirty( dimension );
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
      DynamicsWorldPtr world( DynamicsWorld::Get() );
      if( world )
      {
        if( mParent == world->GetRootActor().Get() )
        {
          mDynamicsData->body->Connect( GetEventThreadServices() );
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

  DynamicsWorldPtr world( DynamicsWorld::Get() );

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
          joint->Connect( GetEventThreadServices() );
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

      for( int i = 0; i < index; ++i )
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

    for(; it != endIt; ++it )
    {
      if( it->second == joint.Get() )
      {
        ActorPtr attachedActor( it->first );

        if( OnStage() && attachedActor && attachedActor->OnStage() )
        {
          joint->Disconnect( GetEventThreadServices() );
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
      DynamicsData::JointContainer::iterator it( mDynamicsData->joints.find( attachedActor.Get() ) );
      if( mDynamicsData->joints.end() != it )
      {
        DynamicsJointPtr joint( it->second );
        joint->Connect( GetEventThreadServices() );
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
      DynamicsData::JointContainer::iterator it( mDynamicsData->joints.find( attachedActor.Get() ) );
      if( mDynamicsData->joints.end() != it )
      {
        DynamicsJointPtr joint( it->second );
        joint->Disconnect( GetEventThreadServices() );
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
      mDynamicsData->body->Connect( GetEventThreadServices() );

      // Connect all joints where attachedActor is also on stage
      if( !mDynamicsData->joints.empty() )
      {
        DynamicsData::JointContainer::iterator it( mDynamicsData->joints.begin() );
        DynamicsData::JointContainer::iterator endIt( mDynamicsData->joints.end() );

        for(; it != endIt; ++it )
        {
          Actor* attachedActor( it->first );
          if( NULL != attachedActor && attachedActor->OnStage() )
          {
            DynamicsJointPtr joint( it->second );

            joint->Connect( GetEventThreadServices() );
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
      mDynamicsData->body->Disconnect( GetEventThreadServices() );

      // Disconnect all joints
      if( !mDynamicsData->joints.empty() )
      {
        DynamicsData::JointContainer::iterator it( mDynamicsData->joints.begin() );
        DynamicsData::JointContainer::iterator endIt( mDynamicsData->joints.end() );

        for(; it != endIt; ++it )
        {
          DynamicsJointPtr joint( it->second );

          joint->Disconnect( GetEventThreadServices() );
        }
      }
    }
  }
}

#endif // DYNAMICS_SUPPORT

void Actor::SetOverlay( bool enable )
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
    SetDrawModeMessage( GetEventThreadServices(), *mNode, drawMode );
  }
}

DrawMode::Type Actor::GetDrawMode() const
{
  return mDrawMode;
}

bool Actor::ScreenToLocal( float& localX, float& localY, float screenX, float screenY ) const
{
  // only valid when on-stage
  if( OnStage() )
  {
    const RenderTaskList& taskList = Stage::GetCurrent()->GetRenderTaskList();

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

bool Actor::ScreenToLocal( RenderTask& renderTask, float& localX, float& localY, float screenX, float screenY ) const
{
  bool retval = false;
  // only valid when on-stage
  if( OnStage() )
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

bool Actor::ScreenToLocal( const Matrix& viewMatrix, const Matrix& projectionMatrix, const Viewport& viewport, float& localX, float& localY, float screenX, float screenY ) const
{
  // Early-out if mNode is NULL
  if( !OnStage() )
  {
    return false;
  }

  BufferIndex bufferIndex( GetEventThreadServices().GetEventBufferIndex() );

  // Calculate the ModelView matrix
  Matrix modelView( false/*don't init*/);
  // need to use the components as world matrix is only updated for actors that need it
  modelView.SetTransformComponents( mNode->GetWorldScale( bufferIndex ), mNode->GetWorldOrientation( bufferIndex ), mNode->GetWorldPosition( bufferIndex ) );
  Matrix::Multiply( modelView, modelView, viewMatrix );

  // Calculate the inverted ModelViewProjection matrix; this will be used for 2 unprojects
  Matrix invertedMvp( false/*don't init*/);
  Matrix::Multiply( invertedMvp, modelView, projectionMatrix );
  bool success = invertedMvp.Invert();

  // Convert to GL coordinates
  Vector4 screenPos( screenX - viewport.x, viewport.height - ( screenY - viewport.y ), 0.f, 1.f );

  Vector4 nearPos;
  if( success )
  {
    success = Unproject( screenPos, invertedMvp, viewport.width, viewport.height, nearPos );
  }

  Vector4 farPos;
  if( success )
  {
    screenPos.z = 1.0f;
    success = Unproject( screenPos, invertedMvp, viewport.width, viewport.height, farPos );
  }

  if( success )
  {
    Vector4 local;
    if( XyPlaneIntersect( nearPos, farPos, local ) )
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

  BufferIndex bufferIndex( GetEventThreadServices().GetEventBufferIndex() );

  // Transforms the ray to the local reference system. As the test is against a sphere, only the translation and scale are needed.
  const Vector3& translation( mNode->GetWorldPosition( bufferIndex ) );
  Vector3 rayOriginLocal( rayOrigin.x - translation.x, rayOrigin.y - translation.y, rayOrigin.z - translation.z );

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

  return ( b2 * b2 - a * c ) >= 0.f;
}

bool Actor::RayActorTest( const Vector4& rayOrigin, const Vector4& rayDir, Vector4& hitPointLocal, float& distance ) const
{
  bool hit = false;

  if( OnStage() &&
  NULL != mNode )
  {
    BufferIndex bufferIndex( GetEventThreadServices().GetEventBufferIndex() );

    // Transforms the ray to the local reference system.

    // Calculate the inverse of Model matrix
    Matrix invModelMatrix( false/*don't init*/);
    // need to use the components as world matrix is only updated for actors that need it
    invModelMatrix.SetInverseTransformComponents( mNode->GetWorldScale( bufferIndex ), mNode->GetWorldOrientation( bufferIndex ), mNode->GetWorldPosition( bufferIndex ) );

    Vector4 rayOriginLocal( invModelMatrix * rayOrigin );
    Vector4 rayDirLocal( invModelMatrix * rayDir - invModelMatrix.GetTranslation() );

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

void Actor::SetLeaveRequired( bool required )
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
  return !mTouchedSignal.Empty() || mDerivedRequiresTouch;
}

bool Actor::GetHoverRequired() const
{
  return !mHoveredSignal.Empty() || mDerivedRequiresHover;
}

bool Actor::GetMouseWheelEventRequired() const
{
  return !mMouseWheelEventSignal.Empty() || mDerivedRequiresMouseWheelEvent;
}

bool Actor::IsHittable() const
{
  return IsSensitive() && IsVisible() && ( GetCurrentWorldColor().a > FULLY_TRANSPARENT ) && IsNodeConnected();
}

ActorGestureData& Actor::GetGestureData()
{
  // Likely scenario is that once gesture-data is created for this actor, the actor will require
  // that gesture for its entire life-time so no need to destroy it until the actor is destroyed
  if( NULL == mGestureData )
  {
    mGestureData = new ActorGestureData;
  }
  return *mGestureData;
}

bool Actor::IsGestureRequred( Gesture::Type type ) const
{
  return mGestureData && mGestureData->IsGestureRequred( type );
}

bool Actor::EmitTouchEventSignal( const TouchEvent& event )
{
  bool consumed = false;

  if( !mTouchedSignal.Empty() )
  {
    Dali::Actor handle( this );
    consumed = mTouchedSignal.Emit( handle, event );
  }

  if( !consumed )
  {
    // Notification for derived classes
    consumed = OnTouchEvent( event );
  }

  return consumed;
}

bool Actor::EmitHoverEventSignal( const HoverEvent& event )
{
  bool consumed = false;

  if( !mHoveredSignal.Empty() )
  {
    Dali::Actor handle( this );
    consumed = mHoveredSignal.Emit( handle, event );
  }

  if( !consumed )
  {
    // Notification for derived classes
    consumed = OnHoverEvent( event );
  }

  return consumed;
}

bool Actor::EmitMouseWheelEventSignal( const MouseWheelEvent& event )
{
  bool consumed = false;

  if( !mMouseWheelEventSignal.Empty() )
  {
    Dali::Actor handle( this );
    consumed = mMouseWheelEventSignal.Emit( handle, event );
  }

  if( !consumed )
  {
    // Notification for derived classes
    consumed = OnMouseWheelEvent( event );
  }

  return consumed;
}

Dali::Actor::TouchSignalType& Actor::TouchedSignal()
{
  return mTouchedSignal;
}

Dali::Actor::HoverSignalType& Actor::HoveredSignal()
{
  return mHoveredSignal;
}

Dali::Actor::MouseWheelEventSignalType& Actor::MouseWheelEventSignal()
{
  return mMouseWheelEventSignal;
}

Dali::Actor::OnStageSignalType& Actor::OnStageSignal()
{
  return mOnStageSignal;
}

Dali::Actor::OffStageSignalType& Actor::OffStageSignal()
{
  return mOffStageSignal;
}

Dali::Actor::OnRelayoutSignalType& Actor::OnRelayoutSignal()
{
  return mOnRelayoutSignal;
}

bool Actor::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  Actor* actor = dynamic_cast< Actor* >( object );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_TOUCHED ) ) // don't want to convert char* to string
  {
    actor->TouchedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_HOVERED ) )
  {
    actor->HoveredSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_MOUSE_WHEEL_EVENT ) )
  {
    actor->MouseWheelEventSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_ON_STAGE ) )
  {
    actor->OnStageSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_OFF_STAGE ) )
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
: mParent( NULL ),
  mChildren( NULL ),
  mNode( NULL ),
  mParentOrigin( NULL ),
  mAnchorPoint( NULL ),
  mRelayoutData( NULL ),
#ifdef DYNAMICS_SUPPORT
        mDynamicsData( NULL ),
#endif
        mGestureData( NULL ), mAttachment(), mTargetSize( 0.0f, 0.0f, 0.0f ), mName(), mId( ++mActorCounter ), // actor ID is initialised to start from 1, and 0 is reserved
        mIsRoot( ROOT_LAYER == derivedType ), mIsRenderable( RENDERABLE == derivedType ), mIsLayer( LAYER == derivedType || ROOT_LAYER == derivedType ), mIsOnStage( false ), mIsDynamicsRoot( false ), mSensitive( true ), mLeaveRequired( false ), mKeyboardFocusable( false ), mDerivedRequiresTouch( false ), mDerivedRequiresHover( false ), mDerivedRequiresMouseWheelEvent( false ), mOnStageSignalled( false ), mInheritOrientation( true ), mInheritScale( true ), mDrawMode( DrawMode::NORMAL ), mPositionInheritanceMode( Node::DEFAULT_POSITION_INHERITANCE_MODE ), mColorMode( Node::DEFAULT_COLOR_MODE )
{
}

void Actor::Initialize()
{
  // Node creation
  SceneGraph::Node* node = CreateNode();

  AddNodeMessage( GetEventThreadServices().GetUpdateManager(), *node ); // Pass ownership to scene-graph
  mNode = node; // Keep raw-pointer to Node

  OnInitialize();

  GetEventThreadServices().RegisterObject( this );
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
  if( EventThreadServices::IsCoreRunning() )
  {
    if( NULL != mNode )
    {
      DestroyNodeMessage( GetEventThreadServices().GetUpdateManager(), *mNode );
      mNode = NULL; // Node is about to be destroyed
    }

    GetEventThreadServices().UnregisterObject( this );
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

  // Delete optional relayout data
  if( mRelayoutData )
  {
    delete mRelayoutData;
  }
}

void Actor::ConnectToStage( int index )
{
  // This container is used instead of walking the Actor hierachy.
  // It protects us when the Actor hierachy is modified during OnStageConnectionExternal callbacks.
  ActorContainer connectionList;

  // This stage is atomic i.e. not interrupted by user callbacks
  RecursiveConnectToStage( connectionList, index );

  // Notify applications about the newly connected actors.
  const ActorIter endIter = connectionList.end();
  for( ActorIter iter = connectionList.begin(); iter != endIter; ++iter )
  {
    Actor& actor = GetImplementation( *iter );
    actor.NotifyStageConnection();
  }

  RelayoutRequest();
}

void Actor::RecursiveConnectToStage( ActorContainer& connectionList, int index )
{
  DALI_ASSERT_ALWAYS( !OnStage() );

  mIsOnStage = true;

  ConnectToSceneGraph( index );

  // Notification for internal derived classes
  OnStageConnectionInternal();

  // This stage is atomic; avoid emitting callbacks until all Actors are connected
  connectionList.push_back( Dali::Actor( this ) );

  // Recursively connect children
  if( mChildren )
  {
    ActorConstIter endIter = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != endIter; ++iter )
    {
      Actor& actor = GetImplementation( *iter );
      actor.RecursiveConnectToStage( connectionList );
    }
  }
}

/**
 * This method is called when the Actor is connected to the Stage.
 * The parent must have added its Node to the scene-graph.
 * The child must connect its Node to the parent's Node.
 * This is resursive; the child calls ConnectToStage() for its children.
 */
void Actor::ConnectToSceneGraph( int index )
{
  DALI_ASSERT_DEBUG( mNode != NULL); DALI_ASSERT_DEBUG( mParent != NULL); DALI_ASSERT_DEBUG( mParent->mNode != NULL );

  if( NULL != mNode )
  {
    // Reparent Node in next Update
    ConnectNodeMessage( GetEventThreadServices().GetUpdateManager(), *(mParent->mNode), *mNode, index );
  }

  // Notify attachment
  if( mAttachment )
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

  // Request relayout on all actors that are added to the scenegraph
  RelayoutRequest();

  // Notification for Object::Observers
  OnSceneObjectAdd();
}

void Actor::NotifyStageConnection()
{
  // Actors can be removed (in a callback), before the on-stage stage is reported.
  // The actor may also have been reparented, in which case mOnStageSignalled will be true.
  if( OnStage() && !mOnStageSignalled )
  {
    // Notification for external (CustomActor) derived classes
    OnStageConnectionExternal();

    if( !mOnStageSignal.Empty() )
    {
      Dali::Actor handle( this );
      mOnStageSignal.Emit( handle );
    }

    // Guard against Remove during callbacks
    if( OnStage() )
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
    Actor& actor = GetImplementation( *iter );
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
  disconnectionList.push_back( Dali::Actor( this ) );

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
  // Notification for Object::Observers
  OnSceneObjectRemove();

  // Notify attachment
  if( mAttachment )
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
  if ( EventThreadServices::IsCoreRunning() && !OnStage() && mOnStageSignalled )
  {
    // Notification for external (CustomeActor) derived classes
    OnStageDisconnectionExternal();

    if( !mOffStageSignal.Empty() )
    {
      Dali::Actor handle( this );
      mOffStageSignal.Emit( handle );
    }

    // Guard against Add during callbacks
    if( !OnStage() )
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

unsigned int Actor::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void Actor::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const char* Actor::GetDefaultPropertyName( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].name;
  }

  return NULL;
}

Property::Index Actor::GetDefaultPropertyIndex( const std::string& name ) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in default properties
  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_PROPERTY_DETAILS[ i ];
    if( 0 == strcmp( name.c_str(), property->name ) ) // dont want to convert rhs to string
    {
      index = i;
      break;
    }
  }

  return index;
}

bool Actor::IsDefaultPropertyWritable( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].writable;
  }

  return false;
}

bool Actor::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].animatable;
  }

  return false;
}

bool Actor::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].constraintInput;
  }

  return false;
}

Property::Type Actor::GetDefaultPropertyType( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].type;
  }

  // index out of range...return Property::NONE
  return Property::NONE;
}

void Actor::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  switch( index )
  {
    case Dali::Actor::Property::PARENT_ORIGIN:
    {
      SetParentOrigin( property.Get< Vector3 >() );
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_X:
    {
      SetParentOriginX( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Y:
    {
      SetParentOriginY( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Z:
    {
      SetParentOriginZ( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT:
    {
      SetAnchorPoint( property.Get< Vector3 >() );
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_X:
    {
      SetAnchorPointX( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_Y:
    {
      SetAnchorPointY( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_Z:
    {
      SetAnchorPointZ( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::SIZE:
    {
      SetSize( property.Get< Vector3 >() );
      break;
    }

    case Dali::Actor::Property::SIZE_WIDTH:
    {
      SetWidth( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::SIZE_HEIGHT:
    {
      SetHeight( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::SIZE_DEPTH:
    {
      SetDepth( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::POSITION:
    {
      SetPosition( property.Get< Vector3 >() );
      break;
    }

    case Dali::Actor::Property::POSITION_X:
    {
      SetX( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::POSITION_Y:
    {
      SetY( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::POSITION_Z:
    {
      SetZ( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::ORIENTATION:
    {
      SetOrientation( property.Get< Quaternion >() );
      break;
    }

    case Dali::Actor::Property::SCALE:
    {
      SetScale( property.Get< Vector3 >() );
      break;
    }

    case Dali::Actor::Property::SCALE_X:
    {
      SetScaleX( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::SCALE_Y:
    {
      SetScaleY( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::SCALE_Z:
    {
      SetScaleZ( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::VISIBLE:
    {
      SetVisible( property.Get< bool >() );
      break;
    }

    case Dali::Actor::Property::COLOR:
    {
      SetColor( property.Get< Vector4 >() );
      break;
    }

    case Dali::Actor::Property::COLOR_RED:
    {
      SetColorRed( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::COLOR_GREEN:
    {
      SetColorGreen( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::COLOR_BLUE:
    {
      SetColorBlue( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::COLOR_ALPHA:
    {
      SetOpacity( property.Get< float >() );
      break;
    }

    case Dali::Actor::Property::NAME:
    {
      SetName( property.Get< std::string >() );
      break;
    }

    case Dali::Actor::Property::SENSITIVE:
    {
      SetSensitive( property.Get< bool >() );
      break;
    }

    case Dali::Actor::Property::LEAVE_REQUIRED:
    {
      SetLeaveRequired( property.Get< bool >() );
      break;
    }

    case Dali::Actor::Property::INHERIT_ORIENTATION:
    {
      SetInheritOrientation( property.Get< bool >() );
      break;
    }

    case Dali::Actor::Property::INHERIT_SCALE:
    {
      SetInheritScale( property.Get< bool >() );
      break;
    }

    case Dali::Actor::Property::COLOR_MODE:
    {
      SetColorMode( Scripting::GetColorMode( property.Get< std::string >() ) );
      break;
    }

    case Dali::Actor::Property::POSITION_INHERITANCE:
    {
      SetPositionInheritanceMode( Scripting::GetPositionInheritanceMode( property.Get< std::string >() ) );
      break;
    }

    case Dali::Actor::Property::DRAW_MODE:
    {
      SetDrawMode( Scripting::GetDrawMode( property.Get< std::string >() ) );
      break;
    }

    case Dali::Actor::Property::SIZE_MODE_FACTOR:
    {
      SetSizeModeFactor( property.Get< Vector3 >() );
      break;
    }

    case Dali::Actor::Property::WIDTH_RESIZE_POLICY:
    {
      SetResizePolicy( Scripting::GetEnumeration< ResizePolicy::Type >( property.Get< std::string >().c_str(), ResizePolicy::TypeTable, ResizePolicy::TypeTableCount ), Dimension::WIDTH );
      break;
    }

    case Dali::Actor::Property::HEIGHT_RESIZE_POLICY:
    {
      SetResizePolicy( Scripting::GetEnumeration< ResizePolicy::Type >( property.Get< std::string >().c_str(), ResizePolicy::TypeTable, ResizePolicy::TypeTableCount ), Dimension::HEIGHT );
      break;
    }

    case Dali::Actor::Property::SIZE_SCALE_POLICY:
    {
      SetSizeScalePolicy( Scripting::GetEnumeration< SizeScalePolicy::Type >( property.Get< std::string >().c_str(), SizeScalePolicy::TypeTable, SizeScalePolicy::TypeTableCount ) );
      break;
    }

    case Dali::Actor::Property::WIDTH_FOR_HEIGHT:
    {
      if( property.Get< bool >() )
      {
        SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::WIDTH );
      }
      break;
    }

    case Dali::Actor::Property::HEIGHT_FOR_WIDTH:
    {
      if( property.Get< bool >() )
      {
        SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
      }
      break;
    }

    case Dali::Actor::Property::PADDING:
    {
      Vector4 padding = property.Get< Vector4 >();
      SetPadding( Vector2( padding.x, padding.y ), Dimension::WIDTH );
      SetPadding( Vector2( padding.z, padding.w ), Dimension::HEIGHT );
      break;
    }

    case Dali::Actor::Property::MINIMUM_SIZE:
    {
      Vector2 size = property.Get< Vector2 >();
      SetMinimumSize( size.x, Dimension::WIDTH );
      SetMinimumSize( size.y, Dimension::HEIGHT );
      break;
    }

    case Dali::Actor::Property::MAXIMUM_SIZE:
    {
      Vector2 size = property.Get< Vector2 >();
      SetMaximumSize( size.x, Dimension::WIDTH );
      SetMaximumSize( size.y, Dimension::HEIGHT );
      break;
    }

    default:
    {
      // this can happen in the case of a non-animatable default property so just do nothing
      break;
    }
  }
}

// TODO: This method needs to be removed
void Actor::SetSceneGraphProperty( Property::Index index, const PropertyMetadata& entry, const Property::Value& value )
{
  OnPropertySet( index, value );

  switch( entry.type )
  {
    case Property::BOOLEAN:
    {
      const AnimatableProperty< bool >* property = dynamic_cast< const AnimatableProperty< bool >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<bool>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<bool>::Bake, value.Get<bool>() );

      break;
    }

    case Property::INTEGER:
    {
      const AnimatableProperty< int >* property = dynamic_cast< const AnimatableProperty< int >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<int>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<int>::Bake, value.Get<int>() );

      break;
    }

    case Property::UNSIGNED_INTEGER:
    {
      const AnimatableProperty< unsigned int >* property = dynamic_cast< const AnimatableProperty< unsigned int >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<unsigned int>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<unsigned int>::Bake, value.Get<unsigned int>() );

      break;
    }

    case Property::FLOAT:
    {
      const AnimatableProperty< float >* property = dynamic_cast< const AnimatableProperty< float >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<float>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<float>::Bake, value.Get<float>() );

      break;
    }

    case Property::VECTOR2:
    {
      const AnimatableProperty< Vector2 >* property = dynamic_cast< const AnimatableProperty< Vector2 >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<Vector2>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector2>::Bake, value.Get<Vector2>() );

      break;
    }

    case Property::VECTOR3:
    {
      const AnimatableProperty< Vector3 >* property = dynamic_cast< const AnimatableProperty< Vector3 >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector3>::Bake, value.Get<Vector3>() );

      break;
    }

    case Property::VECTOR4:
    {
      const AnimatableProperty< Vector4 >* property = dynamic_cast< const AnimatableProperty< Vector4 >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<Vector4>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector4>::Bake, value.Get<Vector4>() );

      break;
    }

    case Property::ROTATION:
    {
      const AnimatableProperty< Quaternion >* property = dynamic_cast< const AnimatableProperty< Quaternion >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<Quaternion>::Send( GetEventThreadServices(), mNode, property,&AnimatableProperty<Quaternion>::Bake,  value.Get<Quaternion>() );

      break;
    }

    case Property::MATRIX:
    {
      const AnimatableProperty< Matrix >* property = dynamic_cast< const AnimatableProperty< Matrix >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<Matrix>::Send( GetEventThreadServices(), mNode, property,&AnimatableProperty<Matrix>::Bake,  value.Get<Matrix>() );

      break;
    }

    case Property::MATRIX3:
    {
      const AnimatableProperty< Matrix3 >* property = dynamic_cast< const AnimatableProperty< Matrix3 >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<Matrix3>::Send( GetEventThreadServices(), mNode, property,&AnimatableProperty<Matrix3>::Bake,  value.Get<Matrix3>() );

      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS( false && "Property type enumeration out of bounds" ); // should not come here
      break;
    }
  }
}

Property::Value Actor::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  switch( index )
  {
    case Dali::Actor::Property::PARENT_ORIGIN:
    {
      value = GetCurrentParentOrigin();
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_X:
    {
      value = GetCurrentParentOrigin().x;
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Y:
    {
      value = GetCurrentParentOrigin().y;
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Z:
    {
      value = GetCurrentParentOrigin().z;
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT:
    {
      value = GetCurrentAnchorPoint();
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_X:
    {
      value = GetCurrentAnchorPoint().x;
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_Y:
    {
      value = GetCurrentAnchorPoint().y;
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_Z:
    {
      value = GetCurrentAnchorPoint().z;
      break;
    }

    case Dali::Actor::Property::SIZE:
    {
      value = GetCurrentSize();
      break;
    }

    case Dali::Actor::Property::SIZE_WIDTH:
    {
      value = GetCurrentSize().width;
      break;
    }

    case Dali::Actor::Property::SIZE_HEIGHT:
    {
      value = GetCurrentSize().height;
      break;
    }

    case Dali::Actor::Property::SIZE_DEPTH:
    {
      value = GetCurrentSize().depth;
      break;
    }

    case Dali::Actor::Property::POSITION:
    {
      value = GetCurrentPosition();
      break;
    }

    case Dali::Actor::Property::POSITION_X:
    {
      value = GetCurrentPosition().x;
      break;
    }

    case Dali::Actor::Property::POSITION_Y:
    {
      value = GetCurrentPosition().y;
      break;
    }

    case Dali::Actor::Property::POSITION_Z:
    {
      value = GetCurrentPosition().z;
      break;
    }

    case Dali::Actor::Property::WORLD_POSITION:
    {
      value = GetCurrentWorldPosition();
      break;
    }

    case Dali::Actor::Property::WORLD_POSITION_X:
    {
      value = GetCurrentWorldPosition().x;
      break;
    }

    case Dali::Actor::Property::WORLD_POSITION_Y:
    {
      value = GetCurrentWorldPosition().y;
      break;
    }

    case Dali::Actor::Property::WORLD_POSITION_Z:
    {
      value = GetCurrentWorldPosition().z;
      break;
    }

    case Dali::Actor::Property::ORIENTATION:
    {
      value = GetCurrentOrientation();
      break;
    }

    case Dali::Actor::Property::WORLD_ORIENTATION:
    {
      value = GetCurrentWorldOrientation();
      break;
    }

    case Dali::Actor::Property::SCALE:
    {
      value = GetCurrentScale();
      break;
    }

    case Dali::Actor::Property::SCALE_X:
    {
      value = GetCurrentScale().x;
      break;
    }

    case Dali::Actor::Property::SCALE_Y:
    {
      value = GetCurrentScale().y;
      break;
    }

    case Dali::Actor::Property::SCALE_Z:
    {
      value = GetCurrentScale().z;
      break;
    }

    case Dali::Actor::Property::WORLD_SCALE:
    {
      value = GetCurrentWorldScale();
      break;
    }

    case Dali::Actor::Property::VISIBLE:
    {
      value = IsVisible();
      break;
    }

    case Dali::Actor::Property::COLOR:
    {
      value = GetCurrentColor();
      break;
    }

    case Dali::Actor::Property::COLOR_RED:
    {
      value = GetCurrentColor().r;
      break;
    }

    case Dali::Actor::Property::COLOR_GREEN:
    {
      value = GetCurrentColor().g;
      break;
    }

    case Dali::Actor::Property::COLOR_BLUE:
    {
      value = GetCurrentColor().b;
      break;
    }

    case Dali::Actor::Property::COLOR_ALPHA:
    {
      value = GetCurrentColor().a;
      break;
    }

    case Dali::Actor::Property::WORLD_COLOR:
    {
      value = GetCurrentWorldColor();
      break;
    }

    case Dali::Actor::Property::WORLD_MATRIX:
    {
      value = GetCurrentWorldMatrix();
      break;
    }

    case Dali::Actor::Property::NAME:
    {
      value = GetName();
      break;
    }

    case Dali::Actor::Property::SENSITIVE:
    {
      value = IsSensitive();
      break;
    }

    case Dali::Actor::Property::LEAVE_REQUIRED:
    {
      value = GetLeaveRequired();
      break;
    }

    case Dali::Actor::Property::INHERIT_ORIENTATION:
    {
      value = IsOrientationInherited();
      break;
    }

    case Dali::Actor::Property::INHERIT_SCALE:
    {
      value = IsScaleInherited();
      break;
    }

    case Dali::Actor::Property::COLOR_MODE:
    {
      value = Scripting::GetColorMode( GetColorMode() );
      break;
    }

    case Dali::Actor::Property::POSITION_INHERITANCE:
    {
      value = Scripting::GetPositionInheritanceMode( GetPositionInheritanceMode() );
      break;
    }

    case Dali::Actor::Property::DRAW_MODE:
    {
      value = Scripting::GetDrawMode( GetDrawMode() );
      break;
    }

    case Dali::Actor::Property::SIZE_MODE_FACTOR:
    {
      value = GetSizeModeFactor();
      break;
    }

    case Dali::Actor::Property::WIDTH_RESIZE_POLICY:
    {
      value = Scripting::GetLinearEnumerationName< ResizePolicy::Type >( GetResizePolicy( Dimension::WIDTH ), ResizePolicy::TypeTable, ResizePolicy::TypeTableCount );
      break;
    }

    case Dali::Actor::Property::HEIGHT_RESIZE_POLICY:
    {
      value = Scripting::GetLinearEnumerationName< ResizePolicy::Type >( GetResizePolicy( Dimension::HEIGHT ), ResizePolicy::TypeTable, ResizePolicy::TypeTableCount );
      break;
    }

    case Dali::Actor::Property::SIZE_SCALE_POLICY:
    {
      value = Scripting::GetLinearEnumerationName< SizeScalePolicy::Type >( GetSizeScalePolicy(), SizeScalePolicy::TypeTable, SizeScalePolicy::TypeTableCount );
      break;
    }

    case Dali::Actor::Property::WIDTH_FOR_HEIGHT:
    {
      value = ( GetResizePolicy( Dimension::WIDTH ) == ResizePolicy::DIMENSION_DEPENDENCY ) && ( GetDimensionDependency( Dimension::WIDTH ) == Dimension::HEIGHT );
      break;
    }

    case Dali::Actor::Property::HEIGHT_FOR_WIDTH:
    {
      value = ( GetResizePolicy( Dimension::HEIGHT ) == ResizePolicy::DIMENSION_DEPENDENCY ) && ( GetDimensionDependency( Dimension::HEIGHT ) == Dimension::WIDTH );
      break;
    }

    case Dali::Actor::Property::PADDING:
    {
      Vector2 widthPadding = GetPadding( Dimension::WIDTH );
      Vector2 heightPadding = GetPadding( Dimension::HEIGHT );
      value = Vector4( widthPadding.x, widthPadding.y, heightPadding.x, heightPadding.y );
      break;
    }

    case Dali::Actor::Property::MINIMUM_SIZE:
    {
      value = Vector2( GetMinimumSize( Dimension::WIDTH ), GetMinimumSize( Dimension::HEIGHT ) );
      break;
    }

    case Dali::Actor::Property::MAXIMUM_SIZE:
    {
      value = Vector2( GetMaximumSize( Dimension::WIDTH ), GetMaximumSize( Dimension::HEIGHT ) );
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS( false && "Actor Property index invalid" ); // should not come here
      break;
    }
  }

  return value;
}

const SceneGraph::PropertyOwner* Actor::GetPropertyOwner() const
{
  return mNode;
}

const SceneGraph::PropertyOwner* Actor::GetSceneObject() const
{
  // This method should only return an object connected to the scene-graph
  return OnStage() ? mNode : NULL;
}

const PropertyBase* Actor::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable( index ) && "Property is not animatable" );

  const PropertyBase* property( NULL );

  // This method should only return a property of an object connected to the scene-graph
  if( !OnStage() )
  {
    return property;
  }

  if ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX && index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )
  {
    AnimatablePropertyMetadata* animatable = FindAnimatableProperty( index );
    if( !animatable )
    {
      const TypeInfo* typeInfo( GetTypeInfo() );
      if ( typeInfo )
      {
        if( Property::INVALID_INDEX != RegisterSceneGraphProperty( typeInfo->GetPropertyName( index ), index, Property::Value( typeInfo->GetPropertyType( index ) ) ) )
        {
          animatable = FindAnimatableProperty( index );
        }
      }
    }
    DALI_ASSERT_ALWAYS( animatable && "Property index is invalid" );

    property = animatable->GetSceneGraphProperty();
  }
  else if ( index >= DEFAULT_PROPERTY_MAX_COUNT )
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    DALI_ASSERT_ALWAYS( custom && "Property index is invalid" );

    property = custom->GetSceneGraphProperty();
  }
  else if( NULL != mNode )
  {
    switch( index )
    {
      case Dali::Actor::Property::SIZE:
        property = &mNode->mSize;
        break;

      case Dali::Actor::Property::SIZE_WIDTH:
        property = &mNode->mSize;
        break;

      case Dali::Actor::Property::SIZE_HEIGHT:
        property = &mNode->mSize;
        break;

      case Dali::Actor::Property::SIZE_DEPTH:
        property = &mNode->mSize;
        break;

      case Dali::Actor::Property::POSITION:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::Property::POSITION_X:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::Property::POSITION_Y:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::Property::POSITION_Z:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::Property::ORIENTATION:
        property = &mNode->mOrientation;
        break;

      case Dali::Actor::Property::SCALE:
        property = &mNode->mScale;
        break;

      case Dali::Actor::Property::SCALE_X:
        property = &mNode->mScale;
        break;

      case Dali::Actor::Property::SCALE_Y:
        property = &mNode->mScale;
        break;

      case Dali::Actor::Property::SCALE_Z:
        property = &mNode->mScale;
        break;

      case Dali::Actor::Property::VISIBLE:
        property = &mNode->mVisible;
        break;

      case Dali::Actor::Property::COLOR:
        property = &mNode->mColor;
        break;

      case Dali::Actor::Property::COLOR_RED:
        property = &mNode->mColor;
        break;

      case Dali::Actor::Property::COLOR_GREEN:
        property = &mNode->mColor;
        break;

      case Dali::Actor::Property::COLOR_BLUE:
        property = &mNode->mColor;
        break;

      case Dali::Actor::Property::COLOR_ALPHA:
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
  if( !OnStage() )
  {
    return property;
  }

  if ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX && index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )
  {
    AnimatablePropertyMetadata* animatable = FindAnimatableProperty( index );
    if( !animatable )
    {
      const TypeInfo* typeInfo( GetTypeInfo() );
      if ( typeInfo )
      {
        if( Property::INVALID_INDEX != RegisterSceneGraphProperty( typeInfo->GetPropertyName( index ), index, Property::Value( typeInfo->GetPropertyType( index ) ) ) )
        {
          animatable = FindAnimatableProperty( index );
        }
      }
    }
    DALI_ASSERT_ALWAYS( animatable && "Property index is invalid" );

    property = animatable->GetSceneGraphProperty();
  }
  else if ( index >= DEFAULT_PROPERTY_MAX_COUNT )
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    DALI_ASSERT_ALWAYS( custom && "Property index is invalid" );
    property = custom->GetSceneGraphProperty();
  }
  else if( NULL != mNode )
  {
    switch( index )
    {
      case Dali::Actor::Property::PARENT_ORIGIN:
        property = &mNode->mParentOrigin;
        break;

      case Dali::Actor::Property::PARENT_ORIGIN_X:
        property = &mNode->mParentOrigin;
        break;

      case Dali::Actor::Property::PARENT_ORIGIN_Y:
        property = &mNode->mParentOrigin;
        break;

      case Dali::Actor::Property::PARENT_ORIGIN_Z:
        property = &mNode->mParentOrigin;
        break;

      case Dali::Actor::Property::ANCHOR_POINT:
        property = &mNode->mAnchorPoint;
        break;

      case Dali::Actor::Property::ANCHOR_POINT_X:
        property = &mNode->mAnchorPoint;
        break;

      case Dali::Actor::Property::ANCHOR_POINT_Y:
        property = &mNode->mAnchorPoint;
        break;

      case Dali::Actor::Property::ANCHOR_POINT_Z:
        property = &mNode->mAnchorPoint;
        break;

      case Dali::Actor::Property::SIZE:
        property = &mNode->mSize;
        break;

      case Dali::Actor::Property::SIZE_WIDTH:
        property = &mNode->mSize;
        break;

      case Dali::Actor::Property::SIZE_HEIGHT:
        property = &mNode->mSize;
        break;

      case Dali::Actor::Property::SIZE_DEPTH:
        property = &mNode->mSize;
        break;

      case Dali::Actor::Property::POSITION:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::Property::POSITION_X:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::Property::POSITION_Y:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::Property::POSITION_Z:
        property = &mNode->mPosition;
        break;

      case Dali::Actor::Property::WORLD_POSITION:
        property = &mNode->mWorldPosition;
        break;

      case Dali::Actor::Property::WORLD_POSITION_X:
        property = &mNode->mWorldPosition;
        break;

      case Dali::Actor::Property::WORLD_POSITION_Y:
        property = &mNode->mWorldPosition;
        break;

      case Dali::Actor::Property::WORLD_POSITION_Z:
        property = &mNode->mWorldPosition;
        break;

      case Dali::Actor::Property::ORIENTATION:
        property = &mNode->mOrientation;
        break;

      case Dali::Actor::Property::WORLD_ORIENTATION:
        property = &mNode->mWorldOrientation;
        break;

      case Dali::Actor::Property::SCALE:
        property = &mNode->mScale;
        break;

      case Dali::Actor::Property::SCALE_X:
        property = &mNode->mScale;
        break;

      case Dali::Actor::Property::SCALE_Y:
        property = &mNode->mScale;
        break;

      case Dali::Actor::Property::SCALE_Z:
        property = &mNode->mScale;
        break;

      case Dali::Actor::Property::WORLD_SCALE:
        property = &mNode->mWorldScale;
        break;

      case Dali::Actor::Property::VISIBLE:
        property = &mNode->mVisible;
        break;

      case Dali::Actor::Property::COLOR:
        property = &mNode->mColor;
        break;

      case Dali::Actor::Property::COLOR_RED:
        property = &mNode->mColor;
        break;

      case Dali::Actor::Property::COLOR_GREEN:
        property = &mNode->mColor;
        break;

      case Dali::Actor::Property::COLOR_BLUE:
        property = &mNode->mColor;
        break;

      case Dali::Actor::Property::COLOR_ALPHA:
        property = &mNode->mColor;
        break;

      case Dali::Actor::Property::WORLD_COLOR:
        property = &mNode->mWorldColor;
        break;

      case Dali::Actor::Property::WORLD_MATRIX:
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

  switch( index )
  {
    case Dali::Actor::Property::PARENT_ORIGIN_X:
    case Dali::Actor::Property::ANCHOR_POINT_X:
    case Dali::Actor::Property::SIZE_WIDTH:
    case Dali::Actor::Property::POSITION_X:
    case Dali::Actor::Property::WORLD_POSITION_X:
    case Dali::Actor::Property::SCALE_X:
    case Dali::Actor::Property::COLOR_RED:
    {
      componentIndex = 0;
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Y:
    case Dali::Actor::Property::ANCHOR_POINT_Y:
    case Dali::Actor::Property::SIZE_HEIGHT:
    case Dali::Actor::Property::POSITION_Y:
    case Dali::Actor::Property::WORLD_POSITION_Y:
    case Dali::Actor::Property::SCALE_Y:
    case Dali::Actor::Property::COLOR_GREEN:
    {
      componentIndex = 1;
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Z:
    case Dali::Actor::Property::ANCHOR_POINT_Z:
    case Dali::Actor::Property::SIZE_DEPTH:
    case Dali::Actor::Property::POSITION_Z:
    case Dali::Actor::Property::WORLD_POSITION_Z:
    case Dali::Actor::Property::SCALE_Z:
    case Dali::Actor::Property::COLOR_BLUE:
    {
      componentIndex = 2;
      break;
    }

    case Dali::Actor::Property::COLOR_ALPHA:
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

void Actor::SetParent( Actor* parent, int index )
{
  if( parent )
  {
    DALI_ASSERT_ALWAYS( !mParent && "Actor cannot have 2 parents" );

    mParent = parent;

    if ( EventThreadServices::IsCoreRunning() && // Don't emit signals or send messages during Core destruction
         parent->OnStage() )
    {
      // Instruct each actor to create a corresponding node in the scene graph
      ConnectToStage( index );
    }
  }
  else // parent being set to NULL
  {
    DALI_ASSERT_ALWAYS( mParent != NULL && "Actor should have a parent" );

    mParent = NULL;

    if ( EventThreadServices::IsCoreRunning() && // Don't emit signals or send messages during Core destruction
         OnStage() )
    {
      DALI_ASSERT_ALWAYS( mNode != NULL );

      if( NULL != mNode )
      {
        // Disconnect the Node & its children from the scene-graph.
        DisconnectNodeMessage( GetEventThreadServices().GetUpdateManager(), *mNode );
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

bool Actor::DoAction( BaseObject* object, const std::string& actionName, const std::vector< Property::Value >& attributes )
{
  bool done = false;
  Actor* actor = dynamic_cast< Actor* >( object );

  if( actor )
  {
    if( 0 == strcmp( actionName.c_str(), ACTION_SHOW ) ) // dont want to convert char* to string
    {
      actor->SetVisible( true );
      done = true;
    }
    else if( 0 == strcmp( actionName.c_str(), ACTION_HIDE ) )
    {
      actor->SetVisible( false );
      done = true;
    }
  }

  return done;
}

void Actor::EnsureRelayoutData() const
{
  // Assign relayout data.
  if( !mRelayoutData )
  {
    mRelayoutData = new RelayoutData();
  }
}

bool Actor::RelayoutDependentOnParent( Dimension::Type dimension )
{
  // Check if actor is dependent on parent
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( ( dimension & ( 1 << i ) ) )
    {
      const ResizePolicy::Type resizePolicy = GetResizePolicy( static_cast< Dimension::Type >( 1 << i ) );
      if( resizePolicy == ResizePolicy::FILL_TO_PARENT || resizePolicy == ResizePolicy::SIZE_RELATIVE_TO_PARENT || resizePolicy == ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT )
      {
        return true;
      }
    }
  }

  return false;
}

bool Actor::RelayoutDependentOnChildren( Dimension::Type dimension )
{
  // Check if actor is dependent on children
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( ( dimension & ( 1 << i ) ) )
    {
      const ResizePolicy::Type resizePolicy = GetResizePolicy( static_cast< Dimension::Type >( 1 << i ) );
      switch( resizePolicy )
      {
        case ResizePolicy::FIT_TO_CHILDREN:
        case ResizePolicy::USE_NATURAL_SIZE:      // i.e. For things that calculate their size based on children
        {
          return true;
        }

        default:
        {
          break;
        }
      }
    }
  }

  return false;
}

bool Actor::RelayoutDependentOnChildrenBase( Dimension::Type dimension )
{
  return Actor::RelayoutDependentOnChildren( dimension );
}

bool Actor::RelayoutDependentOnDimension( Dimension::Type dimension, Dimension::Type dependentDimension )
{
  // Check each possible dimension and see if it is dependent on the input one
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      return mRelayoutData->resizePolicies[ i ] == ResizePolicy::DIMENSION_DEPENDENCY && mRelayoutData->dimensionDependencies[ i ] == dependentDimension;
    }
  }

  return false;
}

void Actor::SetNegotiatedDimension( float negotiatedDimension, Dimension::Type dimension )
{
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      mRelayoutData->negotiatedDimensions[ i ] = negotiatedDimension;
    }
  }
}

float Actor::GetNegotiatedDimension( Dimension::Type dimension ) const
{
  // If more than one dimension is requested, just return the first one found
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( ( dimension & ( 1 << i ) ) )
    {
      return mRelayoutData->negotiatedDimensions[ i ];
    }
  }

  return 0.0f;   // Default
}

void Actor::SetPadding( const Vector2& padding, Dimension::Type dimension )
{
  EnsureRelayoutData();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      mRelayoutData->dimensionPadding[ i ] = padding;
    }
  }
}

Vector2 Actor::GetPadding( Dimension::Type dimension ) const
{
  EnsureRelayoutData();

  // If more than one dimension is requested, just return the first one found
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( ( dimension & ( 1 << i ) ) )
    {
      return mRelayoutData->dimensionPadding[ i ];
    }
  }

  return Vector2( 0.0f, 0.0f );   // Default
}

void Actor::SetLayoutNegotiated( bool negotiated, Dimension::Type dimension )
{
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      mRelayoutData->dimensionNegotiated[ i ] = negotiated;
    }
  }
}

bool Actor::IsLayoutNegotiated( Dimension::Type dimension ) const
{
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( ( dimension & ( 1 << i ) ) && mRelayoutData->dimensionNegotiated[ i ] )
    {
      return true;
    }
  }

  return false;
}

float Actor::CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension )
{
  // Could be overridden in derived classes.
  return CalculateChildSizeBase( child, dimension );
}

float Actor::CalculateChildSizeBase( const Dali::Actor& child, Dimension::Type dimension )
{
  // Fill to parent, taking size mode factor into account
  switch( child.GetResizePolicy( dimension ) )
  {
    case ResizePolicy::FILL_TO_PARENT:
    {
      return GetLatestSize( dimension );
    }

    case ResizePolicy::SIZE_RELATIVE_TO_PARENT:
    {
      return GetLatestSize( dimension ) * GetDimensionValue( child.GetSizeModeFactor(), dimension );
    }

    case ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT:
    {
      return GetLatestSize( dimension ) + GetDimensionValue( child.GetSizeModeFactor(), dimension );
    }

    default:
    {
      return GetLatestSize( dimension );
    }
  }
}

float Actor::GetHeightForWidth( float width )
{
  // Could be overridden in derived classes.
  float height = 0.0f;

  const Vector3 naturalSize = GetNaturalSize();
  if( naturalSize.width > 0.0f )
  {
    height = naturalSize.height * width / naturalSize.width;
  }

  return height;
}

float Actor::GetWidthForHeight( float height )
{
  // Could be overridden in derived classes.
  float width = 0.0f;

  const Vector3 naturalSize = GetNaturalSize();
  if( naturalSize.height > 0.0f )
  {
    width = naturalSize.width * height / naturalSize.height;
  }

  return width;
}

float Actor::GetLatestSize( Dimension::Type dimension ) const
{
  return IsLayoutNegotiated( dimension ) ? GetNegotiatedDimension( dimension ) : GetSize( dimension );
}

float Actor::GetRelayoutSize( Dimension::Type dimension ) const
{
  Vector2 padding = GetPadding( dimension );

  return GetLatestSize( dimension ) + padding.x + padding.y;
}

float Actor::NegotiateFromParent( Dimension::Type dimension )
{
  Actor* parent = GetParent();
  if( parent )
  {
    Vector2 padding( GetPadding( dimension ) );
    Vector2 parentPadding( parent->GetPadding( dimension ) );
    return parent->CalculateChildSize( Dali::Actor( this ), dimension ) - parentPadding.x - parentPadding.y - padding.x - padding.y;
  }

  return 0.0f;
}

float Actor::NegotiateFromChildren( Dimension::Type dimension )
{
  float maxDimensionPoint = 0.0f;

  for( unsigned int i = 0, count = GetChildCount(); i < count; ++i )
  {
    Dali::Actor child = GetChildAt( i );
    Actor& childImpl = GetImplementation( child );

    if( !childImpl.RelayoutDependentOnParent( dimension ) )
    {
      // Calculate the min and max points that the children range across
      float childPosition = GetDimensionValue( childImpl.GetTargetPosition(), dimension );
      float dimensionSize = childImpl.GetRelayoutSize( dimension );
      maxDimensionPoint = std::max( maxDimensionPoint, childPosition + dimensionSize );
    }
  }

  return maxDimensionPoint;
}

float Actor::GetSize( Dimension::Type dimension ) const
{
  return GetDimensionValue( GetTargetSize(), dimension );
}

float Actor::GetNaturalSize( Dimension::Type dimension ) const
{
  return GetDimensionValue( GetNaturalSize(), dimension );
}

float Actor::CalculateSize( Dimension::Type dimension, const Vector2& maximumSize )
{
  switch( GetResizePolicy( dimension ) )
  {
    case ResizePolicy::USE_NATURAL_SIZE:
    {
      return GetNaturalSize( dimension );
    }

    case ResizePolicy::FIXED:
    {
      return GetDimensionValue( GetPreferredSize(), dimension );
    }

    case ResizePolicy::USE_ASSIGNED_SIZE:
    {
      return GetDimensionValue( maximumSize, dimension );
    }

    case ResizePolicy::FILL_TO_PARENT:
    case ResizePolicy::SIZE_RELATIVE_TO_PARENT:
    case ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT:
    {
      return NegotiateFromParent( dimension );
    }

    case ResizePolicy::FIT_TO_CHILDREN:
    {
      return NegotiateFromChildren( dimension );
    }

    case ResizePolicy::DIMENSION_DEPENDENCY:
    {
      const Dimension::Type dimensionDependency = GetDimensionDependency( dimension );

      // Custom rules
      if( dimension == Dimension::WIDTH && dimensionDependency == Dimension::HEIGHT )
      {
        return GetWidthForHeight( GetNegotiatedDimension( Dimension::HEIGHT ) );
      }

      if( dimension == Dimension::HEIGHT && dimensionDependency == Dimension::WIDTH )
      {
        return GetHeightForWidth( GetNegotiatedDimension( Dimension::WIDTH ) );
      }

      break;
    }

    default:
    {
      break;
    }
  }

  return 0.0f;  // Default
}

float Actor::ClampDimension( float size, Dimension::Type dimension )
{
  const float minSize = GetMinimumSize( dimension );
  const float maxSize = GetMaximumSize( dimension );

  return std::max( minSize, std::min( size, maxSize ) );
}

void Actor::NegotiateDimension( Dimension::Type dimension, const Vector2& allocatedSize, ActorDimensionStack& recursionStack )
{
  // Check if it needs to be negotiated
  if( IsLayoutDirty( dimension ) && !IsLayoutNegotiated( dimension ) )
  {
    // Check that we havn't gotten into an infinite loop
    ActorDimensionPair searchActor = ActorDimensionPair( this, dimension );
    bool recursionFound = false;
    for( ActorDimensionStack::iterator it = recursionStack.begin(), itEnd = recursionStack.end(); it != itEnd; ++it )
    {
      if( *it == searchActor )
      {
        recursionFound = true;
        break;
      }
    }

    if( !recursionFound )
    {
      // Record the path that we have taken
      recursionStack.push_back( ActorDimensionPair( this, dimension ) );

      // Dimension dependency check
      for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
      {
        Dimension::Type dimensionToCheck = static_cast< Dimension::Type >( 1 << i );

        if( RelayoutDependentOnDimension( dimension, dimensionToCheck ) )
        {
          NegotiateDimension( dimensionToCheck, allocatedSize, recursionStack );
        }
      }

      // Parent dependency check
      Actor* parent = GetParent();
      if( parent && RelayoutDependentOnParent( dimension ) )
      {
        parent->NegotiateDimension( dimension, allocatedSize, recursionStack );
      }

      // Children dependency check
      if( RelayoutDependentOnChildren( dimension ) )
      {
        for( unsigned int i = 0, count = GetChildCount(); i < count; ++i )
        {
          Dali::Actor child = GetChildAt( i );
          Actor& childImpl = GetImplementation( child );

          // Only relayout child first if it is not dependent on this actor
          if( !childImpl.RelayoutDependentOnParent( dimension ) )
          {
            childImpl.NegotiateDimension( dimension, allocatedSize, recursionStack );
          }
        }
      }

      // For deriving classes
      OnCalculateRelayoutSize( dimension );

      // All dependencies checked, calculate the size and set negotiated flag
      const float newSize = ClampDimension( CalculateSize( dimension, allocatedSize ), dimension );

      SetNegotiatedDimension( newSize, dimension );
      SetLayoutNegotiated( true, dimension );

      // For deriving classes
      OnLayoutNegotiated( newSize, dimension );

      // This actor has been successfully processed, pop it off the recursion stack
      recursionStack.pop_back();
    }
    else
    {
      // TODO: Break infinite loop
      SetLayoutNegotiated( true, dimension );
    }
  }
}

void Actor::NegotiateDimensions( const Vector2& allocatedSize )
{
  // Negotiate all dimensions that require it
  ActorDimensionStack recursionStack;

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    const Dimension::Type dimension = static_cast< Dimension::Type >( 1 << i );

    // Negotiate
    NegotiateDimension( dimension, allocatedSize, recursionStack );
  }
}

Vector2 Actor::ApplySizeSetPolicy( const Vector2 size )
{
  switch( mRelayoutData->sizeSetPolicy )
  {
    case SizeScalePolicy::USE_SIZE_SET:
    {
      return size;
    }

    case SizeScalePolicy::FIT_WITH_ASPECT_RATIO:
    {
      // Scale size to fit within the original size bounds, keeping the natural size aspect ratio
      const Vector3 naturalSize = GetNaturalSize();
      if( naturalSize.width > 0.0f && naturalSize.height > 0.0f && size.width > 0.0f && size.height > 0.0f )
      {
        const float sizeRatio = size.width / size.height;
        const float naturalSizeRatio = naturalSize.width / naturalSize.height;

        if( naturalSizeRatio < sizeRatio )
        {
          return Vector2( naturalSizeRatio * size.height, size.height );
        }
        else if( naturalSizeRatio > sizeRatio )
        {
          return Vector2( size.width, size.width / naturalSizeRatio );
        }
        else
        {
          return size;
        }
      }

      break;
    }

    case SizeScalePolicy::FILL_WITH_ASPECT_RATIO:
    {
      // Scale size to fill the original size bounds, keeping the natural size aspect ratio. Potentially exceeding the original bounds.
      const Vector3 naturalSize = GetNaturalSize();
      if( naturalSize.width > 0.0f && naturalSize.height > 0.0f && size.width > 0.0f && size.height > 0.0f )
      {
        const float sizeRatio = size.width / size.height;
        const float naturalSizeRatio = naturalSize.width / naturalSize.height;

        if( naturalSizeRatio < sizeRatio )
        {
          return Vector2( size.width, size.width / naturalSizeRatio );
        }
        else if( naturalSizeRatio > sizeRatio )
        {
          return Vector2( naturalSizeRatio * size.height, size.height );
        }
        else
        {
          return size;
        }
      }
    }

    default:
    {
      break;
    }
  }

  return size;
}

void Actor::SetNegotiatedSize( RelayoutContainer& container )
{
  // Do the set actor size
  Vector2 negotiatedSize( GetLatestSize( Dimension::WIDTH ), GetLatestSize( Dimension::HEIGHT ) );

  // Adjust for size set policy
  negotiatedSize = ApplySizeSetPolicy( negotiatedSize );

  // Lock the flag to stop recursive relayouts on set size
  mRelayoutData->insideRelayout = true;
  SetSize( negotiatedSize );
  mRelayoutData->insideRelayout = false;

  // Clear flags for all dimensions
  SetLayoutDirty( false );

  // Give deriving classes a chance to respond
  OnRelayout( negotiatedSize, container );

  if( !mOnRelayoutSignal.Empty() )
  {
    Dali::Actor handle( this );
    mOnRelayoutSignal.Emit( handle );
  }
}

void Actor::NegotiateSize( const Vector2& allocatedSize, RelayoutContainer& container )
{
  // Do the negotiation
  NegotiateDimensions( allocatedSize );

  // Set the actor size
  SetNegotiatedSize( container );

  // Negotiate down to children
  const Vector2 newBounds = GetTargetSize().GetVectorXY();

  for( unsigned int i = 0, count = GetChildCount(); i < count; ++i )
  {
    Dali::Actor child = GetChildAt( i );

    // Only relayout if required
    if( GetImplementation( child ).RelayoutRequired() )
    {
      container.Add( child, newBounds );
    }
  }
}

void Actor::RelayoutRequest( Dimension::Type dimension )
{
  Internal::RelayoutController* relayoutController = Internal::RelayoutController::Get();
  if( relayoutController )
  {
    Dali::Actor self( this );
    relayoutController->RequestRelayout( self, dimension );
  }
}

void Actor::PropagateRelayoutFlags()
{
  Internal::RelayoutController* relayoutController = Internal::RelayoutController::Get();
  if( relayoutController )
  {
    Dali::Actor self( this );
    relayoutController->PropagateFlags( self );
  }
}

void Actor::OnCalculateRelayoutSize( Dimension::Type dimension )
{
}

void Actor::OnLayoutNegotiated( float size, Dimension::Type dimension )
{
}

void Actor::SetPreferredSize( const Vector2& size )
{
  EnsureRelayoutData();

  if( size.width > 0.0f )
  {
    SetResizePolicy( ResizePolicy::FIXED, Dimension::WIDTH );
  }

  if( size.height > 0.0f )
  {
    SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
  }

  mRelayoutData->preferredSize = size;

  RelayoutRequest();
}

Vector2 Actor::GetPreferredSize() const
{
  EnsureRelayoutData();

  return mRelayoutData->preferredSize;
}

void Actor::SetMinimumSize( float size, Dimension::Type dimension )
{
  EnsureRelayoutData();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      mRelayoutData->minimumSize[ i ] = size;
    }
  }

  RelayoutRequest();
}

float Actor::GetMinimumSize( Dimension::Type dimension ) const
{
  EnsureRelayoutData();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      return mRelayoutData->minimumSize[ i ];
    }
  }

  return 0.0f;  // Default
}

void Actor::SetMaximumSize( float size, Dimension::Type dimension )
{
  EnsureRelayoutData();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      mRelayoutData->maximumSize[ i ] = size;
    }
  }

  RelayoutRequest();
}

float Actor::GetMaximumSize( Dimension::Type dimension ) const
{
  EnsureRelayoutData();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      return mRelayoutData->maximumSize[ i ];
    }
  }

  return 0.0f;  // Default
}

} // namespace Internal

} // namespace Dali
