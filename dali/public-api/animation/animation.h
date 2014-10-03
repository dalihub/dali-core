#ifndef __DALI_ANIMATION_H__
#define __DALI_ANIMATION_H__

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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/any.h>
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

typedef boost::function<bool       (float alpha, const bool& current)>       AnimatorFunctionBool;      ///< Animator function signature for boolean properties.
typedef boost::function<float      (float alpha, const float& current)>      AnimatorFunctionFloat;     ///< Animator function signature for float properties.
typedef boost::function<int        (float alpha, const int& current)>        AnimatorFunctionInteger;   ///< Animator function signature for integer properties.
typedef boost::function<Vector2    (float alpha, const Vector2& current)>    AnimatorFunctionVector2;   ///< Animator function signature for Vector2 properties.
typedef boost::function<Vector3    (float alpha, const Vector3& current)>    AnimatorFunctionVector3;   ///< Animator function signature for Vector3 properties.
typedef boost::function<Vector4    (float alpha, const Vector4& current)>    AnimatorFunctionVector4;   ///< Animator function signature for Vector4 properties.
typedef boost::function<Quaternion (float alpha, const Quaternion& current)> AnimatorFunctionQuaternion;///< Animator function signature for Quaternion properties.

namespace Internal DALI_INTERNAL
{
class Animation;
}

