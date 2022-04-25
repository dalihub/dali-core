/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/hash.h>
#include <stdlib.h>

#include <string>

namespace
{
Dali::Vector<std::uint8_t> ConverterVector(const std::vector<std::uint8_t>& v)
{
  Dali::Vector<std::uint8_t> res;
  for(const auto it : v)
  {
    res.PushBack(it);
  }
  return res;
};
} // namespace

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

  const std::string testString1("highp vec4 glowColor = vec4( uGlowColor.rgb, uGlowColor.a * clampedColor.a );");
  const std::string testString2("lowp vec4 glowColor = vec4( uGlowColor.rgb, uGlowColor.a * clampedColor.a );");

  DALI_TEST_CHECK(Dali::CalculateHash(testString1) != Dali::CalculateHash(testString2));
  DALI_TEST_CHECK(Dali::CalculateHash(testString1, testString2) != Dali::CalculateHash(testString2, testString1));

  END_TEST;
}

int UtcDaliHashNegative(void)
{
  // negative test, check hash value == initial value
  const std::string emptyString;

  DALI_TEST_CHECK(Dali::CalculateHash(emptyString) != 0);
  DALI_TEST_CHECK(Dali::CalculateHash(emptyString, emptyString) != 0);

  END_TEST;
}

int UtcDaliHashBuffer(void)
{
  TestApplication application;

  tet_infoline("UtcDaliHashBuffer");

  const std::vector<std::uint8_t> testBuffer1 = {11, 22, 33, 0, 0};
  const std::vector<std::uint8_t> testBuffer2 = {11, 22, 33};
  const std::vector<std::uint8_t> testBuffer3 = {0, 0, 33, 22, 11};
  const std::vector<std::uint8_t> testBuffer4 = {};

  const std::string testString1 = std::string(testBuffer1.begin(), testBuffer1.end());
  const std::string testString2 = std::string(testBuffer2.begin(), testBuffer2.end());
  const std::string testString3 = std::string(testBuffer3.begin(), testBuffer3.end());
  const std::string testString4 = std::string(testBuffer4.begin(), testBuffer4.end());

  tet_printf("const char * type terminate by \\0 char. we cannot seperate two buffer only by std::string().c_str()\n");
  DALI_TEST_CHECK(Dali::CalculateHash(testString1) == Dali::CalculateHash(testString2));
  DALI_TEST_CHECK(Dali::CalculateHash(testString3) == Dali::CalculateHash(testString4));

  tet_printf("To fix this limitation, we make one more API for hashing std::vector<std::uint8_t>\n");
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer1) != Dali::CalculateHash(testBuffer2));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer1) != Dali::CalculateHash(testBuffer3));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer1) != Dali::CalculateHash(testBuffer4));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer2) != Dali::CalculateHash(testBuffer3));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer2) != Dali::CalculateHash(testBuffer4));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer3) != Dali::CalculateHash(testBuffer4));

  END_TEST;
}

int UtcDaliHashBuffer02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliHashBuffer02");
  tet_infoline("Same string must have same hash value. even though generated by std::string and std::vector<std::uint8_t>.");

  const std::string               testString("highp vec4 glowColor = vec4( uGlowColor.rgb, uGlowColor.a * clampedColor.a );");
  const std::vector<std::uint8_t> testBuffer = std::vector<std::uint8_t>(testString.begin(), testString.end());

  DALI_TEST_CHECK(Dali::CalculateHash(testString) == Dali::CalculateHash(testBuffer));

  END_TEST;
}

