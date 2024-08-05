/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <algorithm>
#include <cmath>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/object/type-registry.h>

#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/capabilities.h>

#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/touch-integ.h>

#include <dali/internal/event/actors/actor-coords.h>
#include <dali/internal/event/actors/actor-parent.h>
#include <dali/internal/event/actors/actor-property-handler.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/type-info-impl.h>
#include <dali/internal/event/events/actor-gesture-data.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/rendering/renderer-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/nodes/node-messages.h>
#include <dali/public-api/size-negotiation/relayout-container.h>

using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::Node;
using Dali::Internal::SceneGraph::PropertyBase;

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_DEPTH_TIMER");
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
DALI_PROPERTY("parentOrigin", VECTOR3, true, false, true, Dali::Actor::Property::PARENT_ORIGIN)
DALI_PROPERTY("parentOriginX", FLOAT, true, false, true, Dali::Actor::Property::PARENT_ORIGIN_X)
DALI_PROPERTY("parentOriginY", FLOAT, true, false, true, Dali::Actor::Property::PARENT_ORIGIN_Y)
DALI_PROPERTY("parentOriginZ", FLOAT, true, false, true, Dali::Actor::Property::PARENT_ORIGIN_Z)
DALI_PROPERTY("anchorPoint", VECTOR3, true, false, true, Dali::Actor::Property::ANCHOR_POINT)
DALI_PROPERTY("anchorPointX", FLOAT, true, false, true, Dali::Actor::Property::ANCHOR_POINT_X)
DALI_PROPERTY("anchorPointY", FLOAT, true, false, true, Dali::Actor::Property::ANCHOR_POINT_Y)
DALI_PROPERTY("anchorPointZ", FLOAT, true, false, true, Dali::Actor::Property::ANCHOR_POINT_Z)
DALI_PROPERTY("size", VECTOR3, true, true, true, Dali::Actor::Property::SIZE)
DALI_PROPERTY("sizeWidth", FLOAT, true, true, true, Dali::Actor::Property::SIZE_WIDTH)
DALI_PROPERTY("sizeHeight", FLOAT, true, true, true, Dali::Actor::Property::SIZE_HEIGHT)
DALI_PROPERTY("sizeDepth", FLOAT, true, true, true, Dali::Actor::Property::SIZE_DEPTH)
DALI_PROPERTY("position", VECTOR3, true, true, true, Dali::Actor::Property::POSITION)
DALI_PROPERTY("positionX", FLOAT, true, true, true, Dali::Actor::Property::POSITION_X)
DALI_PROPERTY("positionY", FLOAT, true, true, true, Dali::Actor::Property::POSITION_Y)
DALI_PROPERTY("positionZ", FLOAT, true, true, true, Dali::Actor::Property::POSITION_Z)
DALI_PROPERTY("worldPosition", VECTOR3, false, false, true, Dali::Actor::Property::WORLD_POSITION)
DALI_PROPERTY("worldPositionX", FLOAT, false, false, true, Dali::Actor::Property::WORLD_POSITION_X)
DALI_PROPERTY("worldPositionY", FLOAT, false, false, true, Dali::Actor::Property::WORLD_POSITION_Y)
DALI_PROPERTY("worldPositionZ", FLOAT, false, false, true, Dali::Actor::Property::WORLD_POSITION_Z)
DALI_PROPERTY("orientation", ROTATION, true, true, true, Dali::Actor::Property::ORIENTATION)
DALI_PROPERTY("worldOrientation", ROTATION, false, false, true, Dali::Actor::Property::WORLD_ORIENTATION)
DALI_PROPERTY("scale", VECTOR3, true, true, true, Dali::Actor::Property::SCALE)
DALI_PROPERTY("scaleX", FLOAT, true, true, true, Dali::Actor::Property::SCALE_X)
DALI_PROPERTY("scaleY", FLOAT, true, true, true, Dali::Actor::Property::SCALE_Y)
DALI_PROPERTY("scaleZ", FLOAT, true, true, true, Dali::Actor::Property::SCALE_Z)
DALI_PROPERTY("worldScale", VECTOR3, false, false, true, Dali::Actor::Property::WORLD_SCALE)
DALI_PROPERTY("visible", BOOLEAN, true, true, true, Dali::Actor::Property::VISIBLE)
DALI_PROPERTY("color", VECTOR4, true, true, true, Dali::Actor::Property::COLOR)
DALI_PROPERTY("colorRed", FLOAT, true, true, true, Dali::Actor::Property::COLOR_RED)
DALI_PROPERTY("colorGreen", FLOAT, true, true, true, Dali::Actor::Property::COLOR_GREEN)
DALI_PROPERTY("colorBlue", FLOAT, true, true, true, Dali::Actor::Property::COLOR_BLUE)
DALI_PROPERTY("colorAlpha", FLOAT, true, true, true, Dali::Actor::Property::COLOR_ALPHA)
DALI_PROPERTY("worldColor", VECTOR4, false, false, true, Dali::Actor::Property::WORLD_COLOR)
DALI_PROPERTY("worldMatrix", MATRIX, false, false, true, Dali::Actor::Property::WORLD_MATRIX)
DALI_PROPERTY("name", STRING, true, false, false, Dali::Actor::Property::NAME)
DALI_PROPERTY("sensitive", BOOLEAN, true, false, false, Dali::Actor::Property::SENSITIVE)
DALI_PROPERTY("leaveRequired", BOOLEAN, true, false, false, Dali::Actor::Property::LEAVE_REQUIRED)
DALI_PROPERTY("inheritOrientation", BOOLEAN, true, false, false, Dali::Actor::Property::INHERIT_ORIENTATION)
DALI_PROPERTY("inheritScale", BOOLEAN, true, false, false, Dali::Actor::Property::INHERIT_SCALE)
DALI_PROPERTY("colorMode", INTEGER, true, false, false, Dali::Actor::Property::COLOR_MODE)
DALI_PROPERTY("drawMode", INTEGER, true, false, false, Dali::Actor::Property::DRAW_MODE)
DALI_PROPERTY("sizeModeFactor", VECTOR3, true, false, false, Dali::Actor::Property::SIZE_MODE_FACTOR)
DALI_PROPERTY("widthResizePolicy", STRING, true, false, false, Dali::Actor::Property::WIDTH_RESIZE_POLICY)
DALI_PROPERTY("heightResizePolicy", STRING, true, false, false, Dali::Actor::Property::HEIGHT_RESIZE_POLICY)
DALI_PROPERTY("sizeScalePolicy", INTEGER, true, false, false, Dali::Actor::Property::SIZE_SCALE_POLICY)
DALI_PROPERTY("widthForHeight", BOOLEAN, true, false, false, Dali::Actor::Property::WIDTH_FOR_HEIGHT)
DALI_PROPERTY("heightForWidth", BOOLEAN, true, false, false, Dali::Actor::Property::HEIGHT_FOR_WIDTH)
DALI_PROPERTY("padding", VECTOR4, true, false, false, Dali::Actor::Property::PADDING)
DALI_PROPERTY("minimumSize", VECTOR2, true, false, false, Dali::Actor::Property::MINIMUM_SIZE)
DALI_PROPERTY("maximumSize", VECTOR2, true, false, false, Dali::Actor::Property::MAXIMUM_SIZE)
DALI_PROPERTY("inheritPosition", BOOLEAN, true, false, false, Dali::Actor::Property::INHERIT_POSITION)
DALI_PROPERTY("clippingMode", STRING, true, false, false, Dali::Actor::Property::CLIPPING_MODE)
DALI_PROPERTY("layoutDirection", STRING, true, false, false, Dali::Actor::Property::LAYOUT_DIRECTION)
DALI_PROPERTY("inheritLayoutDirection", BOOLEAN, true, false, false, Dali::Actor::Property::INHERIT_LAYOUT_DIRECTION)
DALI_PROPERTY("opacity", FLOAT, true, true, true, Dali::Actor::Property::OPACITY)
DALI_PROPERTY("screenPosition", VECTOR2, false, false, false, Dali::Actor::Property::SCREEN_POSITION)
DALI_PROPERTY("positionUsesAnchorPoint", BOOLEAN, true, false, false, Dali::Actor::Property::POSITION_USES_ANCHOR_POINT)
DALI_PROPERTY("culled", BOOLEAN, false, false, true, Dali::Actor::Property::CULLED)
DALI_PROPERTY("id", INTEGER, false, false, false, Dali::Actor::Property::ID)
DALI_PROPERTY("hierarchyDepth", INTEGER, false, false, false, Dali::Actor::Property::HIERARCHY_DEPTH)
DALI_PROPERTY("isRoot", BOOLEAN, false, false, false, Dali::Actor::Property::IS_ROOT)
DALI_PROPERTY("isLayer", BOOLEAN, false, false, false, Dali::Actor::Property::IS_LAYER)
DALI_PROPERTY("connectedToScene", BOOLEAN, false, false, false, Dali::Actor::Property::CONNECTED_TO_SCENE)
DALI_PROPERTY("keyboardFocusable", BOOLEAN, true, false, false, Dali::Actor::Property::KEYBOARD_FOCUSABLE)
DALI_PROPERTY("updateAreaHint", VECTOR4, true, false, false, Dali::Actor::Property::UPDATE_AREA_HINT)
DALI_PROPERTY("siblingOrder", INTEGER, true, false, false, Dali::DevelActor::Property::SIBLING_ORDER)
DALI_PROPERTY("captureAllTouchAfterStart", BOOLEAN, true, false, false, Dali::DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START)
DALI_PROPERTY("touchAreaOffset", RECTANGLE, true, false, false, Dali::DevelActor::Property::TOUCH_AREA_OFFSET)
DALI_PROPERTY("blendEquation", INTEGER, true, false, false, Dali::DevelActor::Property::BLEND_EQUATION)
DALI_PROPERTY("touchFocusable", BOOLEAN, true, false, false, Dali::DevelActor::Property::TOUCH_FOCUSABLE)
DALI_PROPERTY("keyboardFocusableChildren", BOOLEAN, true, false, false, Dali::DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN)
DALI_PROPERTY("userInteractionEnabled", BOOLEAN, true, false, false, Dali::DevelActor::Property::USER_INTERACTION_ENABLED)
DALI_PROPERTY("allowOnlyOwnTouch", BOOLEAN, true, false, false, Dali::DevelActor::Property::ALLOW_ONLY_OWN_TOUCH)
DALI_PROPERTY("useTextureUpdateArea", BOOLEAN, true, false, false, Dali::DevelActor::Property::USE_TEXTURE_UPDATE_AREA)
DALI_PROPERTY("dispatchTouchMotion", BOOLEAN, true, false, false, Dali::DevelActor::Property::DISPATCH_TOUCH_MOTION)
DALI_PROPERTY("dispatchHoverMotion", BOOLEAN, true, false, false, Dali::DevelActor::Property::DISPATCH_HOVER_MOTION)
DALI_PROPERTY_TABLE_END(DEFAULT_ACTOR_PROPERTY_START_INDEX, ActorDefaultProperties)

