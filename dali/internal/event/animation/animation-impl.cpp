/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/public-api/object/property-map.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/event/animation/animation-playlist.h>
#include <dali/internal/event/animation/animator-connector.h>
#include <dali/internal/event/animation/key-frames-impl.h>
#include <dali/internal/event/animation/path-impl.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/type-registry.h>

using Dali::Internal::SceneGraph::AnimatorBase;
using Dali::Internal::SceneGraph::Shader;
using Dali::Internal::SceneGraph::UpdateManager;

namespace Dali
{
namespace Internal
{
static bool SHOW_VALUE = true;
static bool HIDE_VALUE = false;

namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);

#if defined(DEBUG_ENABLED)
Debug::Filter* gAnimFilter = Debug::Filter::New(Debug::NoLogging, false, "DALI_LOG_ANIMATION");
#endif

static constexpr size_t WARNING_PRINT_THRESHOLD = 10000u;

// Signals

static constexpr std::string_view SIGNAL_FINISHED = "finished";

// Actions

static constexpr std::string_view ACTION_PLAY  = "play";
static constexpr std::string_view ACTION_STOP  = "stop";
static constexpr std::string_view ACTION_PAUSE = "pause";

#if defined(DEBUG_ENABLED) || defined(TRACE_ENABLED)
// clang-format off
#define GET_ANIMATION_INTERNAL_STATE_STRING(internalState)                                         \
  internalState == Animation::InternalState::STOPPED                 ? "STOPPED"                 : \
  internalState == Animation::InternalState::PLAYING                 ? "PLAYING"                 : \
  internalState == Animation::InternalState::PAUSED                  ? "PAUSED"                  : \
  internalState == Animation::InternalState::CLEARED                 ? "CLEARED"                 : \
  internalState == Animation::InternalState::STOPPING                ? "STOPPING"                : \
  internalState == Animation::InternalState::PLAYING_DURING_STOPPING ? "PLAYING_DURING_STOPPING" : \
  internalState == Animation::InternalState::PAUSED_DURING_STOPPING  ? "PAUSED_DURING_STOPPING"  : \
                                                                       "Unknown"

#define GET_ANIMATION_STATE_STRING(state)                \
  state == Dali::Animation::State::STOPPED ? "STOPPED" : \
  state == Dali::Animation::State::PLAYING ? "PLAYING" : \
  state == Dali::Animation::State::PAUSED  ? "PAUSED"  : \
                                             "Unknown"

#define GET_ANIMATION_END_ACTION_STRING(endAction) \
  endAction == Dali::Animation::EndAction::BAKE       ? "BAKE" :       \
  endAction == Dali::Animation::EndAction::DISCARD    ? "DISCARD" :    \
  endAction == Dali::Animation::EndAction::BAKE_FINAL ? "BAKE_FINAL" : \
                                                        "Unknown"
// clang-format on

#if defined(TRACE_ENABLED)
// DevNote : Use trace marker to print logs at release mode.
#define DALI_LOG_ANIMATION_INFO(format, ...)         \
  if(gTraceFilter && gTraceFilter->IsTraceEnabled()) \
  {                                                  \
    DALI_LOG_DEBUG_INFO(format, ##__VA_ARGS__);      \
  }                                                  \
  DALI_LOG_INFO(gAnimFilter, Debug::Verbose, format, ##__VA_ARGS__)
#else
#define DALI_LOG_ANIMATION_INFO(format, ...) \
  DALI_LOG_INFO(gAnimFilter, Debug::Verbose, format, ##__VA_ARGS__)
#endif
#else
#define DALI_LOG_ANIMATION_INFO(format, ...)
#endif

BaseHandle Create()
{
  return Dali::Animation::New(0.f);
}

TypeRegistration mType(typeid(Dali::Animation), typeid(Dali::BaseHandle), Create);

SignalConnectorType signalConnector1(mType, std::string(SIGNAL_FINISHED), &Animation::DoConnectSignal);

TypeAction action1(mType, std::string(ACTION_PLAY), &Animation::DoAction);
TypeAction action2(mType, std::string(ACTION_STOP), &Animation::DoAction);
TypeAction action3(mType, std::string(ACTION_PAUSE), &Animation::DoAction);

const Dali::Animation::EndAction     DEFAULT_END_ACTION(Dali::Animation::BAKE);
const Dali::Animation::EndAction     DEFAULT_DISCONNECT_ACTION(Dali::Animation::BAKE_FINAL);
const Dali::Animation::Interpolation DEFAULT_INTERPOLATION(Dali::Animation::LINEAR);
const Dali::AlphaFunction            DEFAULT_ALPHA_FUNCTION(Dali::AlphaFunction::DEFAULT);

/**
 * Helper to tell if a property is animatable (if we have animators for it)
 *
 * @param type type to check
 * @return true if animatable
 */
inline bool IsAnimatable(Property::Type type)
{
  bool animatable = false;
  switch(type)
  {
    case Property::BOOLEAN:
    case Property::FLOAT:
    case Property::INTEGER:
    case Property::VECTOR2:
    case Property::VECTOR3:
    case Property::VECTOR4:
    case Property::ROTATION:
    {
      animatable = true;
      break;
    }
    case Property::MATRIX:  // matrix is allowed as a scene graph property but there's no animators for it
    case Property::MATRIX3: // matrix3 is allowed as a scene graph property but there's no animators for it
    case Property::NONE:
    case Property::RECTANGLE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::EXTENTS:
    {
      break;
    }
  }
  return animatable;
}

/**
 * Helper to validate and convert animation input values
 *
 * @param[in] propertyType type of the property that is being animated
 * @param[in] period time period of the animation
 * @param[in, out] convertedValue if the value needs conversion, this will contain the converted value.
 */
void ValidateAndConvertParameters(Property::Type propertyType, const TimePeriod& period, Property::Value& convertedValue)
{
  // destination value has to be animatable
  DALI_ASSERT_ALWAYS(IsAnimatable(propertyType) && "Property type is not animatable");
  DALI_ASSERT_ALWAYS(IsAnimatable(convertedValue.GetType()) && "Target value is not animatable");
  DALI_ASSERT_ALWAYS(period.durationSeconds >= 0 && "Duration must be >=0");

  DALI_ASSERT_ALWAYS(convertedValue.ConvertType(propertyType) && "Target types could not be convert to Property type");
}

/**
 * @brief Converts the internal state to the target state.
 * @param[in, out] currentState The current state of the animation.
 * @param[in] targetState The target state of the animation.
 * @return True if the animation state has been changed.
 */
bool InternalStateConverter(uint32_t animationId, Internal::Animation::InternalState& currentState, Dali::Animation::State targetState)
{
  bool changed = false;
  DALI_LOG_ANIMATION_INFO("Animation[%u] state change %s -> %s\n", animationId, GET_ANIMATION_INTERNAL_STATE_STRING(currentState), GET_ANIMATION_STATE_STRING(targetState));
  switch(targetState)
  {
    case Dali::Animation::PLAYING:
    {
      switch(currentState)
      {
        case Internal::Animation::CLEARED:
        case Internal::Animation::STOPPED:
        case Internal::Animation::PAUSED:
        {
          currentState = Internal::Animation::InternalState::PLAYING;
          changed      = true;
          break;
        }
        case Internal::Animation::STOPPING:
        case Internal::Animation::PAUSED_DURING_STOPPING:
        {
          currentState = Internal::Animation::InternalState::PLAYING_DURING_STOPPING;
          changed      = true;
          break;
        }
        default:
        {
          break;
        }
      }
      break;
    }
    case Dali::Animation::PAUSED:
    {
      switch(currentState)
      {
        case Internal::Animation::CLEARED:
        case Internal::Animation::STOPPED:
        case Internal::Animation::PLAYING:
        {
          currentState = Internal::Animation::InternalState::PAUSED;
          changed      = true;
          break;
        }
        case Internal::Animation::STOPPING:
        case Internal::Animation::PLAYING_DURING_STOPPING:
        {
          currentState = Internal::Animation::InternalState::PAUSED_DURING_STOPPING;
          changed      = true;
          break;
        }
        default:
        {
          break;
        }
      }
      break;
    }
    case Dali::Animation::STOPPED:
    {
      switch(currentState)
      {
        case Internal::Animation::PLAYING:
        case Internal::Animation::PLAYING_DURING_STOPPING:
        case Internal::Animation::PAUSED:
        case Internal::Animation::PAUSED_DURING_STOPPING:
        {
          currentState = Internal::Animation::InternalState::STOPPING;
          changed      = true;
          break;
        }
        default:
        {
          break;
        }
      }
      break;
    }
  }
  return changed;
}

} // anonymous namespace

AnimationPtr Animation::New(float durationSeconds)
{
  if(durationSeconds < 0.0f)
  {
    DALI_LOG_WARNING("duration should be greater than 0.0f.\n");
    durationSeconds = 0.0f;
  }

  ThreadLocalStorage& tls       = ThreadLocalStorage::Get();
  AnimationPtr        animation = new Animation(tls.GetEventThreadServices(), tls.GetAnimationPlaylist(), durationSeconds, DEFAULT_END_ACTION, DEFAULT_DISCONNECT_ACTION, DEFAULT_ALPHA_FUNCTION);

  // Second-phase construction
  animation->Initialize();

  return animation;
}

Animation::Animation(EventThreadServices& eventThreadServices, AnimationPlaylist& playlist, float durationSeconds, EndAction endAction, EndAction disconnectAction, AlphaFunction defaultAlpha)
: EventThreadServicesHolder(eventThreadServices),
  mPlaylist(playlist),
  mDefaultAlpha(defaultAlpha),
  mDurationSeconds(durationSeconds),
  mEndAction(endAction),
  mDisconnectAction(disconnectAction)
{
}

void Animation::Initialize()
{
  // Connect to the animation playlist
  mPlaylist.AnimationCreated(*this);

  CreateSceneObject();

  RegisterObject();
}

Animation::~Animation()
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("~Animation[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }

  // Guard to allow handle destruction after Core has been destroyed
  if(Stage::IsInstalled())
  {
    // Disconnect from the animation playlist
    mPlaylist.AnimationDestroyed(*this);

    DestroySceneObject();

    UnregisterObject();
  }
}

void Animation::CreateSceneObject()
{
  DALI_ASSERT_DEBUG(mAnimation == NULL);

  // Create a new animation, Keep a const pointer to the animation.
  mAnimation = SceneGraph::Animation::New(mDurationSeconds, mSpeedFactor, mPlayRange, mLoopCount, mEndAction, mDisconnectAction);

  // Set id of scene graph animation
  mAnimationId = mAnimation->GetNotifyId();

  DALI_LOG_ANIMATION_INFO("Animation[%u] Created\n", mAnimationId);

  OwnerPointer<SceneGraph::Animation> transferOwnership(const_cast<SceneGraph::Animation*>(mAnimation));
  AddAnimationMessage(GetEventThreadServices().GetUpdateManager(), transferOwnership);

  // Setup mapping infomations between scenegraph animation
  mPlaylist.MapNotifier(mAnimation, *this);
}

void Animation::DestroySceneObject()
{
  if(mAnimation != nullptr)
  {
    // Remove mapping infomations
    mPlaylist.UnmapNotifier(mAnimation);

    DALI_LOG_ANIMATION_INFO("Animation[%u] Destroyed\n", mAnimationId);

    // Remove animation using a message to the update manager
    RemoveAnimationMessage(GetEventThreadServices().GetUpdateManager(), *mAnimation);
    mAnimation = nullptr;

    // Reset id
    mAnimationId = 0u;
  }
}

void Animation::SetDuration(float seconds)
{
  if(seconds < 0.0f)
  {
    DALI_LOG_WARNING("duration should be greater than 0.0f.\n");
    seconds = 0.0f;
  }

  mDurationSeconds = seconds;
  DALI_LOG_ANIMATION_INFO("Animation[%u] SetDuration %f (s)\n", mAnimationId, mDurationSeconds);

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetDurationMessage(GetEventThreadServices(), *mAnimation, seconds);
}

void Animation::SetProgressNotification(float progress)
{
  // mAnimation is being used in a separate thread; queue a message to set the value
  mProgressReachedMarker = progress;
}

float Animation::GetProgressNotification()
{
  return mProgressReachedMarker;
}

float Animation::GetDuration() const
{
  // This is not animatable; the cached value is up-to-date.
  return mDurationSeconds;
}

void Animation::SetLooping(bool on)
{
  SetLoopCount(on ? 0 : 1);
}

void Animation::SetLoopCount(int32_t count)
{
  DALI_LOG_ANIMATION_INFO("Animation[%u] SetLoopCount[%d]\n", mAnimationId, count);

  // Cache for public getters
  mLoopCount = count;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetLoopingMessage(GetEventThreadServices(), *mAnimation, mLoopCount);
}

int32_t Animation::GetLoopCount()
{
  return mLoopCount;
}

int32_t Animation::GetCurrentLoop()
{
  int32_t loopCount = 0;
  if(mAnimation) // always exists in practice
  {
    loopCount = mAnimation->GetCurrentLoop();
  }

  return loopCount;
}

bool Animation::IsLooping() const
{
  return mLoopCount != 1;
}

void Animation::SetEndAction(EndAction action)
{
  DALI_LOG_ANIMATION_INFO("Animation[%u] SetEndAction[%s]\n", mAnimationId, GET_ANIMATION_END_ACTION_STRING(action));

  // Cache for public getters
  mEndAction = action;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetEndActionMessage(GetEventThreadServices(), *mAnimation, action);
}

Dali::Animation::EndAction Animation::GetEndAction() const
{
  // This is not animatable; the cached value is up-to-date.
  return mEndAction;
}

void Animation::SetDisconnectAction(EndAction action)
{
  // Cache for public getters
  mDisconnectAction = action;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetDisconnectActionMessage(GetEventThreadServices(), *mAnimation, action);
}

Dali::Animation::EndAction Animation::GetDisconnectAction() const
{
  // This is not animatable; the cached value is up-to-date.
  return mDisconnectAction;
}

void Animation::Play()
{
  DALI_LOG_ANIMATION_INFO("Animation[%u] Play() connectors : %zu, internal state : %s\n", mAnimationId, mConnectors.Count(), GET_ANIMATION_INTERNAL_STATE_STRING(mState));

  // Update the current playlist
  mPlaylist.OnPlay(*this);

  InternalStateConverter(GetAnimationId(), mState, Dali::Animation::PLAYING);

  NotifyObjects(Notify::USE_TARGET_VALUE);

  SendFinalProgressNotificationMessage();

  // mAnimation is being used in a separate thread; queue a Play message
  PlayAnimationMessage(GetEventThreadServices(), *mAnimation);
}

void Animation::PlayFrom(float progress)
{
  if(progress >= mPlayRange.x && progress <= mPlayRange.y)
  {
    DALI_LOG_ANIMATION_INFO("Animation[%u] PlayFrom(%f) connectors : %zu, internal state : %s\n", mAnimationId, progress, mConnectors.Count(), GET_ANIMATION_INTERNAL_STATE_STRING(mState));

    // Update the current playlist
    mPlaylist.OnPlay(*this);

    InternalStateConverter(GetAnimationId(), mState, Dali::Animation::PLAYING);

    NotifyObjects(Notify::USE_TARGET_VALUE);

    SendFinalProgressNotificationMessage();

    // mAnimation is being used in a separate thread; queue a Play message
    PlayAnimationFromMessage(GetEventThreadServices(), *mAnimation, progress);
  }
}

void Animation::PlayAfter(float delaySeconds)
{
  // The negative delay means play immediately.
  delaySeconds = std::max(0.f, delaySeconds);

  mDelaySeconds = delaySeconds;

  DALI_LOG_ANIMATION_INFO("Animation[%u] PlayAfter(%f) connectors : %zu, internal state : %s\n", mAnimationId, mDelaySeconds, mConnectors.Count(), GET_ANIMATION_INTERNAL_STATE_STRING(mState));

  // Update the current playlist
  mPlaylist.OnPlay(*this);

  InternalStateConverter(GetAnimationId(), mState, Dali::Animation::PLAYING);

  NotifyObjects(Notify::USE_TARGET_VALUE);

  SendFinalProgressNotificationMessage();

  // mAnimation is being used in a separate thread; queue a message to set the value
  PlayAfterMessage(GetEventThreadServices(), *mAnimation, delaySeconds);
}

void Animation::Pause()
{
  DALI_LOG_ANIMATION_INFO("Animation[%u] Pause() internal state : %s\n", mAnimationId, GET_ANIMATION_INTERNAL_STATE_STRING(mState));
  if(InternalStateConverter(GetAnimationId(), mState, Dali::Animation::PAUSED))
  {
    // mAnimation is being used in a separate thread; queue a Pause message
    PauseAnimationMessage(GetEventThreadServices(), *mAnimation);

    // Notify the objects with the _paused_, i.e. current values
    NotifyObjects(Notify::FORCE_CURRENT_VALUE);
  }
}

Dali::Animation::State Animation::GetState() const
{
  Dali::Animation::State state = Dali::Animation::State::STOPPED;
  switch(mState)
  {
    case Dali::Internal::Animation::InternalState::STOPPED:
    case Dali::Internal::Animation::InternalState::CLEARED:
    case Dali::Internal::Animation::InternalState::STOPPING:
    {
      state = Dali::Animation::State::STOPPED;
      break;
    }
    case Dali::Internal::Animation::InternalState::PLAYING:
    case Dali::Internal::Animation::InternalState::PLAYING_DURING_STOPPING:
    {
      state = Dali::Animation::State::PLAYING;
      break;
    }
    case Dali::Internal::Animation::InternalState::PAUSED:
    case Dali::Internal::Animation::InternalState::PAUSED_DURING_STOPPING:
    {
      state = Dali::Animation::State::PAUSED;
      break;
    }
  }
  return state;
}

void Animation::Stop()
{
  DALI_LOG_ANIMATION_INFO("Animation[%u] Stop() internal state : %s\n", mAnimationId, GET_ANIMATION_INTERNAL_STATE_STRING(mState));
  if(InternalStateConverter(GetAnimationId(), mState, Dali::Animation::STOPPED))
  {
    // mAnimation is being used in a separate thread; queue a Stop message
    StopAnimationMessage(GetEventThreadServices().GetUpdateManager(), *mAnimation);

    // Only notify the objects with the _stopped_, i.e. current values if the end action is set to BAKE
    if(mEndAction == EndAction::BAKE)
    {
      NotifyObjects(Notify::USE_CURRENT_VALUE);
    }
  }
}

void Animation::Clear()
{
  DALI_ASSERT_DEBUG(mAnimation);

  DALI_LOG_ANIMATION_INFO("Animation[%u] Clear() connectors : %zu, internal state : %s\n", mAnimationId, mConnectors.Count(), GET_ANIMATION_INTERNAL_STATE_STRING(mState));

  if(mConnectors.Empty() && mState == Dali::Internal::Animation::CLEARED)
  {
    // Animation is empty. Fast-out
    return;
  }

  // Only notify the objects with the current values if the end action is set to BAKE
  if(mEndAction == EndAction::BAKE && GetState() != Dali::Animation::STOPPED)
  {
    NotifyObjects(Notify::USE_CURRENT_VALUE);
  }

  // Remove all the connectors
  mConnectors.Clear();

  // Reset the connector target values
  mConnectorTargetValues.clear();
  mConnectorTargetValuesSortRequired = false;

  // mAnimation is being used in a separate thread; queue a Clear message
  ClearAnimationMessage(GetEventThreadServices().GetUpdateManager(), *mAnimation);

  // Reset the notification count and relative values, since the new scene-object has never been played
  mNotificationCount = 0;
  mState             = Dali::Internal::Animation::CLEARED;

  // Update the current playlist
  mPlaylist.OnClear(*this, true);
}

void Animation::AnimateBy(Property& target, Property::Value relativeValue)
{
  AnimateBy(target, std::move(relativeValue), mDefaultAlpha, TimePeriod(mDurationSeconds));
}

void Animation::AnimateBy(Property& target, Property::Value relativeValue, AlphaFunction alpha)
{
  AnimateBy(target, std::move(relativeValue), alpha, TimePeriod(mDurationSeconds));
}

void Animation::AnimateBy(Property& target, Property::Value relativeValue, TimePeriod period)
{
  AnimateBy(target, std::move(relativeValue), mDefaultAlpha, period);
}

void Animation::AnimateBy(Property& target, Property::Value relativeValue, AlphaFunction alpha, TimePeriod period)
{
  Object&              object       = GetImplementation(target.object);
  const Property::Type propertyType = (target.componentIndex == Property::INVALID_COMPONENT_INDEX) ? object.GetPropertyType(target.propertyIndex) : Property::FLOAT;

  // validate and convert animation parameters, if component index is set then use float as checked type
  ValidateAndConvertParameters(propertyType, period, relativeValue);

  ExtendDuration(period);

  // keep the current count.
  auto connectorIndex = mConnectors.Count();

  switch(propertyType)
  {
    case Property::BOOLEAN:
    {
      AddAnimatorConnector(AnimatorConnector<bool>::New(object,
                                                        target.propertyIndex,
                                                        target.componentIndex,
                                                        AnimateByBoolean(relativeValue.Get<bool>()),
                                                        alpha,
                                                        period));
      break;
    }

    case Property::INTEGER:
    {
      AddAnimatorConnector(AnimatorConnector<int32_t>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           AnimateByInteger(relativeValue.Get<int32_t>()),
                                                           alpha,
                                                           period));
      break;
    }

    case Property::FLOAT:
    {
      AddAnimatorConnector(AnimatorConnector<float>::New(object,
                                                         target.propertyIndex,
                                                         target.componentIndex,
                                                         AnimateByFloat(relativeValue.Get<float>()),
                                                         alpha,
                                                         period));
      break;
    }

    case Property::VECTOR2:
    {
      AddAnimatorConnector(AnimatorConnector<Vector2>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           AnimateByVector2(relativeValue.Get<Vector2>()),
                                                           alpha,
                                                           period));
      break;
    }

    case Property::VECTOR3:
    {
      AddAnimatorConnector(AnimatorConnector<Vector3>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           AnimateByVector3(relativeValue.Get<Vector3>()),
                                                           alpha,
                                                           period));
      break;
    }

    case Property::VECTOR4:
    {
      AddAnimatorConnector(AnimatorConnector<Vector4>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           AnimateByVector4(relativeValue.Get<Vector4>()),
                                                           alpha,
                                                           period));
      break;
    }

