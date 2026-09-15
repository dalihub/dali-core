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
#include <dali/public-api/events/long-press-gesture-detector.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-detector-impl.h>
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-profile.h>

#define DALI_ASSERT_VALID_LONG_PRESS_OPTIONS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from LongPressGestureDetector::Options object")

namespace DALI_NAMESPACE
{
LongPressGestureDetector::LongPressGestureDetector(Internal::LongPressGestureDetector* internal)
: GestureDetector(internal)
{
}

LongPressGestureDetector::LongPressGestureDetector() = default;

LongPressGestureDetector LongPressGestureDetector::New()
{
  Internal::LongPressGestureDetectorPtr internal = Internal::LongPressGestureDetector::New();

  return LongPressGestureDetector(internal.Get());
}

LongPressGestureDetector LongPressGestureDetector::New(uint32_t touchesRequired)
{
  Internal::LongPressGestureDetectorPtr internal = Internal::LongPressGestureDetector::New(touchesRequired);

  return LongPressGestureDetector(internal.Get());
}

LongPressGestureDetector LongPressGestureDetector::New(uint32_t minTouches, uint32_t maxTouches)
{
  Internal::LongPressGestureDetectorPtr internal = Internal::LongPressGestureDetector::New(minTouches, maxTouches);

  return LongPressGestureDetector(internal.Get());
}

LongPressGestureDetector LongPressGestureDetector::DownCast(BaseHandle handle)
{
  return LongPressGestureDetector(dynamic_cast<Dali::Internal::LongPressGestureDetector*>(handle.GetObjectPtr()));
}

LongPressGestureDetector::~LongPressGestureDetector() = default;

LongPressGestureDetector::LongPressGestureDetector(const LongPressGestureDetector& handle) = default;

LongPressGestureDetector& LongPressGestureDetector::operator=(const LongPressGestureDetector& rhs) = default;

LongPressGestureDetector::LongPressGestureDetector(LongPressGestureDetector&& handle) noexcept = default;

LongPressGestureDetector& LongPressGestureDetector::operator=(LongPressGestureDetector&& rhs) noexcept = default;

void LongPressGestureDetector::SetTouchesRequired(uint32_t touches)
{
  GetImplementation(*this).SetTouchesRequired(touches);
}

void LongPressGestureDetector::SetTouchesRequired(uint32_t minTouches, uint32_t maxTouches)
{
  GetImplementation(*this).SetTouchesRequired(minTouches, maxTouches);
}

uint32_t LongPressGestureDetector::GetMinimumTouchesRequired() const
{
  return GetImplementation(*this).GetMinimumTouchesRequired();
}

uint32_t LongPressGestureDetector::GetMaximumTouchesRequired() const
{
  return GetImplementation(*this).GetMaximumTouchesRequired();
}

LongPressGestureDetector::DetectedSignalType& LongPressGestureDetector::DetectedSignal()
{
  return GetImplementation(*this).DetectedSignal();
}

LongPressGestureDetector::Options LongPressGestureDetector::GetDefaultOptions() const
{
  return Options(GetImplementation(*this).GetDefaultProfile());
}

void LongPressGestureDetector::SetDeviceOptions(const GestureDeviceSelector& selector, const Options& options)
{
  const Internal::LongPressGestureProfile& profile = options.GetProfile();
  DALI_LOG_RELEASE_INFO("detector=%p selector(match=%d class=%d subclass=%d name='%s') minimumTouches=%u maximumTouches=%u\n",
                        &GetImplementation(*this), static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr(), profile.minimumTouches, profile.maximumTouches);
  GetImplementation(*this).SetDeviceProfile(selector, profile);
}

bool LongPressGestureDetector::GetDeviceOptions(const GestureDeviceSelector& selector, Options& options) const
{
  const Internal::LongPressGestureProfile* profile = GetImplementation(*this).GetDeviceProfile(selector);
  if(profile)
  {
    options = Options(*profile);
    return true;
  }
  return false;
}

void LongPressGestureDetector::ClearDeviceOptions(const GestureDeviceSelector& selector)
{
  DALI_LOG_RELEASE_INFO("detector=%p selector(match=%d class=%d subclass=%d name='%s')\n", &GetImplementation(*this), static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr());
  GetImplementation(*this).ClearDeviceProfile(selector);
}

// LongPressGestureDetector::Options

struct LongPressGestureDetector::Options::Impl
{
  Impl() = default;

  explicit Impl(const Internal::LongPressGestureProfile& profile)
  : mProfile(profile)
  {
  }

  Internal::LongPressGestureProfile mProfile;
};

LongPressGestureDetector::Options::Options()
: mImpl(MakeUnique<Impl>())
{
}

LongPressGestureDetector::Options::Options(const Internal::LongPressGestureProfile& profile)
: mImpl(MakeUnique<Impl>(profile))
{
}

LongPressGestureDetector::Options::Options(const Options& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_LONG_PRESS_OPTIONS(rhs.mImpl);
  mImpl = MakeUnique<Impl>(*rhs.mImpl);
}

LongPressGestureDetector::Options::Options(Options&& rhs) noexcept = default;

LongPressGestureDetector::Options& LongPressGestureDetector::Options::operator=(const Options& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_LONG_PRESS_OPTIONS(rhs.mImpl);
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

LongPressGestureDetector::Options& LongPressGestureDetector::Options::operator=(Options&& rhs) noexcept = default;

LongPressGestureDetector::Options::~Options() = default;

void LongPressGestureDetector::Options::SetTouchesRequired(uint32_t touches)
{
  DALI_ASSERT_VALID_LONG_PRESS_OPTIONS(mImpl);
  mImpl->mProfile.minimumTouches = touches;
  mImpl->mProfile.maximumTouches = touches;
}

void LongPressGestureDetector::Options::SetTouchesRequired(uint32_t minTouches, uint32_t maxTouches)
{
  DALI_ASSERT_VALID_LONG_PRESS_OPTIONS(mImpl);
  mImpl->mProfile.minimumTouches = minTouches;
  mImpl->mProfile.maximumTouches = maxTouches;
}

uint32_t LongPressGestureDetector::Options::GetMinimumTouchesRequired() const
{
  DALI_ASSERT_VALID_LONG_PRESS_OPTIONS(mImpl);
  return mImpl->mProfile.minimumTouches;
}

uint32_t LongPressGestureDetector::Options::GetMaximumTouchesRequired() const
{
  DALI_ASSERT_VALID_LONG_PRESS_OPTIONS(mImpl);
  return mImpl->mProfile.maximumTouches;
}

const Internal::LongPressGestureProfile& LongPressGestureDetector::Options::GetProfile() const
{
  DALI_ASSERT_VALID_LONG_PRESS_OPTIONS(mImpl);
  return mImpl->mProfile;
}

} //namespace DALI_NAMESPACE

#undef DALI_ASSERT_VALID_LONG_PRESS_OPTIONS
