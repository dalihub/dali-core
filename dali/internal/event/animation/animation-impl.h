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
class ProxyObject;
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

  typedef Dali::Animation::AnyFunction AnyFunction;
  typedef Dali::Animation::EndAction EndAction;

  typedef void (*FinishedCallback)(Object* object);

  /**
   * Create a new Animation object.
   * @param[in] durationSeconds The duration of the animation.
   * @return A smart-pointer to the newly allocated Animation.
   */
  static AnimationPtr New(float durationSeconds);

  /**
   * Create a new Animation object.
   * @param[in] durationSeconds The duration of the animation.
   * @param[in] endAction The action to perform when the animation ends.
   * @param[in] destroyAction The action to perform when the animation ends.
   * @return A smart-pointer to the newly allocated Animation.
   */
  static AnimationPtr New(float durationSeconds, EndAction endAction, EndAction destroyAction);

  /**
   * Create a new Animation object.
   * @param[in] durationSeconds The duration of the animation.
   * @param[in] endAction The action to perform when the animation ends.
   * @param[in] destroyAction The action to perform when the animation ends.
   * @param[in] alpha The default alpha function to apply to animators.
   * @return A smart-pointer to the newly allocated Animation.
   */
  static AnimationPtr New(float durationSeconds, EndAction endAction, EndAction destroyAction, AlphaFunction alpha);

  /**
   * Set the duration of an animation.
   * @pre durationSeconds must be greater than zero.
   * @param[in] seconds The duration in seconds.
   */
  void SetDuration(float seconds);

  /**
   * Retrieve the duration of an animation.
   * @return The duration in seconds.
   */
  float GetDuration() const;

  /**
   * Set whether the animation will loop.
   * @param[in] looping True if the animation will loop.
   */
  void SetLooping(bool looping);

  /**
   * Query whether the animation will loop.
   * @return True if the animation will loop.
   */
  bool IsLooping() const;

  /**
   * Set the end action of the animation.
   * @param[in] action The end action.
   */
  void SetEndAction(EndAction action);

  /**
   * Returns the end action of the animation.
   */
  EndAction GetEndAction() const;

  /**
   * Set the destroy action of the animation.
   * @param[in] action The destroy action.
   */
  void SetDestroyAction(EndAction action);

  /**
   * Returns the destroy action of the animation.
   */
  EndAction GetDestroyAction() const;

  /**
   * Set the default alpha function for an animation.
   * This is applied to individual property animations, if no further alpha functions are supplied.
   * @param[in] alpha The default alpha function.
   */
  void SetDefaultAlphaFunction(AlphaFunction alpha)
  {
    mDefaultAlpha = alpha;
  }

  /**
   * Retrieve the default alpha function for an animation.
   * @return The default alpha function.
   */
  AlphaFunction GetDefaultAlphaFunction() const
  {
    return mDefaultAlpha;
  }

  /**
   * (Re)start the animation.
   */
  void Play();

  /**
   * (Re)start the animation from a given point.
   * @param[in] progress The progress between [0,1] from where the animation should start
   */
  void PlayFrom( float progress );

  /**
   * Pause the animation.
   */
  void Pause();

  /**
   * Stop the animation.
   */
  void Stop();

  /**
   * Clear the animation.
   * This disconnects any objects that were being animated, effectively stopping the animation.
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
  Dali::Animation::AnimationSignalV2& FinishedSignal();

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
  void AnimateTo(ProxyObject& targetObject, Property::Index targetPropertyIndex, int componentIndex, Property::Value& destinationValue, AlphaFunction alpha, TimePeriod period);


  /**
   * Animate a property between the keyframe time / value pairs.
   * It uses the current duration of the animation to run the key
   * frame animator. Note, If the animation duration is subsequently
   * changed, this does not change to match.
   *
   * @param[in] target The target object + property to animate
   * @param[in] keyFrames The set of time / value pairs between which to animate.
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames);

  /**
   * Animate a property between the keyframe time / value pairs.
   * @param[in] target The target object + property to animate
   * @param[in] keyFrames The set of time / value pairs between which to animate.
   * @param[in] period The effect will occur duing this time period.
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames, TimePeriod period);

  /**
   * Animate a property between the keyframe time / value pairs.
   * @param[in] target The target object + property to animate
   * @param[in] keyFrames The set of time / value pairs between which to animate.
   * @param[in] alpha The alpha function to apply to the overall progress.
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha);

  /**
   * Animate a property between the keyframe time / value pairs.
   * @param[in] target The target object + property to animate
   * @param[in] keyFrames The set of time / value pairs between which to animate.
   * @param[in] alpha The alpha function to apply to the overall progress.
   * @param[in] period The effect will occur duing this time period.
   */
  void AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period);

  /**
   * @copydoc Dali::Animation::Animate( Property target, Property::Type targetType, AnyFunction func )
   */
  void Animate( Property& target, Property::Type targetType, AnyFunction& func );

  /**
   * @copydoc Dali::Animation::Animate(Property target, Property::Type targetType, AnyFunction func, AlphaFunction alpha )
   */
  void Animate( Property& target, Property::Type targetType, AnyFunction& func, AlphaFunction& alpha );

  /**
   * @copydoc Dali::Animation::Animate(Property target, Property::Type targetType, AnyFunction func, TimePeriod period)
   */
  void Animate( Property& target, Property::Type targetType, AnyFunction& func, TimePeriod period );

  /**
   * @copydoc Dali::Animation::Animate(Property target, Property::Type targetType, AnyFunction func, AlphaFunction alpha, TimePeriod period)
   */
  void Animate( Property& target, Property::Type targetType, AnyFunction& func, AlphaFunction& alpha, TimePeriod period );

  // Action-specific convenience functions

  /**
   * Translate an actor.
   * The default alpha function will be used.
   * The translation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x Translation in the X-direction.
   * @param [in] y Translation in the Y-direction.
   * @param [in] z Translation in the Z-direction.
   */
  void MoveBy(Actor& actor, float x, float y, float z);

  /**
   * Translate an actor. This overload allows the alpha function to be customized.
   * The translation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] translation The translation.
   * @param [in] alpha The alpha function to apply.
   */
  void MoveBy(Actor& actor, const Vector3& translation, AlphaFunction alpha);

  /**
   * Translate an actor. This overload allows the translation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be greater than zero.
   * @param [in] actor The actor to animate.
   * @param [in] translation The translation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void MoveBy(Actor& actor, const Vector3& translation, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Translate an actor to a target position.
   * The default alpha function will be used.
   * The translation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x Translation in the X-direction.
   * @param [in] y Translation in the Y-direction.
   * @param [in] z Translation in the Z-direction.
   */
  void MoveTo(Actor& actor, float x, float y, float z);

  /**
   * Translate an actor to a target position. This overload allows the alpha function to be customized.
   * The translation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] translation The translation.
   * @param [in] alpha The alpha function to apply.
   */
  void MoveTo(Actor& actor, const Vector3& translation, AlphaFunction alpha);

  /**
   * Translate an actor to a target position. This overload allows the translation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be greater than zero.
   * @param [in] actor The actor to animate.
   * @param [in] translation The translation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void MoveTo(Actor& actor, const Vector3& translation, AlphaFunction alpha,  float delaySeconds, float durationSeconds);

  /**
   * Translate an actor using a custom function.
   * The animatorFunc will be called from the rendering thread; it should return quickly, to avoid performance degredation.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] func The function to call during the animation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void Move(Actor& actor, AnimatorFunctionVector3 func, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Rotate an actor around an arbitrary axis.
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in radians.
   * @param [in] axis The axis of rotation.
   */
  void RotateBy(Actor& actor, Radian angle, const Vector3& axis);

  /**
   * Rotate an actor around an arbitrary axis. This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in radians.
   * @param [in] axis The axis of rotation.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateBy(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha);

  /**
   * Rotate an actor around an arbitrary axis. This overload allows the rotation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in radians.
   * @param [in] axis The axis of rotation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the rotation.
   */
  void RotateBy(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Rotate an actor to a target orientation.
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in radians.
   * @param [in] axis The target axis of rotation.
   */
  void RotateTo(Actor& actor, Radian angle, const Vector3& axis);

  /**
   * Rotate an actor to a target orientation.
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] orientation The target orientation.
   */
  void RotateTo(Actor& actor, const Quaternion& orientation);

  /**
   * Rotate an actor to a target orientation. This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in radians.
   * @param [in] axis The target axis of rotation.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateTo(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha);

  /**
   * Rotate an actor to a target orientation. This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] orientation The target orientation.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateTo(Actor& actor, const Quaternion& orientation, AlphaFunction alpha);

  /**
   * Rotate an actor to a target orientation. This overload allows the rotation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be greater than zero.
   * @param [in] actor The actor to animate.
   * @param [in] orientation The target orientation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the rotation.
   */
  void RotateTo(Actor& actor, const Quaternion& orientation, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Rotate an actor to a target orientation. This overload allows the rotation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be greater than zero.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in radians.
   * @param [in] axis The target axis of rotation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the rotation.
   */
  void RotateTo(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Rotate an actor using a custom function.
   * The animatorFunc will be called from the rendering thread; it should return quickly, to avoid performance degredation.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] func The function to call during the animation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void Rotate(Actor& actor, AnimatorFunctionQuaternion func, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Scale an actor.
   * The default alpha function will be used.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x Scale factor in the X-direction.
   * @param [in] y Scale factor in the Y-direction.
   * @param [in] z Scale factor in the Z-direction.
   */
  void ScaleBy(Actor& actor, float x, float y, float z);

  /**
   * Scale an actor. This overload allows the alpha function to be customized.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] scale The scale factor.
   * @param [in] alpha The alpha function to apply.
   */
  void ScaleBy(Actor& actor, const Vector3& scale, AlphaFunction alpha);

  /**
   * Scale an actor. This overload allows the translation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be greater than zero.
   * @param [in] actor The actor to animate.
   * @param [in] scale The scale factor.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void ScaleBy(Actor& actor, const Vector3& scale, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Scale an actor to a target scale factor.
   * The default alpha function will be used.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x Target scale-factor in the X-direction.
   * @param [in] y Target scale-factor in the Y-direction.
   * @param [in] z Target scale-factor in the Z-direction.
   */
  void ScaleTo(Actor& actor, float x, float y, float z);

  /**
   * Scale an actor to a target scale factor. This overload allows the alpha function to be customized.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] scale The target scale factor.
   * @param [in] alpha The alpha function to apply.
   */
  void ScaleTo(Actor& actor, const Vector3& scale, AlphaFunction alpha);

  /**
   * Scale an actor to a target scale factor. This overload allows the translation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be greater than zero.
   * @param [in] actor The actor to animate.
   * @param [in] scale The target scale factor.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void ScaleTo(Actor& actor, const Vector3& scale, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Show an actor during the animation.
   * @param [in] actor The actor to animate.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   */
  void Show(Actor& actor, float delaySeconds);

  /**
   * Hide an actor during the animation.
   * @param [in] actor The actor to animate.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   */
  void Hide(Actor& actor, float delaySeconds);

  /**
   * Animate the opacity of an actor.
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The relative change in opacity.
   */
  void OpacityBy(Actor& actor, float opacity);

  /**
   * Animate the opacity of an actor. This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The relative change in opacity.
   * @param [in] alpha The alpha function to apply.
   */
  void OpacityBy(Actor& actor, float opacity, AlphaFunction alpha);

  /**
   * Animate the opacity of an actor. This overload allows the translation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be greater than zero.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The relative change in opacity.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void OpacityBy(Actor& actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Animate an actor to a target opacity.
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The target opacity.
   */
  void OpacityTo(Actor& actor, float opacity);

  /**
   * Animate an actor to a target opacity. This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The target opacity.
   * @param [in] alpha The alpha function to apply.
   */
  void OpacityTo(Actor& actor, float opacity, AlphaFunction alpha);

  /**
   * Animate an actor to a target opacity. This overload allows the translation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be greater than zero.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The target opacity.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void OpacityTo(Actor& actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Animate the color of an actor.
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The relative change in color.
   */
  void ColorBy(Actor& actor, const Vector4& color);

  /**
   * Animate the color of an actor. This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The relative change in color.
   * @param [in] alpha The alpha function to apply.
   */
  void ColorBy(Actor& actor, const Vector4& color, AlphaFunction alpha);

  /**
   * Animate the color of an actor. This overload allows the translation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] color The relative change in color.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void ColorBy(Actor& actor, const Vector4& color, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Animate an actor to a target color.
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The target color.
   */
  void ColorTo(Actor& actor, const Vector4& color);

  /**
   * Animate an actor to a target color. This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The target color.
   * @param [in] alpha The alpha function to apply.
   */
  void ColorTo(Actor& actor, const Vector4& color, AlphaFunction alpha);

  /**
   * Animate an actor to a target color. This overload allows the translation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] color The target color.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void ColorTo(Actor& actor, const Vector4& color, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Resize an actor.
   * The default alpha function will be used.
   * The resizing will start & end when the animation begins & ends.
   * The depth defaults to the minimum of width & height.
   * @param [in] actor The actor to animate.
   * @param [in] width The target width.
   * @param [in] height The target height.
   */
  void Resize(Actor& actor, float width, float height);

  /**
   * Resize an actor. This overload allows the alpha function to be customized.
   * The resizing will start & end when the animation begins & ends.
   * The depth defaults to the minimum of width & height.
   * @param [in] actor The actor to animate.
   * @param [in] width The target width.
   * @param [in] height The target height.
   * @param [in] alpha The alpha function to apply.
   */
  void Resize(Actor& actor, float width, float height, AlphaFunction alpha);

  /**
   * Resize an actor. This overload allows the resizing start & end time to be customized.
   * The depth defaults to the minimum of width & height.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be greater than zero.
   * @param [in] actor The actor to animate.
   * @param [in] width The target width.
   * @param [in] height The target height.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void Resize(Actor& actor, float width, float height, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Resize an actor.
   * The default alpha function will be used.
   * The resizing will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] size The target size.
   */
  void Resize(Actor& actor, const Vector3& size);

  /**
   * Resize an actor. This overload allows the alpha function to be customized.
   * The resizing will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] size The target size.
   * @param [in] alpha The alpha function to apply.
   */
  void Resize(Actor& actor, const Vector3& size, AlphaFunction alpha);

  /**
   * Resize an actor. This overload allows the resizing start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be greater than zero.
   * @param [in] actor The actor to animate.
   * @param [in] size The target size.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
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
   * @param[in] destroyAction The action to perform when the animation is destroyed.
   * @param[in] defaultAlpha The default alpha function to apply to animators.
   */
  Animation( SceneGraph::UpdateManager& updateManager,
             AnimationPlaylist& playlist,
             float durationSeconds,
             EndAction endAction,
             EndAction destroyAction,
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

  Dali::Animation::AnimationSignalV2 mFinishedSignal;

  FinishedCallback mFinishedCallback;
  Object* mFinishedCallbackObject;

  AnimatorConnectorContainer mConnectors; ///< Owned by the Animation

  // Cached for public getters
  float mDurationSeconds;
  float mSpeedFactor;
  bool mIsLooping;
  Vector2 mPlayRange;
  EndAction mEndAction;
  EndAction mDestroyAction;
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
