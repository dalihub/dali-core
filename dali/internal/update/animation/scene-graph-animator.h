#ifndef DALI_INTERNAL_SCENE_GRAPH_ANIMATOR_H
#define DALI_INTERNAL_SCENE_GRAPH_ANIMATOR_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <cmath>
#include <functional>

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/radian.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/event/animation/key-frames-impl.h>
#include <dali/internal/event/animation/path-impl.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/internal/update/animation/property-accessor.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

using Interpolation = Dali::Animation::Interpolation;


namespace SceneGraph
{

/**
 * An abstract base class for Animators, which can be added to scene graph animations.
 * Each animator changes a single property of an object in the scene graph.
 */
class AnimatorBase : public PropertyOwner::Observer
{
public:

  using AlphaFunc = float (*)(float progress); ///< Definition of an alpha function

  /**
   * Observer to determine when the animator is no longer present
   */
  class LifecycleObserver
  {
  public:
    /**
     * Called shortly before the animator is destroyed.
     */
    virtual void ObjectDestroyed() = 0;

  protected:
    /**
     * Virtual destructor, no deletion through this interface
     */
    virtual ~LifecycleObserver() = default;
  };


  /**
   * Constructor.
   */
  AnimatorBase( PropertyOwner* propertyOwner,
                AlphaFunction alphaFunction,
                const TimePeriod& timePeriod )
  : mLifecycleObserver( nullptr ),
    mPropertyOwner( propertyOwner ),
    mDurationSeconds( timePeriod.durationSeconds ),
    mIntervalDelaySeconds( timePeriod.delaySeconds ),
    mSpeedFactor( 1.0f ),
    mCurrentProgress( 0.f ),
    mAlphaFunction( alphaFunction ),
    mDisconnectAction( Dali::Animation::BAKE_FINAL ),
    mAnimationPlaying( false ),
    mEnabled( true ),
    mConnectedToSceneGraph( false ),
    mAutoReverseEnabled( false )
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatorBase() override
  {
    if (mPropertyOwner && mConnectedToSceneGraph)
    {
      mPropertyOwner->RemoveObserver(*this);
    }
    if( mLifecycleObserver != nullptr )
    {
      mLifecycleObserver->ObjectDestroyed();
    }
  }

  void AddLifecycleObserver( LifecycleObserver& observer )
  {
    mLifecycleObserver = &observer;
  }

  void RemoveLifecycleObserver( LifecycleObserver& observer )
  {
    mLifecycleObserver = nullptr;
  }

private: // From PropertyOwner::Observer

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerConnected( PropertyOwner& owner )
   */
  void PropertyOwnerConnected( PropertyOwner& owner ) final
  {
    mEnabled = true;
  }

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerDisconnected( BufferIndex bufferIndex, PropertyOwner& owner )
   */
  void PropertyOwnerDisconnected( BufferIndex bufferIndex, PropertyOwner& owner ) final
  {
    // If we are active, then bake the value if required
    if ( mAnimationPlaying && mDisconnectAction != Dali::Animation::DISCARD )
    {
      // Bake to target-value if BakeFinal, otherwise bake current value
      Update( bufferIndex, ( mDisconnectAction == Dali::Animation::BAKE ? mCurrentProgress : 1.0f ), true );
    }

    mEnabled = false;
  }

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerDestroyed( PropertyOwner& owner )
   */
  void PropertyOwnerDestroyed( PropertyOwner& owner ) final
  {
    mPropertyOwner = nullptr;
  }

public:
  /**
   * Called when Animator is added to the scene-graph in update-thread.
   */
  void ConnectToSceneGraph()
  {
    mConnectedToSceneGraph = true;
    mPropertyOwner->AddObserver(*this);

    // Enable if the target object is valid and connected to the scene graph.
    mEnabled = mPropertyOwner->IsAnimationPossible();
  }

  /**
   * Set the duration of the animator.
   * @pre durationSeconds must be zero or greater; zero is useful when animating boolean values.
   * @param [in] seconds Duration in seconds.
   */
  void SetDuration(float seconds)
  {
    DALI_ASSERT_DEBUG(seconds >= 0.0f);

    mDurationSeconds = seconds;
  }

