//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "dali/public-api/dali-core.h"

#include <cstdio>

// Link with TET Test application, need to redefine TET functions
#define tet_infoline printf
#define tet_printf printf
#include "test-application.h"

/*****************************************************************************
 * Test to see if dali is linking correctly.
 * Only really tests that the internal function/method definitions exist and
 * links to functions/methods in other libraries linked by Dali-Core.
 * If a definition of the Public or Integration API does not exist, then it
 * will not be checked here.
 * Also ensures TET Test Application is kept up-to-date.
 */

int main(int argc, char **argv)
{
#ifndef _ARCH_ARM_
  Dali::TestApplication application;
#endif
  return 0;
}
