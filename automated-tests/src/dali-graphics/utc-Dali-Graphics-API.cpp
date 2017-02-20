/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/graphics-api/graphics-api-manager.h>

using Dali::Graphics::API::Manager;

void utc_dali_internal_memorypoolobjectallocator_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_memorypoolobjectallocator_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

class TestManager : public Manager
{
public:
  virtual int GetNumber() const override
  {
    return 42;
  }
};

} // namespace

int UtcDaliGraphicsAPI(void)
{
  TestManager testManager;

  auto n = testManager.GetNumber();

  DALI_TEST_EQUALS(n, 42, TEST_LOCATION);

  END_TEST;
}