// Signals

static constexpr std::string_view SIGNAL_HOVERED                      = "hovered";
static constexpr std::string_view SIGNAL_WHEEL_EVENT                  = "wheelEvent";
static constexpr std::string_view SIGNAL_ON_SCENE                     = "onScene";
static constexpr std::string_view SIGNAL_OFF_SCENE                    = "offScene";
static constexpr std::string_view SIGNAL_ON_RELAYOUT                  = "onRelayout";
static constexpr std::string_view SIGNAL_TOUCHED                      = "touched";
static constexpr std::string_view SIGNAL_VISIBILITY_CHANGED           = "visibilityChanged";
static constexpr std::string_view SIGNAL_INHERITED_VISIBILITY_CHANGED = "inheritedVisibilityChanged";
static constexpr std::string_view SIGNAL_LAYOUT_DIRECTION_CHANGED     = "layoutDirectionChanged";
static constexpr std::string_view SIGNAL_CHILD_ADDED                  = "childAdded";
static constexpr std::string_view SIGNAL_CHILD_REMOVED                = "childRemoved";

// Actions

static constexpr std::string_view ACTION_SHOW = "show";
static constexpr std::string_view ACTION_HIDE = "hide";

BaseHandle CreateActor()
{
  return Dali::Actor::New();
}

/**
 * Connects a callback function with the object's signals.
 * @param[in] object The object providing the signal.
 * @param[in] tracker Used to disconnect the signal.
 * @param[in] signalName The signal to connect to.
 * @param[in] functor A newly allocated FunctorDelegate.
 * @return True if the signal was connected.
 * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
 */
