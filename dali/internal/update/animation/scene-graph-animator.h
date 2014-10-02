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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/internal/common/owner-container.h>
#include <dali/internal/event/animation/key-frames-impl.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/internal/common/observer-pointer.h>
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/radian.h>

namespace Dali
{

namespace Internal
{

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
    mAlphaFunc(AlphaFunctions::Linear),
    mDisconnectAction(Dali::Animation::BakeFinal),
    mActive(false)
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatorBase()
  {
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
  void SetAlphaFunc(AlphaFunc alphaFunc)
  {
    mAlphaFunc = alphaFunc;
  }

  /**
   * Retrieve the alpha function of an animator.
   * @return The function.
   */
  AlphaFunc GetAlphaFunc() const
  {
    return mAlphaFunc;
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
   * @param [in] action The disconnect action.
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

  /**
   * This must be called when the animator is attached to the scene-graph.
   * @pre The animatable scene object must also be attached to the scene-graph.
   * @param[in] propertyOwner The scene-object that owns the animatable property.
   */
  virtual void Attach( PropertyOwner* propertyOwner ) = 0;

  /**
   * Update the scene object attached to the animator.
   * @param[in] bufferIndex The buffer to animate.
   * @param[in] progress A value from 0 to 1, where 0 is the start of the animation, and 1 is the end point.
   * @param[in] bake Bake.
   * @return True if the update was applied, false if the animator has been orphaned.
   */
  virtual bool Update(BufferIndex bufferIndex, float progress, bool bake) = 0;

  /**
   * Query whether the animator is still attached to a scene object.
   * The attachment will be automatically severed, when the object is destroyed.
   * @return True if the animator is attached.
   */
  virtual bool IsAttached() const = 0;

protected:

  float mDurationSeconds;
  float mInitialDelaySeconds;

  AlphaFunc mAlphaFunc;

  Dali::Animation::EndAction mDisconnectAction;
  bool mActive:1;
};

/**
 * An animator for a specific property type PropertyType.
 */
template < typename PropertyType, typename PropertyAccessorType >
class Animator : public AnimatorBase, public PropertyOwner::Observer
{
public:

  typedef boost::function< PropertyType (float, const PropertyType&) > AnimatorFunction;

  /**
   * Construct a new property animator.
   * @param[in] property The animatable property; only valid while the Animator is attached.
   * @param[in] animatorFunction The function used to animate the property.
   * @param[in] alphaFunction The alpha function to apply.
   * @param[in] timePeriod The time period of this animation.
   * @return A newly allocated animator.
   */
  static AnimatorBase* New( const PropertyBase& property,
                            AnimatorFunction animatorFunction,
                            AlphaFunction alphaFunction,
                            const TimePeriod& timePeriod )
  {
    typedef Animator< PropertyType, PropertyAccessorType > AnimatorType;

    // The property was const in the actor-thread, but animators are used in the scene-graph thread.
    AnimatorType* animator = new AnimatorType( const_cast<PropertyBase*>( &property ),
                                               animatorFunction );

    animator->SetAlphaFunc( alphaFunction );
    animator->SetInitialDelay( timePeriod.delaySeconds );
    animator->SetDuration( timePeriod.durationSeconds );

    return animator;
  }

  /**
   * Virtual destructor.
   */
  virtual ~Animator()
  {
    if (mPropertyOwner)
    {
      mPropertyOwner->RemoveObserver(*this);
    }
  }

  /**
   * From AnimatorBase.
   * @param[in] propertyOwner The scene-object that owns the animatable property.
   */
  virtual void Attach( PropertyOwner* propertyOwner )
  {
    mPropertyOwner = propertyOwner;

    if (mPropertyOwner)
    {
      mPropertyOwner->AddObserver(*this);
    }
  }

  /**
   * From AnimatorBase.
   */
  virtual bool Update( BufferIndex bufferIndex, float progress, bool bake )
  {
    // If the object dies, the animator has no effect
    if ( mPropertyOwner )
    {
      float alpha = mAlphaFunc( progress );

      const PropertyType& current = mPropertyAccessor.Get( bufferIndex );

      const PropertyType result = mAnimatorFunction( alpha, current );

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

    return IsAttached(); // return false if orphaned
  }

  /**
   * From AnimatorBase.
   */
  virtual bool IsAttached() const
  {
    return NULL != mPropertyOwner;
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
    mPropertyOwner = NULL;
    mPropertyAccessor.Reset();
  }

  /**
   * Called shortly before mPropertyOwner is destroyed, along with its property.
   */
  virtual void PropertyOwnerDestroyed( PropertyOwner& owner )
  {
    mPropertyOwner = NULL;
    mPropertyAccessor.Reset();
  }

private:

  /**
   * Private constructor; see also Animator::New().
   */
  Animator( PropertyBase* property,
            AnimatorFunction animatorFunction )
  : mPropertyOwner( NULL ),
    mPropertyAccessor( property ),
    mAnimatorFunction( animatorFunction ),
    mCurrentProgress( 0.0f )
  {
  }

  // Undefined
  Animator( const Animator& );

  // Undefined
  Animator& operator=( const Animator& );

protected:

  PropertyOwner* mPropertyOwner;
  PropertyAccessorType mPropertyAccessor;

  AnimatorFunction mAnimatorFunction;
  float mCurrentProgress;
};

} // namespace SceneGraph


// Common Update functions

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

struct AnimateByInteger
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

struct AnimateToInteger
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
  : mAngleRadians(angleRadians),
    mAxis(axis.x, axis.y, axis.z, 0.0f)
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

  float mAngleRadians;
  Vector4 mAxis;
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
  KeyFrameBooleanFunctor(KeyFrameBooleanPtr keyFrames)
  : mKeyFrames(keyFrames)
  {
  }

  bool operator()(float progress, const bool& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress);
    }
    return property;
  }

