#ifndef __DALI_INTERNAL_SCENE_GRAPH_ANIMATOR_H__
#define __DALI_INTERNAL_SCENE_GRAPH_ANIMATOR_H__

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
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/event/animation/key-frames-impl.h>
#include <dali/internal/event/animation/path-impl.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/radian.h>
#include <dali/internal/update/animation/property-accessor.h>


namespace Dali
{

namespace Internal
{

typedef Dali::Animation::Interpolation Interpolation;

struct AnimatorFunctionBase;

namespace SceneGraph
{

class AnimatorBase;

typedef OwnerContainer< AnimatorBase* > AnimatorContainer;

typedef AnimatorContainer::Iterator AnimatorIter;
typedef AnimatorContainer::ConstIterator AnimatorConstIter;

/**
 * An abstract base class for Animators, which can be added to scene graph animations.
 * Each animator changes a single property of an object in the scene graph.
 */
class AnimatorBase
{
public:

  typedef float (*AlphaFunc)(float progress); ///< Definition of an alpha function

  /**
   * Constructor.
   */
  AnimatorBase()
  : mDurationSeconds(1.0f),
    mInitialDelaySeconds(0.0f),
    mAlphaFunction(AlphaFunction::DEFAULT),
    mDisconnectAction(Dali::Animation::BakeFinal),
    mActive(false),
    mEnabled(true),
    mConnectedToSceneGraph(false)
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatorBase()
  {
  }

  /**
   * Called when Animator is added to the scene-graph in update-thread.
   */
  virtual void ConnectToSceneGraph() = 0;

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
  float GetDuration()
  {
    return mDurationSeconds;
  }

  /**
   * Set the delay before the animator should take effect.
   * The default is zero i.e. no delay.
   * @param [in] seconds The delay in seconds.
   */
  void SetInitialDelay(float seconds)
  {
    mInitialDelaySeconds = seconds;
  }

  /**
   * Retrieve the initial delay of the animator.
   * @return The delay in seconds.
   */
  float GetInitialDelay()
  {
    return mInitialDelaySeconds;
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

  /*
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
        while( fabs( progress - currentX ) > tolerance )
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
    mActive = active;
  }

  /**
   * Retrieve whether the animator has been set to active or not.
   * @return The active state.
   */
  bool GetActive() const
  {
    return mActive;
  }

  /*
   * Retrive wheter the animator's target object is valid and on the stage.
   * @return The enabled state.
   */
  bool IsEnabled() const
  {
    return mEnabled;
  }
  /**
   * Returns wheter the target object of the animator is still valid
   * or has been destroyed.
   * @return True if animator is orphan, false otherwise   *
   * @note The SceneGraph::Animation will delete any orphan animator in its Update method.
   */
  virtual bool Orphan() = 0;

  /**
   * Update the scene object attached to the animator.
   * @param[in] bufferIndex The buffer to animate.
   * @param[in] progress A value from 0 to 1, where 0 is the start of the animation, and 1 is the end point.
   * @param[in] bake Bake.
   */
  virtual void Update(BufferIndex bufferIndex, float progress, bool bake) = 0;

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

  float mDurationSeconds;
  float mInitialDelaySeconds;

  AlphaFunction mAlphaFunction;

  Dali::Animation::EndAction mDisconnectAction;     ///< EndAction to apply when target object gets disconnected from the stage.
  bool mActive:1;                                   ///< Animator is "active" while it's running.
  bool mEnabled:1;                                  ///< Animator is "enabled" while its target object is valid and on the stage.
  bool mConnectedToSceneGraph:1;                    ///< True if ConnectToSceneGraph() has been called in update-thread.
};

/**
 * An animator for a specific property type PropertyType.
 */
template < typename PropertyType, typename PropertyAccessorType >
class Animator : public AnimatorBase, public PropertyOwner::Observer
{
public:

  /**
   * Construct a new property animator.
   * @param[in] property The animatable property; only valid while the Animator is attached.
   * @param[in] animatorFunction The function used to animate the property.
   * @param[in] alphaFunction The alpha function to apply.
   * @param[in] timePeriod The time period of this animation.
   * @return A newly allocated animator.
   */
  static AnimatorBase* New( const PropertyOwner& propertyOwner,
                            const PropertyBase& property,
                            AnimatorFunctionBase* animatorFunction,
                            AlphaFunction alphaFunction,
                            const TimePeriod& timePeriod )
  {
    typedef Animator< PropertyType, PropertyAccessorType > AnimatorType;

    // The property was const in the actor-thread, but animators are used in the scene-graph thread.
    AnimatorType* animator = new AnimatorType( const_cast<PropertyOwner*>( &propertyOwner ),
                                               const_cast<PropertyBase*>( &property ),
                                               animatorFunction );

    animator->SetAlphaFunction( alphaFunction );
    animator->SetInitialDelay( timePeriod.delaySeconds );
    animator->SetDuration( timePeriod.durationSeconds );

    return animator;
  }