/**
 * @brief Dali::Animation can be used to animate the properties of any number of objects, typically Actors.
 *
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
 *   animation.FinishedSignal().Connect( ExampleCallback );
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

  typedef SignalV2< void (Animation&) > AnimationSignalV2; ///< Animation finished signal type

  typedef Any AnyFunction; ///< Interpolation function
  typedef boost::function<Vector3 (float alpha, const Vector3& current)> Vector3AnimatorFunc; ///< Interpolation function
  typedef boost::function<Quaternion (float alpha, const Quaternion& current)> QuaternionAnimatorFunc; ///< Interpolation function

  /**
   * @brief What to do when the animation ends, is stopped or is destroyed
   */
  enum EndAction
  {
    Bake,     ///< When the animation ends, the animated property values are saved.
    Discard,  ///< When the animation ends, the animated property values are forgotten.
    BakeFinal ///< If the animation is stopped, the animated property values are saved as if the animation had run to completion, otherwise behaves like Bake.
  };

  //Signal Names
  static const char* const SIGNAL_FINISHED; ///< name "finished"

  //Action Names
  static const char* const ACTION_PLAY;     ///< name "play"
  static const char* const ACTION_STOP;     ///< name "stop"
  static const char* const ACTION_PAUSE;    ///< name "pause"

  /**
   * @brief Create an uninitialized Animation; this can be initialized with Animation::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Animation();

  /**
   * @brief Create an initialized Animation.
   *
   * The animation will not loop.
   * The default end action is "Bake".
   * The default alpha function is linear.
   * @pre durationSeconds must be greater than zero.
   * @param [in] durationSeconds The duration in seconds.
   * @return A handle to a newly allocated Dali resource.
   */
  static Animation New(float durationSeconds);

  /**
   * @brief Downcast an Object handle to Animation.
   *
   * If handle points to an Animation object the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a Animation object or an uninitialized handle
   */
  static Animation DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Animation();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Animation(const Animation& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Animation& operator=(const Animation& rhs);

  /**
   * @brief This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   *
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  Animation& operator=(BaseHandle::NullType* rhs);

  /**
   * @brief Set the duration of an animation.
   *
   * @pre durationSeconds must be greater than zero.
   * @param[in] seconds The duration in seconds.
   */
  void SetDuration(float seconds);

  /**
   * @brief Retrieve the duration of an animation.
   *
   * @return The duration in seconds.
   */
  float GetDuration() const;

  /**
   * @brief Set whether the animation will loop.
   *
   * @param[in] looping True if the animation will loop.
   */
  void SetLooping(bool looping);

  /**
   * @brief Query whether the animation will loop.
   *
   * @return True if the animation will loop.
   */
  bool IsLooping() const;

  /**
   * @brief Set the end action of the animation.
   *
   * This action is performed when the animation ends or if it is stopped.
   * Default end action is bake
   * @param[in] action The end action.
   */
  void SetEndAction(EndAction action);

  /**
   * @brief Returns the end action of the animation.
   *
   * @return The end action.
   */
  EndAction GetEndAction() const;

  /**
   * @brief Set the disconnect action.
   *
   * If any of the animated property owners are disconnected from the stage, then this action is performed.
   * Default action is to BakeFinal.
   * @param[in] disconnectAction The disconnect action.
   */
  void SetDisconnectAction( EndAction disconnectAction );

  /**
   * @brief Returns the disconnect action.
   *
   * @return The disconnect action.
   */
  EndAction GetDisconnectAction() const;

  /**
   * @brief Set the default alpha function for an animation.
   *
   * This is applied to individual property animations, if no further alpha functions are supplied.
   * @param[in] alpha The default alpha function.
   */
  void SetDefaultAlphaFunction(AlphaFunction alpha);

  /**
   * @brief Retrieve the default alpha function for an animation.
   *
   * @return The default alpha function.
   */
  AlphaFunction GetDefaultAlphaFunction() const;

  /*
   * @brief Sets the progress of the animation.
   * The animation will play (or continue playing) from this point. The progress
   * must be in the 0-1 interval or in the play range interval if defined ( See SetPlayRange ),
   * otherwise, it will be ignored.
   *
   * @param[in] progress The new progress as a normalized value between [0,1] or between the
   * play range if specified.
   */
  void SetCurrentProgress( float progress );

  /**
  * @brief Retrieve the current progress of the animation.
  *
  * @return The current progress as a normalized value between [0,1].
  */
  float GetCurrentProgress();

  /**
   * @brief Specifies an speed factor for the animation.
   *
   * The speed factor is a multiplier of the normal velocity of the animation. Values between [0,1] will
   * slow down the animation and values above one will speed up the animation. It is also possible to specify a negative multiplier
   * to play the animation in reverse.
   *
   * @param[in] factor A value which will multiply the velocity.
   */
  void SetSpeedFactor( float factor );

  /**
   * @brief Retrieve the speed factor of the animation
   *
   * @return speed factor
   */
  float GetSpeedFactor() const;

  /**
   * @brief Set the playing range.
   * Animation will play between the values specified. Both values ( range.x and range.y ) should be between 0-1,
   * otherwise they will be ignored. If the range provided is not in proper order ( minimum,maximum ), it will be reordered.
   *
   * @param[in] range Two values between [0,1] to specify minimum and maximum progress. The
   * animation will play between those values.
   */
  void SetPlayRange( const Vector2& range );

  /**
   * @brief Get the playing range
   *
   * @return The play range defined for the animation.
   */
  Vector2 GetPlayRange() const;

  /**
   * @brief Play the animation.
   */
  void Play();

  /**
   * @brief Play the animation from a given point.
   * The progress must be in the 0-1 interval or in the play range interval if defined ( See SetPlayRange ),
   * otherwise, it will be ignored.
   *
   * @param[in] progress A value between [0,1], or between the play range if specified, form where the animation should start playing
   */
  void PlayFrom( float progress );

  /**
   * @brief Pause the animation.
   */
  void Pause();

  /**
   * @brief Stop the animation.
   */
  void Stop();

  /**
   * @brief Clear the animation.
   *
   * This disconnects any objects that were being animated, effectively stopping the animation.
   */
  void Clear();

  /**
   * @brief Connect to this signal to be notified when an Animation's animations have finished.
   *
   * @return A signal object to Connect() with.
   */
  AnimationSignalV2& FinishedSignal();

  /**
   * @brief Animate a property value by a relative amount.
   *
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] target The target object/property to animate.
   * @param [in] relativeValue The property value will change by this amount.
   */
  void AnimateBy(Property target, Property::Value relativeValue);

  /**
   * @brief Animate a property value by a relative amount.
   *
   * The effect will start & end when the animation begins & ends.
   * @param [in] target The target object/property to animate.
   * @param [in] relativeValue The property value will change by this amount.
   * @param [in] alpha The alpha function to apply.
   */
  void AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha);

  /**
   * @brief Animate a property value by a relative amount.
   *
   * The default alpha function will be used.
   * @param [in] target The target object/property to animate.
   * @param [in] relativeValue The property value will increase/decrease by this amount.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateBy(Property target, Property::Value relativeValue, TimePeriod period);

  /**
   * @brief Animate a property value by a relative amount.
   *
   * @param [in] target The target object/property to animate.
   * @param [in] relativeValue The property value will increase/decrease by this amount.
   * @param [in] alpha The alpha function to apply.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha, TimePeriod period);

  /**
   * @brief Animate a property to a destination value.
   *
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] target The target object/property to animate.
   * @param [in] destinationValue The destination value.
   */
  void AnimateTo(Property target, Property::Value destinationValue);

  /**
   * @brief Animate a property to a destination value.
   *
   * The effect will start & end when the animation begins & ends.
   * @param [in] target The target object/property to animate.
   * @param [in] destinationValue The destination value.
   * @param [in] alpha The alpha function to apply.
   */
  void AnimateTo(Property target, Property::Value destinationValue, AlphaFunction alpha);

  /**
   * @brief Animate a property to a destination value.
   *
   * The default alpha function will be used.
   * @param [in] target The target object/property to animate.
   * @param [in] destinationValue The destination value.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateTo(Property target, Property::Value destinationValue, TimePeriod period);

  /**
   * @brief Animate a property to a destination value.
   *
   * @param [in] target The target object/property to animate.
   * @param [in] destinationValue The destination value.
   * @param [in] alpha The alpha function to apply.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateTo(Property target, Property::Value destinationValue, AlphaFunction alpha, TimePeriod period);

  /**
   * @brief Animate a property between keyframes.
   *
   * @param [in] target The target object/property to animate.
   * @param [in] keyFrames The key frames
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames);

  /**
   * @brief Animate a property between keyframes.
   *
   * @param [in] target The target object/property to animate.
   * @param [in] keyFrames The key frames
   * @param [in] alpha The alpha function to apply.
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha);

  /**
   * @brief Animate a property between keyframes.
   *
   * @param [in] target The target object/property to animate.
   * @param [in] keyFrames The key frames
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, TimePeriod period);

  /**
   * @brief Animate a property between keyframes.
   *
   * @param [in] target The target object/property to animate.
   * @param [in] keyFrames The key frames
   * @param [in] alpha The alpha function to apply.
   * @param [in] period The effect will occur during this time period.
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period);

  /**
   * @brief Animate a property using a custom function.
   *
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
   * @brief Animate a property using a custom function.
   *
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
   * @brief Animate a property using a custom function.
   *
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
   * @brief Animate a property using a custom function.
   *
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
   * @brief Move an actor relative to its position.
   *
   * The default alpha function will be used.
   * The move will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x axis displacement.
   * @param [in] y axis displacement.
   * @param [in] z axis displacement.
   */
  void MoveBy(Actor actor, float x, float y, float z);

  /**
   * @brief Move an actor relative to its position.
   *
   * This overload allows the alpha function to be customized.
   * The move will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] displacement relative to current position.
   * @param [in] alpha The alpha function to apply.
   */
  void MoveBy(Actor actor, Vector3 displacement, AlphaFunction alpha);

  /**
   * @brief Move an actor relative to its position.
   *
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
   * @brief Move an actor to a target position.
   *
   * The default alpha function will be used.
   * The move will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x axis position.
   * @param [in] y axis position.
   * @param [in] z axis position.
   */
  void MoveTo(Actor actor, float x, float y, float z);

  /**
   * @brief Move an actor to a target position.
   *
   * This overload allows the alpha function to be customized.
   * The move will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] position to move to
   * @param [in] alpha The alpha function to apply.
   */
  void MoveTo(Actor actor, Vector3 position, AlphaFunction alpha);

  /**
   * @brief Move an actor to a target position.
   *
   * This overload allows the translation start & end time to be customized.
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
   * @brief Move an actor using a custom function.
   *
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
   * @brief Rotate an actor around an arbitrary axis.
   *
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in degrees.
   * @param [in] axis The axis to rotate around
   */
  void RotateBy(Actor actor, Degree angle, Vector3 axis);

  /**
   * @brief Rotate an actor around an arbitrary axis.
   *
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in radians.
   * @param [in] axis The axis to rotate around
   */
  void RotateBy(Actor actor, Radian angle, Vector3 axis);

  /**
   * @brief Rotate an actor around an arbitrary axis.
   *
   * This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in radians.
   * @param [in] axis The axis to rotate around.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateBy(Actor actor, Degree angle, Vector3 axis, AlphaFunction alpha);

  /**
   * @brief Rotate an actor around an arbitrary axis.
   *
   * This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The angle in radians.
   * @param [in] axis The axis to rotate around.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateBy(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha);

  /**
   * @brief Rotate an actor around an arbitrary axis.
   *
   * This overload allows the rotation start & end time to be customized.
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
   * @brief Rotate an actor around an arbitrary axis.
   *
   * This overload allows the rotation start & end time to be customized.
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
   * @brief Rotate an actor to a target orientation.
   *
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in degrees.
   * @param [in] axis The target axis of rotation.
   */
  void RotateTo(Actor actor, Degree angle, Vector3 axis);

  /**
   * @brief Rotate an actor to a target orientation.
   *
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in radians.
   * @param [in] axis The target axis of rotation.
   */
  void RotateTo(Actor actor, Radian angle, Vector3 axis);

  /**
   * @brief Rotate an actor to a target orientation.
   *
   * The default alpha function will be used.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] orientation The target orientation.
   */
  void RotateTo(Actor actor, Quaternion orientation);

  /**
   * @brief Rotate an actor to a target orientation.
   *
   * This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in degrees.
   * @param [in] axis The target axis of rotation.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateTo(Actor actor, Degree angle, Vector3 axis, AlphaFunction alpha);

  /**
   * @brief Rotate an actor to a target orientation.
   *
   * This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] angle The target rotation angle in radians.
   * @param [in] axis The target axis of rotation.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateTo(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha);

  /**
   * @brief Rotate an actor to a target orientation.
   *
   * This overload allows the alpha function to be customized.
   * The rotation will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] orientation The target orientation.
   * @param [in] alpha The alpha function to apply.
   */
  void RotateTo(Actor actor, Quaternion orientation, AlphaFunction alpha);

  /**
   * @brief Rotate an actor to a target orientation.
   *
   * This overload allows the rotation start & end time to be customized.
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
   * @brief Rotate an actor to a target orientation.
   *
   * This overload allows the rotation start & end time to be customized.
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
   * @brief Rotate an actor to a target orientation.
   *
   * This overload allows the rotation start & end time to be customized.
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
   * @brief Rotate an actor using a custom function.
   *
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
   * @brief Scale an actor.
   *
   * The default alpha function will be used.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x Scale factor in the X-direction.
   * @param [in] y Scale factor in the Y-direction.
   * @param [in] z Scale factor in the Z-direction.
   */
  void ScaleBy(Actor actor, float x, float y, float z);

  /**
   * @brief Scale an actor.
   *
   * This overload allows the alpha function to be customized.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] scale The scale factor.
   * @param [in] alpha The alpha function to apply.
   */
  void ScaleBy(Actor actor, Vector3 scale, AlphaFunction alpha);

  /**
   * @brief Scale an actor.
   *
   * This overload allows the scaling start & end time to be customized.
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
   * @brief Scale an actor to a target scale factor.
   *
   * The default alpha function will be used.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] x Target scale-factor in the X-direction.
   * @param [in] y Target scale-factor in the Y-direction.
   * @param [in] z Target scale-factor in the Z-direction.
   */
  void ScaleTo(Actor actor, float x, float y, float z);

  /**
   * @brief Scale an actor to a target scale factor.
   *
   * This overload allows the alpha function to be customized.
   * The scaling will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] scale The target scale factor.
   * @param [in] alpha The alpha function to apply.
   */
  void ScaleTo(Actor actor, Vector3 scale, AlphaFunction alpha);

  /**
   * @brief Scale an actor to a target scale factor.
   *
   * This overload allows the scaling start & end time to be customized.
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
   * @brief Show an actor during the animation.
   *
   * @param [in] actor The actor to animate.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   */
  void Show(Actor actor, float delaySeconds);

  /**
   * @brief Hide an actor during the animation.
   *
   * @param [in] actor The actor to animate.
   * @param [in] delaySeconds The initial delay from the start of the animation.
   */
  void Hide(Actor actor, float delaySeconds);

  /**
   * @brief Animate the opacity of an actor.
   *
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The relative change in opacity.
   */
  void OpacityBy(Actor actor, float opacity);

  /**
   * @brief Animate the opacity of an actor.
   *
   * This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The relative change in opacity.
   * @param [in] alpha The alpha function to apply.
   */
  void OpacityBy(Actor actor, float opacity, AlphaFunction alpha);

  /**
   * @brief Animate the opacity of an actor.
   *
   * This overload allows the animation start & end time to be customized.
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
   * @brief Animate an actor to a target opacity.
   *
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The target opacity.
   */
  void OpacityTo(Actor actor, float opacity);

  /**
   * @brief Animate an actor to a target opacity.
   *
   * This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] opacity The target opacity.
   * @param [in] alpha The alpha function to apply.
   */
  void OpacityTo(Actor actor, float opacity, AlphaFunction alpha);

  /**
   * @brief Animate an actor to a target opacity.
   *
   * This overload allows the animation start & end time to be customized.
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
   * @brief Animate the color of an actor.
   *
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The relative change in color.
   */
  void ColorBy(Actor actor, Vector4 color);

  /**
   * @brief Animate the color of an actor.
   *
   * This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The relative change in color.
   * @param [in] alpha The alpha function to apply.
   */
  void ColorBy(Actor actor, Vector4 color, AlphaFunction alpha);

  /**
   * @brief Animate the color of an actor.
   *
   * This overload allows the animation start & end time to be customized.
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
   * @brief Animate an actor to a target color.
   *
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The target color.
   */
  void ColorTo(Actor actor, Vector4 color);

  /**
   * @brief Animate an actor to a target color.
   *
   * This overload allows the alpha function to be customized.
   * The effect will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] color The target color.
   * @param [in] alpha The alpha function to apply.
   */
  void ColorTo(Actor actor, Vector4 color, AlphaFunction alpha);

  /**
   * @brief Animate an actor to a target color.
   *
   * This overload allows the animation start & end time to be customized.
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
   * @brief Resize an actor.
   *
   * The default alpha function will be used.
   * The resizing will start & end when the animation begins & ends.
   * The depth defaults to the minimum of width & height.
   * @param [in] actor The actor to animate.
   * @param [in] width The target width.
   * @param [in] height The target height.
   */
  void Resize(Actor actor, float width, float height);

  /**
   * @brief Resize an actor.
   *
   * This overload allows the alpha function to be customized.
   * The resizing will start & end when the animation begins & ends.
   * The depth defaults to the minimum of width & height.
   * @param [in] actor The actor to animate.
   * @param [in] width The target width.
   * @param [in] height The target height.
   * @param [in] alpha The alpha function to apply.
   */
  void Resize(Actor actor, float width, float height, AlphaFunction alpha);

  /**
   * @brief Resize an actor.
   *
   * This overload allows the resizing start & end time to be customized.
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
   * @brief Resize an actor.
   *
   * The default alpha function will be used.
   * The resizing will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] size The target size.
   */
  void Resize(Actor actor, Vector3 size);

  /**
   * @brief Resize an actor.
   *
   * This overload allows the alpha function to be customized.
   * The resizing will start & end when the animation begins & ends.
   * @param [in] actor The actor to animate.
   * @param [in] size The target size.
   * @param [in] alpha The alpha function to apply.
   */
  void Resize(Actor actor, Vector3 size, AlphaFunction alpha);

  /**
   * @brief Resize an actor.
   *
   * This overload allows the resizing start & end time to be customized.
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
   * @brief This constructor is used by Dali New() methods
   * @param [in] animation A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Animation(Internal::Animation* animation);

private:

  /**
   * @brief Animate a property using a custom function.
   *
   * @pre The property type is equal expectedType.
   * @param [in] target The target object/property to animate.
   * @param [in] targetType The expected type of the property.
   * @param [in] func The function to call during the animation.
   */
  void Animate( Property target,
                Property::Type targetType,
                AnyFunction func );

  /**
   * @brief Animate a property using a custom function.
   *
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
   * @brief Animate a property using a custom function.
   *
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
   * @brief Animate a property using a custom function.
   *
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

#endif // __DALI_ANIMATION_H__
