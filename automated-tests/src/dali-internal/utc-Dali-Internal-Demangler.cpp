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
#include <dali/internal/event/common/demangler.h>
#include <typeinfo>

using namespace Dali;
using namespace Dali::Internal;

void utc_dali_internal_demangler_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_demangler_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliInternalDemangleClassName(void)
{
  TestApplication application;

  tet_infoline("Test DemangleClassName function.");

  // Test with empty string - should return empty string
  std::string emptyResult = DemangleClassName("");
  DALI_TEST_CHECK(emptyResult.empty());

  // Test with a nested class name format (Dali::Actor)
  // typeid(Dali::Actor).name() returns something like "N5Dali5ActorE"
  std::string actorResult = DemangleClassName(typeid(Dali::Actor).name());
  DALI_TEST_EQUALS(actorResult, "Actor", TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalDemangleTypeInfoName(void)
{
  TestApplication application;

  tet_infoline("Test DemangleTypeInfoName function.");

  // Test with a valid mangled name (int on GCC/Clang)
  std::string intResult = DemangleTypeInfoName(typeid(int).name());
  DALI_TEST_EQUALS(intResult, "int", TEST_LOCATION);

  // Test with a valid mangled name (float on GCC/Clang)
  std::string floatResult = DemangleTypeInfoName(typeid(float).name());
  DALI_TEST_EQUALS(floatResult, "float", TEST_LOCATION);

  // Test with null pointer - should return empty string
  std::string nullResult = DemangleTypeInfoName(nullptr);
  DALI_TEST_CHECK(nullResult.empty());

  // Test with empty string - should return empty string
  std::string emptyResult = DemangleTypeInfoName("");
  DALI_TEST_CHECK(emptyResult.empty());

  // Test with a mangled name that cannot be demangled
  // On GCC/Clang, an invalid mangled name should return the original string
  std::string invalidResult = DemangleTypeInfoName("_INVALID_MANGLED_NAME");
  DALI_TEST_EQUALS(invalidResult, "_INVALID_MANGLED_NAME", TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalDemangleTypeInfoNameNamespaced(void)
{
  TestApplication application;

  tet_infoline("Test DemangleTypeInfoName with namespaced types.");

  // Test with a namespaced type (Dali::Actor)
  std::string actorResult = DemangleTypeInfoName(typeid(Dali::Actor).name());
  DALI_TEST_EQUALS(actorResult, "Dali::Actor", TEST_LOCATION);

  // Test with a nested type
  std::string vectorResult = DemangleTypeInfoName(typeid(std::vector<int>).name());
  // The exact format varies by compiler, but it should contain "vector"
  DALI_TEST_CHECK(vectorResult.find("vector") != std::string::npos);

  END_TEST;
}
