#ifndef DALI_ANIMATION_H
#define DALI_ANIMATION_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/animation/path.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
/**
 * @addtogroup dali_core_animation
 * @{
 */

class Actor;
struct Property;
struct Vector2;
struct Vector3;

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
 * mAnimation.AnimateTo(Property(mActor, Actor::Property::POSITION), Vector3(10.0f, 50.0f, 0.0f));
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
 *   animation.AnimateTo(Property(actor, Actor::Property::POSITION), 10.0f, 50.0f, 0.0f);
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
 *
 * The overall animation time is superseded by the values given in the TimePeriod structure used when calling the AnimateTo(), AnimateBy(), AnimateBetween() and Animate() methods.
 * If any of the individual calls to those functions exceeds the overall animation time, then the overall animation time is automatically extended.
 *
 * Using AnimateTo and AnimateBy for the same property of the same Actor will yield undefined behaviour especially if the TimePeriod overlaps.
 *
 * After calling Animation::Play(), Handle::GetProperty will return the target value of the animated property.
 *
 * Signals
 * | %Signal Name | Method                   |
 * |--------------|--------------------------|
 * | finished     | @ref FinishedSignal()    |
 *
 * Actions
 * | %Action Name | %Animation method called |
 * |--------------|--------------------------|
 * | play         | Play()                   |
 * | stop         | Stop()                   |
 * | pause        | Pause()                  |
 * @SINCE_1_0.0
 */
class DALI_CORE_API Animation : public BaseHandle
{
public:

  typedef Signal< void (Animation&) > AnimationSignalType; ///< Animation finished signal type @SINCE_1_0.0

  typedef Any AnyFunction; ///< Interpolation function @SINCE_1_0.0

  /**
   * @brief Enumeration for what to do when the animation ends, is stopped, or is destroyed.
   * @SINCE_1_0.0
   */
  enum EndAction
  {
    Bake,     ///< When the animation ends, the animated property values are saved. @SINCE_1_0.0
    Discard,  ///< When the animation ends, the animated property values are forgotten. @SINCE_1_0.0
    BakeFinal ///< If the animation is stopped, the animated property values are saved as if the animation had run to completion, otherwise behaves like Bake. @SINCE_1_0.0
  };

  /**
   * @brief Enumeration for what interpolation method to use on key-frame animations.
   * @SINCE_1_0.0
   */
  enum Interpolation
  {
    Linear,   ///< Values in between key frames are interpolated using a linear polynomial. (Default) @SINCE_1_0.0
    Cubic     ///< Values in between key frames are interpolated using a cubic polynomial. @SINCE_1_0.0
  };

  /**
   * @brief Enumeration for what state the animation is in.
   *
   * Note: Calling Reset() on this class will NOT reset the animation. It will call BaseHandle::Reset() which drops the object handle.
   *
   * @SINCE_1_1.21
   */
  enum State
  {
    STOPPED,   ///< Animation has stopped @SINCE_1_1.21
    PLAYING,   ///< The animation is playing @SINCE_1_1.21
    PAUSED     ///< The animation is paused @SINCE_1_1.21
  };

  /**
   * @brief Enumeration for what looping mode is in.
   *
   * @SINCE_1_2.60
   */
  enum LoopingMode
  {
    RESTART,      ///< When the animation arrives at the end in looping mode, the animation restarts from the beginning. @SINCE_1_2.60
    AUTO_REVERSE  ///< When the animation arrives at the end in looping mode, the animation reverses direction and runs backwards again. @SINCE_1_2.60
  };

  /**
   * @brief Creates an uninitialized Animation; this can be initialized with Animation::New().
   *
   * Calling member functions with an uninitialized Animation handle is not allowed.
   * @SINCE_1_0.0
   */
  Animation();

  /**
   * @brief Creates an initialized Animation.
   *
   * The animation will not loop.
   * The default end action is "Bake".
   * The default alpha function is linear.
   * @SINCE_1_0.0
   * @param[in] durationSeconds The duration in seconds
   * @return A handle to a newly allocated Dali resource
   * @note durationSeconds can not be negative.
   */
  static Animation New(float durationSeconds);

