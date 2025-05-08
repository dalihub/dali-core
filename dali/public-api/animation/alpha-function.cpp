/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/alpha-function.h>

// INTERNAL INCLUDES

namespace
{

Dali::SpringData GetSpringDefaultData(Dali::AlphaFunction::SpringType springType)
{
  switch(springType)
  {
    case Dali::AlphaFunction::SpringType::GENTLE:
    {
      return {100.0f, 15.0f, 1.0f};
    }
    case Dali::AlphaFunction::SpringType::QUICK:
    {
      return {300.0f, 20.0f, 1.0f};
    }
    case Dali::AlphaFunction::SpringType::BOUNCY:
    {
      return {600.0f, 15.0f, 1.0f};
    }
    case Dali::AlphaFunction::SpringType::SLOW:
    {
      return {94.0f, 18.5f, 1.0f};
    }
    default:
    {
      return {100.0f, 15.0f, 1.0f};
    }
  }
}
}

namespace Dali
{
AlphaFunction::AlphaFunction()
: mMode(BUILTIN_FUNCTION),
  mBuiltin(DEFAULT),
  mSpringData({100.0f, 15.0f, 1.0f})
{
}

AlphaFunction::AlphaFunction(BuiltinFunction function)
: mMode(BUILTIN_FUNCTION),
  mBuiltin(function),
  mSpringData({100.0f, 15.0f, 1.0f})
{
}

AlphaFunction::AlphaFunction(AlphaFunctionPrototype function)
: mMode(CUSTOM_FUNCTION),
  mBuiltin(DEFAULT),
  mCustom(function)
{
}

AlphaFunction::AlphaFunction(const Vector2& controlPoint0, const Vector2& controlPoint1)
: mMode(BEZIER),
  mBuiltin(DEFAULT),
  mBezierControlPoints(
    Vector4(Clamp(controlPoint0.x, 0.0f, 1.0f),
            controlPoint0.y,
            Clamp(controlPoint1.x, 0.0f, 1.0f),
            controlPoint1.y))
{
}

AlphaFunction::AlphaFunction(SpringType springType)
: mMode(SPRING),
  mBuiltin(DEFAULT),
  mSpringData(GetSpringDefaultData(springType))
{
}

AlphaFunction::AlphaFunction(Dali::SpringData springData)
: mMode(CUSTOM_SPRING),
  mBuiltin(DEFAULT),
  mSpringData(springData)
{
}

Vector4 AlphaFunction::GetBezierControlPoints() const
{
  return (mMode == BEZIER) ? mBezierControlPoints : Vector4::ZERO;
}

AlphaFunctionPrototype AlphaFunction::GetCustomFunction() const
{
  return (mMode == CUSTOM_FUNCTION) ? mCustom : nullptr;
}

AlphaFunction::BuiltinFunction AlphaFunction::GetBuiltinFunction() const
{
  return mBuiltin;
}

AlphaFunction::Mode AlphaFunction::GetMode() const
{
  return mMode;
}

const Dali::SpringData& AlphaFunction::GetSpringData() const
{
  return mSpringData;
}

} // namespace Dali
