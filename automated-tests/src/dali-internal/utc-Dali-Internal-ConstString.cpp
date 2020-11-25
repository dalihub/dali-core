/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <random>
#include <string>

// Internal headers are allowed here
#include <dali/internal/common/const-string.h>

using namespace Dali;

namespace
{
std::string RandomString(size_t length)
{
  static auto& chrs =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  thread_local static std::mt19937                                          rg{std::random_device{}()};
  thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

  std::string s;

  s.reserve(length);

  while(length--)
    s += chrs[pick(rg)];

  return s;
}
} // namespace

void utc_dali_internal_conststring_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_conststring_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliConstStringEmpty(void)
{
  Internal::ConstString str1;
  DALI_TEST_EQUALS(str1.IsEmpty(), true, TEST_LOCATION);

  Internal::ConstString str2("hello");
  DALI_TEST_EQUALS(str2.IsEmpty(), false, TEST_LOCATION);

  str2.Clear();
  DALI_TEST_EQUALS(str2.IsEmpty(), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstStringConstruct(void)
{
  Internal::ConstString str1("string1");
  DALI_TEST_EQUALS(str1.GetCString(), "string1", TEST_LOCATION);

  Internal::ConstString str2("string1");

  DALI_TEST_EQUALS(str2.GetCString(), "string1", TEST_LOCATION);

  bool samePointer = (str1.GetCString() == str2.GetCString());
  DALI_TEST_EQUALS(samePointer, true, TEST_LOCATION);

  str1.Clear();
  DALI_TEST_EQUALS(str2.GetCString(), "string1", TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstStringConstructStringView(void)
{
  Internal::ConstString str1(std::string_view("random string"));

  DALI_TEST_EQUALS(str1.GetStringView().data(), "random string", TEST_LOCATION);
  DALI_TEST_EQUALS(str1.GetStringView().size(), 13, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstStringSetString(void)
{
  Internal::ConstString str1(std::string_view("current string"));

  DALI_TEST_EQUALS(str1.GetStringView().data(), "current string", TEST_LOCATION);

  str1.SetString("new string");

  DALI_TEST_EQUALS(str1.GetCString(), "new string", TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstStringStressTest(void)
{
  static constexpr size_t DB_SIZE = 2000;

  std::vector<std::string> Database;
  Database.reserve(DB_SIZE);

  std::vector<Internal::ConstString> constStringDB;
  constStringDB.reserve(DB_SIZE);

  std::vector<Internal::ConstString> constStringDB1;
  constStringDB1.reserve(DB_SIZE);

  for(auto i = 0u; i < DB_SIZE; i++)
  {
    if(i % 3 == 0)
    {
      Database.push_back(RandomString(10));
    }
    else if(i % 4 == 0)
    {
      Database.push_back(RandomString(7));
    }
    else
    {
      Database.push_back(RandomString(11));
    }
    constStringDB.push_back(Internal::ConstString(Database[i]));
    constStringDB1.push_back(Internal::ConstString(Database[i]));
  }

  // check eqality betwwen original string and constString
  for(auto i = 0u; i < DB_SIZE; i++)
  {
    DALI_TEST_EQUALS(constStringDB[i].GetCString(), Database[i].c_str(), TEST_LOCATION);
  }

  // check pointer eqality betwwen 2 constString
  for(auto i = 0u; i < DB_SIZE; i++)
  {
    bool pointerEqual = (constStringDB[i] == constStringDB1[i]);
    DALI_TEST_EQUALS(pointerEqual, true, TEST_LOCATION);
  }

  END_TEST;
}
