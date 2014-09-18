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
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/effects/shader-effect-impl.h>

namespace Dali
{

const char* const Animation::SIGNAL_FINISHED = "finished";
const char* const Animation::ACTION_PLAY = "play";
const char* const Animation::ACTION_STOP = "stop";
const char* const Animation::ACTION_PAUSE = "pause";

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

Animation& Animation::operator=(BaseHandle::NullType* rhs)
{
  DALI_ASSERT_ALWAYS( (rhs == NULL) && "Can only assign NULL pointer to handle");
  Reset();
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

void Animation::SetDestroyAction(Dali::Animation::EndAction destroyAction)
{
  GetImplementation(*this).SetDestroyAction(destroyAction);
}

Dali::Animation::EndAction Animation::GetDestroyAction() const
{
  return GetImplementation(*this).GetDestroyAction();
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

Animation::AnimationSignalV2& Animation::FinishedSignal()
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

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames));
}

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames, TimePeriod period)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames), period);
}

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames), alpha);
}

void Animation::AnimateBetween(Property target, KeyFrames& keyFrames, AlphaFunction alpha, TimePeriod period)
{
  GetImplementation(*this).AnimateBetween(target, GetImplementation(keyFrames), alpha, period);
}

void Animation::Animate( Property target, Property::Type targetType, AnyFunction func )
{
  GetImplementation(*this).Animate( target, targetType, func );
}

void Animation::Animate( Property target, Property::Type targetType, AnyFunction func, AlphaFunction alpha )
{
  GetImplementation(*this).Animate( target, targetType, func, alpha );
}

void Animation::Animate( Property target, Property::Type targetType, AnyFunction func, TimePeriod period )
{
  GetImplementation(*this).Animate( target, targetType, func, period );
}

void Animation::Animate( Property target, Property::Type targetType, AnyFunction func, AlphaFunction alpha, TimePeriod period )
{
  GetImplementation(*this).Animate( target, targetType, func, alpha, period );
}

// Actor specific animations

void Animation::MoveBy(Actor actor, float x, float y, float z)
{
  GetImplementation(*this).MoveBy(GetImplementation(actor), x, y, z);
}

void Animation::MoveBy(Actor actor, Vector3 displacement, AlphaFunction alpha)
{
  GetImplementation(*this).MoveBy(GetImplementation(actor), displacement, alpha);
}

void Animation::MoveBy(Actor actor, Vector3 displacement, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).MoveBy(GetImplementation(actor), displacement, alpha, delaySeconds, durationSeconds);
}

void Animation::MoveTo(Actor actor, float x, float y, float z)
{
  GetImplementation(*this).MoveTo(GetImplementation(actor), x, y, z);
}

void Animation::MoveTo(Actor actor, Vector3 position, AlphaFunction alpha)
{
  GetImplementation(*this).MoveTo(GetImplementation(actor), position, alpha);
}

void Animation::MoveTo(Actor actor, Vector3 position, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).MoveTo(GetImplementation(actor), position, alpha, delaySeconds, durationSeconds);
}

void Animation::Move(Actor actor, AnimatorFunctionVector3 animatorFunc, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).Move(GetImplementation(actor), animatorFunc, alpha, delaySeconds, durationSeconds);
}

void Animation::RotateBy(Actor actor, Degree angle, Vector3 axis)
{
  GetImplementation(*this).RotateBy(GetImplementation(actor), Radian(angle), axis);
}

void Animation::RotateBy(Actor actor, Radian angle, Vector3 axis)
{
  GetImplementation(*this).RotateBy(GetImplementation(actor), angle, axis);
}

void Animation::RotateBy(Actor actor, Degree angle, Vector3 axis, AlphaFunction alpha)
{
  GetImplementation(*this).RotateBy(GetImplementation(actor), Radian(angle), axis, alpha);
}

void Animation::RotateBy(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha)
{
  GetImplementation(*this).RotateBy(GetImplementation(actor), angle, axis, alpha);
}

void Animation::RotateBy(Actor actor, Degree angle, Vector3 axis, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).RotateBy(GetImplementation(actor), Radian(angle), axis, alpha, delaySeconds, durationSeconds);
}

void Animation::RotateBy(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).RotateBy(GetImplementation(actor), angle, axis, alpha, delaySeconds, durationSeconds);
}

void Animation::RotateTo(Actor actor, Degree angle, Vector3 axis)
{
  GetImplementation(*this).RotateTo(GetImplementation(actor), Radian(angle), axis);
}

void Animation::RotateTo(Actor actor, Radian angle, Vector3 axis)
{
  GetImplementation(*this).RotateTo(GetImplementation(actor), angle, axis);
}

void Animation::RotateTo(Actor actor, Quaternion rotation)
{
  GetImplementation(*this).RotateTo(GetImplementation(actor), rotation);
}

void Animation::RotateTo(Actor actor, Degree angle, Vector3 axis, AlphaFunction alpha)
{
  GetImplementation(*this).RotateTo(GetImplementation(actor), Radian(angle), axis, alpha);
}

void Animation::RotateTo(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha)
{
  GetImplementation(*this).RotateTo(GetImplementation(actor), angle, axis, alpha);
}

void Animation::RotateTo(Actor actor, Quaternion rotation, AlphaFunction alpha)
{
  GetImplementation(*this).RotateTo(GetImplementation(actor), rotation, alpha);
}

void Animation::RotateTo(Actor actor, Degree angle, Vector3 axis, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).RotateTo(GetImplementation(actor), Radian(angle), axis, alpha, delaySeconds, durationSeconds);
}