static bool DoConnectSignal(BaseObject*                 object,
                            ConnectionTrackerInterface* tracker,
                            const std::string&          signalName,
                            FunctorDelegate*            functor)
{
  bool   connected(true);
  Actor* actor = static_cast<Actor*>(object); // TypeRegistry guarantees that this is the correct type.

  std::string_view name(signalName);

  if(name == SIGNAL_HOVERED)
  {
    actor->HoveredSignal().Connect(tracker, functor);
  }
  else if(signalName == SIGNAL_WHEEL_EVENT)
  {
    actor->WheelEventSignal().Connect(tracker, functor);
  }
  else if(name == SIGNAL_ON_SCENE)
  {
    actor->OnSceneSignal().Connect(tracker, functor);
  }
  else if(name == SIGNAL_OFF_SCENE)
  {
    actor->OffSceneSignal().Connect(tracker, functor);
  }
  else if(name == SIGNAL_ON_RELAYOUT)
  {
    actor->OnRelayoutSignal().Connect(tracker, functor);
  }
  else if(name == SIGNAL_TOUCHED)
  {
    actor->TouchedSignal().Connect(tracker, functor);
  }
  else if(name == SIGNAL_VISIBILITY_CHANGED)
  {
    actor->VisibilityChangedSignal().Connect(tracker, functor);
  }
  else if(name == SIGNAL_INHERITED_VISIBILITY_CHANGED)
  {
    actor->InheritedVisibilityChangedSignal().Connect(tracker, functor);
  }
  else if(name == SIGNAL_LAYOUT_DIRECTION_CHANGED)
  {
    actor->LayoutDirectionChangedSignal().Connect(tracker, functor);
  }
  else if(name == SIGNAL_CHILD_ADDED)
  {
    actor->ChildAddedSignal().Connect(tracker, functor);
  }
  else if(name == SIGNAL_CHILD_REMOVED)
  {
    actor->ChildRemovedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

/**
 * Performs actions as requested using the action name.
 * @param[in] object The object on which to perform the action.
 * @param[in] actionName The action to perform.
 * @param[in] attributes The attributes with which to perfrom this action.
 * @return true if the action was done.
 */
bool DoAction(BaseObject*          object,
              const std::string&   actionName,
              const Property::Map& attributes)
{
  bool   done  = false;
  Actor* actor = dynamic_cast<Actor*>(object);

  if(actor)
  {
    std::string_view name(actionName);
    if(name == ACTION_SHOW)
    {
      actor->SetVisible(true);
      done = true;
    }
    else if(name == ACTION_HIDE)
    {
      actor->SetVisible(false);
      done = true;
    }
  }

  return done;
}

TypeRegistration mType(typeid(Dali::Actor), typeid(Dali::Handle), CreateActor, ActorDefaultProperties);

SignalConnectorType signalConnector2(mType, std::string(SIGNAL_HOVERED), &DoConnectSignal);
SignalConnectorType signalConnector3(mType, std::string(SIGNAL_WHEEL_EVENT), &DoConnectSignal);
SignalConnectorType signalConnector4(mType, std::string(SIGNAL_ON_SCENE), &DoConnectSignal);
SignalConnectorType signalConnector5(mType, std::string(SIGNAL_OFF_SCENE), &DoConnectSignal);
SignalConnectorType signalConnector6(mType, std::string(SIGNAL_ON_RELAYOUT), &DoConnectSignal);
SignalConnectorType signalConnector7(mType, std::string(SIGNAL_TOUCHED), &DoConnectSignal);
SignalConnectorType signalConnector8(mType, std::string(SIGNAL_VISIBILITY_CHANGED), &DoConnectSignal);
SignalConnectorType signalConnector9(mType, std::string(SIGNAL_INHERITED_VISIBILITY_CHANGED), &DoConnectSignal);
SignalConnectorType signalConnector10(mType, std::string(SIGNAL_LAYOUT_DIRECTION_CHANGED), &DoConnectSignal);
SignalConnectorType signalConnector11(mType, std::string(SIGNAL_CHILD_ADDED), &DoConnectSignal);
SignalConnectorType signalConnector12(mType, std::string(SIGNAL_CHILD_REMOVED), &DoConnectSignal);

TypeAction a1(mType, std::string(ACTION_SHOW), &DoAction);
TypeAction a2(mType, std::string(ACTION_HIDE), &DoAction);

/// Helper for emitting a signal
template<typename Signal, typename Event>
bool EmitConsumingSignal(Actor& actor, Signal& signal, const Event& event)
{
  bool consumed = false;

  if(!signal.Empty())
  {
    Dali::Actor handle(&actor);
    consumed = signal.Emit(handle, event);
  }
  return consumed;
}

/// Helper for emitting a signal
/// If any one of the multiple callbacks returns true, the entire callback is consumed.
template<typename Signal, typename Event>
bool EmitConsumingSignalOr(Actor& actor, Signal& signal, const Event& event)
{
  bool consumed = false;

  if(!signal.Empty())
  {
    Dali::Actor handle(&actor);
    consumed = signal.EmitOr(handle, event);
  }
  return consumed;
}

/// Helper for emitting signals with multiple parameters
template<typename Signal, typename... Param>
void EmitSignal(Actor& actor, Signal& signal, Param... params)
{
  if(!signal.Empty())
  {
    Dali::Actor handle(&actor);
    signal.Emit(handle, params...);
  }
}

using ActorParentSiblingOrderMethod           = void (ActorParent::*)(Actor&);
using ActorParentSiblingOrderMethodWithTarget = void (ActorParent::*)(Actor&, Actor&);

/// Helper to check and call actor sibling methods in ActorParent
void CheckParentAndCall(ActorParent* parent, Actor& actor, ActorParentSiblingOrderMethod memberFunction)
{
  if(parent)
  {
    (parent->*memberFunction)(actor);
  }
  else
  {
    DALI_LOG_WARNING("Actor must have a parent, Sibling order not changed.\n");
  }
}

/// Helper to check and call actor sibling methods with a target parameter in ActorParent
void CheckParentAndCall(ActorParent* parent, Actor& actor, Actor& target, ActorParentSiblingOrderMethodWithTarget memberFunction)
{
  if(parent)
  {
    (parent->*memberFunction)(actor, target);
  }
  else
  {
    DALI_LOG_WARNING("Actor must have a parent, Sibling order not changed.\n");
  }
}

} // unnamed namespace

ActorPtr Actor::New()
{
  // pass a reference to actor, actor does not own its node
  ActorPtr actor(new Actor(BASIC, *CreateNode()));

  // Second-phase construction
  actor->Initialize();

  return actor;
}

const SceneGraph::Node* Actor::CreateNode()
{
  // create node. Nodes are owned by the update manager
  SceneGraph::Node*              node = SceneGraph::Node::New();
  OwnerPointer<SceneGraph::Node> transferOwnership(node);
  Internal::ThreadLocalStorage*  tls = Internal::ThreadLocalStorage::GetInternal();

  DALI_ASSERT_ALWAYS(tls && "ThreadLocalStorage is null");

  AddNodeMessage(tls->GetUpdateManager(), transferOwnership);

  return node;
}

void Actor::SetName(std::string_view name)
{
  mName = name;

  // ATTENTION: string for debug purposes is not thread safe.
  DALI_LOG_SET_OBJECT_STRING(const_cast<SceneGraph::Node*>(&GetNode()), mName.c_str());
}

uint32_t Actor::GetId() const
{
  return GetNode().GetId();
}

Dali::Layer Actor::GetLayer()
{
  Dali::Layer layer;

  // Short-circuit for Layer derived actors
  if(mIsLayer)
  {
    layer = Dali::Layer(static_cast<Dali::Internal::Layer*>(this)); // static cast as we trust the flag
  }

  // Find the immediate Layer parent
  for(Actor* parent = GetParent(); !layer && parent != nullptr; parent = parent->GetParent())
  {
    if(parent->IsLayer())
    {
      layer = Dali::Layer(static_cast<Dali::Internal::Layer*>(parent)); // static cast as we trust the flag
    }
  }

  return layer;
}

void Actor::Unparent()
{
  if(mParent)
  {
    // Remove this actor from the parent. The remove will put a relayout request in for
    // the parent if required
    mParent->Remove(*this);
    // mParent is now NULL!
  }
}

void Actor::SetParentOrigin(const Vector3& origin)
{
  // node is being used in a separate thread; queue a message to set the value & base value
  SetParentOriginMessage(GetEventThreadServices(), GetNode(), origin);

  // Cache for event-thread access
  if(!mParentOrigin)
  {
    // not allocated, check if different from default
    if(ParentOrigin::DEFAULT != origin)
    {
      mParentOrigin = new Vector3(origin);
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
  return (mParentOrigin) ? *mParentOrigin : ParentOrigin::DEFAULT;
}

void Actor::SetAnchorPoint(const Vector3& anchor)
{
  // node is being used in a separate thread; queue a message to set the value & base value
  SetAnchorPointMessage(GetEventThreadServices(), GetNode(), anchor);

  // Cache for event-thread access
  if(!mAnchorPoint)
  {
    // not allocated, check if different from default
    if(AnchorPoint::DEFAULT != anchor)
    {
      mAnchorPoint = new Vector3(anchor);
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
  return (mAnchorPoint) ? *mAnchorPoint : AnchorPoint::DEFAULT;
}

void Actor::SetPosition(const Vector3& position)
{
  mTargetPosition = position;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::Bake, position);
}

void Actor::SetX(float x)
{
  mTargetPosition.x = x;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeX, x);
}

void Actor::SetY(float y)
{
  mTargetPosition.y = y;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeY, y);
}

void Actor::SetZ(float z)
{
  mTargetPosition.z = z;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeZ, z);
}

void Actor::TranslateBy(const Vector3& distance)
{
  mTargetPosition += distance;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mPosition, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeRelative, distance);
}

