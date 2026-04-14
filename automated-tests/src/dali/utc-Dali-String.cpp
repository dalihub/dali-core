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
#include <dali/public-api/common/dali-string-view.h>
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

int UtcDaliStringUseAfterMoveP(void)
{
  TestApplication application;
  const char*     result = "Sample String";
  String          string1(result);

  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), result) == 0);

  String string2(std::move(string1));
  DALI_TEST_CHECK(!string2.Empty());
  DALI_TEST_EQUALS(string2.CStr(), result, TEST_LOCATION);
  DALI_TEST_CHECK(string1.Empty());

  string1 += 'A';
  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), "A") == 0);

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

int UtcDaliStringCStringClear(void)
{
  TestApplication application;
  std::string     testString("A test string of some length");
  String          string1(testString.c_str());

  DALI_TEST_CHECK(!string1.Empty());
  DALI_TEST_CHECK(strcmp(string1.CStr(), testString.c_str()) == 0);
  DALI_TEST_EQUALS(string1.Size(), testString.size(), TEST_LOCATION);

  string1.Clear();
  DALI_TEST_CHECK(string1.Empty());
  DALI_TEST_CHECK(string1.Size() == 0);
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

int UtcDaliStringAssignCStringOperatorP(void)
{
  tet_printf("Test assignment operator from const char*\n");
  TestApplication application;

  String string1("Initial Value");
  DALI_TEST_EQUALS(string1.CStr(), "Initial Value", TEST_LOCATION);

  tet_printf("Assign a new const char* value\n");
  string1 = "New Value";
  DALI_TEST_EQUALS(string1.CStr(), "New Value", TEST_LOCATION);
  DALI_TEST_EQUALS(string1.Size(), 9u, TEST_LOCATION);

  tet_printf("Assign to an empty string\n");
  String string2;
  DALI_TEST_CHECK(string2.Empty());
  string2 = "Assigned";
  DALI_TEST_EQUALS(string2.CStr(), "Assigned", TEST_LOCATION);
  DALI_TEST_CHECK(!string2.Empty());

  tet_printf("Assign an empty string\n");
  string2 = "";
  DALI_TEST_CHECK(string2.Empty());
  DALI_TEST_EQUALS(string2.Size(), 0u, TEST_LOCATION);

  tet_printf("Check return value allows chaining\n");
  String string3;
  string3 = "Chained";
  DALI_TEST_EQUALS(string3.CStr(), "Chained", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringAssignCStringOperatorN(void)
{
  tet_printf("Test assignment operator from nullptr\n");
  TestApplication application;

  String string1("Some Value");
  string1 = nullptr;
  DALI_TEST_CHECK(string1.Empty());

  END_TEST;
}

int UtcDaliStringOperatorEqualsCStringP(void)
{
  tet_printf("Test equality operator with const char*\n");
  TestApplication application;

  String string1("Hello");
  DALI_TEST_CHECK(string1 == "Hello");
  DALI_TEST_CHECK(!(string1 == "World"));
  DALI_TEST_CHECK(!(string1 == "Hell"));
  DALI_TEST_CHECK(!(string1 == "HelloX"));

  tet_printf("Test empty string comparison\n");
  String empty;
  DALI_TEST_CHECK(empty == "");
  DALI_TEST_CHECK(!(empty == "NonEmpty"));

  tet_printf("Test case sensitivity\n");
  DALI_TEST_CHECK(!(string1 == "hello"));
  DALI_TEST_CHECK(!(string1 == "HELLO"));

  END_TEST;
}

int UtcDaliStringOperatorEqualsCStringN(void)
{
  tet_printf("Test equality operator with nullptr\n");
  TestApplication application;

  String string1("Hello");
  DALI_TEST_CHECK(!(string1 == nullptr));

  String empty;
  DALI_TEST_CHECK(empty == nullptr);

  END_TEST;
}

int UtcDaliStringOperatorNotEqualsCStringP(void)
{
  tet_printf("Test inequality operator with const char* on left side\n");
  TestApplication application;

  String string1("Hello");
  DALI_TEST_CHECK(!("Hello" != string1));
  DALI_TEST_CHECK("World" != string1);
  DALI_TEST_CHECK("Hell" != string1);
  DALI_TEST_CHECK("HelloX" != string1);

  tet_printf("Test empty string comparison\n");
  String empty;
  DALI_TEST_CHECK(!("" != empty));
  DALI_TEST_CHECK("NonEmpty" != empty);

  tet_printf("Test case sensitivity\n");
  DALI_TEST_CHECK("hello" != string1);
  DALI_TEST_CHECK("HELLO" != string1);

  tet_printf("Test nullptr comparison\n");
  String nonEmpty("Test");
  DALI_TEST_CHECK(nullptr != nonEmpty);
  DALI_TEST_CHECK(!(nullptr != empty));

  END_TEST;
}

// StringView tests

int UtcDaliStringViewDefaultConstructorP(void)
{
  TestApplication application;
  StringView      view;

  DALI_TEST_CHECK(view.Empty());
  DALI_TEST_EQUALS(view.Size(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringViewCStringConstructorP(void)
{
  TestApplication application;
  const char*     testStr = "Hello StringView";
  StringView      view(testStr);

  DALI_TEST_CHECK(!view.Empty());
  DALI_TEST_EQUALS(view.Size(), strlen(testStr), TEST_LOCATION);
  DALI_TEST_CHECK(memcmp(view.Data(), testStr, view.Size()) == 0);

  END_TEST;
}

int UtcDaliStringViewCStringConstructorN(void)
{
  TestApplication application;
  StringView      view(nullptr);

  DALI_TEST_CHECK(view.Empty());
  DALI_TEST_EQUALS(view.Size(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringViewPointerLengthConstructorP(void)
{
  TestApplication application;
  const char*     testStr = "Hello StringView World";
  StringView      view(testStr, 5);

  DALI_TEST_CHECK(!view.Empty());
  DALI_TEST_EQUALS(view.Size(), 5u, TEST_LOCATION);
  DALI_TEST_CHECK(memcmp(view.Data(), "Hello", 5) == 0);

  END_TEST;
}

int UtcDaliStringViewFromDaliStringP(void)
{
  TestApplication application;
  const char*     testStr = "A Dali String";
  String          string(testStr);
  StringView      view(string);

  DALI_TEST_CHECK(!view.Empty());
  DALI_TEST_EQUALS(view.Size(), string.Size(), TEST_LOCATION);
  DALI_TEST_CHECK(memcmp(view.Data(), string.CStr(), view.Size()) == 0);

  END_TEST;
}

int UtcDaliStringViewFromEmptyDaliStringP(void)
{
  TestApplication application;
  String          string;
  StringView      view(string);

  DALI_TEST_CHECK(view.Empty());
  DALI_TEST_EQUALS(view.Size(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringViewCopyConstructorP(void)
{
  TestApplication application;
  const char*     testStr = "Copy this view";
  StringView      view1(testStr);
  StringView      view2(view1);

  DALI_TEST_CHECK(!view2.Empty());
  DALI_TEST_EQUALS(view2.Size(), view1.Size(), TEST_LOCATION);
  DALI_TEST_CHECK(view2.Data() == view1.Data());

  END_TEST;
}

int UtcDaliStringViewAssignOperatorP(void)
{
  TestApplication application;
  const char*     testStr = "Assign this view";
  StringView      view1(testStr);
  StringView      view2;
  view2 = view1;

  DALI_TEST_CHECK(!view2.Empty());
  DALI_TEST_EQUALS(view2.Size(), view1.Size(), TEST_LOCATION);
  DALI_TEST_CHECK(view2.Data() == view1.Data());

  END_TEST;
}

int UtcDaliStringViewDataP(void)
{
  TestApplication application;
  const char*     testStr = "Data test";
  StringView      view(testStr);

  DALI_TEST_CHECK(view.Data() == testStr);

  END_TEST;
}

int UtcDaliStringViewEqualityOperatorP(void)
{
  TestApplication application;
  StringView      view1("Hello");
  StringView      view2("Hello");
  StringView      view3("World");
  StringView      empty1;
  StringView      empty2;

  DALI_TEST_CHECK(view1 == view2);
  DALI_TEST_CHECK(!(view1 == view3));
  DALI_TEST_CHECK(empty1 == empty2);
  DALI_TEST_CHECK(!(view1 == empty1));

  END_TEST;
}

int UtcDaliStringViewEqualityCStringOperatorP(void)
{
  tet_printf("Test StringView equality operator with const char*\n");
  TestApplication application;

  StringView view1("Hello");
  DALI_TEST_CHECK(view1 == "Hello");
  DALI_TEST_CHECK(!(view1 == "World"));
  DALI_TEST_CHECK(!(view1 == "Hell"));
  DALI_TEST_CHECK(!(view1 == "HelloX"));

  tet_printf("Test empty view comparison with const char*\n");
  StringView empty;
  DALI_TEST_CHECK(empty == "");
  DALI_TEST_CHECK(!(empty == "NonEmpty"));

  tet_printf("Test case sensitivity\n");
  DALI_TEST_CHECK(!(view1 == "hello"));
  DALI_TEST_CHECK(!(view1 == "HELLO"));

  tet_printf("Test substring view comparison\n");
  StringView partial("Hello World", 5);
  DALI_TEST_CHECK(partial == "Hello");
  DALI_TEST_CHECK(!(partial == "Hello World"));

  END_TEST;
}

int UtcDaliStringViewEqualityCStringLeftSideP(void)
{
  tet_printf("Test equality operator with const char* on left side\n");
  TestApplication application;

  StringView view1("Hello");
  DALI_TEST_CHECK("Hello" == view1);
  DALI_TEST_CHECK(!("World" == view1));
  DALI_TEST_CHECK(!("Hell" == view1));
  DALI_TEST_CHECK(!("HelloX" == view1));

  tet_printf("Test empty view comparison with const char* on left side\n");
  StringView empty;
  DALI_TEST_CHECK("" == empty);
  DALI_TEST_CHECK(!("NonEmpty" == empty));

  tet_printf("Test case sensitivity\n");
  DALI_TEST_CHECK(!("hello" == view1));
  DALI_TEST_CHECK(!("HELLO" == view1));

  tet_printf("Test substring view comparison\n");
  StringView partial("Hello World", 5);
  DALI_TEST_CHECK("Hello" == partial);
  DALI_TEST_CHECK(!("Hello World" == partial));

  END_TEST;
}

int UtcDaliStringViewNotEqualsCStringP(void)
{
  tet_printf("Test StringView inequality operator (free function with const char* on left)\n");
  TestApplication application;

  StringView view1("Hello");
  DALI_TEST_CHECK("World" != view1);
  DALI_TEST_CHECK("Hell" != view1);
  DALI_TEST_CHECK("HelloX" != view1);
  DALI_TEST_CHECK(!("Hello" != view1));

  tet_printf("Test empty view inequality comparison\n");
  StringView empty;
  DALI_TEST_CHECK("NonEmpty" != empty);
  DALI_TEST_CHECK(!("" != empty));

  tet_printf("Test case sensitivity\n");
  DALI_TEST_CHECK("hello" != view1);
  DALI_TEST_CHECK("HELLO" != view1);
  DALI_TEST_CHECK(!("Hello" != view1));

  tet_printf("Test substring view inequality comparison\n");
  StringView partial("Hello World", 5);
  DALI_TEST_CHECK("Hello World" != partial);
  DALI_TEST_CHECK(!("Hello" != partial));

  tet_printf("Test that operator!= works as negation of operator==\n");
  StringView view2("Test");
  DALI_TEST_EQUALS(("Other" != view2), !(view2 == "Other"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringFromStringView(void)
{
  tet_printf("Test construction of String from StringView\n");

  TestApplication application;
  const char*     testStr1 = "";
  StringView      view1(testStr1);
  const char*     testStr2 = "Data test";
  StringView      view2(testStr2);

  DALI_TEST_CHECK(view1.Data() == testStr1);
  DALI_TEST_CHECK(view2.Data() == testStr2);

  String result1(view1);
  String result2(view2);

  DALI_TEST_EQUALS(result1.Size(), 0, TEST_LOCATION);
  DALI_TEST_CHECK(result1.Empty());

  DALI_TEST_EQUALS(result2.Size(), strlen(testStr2), TEST_LOCATION);
  DALI_TEST_CHECK(!result2.Empty());
  DALI_TEST_EQUALS(strcmp(result2.CStr(), testStr2), 0, TEST_LOCATION);

  tet_printf("Tests all pass\n");

  END_TEST;
}

int UtcDaliStringAssignStringView(void)
{
  tet_printf("Test assigment of String from StringView\n");
  TestApplication  application;
  const char*      testStr1 = "";
  StringView       view1(testStr1);
  const char*      testStr2 = "Data test";
  StringView       view2(testStr2);
  const char*      testStr3 = "Const StringView test";
  const StringView view3(testStr3);

  DALI_TEST_CHECK(view1.Data() == testStr1);
  DALI_TEST_CHECK(view2.Data() == testStr2);
  DALI_TEST_CHECK(view3.Data() == testStr3);

  String result1 = view1;
  String result2 = view2;
  String result3 = view3;

  tet_printf("Test that empty StringView can be assigned\n");
  DALI_TEST_EQUALS(result1.Size(), 0, TEST_LOCATION);
  DALI_TEST_CHECK(result1.Empty());

  tet_printf("Test that normal StringView can be assigned\n");
  DALI_TEST_EQUALS(result2.Size(), strlen(testStr2), TEST_LOCATION);
  DALI_TEST_CHECK(!result2.Empty());
  DALI_TEST_EQUALS(strcmp(result2.CStr(), testStr2), 0, TEST_LOCATION);

  tet_printf("Test that const StringView can be assigned\n");
  DALI_TEST_EQUALS(result3.Size(), strlen(testStr3), TEST_LOCATION);
  DALI_TEST_CHECK(!result3.Empty());
  DALI_TEST_EQUALS(strcmp(result3.CStr(), testStr3), 0, TEST_LOCATION);

  tet_printf("Tests all pass\n");

  END_TEST;
}
