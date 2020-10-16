/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/devel-api/actors/layer-devel.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/internal/event/actors/actor-property-handler.h>
#include <dali/internal/event/actors/actor-relayouter.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/type-info-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>
#include <dali/internal/update/nodes/node-messages.h>
#include <dali/internal/event/events/actor-gesture-data.h>
#include <dali/integration-api/debug.h>

using Dali::Internal::SceneGraph::Node;
using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::PropertyBase;

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_DEPTH_TIMER" );
Debug::Filter* gLogRelayoutFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RELAYOUT_TIMER" );
#endif

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{

// Properties

/**
 * We want to discourage the use of property strings (minimize string comparisons),
 * particularly for the default properties.
 *              Name                  Type   writable animatable constraint-input  enum for index-checking
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "parentOrigin",              VECTOR3,  true,  false, true,  Dali::Actor::Property::PARENT_ORIGIN )
DALI_PROPERTY( "parentOriginX",             FLOAT,    true,  false, true,  Dali::Actor::Property::PARENT_ORIGIN_X )
DALI_PROPERTY( "parentOriginY",             FLOAT,    true,  false, true,  Dali::Actor::Property::PARENT_ORIGIN_Y )
DALI_PROPERTY( "parentOriginZ",             FLOAT,    true,  false, true,  Dali::Actor::Property::PARENT_ORIGIN_Z )
DALI_PROPERTY( "anchorPoint",               VECTOR3,  true,  false, true,  Dali::Actor::Property::ANCHOR_POINT )
DALI_PROPERTY( "anchorPointX",              FLOAT,    true,  false, true,  Dali::Actor::Property::ANCHOR_POINT_X )
DALI_PROPERTY( "anchorPointY",              FLOAT,    true,  false, true,  Dali::Actor::Property::ANCHOR_POINT_Y )
DALI_PROPERTY( "anchorPointZ",              FLOAT,    true,  false, true,  Dali::Actor::Property::ANCHOR_POINT_Z )
DALI_PROPERTY( "size",                      VECTOR3,  true,  true,  true,  Dali::Actor::Property::SIZE )
DALI_PROPERTY( "sizeWidth",                 FLOAT,    true,  true,  true,  Dali::Actor::Property::SIZE_WIDTH )
DALI_PROPERTY( "sizeHeight",                FLOAT,    true,  true,  true,  Dali::Actor::Property::SIZE_HEIGHT )
DALI_PROPERTY( "sizeDepth",                 FLOAT,    true,  true,  true,  Dali::Actor::Property::SIZE_DEPTH )
DALI_PROPERTY( "position",                  VECTOR3,  true,  true,  true,  Dali::Actor::Property::POSITION )
DALI_PROPERTY( "positionX",                 FLOAT,    true,  true,  true,  Dali::Actor::Property::POSITION_X )
DALI_PROPERTY( "positionY",                 FLOAT,    true,  true,  true,  Dali::Actor::Property::POSITION_Y )
DALI_PROPERTY( "positionZ",                 FLOAT,    true,  true,  true,  Dali::Actor::Property::POSITION_Z )
DALI_PROPERTY( "worldPosition",             VECTOR3,  false, false, true,  Dali::Actor::Property::WORLD_POSITION )
DALI_PROPERTY( "worldPositionX",            FLOAT,    false, false, true,  Dali::Actor::Property::WORLD_POSITION_X )
DALI_PROPERTY( "worldPositionY",            FLOAT,    false, false, true,  Dali::Actor::Property::WORLD_POSITION_Y )
DALI_PROPERTY( "worldPositionZ",            FLOAT,    false, false, true,  Dali::Actor::Property::WORLD_POSITION_Z )
DALI_PROPERTY( "orientation",               ROTATION, true,  true,  true,  Dali::Actor::Property::ORIENTATION )
DALI_PROPERTY( "worldOrientation",          ROTATION, false, false, true,  Dali::Actor::Property::WORLD_ORIENTATION )
DALI_PROPERTY( "scale",                     VECTOR3,  true,  true,  true,  Dali::Actor::Property::SCALE )
DALI_PROPERTY( "scaleX",                    FLOAT,    true,  true,  true,  Dali::Actor::Property::SCALE_X )
DALI_PROPERTY( "scaleY",                    FLOAT,    true,  true,  true,  Dali::Actor::Property::SCALE_Y )
DALI_PROPERTY( "scaleZ",                    FLOAT,    true,  true,  true,  Dali::Actor::Property::SCALE_Z )
DALI_PROPERTY( "worldScale",                VECTOR3,  false, false, true,  Dali::Actor::Property::WORLD_SCALE )
DALI_PROPERTY( "visible",                   BOOLEAN,  true,  true,  true,  Dali::Actor::Property::VISIBLE )
DALI_PROPERTY( "color",                     VECTOR4,  true,  true,  true,  Dali::Actor::Property::COLOR )
DALI_PROPERTY( "colorRed",                  FLOAT,    true,  true,  true,  Dali::Actor::Property::COLOR_RED )
DALI_PROPERTY( "colorGreen",                FLOAT,    true,  true,  true,  Dali::Actor::Property::COLOR_GREEN )
DALI_PROPERTY( "colorBlue",                 FLOAT,    true,  true,  true,  Dali::Actor::Property::COLOR_BLUE )
DALI_PROPERTY( "colorAlpha",                FLOAT,    true,  true,  true,  Dali::Actor::Property::COLOR_ALPHA )
DALI_PROPERTY( "worldColor",                VECTOR4,  false, false, true,  Dali::Actor::Property::WORLD_COLOR )
DALI_PROPERTY( "worldMatrix",               MATRIX,   false, false, true,  Dali::Actor::Property::WORLD_MATRIX )
DALI_PROPERTY( "name",                      STRING,   true,  false, false, Dali::Actor::Property::NAME )
DALI_PROPERTY( "sensitive",                 BOOLEAN,  true,  false, false, Dali::Actor::Property::SENSITIVE )
DALI_PROPERTY( "leaveRequired",             BOOLEAN,  true,  false, false, Dali::Actor::Property::LEAVE_REQUIRED )
DALI_PROPERTY( "inheritOrientation",        BOOLEAN,  true,  false, false, Dali::Actor::Property::INHERIT_ORIENTATION )
DALI_PROPERTY( "inheritScale",              BOOLEAN,  true,  false, false, Dali::Actor::Property::INHERIT_SCALE )
DALI_PROPERTY( "colorMode",                 INTEGER,  true,  false, false, Dali::Actor::Property::COLOR_MODE )
DALI_PROPERTY( "drawMode",                  INTEGER,  true,  false, false, Dali::Actor::Property::DRAW_MODE )
DALI_PROPERTY( "sizeModeFactor",            VECTOR3,  true,  false, false, Dali::Actor::Property::SIZE_MODE_FACTOR )
DALI_PROPERTY( "widthResizePolicy",         STRING,   true,  false, false, Dali::Actor::Property::WIDTH_RESIZE_POLICY )
DALI_PROPERTY( "heightResizePolicy",        STRING,   true,  false, false, Dali::Actor::Property::HEIGHT_RESIZE_POLICY )
DALI_PROPERTY( "sizeScalePolicy",           INTEGER,  true,  false, false, Dali::Actor::Property::SIZE_SCALE_POLICY )
DALI_PROPERTY( "widthForHeight",            BOOLEAN,  true,  false, false, Dali::Actor::Property::WIDTH_FOR_HEIGHT )
DALI_PROPERTY( "heightForWidth",            BOOLEAN,  true,  false, false, Dali::Actor::Property::HEIGHT_FOR_WIDTH )
DALI_PROPERTY( "padding",                   VECTOR4,  true,  false, false, Dali::Actor::Property::PADDING )
DALI_PROPERTY( "minimumSize",               VECTOR2,  true,  false, false, Dali::Actor::Property::MINIMUM_SIZE )
DALI_PROPERTY( "maximumSize",               VECTOR2,  true,  false, false, Dali::Actor::Property::MAXIMUM_SIZE )
DALI_PROPERTY( "inheritPosition",           BOOLEAN,  true,  false, false, Dali::Actor::Property::INHERIT_POSITION )
DALI_PROPERTY( "clippingMode",              STRING,   true,  false, false, Dali::Actor::Property::CLIPPING_MODE )
DALI_PROPERTY( "layoutDirection",           STRING,   true,  false, false, Dali::Actor::Property::LAYOUT_DIRECTION )
DALI_PROPERTY( "inheritLayoutDirection",    BOOLEAN,  true,  false, false, Dali::Actor::Property::INHERIT_LAYOUT_DIRECTION )
DALI_PROPERTY( "opacity",                   FLOAT,    true,  true,  true,  Dali::Actor::Property::OPACITY )
DALI_PROPERTY( "screenPosition",            VECTOR2,  false, false, false, Dali::Actor::Property::SCREEN_POSITION )
DALI_PROPERTY( "positionUsesAnchorPoint",   BOOLEAN,  true,  false, false, Dali::Actor::Property::POSITION_USES_ANCHOR_POINT )
DALI_PROPERTY( "culled",                    BOOLEAN,  false, false, true,  Dali::Actor::Property::CULLED )
DALI_PROPERTY( "id",                        INTEGER,  false, false, false, Dali::Actor::Property::ID )
DALI_PROPERTY( "hierarchyDepth",            INTEGER,  false, false, false, Dali::Actor::Property::HIERARCHY_DEPTH )
DALI_PROPERTY( "isRoot",                    BOOLEAN,  false, false, false, Dali::Actor::Property::IS_ROOT )
DALI_PROPERTY( "isLayer",                   BOOLEAN,  false, false, false, Dali::Actor::Property::IS_LAYER )
DALI_PROPERTY( "connectedToScene",          BOOLEAN,  false, false, false, Dali::Actor::Property::CONNECTED_TO_SCENE )
DALI_PROPERTY( "keyboardFocusable",         BOOLEAN,  true,  false, false, Dali::Actor::Property::KEYBOARD_FOCUSABLE )
DALI_PROPERTY( "siblingOrder",              INTEGER,  true,  false, false, Dali::DevelActor::Property::SIBLING_ORDER )
DALI_PROPERTY( "updateSizeHint",            VECTOR2,  true,  false, false, Dali::DevelActor::Property::UPDATE_SIZE_HINT )
DALI_PROPERTY( "captureAllTouchAfterStart", BOOLEAN,  true,  false, false, Dali::DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX, ActorDefaultProperties )

// Signals

const char* const SIGNAL_HOVERED = "hovered";
const char* const SIGNAL_WHEEL_EVENT = "wheelEvent";
const char* const SIGNAL_ON_SCENE = "onScene";
const char* const SIGNAL_OFF_SCENE = "offScene";
const char* const SIGNAL_ON_RELAYOUT = "onRelayout";
const char* const SIGNAL_TOUCHED = "touched";
const char* const SIGNAL_VISIBILITY_CHANGED = "visibilityChanged";
const char* const SIGNAL_LAYOUT_DIRECTION_CHANGED = "layoutDirectionChanged";
const char* const SIGNAL_CHILD_ADDED = "childAdded";
const char* const SIGNAL_CHILD_REMOVED = "childRemoved";

// Actions

const char* const ACTION_SHOW = "show";
const char* const ACTION_HIDE = "hide";

BaseHandle CreateActor()
{
  return Dali::Actor::New();
}

TypeRegistration mType( typeid(Dali::Actor), typeid(Dali::Handle), CreateActor, ActorDefaultProperties );

SignalConnectorType signalConnector2( mType, SIGNAL_HOVERED, &Actor::DoConnectSignal );
SignalConnectorType signalConnector3( mType, SIGNAL_WHEEL_EVENT, &Actor::DoConnectSignal );
SignalConnectorType signalConnector4( mType, SIGNAL_ON_SCENE, &Actor::DoConnectSignal );
SignalConnectorType signalConnector5( mType, SIGNAL_OFF_SCENE, &Actor::DoConnectSignal );
SignalConnectorType signalConnector6( mType, SIGNAL_ON_RELAYOUT, &Actor::DoConnectSignal );
SignalConnectorType signalConnector7( mType, SIGNAL_TOUCHED, &Actor::DoConnectSignal );
SignalConnectorType signalConnector8( mType, SIGNAL_VISIBILITY_CHANGED, &Actor::DoConnectSignal );
SignalConnectorType signalConnector9( mType, SIGNAL_LAYOUT_DIRECTION_CHANGED, &Actor::DoConnectSignal );
SignalConnectorType signalConnector10( mType, SIGNAL_CHILD_ADDED, &Actor::DoConnectSignal );
SignalConnectorType signalConnector11( mType, SIGNAL_CHILD_REMOVED, &Actor::DoConnectSignal );

TypeAction a1( mType, ACTION_SHOW, &Actor::DoAction );
TypeAction a2( mType, ACTION_HIDE, &Actor::DoAction );

/**
 * @brief Extract a given dimension from a Vector2
 *
 * @param[in] values The values to extract from
 * @param[in] dimension The dimension to extract
 * @return Return the value for the dimension
 */
constexpr float GetDimensionValue( const Vector2& values, Dimension::Type dimension )
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

/**
 * @brief Recursively emits the visibility-changed-signal on the actor tree.
 * @param[in] actor The actor to emit the signal on
 * @param[in] visible The new visibility of the actor
 * @param[in] type Whether the actor's visible property has changed or a parent's
 */
void EmitVisibilityChangedSignalRecursively( ActorPtr actor, bool visible, DevelActor::VisibilityChange::Type type )
{
  if( actor )
  {
    actor->EmitVisibilityChangedSignal( visible, type );

    if( actor->GetChildCount() > 0 )
    {
      for( ActorPtr& child : actor->GetChildrenInternal() )
      {
        EmitVisibilityChangedSignalRecursively( child, visible, DevelActor::VisibilityChange::PARENT );
      }
    }
  }
}

/// Helper for emitting a signal
template<typename Signal, typename Event>
bool EmitConsumingSignal( Actor& actor, Signal& signal, const Event& event )
{
  bool consumed = false;

  if( !signal.Empty() )
  {
    Dali::Actor handle( &actor );
    consumed = signal.Emit( handle, event );
  }

  return consumed;
}

/// Helper for emitting signals with multiple parameters
template<typename Signal, typename... Param>
void EmitSignal( Actor& actor, Signal& signal, Param... params)
{
  if( !signal.Empty() )
  {
    Dali::Actor handle( &actor );
    signal.Emit( handle, params... );
  }
}

bool ScreenToLocalInternal(
    const Matrix& viewMatrix,
    const Matrix& projectionMatrix,
    const Matrix& worldMatrix,
    const Viewport& viewport,
    const Vector3& currentSize,
    float& localX,
    float& localY,
    float screenX,
    float screenY )
{
  // Get the ModelView matrix
  Matrix modelView;
  Matrix::Multiply( modelView, worldMatrix, viewMatrix );

  // Calculate the inverted ModelViewProjection matrix; this will be used for 2 unprojects
  Matrix invertedMvp( false/*don't init*/);
  Matrix::Multiply( invertedMvp, modelView, projectionMatrix );
  bool success = invertedMvp.Invert();

  // Convert to GL coordinates
  Vector4 screenPos( screenX - static_cast<float>( viewport.x ), static_cast<float>( viewport.height ) - screenY - static_cast<float>( viewport.y ), 0.f, 1.f );

  Vector4 nearPos;
  if( success )
  {
    success = Unproject( screenPos, invertedMvp, static_cast<float>( viewport.width ), static_cast<float>( viewport.height ), nearPos );
  }

  Vector4 farPos;
  if( success )
  {
    screenPos.z = 1.0f;
    success = Unproject( screenPos, invertedMvp, static_cast<float>( viewport.width ), static_cast<float>( viewport.height ), farPos );
  }

  if( success )
  {
    Vector4 local;
    if( XyPlaneIntersect( nearPos, farPos, local ) )
    {
      Vector3 size = currentSize;
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

} // unnamed namespace

ActorPtr Actor::New()
{
  // pass a reference to actor, actor does not own its node
  ActorPtr actor( new Actor( BASIC, *CreateNode() ) );

  // Second-phase construction
  actor->Initialize();

  return actor;
}

const SceneGraph::Node* Actor::CreateNode()
{
  // create node. Nodes are owned by the update manager
  SceneGraph::Node* node = SceneGraph::Node::New();
  OwnerPointer< SceneGraph::Node > transferOwnership( node );
  Internal::ThreadLocalStorage* tls = Internal::ThreadLocalStorage::GetInternal();

  DALI_ASSERT_ALWAYS( tls && "ThreadLocalStorage is null" );

  AddNodeMessage( tls->GetUpdateManager(), transferOwnership );

  return node;
}

void Actor::SetName( const std::string& name )
{
  mName = name;

  // ATTENTION: string for debug purposes is not thread safe.
  DALI_LOG_SET_OBJECT_STRING( const_cast< SceneGraph::Node* >( &GetNode() ), name );
}

uint32_t Actor::GetId() const
{
  return GetNode().GetId();
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
  for( Actor* parent = mParent; !layer && parent != nullptr; parent = parent->GetParent() )
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
      oldParent->Remove( child ); // This causes OnChildRemove callback & ChildRemoved signal

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
      mChildren->push_back( ActorPtr( &child ) );

      // SetParent asserts that child can be added
      child.SetParent( this );

      // Notification for derived classes
      OnChildAdd( child );
      EmitChildAddedSignal( child );

      InheritLayoutDirectionRecursively( ActorPtr( &child ), mLayoutDirection );

      // Only put in a relayout request if there is a suitable dependency
      if( RelayoutDependentOnChildren() )
      {
        RelayoutRequest();
      }
    }
  }
}

void Actor::Remove( Actor& child )
{
  if( (this == &child) || (!mChildren) )
  {
    // no children or removing itself
    return;
  }

  ActorPtr removed;

  // Find the child in mChildren, and unparent it
  ActorIter end = mChildren->end();
  for( ActorIter iter = mChildren->begin(); iter != end; ++iter )
  {
    ActorPtr actor = (*iter);

    if( actor.Get() == &child )
    {
      // Keep handle for OnChildRemove notification
      removed = actor;

      // Do this first, since user callbacks from within SetParent() may need to add the child
      mChildren->erase( iter );

      DALI_ASSERT_DEBUG( actor->GetParent() == this );
      actor->SetParent( nullptr );

      break;
    }
  }

  if( removed )
  {
    // Only put in a relayout request if there is a suitable dependency
    if( RelayoutDependentOnChildren() )
    {
      RelayoutRequest();
    }
  }

  // Notification for derived classes
  OnChildRemove( child );
  EmitChildRemovedSignal( child );
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

uint32_t Actor::GetChildCount() const
{
  return ( nullptr != mChildren ) ? static_cast<uint32_t>( mChildren->size() ) : 0; // only 4,294,967,295 children per actor
}

ActorPtr Actor::GetChildAt( uint32_t index ) const
{
  DALI_ASSERT_ALWAYS( index < GetChildCount() );

  return ( ( mChildren ) ? ( *mChildren )[ index ] : ActorPtr() );
}

ActorPtr Actor::FindChildByName( const std::string& actorName )
{
  ActorPtr child = nullptr;
  if( actorName == mName )
  {
    child = this;
  }
  else if( mChildren )
  {
    ActorIter end = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != end; ++iter )
    {
      child = (*iter)->FindChildByName( actorName );

      if( child )
      {
        break;
      }
    }
  }
  return child;
}

ActorPtr Actor::FindChildById( const uint32_t id )
{
  ActorPtr child = nullptr;
  if( id == GetId() )
  {
    child = this;
  }
  else if( mChildren )
  {
    ActorIter end = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != end; ++iter )
    {
      child = (*iter)->FindChildById( id );

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
  // node is being used in a separate thread; queue a message to set the value & base value
  SetParentOriginMessage( GetEventThreadServices(), GetNode(), origin );

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

const Vector3& Actor::GetCurrentParentOrigin() const
{
  // Cached for event-thread access
  return ( mParentOrigin ) ? *mParentOrigin : ParentOrigin::DEFAULT;
}

void Actor::SetAnchorPoint( const Vector3& anchor )
{
  // node is being used in a separate thread; queue a message to set the value & base value
  SetAnchorPointMessage( GetEventThreadServices(), GetNode(), anchor );

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

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::Bake, position );
}

void Actor::SetX( float x )
{
  mTargetPosition.x = x;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeX, x );
}

void Actor::SetY( float y )
{
  mTargetPosition.y = y;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeY, y );
}