const Vector3& Actor::GetCurrentPosition() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetPosition(GetEventThreadServices().GetEventBufferIndex());
}

const Vector3& Actor::GetCurrentWorldPosition() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetWorldPosition(GetEventThreadServices().GetEventBufferIndex());
}

const Vector2 Actor::CalculateScreenPosition() const
{
  if(mScene)
  {
    if(mLayer3DParentsCount == 0)
    {
      // We can assume that this actor is under 2d layer. Use faster, but imprecise algorithm
      return CalculateActorScreenPosition(*this);
    }
    else
    {
      return CalculateActorScreenPositionRenderTaskList(*this);
    }
  }
  return Vector2::ZERO;
}

const Vector2 Actor::GetCurrentScreenPosition() const
{
  if(mScene)
  {
    BufferIndex bufferIndex = GetEventThreadServices().GetEventBufferIndex();
    if(mLayer3DParentsCount == 0)
    {
      // We can assume that this actor is under 2d layer. Use faster, but imprecise algorithm
      return CalculateCurrentActorScreenPosition(*this, bufferIndex);
    }
    else
    {
      return CalculateCurrentActorScreenPositionRenderTaskList(*this, bufferIndex);
    }
  }
  return Vector2::ZERO;
}

void Actor::SetInheritPosition(bool inherit)
{
  if(mInheritPosition != inherit)
  {
    // non animatable so keep local copy
    mInheritPosition = inherit;
    SetInheritPositionMessage(GetEventThreadServices(), GetNode(), inherit);
  }
}

void Actor::SetOrientation(const Radian& angle, const Vector3& axis)
{
  Vector3 normalizedAxis(axis.x, axis.y, axis.z);
  normalizedAxis.Normalize();

  Quaternion orientation(angle, normalizedAxis);

  SetOrientation(orientation);
}

void Actor::SetOrientation(const Quaternion& orientation)
{
  mTargetOrientation = orientation;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Quaternion>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mOrientation, &SceneGraph::TransformManagerPropertyHandler<Quaternion>::Bake, orientation);
}

void Actor::RotateBy(const Radian& angle, const Vector3& axis)
{
  RotateBy(Quaternion(angle, axis));
}

void Actor::RotateBy(const Quaternion& relativeRotation)
{
  mTargetOrientation *= Quaternion(relativeRotation);

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Quaternion>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mOrientation, &SceneGraph::TransformManagerPropertyHandler<Quaternion>::BakeRelative, relativeRotation);
}

const Quaternion& Actor::GetCurrentOrientation() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetOrientation(GetEventThreadServices().GetEventBufferIndex());
}

const Quaternion& Actor::GetCurrentWorldOrientation() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetWorldOrientation(GetEventThreadServices().GetEventBufferIndex());
}

void Actor::SetScale(const Vector3& scale)
{
  mTargetScale = scale;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::Bake, scale);
}

void Actor::SetScaleX(float x)
{
  mTargetScale.x = x;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeX, x);
}

void Actor::SetScaleY(float y)
{
  mTargetScale.y = y;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeY, y);
}

void Actor::SetScaleZ(float z)
{
  mTargetScale.z = z;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeZ, z);
}

void Actor::ScaleBy(const Vector3& relativeScale)
{
  mTargetScale *= relativeScale;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformPropertyMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mScale, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeRelativeMultiply, relativeScale);
}

const Vector3& Actor::GetCurrentScale() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetScale(GetEventThreadServices().GetEventBufferIndex());
}

const Vector3& Actor::GetCurrentWorldScale() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetWorldScale(GetEventThreadServices().GetEventBufferIndex());
}

void Actor::SetInheritScale(bool inherit)
{
  if(mInheritScale != inherit)
  {
    // non animatable so keep local copy
    mInheritScale = inherit;
    // node is being used in a separate thread; queue a message to set the value
    SetInheritScaleMessage(GetEventThreadServices(), GetNode(), inherit);
  }
}

Matrix Actor::GetCurrentWorldMatrix() const
{
  return GetNode().GetWorldMatrix(0);
}

void Actor::SetVisible(bool visible)
{
  SetVisibleInternal(visible, SendMessage::TRUE);
}

bool Actor::IsVisible() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().IsVisible(GetEventThreadServices().GetEventBufferIndex());
}

void Actor::SetOpacity(float opacity)
{
  mTargetColor.a = opacity;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyComponentMessage<Vector4>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mColor, &AnimatableProperty<Vector4>::BakeW, opacity);

  RequestRenderingMessage(GetEventThreadServices().GetUpdateManager());
}

float Actor::GetCurrentOpacity() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetOpacity(GetEventThreadServices().GetEventBufferIndex());
}

const Vector4& Actor::GetCurrentWorldColor() const
{
  return GetNode().GetWorldColor(GetEventThreadServices().GetEventBufferIndex());
}

void Actor::SetColor(const Vector4& color)
{
  mTargetColor = color;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyMessage<Vector4>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mColor, &AnimatableProperty<Vector4>::Bake, color);

  RequestRenderingMessage(GetEventThreadServices().GetUpdateManager());
}

void Actor::SetColorRed(float red)
{
  mTargetColor.r = red;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyComponentMessage<Vector4>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mColor, &AnimatableProperty<Vector4>::BakeX, red);

  RequestRenderingMessage(GetEventThreadServices().GetUpdateManager());
}

void Actor::SetColorGreen(float green)
{
  mTargetColor.g = green;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyComponentMessage<Vector4>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mColor, &AnimatableProperty<Vector4>::BakeY, green);

  RequestRenderingMessage(GetEventThreadServices().GetUpdateManager());
}

void Actor::SetColorBlue(float blue)
{
  mTargetColor.b = blue;

  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodePropertyComponentMessage<Vector4>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mColor, &AnimatableProperty<Vector4>::BakeZ, blue);

  RequestRenderingMessage(GetEventThreadServices().GetUpdateManager());
}

const Vector4& Actor::GetCurrentColor() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetColor(GetEventThreadServices().GetEventBufferIndex());
}

void Actor::SetInheritOrientation(bool inherit)
{
  if(mInheritOrientation != inherit)
  {
    // non animatable so keep local copy
    mInheritOrientation = inherit;
    // node is being used in a separate thread; queue a message to set the value
    SetInheritOrientationMessage(GetEventThreadServices(), GetNode(), inherit);
  }
}

void Actor::SetSizeModeFactor(const Vector3& factor)
{
  mSizer.SetSizeModeFactor(factor);
}

const Vector3& Actor::GetSizeModeFactor() const
{
  return mSizer.GetSizeModeFactor();
}

void Actor::SetColorMode(ColorMode colorMode)
{
  // non animatable so keep local copy
  mColorMode = colorMode;
  // node is being used in a separate thread; queue a message to set the value
  SetColorModeMessage(GetEventThreadServices(), GetNode(), colorMode);
}

void Actor::SetSize(float width, float height)
{
  SetSize(Vector2(width, height));
}

void Actor::SetSize(float width, float height, float depth)
{
  SetSize(Vector3(width, height, depth));
}

void Actor::SetSize(const Vector2& size)
{
  SetSize(Vector3(size.width, size.height, 0.f));
}

void Actor::SetSize(const Vector3& size)
{
  mSizer.SetSize(size);
}

void Actor::SetWidth(float width)
{
  mSizer.SetWidth(width);
}

void Actor::SetHeight(float height)
{
  mSizer.SetHeight(height);
}

