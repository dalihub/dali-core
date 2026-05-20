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

#ifndef DALI_INTEGRATION_API_LOCALE_NUMERIC_GUARD_H
#define DALI_INTEGRATION_API_LOCALE_NUMERIC_GUARD_H

// EXTERNAL INCLUDES
#include <clocale> ///< for setlocale
#include <cstdlib> ///< for free
#include <cstring> ///< for strdup

namespace Dali
{

/**
 * @brief RAII guard that sets LC_NUMERIC to "C" on construction and restores
 *        the previous locale on destruction.
 *
 * This ensures floating-point values are formatted with a dot (e.g. "3.14")
 * rather than a locale-specific decimal separator (e.g. "3,14" in de_DE),
 * which is required for generating valid JSON output and other data exchange
 * formats that mandate the C locale numeric representation.
 *
 * Usage:
 * @code
 * void MyJsonDumper()
 * {
 *   LocaleNumericGuard localeGuard; // LC_NUMERIC set to "C" here
 *   // ... code that outputs floats ...
 * } // LC_NUMERIC restored to previous value here
 * @endcode
 */
class LocaleNumericGuard
{
public:
  LocaleNumericGuard()
  : mPreviousLocale(nullptr)
  {
    // Save the current LC_NUMERIC locale
    mPreviousLocale = setlocale(LC_NUMERIC, nullptr);
    if(mPreviousLocale)
    {
      mPreviousLocale = strdup(mPreviousLocale);
    }
    // Set LC_NUMERIC to "C" for locale-independent numeric formatting
    setlocale(LC_NUMERIC, "C");
  }

  ~LocaleNumericGuard()
  {
    // Restore the previous LC_NUMERIC locale
    if(mPreviousLocale)
    {
      setlocale(LC_NUMERIC, mPreviousLocale);
      free(mPreviousLocale);
    }
  }

  // Non-copyable. Non-movable.
  LocaleNumericGuard(const LocaleNumericGuard&)                    = delete;
  LocaleNumericGuard& operator=(const LocaleNumericGuard&)         = delete;
  LocaleNumericGuard(LocaleNumericGuard&& rhs) noexcept            = delete;
  LocaleNumericGuard& operator=(LocaleNumericGuard&& rhs) noexcept = delete;

private:
  char* mPreviousLocale;
};

} // namespace Dali

#endif // DALI_INTEGRATION_API_LOCALE_NUMERIC_GUARD_H