void Actor::SetZ( float z )
{
  mTargetPosition.z = z;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeZ, z );
}

void Actor::TranslateBy( const Vector3& distance )
{
  mTargetPosition += distance;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeRelative, distance );
}

const Vector3& Actor::GetCurrentPosition() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetPosition(GetEventThreadServices().GetEventBufferIndex());
}

const Vector3& Actor::GetCurrentWorldPosition() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetWorldPosition( GetEventThreadServices().GetEventBufferIndex() );
}

const Vector2 Actor::GetCurrentScreenPosition() const
{
  if( mScene && OnScene() )
  {
    Vector3 worldPosition =  GetNode().GetWorldPosition( GetEventThreadServices().GetEventBufferIndex() );
    Vector3 cameraPosition = mScene->GetDefaultCameraActor().GetNode().GetWorldPosition( GetEventThreadServices().GetEventBufferIndex() );
    worldPosition -= cameraPosition;

    Vector3 actorSize = GetCurrentSize() * GetCurrentWorldScale();
    Vector2 halfSceneSize( mScene->GetSize() * 0.5f ); // World position origin is center of scene
    Vector3 halfActorSize( actorSize * 0.5f );
    Vector3 anchorPointOffSet = halfActorSize - actorSize * ( mPositionUsesAnchorPoint ? GetCurrentAnchorPoint() : AnchorPoint::TOP_LEFT );

    return Vector2( halfSceneSize.width + worldPosition.x - anchorPointOffSet.x,
                    halfSceneSize.height + worldPosition.y - anchorPointOffSet.y );
  }

  return Vector2::ZERO;
}

