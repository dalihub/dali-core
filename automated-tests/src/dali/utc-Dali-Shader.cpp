/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_shader_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_shader_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static const char* VertexSource =
"This is a custom vertex shader\n"
"made on purpose to look nothing like a normal vertex shader inside dali\n";

static const char* FragmentSource =
"This is a custom fragment shader\n"
"made on purpose to look nothing like a normal fragment shader inside dali\n";

} // anon namespace


int UtcDaliShaderMethodNew01(void)
{
  TestApplication application;

  Shader shader = Shader::New( VertexSource, FragmentSource );
  DALI_TEST_EQUALS((bool)shader, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderMethodNew02(void)
{
  TestApplication application;

  Shader shader;
  DALI_TEST_EQUALS((bool)shader, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderDownCast01(void)
{
  TestApplication application;

  Shader shader = Shader::New(VertexSource, FragmentSource);

  BaseHandle handle(shader);
  Shader shader2 = Shader::DownCast(handle);
  DALI_TEST_EQUALS( (bool)shader2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliShaderDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  Shader shader = Shader::DownCast(handle);
  DALI_TEST_EQUALS( (bool)shader, false, TEST_LOCATION );
  END_TEST;
}
