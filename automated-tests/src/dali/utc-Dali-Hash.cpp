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

#include <string>
#include <stdlib.h>
#include <dali/public-api/common/hash.h>
#include <dali-test-suite-utils.h>

void utc_dali_hash_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_hash_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliHash(void)
{
  // To fully test the Hash distribution we need to use a tool like http://code.google.com/p/smhasher/
  // DALi currently uses the hash for variable length strings which come from:
  // shader vert+frag source, font family + style, image filename.
  TestApplication application;

  tet_infoline("UtcDaliHash");

  const std::string testString1( "highp vec4 glowColor = vec4( uGlowColor.rgb, uGlowColor.a * clampedColor.a );");
  const std::string testString2( "lowp vec4 glowColor = vec4( uGlowColor.rgb, uGlowColor.a * clampedColor.a );");

  DALI_TEST_CHECK( Dali::CalculateHash( testString1 ) != Dali::CalculateHash( testString2 ) );
  DALI_TEST_CHECK( Dali::CalculateHash( testString1, testString2 ) != Dali::CalculateHash( testString2, testString1 ) );

  END_TEST;
}

int UtcDaliHashNegative(void)
{
  // negative test, check hash value == initial value
  const std::string emptyString;

  DALI_TEST_CHECK( Dali::CalculateHash( emptyString ) != 0 );
  DALI_TEST_CHECK( Dali::CalculateHash( emptyString, emptyString ) != 0 );

  END_TEST;
}