    case Property::ROTATION:
    {
      AngleAxis angleAxis = relativeValue.Get<AngleAxis>();

      AddAnimatorConnector(AnimatorConnector<Quaternion>::New(object,
                                                              target.propertyIndex,
                                                              target.componentIndex,
                                                              RotateByAngleAxis(angleAxis.angle, angleAxis.axis),
                                                              alpha,
                                                              period));
      break;
    }

    default:
    {
      DALI_ASSERT_DEBUG(false && "Property  not supported");
    }
  }

  AppendConnectorTargetValues({std::move(relativeValue), period, connectorIndex, Animation::BY});
}

void Animation::AnimateTo(Property& target, Property::Value destinationValue)
{
  AnimateTo(target, std::move(destinationValue), mDefaultAlpha, TimePeriod(mDurationSeconds));
}

void Animation::AnimateTo(Property& target, Property::Value destinationValue, AlphaFunction alpha)
{
  AnimateTo(target, std::move(destinationValue), alpha, TimePeriod(mDurationSeconds));
}

void Animation::AnimateTo(Property& target, Property::Value destinationValue, TimePeriod period)
{
  AnimateTo(target, std::move(destinationValue), mDefaultAlpha, period);
}

void Animation::AnimateTo(Property& target, Property::Value destinationValue, AlphaFunction alpha, TimePeriod period)
{
  Object&              object       = GetImplementation(target.object);
  const Property::Type propertyType = (target.componentIndex == Property::INVALID_COMPONENT_INDEX) ? object.GetPropertyType(target.propertyIndex) : Property::FLOAT;

  // validate and convert animation parameters, if component index is set then use float as checked type
  ValidateAndConvertParameters(propertyType, period, destinationValue);

  ExtendDuration(period);

  // keep the current count.
  auto connectorIndex = mConnectors.Count();

  switch(propertyType)
  {
    case Property::BOOLEAN:
    {
      AddAnimatorConnector(AnimatorConnector<bool>::New(object,
                                                        target.propertyIndex,
                                                        target.componentIndex,
                                                        AnimateToBoolean(destinationValue.Get<bool>()),
                                                        alpha,
                                                        period));
      break;
    }

    case Property::INTEGER:
    {
      AddAnimatorConnector(AnimatorConnector<int32_t>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           AnimateToInteger(destinationValue.Get<int32_t>()),
                                                           alpha,
                                                           period));
      break;
    }

    case Property::FLOAT:
    {
      AddAnimatorConnector(AnimatorConnector<float>::New(object,
                                                         target.propertyIndex,
                                                         target.componentIndex,
                                                         AnimateToFloat(destinationValue.Get<float>()),
                                                         alpha,
                                                         period));
      break;
    }

    case Property::VECTOR2:
    {
      AddAnimatorConnector(AnimatorConnector<Vector2>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           AnimateToVector2(destinationValue.Get<Vector2>()),
                                                           alpha,
                                                           period));
      break;
    }

    case Property::VECTOR3:
    {
      AddAnimatorConnector(AnimatorConnector<Vector3>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           AnimateToVector3(destinationValue.Get<Vector3>()),
                                                           alpha,
                                                           period));
      break;
    }

    case Property::VECTOR4:
    {
      AddAnimatorConnector(AnimatorConnector<Vector4>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           AnimateToVector4(destinationValue.Get<Vector4>()),
                                                           alpha,
                                                           period));
      break;
    }

    case Property::ROTATION:
    {
      AddAnimatorConnector(AnimatorConnector<Quaternion>::New(object,
                                                              target.propertyIndex,
                                                              target.componentIndex,
                                                              RotateToQuaternion(destinationValue.Get<Quaternion>()),
                                                              alpha,
                                                              period));
      break;
    }

    default:
    {
      DALI_ASSERT_DEBUG(false && "Property  not supported");
    }
  }

  AppendConnectorTargetValues({std::move(destinationValue), period, connectorIndex, Animation::TO});
}

