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

#include <iostream>
#include <stdlib.h>

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_constrainable_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_constrainable_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliConstrainableDownCast(void)
{
  TestApplication application;

  Handle handle = Constrainable::New();

  Constrainable customHandle1 = Constrainable::DownCast( handle );
  DALI_TEST_CHECK( customHandle1 );

  Constrainable customHandle2 = DownCast< Constrainable >( handle );
  DALI_TEST_CHECK( customHandle2 );
  END_TEST;
}

int UtcDaliConstrainableDownCastNegative(void)
{
  TestApplication application;

  Image image = ResourceImage::New( "temp" );
  Constrainable customHandle1 = Constrainable::DownCast( image );
  DALI_TEST_CHECK( ! customHandle1 );

  Constrainable empty;
  Constrainable customHandle2 = Constrainable::DownCast( empty );
  DALI_TEST_CHECK( ! customHandle2 );
  END_TEST;
}

int UtcDaliConstrainableCustomProperty(void)
{
  TestApplication application;

  Constrainable handle = Constrainable::New();

  float startValue(1.0f);
  Property::Index index = handle.RegisterProperty( "test-property", startValue );
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  handle.SetProperty( index, 5.0f );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == 5.0f );
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == 5.0f );
  END_TEST;
}