void Actor::SetInheritPosition( bool inherit )
{
  if( mInheritPosition != inherit )
  {
    // non animatable so keep local copy
    mInheritPosition = inherit;
    SetInheritPositionMessage( GetEventThreadServices(), GetNode(), inherit );
  }
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
  mTargetOrientation = orientation;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Quaternion>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mOrientation, &SceneGraph::TransformManagerPropertyHandler<Quaternion>::Bake, orientation );
}

void Actor::RotateBy( const Radian& angle, const Vector3& axis )
{
  RotateBy( Quaternion(angle, axis) );
}

void Actor::RotateBy( const Quaternion& relativeRotation )
{
  mTargetOrientation *= Quaternion( relativeRotation );

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Quaternion>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mOrientation, &SceneGraph::TransformManagerPropertyHandler<Quaternion>::BakeRelative, relativeRotation );
}

const Quaternion& Actor::GetCurrentOrientation() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetOrientation(GetEventThreadServices().GetEventBufferIndex());
}

const Quaternion& Actor::GetCurrentWorldOrientation() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetWorldOrientation( GetEventThreadServices().GetEventBufferIndex() );
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
  mTargetScale = scale;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::Bake, scale );
}

void Actor::SetScaleX( float x )
{
  mTargetScale.x = x;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeX, x );
}

void Actor::SetScaleY( float y )
{
  mTargetScale.y = y;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeY, y );
}

void Actor::SetScaleZ( float z )
{
  mTargetScale.z = z;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeZ, z );
}

void Actor::ScaleBy(const Vector3& relativeScale)
{
  mTargetScale *= relativeScale;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeRelativeMultiply, relativeScale );
}

const Vector3& Actor::GetCurrentScale() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetScale(GetEventThreadServices().GetEventBufferIndex());
}

const Vector3& Actor::GetCurrentWorldScale() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetWorldScale( GetEventThreadServices().GetEventBufferIndex() );
}

void Actor::SetInheritScale( bool inherit )
{
  if( mInheritScale != inherit )
  {
    // non animatable so keep local copy
    mInheritScale = inherit;
    // node is being used in a separate thread; queue a message to set the value
    SetInheritScaleMessage( GetEventThreadServices(), GetNode(), inherit );
  }
}

Matrix Actor::GetCurrentWorldMatrix() const
{
  return GetNode().GetWorldMatrix(0);
}

void Actor::SetVisible( bool visible )
{
  SetVisibleInternal( visible, SendMessage::TRUE );
}

bool Actor::IsVisible() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().IsVisible( GetEventThreadServices().GetEventBufferIndex() );
}

void Actor::SetOpacity( float opacity )
{
  mTargetColor.a = opacity;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mColor, &AnimatableProperty<Vector4>::BakeW, opacity );

  RequestRenderingMessage( GetEventThreadServices().GetUpdateManager() );
}

float Actor::GetCurrentOpacity() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetOpacity(GetEventThreadServices().GetEventBufferIndex());
}

const Vector4& Actor::GetCurrentWorldColor() const
{
  return GetNode().GetWorldColor( GetEventThreadServices().GetEventBufferIndex() );
}

void Actor::SetColor( const Vector4& color )
{
  mTargetColor = color;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyMessage<Vector4>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mColor, &AnimatableProperty<Vector4>::Bake, color );

  RequestRenderingMessage( GetEventThreadServices().GetUpdateManager() );
}

void Actor::SetColorRed( float red )
{
  mTargetColor.r = red;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mColor, &AnimatableProperty<Vector4>::BakeX, red );

  RequestRenderingMessage( GetEventThreadServices().GetUpdateManager() );
}

void Actor::SetColorGreen( float green )
{
  mTargetColor.g = green;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mColor, &AnimatableProperty<Vector4>::BakeY, green );

  RequestRenderingMessage( GetEventThreadServices().GetUpdateManager() );
}

void Actor::SetColorBlue( float blue )
{
  mTargetColor.b = blue;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mColor, &AnimatableProperty<Vector4>::BakeZ, blue );

  RequestRenderingMessage( GetEventThreadServices().GetUpdateManager() );
}

