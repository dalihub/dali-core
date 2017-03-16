/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <memory>

#include <dali-test-suite-utils.h>
#include <dali/graphics/graphics-object-owner.h>

using namespace Dali::Graphics;

void utc_dali_graphics_api_handle_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_graphics_api_handle_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

struct TestType final
{
  /**
   * @brief default conmstructor
   */
  TestType() = default;

  /**
   * @brief constructor
   */
  constexpr TestType(int value) : mValue(value)
  {
  }

  //data
  int mValue = 0;
};

struct TestTypeFactory final : public API::BaseFactory< TestType >
{
  virtual typename API::BaseFactory< TestType >::PointerType Create() const override
  {
    return typename API::BaseFactory< TestType >::PointerType{new TestType{42}};
  }
};

} // namespace

int UtcDaliGraphicsAPIHandle(void)
{
  ObjectOwner< TestType > objectOwner;

  auto handle = objectOwner.CreateObject(TestTypeFactory{});
  DALI_TEST_EQUALS(handle, 0, TEST_LOCATION);

  auto value = objectOwner[handle].mValue;

  DALI_TEST_CHECK(value == 42);

  END_TEST;
}
