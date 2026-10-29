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

// EXTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <limits>

// INTERNAL INCLUDES
#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-pair.h>
#include <dali/public-api/common/insets.h>
#include <dali/public-api/math/angle-axis.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/type-info-id.h>

using namespace Dali;

namespace
{
constexpr bool CheckConstruction(float value = 1.f)
{
  const float values[]{value, 2.f, 3.f, 4.f};
  Vector2     zero2;
  Vector3     zero3;
  Vector4     zero4;
  Vector2     vector2(values);
  Vector3     vector3(values);
  Vector4     vector4(values);
  AngleAxis   zero;
  AngleAxis   rotation(Radian(1.f), vector3);
  Rect<int>   empty;
  Rect<int>   rectangle(Vector4(2.f, 3.f, 4.f, 5.f));
  return zero2.x == 0.f && zero3.y == 0.f && zero4.w == 0.f &&
         vector2.y == 2.f && vector3.z == 3.f && vector4.w == 4.f &&
         zero.angle.radian == 0.f && zero.axis.z == 0.f &&
         rotation.angle.radian == 1.f && rotation.axis.x == 1.f &&
         empty.IsEmpty() && rectangle.x == 2 && rectangle.height == 5;
}

constexpr bool CheckAngles()
{
  Degree degree;
  Radian angle;
  angle = 2.f;
  if(static_cast<float>(angle) != 2.f)
  {
    return false;
  }
  angle = Degree(180.f);
  return degree.degree == 0.f && angle.radian == Radian(Degree(180.f)).radian &&
         Radian(2.f) > Radian(1.f) && Radian(1.f) < Radian(2.f) &&
         Radian(4.f) > Degree(180.f) && Degree(180.f) > Radian(1.f) &&
         Radian(1.f) < Degree(180.f) && Degree(180.f) < Radian(4.f) &&
         (Radian(2.f) * 3.f).radian == 6.f && (-Radian(2.f)).radian == -2.f &&
         Clamp(Radian(3.f), 0.f, 2.f).radian == 2.f &&
         Clamp(Degree(3.f), 0.f, 2.f).degree == 2.f;
}

constexpr bool CheckRectangles()
{
  Rect<int> rectangle;
  rectangle.Set(1, 2, 3, 4);
  if(!rectangle.IsValid() || rectangle.IsEmpty() || rectangle.Left() != 1 ||
     rectangle.Right() != 4 || rectangle.Top() != 2 || rectangle.Bottom() != 6 || rectangle.Area() != 12)
  {
    return false;
  }
  const Rect<int> contained(2, 3, 1, 1);
  if(!rectangle.Contains(contained) || !rectangle.Intersects(contained) || !rectangle.Intersect(contained) || rectangle != contained)
  {
    return false;
  }
  if(rectangle.Intersect(Rect<int>(10, 10, 1, 1)))
  {
    return false;
  }
  rectangle.Merge(Rect<int>(1, 2, 3, 4));
  rectangle.Inset(1, 2);
  return rectangle == Rect<int>(0, 0, 5, 8) && !Rect<int>(0, 0, -1, 1).IsValid() &&
         Rect<float>().IsEmpty() && !Rect<float>(0.f, 0.f, 1.f, 1.f).IsEmpty() &&
         Rect<float>(0.f, 0.f, 1.f, 0.f).IsEmpty();
}

constexpr bool CheckAdditionalValues(float epsilonValue = 100.f)
{
  TimePeriod   duration(2.f);
  TimePeriod   delayed(1.f, 2.f);
  LocalSource  local(3);
  ParentSource parent(4);
  Insets       empty;
  Insets       symmetric(2.f, 3.f);
  Insets       edges(1.f, 2.f, 3.f, 4.f);
  const float  values[]{1.f, 2.f, 3.f, 4.f};
  empty = values;
  RelativeToConstraint      relative(2.f);
  RelativeToConstraint      components(Vector3(1.f, 2.f, 3.f));
  RelativeToConstraintFloat scalar(4.f);
  const TypeInfoId          type(1u, "value");
  if(GetRangedEpsilon(epsilonValue, 1.f) != Math::MACHINE_EPSILON_100 ||
     GetRangedEpsilon(epsilonValue * 10.f, 1.f) != Math::MACHINE_EPSILON_1000 ||
     GetRangedEpsilon(epsilonValue * 100.f, 1.f) != Math::MACHINE_EPSILON_10000 ||
     GetRangedEpsilon(std::numeric_limits<float>::quiet_NaN(), epsilonValue) != Math::MACHINE_EPSILON_10000)
  {
    return false;
  }
  return duration.delaySeconds == 0.f && delayed.delaySeconds == 1.f && duration.durationSeconds == 2.f &&
         local.propertyIndex == 3 && parent.propertyIndex == 4 && empty == edges && symmetric != edges &&
         relative.mScale.x == 2.f && components.mScale.z == 3.f && scalar.mScale == 4.f &&
         type.IsValid() && type.GetId() == 1u && type.GetName()[0] == 'v' &&
         !TypeInfoId().IsValid() && TypeInfoId().GetName()[0] == 'v' &&
         Degree(Radian(Math::PI)).degree == 180.f && Degree(1.f) == Degree(1.f) && Degree(1.f) != Degree(2.f) &&
         Radian(1.f) == Radian(1.f) && Radian(1.f) != Radian(2.f) &&
         Radian(0.f) == Degree(0.f) && Degree(0.f) == Radian(0.f) &&
         Radian(0.f) != Degree(1.f) && Degree(1.f) != Radian(0.f) &&
         AngleAxis() == AngleAxis() && Rect<float>() == Rect<float>();
}

constexpr bool CheckMatrices()
{
  Matrix      zero;
  const float values[]{0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f};
  Matrix      matrix(values);
  Matrix      copy(matrix);
  Matrix      moved(static_cast<Matrix&&>(copy));
  copy  = matrix;
  moved = static_cast<Matrix&&>(copy);
  matrix.Transpose();
  if(matrix.AsFloat()[1] != 4.f || static_cast<const Matrix&>(matrix).AsFloat()[4] != 1.f || zero.AsFloat()[0] != 0.f)
  {
    return false;
  }
  matrix.SetIdentity();
  matrix.SetXAxis(Vector3(2.f, 3.f, 4.f));
  matrix.SetYAxis(Vector3(5.f, 6.f, 7.f));
  matrix.SetZAxis(Vector3(8.f, 9.f, 10.f));
  if(matrix.GetXAxis().y != 3.f || matrix.GetYAxis().z != 7.f || matrix.GetZAxis().x != 8.f)
  {
    return false;
  }
  matrix.SetIdentityAndScale(Vector3(2.f, 3.f, 4.f));
  matrix.SetTranslation(Vector4(5.f, 6.f, 7.f, 8.f));
  if(matrix.AsFloat()[15] != 8.f)
  {
    return false;
  }
  matrix.SetTranslation(Vector3(9.f, 10.f, 11.f));
  if(matrix.AsFloat()[12] != 9.f || matrix.AsFloat()[15] != 1.f || matrix != matrix || !(matrix == matrix) || matrix == moved || !(matrix != moved))
  {
    return false;
  }
  Matrix3 extracted(matrix);
  Matrix3 assigned;
  assigned = matrix;
  if(extracted != assigned || !(extracted == assigned))
  {
    return false;
  }
  assigned.Scale(-2.f);
  if(assigned.Magnitude() != 6.f || (extracted * Vector3(1.f, 2.f, 3.f)).z != 12.f || assigned == extracted || !(assigned != extracted))
  {
    return false;
  }
  Matrix3 small;
  Matrix3 elements(0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f);
  Matrix3 smallCopy(elements);
  Matrix3 smallMoved(static_cast<Matrix3&&>(smallCopy));
  smallCopy  = elements;
  smallMoved = static_cast<Matrix3&&>(smallCopy);
  if(!elements.Transpose() || elements.AsFloat()[1] != 3.f || static_cast<const Matrix3&>(elements).AsFloat()[3] != 1.f || small.AsFloat()[0] != 0.f)
  {
    return false;
  }
  small.SetIdentity();
  Quaternion identity;
  Quaternion rotation(0.f, 1.f, 2.f, 3.f);
  return small.AsFloat()[4] == 1.f && identity.IsIdentity() && !rotation.IsIdentity() && rotation.AsVector().z == 3.f;
}

constexpr bool CheckIntegerPairs()
{
  Int32Pair pair(1u, 2u);
  pair.SetWidth(-3);
  pair.SetHeight(-4);
  if(pair.GetWidth() != -3 || pair.GetHeight() != -4)
  {
    return false;
  }
  pair.SetWidth(std::numeric_limits<int32_t>::min());
  pair.SetHeight(std::numeric_limits<int32_t>::max());
  if(pair.GetWidth() != std::numeric_limits<int32_t>::min() || pair.GetHeight() != std::numeric_limits<int32_t>::max())
  {
    return false;
  }
  pair.SetX(5);
  pair.SetY(6);
  pair.SetStart(-7);
  pair.SetEnd(-8);
  Int32Pair  other;
  Uint16Pair size(2u, 3u);
  size.SetWidth(4u);
  size.SetHeight(5u);
  return pair.GetX() == -7 && pair.GetY() == -8 && pair.GetStart() == -7 && pair.GetEnd() == -8 &&
         size.GetWidth() == 4u && size.GetHeight() == 5u && pair == pair && pair != other &&
         other < pair && pair > other;
}

constexpr bool CheckPairs()
{
  Pair<int, int>   zero;
  const int        one = 1;
  const int        two = 2;
  Pair<int, int>   pair(one, two);
  Pair<long, long> converted(pair);
  Pair<long, long> moved(Pair<int, int>(1, 2));
  converted = pair;
  moved     = Pair<int, int>(1, 2);
  pair.Swap(zero);
  Swap(pair, zero);
  const auto made = MakePair(1, 2);
  return pair == made && pair != zero && zero < pair && pair > zero && pair >= made && zero <= pair &&
         converted.first == 1 && moved.second == 2 && Forward<int>(3) == 3;
}

template<typename VectorType>
constexpr bool CheckVectorArithmetic()
{
  VectorType value;
  VectorType one;
  for(unsigned int index = 0; index < sizeof(VectorType) / sizeof(float); ++index)
  {
    one[index] = 1.f;
  }
  value += one;
  value -= one;
  value = (value + one) - one;
  value = value * one;
  value *= one;
  value *= 2.f;
  value = value * 2.f;
  value /= one;
  value /= 2.f;
  value = value / one;
  value = -(value / 2.f);
  const VectorType copy(value);
  if(!(value == copy) || value != copy)
  {
    return false;
  }
  for(unsigned int index = 0; index < sizeof(VectorType) / sizeof(float); ++index)
  {
    VectorType changed(copy);
    changed[index] = 1.f;
    if(changed == copy || static_cast<const VectorType&>(changed)[index] != 1.f)
    {
      return false;
    }
  }
  const float values[]{2.f, 3.f, 4.f, 5.f};
  value = values;
  return value[0] == 2.f && Min(value, one)[0] == 1.f && Max(value, one)[0] == 2.f;
}

static_assert(CheckAdditionalValues());
static_assert(CheckMatrices());
static_assert(CheckPairs());
static_assert(CheckVectorArithmetic<Vector2>());
static_assert(CheckVectorArithmetic<Vector3>());
static_assert(CheckVectorArithmetic<Vector4>());

static_assert(CheckConstruction(), "Value constructors must support C++17 constant evaluation");
static_assert(CheckAngles(), "Angle operations must support C++17 constant evaluation");
static_assert(CheckRectangles(), "Rectangle operations must support C++17 constant evaluation");
} // unnamed namespace

int UtcDaliConstexprValueTypes(void)
{
  volatile float value = 1.f;
  DALI_TEST_CHECK(CheckConstruction(value));
  DALI_TEST_CHECK(CheckAngles());
  DALI_TEST_CHECK(CheckRectangles());
  END_TEST;
}

int UtcDaliConstexprAdditionalValueTypes(void)
{
  volatile float epsilonValue = 100.f;
  DALI_TEST_CHECK(CheckAdditionalValues(epsilonValue));
  DALI_TEST_CHECK(CheckMatrices());
  DALI_TEST_CHECK(CheckIntegerPairs());
  DALI_TEST_CHECK(CheckPairs());
  DALI_TEST_CHECK(CheckVectorArithmetic<Vector2>());
  DALI_TEST_CHECK(CheckVectorArithmetic<Vector3>());
  DALI_TEST_CHECK(CheckVectorArithmetic<Vector4>());
  END_TEST;
}
