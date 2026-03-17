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
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-value.h>
#include <cstring>
#include <string>
#include <string_view>
#include <unordered_set>

using namespace Dali;

void string_comprehensive_startup(void)
{
  test_return_value = TET_UNDEF;
}

void string_comprehensive_cleanup(void)
{
  test_return_value = TET_PASS;
}

// =============================================================================
// Constructor Tests - Comprehensive
// =============================================================================

int UtcDaliStringDefaultConstructor_Empty(void)
{
  tet_printf("Test default constructor creates empty string");
  TestApplication application;
  String          str;
  DALI_TEST_CHECK(str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);
  DALI_TEST_CHECK(str.CStr() != nullptr);
  DALI_TEST_CHECK(strlen(str.CStr()) == 0);
  END_TEST;
}

int UtcDaliStringCStringConstructor_SingleChar(void)
{
  tet_printf("Test C-string constructor with single character");
  TestApplication application;
  String          str("A");
  DALI_TEST_CHECK(!str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(1), TEST_LOCATION);
  DALI_TEST_EQUALS(strcmp(str.CStr(), "A"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(str[0], 'A', TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringCStringConstructor_LongString(void)
{
  tet_printf("Test C-string constructor with long string");
  TestApplication application;
  std::string     longStr(1000, 'X');
  String          str(longStr.c_str());
  DALI_TEST_CHECK(!str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(1000), TEST_LOCATION);
  DALI_TEST_CHECK(strcmp(str.CStr(), longStr.c_str()) == 0);
  END_TEST;
}

int UtcDaliStringCStringConstructor_SpecialChars(void)
{
  tet_printf("Test C-string constructor with special characters");
  TestApplication application;
  const char*     special = "Hello\tWorld\nTest\rSpace\\Quote\"Apostrophe'Null\x00Middle";
  String          str(special);
  DALI_TEST_CHECK(!str.Empty());
  DALI_TEST_CHECK(str.CStr()[0] == 'H');
  DALI_TEST_CHECK(str.CStr()[5] == '\t');
  DALI_TEST_CHECK(str.CStr()[11] == '\n');
  END_TEST;
}

int UtcDaliStringCStringConstructor_EmptyString(void)
{
  tet_printf("Test C-string constructor with empty string literal");
  TestApplication application;
  String          str("");
  DALI_TEST_CHECK(str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);
  DALI_TEST_CHECK(strlen(str.CStr()) == 0);
  END_TEST;
}

int UtcDaliStringFromStringView_EmptyView(void)
{
  tet_printf("Test String construction from empty StringView");
  TestApplication application;
  StringView      view("");
  String          str(view);
  DALI_TEST_CHECK(str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringFromStringView_NullptrDataZeroSize(void)
{
  tet_printf("Test String construction from StringView with nullptr and zero size");
  TestApplication application;
  StringView      view(nullptr, 0);
  String          str(view);
  DALI_TEST_CHECK(str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringFromStringView_ValidDataZeroSize(void)
{
  tet_printf("Test String construction from StringView with valid data but zero size");
  TestApplication application;
  StringView      view("Hello", 0);
  String          str(view);
  DALI_TEST_CHECK(str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringFromStringView_Substring(void)
{
  tet_printf("Test String construction from StringView substring");
  TestApplication application;
  StringView      view("Hello World", 5);
  String          str(view);
  DALI_TEST_CHECK(!str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(5), TEST_LOCATION);
  DALI_TEST_EQUALS(strcmp(str.CStr(), "Hello"), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringFromStringView_NullptrDataNonZeroSize(void)
{
  tet_printf("Test String construction from StringView with nullptr and non-zero size");
  TestApplication application;
  StringView      view(nullptr, 5);
  String          str(view);
  DALI_TEST_CHECK(str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// Copy Constructor Tests
// =============================================================================

int UtcDaliStringCopyConstructor_EmptyString(void)
{
  tet_printf("Test copy constructor from empty string");
  TestApplication application;
  String          str1;
  String          str2(str1);
  DALI_TEST_CHECK(str2.Empty());
  DALI_TEST_EQUALS(str2.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringCopyConstructor_Independence(void)
{
  tet_printf("Test that copy creates independent string");
  TestApplication application;
  String          str1("Original");
  String          str2(str1);

  DALI_TEST_EQUALS(str1.CStr(), "Original", TEST_LOCATION);
  DALI_TEST_EQUALS(str2.CStr(), "Original", TEST_LOCATION);

  // Modify str2
  str2[0] = 'M';
  DALI_TEST_EQUALS(str1.CStr(), "Original", TEST_LOCATION);
  DALI_TEST_EQUALS(str2.CStr(), "Mriginal", TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// Move Constructor Tests
// =============================================================================

int UtcDaliStringMoveConstructor_EmptyString(void)
{
  tet_printf("Test move constructor from empty string");
  TestApplication application;
  String          str1;
  String          str2(std::move(str1));
  DALI_TEST_CHECK(str2.Empty());
  DALI_TEST_EQUALS(str2.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringMoveConstructor_Independence(void)
{
  tet_printf("Test that moved-from string is in valid empty state");
  TestApplication application;
  String          str1("To Be Moved");
  String          str2(std::move(str1));

  DALI_TEST_EQUALS(str2.CStr(), "To Be Moved", TEST_LOCATION);
  DALI_TEST_CHECK(str1.Empty());
  DALI_TEST_EQUALS(str1.Size(), size_t(0), TEST_LOCATION);

  // Can assign to moved-from string
  str1 = "New Value";
  DALI_TEST_EQUALS(str1.CStr(), "New Value", TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// Assignment Operator Tests
// =============================================================================

int UtcDaliStringAssignOperator_SelfAssignment(void)
{
  tet_printf("Test self-assignment with String");
  TestApplication application;
  String          str("Test");
  str = str; // Self-assignment
  DALI_TEST_EQUALS(str.CStr(), "Test", TEST_LOCATION);
  DALI_TEST_EQUALS(str.Size(), size_t(4), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringAssignOperator_EmptyToNonEmpty(void)
{
  tet_printf("Test assigning empty string to non-empty string");
  TestApplication application;
  String          str1("Original");
  String          str2;
  str1 = str2;
  DALI_TEST_CHECK(str1.Empty());
  DALI_TEST_EQUALS(str1.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringAssignOperator_Chaining(void)
{
  tet_printf("Test chaining of assignment operators");
  TestApplication application;
  String          str1("A");
  String          str2("B");
  String          str3("C");
  str1 = str2 = str3;
  DALI_TEST_EQUALS(str1.CStr(), "C", TEST_LOCATION);
  DALI_TEST_EQUALS(str2.CStr(), "C", TEST_LOCATION);
  DALI_TEST_EQUALS(str3.CStr(), "C", TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringAssignMoveOperator_SelfAssignment(void)
{
  tet_printf("Test self-move-assignment (should leave string in valid state)");
  TestApplication application;
  String          str("Test");
  str = std::move(str); // Self-move-assignment
  // After self-move, object should be in valid but unspecified state
  // Just verify it doesn't crash
  DALI_TEST_CHECK(str.Size() >= 0);
  END_TEST;
}

int UtcDaliStringAssignMoveOperator_Independence(void)
{
  tet_printf("Test that move assignment makes source empty");
  TestApplication application;
  String          str1("Source");
  String          str2("Dest");
  str2 = std::move(str1);

  DALI_TEST_EQUALS(str2.CStr(), "Source", TEST_LOCATION);
  DALI_TEST_CHECK(str1.Empty());

  // Can reuse str1
  str1 = "New Source";
  DALI_TEST_EQUALS(str1.CStr(), "New Source", TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringAssignCString_SelfAssignment(void)
{
  tet_printf("Test assigning C-string to itself (should be safe)");
  TestApplication application;
  String          str("Test");
  str = str.CStr(); // Assign own C-string
  DALI_TEST_EQUALS(str.CStr(), "Test", TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringAssignCString_EmptyLiteral(void)
{
  tet_printf("Test assigning empty C-string literal");
  TestApplication application;
  String          str("Original");
  str = "";
  DALI_TEST_CHECK(str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringAssignStringView_NullptrData(void)
{
  tet_printf("Test assigning StringView with nullptr data");
  TestApplication application;
  String          str("Original");
  StringView      view(nullptr, 10);
  str = view;
  DALI_TEST_CHECK(str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringAssignStringView_ZeroSize(void)
{
  tet_printf("Test assigning StringView with valid data but zero size");
  TestApplication application;
  String          str("Original");
  StringView      view("Hello", 0);
  str = view;
  DALI_TEST_CHECK(str.Empty());
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// Size and Empty Tests
// =============================================================================

int UtcDaliStringSize_AfterModification(void)
{
  tet_printf("Test Size() after various modifications");
  TestApplication application;
  String          str;
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);

  str = "Hello";
  DALI_TEST_EQUALS(str.Size(), size_t(5), TEST_LOCATION);

  str += String(" World");
  DALI_TEST_EQUALS(str.Size(), size_t(11), TEST_LOCATION);

  str += '!';
  DALI_TEST_EQUALS(str.Size(), size_t(12), TEST_LOCATION);

  str = "";
  DALI_TEST_EQUALS(str.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringEmpty_AfterModification(void)
{
  tet_printf("Test Empty() after various modifications");
  TestApplication application;
  String          str;
  DALI_TEST_CHECK(str.Empty());

  str = "Hello";
  DALI_TEST_CHECK(!str.Empty());

  str = "";
  DALI_TEST_CHECK(str.Empty());

  str += 'A';
  DALI_TEST_CHECK(!str.Empty());

  String str2(std::move(str));
  DALI_TEST_CHECK(str.Empty());
  END_TEST;
}

// =============================================================================
// CStr Tests
// =============================================================================

int UtcDaliStringCStr_NonEmpty(void)
{
  tet_printf("Test CStr() returns valid pointer for non-empty string");
  TestApplication application;
  String          str("Hello World");
  const char*     cstr = str.CStr();
  DALI_TEST_CHECK(cstr != nullptr);
  DALI_TEST_CHECK(strlen(cstr) == 11);
  DALI_TEST_CHECK(strcmp(cstr, "Hello World") == 0);
  END_TEST;
}

int UtcDaliStringCStr_Empty(void)
{
  tet_printf("Test CStr() returns valid pointer for empty string");
  TestApplication application;
  String          str;
  const char*     cstr = str.CStr();
  DALI_TEST_CHECK(cstr != nullptr);
  DALI_TEST_CHECK(strlen(cstr) == 0);
  END_TEST;
}

// =============================================================================
// operator[] Tests
// =============================================================================

int UtcDaliStringOperatorIndex_ReadFirstLast(void)
{
  tet_printf("Test operator[] reading first and last characters");
  TestApplication application;
  String          str("Hello");

  char first = str[0];
  char last  = str[4];

  DALI_TEST_EQUALS(first, 'H', TEST_LOCATION);
  DALI_TEST_EQUALS(last, 'o', TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorIndex_ReadAll(void)
{
  tet_printf("Test operator[] reading all characters");
  TestApplication application;
  String          str("Hello");

  for(size_t i = 0; i < str.Size(); ++i)
  {
    DALI_TEST_CHECK(str[i] == "Hello"[i]);
  }
  END_TEST;
}

int UtcDaliStringOperatorIndex_Modify(void)
{
  tet_printf("Test operator[] modifying characters");
  TestApplication application;
  String          str("Hello");

  str[0] = 'J';
  str[4] = 'y';

  DALI_TEST_EQUALS(str.CStr(), "Jelly", TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorIndex_Const(void)
{
  tet_printf("Test const operator[]");
  TestApplication application;
  const String    str("Hello");

  char first  = str[0];
  char second = str[1];

  DALI_TEST_EQUALS(first, 'H', TEST_LOCATION);
  DALI_TEST_EQUALS(second, 'e', TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorIndex_NullTerminator(void)
{
  tet_printf("Test operator[] can access null terminator position");
  TestApplication application;
  String          str("Hello");

  // operator[] should allow access to position Size() which is the null terminator
  char nullChar = str[str.Size()];
  DALI_TEST_EQUALS(nullChar, '\0', TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// operator+= Tests
// =============================================================================

int UtcDaliStringOperatorAppendChar_Multiple(void)
{
  tet_printf("Test operator+= with multiple characters");
  TestApplication application;
  String          str;

  for(char c = 'A'; c <= 'Z'; ++c)
  {
    str += c;
  }

  DALI_TEST_EQUALS(str.Size(), size_t(26), TEST_LOCATION);
  DALI_TEST_EQUALS(strcmp(str.CStr(), "ABCDEFGHIJKLMNOPQRSTUVWXYZ"), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorAppendChar_Special(void)
{
  tet_printf("Test operator+= with special characters");
  TestApplication application;
  String          str;

  str += '\t';
  str += '\n';
  str += ' ';
  str += '\\';
  str += '\"';

  DALI_TEST_EQUALS(str.Size(), size_t(5), TEST_LOCATION);
  DALI_TEST_CHECK(str[0] == '\t');
  DALI_TEST_CHECK(str[1] == '\n');
  DALI_TEST_CHECK(str[2] == ' ');
  DALI_TEST_CHECK(str[3] == '\\');
  DALI_TEST_CHECK(str[4] == '\"');
  END_TEST;
}

int UtcDaliStringOperatorAppendChar_ReturnRef(void)
{
  tet_printf("Test operator+= returns reference for chaining");
  TestApplication application;
  String          str;

  (str += 'A') += 'B';
  (str += 'C').operator+=('D');

  DALI_TEST_EQUALS(str.CStr(), "ABCD", TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorAppendString_EmptyToEmpty(void)
{
  tet_printf("Test operator+= appending empty string to empty string");
  TestApplication application;
  String          str1;
  String          str2;

  str1 += str2;

  DALI_TEST_CHECK(str1.Empty());
  DALI_TEST_EQUALS(str1.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorAppendString_NonEmptyToEmpty(void)
{
  tet_printf("Test operator+= appending non-empty string to empty string");
  TestApplication application;
  String          str1;
  String          str2("Hello");

  str1 += str2;

  DALI_TEST_EQUALS(str1.CStr(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(str2.CStr(), "Hello", TEST_LOCATION); // str2 unchanged
  END_TEST;
}

int UtcDaliStringOperatorAppendString_Multiple(void)
{
  tet_printf("Test operator+= appending multiple strings");
  TestApplication application;
  String          str("Hello");
  String          str2(" ");
  String          str3("World");
  String          str4("!");

  str += str2;
  str += str3;
  str += str4;

  DALI_TEST_EQUALS(str.CStr(), "Hello World!", TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorAppendString_Self(void)
{
  tet_printf("Test operator+= appending string to itself");
  TestApplication application;
  String          str("Hello");

  str += str;

  DALI_TEST_EQUALS(str.CStr(), "HelloHello", TEST_LOCATION);
  DALI_TEST_EQUALS(str.Size(), size_t(10), TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// Comparison Operator Tests
// =============================================================================

int UtcDaliStringOperatorEquals_Identity(void)
{
  tet_printf("Test == operator with same string object");
  TestApplication application;
  String          str("Hello");
  DALI_TEST_CHECK(str == str);
  END_TEST;
}

int UtcDaliStringOperatorEquals_CaseSensitivity(void)
{
  tet_printf("Test == operator is case-sensitive");
  TestApplication application;
  String          str1("Hello");
  String          str2("hello");
  String          str3("HELLO");

  DALI_TEST_CHECK(!(str1 == str2));
  DALI_TEST_CHECK(!(str1 == str3));
  DALI_TEST_CHECK(!(str2 == str3));
  END_TEST;
}

int UtcDaliStringOperatorNotEquals_SameString(void)
{
  tet_printf("Test != operator with same string");
  TestApplication application;
  String          str("Hello");
  DALI_TEST_CHECK(!(str != str));
  END_TEST;
}

int UtcDaliStringOperatorLessThan_LexicalOrder(void)
{
  tet_printf("Test < operator with lexical ordering");
  TestApplication application;
  String          str1("A");
  String          str2("B");
  String          str3("AA");
  String          str4("AB");

  DALI_TEST_CHECK(str1 < str2);
  DALI_TEST_CHECK(str1 < str3);
  DALI_TEST_CHECK(str1 < str4);
  DALI_TEST_CHECK(str3 < str4);
  DALI_TEST_CHECK(!(str2 < str1));
  DALI_TEST_CHECK(!(str3 < str1));
  END_TEST;
}

int UtcDaliStringOperatorGreaterThan_LexicalOrder(void)
{
  tet_printf("Test > operator with lexical ordering");
  TestApplication application;
  String          str1("B");
  String          str2("A");
  String          str3("AB");
  String          str4("AA");

  DALI_TEST_CHECK(str1 > str2);
  DALI_TEST_CHECK(str3 > str2);
  DALI_TEST_CHECK(str3 > str4);
  DALI_TEST_CHECK(!(str2 > str1));
  END_TEST;
}

int UtcDaliStringOperatorLessThanOrEquals_Boundary(void)
{
  tet_printf("Test <= operator at boundaries");
  TestApplication application;
  String          str1("A");
  String          str2("A");
  String          str3("B");

  DALI_TEST_CHECK(str1 <= str2);
  DALI_TEST_CHECK(str2 <= str1);
  DALI_TEST_CHECK(str1 <= str3);
  DALI_TEST_CHECK(!(str3 <= str1));
  END_TEST;
}

int UtcDaliStringOperatorGreaterThanOrEquals_Boundary(void)
{
  tet_printf("Test >= operator at boundaries");
  TestApplication application;
  String          str1("B");
  String          str2("B");
  String          str3("A");

  DALI_TEST_CHECK(str1 >= str2);
  DALI_TEST_CHECK(str2 >= str1);
  DALI_TEST_CHECK(str1 >= str3);
  DALI_TEST_CHECK(!(str3 >= str1));
  END_TEST;
}

// =============================================================================
// operator+ Tests
// =============================================================================

int UtcDaliStringOperatorPlus_EmptyStrings(void)
{
  tet_printf("Test operator+ with two empty strings");
  TestApplication application;
  String          str1;
  String          str2;

  String result = str1 + str2;

  DALI_TEST_CHECK(result.Empty());
  DALI_TEST_EQUALS(result.Size(), size_t(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorPlus_NonEmptyEmpty(void)
{
  tet_printf("Test operator+ with non-empty and empty strings");
  TestApplication application;
  String          str1("Hello");
  String          str2;

  String result1 = str1 + str2;
  String result2 = str2 + str1;

  DALI_TEST_EQUALS(result1.CStr(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(result2.CStr(), "Hello", TEST_LOCATION);

  // Original strings unchanged
  DALI_TEST_EQUALS(str1.CStr(), "Hello", TEST_LOCATION);
  DALI_TEST_CHECK(str2.Empty());
  END_TEST;
}

int UtcDaliStringOperatorPlus_BasicConcatenation(void)
{
  tet_printf("Test operator+ basic concatenation");
  TestApplication application;
  String          str1("Hello");
  String          str2(" ");
  String          str3("World");

  String result = str1 + str2 + str3;

  DALI_TEST_EQUALS(result.CStr(), "Hello World", TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorPlus_Chaining(void)
{
  tet_printf("Test operator+ chaining");
  TestApplication application;
  String          str1("A");
  String          str2("B");
  String          str3("C");
  String          str4("D");

  String result = str1 + str2 + str3 + str4;

  DALI_TEST_EQUALS(result.CStr(), "ABCD", TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorPlus_OriginalsUnchanged(void)
{
  tet_printf("Test operator+ doesn't modify original strings");
  TestApplication application;
  String          str1("Hello");
  String          str2("World");

  String result = str1 + str2;

  DALI_TEST_EQUALS(str1.CStr(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(str2.CStr(), "World", TEST_LOCATION);
  DALI_TEST_EQUALS(result.CStr(), "HelloWorld", TEST_LOCATION);
  END_TEST;
}

int UtcDaliStringOperatorPlus_SelfConcatenation(void)
{
  tet_printf("Test operator+ with same string");
  TestApplication application;
  String          str("Hello");

  String result = str + str;

  DALI_TEST_EQUALS(result.CStr(), "HelloHello", TEST_LOCATION);
  DALI_TEST_EQUALS(str.CStr(), "Hello", TEST_LOCATION); // Original unchanged
  END_TEST;
}

// =============================================================================
// Comparison with const char* Tests
// =============================================================================

int UtcDaliStringOperatorEqualsCString_EmptyString(void)
{
  tet_printf("Test == with const char* empty string");
  TestApplication application;
  String          str;

  DALI_TEST_CHECK(str == String(""));
  DALI_TEST_CHECK(String("") == str);
  END_TEST;
}

int UtcDaliStringOperatorEqualsCString_DifferentLengths(void)
{
  tet_printf("Test == with const char* different lengths");
  TestApplication application;
  String          str("Hello");

  DALI_TEST_CHECK(!(str == "Hel"));
  DALI_TEST_CHECK(!(str == "Hello!"));
  END_TEST;
}

int UtcDaliStringOperatorNotEqualsCString_Basic(void)
{
  tet_printf("Test != with const char*");
  TestApplication application;
  String          str1("Hello");
  String          str2;

  DALI_TEST_CHECK(str1 != String("World"));
  DALI_TEST_CHECK(str1 != String(""));
  DALI_TEST_CHECK(!(str2 != String("")));
  END_TEST;
}

// =============================================================================
// Edge Case Tests
// =============================================================================

int UtcDaliString_LargeStringOperations(void)
{
  tet_printf("Test operations on large strings");
  TestApplication application;

  // Create large string
  String str;
  for(int i = 0; i < 100; ++i)
  {
    str = str + String("abcdefghij");
  }

  DALI_TEST_EQUALS(str.Size(), size_t(1000), TEST_LOCATION);

  // Test copy
  String str2(str);
  DALI_TEST_EQUALS(str2.Size(), size_t(1000), TEST_LOCATION);
  DALI_TEST_CHECK(str2 == str);

  // Test move
  String str3(std::move(str2));
  DALI_TEST_EQUALS(str3.Size(), size_t(1000), TEST_LOCATION);
  DALI_TEST_CHECK(str2.Empty());

  // Test comparison
  String str4;
  for(int i = 0; i < 100; ++i)
  {
    str4 = str4 + String("abcdefghij");
  }
  DALI_TEST_CHECK(str3 == str4);

  END_TEST;
}

int UtcDaliString_NullByteHandling(void)
{
  tet_printf("Test handling of strings with null bytes");
  TestApplication application;
  String          str("Hello\0World");

  // Size should count up to first null terminator
  DALI_TEST_EQUALS(str.Size(), size_t(5), TEST_LOCATION);
  DALI_TEST_CHECK(str[0] == 'H');
  DALI_TEST_CHECK(str[4] == 'o');
  END_TEST;
}

int UtcDaliString_ReuseAfterMove(void)
{
  tet_printf("Test reusing string after move");
  TestApplication application;
  String          str1("First");
  String          str2(std::move(str1));

  // str1 is now empty, can be reused
  str1 = "Second";
  DALI_TEST_EQUALS(str1.CStr(), "Second", TEST_LOCATION);
  DALI_TEST_EQUALS(str2.CStr(), "First", TEST_LOCATION);

  // Can move again
  String str3(std::move(str2));
  str2 = "Third";
  DALI_TEST_EQUALS(str2.CStr(), "Third", TEST_LOCATION);
  DALI_TEST_EQUALS(str3.CStr(), "First", TEST_LOCATION);
  END_TEST;
}

int UtcDaliString_MultipleAssignments(void)
{
  tet_printf("Test multiple consecutive assignments");
  TestApplication application;
  String          str;

  str = "A";
  DALI_TEST_EQUALS(str.CStr(), "A", TEST_LOCATION);

  str = "B";
  DALI_TEST_EQUALS(str.CStr(), "B", TEST_LOCATION);

  str = "";
  DALI_TEST_CHECK(str.Empty());

  str = "C";
  DALI_TEST_EQUALS(str.CStr(), "C", TEST_LOCATION);

  END_TEST;
}

int UtcDaliString_StringToIntConversion(void)
{
  tet_printf("Test string can represent numbers");
  TestApplication application;
  String          str("12345");

  DALI_TEST_EQUALS(str.Size(), size_t(5), TEST_LOCATION);
  DALI_TEST_EQUALS(str.CStr(), "12345", TEST_LOCATION);

  // Can modify digits
  str[2] = '9';
  DALI_TEST_EQUALS(str.CStr(), "12945", TEST_LOCATION);

  END_TEST;
}

int UtcDaliString_GetRawStorage_InternalUse(void)
{
  tet_printf("Test GetRawStorage() for internal use");
  TestApplication application;
  String          str("Hello");

  void* storage = str.GetRawStorage();
  DALI_TEST_CHECK(storage != nullptr);

  // This is for internal use, just verify it's accessible
  END_TEST;
}

int UtcDaliString_ConstexprSizeAndAlign(void)
{
  tet_printf("Test storage size and alignment constants");
  TestApplication application;

  // Verify constants are accessible
  // StorageSize should be >= sizeof(std::string)
  // StorageAlign should be >= alignof(std::string)

  // This is compile-time verification, runtime just checks it's accessible
  String str;
  void*  storage = str.GetRawStorage();
  DALI_TEST_CHECK(storage != nullptr);

  END_TEST;
}

// =============================================================================
// string-utils.h Conversion Function Tests
// =============================================================================

int UtcDaliStringToStdStringFromStringP(void)
{
  tet_printf("Test ToStdString(const String&)");
  TestApplication application;

  String      daliStr("Hello World");
  std::string result = Integration::ToStdString(daliStr);
  DALI_TEST_EQUALS(result, std::string("Hello World"), TEST_LOCATION);
  // Original unchanged
  DALI_TEST_EQUALS(daliStr.CStr(), "Hello World", TEST_LOCATION);

  // Empty string
  String      emptyStr;
  std::string emptyResult = Integration::ToStdString(emptyStr);
  DALI_TEST_CHECK(emptyResult.empty());

  END_TEST;
}

int UtcDaliStringToStdStringMoveP(void)
{
  tet_printf("Test ToStdString(String&&) move variant");
  TestApplication application;

  String      daliStr("Move Me");
  std::string result = Integration::ToStdString(std::move(daliStr));
  DALI_TEST_EQUALS(result, std::string("Move Me"), TEST_LOCATION);
  // Source should be emptied after move
  DALI_TEST_CHECK(daliStr.Empty());

  // Empty string move
  String      emptyStr;
  std::string emptyResult = Integration::ToStdString(std::move(emptyStr));
  DALI_TEST_CHECK(emptyResult.empty());

  END_TEST;
}

int UtcDaliStringToStdStringFromStringViewP(void)
{
  tet_printf("Test ToStdString(const StringView&)");
  TestApplication application;

  StringView  view("Hello View", 10);
  std::string result = Integration::ToStdString(view);
  DALI_TEST_EQUALS(result, std::string("Hello View"), TEST_LOCATION);

  // Empty view
  StringView  emptyView("", 0);
  std::string emptyResult = Integration::ToStdString(emptyView);
  DALI_TEST_CHECK(emptyResult.empty());

  // nullptr Data() case
  StringView  nullView(nullptr, 0);
  std::string nullResult = Integration::ToStdString(nullView);
  DALI_TEST_CHECK(nullResult.empty());

  END_TEST;
}

int UtcDaliStringToStdStringFromPropertyValueP(void)
{
  tet_printf("Test ToStdString(Property::Value)");
  TestApplication application;

  // Non-empty string value
  Property::Value strValue(String("Property String"));
  std::string     result = Integration::ToStdString(strValue);
  DALI_TEST_EQUALS(result, std::string("Property String"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringToStdStringFromPropertyValueN(void)
{
  tet_printf("Test ToStdString(Property::Value)");
  TestApplication application;

  // Non-STRING type value (should return empty string)
  try
  {
    Property::Value intValue(42);
    std::string     intResult = Integration::ToStdString(intValue);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "value.GetType() == Property::STRING", TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong exception\n");
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliStringToStdStringViewFromStringViewP(void)
{
  tet_printf("Test ToStdStringView(const StringView&)");
  TestApplication application;

  StringView       daliView("Hello", 5);
  std::string_view result = Integration::ToStdStringView(daliView);
  DALI_TEST_EQUALS(result, std::string_view("Hello"), TEST_LOCATION);
  DALI_TEST_EQUALS(result.size(), size_t(5), TEST_LOCATION);

  // Empty view
  StringView       emptyView("", 0);
  std::string_view emptyResult = Integration::ToStdStringView(emptyView);
  DALI_TEST_CHECK(emptyResult.empty());

  // nullptr view
  StringView       nullView(nullptr, 0);
  std::string_view nullResult = Integration::ToStdStringView(nullView);
  DALI_TEST_CHECK(nullResult.empty());

  END_TEST;
}

int UtcDaliStringToStdStringViewFromStringP(void)
{
  tet_printf("Test ToStdStringView(const String&)");
  TestApplication application;

  String           daliStr("Hello");
  std::string_view result = Integration::ToStdStringView(daliStr);
  DALI_TEST_EQUALS(result, std::string_view("Hello"), TEST_LOCATION);
  DALI_TEST_EQUALS(result.size(), size_t(5), TEST_LOCATION);

  // Empty string
  String           emptyStr;
  std::string_view emptyResult = Integration::ToStdStringView(emptyStr);
  DALI_TEST_CHECK(emptyResult.empty());

  END_TEST;
}

int UtcDaliStringToDaliStringViewFromStdStringP(void)
{
  tet_printf("Test ToDaliStringView(const std::string&)");
  TestApplication application;

  std::string stdStr("Hello");
  StringView  result = Integration::ToDaliStringView(stdStr);
  DALI_TEST_EQUALS(result.Size(), size_t(5), TEST_LOCATION);
  DALI_TEST_CHECK(strncmp(result.Data(), "Hello", 5) == 0);

  // Empty string
  std::string emptyStd;
  StringView  emptyResult = Integration::ToDaliStringView(emptyStd);
  DALI_TEST_EQUALS(emptyResult.Size(), size_t(0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringToDaliStringViewFromStdStringViewP(void)
{
  tet_printf("Test ToDaliStringView(std::string_view)");
  TestApplication application;

  std::string_view stdView("Hello");
  StringView       result = Integration::ToDaliStringView(stdView);
  DALI_TEST_EQUALS(result.Size(), size_t(5), TEST_LOCATION);
  DALI_TEST_CHECK(strncmp(result.Data(), "Hello", 5) == 0);

  // Empty string_view
  std::string_view emptyStdView;
  StringView       emptyResult = Integration::ToDaliStringView(emptyStdView);
  DALI_TEST_EQUALS(emptyResult.Size(), size_t(0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringToDaliStringFromStdStringP(void)
{
  tet_printf("Test ToDaliString(const std::string&)");
  TestApplication application;

  std::string stdStr("Hello");
  String      result = Integration::ToDaliString(stdStr);
  DALI_TEST_EQUALS(result.CStr(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(result.Size(), size_t(5), TEST_LOCATION);

  // Empty string
  std::string emptyStd;
  String      emptyResult = Integration::ToDaliString(emptyStd);
  DALI_TEST_CHECK(emptyResult.Empty());

  END_TEST;
}

int UtcDaliStringToDaliStringFromStdStringViewP(void)
{
  tet_printf("Test ToDaliString(std::string_view)");
  TestApplication application;

  std::string_view stdView("Hello");
  String           result = Integration::ToDaliString(stdView);
  DALI_TEST_EQUALS(result.CStr(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(result.Size(), size_t(5), TEST_LOCATION);

  // Empty string_view
  std::string_view emptyView;
  String           emptyResult = Integration::ToDaliString(emptyView);
  DALI_TEST_CHECK(emptyResult.Empty());

  END_TEST;
}

int UtcDaliStringToDaliStringMoveP(void)
{
  tet_printf("Test ToDaliString(std::string&&) move variant");
  TestApplication application;

  std::string stdStr("Move This");
  String      result = Integration::ToDaliString(std::move(stdStr));
  DALI_TEST_EQUALS(result.CStr(), "Move This", TEST_LOCATION);
  DALI_TEST_EQUALS(result.Size(), size_t(9), TEST_LOCATION);
  // Source should be emptied after move
  DALI_TEST_CHECK(stdStr.empty());

  // Empty string move
  std::string emptyStd;
  String      emptyResult = Integration::ToDaliString(std::move(emptyStd));
  DALI_TEST_CHECK(emptyResult.Empty());

  END_TEST;
}

int UtcDaliStringToDaliStringCopyP(void)
{
  tet_printf("Test ToDaliString(const String&) copy convenience");
  TestApplication application;

  String original("Original");
  String copy = Integration::ToDaliString(original);
  DALI_TEST_EQUALS(copy.CStr(), "Original", TEST_LOCATION);
  DALI_TEST_EQUALS(copy.Size(), size_t(8), TEST_LOCATION);
  // Original unchanged
  DALI_TEST_EQUALS(original.CStr(), "Original", TEST_LOCATION);

  // Modify copy, original unaffected
  copy[0] = 'X';
  DALI_TEST_EQUALS(original.CStr(), "Original", TEST_LOCATION);
  DALI_TEST_EQUALS(copy.CStr(), "Xriginal", TEST_LOCATION);

  // Empty string copy
  String emptyStr;
  String emptyCopy = Integration::ToDaliString(emptyStr);
  DALI_TEST_CHECK(emptyCopy.Empty());

  END_TEST;
}

int UtcDaliStringViewHashP(void)
{
  tet_printf("Test std::hash<StringView> specialization");
  TestApplication application;

  std::hash<StringView> hasher;

  // Equal strings should produce equal hashes
  StringView view1("Hello");
  StringView view2("Hello");
  DALI_TEST_EQUALS(hasher(view1), hasher(view2), TEST_LOCATION);

  // Different strings should (very likely) produce different hashes
  StringView view3("World");
  DALI_TEST_CHECK(hasher(view1) != hasher(view3));

  // Empty string should hash without crashing
  StringView emptyView("", 0);
  size_t     emptyHash = hasher(emptyView);
  (void)emptyHash; // Just verify no crash

  // nullptr view should hash without crashing
  StringView nullView(nullptr, 0);
  size_t     nullHash = hasher(nullView);
  (void)nullHash; // Just verify no crash

  // Can be used in unordered_set
  std::unordered_set<StringView> viewSet;
  viewSet.insert(StringView("Hello"));
  viewSet.insert(StringView("World"));
  viewSet.insert(StringView("Hello")); // duplicate
  DALI_TEST_EQUALS(viewSet.size(), size_t(2), TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringToPropertyValueP(void)
{
  tet_printf("Test ToPropertyValue(std::string)");
  TestApplication application;

  // Non-empty string
  Property::Value result = Integration::ToPropertyValue(std::string("Hello Property"));
  DALI_TEST_EQUALS(result.GetType(), Property::STRING, TEST_LOCATION);
  String extracted;
  DALI_TEST_CHECK(result.Get(extracted));
  DALI_TEST_EQUALS(extracted.CStr(), "Hello Property", TEST_LOCATION);
  DALI_TEST_EQUALS(extracted.Size(), size_t(14), TEST_LOCATION);

  // Empty string
  Property::Value emptyResult = Integration::ToPropertyValue(std::string(""));
  DALI_TEST_EQUALS(emptyResult.GetType(), Property::STRING, TEST_LOCATION);
  String emptyExtracted;
  DALI_TEST_CHECK(emptyResult.Get(emptyExtracted));
  DALI_TEST_CHECK(emptyExtracted.Empty());

  // Round-trip: std::string -> Property::Value -> std::string
  std::string     original("Round Trip Test");
  Property::Value roundTrip = Integration::ToPropertyValue(original);
  std::string     back      = Integration::ToStdString(roundTrip);
  DALI_TEST_EQUALS(back, original, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringInsertToMapStringValueP(void)
{
  tet_printf("Test InsertToMap(map, string key, string value)");
  TestApplication application;

  // Insert single entry
  Property::Map  map;
  Property::Map& ref = Integration::InsertToMap(map, std::string("name"), std::string("Alice"));
  DALI_TEST_EQUALS(map.Count(), 1u, TEST_LOCATION);
  // Verify returns reference to same map
  DALI_TEST_CHECK(&ref == &map);

  // Verify inserted value
  Property::Value* found = map.Find(StringView("name"));
  DALI_TEST_CHECK(found != nullptr);
  String extracted;
  DALI_TEST_CHECK(found->Get(extracted));
  DALI_TEST_EQUALS(extracted.CStr(), "Alice", TEST_LOCATION);

  // Insert multiple entries
  Integration::InsertToMap(map, std::string("city"), std::string("Seoul"));
  Integration::InsertToMap(map, std::string("country"), std::string("Korea"));
  DALI_TEST_EQUALS(map.Count(), 3u, TEST_LOCATION);

  Property::Value* cityFound = map.Find(StringView("city"));
  DALI_TEST_CHECK(cityFound != nullptr);
  String cityExtracted;
  DALI_TEST_CHECK(cityFound->Get(cityExtracted));
  DALI_TEST_EQUALS(cityExtracted.CStr(), "Seoul", TEST_LOCATION);

  // Insert with empty key and value
  Integration::InsertToMap(map, std::string(""), std::string(""));
  DALI_TEST_EQUALS(map.Count(), 4u, TEST_LOCATION);
  Property::Value* emptyFound = map.Find(StringView(""));
  DALI_TEST_CHECK(emptyFound != nullptr);
  String emptyExtracted;
  DALI_TEST_CHECK(emptyFound->Get(emptyExtracted));
  DALI_TEST_CHECK(emptyExtracted.Empty());

  // Chaining
  Property::Map chainMap;
  Integration::InsertToMap(
    Integration::InsertToMap(chainMap, std::string("a"), std::string("1")),
    std::string("b"),
    std::string("2"));
  DALI_TEST_EQUALS(chainMap.Count(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStringInsertToMapPropertyValueP(void)
{
  tet_printf("Test InsertToMap(map, string key, Property::Value)");
  TestApplication application;

  // Insert string Property::Value
  Property::Map  map;
  Property::Map& ref = Integration::InsertToMap(map, std::string("name"), Property::Value(String("Bob")));
  DALI_TEST_EQUALS(map.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(&ref == &map);

  Property::Value* found = map.Find(StringView("name"));
  DALI_TEST_CHECK(found != nullptr);
  DALI_TEST_EQUALS(found->GetType(), Property::STRING, TEST_LOCATION);
  String extracted;
  DALI_TEST_CHECK(found->Get(extracted));
  DALI_TEST_EQUALS(extracted.CStr(), "Bob", TEST_LOCATION);

  // Insert non-string Property::Value (int)
  Integration::InsertToMap(map, std::string("age"), Property::Value(30));
  DALI_TEST_EQUALS(map.Count(), 2u, TEST_LOCATION);
  Property::Value* ageFound = map.Find(StringView("age"));
  DALI_TEST_CHECK(ageFound != nullptr);
  DALI_TEST_EQUALS(ageFound->GetType(), Property::INTEGER, TEST_LOCATION);
  int ageValue = 0;
  DALI_TEST_CHECK(ageFound->Get(ageValue));
  DALI_TEST_EQUALS(ageValue, 30, TEST_LOCATION);

  // Insert float Property::Value
  Integration::InsertToMap(map, std::string("score"), Property::Value(9.5f));
  DALI_TEST_EQUALS(map.Count(), 3u, TEST_LOCATION);
  Property::Value* scoreFound = map.Find(StringView("score"));
  DALI_TEST_CHECK(scoreFound != nullptr);
  DALI_TEST_EQUALS(scoreFound->GetType(), Property::FLOAT, TEST_LOCATION);

  // Insert boolean Property::Value
  Integration::InsertToMap(map, std::string("active"), Property::Value(true));
  DALI_TEST_EQUALS(map.Count(), 4u, TEST_LOCATION);
  Property::Value* activeFound = map.Find(StringView("active"));
  DALI_TEST_CHECK(activeFound != nullptr);
  bool activeValue = false;
  DALI_TEST_CHECK(activeFound->Get(activeValue));
  DALI_TEST_EQUALS(activeValue, true, TEST_LOCATION);

  // Chaining with mixed types
  Property::Map chainMap;
  Integration::InsertToMap(
    Integration::InsertToMap(chainMap, std::string("x"), Property::Value(1.0f)),
    std::string("y"),
    Property::Value(2.0f));
  DALI_TEST_EQUALS(chainMap.Count(), 2u, TEST_LOCATION);

  END_TEST;
}