const Vector4& Actor::GetCurrentColor() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetColor(GetEventThreadServices().GetEventBufferIndex());
}

void Actor::SetInheritOrientation( bool inherit )
{
  if( mInheritOrientation != inherit )
  {
    // non animatable so keep local copy
    mInheritOrientation = inherit;
    // node is being used in a separate thread; queue a message to set the value
    SetInheritOrientationMessage( GetEventThreadServices(), GetNode(), inherit );
  }
}

void Actor::SetSizeModeFactor( const Vector3& factor )
{
  EnsureRelayouter();

  mRelayoutData->sizeModeFactor = factor;
}

const Vector3& Actor::GetSizeModeFactor() const
{
  if ( mRelayoutData )
  {
    return mRelayoutData->sizeModeFactor;
  }

  return Relayouter::DEFAULT_SIZE_MODE_FACTOR;
}

void Actor::SetColorMode( ColorMode colorMode )
{
  // non animatable so keep local copy
  mColorMode = colorMode;
  // node is being used in a separate thread; queue a message to set the value
  SetColorModeMessage( GetEventThreadServices(), GetNode(), colorMode );
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
  SetSize( Vector3( size.width, size.height, 0.f ) );
}

void Actor::SetSizeInternal( const Vector2& size )
{
  SetSizeInternal( Vector3( size.width, size.height, 0.f ) );
}

void Actor::SetSize( const Vector3& size )
{
  if( IsRelayoutEnabled() && !mRelayoutData->insideRelayout )
  {
    // TODO we cannot just ignore the given Z but that means rewrite the size negotiation!!
    SetPreferredSize( size.GetVectorXY() );
  }
  else
  {
    SetSizeInternal( size );
  }
}

void Actor::SetSizeInternal( const Vector3& size )
{
  // dont allow recursive loop
  DALI_ASSERT_ALWAYS( !mInsideOnSizeSet && "Cannot call SetSize from OnSizeSet" );
  // check that we have a node AND the new size width, height or depth is at least a little bit different from the old one
  if( ( fabsf( mTargetSize.width - size.width  ) > Math::MACHINE_EPSILON_1 )||
      ( fabsf( mTargetSize.height- size.height ) > Math::MACHINE_EPSILON_1 )||
      ( fabsf( mTargetSize.depth - size.depth  ) > Math::MACHINE_EPSILON_1 ) )
  {
    mTargetSize = size;

    // node is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformPropertyMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::Bake, mTargetSize );

    // Notification for derived classes
    mInsideOnSizeSet = true;
    OnSizeSet( mTargetSize );
    mInsideOnSizeSet = false;

    // Raise a relayout request if the flag is not locked
    if( mRelayoutData && !mRelayoutData->insideRelayout )
    {
      RelayoutRequest();
    }
  }
}

void Actor::SetWidth( float width )
{
  if( IsRelayoutEnabled() && !mRelayoutData->insideRelayout )
  {
    SetResizePolicy( ResizePolicy::FIXED, Dimension::WIDTH );
    mRelayoutData->preferredSize.width = width;
  }
  else
  {
    mTargetSize.width = width;

    // node is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeX, width );
  }

  mUseAnimatedSize &= ~AnimatedSizeFlag::WIDTH;

  RelayoutRequest();
}

void Actor::SetHeight( float height )
{
  if( IsRelayoutEnabled() && !mRelayoutData->insideRelayout )
  {
    SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
    mRelayoutData->preferredSize.height = height;
  }
  else
  {
    mTargetSize.height = height;

    // node is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeY, height );
  }

  mUseAnimatedSize &= ~AnimatedSizeFlag::HEIGHT;

  RelayoutRequest();
}

void Actor::SetDepth( float depth )
{
  mTargetSize.depth = depth;

  mUseAnimatedSize &= ~AnimatedSizeFlag::DEPTH;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeZ, depth );
}

Vector3 Actor::GetTargetSize() const
{
  Vector3 size = mTargetSize;

  if( mUseAnimatedSize & AnimatedSizeFlag::WIDTH )
  {
    // Should return animated size if size is animated
    size.width = mAnimatedSize.width;
  }
  else
  {
    // Should return preferred size if size is fixed as set by SetSize
    if( GetResizePolicy( Dimension::WIDTH ) == ResizePolicy::FIXED )
    {
      size.width = GetPreferredSize().width;
    }
  }

  if( mUseAnimatedSize & AnimatedSizeFlag::HEIGHT )
  {
    size.height = mAnimatedSize.height;
  }
  else
  {
    if( GetResizePolicy( Dimension::HEIGHT ) == ResizePolicy::FIXED )
    {
      size.height = GetPreferredSize().height;
    }
  }

  if( mUseAnimatedSize & AnimatedSizeFlag::DEPTH )
  {
    size.depth = mAnimatedSize.depth;
  }

  return size;
}

const Vector3& Actor::GetCurrentSize() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetSize( GetEventThreadServices().GetEventBufferIndex() );
}

Vector3 Actor::GetNaturalSize() const
{
  // It is up to deriving classes to return the appropriate natural size
  return Vector3( 0.0f, 0.0f, 0.0f );
}

void Actor::SetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
{
  EnsureRelayouter().SetResizePolicy(policy, dimension, mTargetSize);

  OnSetResizePolicy( policy, dimension );

  // Trigger relayout on this control
  RelayoutRequest();
}

ResizePolicy::Type Actor::GetResizePolicy( Dimension::Type dimension ) const
{
  if ( mRelayoutData )
  {
    return mRelayoutData->GetResizePolicy(dimension);
  }

  return ResizePolicy::DEFAULT;
}

void Actor::SetSizeScalePolicy( SizeScalePolicy::Type policy )
{
  EnsureRelayouter();

  mRelayoutData->sizeSetPolicy = policy;

  // Trigger relayout on this control
  RelayoutRequest();
}

SizeScalePolicy::Type Actor::GetSizeScalePolicy() const
{
  if ( mRelayoutData )
  {
    return mRelayoutData->sizeSetPolicy;
  }

  return Relayouter::DEFAULT_SIZE_SCALE_POLICY;
}

void Actor::SetDimensionDependency( Dimension::Type dimension, Dimension::Type dependency )
{
  EnsureRelayouter().SetDimensionDependency(dimension, dependency);
}

Dimension::Type Actor::GetDimensionDependency( Dimension::Type dimension ) const
{
  if ( mRelayoutData )
  {
    return mRelayoutData->GetDimensionDependency(dimension);
  }

  return Dimension::ALL_DIMENSIONS;   // Default
}

