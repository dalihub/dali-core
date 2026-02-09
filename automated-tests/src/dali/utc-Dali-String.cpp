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
#include <dali/devel-api/common/stage-devel.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/context-notifier.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali/public-api/dali-core.h>
#include <dali/public-api/events/key-event.h>
#include <stdlib.h>
#include <unistd.h>

#include <dali/integration-api/stream-operators.h>
#include <iostream>

using namespace Dali;

void string_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void string_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliStringDefaultConstructorP(void)
{
  TestApplication application;
  String          string;

  DALI_TEST_CHECK(string.Empty());
  END_TEST;
}

int UtcDaliStringCStringConstructorP(void)
{
  TestApplication application;
  String          string1("1");
  String          string3("3");

  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), "1") == 0);

  auto&        str2 = string1;
  const auto&  str3 = string3;
  const String constStr("Const String");

  str2 += String("Blah");
  str2 += str3;
  str2 += constStr;

  DALI_TEST_EQUALS(str2.CStr(), "1Blah3Const String", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringCStringCopyConstructorP(void)
{
  tet_printf("Test copy constructor");
  TestApplication application;
  const char*     result = "Sample String";
  String          string1(result);
  String          empty;
  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), result) == 0);

  tet_printf("Check that the new string contains the original string content");
  String string2(string1);
  DALI_TEST_CHECK(!string2.Empty());
  DALI_TEST_EQUALS(string2.CStr(), result, TEST_LOCATION);

  string1 = empty;
  tet_printf("Check that clearing the original string doesn't lose the copy");
  DALI_TEST_CHECK(!string2.Empty());
  DALI_TEST_EQUALS(string2.CStr(), result, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringCStringAssignOperatorP(void)
{
  tet_printf("Test assignment operator");
  TestApplication application;
  const char*     result = "Sample String";
  String          string1(result);
  String          empty;
  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), result) == 0);

  tet_printf("Check that the new string contains the original string content");
  String string2 = string1;
  DALI_TEST_CHECK(!string2.Empty());
  DALI_TEST_EQUALS(string2.CStr(), result, TEST_LOCATION);

  string1 = empty;
  tet_printf("Check that clearing the original string doesn't lose the copy");
  DALI_TEST_CHECK(!string2.Empty());
  DALI_TEST_EQUALS(string2.CStr(), result, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringCStringMoveConstructorP(void)
{
  tet_printf("Test move constructor");
  TestApplication application;
  const char*     result = "Sample String";
  String          string1(result);

  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), result) == 0);

  tet_printf("Check that the new string contains the original string content");
  String string2(std::move(string1));
  DALI_TEST_CHECK(!string2.Empty());
  DALI_TEST_EQUALS(string2.CStr(), result, TEST_LOCATION);
  DALI_TEST_CHECK(string1.Empty());

  END_TEST;
}

int UtcDaliStringCStringAssignMoveOperatorP(void)
{
  tet_printf("Test move assignment operator");
  TestApplication application;
  const char*     result = "Sample String";
  String          string1(result);

  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), result) == 0);

  tet_printf("Check that the new string contains the original string content");
  String string2 = std::move(string1);
  DALI_TEST_CHECK(!string2.Empty());
  DALI_TEST_EQUALS(string2.CStr(), result, TEST_LOCATION);
  DALI_TEST_CHECK(string1.Empty());

  END_TEST;
}

int UtcDaliStringCStringConstructorN(void)
{
  TestApplication application;
  String          string1(nullptr);
  std::string     emptyStr;
  DALI_TEST_CHECK(string1.Empty());
  DALI_TEST_EQUALS(string1.CStr(), emptyStr.c_str(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringDestructorP(void)
{
  TestApplication application;
  String*         string = new String();
  delete string;
  string = NULL;

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliStringCStringSizeP(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  String          string1(testString.c_str());

  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), testString.c_str()) == 0);
  DALI_TEST_EQUALS(string1.Size(), testString.size(), TEST_LOCATION);

  END_TEST;
}
int UtcDaliStringCStringSizeN(void)
{
  TestApplication application;
  String          string1;

  DALI_TEST_EQUALS(string1.Size(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringCStringEmpty(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  String          string1(testString.c_str());

  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), testString.c_str()) == 0);
  DALI_TEST_EQUALS(string1.Size(), testString.size(), TEST_LOCATION);

  String string2;
  DALI_TEST_CHECK(string2.Empty());
  DALI_TEST_CHECK(string2.Size() == 0);
  END_TEST;
}

int UtcDaliStringCStringCStrP(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  const char*     testString2("BlahBlah");
  String          string1(testString.c_str());

  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), testString.c_str()) == 0);
  DALI_TEST_CHECK(strcmp(string1.CStr(), testString2) != 0);

  END_TEST;
}