void Animation::AnimateBetween(Property target, Dali::KeyFrames keyFrames)
{
  AnimateBetween(target, keyFrames, mDefaultAlpha, TimePeriod(mDurationSeconds), DEFAULT_INTERPOLATION);
}

void Animation::AnimateBetween(Property target, Dali::KeyFrames keyFrames, Interpolation interpolation)
{
  AnimateBetween(target, keyFrames, mDefaultAlpha, TimePeriod(mDurationSeconds), interpolation);
}

void Animation::AnimateBetween(Property target, Dali::KeyFrames keyFrames, TimePeriod period)
{
  AnimateBetween(target, keyFrames, mDefaultAlpha, period, DEFAULT_INTERPOLATION);
}

void Animation::AnimateBetween(Property target, Dali::KeyFrames keyFrames, TimePeriod period, Interpolation interpolation)
{
  AnimateBetween(target, keyFrames, mDefaultAlpha, period, interpolation);
}

void Animation::AnimateBetween(Property target, Dali::KeyFrames keyFrames, AlphaFunction alpha)
{
  AnimateBetween(target, keyFrames, alpha, TimePeriod(mDurationSeconds), DEFAULT_INTERPOLATION);
}

void Animation::AnimateBetween(Property target, Dali::KeyFrames keyFrames, AlphaFunction alpha, Interpolation interpolation)
{
  AnimateBetween(target, keyFrames, alpha, TimePeriod(mDurationSeconds), interpolation);
}