void Actor::SetDepth(float depth)
{
  mSizer.SetDepth(depth);
  // node is being used in a separate thread; queue a message to set the value & base value
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeZ, depth);
}

Vector3 Actor::GetTargetSize() const
{
  return mSizer.GetTargetSize();
}

const Vector3& Actor::GetCurrentSize() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetNode().GetSize(GetEventThreadServices().GetEventBufferIndex());
}

Vector3 Actor::GetNaturalSize() const
{
  // It is up to deriving classes to return the appropriate natural size
  return Vector3(0.0f, 0.0f, 0.0f);
}

void Actor::SetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
  mSizer.SetResizePolicy(policy, dimension);
}

ResizePolicy::Type Actor::GetResizePolicy(Dimension::Type dimension) const
{
  return mSizer.GetResizePolicy(dimension);
}

void Actor::SetRelayoutEnabled(bool relayoutEnabled)
{
  mSizer.SetRelayoutEnabled(relayoutEnabled);
}

bool Actor::IsRelayoutEnabled() const
{
  return mSizer.IsRelayoutEnabled();
}

void Actor::SetLayoutDirty(bool dirty, Dimension::Type dimension)
{
  mSizer.SetLayoutDirty(dirty, dimension);
}

bool Actor::IsLayoutDirty(Dimension::Type dimension) const
{
  return mSizer.IsLayoutDirty(dimension);
}

bool Actor::RelayoutPossible(Dimension::Type dimension) const
{
  return mSizer.RelayoutPossible(dimension);
}

bool Actor::RelayoutRequired(Dimension::Type dimension) const
{
  return mSizer.RelayoutRequired(dimension);
}

uint32_t Actor::AddRenderer(Renderer& renderer)
{
  if(!mRenderers)
  {
    mRenderers = new RendererContainer(GetEventThreadServices());
  }
  return mRenderers->Add(GetNode(), renderer, mIsBlendEquationSet, mBlendEquation);
}

uint32_t Actor::GetRendererCount() const
{
  return mRenderers ? mRenderers->GetCount() : 0u;
}

RendererPtr Actor::GetRendererAt(uint32_t index)
{
  return mRenderers ? mRenderers->GetRendererAt(index) : nullptr;
}

void Actor::RemoveRenderer(Renderer& renderer)
{
  if(mRenderers)
  {
    mRenderers->Remove(GetNode(), renderer);
  }
}

void Actor::RemoveRenderer(uint32_t index)
{
  if(mRenderers)
  {
    mRenderers->Remove(GetNode(), index);
  }
}

void Actor::SetBlendEquation(DevelBlendEquation::Type blendEquation)
{
  if(Dali::Capabilities::IsBlendEquationSupported(blendEquation))
  {
    if(mBlendEquation != blendEquation)
    {
      mBlendEquation = blendEquation;
      if(mRenderers)
      {
        mRenderers->SetBlending(blendEquation);
      }
    }
    mIsBlendEquationSet = true;
  }
  else
  {
    DALI_LOG_ERROR("Invalid blend equation is entered.\n");
  }
}

DevelBlendEquation::Type Actor::GetBlendEquation() const
{
  return mBlendEquation;
}

void Actor::SetTransparent(bool transparent)
{
  SetTransparentMessage(GetEventThreadServices(), GetNode(), transparent);
}

bool Actor::IsTransparent() const
{
  return GetNode().IsTransparent();
}

void Actor::SetDrawMode(DrawMode::Type drawMode)
{
  // this flag is not animatable so keep the value
  mDrawMode = drawMode;

  // node is being used in a separate thread; queue a message to set the value
  SetDrawModeMessage(GetEventThreadServices(), GetNode(), drawMode);
}

bool Actor::ScreenToLocal(float& localX, float& localY, float screenX, float screenY) const
{
  return mScene && OnScene() && ConvertScreenToLocalRenderTaskList(mScene->GetRenderTaskList(), GetNode().GetWorldMatrix(0), GetCurrentSize(), localX, localY, screenX, screenY);
}

bool Actor::ScreenToLocal(const RenderTask& renderTask, float& localX, float& localY, float screenX, float screenY) const
{
  return OnScene() && ConvertScreenToLocalRenderTask(renderTask, GetNode().GetWorldMatrix(0), GetCurrentSize(), localX, localY, screenX, screenY);
}

bool Actor::ScreenToLocal(const Matrix& viewMatrix, const Matrix& projectionMatrix, const Viewport& viewport, float& localX, float& localY, float screenX, float screenY) const
{
  return OnScene() && ConvertScreenToLocal(viewMatrix, projectionMatrix, GetNode().GetWorldMatrix(0), GetCurrentSize(), viewport, localX, localY, screenX, screenY);
}

ActorGestureData& Actor::GetGestureData()
{
  // Likely scenario is that once gesture-data is created for this actor, the actor will require
  // that gesture for its entire life-time so no need to destroy it until the actor is destroyed
  if(nullptr == mGestureData)
  {
    mGestureData = new ActorGestureData;
  }
  return *mGestureData;
}

bool Actor::IsGestureRequired(GestureType::Value type) const
{
  return mGestureData && mGestureData->IsGestureRequired(type);
}

bool Actor::EmitInterceptTouchEventSignal(const Dali::TouchEvent& touch)
{
  if(mScene && mScene->IsGeometryHittestEnabled())
  {
    return EmitConsumingSignalOr(*this, mInterceptTouchedSignal, touch);
  }
  return EmitConsumingSignal(*this, mInterceptTouchedSignal, touch);
}

bool Actor::EmitTouchEventSignal(const Dali::TouchEvent& touch)
{
  if(mScene && mScene->IsGeometryHittestEnabled())
  {
    return EmitConsumingSignalOr(*this, mTouchedSignal, touch);
  }
  return EmitConsumingSignal(*this, mTouchedSignal, touch);
}

bool Actor::EmitHoverEventSignal(const Dali::HoverEvent& event)
{
  return EmitConsumingSignal(*this, mHoveredSignal, event);
}

bool Actor::EmitInterceptWheelEventSignal(const Dali::WheelEvent& event)
{
  return EmitConsumingSignal(*this, mInterceptWheelSignal, event);
}

bool Actor::EmitWheelEventSignal(const Dali::WheelEvent& event)
{
  return EmitConsumingSignal(*this, mWheelEventSignal, event);
}

void Actor::EmitVisibilityChangedSignal(bool visible, DevelActor::VisibilityChange::Type type)
{
  EmitSignal(*this, mVisibilityChangedSignal, visible, type);
}

void Actor::EmitInheritedVisibilityChangedSignal(bool visible)
{
  EmitSignal(*this, mInheritedVisibilityChangedSignal, visible);
}

void Actor::EmitLayoutDirectionChangedSignal(LayoutDirection::Type type)
{
  EmitSignal(*this, mLayoutDirectionChangedSignal, type);
}

bool Actor::EmitHitTestResultSignal(Integration::Point point, Vector2 hitPointLocal, uint32_t timeStamp)
{
  bool hit = true;

  if(IsHitTestResultRequired())
  {
    Dali::Actor        handle(this);
    Integration::Point newPoint(point);
    newPoint.SetHitActor(handle);
    newPoint.SetLocalPosition(hitPointLocal);
    Dali::TouchEvent touchEvent = Dali::Integration::NewTouchEvent(timeStamp, newPoint);
    hit                         = mHitTestResultSignal.Emit(handle, touchEvent);
  }
  return hit;
}

DevelActor::ChildChangedSignalType& Actor::ChildAddedSignal()
{
  return mParentImpl.ChildAddedSignal();
}

