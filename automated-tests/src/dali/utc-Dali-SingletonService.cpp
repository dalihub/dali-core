/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/devel-api/common/singleton-service.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

namespace
{
class TestHandle : public BaseHandle
{
public:
  TestHandle()
  {
  }
  TestHandle(BaseObject* object)
  : BaseHandle(object)
  {
  }
};

class TestObject : public BaseObject
{
};

} // unnamed namespace

void utc_dali_singleton_service_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_singleton_service_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliSingletonServiceGet(void)
{
  // Attempt to retrieve SingletonService before creating application
  SingletonService singletonService;
  singletonService = SingletonService::Get();
  DALI_TEST_CHECK(!singletonService);

  // Create Application class, should be able to retrieve SingletonService now
  TestApplication application;
  singletonService = SingletonService::Get();
  DALI_TEST_CHECK(singletonService);

  END_TEST;
}

int UtcDaliSingletonServiceRegisterAndGetSingleton(void)
{
  TestApplication  application;
  SingletonService singletonService(SingletonService::Get());

  // Attempt to register an empty handle
  TestHandle handle;
  singletonService.Register(typeid(handle), handle);
  DALI_TEST_CHECK(!singletonService.GetSingleton(typeid(handle)));

  // Create an actor instance and retrieve, should be valid this time
  handle = TestHandle(new TestObject);
  singletonService.Register(typeid(handle), handle);
  DALI_TEST_CHECK(singletonService.GetSingleton(typeid(handle)));

  END_TEST;
}