void Animation::RotateTo(Actor actor, Radian angle, Vector3 axis, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).RotateTo(GetImplementation(actor), angle, axis, alpha, delaySeconds, durationSeconds);
}

void Animation::RotateTo(Actor actor, Quaternion rotation, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).RotateTo(GetImplementation(actor), rotation, alpha, delaySeconds, durationSeconds);
}

void Animation::Rotate(Actor actor, AnimatorFunctionQuaternion animatorFunc, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).Rotate(GetImplementation(actor), animatorFunc, alpha, delaySeconds, durationSeconds);
}

void Animation::ScaleBy(Actor actor, float x, float y, float z)
{
  GetImplementation(*this).ScaleBy(GetImplementation(actor), x, y, z);
}

void Animation::ScaleBy(Actor actor, Vector3 scale, AlphaFunction alpha)
{
  GetImplementation(*this).ScaleBy(GetImplementation(actor), scale, alpha);
}

void Animation::ScaleBy(Actor actor, Vector3 scale, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).ScaleBy(GetImplementation(actor), scale, alpha, delaySeconds, durationSeconds);
}

void Animation::ScaleTo(Actor actor, float x, float y, float z)
{
  GetImplementation(*this).ScaleTo(GetImplementation(actor), x, y, z);
}

void Animation::ScaleTo(Actor actor, Vector3 scale, AlphaFunction alpha)
{
  GetImplementation(*this).ScaleTo(GetImplementation(actor), scale, alpha);
}

void Animation::ScaleTo(Actor actor, Vector3 scale, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).ScaleTo(GetImplementation(actor), scale, alpha, delaySeconds, durationSeconds);
}

void Animation::Show(Actor actor, float delaySeconds)
{
  GetImplementation(*this).Show(GetImplementation(actor), delaySeconds);
}

void Animation::Hide(Actor actor, float delaySeconds)
{
  GetImplementation(*this).Hide(GetImplementation(actor), delaySeconds);
}

void Animation::OpacityBy(Actor actor, float opacity)
{
  GetImplementation(*this).OpacityBy(GetImplementation(actor), opacity);
}

void Animation::OpacityBy(Actor actor, float opacity, AlphaFunction alpha)
{
  GetImplementation(*this).OpacityBy(GetImplementation(actor), opacity, alpha);
}

void Animation::OpacityBy(Actor actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).OpacityBy(GetImplementation(actor), opacity, alpha, delaySeconds, durationSeconds);
}

void Animation::OpacityTo(Actor actor, float opacity)
{
  GetImplementation(*this).OpacityTo(GetImplementation(actor), opacity);
}

void Animation::OpacityTo(Actor actor, float opacity, AlphaFunction alpha)
{
  GetImplementation(*this).OpacityTo(GetImplementation(actor), opacity, alpha);
}

void Animation::OpacityTo(Actor actor, float opacity, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).OpacityTo(GetImplementation(actor), opacity, alpha, delaySeconds, durationSeconds);
}

void Animation::ColorBy(Actor actor, Vector4 color)
{
  GetImplementation(*this).ColorBy(GetImplementation(actor), color);
}

void Animation::ColorBy(Actor actor, Vector4 color, AlphaFunction alpha)
{
  GetImplementation(*this).ColorBy(GetImplementation(actor), color, alpha);
}

void Animation::ColorBy(Actor actor, Vector4 color, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).ColorBy(GetImplementation(actor), color, alpha, delaySeconds, durationSeconds);
}

void Animation::ColorTo(Actor actor, Vector4 color)
{
  GetImplementation(*this).ColorTo(GetImplementation(actor), color);
}

void Animation::ColorTo(Actor actor, Vector4 color, AlphaFunction alpha)
{
  GetImplementation(*this).ColorTo(GetImplementation(actor), color, alpha);
}

void Animation::ColorTo(Actor actor, Vector4 color, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).ColorTo(GetImplementation(actor), color, alpha, delaySeconds, durationSeconds);
}

void Animation::Resize(Actor actor, float width, float height)
{
  GetImplementation(*this).Resize(GetImplementation(actor), width, height);
}

void Animation::Resize(Actor actor, float width, float height, AlphaFunction alpha)
{
  GetImplementation(*this).Resize(GetImplementation(actor), width, height, alpha);
}

void Animation::Resize(Actor actor,float width, float height, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).Resize(GetImplementation(actor), width, height, alpha, delaySeconds, durationSeconds);
}

void Animation::Resize(Actor actor, Vector3 size)
{
  GetImplementation(*this).Resize(GetImplementation(actor), size);
}

void Animation::Resize(Actor actor, Vector3 size, AlphaFunction alpha)
{
  GetImplementation(*this).Resize(GetImplementation(actor), size, alpha);
}

void Animation::Resize(Actor actor, Vector3 size, AlphaFunction alpha, float delaySeconds, float durationSeconds)
{
  GetImplementation(*this).Resize(GetImplementation(actor), size, alpha, delaySeconds, durationSeconds);
}

float Animation::GetCurrentProgress()
{
  return GetImplementation(*this).GetCurrentProgress();
}

void Animation::SetCurrentProgress( float progress )
{
  return GetImplementation(*this).SetCurrentProgress( progress );
}

void Animation::SetSpeedFactor( float factor )
{
  GetImplementation(*this).SetSpeedFactor( factor );
}

float Animation::GetSpeedFactor() const
{
  return GetImplementation(*this).GetSpeedFactor();
}

} // namespace Dali