DevelActor::ChildChangedSignalType& Actor::ChildRemovedSignal()
{
  return mParentImpl.ChildRemovedSignal();
}

DevelActor::ChildOrderChangedSignalType& Actor::ChildOrderChangedSignal()
{
  return mParentImpl.ChildOrderChangedSignal();
}

Actor::Actor(DerivedType derivedType, const SceneGraph::Node& node)
: Object(&node),
  mParentImpl(*this),
  mSizer(*this),
  mParent(nullptr),
  mScene(nullptr),
  mRenderers(nullptr),
  mParentOrigin(nullptr),
  mAnchorPoint(nullptr),
  mGestureData(nullptr),
  mInterceptTouchedSignal(),
  mTouchedSignal(),
  mHoveredSignal(),
  mInterceptWheelSignal(),
  mWheelEventSignal(),
  mOnSceneSignal(),
  mOffSceneSignal(),
  mOnRelayoutSignal(),
  mVisibilityChangedSignal(),
  mInheritedVisibilityChangedSignal(),
  mLayoutDirectionChangedSignal(),
  mHitTestResultSignal(),
  mTargetOrientation(Quaternion::IDENTITY),
  mTargetColor(Color::WHITE),
  mTargetPosition(Vector3::ZERO),
  mTargetScale(Vector3::ONE),
  mTouchAreaOffset(0, 0, 0, 0),
  mName(),
  mSortedDepth(0u),
  mDepth(0u),
  mLayer3DParentsCount(0),
  mIsRoot(ROOT_LAYER == derivedType),
  mIsLayer(LAYER == derivedType || ROOT_LAYER == derivedType),
  mIsOnScene(false),
  mSensitive(true),
  mLeaveRequired(false),
  mKeyboardFocusable(false),
  mKeyboardFocusableChildren(true),
  mTouchFocusable(false),
  mOnSceneSignalled(false),
  mInheritPosition(true),
  mInheritOrientation(true),
  mInheritScale(true),
  mPositionUsesAnchorPoint(true),
  mVisible(true),
  mInheritLayoutDirection(true),
  mCaptureAllTouchAfterStart(false),
  mIsBlendEquationSet(false),
  mNeedGesturePropagation(false),
  mUserInteractionEnabled(true),
  mAllowOnlyOwnTouch(false),
  mUseTextureUpdateArea(false),
  mDispatchTouchMotion(true),
  mDispatchHoverMotion(true),
  mLayoutDirection(LayoutDirection::LEFT_TO_RIGHT),
  mDrawMode(DrawMode::NORMAL),
  mColorMode(Node::DEFAULT_COLOR_MODE),
  mClippingMode(ClippingMode::DISABLED),
  mHoverState(PointState::FINISHED),
  mBlendEquation(DevelBlendEquation::ADD)
{
}

void Actor::Initialize()
{
  OnInitialize();

  GetEventThreadServices().RegisterObject(this);
}

Actor::~Actor()
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("~Actor[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }

  // Remove mParent pointers from children even if we're destroying core,
  // to guard against GetParent() & Unparent() calls from CustomActor destructors.
  UnparentChildren();

  // Guard to allow handle destruction after Core has been destroyed
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning()))
  {
    if(mRenderers)
    {
      // Detach all renderers before delete container.
      mRenderers->RemoveAll(GetNode());
    }

    // Root layer will destroy its node in its own destructor
    if(!mIsRoot)
    {
      DestroyNodeMessage(GetEventThreadServices().GetUpdateManager(), GetNode());

      GetEventThreadServices().UnregisterObject(this);
    }
  }
  // Cleanup renderer list
  delete mRenderers;

  // Cleanup optional gesture data
  delete mGestureData;

  // Cleanup optional parent origin and anchor
  delete mParentOrigin;
  delete mAnchorPoint;
}

void Actor::Add(Actor& child, bool notify)
{
  mParentImpl.Add(child, notify);
}

void Actor::Remove(Actor& child, bool notify)
{
  mParentImpl.Remove(child, notify);
}

void Actor::SwitchParent(Actor& newParent)
{
  if(this == &newParent)
  {
    DALI_LOG_ERROR("Cannot add actor to itself");
    return;
  }

  if(!this->OnScene() || !newParent.OnScene())
  {
    DALI_LOG_ERROR("Both of current parent and new parent must be on Scene");
    return;
  }

  newParent.Add(*this, false);
}

uint32_t Actor::GetChildCount() const
{
  return mParentImpl.GetChildCount();
}

ActorPtr Actor::GetChildAt(uint32_t index) const
{
  return mParentImpl.GetChildAt(index);
}

ActorContainer& Actor::GetChildrenInternal()
{
  return mParentImpl.GetChildrenInternal();
}

ActorPtr Actor::FindChildByName(const std::string_view& actorName)
{
  return mParentImpl.FindChildByName(actorName);
}

ActorPtr Actor::FindChildById(const uint32_t id)
{
  return mParentImpl.FindChildById(id);
}

void Actor::UnparentChildren()
{
  mParentImpl.UnparentChildren();
}

void Actor::ConnectToScene(uint32_t parentDepth, uint32_t layer3DParentsCount, bool notify)
{
  // This container is used instead of walking the Actor hierarchy.
  // It protects us when the Actor hierarchy is modified during OnSceneConnectionExternal callbacks.
  ActorContainer connectionList;

  if(mScene)
  {
    mScene->RequestRebuildDepthTree();
  }

  // This stage is not interrupted by user callbacks.
  mParentImpl.RecursiveConnectToScene(connectionList, layer3DParentsCount, parentDepth + 1);

  // Notify applications about the newly connected actors.
  for(const auto& actor : connectionList)
  {
    actor->NotifyStageConnection(notify);
  }

  RelayoutRequest();
}

/**
 * This method is called when the Actor is connected to the Stage.
 * The parent must have added its Node to the scene-graph.
 * The child must connect its Node to the parent's Node.
 * This is recursive; the child calls ConnectToScene() for its children.
 */
void Actor::ConnectToSceneGraph()
{
  DALI_ASSERT_DEBUG(mParent != NULL);

  // Reparent Node in next Update
  ConnectNodeMessage(GetEventThreadServices().GetUpdateManager(), GetParent()->GetNode(), GetNode());

  // Request relayout on all actors that are added to the scenegraph
  RelayoutRequest();

  // Notification for Object::Observers
  OnSceneObjectAdd();
}

void Actor::NotifyStageConnection(bool notify)
{
  // Actors can be removed (in a callback), before the on-stage stage is reported.
  // The actor may also have been reparented, in which case mOnSceneSignalled will be true.
  if(OnScene() && !mOnSceneSignalled)
  {
    if(notify)
    {
      // Notification for external (CustomActor) derived classes
      OnSceneConnectionExternal(mDepth);

      if(!mOnSceneSignal.Empty())
      {
        Dali::Actor handle(this);
        mOnSceneSignal.Emit(handle);
      }
    }

    // Guard against Remove during callbacks
    if(OnScene())
    {
      mOnSceneSignalled = true; // signal required next time Actor is removed
    }
  }
}