void Actor::SetRelayoutEnabled( bool relayoutEnabled )
{
  // If relayout data has not been allocated yet and the client is requesting
  // to disable it, do nothing
  if( mRelayoutData || relayoutEnabled )
  {
    EnsureRelayouter();

    DALI_ASSERT_DEBUG( mRelayoutData && "mRelayoutData not created" );

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
  EnsureRelayouter().SetLayoutDirty(dirty, dimension);
}

bool Actor::IsLayoutDirty( Dimension::Type dimension ) const
{
  return mRelayoutData && mRelayoutData->IsLayoutDirty(dimension);
}

bool Actor::RelayoutPossible( Dimension::Type dimension ) const
{
  return mRelayoutData && mRelayoutData->relayoutEnabled && !IsLayoutDirty( dimension );
}

bool Actor::RelayoutRequired( Dimension::Type dimension ) const
{
  return mRelayoutData && mRelayoutData->relayoutEnabled && IsLayoutDirty( dimension );
}

uint32_t Actor::AddRenderer( Renderer& renderer )
{
  if( !mRenderers )
  {
    mRenderers = new RendererContainer;
  }

  uint32_t index = static_cast<uint32_t>( mRenderers->size() ); //  4,294,967,295 renderers per actor
  RendererPtr rendererPtr = RendererPtr( &renderer );
  mRenderers->push_back( rendererPtr );
  AttachRendererMessage( GetEventThreadServices(), GetNode(), renderer.GetRendererSceneObject() );
  return index;
}

uint32_t Actor::GetRendererCount() const
{
  uint32_t rendererCount(0);
  if( mRenderers )
  {
    rendererCount = static_cast<uint32_t>( mRenderers->size() ); //  4,294,967,295 renderers per actor
  }

  return rendererCount;
}

RendererPtr Actor::GetRendererAt( uint32_t index )
{
  RendererPtr renderer;
  if( index < GetRendererCount() )
  {
    renderer = ( *mRenderers )[ index ];
  }

  return renderer;
}

void Actor::RemoveRenderer( Renderer& renderer )
{
  if( mRenderers )
  {
    RendererIter end = mRenderers->end();
    for( RendererIter iter = mRenderers->begin(); iter != end; ++iter )
    {
      if( (*iter).Get() == &renderer )
      {
        mRenderers->erase( iter );
        DetachRendererMessage( GetEventThreadServices(), GetNode(), renderer.GetRendererSceneObject() );
        break;
      }
    }
  }
}

void Actor::RemoveRenderer( uint32_t index )
{
  if( index < GetRendererCount() )
  {
    RendererPtr renderer = ( *mRenderers )[ index ];
    DetachRendererMessage( GetEventThreadServices(), GetNode(), renderer.Get()->GetRendererSceneObject() );
    mRenderers->erase( mRenderers->begin()+index );
  }
}

void Actor::SetDrawMode( DrawMode::Type drawMode )
{
  // this flag is not animatable so keep the value
  mDrawMode = drawMode;

  // node is being used in a separate thread; queue a message to set the value
  SetDrawModeMessage( GetEventThreadServices(), GetNode(), drawMode );
}

bool Actor::ScreenToLocal( float& localX, float& localY, float screenX, float screenY ) const
{
  // only valid when on-stage
  if( mScene && OnScene() )
  {
    const RenderTaskList& taskList = mScene->GetRenderTaskList();

    Vector2 converted( screenX, screenY );

    // do a reverse traversal of all lists (as the default onscreen one is typically the last one)
    uint32_t taskCount = taskList.GetTaskCount();
    for( uint32_t i = taskCount; i > 0; --i )
    {
      RenderTaskPtr task = taskList.GetTask( i - 1 );
      if( ScreenToLocal( *task, localX, localY, screenX, screenY ) )
      {
        // found a task where this conversion was ok so return
        return true;
      }
    }
  }
  return false;
}

bool Actor::ScreenToLocal( const RenderTask& renderTask, float& localX, float& localY, float screenX, float screenY ) const
{
  bool retval = false;
  // only valid when on-stage
  if( OnScene() )
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
  return OnScene() && ScreenToLocalInternal(viewMatrix, projectionMatrix, GetNode().GetWorldMatrix(0), viewport, GetCurrentSize(), localX, localY, screenX, screenY);
}

ActorGestureData& Actor::GetGestureData()
{
  // Likely scenario is that once gesture-data is created for this actor, the actor will require
  // that gesture for its entire life-time so no need to destroy it until the actor is destroyed
  if( nullptr == mGestureData )
  {
    mGestureData = new ActorGestureData;
  }
  return *mGestureData;
}

bool Actor::IsGestureRequired( GestureType::Value type ) const
{
  return mGestureData && mGestureData->IsGestureRequired( type );
}

bool Actor::EmitInterceptTouchEventSignal( const Dali::TouchEvent& touch )
{
  return EmitConsumingSignal( *this, mInterceptTouchedSignal, touch );
}

bool Actor::EmitTouchEventSignal( const Dali::TouchEvent& touch )
{
  return EmitConsumingSignal( *this, mTouchedSignal, touch );
}

bool Actor::EmitHoverEventSignal( const Dali::HoverEvent& event )
{
  return EmitConsumingSignal( *this, mHoveredSignal, event );
}

bool Actor::EmitWheelEventSignal( const Dali::WheelEvent& event )
{
  return EmitConsumingSignal( *this, mWheelEventSignal, event );
}

void Actor::EmitVisibilityChangedSignal( bool visible, DevelActor::VisibilityChange::Type type )
{
  EmitSignal( *this, mVisibilityChangedSignal, visible, type );
}

void Actor::EmitLayoutDirectionChangedSignal( LayoutDirection::Type type )
{
  EmitSignal( *this, mLayoutDirectionChangedSignal, type );
}

void Actor::EmitChildAddedSignal( Actor& child )
{
  EmitSignal( child, mChildAddedSignal );
}

void Actor::EmitChildRemovedSignal( Actor& child )
{
  EmitSignal( child, mChildRemovedSignal );
}

bool Actor::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  Actor* actor = static_cast< Actor* >( object ); // TypeRegistry guarantees that this is the correct type.

  if( 0 == signalName.compare( SIGNAL_HOVERED ) )
  {
    actor->HoveredSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_WHEEL_EVENT ) )
  {
    actor->WheelEventSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_ON_SCENE ) )
  {
    actor->OnSceneSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_OFF_SCENE ) )
  {
    actor->OffSceneSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_ON_RELAYOUT ) )
  {
    actor->OnRelayoutSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_TOUCHED ) )
  {
    actor->TouchedSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_VISIBILITY_CHANGED ) )
  {
    actor->VisibilityChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_LAYOUT_DIRECTION_CHANGED ) )
  {
    actor->LayoutDirectionChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_CHILD_ADDED ) )
  {
    actor->ChildAddedSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_CHILD_REMOVED ) )
  {
    actor->ChildRemovedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

Actor::Actor( DerivedType derivedType, const SceneGraph::Node& node )
: Object( &node ),
  mScene( nullptr ),
  mParent( nullptr ),
  mChildren( nullptr ),
  mRenderers( nullptr ),
  mParentOrigin( nullptr ),
  mAnchorPoint( nullptr ),
  mRelayoutData( nullptr ),
  mGestureData( nullptr ),
  mInterceptTouchedSignal(),
  mTouchedSignal(),
  mHoveredSignal(),
  mWheelEventSignal(),
  mOnSceneSignal(),
  mOffSceneSignal(),
  mOnRelayoutSignal(),
  mVisibilityChangedSignal(),
  mLayoutDirectionChangedSignal(),
  mChildAddedSignal(),
  mChildRemovedSignal(),
  mChildOrderChangedSignal(),
  mTargetOrientation( Quaternion::IDENTITY ),
  mTargetColor( Color::WHITE ),
  mTargetSize( Vector3::ZERO ),
  mTargetPosition( Vector3::ZERO ),
  mTargetScale( Vector3::ONE ),
  mAnimatedSize( Vector3::ZERO ),
  mName(),
  mSortedDepth( 0u ),
  mDepth( 0u ),
  mUseAnimatedSize( AnimatedSizeFlag::CLEAR ),
  mIsRoot( ROOT_LAYER == derivedType ),
  mIsLayer( LAYER == derivedType || ROOT_LAYER == derivedType ),
  mIsOnScene( false ),
  mSensitive( true ),
  mLeaveRequired( false ),
  mKeyboardFocusable( false ),
  mOnSceneSignalled( false ),
  mInsideOnSizeSet( false ),
  mInheritPosition( true ),
  mInheritOrientation( true ),
  mInheritScale( true ),
  mPositionUsesAnchorPoint( true ),
  mVisible( true ),
  mInheritLayoutDirection( true ),
  mCaptureAllTouchAfterStart( false ),
  mLayoutDirection( LayoutDirection::LEFT_TO_RIGHT ),
  mDrawMode( DrawMode::NORMAL ),
  mColorMode( Node::DEFAULT_COLOR_MODE ),
  mClippingMode( ClippingMode::DISABLED )
{
}

void Actor::Initialize()
{
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
      (*iter)->SetParent( nullptr );
    }
  }
  delete mChildren;
  delete mRenderers;

  // Guard to allow handle destruction after Core has been destroyed
  if( EventThreadServices::IsCoreRunning() )
  {
    // Root layer will destroy its node in its own destructor
    if ( !mIsRoot )
    {
      DestroyNodeMessage( GetEventThreadServices().GetUpdateManager(), GetNode() );

      GetEventThreadServices().UnregisterObject( this );
    }
  }

  // Cleanup optional gesture data
  delete mGestureData;

  // Cleanup optional parent origin and anchor
  delete mParentOrigin;
  delete mAnchorPoint;

  // Delete optional relayout data
  delete mRelayoutData;
}

void Actor::ConnectToScene( uint32_t parentDepth )
{
  // This container is used instead of walking the Actor hierarchy.
  // It protects us when the Actor hierarchy is modified during OnSceneConnectionExternal callbacks.
  ActorContainer connectionList;

  if( mScene )
  {
    mScene->RequestRebuildDepthTree();
  }

  // This stage is atomic i.e. not interrupted by user callbacks.
  RecursiveConnectToScene( connectionList, parentDepth + 1 );

  // Notify applications about the newly connected actors.
  const ActorIter endIter = connectionList.end();
  for( ActorIter iter = connectionList.begin(); iter != endIter; ++iter )
  {
    (*iter)->NotifyStageConnection();
  }

  RelayoutRequest();
}

void Actor::RecursiveConnectToScene( ActorContainer& connectionList, uint32_t depth )
{
  DALI_ASSERT_ALWAYS( !OnScene() );

  mIsOnScene = true;
  mDepth = static_cast< uint16_t >( depth ); // overflow ignored, not expected in practice

  ConnectToSceneGraph();

  // Notification for internal derived classes
  OnSceneConnectionInternal();

  // This stage is atomic; avoid emitting callbacks until all Actors are connected
  connectionList.push_back( ActorPtr( this ) );

  // Recursively connect children
  if( mChildren )
  {
    ActorConstIter endIter = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != endIter; ++iter )
    {
      (*iter)->SetScene( *mScene );
      (*iter)->RecursiveConnectToScene( connectionList, depth + 1 );
    }
  }
}