void Animation::AnimateBetween(Property target, Dali::KeyFrames keyFrames, AlphaFunction alpha, TimePeriod period)
{
  AnimateBetween(target, keyFrames, alpha, period, DEFAULT_INTERPOLATION);
}

void Animation::AnimateBetween(Property target, Dali::KeyFrames keyFrames, AlphaFunction alpha, TimePeriod period, Interpolation interpolation)
{
  Object&          object        = GetImplementation(target.object);
  const KeyFrames& keyFramesImpl = GetImplementation(keyFrames);

  const Property::Type propertyType = (target.componentIndex == Property::INVALID_COMPONENT_INDEX) ? object.GetPropertyType(target.propertyIndex) : Property::FLOAT;

  auto lastKeyFrameValue = keyFramesImpl.GetLastKeyFrameValue();
  ValidateAndConvertParameters(propertyType, period, lastKeyFrameValue);

  if(DALI_UNLIKELY(propertyType != keyFramesImpl.GetType()))
  {
    // Test for conversion valid, and convert keyframe values to matched property type.
    Dali::KeyFrames convertedKeyFrames = Dali::KeyFrames::New();
    auto            keyFrameCount      = keyFramesImpl.GetKeyFrameCount();
    for(auto frameIndex = 0u; frameIndex < keyFrameCount; ++frameIndex)
    {
      float           progress;
      Property::Value value;
      keyFramesImpl.GetKeyFrame(frameIndex, progress, value);
      DALI_ASSERT_ALWAYS(value.ConvertType(propertyType) && "Target types could not be convert to Property type");

      convertedKeyFrames.Add(progress, value);
    }

    // Retry to animation as the converted keyframes.
    AnimateBetween(target, convertedKeyFrames, alpha, period, interpolation);
    return;
  }

  ExtendDuration(period);

  AppendConnectorTargetValues({lastKeyFrameValue, period, mConnectors.Count(), BETWEEN});

  switch(propertyType)
  {
    case Dali::Property::BOOLEAN:
    {
      auto kf = GetSpecialization<const KeyFrameBoolean*>(keyFramesImpl);
      AddAnimatorConnector(AnimatorConnector<bool>::New(object,
                                                        target.propertyIndex,
                                                        target.componentIndex,
                                                        KeyFrameBooleanFunctor(*kf), // takes a copy of the keyframe
                                                        alpha,
                                                        period));
      break;
    }

    case Dali::Property::INTEGER:
    {
      auto kf = GetSpecialization<const KeyFrameInteger*>(keyFramesImpl);
      AddAnimatorConnector(AnimatorConnector<int32_t>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           KeyFrameIntegerFunctor(*kf, interpolation), // takes a copy of the keyframe
                                                           alpha,
                                                           period));
      break;
    }

    case Dali::Property::FLOAT:
    {
      auto kf = GetSpecialization<const KeyFrameNumber*>(keyFramesImpl);
      AddAnimatorConnector(AnimatorConnector<float>::New(object,
                                                         target.propertyIndex,
                                                         target.componentIndex,
                                                         KeyFrameNumberFunctor(*kf, interpolation), // takes a copy of the keyframe
                                                         alpha,
                                                         period));
      break;
    }

    case Dali::Property::VECTOR2:
    {
      auto kf = GetSpecialization<const KeyFrameVector2*>(keyFramesImpl);
      AddAnimatorConnector(AnimatorConnector<Vector2>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           KeyFrameVector2Functor(*kf, interpolation), // takes a copy of the keyframe
                                                           alpha,
                                                           period));
      break;
    }

    case Dali::Property::VECTOR3:
    {
      auto kf = GetSpecialization<const KeyFrameVector3*>(keyFramesImpl);
      AddAnimatorConnector(AnimatorConnector<Vector3>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           KeyFrameVector3Functor(*kf, interpolation), // takes a copy of the keyframe
                                                           alpha,
                                                           period));
      break;
    }

    case Dali::Property::VECTOR4:
    {
      auto kf = GetSpecialization<const KeyFrameVector4*>(keyFramesImpl);
      AddAnimatorConnector(AnimatorConnector<Vector4>::New(object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           KeyFrameVector4Functor(*kf, interpolation), // takes a copy of the keyframe
                                                           alpha,
                                                           period));
      break;
    }

    case Dali::Property::ROTATION:
    {
      auto kf = GetSpecialization<const KeyFrameQuaternion*>(keyFramesImpl);
      AddAnimatorConnector(AnimatorConnector<Quaternion>::New(object,
                                                              target.propertyIndex,
                                                              target.componentIndex,
                                                              KeyFrameQuaternionFunctor(*kf), // takes a copy of the keyframe
                                                              alpha,
                                                              period));
      break;
    }

    default:
    {
      DALI_ASSERT_DEBUG(false && "Property  not supported");
    }
  }
}

