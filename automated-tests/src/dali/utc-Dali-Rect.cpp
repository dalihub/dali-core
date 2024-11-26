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
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_rect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_rect_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Positive test case for a method
int UtcDaliRectCons01(void)
{
  TestApplication application;

  Rect<float> rect;
  DALI_TEST_EQUALS(rect.x, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.y, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.width, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.height, 0.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectCons02(void)
{
  TestApplication application;

  Rect<float> rect(10.0f, 20.0f, 400.0f, 200.0f);
  DALI_TEST_EQUALS(rect.x, 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.y, 20.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.width, 400.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.height, 200.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectCons03(void)
{
  TestApplication application;

  Rect<float> rect(10.0f, 20.0f, 400.0f, 200.0f);

  Rect<float> r2(rect);

  DALI_TEST_EQUALS(r2.x, 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.y, 20.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.width, 400.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.height, 200.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectCons04(void)
{
  TestApplication application;

  Vector4 vec4(10.0f, 20.0f, 400.0f, 200.0f);

  Rect<float> rect(vec4);

  DALI_TEST_EQUALS(rect.x, 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.y, 20.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.width, 400.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.height, 200.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectMoveConstructor(void)
{
  TestApplication application;

  Rect<float> rect(10.0f, 20.0f, 400.0f, 200.0f);

  Rect<float> r2 = std::move(rect);

  DALI_TEST_EQUALS(r2.x, 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.y, 20.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.width, 400.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.height, 200.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectMoveAssignmentOperator(void)
{
  TestApplication application;

  Rect<float> rect(10.0f, 20.0f, 400.0f, 200.0f);

  Rect<float> r2;
  r2 = std::move(rect);

  DALI_TEST_EQUALS(r2.x, 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.y, 20.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.width, 400.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.height, 200.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectAssignmentOperatorRect(void)
{
  TestApplication application;

  Rect<float> rect(10.0f, 20.0f, 400.0f, 200.0f);

  Rect<float> r2;
  r2 = rect;

  DALI_TEST_EQUALS(r2.x, 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.y, 20.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.width, 400.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.height, 200.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectAssignmentOperatorVector4(void)
{
  TestApplication application;

  Vector4 vec4(10.0f, 20.0f, 400.0f, 200.0f);

  Rect<float> rect;
  rect = vec4;

  DALI_TEST_EQUALS(rect.x, 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.y, 20.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.width, 400.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.height, 200.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectSet(void)
{
  TestApplication application;

  Rect<float> rect(10.0f, 20.0f, 400.0f, 200.0f);

  rect.Set(1.0f, 2.0f, 3.0f, 4.0f);

  DALI_TEST_EQUALS(rect.x, 1.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.y, 2.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.width, 3.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.height, 4.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectIsEmpty(void)
{
  TestApplication application;

  Rect<int>          ri;
  Rect<float>        rf(10.0f, 20.0f, 400.0f, 200.0f);
  Rect<float>        rf2;
  Rect<float>        rf3(10.0f, 20.0f, 0.0f, 200.0f);
  Rect<float>        rf4(10.0f, 20.0f, 400.0f, 0.0f);
  Rect<double>       rd(10.0, 20.0, 0.0, 200.0);
  Rect<unsigned int> ru(0u, 0u, 4u, 0u);

  DALI_TEST_CHECK(!rf.IsEmpty());
  DALI_TEST_CHECK(rf2.IsEmpty());
  DALI_TEST_CHECK(rf3.IsEmpty());
  DALI_TEST_CHECK(rf4.IsEmpty());
  DALI_TEST_CHECK(ri.IsEmpty());
  DALI_TEST_CHECK(rd.IsEmpty());
  DALI_TEST_CHECK(ru.IsEmpty());
  END_TEST;
}

int UtcDaliRectLeft(void)
{
  TestApplication application;

  Rect<float> rf(10.0f, 20.0f, 400.0f, 200.0f);

  DALI_TEST_EQUALS(rf.Left(), 10.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectRight(void)
{
  TestApplication application;

  Rect<float> rf(10.0f, 20.0f, 400.0f, 200.0f);

  DALI_TEST_EQUALS(rf.Right(), 410.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectTop(void)
{
  TestApplication application;

  Rect<float> rf(10.0f, 20.0f, 400.0f, 200.0f);

  DALI_TEST_EQUALS(rf.Top(), 20.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectBottom(void)
{
  TestApplication application;

  Rect<float> rf(10.0f, 20.0f, 400.0f, 200.0f);

  DALI_TEST_EQUALS(rf.Bottom(), 220.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectArea(void)
{
  TestApplication application;

  Rect<float> rf(10.0f, 20.0f, 400.0f, 200.0f);

  DALI_TEST_EQUALS(rf.Area(), 80000.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectIntersects(void)
{
  TestApplication application;

  Rect<float> rf1(10.0f, 20.0f, 200.0f, 200.0f);
  Rect<float> rf2(10.0f, 120.0f, 200.0f, 200.0f);
  Rect<float> rf3(10.0f, -80.0f, 200.0f, 200.0f);
  Rect<float> rf4(110.0f, 20.0f, 200.0f, 200.0f);
  Rect<float> rf5(-90.0f, 20.0f, 200.0f, 200.0f);
  Rect<float> rf6(1000.0f, 1200.0f, 10.0f, 10.0f);

  DALI_TEST_CHECK(rf1.Intersects(rf2));
  DALI_TEST_CHECK(rf1.Intersects(rf3));
  DALI_TEST_CHECK(rf1.Intersects(rf4));
  DALI_TEST_CHECK(rf1.Intersects(rf5));
  DALI_TEST_CHECK(!rf1.Intersects(rf6));
  END_TEST;
}

int UtcDaliRectContains(void)
{
  TestApplication application;

  Rect<float> rf1(10.0f, 20.0f, 200.0f, 200.0f);

  Rect<float> rf2(10.0f, 120.0f, 200.0f, 200.0f);
  Rect<float> rf3(10.0f, -80.0f, 200.0f, 200.0f);
  Rect<float> rf4(110.0f, 20.0f, 200.0f, 200.0f);
  Rect<float> rf5(-90.0f, 20.0f, 200.0f, 200.0f);
  Rect<float> rf6(1000.0f, 1200.0f, 10.0f, 10.0f);

  Rect<float> rf7(50.0f, 70.0f, 50.0f, 50.0f);

  Rect<float> rf8(10.0f, 20.0f, 100.0f, 100.0f);
  Rect<float> rf9(110.0f, 20.0f, 100.0f, 100.0f);
  Rect<float> rf10(110.0f, 120.0f, 100.0f, 100.0f);
  Rect<float> rf11(10.0f, 120.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(rf1.Contains(rf1));
  DALI_TEST_CHECK(!rf1.Contains(rf2));
  DALI_TEST_CHECK(!rf1.Contains(rf3));
  DALI_TEST_CHECK(!rf1.Contains(rf4));
  DALI_TEST_CHECK(!rf1.Contains(rf5));
  DALI_TEST_CHECK(!rf1.Contains(rf6));

  DALI_TEST_CHECK(rf1.Contains(rf7));

  DALI_TEST_CHECK(rf1.Contains(rf8));
  DALI_TEST_CHECK(rf1.Contains(rf9));
  DALI_TEST_CHECK(rf1.Contains(rf10));
  DALI_TEST_CHECK(rf1.Contains(rf11));
  END_TEST;
}

int UtcDaliRectOperatorNotEquals(void)
{
  TestApplication application;

  Rect<float> rf1(10.0f, 20.0f, 200.0f, 200.0f);
  Rect<float> rf2(10.0f, 120.0f, 200.0f, 200.0f);
  Rect<float> rf3(10.0f, -80.0f, 200.0f, 200.0f);
  Rect<float> rf4(110.0f, 20.0f, 200.0f, 200.0f);
  Rect<float> rf5(-90.0f, 20.0f, 200.0f, 200.0f);
  Rect<float> rf6(1000.0f, 1200.0f, 10.0f, 10.0f);
  Rect<float> rf7(50.0f, 70.0f, 50.0f, 50.0f);
  Rect<float> rf8(10.0f, 20.0f, 100.0f, 100.0f);
  Rect<float> rf9(110.0f, 20.0f, 100.0f, 100.0f);
  Rect<float> rf10(110.0f, 120.0f, 100.0f, 100.0f);
  Rect<float> rf11(10.0f, 120.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(rf1 != rf2);
  DALI_TEST_CHECK(rf1 != rf3);
  DALI_TEST_CHECK(rf1 != rf4);
  DALI_TEST_CHECK(rf1 != rf5);
  DALI_TEST_CHECK(rf1 != rf6);
  DALI_TEST_CHECK(rf1 != rf7);
  DALI_TEST_CHECK(rf1 != rf8);
  DALI_TEST_CHECK(rf1 != rf9);
  DALI_TEST_CHECK(rf1 != rf10);
  DALI_TEST_CHECK(rf1 != rf11);
  END_TEST;
}

int UtcDaliRectOperatorEquals(void)
{
  TestApplication application;

  Rect<float> rf1(10.0f, 20.0f, 200.0f, 200.0f);
  Rect<float> rf1p(10.0f, 20.0f, 200.0f, 200.0f);

  Rect<float> rf2(110.0f, 20.0f, 200.0f, 200.0f);
  Rect<float> rf3(10.0f, 120.0f, 200.0f, 200.0f);
  Rect<float> rf4(10.0f, 20.0f, 300.0f, 200.0f);
  Rect<float> rf5(10.0f, 20.0f, 200.0f, 500.0f);

  Rect<float> rf6(0.0f, 0.0f, 9.0f, 10.0f);

  DALI_TEST_CHECK(rf1 == rf1p);
  DALI_TEST_CHECK(rf1 == rf1);
  DALI_TEST_CHECK(!(rf1 == rf2));
  DALI_TEST_CHECK(!(rf1 == rf3));
  DALI_TEST_CHECK(!(rf1 == rf4));
  DALI_TEST_CHECK(!(rf1 == rf5));
  DALI_TEST_CHECK(!(rf1 == rf6));

  // integers
  Rect<int> ri1(10, 20, 200, 200);
  Rect<int> ri1p(10, 20, 200, 200);

  DALI_TEST_CHECK(ri1 == ri1p);
  END_TEST;
}

int UtcDaliRectOStreamOperatorP(void)
{
  TestApplication    application;
  std::ostringstream oss;

  Rect<int> rect(1, 2, 10, 10);

  oss << rect;

  std::string expectedOutput = "[1, 2, 10, 10]";

  DALI_TEST_EQUALS(oss.str(), expectedOutput, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRectIntersectP(void)
{
  TestApplication application;

  // Test for float
  {
    Rect<float> baseRect(100.0f, 200.0f, 200.0f, 300.0f);

    Rect<float> rf1 = baseRect;

    tet_printf("Check rf1 changed if intersect successed\n");
    Rect<float> rf2(50.0f, 250.3f, 200.0f, 300.0f);
    DALI_TEST_CHECK(rf1.Intersect(rf2));
    DALI_TEST_EQUALS(rf1, Rect<float>(100.0f, 250.3f, 150.0f, 249.7f), TEST_LOCATION);

    tet_printf("Check rf1 didnt changed if not intersect\n");
    Rect<float> rf3(254.5f, 0.0f, 30.6f, 234.56f);
    DALI_TEST_CHECK(!rf1.Intersect(rf3));
    DALI_TEST_EQUALS(rf1, Rect<float>(100.0f, 250.3f, 150.0f, 249.7f), TEST_LOCATION);

    tet_printf("Check baseRect was intersect with rf3\n");
    rf1 = baseRect;
    DALI_TEST_CHECK(rf1.Intersect(rf3));
    DALI_TEST_EQUALS(rf1, Rect<float>(254.5f, 200.0f, 30.6f, 34.56f), TEST_LOCATION);
  }

  // Test for int
  {
    Rect<int> baseRect(100, 200, 200, 300);

    Rect<int> rf1 = baseRect;

    tet_printf("Check rf1 changed if intersect successed\n");
    Rect<int> rf2(50, 150, 200, 300);
    DALI_TEST_CHECK(rf1.Intersect(rf2));
    DALI_TEST_EQUALS(rf1, Rect<int>(100, 200, 150, 250), TEST_LOCATION);

    tet_printf("Check rf1 didnt changed if not intersect\n");
    Rect<int> rf3(254, 0, 10, 234);
    DALI_TEST_CHECK(!rf1.Intersect(rf3));
    DALI_TEST_EQUALS(rf1, Rect<int>(100, 200, 150, 250), TEST_LOCATION);

    tet_printf("Check baseRect was intersect with rf3\n");
    rf1 = baseRect;
    DALI_TEST_CHECK(rf1.Intersect(rf3));
    DALI_TEST_EQUALS(rf1, Rect<int>(254, 200, 10, 34), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRectMergeP(void)
{
  TestApplication application;

  // Test for float
  {
    Rect<float> baseRect(100.0f, 200.0f, 200.0f, 300.0f);

    Rect<float> rf1 = baseRect;

    tet_printf("Check rf1 changed after merge\n");
    Rect<float> rf2(50.0f, 250.3f, 200.0f, 300.0f);
    rf1.Merge(rf2);
    DALI_TEST_EQUALS(rf1, Rect<float>(50.0f, 200.0f, 250.0f, 350.3f), TEST_LOCATION);

    tet_printf("Check rf1 changed after merge\n");
    Rect<float> rf3(354.5f, 0.0f, 30.6f, 234.56f);
    rf1.Merge(rf3);
    DALI_TEST_EQUALS(rf1, Rect<float>(50.0f, 0.0f, 335.1f, 550.3f), TEST_LOCATION);

    tet_printf("Check baseRect merge with rf3\n");
    rf1 = baseRect;
    rf1.Merge(rf3);
    DALI_TEST_EQUALS(rf1, Rect<float>(100.0f, 0.0f, 285.1f, 500.0f), TEST_LOCATION);
  }

  // Test for int
  {
    Rect<int> baseRect(100, 200, 200, 300);

    Rect<int> rf1 = baseRect;

    tet_printf("Check rf1 changed after merge\n");
    Rect<int> rf2(50, 150, 200, 300);
    rf1.Merge(rf2);
    DALI_TEST_EQUALS(rf1, Rect<int>(50, 150, 250, 350), TEST_LOCATION);

    tet_printf("Check rf1 changed after merge\n");
    Rect<int> rf3(354, 0, 10, 234);
    rf1.Merge(rf3);
    DALI_TEST_EQUALS(rf1, Rect<int>(50, 0, 314, 500), TEST_LOCATION);

    tet_printf("Check baseRect merge with rf3\n");
    rf1 = baseRect;
    rf1.Merge(rf3);
    DALI_TEST_EQUALS(rf1, Rect<int>(100, 0, 264, 500), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRectInsetP(void)
{
  TestApplication application;

  // Test for float
  {
    Rect<float> rf1(100.0f, 200.0f, 200.0f, 300.0f);

    tet_printf("Check rf1 changed after inset\n");
    rf1.Inset(-50.0f, 50.0f);
    DALI_TEST_EQUALS(rf1, Rect<float>(150.0f, 150.0f, 100.0f, 400.0f), TEST_LOCATION);

    rf1.Inset(100.0f, -100.0f);
    DALI_TEST_EQUALS(rf1, Rect<float>(50.0f, 250.0f, 300.0f, 200.0f), TEST_LOCATION);

    tet_printf("Check rf1 invalid after large inset\n");
    rf1.Inset(-150.0f, -150.0f);
    DALI_TEST_CHECK(!rf1.IsValid());
  }

  // Test for int
  {
    Rect<int> rf1(100, 200, 200, 300);

    tet_printf("Check rf1 changed after inset\n");
    rf1.Inset(-50, 50);
    DALI_TEST_EQUALS(rf1, Rect<int>(150, 150, 100, 400), TEST_LOCATION);

    rf1.Inset(100, -100);
    DALI_TEST_EQUALS(rf1, Rect<int>(50, 250, 300, 200), TEST_LOCATION);

    tet_printf("Check rf1 invalid after large inset\n");
    rf1.Inset(-100, -101);
    DALI_TEST_CHECK(!rf1.IsValid());
  }

  END_TEST;
}
