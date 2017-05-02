/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/type-info-impl.h>
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

using Dali::Internal::SceneGraph::Node;
using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::PropertyBase;

namespace Dali
{

namespace Internal
{

unsigned int Actor::mActorCounter = 0;

namespace
{
/// Using a function because of library initialisation order. Vector3::ONE may not have been initialised yet.
inline const Vector3& GetDefaultSizeModeFactor()
{
  return Vector3::ONE;
}

/// Using a function because of library initialisation order. Vector2::ZERO may not have been initialised yet.
inline const Vector2& GetDefaultPreferredSize()
{
  return Vector2::ZERO;
}

/// Using a function because of library initialisation order. Vector2::ZERO may not have been initialised yet.
inline const Vector2& GetDefaultDimensionPadding()
{
  return Vector2::ZERO;
}

const SizeScalePolicy::Type DEFAULT_SIZE_SCALE_POLICY = SizeScalePolicy::USE_SIZE_SET;

int GetSiblingOrder( ActorPtr actor )
{
  Property::Value value  = actor->GetProperty(Dali::DevelActor::Property::SIBLING_ORDER );
  int order;
  value.Get( order );
  return order;
}

bool ValidateActors( const Internal::Actor& actor, const Internal::Actor& target )
{
  bool validTarget = true;

  if( &actor == &target )
  {
    DALI_LOG_WARNING( "Source actor and target actor can not be the same, Sibling order not changed.\n" );
    validTarget = false;
  }
  else if( actor.GetParent() != target.GetParent() )
  {
    DALI_LOG_WARNING( "Source actor and target actor need to have common parent, Sibling order not changed.\n" );
    validTarget = false;
  }

  return validTarget;
}

} // unnamed namespace

/**
 * Struct to collect relayout variables
 */
struct Actor::RelayoutData
{
  RelayoutData()
    : sizeModeFactor( GetDefaultSizeModeFactor() ), preferredSize( GetDefaultPreferredSize() ), sizeSetPolicy( DEFAULT_SIZE_SCALE_POLICY ), relayoutEnabled( false ), insideRelayout( false )
  {
    // Set size negotiation defaults
    for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      resizePolicies[ i ] = ResizePolicy::DEFAULT;
      negotiatedDimensions[ i ] = 0.0f;
      dimensionNegotiated[ i ] = false;
      dimensionDirty[ i ] = false;
      dimensionDependencies[ i ] = Dimension::ALL_DIMENSIONS;
      dimensionPadding[ i ] = GetDefaultDimensionPadding();
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
DALI_PROPERTY( "colorMode",                 STRING,   true,  false, false, Dali::Actor::Property::COLOR_MODE )
DALI_PROPERTY( "positionInheritance",       STRING,   true,  false, false, Dali::Actor::Property::POSITION_INHERITANCE )
DALI_PROPERTY( "drawMode",                  STRING,   true,  false, false, Dali::Actor::Property::DRAW_MODE )
DALI_PROPERTY( "sizeModeFactor",            VECTOR3,  true,  false, false, Dali::Actor::Property::SIZE_MODE_FACTOR )
DALI_PROPERTY( "widthResizePolicy",         STRING,   true,  false, false, Dali::Actor::Property::WIDTH_RESIZE_POLICY )
DALI_PROPERTY( "heightResizePolicy",        STRING,   true,  false, false, Dali::Actor::Property::HEIGHT_RESIZE_POLICY )
DALI_PROPERTY( "sizeScalePolicy",           STRING,   true,  false, false, Dali::Actor::Property::SIZE_SCALE_POLICY )
DALI_PROPERTY( "widthForHeight",            BOOLEAN,  true,  false, false, Dali::Actor::Property::WIDTH_FOR_HEIGHT )
DALI_PROPERTY( "heightForWidth",            BOOLEAN,  true,  false, false, Dali::Actor::Property::HEIGHT_FOR_WIDTH )
DALI_PROPERTY( "padding",                   VECTOR4,  true,  false, false, Dali::Actor::Property::PADDING )
DALI_PROPERTY( "minimumSize",               VECTOR2,  true,  false, false, Dali::Actor::Property::MINIMUM_SIZE )
DALI_PROPERTY( "maximumSize",               VECTOR2,  true,  false, false, Dali::Actor::Property::MAXIMUM_SIZE )
DALI_PROPERTY( "inheritPosition",           BOOLEAN,  true,  false, false, Dali::Actor::Property::INHERIT_POSITION )
DALI_PROPERTY( "clippingMode",              STRING,   true,  false, false, Dali::Actor::Property::CLIPPING_MODE )
DALI_PROPERTY( "siblingOrder",              INTEGER,  true,  false, false, Dali::DevelActor::Property::SIBLING_ORDER )
DALI_PROPERTY( "opacity",                   FLOAT,    true,  true,  true,  Dali::DevelActor::Property::OPACITY )
DALI_PROPERTY( "screenPosition",            VECTOR2,  false, false, false, Dali::DevelActor::Property::SCREEN_POSITION )
DALI_PROPERTY( "positionUsesAnchorPoint",   BOOLEAN,  true,  false, false, Dali::DevelActor::Property::POSITION_USES_ANCHOR_POINT )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

// Signals

const char* const SIGNAL_TOUCHED = "touched";
const char* const SIGNAL_HOVERED = "hovered";
const char* const SIGNAL_WHEEL_EVENT = "wheelEvent";
const char* const SIGNAL_ON_STAGE = "onStage";
const char* const SIGNAL_OFF_STAGE = "offStage";
const char* const SIGNAL_ON_RELAYOUT = "onRelayout";
const char* const SIGNAL_TOUCH = "touch";

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
SignalConnectorType signalConnector3( mType, SIGNAL_WHEEL_EVENT, &Actor::DoConnectSignal );
SignalConnectorType signalConnector4( mType, SIGNAL_ON_STAGE, &Actor::DoConnectSignal );
SignalConnectorType signalConnector5( mType, SIGNAL_OFF_STAGE, &Actor::DoConnectSignal );
SignalConnectorType signalConnector6( mType, SIGNAL_ON_RELAYOUT, &Actor::DoConnectSignal );
SignalConnectorType signalConnector7( mType, SIGNAL_TOUCH, &Actor::DoConnectSignal );

TypeAction a1( mType, ACTION_SHOW, &Actor::DoAction );
TypeAction a2( mType, ACTION_HIDE, &Actor::DoAction );

struct AnchorValue
{
  const char* name;
  const Vector3& value;
};

DALI_ENUM_TO_STRING_TABLE_BEGIN_WITH_TYPE( AnchorValue, ANCHOR_CONSTANT )
DALI_ENUM_TO_STRING_WITH_SCOPE( AnchorPoint, TOP_LEFT )
DALI_ENUM_TO_STRING_WITH_SCOPE( AnchorPoint, TOP_CENTER )
DALI_ENUM_TO_STRING_WITH_SCOPE( AnchorPoint, TOP_RIGHT )
DALI_ENUM_TO_STRING_WITH_SCOPE( AnchorPoint, CENTER_LEFT )
DALI_ENUM_TO_STRING_WITH_SCOPE( AnchorPoint, CENTER )
DALI_ENUM_TO_STRING_WITH_SCOPE( AnchorPoint, CENTER_RIGHT )
DALI_ENUM_TO_STRING_WITH_SCOPE( AnchorPoint, BOTTOM_LEFT )
DALI_ENUM_TO_STRING_WITH_SCOPE( AnchorPoint, BOTTOM_CENTER )
DALI_ENUM_TO_STRING_WITH_SCOPE( AnchorPoint, BOTTOM_RIGHT )
DALI_ENUM_TO_STRING_TABLE_END( ANCHOR_CONSTANT )

DALI_ENUM_TO_STRING_TABLE_BEGIN( COLOR_MODE )
DALI_ENUM_TO_STRING( USE_OWN_COLOR )
DALI_ENUM_TO_STRING( USE_PARENT_COLOR )
DALI_ENUM_TO_STRING( USE_OWN_MULTIPLY_PARENT_COLOR )
DALI_ENUM_TO_STRING( USE_OWN_MULTIPLY_PARENT_ALPHA )
DALI_ENUM_TO_STRING_TABLE_END( COLOR_MODE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( POSITION_INHERITANCE_MODE )
DALI_ENUM_TO_STRING( INHERIT_PARENT_POSITION )
DALI_ENUM_TO_STRING( USE_PARENT_POSITION )
DALI_ENUM_TO_STRING( USE_PARENT_POSITION_PLUS_LOCAL_POSITION )
DALI_ENUM_TO_STRING( DONT_INHERIT_POSITION )
DALI_ENUM_TO_STRING_TABLE_END( POSITION_INHERITANCE_MODE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( DRAW_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( DrawMode, NORMAL )
DALI_ENUM_TO_STRING_WITH_SCOPE( DrawMode, OVERLAY_2D )
DALI_ENUM_TO_STRING_WITH_SCOPE( DrawMode, STENCIL )
DALI_ENUM_TO_STRING_TABLE_END( DRAW_MODE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( RESIZE_POLICY )
DALI_ENUM_TO_STRING_WITH_SCOPE( ResizePolicy, FIXED )
DALI_ENUM_TO_STRING_WITH_SCOPE( ResizePolicy, USE_NATURAL_SIZE )
DALI_ENUM_TO_STRING_WITH_SCOPE( ResizePolicy, FILL_TO_PARENT )
DALI_ENUM_TO_STRING_WITH_SCOPE( ResizePolicy, SIZE_RELATIVE_TO_PARENT )
DALI_ENUM_TO_STRING_WITH_SCOPE( ResizePolicy, SIZE_FIXED_OFFSET_FROM_PARENT )
DALI_ENUM_TO_STRING_WITH_SCOPE( ResizePolicy, FIT_TO_CHILDREN )
DALI_ENUM_TO_STRING_WITH_SCOPE( ResizePolicy, DIMENSION_DEPENDENCY )
DALI_ENUM_TO_STRING_WITH_SCOPE( ResizePolicy, USE_ASSIGNED_SIZE )
DALI_ENUM_TO_STRING_TABLE_END( RESIZE_POLICY )

DALI_ENUM_TO_STRING_TABLE_BEGIN( SIZE_SCALE_POLICY )
DALI_ENUM_TO_STRING_WITH_SCOPE( SizeScalePolicy, USE_SIZE_SET )
DALI_ENUM_TO_STRING_WITH_SCOPE( SizeScalePolicy, FIT_WITH_ASPECT_RATIO )
DALI_ENUM_TO_STRING_WITH_SCOPE( SizeScalePolicy, FILL_WITH_ASPECT_RATIO )
DALI_ENUM_TO_STRING_TABLE_END( SIZE_SCALE_POLICY )

DALI_ENUM_TO_STRING_TABLE_BEGIN( CLIPPING_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( ClippingMode, DISABLED )
DALI_ENUM_TO_STRING_WITH_SCOPE( ClippingMode, CLIP_CHILDREN )
DALI_ENUM_TO_STRING_TABLE_END( CLIPPING_MODE )


bool GetAnchorPointConstant( const std::string& value, Vector3& anchor )
{
  for( unsigned int i = 0; i < ANCHOR_CONSTANT_TABLE_COUNT; ++i )
  {
    size_t sizeIgnored = 0;
    if( CompareTokens( value.c_str(), ANCHOR_CONSTANT_TABLE[ i ].name, sizeIgnored ) )
    {
      anchor = ANCHOR_CONSTANT_TABLE[ i ].value;
      return true;
    }
  }
  return false;
}

inline bool GetParentOriginConstant( const std::string& value, Vector3& parentOrigin )
{
  // Values are the same so just use the same table as anchor-point
  return GetAnchorPointConstant( value, parentOrigin );
}

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

unsigned int GetDepthIndex( uint16_t depth, uint16_t siblingOrder )
{
  return depth * Dali::DevelLayer::ACTOR_DEPTH_MULTIPLIER + siblingOrder * Dali::DevelLayer::SIBLING_ORDER_MULTIPLIER;
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
      ActorContainer& children = actor->GetChildrenInternal();
      for( ActorIter iter = children.begin(), endIter = children.end(); iter != endIter; ++iter )
      {
        EmitVisibilityChangedSignalRecursively( *iter, visible, DevelActor::VisibilityChange::PARENT );
      }
    }
  }
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
      mChildren->push_back( ActorPtr( &child ) );

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
      actor->SetParent( NULL );

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

ActorPtr Actor::GetChildAt( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < GetChildCount() );

  return ( ( mChildren ) ? ( *mChildren )[ index ] : ActorPtr() );
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
      child = (*iter)->FindChildByName( actorName );

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
    SceneGraph::NodeTransformPropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::Bake, position );
  }
}

void Actor::SetX( float x )
{
  mTargetPosition.x = x;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeX, x );
  }
}

void Actor::SetY( float y )
{
  mTargetPosition.y = y;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeY, y );
  }
}

void Actor::SetZ( float z )
{
  mTargetPosition.z = z;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeZ, z );
  }
}