  /**
   * Virtual destructor.
   */
  virtual ~Animator()
  {
    if (mPropertyOwner && mConnectedToSceneGraph)
    {
      mPropertyOwner->RemoveObserver(*this);
    }

    delete mAnimatorFunction;
  }

  /**
   * Called when Animator is added to the scene-graph in update-thread.
   */
  virtual void ConnectToSceneGraph()
  {
    mConnectedToSceneGraph = true;
    mPropertyOwner->AddObserver(*this);
  }

  /**
   * Called when mPropertyOwner is connected to the scene graph.
   */
  virtual void PropertyOwnerConnected( PropertyOwner& owner )
  {
    mEnabled = true;
  }

  /**
   * Called when mPropertyOwner is disconnected from the scene graph.
   */
  virtual void PropertyOwnerDisconnected( BufferIndex bufferIndex, PropertyOwner& owner )
  {
    // If we are active, then bake the value if required
    if ( mActive && mDisconnectAction != Dali::Animation::Discard )
    {
      // Bake to target-value if BakeFinal, otherwise bake current value
      Update( bufferIndex, ( mDisconnectAction == Dali::Animation::Bake ? mCurrentProgress : 1.0f ), true );
    }

    mActive = false;
    mEnabled = false;
  }

  /**
   * Called shortly before mPropertyOwner is destroyed
   */
  virtual void PropertyOwnerDestroyed( PropertyOwner& owner )
  {
    mPropertyOwner = NULL;
  }

  /**
   * From AnimatorBase.
   */
  virtual void Update( BufferIndex bufferIndex, float progress, bool bake )
  {
    float alpha = ApplyAlphaFunction(progress);

    const PropertyType& current = mPropertyAccessor.Get( bufferIndex );

    const PropertyType result = (*mAnimatorFunction)( alpha, current );
    if ( bake )
    {
      mPropertyAccessor.Bake( bufferIndex, result );
    }
    else
    {
      mPropertyAccessor.Set( bufferIndex, result );
    }

    mCurrentProgress = progress;
  }

  /**
   * From AnimatorBase.
   */
  virtual bool Orphan()
  {
    return (mPropertyOwner == NULL);
  }

private:

  /**
   * Private constructor; see also Animator::New().
   */
  Animator( PropertyOwner* propertyOwner,
            PropertyBase* property,
            AnimatorFunctionBase* animatorFunction )
  : mPropertyOwner( propertyOwner ),
    mPropertyAccessor( property ),
    mAnimatorFunction( animatorFunction ),
    mCurrentProgress( 0.0f )
  {
    // WARNING - this object is created in the event-thread
    // The scene-graph mPropertyOwner object cannot be observed here
  }

  // Undefined
  Animator( const Animator& );

  // Undefined
  Animator& operator=( const Animator& );

protected:

  PropertyOwner* mPropertyOwner;
  PropertyAccessorType mPropertyAccessor;

  AnimatorFunctionBase* mAnimatorFunction;
  float mCurrentProgress;
};



/**
 * An animator for a specific property type PropertyType.
 */
template <typename T, typename PropertyAccessorType>
class AnimatorTransformProperty : public AnimatorBase, public PropertyOwner::Observer
{
public:

