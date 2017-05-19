#ifndef __DALI_INTERNAL_ANIMATION_H__
#define __DALI_INTERNAL_ANIMATION_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/event/animation/animator-connector-base.h>
#include <dali/internal/event/animation/key-frames-impl.h>
#include <dali/internal/event/animation/path-impl.h>
#include <dali/internal/event/common/object-impl.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class Animation;
class UpdateManager;
}

class Actor;
class Animation;
class AnimationPlaylist;
class Object;

typedef IntrusivePtr<Animation> AnimationPtr;
typedef std::vector<AnimationPtr> AnimationContainer;

typedef AnimationContainer::iterator AnimationIter;
typedef AnimationContainer::const_iterator AnimationConstIter;

/**
 * Animation is a proxy for a SceneGraph::Animation object.
 * The UpdateManager owns the Animation object, but the lifetime of the animation is
 * indirectly controlled by the Animation.
 */
class Animation : public BaseObject
{
public:

  typedef Dali::Animation::EndAction EndAction;
  typedef Dali::Animation::Interpolation Interpolation;

  /**
   * Create a new Animation object.
   * @param[in] durationSeconds The duration of the animation.
   * @return A smart-pointer to the newly allocated Animation.
   */
  static AnimationPtr New(float durationSeconds);

  /**
   * @copydoc Dali::Animation::SetDuration()
   */
  void SetDuration(float seconds);

  /**
   * @copydoc Dali::Animation::GetDuration()
   */
  float GetDuration() const;

  /**
   * @copydoc Dali::Animation::SetLooping()
   */
  void SetLooping(bool on);

  /**
   * @copydoc Dali::Animation::SetLoopCount()
   */
  void SetLoopCount(int count);

  /**
   * @copydoc Dali::Animation::GetLoopCount()
   */
  int GetLoopCount();

  /**
   * @copydoc Dali::Animation::GetCurrentLoop()
   */
  int GetCurrentLoop();

  /**
   * @copydoc Dali::Animation::IsLooping()
   */
  bool IsLooping() const;

  /**
   * @copydoc Dali::Animation::SetEndAction()
   */
  void SetEndAction(EndAction action);

  /**
   * @copydoc Dali::Animation::GetEndAction()
   */
  EndAction GetEndAction() const;

  /**
   * @copydoc Dali::Animation::SetDisconnectAction()
   */
  void SetDisconnectAction(EndAction action);

  /**
   * @copydoc Dali::Animation::GetDisconnectAction()
   */
  EndAction GetDisconnectAction() const;

  /**
   * @copydoc Dali::Animation::SetDefaultAlphaFunction()
   */
  void SetDefaultAlphaFunction(AlphaFunction alpha)
  {
    mDefaultAlpha = alpha;
  }

  /**
   * @copydoc Dali::Animation::GetDefaultAlphaFunction()
   */
  AlphaFunction GetDefaultAlphaFunction() const
  {
    return mDefaultAlpha;
  }

  /**
   * @copydoc Dali::Animation::Play()
   */
  void Play();

  /**
   * @copydoc Dali::Animation::PlayFrom()
   */
  void PlayFrom( float progress );

  /**
   * @copydoc Dali::Animation::Pause()
   */
  void Pause();

  /**
   * @copydoc Dali::Animation::GetState()
   */
  Dali::Animation::State GetState() const;

  /**
   * @copydoc Dali::Animation::Stop()
   */
  void Stop();

  /**
   * @copydoc Dali::Animation::Clear()
   */
  void Clear();

  /**
   * Query whether a Finished signal should be emitted for this animation.
   * This should only be called by NotificationManager, before signals are emitted.
   * @post HasFinished() will return false on subsequent calls, until the animation is replayed to completion.
   */
  bool HasFinished();

  /**
   * @copydoc Dali::Animation::FinishedSignal()
   */
  Dali::Animation::AnimationSignalType& FinishedSignal();

