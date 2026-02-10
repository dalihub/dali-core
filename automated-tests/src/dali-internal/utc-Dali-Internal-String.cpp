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
#include <dali/public-api/signals/callback.h>
#include <stdlib.h>

#include <iostream>

// Internal headers are allowed here
#include <dali/integration-api/dali-string-impl.h>
#include <dali/integration-api/string-utils.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/events/actor-observer.h>

using namespace Dali;

int UtcDaliStringImplMove(void)
{
  tet_printf("Testing Dali::String move semantics...\n");

  // Test 1: Move from Dali::String to std::string
  {
    tet_printf("Test 1: Move conversion...\n");
    String      daliStr("Hello, World!");
    std::string stdStr = Dali::Integration::ToStdString(std::move(daliStr));

    DALI_TEST_CHECK(stdStr == "Hello, World!");
    DALI_TEST_CHECK(daliStr.Empty());
    DALI_TEST_CHECK(daliStr.Size() == 0);
    tet_printf("  ✓ Move conversion works correctly\n");
  }

  // Test 2: Copy conversion (original functionality)
  {
    tet_printf("Test 2: Copy conversion...\n");
    String      daliStr("Original string");
    std::string stdStr = Dali::Integration::ToStdString(daliStr);

    DALI_TEST_CHECK(stdStr == "Original string");
    DALI_TEST_CHECK(!daliStr.Empty());
    DALI_TEST_CHECK(daliStr.Size() == 15);
    tet_printf("  ✓ Copy conversion works correctly\n");
  }

  // Test 3: Large string (tests heap allocation move)
  {
    tet_printf("Test 3: Large string move...\n");
    std::string largeContent(1000, 'X');
    String      daliStr(StringView(largeContent.data(), largeContent.size()));

    std::string moved = Dali::Integration::ToStdString(std::move(daliStr));

    DALI_TEST_CHECK(moved == largeContent);
    DALI_TEST_CHECK(daliStr.Empty());
    tet_printf("  ✓ Large string move works correctly\n");
  }

  // Test 4: Empty string move
  {
    tet_printf("Test 4: Empty string move...\n");
    String      daliStr;
    std::string stdStr = Dali::Integration::ToStdString(std::move(daliStr));

    DALI_TEST_CHECK(stdStr.empty());
    DALI_TEST_CHECK(daliStr.Empty());
    tet_printf("  ✓ Empty string move works correctly\n");
  }

  // Test 5: String with null characters
  {
    tet_printf("Test 5: String with null characters...\n");
    std::string content("Hello\0World", 11);
    String      daliStr(StringView(content.data(), content.size()));

    std::string moved = Dali::Integration::ToStdString(std::move(daliStr));

    DALI_TEST_CHECK(moved == content);
    DALI_TEST_CHECK(moved.size() == 11);
    DALI_TEST_CHECK(daliStr.Empty());
    tet_printf("  ✓ String with null characters works correctly\n");
  }

  // Test 6: Multiple moves
  {
    tet_printf("Test 6: Multiple moves...\n");
    String daliStr1("First");
    String daliStr2("Second");

    std::string stdStr1 = Dali::Integration::ToStdString(std::move(daliStr1));
    std::string stdStr2 = Dali::Integration::ToStdString(std::move(daliStr2));

    DALI_TEST_CHECK(stdStr1 == "First");
    DALI_TEST_CHECK(stdStr2 == "Second");
    DALI_TEST_CHECK(daliStr1.Empty());
    DALI_TEST_CHECK(daliStr2.Empty());
    tet_printf("  ✓ Multiple moves work correctly\n");
  }

  // Test 7: Move and reuse
  {
    tet_printf("Test 7: Move and reuse...\n");
    String daliStr("Original");

    std::string stdStr = Dali::Integration::ToStdString(std::move(daliStr));
    DALI_TEST_CHECK(stdStr == "Original");
    DALI_TEST_CHECK(daliStr.Empty());

    // Reuse the moved-from string
    daliStr = String("New content");
    DALI_TEST_CHECK(daliStr.CStr() == std::string("New content"));

    tet_printf("  ✓ Move and reuse works correctly\n");
  }

  END_TEST;
}