void Actor::TranslateBy( const Vector3& distance )
{
  mTargetPosition += distance;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformPropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeRelative, distance );
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

const Vector2 Actor::GetCurrentScreenPosition() const
{
  if( OnStage() && NULL != mNode )
  {
    StagePtr stage = Stage::GetCurrent();
    Vector3 worldPosition =  mNode->GetWorldPosition( GetEventThreadServices().GetEventBufferIndex() );
    Vector3 actorSize = GetCurrentSize() * GetCurrentScale();
    Vector2 halfStageSize( stage->GetSize() * 0.5f ); // World position origin is center of stage
    Vector3 halfActorSize( actorSize * 0.5f );
    Vector3 anchorPointOffSet = halfActorSize - actorSize * ( mPositionUsesAnchorPoint ? GetCurrentAnchorPoint() : AnchorPoint::TOP_LEFT );

    return Vector2( halfStageSize.width + worldPosition.x - anchorPointOffSet.x,
                    halfStageSize.height + worldPosition.y - anchorPointOffSet.y );
  }

  return Vector2::ZERO;
}

void Actor::SetPositionInheritanceMode( PositionInheritanceMode mode )
{
  // this flag is not animatable so keep the value
  mPositionInheritanceMode = mode;
  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value
    SetInheritPositionMessage( GetEventThreadServices(), *mNode, mode == INHERIT_PARENT_POSITION );
  }
}

PositionInheritanceMode Actor::GetPositionInheritanceMode() const
{
  // Cached for event-thread access
  return mPositionInheritanceMode;
}

void Actor::SetInheritPosition( bool inherit )
{
  if( mInheritPosition != inherit && NULL != mNode )
  {
    // non animateable so keep local copy
    mInheritPosition = inherit;
    SetInheritPositionMessage( GetEventThreadServices(), *mNode, inherit );
  }
}

bool Actor::IsPositionInherited() const
{
  return mInheritPosition;
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

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformPropertyMessage<Quaternion>::Send( GetEventThreadServices(), mNode, &mNode->mOrientation, &SceneGraph::TransformManagerPropertyHandler<Quaternion>::Bake, orientation );
  }
}

void Actor::RotateBy( const Radian& angle, const Vector3& axis )
{
  RotateBy( Quaternion(angle, axis) );
}

void Actor::RotateBy( const Quaternion& relativeRotation )
{
  mTargetOrientation *= Quaternion( relativeRotation );

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformPropertyMessage<Quaternion>::Send( GetEventThreadServices(), mNode, &mNode->mOrientation, &SceneGraph::TransformManagerPropertyHandler<Quaternion>::BakeRelative, relativeRotation );
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
  mTargetScale = scale;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformPropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::Bake, scale );
  }
}

void Actor::SetScaleX( float x )
{
  mTargetScale.x = x;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeX, x );
  }
}

void Actor::SetScaleY( float y )
{
  mTargetScale.y = y;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeY, y );
  }
}

void Actor::SetScaleZ( float z )
{
  mTargetScale.z = z;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeZ, z );
  }
}

void Actor::ScaleBy(const Vector3& relativeScale)
{
  mTargetScale *= relativeScale;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformPropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeRelativeMultiply, relativeScale );
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

  if( mInheritScale != inherit && NULL != mNode )
  {
    // non animateable so keep local copy
    mInheritScale = inherit;
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
    return mNode->GetWorldMatrix(0);
  }

  return Matrix::IDENTITY;
}

void Actor::SetVisible( bool visible )
{
  if( mVisible != visible )
  {
    if( NULL != mNode )
    {
      // mNode is being used in a separate thread; queue a message to set the value & base value
      SceneGraph::NodePropertyMessage<bool>::Send( GetEventThreadServices(), mNode, &mNode->mVisible, &AnimatableProperty<bool>::Bake, visible );
    }

    mVisible = visible;

    // Emit the signal on this actor and all its children
    EmitVisibilityChangedSignalRecursively( this, visible, DevelActor::VisibilityChange::SELF );
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
  mTargetColor.a = opacity;

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

ClippingMode::Type Actor::GetClippingMode() const
{
  return mClippingMode;
}

unsigned int Actor::GetSortingDepth()
{
  return GetDepthIndex( mDepth, mSiblingOrder );
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
  mTargetColor = color;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyMessage<Vector4>::Send( GetEventThreadServices(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::Bake, color );
  }
}

void Actor::SetColorRed( float red )
{
  mTargetColor.r = red;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeX, red );
  }
}

