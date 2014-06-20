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

// CLASS HEADER
#include <dali/internal/event/animation/animation-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/radian.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/animation/animator-connector.h>
#include <dali/internal/event/animation/animation-playlist.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/effects/shader-effect-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>

using namespace std;

using Dali::Internal::SceneGraph::UpdateManager;
using Dali::Internal::SceneGraph::AnimatorBase;
using Dali::Internal::SceneGraph::Shader;

namespace Dali
{

namespace Internal
{

static bool SHOW_VALUE = true;
static bool HIDE_VALUE = false;

namespace
{

BaseHandle Create()
{
  return Dali::Animation::New(0.f);
}

TypeRegistration mType( typeid(Dali::Animation), typeid(Dali::BaseHandle), Create );

SignalConnectorType signalConnector1( mType, Dali::Animation::SIGNAL_FINISHED, &Animation::DoConnectSignal );

TypeAction action1( mType, Dali::Animation::ACTION_PLAY, &Animation::DoAction );
TypeAction action2( mType, Dali::Animation::ACTION_STOP, &Animation::DoAction );
TypeAction action3( mType, Dali::Animation::ACTION_PAUSE, &Animation::DoAction );

} // anon namespace


AnimationPtr Animation::New(float durationSeconds)
{
  return New(durationSeconds, Dali::Animation::Bake, Dali::Animation::Bake, Dali::AlphaFunctions::Linear);
}

AnimationPtr Animation::New(float durationSeconds, EndAction endAction, EndAction destroyAction)
{
  return New(durationSeconds, endAction, destroyAction, Dali::AlphaFunctions::Linear);
}

AnimationPtr Animation::New(float durationSeconds, EndAction endAction, EndAction destroyAction, AlphaFunction alpha)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  UpdateManager& updateManager = tls.GetUpdateManager();

  AnimationPlaylist& playlist = Stage::GetCurrent()->GetAnimationPlaylist();

  AnimationPtr progress = new Animation( updateManager, playlist, durationSeconds, endAction, destroyAction, alpha );

  // Second-phase construction
  progress->Initialize();

  return progress;
}

Animation::Animation( UpdateManager& updateManager, AnimationPlaylist& playlist, float durationSeconds, EndAction endAction, EndAction destroyAction, AlphaFunction defaultAlpha )
: mUpdateManager( updateManager ),
  mPlaylist( playlist ),
  mAnimation( NULL ),
  mNotificationCount( 0 ),
  mFinishedCallback( NULL ),
  mFinishedCallbackObject( NULL ),
  mDurationSeconds( durationSeconds ),
  mIsLooping( false ),
  mEndAction( endAction ),
  mDestroyAction( destroyAction ),
  mDefaultAlpha( defaultAlpha )
{
}

void Animation::Initialize()
{
  // Connect to the animation playlist
  mPlaylist.AnimationCreated( *this );

  CreateSceneObject();

  RegisterObject();
}

Animation::~Animation()
{
  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    // Disconnect from the animation playlist
    mPlaylist.AnimationDestroyed( *this );

    DestroySceneObject();

    UnregisterObject();
  }
}

void Animation::CreateSceneObject()
{
  DALI_ASSERT_DEBUG( mAnimation == NULL );

  // Create a new animation, temporarily owned
  SceneGraph::Animation* animation = SceneGraph::Animation::New( mDurationSeconds, mIsLooping, mEndAction, mDestroyAction );

  // Keep a const pointer to the animation.
  mAnimation = animation;

  // Transfer animation ownership to the update manager through a message
  AddAnimationMessage( mUpdateManager, animation );
}

void Animation::DestroySceneObject()
{
  if ( mAnimation != NULL )
  {
    // Remove animation using a message to the update manager
    RemoveAnimationMessage( mUpdateManager, *mAnimation );
    mAnimation = NULL;
  }
}

void Animation::SetDuration(float seconds)
{
  // Cache for public getters
  mDurationSeconds = seconds;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetDurationMessage( mUpdateManager.GetEventToUpdate(), *mAnimation, seconds );
}

float Animation::GetDuration() const
{
  // This is not animatable; the cached value is up-to-date.
  return mDurationSeconds;
}

void Animation::SetLooping(bool looping)
{
  // Cache for public getters
  mIsLooping = looping;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetLoopingMessage( mUpdateManager.GetEventToUpdate(), *mAnimation, looping );
}