/**
 * This method is called when the Actor is connected to the Stage.
 * The parent must have added its Node to the scene-graph.
 * The child must connect its Node to the parent's Node.
 * This is recursive; the child calls ConnectToScene() for its children.
 */
void Actor::ConnectToSceneGraph()
{
  DALI_ASSERT_DEBUG( mParent != NULL);

  // Reparent Node in next Update
  ConnectNodeMessage( GetEventThreadServices().GetUpdateManager(), mParent->GetNode(), GetNode() );

  // Request relayout on all actors that are added to the scenegraph
  RelayoutRequest();

  // Notification for Object::Observers
  OnSceneObjectAdd();
}

void Actor::NotifyStageConnection()
{
  // Actors can be removed (in a callback), before the on-stage stage is reported.
  // The actor may also have been reparented, in which case mOnSceneSignalled will be true.
  if( OnScene() && !mOnSceneSignalled )
  {
    // Notification for external (CustomActor) derived classes
    OnSceneConnectionExternal( mDepth );

    if( !mOnSceneSignal.Empty() )
    {
      Dali::Actor handle( this );
      mOnSceneSignal.Emit( handle );
    }

    // Guard against Remove during callbacks
    if( OnScene() )
    {
      mOnSceneSignalled = true; // signal required next time Actor is removed
    }
  }
}

void Actor::DisconnectFromStage()
{
  // This container is used instead of walking the Actor hierachy.
  // It protects us when the Actor hierachy is modified during OnSceneDisconnectionExternal callbacks.
  ActorContainer disconnectionList;

  if( mScene )
  {
    mScene->RequestRebuildDepthTree();
  }

  // This stage is atomic i.e. not interrupted by user callbacks
  RecursiveDisconnectFromStage( disconnectionList );

  // Notify applications about the newly disconnected actors.
  const ActorIter endIter = disconnectionList.end();
  for( ActorIter iter = disconnectionList.begin(); iter != endIter; ++iter )
  {
    (*iter)->NotifyStageDisconnection();
  }
}

void Actor::RecursiveDisconnectFromStage( ActorContainer& disconnectionList )
{
  // need to change state first so that internals relying on IsOnScene() inside OnSceneDisconnectionInternal() get the correct value
  mIsOnScene = false;

  // Recursively disconnect children
  if( mChildren )
  {
    ActorConstIter endIter = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != endIter; ++iter )
    {
      (*iter)->RecursiveDisconnectFromStage( disconnectionList );
    }
  }

  // This stage is atomic; avoid emitting callbacks until all Actors are disconnected
  disconnectionList.push_back( ActorPtr( this ) );

  // Notification for internal derived classes
  OnSceneDisconnectionInternal();

  DisconnectFromSceneGraph();
}

/**
 * This method is called by an actor or its parent, before a node removal message is sent.
 * This is recursive; the child calls DisconnectFromStage() for its children.
 */
void Actor::DisconnectFromSceneGraph()
{
  // Notification for Object::Observers
  OnSceneObjectRemove();
}

void Actor::NotifyStageDisconnection()
{
  // Actors can be added (in a callback), before the off-stage state is reported.
  // Also if the actor was added & removed before mOnSceneSignalled was set, then we don't notify here.
  // only do this step if there is a stage, i.e. Core is not being shut down
  if ( EventThreadServices::IsCoreRunning() && !OnScene() && mOnSceneSignalled )
  {
    // Notification for external (CustomeActor) derived classes
    OnSceneDisconnectionExternal();

    if( !mOffSceneSignal.Empty() )
    {
      Dali::Actor handle( this );
      mOffSceneSignal.Emit( handle );
    }

    // Guard against Add during callbacks
    if( !OnScene() )
    {
      mOnSceneSignalled = false; // signal required next time Actor is added
    }
  }
}

bool Actor::IsNodeConnected() const
{
  bool connected( false );

  if( OnScene() )
  {
    if( IsRoot() || GetNode().GetParent() )
    {
      connected = true;
    }
  }

  return connected;
}

// This method initiates traversal of the actor tree using depth-first
// traversal to set a depth index based on traversal order. It sends a
// single message to update manager to update all the actor's nodes in
// this tree with the depth index. The sceneGraphNodeDepths vector's
// elements are ordered by depth, and could be used to reduce sorting
// in the update thread.
void Actor::RebuildDepthTree()
{
  DALI_LOG_TIMER_START(depthTimer);

  // Vector of scene-graph nodes and their depths to send to UpdateManager
  // in a single message
  OwnerPointer<SceneGraph::NodeDepths> sceneGraphNodeDepths( new SceneGraph::NodeDepths() );

  int32_t depthIndex = 1;
  DepthTraverseActorTree( sceneGraphNodeDepths, depthIndex );

  SetDepthIndicesMessage( GetEventThreadServices().GetUpdateManager(), sceneGraphNodeDepths );
  DALI_LOG_TIMER_END(depthTimer, gLogFilter, Debug::Concise, "Depth tree traversal time: ");
}

void Actor::DepthTraverseActorTree( OwnerPointer<SceneGraph::NodeDepths>& sceneGraphNodeDepths, int32_t& depthIndex )
{
  mSortedDepth = depthIndex * DevelLayer::SIBLING_ORDER_MULTIPLIER;
  sceneGraphNodeDepths->Add( const_cast<SceneGraph::Node*>( &GetNode() ), mSortedDepth );

  // Create/add to children of this node
  if( mChildren )
  {
    for( ActorContainer::iterator it = mChildren->begin(); it != mChildren->end(); ++it )
    {
      Actor* childActor = (*it).Get();
      ++depthIndex;
      childActor->DepthTraverseActorTree( sceneGraphNodeDepths, depthIndex );
    }
  }
}

void Actor::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  PropertyHandler::SetDefaultProperty(*this, index, property);
}

// TODO: This method needs to be removed
void Actor::SetSceneGraphProperty( Property::Index index, const PropertyMetadata& entry, const Property::Value& value )
{
  PropertyHandler::SetSceneGraphProperty(index, entry, value, GetEventThreadServices(), GetNode());
}

Property::Value Actor::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  if( ! GetCachedPropertyValue( index, value ) )
  {
    // If property value is not stored in the event-side, then it must be a scene-graph only property
    GetCurrentPropertyValue( index, value );
  }

  return value;
}

Property::Value Actor::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  Property::Value value;

  if( ! GetCurrentPropertyValue( index, value ) )
  {
    // If unable to retrieve scene-graph property value, then it must be an event-side only property
    GetCachedPropertyValue( index, value );
  }

  return value;
}

void Actor::OnNotifyDefaultPropertyAnimation( Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType )
{
  PropertyHandler::OnNotifyDefaultPropertyAnimation(*this, animation, index, value, animationType);
}

const PropertyBase* Actor::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  const PropertyBase* property = PropertyHandler::GetSceneObjectAnimatableProperty(index, GetNode());
  if( !property )
  {
    // not our property, ask base
    property = Object::GetSceneObjectAnimatableProperty( index );
  }

  return property;
}

const PropertyInputImpl* Actor::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = PropertyHandler::GetSceneObjectInputProperty(index, GetNode());
  if( !property )
  {
    // reuse animatable property getter as animatable properties are inputs as well
    // animatable property chains back to Object::GetSceneObjectInputProperty() so all properties get covered
    property = GetSceneObjectAnimatableProperty( index );
  }

  return property;
}

int32_t Actor::GetPropertyComponentIndex( Property::Index index ) const
{
  int32_t componentIndex = PropertyHandler::GetPropertyComponentIndex(index);
  if( Property::INVALID_COMPONENT_INDEX == componentIndex )
  {
    // ask base
    componentIndex = Object::GetPropertyComponentIndex( index );
  }

  return componentIndex;
}

void Actor::SetParent( Actor* parent )
{
  if( parent )
  {
    DALI_ASSERT_ALWAYS( !mParent && "Actor cannot have 2 parents" );

    mParent = parent;

    mScene = parent->mScene;

    if ( EventThreadServices::IsCoreRunning() && // Don't emit signals or send messages during Core destruction
         parent->OnScene() )
    {
      // Instruct each actor to create a corresponding node in the scene graph
      ConnectToScene( parent->GetHierarchyDepth() );
    }

    // Resolve the name and index for the child properties if any
    ResolveChildProperties();
  }
  else // parent being set to NULL
  {
    DALI_ASSERT_ALWAYS( mParent != nullptr && "Actor should have a parent" );

    mParent = nullptr;

    if ( EventThreadServices::IsCoreRunning() && // Don't emit signals or send messages during Core destruction
         OnScene() )
    {
      // Disconnect the Node & its children from the scene-graph.
      DisconnectNodeMessage( GetEventThreadServices().GetUpdateManager(), GetNode() );

      // Instruct each actor to discard pointers to the scene-graph
      DisconnectFromStage();
    }

    mScene = nullptr;
  }
}