void Actor::SetColorGreen( float green )
{
  mTargetColor.g = green;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), mNode, &mNode->mColor, &AnimatableProperty<Vector4>::BakeY, green );
  }
}

void Actor::SetColorBlue( float blue )
{
  mTargetColor.b = blue;

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
  if( mInheritOrientation != inherit && NULL != mNode)
  {
    // non animateable so keep local copy
    mInheritOrientation = inherit;
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
  if ( mRelayoutData )
  {
    return mRelayoutData->sizeModeFactor;
  }

  return GetDefaultSizeModeFactor();
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
  if( ( NULL != mNode )&&
      ( ( fabsf( mTargetSize.width - size.width  ) > Math::MACHINE_EPSILON_1 )||
        ( fabsf( mTargetSize.height- size.height ) > Math::MACHINE_EPSILON_1 )||
        ( fabsf( mTargetSize.depth - size.depth  ) > Math::MACHINE_EPSILON_1 ) ) )
  {
    mTargetSize = size;

    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformPropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::Bake, mTargetSize );

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

void Actor::NotifySizeAnimation( Animation& animation, const Vector3& targetSize )
{
  mTargetSize = targetSize;

  // Notify deriving classes
  OnSizeAnimation( animation, mTargetSize );
}

void Actor::NotifySizeAnimation( Animation& animation, float targetSize, Property::Index property )
{
  if ( Dali::Actor::Property::SIZE_WIDTH == property )
  {
    mTargetSize.width = targetSize;
  }
  else if ( Dali::Actor::Property::SIZE_HEIGHT == property )
  {
    mTargetSize.height = targetSize;
  }
  else if ( Dali::Actor::Property::SIZE_DEPTH == property )
  {
    mTargetSize.depth = targetSize;
  }
  // Notify deriving classes
  OnSizeAnimation( animation, mTargetSize );
}

void Actor::NotifyPositionAnimation( Animation& animation, const Vector3& targetPosition )
{
  mTargetPosition = targetPosition;
}

void Actor::NotifyPositionAnimation( Animation& animation, float targetPosition, Property::Index property )
{
  if ( Dali::Actor::Property::POSITION_X == property )
  {
    mTargetPosition.x = targetPosition;
  }
  else if ( Dali::Actor::Property::POSITION_Y == property )
  {
    mTargetPosition.y = targetPosition;
  }
  else if ( Dali::Actor::Property::POSITION_Z == property )
  {
    mTargetPosition.z = targetPosition;
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

    if( NULL != mNode )
    {
      // mNode is being used in a separate thread; queue a message to set the value & base value
      SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeX, width );
    }
  }

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

    if( NULL != mNode )
    {
      // mNode is being used in a separate thread; queue a message to set the value & base value
      SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeY, height );
    }
  }

  RelayoutRequest();
}

void Actor::SetDepth( float depth )
{
  mTargetSize.depth = depth;

  if( NULL != mNode )
  {
    // mNode is being used in a separate thread; queue a message to set the value & base value
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, &mNode->mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeZ, depth );
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

  ResizePolicy::Type originalWidthPolicy = GetResizePolicy(Dimension::WIDTH);
  ResizePolicy::Type originalHeightPolicy = GetResizePolicy(Dimension::HEIGHT);

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

  // If the resize policy is set to be FIXED, the preferred size
  // should be overrided by the target size. Otherwise the target
  // size should be overrided by the preferred size.

  if( dimension & Dimension::WIDTH )
  {
    if( originalWidthPolicy != ResizePolicy::FIXED && policy == ResizePolicy::FIXED )
    {
      mRelayoutData->preferredSize.width = mTargetSize.width;
    }
    else if( originalWidthPolicy == ResizePolicy::FIXED && policy != ResizePolicy::FIXED )
    {
      mTargetSize.width = mRelayoutData->preferredSize.width;
    }
  }

  if( dimension & Dimension::HEIGHT )
  {
    if( originalHeightPolicy != ResizePolicy::FIXED && policy == ResizePolicy::FIXED )
    {
      mRelayoutData->preferredSize.height = mTargetSize.height;
    }
    else if( originalHeightPolicy == ResizePolicy::FIXED && policy != ResizePolicy::FIXED )
    {
      mTargetSize.height = mRelayoutData->preferredSize.height;
    }
  }

  OnSetResizePolicy( policy, dimension );

  // Trigger relayout on this control
  RelayoutRequest();
}

ResizePolicy::Type Actor::GetResizePolicy( Dimension::Type dimension ) const
{
  if ( mRelayoutData )
  {
    // If more than one dimension is requested, just return the first one found
    for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      if( ( dimension & ( 1 << i ) ) )
      {
        return mRelayoutData->resizePolicies[ i ];
      }
    }
  }

  return ResizePolicy::DEFAULT;
}

void Actor::SetSizeScalePolicy( SizeScalePolicy::Type policy )
{
  EnsureRelayoutData();

  mRelayoutData->sizeSetPolicy = policy;
}

SizeScalePolicy::Type Actor::GetSizeScalePolicy() const
{
  if ( mRelayoutData )
  {
    return mRelayoutData->sizeSetPolicy;
  }

  return DEFAULT_SIZE_SCALE_POLICY;
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
  if ( mRelayoutData )
  {
    // If more than one dimension is requested, just return the first one found
    for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      if( ( dimension & ( 1 << i ) ) )
      {
        return mRelayoutData->dimensionDependencies[ i ];
      }
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
  if ( mRelayoutData )
  {
    for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      if( ( dimension & ( 1 << i ) ) && mRelayoutData->dimensionDirty[ i ] )
      {
        return true;
      }
    }
  }

  return false;
}

bool Actor::RelayoutPossible( Dimension::Type dimension ) const
{
  return mRelayoutData && mRelayoutData->relayoutEnabled && !IsLayoutDirty( dimension );
}

bool Actor::RelayoutRequired( Dimension::Type dimension ) const
{
  return mRelayoutData && mRelayoutData->relayoutEnabled && IsLayoutDirty( dimension );
}

unsigned int Actor::AddRenderer( Renderer& renderer )
{
  if( !mRenderers )
  {
    mRenderers = new RendererContainer;
  }

  unsigned int index = mRenderers->size();
  RendererPtr rendererPtr = RendererPtr( &renderer );
  mRenderers->push_back( rendererPtr );
  AddRendererMessage( GetEventThreadServices(), *mNode, renderer.GetRendererSceneObject() );
  return index;
}

unsigned int Actor::GetRendererCount() const
{
  unsigned int rendererCount(0);
  if( mRenderers )
  {
    rendererCount = mRenderers->size();
  }

  return rendererCount;
}

RendererPtr Actor::GetRendererAt( unsigned int index )
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
        RemoveRendererMessage( GetEventThreadServices(), *mNode, renderer.GetRendererSceneObject() );
        break;
      }
    }
  }
}

void Actor::RemoveRenderer( unsigned int index )
{
  if( index < GetRendererCount() )
  {
    RendererPtr renderer = ( *mRenderers )[ index ];
    RemoveRendererMessage( GetEventThreadServices(), *mNode, renderer.Get()->GetRendererSceneObject() );
    mRenderers->erase( mRenderers->begin()+index );
  }
}

bool Actor::IsOverlay() const
{
  return ( DrawMode::OVERLAY_2D == mDrawMode );
}