bool Animation::HasFinished()
{
  bool hasFinished(false);

  const int32_t playedCount(mAnimation->GetPlayedCount());

  DALI_LOG_ANIMATION_INFO("Animation[%u] HasFinished() count : %d -> %d, internal state : %s\n", mAnimationId, mNotificationCount, playedCount, GET_ANIMATION_INTERNAL_STATE_STRING(mState));

  if(playedCount > mNotificationCount)
  {
    switch(mState)
    {
      case Internal::Animation::InternalState::PLAYING:
      case Internal::Animation::InternalState::STOPPING:
      {
        mState      = Dali::Internal::Animation::STOPPED;
        hasFinished = true;
        break;
      }
      case Internal::Animation::InternalState::PLAYING_DURING_STOPPING:
      {
        mState      = Dali::Internal::Animation::PLAYING;
        hasFinished = true;
        break;
      }
      case Internal::Animation::InternalState::PAUSED_DURING_STOPPING:
      {
        mState      = Dali::Internal::Animation::PAUSED;
        hasFinished = true;
        break;
      }
      default:
      {
        break;
      }
    }

    if(hasFinished)
    {
      // Note that only one signal is emitted, if the animation has been played repeatedly
      mNotificationCount = playedCount;
    }
  }
  DALI_LOG_ANIMATION_INFO("Animation[%u] internal state : %s. Finished? %d\n", mAnimationId, GET_ANIMATION_INTERNAL_STATE_STRING(mState), hasFinished);

  return hasFinished;
}

