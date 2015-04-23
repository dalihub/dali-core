#ifndef __DALI_ANIMATION_H__
#define __DALI_ANIMATION_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/animation/path.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

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
 */
class DALI_IMPORT_API Animation : public BaseHandle
{
public:

  typedef Signal< void (Animation&) > AnimationSignalType; ///< Animation finished signal type

  typedef Any AnyFunction; ///< Interpolation function

  /**
   * @brief What to do when the animation ends, is stopped or is destroyed
   */
  enum EndAction
  {
    Bake,     ///< When the animation ends, the animated property values are saved.
    Discard,  ///< When the animation ends, the animated property values are forgotten.
    BakeFinal ///< If the animation is stopped, the animated property values are saved as if the animation had run to completion, otherwise behaves like Bake.
  };

  /**
   * @brief What interpolation method to use on key-frame animations
   */
  enum Interpolation
  {
    Linear,   ///< Values in between key frames are interpolated using a linear polynomial. (Default)
    Cubic     ///< Values in between key frames are interpolated using a cubic polynomial.
  };

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
   * If any of the animated property owners are disconnected from the stage while the animation is being played, then this action is performed.
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
  AnimationSignalType& FinishedSignal();

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
   * @param [in] target The target object + property to animate
   * @param [in] keyFrames The set of time / value pairs between which to animate.
   * @param [in] interpolation The method used to interpolate between values.
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, Interpolation interpolation);

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
   * @param [in] target The target object + property to animate
   * @param [in] keyFrames The set of time / value pairs between which to animate.
   * @param [in] alpha The alpha function to apply.
   * @param [in] interpolation The method used to interpolate between values.
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, Interpolation interpolation);

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
   * @param [in] target The target object + property to animate
   * @param [in] keyFrames The set of time / value pairs between which to animate.
   * @param [in] period The effect will occur duing this time period.
   * @param [in] interpolation The method used to interpolate between values.
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, TimePeriod period, Interpolation interpolation);

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
   * @brief Animate a property between keyframes.
   *
   * @param [in] target The target object + property to animate
   * @param [in] keyFrames The set of time / value pairs between which to animate.
   * @param [in] alpha The alpha function to apply to the overall progress.
   * @param [in] period The effect will occur duing this time period.
   * @param [in] interpolation The method used to interpolate between values.
   */
  void AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period, Interpolation interpolation);


  // Actor-specific convenience methods

  /**
   * @brief Animate an actor's position and orientation through a predefined path. The actor will rotate to orient the supplied
   * forward vector with the path's tangent. If forward is the zero vector then no rotation will happen.
   *
   * @param[in] actor The actor to animate
   * @param[in] path The path. It defines position and orientation
   * @param[in] forward The vector (in local space coordinate system) that will be oriented with the path's tangent direction
   */
  void Animate( Actor actor, Path path, const Vector3& forward );

  /**
   * @brief Animate an actor's position and orientation through a predefined path. The actor will rotate to orient the supplied
   * forward vector with the path's tangent. If forward is the zero vector then no rotation will happen.
   *
   * @param[in] actor The actor to animate
   * @param[in] path The path. It defines position and orientation
   * @param[in] forward The vector (in local space coordinate system) that will be oriented with the path's tangent direction
   * @param [in] alpha The alpha function to apply.
   */
  void Animate( Actor actor, Path path, const Vector3& forward, AlphaFunction alpha );

  /**
   * @brief Animate an actor's position and orientation through a predefined path. The actor will rotate to orient the supplied
   * forward vector with the path's tangent. If forward is the zero vector then no rotation will happen.
   *
   * @param[in] actor The actor to animate
   * @param[in] path The path. It defines position and orientation
   * @param[in] forward The vector (in local space coordinate system) that will be oriented with the path's tangent direction
   * @param [in] period The effect will occur during this time period.
   */
  void Animate( Actor actor, Path path, const Vector3& forward, TimePeriod period );

  /**
   * @brief Animate an actor's position and orientation through a predefined path. The actor will rotate to orient the supplied
   * forward vector with the path's tangent. If forward is the zero vector then no rotation will happen.
   *
   * @param[in] actor The actor to animate
   * @param[in] path The path. It defines position and orientation
   * @param[in] forward The vector (in local space coordinate system) that will be oriented with the path's tangent direction
   * @param [in] alpha The alpha function to apply.
   * @param [in] period The effect will occur during this time period.
   */
  void Animate( Actor actor, Path path, const Vector3& forward, AlphaFunction alpha, TimePeriod period);

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

public: // Not intended for use by Application developers

  /**
   * @brief This constructor is used by Dali New() methods
   * @param [in] animation A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Animation(Internal::Animation* animation);

};

} // namespace Dali

#endif // __DALI_ANIMATION_H__
