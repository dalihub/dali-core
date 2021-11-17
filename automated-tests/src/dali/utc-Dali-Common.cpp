/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/common/dali-common.h>

using namespace Dali;

void utc_dali_common_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_common_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliCommonPrintBackTrace(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::DaliPrintBackTrace");

  try
  {
    Dali::DaliPrintBackTrace();
    tet_result(TET_PASS);
  }
  catch(...)
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}