int UtcDaliStringViewConversions(void)
{
  tet_printf("Testing string view conversion functions...\n");

  // Test 1: ToStdStringView from StringView
  {
    tet_printf("Test 1: ToStdStringView from StringView...\n");
    String           daliStr("Hello, World!");
    StringView       daliView(daliStr.CStr(), daliStr.Size());
    std::string_view stdView = Dali::Integration::ToStdStringView(daliView);

    DALI_TEST_CHECK(stdView.size() == 13);
    DALI_TEST_CHECK(stdView == std::string_view("Hello, World!"));
    DALI_TEST_CHECK(stdView.data() == daliView.Data());
    tet_printf("  ✓ ToStdStringView from StringView works correctly\n");
  }

  // Test 2: ToStdStringView from String
  {
    tet_printf("Test 2: ToStdStringView from String...\n");
    String           daliStr("Test content");
    std::string_view stdView = Dali::Integration::ToStdStringView(daliStr);

    DALI_TEST_CHECK(stdView.size() == 12);
    DALI_TEST_CHECK(stdView == std::string_view("Test content"));
    DALI_TEST_CHECK(stdView.data() == daliStr.CStr());
    tet_printf("  ✓ ToStdStringView from String works correctly\n");
  }

  // Test 3: ToStdStringView with empty StringView
  {
    tet_printf("Test 3: ToStdStringView with empty StringView...\n");
    StringView       emptyView;
    std::string_view stdView = Dali::Integration::ToStdStringView(emptyView);

    DALI_TEST_CHECK(stdView.empty());
    DALI_TEST_CHECK(stdView.size() == 0);
    DALI_TEST_CHECK(stdView.data() == nullptr);
    tet_printf("  ✓ ToStdStringView with empty StringView works correctly\n");
  }

  // Test 4: ToStdStringView with empty String
  {
    tet_printf("Test 4: ToStdStringView with empty String...\n");
    String           emptyStr;
    std::string_view stdView = Dali::Integration::ToStdStringView(emptyStr);

    DALI_TEST_CHECK(stdView.empty());
    DALI_TEST_CHECK(stdView.size() == 0);
    tet_printf("  ✓ ToStdStringView with empty String works correctly\n");
  }

  // Test 5: ToDaliStringView from std::string
  {
    tet_printf("Test 5: ToDaliStringView from std::string...\n");
    std::string stdStr("Content test");
    StringView  daliView = Dali::Integration::ToDaliStringView(stdStr);

    DALI_TEST_CHECK(daliView.Size() == 12);
    DALI_TEST_CHECK(daliView.Data() == stdStr.data());
    DALI_TEST_CHECK(String(daliView).CStr() == stdStr);
    tet_printf("  ✓ ToDaliStringView from std::string works correctly\n");
  }

  // Test 6: ToDaliStringView from std::string_view
  {
    tet_printf("Test 6: ToDaliStringView from std::string_view...\n");
    std::string_view stdView("Another test");
    StringView       daliView = Dali::Integration::ToDaliStringView(stdView);

    DALI_TEST_CHECK(daliView.Size() == 12);
    DALI_TEST_CHECK(daliView.Data() == stdView.data());
    DALI_TEST_CHECK(String(daliView).CStr() == stdView);
    tet_printf("  ✓ ToDaliStringView from std::string_view works correctly\n");
  }

  // Test 7: ToDaliStringView with empty std::string
  {
    tet_printf("Test 7: ToDaliStringView with empty std::string...\n");
    std::string emptyStr;
    StringView  daliView = Dali::Integration::ToDaliStringView(emptyStr);

    DALI_TEST_CHECK(daliView.Empty());
    DALI_TEST_CHECK(daliView.Size() == 0);
    tet_printf("  ✓ ToDaliStringView with empty std::string works correctly\n");
  }

  // Test 8: ToDaliStringView with empty std::string_view
  {
    tet_printf("Test 8: ToDaliStringView with empty std::string_view...\n");
    std::string_view emptyView;
    StringView       daliView = Dali::Integration::ToDaliStringView(emptyView);

    DALI_TEST_CHECK(daliView.Empty());
    DALI_TEST_CHECK(daliView.Size() == 0);
    tet_printf("  ✓ ToDaliStringView with empty std::string_view works correctly\n");
  }

  // Test 9: ToStdStringView with String containing null characters
  {
    tet_printf("Test 9: ToStdStringView with String containing null characters...\n");
    std::string      content("Hello\0World", 11);
    String           daliStr(StringView(content.data(), content.size()));
    std::string_view stdView = Dali::Integration::ToStdStringView(daliStr);

    DALI_TEST_CHECK(stdView.size() == 11);
    DALI_TEST_CHECK(stdView[0] == 'H');
    DALI_TEST_CHECK(stdView[5] == '\0');
    DALI_TEST_CHECK(stdView[10] == 'd');
    tet_printf("  ✓ ToStdStringView with null characters works correctly\n");
  }

  // Test 10: ToDaliStringView with std::string_view containing null characters
  {
    tet_printf("Test 10: ToDaliStringView with std::string_view containing null characters...\n");
    std::string      content("Test\0Data", 9);
    std::string_view stdView(content.data(), content.size());
    StringView       daliView = Dali::Integration::ToDaliStringView(stdView);

    DALI_TEST_CHECK(daliView.Size() == 9);
    DALI_TEST_CHECK(daliView.Data()[0] == 'T');
    DALI_TEST_CHECK(daliView.Data()[4] == '\0');
    DALI_TEST_CHECK(daliView.Data()[8] == 'a');
    tet_printf("  ✓ ToDaliStringView with null characters works correctly\n");
  }

  // Test 11: ToStdStringView reference semantics (should not copy data)
  {
    tet_printf("Test 11: ToStdStringView reference semantics...\n");
    String           daliStr("Reference test");
    std::string_view stdView1 = Dali::Integration::ToStdStringView(daliStr);
    std::string_view stdView2 = Dali::Integration::ToStdStringView(daliStr);

    // Both views should point to the same data
    DALI_TEST_CHECK(stdView1.data() == stdView2.data());
    DALI_TEST_CHECK(stdView1.size() == stdView2.size());
    tet_printf("  ✓ ToStdStringView reference semantics work correctly\n");
  }

  // Test 12: ToDaliStringView reference semantics (should not copy data)
  {
    tet_printf("Test 12: ToDaliStringView reference semantics...\n");
    std::string stdStr("Original content");
    StringView  daliView1 = Dali::Integration::ToDaliStringView(stdStr);
    StringView  daliView2 = Dali::Integration::ToDaliStringView(stdStr);

    // Both views should point to the same data
    DALI_TEST_CHECK(daliView1.Data() == daliView2.Data());
    DALI_TEST_CHECK(daliView1.Size() == daliView2.Size());
    tet_printf("  ✓ ToDaliStringView reference semantics work correctly\n");
  }

  END_TEST;
}