  /**
   * Construct a new property animator.
   * @param[in] property The animatable property; only valid while the Animator is attached.
   * @param[in] animatorFunction The function used to animate the property.
   * @param[in] alphaFunction The alpha function to apply.
   * @param[in] timePeriod The time period of this animation.
   * @return A newly allocated animator.
   */
  static AnimatorBase* New( const PropertyOwner& propertyOwner,
                            const PropertyBase& property,
                            AnimatorFunctionBase* animatorFunction,
                            AlphaFunction alphaFunction,
                            const TimePeriod& timePeriod )
  {

    // The property was const in the actor-thread, but animators are used in the scene-graph thread.
    AnimatorTransformProperty* animator = new AnimatorTransformProperty( const_cast<PropertyOwner*>( &propertyOwner ),
                                               const_cast<PropertyBase*>( &property ),
                                               animatorFunction );

    animator->SetAlphaFunction( alphaFunction );
    animator->SetInitialDelay( timePeriod.delaySeconds );
    animator->SetDuration( timePeriod.durationSeconds );

    return animator;
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatorTransformProperty()
  {
    if (mPropertyOwner && mConnectedToSceneGraph)
    {
      mPropertyOwner->RemoveObserver(*this);
    }

    delete mAnimatorFunction;
  }

  /**
   * Called when Animator is added to the scene-graph in update-thread.
   */
  virtual void ConnectToSceneGraph()
  {
    mConnectedToSceneGraph = true;
    mPropertyOwner->AddObserver(*this);
  }

  /**
   * Called when mPropertyOwner is connected to the scene graph.
   */
  virtual void PropertyOwnerConnected( PropertyOwner& owner )
  {
    mEnabled = true;
  }

  /**
   * Called when mPropertyOwner is disconnected from the scene graph.
   */
  virtual void PropertyOwnerDisconnected( BufferIndex bufferIndex, PropertyOwner& owner )
  {
    // If we are active, then bake the value if required
    if ( mActive && mDisconnectAction != Dali::Animation::Discard )
    {
      // Bake to target-value if BakeFinal, otherwise bake current value
      Update( bufferIndex, ( mDisconnectAction == Dali::Animation::Bake ? mCurrentProgress : 1.0f ), true );
    }

    mActive = false;
    mEnabled = false;
  }

  /**
   * Called shortly before mPropertyOwner is destroyed
   */
  virtual void PropertyOwnerDestroyed( PropertyOwner& owner )
  {
    mPropertyOwner = NULL;
  }

  /**
   * From AnimatorBase.
   */
  virtual void Update( BufferIndex bufferIndex, float progress, bool bake )
  {
    float alpha = ApplyAlphaFunction(progress);

    const T& current = mPropertyAccessor.Get( bufferIndex );

    const T result = (*mAnimatorFunction)( alpha, current );


    if ( bake )
    {
      mPropertyAccessor.Bake( bufferIndex, result );
    }
    else
    {
      mPropertyAccessor.Set( bufferIndex, result );
    }

    mCurrentProgress = progress;
  }

  /**
   * From AnimatorBase.
   */
  virtual bool Orphan()
  {
    return (mPropertyOwner == NULL);
  }

private:

  /**
   * Private constructor; see also Animator::New().
   */
  AnimatorTransformProperty( PropertyOwner* propertyOwner,
            PropertyBase* property,
            AnimatorFunctionBase* animatorFunction )
  : mPropertyOwner( propertyOwner ),
    mPropertyAccessor( property ),
    mAnimatorFunction( animatorFunction ),
    mCurrentProgress( 0.0f )
  {
    // WARNING - this object is created in the event-thread
    // The scene-graph mPropertyOwner object cannot be observed here
  }

  // Undefined
  AnimatorTransformProperty( const AnimatorTransformProperty& );

  // Undefined
  AnimatorTransformProperty& operator=( const AnimatorTransformProperty& );

protected:

  PropertyOwner* mPropertyOwner;
  PropertyAccessorType mPropertyAccessor;

  AnimatorFunctionBase* mAnimatorFunction;
  float mCurrentProgress;
};

} // namespace SceneGraph

/*
 * AnimatorFunction base class.
 * All update functions must inherit from AnimatorFunctionBase and overload the appropiate "()" operator
 */
struct AnimatorFunctionBase
{
  /**
   * Constructor
   */
  AnimatorFunctionBase(){}

  /*
   * Virtual destructor (Intended as base class)
   */
  virtual ~AnimatorFunctionBase(){}

  ///Stub "()" operators.
  virtual bool operator()(float progress, const bool& property)
  {
    return property;
  }

  virtual float operator()(float progress, const int& property)
  {
    return property;
  }

  virtual float operator()(float progress, const unsigned int& property)
  {
    return property;
  }

  virtual float operator()(float progress, const float& property)
  {
    return property;
  }

  virtual Vector2 operator()(float progress, const Vector2& property)
  {
    return property;
  }

  virtual Vector3 operator()(float progress, const Vector3& property)
  {
    return property;
  }

  virtual Vector4 operator()(float progress, const Vector4& property)
  {
    return property;
  }

  virtual Quaternion operator()(float progress, const Quaternion& property)
  {
    return property;
  }
};

// Update functions

struct AnimateByInteger : public AnimatorFunctionBase
{
  AnimateByInteger(const int& relativeValue)
  : mRelative(relativeValue)
  {
  }

  float operator()(float alpha, const int& property)
  {
    return int(property + mRelative * alpha + 0.5f );
  }

  int mRelative;
};

struct AnimateToInteger : public AnimatorFunctionBase
{
  AnimateToInteger(const int& targetValue)
  : mTarget(targetValue)
  {
  }

