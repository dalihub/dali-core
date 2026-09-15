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
#include <dali/public-api/events/tap-gesture-detector.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/tap-gesture/tap-gesture-detector-impl.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-profile.h>

#define DALI_ASSERT_VALID_TAP_OPTIONS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from TapGestureDetector::Options object")

namespace DALI_NAMESPACE
{
TapGestureDetector::TapGestureDetector(Internal::TapGestureDetector* internal)
: GestureDetector(internal)
{
}

TapGestureDetector::TapGestureDetector() = default;

TapGestureDetector TapGestureDetector::New()
{
  Internal::TapGestureDetectorPtr internal = Internal::TapGestureDetector::New();

  return TapGestureDetector(internal.Get());
}

TapGestureDetector TapGestureDetector::New(uint32_t tapsRequired)
{
  Internal::TapGestureDetectorPtr internal = Internal::TapGestureDetector::New(tapsRequired);

  return TapGestureDetector(internal.Get());
}

TapGestureDetector TapGestureDetector::DownCast(BaseHandle handle)
{
  return TapGestureDetector(dynamic_cast<Dali::Internal::TapGestureDetector*>(handle.GetObjectPtr()));
}

TapGestureDetector::~TapGestureDetector() = default;

TapGestureDetector::TapGestureDetector(const TapGestureDetector& handle) = default;

TapGestureDetector& TapGestureDetector::operator=(const TapGestureDetector& rhs) = default;

TapGestureDetector::TapGestureDetector(TapGestureDetector&& handle) noexcept = default;

TapGestureDetector& TapGestureDetector::operator=(TapGestureDetector&& rhs) noexcept = default;

void TapGestureDetector::SetMinimumTapsRequired(uint32_t taps)
{
  GetImplementation(*this).SetMinimumTapsRequired(taps);
}

void TapGestureDetector::SetMaximumTapsRequired(uint32_t taps)
{
  GetImplementation(*this).SetMaximumTapsRequired(taps);
}

uint32_t TapGestureDetector::GetMinimumTapsRequired() const
{
  return GetImplementation(*this).GetMinimumTapsRequired();
}

uint32_t TapGestureDetector::GetMaximumTapsRequired() const
{
  return GetImplementation(*this).GetMaximumTapsRequired();
}

void TapGestureDetector::ReceiveAllTapEvents(bool receive)
{
  return GetImplementation(*this).ReceiveAllTapEvents(receive);
}

TapGestureDetector::DetectedSignalType& TapGestureDetector::DetectedSignal()
{
  return GetImplementation(*this).DetectedSignal();
}

bool TapGestureDetector::IsReceiveAllTapEventsEnabled() const
{
  return GetImplementation(*this).IsReceiveAllTapEventsEnabled();
}

TapGestureDetector::Options TapGestureDetector::GetDefaultOptions() const
{
  return Options(GetImplementation(*this).GetDefaultProfile());
}

void TapGestureDetector::SetDeviceOptions(const GestureDeviceSelector& selector, const Options& options)
{
  const Internal::TapGestureProfile& profile = options.GetProfile();
  DALI_LOG_RELEASE_INFO("detector=%p selector(match=%d class=%d subclass=%d name='%s') minimumTaps=%u maximumTaps=%u receiveAllTapEvents=%d\n",
                        &GetImplementation(*this), static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr(), profile.minimumTaps, profile.maximumTaps, profile.receiveAllTapEvents);
  GetImplementation(*this).SetDeviceProfile(selector, profile);
}

bool TapGestureDetector::GetDeviceOptions(const GestureDeviceSelector& selector, Options& options) const
{
  const Internal::TapGestureProfile* profile = GetImplementation(*this).GetDeviceProfile(selector);
  if(profile)
  {
    options = Options(*profile);
    return true;
  }
  return false;
}

void TapGestureDetector::ClearDeviceOptions(const GestureDeviceSelector& selector)
{
  DALI_LOG_RELEASE_INFO("detector=%p selector(match=%d class=%d subclass=%d name='%s')\n", &GetImplementation(*this), static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr());
  GetImplementation(*this).ClearDeviceProfile(selector);
}

// TapGestureDetector::Options

struct TapGestureDetector::Options::Impl
{
  Impl() = default;

  explicit Impl(const Internal::TapGestureProfile& profile)
  : mProfile(profile)
  {
  }

  Internal::TapGestureProfile mProfile;
};

TapGestureDetector::Options::Options()
: mImpl(MakeUnique<Impl>())
{
}

TapGestureDetector::Options::Options(const Internal::TapGestureProfile& profile)
: mImpl(MakeUnique<Impl>(profile))
{
}

TapGestureDetector::Options::Options(const Options& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_TAP_OPTIONS(rhs.mImpl);
  mImpl = MakeUnique<Impl>(*rhs.mImpl);
}

TapGestureDetector::Options::Options(Options&& rhs) noexcept = default;

TapGestureDetector::Options& TapGestureDetector::Options::operator=(const Options& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_TAP_OPTIONS(rhs.mImpl);
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

TapGestureDetector::Options& TapGestureDetector::Options::operator=(Options&& rhs) noexcept = default;

TapGestureDetector::Options::~Options() = default;

void TapGestureDetector::Options::SetMinimumTapsRequired(uint32_t minimumTaps)
{
  DALI_ASSERT_VALID_TAP_OPTIONS(mImpl);
  mImpl->mProfile.minimumTaps = minimumTaps;
}

void TapGestureDetector::Options::SetMaximumTapsRequired(uint32_t maximumTaps)
{
  DALI_ASSERT_VALID_TAP_OPTIONS(mImpl);
  mImpl->mProfile.maximumTaps = maximumTaps;
}

void TapGestureDetector::Options::SetReceiveAllTapEventsEnabled(bool enabled)
{
  DALI_ASSERT_VALID_TAP_OPTIONS(mImpl);
  mImpl->mProfile.receiveAllTapEvents = enabled;
}

uint32_t TapGestureDetector::Options::GetMinimumTapsRequired() const
{
  DALI_ASSERT_VALID_TAP_OPTIONS(mImpl);
  return mImpl->mProfile.minimumTaps;
}

uint32_t TapGestureDetector::Options::GetMaximumTapsRequired() const
{
  DALI_ASSERT_VALID_TAP_OPTIONS(mImpl);
  return mImpl->mProfile.maximumTaps;
}

bool TapGestureDetector::Options::IsReceiveAllTapEventsEnabled() const
{
  DALI_ASSERT_VALID_TAP_OPTIONS(mImpl);
  return mImpl->mProfile.receiveAllTapEvents;
}

const Internal::TapGestureProfile& TapGestureDetector::Options::GetProfile() const
{
  DALI_ASSERT_VALID_TAP_OPTIONS(mImpl);
  return mImpl->mProfile;
}

} //namespace DALI_NAMESPACE

#undef DALI_ASSERT_VALID_TAP_OPTIONS
