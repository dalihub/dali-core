#ifndef DALI_GRAPHICS_ASSERTS_H
#define DALI_GRAPHICS_ASSERTS_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Graphics
{
namespace Utility
{

template< typename T >
void Assert(bool condition, const T& message)
{
  if(!condition)
  {
    throw message;
  }
}

template< typename T >
void AssertEqual(const T& lhs, const T& rhs)
{
  Assert(lhs == rhs, "Values are not equal.");
}

template< typename T >
void AssertLessOrEqual(const T& lhs, const T& rhs)
{
  Assert(lhs <= rhs, "Value must be less or equal.");
}

template< typename T >
void AssertGraterOrEqual(const T& lhs, const T& rhs)
{
  Assert(lhs >= rhs, "Value must be grater or equal.");
}

} // namespace Utility
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_ASSERTS_H