bool Animation::IsLooping() const
{
  // This is not animatable; the cached value is up-to-date.
  return mIsLooping;
}

void Animation::SetEndAction(EndAction action)
{
  // Cache for public getters
  mEndAction = action;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetEndActionMessage( mUpdateManager.GetEventToUpdate(), *mAnimation, action );
}

Dali::Animation::EndAction Animation::GetEndAction() const
{
  // This is not animatable; the cached value is up-to-date.
  return mEndAction;
}

void Animation::SetDestroyAction(EndAction action)
{
  // Cache for public getters
  mDestroyAction = action;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetDestroyActionMessage( mUpdateManager.GetEventToUpdate(), *mAnimation, action );
}

Dali::Animation::EndAction Animation::GetDestroyAction() const
{
  // This is not animatable; the cached value is up-to-date.
  return mDestroyAction;
}

void Animation::Play()
{
  // Update the current playlist
  mPlaylist.OnPlay( *this );

  // mAnimation is being used in a separate thread; queue a Play message
  PlayAnimationMessage( mUpdateManager.GetEventToUpdate(), *mAnimation );
}

void Animation::PlayFrom( float progress )
{
  if( progress >= 0.0f && progress <= 1.0f )
  {
    // Update the current playlist
    mPlaylist.OnPlay( *this );

    // mAnimation is being used in a separate thread; queue a Play message
    PlayAnimationFromMessage( mUpdateManager.GetEventToUpdate(), *mAnimation, progress );
  }
}

void Animation::Pause()
{
  // mAnimation is being used in a separate thread; queue a Pause message
  PauseAnimationMessage( mUpdateManager.GetEventToUpdate(), *mAnimation );
}

void Animation::Stop()
{
  // mAnimation is being used in a separate thread; queue a Stop message
  StopAnimationMessage( mUpdateManager, *mAnimation );
}

void Animation::Clear()
{
  DALI_ASSERT_DEBUG(mAnimation);

  // Remove all the connectors
  mConnectors.Clear();

  // Replace the old scene-object with a new one
  DestroySceneObject();
  CreateSceneObject();

  // Reset the notification count, since the new scene-object has never been played
  mNotificationCount = 0;

  // Update the current playlist
  mPlaylist.OnClear( *this );
}

void Animation::AnimateBy(Property& target, Property::Value& relativeValue)
{
  AnimateBy(target, relativeValue, AlphaFunctions::Default, mDurationSeconds);
}

void Animation::AnimateBy(Property& target, Property::Value& relativeValue, AlphaFunction alpha)
{
  AnimateBy(target, relativeValue, alpha, mDurationSeconds);
}

void Animation::AnimateBy(Property& target, Property::Value& relativeValue, TimePeriod period)
{
  AnimateBy(target, relativeValue, AlphaFunctions::Default, period);
}

