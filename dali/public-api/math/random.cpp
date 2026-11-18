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

// CLASS HEADER
#include <dali/public-api/math/random.h>

// EXTERNAL INCLUDES
#include <cstdlib> // for srand() and rand()
#include <ctime>   // for time(nullptr)

namespace DALI_NAMESPACE
{
namespace Random
{
float Range(float f0, float f1)
{
  float minf = f0 < f1 ? f0 : f1;
  float maxf = f0 > f1 ? f0 : f1;

  // Seed the C library generator once per process; the static initializer runs exactly once
  static const bool seeded = []()
  {
    srand(static_cast<uint32_t>(time(nullptr)));
    return true;
  }();
  (void)seeded;

  int32_t randValue = rand();
  return static_cast<float>(randValue & 0xfff) * (1.0f / 4095.0f) * (maxf - minf) + minf;
}

Vector4 Axis()
{
  Vector4 axis;
  // This function needs to return a vector with direction, that is to say a non-zero vector
  // There is a possibility that a random vector will be a zero vector, so a loop is needed to ensure that a non-zero vector is returned
  do
  {
    axis.x = Range(-1.0f, 1.0f);
    axis.y = Range(0.0f, 1.0f);
    axis.z = Range(0.0f, 1.0f);
    axis.w = 0.0f;
  } while(axis == Vector4::ZERO);
  axis.Normalize3();
  return axis;
}

} // namespace Random
} // namespace DALI_NAMESPACE
