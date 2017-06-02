/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-map.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/radian.h>
#include <dali/internal/event/animation/animation-playlist.h>
#include <dali/internal/event/animation/animator-connector.h>
#include <dali/internal/event/animation/path-impl.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/update/manager/update-manager.h>

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

// Signals

const char* const SIGNAL_FINISHED = "finished";

// Actions

const char* const ACTION_PLAY =     "play";
const char* const ACTION_STOP =     "stop";
const char* const ACTION_PAUSE =    "pause";

BaseHandle Create()
{
  return Dali::Animation::New(0.f);
}

TypeRegistration mType( typeid( Dali::Animation ), typeid( Dali::BaseHandle ), Create );

SignalConnectorType signalConnector1( mType, SIGNAL_FINISHED, &Animation::DoConnectSignal );

TypeAction action1( mType, ACTION_PLAY,  &Animation::DoAction );
TypeAction action2( mType, ACTION_STOP,  &Animation::DoAction );
TypeAction action3( mType, ACTION_PAUSE, &Animation::DoAction );

const Dali::Animation::EndAction DEFAULT_END_ACTION( Dali::Animation::Bake );
const Dali::Animation::EndAction DEFAULT_DISCONNECT_ACTION( Dali::Animation::BakeFinal );
const Dali::Animation::Interpolation DEFAULT_INTERPOLATION( Dali::Animation::Linear );
const Dali::AlphaFunction DEFAULT_ALPHA_FUNCTION( Dali::AlphaFunction::DEFAULT );

} // anon namespace


AnimationPtr Animation::New(float durationSeconds)
{
  Stage* stage = Stage::GetCurrent();

  if( stage )
  {
    AnimationPlaylist& playlist = stage->GetAnimationPlaylist();

    if( durationSeconds < 0.0f )
    {
      DALI_LOG_WARNING("duration should be greater than 0.0f.\n");
      durationSeconds = 0.0f;
    }

    AnimationPtr animation = new Animation( *stage, playlist, durationSeconds, DEFAULT_END_ACTION, DEFAULT_DISCONNECT_ACTION, DEFAULT_ALPHA_FUNCTION );

    // Second-phase construction
    animation->Initialize();

    return animation;
  }
  else
  {
    return NULL;
  }
}

Animation::Animation( EventThreadServices& eventThreadServices, AnimationPlaylist& playlist, float durationSeconds, EndAction endAction, EndAction disconnectAction, AlphaFunction defaultAlpha )
: mAnimation( NULL ),
  mEventThreadServices( eventThreadServices ),
  mPlaylist( playlist ),
  mFinishedSignal(),
  mConnectors(),
  mConnectorTargetValues(),
  mPlayRange( Vector2(0.0f,1.0f)),
  mDurationSeconds( durationSeconds ),
  mSpeedFactor(1.0f),
  mNotificationCount( 0 ),
  mLoopCount(1),
  mCurrentLoop(0),
  mEndAction( endAction ),
  mDisconnectAction( disconnectAction ),
  mDefaultAlpha( defaultAlpha ),
  mState(Dali::Animation::STOPPED)
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
  SceneGraph::Animation* animation = SceneGraph::Animation::New( mDurationSeconds, mSpeedFactor, mPlayRange, mLoopCount, mEndAction, mDisconnectAction );

  // Keep a const pointer to the animation.
  mAnimation = animation;

  // Transfer animation ownership to the update manager through a message
  AddAnimationMessage( mEventThreadServices.GetUpdateManager(), animation );
}

void Animation::DestroySceneObject()
{
  if ( mAnimation != NULL )
  {
    // Remove animation using a message to the update manager
    RemoveAnimationMessage( mEventThreadServices.GetUpdateManager(), *mAnimation );
    mAnimation = NULL;
  }
}

void Animation::SetDuration(float seconds)
{
  if( seconds < 0.0f )
  {
    DALI_LOG_WARNING("duration should be greater than 0.0f.\n");
    seconds = 0.0f;
  }

  // Cache for public getters
  mDurationSeconds = seconds;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetDurationMessage( mEventThreadServices, *mAnimation, seconds );
}

float Animation::GetDuration() const
{
  // This is not animatable; the cached value is up-to-date.
  return mDurationSeconds;
}

void Animation::SetLooping(bool on)
{
  SetLoopCount( on ? 0 : 1 );
}