bool Actor::DoAction( BaseObject* object, const std::string& actionName, const Property::Map& /* attributes */ )
{
  bool done = false;
  Actor* actor = dynamic_cast< Actor* >( object );

  if( actor )
  {
    if( 0 == actionName.compare( ACTION_SHOW ) )
    {
      actor->SetVisible( true );
      done = true;
    }
    else if( 0 == actionName.compare( ACTION_HIDE ) )
    {
      actor->SetVisible( false );
      done = true;
    }
  }

  return done;
}

Rect<> Actor::CalculateScreenExtents( ) const
{
  auto screenPosition = GetCurrentScreenPosition();
  Vector3 size = GetCurrentSize() * GetCurrentWorldScale();
  Vector3 anchorPointOffSet = size * ( mPositionUsesAnchorPoint ? GetCurrentAnchorPoint() : AnchorPoint::TOP_LEFT );
  Vector2 position = Vector2( screenPosition.x - anchorPointOffSet.x, screenPosition.y - anchorPointOffSet.y );
  return { position.x, position.y, size.x, size.y };
}

bool Actor::GetCachedPropertyValue( Property::Index index, Property::Value& value ) const
{
  return PropertyHandler::GetCachedPropertyValue(*this, index, value);
}

bool Actor::GetCurrentPropertyValue( Property::Index index, Property::Value& value  ) const
{
  return PropertyHandler::GetCurrentPropertyValue(*this, index, value);
}

Actor::Relayouter& Actor::EnsureRelayouter()
{
  // Assign relayouter
  if( !mRelayoutData )
  {
    mRelayoutData = new Relayouter();
  }

  return *mRelayoutData;
}

bool Actor::RelayoutDependentOnParent( Dimension::Type dimension )
{
  // Check if actor is dependent on parent
  for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
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
  for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
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
  for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
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
  for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
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
  for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
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
  EnsureRelayouter().SetPadding( padding, dimension );
}

Vector2 Actor::GetPadding( Dimension::Type dimension ) const
{
  if ( mRelayoutData )
  {
    // If more than one dimension is requested, just return the first one found
    for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      if( ( dimension & ( 1 << i ) ) )
      {
        return mRelayoutData->dimensionPadding[ i ];
      }
    }
  }

  return Relayouter::DEFAULT_DIMENSION_PADDING;
}

void Actor::SetLayoutNegotiated( bool negotiated, Dimension::Type dimension )
{
  EnsureRelayouter().SetLayoutNegotiated(negotiated, dimension);
}

bool Actor::IsLayoutNegotiated( Dimension::Type dimension ) const
{
  return mRelayoutData && mRelayoutData->IsLayoutNegotiated(dimension);
}

float Actor::GetHeightForWidthBase( float width )
{
  float height = 0.0f;

  const Vector3 naturalSize = GetNaturalSize();
  if( naturalSize.width > 0.0f )
  {
    height = naturalSize.height * width / naturalSize.width;
  }
  else // we treat 0 as 1:1 aspect ratio
  {
    height = width;
  }

  return height;
}

float Actor::GetWidthForHeightBase( float height )
{
  float width = 0.0f;

  const Vector3 naturalSize = GetNaturalSize();
  if( naturalSize.height > 0.0f )
  {
    width = naturalSize.width * height / naturalSize.height;
  }
  else // we treat 0 as 1:1 aspect ratio
  {
    width = height;
  }

  return width;
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
      return GetLatestSize( dimension ) * GetDimensionValue( child.GetProperty< Vector3 >( Dali::Actor::Property::SIZE_MODE_FACTOR ), dimension );
    }

    case ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT:
    {
      return GetLatestSize( dimension ) + GetDimensionValue( child.GetProperty< Vector3 >( Dali::Actor::Property::SIZE_MODE_FACTOR ), dimension );
    }

    default:
    {
      return GetLatestSize( dimension );
    }
  }
}

float Actor::CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension )
{
  // Can be overridden in derived class
  return CalculateChildSizeBase( child, dimension );
}

float Actor::GetHeightForWidth( float width )
{
  // Can be overridden in derived class
  return GetHeightForWidthBase( width );
}

float Actor::GetWidthForHeight( float height )
{
  // Can be overridden in derived class
  return GetWidthForHeightBase( height );
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

  for( uint32_t i = 0, count = GetChildCount(); i < count; ++i )
  {
    ActorPtr child = GetChildAt( i );

    if( !child->RelayoutDependentOnParent( dimension ) )
    {
      // Calculate the min and max points that the children range across
      float childPosition = GetDimensionValue( child->GetTargetPosition(), dimension );
      float dimensionSize = child->GetRelayoutSize( dimension );
      maxDimensionPoint = std::max( maxDimensionPoint, childPosition + dimensionSize );
    }
  }

  return maxDimensionPoint;
}

float Actor::GetSize( Dimension::Type dimension ) const
{
  return GetDimensionValue( mTargetSize, dimension );
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
      for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
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
        for( uint32_t i = 0, count = GetChildCount(); i < count; ++i )
        {
          ActorPtr child = GetChildAt( i );

          // Only relayout child first if it is not dependent on this actor
          if( !child->RelayoutDependentOnParent( dimension ) )
          {
            child->NegotiateDimension( dimension, allocatedSize, recursionStack );
          }
        }
      }

      // For deriving classes
      OnCalculateRelayoutSize( dimension );

      // All dependencies checked, calculate the size and set negotiated flag
      const float newSize = Relayouter::ClampDimension( *this, CalculateSize( dimension, allocatedSize ), dimension );

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

  for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    const Dimension::Type dimension = static_cast< Dimension::Type >( 1 << i );

    // Negotiate
    NegotiateDimension( dimension, allocatedSize, recursionStack );
  }
}

Vector2 Actor::ApplySizeSetPolicy( const Vector2& size )
{
  return mRelayoutData->ApplySizeSetPolicy(*this, size);
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
  // Force a size negotiation for actors that has assigned size during relayout
  // This is required as otherwise the flags that force a relayout will not
  // necessarilly be set. This will occur if the actor has already been laid out.
  // The dirty flags are then cleared. Then if the actor is added back into the
  // relayout container afterwards, the dirty flags would still be clear...
  // causing a relayout to be skipped. Here we force any actors added to the
  // container to be relayed out.
  DALI_LOG_TIMER_START( NegSizeTimer1 );

  if( GetUseAssignedSize(Dimension::WIDTH ) )
  {
    SetLayoutNegotiated( false, Dimension::WIDTH );
  }
  if( GetUseAssignedSize( Dimension::HEIGHT ) )
  {
    SetLayoutNegotiated( false, Dimension::HEIGHT );
  }

  // Do the negotiation
  NegotiateDimensions( allocatedSize );

  // Set the actor size
  SetNegotiatedSize( container );

  // Negotiate down to children
  for( uint32_t i = 0, count = GetChildCount(); i < count; ++i )
  {
    ActorPtr child = GetChildAt( i );

    // Forces children that have already been laid out to be relayed out
    // if they have assigned size during relayout.
    if( child->GetUseAssignedSize(Dimension::WIDTH) )
    {
      child->SetLayoutNegotiated(false, Dimension::WIDTH);
      child->SetLayoutDirty(true, Dimension::WIDTH);
    }

    if( child->GetUseAssignedSize(Dimension::HEIGHT) )
    {
      child->SetLayoutNegotiated(false, Dimension::HEIGHT);
      child->SetLayoutDirty(true, Dimension::HEIGHT);
    }

    // Only relayout if required
    if( child->RelayoutRequired() )
    {
      container.Add( Dali::Actor( child.Get() ), mTargetSize.GetVectorXY() );
    }
  }
  DALI_LOG_TIMER_END( NegSizeTimer1, gLogRelayoutFilter, Debug::Concise, "NegotiateSize() took: ");
}

void Actor::SetUseAssignedSize( bool use, Dimension::Type dimension )
{
  if( mRelayoutData )
  {
    mRelayoutData->SetUseAssignedSize(use, dimension);
  }
}