  /**
   * Retrieve the duration of the animator.
   * @return The duration in seconds.
   */
  float GetDuration() const
  {
    return mDurationSeconds;
  }

  void SetSpeedFactor( float factor )
  {
    mSpeedFactor = factor;
  }

  void SetLoopCount(int32_t loopCount)
  {
    mLoopCount = loopCount;
  }

  float SetProgress( float progress )
  {
    float value = 0.0f;

    if( mAutoReverseEnabled )
    {
      if( mSpeedFactor > 0.0f )
      {
        value = 1.0f - 2.0f * std::abs( progress - 0.5f );
      }
      // Reverse mode
      else if( mSpeedFactor < 0.0f )
      {
        value = 2.0f * std::abs( progress - 0.5f );
      }
    }
    else
    {
      value = progress;
    }

    return value;
  }

  /**
   * Set the delay before the animator should take effect.
   * The default is zero i.e. no delay.
   * @param [in] seconds The delay in seconds.
   */
  void SetIntervalDelay(float seconds)
  {
    mIntervalDelaySeconds = seconds;
  }

  /**
   * Retrieve the delay before the animator should take effect.
   * @return The delay in seconds.
   */
  float GetIntervalDelay() const
  {
    return mIntervalDelaySeconds;
  }

  /**
   * Set the alpha function for an animator.
   * @param [in] alphaFunc The alpha function to apply to the animation progress.
   */
  void SetAlphaFunction(const AlphaFunction& alphaFunction)
  {
    mAlphaFunction = alphaFunction;
  }

  /**
   * Retrieve the alpha function of an animator.
   * @return The function.
   */
  AlphaFunction GetAlphaFunction() const
  {
    return mAlphaFunction;
  }

  /**
   * Applies the alpha function to the specified progress
   * @param[in] Current progress
   * @return The progress after the alpha function has been aplied
   */
  float ApplyAlphaFunction( float progress ) const
  {
    float result = progress;

    AlphaFunction::Mode alphaFunctionMode( mAlphaFunction.GetMode() );
    if( alphaFunctionMode == AlphaFunction::BUILTIN_FUNCTION )
    {
      switch(mAlphaFunction.GetBuiltinFunction())
      {
        case AlphaFunction::DEFAULT:
        case AlphaFunction::LINEAR:
        {
          break;
        }
        case AlphaFunction::REVERSE:
        {
          result = 1.0f-progress;
          break;
        }
        case AlphaFunction::EASE_IN_SQUARE:
        {
          result = progress * progress;
          break;
        }
        case AlphaFunction::EASE_OUT_SQUARE:
        {
          result = 1.0f - (1.0f-progress) * (1.0f-progress);
          break;
        }
        case AlphaFunction::EASE_IN:
        {
          result = progress * progress * progress;
          break;
        }
        case AlphaFunction::EASE_OUT:
        {
          result = (progress-1.0f) * (progress-1.0f) * (progress-1.0f) + 1.0f;
          break;
        }
        case AlphaFunction::EASE_IN_OUT:
        {
          result = progress*progress*(3.0f-2.0f*progress);
          break;
        }
        case AlphaFunction::EASE_IN_SINE:
        {
          result = -1.0f * cosf(progress * Math::PI_2) + 1.0f;
          break;
        }
        case AlphaFunction::EASE_OUT_SINE:
        {
          result = sinf(progress * Math::PI_2);
          break;
        }
        case AlphaFunction::EASE_IN_OUT_SINE:
        {
          result = -0.5f * (cosf(Math::PI * progress) - 1.0f);
          break;
        }
        case AlphaFunction::BOUNCE:
        {
          result = sinf(progress * Math::PI);
          break;
        }
        case AlphaFunction::SIN:
        {
          result = 0.5f - cosf(progress * 2.0f * Math::PI) * 0.5f;
          break;
        }
        case AlphaFunction::EASE_OUT_BACK:
        {
          const float sqrt2 = 1.70158f;
          progress -= 1.0f;
          result = 1.0f + progress * progress * ( ( sqrt2 + 1.0f ) * progress + sqrt2 );
          break;
        }
        case AlphaFunction::COUNT:
        {
          break;
        }
      }
    }
    else if(  alphaFunctionMode == AlphaFunction::CUSTOM_FUNCTION )
    {
      AlphaFunctionPrototype customFunction = mAlphaFunction.GetCustomFunction();
      if( customFunction )
      {
        result = customFunction(progress);
      }
    }
    else
    {
      //If progress is very close to 0 or very close to 1 we don't need to evaluate the curve as the result will
      //be almost 0 or almost 1 respectively
      if( ( progress > Math::MACHINE_EPSILON_1 ) && ((1.0f - progress) > Math::MACHINE_EPSILON_1) )
      {
        Dali::Vector4 controlPoints = mAlphaFunction.GetBezierControlPoints();

        static const float tolerance = 0.001f;  //10 iteration max

        //Perform a binary search on the curve
        float lowerBound(0.0f);
        float upperBound(1.0f);
        float currentT(0.5f);
        float currentX = EvaluateCubicBezier( controlPoints.x, controlPoints.z, currentT);
        while( fabsf( progress - currentX ) > tolerance )
        {
          if( progress > currentX )
          {
            lowerBound = currentT;
          }
          else
          {
            upperBound = currentT;
          }
          currentT = (upperBound+lowerBound)*0.5f;
          currentX = EvaluateCubicBezier( controlPoints.x, controlPoints.z, currentT);
        }
        result = EvaluateCubicBezier( controlPoints.y, controlPoints.w, currentT);
      }
    }

    return result;
  }