  float operator()(float alpha, const int& property)
  {
    return int(property + ((mTarget - property) * alpha) + 0.5f);
  }

  int mTarget;
};

struct AnimateByFloat : public AnimatorFunctionBase
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

struct AnimateToFloat : public AnimatorFunctionBase
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

struct AnimateByVector2 : public AnimatorFunctionBase
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

struct AnimateToVector2 : public AnimatorFunctionBase
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

struct AnimateByVector3 : public AnimatorFunctionBase
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

struct AnimateToVector3 : public AnimatorFunctionBase
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

struct AnimateByVector4 : public AnimatorFunctionBase
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

struct AnimateToVector4 : public AnimatorFunctionBase
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

struct AnimateByOpacity : public AnimatorFunctionBase
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

struct AnimateToOpacity : public AnimatorFunctionBase
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

struct AnimateByBoolean : public AnimatorFunctionBase
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

struct AnimateToBoolean : public AnimatorFunctionBase
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

struct RotateByAngleAxis : public AnimatorFunctionBase
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

struct RotateToQuaternion : public AnimatorFunctionBase
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


struct KeyFrameBooleanFunctor : public AnimatorFunctionBase
{
  KeyFrameBooleanFunctor(KeyFrameBooleanPtr keyFrames)
  : mKeyFrames(keyFrames)
  {
  }

  bool operator()(float progress, const bool& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress, Dali::Animation::Linear);
    }
    return property;
  }

  KeyFrameBooleanPtr mKeyFrames;
};

struct KeyFrameIntegerFunctor : public AnimatorFunctionBase
{
  KeyFrameIntegerFunctor(KeyFrameIntegerPtr keyFrames, Interpolation interpolation)
  : mKeyFrames(keyFrames),mInterpolation(interpolation)
  {
  }

  float operator()(float progress, const int& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress, mInterpolation);
    }
    return property;
  }

  KeyFrameIntegerPtr mKeyFrames;
  Interpolation mInterpolation;
};

struct KeyFrameNumberFunctor : public AnimatorFunctionBase
{
  KeyFrameNumberFunctor(KeyFrameNumberPtr keyFrames, Interpolation interpolation)
  : mKeyFrames(keyFrames),mInterpolation(interpolation)
  {
  }

  float operator()(float progress, const float& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress, mInterpolation);
    }
    return property;
  }

  KeyFrameNumberPtr mKeyFrames;
  Interpolation mInterpolation;
};

struct KeyFrameVector2Functor : public AnimatorFunctionBase
{
  KeyFrameVector2Functor(KeyFrameVector2Ptr keyFrames, Interpolation interpolation)
  : mKeyFrames(keyFrames),mInterpolation(interpolation)
  {
  }

  Vector2 operator()(float progress, const Vector2& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress, mInterpolation);
    }
    return property;
  }

  KeyFrameVector2Ptr mKeyFrames;
  Interpolation mInterpolation;
};


struct KeyFrameVector3Functor : public AnimatorFunctionBase
{
  KeyFrameVector3Functor(KeyFrameVector3Ptr keyFrames, Interpolation interpolation)
  : mKeyFrames(keyFrames),mInterpolation(interpolation)
  {
  }

  Vector3 operator()(float progress, const Vector3& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress, mInterpolation);
    }
    return property;
  }

  KeyFrameVector3Ptr mKeyFrames;
  Interpolation mInterpolation;
};

struct KeyFrameVector4Functor : public AnimatorFunctionBase
{
  KeyFrameVector4Functor(KeyFrameVector4Ptr keyFrames, Interpolation interpolation)
  : mKeyFrames(keyFrames),mInterpolation(interpolation)
  {
  }

  Vector4 operator()(float progress, const Vector4& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress, mInterpolation);
    }
    return property;
  }

  KeyFrameVector4Ptr mKeyFrames;
  Interpolation mInterpolation;
};

struct KeyFrameQuaternionFunctor : public AnimatorFunctionBase
{
  KeyFrameQuaternionFunctor(KeyFrameQuaternionPtr keyFrames)
  : mKeyFrames(keyFrames)
  {
  }

  Quaternion operator()(float progress, const Quaternion& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress, Dali::Animation::Linear);
    }
    return property;
  }

  KeyFrameQuaternionPtr mKeyFrames;
};

struct PathPositionFunctor : public AnimatorFunctionBase
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

struct PathRotationFunctor : public AnimatorFunctionBase
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

#endif // __DALI_INTERNAL_SCENE_GRAPH_ANIMATOR_H__
