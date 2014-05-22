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

#include <iostream>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;


// Positive test case for a method
int UtcDaliUtf8SequenceLength(void)
{
  tet_infoline("UtcDaliUtf8SequenceLength ");
  TestApplication application;

  std::string latinText( "amazing" );
  std::string japaneseText( "すごい" );
  std::string accent("é");

  tet_infoline("utf8 1 byte Test  ");
  DALI_TEST_EQUALS( Utf8SequenceLength( latinText[0] ), 1u, TEST_LOCATION  );
  tet_infoline("utf8 3 byte Test  ");
  DALI_TEST_EQUALS( Utf8SequenceLength( japaneseText[0] ), 3u, TEST_LOCATION  );
  tet_infoline("utf8 2 byte Test  ");
  DALI_TEST_EQUALS( Utf8SequenceLength( accent[0] ), 2u, TEST_LOCATION  );
  END_TEST;
}