  /**
   * @brief Downcasts a handle to Animation handle.
   *
   * If handle points to an Animation object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to an Animation object or an uninitialized handle
   */
  static Animation DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Animation();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  Animation(const Animation& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  Animation& operator=(const Animation& rhs);

  /**
   * @brief Sets the duration of an animation.
   *
   * @SINCE_1_0.0
   * @param[in] seconds The duration in seconds
   * @pre DurationSeconds must be greater than zero.
   */
  void SetDuration(float seconds);

  /**
   * @brief Retrieves the duration of an animation.
   *
   * @SINCE_1_0.0
   * @return The duration in seconds
   */
  float GetDuration() const;

  /**
   * @brief Sets whether the animation will loop.
   *
   * This function resets the loop count and should not be used with SetLoopCount(int).
   * Setting this parameter does not cause the animation to Play().
   *
   * @SINCE_1_0.0
   * @param[in] looping True if the animation will loop
   */
  void SetLooping(bool looping);

  /**
   * @brief Enables looping for 'count' repeats.
   *
   * A zero is the same as SetLooping(true) i.e. repeat forever.
   * This function resets the looping value and should not be used with SetLooping(bool).
   * Setting this parameter does not cause the animation to Play().
   *
   * @SINCE_1_1.20
   * @param[in] count The number of times to loop
   */
  void SetLoopCount(int32_t  count);

  /**
   * @brief Gets the loop count.
   *
   * A zero is the same as SetLooping(true) ie repeat forever.
   * The loop count is initially 1 for play once.
   *
   * @SINCE_1_1.20
   * @return The number of times to loop
   */
  int32_t  GetLoopCount();

  /**
   * @brief Gets the current loop count.
   *
   * A value 0 to GetLoopCount() indicating the current loop count when looping.
   *
   * @SINCE_1_1.20
   * @return The current number of loops that have occured
   */
  int32_t  GetCurrentLoop();

  /**
   * @brief Queries whether the animation will loop.
   *
   * @SINCE_1_0.0
   * @return True if the animation will loop
   */
  bool IsLooping() const;

  /**
   * @brief Sets the end action of the animation.
   *
   * This action is performed when the animation ends or if it is stopped.
   * Default end action is bake.
   * @SINCE_1_0.0
   * @param[in] action The end action
   */
  void SetEndAction(EndAction action);

  /**
   * @brief Returns the end action of the animation.
   *
   * @SINCE_1_0.0
   * @return The end action
   */
  EndAction GetEndAction() const;

  /**
   * @brief Sets the disconnect action.
   *
   * If any of the animated property owners are disconnected from the stage while the animation is being played, then this action is performed.
   * Default action is to BakeFinal.
   * @SINCE_1_0.0
   * @param[in] disconnectAction The disconnect action
   */
  void SetDisconnectAction( EndAction disconnectAction );

  /**
   * @brief Returns the disconnect action.
   *
   * @SINCE_1_0.0
   * @return The disconnect action
   */
  EndAction GetDisconnectAction() const;

  /**
   * @brief Sets the default alpha function for an animation.
   *
   * This is applied to individual property animations, if no further alpha functions are supplied.
   * @SINCE_1_0.0
   * @param[in] alpha The default alpha function
   */
  void SetDefaultAlphaFunction(AlphaFunction alpha);

  /**
   * @brief Retrieves the default alpha function for an animation.
   *
   * @SINCE_1_0.0
   * @return The default alpha function
   */
  AlphaFunction GetDefaultAlphaFunction() const;

  /**
   * @brief Sets the progress of the animation.
   *
   * The animation will play (or continue playing) from this point. The progress
   * must be in the 0-1 interval or in the play range interval if defined ( See @ref Animation::SetPlayRange ),
   * otherwise, it will be ignored.
   *
   * @SINCE_1_0.0
   * @param[in] progress The new progress as a normalized value between [0,1]
   * or between the play range if specified
   */
  void SetCurrentProgress( float progress );

  /**
  * @brief Retrieves the current progress of the animation.
  *
  * @SINCE_1_0.0
  * @return The current progress as a normalized value between [0,1]
  */
  float GetCurrentProgress();

  /**
   * @brief Specifies a speed factor for the animation.
   *
   * The speed factor is a multiplier of the normal velocity of the animation. Values between [0,1] will
   * slow down the animation and values above one will speed up the animation. It is also possible to specify a negative multiplier
   * to play the animation in reverse.
   *
   * @SINCE_1_0.0
   * @param[in] factor A value which will multiply the velocity
   */
  void SetSpeedFactor( float factor );

  /**
   * @brief Retrieves the speed factor of the animation.
   *
   * @SINCE_1_0.0
   * @return Speed factor
   */
  float GetSpeedFactor() const;

  /**
   * @brief Sets the playing range.
   *
   * Animation will play between the values specified. Both values ( range.x and range.y ) should be between 0-1,
   * otherwise they will be ignored. If the range provided is not in proper order ( minimum,maximum ), it will be reordered.
   *
   * @SINCE_1_0.0
   * @param[in] range Two values between [0,1] to specify minimum and maximum progress. The
   * animation will play between those values
   */
  void SetPlayRange( const Vector2& range );

  /**
   * @brief Gets the playing range.
   *
   * @SINCE_1_0.0
   * @return The play range defined for the animation
   */
  Vector2 GetPlayRange() const;

  /**
   * @brief Play the animation.
   * @SINCE_1_0.0
   */
  void Play();

  /**
   * @brief Plays the animation from a given point.
   *
   * The progress must be in the 0-1 interval or in the play range interval if defined ( See @ref Animation::SetPlayRange ),
   * otherwise, it will be ignored.
   *
   * @SINCE_1_0.0
   * @param[in] progress A value between [0,1], or between the play range if specified, from where the animation should start playing
   */
  void PlayFrom( float progress );

  /**
   * @brief Play the animation after a given delay time.
   *
   * The delay time is not included in the looping time.
   * When the delay time is negative value, it would treat as play immediately.
   * @SINCE_1_2.60
   * @param[in] delaySeconds The delay time
   */
  void PlayAfter( float delaySeconds );

  /**
   * @brief Pauses the animation.
   * @SINCE_1_0.0
   */
  void Pause();

  /**
   * @brief Queries the state of the animation.
   * @SINCE_1_1.21
   * @return The Animation::State
   */
  State GetState() const;

  /**
   * @brief Stops the animation.
   * @SINCE_1_0.0
   */
  void Stop();

  /**
   * @brief Clears the animation.
   *
   * This disconnects any objects that were being animated, effectively stopping the animation.
   * @SINCE_1_0.0
   */
  void Clear();

  /**
   * @brief Sets the looping mode.
   *
   * Animation plays forwards and then restarts from the beginning or runs backwards again.
   * @SINCE_1_2.60
   * @param[in] loopingMode The looping mode is one of RESTART and AUTO_REVERSE
   */
  void SetLoopingMode( LoopingMode loopingMode );

  /**
   * @brief Gets one of the current looping mode.
   *
   * @SINCE_1_2.60
   * @return The current looping mode
   */
  LoopingMode GetLoopingMode() const;

  /**
   * @brief Connects to this signal to be notified when an Animation's animations have finished.
   *
   * @SINCE_1_0.0
   * @return A signal object to connect with
   */
  AnimationSignalType& FinishedSignal();

  /**
   * @brief Animates a property value by a relative amount.
   *
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @SINCE_1_0.0
   * @param[in] target The target object/property to animate
   * @param[in] relativeValue The property value will change by this amount
   */
  void AnimateBy(Property target, Property::Value relativeValue);

  /**
   * @brief Animates a property value by a relative amount.
   *
   * The effect will start & end when the animation begins & ends.
   * @SINCE_1_0.0
   * @param[in] target The target object/property to animate
   * @param[in] relativeValue The property value will change by this amount
   * @param[in] alpha The alpha function to apply
   */
  void AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha);