int UtcDaliStringImplDirect(void)
{
  tet_printf("Testing Dali::Internal::StringImpl directly...\n");

  // Test 1: StringImpl constructor and Extract
  {
    tet_printf("Test 1: StringImpl constructor and Extract...\n");
    String               daliStr("Test string");
    Internal::StringImpl impl(std::move(daliStr));

    DALI_TEST_CHECK(daliStr.Empty());

    std::string stdStr = impl.Extract();
    DALI_TEST_CHECK(stdStr == "Test string");
    tet_printf("  ✓ StringImpl constructor and Extract work correctly\n");
  }

  // Test 2: StringImpl GetString accessor
  {
    tet_printf("Test 2: StringImpl GetString accessor...\n");
    String               daliStr("Accessor test");
    Internal::StringImpl impl(std::move(daliStr));

    const std::string& strRef = impl.GetString();
    DALI_TEST_CHECK(strRef == "Accessor test");
    tet_printf("  ✓ StringImpl GetString accessor works correctly\n");
  }

  // Test 3: Empty StringImpl
  {
    tet_printf("Test 3: Empty StringImpl...\n");
    String               daliStr;
    Internal::StringImpl impl(std::move(daliStr));

    std::string stdStr = impl.Extract();
    DALI_TEST_CHECK(stdStr.empty());
    tet_printf("  ✓ Empty StringImpl works correctly\n");
  }

  // Test 4: StringImpl with large string
  {
    tet_printf("Test 4: StringImpl with large string...\n");
    std::string          large(2000, 'Y');
    String               daliStr(StringView(large.data(), large.size()));
    Internal::StringImpl impl(std::move(daliStr));

    std::string extracted = impl.Extract();
    DALI_TEST_CHECK(extracted == large);
    tet_printf("  ✓ StringImpl with large string works correctly\n");
  }

  END_TEST;
}

