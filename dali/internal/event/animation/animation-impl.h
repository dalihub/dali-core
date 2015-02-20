#ifndef __DALI_INTERNAL_ANIMATION_H__
#define __DALI_INTERNAL_ANIMATION_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/event/animation/animator-connector-base.h>
#include <dali/internal/event/animation/key-frames-impl.h>
#include <dali/internal/event/animation/path-impl.h>

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
class ShaderEffect;

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

  typedef void (*FinishedCallback)(Object* object);

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
  void SetLooping(bool looping);

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
  static bool DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes);

  /**
   * This callback is intended for internal use only, to avoid the overhead of using a signal.
   * @param[in] callback The callback function to connect.
   * @param[in] object The internal object requesting the callback, or NULL.
   */
  void SetFinishedCallback( FinishedCallback callback, Object* object );

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
   * @copydoc Dali::Animation::MoveBy(Actor actor, float x, float y, float z)
   */
  void MoveBy(Actor& actor, float x, float y, float z);

  /**
   * @copydoc Dali::Animation::MoveBy(Actor actor, Vector3 displacement, AlphaFunction alpha)
   */
  void MoveBy(Actor& actor, const Vector3& translation, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::MoveBy(Actor actor, Vector3 displacement, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void MoveBy(Actor& actor, const Vector3& translation, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::MoveTo(Actor actor, float x, float y, float z)
   */
  void MoveTo(Actor& actor, float x, float y, float z);

  /**
   * @copydoc Dali::Animation::MoveTo(Actor actor, Vector3 position, AlphaFunction alpha)
   */
  void MoveTo(Actor& actor, const Vector3& translation, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::MoveTo(Actor actor, Vector3 position, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void MoveTo(Actor& actor, const Vector3& translation, AlphaFunction alpha,  float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::RotateBy(Actor actor, Radian angle, Vector3 axis)
   */
  void RotateBy(Actor& actor, Radian angle, const Vector3& axis);

  /**
   * @copydoc Dali::Animation::RotateBy(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha)()
   */
  void RotateBy(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::RotateBy(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void RotateBy(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::RotateTo(Actor actor, Radian angle, Vector3 axis)
   */
  void RotateTo(Actor& actor, Radian angle, const Vector3& axis);

  /**
   * @copydoc Dali::Animation::RotateTo(Actor actor, Quaternion orientation)
   */
  void RotateTo(Actor& actor, const Quaternion& orientation);

  /**
   * @copydoc Dali::Animation::RotateTo(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha)
   */
  void RotateTo(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::RotateTo(Actor actor, Quaternion orientation, AlphaFunction alpha)
   */
  void RotateTo(Actor& actor, const Quaternion& orientation, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::RotateTo(Actor actor, Quaternion orientation, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void RotateTo(Actor& actor, const Quaternion& orientation, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::RotateTo(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha, float delaySeconds, float durationSeconds)()
   */
  void RotateTo(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::ScaleBy(Actor actor, float x, float y, float z)()
   */
  void ScaleBy(Actor& actor, float x, float y, float z);

  /**
   * @copydoc Dali::Animation::ScaleBy(Actor actor, Vector3 scale, AlphaFunction alpha)
   */
  void ScaleBy(Actor& actor, const Vector3& scale, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::ScaleBy(Actor actor, Vector3 scale, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void ScaleBy(Actor& actor, const Vector3& scale, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::ScaleTo(Actor actor, float x, float y, float z)
   */
  void ScaleTo(Actor& actor, float x, float y, float z);

  /**
   * @copydoc Dali::Animation::ScaleTo(Actor actor, Vector3 scale, AlphaFunction alpha)
   */
  void ScaleTo(Actor& actor, const Vector3& scale, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::ScaleTo(Actor actor, Vector3 scale, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void ScaleTo(Actor& actor, const Vector3& scale, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::Show()
   */
  void Show(Actor& actor, float delaySeconds);

  /**
   * @copydoc Dali::Animation::Hide()
   */
  void Hide(Actor& actor, float delaySeconds);

  /**
   * @copydoc Dali::Animation::OpacityBy(Actor actor, float opacity)
   */
  void OpacityBy(Actor& actor, float opacity);

  /**
   * @copydoc Dali::Animation::OpacityBy(Actor actor, float opacity, AlphaFunction alpha)
   */
  void OpacityBy(Actor& actor, float opacity, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::OpacityBy(Actor actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds)()
   */
  void OpacityBy(Actor& actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::OpacityTo(Actor actor, float opacity)
   */
  void OpacityTo(Actor& actor, float opacity);

  /**
   * @copydoc Dali::Animation::OpacityTo(Actor actor, float opacity, AlphaFunction alpha)
   */
  void OpacityTo(Actor& actor, float opacity, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::OpacityTo(Actor actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void OpacityTo(Actor& actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::ColorBy(Actor actor, Vector4 color)
   */
  void ColorBy(Actor& actor, const Vector4& color);

  /**
   * @copydoc Dali::Animation::ColorBy(Actor actor, Vector4 color, AlphaFunction alpha)
   */
  void ColorBy(Actor& actor, const Vector4& color, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::ColorBy(Actor actor, Vector4 color, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void ColorBy(Actor& actor, const Vector4& color, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::ColorTo(Actor actor, Vector4 color)
   */
  void ColorTo(Actor& actor, const Vector4& color);

  /**
   * @copydoc Dali::Animation::ColorTo(Actor actor, Vector4 color, AlphaFunction alpha)
   */
  void ColorTo(Actor& actor, const Vector4& color, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::ColorTo(Actor actor, Vector4 color, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void ColorTo(Actor& actor, const Vector4& color, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::Resize(Actor actor, float width, float height)
   */
  void Resize(Actor& actor, float width, float height);

  /**
   * @copydoc Dali::Animation::Resize(Actor actor, float width, float height, AlphaFunction alpha)
   */
  void Resize(Actor& actor, float width, float height, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::Resize(Actor actor, float width, float height, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void Resize(Actor& actor, float width, float height, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * @copydoc Dali::Animation::Resize(Actor actor, Vector3 size)
   */
  void Resize(Actor& actor, const Vector3& size);

  /**
   * @copydoc Dali::Animation::Resize(Actor actor, Vector3 size, AlphaFunction alpha)
   */
  void Resize(Actor& actor, const Vector3& size, AlphaFunction alpha);

  /**
   * @copydoc Dali::Animation::Resize(Actor actor, Vector3 size, AlphaFunction alpha, float delaySeconds, float durationSeconds)
   */
  void Resize(Actor& actor, const Vector3& size, AlphaFunction alpha, float delaySeconds, float durationSeconds);

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
   * Retrieve the UpdateManager associated with this animation.
   * @return The UpdateManager.
   */
  SceneGraph::UpdateManager& GetUpdateManager()
  {
    return mUpdateManager;
  }

protected:

  /**
   * Construct a new Animation.
   * @param[in] updateManager The UpdateManager associated with this animation.
   * @param[in] playlist The list of currently playing animations.
   * @param[in] durationSeconds The duration of the animation in seconds.
   * @param[in] endAction The action to perform when the animation ends.
   * @param[in] disconnectAction The action to perform when the property owner of an animator is disconnected.
   * @param[in] defaultAlpha The default alpha function to apply to animators.
   */
  Animation( SceneGraph::UpdateManager& updateManager,
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

  SceneGraph::UpdateManager& mUpdateManager;
  AnimationPlaylist& mPlaylist;

  const SceneGraph::Animation* mAnimation;

  int mNotificationCount; ///< Keep track of how many Finished signals have been emitted.

  Dali::Animation::AnimationSignalType mFinishedSignal;

  FinishedCallback mFinishedCallback;
  Object* mFinishedCallbackObject;

  AnimatorConnectorContainer mConnectors; ///< Owned by the Animation

  // Cached for public getters
  float mDurationSeconds;
  float mSpeedFactor;
  bool mIsLooping;
  Vector2 mPlayRange;
  EndAction mEndAction;
  EndAction mDisconnectAction;
  AlphaFunction mDefaultAlpha;

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