  /**
   * @brief Animates a property value by a relative amount.
   *
   * The default alpha function will be used.
   * @SINCE_1_0.0
   * @param[in] target The target object/property to animate
   * @param[in] relativeValue The property value will increase/decrease by this amount
   * @param[in] period The effect will occur during this time period
   */
  void AnimateBy(Property target, Property::Value relativeValue, TimePeriod period);

  /**
   * @brief Animates a property value by a relative amount.
   *
   * @SINCE_1_0.0
   * @param[in] target The target object/property to animate
   * @param[in] relativeValue The property value will increase/decrease by this amount
   * @param[in] alpha The alpha function to apply
   * @param[in] period The effect will occur during this time period
   */
  void AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha, TimePeriod period);

  /**
   * @brief Animates a property to a destination value.
   *
   * The default alpha function will be used.
   * The effect will start & end when the animation begins & ends.
   * @SINCE_1_0.0
   * @param[in] target The target object/property to animate
   * @param[in] destinationValue The destination value
   */
  void AnimateTo(Property target, Property::Value destinationValue);

  /**
   * @brief Animates a property to a destination value.
   *
   * The effect will start & end when the animation begins & ends.
   * @SINCE_1_0.0
   * @param[in] target The target object/property to animate
   * @param[in] destinationValue The destination value
   * @param[in] alpha The alpha function to apply
   */
  void AnimateTo(Property target, Property::Value destinationValue, AlphaFunction alpha);

  /**
   * @brief Animates a property to a destination value.
   *
   * The default alpha function will be used.
   * @SINCE_1_0.0
   * @param[in] target The target object/property to animate
   * @param[in] destinationValue The destination value
   * @param[in] period The effect will occur during this time period
   */
  void AnimateTo(Property target, Property::Value destinationValue, TimePeriod period);

  /**
   * @brief Animates a property to a destination value.
   *
   * @SINCE_1_0.0
   * @param[in] target The target object/property to animate
   * @param[in] destinationValue The destination value
   * @param[in] alpha The alpha function to apply
   * @param[in] period The effect will occur during this time period
   */
  void AnimateTo(Property target, Property::Value destinationValue, AlphaFunction alpha, TimePeriod period);

   /**
   * @brief Animates a property between keyframes.
   *
   * @SINCE_1_0.0
   * @param[in] target The target object property to animate
   * @param[in] keyFrames The set of time/value pairs between which to animate
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames);

  /**
   * @brief Animates a property between keyframes.
   *
   * @SINCE_1_0.0
   * @param[in] target The target object property to animate
   * @param[in] keyFrames The set of time/value pairs between which to animate
   * @param[in] interpolation The method used to interpolate between values
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, Interpolation interpolation);

  /**
   * @brief Animates a property between keyframes.
   *
   * @SINCE_1_0.0
   * @param[in] target The target object property to animate
   * @param[in] keyFrames The set of time/value pairs between which to animate
   * @param[in] alpha The alpha function to apply
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha);

  /**
   * @brief Animates a property between keyframes.
   *
   * @SINCE_1_0.0
   * @param[in] target The target object property to animate
   * @param[in] keyFrames The set of time/value pairs between which to animate
   * @param[in] alpha The alpha function to apply
   * @param[in] interpolation The method used to interpolate between values
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, Interpolation interpolation);

  /**
   * @brief Animates a property between keyframes.
   *
   * @SINCE_1_0.0
   * @param[in] target The target object property to animate
   * @param[in] keyFrames The set of time/value pairs between which to animate
   * @param[in] period The effect will occur during this time period
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, TimePeriod period);

  /**
   * @brief Animates a property between keyframes.
   *
   * @SINCE_1_0.0
   * @param[in] target The target object property to animate
   * @param[in] keyFrames The set of time/value pairs between which to animate
   * @param[in] period The effect will occur during this time period
   * @param[in] interpolation The method used to interpolate between values
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, TimePeriod period, Interpolation interpolation);

  /**
   * @brief Animates a property between keyframes.
   *
   * @SINCE_1_0.0
   * @param[in] target The target object property to animate
   * @param[in] keyFrames The set of time/value pairs between which to animate
   * @param[in] alpha The alpha function to apply
   * @param[in] period The effect will occur during this time period
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period);

  /**
   * @brief Animates a property between keyframes.
   *
   * @SINCE_1_0.0
   * @param[in] target The target object property to animate
   * @param[in] keyFrames The set of time/value pairs between which to animate
   * @param[in] alpha The alpha function to apply to the overall progress
   * @param[in] period The effect will occur during this time period
   * @param[in] interpolation The method used to interpolate between values
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period, Interpolation interpolation);


  // Actor-specific convenience methods

  /**
   * @brief Animates an actor's position and orientation through a predefined path.
   *
   * The actor will rotate to orient the supplied
   * forward vector with the path's tangent. If forward is the zero vector then no rotation will happen.
   *
   * @SINCE_1_0.0
   * @param[in] actor The actor to animate
   * @param[in] path The path. It defines position and orientation
   * @param[in] forward The vector (in local space coordinate system) that will be oriented with the path's tangent direction
   */
  void Animate( Actor actor, Path path, const Vector3& forward );

  /**
   * @brief Animates an actor's position and orientation through a predefined path.
   *
   * The actor will rotate to orient the supplied
   * forward vector with the path's tangent. If forward is the zero vector then no rotation will happen.
   *
   * @SINCE_1_0.0
   * @param[in] actor The actor to animate
   * @param[in] path The path. It defines position and orientation
   * @param[in] forward The vector (in local space coordinate system) that will be oriented with the path's tangent direction
   * @param[in] alpha The alpha function to apply
   */
  void Animate( Actor actor, Path path, const Vector3& forward, AlphaFunction alpha );

  /**
   * @brief Animates an actor's position and orientation through a predefined path.
   *
   * The actor will rotate to orient the supplied
   * forward vector with the path's tangent. If forward is the zero vector then no rotation will happen.
   *
   * @SINCE_1_0.0
   * @param[in] actor The actor to animate
   * @param[in] path The path. It defines position and orientation
   * @param[in] forward The vector (in local space coordinate system) that will be oriented with the path's tangent direction
   * @param[in] period The effect will occur during this time period
   */
  void Animate( Actor actor, Path path, const Vector3& forward, TimePeriod period );

  /**
   * @brief Animates an actor's position and orientation through a predefined path.
   *
   * The actor will rotate to orient the supplied
   * forward vector with the path's tangent. If forward is the zero vector then no rotation will happen.
   *
   * @SINCE_1_0.0
   * @param[in] actor The actor to animate
   * @param[in] path The path. It defines position and orientation
   * @param[in] forward The vector (in local space coordinate system) that will be oriented with the path's tangent direction
   * @param[in] alpha The alpha function to apply
   * @param[in] period The effect will occur during this time period
   */
  void Animate( Actor actor, Path path, const Vector3& forward, AlphaFunction alpha, TimePeriod period);

  /**
   * @brief Shows an actor during the animation.
   *
   * @SINCE_1_0.0
   * @param[in] actor The actor to animate
   * @param[in] delaySeconds The initial delay from the start of the animation
   */
  void Show(Actor actor, float delaySeconds);

  /**
   * @brief Hides an actor during the animation.
   *
   * @SINCE_1_0.0
   * @param[in] actor The actor to animate
   * @param[in] delaySeconds The initial delay from the start of the animation
   */
  void Hide(Actor actor, float delaySeconds);

public: // Not intended for use by Application developers

  /// @cond internal
  /**
   * @brief This constructor is used by Animation::New() methods.
   * @SINCE_1_0.0
   * @param[in] animation A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Animation(Internal::Animation* animation);
  /// @endcond

};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_ANIMATION_H