int UtcDaliStringImplConstAccessors(void)
{
  tet_printf("Testing StringImpl const accessor functions...\n");

  // Test 1: Const GetString accessor
  {
    tet_printf("Test 1: Const GetString accessor...\n");
    String               daliStr("Const test string");
    Internal::StringImpl impl(std::move(daliStr));

    // Create a const reference to test the const overload
    const Internal::StringImpl& constImpl = impl;
    const std::string&          strRef    = constImpl.GetString();

    DALI_TEST_CHECK(strRef == "Const test string");
    DALI_TEST_CHECK(strRef.size() == 17);
    tet_printf("  ✓ Const GetString accessor works correctly\n");
  }

  // Test 2: Const GetString with empty string
  {
    tet_printf("Test 2: Const GetString with empty string...\n");
    String               daliStr;
    Internal::StringImpl impl(std::move(daliStr));

    // Create a const reference to test the const overload
    const Internal::StringImpl& constImpl = impl;
    const std::string&          strRef    = constImpl.GetString();

    DALI_TEST_CHECK(strRef.empty());
    DALI_TEST_CHECK(strRef.size() == 0);
    tet_printf("  ✓ Const GetString with empty string works correctly\n");
  }

  // Test 3: Const GetString with large string
  {
    tet_printf("Test 3: Const GetString with large string...\n");
    std::string          large(1500, 'Z');
    String               daliStr(StringView(large.data(), large.size()));
    Internal::StringImpl impl(std::move(daliStr));

    // Create a const reference to test the const overload
    const Internal::StringImpl& constImpl = impl;
    const std::string&          strRef    = constImpl.GetString();

    DALI_TEST_CHECK(strRef == large);
    DALI_TEST_CHECK(strRef.size() == 1500);
    tet_printf("  ✓ Const GetString with large string works correctly\n");
  }

  // Test 4: Const GetString with null characters
  {
    tet_printf("Test 4: Const GetString with null characters...\n");
    std::string          content("Null\0Embedded", 13);
    String               daliStr(StringView(content.data(), content.size()));
    Internal::StringImpl impl(std::move(daliStr));

    // Create a const reference to test the const overload
    const Internal::StringImpl& constImpl = impl;
    const std::string&          strRef    = constImpl.GetString();

    DALI_TEST_CHECK(strRef.size() == 13);
    DALI_TEST_CHECK(strRef[0] == 'N');
    DALI_TEST_CHECK(strRef[4] == '\0');
    DALI_TEST_CHECK(strRef[12] == 'd');
    tet_printf("  ✓ Const GetString with null characters works correctly\n");
  }

  // Test 5: Verify const and non-const GetString return same data
  {
    tet_printf("Test 5: Verify const and non-const GetString return same data...\n");
    String               daliStr("Consistency check");
    Internal::StringImpl impl(std::move(daliStr));

    // Get reference via non-const accessor
    std::string& nonConstRef = impl.GetString();

    // Get reference via const accessor
    const Internal::StringImpl& constImpl = impl;
    const std::string&          constRef  = constImpl.GetString();

    // Both should point to the same data
    DALI_TEST_CHECK(&nonConstRef == &constRef);
    DALI_TEST_CHECK(nonConstRef == constRef);
    DALI_TEST_CHECK(nonConstRef == "Consistency check");
    tet_printf("  ✓ Const and non-const GetString return same data\n");
  }

  // Test 6: Multiple const accesses
  {
    tet_printf("Test 6: Multiple const accesses...\n");
    String               daliStr("Multiple access test");
    Internal::StringImpl impl(std::move(daliStr));

    // Create a const reference
    const Internal::StringImpl& constImpl = impl;

    // Multiple accesses should all return valid references
    const std::string& ref1 = constImpl.GetString();
    const std::string& ref2 = constImpl.GetString();

    DALI_TEST_CHECK(ref1 == ref2);
    DALI_TEST_CHECK(ref1 == "Multiple access test");
    DALI_TEST_CHECK(&ref1 == &ref2); // Same underlying data
    tet_printf("  ✓ Multiple const accesses work correctly\n");
  }

  END_TEST;
}