  /**
   * Emit the Finished signal
   */
  void EmitSignalFinish();

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
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action was done
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes);

  /**
   * @copydoc Dali::Animation::AnimateBy(Property target, Property::Value relativeValue)
   */
  void AnimateBy(Property& target, Property::Value& relativeValue);

  /**
   * @copydoc Dali::Animation::AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha)
   */
  void AnimateBy(Property& target, Property::Value& relativeValue, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::AnimateBy(Property target, Property::Value relativeValue, TimePeriod period)
   */
  void AnimateBy(Property& target, Property::Value& relativeValue, TimePeriod period);

  /**
   * @copydoc Dali::Animation::AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha, TimePeriod period)
   */
  void AnimateBy(Property& target, Property::Value& relativeValue, AlphaFunction alpha, TimePeriod period);

  /**
   * @copydoc Dali::Animation::AnimateTo(Property target, Property::Value destinationValue)
   */
  void AnimateTo(Property& target, Property::Value& destinationValue);

  /**
   * @copydoc Dali::Animation::AnimateTo(Property target, Property::Value destinationValue, AlphaFunction alpha)
   */
  void AnimateTo(Property& target, Property::Value& destinationValue, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::AnimateTo(Property target, Property::Value destinationValue, TimePeriod period)
   */
  void AnimateTo(Property& target, Property::Value& destinationValue, TimePeriod period);

  /**
   * @copydoc Dali::Animation::AnimateTo(Property target, Property::Value destinationValue, AlphaFunction alpha, TimePeriod period)
   */
  void AnimateTo(Property& target, Property::Value& destinationValue, AlphaFunction alpha, TimePeriod period);

  /**
   * Animate a property to a destination value.
   * @param [in] targetObject The target object to animate.
   * @param [in] targetPropertyIndex The index of the target property.
   * @param [in] componentIndex Index to a sub component of a property, for use with Vector2, Vector3 and Vector4
   * @param [in] destinationValue The destination value.
   * @param [in] alpha The alpha function to apply.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateTo(Object& targetObject, Property::Index targetPropertyIndex, int componentIndex, Property::Value& destinationValue, AlphaFunction alpha, TimePeriod period);

  /**
   * @copydoc Dali::Animation::AnimateBetween(Property target, KeyFrames& keyFrames)
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames);

  /**
   * @copydoc Dali::Animation::AnimateBetween(Property target, KeyFrames& keyFrames, Interpolation interpolation)
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames, Interpolation interpolation );

  /**
   * @copydoc Dali::Animation::AnimateBetween(Property target, KeyFrames& keyFrames, TimePeriod period)
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames, TimePeriod period);

  /**
   * @copydoc Dali::Animation::AnimateBetween(Property target, KeyFrames& keyFrames, TimePeriod period, Interpolation interpolation)
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames, TimePeriod period, Interpolation interpolation);

  /**
   * @copydoc Dali::Animation::AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha)
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, Interpolation interpolation)
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha, Interpolation interpolation);

  /**
   * @copydoc Dali::Animation::AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period)
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period);

  /**
   * @copydoc Dali::Animation::AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period, Interpolation interpolation )
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period, Interpolation interpolation );

  // Actor-specific convenience functions

  /**
   * @copydoc Dali::Animation::Animate( Actor actor, Path path, const Vector3& forward )
   */
  void Animate( Actor& actor, const Path& path, const Vector3& forward );

  /**
   * @copydoc Dali::Animation::Animate( Actor actor, Path path, const Vector3& forward, AlphaFunction alpha )
   */
  void Animate( Actor& actor, const Path& path, const Vector3& forward, AlphaFunction alpha );

  /**
   * @copydoc Dali::Animation::Animate( Actor actor, Path path, const Vector3& forward, TimePeriod period )
   */
  void Animate( Actor& actor, const Path& path, const Vector3& forward, TimePeriod period );

  /**
   * @copydoc Dali::Animation::Animate( Actor actor, Path path, const Vector3& forward, AlphaFunction alpha, TimePeriod period)
   */
  void Animate( Actor& actor, const Path& path, const Vector3& forward, AlphaFunction alpha, TimePeriod period);

  /**
   * @copydoc Dali::Animation::Show()
   */
  void Show(Actor& actor, float delaySeconds);

  /**
   * @copydoc Dali::Animation::Hide()
   */
  void Hide(Actor& actor, float delaySeconds);

  /*
   * @copydoc Dali::Animation::SetCurrentProgress()
   */
  void SetCurrentProgress(float progress);

  /*
   * @copydoc Dali::Animation::GetCurrentProgress()
   */
  float GetCurrentProgress();

  /*
   * @copydoc Dali::Animation::SetSpeedFactor()
   */
  void SetSpeedFactor( float factor );

  /*
   * @copydoc Dali::Animation::GetSpeedFactor()
   */
  float GetSpeedFactor() const;

  /*
   * @copydoc Dali::Animation::SetPlayRange()
   */
  void SetPlayRange( const Vector2& range );

  /*
   * @copydoc Dali::Animation::GetPlayRange
   */
  Vector2 GetPlayRange() const;

public: // For connecting animators to animations

  /**
   * Add an animator connector.
   * @param[in] connector The animator connector.
   */
  void AddAnimatorConnector( AnimatorConnectorBase* connector );

  /**
   * Retrieve the SceneGraph::Animation object.
   * @return The animation.
   */
  const SceneGraph::Animation* GetSceneObject()
  {
    return mAnimation;
  }

  /**
   * Retrieve the event thread services object
   * @return The interface for sending messages to the scene graph
   */
  EventThreadServices& GetEventThreadServices()
  {
    return mEventThreadServices;
  }

protected:

  /**
   * Construct a new Animation.
   * @param[in] eventThreadServices The interface for sending messages to the scene graph
   * @param[in] playlist The list of currently playing animations.
   * @param[in] durationSeconds The duration of the animation in seconds.
   * @param[in] endAction The action to perform when the animation ends.
   * @param[in] disconnectAction The action to perform when the property owner of an animator is disconnected.
   * @param[in] defaultAlpha The default alpha function to apply to animators.
   */
  Animation( EventThreadServices& eventThreadServices,
             AnimationPlaylist& playlist,
             float durationSeconds,
             EndAction endAction,
             EndAction disconnectAction,
             AlphaFunction defaultAlpha);

  /**
   * Second-phase constructor.
   */
  void Initialize();

  /**
   * Helper to create a scene-graph animation
   */
  void CreateSceneObject();

  /**
   * Helper to create a scene-graph animation
   */
  void DestroySceneObject();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Animation();

private:

  /**
   * Extends the duration when an animator is added with TimePeriod that exceeds current duration.
   * @param[in] timePeriod The time period for an animator.
   */
  void ExtendDuration( const TimePeriod& timePeriod );

  // Undefined
  Animation(const Animation&);

  // Undefined
  Animation& operator=(const Animation& rhs);

private:

  struct ConnectorTargetValues
  {
    ConnectorTargetValues()
    : targetValue(),
      timePeriod( 0.0f ),
      connectorIndex( 0 ),
      propertyChangeType( Object::PropertyChange::SET )
    {
    }

    Property::Value targetValue;
    TimePeriod timePeriod;
    unsigned int connectorIndex;
    Object::PropertyChange::Type propertyChangeType;
  };

private:

  /**
   * Compares the end times of the animators returning true if lhs end time is less than rhs end time.
   * @param[in] lhs The first comparator
   * @param[in] rhs The second comparator
   * @return True if end time of lhs is less, false otherwise.
   */
  static bool CompareConnectorEndTimes( const ConnectorTargetValues& lhs, const ConnectorTargetValues& rhs );

private:

  const SceneGraph::Animation* mAnimation;

  EventThreadServices& mEventThreadServices;
  AnimationPlaylist& mPlaylist;

  Dali::Animation::AnimationSignalType mFinishedSignal;

  AnimatorConnectorContainer mConnectors; ///< Owned by the Animation

  typedef std::vector< ConnectorTargetValues > ConnectorTargetValuesContainer;
  ConnectorTargetValuesContainer mConnectorTargetValues; //< Used to store animating property target value information

  Vector2 mPlayRange;

  float mDurationSeconds;
  float mSpeedFactor;
  int mNotificationCount; ///< Keep track of how many Finished signals have been emitted.
  int mLoopCount;
  int mCurrentLoop;
  EndAction mEndAction;
  EndAction mDisconnectAction;
  AlphaFunction mDefaultAlpha;
  Dali::Animation::State mState;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Animation& GetImplementation(Dali::Animation& animation)
{
  DALI_ASSERT_ALWAYS( animation && "Animation handle is empty" );

  BaseObject& handle = animation.GetBaseObject();

  return static_cast<Internal::Animation&>(handle);
}

inline const Internal::Animation& GetImplementation(const Dali::Animation& animation)
{
  DALI_ASSERT_ALWAYS( animation && "Animation handle is empty" );

  const BaseObject& handle = animation.GetBaseObject();

  return static_cast<const Internal::Animation&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_ANIMATION_H__
