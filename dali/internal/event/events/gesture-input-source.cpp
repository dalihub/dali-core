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
#include <dali/internal/event/events/gesture-input-source.h>

// INTERNAL INCLUDES
#include <dali/integration-api/events/point.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
GestureInputSource GestureInputSource::FromPoint(const Integration::Point& point)
{
  GestureInputSource source;
  source.deviceClass    = point.GetDeviceClass();
  source.deviceSubclass = point.GetDeviceSubclass();
  source.deviceName     = point.GetDeviceName();
  source.valid          = true;
  return source;
}

bool GestureInputSource::IsSameDevice(const GestureInputSource& other) const
{
  return deviceClass == other.deviceClass && deviceSubclass == other.deviceSubclass && deviceName == other.deviceName;
}

bool GestureInputSource::Matches(const GestureDeviceSelector& selector) const
{
  switch(selector.GetMatchType())
  {
    case GestureDeviceSelector::MatchType::DEVICE_NAME:
    {
      return !deviceName.Empty() && deviceName == selector.GetDeviceName();
    }
    case GestureDeviceSelector::MatchType::DEVICE_CLASS:
    {
      return deviceClass == selector.GetDeviceClass();
    }
    case GestureDeviceSelector::MatchType::DEVICE_CLASS_AND_SUBCLASS:
    {
      return deviceClass == selector.GetDeviceClass() && deviceSubclass == selector.GetDeviceSubclass();
    }
  }
  return false;
}

} // namespace Internal

} // namespace DALI_NAMESPACE
