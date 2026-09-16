/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/pan-gesture-detector.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/pan-gesture/pan-gesture-detector-impl.h>
#include <dali/internal/event/events/pan-gesture/pan-gesture-profile.h>

#define DALI_ASSERT_VALID_PAN_OPTIONS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from PanGestureDetector::Options object")

namespace DALI_NAMESPACE
{
const Radian PanGestureDetector::DIRECTION_LEFT(-Math::PI);
const Radian PanGestureDetector::DIRECTION_RIGHT(0.0f);
const Radian PanGestureDetector::DIRECTION_UP(-0.5f * Math::PI);
const Radian PanGestureDetector::DIRECTION_DOWN(0.5f * Math::PI);
const Radian PanGestureDetector::DIRECTION_HORIZONTAL(-Math::PI);
const Radian PanGestureDetector::DIRECTION_VERTICAL(-0.5f * Math::PI);
const Radian PanGestureDetector::DEFAULT_THRESHOLD(0.25f * Math::PI);

PanGestureDetector::PanGestureDetector(Internal::PanGestureDetector* internal)
: GestureDetector(internal)
{
}

PanGestureDetector::PanGestureDetector() = default;

PanGestureDetector PanGestureDetector::New()
{
  Internal::PanGestureDetectorPtr internal = Internal::PanGestureDetector::New();

  return PanGestureDetector(internal.Get());
}

PanGestureDetector PanGestureDetector::DownCast(BaseHandle handle)
{
  return PanGestureDetector(dynamic_cast<Dali::Internal::PanGestureDetector*>(handle.GetObjectPtr()));
}

PanGestureDetector::~PanGestureDetector() = default;

PanGestureDetector::PanGestureDetector(const PanGestureDetector& handle) = default;

PanGestureDetector& PanGestureDetector::operator=(const PanGestureDetector& rhs) = default;

PanGestureDetector::PanGestureDetector(PanGestureDetector&& handle) noexcept = default;

PanGestureDetector& PanGestureDetector::operator=(PanGestureDetector&& rhs) noexcept = default;

void PanGestureDetector::SetMinimumTouchesRequired(uint32_t minimum)
{
  GetImplementation(*this).SetMinimumTouchesRequired(minimum);
}

void PanGestureDetector::SetMaximumTouchesRequired(uint32_t maximum)
{
  GetImplementation(*this).SetMaximumTouchesRequired(maximum);
}

void PanGestureDetector::SetMaximumMotionEventAge(uint32_t maximumAge)
{
  GetImplementation(*this).SetMaximumMotionEventAge(maximumAge);
}

uint32_t PanGestureDetector::GetMinimumTouchesRequired() const
{
  return GetImplementation(*this).GetMinimumTouchesRequired();
}

uint32_t PanGestureDetector::GetMaximumTouchesRequired() const
{
  return GetImplementation(*this).GetMaximumTouchesRequired();
}

uint32_t PanGestureDetector::GetMaximumMotionEventAge() const
{
  return GetImplementation(*this).GetMaximumMotionEventAge();
}

void PanGestureDetector::AddAngle(Radian angle, Radian threshold)
{
  GetImplementation(*this).AddAngle(angle, threshold);
}

void PanGestureDetector::AddDirection(Radian direction, Radian threshold)
{
  GetImplementation(*this).AddDirection(direction, threshold);
}

uint32_t PanGestureDetector::GetAngleCount() const
{
  return GetImplementation(*this).GetAngleCount();
}

PanGestureDetector::AngleThresholdPair PanGestureDetector::GetAngle(uint32_t index) const
{
  return GetImplementation(*this).GetAngle(index);
}

void PanGestureDetector::ClearAngles()
{
  GetImplementation(*this).ClearAngles();
}

void PanGestureDetector::RemoveAngle(Radian angle)
{
  GetImplementation(*this).RemoveAngle(angle);
}

void PanGestureDetector::RemoveDirection(Radian direction)
{
  GetImplementation(*this).RemoveDirection(direction);
}

PanGestureDetector::DetectedSignalType& PanGestureDetector::DetectedSignal()
{
  return GetImplementation(*this).DetectedSignal();
}

void PanGestureDetector::SetPanGestureProperties(const Dali::PanGesture& pan)
{
  Internal::PanGestureDetector::SetPanGestureProperties(pan);
}

PanGestureDetector::Options PanGestureDetector::GetDefaultOptions() const
{
  return Options(GetImplementation(*this).GetDefaultProfile());
}

void PanGestureDetector::SetDeviceOptions(const GestureDeviceSelector& selector, const Options& options)
{
  const Internal::PanGestureProfile& profile = options.GetProfile();
  DALI_LOG_RELEASE_INFO("detector=%p selector(match=%d class=%d subclass=%d name='%s') minimumTouches=%u maximumTouches=%u maximumMotionEventAge=%u angles=%u\n",
                        &GetImplementation(*this), static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr(), profile.minimumTouches, profile.maximumTouches, profile.maximumMotionEventAge, profile.GetAngleCount());
  GetImplementation(*this).SetDeviceProfile(selector, profile);
}

bool PanGestureDetector::GetDeviceOptions(const GestureDeviceSelector& selector, Options& options) const
{
  const Internal::PanGestureProfile* profile = GetImplementation(*this).GetDeviceProfile(selector);
  if(profile)
  {
    options = Options(*profile);
    return true;
  }
  return false;
}

void PanGestureDetector::ClearDeviceOptions(const GestureDeviceSelector& selector)
{
  DALI_LOG_RELEASE_INFO("detector=%p selector(match=%d class=%d subclass=%d name='%s')\n", &GetImplementation(*this), static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr());
  GetImplementation(*this).ClearDeviceProfile(selector);
}

// PanGestureDetector::Options

struct PanGestureDetector::Options::Impl
{
  Impl() = default;

