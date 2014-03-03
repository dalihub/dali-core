#ifndef __DALI_ANIMATION_H__
#define __DALI_ANIMATION_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES
#include <boost/any.hpp>
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

class Actor;
struct Degree;
class Quaternion;
struct Radian;
class ShaderEffect;
struct Property;
struct Vector2;
struct Vector3;
struct Vector4;

typedef boost::function<bool       (float alpha, const bool& current)>       AnimatorFunctionBool;
typedef boost::function<float      (float alpha, const float& current)>      AnimatorFunctionFloat;
typedef boost::function<Vector2    (float alpha, const Vector2& current)>    AnimatorFunctionVector2;
typedef boost::function<Vector3    (float alpha, const Vector3& current)>    AnimatorFunctionVector3;
typedef boost::function<Vector4    (float alpha, const Vector4& current)>    AnimatorFunctionVector4;
typedef boost::function<Quaternion (float alpha, const Quaternion& current)> AnimatorFunctionQuaternion;

namespace Internal DALI_INTERNAL
{
class Animation;
}

/**
 * Dali::Animation can be used to animate the properties of any number of objects, typically Actors.
 * An example animation setup is shown below:
 *
 * @code
 *
 * struct MyProgram
 * {
 *   Actor mActor; // The object we wish to animate
 *   Animation mAnimation; // Keep this to control the animation
 * }
 *
 * // ...To play the animation
 *
 * mAnimation = Animation::New(3.0f); // duration 3 seconds
 * mAnimation.MoveTo(mActor, 10.0f, 50.0f, 0.0f);
 * mAnimation.Play();
 *
 * @endcode
 *
 * Dali::Animation supports "fire and forget" behaviour i.e. an animation continues to play if the handle is discarded.
 * Note that in the following example, the "Finish" signal will be emitted:
 *
 * @code
 *
 * void ExampleCallback( Animation& source )
 * {
 *   std::cout << "Animation has finished" << std::endl;
 * }
 *
 * void ExampleAnimation( Actor actor )
 * {
 *   Animation animation = Animation::New(2.0f); // duration 2 seconds
 *   animation.MoveTo(actor, 10.0f, 50.0f, 0.0f);
 *   animation.SignalFinished().Connect( ExampleCallback );
 *   animation.Play();
 * } // At this point the animation handle has gone out of scope
 *
 * Actor actor = Actor::New();
 * Stage::GetCurrent().Add( actor );
 *
 * // Fire animation and forget about it
 * ExampleAnimation( actor );
 *
 * // However the animation will continue, and "Animation has finished" will be printed after 2 seconds.
 *
 * @endcode
 *
 * If the "Finish" signal is connected to a member function of an object, it must be disconnected before the object is destroyed.
 * This is typically done in the object destructor, and requires either the Dali::Connection object or Dali::Animation handle to be stored.
 */
class DALI_IMPORT_API Animation : public BaseHandle
{
public:

  typedef SignalV2< void (Animation&) > AnimationSignalV2;

  typedef boost::any AnyFunction;
  typedef boost::function<Vector3 (float alpha, const Vector3& current)> Vector3AnimatorFunc;
  typedef boost::function<Quaternion (float alpha, const Quaternion& current)> QuaternionAnimatorFunc;

  enum EndAction
  {
    Bake,   ///< When the animation ends, the animated property values are saved.
    Discard ///< When the animation ends, the animated property values are forgotten.
  };

  //Signal Names
  static const char* const SIGNAL_FINISHED;

  //Action Names
  static const char* const ACTION_PLAY;
  static const char* const ACTION_STOP;
  static const char* const ACTION_PAUSE;

  /**
   * Create an uninitialized Animation; this can be initialized with Animation::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Animation();

  /**
   * Create an initialized Animation.
   * The animation will not loop.
   * The default end action is "Bake".
   * The default alpha function is linear.
   * @pre durationSeconds must be greater than zero.
   * @param [in] durationSeconds The duration in seconds.
   * @return A handle to a newly allocated Dali resource.
   */
  static Animation New(float durationSeconds);

  /**
   * Downcast an Object handle to Animation. If handle points to an Animation object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Animation object or an uninitialized handle
   */
  static Animation DownCast( BaseHandle handle );

  /**
   * Destructor
   */
  virtual ~Animation();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

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
   * This action is performed when the animation ends.
   * Default end action is bake
   * @param[in] action The end action.
   */
  void SetEndAction(EndAction action);

  /**
   * Returns the end action of the animation.
   * @return The end action.
   */
  EndAction GetEndAction() const;

