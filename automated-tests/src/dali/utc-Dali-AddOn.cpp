/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/addon-binder.h>

#include "dali-test-suite-utils/test-addon-manager.h"

struct DummyAddOn : public Dali::AddOn::AddOnBinder
{
  DummyAddOn()
  : Dali::AddOn::AddOnBinder("SampleAddOn")
  {
  }

  ~DummyAddOn() override = default;

  ADDON_BIND_FUNCTION(DoSum, int(int, int));

  ADDON_BIND_FUNCTION(StringLen, int());
};

int UtcDaliAddOnBinderP(void)
{
  TestApplication application;

  auto* addOnManager = new Test::AddOnManager();

  tet_infoline("Testing Dali::AddOn::AddOnBinder");

  DummyAddOn addon;

  // Test whether library handle is non-null
  DALI_TEST_EQUALS(addon.GetHandle(), (void*)1, TEST_LOCATION);

  // Test whether addon is valid
  auto isValid = addon.IsValid();
  DALI_TEST_EQUALS(isValid, true, TEST_LOCATION);

  // Test AddOnInfo
  const auto& info = addon.GetAddOnInfo();
  DALI_TEST_EQUALS(info.name, "SampleAddOn", TEST_LOCATION);

  delete addOnManager;

  END_TEST;
}

int UtcDaliAddOnManagerNotSupportedP(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::AddOn::AddOnBinder when AddOnManager not supported");

  // Not supported
  using VoidPtr = void*;
  DALI_TEST_EQUALS(VoidPtr(Dali::Integration::AddOnManager::Get()), VoidPtr(nullptr), TEST_LOCATION);

  DummyAddOn addon{};

  // Test whether library handle is non-null
  DALI_TEST_EQUALS(addon.GetHandle(), (void*)0, TEST_LOCATION);

  // Test whether addon is valid
  auto isValid = addon.IsValid();
  DALI_TEST_EQUALS(isValid, false, TEST_LOCATION);

  END_TEST;
}