  /**
   * Whether to bake the animation if attached property owner is disconnected.
   * Property is only baked if the animator is active.
   * @param [in] action The disconnect action.
   */
  void SetDisconnectAction( Dali::Animation::EndAction action )
  {
    mDisconnectAction = action;
  }

  /**
   * Retrieve the disconnect action of an animator.
   * @return The disconnect action.
   */
  Dali::Animation::EndAction GetDisconnectAction() const
  {
    return mDisconnectAction;
  }

  /**
   * Whether the animator is active or not.
   * @param [in] active The new active state.
   * @post When the animator becomes active, it applies the disconnect-action if the property owner is then disconnected.
   * @note When the property owner is disconnected, the active state is set to false.
   */
  void SetActive( bool active )
  {
    mAnimationPlaying = active;
  }

  /**
   * Whether the animator's target object is valid and on the stage.
   * @return The enabled state.
   */
  bool IsEnabled() const
  {
    return mEnabled;
  }

  /**
   * @brief Sets the looping mode.
   * @param[in] loopingMode True when the looping mode is AUTO_REVERSE
   */
  void SetLoopingMode( bool loopingMode )
  {
    mAutoReverseEnabled = loopingMode;
  }

  /**
   * Returns wheter the target object of the animator is still valid
   * or has been destroyed.
   * @return True if animator is orphan, false otherwise   *
   * @note The SceneGraph::Animation will delete any orphan animator in its Update method.
   */
  bool Orphan()
  {
    return (mPropertyOwner == nullptr);
  }

  /**
   * Update the scene object attached to the animator.
   * @param[in] bufferIndex The buffer to animate.
   * @param[in] progress A value from 0 to 1, where 0 is the start of the animation, and 1 is the end point.
   * @param[in] bake Bake.
   */
  void Update( BufferIndex bufferIndex, float progress, bool bake )
  {
    if( mLoopCount >= 0 )
    {
      // Update the progress value
      progress = SetProgress( progress );
    }

    if( mPropertyOwner )
    {
      mPropertyOwner->SetUpdated( true );
    }

    float alpha = ApplyAlphaFunction( progress );

    // PropertyType specific part
    DoUpdate( bufferIndex, bake, alpha );

    mCurrentProgress = progress;
  }

  /**
   * Type specific part of the animator
   * @param bufferIndex index to use
   * @param bake whether to bake or not
   * @param alpha value from alpha based on progress
   */
  virtual void DoUpdate( BufferIndex bufferIndex, bool bake, float alpha ) = 0;

protected:

  /**
   * Helper function to evaluate a cubic bezier curve assuming first point is at 0.0 and last point is at 1.0
   * @param[in] p0 First control point of the bezier curve
   * @param[in] p1 Second control point of the bezier curve
   * @param[in] t A floating point value between 0.0 and 1.0
   * @return Value of the curve at progress t
   */
  inline float EvaluateCubicBezier( float p0, float p1, float t ) const
  {
    float tSquare = t*t;
    return 3.0f*(1.0f-t)*(1.0f-t)*t*p0 + 3.0f*(1.0f-t)*tSquare*p1 + tSquare*t;
  }

  LifecycleObserver* mLifecycleObserver;
  PropertyOwner* mPropertyOwner;

  float mDurationSeconds;
  float mIntervalDelaySeconds;
  float mSpeedFactor;
  float mCurrentProgress;

  AlphaFunction mAlphaFunction;

  int32_t                    mLoopCount{1};
  Dali::Animation::EndAction mDisconnectAction;     ///< EndAction to apply when target object gets disconnected from the stage.
  bool mAnimationPlaying:1;                         ///< whether disconnect has been applied while it's running.
  bool mEnabled:1;                                  ///< Animator is "enabled" while its target object is valid and on the stage.
  bool mConnectedToSceneGraph:1;                    ///< True if ConnectToSceneGraph() has been called in update-thread.
  bool mAutoReverseEnabled:1;
};

/**
 * An animator for a specific property type PropertyType.
 */
template<typename PropertyType, typename PropertyAccessorType>
class Animator final : public AnimatorBase
{
  using AnimatorFunction = std::function<PropertyType(float, const PropertyType&)>;

  AnimatorFunction mAnimatorFunction;

public:

  /**
   * Construct a new property animator.
   * @param[in] property The animatable property; only valid while the Animator is attached.
   * @param[in] animatorFunction The function used to animate the property.
   * @param[in] alphaFunction The alpha function to apply.
   * @param[in] timePeriod The time period of this animation.
   * @return A newly allocated animator.
   */
  static AnimatorBase* New(const PropertyOwner& propertyOwner,
                           const PropertyBase&  property,
                           AnimatorFunction     animatorFunction,
                           AlphaFunction        alphaFunction,
                           const TimePeriod&    timePeriod)
  {
    // The property was const in the actor-thread, but animators are used in the scene-graph thread.
    return new Animator(const_cast<PropertyOwner*>(&propertyOwner),
                        const_cast<PropertyBase*>(&property),
                        std::move(animatorFunction),
                        alphaFunction,
                        timePeriod);
  }

  /**
   * @copydoc AnimatorBase::DoUpdate( BufferIndex bufferIndex, bool bake, float alpha )
   */
  void DoUpdate( BufferIndex bufferIndex, bool bake, float alpha ) final
  {
    const PropertyType& current = mPropertyAccessor.Get( bufferIndex );

    // need to cast the return value in case property is integer
    const PropertyType result = static_cast<PropertyType>(mAnimatorFunction(alpha, current));

    if ( bake )
    {
      mPropertyAccessor.Bake( bufferIndex, result );
    }
    else
    {
      mPropertyAccessor.Set( bufferIndex, result );
    }
  }

private:

  /**
   * Private constructor; see also Animator::New().
   */
  Animator(PropertyOwner*    propertyOwner,
           PropertyBase*     property,
           AnimatorFunction  animatorFunction,
           AlphaFunction     alphaFunction,
           const TimePeriod& timePeriod)
  : AnimatorBase(propertyOwner, alphaFunction, timePeriod),
    mAnimatorFunction(std::move(animatorFunction)),
    mPropertyAccessor(property)
  {
    // WARNING - this object is created in the event-thread
    // The scene-graph mPropertyOwner object cannot be observed here
  }

  // Undefined
  Animator( const Animator& );

  // Undefined
  Animator& operator=( const Animator& );

protected:

  PropertyAccessorType mPropertyAccessor;

};



/**
 * An animator for a specific property type PropertyType.
 */
template<typename PropertyType, typename PropertyAccessorType>
class AnimatorTransformProperty final : public AnimatorBase
{
  using AnimatorFunction = std::function<PropertyType(float, const PropertyType&)>;

  AnimatorFunction mAnimatorFunction;

public:

  /**
   * Construct a new property animator.
   * @param[in] property The animatable property; only valid while the Animator is attached.
   * @param[in] animatorFunction The function used to animate the property.
   * @param[in] alphaFunction The alpha function to apply.
   * @param[in] timePeriod The time period of this animation.
   * @return A newly allocated animator.
   */
  static AnimatorBase* New(const PropertyOwner& propertyOwner,
                           const PropertyBase&  property,
                           AnimatorFunction     animatorFunction,
                           AlphaFunction        alphaFunction,
                           const TimePeriod&    timePeriod)
  {

    // The property was const in the actor-thread, but animators are used in the scene-graph thread.
    return new AnimatorTransformProperty(const_cast<PropertyOwner*>(&propertyOwner),
                                         const_cast<PropertyBase*>(&property),
                                         std::move(animatorFunction),
                                         alphaFunction,
                                         timePeriod);
  }

  /**
   * @copydoc AnimatorBase::DoUpdate( BufferIndex bufferIndex, bool bake, float alpha )
   */
  void DoUpdate( BufferIndex bufferIndex, bool bake, float alpha ) final
  {
    const PropertyType& current = mPropertyAccessor.Get( bufferIndex );

    // need to cast the return value in case property is integer
    const PropertyType result = static_cast<PropertyType>(mAnimatorFunction(alpha, current));

    if ( bake )
    {
      mPropertyAccessor.Bake( bufferIndex, result );
    }
    else
    {
      mPropertyAccessor.Set( bufferIndex, result );
    }
  }

private:

  /**
   * Private constructor; see also Animator::New().
   */
  AnimatorTransformProperty(PropertyOwner*    propertyOwner,
                            PropertyBase*     property,
                            AnimatorFunction  animatorFunction,
                            AlphaFunction     alphaFunction,
                            const TimePeriod& timePeriod)
  : AnimatorBase(propertyOwner, alphaFunction, timePeriod),
    mAnimatorFunction(std::move(animatorFunction)),
    mPropertyAccessor(property)
  {
    // WARNING - this object is created in the event-thread
    // The scene-graph mPropertyOwner object cannot be observed here
  }

  // Undefined
  AnimatorTransformProperty() = delete;
  AnimatorTransformProperty( const AnimatorTransformProperty& ) = delete;
  AnimatorTransformProperty& operator=( const AnimatorTransformProperty& ) = delete;

protected:

  PropertyAccessorType mPropertyAccessor;

};

} // namespace SceneGraph

// Update functions

struct AnimateByInteger
{
  AnimateByInteger(const int& relativeValue)
  : mRelative(relativeValue)
  {
  }

  float operator()(float alpha, const int32_t& property)
  {
    // integers need to be correctly rounded
    return roundf(static_cast<float>( property ) + static_cast<float>( mRelative ) * alpha );
  }

  int32_t mRelative;
};

struct AnimateToInteger
{
  AnimateToInteger(const int& targetValue)
  : mTarget(targetValue)
  {
  }

  float operator()(float alpha, const int32_t& property)
  {
    // integers need to be correctly rounded
    return roundf(static_cast<float>( property ) + (static_cast<float>(mTarget - property) * alpha) );
  }

  int32_t mTarget;
};

struct AnimateByFloat
{
  AnimateByFloat(const float& relativeValue)
  : mRelative(relativeValue)
  {
  }

  float operator()(float alpha, const float& property)
  {
    return float(property + mRelative * alpha);
  }

  float mRelative;
};

struct AnimateToFloat
{
  AnimateToFloat(const float& targetValue)
  : mTarget(targetValue)
  {
  }

  float operator()(float alpha, const float& property)
  {
    return float(property + ((mTarget - property) * alpha));
  }

  float mTarget;
};

struct AnimateByVector2
{
  AnimateByVector2(const Vector2& relativeValue)
  : mRelative(relativeValue)
  {
  }

  Vector2 operator()(float alpha, const Vector2& property)
  {
    return Vector2(property + mRelative * alpha);
  }

  Vector2 mRelative;
};

struct AnimateToVector2
{
  AnimateToVector2(const Vector2& targetValue)
  : mTarget(targetValue)
  {
  }