Dali::Animation::AnimationSignalType& Animation::FinishedSignal()
{
  return mFinishedSignal;
}

Dali::Animation::AnimationSignalType& Animation::ProgressReachedSignal()
{
  return mProgressReachedSignal;
}

void Animation::EmitSignalFinish()
{
  DALI_LOG_ANIMATION_INFO("Animation[%u] EmitSignalFinish(), signal count : %zu, internal state : %s\n", mAnimationId, mFinishedSignal.GetConnectionCount(), GET_ANIMATION_INTERNAL_STATE_STRING(mState));
  if(!mFinishedSignal.Empty())
  {
    Dali::Animation handle(this);
    mFinishedSignal.Emit(handle);
  }
}

void Animation::EmitSignalProgressReached()
{
  if(!mProgressReachedSignal.Empty())
  {
    Dali::Animation handle(this);
    mProgressReachedSignal.Emit(handle);
  }
}

bool Animation::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  bool       connected(false);
  Animation* animation = static_cast<Animation*>(object); // TypeRegistry guarantees that this is the correct type.

  if(SIGNAL_FINISHED == signalName)
  {
    animation->FinishedSignal().Connect(tracker, functor);
    connected = true;
  }

  return connected;
}

void Animation::AddAnimatorConnector(AnimatorConnectorBase* connector)
{
  DALI_ASSERT_DEBUG(NULL != connector);

  connector->SetParent(*this);

  mConnectors.PushBack(connector);
}

