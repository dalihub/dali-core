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

// CLASS HEADER
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/effects/shader-effect-impl.h>

namespace Dali
{

Animation::Animation()
{
}

Animation::Animation(Internal::Animation* animation)
: BaseHandle(animation)
{
}

Animation Animation::New(float durationSeconds)
{
  Internal::AnimationPtr internal = Dali::Internal::Animation::New(durationSeconds);

  return Animation(internal.Get());
}

Animation Animation::DownCast( BaseHandle handle )
{
  return Animation( dynamic_cast<Dali::Internal::Animation*>(handle.GetObjectPtr()) );
}

Animation::~Animation()
{
}

Animation::Animation(const Animation& handle)
: BaseHandle(handle)
{
}

Animation& Animation::operator=(const Animation& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void Animation::SetDuration(float durationSeconds)
{
  GetImplementation(*this).SetDuration(durationSeconds);
}

float Animation::GetDuration() const
{
  return GetImplementation(*this).GetDuration();
}

void Animation::SetLooping(bool looping)
{
  GetImplementation(*this).SetLooping(looping);
}

bool Animation::IsLooping() const
{
  return GetImplementation(*this).IsLooping();
}

void Animation::SetEndAction(Dali::Animation::EndAction endAction)
{
  GetImplementation(*this).SetEndAction(endAction);
}

Dali::Animation::EndAction Animation::GetEndAction() const
{
  return GetImplementation(*this).GetEndAction();
}

void Animation::SetDisconnectAction( Animation::EndAction disconnectAction )
{
  GetImplementation(*this).SetDisconnectAction( disconnectAction );
}

Animation::EndAction Animation::GetDisconnectAction() const
{
  return GetImplementation(*this).GetDisconnectAction();
}

void Animation::SetDefaultAlphaFunction(AlphaFunction alpha)
{
  GetImplementation(*this).SetDefaultAlphaFunction(alpha);
}

AlphaFunction Animation::GetDefaultAlphaFunction() const
{
  return GetImplementation(*this).GetDefaultAlphaFunction();
}

void Animation::Play()
{
  GetImplementation(*this).Play();
}

void Animation::PlayFrom(float progress)
{
  GetImplementation(*this).PlayFrom(progress);
}


void Animation::Pause()
{
  GetImplementation(*this).Pause();
}

void Animation::Stop()
{
  GetImplementation(*this).Stop();
}

void Animation::Clear()
{
  GetImplementation(*this).Clear();
}

Animation::AnimationSignalType& Animation::FinishedSignal()
{
  return GetImplementation(*this).FinishedSignal();
}

void Animation::AnimateBy(Property target, Property::Value relativeValue)
{
  GetImplementation(*this).AnimateBy(target, relativeValue);
}

void Animation::AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha)
{
  GetImplementation(*this).AnimateBy(target, relativeValue, alpha);
}

void Animation::AnimateBy(Property target, Property::Value relativeValue, TimePeriod period)
{
  GetImplementation(*this).AnimateBy(target, relativeValue, period);
}

void Animation::AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha, TimePeriod period)
{
  GetImplementation(*this).AnimateBy(target, relativeValue, alpha, period);
}

void Animation::AnimateTo(Property target, Property::Value destinationValue)
{
  GetImplementation(*this).AnimateTo(target, destinationValue);
}

void Animation::AnimateTo(Property target, Property::Value destinationValue, AlphaFunction alpha)
{
  GetImplementation(*this).AnimateTo(target, destinationValue, alpha);
}

void Animation::AnimateTo(Property target, Property::Value destinationValue, TimePeriod period)
{
  GetImplementation(*this).AnimateTo(target, destinationValue, period);
}

void Animation::AnimateTo(Property target, Property::Value destinationValue, AlphaFunction alpha, TimePeriod period)
{
  GetImplementation(*this).AnimateTo(target, destinationValue, alpha, period);
}

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames )
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames) );
}

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames, Interpolation interpolation)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames), interpolation );
}

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames, TimePeriod period)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames), period);
}

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames, TimePeriod period, Interpolation interpolation)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames), period, interpolation );
}

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames), alpha);
}

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, Interpolation interpolation)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames), alpha, interpolation);
}


void Animation::AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames), alpha, period);
}

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period, Interpolation interpolation)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames), alpha, period, interpolation);
}


// Actor specific animations

void Animation::Animate( Actor actor, Path path, const Vector3& forward )
{
  GetImplementation(*this).Animate(GetImplementation(actor), GetImplementation( path ), forward );
}

void Animation::Animate( Actor actor, Path path, const Vector3& forward, AlphaFunction alpha )
{
  GetImplementation(*this).Animate(GetImplementation(actor), GetImplementation( path ), forward, alpha );
}

void Animation::Animate( Actor actor, Path path, const Vector3& forward, TimePeriod period )
{
  GetImplementation(*this).Animate(GetImplementation(actor), GetImplementation( path ), forward, period);
}

void Animation::Animate( Actor actor, Path path, const Vector3& forward, AlphaFunction alpha, TimePeriod period)
{
  GetImplementation(*this).Animate(GetImplementation(actor), GetImplementation( path ), forward, alpha, period );
}

void Animation::Show(Actor actor, float delaySeconds)
{
  GetImplementation(*this).Show(GetImplementation(actor), delaySeconds);
}

void Animation::Hide(Actor actor, float delaySeconds)
{
  GetImplementation(*this).Hide(GetImplementation(actor), delaySeconds);
}

void Animation::SetCurrentProgress( float progress )
{
  return GetImplementation(*this).SetCurrentProgress( progress );
}

float Animation::GetCurrentProgress()
{
  return GetImplementation(*this).GetCurrentProgress();
}

void Animation::SetSpeedFactor( float factor )
{
  GetImplementation(*this).SetSpeedFactor( factor );
}

float Animation::GetSpeedFactor() const
{
  return GetImplementation(*this).GetSpeedFactor();
}

void Animation::SetPlayRange( const Vector2& range )
{
  GetImplementation(*this).SetPlayRange(range);
}

Vector2 Animation::GetPlayRange() const
{
  return GetImplementation(*this).GetPlayRange();
}

} // namespace Dali