void Actor::DisconnectFromStage(bool notify)
{
  // This container is used instead of walking the Actor hierachy.
  // It protects us when the Actor hierachy is modified during OnSceneDisconnectionExternal callbacks.
  ActorContainer disconnectionList;

  if(mScene)
  {
    mScene->RequestRebuildDepthTree();
  }

  // This stage is not interrupted by user callbacks
  mParentImpl.RecursiveDisconnectFromScene(disconnectionList);

  // Notify applications about the newly disconnected actors.
  for(const auto& actor : disconnectionList)
  {
    actor->NotifyStageDisconnection(notify);
  }
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

void Actor::NotifyStageDisconnection(bool notify)
{
  // Actors can be added (in a callback), before the off-stage state is reported.
  // Also if the actor was added & removed before mOnSceneSignalled was set, then we don't notify here.
  // only do this step if there is a stage, i.e. Core is not being shut down
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning()) && !OnScene() && mOnSceneSignalled)
  {
    if(notify)
    {
      // Notification for external (CustomeActor) derived classes
      OnSceneDisconnectionExternal();

      if(!mOffSceneSignal.Empty())
      {
        Dali::Actor handle(this);
        mOffSceneSignal.Emit(handle);
      }
    }

    // Guard against Add during callbacks
    if(!OnScene())
    {
      mOnSceneSignalled = false; // signal required next time Actor is added
    }
  }
}

bool Actor::IsNodeConnected() const
{
  return OnScene() && (IsRoot() || GetNode().GetParent());
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
  OwnerPointer<SceneGraph::NodeDepths> sceneGraphNodeDepths(new SceneGraph::NodeDepths());

  int32_t depthIndex = 1;
  mParentImpl.DepthTraverseActorTree(sceneGraphNodeDepths, depthIndex);

  SetDepthIndicesMessage(GetEventThreadServices().GetUpdateManager(), sceneGraphNodeDepths);
  DALI_LOG_TIMER_END(depthTimer, gLogFilter, Debug::Concise, "Depth tree traversal time: ");
}

void Actor::EmitInheritedVisibilityChangedSignalRecursively(bool visible)
{
  ActorContainer inheritedVisibilityChangedList;
  mParentImpl.InheritVisibilityRecursively(inheritedVisibilityChangedList);
  // Notify applications about the newly connected actors.
  for(const auto& actor : inheritedVisibilityChangedList)
  {
    actor->EmitInheritedVisibilityChangedSignal(visible);
  }
}

void Actor::SetDefaultProperty(Property::Index index, const Property::Value& property)
{
  PropertyHandler::SetDefaultProperty(*this, index, property);
}

// TODO: This method needs to be removed
void Actor::SetSceneGraphProperty(Property::Index index, const PropertyMetadata& entry, const Property::Value& value)
{
  PropertyHandler::SetSceneGraphProperty(index, entry, value, GetEventThreadServices(), GetNode());
}

Property::Value Actor::GetDefaultProperty(Property::Index index) const
{
  Property::Value value;

  if(!GetCachedPropertyValue(index, value))
  {
    // If property value is not stored in the event-side, then it must be a scene-graph only property
    GetCurrentPropertyValue(index, value);
  }

  return value;
}

Property::Value Actor::GetDefaultPropertyCurrentValue(Property::Index index) const
{
  Property::Value value;

  if(!GetCurrentPropertyValue(index, value))
  {
    // If unable to retrieve scene-graph property value, then it must be an event-side only property
    GetCachedPropertyValue(index, value);
  }

  return value;
}

void Actor::OnNotifyDefaultPropertyAnimation(Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType)
{
  PropertyHandler::OnNotifyDefaultPropertyAnimation(*this, animation, index, value, animationType);
}

const PropertyBase* Actor::GetSceneObjectAnimatableProperty(Property::Index index) const
{
  const PropertyBase* property = PropertyHandler::GetSceneObjectAnimatableProperty(index, GetNode());
  if(!property)
  {
    // not our property, ask base
    property = Object::GetSceneObjectAnimatableProperty(index);
  }

  return property;
}

const PropertyInputImpl* Actor::GetSceneObjectInputProperty(Property::Index index) const
{
  const PropertyInputImpl* property = PropertyHandler::GetSceneObjectInputProperty(index, GetNode());
  if(!property)
  {
    // reuse animatable property getter as animatable properties are inputs as well
    // animatable property chains back to Object::GetSceneObjectInputProperty() so all properties get covered
    property = GetSceneObjectAnimatableProperty(index);
  }

  return property;
}

int32_t Actor::GetPropertyComponentIndex(Property::Index index) const
{
  int32_t componentIndex = PropertyHandler::GetPropertyComponentIndex(index);
  if(Property::INVALID_COMPONENT_INDEX == componentIndex)
  {
    // ask base
    componentIndex = Object::GetPropertyComponentIndex(index);
  }

  return componentIndex;
}

const SceneGraph::Node& Actor::GetNode() const
{
  return *static_cast<const SceneGraph::Node*>(mUpdateObject);
}

void Actor::Raise()
{
  CheckParentAndCall(mParent, *this, &ActorParent::RaiseChild);
}

void Actor::Lower()
{
  CheckParentAndCall(mParent, *this, &ActorParent::LowerChild);
}

void Actor::RaiseToTop()
{
  CheckParentAndCall(mParent, *this, &ActorParent::RaiseChildToTop);
}

void Actor::LowerToBottom()
{
  CheckParentAndCall(mParent, *this, &ActorParent::LowerChildToBottom);
}

void Actor::RaiseAbove(Internal::Actor& target)
{
  CheckParentAndCall(mParent, *this, target, &ActorParent::RaiseChildAbove);
}

void Actor::LowerBelow(Internal::Actor& target)
{
  CheckParentAndCall(mParent, *this, target, &ActorParent::LowerChildBelow);
}

void Actor::SetParent(ActorParent* parent, bool notify)
{
  bool emitInheritedVisible = false;
  bool visiblility          = true;
  if(parent)
  {
    DALI_ASSERT_ALWAYS(!mParent && "Actor cannot have 2 parents");

    mParent            = parent;
    Actor* parentActor = static_cast<Actor*>(parent);
    mScene             = parentActor->mScene;

    if(!EventThreadServices::IsShuttingDown() && // Don't emit signals or send messages during Core destruction
       parentActor->OnScene())
    {
      // Instruct each actor to create a corresponding node in the scene graph
      ConnectToScene(parentActor->GetHierarchyDepth(), parentActor->GetLayer3DParentCount(), notify);

      Actor* actor         = this;
      // OnScene should be checked, this actor can be removed during OnSceneConnection.
      emitInheritedVisible = OnScene() && mScene->IsVisible();
      while(emitInheritedVisible && actor)
      {
        emitInheritedVisible &= actor->GetProperty(Dali::Actor::Property::VISIBLE).Get<bool>();
        actor = actor->GetParent();
      }
    }

    // Resolve the name and index for the child properties if any
    ResolveChildProperties();
  }
  else // parent being set to NULL
  {
    DALI_ASSERT_ALWAYS(mParent != nullptr && "Actor should have a parent");

    if(!EventThreadServices::IsShuttingDown() && // Don't emit signals or send messages during Core destruction
       OnScene())
    {
      Actor* actor         = this;
      emitInheritedVisible = mScene->IsVisible();
      while(emitInheritedVisible && actor)
      {
        emitInheritedVisible &= actor->GetProperty(Dali::Actor::Property::VISIBLE).Get<bool>();
        actor = actor->GetParent();
      }
      visiblility = false;
    }

    mParent = nullptr;

    if(!EventThreadServices::IsShuttingDown() && // Don't emit signals or send messages during Core destruction
       OnScene())
    {
      // Disconnect the Node & its children from the scene-graph.
      DisconnectNodeMessage(GetEventThreadServices().GetUpdateManager(), GetNode());

      // Instruct each actor to discard pointers to the scene-graph
      DisconnectFromStage(notify);
    }

    mScene = nullptr;
  }

  if(emitInheritedVisible)
  {
    EmitInheritedVisibilityChangedSignalRecursively(visiblility);
  }
}