void Actor::SetDrawMode( DrawMode::Type drawMode )
{
  // this flag is not animatable so keep the value
  mDrawMode = drawMode;
  if( ( NULL != mNode ) && ( drawMode != DrawMode::STENCIL ) )
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
  StagePtr stage = Stage::GetCurrent();
  if( stage && OnStage() )
  {
    const RenderTaskList& taskList = stage->GetRenderTaskList();

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

bool Actor::ScreenToLocal( const RenderTask& renderTask, float& localX, float& localY, float screenX, float screenY ) const
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

  // Get the ModelView matrix
  Matrix modelView;
  Matrix::Multiply( modelView, mNode->GetWorldMatrix(0), viewMatrix );

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

bool Actor::RayActorTest( const Vector4& rayOrigin, const Vector4& rayDir, Vector2& hitPointLocal, float& distance ) const
{
  bool hit = false;

  if( OnStage() && NULL != mNode )
  {
    // Transforms the ray to the local reference system.
    // Calculate the inverse of Model matrix
    Matrix invModelMatrix( false/*don't init*/);

    BufferIndex bufferIndex( GetEventThreadServices().GetEventBufferIndex() );
    invModelMatrix = mNode->GetWorldMatrix(0);
    invModelMatrix.Invert();

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
  return !mTouchedSignal.Empty() || !mTouchSignal.Empty() || mDerivedRequiresTouch;
}

bool Actor::GetHoverRequired() const
{
  return !mHoveredSignal.Empty() || mDerivedRequiresHover;
}

bool Actor::GetWheelEventRequired() const
{
  return !mWheelEventSignal.Empty() || mDerivedRequiresWheelEvent;
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

bool Actor::EmitTouchEventSignal( const TouchEvent& event, const Dali::TouchData& touch )
{
  bool consumed = false;

  if( !mTouchSignal.Empty() )
  {
    Dali::Actor handle( this );
    consumed = mTouchSignal.Emit( handle, touch );
  }

  if( !mTouchedSignal.Empty() )
  {
    Dali::Actor handle( this );
    consumed |= mTouchedSignal.Emit( handle, event );
  }

  if( !consumed )
  {
    // Notification for derived classes
    consumed = OnTouchEvent( event ); // TODO
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

bool Actor::EmitWheelEventSignal( const WheelEvent& event )
{
  bool consumed = false;

  if( !mWheelEventSignal.Empty() )
  {
    Dali::Actor handle( this );
    consumed = mWheelEventSignal.Emit( handle, event );
  }

  if( !consumed )
  {
    // Notification for derived classes
    consumed = OnWheelEvent( event );
  }

  return consumed;
}

void Actor::EmitVisibilityChangedSignal( bool visible, DevelActor::VisibilityChange::Type type )
{
  if( ! mVisibilityChangedSignal.Empty() )
  {
    Dali::Actor handle( this );
    mVisibilityChangedSignal.Emit( handle, visible, type );
  }
}

Dali::Actor::TouchSignalType& Actor::TouchedSignal()
{
  return mTouchedSignal;
}

Dali::Actor::TouchDataSignalType& Actor::TouchSignal()
{
  return mTouchSignal;
}

Dali::Actor::HoverSignalType& Actor::HoveredSignal()
{
  return mHoveredSignal;
}

Dali::Actor::WheelEventSignalType& Actor::WheelEventSignal()
{
  return mWheelEventSignal;
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

DevelActor::VisibilityChangedSignalType& Actor::VisibilityChangedSignal()
{
  return mVisibilityChangedSignal;
}

bool Actor::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  Actor* actor = static_cast< Actor* >( object ); // TypeRegistry guarantees that this is the correct type.

  if( 0 == signalName.compare( SIGNAL_TOUCHED ) )
  {
    actor->TouchedSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_HOVERED ) )
  {
    actor->HoveredSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_WHEEL_EVENT ) )
  {
    actor->WheelEventSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_ON_STAGE ) )
  {
    actor->OnStageSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_OFF_STAGE ) )
  {
    actor->OffStageSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_ON_RELAYOUT ) )
  {
    actor->OnRelayoutSignal().Connect( tracker, functor );
  }
  else if( 0 == signalName.compare( SIGNAL_TOUCH ) )
  {
    actor->TouchSignal().Connect( tracker, functor );
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
  mRenderers( NULL ),
  mNode( NULL ),
  mParentOrigin( NULL ),
  mAnchorPoint( NULL ),
  mRelayoutData( NULL ),
  mGestureData( NULL ),
  mTargetOrientation( Quaternion::IDENTITY ),
  mTargetColor( Color::WHITE ),
  mTargetSize( Vector3::ZERO ),
  mTargetPosition( Vector3::ZERO ),
  mTargetScale( Vector3::ONE ),
  mName(),
  mId( ++mActorCounter ), // actor ID is initialised to start from 1, and 0 is reserved
  mDepth( 0u ),
  mSiblingOrder(0u),
  mIsRoot( ROOT_LAYER == derivedType ),
  mIsLayer( LAYER == derivedType || ROOT_LAYER == derivedType ),
  mIsOnStage( false ),
  mSensitive( true ),
  mLeaveRequired( false ),
  mKeyboardFocusable( false ),
  mDerivedRequiresTouch( false ),
  mDerivedRequiresHover( false ),
  mDerivedRequiresWheelEvent( false ),
  mOnStageSignalled( false ),
  mInsideOnSizeSet( false ),
  mInheritPosition( true ),
  mInheritOrientation( true ),
  mInheritScale( true ),
  mPositionUsesAnchorPoint( true ),
  mVisible( true ),
  mDrawMode( DrawMode::NORMAL ),
  mPositionInheritanceMode( Node::DEFAULT_POSITION_INHERITANCE_MODE ),
  mColorMode( Node::DEFAULT_COLOR_MODE ),
  mClippingMode( ClippingMode::DISABLED )
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
      (*iter)->SetParent( NULL );
    }
  }
  delete mChildren;
  delete mRenderers;

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

void Actor::ConnectToStage( unsigned int parentDepth )
{
  // This container is used instead of walking the Actor hierarchy.
  // It protects us when the Actor hierarchy is modified during OnStageConnectionExternal callbacks.
  ActorContainer connectionList;

  // This stage is atomic i.e. not interrupted by user callbacks.
  RecursiveConnectToStage( connectionList, parentDepth + 1 );

  // Notify applications about the newly connected actors.
  const ActorIter endIter = connectionList.end();
  for( ActorIter iter = connectionList.begin(); iter != endIter; ++iter )
  {
    (*iter)->NotifyStageConnection();
  }

  RelayoutRequest();
}

void Actor::RecursiveConnectToStage( ActorContainer& connectionList, unsigned int depth )
{
  DALI_ASSERT_ALWAYS( !OnStage() );

  mIsOnStage = true;
  mDepth = depth;
  SetDepthIndexMessage( GetEventThreadServices(), *mNode, GetDepthIndex( mDepth, mSiblingOrder ) );

  ConnectToSceneGraph();

  // Notification for internal derived classes
  OnStageConnectionInternal();

  // This stage is atomic; avoid emitting callbacks until all Actors are connected
  connectionList.push_back( ActorPtr( this ) );

  // Recursively connect children
  if( mChildren )
  {
    ActorConstIter endIter = mChildren->end();
    for( ActorIter iter = mChildren->begin(); iter != endIter; ++iter )
    {
      (*iter)->RecursiveConnectToStage( connectionList, depth+1 );
    }
  }
}

/**
 * This method is called when the Actor is connected to the Stage.
 * The parent must have added its Node to the scene-graph.
 * The child must connect its Node to the parent's Node.
 * This is recursive; the child calls ConnectToStage() for its children.
 */
void Actor::ConnectToSceneGraph()
{
  DALI_ASSERT_DEBUG( mNode != NULL); DALI_ASSERT_DEBUG( mParent != NULL); DALI_ASSERT_DEBUG( mParent->mNode != NULL );

  if( NULL != mNode )
  {
    // Reparent Node in next Update
    ConnectNodeMessage( GetEventThreadServices().GetUpdateManager(), *(mParent->mNode), *mNode );
  }

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
    OnStageConnectionExternal( mDepth );

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
    (*iter)->NotifyStageDisconnection();
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
      (*iter)->RecursiveDisconnectFromStage( disconnectionList );
    }
  }

  // This stage is atomic; avoid emitting callbacks until all Actors are disconnected
  disconnectionList.push_back( ActorPtr( this ) );

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

  if( OnStage() && ( NULL != mNode ) )
  {
    if( IsRoot() || mNode->GetParent() )
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
  indices.Reserve( DEFAULT_PROPERTY_COUNT );

  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.PushBack( i );
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
    if( 0 == name.compare( property->name ) )
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
      Property::Type type = property.GetType();
      if( type == Property::VECTOR3 )
      {
        SetParentOrigin( property.Get< Vector3 >() );
      }
      else if ( type == Property::STRING )
      {
        std::string parentOriginString;
        property.Get( parentOriginString );
        Vector3 parentOrigin;
        if( GetParentOriginConstant( parentOriginString, parentOrigin ) )
        {
          SetParentOrigin( parentOrigin );
        }
      }
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
      Property::Type type = property.GetType();
      if( type == Property::VECTOR3 )
      {
        SetAnchorPoint( property.Get< Vector3 >() );
      }
      else if ( type == Property::STRING )
      {
        std::string anchorPointString;
        property.Get( anchorPointString );
        Vector3 anchor;
        if( GetAnchorPointConstant( anchorPointString, anchor ) )
        {
          SetAnchorPoint( anchor );
        }
      }
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
    case Dali::DevelActor::Property::OPACITY:
    {
      float value;
      if( property.Get( value ) )
      {
        SetOpacity( value );
      }
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

    case Dali::Actor::Property::INHERIT_POSITION:
    {
      SetInheritPosition( property.Get< bool >() );
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
      ColorMode mode = mColorMode;
      if ( Scripting::GetEnumerationProperty< ColorMode >( property, COLOR_MODE_TABLE, COLOR_MODE_TABLE_COUNT, mode ) )
      {
        SetColorMode( mode );
      }
      break;
    }

    case Dali::Actor::Property::POSITION_INHERITANCE:
    {
      PositionInheritanceMode mode = mPositionInheritanceMode;
      if( Scripting::GetEnumerationProperty< PositionInheritanceMode >( property, POSITION_INHERITANCE_MODE_TABLE, POSITION_INHERITANCE_MODE_TABLE_COUNT, mode ) )
      {
        SetPositionInheritanceMode( mode );
      }
      break;
    }

    case Dali::Actor::Property::DRAW_MODE:
    {
      DrawMode::Type mode = mDrawMode;
      if( Scripting::GetEnumerationProperty< DrawMode::Type >( property, DRAW_MODE_TABLE, DRAW_MODE_TABLE_COUNT, mode ) )
      {
        SetDrawMode( mode );
      }
      break;
    }

    case Dali::Actor::Property::SIZE_MODE_FACTOR:
    {
      SetSizeModeFactor( property.Get< Vector3 >() );
      break;
    }

    case Dali::Actor::Property::WIDTH_RESIZE_POLICY:
    {
      ResizePolicy::Type type = static_cast< ResizePolicy::Type >( -1 ); // Set to invalid number so it definitely gets set.
      if( Scripting::GetEnumerationProperty< ResizePolicy::Type >( property, RESIZE_POLICY_TABLE, RESIZE_POLICY_TABLE_COUNT, type ) )
      {
        SetResizePolicy( type, Dimension::WIDTH );
      }
      break;
    }

    case Dali::Actor::Property::HEIGHT_RESIZE_POLICY:
    {
      ResizePolicy::Type type = static_cast< ResizePolicy::Type >( -1 ); // Set to invalid number so it definitely gets set.
      if( Scripting::GetEnumerationProperty< ResizePolicy::Type >( property, RESIZE_POLICY_TABLE, RESIZE_POLICY_TABLE_COUNT, type ) )
      {
        SetResizePolicy( type, Dimension::HEIGHT );
      }
      break;
    }

    case Dali::Actor::Property::SIZE_SCALE_POLICY:
    {
      SizeScalePolicy::Type type;
      if( Scripting::GetEnumeration< SizeScalePolicy::Type >( property.Get< std::string >().c_str(), SIZE_SCALE_POLICY_TABLE, SIZE_SCALE_POLICY_TABLE_COUNT, type ) )
      {
        SetSizeScalePolicy( type );
      }
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

    case Dali::DevelActor::Property::SIBLING_ORDER:
    {
      int value;

      if( property.Get( value ) )
      {
        if( static_cast<unsigned int>(value) != mSiblingOrder )
        {
          SetSiblingOrder( value );
        }
      }
      break;
    }

    case Dali::Actor::Property::CLIPPING_MODE:
    {
      ClippingMode::Type convertedValue = mClippingMode;
      if( Scripting::GetEnumerationProperty< ClippingMode::Type >( property, CLIPPING_MODE_TABLE, CLIPPING_MODE_TABLE_COUNT, convertedValue ) )
      {
        mClippingMode = convertedValue;
        if( NULL != mNode )
        {
          SetClippingModeMessage( GetEventThreadServices(), *mNode, mClippingMode );
        }
      }
      break;
    }

    case Dali::DevelActor::Property::POSITION_USES_ANCHOR_POINT:
    {
      bool value = false;
      if( property.Get( value ) && value != mPositionUsesAnchorPoint )
      {
        mPositionUsesAnchorPoint = value;
        if( NULL != mNode )
        {
          SetPositionUsesAnchorPointMessage( GetEventThreadServices(), *mNode, mPositionUsesAnchorPoint );
        }
      }
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
  switch( entry.GetType() )
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
      if(entry.componentIndex == 0)
      {
        SceneGraph::NodePropertyComponentMessage<Vector2>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector2>::BakeX, value.Get<float>() );
      }
      else if(entry.componentIndex == 1)
      {
        SceneGraph::NodePropertyComponentMessage<Vector2>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector2>::BakeY, value.Get<float>() );
      }
      else
      {
        SceneGraph::NodePropertyMessage<Vector2>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector2>::Bake, value.Get<Vector2>() );
      }

      break;
    }

    case Property::VECTOR3:
    {
      const AnimatableProperty< Vector3 >* property = dynamic_cast< const AnimatableProperty< Vector3 >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      if(entry.componentIndex == 0)
      {
        SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector3>::BakeX, value.Get<float>() );
      }
      else if(entry.componentIndex == 1)
      {
        SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector3>::BakeY, value.Get<float>() );
      }
      else if(entry.componentIndex == 2)
      {
        SceneGraph::NodePropertyComponentMessage<Vector3>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector3>::BakeZ, value.Get<float>() );
      }
      else
      {
        SceneGraph::NodePropertyMessage<Vector3>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector3>::Bake, value.Get<Vector3>() );
      }

      break;
    }

    case Property::VECTOR4:
    {
      const AnimatableProperty< Vector4 >* property = dynamic_cast< const AnimatableProperty< Vector4 >* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      if(entry.componentIndex == 0)
      {
        SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector4>::BakeX, value.Get<float>() );
      }
      else if(entry.componentIndex == 1)
      {
        SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector4>::BakeY, value.Get<float>() );
      }
      else if(entry.componentIndex == 2)
      {
        SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector4>::BakeZ, value.Get<float>() );
      }
      else if(entry.componentIndex == 3)
      {
        SceneGraph::NodePropertyComponentMessage<Vector4>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector4>::BakeW, value.Get<float>() );
      }
      else
      {
        SceneGraph::NodePropertyMessage<Vector4>::Send( GetEventThreadServices(), mNode, property, &AnimatableProperty<Vector4>::Bake, value.Get<Vector4>() );
      }

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
      // nothing to do for other types
    }
  } // entry.GetType
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
    AnimatablePropertyMetadata* animatable = RegisterAnimatableProperty( index );
    DALI_ASSERT_ALWAYS( animatable && "Property index is invalid" );

    property = animatable->GetSceneGraphProperty();
  }
  else if ( ( index >= CHILD_PROPERTY_REGISTRATION_START_INDEX ) && // Child properties are also stored as custom properties
            ( index <= PROPERTY_CUSTOM_MAX_INDEX ) )
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
      case Dali::DevelActor::Property::OPACITY:
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
    AnimatablePropertyMetadata* animatable = RegisterAnimatableProperty( index );
    DALI_ASSERT_ALWAYS( animatable && "Property index is invalid" );

    property = animatable->GetSceneGraphProperty();
  }
  else if ( ( index >= CHILD_PROPERTY_REGISTRATION_START_INDEX ) && // Child properties are also stored as custom properties
            ( index <= PROPERTY_CUSTOM_MAX_INDEX ) )
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
      case Dali::DevelActor::Property::OPACITY:
      {
        property = &mNode->mColor;
        break;
      }

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

  if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // check whether the animatable property is registered already, if not then register one.
    AnimatablePropertyMetadata* animatableProperty = RegisterAnimatableProperty(index);
    if( animatableProperty )
    {
      componentIndex = animatableProperty->componentIndex;
    }
  }
  else
  {
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
      case Dali::DevelActor::Property::OPACITY:
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
  }

  return componentIndex;
}

