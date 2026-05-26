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

#include <dali-test-suite-utils.h>
#include <dali/integration-api/locale-numeric-guard.h>
#include <dali/public-api/dali-core.h>
#include <clocale>

#include <iostream>

using namespace Dali;

void utc_dali_locale_numeric_guard_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_locale_numeric_guard_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLocaleNumericGuard01(void)
{
  const char* originalLocale = "en_US.UTF-8";
  const char* classicLocale  = "C";

  char* prev = setlocale(LC_NUMERIC, originalLocale);
  DALI_LOG_ERROR("prev : %s\n", prev);

  DALI_TEST_EQUALS(std::string(originalLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);
  {
    DALI_TEST_EQUALS(std::string(originalLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);
    Dali::LocaleNumericGuard guard;
    DALI_TEST_EQUALS(std::string(classicLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);
  }
  DALI_TEST_EQUALS(std::string(originalLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);

  // Test multiple depth.
  {
    DALI_TEST_EQUALS(std::string(originalLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);
    Dali::LocaleNumericGuard guard;
    DALI_TEST_EQUALS(std::string(classicLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);
    {
      DALI_TEST_EQUALS(std::string(classicLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);
      Dali::LocaleNumericGuard guard2;
      DALI_TEST_EQUALS(std::string(classicLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);
      {
        DALI_TEST_EQUALS(std::string(classicLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);
        Dali::LocaleNumericGuard guard3;
        DALI_TEST_EQUALS(std::string(classicLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);
      }
    }
    DALI_TEST_EQUALS(std::string(classicLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);
  }
  DALI_TEST_EQUALS(std::string(originalLocale), std::string(setlocale(LC_NUMERIC, nullptr)), TEST_LOCATION);

  // Revert it
  setlocale(LC_NUMERIC, prev);

  END_TEST;
}