int UtcDaliStringImplCopyVsMove(void)
{
  tet_printf("Testing Copy vs Move efficiency...\n");

  // Test 1: Copy should preserve source
  {
    tet_printf("Test 1: Copy preserves source...\n");
    String      daliStr("Copy test");
    std::string stdStr = Dali::Integration::ToStdString(daliStr);

    DALI_TEST_CHECK(stdStr == "Copy test");
    DALI_TEST_CHECK(!daliStr.Empty());
    DALI_TEST_CHECK(daliStr.CStr() == std::string("Copy test"));
    tet_printf("  ✓ Copy preserves source correctly\n");
  }

  // Test 2: Move should empty source
  {
    tet_printf("Test 2: Move empties source...\n");
    String      daliStr("Move test");
    std::string stdStr = Dali::Integration::ToStdString(std::move(daliStr));

    DALI_TEST_CHECK(stdStr == "Move test");
    DALI_TEST_CHECK(daliStr.Empty());
    tet_printf("  ✓ Move empties source correctly\n");
  }

  // Test 3: Verify no data loss in move
  {
    tet_printf("Test 3: Verify no data loss in move...\n");
    String      daliStr("Data integrity test");
    std::string originalCopy = Dali::Integration::ToStdString(daliStr);

    std::string movedStr = Dali::Integration::ToStdString(std::move(daliStr));

    DALI_TEST_CHECK(originalCopy == movedStr);
    DALI_TEST_CHECK(originalCopy == "Data integrity test");
    tet_printf("  ✓ No data loss in move\n");
  }

  END_TEST;
}

int UtcDaliStringImplToDaliStringP(void)
{
  tet_printf("Testing StringImpl::ToDaliString...\n");

  // Test 1: Move a std::string into a Dali::String
  {
    tet_printf("Test 1: Basic move from std::string...\n");
    std::string stdStr("Hello from std::string");
    String      daliStr = Internal::StringImpl::ToDaliString(std::move(stdStr));

    DALI_TEST_EQUALS(daliStr.CStr(), "Hello from std::string", TEST_LOCATION);
    DALI_TEST_EQUALS(daliStr.Size(), 22u, TEST_LOCATION);
    DALI_TEST_CHECK(stdStr.empty());
    tet_printf("  Done\n");
  }

  // Test 2: Move an empty std::string
  {
    tet_printf("Test 2: Move empty std::string...\n");
    std::string stdStr;
    String      daliStr = Internal::StringImpl::ToDaliString(std::move(stdStr));

    DALI_TEST_CHECK(daliStr.Empty());
    DALI_TEST_EQUALS(daliStr.Size(), 0u, TEST_LOCATION);
    tet_printf("  Done\n");
  }

  // Test 3: Move a large std::string (heap-allocated, beyond SSO)
  {
    tet_printf("Test 3: Move large std::string...\n");
    std::string large(2000, 'A');
    String      daliStr = Internal::StringImpl::ToDaliString(std::move(large));

    DALI_TEST_EQUALS(daliStr.Size(), 2000u, TEST_LOCATION);
    DALI_TEST_CHECK(daliStr[0] == 'A');
    DALI_TEST_CHECK(daliStr[1999] == 'A');
    DALI_TEST_CHECK(large.empty());
    tet_printf("  Done\n");
  }

  // Test 4: Round-trip: Dali::String -> std::string -> Dali::String
  {
    tet_printf("Test 4: Round-trip conversion...\n");
    String original("Round trip test");

    std::string stdStr    = Dali::Integration::ToStdString(std::move(original));
    String      roundTrip = Internal::StringImpl::ToDaliString(std::move(stdStr));

    DALI_TEST_EQUALS(roundTrip.CStr(), "Round trip test", TEST_LOCATION);
    DALI_TEST_CHECK(original.Empty());
    DALI_TEST_CHECK(stdStr.empty());
    tet_printf("  Done\n");
  }

  // Test 5: The resulting Dali::String is fully functional
  {
    tet_printf("Test 5: Resulting Dali::String is usable...\n");
    std::string stdStr("Usable");
    String      daliStr = Internal::StringImpl::ToDaliString(std::move(stdStr));

    // Test append
    daliStr += String(" string");
    DALI_TEST_EQUALS(daliStr.CStr(), "Usable string", TEST_LOCATION);

    // Test copy
    String copy(daliStr);
    DALI_TEST_EQUALS(copy.CStr(), "Usable string", TEST_LOCATION);

    // Test move
    String moved(std::move(daliStr));
    DALI_TEST_EQUALS(moved.CStr(), "Usable string", TEST_LOCATION);
    DALI_TEST_CHECK(daliStr.Empty());

    tet_printf("  Done\n");
  }

  END_TEST;
}