void Animation::AnimateBy(Property& target, Property::Value& relativeValue, AlphaFunction alpha, TimePeriod period)
{
  ProxyObject& proxy = dynamic_cast<ProxyObject&>( GetImplementation(target.object) );

  switch ( relativeValue.GetType() )
  {
    case Property::BOOLEAN:
    {
      AddAnimatorConnector( AnimatorConnector<bool>::New( proxy,
                                                          target.propertyIndex,
                                                          target.componentIndex,
                                                          AnimateByBoolean(relativeValue.Get<bool>()),
                                                          alpha,
                                                          period ) );
      break;
    }

    case Property::FLOAT:
    {
      AddAnimatorConnector( AnimatorConnector<float>::New( proxy,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           AnimateByFloat(relativeValue.Get<float>()),
                                                           alpha,
                                                           period ) );
      break;
    }

    case Property::VECTOR2:
    {
      AddAnimatorConnector( AnimatorConnector<Vector2>::New( proxy,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             AnimateByVector2(relativeValue.Get<Vector2>()),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Property::VECTOR3:
    {
      AddAnimatorConnector( AnimatorConnector<Vector3>::New( proxy,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             AnimateByVector3(relativeValue.Get<Vector3>()),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Property::VECTOR4:
    {
      AddAnimatorConnector( AnimatorConnector<Vector4>::New( proxy,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             AnimateByVector4(relativeValue.Get<Vector4>()),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Property::ROTATION:
    {
      AngleAxis angleAxis = relativeValue.Get<AngleAxis>();

      AddAnimatorConnector( AnimatorConnector<Quaternion>::New( proxy,
                                                                target.propertyIndex,
                                                                target.componentIndex,
                                                                RotateByAngleAxis(angleAxis.angle, angleAxis.axis),
                                                                alpha,
                                                                period ) );
      break;
    }

    default:
      DALI_ASSERT_ALWAYS( false && "Property type enumeration out of bounds" ); // should never come here
      break;
  }
}

void Animation::AnimateTo(Property& target, Property::Value& destinationValue)
{
  AnimateTo(target, destinationValue, AlphaFunctions::Default, mDurationSeconds);
}

void Animation::AnimateTo(Property& target, Property::Value& destinationValue, AlphaFunction alpha)
{
  AnimateTo(target, destinationValue, alpha, mDurationSeconds);
}

void Animation::AnimateTo(Property& target, Property::Value& destinationValue, TimePeriod period)
{
  AnimateTo(target, destinationValue, AlphaFunctions::Default, period);
}

void Animation::AnimateTo(Property& target, Property::Value& destinationValue, AlphaFunction alpha, TimePeriod period)
{
  ProxyObject& proxy = dynamic_cast<ProxyObject&>( GetImplementation(target.object) );

  AnimateTo( proxy, target.propertyIndex, target.componentIndex, destinationValue, alpha, period );
}

void Animation::AnimateTo(ProxyObject& targetObject, Property::Index targetPropertyIndex, int componentIndex, Property::Value& destinationValue, AlphaFunction alpha, TimePeriod period)
{
  Property::Type type = targetObject.GetPropertyType(targetPropertyIndex);
  if(componentIndex != Property::INVALID_COMPONENT_INDEX)
  {
    if( type == Property::VECTOR2
        || type == Property::VECTOR3
        || type == Property::VECTOR4 )
    {
      type = Property::FLOAT;
    }
  }
  DALI_ASSERT_ALWAYS( type == destinationValue.GetType() && "DestinationValue does not match Target Property type" );

  switch (destinationValue.GetType())
  {
    case Property::BOOLEAN:
    {
      AddAnimatorConnector( AnimatorConnector<bool>::New(targetObject,
                                                         targetPropertyIndex,
                                                         componentIndex,
                                                         AnimateToBoolean(destinationValue.Get<bool>()),
                                                         alpha,
                                                         period) );
      break;
    }

    case Property::FLOAT:
    {
      AddAnimatorConnector( AnimatorConnector<float>::New(targetObject,
                                                          targetPropertyIndex,
                                                          componentIndex,
                                                          AnimateToFloat(destinationValue.Get<float>()),
                                                          alpha,
                                                          period) );
      break;
    }

    case Property::VECTOR2:
    {
      AddAnimatorConnector( AnimatorConnector<Vector2>::New(targetObject,
                                                            targetPropertyIndex,
                                                            componentIndex,
                                                            AnimateToVector2(destinationValue.Get<Vector2>()),
                                                            alpha,
                                                            period) );
      break;
    }

    case Property::VECTOR3:
    {
      if ( Dali::Actor::SIZE == targetPropertyIndex )
      {
        // Test whether this is actually an Actor
        Actor* maybeActor = dynamic_cast<Actor*>( &targetObject );
        if ( maybeActor )
        {
          // Notify the actor that its size is being animated
          maybeActor->OnSizeAnimation( *this, destinationValue.Get<Vector3>() );
        }
      }

      AddAnimatorConnector( AnimatorConnector<Vector3>::New(targetObject,
                                                            targetPropertyIndex,
                                                            componentIndex,
                                                            AnimateToVector3(destinationValue.Get<Vector3>()),
                                                            alpha,
                                                            period) );
      break;
    }

    case Property::VECTOR4:
    {
      AddAnimatorConnector( AnimatorConnector<Vector4>::New(targetObject,
                                                            targetPropertyIndex,
                                                            componentIndex,
                                                            AnimateToVector4(destinationValue.Get<Vector4>()),
                                                            alpha,
                                                            period) );
      break;
    }

    case Property::ROTATION:
    {
      AddAnimatorConnector( AnimatorConnector<Quaternion>::New(targetObject,
                                                               targetPropertyIndex,
                                                               componentIndex,
                                                               RotateToQuaternion(destinationValue.Get<Quaternion>()),
                                                               alpha,
                                                               period) );
      break;
    }

    default:
      DALI_ASSERT_ALWAYS( false && "Property type enumeration out of bounds" ); // should never come here
      break;
  }
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames)
{
  AnimateBetween(target, keyFrames, mDefaultAlpha, mDurationSeconds);
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames, TimePeriod period)
{
  AnimateBetween(target, keyFrames, mDefaultAlpha, period);
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha)
{
  AnimateBetween(target, keyFrames, alpha, mDurationSeconds);
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period)
{
  ProxyObject& proxy = dynamic_cast<ProxyObject&>( GetImplementation(target.object) );

  switch(keyFrames.GetType())
  {
    case Dali::Property::BOOLEAN:
    {
      const KeyFrameBoolean* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameBooleanPtr kfCopy = KeyFrameBoolean::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<bool>::New( proxy,
                                                          target.propertyIndex,
                                                          target.componentIndex,
                                                          KeyFrameBooleanFunctor(kfCopy),
                                                          alpha,
                                                          period ) );
      break;
    }
    case Dali::Property::FLOAT:
    {
      const KeyFrameNumber* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameNumberPtr kfCopy = KeyFrameNumber::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<float>::New( proxy,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           KeyFrameNumberFunctor(kfCopy),
                                                           alpha,
                                                           period ) );
      break;
    }

    case Dali::Property::VECTOR2:
    {
      const KeyFrameVector2* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameVector2Ptr kfCopy = KeyFrameVector2::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<Vector2>::New( proxy,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             KeyFrameVector2Functor(kfCopy),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Dali::Property::VECTOR3:
    {
      const KeyFrameVector3* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameVector3Ptr kfCopy = KeyFrameVector3::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<Vector3>::New( proxy,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             KeyFrameVector3Functor(kfCopy),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Dali::Property::VECTOR4:
    {
      const KeyFrameVector4* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameVector4Ptr kfCopy = KeyFrameVector4::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<Vector4>::New( proxy,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             KeyFrameVector4Functor(kfCopy),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Dali::Property::ROTATION:
    {
      const KeyFrameQuaternion* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameQuaternionPtr kfCopy = KeyFrameQuaternion::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<Quaternion>::New( proxy,
                                                                target.propertyIndex,
                                                                target.componentIndex,
                                                                KeyFrameQuaternionFunctor(kfCopy),
                                                                alpha,
                                                                period ) );
      break;
    }

    default: // not all property types are animateable
      break;
  }
}

void Animation::Animate( Property& target, Property::Type targetType, AnyFunction& func )
{
  Animate( target, targetType, func, mDefaultAlpha, mDurationSeconds );
}

void Animation::Animate( Property& target, Property::Type targetType, AnyFunction& func, AlphaFunction& alpha )
{
  Animate( target, targetType, func, alpha, mDurationSeconds );
}

void Animation::Animate( Property& target, Property::Type targetType, AnyFunction& func, TimePeriod period )
{
  Animate( target, targetType, func, mDefaultAlpha, period );
}

void Animation::Animate( Property& target, Property::Type targetType, AnyFunction& func, AlphaFunction& alpha, TimePeriod period )
{
  Property::Type type = target.object.GetPropertyType(target.propertyIndex);
  if(target.componentIndex != Property::INVALID_COMPONENT_INDEX)
  {
    if( type == Property::VECTOR2
        || type == Property::VECTOR3
        || type == Property::VECTOR4 )
    {
      type = Property::FLOAT;
    }
  }
  DALI_ASSERT_ALWAYS( type == targetType && "Animation function must match target property type" );

  ProxyObject& proxy = dynamic_cast<ProxyObject&>( GetImplementation(target.object) );

  switch ( targetType )
  {
    case Property::BOOLEAN:
    {
      AddAnimatorConnector( AnimatorConnector<bool>::New(proxy,
                                                         target.propertyIndex,
                                                         target.componentIndex,
                                                         AnyCast< AnimatorFunctionBool >( func ),
                                                         alpha,
                                                         period) );
      break;
    }

    case Property::FLOAT:
    {
      AddAnimatorConnector( AnimatorConnector<float>::New(proxy,
                                                          target.propertyIndex,
                                                          target.componentIndex,
                                                          AnyCast< AnimatorFunctionFloat >( func ),
                                                          alpha,
                                                          period) );
      break;
    }

    case Property::VECTOR2:
    {
      AddAnimatorConnector( AnimatorConnector<Vector2>::New(proxy,
                                                            target.propertyIndex,
                                                            target.componentIndex,
                                                            AnyCast< AnimatorFunctionVector2 >( func ),
                                                            alpha,
                                                            period) );
      break;
    }

    case Property::VECTOR3:
    {
      AddAnimatorConnector( AnimatorConnector<Vector3>::New(proxy,
                                                            target.propertyIndex,
                                                            target.componentIndex,
                                                            AnyCast< AnimatorFunctionVector3 >( func ),
                                                            alpha,
                                                            period) );
      break;
    }

    case Property::VECTOR4:
    {
      AddAnimatorConnector( AnimatorConnector<Vector4>::New(proxy,
                                                            target.propertyIndex,
                                                            target.componentIndex,
                                                            AnyCast< AnimatorFunctionVector4 >( func ),
                                                            alpha,
                                                            period) );
      break;
    }

    case Property::ROTATION:
    {
      AddAnimatorConnector( AnimatorConnector<Quaternion>::New(proxy,
                                                               target.propertyIndex,
                                                               target.componentIndex,
                                                               AnyCast< AnimatorFunctionQuaternion >( func ),
                                                               alpha,
                                                               period) );
      break;
    }

    default:
      DALI_ASSERT_ALWAYS(false && "Property type enumeration out of bounds" ); // should never come here
      break;
  }
}

bool Animation::HasFinished()
{
  bool hasFinished(false);
  const int playCount(mAnimation->GetPlayCount());

  // If the play count has been incremented, then another notification is required
  if (playCount > mNotificationCount)
  {
    // Note that only one signal is emitted, if the animation has been played repeatedly
    mNotificationCount = playCount;

    hasFinished = true;
  }

  return hasFinished;
}

Dali::Animation::AnimationSignalV2& Animation::FinishedSignal()
{
  return mFinishedSignal;
}

void Animation::EmitSignalFinish()
{
  if ( !mFinishedSignal.Empty() )
  {
    Dali::Animation handle( this );
    mFinishedSignal.Emit( handle );
  }

  // This callback is used internally, to avoid the overhead of using a signal.
  if ( mFinishedCallback )
  {
    mFinishedCallback( mFinishedCallbackObject );
  }
}

bool Animation::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  Animation* animation = dynamic_cast<Animation*>(object);

  if ( Dali::Animation::SIGNAL_FINISHED == signalName )
  {
    animation->FinishedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void Animation::SetFinishedCallback( FinishedCallback callback, Object* object )
{
  mFinishedCallback = callback;
  mFinishedCallbackObject = object;
}

void Animation::AddAnimatorConnector( AnimatorConnectorBase* connector )
{
  DALI_ASSERT_DEBUG( NULL != connector );

  connector->SetParent(*this);

  mConnectors.PushBack( connector );
}

void Animation::MoveBy(Actor& actor, float x, float y, float z)
{
  MoveBy(actor, Vector3(x, y, z), mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::MoveBy(Actor& actor, const Vector3& displacement, AlphaFunction alpha)
{
  MoveBy(actor, displacement, alpha, 0.0f, GetDuration());
}

void Animation::MoveBy(Actor& actor, const Vector3& displacement, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector3>::New( actor,
                                                         Dali::Actor::POSITION,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateByVector3(displacement),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::MoveTo(Actor& actor, float x, float y, float z)
{
  MoveTo(actor, Vector3(x, y, z), mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::MoveTo(Actor& actor, const Vector3& position, AlphaFunction alpha)
{
  MoveTo(actor, position, alpha, 0.0f, GetDuration());
}

void Animation::MoveTo(Actor& actor, const Vector3& position, AlphaFunction alpha,  float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector3>::New( actor,
                                                         Dali::Actor::POSITION,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateToVector3(position),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::Move(Actor& actor, AnimatorFunctionVector3 func, AlphaFunction alpha,  float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector3>::New( actor,
                                                         Dali::Actor::POSITION,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         func,
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::RotateBy(Actor& actor, Radian angle, const Vector3& axis)
{
  RotateBy(actor, angle, axis, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::RotateBy(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha)
{
  RotateBy(actor, angle, axis, alpha, 0.0f, GetDuration());
}

void Animation::RotateBy(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Quaternion>::New( actor,
                                                            Dali::Actor::ROTATION,
                                                            Property::INVALID_COMPONENT_INDEX,
                                                            RotateByAngleAxis(angle, axis),
                                                            alpha,
                                                            TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::RotateTo(Actor& actor, Radian angle, const Vector3& axis)
{
  Vector4 normalizedAxis(axis.x, axis.y, axis.z, 0.0f);
  normalizedAxis.Normalize();

  Quaternion orientation(Quaternion::FromAxisAngle(normalizedAxis, angle));

  RotateTo(actor, orientation, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::RotateTo(Actor& actor, const Quaternion& orientation)
{
  RotateTo(actor, orientation, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::RotateTo(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha)
{
  Vector4 normalizedAxis(axis.x, axis.y, axis.z, 0.0f);
  normalizedAxis.Normalize();

  Quaternion orientation(Quaternion::FromAxisAngle(normalizedAxis, angle));

  RotateTo(actor, orientation, alpha, 0.0f, GetDuration());
}

void Animation::RotateTo(Actor& actor, const Quaternion& orientation, AlphaFunction alpha)
{
  RotateTo(actor, orientation, alpha, 0.0f, GetDuration());
}

void Animation::RotateTo(Actor& actor, Radian angle, const Vector3& axis, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  Vector4 normalizedAxis(axis.x, axis.y, axis.z, 0.0f);
  normalizedAxis.Normalize();

  Quaternion orientation(Quaternion::FromAxisAngle(normalizedAxis, angle));

  RotateTo(actor, orientation, alpha, delaySeconds, durationSeconds);
}

void Animation::RotateTo(Actor& actor, const Quaternion& rotation, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Quaternion>::New( actor,
                                                            Dali::Actor::ROTATION,
                                                            Property::INVALID_COMPONENT_INDEX,
                                                            RotateToQuaternion(rotation),
                                                            alpha,
                                                            TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::Rotate(Actor& actor, AnimatorFunctionQuaternion func, AlphaFunction alpha,  float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Quaternion>::New( actor,
                                                            Dali::Actor::ROTATION,
                                                            Property::INVALID_COMPONENT_INDEX,
                                                            func,
                                                            alpha,
                                                            TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::ScaleBy(Actor& actor, float x, float y, float z)
{
  ScaleBy(actor, Vector3(x, y, z), mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::ScaleBy(Actor& actor, const Vector3& scale, AlphaFunction alpha)
{
  ScaleBy(actor, scale, alpha, 0.0f, GetDuration());
}

void Animation::ScaleBy(Actor& actor, const Vector3& scale, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector3>::New( actor,
                                                         Dali::Actor::SCALE,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateByVector3(scale),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::ScaleTo(Actor& actor, float x, float y, float z)
{
  ScaleTo(actor, Vector3(x, y, z), mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::ScaleTo(Actor& actor, const Vector3& scale, AlphaFunction alpha)
{
  ScaleTo(actor, scale, alpha, 0.0f, GetDuration());
}

void Animation::ScaleTo(Actor& actor, const Vector3& scale, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector3>::New( actor,
                                                         Dali::Actor::SCALE,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateToVector3(scale),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::Show(Actor& actor, float delaySeconds)
{
  AddAnimatorConnector( AnimatorConnector<bool>::New( actor,
                                                      Dali::Actor::VISIBLE,
                                                      Property::INVALID_COMPONENT_INDEX,
                                                      AnimateToBoolean(SHOW_VALUE),
                                                      AlphaFunctions::Default,
                                                      TimePeriod(delaySeconds, 0.0f/*immediate*/) ) );
}

void Animation::Hide(Actor& actor, float delaySeconds)
{
  AddAnimatorConnector( AnimatorConnector<bool>::New( actor,
                                                      Dali::Actor::VISIBLE,
                                                      Property::INVALID_COMPONENT_INDEX,
                                                      AnimateToBoolean(HIDE_VALUE),
                                                      AlphaFunctions::Default,
                                                      TimePeriod(delaySeconds, 0.0f/*immediate*/) ) );
}

void Animation::OpacityBy(Actor& actor, float opacity)
{
  OpacityBy(actor, opacity, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::OpacityBy(Actor& actor, float opacity, AlphaFunction alpha)
{
  OpacityBy(actor, opacity, alpha, 0.0f, GetDuration());
}

void Animation::OpacityBy(Actor& actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector4>::New( actor,
                                                         Dali::Actor::COLOR,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateByOpacity(opacity),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::OpacityTo(Actor& actor, float opacity)
{
  OpacityTo(actor, opacity, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::OpacityTo(Actor& actor, float opacity, AlphaFunction alpha)
{
  OpacityTo(actor, opacity, alpha, 0.0f, GetDuration());
}

void Animation::OpacityTo(Actor& actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector4>::New( actor,
                                                         Dali::Actor::COLOR,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateToOpacity(opacity),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::ColorBy(Actor& actor, const Vector4& color)
{
  ColorBy(actor, color, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::ColorBy(Actor& actor, const Vector4& color, AlphaFunction alpha)
{
  ColorBy(actor, color, alpha, 0.0f, GetDuration());
}

void Animation::ColorBy(Actor& actor, const Vector4& color, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector4>::New( actor,
                                                         Dali::Actor::COLOR,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateByVector4(color),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::ColorTo(Actor& actor, const Vector4& color)
{
  ColorTo(actor, color, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::ColorTo(Actor& actor, const Vector4& color, AlphaFunction alpha)
{
  ColorTo(actor, color, alpha, 0.0f, GetDuration());
}

void Animation::ColorTo(Actor& actor, const Vector4& color, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector4>::New( actor,
                                                         Dali::Actor::COLOR,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateToVector4(color),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::Resize(Actor& actor, float width, float height)
{
  Resize(actor, width, height, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::Resize(Actor& actor, float width, float height, AlphaFunction alpha)
{
  Resize(actor, width, height, alpha, 0.0f, GetDuration());
}

void Animation::Resize(Actor& actor, float width, float height, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  Vector3 targetSize( width, height, min(width, height) );

  // notify the actor impl that its size is being animated
  actor.OnSizeAnimation( *this, targetSize );

  AddAnimatorConnector( AnimatorConnector<Vector3>::New( actor,
                                                         Dali::Actor::SIZE,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateToVector3(targetSize),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::Resize(Actor& actor, const Vector3& size)
{
  Resize(actor, size, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::Resize(Actor& actor, const Vector3& size, AlphaFunction alpha)
{
  Resize(actor, size, alpha, 0.0f, GetDuration());
}

void Animation::Resize(Actor& actor, const Vector3& size, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  // notify the actor impl that its size is being animated
  actor.OnSizeAnimation( *this, size );

  AddAnimatorConnector( AnimatorConnector<Vector3>::New( actor,
                                                         Dali::Actor::SIZE,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateToVector3(size),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::ParentOriginTo(Actor& actor, const Vector3& parentOrigin)
{
  ParentOriginTo(actor, parentOrigin, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::ParentOriginTo(Actor& actor, const Vector3& parentOrigin, AlphaFunction alpha)
{
  ParentOriginTo(actor, parentOrigin, alpha, 0.0f, GetDuration());
}

void Animation::ParentOriginTo(Actor& actor, const Vector3& parentOrigin, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector3>::New( actor,
                                                         Dali::Actor::PARENT_ORIGIN,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateToVector3(parentOrigin),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::AnchorPointTo(Actor& actor, const Vector3& anchorPoint)
{
  AnchorPointTo(actor, anchorPoint, mDefaultAlpha, 0.0f, GetDuration());
}

void Animation::AnchorPointTo(Actor& actor, const Vector3& anchorPoint, AlphaFunction alpha)
{
  AnchorPointTo(actor, anchorPoint, alpha, 0.0f, GetDuration());
}

void Animation::AnchorPointTo(Actor& actor, const Vector3& anchorPoint, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  AddAnimatorConnector( AnimatorConnector<Vector3>::New( actor,
                                                         Dali::Actor::ANCHOR_POINT,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         AnimateToVector3(anchorPoint),
                                                         alpha,
                                                         TimePeriod(delaySeconds, durationSeconds) ) );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, float value )
{
  AnimateProperty( shaderEffect, name, value, GetDefaultAlphaFunction(), 0, GetDuration() );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, float value, AlphaFunction alpha )
{
  AnimateProperty( shaderEffect, name, value, alpha, 0, GetDuration() );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, float value, AlphaFunction alpha, float delaySeconds, float durationSeconds )
{
  Property::Value propertyValue( value );

  // Register the property if it does not exist
  Property::Index index = shaderEffect.GetPropertyIndex( name );
  if ( Property::INVALID_INDEX == index )
  {
    index = shaderEffect.RegisterProperty( name, propertyValue );
  }

  AnimateTo( shaderEffect, index, Property::INVALID_COMPONENT_INDEX, propertyValue, alpha, TimePeriod(delaySeconds, durationSeconds) );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, Vector2 value )
{
  AnimateProperty( shaderEffect, name, value, GetDefaultAlphaFunction(), 0, GetDuration());
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, Vector2 value, AlphaFunction alpha )
{
  AnimateProperty( shaderEffect, name, value, alpha, 0, GetDuration() );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, Vector2 value, AlphaFunction alpha, float delaySeconds, float durationSeconds )
{
  Property::Value propertyValue( value );

  // Register the property if it does not exist
  Property::Index index = shaderEffect.GetPropertyIndex( name );
  if ( Property::INVALID_INDEX == index )
  {
    index = shaderEffect.RegisterProperty( name, propertyValue );
  }

  AnimateTo( shaderEffect, index, Property::INVALID_COMPONENT_INDEX, propertyValue, alpha, TimePeriod(delaySeconds, durationSeconds) );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, Vector3 value )
{
  AnimateProperty( shaderEffect, name, value, GetDefaultAlphaFunction(), 0, GetDuration() );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, Vector3 value, AlphaFunction alpha )
{
  AnimateProperty( shaderEffect, name, value, alpha, 0, GetDuration() );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, Vector3 value, AlphaFunction alpha, float delaySeconds, float durationSeconds )
{
  Property::Value propertyValue( value );

  // Register the property if it does not exist
  Property::Index index = shaderEffect.GetPropertyIndex( name );
  if ( Property::INVALID_INDEX == index )
  {
    index = shaderEffect.RegisterProperty( name, propertyValue );
  }

  AnimateTo( shaderEffect, index, Property::INVALID_COMPONENT_INDEX, propertyValue, alpha, TimePeriod(delaySeconds, durationSeconds) );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, Vector4 value )
{
  AnimateProperty( shaderEffect, name, value, GetDefaultAlphaFunction(), 0, GetDuration() );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, Vector4 value, AlphaFunction alpha )
{
  AnimateProperty( shaderEffect, name, value, alpha, 0, GetDuration() );
}

void Animation::AnimateProperty( Internal::ShaderEffect& shaderEffect, const std::string& name, Vector4 value, AlphaFunction alpha, float delaySeconds, float durationSeconds )
{
  Property::Value propertyValue( value );

  // Register the property if it does not exist
  Property::Index index = shaderEffect.GetPropertyIndex( name );
  if ( Property::INVALID_INDEX == index )
  {
    index = shaderEffect.RegisterProperty( name, propertyValue );
  }

  AnimateTo( shaderEffect, index, Property::INVALID_COMPONENT_INDEX, propertyValue, alpha, TimePeriod(delaySeconds, durationSeconds) );
}

bool Animation::DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes)
{
  bool done = false;
  Animation* animation = dynamic_cast<Animation*>(object);

  if( animation )
  {
    if(Dali::Animation::ACTION_PLAY == actionName)
    {
      if(attributes.size() > 0)
      {
        animation->SetDuration(attributes[0].Get<float>());
      }

      animation->Play();
      done = true;
    }
    else if(Dali::Animation::ACTION_STOP == actionName)
    {
      animation->Stop();
      done = true;
    }
    else if(Dali::Animation::ACTION_PAUSE == actionName)
    {
      animation->Pause();
      done = true;
    }
  }

  return done;
}

float Animation::GetCurrentProgress()
{
  if( mAnimation )
  {
    return mAnimation->GetCurrentProgress();
  }

  return 0.0f;
}

void Animation::SetCurrentProgress(float progress)
{
  if( mAnimation && progress >= 0.0f && progress <= 1.0f )
  {
    // mAnimation is being used in a separate thread; queue a message to set the current progress
    SetCurrentProgressMessage( mUpdateManager.GetEventToUpdate(), *mAnimation, progress );
  }
}

} // namespace Internal

} // namespace Dali