int UtcDaliStringCStringSetChar(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  std::string     resultString("A Test String of some length");
  String          string1(testString.c_str());

  string1[2] = 'T';
  string1[5] = 'S';

  DALI_TEST_CHECK(strcmp(string1.CStr(), resultString.c_str()) != 0);

  END_TEST;
}

int UtcDaliStringCStringGetChar(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  const String    string1(testString.c_str());

  const char& readCharacter1 = string1[0];
  const char& readCharacter2 = string1[2];
  DALI_TEST_EQUALS(readCharacter1, 'A', TEST_LOCATION);
  DALI_TEST_EQUALS(readCharacter2, 't', TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringCStringOperatorAppendChar(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  std::string     resultString("A test string of some length 12");
  String          string1(testString.c_str());

  string1 += ' ';
  string1 += '1';
  string1 += '2';

  DALI_TEST_EQUALS(string1.CStr(), resultString.c_str(), TEST_LOCATION);

  String string2;

  string2 += ' ';
  string2 += '1';
  string2 += '2';

  DALI_TEST_EQUALS(string2.CStr(), " 12", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringCStringOperatorAppendStr(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  std::string     resultString("A test string of some length 12");
  String          string1(testString.c_str());
  String          string2(" 12");
  string1 += string2;

  DALI_TEST_EQUALS(string1.CStr(), resultString.c_str(), TEST_LOCATION);

  String string3;
  string3 += string2;

  DALI_TEST_EQUALS(string3.CStr(), " 12", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringCStringOperatorEquals(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  std::string     resultString("A test string of some length 12");
  String          string1(testString.c_str());
  String          string2(" 12");
  string1 += string2;

  DALI_TEST_CHECK(string1 == String(resultString.c_str()));

  END_TEST;
}

int UtcDaliStringCStringOperatorNotEquals(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  std::string     resultString("A test string of some length");
  String          string1(testString.c_str());
  String          string2("A bunch of characters");

  DALI_TEST_CHECK(string1 != string2);

  END_TEST;
}

int UtcDaliStringCStringOperatorLessThan(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  std::string     resultString("A test string of some length");
  String          string1(testString.c_str());
  String          string2("A bunch of characters");

  DALI_TEST_CHECK(string2 < string1);

  END_TEST;
}

int UtcDaliStringCStringOperatorLessThanOrEquals(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  std::string     resultString("A test string of some length");
  String          string1(testString.c_str());
  String          string2("A bunch of characters");
  String          string3(testString.c_str());

  DALI_TEST_CHECK(string2 <= string1);
  DALI_TEST_CHECK(string1 <= string1);
  DALI_TEST_CHECK(string3 <= string1);
  DALI_TEST_CHECK(string1 <= string3);

  END_TEST;
}

int UtcDaliStringCStringOperatorGreaterThan(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  std::string     resultString("A test string of some length");
  String          string1(testString.c_str());
  String          string2("A wierd bunch of characters");

  DALI_TEST_CHECK(string2 > string1);

  END_TEST;
}

int UtcDaliStringCStringOperatorGreaterThanOrEquals(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  std::string     resultString("A test string of some length");
  String          string1(testString.c_str());
  String          string2("A wierd bunch of characters");
  String          string3(testString.c_str());

  DALI_TEST_CHECK(string2 >= string1);
  DALI_TEST_CHECK(string1 >= string1);
  DALI_TEST_CHECK(string3 >= string1);
  DALI_TEST_CHECK(string1 >= string3);

  END_TEST;
}

int UtcDaliStringCStringOperatorStreamOperator(void)
{
  TestApplication application;
  std::string     resultString("A test string of some length");
  String          testString(resultString.c_str());

  std::ostringstream oss;
  oss << testString;

  DALI_TEST_EQUALS(oss.str().c_str(), resultString.c_str(), TEST_LOCATION);
  END_TEST;
};