void Animation::SetLoopCount(int count)
{
  // Cache for public getters
  mLoopCount = count;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetLoopingMessage( mEventThreadServices, *mAnimation, mLoopCount );
}

int Animation::GetLoopCount()
{
  return mLoopCount;
}

int Animation::GetCurrentLoop()
{
  return mCurrentLoop;
}

bool Animation::IsLooping() const
{
  return mLoopCount != 1;
}

void Animation::SetEndAction(EndAction action)
{
  // Cache for public getters
  mEndAction = action;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetEndActionMessage( mEventThreadServices, *mAnimation, action );
}

Dali::Animation::EndAction Animation::GetEndAction() const
{
  // This is not animatable; the cached value is up-to-date.
  return mEndAction;
}

void Animation::SetDisconnectAction(EndAction action)
{
  // Cache for public getters
  mDisconnectAction = action;

  // mAnimation is being used in a separate thread; queue a message to set the value
  SetDisconnectActionMessage( mEventThreadServices, *mAnimation, action );
}

Dali::Animation::EndAction Animation::GetDisconnectAction() const
{
  // This is not animatable; the cached value is up-to-date.
  return mDisconnectAction;
}

void Animation::Play()
{
  // Update the current playlist
  mPlaylist.OnPlay( *this );

  mState = Dali::Animation::PLAYING;

  NotifyObjects();

  // mAnimation is being used in a separate thread; queue a Play message
  PlayAnimationMessage( mEventThreadServices, *mAnimation );
}

void Animation::PlayFrom( float progress )
{
  if( progress >= mPlayRange.x && progress <= mPlayRange.y )
  {
    // Update the current playlist
    mPlaylist.OnPlay( *this );

    mState = Dali::Animation::PLAYING;

    NotifyObjects();

    // mAnimation is being used in a separate thread; queue a Play message
    PlayAnimationFromMessage( mEventThreadServices, *mAnimation, progress );
  }
}

void Animation::Pause()
{
  mState = Dali::Animation::PAUSED;

  // mAnimation is being used in a separate thread; queue a Pause message
  PauseAnimationMessage( mEventThreadServices, *mAnimation );
}

Dali::Animation::State Animation::GetState() const
{
  return mState;
}

void Animation::Stop()
{
  mState = Dali::Animation::STOPPED;

  // mAnimation is being used in a separate thread; queue a Stop message
  StopAnimationMessage( mEventThreadServices.GetUpdateManager(), *mAnimation );
}

void Animation::Clear()
{
  DALI_ASSERT_DEBUG(mAnimation);

  // Remove all the connectors
  mConnectors.Clear();

  // Reset the connector target values
  mConnectorTargetValues.clear();

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
  AnimateBy(target, relativeValue, mDefaultAlpha, TimePeriod(mDurationSeconds));
}

void Animation::AnimateBy(Property& target, Property::Value& relativeValue, AlphaFunction alpha)
{
  AnimateBy(target, relativeValue, alpha, TimePeriod(mDurationSeconds));
}

void Animation::AnimateBy(Property& target, Property::Value& relativeValue, TimePeriod period)
{
  AnimateBy(target, relativeValue, mDefaultAlpha, period);
}

