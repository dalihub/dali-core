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

// CLASS HEADER
#include <dali/public-api/animation/alpha-function.h>

// INTERNAL INCLUDES

namespace Dali
{

AlphaFunction::AlphaFunction()
:mBezierControlPoints(Vector4::ZERO),
 mCustom(0),
 mBuiltin(DEFAULT),
 mMode(BUILTIN_FUNCTION)
{}

AlphaFunction::AlphaFunction( BuiltinFunction function)
:mBezierControlPoints(Vector4::ZERO),
 mCustom(0),
 mBuiltin(function),
 mMode(BUILTIN_FUNCTION)
{}

AlphaFunction::AlphaFunction( AlphaFunctionPrototype function)
:mBezierControlPoints(Vector4::ZERO),
 mCustom(function),
 mBuiltin(DEFAULT),
 mMode(CUSTOM_FUNCTION)
{}

AlphaFunction::AlphaFunction( const Vector2& controlPoint0, const Vector2& controlPoint1 )
:mBezierControlPoints(Vector4(Clamp(controlPoint0.x,0.0f,1.0f),controlPoint0.y,
                              Clamp(controlPoint1.x,0.0f,1.0f),controlPoint1.y)),
 mCustom(0),
 mBuiltin(DEFAULT),
 mMode(BEZIER)
{
}

Vector4 AlphaFunction::GetBezierControlPoints() const
{
  return mBezierControlPoints;
}

AlphaFunctionPrototype AlphaFunction::GetCustomFunction() const
{
  return mCustom;
}

AlphaFunction::BuiltinFunction AlphaFunction::GetBuiltinFunction() const
{
  return mBuiltin;
}

AlphaFunction::Mode AlphaFunction::GetMode() const
{
  return mMode;
}

} // namespace Dali
