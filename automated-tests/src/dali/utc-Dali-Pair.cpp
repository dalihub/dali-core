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
 * distributed under the License is distributed on an an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali-test-suite-utils.h>
#include <dali/public-api/common/dali-pair.h>

using namespace Dali;

int UtcDaliPairDefaultConstructorP(void)
{
  Pair<int, int> p;

  DALI_TEST_EQUALS(p.first, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(p.second, 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairTwoValueConstructorP(void)
{
  Pair<int, int> p(42, 10);

  DALI_TEST_EQUALS(p.first, 42, TEST_LOCATION);
  DALI_TEST_EQUALS(p.second, 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairMoveConstructorFromValuesP(void)
{
  int x = 10;
  int y = 20;
  Pair<int, int> p(std::move(x), std::move(y));

  DALI_TEST_EQUALS(p.first, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(p.second, 20, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairCopyConstructorP(void)
{
  Pair<int, int> p1(42, 10);
  Pair<int, int> p2(p1);

  DALI_TEST_EQUALS(p2.first, 42, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.second, 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairMoveConstructorP(void)
{
  Pair<int, int> p1(42, 10);
  Pair<int, int> p2(std::move(p1));

  DALI_TEST_EQUALS(p2.first, 42, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.second, 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairCopyAssignmentP(void)
{
  Pair<int, int> p1(42, 10);
  Pair<int, int> p2;
  p2 = p1;

  DALI_TEST_EQUALS(p2.first, 42, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.second, 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairMoveAssignmentP(void)
{
  Pair<int, int> p1(42, 10);
  Pair<int, int> p2;
  p2 = std::move(p1);

  DALI_TEST_EQUALS(p2.first, 42, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.second, 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairConvertingAssignmentP(void)
{
  // Test converting copy assignment
  {
    Pair<int, int> p1(42, 10);
    Pair<int, long> p2;
    p2 = p1;

    DALI_TEST_EQUALS(p2.first, 42, TEST_LOCATION);
    DALI_TEST_EQUALS(p2.second, 10L, TEST_LOCATION);
  }

  // Test converting move assignment
  {
    Pair<int, int> p1(42, 10);
    Pair<int, long> p2;
    p2 = std::move(p1);

    DALI_TEST_EQUALS(p2.first, 42, TEST_LOCATION);
    DALI_TEST_EQUALS(p2.second, 10L, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPairEqualityOperatorP(void)
{
  Pair<int, int> p1(42, 10);
  Pair<int, int> p2(42, 10);
  Pair<int, int> p3(10, 20);

  DALI_TEST_EQUALS(p1 == p2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(p1 == p3, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairInequalityOperatorP(void)
{
  Pair<int, int> p1(42, 10);
  Pair<int, int> p2(42, 10);
  Pair<int, int> p3(10, 20);

  DALI_TEST_EQUALS(p1 != p2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(p1 != p3, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairLessThanOperatorP(void)
{
  Pair<int, int> p1(1, 1);
  Pair<int, int> p2(2, 2);
  Pair<int, int> p3(1, 2);

  DALI_TEST_EQUALS(p1 < p2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(p2 < p1, false, TEST_LOCATION);
  DALI_TEST_EQUALS(p1 < p3, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairLessThanOrEqualOperatorP(void)
{
  Pair<int, int> p1(1, 1);
  Pair<int, int> p2(2, 2);
  Pair<int, int> p3(1, 1);

  DALI_TEST_EQUALS(p1 <= p2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(p1 <= p3, true, TEST_LOCATION);
  DALI_TEST_EQUALS(p2 <= p1, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairGreaterThanOperatorP(void)
{
  Pair<int, int> p1(2, 2);
  Pair<int, int> p2(1, 1);
  Pair<int, int> p3(1, 1);

  DALI_TEST_EQUALS(p1 > p2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(p1 > p3, true, TEST_LOCATION);
  DALI_TEST_EQUALS(p2 > p1, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaiPairGreaterThanOrEqualOperatorP(void)
{
  Pair<int, int> p1(2, 2);
  Pair<int, int> p2(1, 1);
  Pair<int, int> p3(1, 1);

  DALI_TEST_EQUALS(p1 >= p2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(p1 >= p3, true, TEST_LOCATION);
  DALI_TEST_EQUALS(p2 >= p1, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairMakePairP(void)
{
  // Test basic MakePair
  {
    auto p = MakePair(42, 10);
    DALI_TEST_EQUALS(p.first, 42, TEST_LOCATION);
    DALI_TEST_EQUALS(p.second, 10, TEST_LOCATION);
  }

  // Test type decay (references should be removed)
  {
    int x = 10;
    int y = 20;
    auto p = MakePair(x, y);
    DALI_TEST_EQUALS(static_cast<bool>(std::is_same<decltype(p.first), int>::value), true, TEST_LOCATION);
  }

  // Test with move semantics
  {
    std::string s1 = "hello";
    std::string s2 = "world";
    auto p = MakePair(std::move(s1), std::move(s2));
    DALI_TEST_EQUALS(p.first, "hello", TEST_LOCATION);
    DALI_TEST_EQUALS(p.second, "world", TEST_LOCATION);

    // If the move actually happened, the source strings should be empty.
    DALI_TEST_EQUALS(s1.empty(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(s2.empty(), true, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPairSwapP(void)
{
  Pair<int, int> p1(1, 1);
  Pair<int, int> p2(2, 2);

  Swap(p1, p2);

  DALI_TEST_EQUALS(p1.first, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(p1.second, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.first, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.second, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairSwapMethodP(void)
{
  Pair<int, int> p1(1, 1);
  Pair<int, int> p2(2, 2);

  p1.Swap(p2);

  DALI_TEST_EQUALS(p1.first, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(p1.second, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.first, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.second, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairConvertingConstructorP(void)
{
  // Test conversion from Pair<int, int> to Pair<int, long>
  Pair<int, int> p1(42, 10);
  Pair<int, long> p2(p1);

  DALI_TEST_EQUALS(p2.first, 42, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.second, 10L, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(std::is_same<decltype(p2.second), long>::value), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaiPairConvertingMoveConstructorP(void)
{
  // Test move conversion from Pair<int, int> to Pair<int, long>
  Pair<int, int> p1(42, 10);
  Pair<int, long> p2(std::move(p1));

  DALI_TEST_EQUALS(p2.first, 42, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.second, 10L, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(std::is_same<decltype(p2.second), long>::value), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairMemberTypesP(void)
{
  Pair<int, int> p;
  DALI_TEST_EQUALS(static_cast<bool>(std::is_same<typename decltype(p)::FirstType, int>::value), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(std::is_same<typename decltype(p)::SecondType, int>::value), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(std::is_same<decltype(p.first), int>::value), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(std::is_same<decltype(p.second), int>::value), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPairLexicographicalComparisonP(void)
{
  // Test lexicographical comparison
  Pair<int, int> p1(1, 3);
  Pair<int, int> p2(1, 2);
  Pair<int, int> p3(2, 1);

  DALI_TEST_EQUALS(p1 < p3, true, TEST_LOCATION); // 1 < 2
  DALI_TEST_EQUALS(p1 < p2, false, TEST_LOCATION); // 3 > 2

  END_TEST;
}