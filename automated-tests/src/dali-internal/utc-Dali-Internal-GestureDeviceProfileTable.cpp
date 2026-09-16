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

#include <dali-test-suite-utils.h>
#include <dali/integration-api/events/point.h>
#include <dali/internal/event/events/gesture-device-profile-table.h>
#include <dali/internal/event/events/gesture-input-source.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

using namespace Dali;

void utc_dali_internal_gesture_device_profile_table_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_gesture_device_profile_table_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
Internal::GestureInputSource MakeSource(Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass, const char* deviceName)
{
  Integration::Point point;
  point.SetDeviceClass(deviceClass);
  point.SetDeviceSubclass(deviceSubclass);
  point.SetDeviceName(Dali::String(deviceName));
  return Internal::GestureInputSource::FromPoint(point);
}

const GestureDeviceSelector TOUCH_CLASS  = GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH);
const GestureDeviceSelector TOUCH_FINGER = GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::TOUCH, Device::Subclass::FINGER);
const GestureDeviceSelector NAMED_REMOTE = GestureDeviceSelector::ByDeviceName(Dali::String("Pointing Device"));
} // namespace

int UtcDaliGestureInputSourceFromPointP(void)
{
  TestApplication application;

  Internal::GestureInputSource defaultSource;
  DALI_TEST_CHECK(!defaultSource.valid);
  DALI_TEST_EQUALS(defaultSource.deviceClass, Device::Class::NONE, TEST_LOCATION);

  Internal::GestureInputSource source = MakeSource(Device::Class::POINTER, Device::Subclass::REMOCON, "Pointing Device");
  DALI_TEST_CHECK(source.valid);
  DALI_TEST_EQUALS(source.deviceClass, Device::Class::POINTER, TEST_LOCATION);
  DALI_TEST_EQUALS(source.deviceSubclass, Device::Subclass::REMOCON, TEST_LOCATION);
  DALI_TEST_EQUALS(source.deviceName, "Pointing Device", TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureInputSourceMatchesP(void)
{
  TestApplication application;

  Internal::GestureInputSource named   = MakeSource(Device::Class::POINTER, Device::Subclass::REMOCON, "Pointing Device");
  Internal::GestureInputSource unnamed = MakeSource(Device::Class::TOUCH, Device::Subclass::FINGER, "");

  DALI_TEST_CHECK(named.Matches(NAMED_REMOTE));
  DALI_TEST_CHECK(named.Matches(GestureDeviceSelector::ByDeviceClass(Device::Class::POINTER)));
  DALI_TEST_CHECK(named.Matches(GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::POINTER, Device::Subclass::REMOCON)));
  DALI_TEST_CHECK(!named.Matches(TOUCH_CLASS));
  DALI_TEST_CHECK(!named.Matches(GestureDeviceSelector::ByDeviceName(Dali::String("pointing device")))); // case sensitive

  // A source without a name never matches a name selector, but still matches class selectors.
  DALI_TEST_CHECK(!unnamed.Matches(NAMED_REMOTE));
  DALI_TEST_CHECK(unnamed.Matches(TOUCH_CLASS));
  DALI_TEST_CHECK(unnamed.Matches(TOUCH_FINGER));
  DALI_TEST_CHECK(!unnamed.Matches(GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::TOUCH, Device::Subclass::PALM)));

  END_TEST;
}

int UtcDaliGestureDeviceProfileTableSetFindClearP(void)
{
  TestApplication application;

  Internal::GestureDeviceProfileTable<int> table;
  DALI_TEST_CHECK(table.Empty());
  DALI_TEST_CHECK(table.Find(TOUCH_CLASS) == nullptr);

  table.Set(TOUCH_CLASS, 1);
  table.Set(TOUCH_FINGER, 2);
  table.Set(NAMED_REMOTE, 3);
  DALI_TEST_CHECK(!table.Empty());
  DALI_TEST_EQUALS(*table.Find(TOUCH_CLASS), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(*table.Find(TOUCH_FINGER), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(*table.Find(NAMED_REMOTE), 3, TEST_LOCATION);

  // Find is exact: a class selector does not find a class+subclass entry and vice versa.
  DALI_TEST_CHECK(table.Find(GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE)) == nullptr);
  DALI_TEST_CHECK(table.Find(GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::TOUCH, Device::Subclass::NONE)) == nullptr);

  // Set replaces the profile for the same selector instead of adding a second one.
  table.Set(TOUCH_CLASS, 10);
  DALI_TEST_EQUALS(*table.Find(TOUCH_CLASS), 10, TEST_LOCATION);

  DALI_TEST_CHECK(table.Clear(TOUCH_FINGER));
  DALI_TEST_CHECK(table.Find(TOUCH_FINGER) == nullptr);
  DALI_TEST_CHECK(!table.Clear(TOUCH_FINGER)); // already gone: no-op

  table.ClearAll();
  DALI_TEST_CHECK(table.Empty());
  DALI_TEST_CHECK(table.Find(NAMED_REMOTE) == nullptr);

  END_TEST;
}

int UtcDaliGestureDeviceProfileTableResolvePrecedenceP(void)
{
  TestApplication application;

  Internal::GestureDeviceProfileTable<int> table;
  table.Set(TOUCH_CLASS, 1);
  table.Set(TOUCH_FINGER, 2);
  table.Set(GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::POINTER, Device::Subclass::REMOCON), 4);
  table.Set(NAMED_REMOTE, 3);

  // Name beats class+subclass beats class.
  DALI_TEST_EQUALS(*table.Resolve(MakeSource(Device::Class::POINTER, Device::Subclass::REMOCON, "Pointing Device")), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(*table.Resolve(MakeSource(Device::Class::POINTER, Device::Subclass::REMOCON, "Other Remote")), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(*table.Resolve(MakeSource(Device::Class::TOUCH, Device::Subclass::FINGER, "")), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(*table.Resolve(MakeSource(Device::Class::TOUCH, Device::Subclass::PALM, "")), 1, TEST_LOCATION);

  // A name entry applies regardless of the class the device reports.
  DALI_TEST_EQUALS(*table.Resolve(MakeSource(Device::Class::MOUSE, Device::Subclass::NONE, "Pointing Device")), 3, TEST_LOCATION);

  // No match at all falls through to the caller's default.
  DALI_TEST_CHECK(table.Resolve(MakeSource(Device::Class::MOUSE, Device::Subclass::NONE, "")) == nullptr);

  // NONE is a valid class to register for platforms without device metadata.
  table.Set(GestureDeviceSelector::ByDeviceClass(Device::Class::NONE), 5);
  DALI_TEST_EQUALS(*table.Resolve(MakeSource(Device::Class::NONE, Device::Subclass::NONE, "")), 5, TEST_LOCATION);

  END_TEST;
}
