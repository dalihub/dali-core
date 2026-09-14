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
#include <dali/public-api/common/insets.h>

// EXTERNAL INCLUDES
#include <math.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h>

namespace DALI_NAMESPACE
{
Insets::Insets()
: start(0.0f),
  end(0.0f),
  top(0.0f),
  bottom(0.0f)
{
}

Insets::Insets(float start, float end, float top, float bottom)
: start(start),
  end(end),
  top(top),
  bottom(bottom)
{
}

Insets::Insets(float horizontal, float vertical)
: start(horizontal),
  end(horizontal),
  top(vertical),
  bottom(vertical)
{
}

Insets& Insets::operator=(const float* array)
{
  start  = array[0];
  end    = array[1];
  top    = array[2];
  bottom = array[3];

  return *this;
}

bool Insets::operator==(const Insets& rhs) const
{
  return (fabsf(start - rhs.start) <= GetRangedEpsilon(start, rhs.start)) &&
         (fabsf(end - rhs.end) <= GetRangedEpsilon(end, rhs.end)) &&
         (fabsf(top - rhs.top) <= GetRangedEpsilon(top, rhs.top)) &&
         (fabsf(bottom - rhs.bottom) <= GetRangedEpsilon(bottom, rhs.bottom));
}

bool Insets::operator!=(const Insets& rhs) const
{
  return !(*this == rhs);
}

} //namespace DALI_NAMESPACE