void Animation::Animate(Actor& actor, const Path& path, const Vector3& forward)
{
  Animate(actor, path, forward, mDefaultAlpha, TimePeriod(mDurationSeconds));
}

void Animation::Animate(Actor& actor, const Path& path, const Vector3& forward, AlphaFunction alpha)
{
  Animate(actor, path, forward, alpha, TimePeriod(mDurationSeconds));
}

void Animation::Animate(Actor& actor, const Path& path, const Vector3& forward, TimePeriod period)
{
  Animate(actor, path, forward, mDefaultAlpha, period);
}

void Animation::Animate(Actor& actor, const Path& path, const Vector3& forward, AlphaFunction alpha, TimePeriod period)
{
  ExtendDuration(period);

  PathPtr pathCopy = Path::Clone(path);

  // Position animation
  AddAnimatorConnector(AnimatorConnector<Vector3>::New(actor,
                                                       Dali::Actor::Property::POSITION,
                                                       Property::INVALID_COMPONENT_INDEX,
                                                       PathPositionFunctor(pathCopy),
                                                       alpha,
                                                       period));

  // If forward is zero, PathRotationFunctor will always return the unit quaternion
  if(forward != Vector3::ZERO)
  {
    // Rotation animation
    AddAnimatorConnector(AnimatorConnector<Quaternion>::New(actor,
                                                            Dali::Actor::Property::ORIENTATION,
                                                            Property::INVALID_COMPONENT_INDEX,
                                                            PathRotationFunctor(pathCopy, forward),
                                                            alpha,
                                                            period));
  }
}

void Animation::Show(Actor& actor, float delaySeconds)
{
  ExtendDuration(TimePeriod(delaySeconds, 0));

  AddAnimatorConnector(AnimatorConnector<bool>::New(actor,
                                                    Dali::Actor::Property::VISIBLE,
                                                    Property::INVALID_COMPONENT_INDEX,
                                                    AnimateToBoolean(SHOW_VALUE),
                                                    mDefaultAlpha,
                                                    TimePeriod(delaySeconds, 0.0f /*immediate*/)));
}

void Animation::Hide(Actor& actor, float delaySeconds)
{
  ExtendDuration(TimePeriod(delaySeconds, 0));

  AddAnimatorConnector(AnimatorConnector<bool>::New(actor,
                                                    Dali::Actor::Property::VISIBLE,
                                                    Property::INVALID_COMPONENT_INDEX,
                                                    AnimateToBoolean(HIDE_VALUE),
                                                    mDefaultAlpha,
                                                    TimePeriod(delaySeconds, 0.0f /*immediate*/)));
}

bool Animation::DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  bool       done      = false;
  Animation* animation = dynamic_cast<Animation*>(object);

  if(animation)
  {
    std::string_view name(actionName);

    if(name == ACTION_PLAY)
    {
      if(Property::Value* value = attributes.Find("duration", Property::FLOAT))
      {
        animation->SetDuration(value->Get<float>());
      }

      animation->Play();
      done = true;
    }
    else if(name == ACTION_STOP)
    {
      animation->Stop();
      done = true;
    }
    else if(name == ACTION_PAUSE)
    {
      animation->Pause();
      done = true;
    }
  }

  return done;
}

void Animation::SetCurrentProgress(float progress)
{
  if(mAnimation && progress >= mPlayRange.x && progress <= mPlayRange.y)
  {
    // mAnimation is being used in a separate thread; queue a message to set the current progress
    SetCurrentProgressMessage(GetEventThreadServices(), *mAnimation, progress);
  }
}

float Animation::GetCurrentProgress()
{
  float progress = 0.f;
  if(mAnimation) // always exists in practice
  {
    progress = mAnimation->GetCurrentProgress();
  }

  return progress;
}

void Animation::ExtendDuration(const TimePeriod& timePeriod)
{
  float duration = timePeriod.delaySeconds + timePeriod.durationSeconds;

  if(duration > mDurationSeconds)
  {
    SetDuration(duration);
  }
}