void Actor::SetParent( Actor* parent )
{
  if( parent )
  {
    DALI_ASSERT_ALWAYS( !mParent && "Actor cannot have 2 parents" );

    mParent = parent;

    if ( EventThreadServices::IsCoreRunning() && // Don't emit signals or send messages during Core destruction
         parent->OnStage() )
    {
      // Instruct each actor to create a corresponding node in the scene graph
      ConnectToStage( parent->GetHierarchyDepth() );
    }

    // Resolve the name and index for the child properties if any
    ResolveChildProperties();
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

bool Actor::GetCachedPropertyValue( Property::Index index, Property::Value& value ) const
{
  bool valueSet = true;

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
      Vector3 size = GetTargetSize();

      // Should return preferred size if size is fixed as set by SetSize
      if( GetResizePolicy( Dimension::WIDTH ) == ResizePolicy::FIXED )
      {
        size.width = GetPreferredSize().width;
      }
      if( GetResizePolicy( Dimension::HEIGHT ) == ResizePolicy::FIXED )
      {
        size.height = GetPreferredSize().height;
      }

      value = size;

      break;
    }

    case Dali::Actor::Property::SIZE_WIDTH:
    {
      if( GetResizePolicy( Dimension::WIDTH ) == ResizePolicy::FIXED )
      {
        // Should return preferred size if size is fixed as set by SetSize
        value = GetPreferredSize().width;
      }
      else
      {
        value = GetTargetSize().width;
      }
      break;
    }

    case Dali::Actor::Property::SIZE_HEIGHT:
    {
      if( GetResizePolicy( Dimension::HEIGHT ) == ResizePolicy::FIXED )
      {
        // Should return preferred size if size is fixed as set by SetSize
        value = GetPreferredSize().height;
      }
      else
      {
        value = GetTargetSize().height;
      }
      break;
    }

    case Dali::Actor::Property::SIZE_DEPTH:
    {
      value = GetTargetSize().depth;
      break;
    }

    case Dali::Actor::Property::POSITION:
    {
      value = GetTargetPosition();
      break;
    }

    case Dali::Actor::Property::POSITION_X:
    {
      value = GetTargetPosition().x;
      break;
    }

    case Dali::Actor::Property::POSITION_Y:
    {
      value = GetTargetPosition().y;
      break;
    }

    case Dali::Actor::Property::POSITION_Z:
    {
      value = GetTargetPosition().z;
      break;
    }

    case Dali::Actor::Property::ORIENTATION:
    {
      value = mTargetOrientation;
      break;
    }

    case Dali::Actor::Property::SCALE:
    {
      value = mTargetScale;
      break;
    }

    case Dali::Actor::Property::SCALE_X:
    {
      value = mTargetScale.x;
      break;
    }

    case Dali::Actor::Property::SCALE_Y:
    {
      value = mTargetScale.y;
      break;
    }

    case Dali::Actor::Property::SCALE_Z:
    {
      value = mTargetScale.z;
      break;
    }

    case Dali::Actor::Property::VISIBLE:
    {
      value = mVisible;
      break;
    }

    case Dali::Actor::Property::COLOR:
    {
      value = mTargetColor;
      break;
    }

    case Dali::Actor::Property::COLOR_RED:
    {
      value = mTargetColor.r;
      break;
    }

    case Dali::Actor::Property::COLOR_GREEN:
    {
      value = mTargetColor.g;
      break;
    }

    case Dali::Actor::Property::COLOR_BLUE:
    {
      value = mTargetColor.b;
      break;
    }

    case Dali::Actor::Property::COLOR_ALPHA:
    case Dali::DevelActor::Property::OPACITY:
    {
      value = mTargetColor.a;
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

    case Dali::Actor::Property::INHERIT_POSITION:
    {
      value = IsPositionInherited();
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
      value = Scripting::GetLinearEnumerationName< ColorMode >( GetColorMode(), COLOR_MODE_TABLE, COLOR_MODE_TABLE_COUNT );
      break;
    }

    case Dali::Actor::Property::POSITION_INHERITANCE:
    {
      value = Scripting::GetLinearEnumerationName< PositionInheritanceMode >( GetPositionInheritanceMode(), POSITION_INHERITANCE_MODE_TABLE, POSITION_INHERITANCE_MODE_TABLE_COUNT );
      break;
    }

    case Dali::Actor::Property::DRAW_MODE:
    {
      value = Scripting::GetEnumerationName< DrawMode::Type >( GetDrawMode(), DRAW_MODE_TABLE, DRAW_MODE_TABLE_COUNT );
      break;
    }

    case Dali::Actor::Property::SIZE_MODE_FACTOR:
    {
      value = GetSizeModeFactor();
      break;
    }

    case Dali::Actor::Property::WIDTH_RESIZE_POLICY:
    {
      value = Scripting::GetLinearEnumerationName< ResizePolicy::Type >( GetResizePolicy( Dimension::WIDTH ), RESIZE_POLICY_TABLE, RESIZE_POLICY_TABLE_COUNT );
      break;
    }

    case Dali::Actor::Property::HEIGHT_RESIZE_POLICY:
    {
      value = Scripting::GetLinearEnumerationName< ResizePolicy::Type >( GetResizePolicy( Dimension::HEIGHT ), RESIZE_POLICY_TABLE, RESIZE_POLICY_TABLE_COUNT );
      break;
    }

    case Dali::Actor::Property::SIZE_SCALE_POLICY:
    {
      value = Scripting::GetLinearEnumerationName< SizeScalePolicy::Type >( GetSizeScalePolicy(), SIZE_SCALE_POLICY_TABLE, SIZE_SCALE_POLICY_TABLE_COUNT );
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

    case Dali::Actor::Property::CLIPPING_MODE:
    {
      value = mClippingMode;
      break;
    }

    case Dali::DevelActor::Property::SIBLING_ORDER:
    {
      value = static_cast<int>(mSiblingOrder);
      break;
    }

    case Dali::DevelActor::Property::SCREEN_POSITION:
    {
      value = GetCurrentScreenPosition();
      break;
    }

    case Dali::DevelActor::Property::POSITION_USES_ANCHOR_POINT:
    {
      value = mPositionUsesAnchorPoint;
      break;
    }

    default:
    {
      // Must be a scene-graph only property
      valueSet = false;
      break;
    }
  }

  return valueSet;
}

bool Actor::GetCurrentPropertyValue( Property::Index index, Property::Value& value  ) const
{
  bool valueSet = true;

  switch( index )
  {
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
    case Dali::DevelActor::Property::OPACITY:
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

    default:
    {
      // Must be an event-side only property
      valueSet = false;
      break;
    }
  }

  return valueSet;
}

void Actor::EnsureRelayoutData()
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
  if ( mRelayoutData )
  {
    // If more than one dimension is requested, just return the first one found
    for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      if( ( dimension & ( 1 << i ) ) )
      {
        return mRelayoutData->dimensionPadding[ i ];
      }
    }
  }

  return GetDefaultDimensionPadding();
}