bool Actor::GetUseAssignedSize( Dimension::Type dimension ) const
{
  return mRelayoutData && mRelayoutData->GetUseAssignedSize(dimension);
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

void Actor::SetPreferredSize( const Vector2& size )
{
  EnsureRelayouter();

  // If valid width or height, then set the resize policy to FIXED
  // A 0 width or height may also be required so if the resize policy has not been changed, i.e. is still set to DEFAULT,
  // then change to FIXED as well

  if( size.width > 0.0f || GetResizePolicy( Dimension::WIDTH ) == ResizePolicy::DEFAULT )
  {
    SetResizePolicy( ResizePolicy::FIXED, Dimension::WIDTH );
  }

  if( size.height > 0.0f || GetResizePolicy( Dimension::HEIGHT ) == ResizePolicy::DEFAULT )
  {
    SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
  }

  mRelayoutData->preferredSize = size;

  mUseAnimatedSize = AnimatedSizeFlag::CLEAR;

  RelayoutRequest();
}

Vector2 Actor::GetPreferredSize() const
{
  if ( mRelayoutData )
  {
    return Vector2( mRelayoutData->preferredSize );
  }

  return Relayouter::DEFAULT_PREFERRED_SIZE;
}

void Actor::SetMinimumSize( float size, Dimension::Type dimension )
{
  EnsureRelayouter().SetMinimumSize(size, dimension);
  RelayoutRequest();
}

float Actor::GetMinimumSize( Dimension::Type dimension ) const
{
  if ( mRelayoutData )
  {
    return mRelayoutData->GetMinimumSize(dimension);
  }

  return 0.0f;  // Default
}

void Actor::SetMaximumSize( float size, Dimension::Type dimension )
{
  EnsureRelayouter().SetMaximumSize(size, dimension);
  RelayoutRequest();
}

float Actor::GetMaximumSize( Dimension::Type dimension ) const
{
  if ( mRelayoutData )
  {
    return mRelayoutData->GetMaximumSize(dimension);
  }

  return FLT_MAX;  // Default
}

void Actor::SetVisibleInternal( bool visible, SendMessage::Type sendMessage )
{
  if( mVisible != visible )
  {
    if( sendMessage == SendMessage::TRUE )
    {
      // node is being used in a separate thread; queue a message to set the value & base value
      SceneGraph::NodePropertyMessage<bool>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mVisible, &AnimatableProperty<bool>::Bake, visible );

      RequestRenderingMessage( GetEventThreadServices().GetUpdateManager() );
    }

    mVisible = visible;

    // Emit the signal on this actor and all its children
    EmitVisibilityChangedSignalRecursively( this, visible, DevelActor::VisibilityChange::SELF );
  }
}

void Actor::SetSiblingOrder( uint32_t order )
{
  if ( mParent )
  {
    ActorContainer& siblings = *(mParent->mChildren);
    uint32_t currentOrder = GetSiblingOrder();

    if( order != currentOrder )
    {
      if( order == 0 )
      {
        LowerToBottom();
      }
      else if( order < siblings.size() -1 )
      {
        if( order > currentOrder )
        {
          RaiseAbove( *siblings[order] );
        }
        else
        {
          LowerBelow( *siblings[order] );
        }
      }
      else
      {
        RaiseToTop();
      }
    }
  }
}

uint32_t Actor::GetSiblingOrder() const
{
  uint32_t order = 0;

  if ( mParent )
  {
    ActorContainer& siblings = *(mParent->mChildren);
    for( std::size_t i = 0; i < siblings.size(); ++i )
    {
      if( siblings[i] == this )
      {
        order = static_cast<uint32_t>( i );
        break;
      }
    }
  }

  return order;
}

void Actor::RequestRebuildDepthTree()
{
  if( mIsOnScene )
  {
    if( mScene )
    {
      mScene->RequestRebuildDepthTree();
    }
  }
}

void Actor::Raise()
{
  if ( mParent )
  {
    ActorContainer& siblings = *(mParent->mChildren);
    if( siblings.back() != this ) // If not already at end
    {
      for( std::size_t i=0; i<siblings.size(); ++i )
      {
        if( siblings[i] == this )
        {
          // Swap with next
          ActorPtr next = siblings[i+1];
          siblings[i+1] = this;
          siblings[i] = next;
          break;
        }
      }
    }

    Dali::Actor handle( this );
    mParent->mChildOrderChangedSignal.Emit( handle );

    RequestRebuildDepthTree();
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::Lower()
{
  if ( mParent )
  {
    ActorContainer& siblings = *(mParent->mChildren);
    if( siblings.front() != this ) // If not already at beginning
    {
      for( std::size_t i=1; i<siblings.size(); ++i )
      {
        if( siblings[i] == this )
        {
          // Swap with previous
          ActorPtr previous = siblings[i-1];
          siblings[i-1] = this;
          siblings[i] = previous;
          break;
        }
      }
    }

    Dali::Actor handle( this );
    mParent->mChildOrderChangedSignal.Emit( handle );

    RequestRebuildDepthTree();
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::RaiseToTop()
{
  if ( mParent )
  {
    ActorContainer& siblings = *(mParent->mChildren);
    if( siblings.back() != this ) // If not already at end
    {
      ActorContainer::iterator iter = std::find( siblings.begin(), siblings.end(), this );
      if( iter != siblings.end() )
      {
        siblings.erase(iter);
        siblings.push_back(ActorPtr(this));
      }
    }

    Dali::Actor handle( this );
    mParent->mChildOrderChangedSignal.Emit( handle );

    RequestRebuildDepthTree();
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::LowerToBottom()
{
  if ( mParent )
  {
    ActorContainer& siblings = *(mParent->mChildren);
    if( siblings.front() != this ) // If not already at bottom,
    {
      ActorPtr thisPtr(this); // ensure this actor remains referenced.

      ActorContainer::iterator iter = std::find( siblings.begin(), siblings.end(), this );
      if( iter != siblings.end() )
      {
        siblings.erase(iter);
        siblings.insert(siblings.begin(), thisPtr);
      }
    }

    Dali::Actor handle( this );
    mParent->mChildOrderChangedSignal.Emit( handle );

    RequestRebuildDepthTree();
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::RaiseAbove( Internal::Actor& target )
{
  if ( mParent )
  {
    ActorContainer& siblings = *(mParent->mChildren);
    if( siblings.back() != this && target.mParent == mParent ) // If not already at top
    {
      ActorPtr thisPtr(this); // ensure this actor remains referenced.

      ActorContainer::iterator targetIter = std::find( siblings.begin(), siblings.end(), &target );
      ActorContainer::iterator thisIter = std::find( siblings.begin(), siblings.end(), this );
      if( thisIter < targetIter )
      {
        siblings.erase(thisIter);
        // Erasing early invalidates the targetIter. (Conversely, inserting first may also
        // invalidate thisIter)
        targetIter = std::find( siblings.begin(), siblings.end(), &target );
        ++targetIter;
        siblings.insert(targetIter, thisPtr);
      }

      Dali::Actor handle( this );
      mParent->mChildOrderChangedSignal.Emit( handle );

      RequestRebuildDepthTree();
    }
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::LowerBelow( Internal::Actor& target )
{
  if ( mParent )
  {
    ActorContainer& siblings = *(mParent->mChildren);
    if( siblings.front() != this && target.mParent == mParent ) // If not already at bottom
    {
      ActorPtr thisPtr(this); // ensure this actor remains referenced.

      ActorContainer::iterator targetIter = std::find( siblings.begin(), siblings.end(), &target );
      ActorContainer::iterator thisIter = std::find( siblings.begin(), siblings.end(), this );

      if( thisIter > targetIter )
      {
        siblings.erase(thisIter); // this only invalidates iterators at or after this point.
        siblings.insert(targetIter, thisPtr);
      }

      Dali::Actor handle( this );
      mParent->mChildOrderChangedSignal.Emit( handle );

      RequestRebuildDepthTree();
    }
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::SetInheritLayoutDirection( bool inherit )
{
  if( mInheritLayoutDirection != inherit )
  {
    mInheritLayoutDirection = inherit;

    if( inherit && mParent )
    {
      InheritLayoutDirectionRecursively( this, mParent->mLayoutDirection );
    }
  }
}

void Actor::InheritLayoutDirectionRecursively( ActorPtr actor, Dali::LayoutDirection::Type direction, bool set )
{
  if( actor && ( actor->mInheritLayoutDirection || set ) )
  {
    if( actor->mLayoutDirection != direction )
    {
      actor->mLayoutDirection = direction;
      actor->EmitLayoutDirectionChangedSignal( direction );
      actor->RelayoutRequest();
    }

    if( actor->GetChildCount() > 0 )
    {
      for( ActorPtr& child : actor->GetChildrenInternal() )
      {
        InheritLayoutDirectionRecursively( child, direction );
      }
    }
  }
}

void Actor::SetUpdateSizeHint( const Vector2& updateSizeHint )
{
  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyMessage<Vector3>::Send( GetEventThreadServices(), &GetNode(), &GetNode().mUpdateSizeHint, &AnimatableProperty<Vector3>::Bake, Vector3(updateSizeHint.width, updateSizeHint.height, 0.f ) );
}

} // namespace Internal

} // namespace Dali