int UtcDaliHashBufferNegative(void)
{
  // negative test, check hash value == initial value
  const std::vector<std::uint8_t> emptyBuffer;
  const std::vector<std::uint8_t> notEmptyBuffer1{0};
  const std::vector<std::uint8_t> notEmptyBuffer2{0, 0};

  // Check with zero-only buffers also did not zero.
  DALI_TEST_CHECK(Dali::CalculateHash(emptyBuffer) != 0);
  DALI_TEST_CHECK(Dali::CalculateHash(notEmptyBuffer1) != 0);
  DALI_TEST_CHECK(Dali::CalculateHash(notEmptyBuffer2) != 0);

  // Check with zero-only buffers with difference lengths have difference value each other.
  DALI_TEST_CHECK(Dali::CalculateHash(emptyBuffer) != Dali::CalculateHash(notEmptyBuffer1));
  DALI_TEST_CHECK(Dali::CalculateHash(emptyBuffer) != Dali::CalculateHash(notEmptyBuffer2));
  DALI_TEST_CHECK(Dali::CalculateHash(notEmptyBuffer1) != Dali::CalculateHash(notEmptyBuffer2));

  END_TEST;
}

int UtcDaliHashDaliBuffer(void)
{
  TestApplication application;

  tet_infoline("UtcDaliHashDaliBuffer");

  Dali::Vector<std::uint8_t> testBuffer1 = ConverterVector({11, 22, 33, 0, 0});
  Dali::Vector<std::uint8_t> testBuffer2 = ConverterVector({11, 22, 33});
  Dali::Vector<std::uint8_t> testBuffer3 = ConverterVector({0, 0, 33, 22, 11});
  Dali::Vector<std::uint8_t> testBuffer4 = ConverterVector({});

  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer1) != Dali::CalculateHash(testBuffer2));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer1) != Dali::CalculateHash(testBuffer3));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer1) != Dali::CalculateHash(testBuffer4));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer2) != Dali::CalculateHash(testBuffer3));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer2) != Dali::CalculateHash(testBuffer4));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer3) != Dali::CalculateHash(testBuffer4));

  END_TEST;
}

int UtcDaliHashDaliBuffer02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliHashDaliBuffer02");
  tet_infoline("Same string must have same hash value. even though generated by std::string and std::vector<std::uint8_t> and Dali::Vector<std::uint8_t>.");

  const std::string                testString("highp vec4 glowColor = vec4( uGlowColor.rgb, uGlowColor.a * clampedColor.a );");
  const std::vector<std::uint8_t>  testBuffer  = std::vector<std::uint8_t>(testString.begin(), testString.end());
  const Dali::Vector<std::uint8_t> testBuffer2 = ConverterVector(testBuffer);

  DALI_TEST_CHECK(Dali::CalculateHash(testString) == Dali::CalculateHash(testBuffer2));
  DALI_TEST_CHECK(Dali::CalculateHash(testBuffer) == Dali::CalculateHash(testBuffer2));

  END_TEST;
}

int UtcDaliHashDaliBufferNegative(void)
{
  // negative test, check hash value == initial value
  const Dali::Vector<std::uint8_t> emptyBuffer;
  const Dali::Vector<std::uint8_t> notEmptyBuffer1 = ConverterVector({0});
  const Dali::Vector<std::uint8_t> notEmptyBuffer2 = ConverterVector({0, 0});

  // Check with zero-only buffers also did not zero.
  DALI_TEST_CHECK(Dali::CalculateHash(emptyBuffer) != 0);
  DALI_TEST_CHECK(Dali::CalculateHash(notEmptyBuffer1) != 0);
  DALI_TEST_CHECK(Dali::CalculateHash(notEmptyBuffer2) != 0);

  // Check with zero-only buffers with difference lengths have difference value each other.
  DALI_TEST_CHECK(Dali::CalculateHash(emptyBuffer) != Dali::CalculateHash(notEmptyBuffer1));
  DALI_TEST_CHECK(Dali::CalculateHash(emptyBuffer) != Dali::CalculateHash(notEmptyBuffer2));
  DALI_TEST_CHECK(Dali::CalculateHash(notEmptyBuffer1) != Dali::CalculateHash(notEmptyBuffer2));

  END_TEST;
}