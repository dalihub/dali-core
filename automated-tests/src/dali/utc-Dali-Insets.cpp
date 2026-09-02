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

#include <iostream>

using namespace Dali;

void utc_dali_insets_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_insets_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliInsetsConstructor01(void)
{
  TestApplication application;

  Insets insets;
  DALI_TEST_EQUALS(insets.start, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.end, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.top, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.bottom, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInsetsConstructor02(void)
{
  TestApplication application;

  Insets insets(10.5f, -20.25f, 400.0f, -200.75f);
  DALI_TEST_EQUALS(insets.start, 10.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.end, -20.25f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.top, 400.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.bottom, -200.75f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInsetsConstructorSymmetric(void)
{
  TestApplication application;

  Insets insets(10.0f, 20.0f);
  DALI_TEST_EQUALS(insets.start, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.end, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.top, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.bottom, 20.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInsetsCopyConstructor(void)
{
  TestApplication application;

  Insets insets(10.5f, -20.5f, 400.0f, -200.0f);
  Insets insets2(insets);

  DALI_TEST_EQUALS(insets2.start, 10.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets2.end, -20.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets2.top, 400.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets2.bottom, -200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInsetsCopyAssignment(void)
{
  TestApplication application;

  Insets insets;
  Insets insets2(10.5f, -20.5f, 400.0f, -200.0f);
  insets = insets2;

  DALI_TEST_EQUALS(insets.start, 10.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.end, -20.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.top, 400.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.bottom, -200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInsetsMoveConstructor(void)
{
  TestApplication application;

  Insets insets(10.5f, -20.5f, 400.0f, -200.0f);
  Insets insets2(std::move(insets));

  DALI_TEST_EQUALS(insets2.start, 10.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets2.end, -20.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets2.top, 400.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets2.bottom, -200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInsetsMoveAssignment(void)
{
  TestApplication application;

  Insets insets;
  Insets insets2(10.5f, -20.5f, 400.0f, -200.0f);
  insets = std::move(insets2);

  DALI_TEST_EQUALS(insets.start, 10.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.end, -20.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.top, 400.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.bottom, -200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInsetsAssignP(void)
{
  Insets      insets;
  const float array[] = {1.0f, 2.0f, 3.0f, 4.0f};
  insets              = (const float*)array;

  DALI_TEST_EQUALS(insets.start, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.end, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.top, 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(insets.bottom, 4.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInsetsOperatorNotEquals(void)
{
  TestApplication application;

  Insets insets1(10.0f, 20.0f, 200.0f, 200.0f);
  Insets insets2(10.0f, 120.0f, 200.0f, 200.0f);
  Insets insets3(10.0f, 80.0f, 200.0f, 200.0f);

  DALI_TEST_CHECK(insets1 != insets2);
  DALI_TEST_CHECK(insets1 != insets3);
  END_TEST;
}

int UtcDaliInsetsOperatorEquals(void)
{
  TestApplication application;

  Insets insets1(10.0f, 20.0f, 200.0f, 200.0f);
  Insets insets1p(10.0f, 20.0f, 200.0f, 200.0f);

  Insets insets2(10.0f, 120.0f, 200.0f, 200.0f);
  Insets insets3(10.0f, 80.0f, 200.0f, 200.0f);

  DALI_TEST_CHECK(insets1 == insets1p);
  DALI_TEST_CHECK(insets1 == insets1);
  DALI_TEST_CHECK(!(insets1 == insets2));
  DALI_TEST_CHECK(!(insets1 == insets3));

  END_TEST;
}

int UtcDaliInsetsOStreamOperatorP(void)
{
  TestApplication    application;
  std::ostringstream oss;

  Insets insets(1.0f, 2.0f, 10.0f, 10.0f);

  oss << insets;

  std::string expectedOutput = "[1, 2, 10, 10]";

  DALI_TEST_EQUALS(oss.str(), expectedOutput, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInsetsPropertyValue(void)
{
  TestApplication application;

  Insets          insets(1.0f, 2.5f, 10.0f, -10.0f);
  Property::Value value(insets);

  DALI_TEST_EQUALS(value.GetType(), Property::INSETS, TEST_LOCATION);
  DALI_TEST_EQUALS(PropertyTypes::GetName(Property::INSETS), "INSETS", TEST_LOCATION);
  DALI_TEST_EQUALS(PropertyTypes::Get<Insets>(), Property::INSETS, TEST_LOCATION);

  Insets result;
  DALI_TEST_CHECK(value.Get(result));
  DALI_TEST_CHECK(result == insets);
  DALI_TEST_CHECK(value.Get<Insets>() == insets);

  // Copy, move and equality.
  Property::Value copied(value);
  DALI_TEST_CHECK(copied == value);

  Property::Value assigned;
  assigned = value;
  DALI_TEST_CHECK(assigned == value);
  DALI_TEST_EQUALS(assigned.GetHash(), value.GetHash(), TEST_LOCATION);

  Property::Value moved(std::move(copied));
  DALI_TEST_CHECK(moved == value);

  // Default construction by type.
  Property::Value defaulted(Property::INSETS);
  DALI_TEST_EQUALS(defaulted.GetType(), Property::INSETS, TEST_LOCATION);
  DALI_TEST_CHECK(defaulted.Get<Insets>() == Insets());

  END_TEST;
}

int UtcDaliInsetsPropertyValueSameTypeAssignment(void)
{
  TestApplication application;

  // Assigning between two values that are already INSETS takes the in-place
  // assignment path rather than reconstructing the stored object.
  Property::Value value(Insets(1.0f, 2.5f, 10.0f, -10.0f));
  Property::Value other(Insets(4.0f, 5.0f, 6.0f, 7.0f));

  other = value;
  DALI_TEST_EQUALS(other.GetType(), Property::INSETS, TEST_LOCATION);
  DALI_TEST_CHECK(other.Get<Insets>() == Insets(1.0f, 2.5f, 10.0f, -10.0f));
  DALI_TEST_CHECK(other == value);

  // The source must be untouched, and the copy must be independent of it.
  value = Property::Value(Insets(8.0f, 9.0f, 10.0f, 11.0f));
  DALI_TEST_CHECK(other.Get<Insets>() == Insets(1.0f, 2.5f, 10.0f, -10.0f));

  // Self assignment must leave the value alone.
  Property::Value& alias = other;
  other                  = alias;
  DALI_TEST_CHECK(other.Get<Insets>() == Insets(1.0f, 2.5f, 10.0f, -10.0f));

  END_TEST;
}

int UtcDaliInsetsCustomProperty(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  Insets          startValue(1.0f, 2.0f, 3.0f, 4.0f);
  Property::Index index = handle.RegisterProperty("testProperty", startValue, Property::READ_WRITE);
  DALI_TEST_CHECK(handle.GetProperty<Insets>(index) == startValue);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(handle.GetProperty<Insets>(index) == startValue);

  // Negative test i.e. there is no conversion from float to Insets
  handle.SetProperty(index, float(1.5));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(handle.GetProperty<Insets>(index) == startValue);

  // Negative test i.e. there is no conversion from Extents to Insets either
  handle.SetProperty(index, Extents(9, 9, 9, 9));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(handle.GetProperty<Insets>(index) == startValue);

  // Positive test (sanity check)
  Insets endValue(5.5f, 6.5f, 7.5f, 8.5f);
  handle.SetProperty(index, endValue);
  DALI_TEST_CHECK(handle.GetProperty<Insets>(index) == endValue);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(handle.GetProperty<Insets>(index) == endValue);

  // A Vector4 is convertible to Insets, so it must be accepted.
  handle.SetProperty(index, Vector4(1.25f, 2.25f, 3.25f, 4.25f));
  DALI_TEST_CHECK(handle.GetProperty<Insets>(index) == Insets(1.25f, 2.25f, 3.25f, 4.25f));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(handle.GetProperty<Insets>(index) == Insets(1.25f, 2.25f, 3.25f, 4.25f));

  END_TEST;
}

int UtcDaliInsetsPropertyValueConversion(void)
{
  TestApplication application;

  // Vector4 -> Insets
  Property::Value vector4Value(Vector4(1.5f, 2.5f, 3.5f, 4.5f));
  Insets          fromVector4;
  DALI_TEST_CHECK(vector4Value.Get(fromVector4));
  DALI_TEST_CHECK(fromVector4 == Insets(1.5f, 2.5f, 3.5f, 4.5f));

  // Extents is an independent type, so it must not convert to Insets.
  Property::Value extentsValue(Extents(1, 2, 3, 4));
  Insets          unconverted;
  DALI_TEST_CHECK(!extentsValue.Get(unconverted));

  // Insets must not convert to Extents either.
  Property::Value insetsValue(Insets(1.0f, 2.0f, 3.0f, 4.0f));
  Extents         toExtents;
  DALI_TEST_CHECK(!insetsValue.Get(toExtents));

  // Non-convertible type
  Property::Value floatValue(1.0f);
  DALI_TEST_CHECK(!floatValue.Get(unconverted));

  END_TEST;
}

int UtcDaliInsetsPropertyValueOStream(void)
{
  TestApplication    application;
  std::ostringstream oss;

  oss << Property::Value(Insets(1.0f, 2.0f, 10.0f, 10.0f));

  DALI_TEST_EQUALS(oss.str(), std::string("[1, 2, 10, 10]"), TEST_LOCATION);
  END_TEST;
}