Rect<> Actor::CalculateScreenExtents() const
{
  if(mLayer3DParentsCount == 0)
  {
    // We can assume that this actor is under 2d layer. Use faster, but imprecise algorithm
    return CalculateActorScreenExtents(*this);
  }
  else
  {
    return CalculateActorScreenExtentsRenderTaskList(*this);
  }
}

Rect<> Actor::CalculateCurrentScreenExtents() const
{
  BufferIndex bufferIndex = GetEventThreadServices().GetEventBufferIndex();
  if(mLayer3DParentsCount == 0)
  {
    // We can assume that this actor is under 2d layer. Use faster, but imprecise algorithm
    return CalculateCurrentActorScreenExtents(*this, bufferIndex);
  }
  else
  {
    return CalculateCurrentActorScreenExtentsRenderTaskList(*this, bufferIndex);
  }
}

Vector3 Actor::GetAnchorPointForPosition() const
{
  return (mPositionUsesAnchorPoint ? GetCurrentAnchorPoint() : AnchorPoint::TOP_LEFT);
}

bool Actor::GetCachedPropertyValue(Property::Index index, Property::Value& value) const
{
  return PropertyHandler::GetCachedPropertyValue(*this, index, value);
}

bool Actor::GetCurrentPropertyValue(Property::Index index, Property::Value& value) const
{
  return PropertyHandler::GetCurrentPropertyValue(*this, index, value);
}

bool Actor::RelayoutDependentOnParent(Dimension::Type dimension)
{
  return mSizer.RelayoutDependentOnParent(dimension);
}

bool Actor::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return mSizer.RelayoutDependentOnChildrenBase(dimension);
}

bool Actor::RelayoutDependentOnDimension(Dimension::Type dimension, Dimension::Type dependentDimension)
{
  return mSizer.RelayoutDependentOnDimension(dimension, dependentDimension);
}

void Actor::SetPadding(const Vector2& padding, Dimension::Type dimension)
{
  mSizer.SetPadding(padding, dimension);
}

Vector2 Actor::GetPadding(Dimension::Type dimension) const
{
  return mSizer.GetPadding(dimension);
}

void Actor::SetLayoutNegotiated(bool negotiated, Dimension::Type dimension)
{
  mSizer.SetLayoutNegotiated(negotiated, dimension);
}

bool Actor::IsLayoutNegotiated(Dimension::Type dimension) const
{
  return mSizer.IsLayoutNegotiated(dimension);
}

float Actor::GetHeightForWidthBase(float width)
{
  // Can be overridden in derived class
  return mSizer.GetHeightForWidthBase(width);
}

float Actor::GetWidthForHeightBase(float height)
{
  // Can be overridden in derived class
  return mSizer.GetWidthForHeightBase(height);
}

float Actor::CalculateChildSizeBase(const Dali::Actor& child, Dimension::Type dimension)
{
  // Can be overridden in derived class
  return mSizer.CalculateChildSizeBase(child, dimension);
}

bool Actor::RelayoutDependentOnChildrenBase(Dimension::Type dimension)
{
  return mSizer.RelayoutDependentOnChildrenBase(dimension);
}

float Actor::CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
{
  // Can be overridden in derived class
  return mSizer.CalculateChildSizeBase(child, dimension);
}

float Actor::GetHeightForWidth(float width)
{
  // Can be overridden in derived class
  return mSizer.GetHeightForWidthBase(width);
}

float Actor::GetWidthForHeight(float height)
{
  // Can be overridden in derived class
  return mSizer.GetWidthForHeightBase(height);
}

float Actor::GetRelayoutSize(Dimension::Type dimension) const
{
  return mSizer.GetRelayoutSize(dimension);
}

void Actor::NegotiateSize(const Vector2& allocatedSize, RelayoutContainer& container)
{
  mSizer.NegotiateSize(allocatedSize, container);
}

void Actor::RelayoutRequest(Dimension::Type dimension)
{
  mSizer.RelayoutRequest(dimension);
}

void Actor::SetMinimumSize(float size, Dimension::Type dimension)
{
  mSizer.SetMinimumSize(size, dimension);
}

float Actor::GetMinimumSize(Dimension::Type dimension) const
{
  return mSizer.GetMinimumSize(dimension);
}

void Actor::SetMaximumSize(float size, Dimension::Type dimension)
{
  mSizer.SetMaximumSize(size, dimension);
}

float Actor::GetMaximumSize(Dimension::Type dimension) const
{
  return mSizer.GetMaximumSize(dimension);
}

void Actor::SetVisibleInternal(bool visible, SendMessage::Type sendMessage)
{
  if(mVisible != visible)
  {
    if(sendMessage == SendMessage::TRUE)
    {
      // node is being used in a separate thread; queue a message to set the value & base value
      SceneGraph::NodePropertyMessage<bool>::Send(GetEventThreadServices(), &GetNode(), &GetNode().mVisible, &AnimatableProperty<bool>::Bake, visible);

      RequestRenderingMessage(GetEventThreadServices().GetUpdateManager());
    }

    Actor* actor                = this->GetParent();
    bool   emitInheritedVisible = OnScene() && mScene->IsVisible();
    while(emitInheritedVisible && actor)
    {
      emitInheritedVisible &= actor->GetProperty(Dali::Actor::Property::VISIBLE).Get<bool>();
      actor = actor->GetParent();
    }

    mVisible = visible;

    // Emit the signal on this actor and all its children
    mParentImpl.EmitVisibilityChangedSignalRecursively(visible, DevelActor::VisibilityChange::SELF);
    if(emitInheritedVisible)
    {
      EmitInheritedVisibilityChangedSignalRecursively(visible);
    }
  }
}

void Actor::SetSiblingOrderOfChild(Actor& child, uint32_t order)
{
  mParentImpl.SetSiblingOrderOfChild(child, order);
}

uint32_t Actor::GetSiblingOrderOfChild(const Actor& child) const
{
  return mParentImpl.GetSiblingOrderOfChild(child);
}

void Actor::RaiseChild(Actor& child)
{
  mParentImpl.RaiseChild(child);
}

void Actor::LowerChild(Actor& child)
{
  mParentImpl.LowerChild(child);
}

void Actor::RaiseChildToTop(Actor& child)
{
  mParentImpl.RaiseChildToTop(child);
}

void Actor::LowerChildToBottom(Actor& child)
{
  mParentImpl.LowerChildToBottom(child);
}

void Actor::RaiseChildAbove(Actor& child, Actor& target)
{
  mParentImpl.RaiseChildAbove(child, target);
}

void Actor::LowerChildBelow(Actor& child, Actor& target)
{
  mParentImpl.LowerChildBelow(child, target);
}

void Actor::SetInheritLayoutDirection(bool inherit)
{
  if(mInheritLayoutDirection != inherit)
  {
    mInheritLayoutDirection = inherit;

    if(inherit && mParent)
    {
      mParentImpl.InheritLayoutDirectionRecursively(GetParent()->mLayoutDirection);
    }
  }
}

void Actor::SetUpdateAreaHint(const Vector4& updateAreaHint)
{
  // node is being used in a separate thread; queue a message to set the value & base value
  SetUpdateAreaHintMessage(GetEventThreadServices(), GetNode(), updateAreaHint);
}

} // namespace Internal

} // namespace Dali