  Vector2 operator()(float alpha, const Vector2& property)
  {
    return Vector2(property + ((mTarget - property) * alpha));
  }

  Vector2 mTarget;
};

struct AnimateByVector3
{
  AnimateByVector3(const Vector3& relativeValue)
  : mRelative(relativeValue)
  {
  }

  Vector3 operator()(float alpha, const Vector3& property)
  {
    return Vector3(property + mRelative * alpha);
  }

  Vector3 mRelative;
};

struct AnimateToVector3
{
  AnimateToVector3(const Vector3& targetValue)
  : mTarget(targetValue)
  {
  }

  Vector3 operator()(float alpha, const Vector3& property)
  {
    return Vector3(property + ((mTarget - property) * alpha));
  }

  Vector3 mTarget;
};

struct AnimateByVector4
{
  AnimateByVector4(const Vector4& relativeValue)
  : mRelative(relativeValue)
  {
  }

  Vector4 operator()(float alpha, const Vector4& property)
  {
    return Vector4(property + mRelative * alpha);
  }

  Vector4 mRelative;
};

struct AnimateToVector4
{
  AnimateToVector4(const Vector4& targetValue)
  : mTarget(targetValue)
  {
  }

  Vector4 operator()(float alpha, const Vector4& property)
  {
    return Vector4(property + ((mTarget - property) * alpha));
  }

  Vector4 mTarget;
};

struct AnimateByOpacity
{
  AnimateByOpacity(const float& relativeValue)
  : mRelative(relativeValue)
  {
  }

  Vector4 operator()(float alpha, const Vector4& property)
  {
    Vector4 result(property);
    result.a += mRelative * alpha;

    return result;
  }

  float mRelative;
};

struct AnimateToOpacity
{
  AnimateToOpacity(const float& targetValue)
  : mTarget(targetValue)
  {
  }

  Vector4 operator()(float alpha, const Vector4& property)
  {
    Vector4 result(property);
    result.a = property.a + ((mTarget - property.a) * alpha);

    return result;
  }

  float mTarget;
};

struct AnimateByBoolean
{
  AnimateByBoolean(bool relativeValue)
  : mRelative(relativeValue)
  {
  }

  bool operator()(float alpha, const bool& property)
  {
    // Alpha is not useful here, just keeping to the same template as other update functors
    return bool(alpha >= 1.0f ? (property || mRelative) : property);
  }

  bool mRelative;
};

struct AnimateToBoolean
{
  AnimateToBoolean(bool targetValue)
  : mTarget(targetValue)
  {
  }

  bool operator()(float alpha, const bool& property)
  {
    // Alpha is not useful here, just keeping to the same template as other update functors
    return bool(alpha >= 1.0f ? mTarget : property);
  }

  bool mTarget;
};

struct RotateByAngleAxis
{
  RotateByAngleAxis(const Radian& angleRadians, const Vector3& axis)
  : mAngleRadians( angleRadians ),
    mAxis(axis.x, axis.y, axis.z)
  {
  }

  Quaternion operator()(float alpha, const Quaternion& rotation)
  {
    if (alpha > 0.0f)
    {
      return rotation * Quaternion(mAngleRadians * alpha, mAxis);
    }

    return rotation;
  }

  Radian mAngleRadians;
  Vector3 mAxis;
};

struct RotateToQuaternion
{
  RotateToQuaternion(const Quaternion& targetValue)
  : mTarget(targetValue)
  {
  }

  Quaternion operator()(float alpha, const Quaternion& rotation)
  {
    return Quaternion::Slerp(rotation, mTarget, alpha);
  }

  Quaternion mTarget;
};

struct KeyFrameBooleanFunctor
{
  KeyFrameBooleanFunctor(KeyFrameBoolean keyFrames)
  : mKeyFrames(std::move(keyFrames))
  {
  }

  bool operator()(float progress, const bool& property)
  {
    if(mKeyFrames.IsActive(progress))
    {
      return mKeyFrames.GetValue(progress, Dali::Animation::LINEAR);
    }
    return property;
  }

  KeyFrameBoolean mKeyFrames;
};