  explicit Impl(const Internal::PanGestureProfile& profile)
  : mProfile(profile)
  {
  }

  Internal::PanGestureProfile mProfile;
};

PanGestureDetector::Options::Options()
: mImpl(MakeUnique<Impl>())
{
}

PanGestureDetector::Options::Options(const Internal::PanGestureProfile& profile)
: mImpl(MakeUnique<Impl>(profile))
{
}

PanGestureDetector::Options::Options(const Options& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_PAN_OPTIONS(rhs.mImpl);
  mImpl = MakeUnique<Impl>(*rhs.mImpl);
}

PanGestureDetector::Options::Options(Options&& rhs) noexcept = default;

PanGestureDetector::Options& PanGestureDetector::Options::operator=(const Options& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_PAN_OPTIONS(rhs.mImpl);
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

PanGestureDetector::Options& PanGestureDetector::Options::operator=(Options&& rhs) noexcept = default;

PanGestureDetector::Options::~Options() = default;

void PanGestureDetector::Options::SetMinimumTouchesRequired(uint32_t minimum)
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  mImpl->mProfile.minimumTouches = minimum;
}

void PanGestureDetector::Options::SetMaximumTouchesRequired(uint32_t maximum)
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  mImpl->mProfile.maximumTouches = maximum;
}

void PanGestureDetector::Options::SetMaximumMotionEventAge(uint32_t maximumAge)
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  mImpl->mProfile.maximumMotionEventAge = maximumAge;
}

uint32_t PanGestureDetector::Options::GetMinimumTouchesRequired() const
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  return mImpl->mProfile.minimumTouches;
}

uint32_t PanGestureDetector::Options::GetMaximumTouchesRequired() const
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  return mImpl->mProfile.maximumTouches;
}

uint32_t PanGestureDetector::Options::GetMaximumMotionEventAge() const
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  return mImpl->mProfile.maximumMotionEventAge;
}

void PanGestureDetector::Options::AddAngle(Radian angle, Radian threshold)
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  mImpl->mProfile.AddAngle(angle, threshold);
}

void PanGestureDetector::Options::AddDirection(Radian direction, Radian threshold)
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  mImpl->mProfile.AddDirection(direction, threshold);
}

uint32_t PanGestureDetector::Options::GetAngleCount() const
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  return mImpl->mProfile.GetAngleCount();
}

PanGestureDetector::AngleThresholdPair PanGestureDetector::Options::GetAngle(uint32_t index) const
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  return mImpl->mProfile.GetAngle(index);
}

void PanGestureDetector::Options::ClearAngles()
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  mImpl->mProfile.ClearAngles();
}

void PanGestureDetector::Options::RemoveAngle(Radian angle)
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  mImpl->mProfile.RemoveAngle(angle);
}

void PanGestureDetector::Options::RemoveDirection(Radian direction)
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  mImpl->mProfile.RemoveDirection(direction);
}

const Internal::PanGestureProfile& PanGestureDetector::Options::GetProfile() const
{
  DALI_ASSERT_VALID_PAN_OPTIONS(mImpl);
  return mImpl->mProfile;
}

} //namespace DALI_NAMESPACE

#undef DALI_ASSERT_VALID_PAN_OPTIONS
