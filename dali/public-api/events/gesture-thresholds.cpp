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
#include <dali/public-api/events/gesture-thresholds.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/events/gesture-threshold-values.h>
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-processor.h>
#include <dali/internal/event/events/pan-gesture/pan-gesture-processor.h>
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-processor.h>
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-processor.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-processor.h>

namespace DALI_NAMESPACE
{
namespace GestureThresholds
{
namespace
{
Internal::GestureEventProcessor& GetGestureEventProcessor()
{
  return Internal::ThreadLocalStorage::Get().GetGestureEventProcessor();
}
} // unnamed namespace

#define DALI_ASSERT_VALID_PAN_THRESHOLDS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from GestureThresholds::PanThresholds object")

struct PanThresholds::Impl
{
  Impl() = default;

  explicit Impl(const Internal::PanThresholdValues& values)
  : mValues(values)
  {
  }

  Internal::PanThresholdValues mValues;
};

PanThresholds::PanThresholds()
: mImpl(MakeUnique<Impl>())
{
}

PanThresholds::PanThresholds(const Internal::PanThresholdValues& values)
: mImpl(MakeUnique<Impl>(values))
{
}

PanThresholds::PanThresholds(const PanThresholds& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_PAN_THRESHOLDS(rhs.mImpl);
  mImpl = MakeUnique<Impl>(*rhs.mImpl);
}

PanThresholds::PanThresholds(PanThresholds&& rhs) noexcept = default;

PanThresholds& PanThresholds::operator=(const PanThresholds& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_PAN_THRESHOLDS(rhs.mImpl);
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

PanThresholds& PanThresholds::operator=(PanThresholds&& rhs) noexcept = default;

PanThresholds::~PanThresholds() = default;

void PanThresholds::SetMinimumDistance(int32_t value)
{
  DALI_ASSERT_VALID_PAN_THRESHOLDS(mImpl);
  mImpl->mValues.minimumDistance = value;
}

int32_t PanThresholds::GetMinimumDistance() const
{
  DALI_ASSERT_VALID_PAN_THRESHOLDS(mImpl);
  return mImpl->mValues.minimumDistance;
}

void PanThresholds::SetMinimumPanEvents(int32_t value)
{
  DALI_ASSERT_VALID_PAN_THRESHOLDS(mImpl);
  mImpl->mValues.minimumPanEvents = value;
}

int32_t PanThresholds::GetMinimumPanEvents() const
{
  DALI_ASSERT_VALID_PAN_THRESHOLDS(mImpl);
  return mImpl->mValues.minimumPanEvents;
}

const Internal::PanThresholdValues& PanThresholds::GetValues() const
{
  DALI_ASSERT_VALID_PAN_THRESHOLDS(mImpl);
  return mImpl->mValues;
}

PanThresholds GetDefaultPanThresholds()
{
  const Internal::PanGestureProcessor& processor = GetGestureEventProcessor().GetPanGestureProcessor();
  Internal::PanThresholdValues         values;
  values.minimumDistance  = processor.GetMinimumDistance();
  values.minimumPanEvents = processor.GetMinimumPanEvents();
  return PanThresholds(values);
}

void SetPanThresholds(const GestureDeviceSelector& selector, const PanThresholds& thresholds)
{
  const Internal::PanThresholdValues& values = thresholds.GetValues();
  DALI_ASSERT_ALWAYS(values.minimumDistance >= 0 && "Pan thresholds require a minimum distance >= 0");
  DALI_ASSERT_ALWAYS(values.minimumPanEvents >= 1 && "Pan thresholds require minimum pan events >= 1");
  DALI_LOG_RELEASE_INFO("selector(match=%d class=%d subclass=%d name='%s') minimumDistance=%d minimumPanEvents=%d\n", static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr(), values.minimumDistance, values.minimumPanEvents);
  GetGestureEventProcessor().SetPanDeviceThresholds(selector, values);
}

bool GetPanThresholds(const GestureDeviceSelector& selector, PanThresholds& thresholds)
{
  const Internal::PanThresholdValues* values = GetGestureEventProcessor().GetPanDeviceThresholds(selector);
  if(values)
  {
    thresholds = PanThresholds(*values);
    return true;
  }
  return false;
}

void ClearPanThresholds(const GestureDeviceSelector& selector)
{
  DALI_LOG_RELEASE_INFO("selector(match=%d class=%d subclass=%d name='%s')\n", static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr());
  GetGestureEventProcessor().ClearPanDeviceThresholds(selector);
}

#undef DALI_ASSERT_VALID_PAN_THRESHOLDS

#define DALI_ASSERT_VALID_TAP_THRESHOLDS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from GestureThresholds::TapThresholds object")

struct TapThresholds::Impl
{
  Impl() = default;

  explicit Impl(const Internal::TapThresholdValues& values)
  : mValues(values)
  {
  }

  Internal::TapThresholdValues mValues;
};

TapThresholds::TapThresholds()
: mImpl(MakeUnique<Impl>())
{
}

TapThresholds::TapThresholds(const Internal::TapThresholdValues& values)
: mImpl(MakeUnique<Impl>(values))
{
}

TapThresholds::TapThresholds(const TapThresholds& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_TAP_THRESHOLDS(rhs.mImpl);
  mImpl = MakeUnique<Impl>(*rhs.mImpl);
}

TapThresholds::TapThresholds(TapThresholds&& rhs) noexcept = default;

TapThresholds& TapThresholds::operator=(const TapThresholds& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_TAP_THRESHOLDS(rhs.mImpl);
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

TapThresholds& TapThresholds::operator=(TapThresholds&& rhs) noexcept = default;

TapThresholds::~TapThresholds() = default;

void TapThresholds::SetMaximumMultiTapInterval(uint32_t value)
{
  DALI_ASSERT_VALID_TAP_THRESHOLDS(mImpl);
  mImpl->mValues.maximumMultiTapInterval = value;
}

uint32_t TapThresholds::GetMaximumMultiTapInterval() const
{
  DALI_ASSERT_VALID_TAP_THRESHOLDS(mImpl);
  return mImpl->mValues.maximumMultiTapInterval;
}

void TapThresholds::SetMaximumHoldingTime(uint32_t value)
{
  DALI_ASSERT_VALID_TAP_THRESHOLDS(mImpl);
  mImpl->mValues.maximumHoldingTime = value;
}

uint32_t TapThresholds::GetMaximumHoldingTime() const
{
  DALI_ASSERT_VALID_TAP_THRESHOLDS(mImpl);
  return mImpl->mValues.maximumHoldingTime;
}

void TapThresholds::SetMaximumMotionDistance(float value)
{
  DALI_ASSERT_VALID_TAP_THRESHOLDS(mImpl);
  mImpl->mValues.maximumMotionDistance = value;
}

float TapThresholds::GetMaximumMotionDistance() const
{
  DALI_ASSERT_VALID_TAP_THRESHOLDS(mImpl);
  return mImpl->mValues.maximumMotionDistance;
}

const Internal::TapThresholdValues& TapThresholds::GetValues() const
{
  DALI_ASSERT_VALID_TAP_THRESHOLDS(mImpl);
  return mImpl->mValues;
}

TapThresholds GetDefaultTapThresholds()
{
  Internal::GestureEventProcessor& processor = GetGestureEventProcessor();
  Internal::TapThresholdValues     values;
  values.maximumMultiTapInterval = processor.GetTapGestureMaximumMultiTapInterval();
  values.maximumHoldingTime      = processor.GetTapGestureMaximumHoldingTime();
  values.maximumMotionDistance   = processor.GetTapGestureMaximumMotionDistance();
  return TapThresholds(values);
}

void SetTapThresholds(const GestureDeviceSelector& selector, const TapThresholds& thresholds)
{
  const Internal::TapThresholdValues& values = thresholds.GetValues();
  DALI_ASSERT_ALWAYS(values.maximumMultiTapInterval > 0u && "Tap thresholds require a multi tap interval > 0");
  DALI_ASSERT_ALWAYS(values.maximumHoldingTime > 0u && "Tap thresholds require a holding time > 0");
  DALI_ASSERT_ALWAYS(values.maximumMotionDistance >= 0.0f && "Tap thresholds require a motion distance >= 0");
  DALI_LOG_RELEASE_INFO("selector(match=%d class=%d subclass=%d name='%s') maximumMultiTapInterval=%u maximumHoldingTime=%u maximumMotionDistance=%f\n", static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr(), values.maximumMultiTapInterval, values.maximumHoldingTime, values.maximumMotionDistance);
  GetGestureEventProcessor().SetTapDeviceThresholds(selector, values);
}

bool GetTapThresholds(const GestureDeviceSelector& selector, TapThresholds& thresholds)
{
  const Internal::TapThresholdValues* values = GetGestureEventProcessor().GetTapDeviceThresholds(selector);
  if(values)
  {
    thresholds = TapThresholds(*values);
    return true;
  }
  return false;
}

void ClearTapThresholds(const GestureDeviceSelector& selector)
{
  DALI_LOG_RELEASE_INFO("selector(match=%d class=%d subclass=%d name='%s')\n", static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr());
  GetGestureEventProcessor().ClearTapDeviceThresholds(selector);
}

#undef DALI_ASSERT_VALID_TAP_THRESHOLDS

#define DALI_ASSERT_VALID_LONGPRESS_THRESHOLDS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from GestureThresholds::LongPressThresholds object")

struct LongPressThresholds::Impl
{
  Impl() = default;

  explicit Impl(const Internal::LongPressThresholdValues& values)
  : mValues(values)
  {
  }

  Internal::LongPressThresholdValues mValues;
};

LongPressThresholds::LongPressThresholds()
: mImpl(MakeUnique<Impl>())
{
}

LongPressThresholds::LongPressThresholds(const Internal::LongPressThresholdValues& values)
: mImpl(MakeUnique<Impl>(values))
{
}

LongPressThresholds::LongPressThresholds(const LongPressThresholds& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_LONGPRESS_THRESHOLDS(rhs.mImpl);
  mImpl = MakeUnique<Impl>(*rhs.mImpl);
}

LongPressThresholds::LongPressThresholds(LongPressThresholds&& rhs) noexcept = default;

LongPressThresholds& LongPressThresholds::operator=(const LongPressThresholds& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_LONGPRESS_THRESHOLDS(rhs.mImpl);
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

LongPressThresholds& LongPressThresholds::operator=(LongPressThresholds&& rhs) noexcept = default;

LongPressThresholds::~LongPressThresholds() = default;

void LongPressThresholds::SetMinimumHoldingTime(uint32_t value)
{
  DALI_ASSERT_VALID_LONGPRESS_THRESHOLDS(mImpl);
  mImpl->mValues.minimumHoldingTime = value;
}

uint32_t LongPressThresholds::GetMinimumHoldingTime() const
{
  DALI_ASSERT_VALID_LONGPRESS_THRESHOLDS(mImpl);
  return mImpl->mValues.minimumHoldingTime;
}

const Internal::LongPressThresholdValues& LongPressThresholds::GetValues() const
{
  DALI_ASSERT_VALID_LONGPRESS_THRESHOLDS(mImpl);
  return mImpl->mValues;
}

LongPressThresholds GetDefaultLongPressThresholds()
{
  Internal::LongPressThresholdValues values;
  values.minimumHoldingTime = GetGestureEventProcessor().GetLongPressGestureMinimumHoldingTime();
  return LongPressThresholds(values);
}

void SetLongPressThresholds(const GestureDeviceSelector& selector, const LongPressThresholds& thresholds)
{
  const Internal::LongPressThresholdValues& values = thresholds.GetValues();
  DALI_ASSERT_ALWAYS(values.minimumHoldingTime > 0u && "Long press thresholds require a holding time > 0");
  DALI_LOG_RELEASE_INFO("selector(match=%d class=%d subclass=%d name='%s') minimumHoldingTime=%u\n", static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr(), values.minimumHoldingTime);
  GetGestureEventProcessor().SetLongPressDeviceThresholds(selector, values);
}

bool GetLongPressThresholds(const GestureDeviceSelector& selector, LongPressThresholds& thresholds)
{
  const Internal::LongPressThresholdValues* values = GetGestureEventProcessor().GetLongPressDeviceThresholds(selector);
  if(values)
  {
    thresholds = LongPressThresholds(*values);
    return true;
  }
  return false;
}

void ClearLongPressThresholds(const GestureDeviceSelector& selector)
{
  DALI_LOG_RELEASE_INFO("selector(match=%d class=%d subclass=%d name='%s')\n", static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr());
  GetGestureEventProcessor().ClearLongPressDeviceThresholds(selector);
}

#undef DALI_ASSERT_VALID_LONGPRESS_THRESHOLDS

#define DALI_ASSERT_VALID_PINCH_THRESHOLDS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from GestureThresholds::PinchThresholds object")

struct PinchThresholds::Impl
{
  Impl() = default;

  explicit Impl(const Internal::PinchThresholdValues& values)
  : mValues(values)
  {
  }

  Internal::PinchThresholdValues mValues;
};

PinchThresholds::PinchThresholds()
: mImpl(MakeUnique<Impl>())
{
}

PinchThresholds::PinchThresholds(const Internal::PinchThresholdValues& values)
: mImpl(MakeUnique<Impl>(values))
{
}

PinchThresholds::PinchThresholds(const PinchThresholds& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_PINCH_THRESHOLDS(rhs.mImpl);
  mImpl = MakeUnique<Impl>(*rhs.mImpl);
}

PinchThresholds::PinchThresholds(PinchThresholds&& rhs) noexcept = default;

PinchThresholds& PinchThresholds::operator=(const PinchThresholds& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_PINCH_THRESHOLDS(rhs.mImpl);
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

PinchThresholds& PinchThresholds::operator=(PinchThresholds&& rhs) noexcept = default;

PinchThresholds::~PinchThresholds() = default;

void PinchThresholds::SetMinimumDistance(float value)
{
  DALI_ASSERT_VALID_PINCH_THRESHOLDS(mImpl);
  mImpl->mValues.minimumDistance = value;
}

float PinchThresholds::GetMinimumDistance() const
{
  DALI_ASSERT_VALID_PINCH_THRESHOLDS(mImpl);
  return mImpl->mValues.minimumDistance;
}

void PinchThresholds::SetMinimumTouchEvents(uint32_t value)
{
  DALI_ASSERT_VALID_PINCH_THRESHOLDS(mImpl);
  mImpl->mValues.minimumTouchEvents = value;
}

uint32_t PinchThresholds::GetMinimumTouchEvents() const
{
  DALI_ASSERT_VALID_PINCH_THRESHOLDS(mImpl);
  return mImpl->mValues.minimumTouchEvents;
}

void PinchThresholds::SetMinimumTouchEventsAfterStart(uint32_t value)
{
  DALI_ASSERT_VALID_PINCH_THRESHOLDS(mImpl);
  mImpl->mValues.minimumTouchEventsAfterStart = value;
}

uint32_t PinchThresholds::GetMinimumTouchEventsAfterStart() const
{
  DALI_ASSERT_VALID_PINCH_THRESHOLDS(mImpl);
  return mImpl->mValues.minimumTouchEventsAfterStart;
}

const Internal::PinchThresholdValues& PinchThresholds::GetValues() const
{
  DALI_ASSERT_VALID_PINCH_THRESHOLDS(mImpl);
  return mImpl->mValues;
}

PinchThresholds GetDefaultPinchThresholds()
{
  const Internal::PinchGestureProcessor& processor = GetGestureEventProcessor().GetPinchGestureProcessor();
  Internal::PinchThresholdValues         values;
  values.minimumDistance              = processor.GetMinimumPinchDistance();
  values.minimumTouchEvents           = processor.GetMinimumTouchEvents();
  values.minimumTouchEventsAfterStart = processor.GetMinimumTouchEventsAfterStart();
  return PinchThresholds(values);
}

void SetPinchThresholds(const GestureDeviceSelector& selector, const PinchThresholds& thresholds)
{
  const Internal::PinchThresholdValues& values = thresholds.GetValues();
  DALI_ASSERT_ALWAYS(values.minimumTouchEvents > 1u && "Pinch thresholds require minimum touch events > 1");
  DALI_ASSERT_ALWAYS(values.minimumTouchEventsAfterStart > 1u && "Pinch thresholds require minimum touch events after start > 1");
  DALI_LOG_RELEASE_INFO("selector(match=%d class=%d subclass=%d name='%s') minimumDistance=%f minimumTouchEvents=%u minimumTouchEventsAfterStart=%u\n", static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr(), values.minimumDistance, values.minimumTouchEvents, values.minimumTouchEventsAfterStart);
  GetGestureEventProcessor().SetPinchDeviceThresholds(selector, values);
}

bool GetPinchThresholds(const GestureDeviceSelector& selector, PinchThresholds& thresholds)
{
  const Internal::PinchThresholdValues* values = GetGestureEventProcessor().GetPinchDeviceThresholds(selector);
  if(values)
  {
    thresholds = PinchThresholds(*values);
    return true;
  }
  return false;
}

void ClearPinchThresholds(const GestureDeviceSelector& selector)
{
  DALI_LOG_RELEASE_INFO("selector(match=%d class=%d subclass=%d name='%s')\n", static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr());
  GetGestureEventProcessor().ClearPinchDeviceThresholds(selector);
}

#undef DALI_ASSERT_VALID_PINCH_THRESHOLDS

#define DALI_ASSERT_VALID_ROTATION_THRESHOLDS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from GestureThresholds::RotationThresholds object")

struct RotationThresholds::Impl
{
  Impl() = default;

  explicit Impl(const Internal::RotationThresholdValues& values)
  : mValues(values)
  {
  }

  Internal::RotationThresholdValues mValues;
};

RotationThresholds::RotationThresholds()
: mImpl(MakeUnique<Impl>())
{
}

RotationThresholds::RotationThresholds(const Internal::RotationThresholdValues& values)
: mImpl(MakeUnique<Impl>(values))
{
}

RotationThresholds::RotationThresholds(const RotationThresholds& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_ROTATION_THRESHOLDS(rhs.mImpl);
  mImpl = MakeUnique<Impl>(*rhs.mImpl);
}

RotationThresholds::RotationThresholds(RotationThresholds&& rhs) noexcept = default;

RotationThresholds& RotationThresholds::operator=(const RotationThresholds& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_ROTATION_THRESHOLDS(rhs.mImpl);
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

RotationThresholds& RotationThresholds::operator=(RotationThresholds&& rhs) noexcept = default;

RotationThresholds::~RotationThresholds() = default;

void RotationThresholds::SetMinimumTouchEvents(uint32_t value)
{
  DALI_ASSERT_VALID_ROTATION_THRESHOLDS(mImpl);
  mImpl->mValues.minimumTouchEvents = value;
}

uint32_t RotationThresholds::GetMinimumTouchEvents() const
{
  DALI_ASSERT_VALID_ROTATION_THRESHOLDS(mImpl);
  return mImpl->mValues.minimumTouchEvents;
}

void RotationThresholds::SetMinimumTouchEventsAfterStart(uint32_t value)
{
  DALI_ASSERT_VALID_ROTATION_THRESHOLDS(mImpl);
  mImpl->mValues.minimumTouchEventsAfterStart = value;
}

uint32_t RotationThresholds::GetMinimumTouchEventsAfterStart() const
{
  DALI_ASSERT_VALID_ROTATION_THRESHOLDS(mImpl);
  return mImpl->mValues.minimumTouchEventsAfterStart;
}

const Internal::RotationThresholdValues& RotationThresholds::GetValues() const
{
  DALI_ASSERT_VALID_ROTATION_THRESHOLDS(mImpl);
  return mImpl->mValues;
}

RotationThresholds GetDefaultRotationThresholds()
{
  const Internal::RotationGestureProcessor& processor = GetGestureEventProcessor().GetRotationGestureProcessor();
  Internal::RotationThresholdValues         values;
  values.minimumTouchEvents           = processor.GetMinimumTouchEvents();
  values.minimumTouchEventsAfterStart = processor.GetMinimumTouchEventsAfterStart();
  return RotationThresholds(values);
}

void SetRotationThresholds(const GestureDeviceSelector& selector, const RotationThresholds& thresholds)
{
  const Internal::RotationThresholdValues& values = thresholds.GetValues();
  DALI_ASSERT_ALWAYS(values.minimumTouchEvents > 1u && "Rotation thresholds require minimum touch events > 1");
  DALI_ASSERT_ALWAYS(values.minimumTouchEventsAfterStart > 1u && "Rotation thresholds require minimum touch events after start > 1");
  DALI_LOG_RELEASE_INFO("selector(match=%d class=%d subclass=%d name='%s') minimumTouchEvents=%u minimumTouchEventsAfterStart=%u\n", static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr(), values.minimumTouchEvents, values.minimumTouchEventsAfterStart);
  GetGestureEventProcessor().SetRotationDeviceThresholds(selector, values);
}

bool GetRotationThresholds(const GestureDeviceSelector& selector, RotationThresholds& thresholds)
{
  const Internal::RotationThresholdValues* values = GetGestureEventProcessor().GetRotationDeviceThresholds(selector);
  if(values)
  {
    thresholds = RotationThresholds(*values);
    return true;
  }
  return false;
}

void ClearRotationThresholds(const GestureDeviceSelector& selector)
{
  DALI_LOG_RELEASE_INFO("selector(match=%d class=%d subclass=%d name='%s')\n", static_cast<int>(selector.GetMatchType()), static_cast<int>(selector.GetDeviceClass()), static_cast<int>(selector.GetDeviceSubclass()), selector.GetDeviceName().CStr());
  GetGestureEventProcessor().ClearRotationDeviceThresholds(selector);
}

#undef DALI_ASSERT_VALID_ROTATION_THRESHOLDS

} // namespace GestureThresholds

} // namespace DALI_NAMESPACE