struct KeyFrameIntegerFunctor
{
  KeyFrameIntegerFunctor(KeyFrameInteger keyFrames, Interpolation interpolation)
  : mKeyFrames(std::move(keyFrames)),
    mInterpolation(interpolation)
  {
  }

  float operator()(float progress, const int32_t& property)
  {
    if(mKeyFrames.IsActive(progress))
    {
      return static_cast<float>(mKeyFrames.GetValue(progress, mInterpolation));
    }
    return static_cast<float>( property );
  }

  KeyFrameInteger mKeyFrames;
  Interpolation mInterpolation;
};

struct KeyFrameNumberFunctor
{
  KeyFrameNumberFunctor(KeyFrameNumber keyFrames, Interpolation interpolation)
  : mKeyFrames(std::move(keyFrames)),
    mInterpolation(interpolation)
  {
  }

  float operator()(float progress, const float& property)
  {
    if(mKeyFrames.IsActive(progress))
    {
      return mKeyFrames.GetValue(progress, mInterpolation);
    }
    return property;
  }

  KeyFrameNumber mKeyFrames;
  Interpolation mInterpolation;
};

struct KeyFrameVector2Functor
{
  KeyFrameVector2Functor(KeyFrameVector2 keyFrames, Interpolation interpolation)
  : mKeyFrames(std::move(keyFrames)),
    mInterpolation(interpolation)
  {
  }

  Vector2 operator()(float progress, const Vector2& property)
  {
    if(mKeyFrames.IsActive(progress))
    {
      return mKeyFrames.GetValue(progress, mInterpolation);
    }
    return property;
  }

  KeyFrameVector2 mKeyFrames;
  Interpolation mInterpolation;
};

struct KeyFrameVector3Functor
{
  KeyFrameVector3Functor(KeyFrameVector3 keyFrames, Interpolation interpolation)
  : mKeyFrames(std::move(keyFrames)),
    mInterpolation(interpolation)
  {
  }

  Vector3 operator()(float progress, const Vector3& property)
  {
    if(mKeyFrames.IsActive(progress))
    {
      return mKeyFrames.GetValue(progress, mInterpolation);
    }
    return property;
  }

  KeyFrameVector3 mKeyFrames;
  Interpolation mInterpolation;
};

struct KeyFrameVector4Functor
{
  KeyFrameVector4Functor(KeyFrameVector4 keyFrames, Interpolation interpolation)
  : mKeyFrames(std::move(keyFrames)),
    mInterpolation(interpolation)
  {
  }

  Vector4 operator()(float progress, const Vector4& property)
  {
    if(mKeyFrames.IsActive(progress))
    {
      return mKeyFrames.GetValue(progress, mInterpolation);
    }
    return property;
  }

  KeyFrameVector4 mKeyFrames;
  Interpolation mInterpolation;
};

struct KeyFrameQuaternionFunctor
{
  KeyFrameQuaternionFunctor(KeyFrameQuaternion keyFrames)
  : mKeyFrames(std::move(keyFrames))
  {
  }

  Quaternion operator()(float progress, const Quaternion& property)
  {
    if(mKeyFrames.IsActive(progress))
    {
      return mKeyFrames.GetValue(progress, Dali::Animation::LINEAR);
    }
    return property;
  }

  KeyFrameQuaternion mKeyFrames;
};

struct PathPositionFunctor
{
  PathPositionFunctor( PathPtr path )
  : mPath(path)
  {
  }

  Vector3 operator()(float progress, const Vector3& property)
  {
    Vector3 position(property);
    static_cast<void>( mPath->SamplePosition(progress, position) );
    return position;
  }

  PathPtr mPath;
};

struct PathRotationFunctor
{
  PathRotationFunctor( PathPtr path, const Vector3& forward )
  : mPath(path),
    mForward( forward )
  {
    mForward.Normalize();
  }

  Quaternion operator()(float progress, const Quaternion& property)
  {
    Vector3 tangent;
    if( mPath->SampleTangent(progress, tangent) )
    {
      return Quaternion( mForward, tangent );
    }
    else
    {
      return property;
    }
  }

  PathPtr mPath;
  Vector3 mForward;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_ANIMATOR_H
