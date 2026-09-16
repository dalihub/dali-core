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
#include <dali/public-api/events/gesture-device-selector.h>

#define DALI_ASSERT_VALID_GESTURE_DEVICE_SELECTOR(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from GestureDeviceSelector object")

namespace DALI_NAMESPACE
{
struct GestureDeviceSelector::Impl
{
  Impl() = default;

  Impl(MatchType matchType, Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass, const Dali::String& deviceName)
  : mMatchType(matchType),
    mDeviceClass(deviceClass),
    mDeviceSubclass(deviceSubclass),
    mDeviceName(deviceName)
  {
  }

  MatchType              mMatchType{MatchType::DEVICE_CLASS};
  Device::Class::Type    mDeviceClass{Device::Class::NONE};
  Device::Subclass::Type mDeviceSubclass{Device::Subclass::NONE};
  Dali::String           mDeviceName;
};

GestureDeviceSelector GestureDeviceSelector::ByDeviceClass(Device::Class::Type deviceClass)
{
  GestureDeviceSelector selector;
  selector.mImpl = MakeUnique<Impl>(MatchType::DEVICE_CLASS, deviceClass, Device::Subclass::NONE, Dali::String());
  return selector;
}

GestureDeviceSelector GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass)
{
  GestureDeviceSelector selector;
  selector.mImpl = MakeUnique<Impl>(MatchType::DEVICE_CLASS_AND_SUBCLASS, deviceClass, deviceSubclass, Dali::String());
  return selector;
}

GestureDeviceSelector GestureDeviceSelector::ByDeviceName(const Dali::String& deviceName)
{
  DALI_ASSERT_ALWAYS(!deviceName.Empty() && "GestureDeviceSelector::ByDeviceName requires a non-empty device name");

  GestureDeviceSelector selector;
  selector.mImpl = MakeUnique<Impl>(MatchType::DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE, deviceName);
  return selector;
}

GestureDeviceSelector::GestureDeviceSelector()
: mImpl(MakeUnique<Impl>())
{
}

GestureDeviceSelector::GestureDeviceSelector(const GestureDeviceSelector& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_GESTURE_DEVICE_SELECTOR(rhs.mImpl);
  mImpl = MakeUnique<Impl>(*rhs.mImpl);
}

GestureDeviceSelector::GestureDeviceSelector(GestureDeviceSelector&& rhs) noexcept = default;

GestureDeviceSelector& GestureDeviceSelector::operator=(const GestureDeviceSelector& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_GESTURE_DEVICE_SELECTOR(rhs.mImpl);
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

GestureDeviceSelector& GestureDeviceSelector::operator=(GestureDeviceSelector&& rhs) noexcept = default;

GestureDeviceSelector::~GestureDeviceSelector() = default;

GestureDeviceSelector::MatchType GestureDeviceSelector::GetMatchType() const
{
  DALI_ASSERT_VALID_GESTURE_DEVICE_SELECTOR(mImpl);
  return mImpl->mMatchType;
}

Device::Class::Type GestureDeviceSelector::GetDeviceClass() const
{
  DALI_ASSERT_VALID_GESTURE_DEVICE_SELECTOR(mImpl);
  return mImpl->mDeviceClass;
}

Device::Subclass::Type GestureDeviceSelector::GetDeviceSubclass() const
{
  DALI_ASSERT_VALID_GESTURE_DEVICE_SELECTOR(mImpl);
  return mImpl->mDeviceSubclass;
}

Dali::String GestureDeviceSelector::GetDeviceName() const
{
  DALI_ASSERT_VALID_GESTURE_DEVICE_SELECTOR(mImpl);
  return mImpl->mDeviceName;
}

bool GestureDeviceSelector::operator==(const GestureDeviceSelector& rhs) const
{
  DALI_ASSERT_VALID_GESTURE_DEVICE_SELECTOR(mImpl);
  DALI_ASSERT_VALID_GESTURE_DEVICE_SELECTOR(rhs.mImpl);

  if(mImpl->mMatchType != rhs.mImpl->mMatchType)
  {
    return false;
  }

  switch(mImpl->mMatchType)
  {
    case MatchType::DEVICE_NAME:
    {
      return mImpl->mDeviceName == rhs.mImpl->mDeviceName;
    }
    case MatchType::DEVICE_CLASS:
    {
      return mImpl->mDeviceClass == rhs.mImpl->mDeviceClass;
    }
    case MatchType::DEVICE_CLASS_AND_SUBCLASS:
    {
      return mImpl->mDeviceClass == rhs.mImpl->mDeviceClass && mImpl->mDeviceSubclass == rhs.mImpl->mDeviceSubclass;
    }
  }
  return false;
}

bool GestureDeviceSelector::operator!=(const GestureDeviceSelector& rhs) const
{
  return !(*this == rhs);
}

} // namespace DALI_NAMESPACE

#undef DALI_ASSERT_VALID_GESTURE_DEVICE_SELECTOR