int UtcDaliStringUtilsToDaliStringMoveP(void)
{
  tet_printf("Testing Integration::ToDaliString move overload...\n");

  // Test 1: Move via the convenience API
  {
    tet_printf("Test 1: Basic move via ToDaliString(std::string&&)...\n");
    std::string stdStr("Moved via utils");
    String      daliStr = Dali::Integration::ToDaliString(std::move(stdStr));

    DALI_TEST_EQUALS(daliStr.CStr(), "Moved via utils", TEST_LOCATION);
    DALI_TEST_CHECK(stdStr.empty());
    tet_printf("  Done\n");
  }

  // Test 2: Empty string
  {
    tet_printf("Test 2: Move empty string via ToDaliString...\n");
    std::string stdStr;
    String      daliStr = Dali::Integration::ToDaliString(std::move(stdStr));

    DALI_TEST_CHECK(daliStr.Empty());
    tet_printf("  Done\n");
  }

  // Test 3: Large string (beyond SSO)
  {
    tet_printf("Test 3: Move large string via ToDaliString...\n");
    std::string large(2000, 'B');
    String      daliStr = Dali::Integration::ToDaliString(std::move(large));

    DALI_TEST_EQUALS(daliStr.Size(), 2000u, TEST_LOCATION);
    DALI_TEST_CHECK(daliStr[0] == 'B');
    DALI_TEST_CHECK(daliStr[1999] == 'B');
    DALI_TEST_CHECK(large.empty());
    tet_printf("  Done\n");
  }

  // Test 4: Full round-trip via convenience API
  {
    tet_printf("Test 4: Round-trip via convenience API...\n");
    std::string original("Full round trip");
    String      daliStr = Dali::Integration::ToDaliString(std::move(original));
    std::string back    = Dali::Integration::ToStdString(std::move(daliStr));

    DALI_TEST_EQUALS(back.c_str(), "Full round trip", TEST_LOCATION);
    DALI_TEST_CHECK(original.empty());
    DALI_TEST_CHECK(daliStr.Empty());
    tet_printf("  Done\n");
  }

  // Test 5: Copy overload still works (lvalue)
  {
    tet_printf("Test 5: Copy overload preserves source...\n");
    std::string stdStr("Keep me");
    String      daliStr = Dali::Integration::ToDaliString(stdStr);

    DALI_TEST_EQUALS(daliStr.CStr(), "Keep me", TEST_LOCATION);
    DALI_TEST_CHECK(!stdStr.empty());
    DALI_TEST_EQUALS(stdStr.c_str(), "Keep me", TEST_LOCATION);
    tet_printf("  Done\n");
  }

  END_TEST;
}