void Actor::SetLayoutNegotiated( bool negotiated, Dimension::Type dimension )
{
  EnsureRelayoutData();

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
  if ( mRelayoutData )
  {
    for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      if( ( dimension & ( 1 << i ) ) && mRelayoutData->dimensionNegotiated[ i ] )
      {
        return true;
      }
    }
  }

  return false;
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

  for( unsigned int i = 0, count = GetChildCount(); i < count; ++i )
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
      break;
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
  // Force a size negotiation for actors that has assigned size during relayout
  // This is required as otherwise the flags that force a relayout will not
  // necessarilly be set. This will occur if the actor has already been laid out.
  // The dirty flags are then cleared. Then if the actor is added back into the
  // relayout container afterwards, the dirty flags would still be clear...
  // causing a relayout to be skipped. Here we force any actors added to the
  // container to be relayed out.
  if(GetResizePolicy(Dimension::WIDTH) == ResizePolicy::USE_ASSIGNED_SIZE)
  {
    SetLayoutNegotiated(false, Dimension::WIDTH);
  }
  if(GetResizePolicy(Dimension::HEIGHT) == ResizePolicy::USE_ASSIGNED_SIZE)
  {
    SetLayoutNegotiated(false, Dimension::HEIGHT);
  }

  // Do the negotiation
  NegotiateDimensions( allocatedSize );

  // Set the actor size
  SetNegotiatedSize( container );

  // Negotiate down to children
  const Vector2 newBounds = GetTargetSize().GetVectorXY();

  for( unsigned int i = 0, count = GetChildCount(); i < count; ++i )
  {
    ActorPtr child = GetChildAt( i );

    // Forces children that have already been laid out to be relayed out
    // if they have assigned size during relayout.
    if(child->GetResizePolicy(Dimension::WIDTH) == ResizePolicy::USE_ASSIGNED_SIZE)
    {
      child->SetLayoutNegotiated(false, Dimension::WIDTH);
      child->SetLayoutDirty(true, Dimension::WIDTH);
    }
    if(child->GetResizePolicy(Dimension::HEIGHT) == ResizePolicy::USE_ASSIGNED_SIZE)
    {
      child->SetLayoutNegotiated(false, Dimension::HEIGHT);
      child->SetLayoutDirty(true, Dimension::HEIGHT);
    }

    // Only relayout if required
    if( child->RelayoutRequired() )
    {
      container.Add( Dali::Actor( child.Get() ), newBounds );
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
  if ( mRelayoutData )
  {
    return Vector2( mRelayoutData->preferredSize );
  }

  return GetDefaultPreferredSize();
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
  if ( mRelayoutData )
  {
    for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      if( dimension & ( 1 << i ) )
      {
        return mRelayoutData->minimumSize[ i ];
      }
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
  if ( mRelayoutData )
  {
    for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      if( dimension & ( 1 << i ) )
      {
        return mRelayoutData->maximumSize[ i ];
      }
    }
  }

  return FLT_MAX;  // Default
}

Object* Actor::GetParentObject() const
{
  return mParent;
}

void Actor::SetSiblingOrder( unsigned int order )
{
  mSiblingOrder = std::min( order, static_cast<unsigned int>( DevelLayer::SIBLING_ORDER_MULTIPLIER ) );
  if( mIsOnStage )
  {
    SetDepthIndexMessage( GetEventThreadServices(), *mNode, GetDepthIndex( mDepth, mSiblingOrder ) );
  }
}

void Actor::DefragmentSiblingIndexes( ActorContainer& siblings )
{
  // Sibling index may not be in consecutive order as the sibling range is limited ( DevelLayer::SIBLING_ORDER_MULTIPLIER )
  // we need to remove the gaps and ensure the number start from 0 and consecutive hence have a full range.

  // Start at index 0, while index <= highest order
  // Find next index higher than 0
  //   if nextHigher > index+1
  //      set all nextHigher orders to index+1

  // Limitation: May reach the ceiling of DevelLayer::SIBLING_ORDER_MULTIPLIER with highest sibling.

  ActorIter end = siblings.end();
  int highestOrder = 0;
  for( ActorIter iter = siblings.begin(); iter != end; ++iter )
  {
    ActorPtr sibling = (*iter);
    int siblingOrder = sibling->mSiblingOrder;
    highestOrder = std::max( highestOrder, siblingOrder );
  }

  for ( int index = 0; index <= highestOrder; index++ )
  {
    int nextHighest = -1;

    // Find Next highest
    for( ActorIter iter = siblings.begin(); iter != end; ++iter )
    {
      ActorPtr sibling = (*iter);
      int siblingOrder = sibling->mSiblingOrder;

      if ( siblingOrder > index )
      {
        if ( nextHighest == -1 )
        {
          nextHighest = siblingOrder;
        }
        nextHighest = std::min( nextHighest, siblingOrder );
      }
    }

    // Check if a gap exists between indexes, if so set next index to consecutive number
    if ( ( nextHighest - index ) > 1 )
    {
      for( ActorIter iter = siblings.begin(); iter != end; ++iter )
      {
        ActorPtr sibling = (*iter);
        int siblingOrder = sibling->mSiblingOrder;
        if ( siblingOrder == nextHighest )
        {
          sibling->mSiblingOrder =  index + 1;
          if ( sibling->mSiblingOrder >= Dali::DevelLayer::SIBLING_ORDER_MULTIPLIER )
          {
            DALI_LOG_WARNING( "Reached max sibling order level for raising / lowering actors\n" );
            sibling->mSiblingOrder = Dali::DevelLayer::SIBLING_ORDER_MULTIPLIER;
          }
          sibling->SetSiblingOrder( sibling->mSiblingOrder );
        }
      }
    }
  }
}

bool Actor::ShiftSiblingsLevels( ActorContainer& siblings, int targetLevelToShiftFrom )
{
  // Allows exclusive levels for an actor by shifting all sibling levels at the target and above by 1
  bool defragmentationRequired( false );
  ActorIter end = siblings.end();
  for( ActorIter iter = siblings.begin(); ( iter != end ) ; ++iter )
  {
    // Move actors at nearest order and above up by 1
    ActorPtr sibling = (*iter);
    if ( sibling != this )
    {
      // Iterate through container of actors, any actor with a sibling order of the target or greater should
      // be incremented by 1.
      if ( sibling->mSiblingOrder >= targetLevelToShiftFrom )
      {
        sibling->mSiblingOrder++;
        if ( sibling->mSiblingOrder + 1 >= DevelLayer::SIBLING_ORDER_MULTIPLIER )
        {
          // If a sibling order raises so that it is only 1 from the maximum allowed then set flag so
          // can re-order all sibling orders.
          defragmentationRequired = true;
        }
        sibling->SetSiblingOrder( sibling->mSiblingOrder );
      }
    }
  }
  return defragmentationRequired;
}

void Actor::Raise()
{
  /*
     1) Check if already at top and nothing to be done.
        This Actor can have highest sibling order but if not exclusive then another actor at same sibling
        order can be positioned above it due to insertion order of actors.
     2) Find nearest sibling level above, these are the siblings this actor needs to be above
     3) a) There may be other levels above this target level
        b) Increment all sibling levels at the level above nearest(target)
        c) Now have a vacant sibling level
     4) Set this actor's sibling level to nearest +1 as now vacated.

     Note May not just be sibling level + 1 as could be empty levels in-between

     Example:

     1 ) Initial order
        ActorC ( sibling level 4 )
        ActorB ( sibling level 3 )
        ActorA ( sibling level 1 )

     2 )  ACTION: Raise A above B
        a) Find nearest level above A = Level 3
        b) Increment levels above Level 3

           ActorC ( sibling level 5 )
           ActorB ( sibling level 3 )  NEAREST
           ActorA ( sibling level 1 )

     3 ) Set Actor A sibling level to nearest +1 as vacant

         ActorC ( sibling level 5 )
         ActorA ( sibling level 4 )
         ActorB ( sibling level 3 )

     4 ) Sibling order levels have a maximum defined in DevelLayer::SIBLING_ORDER_MULTIPLIER
         If shifting causes this ceiling to be reached. then a defragmentation can be performed to
         remove any empty sibling order gaps and start from sibling level 0 again.
         If the number of actors reaches this maximum and all using exclusive sibling order values then
         defragmention will stop and new sibling orders will be set to same max value.
  */
  if ( mParent )
  {
    int nearestLevel = mSiblingOrder;
    int shortestDistanceToNextLevel = DevelLayer::SIBLING_ORDER_MULTIPLIER;
    bool defragmentationRequired( false );

    ActorContainer* siblings = mParent->mChildren;

    // Find Nearest sibling level above this actor
    ActorIter end = siblings->end();
    for( ActorIter iter = siblings->begin(); iter != end; ++iter )
    {
      ActorPtr sibling = (*iter);
      if ( sibling != this )
      {
        int order = GetSiblingOrder( sibling );

        if ( ( order >= mSiblingOrder ) )
        {
          int distanceToNextLevel =  order - mSiblingOrder;
          if ( distanceToNextLevel < shortestDistanceToNextLevel )
          {
            nearestLevel = order;
            shortestDistanceToNextLevel = distanceToNextLevel;
          }
        }
      }
    }

    if ( nearestLevel < DevelLayer::SIBLING_ORDER_MULTIPLIER ) // Actor is not already exclusively at top
    {
      mSiblingOrder = nearestLevel + 1; // Set sibling level to that above the nearest level
      defragmentationRequired = ShiftSiblingsLevels( *siblings, mSiblingOrder );
      // Move current actor to newly vacated order level
      SetSiblingOrder( mSiblingOrder );
      if ( defragmentationRequired )
      {
        DefragmentSiblingIndexes( *siblings );
      }
    }
    SetSiblingOrder( mSiblingOrder );
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::Lower()
{
  /**
    1) Check if actor already at bottom and if nothing needs to be done
       This Actor can have lowest sibling order but if not exclusive then another actor at same sibling
       order can be positioned above it due to insertion order of actors so need to move this actor below it.
    2) Find nearest sibling level below, this Actor needs to be below it
    3) a) Need to vacate a sibling level below nearest for this actor to occupy
       b) Shift up all sibling order values of actor at the nearest level and levels above it to vacate a level.
       c) Set this actor's sibling level to this newly vacated level.
    4 ) Sibling order levels have a maximum defined in DevelLayer::SIBLING_ORDER_MULTIPLIER
       If shifting causes this ceiling to be reached. then a defragmentation can be performed to
       remove any empty sibling order gaps and start from sibling level 0 again.
       If the number of actors reaches this maximum and all using exclusive sibling order values then
       defragmention will stop and new sibling orders will be set to same max value.
  */

  if ( mParent )
  {
    // 1) Find nearest level below
    int nearestLevel = mSiblingOrder;
    int shortestDistanceToNextLevel = DevelLayer::SIBLING_ORDER_MULTIPLIER;

    ActorContainer* siblings = mParent->mChildren;

    ActorIter end = siblings->end();
    for( ActorIter iter = siblings->begin(); iter != end; ++iter )
    {
      ActorPtr sibling = (*iter);
      if ( sibling != this )
      {
        int order = GetSiblingOrder( sibling );

        if ( order <= mSiblingOrder )
        {
          int distanceToNextLevel =  mSiblingOrder - order;
          if ( distanceToNextLevel < shortestDistanceToNextLevel )
          {
            nearestLevel = order;
            shortestDistanceToNextLevel = distanceToNextLevel;
          }
        }
      }
    }

    bool defragmentationRequired ( false );

    // 2) If actor already not at bottom, raise all actors at required level and above
    if ( shortestDistanceToNextLevel < DevelLayer::SIBLING_ORDER_MULTIPLIER ) // Actor is not already exclusively at bottom
    {
      mSiblingOrder = nearestLevel;
      defragmentationRequired = ShiftSiblingsLevels( *siblings, mSiblingOrder );
      // Move current actor to newly vacated order
      SetSiblingOrder( mSiblingOrder );
      if ( defragmentationRequired )
      {
        DefragmentSiblingIndexes( *siblings );
      }
    }
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::RaiseToTop()
{
  /**
    1 ) Find highest sibling order actor
    2 ) If highest sibling level not itself then set sibling order to that + 1
    3 ) highest sibling order can be same as itself so need to increment over that
    4 ) Sibling order levels have a maximum defined in DevelLayer::SIBLING_ORDER_MULTIPLIER
        If shifting causes this ceiling to be reached. then a defragmentation can be performed to
        remove any empty sibling order gaps and start from sibling level 0 again.
        If the number of actors reaches this maximum and all using exclusive sibling order values then
        defragmention will stop and new sibling orders will be set to same max value.
   */

  if ( mParent )
  {
    int maxOrder = 0;

    ActorContainer* siblings = mParent->mChildren;

    ActorIter end = siblings->end();
    for( ActorIter iter = siblings->begin(); iter != end; ++iter )
    {
      ActorPtr sibling = (*iter);
      if ( sibling != this )
      {
        maxOrder = std::max( GetSiblingOrder( sibling ), maxOrder );
      }
    }

    bool defragmentationRequired( false );

    if ( maxOrder >= mSiblingOrder )
    {
      mSiblingOrder = maxOrder + 1;
      if ( mSiblingOrder + 1 >= DevelLayer::SIBLING_ORDER_MULTIPLIER )
      {
        defragmentationRequired = true;
      }
    }

    SetSiblingOrder( mSiblingOrder );

    if ( defragmentationRequired )
    {
      DefragmentSiblingIndexes( *siblings );
    }
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::LowerToBottom()
{
  /**
    See Actor::LowerToBottom()

    1 ) Check if this actor already at exclusively at the bottom, if so then no more to be done.
    2 ) a ) Check if the bottom position 0 is vacant.
        b ) If 0 position is not vacant then shift up all sibling order values from 0 and above
        c ) 0 sibling position is vacant.
    3 ) Set this actor to vacant sibling order 0;
    4 ) Sibling order levels have a maximum defined in DevelLayer::SIBLING_ORDER_MULTIPLIER
        If shifting causes this ceiling to be reached. then a defragmentation can be performed to
        remove any empty sibling order gaps and start from sibling level 0 again.
        If the number of actors reaches this maximum and all using exclusive sibling order values then
        defragmention will stop and new sibling orders will be set to same max value.
   */

  if ( mParent )
  {
    bool defragmentationRequired( false );
    bool orderZeroFree ( true );

    ActorContainer* siblings = mParent->mChildren;

    bool actorAtLowestOrder = true;
    ActorIter end = siblings->end();
    for( ActorIter iter = siblings->begin(); ( iter != end ) ; ++iter )
    {
      ActorPtr sibling = (*iter);
      if ( sibling != this )
      {
        int siblingOrder = GetSiblingOrder( sibling );
        if ( siblingOrder <= mSiblingOrder )
        {
          actorAtLowestOrder = false;
        }

        if ( siblingOrder == 0 )
        {
          orderZeroFree = false;
        }
      }
    }

    if ( ! actorAtLowestOrder  )
    {
      if ( ! orderZeroFree )
      {
        defragmentationRequired = ShiftSiblingsLevels( *siblings, 0 );
      }
      mSiblingOrder = 0;
      SetSiblingOrder( mSiblingOrder );

      if ( defragmentationRequired )
      {
        DefragmentSiblingIndexes( *siblings );
      }
    }
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::RaiseAbove( Internal::Actor& target )
{
  /**
    1 ) a) Find target actor's sibling order
        b) If sibling order of target is the same as this actor then need to this Actor's sibling order
           needs to be above it or the insertion order will determine which is drawn on top.
    2 ) Shift up by 1 all sibling order greater than target sibling order
    3 ) Set this actor to the sibling order to target +1 as will be a newly vacated gap above
    4 ) Sibling order levels have a maximum defined in DevelLayer::SIBLING_ORDER_MULTIPLIER
        If shifting causes this ceiling to be reached. then a defragmentation can be performed to
        remove any empty sibling order gaps and start from sibling level 0 again.
        If the number of actors reaches this maximum and all using exclusive sibling order values then
        defragmention will stop and new sibling orders will be set to same max value.
   */

  if ( mParent )
  {
    if ( ValidateActors( *this, target ) )
    {
       // Find target's sibling order
       // Set actor sibling order to this number +1
      int targetSiblingOrder = GetSiblingOrder( &target );
      ActorContainer* siblings = mParent->mChildren;
      mSiblingOrder = targetSiblingOrder + 1;
      bool defragmentationRequired = ShiftSiblingsLevels( *siblings, mSiblingOrder );

      SetSiblingOrder( mSiblingOrder );

      if ( defragmentationRequired )
      {
        DefragmentSiblingIndexes( *(mParent->mChildren) );
      }
    }
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

void Actor::LowerBelow( Internal::Actor& target )
{
  /**
     1 ) a) Find target actor's sibling order
         b) If sibling order of target is the same as this actor then need to this Actor's sibling order
            needs to be below it or the insertion order will determine which is drawn on top.
     2 ) Shift the target sibling order and all sibling orders at that level or above by 1
     3 ) Set this actor to the sibling order of the target before it changed.
     4 ) Sibling order levels have a maximum defined in DevelLayer::SIBLING_ORDER_MULTIPLIER
         If shifting causes this ceiling to be reached. then a defragmentation can be performed to
         remove any empty sibling order gaps and start from sibling level 0 again.
         If the number of actors reaches this maximum and all using exclusive sibling order values then
         defragmention will stop and new sibling orders will be set to same max value.
   */

  if ( mParent )
  {
    if ( ValidateActors( *this, target )  )
    {
      bool defragmentationRequired ( false );
      // Find target's sibling order
      // Set actor sibling order to target sibling order - 1
      int targetSiblingOrder = GetSiblingOrder( &target);
      ActorContainer* siblings = mParent->mChildren;
      if ( targetSiblingOrder == 0 )
      {
        //lower to botton
        ActorIter end = siblings->end();
        for( ActorIter iter = siblings->begin(); ( iter != end ) ; ++iter )
        {
          ActorPtr sibling = (*iter);
          if ( sibling != this )
          {
            sibling->mSiblingOrder++;
            if ( sibling->mSiblingOrder + 1 >= DevelLayer::SIBLING_ORDER_MULTIPLIER )
            {
              defragmentationRequired = true;
            }
            sibling->SetSiblingOrder( sibling->mSiblingOrder );
          }
        }
        mSiblingOrder = 0;
      }
      else
      {
        defragmentationRequired = ShiftSiblingsLevels( *siblings, targetSiblingOrder );

        mSiblingOrder = targetSiblingOrder;
      }
      SetSiblingOrder( mSiblingOrder );

      if ( defragmentationRequired )
      {
        DefragmentSiblingIndexes( *(mParent->mChildren) );
      }
    }
  }
  else
  {
    DALI_LOG_WARNING( "Actor must have a parent, Sibling order not changed.\n" );
  }
}

} // namespace Internal

} // namespace Dali