void Animation::AnimateBy(Property& target, Property::Value& relativeValue, AlphaFunction alpha, TimePeriod period)
{
  Object& object = GetImplementation(target.object);
  const Property::Type targetType = object.GetPropertyType(target.propertyIndex);
  const Property::Type destinationType = relativeValue.GetType();

  if ( object.GetPropertyComponentIndex( target.propertyIndex ) != Property::INVALID_COMPONENT_INDEX )
  {
    DALI_ASSERT_ALWAYS(Property::FLOAT == destinationType && "Animated value and Property type don't match");
  }
  else
  {
    DALI_ASSERT_ALWAYS(targetType == destinationType && "Animated value and Property type don't match");
  }

  ExtendDuration(period);

  // Store data to later notify the object that its property is being animated
  ConnectorTargetValues connectorPair;
  connectorPair.targetValue = relativeValue;
  connectorPair.connectorIndex = mConnectors.Count();
  connectorPair.timePeriod = period;
  connectorPair.animatorType = Animation::BY;
  mConnectorTargetValues.push_back( connectorPair );

  switch ( targetType )
  {
    case Property::BOOLEAN:
    {
      AddAnimatorConnector( AnimatorConnector<bool>::New( object,
                                                          target.propertyIndex,
                                                          target.componentIndex,
                                                          new AnimateByBoolean(relativeValue.Get<bool>()),
                                                          alpha,
                                                          period ) );
      break;
    }

    case Property::INTEGER:
    {
      AddAnimatorConnector( AnimatorConnector<int>::New( object,
                                                         target.propertyIndex,
                                                         target.componentIndex,
                                                         new AnimateByInteger(relativeValue.Get<int>()),
                                                         alpha,
                                                         period ) );
      break;
    }

    case Property::FLOAT:
    {
      AddAnimatorConnector( AnimatorConnector<float>::New( object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           new AnimateByFloat(relativeValue.Get<float>()),
                                                           alpha,
                                                           period ) );
      break;
    }

    case Property::VECTOR2:
    {
      AddAnimatorConnector( AnimatorConnector<Vector2>::New( object,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             new AnimateByVector2(relativeValue.Get<Vector2>()),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Property::VECTOR3:
    {
      AddAnimatorConnector( AnimatorConnector<Vector3>::New( object,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             new AnimateByVector3(relativeValue.Get<Vector3>()),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Property::VECTOR4:
    {
      AddAnimatorConnector( AnimatorConnector<Vector4>::New( object,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             new AnimateByVector4(relativeValue.Get<Vector4>()),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Property::ROTATION:
    {
      AngleAxis angleAxis = relativeValue.Get<AngleAxis>();

      AddAnimatorConnector( AnimatorConnector<Quaternion>::New( object,
                                                                target.propertyIndex,
                                                                target.componentIndex,
                                                                new RotateByAngleAxis(angleAxis.angle, angleAxis.axis),
                                                                alpha,
                                                                period ) );
      break;
    }

    default:
    {
      // non animatable types handled already
    }
  }
}

void Animation::AnimateTo(Property& target, Property::Value& destinationValue)
{
  AnimateTo(target, destinationValue, mDefaultAlpha, TimePeriod(mDurationSeconds));
}

void Animation::AnimateTo(Property& target, Property::Value& destinationValue, AlphaFunction alpha)
{
  AnimateTo(target, destinationValue, alpha, TimePeriod(mDurationSeconds));
}

void Animation::AnimateTo(Property& target, Property::Value& destinationValue, TimePeriod period)
{
  AnimateTo(target, destinationValue, mDefaultAlpha, period);
}

void Animation::AnimateTo(Property& target, Property::Value& destinationValue, AlphaFunction alpha, TimePeriod period)
{
  Object& object = GetImplementation(target.object);

  AnimateTo( object, target.propertyIndex, target.componentIndex, destinationValue, alpha, period );
}

void Animation::AnimateTo(Object& targetObject, Property::Index targetPropertyIndex, int componentIndex, Property::Value& destinationValue, AlphaFunction alpha, TimePeriod period)
{
  Property::Type targetType = targetObject.GetPropertyType(targetPropertyIndex);
  if( componentIndex != Property::INVALID_COMPONENT_INDEX )
  {
    if( ( targetType == Property::VECTOR2 ) ||
        ( targetType == Property::VECTOR3 ) ||
        ( targetType == Property::VECTOR4 ) )
    {
      targetType = Property::FLOAT;
    }
  }
  const Property::Type destinationType = destinationValue.GetType();
  DALI_ASSERT_ALWAYS( targetType == destinationType && "Animated value and Property type don't match" );

  ExtendDuration( period );

  // Store data to later notify the object that its property is being animated
  ConnectorTargetValues connectorPair;
  connectorPair.targetValue = destinationValue;
  connectorPair.connectorIndex = mConnectors.Count();
  connectorPair.timePeriod = period;
  connectorPair.animatorType = Animation::TO;
  mConnectorTargetValues.push_back( connectorPair );

  switch ( destinationType )
  {
    case Property::BOOLEAN:
    {
      AddAnimatorConnector( AnimatorConnector<bool>::New( targetObject,
                                                          targetPropertyIndex,
                                                          componentIndex,
                                                          new AnimateToBoolean( destinationValue.Get<bool>() ),
                                                          alpha,
                                                          period ) );
      break;
    }

    case Property::INTEGER:
    {
      AddAnimatorConnector( AnimatorConnector<int>::New( targetObject,
                                                         targetPropertyIndex,
                                                         componentIndex,
                                                         new AnimateToInteger( destinationValue.Get<int>() ),
                                                         alpha,
                                                         period ) );
      break;
    }

    case Property::FLOAT:
    {
      AddAnimatorConnector( AnimatorConnector<float>::New( targetObject,
                                                           targetPropertyIndex,
                                                           componentIndex,
                                                           new AnimateToFloat( destinationValue.Get<float>() ),
                                                           alpha,
                                                           period ) );
      break;
    }

    case Property::VECTOR2:
    {
      AddAnimatorConnector( AnimatorConnector<Vector2>::New( targetObject,
                                                             targetPropertyIndex,
                                                             componentIndex,
                                                             new AnimateToVector2( destinationValue.Get<Vector2>() ),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Property::VECTOR3:
    {
      AddAnimatorConnector( AnimatorConnector<Vector3>::New( targetObject,
                                                             targetPropertyIndex,
                                                             componentIndex,
                                                             new AnimateToVector3( destinationValue.Get<Vector3>() ),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Property::VECTOR4:
    {
      AddAnimatorConnector( AnimatorConnector<Vector4>::New( targetObject,
                                                             targetPropertyIndex,
                                                             componentIndex,
                                                             new AnimateToVector4( destinationValue.Get<Vector4>() ),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Property::ROTATION:
    {
      AddAnimatorConnector( AnimatorConnector<Quaternion>::New( targetObject,
                                                                targetPropertyIndex,
                                                                componentIndex,
                                                                new RotateToQuaternion( destinationValue.Get<Quaternion>() ),
                                                                alpha,
                                                                period ) );
      break;
    }

    default:
    {
      // non animatable types handled already
    }
  }
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames)
{
  AnimateBetween(target, keyFrames, mDefaultAlpha, TimePeriod(mDurationSeconds), DEFAULT_INTERPOLATION );
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames, Interpolation interpolation )
{
  AnimateBetween(target, keyFrames, mDefaultAlpha, TimePeriod(mDurationSeconds), interpolation );
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames, TimePeriod period)
{
  AnimateBetween(target, keyFrames, mDefaultAlpha, period, DEFAULT_INTERPOLATION);
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames, TimePeriod period, Interpolation interpolation)
{
  AnimateBetween(target, keyFrames, mDefaultAlpha, period, interpolation);
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha)
{
  AnimateBetween(target, keyFrames, alpha, TimePeriod(mDurationSeconds), DEFAULT_INTERPOLATION);
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha, Interpolation interpolation)
{
  AnimateBetween(target, keyFrames, alpha, TimePeriod(mDurationSeconds), interpolation);
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period)
{
  AnimateBetween(target, keyFrames, alpha, period, DEFAULT_INTERPOLATION);
}

void Animation::AnimateBetween(Property target, const KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period, Interpolation interpolation)
{
  Object& object = GetImplementation( target.object );

  ExtendDuration( period );

  // Store data to later notify the object that its property is being animated
  ConnectorTargetValues connectorPair;
  connectorPair.targetValue = keyFrames.GetLastKeyFrameValue();
  connectorPair.connectorIndex = mConnectors.Count();
  connectorPair.timePeriod = period;
  connectorPair.animatorType = BETWEEN;
  mConnectorTargetValues.push_back( connectorPair );

  switch(keyFrames.GetType())
  {
    case Dali::Property::BOOLEAN:
    {
      const KeyFrameBoolean* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameBooleanPtr kfCopy = KeyFrameBoolean::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<bool>::New( object,
                                                          target.propertyIndex,
                                                          target.componentIndex,
                                                          new KeyFrameBooleanFunctor(kfCopy),
                                                          alpha,
                                                          period ) );
      break;
    }

    case Dali::Property::INTEGER:
    {
      const KeyFrameInteger* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameIntegerPtr kfCopy = KeyFrameInteger::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<int>::New( object,
                                                         target.propertyIndex,
                                                         target.componentIndex,
                                                         new KeyFrameIntegerFunctor(kfCopy,interpolation),
                                                         alpha,
                                                         period ) );
      break;
    }

    case Dali::Property::FLOAT:
    {
      const KeyFrameNumber* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameNumberPtr kfCopy = KeyFrameNumber::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<float>::New( object,
                                                           target.propertyIndex,
                                                           target.componentIndex,
                                                           new KeyFrameNumberFunctor(kfCopy,interpolation),
                                                           alpha,
                                                           period ) );
      break;
    }

    case Dali::Property::VECTOR2:
    {
      const KeyFrameVector2* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameVector2Ptr kfCopy = KeyFrameVector2::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<Vector2>::New( object,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             new KeyFrameVector2Functor(kfCopy,interpolation),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Dali::Property::VECTOR3:
    {
      const KeyFrameVector3* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameVector3Ptr kfCopy = KeyFrameVector3::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<Vector3>::New( object,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             new KeyFrameVector3Functor(kfCopy,interpolation),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Dali::Property::VECTOR4:
    {
      const KeyFrameVector4* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameVector4Ptr kfCopy = KeyFrameVector4::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<Vector4>::New( object,
                                                             target.propertyIndex,
                                                             target.componentIndex,
                                                             new KeyFrameVector4Functor(kfCopy,interpolation),
                                                             alpha,
                                                             period ) );
      break;
    }

    case Dali::Property::ROTATION:
    {
      const KeyFrameQuaternion* kf;
      GetSpecialization(keyFrames, kf);
      KeyFrameQuaternionPtr kfCopy = KeyFrameQuaternion::Clone(*kf);
      AddAnimatorConnector( AnimatorConnector<Quaternion>::New( object,
                                                                target.propertyIndex,
                                                                target.componentIndex,
                                                                new KeyFrameQuaternionFunctor(kfCopy),
                                                                alpha,
                                                                period ) );
      break;
    }

    default:
    {
      // non animatable types handled by keyframes
    }
  }
}

bool Animation::HasFinished()
{
  bool hasFinished(false);
  const int playedCount(mAnimation->GetPlayedCount());

  // If the play count has been incremented, then another notification is required
  mCurrentLoop = mAnimation->GetCurrentLoop();

  if (playedCount > mNotificationCount)
  {
    // Note that only one signal is emitted, if the animation has been played repeatedly
    mNotificationCount = playedCount;

    hasFinished = true;

    mState = Dali::Animation::STOPPED;
  }

  return hasFinished;
}

Dali::Animation::AnimationSignalType& Animation::FinishedSignal()
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
}

bool Animation::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  Animation* animation = static_cast< Animation* >(object); // TypeRegistry guarantees that this is the correct type.

  if( 0 == signalName.compare( SIGNAL_FINISHED ) )
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

void Animation::AddAnimatorConnector( AnimatorConnectorBase* connector )
{
  DALI_ASSERT_DEBUG( NULL != connector );

  connector->SetParent(*this);

  mConnectors.PushBack( connector );
}

void Animation::Animate( Actor& actor, const Path& path, const Vector3& forward )
{
  Animate( actor, path, forward, mDefaultAlpha, TimePeriod(mDurationSeconds) );
}

void Animation::Animate( Actor& actor, const Path& path, const Vector3& forward, AlphaFunction alpha )
{
  Animate( actor, path, forward, alpha, TimePeriod(mDurationSeconds) );
}

void Animation::Animate( Actor& actor, const Path& path, const Vector3& forward, TimePeriod period )
{
  Animate( actor, path, forward, mDefaultAlpha, period );
}

void Animation::Animate( Actor& actor, const Path& path, const Vector3& forward, AlphaFunction alpha, TimePeriod period)
{
  ExtendDuration( period );

  PathPtr pathCopy = Path::Clone(path);

  //Position animation
  AddAnimatorConnector( AnimatorConnector<Vector3>::New( actor,
                                                         Dali::Actor::Property::POSITION,
                                                         Property::INVALID_COMPONENT_INDEX,
                                                         new PathPositionFunctor( pathCopy ),
                                                         alpha,
                                                         period ) );

  //If forward is zero, PathRotationFunctor will always return the unit quaternion
  if( forward != Vector3::ZERO )
  {
    //Rotation animation
    AddAnimatorConnector( AnimatorConnector<Quaternion>::New( actor,
                                                              Dali::Actor::Property::ORIENTATION,
                                                              Property::INVALID_COMPONENT_INDEX,
                                                              new PathRotationFunctor( pathCopy, forward ),
                                                              alpha,
                                                              period ) );
  }
}

void Animation::Show(Actor& actor, float delaySeconds)
{
  ExtendDuration( TimePeriod(delaySeconds, 0) );

  AddAnimatorConnector( AnimatorConnector<bool>::New( actor,
                                                      Dali::Actor::Property::VISIBLE,
                                                      Property::INVALID_COMPONENT_INDEX,
                                                      new AnimateToBoolean(SHOW_VALUE),
                                                      mDefaultAlpha,
                                                      TimePeriod(delaySeconds, 0.0f/*immediate*/) ) );
}

void Animation::Hide(Actor& actor, float delaySeconds)
{
  ExtendDuration( TimePeriod(delaySeconds, 0) );

  AddAnimatorConnector( AnimatorConnector<bool>::New( actor,
                                                      Dali::Actor::Property::VISIBLE,
                                                      Property::INVALID_COMPONENT_INDEX,
                                                      new AnimateToBoolean(HIDE_VALUE),
                                                      mDefaultAlpha,
                                                      TimePeriod(delaySeconds, 0.0f/*immediate*/) ) );
}

bool Animation::DoAction( BaseObject* object, const std::string& actionName, const Property::Map& attributes )
{
  bool done = false;
  Animation* animation = dynamic_cast<Animation*>( object );

  if( animation )
  {
    if( 0 == actionName.compare( ACTION_PLAY ) )
    {
      if( Property::Value* value = attributes.Find("duration", Property::FLOAT) )
      {
        animation->SetDuration( value->Get<float>() );
      }

      animation->Play();
      done = true;
    }
    else if( 0 == actionName.compare( ACTION_STOP ) )
    {
      animation->Stop();
      done = true;
    }
    else if( 0 == actionName.compare( ACTION_PAUSE ) )
    {
      animation->Pause();
      done = true;
    }
  }

  return done;
}

void Animation::SetCurrentProgress(float progress)
{
  if( mAnimation && progress >= mPlayRange.x && progress <= mPlayRange.y )
  {
    // mAnimation is being used in a separate thread; queue a message to set the current progress
    SetCurrentProgressMessage( mEventThreadServices, *mAnimation, progress );
  }
}

float Animation::GetCurrentProgress()
{
  if( mAnimation )
  {
    return mAnimation->GetCurrentProgress();
  }

  return 0.0f;
}

void Animation::ExtendDuration( const TimePeriod& timePeriod )
{
  float duration = timePeriod.delaySeconds + timePeriod.durationSeconds;

  if( duration > mDurationSeconds )
  {
    SetDuration( duration );
  }
}

void Animation::SetSpeedFactor( float factor )
{
  if( mAnimation )
  {
    mSpeedFactor = factor;
    SetSpeedFactorMessage( mEventThreadServices, *mAnimation, factor );
  }
}

float Animation::GetSpeedFactor() const
{
  return mSpeedFactor;
}

void Animation::SetPlayRange( const Vector2& range)
{
  //Make sure the range specified is between 0.0 and 1.0
  if( range.x >= 0.0f && range.x <= 1.0f && range.y >= 0.0f && range.y <= 1.0f )
  {
    Vector2 orderedRange( range );
    //If the range is not in order swap values
    if( range.x > range.y )
    {
      orderedRange = Vector2(range.y, range.x);
    }

    // Cache for public getters
    mPlayRange = orderedRange;

    // mAnimation is being used in a separate thread; queue a message to set play range
    SetPlayRangeMessage( mEventThreadServices, *mAnimation, orderedRange );
  }
}

Vector2 Animation::GetPlayRange() const
{
  return mPlayRange;
}

bool Animation::CompareConnectorEndTimes( const Animation::ConnectorTargetValues& lhs, const Animation::ConnectorTargetValues& rhs )
{
  return ( ( lhs.timePeriod.delaySeconds + lhs.timePeriod.durationSeconds ) < ( rhs.timePeriod.delaySeconds + rhs.timePeriod.durationSeconds ) );
}

void Animation::NotifyObjects()
{
  if( mEndAction != EndAction::Discard ) // If the animation is discarded, then we do not want to change the target values
  {
    // Sort according to end time with earlier end times coming first, if the end time is the same, then the connectors are not moved
    std::stable_sort( mConnectorTargetValues.begin(), mConnectorTargetValues.end(), CompareConnectorEndTimes );

    // Loop through all connector target values sorted by increasing end time
    ConnectorTargetValuesContainer::const_iterator iter = mConnectorTargetValues.begin();
    const ConnectorTargetValuesContainer::const_iterator endIter = mConnectorTargetValues.end();
    for( ; iter != endIter; ++iter )
    {
      AnimatorConnectorBase* connector = mConnectors[ iter->connectorIndex ];

      Object* object = connector->GetObject();
      if( object )
      {
        object->NotifyPropertyAnimation( *this, connector->GetPropertyIndex(), iter->targetValue, iter->animatorType );
      }
    }
  }
}

} // namespace Internal

} // namespace Dali