void Animation::SetSpeedFactor(float factor)
{
  if(mAnimation)
  {
    mSpeedFactor = factor;
    SetSpeedFactorMessage(GetEventThreadServices(), *mAnimation, factor);
  }
}

float Animation::GetSpeedFactor() const
{
  return mSpeedFactor;
}

void Animation::SetPlayRange(const Vector2& range)
{
  // Make sure the range specified is between 0.0 and 1.0
  if(range.x >= 0.0f && range.x <= 1.0f && range.y >= 0.0f && range.y <= 1.0f)
  {
    Vector2 orderedRange(range);
    // If the range is not in order swap values
    if(range.x > range.y)
    {
      orderedRange = Vector2(range.y, range.x);
    }

    // Cache for public getters
    mPlayRange = orderedRange;

    // mAnimation is being used in a separate thread; queue a message to set play range
    SetPlayRangeMessage(GetEventThreadServices(), *mAnimation, orderedRange);
  }
}

Vector2 Animation::GetPlayRange() const
{
  return mPlayRange;
}

void Animation::SetBlendPoint(float blendPoint)
{
  if(blendPoint >= 0.0f && blendPoint <= 1.0f)
  {
    mBlendPoint = blendPoint;
    SetBlendPointMessage(GetEventThreadServices(), *mAnimation, mBlendPoint);
  }
  else
  {
    DALI_LOG_ERROR("Blend Point should be a value between 0 and 1.\n");
  }
}

float Animation::GetBlendPoint() const
{
  return mBlendPoint;
}

void Animation::SetLoopingMode(Dali::Animation::LoopingMode loopingMode)
{
  mAutoReverseEnabled = (loopingMode == Dali::Animation::LoopingMode::AUTO_REVERSE);

  // mAnimation is being used in a separate thread; queue a message to set play range
  SetLoopingModeMessage(GetEventThreadServices(), *mAnimation, mAutoReverseEnabled);
}

Dali::Animation::LoopingMode Animation::GetLoopingMode() const
{
  return mAutoReverseEnabled ? Dali::Animation::AUTO_REVERSE : Dali::Animation::RESTART;
}

uint32_t Animation::GetAnimationId() const
{
  return mAnimationId;
}

bool Animation::CompareConnectorEndTimes(const Animation::ConnectorTargetValues& lhs, const Animation::ConnectorTargetValues& rhs)
{
  return ((lhs.timePeriod.delaySeconds + lhs.timePeriod.durationSeconds) < (rhs.timePeriod.delaySeconds + rhs.timePeriod.durationSeconds));
}

void Animation::NotifyObjects(Animation::Notify notifyValueType)
{
  // If the animation is discarded, then we do not want to change the target values unless we want to force the current values
  if(mEndAction != EndAction::DISCARD || notifyValueType == Notify::FORCE_CURRENT_VALUE)
  {
    // Sort according to end time with earlier end times coming first, if the end time is the same, then the connectors are not moved
    // Only do this if we're using the target value
    if(mConnectorTargetValuesSortRequired && notifyValueType == Notify::USE_TARGET_VALUE)
    {
      std::stable_sort(mConnectorTargetValues.begin(), mConnectorTargetValues.end(), CompareConnectorEndTimes);

      // Now mConnectorTargetValues sorted. Reset flag.
      mConnectorTargetValuesSortRequired = false;
    }

    // Loop through all connector target values sorted by increasing end time
    ConnectorTargetValuesContainer::const_iterator       iter    = mConnectorTargetValues.begin();
    const ConnectorTargetValuesContainer::const_iterator endIter = mConnectorTargetValues.end();
    for(; iter != endIter; ++iter)
    {
      AnimatorConnectorBase* connector = mConnectors[iter->connectorIndex];

      Object* object = connector->GetObject();
      if(object && object->IsAnimationPossible())
      {
        const auto propertyIndex = connector->GetPropertyIndex();
        object->NotifyPropertyAnimation(
          *this,
          propertyIndex,
          (notifyValueType == Notify::USE_TARGET_VALUE) ? iter->targetValue : object->GetCurrentProperty(propertyIndex),
          (notifyValueType == Notify::USE_TARGET_VALUE) ? iter->animatorType : Animation::TO); // If we're setting the current value, then use TO as we want to set, not adjust, the current value
      }
    }
  }
}

void Animation::SendFinalProgressNotificationMessage()
{
  if(mProgressReachedMarker > 0.0f)
  {
    float progressMarkerSeconds = mDurationSeconds * mProgressReachedMarker;
    SetProgressNotificationMessage(GetEventThreadServices(), *mAnimation, progressMarkerSeconds);
  }
}

void Animation::AppendConnectorTargetValues(ConnectorTargetValues&& connectorTargetValues)
{
  // Check whether we need to sort mConnectorTargetValues or not.
  // Sort will be required only if new item is smaller than last value of container.
  if(!mConnectorTargetValuesSortRequired && !mConnectorTargetValues.empty())
  {
    if(CompareConnectorEndTimes(connectorTargetValues, mConnectorTargetValues.back()))
    {
      mConnectorTargetValuesSortRequired = true;
    }
  }

  // Store data to later notify the object that its property is being animated
  mConnectorTargetValues.push_back(std::move(connectorTargetValues));

  if(DALI_UNLIKELY(mConnectorTargetValues.size() % WARNING_PRINT_THRESHOLD == 0))
  {
    DALI_LOG_ANIMATION_INFO("Animation[%u] Connect %zu Animators! Please check you might append too much items.\n", mAnimationId, mConnectorTargetValues.size());
  }
}

} // namespace Internal

} // namespace Dali
