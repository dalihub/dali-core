#ifndef DALI_SCALAR_MATH_H
#define DALI_SCALAR_MATH_H

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

// INTERNAL INCLUDES
#include <dali/public-api/math/compile-time-math.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
namespace MathUtils
{
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
constexpr bool IsNaN(float x)
{
  return x != x;
}

constexpr bool EqualsZero(float value)
{
  return value == 0.0f;
}
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

constexpr float Abs(float x)
{
  // Avoid an ordered comparison with NaN: it can raise FE_INVALID and is
  // rejected during constant evaluation by older compilers.
  if(IsNaN(x))
  {
    return x;
  }

  // Make -0.0f return 0.0f here.
  if(EqualsZero(x))
  {
    return 0.0f;
  }

  return x < 0.0f ? -x : x;
}

} // namespace MathUtils
} // namespace Internal

/**
 * @brief Get an epsilon that is valid for the given range.
 *
 * @SINCE_1_0.0
 * @param[in] a the first value in the range
 * @param[in] b the second value in the range.
 * @return a suitable epsilon
 */
constexpr float GetRangedEpsilon(float a, float b)
{
  const float absA = Internal::MathUtils::Abs(a);
  const float absB = Internal::MathUtils::Abs(b);

  if(Internal::MathUtils::IsNaN(absA) || Internal::MathUtils::IsNaN(absB))
  {
    return Epsilon<10000>::value;
  }

  const float absF = absA > absB ? absA : absB;

  float epsilon = Epsilon<10000>::value;
  if(absF < 0.1f)
  {
    return Epsilon<0>::value;
  }
  else if(absF < 2.0f)
  {
    return Epsilon<1>::value;
  }
  else if(absF < 20.0f)
  {
    return Epsilon<10>::value;
  }
  else if(absF < 200.0f)
  {
    return Epsilon<100>::value;
  }
  else if(absF < 2000.0f)
  {
    return Epsilon<1000>::value;
  }
  return epsilon;
}

} // namespace DALI_NAMESPACE

#endif // DALI_SCALAR_MATH_H