  /**
   * Set the destroy action of the animation.
   * If the animation is destroyed this action is performed on the following update.
   * Default destroy action is bake
   * @param[in] action The destroy action.
   */
  void SetDestroyAction(EndAction action);

  /**
   * Returns the destroy action of the animation.
   * @return The destroy action.
   */
  EndAction GetDestroyAction() const;

  /**
   * Set the default alpha function for an animation.
   * This is applied to individual property animations, if no further alpha functions are supplied.
   * @param[in] alpha The default alpha function.
   */
  void SetDefaultAlphaFunction(AlphaFunction alpha);

  /**
   * Retrieve the default alpha function for an animation.
   * @return The default alpha function.
   */
  AlphaFunction GetDefaultAlphaFunction() const;

  /**
   * Play the animation.
   */
  void Play();

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
   * Connect to this signal to be notified when an Animation's animations have finished.
   * @return A signal object to Connect() with.
   */
  AnimationSignalV2& FinishedSignal();

  /**
   * Animate a property value by a relative amount.
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] target The target object/property to animate.
   * @param [in] relativeValue The property value will change by this amount.
   */
  void AnimateBy(Property target, Property::Value relativeValue);

  /**
   * Animate a property value by a relative amount.
   * The effect will start & end when the animation begins & ends.
   * @param [in] target The target object/property to animate.
   * @param [in] relativeValue The property value will change by this amount.
   * @param [in] alpha The alpha function to apply.
   */
  void AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha);

  /**
   * Animate a property value by a relative amount.
   * The default alpha function will be used.
   * @param [in] target The target object/property to animate.
   * @param [in] relativeValue The property value will increase/decrease by this amount.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateBy(Property target, Property::Value relativeValue, TimePeriod period);

  /**
   * Animate a property value by a relative amount.
   * @param [in] target The target object/property to animate.
   * @param [in] relativeValue The property value will increase/decrease by this amount.
   * @param [in] alpha The alpha function to apply.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha, TimePeriod period);

  /**
   * Animate a property to a destination value.
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] target The target object/property to animate.
   * @param [in] destinationValue The destination value.
   */
  void AnimateTo(Property target, Property::Value destinationValue);

  /**
   * Animate a property to a destination value.
   * The effect will start & end when the animation begins & ends.
   * @param [in] target The target object/property to animate.
   * @param [in] destinationValue The destination value.
   * @param [in] alpha The alpha function to apply.
   */
  void AnimateTo(Property target, Property::Value destinationValue, AlphaFunction alpha);

  /**
   * Animate a property to a destination value.
   * The default alpha function will be used.
   * @param [in] target The target object/property to animate.
   * @param [in] destinationValue The destination value.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateTo(Property target, Property::Value destinationValue, TimePeriod period);

  /**
   * Animate a property to a destination value.
   * @param [in] target The target object/property to animate.
   * @param [in] destinationValue The destination value.
   * @param [in] alpha The alpha function to apply.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateTo(Property target, Property::Value destinationValue, AlphaFunction alpha, TimePeriod period);

  /**
   * Animate a property between keyframes
   * @param [in] target The target object/property to animate.
   * @param [in] keyFrames The key frames
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames);

  /**
   * Animate a property between keyframes
   * @param [in] target The target object/property to animate.
   * @param [in] keyFrames The key frames
   * @param [in] alpha The alpha function to apply.
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha);

  /**
   * Animate a property between keyframes
   * @param [in] target The target object/property to animate.
   * @param [in] keyFrames The key frames
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, TimePeriod period);

  /**
   * Animate a property between keyframes
   * @param [in] target The target object/property to animate.
   * @param [in] keyFrames The key frames
   * @param [in] alpha The alpha function to apply.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period);

  /**
   * Animate a property using a custom function.
   * The function will be called from a separate animation-thread; it should return quickly, to avoid performance degredation.
   * @pre The property type is equal PropertyTypes::Get<P>().
   * @param [in] target The target object/property to animate.
   * @param [in] animatorFunc The function to call during the animation.
   */
  template <class P>
  void Animate( Property target, boost::function<P (float alpha, const P& current)> animatorFunc )
  {
    Animate( target, PropertyTypes::Get<P>(), animatorFunc );
  }

  /**
   * Animate a property using a custom function.
   * The function will be called from a separate animation-thread; it should return quickly, to avoid performance degredation.
   * @pre The property type is equal PropertyTypes::Get<P>().
   * @param [in] target The target object/property to animate.
   * @param [in] animatorFunc The function to call during the animation.
   * @param [in] alpha The alpha function to apply.
   */
  template <class P>
  void Animate( Property target, boost::function<P (float alpha, const P& current)> animatorFunc, AlphaFunction alpha )
  {
    Animate( target, PropertyTypes::Get<P>(), animatorFunc, alpha );
  }

  /**
   * Animate a property using a custom function.
   * The function will be called from a separate animation-thread; it should return quickly, to avoid performance degredation.
   * @pre The property type is equal PropertyTypes::Get<P>().
   * @param [in] target The target object/property to animate.
   * @param [in] animatorFunc The function to call during the animation.
   * @param [in] period The effect will occur during this time period.
   */
  template <class P>
  void Animate( Property target, boost::function<P (float alpha, const P& current)> animatorFunc, TimePeriod period )
  {
    Animate( target, PropertyTypes::Get<P>(), animatorFunc, period );
  }

  /**
   * Animate a property using a custom function.
   * The function will be called from a separate animation-thread; it should return quickly, to avoid performance degredation.
   * @pre The property type is equal PropertyTypes::Get<P>().
   * @param [in] target The target object/property to animate.
   * @param [in] animatorFunc The function to call during the animation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] period The effect will occur during this time period.
   */
  template <class P>
  void Animate( Property target, boost::function<P (float alpha, const P& current)> animatorFunc, AlphaFunction alpha, TimePeriod period )
  {
    Animate( target, PropertyTypes::Get<P>(), animatorFunc, alpha, period );
  }

  // Actor-specific convenience methods

  /**
   * Move an actor relative to its position
   * The default alpha function will be used.
   * The move will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x axis displacement.
   * @param [in] y axis displacement.
   * @param [in] z axis displacement.
   */
  void MoveBy(Actor actor, float x, float y, float z);

  /**
   * Move an actor relative to its position.
   * This overload allows the alpha function to be customized.
   * The move will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] displacement relative to current position.
   * @param [in] alpha The alpha function to apply.
   */
  void MoveBy(Actor actor, Vector3 displacement, AlphaFunction alpha);

  /**
   * Move an actor relative to its position.
   * This overload allows the translation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] displacement relative to current position.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void MoveBy(Actor actor, Vector3 displacement, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Move an actor to a target position.
   * The default alpha function will be used.
   * The move will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x axis position.
   * @param [in] y axis position.
   * @param [in] z axis position.
   */
  void MoveTo(Actor actor, float x, float y, float z);

  /**
   * Move an actor to a target position. This overload allows the alpha function to be customized.
   * The move will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] position to move to
   * @param [in] alpha The alpha function to apply.
   */
  void MoveTo(Actor actor, Vector3 position, AlphaFunction alpha);

  /**
   * Move an actor to a target position. This overload allows the translation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] position to move to
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void MoveTo(Actor actor, Vector3 position, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Move an actor using a custom function.
   * The animatorFunc will be called from a separate animation-thread; it should return quickly, to avoid performance degredation.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] animatorFunc The function to call during the animation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the translation.
   */
  void Move(Actor actor, AnimatorFunctionVector3 animatorFunc, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Rotate an actor around an arbitrary axis.
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in degrees.
   * @param [in] axis The axis to rotate around
   */
  void RotateBy(Actor actor, Degree angle, Vector3 axis);

  /**
   * Rotate an actor around an arbitrary axis.
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in radians.
   * @param [in] axis The axis to rotate around
   */
  void RotateBy(Actor actor, Radian angle, Vector3 axis);

  /**
   * Rotate an actor around an arbitrary axis. This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in radians.
   * @param [in] axis The axis to rotate around.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateBy(Actor actor, Degree angle, Vector3 axis, AlphaFunction alpha);

  /**
   * Rotate an actor around an arbitrary axis. This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in radians.
   * @param [in] axis The axis to rotate around.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateBy(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha);

  /**
   * Rotate an actor around an arbitrary axis. This overload allows the rotation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in degrees.
   * @param [in] axis The axis to rotate around
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the rotation.
   */
  void RotateBy(Actor actor, Degree angle, Vector3 axis, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Rotate an actor around an arbitrary axis. This overload allows the rotation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in radians.
   * @param [in] axis The axis to rotate around
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the rotation.
   */
  void RotateBy(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Rotate an actor to a target orientation.
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in degrees.
   * @param [in] axis The target axis of rotation.
   */
  void RotateTo(Actor actor, Degree angle, Vector3 axis);

  /**
   * Rotate an actor to a target orientation.
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in radians.
   * @param [in] axis The target axis of rotation.
   */
  void RotateTo(Actor actor, Radian angle, Vector3 axis);

  /**
   * Rotate an actor to a target orientation.
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] orientation The target orientation.
   */
  void RotateTo(Actor actor, Quaternion orientation);

  /**
   * Rotate an actor to a target orientation. This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in degrees.
   * @param [in] axis The target axis of rotation.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateTo(Actor actor, Degree angle, Vector3 axis, AlphaFunction alpha);

  /**
   * Rotate an actor to a target orientation. This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in radians.
   * @param [in] axis The target axis of rotation.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateTo(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha);

  /**
   * Rotate an actor to a target orientation. This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] orientation The target orientation.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateTo(Actor actor, Quaternion orientation, AlphaFunction alpha);

  /**
   * Rotate an actor to a target orientation. This overload allows the rotation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in degrees.
   * @param [in] axis The target axis of rotation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the rotation.
   */
  void RotateTo(Actor actor, Degree angle, Vector3 axis, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Rotate an actor to a target orientation. This overload allows the rotation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in radians.
   * @param [in] axis The target axis of rotation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the rotation.
   */
  void RotateTo(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Rotate an actor to a target orientation. This overload allows the rotation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] orientation The target orientation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the rotation.
   */
  void RotateTo(Actor actor, Quaternion orientation, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Rotate an actor using a custom function.
   * The animatorFunc will be called from a separate animation-thread; it should return quickly, to avoid performance degredation.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] animatorFunc The function to call during the animation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the rotation.
   */
  void Rotate(Actor actor, AnimatorFunctionQuaternion animatorFunc, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Scale an actor.
   * The default alpha function will be used.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x Scale factor in the X-direction.
   * @param [in] y Scale factor in the Y-direction.
   * @param [in] z Scale factor in the Z-direction.
   */
  void ScaleBy(Actor actor, float x, float y, float z);

  /**
   * Scale an actor. This overload allows the alpha function to be customized.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] scale The scale factor.
   * @param [in] alpha The alpha function to apply.
   */
  void ScaleBy(Actor actor, Vector3 scale, AlphaFunction alpha);

  /**
   * Scale an actor. This overload allows the scaling start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] scale The scale factor.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the scaling.
   */
  void ScaleBy(Actor actor, Vector3 scale, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Scale an actor to a target scale factor.
   * The default alpha function will be used.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x Target scale-factor in the X-direction.
   * @param [in] y Target scale-factor in the Y-direction.
   * @param [in] z Target scale-factor in the Z-direction.
   */
  void ScaleTo(Actor actor, float x, float y, float z);

  /**
   * Scale an actor to a target scale factor. This overload allows the alpha function to be customized.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] scale The target scale factor.
   * @param [in] alpha The alpha function to apply.
   */
  void ScaleTo(Actor actor, Vector3 scale, AlphaFunction alpha);

  /**
   * Scale an actor to a target scale factor. This overload allows the scaling start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] scale The target scale factor.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the scaling.
   */
  void ScaleTo(Actor actor, Vector3 scale, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Show an actor during the animation.
   * @param [in] actor The actor to animate.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   */
  void Show(Actor actor, float delaySeconds);

  /**
   * Hide an actor during the animation.
   * @param [in] actor The actor to animate.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   */
  void Hide(Actor actor, float delaySeconds);

  /**
   * Animate the opacity of an actor.
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The relative change in opacity.
   */
  void OpacityBy(Actor actor, float opacity);

  /**
   * Animate the opacity of an actor. This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The relative change in opacity.
   * @param [in] alpha The alpha function to apply.
   */
  void OpacityBy(Actor actor, float opacity, AlphaFunction alpha);

  /**
   * Animate the opacity of an actor. This overload allows the animation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The relative change in opacity.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the opacity animation.
   */
  void OpacityBy(Actor actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Animate an actor to a target opacity.
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The target opacity.
   */
  void OpacityTo(Actor actor, float opacity);

  /**
   * Animate an actor to a target opacity. This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The target opacity.
   * @param [in] alpha The alpha function to apply.
   */
  void OpacityTo(Actor actor, float opacity, AlphaFunction alpha);

  /**
   * Animate an actor to a target opacity. This overload allows the animation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The target opacity.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the opacity animation.
   */
  void OpacityTo(Actor actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Animate the color of an actor.
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The relative change in color.
   */
  void ColorBy(Actor actor, Vector4 color);

  /**
   * Animate the color of an actor. This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The relative change in color.
   * @param [in] alpha The alpha function to apply.
   */
  void ColorBy(Actor actor, Vector4 color, AlphaFunction alpha);

  /**
   * Animate the color of an actor. This overload allows the animation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] color The relative change in color.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the color animation.
   */
  void ColorBy(Actor actor, Vector4 color, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Animate an actor to a target color.
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The target color.
   */
  void ColorTo(Actor actor, Vector4 color);

  /**
   * Animate an actor to a target color. This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The target color.
   * @param [in] alpha The alpha function to apply.
   */
  void ColorTo(Actor actor, Vector4 color, AlphaFunction alpha);

  /**
   * Animate an actor to a target color. This overload allows the animation start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] color The target color.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the color animation.
   */
  void ColorTo(Actor actor, Vector4 color, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Resize an actor.
   * The default alpha function will be used.
   * The resizing will start & end when the animation begins & ends.
   * The depth defaults to the minimum of width & height.
   * @param [in] actor The actor to animate.
   * @param [in] width The target width.
   * @param [in] height The target height.
   */
  void Resize(Actor actor, float width, float height);

  /**
   * Resize an actor. This overload allows the alpha function to be customized.
   * The resizing will start & end when the animation begins & ends.
   * The depth defaults to the minimum of width & height.
   * @param [in] actor The actor to animate.
   * @param [in] width The target width.
   * @param [in] height The target height.
   * @param [in] alpha The alpha function to apply.
   */
  void Resize(Actor actor, float width, float height, AlphaFunction alpha);

  /**
   * Resize an actor. This overload allows the resizing start & end time to be customized.
   * The depth defaults to the minimum of width & height.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] width The target width.
   * @param [in] height The target height.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the resizing.
   */
  void Resize(Actor actor, float width, float height, AlphaFunction alpha, float delaySeconds, float durationSeconds);

  /**
   * Resize an actor.
   * The default alpha function will be used.
   * The resizing will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] size The target size.
   */
  void Resize(Actor actor, Vector3 size);

  /**
   * Resize an actor. This overload allows the alpha function to be customized.
   * The resizing will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] size The target size.
   * @param [in] alpha The alpha function to apply.
   */
  void Resize(Actor actor, Vector3 size, AlphaFunction alpha);

  /**
   * Resize an actor. This overload allows the resizing start & end time to be customized.
   * @pre delaySeconds must be zero or greater.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] actor The actor to animate.
   * @param [in] size The target size.
   * @param [in] alpha The alpha function to apply.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   * @param [in] durationSeconds The duration of the resizing.
   */
  void Resize(Actor actor, Vector3 size, AlphaFunction alpha, float delaySeconds, float durationSeconds);

public: // Not intended for use by Application developers

  /**
   * This constructor is used by Dali New() methods
   * @param [in] animation A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Animation(Internal::Animation* animation);

private:

  /**
   * Animate a property using a custom function.
   * @pre The property type is equal expectedType.
   * @param [in] target The target object/property to animate.
   * @param [in] targetType The expected type of the property.
   * @param [in] func The function to call during the animation.
   */
  void Animate( Property target,
                Property::Type targetType,
                AnyFunction func );

  /**
   * Animate a property using a custom function.
   * @pre The property type is equal expectedType.
   * @param [in] target The target object/property to animate.
   * @param [in] targetType The expected type of the property.
   * @param [in] func The function to call during the animation.
   * @param [in] alpha The alpha function to apply.
   */
  void Animate( Property target,
                Property::Type targetType,
                AnyFunction func,
                AlphaFunction alpha );

  /**
   * Animate a property using a custom function.
   * @pre The property type is equal expectedType.
   * @param [in] target The target object/property to animate.
   * @param [in] targetType The expected type of the property.
   * @param [in] func The function to call during the animation.
   * @param [in] period The effect will occur during this time period.
   */
  void Animate( Property target,
                Property::Type targetType,
                AnyFunction func,
                TimePeriod period );

  /**
   * Animate a property using a custom function.
   * @pre The property type is equal expectedType.
   * @param [in] target The target object/property to animate.
   * @param [in] targetType The expected type of the property.
   * @param [in] func The function to call during the animation.
   * @param [in] alpha The alpha function to apply.
   * @param [in] period The effect will occur during this time period.
   */
  void Animate( Property target,
                Property::Type targetType,
                AnyFunction func,
                AlphaFunction alpha,
                TimePeriod period );
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_ANIMATION_H__
