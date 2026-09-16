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
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <utility>

using namespace Dali;

void utc_dali_gesture_device_selector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gesture_device_selector_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliGestureDeviceSelectorByDeviceClassP(void)
{
  TestApplication application;

  GestureDeviceSelector selector = GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE);
  DALI_TEST_EQUALS(selector.GetMatchType(), GestureDeviceSelector::MatchType::DEVICE_CLASS, TEST_LOCATION);
  DALI_TEST_EQUALS(selector.GetDeviceClass(), Device::Class::MOUSE, TEST_LOCATION);
  DALI_TEST_EQUALS(selector.GetDeviceSubclass(), Device::Subclass::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(selector.GetDeviceName().Empty());

  // NONE is a valid class for platforms that report no device metadata.
  GestureDeviceSelector none = GestureDeviceSelector::ByDeviceClass(Device::Class::NONE);
  DALI_TEST_EQUALS(none.GetDeviceClass(), Device::Class::NONE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureDeviceSelectorByDeviceClassAndSubclassP(void)
{
  TestApplication application;

  GestureDeviceSelector selector = GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::POINTER, Device::Subclass::REMOCON);
  DALI_TEST_EQUALS(selector.GetMatchType(), GestureDeviceSelector::MatchType::DEVICE_CLASS_AND_SUBCLASS, TEST_LOCATION);
  DALI_TEST_EQUALS(selector.GetDeviceClass(), Device::Class::POINTER, TEST_LOCATION);
  DALI_TEST_EQUALS(selector.GetDeviceSubclass(), Device::Subclass::REMOCON, TEST_LOCATION);
  DALI_TEST_CHECK(selector.GetDeviceName().Empty());

  END_TEST;
}

int UtcDaliGestureDeviceSelectorByDeviceNameP(void)
{
  TestApplication application;

  GestureDeviceSelector selector = GestureDeviceSelector::ByDeviceName(Dali::String("Pointing Device"));
  DALI_TEST_EQUALS(selector.GetMatchType(), GestureDeviceSelector::MatchType::DEVICE_NAME, TEST_LOCATION);
  DALI_TEST_EQUALS(selector.GetDeviceName(), "Pointing Device", TEST_LOCATION);
  DALI_TEST_EQUALS(selector.GetDeviceClass(), Device::Class::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(selector.GetDeviceSubclass(), Device::Subclass::NONE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureDeviceSelectorByDeviceNameEmptyN(void)
{
  TestApplication application;

  DALI_TEST_ASSERTION(GestureDeviceSelector::ByDeviceName(Dali::String("")), "requires a non-empty device name");

  END_TEST;
}

int UtcDaliGestureDeviceSelectorDefaultConstructorP(void)
{
  TestApplication application;

  GestureDeviceSelector selector;
  DALI_TEST_EQUALS(selector.GetMatchType(), GestureDeviceSelector::MatchType::DEVICE_CLASS, TEST_LOCATION);
  DALI_TEST_EQUALS(selector.GetDeviceClass(), Device::Class::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(selector == GestureDeviceSelector::ByDeviceClass(Device::Class::NONE));

  END_TEST;
}

int UtcDaliGestureDeviceSelectorEqualityP(void)
{
  TestApplication application;

  // Same match type and same relevant fields
  DALI_TEST_CHECK(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH) == GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH));
  DALI_TEST_CHECK(GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::TOUCH, Device::Subclass::FINGER) ==
                  GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::TOUCH, Device::Subclass::FINGER));
  DALI_TEST_CHECK(GestureDeviceSelector::ByDeviceName(Dali::String("Pointing Device")) == GestureDeviceSelector::ByDeviceName(Dali::String("Pointing Device")));

  // Different relevant fields
  DALI_TEST_CHECK(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH) != GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE));
  DALI_TEST_CHECK(GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::TOUCH, Device::Subclass::FINGER) !=
                  GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::TOUCH, Device::Subclass::PALM));

  // Names are exact and case sensitive
  DALI_TEST_CHECK(GestureDeviceSelector::ByDeviceName(Dali::String("Pointing Device")) != GestureDeviceSelector::ByDeviceName(Dali::String("pointing device")));
  DALI_TEST_CHECK(GestureDeviceSelector::ByDeviceName(Dali::String("Pointing Device")) != GestureDeviceSelector::ByDeviceName(Dali::String("Pointing Device 2")));

  // Different match types never compare equal, even when the class fields coincide
  DALI_TEST_CHECK(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH) !=
                  GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::TOUCH, Device::Subclass::NONE));

  END_TEST;
}

int UtcDaliGestureDeviceSelectorCopyAndMoveP(void)
{
  TestApplication application;

  GestureDeviceSelector original = GestureDeviceSelector::ByDeviceName(Dali::String("Pointing Device"));

  GestureDeviceSelector copied(original);
  DALI_TEST_CHECK(copied == original);
  DALI_TEST_EQUALS(copied.GetDeviceName(), "Pointing Device", TEST_LOCATION);

  GestureDeviceSelector assigned;
  assigned = original;
  DALI_TEST_CHECK(assigned == original);

  GestureDeviceSelector moved(std::move(copied));
  DALI_TEST_CHECK(moved == original);
  DALI_TEST_ASSERTION(copied.GetMatchType(), "moved-from GestureDeviceSelector");

  GestureDeviceSelector moveAssigned;
  moveAssigned = std::move(assigned);
  DALI_TEST_CHECK(moveAssigned == original);
  DALI_TEST_ASSERTION(assigned.GetDeviceName(), "moved-from GestureDeviceSelector");

  END_TEST;
}