  KeyFrameBooleanPtr mKeyFrames;
};

struct KeyFrameNumberFunctor
{
  KeyFrameNumberFunctor(KeyFrameNumberPtr keyFrames)
  : mKeyFrames(keyFrames)
  {
  }

  float operator()(float progress, const float& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress);
    }
    return property;
  }

  KeyFrameNumberPtr mKeyFrames;
};

struct KeyFrameIntegerFunctor
{
  KeyFrameIntegerFunctor(KeyFrameIntegerPtr keyFrames)
  : mKeyFrames(keyFrames)
  {
  }

  float operator()(float progress, const int& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress);
    }
    return property;
  }

  KeyFrameIntegerPtr mKeyFrames;
};

struct KeyFrameVector2Functor
{
  KeyFrameVector2Functor(KeyFrameVector2Ptr keyFrames)
  : mKeyFrames(keyFrames)
  {
  }

  Vector2 operator()(float progress, const Vector2& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress);
    }
    return property;
  }

  KeyFrameVector2Ptr mKeyFrames;
};


struct KeyFrameVector3Functor
{
  KeyFrameVector3Functor(KeyFrameVector3Ptr keyFrames)
  : mKeyFrames(keyFrames)
  {
  }

  Vector3 operator()(float progress, const Vector3& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress);
    }
    return property;
  }

  KeyFrameVector3Ptr mKeyFrames;
};

struct KeyFrameVector4Functor
{
  KeyFrameVector4Functor(KeyFrameVector4Ptr keyFrames)
  : mKeyFrames(keyFrames)
  {
  }

  Vector4 operator()(float progress, const Vector4& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress);
    }
    return property;
  }

  KeyFrameVector4Ptr mKeyFrames;
};

struct KeyFrameQuaternionFunctor
{
  KeyFrameQuaternionFunctor(KeyFrameQuaternionPtr keyFrames)
  : mKeyFrames(keyFrames)
  {
  }

  Quaternion operator()(float progress, const Quaternion& property)
  {
    if(mKeyFrames->IsActive(progress))
    {
      return mKeyFrames->GetValue(progress);
    }
    return property;
  }

  KeyFrameQuaternionPtr mKeyFrames;
};




} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_ANIMATOR_H__
