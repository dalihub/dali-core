/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali/internal/update/common/property-condition-step-functions.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-input.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
const int32_t ARGINDEX_REF_VALUE    = 0;
const int32_t ARGINDEX_STEP_SIZE    = 1;
const int32_t ARGINDEX_CURRENT_STEP = 2;
const int32_t ARGINDEX_FIRST_VALUE  = 3;
const int32_t ARGINDEX_SECOND_VALUE = 4;
const int32_t ARGINDEX_THIRD_VALUE  = 5;

inline float AngleDifference(float a1, float a2, const float angleRangeHalf)
{
  float diff = fabs(a1 - a2);
  return diff < angleRangeHalf ? diff : angleRangeHalf * 2.0f - diff;
}

} // namespace

ConditionFunction Step::GetFunction(Property::Type valueType)
{
  ConditionFunction function = nullptr;

  switch(valueType)
  {
    case Property::INTEGER:
    {
      function = EvalInteger;
      break;
    }
    case Property::FLOAT:
    {
      function = EvalFloat;
      break;
    }
    case Property::VECTOR2:
    {
      function = EvalVector2;
      break;
    }
    case Property::VECTOR3:
    {
      function = EvalVector3;
      break;
    }
    case Property::VECTOR4:
    {
      function = EvalVector4;
      break;
    }
    default:
    {
      function = EvalDefault;
      break;
    }
  } // end switch

  return function;
}

ConditionFunction Step::GetCompareFunction(Property::Type valueType)
{
  ConditionFunction function = nullptr;
  if(valueType == Property::VECTOR3)
  {
    function = EvalAndCompareVector3;
  }
  else if(valueType == Property::ROTATION)
  {
    function = EvalAndCompareQuaternion;
  }
  else
  {
    function = GetFunction(valueType);
  }

  return function;
}

bool Step::Evaluate(const float propertyValue, PropertyNotification::RawArgumentContainer& arg)
{
  const float   refValue    = arg[ARGINDEX_REF_VALUE];
  const float   step        = arg[ARGINDEX_STEP_SIZE];
  const int32_t currentStep = static_cast<int32_t>(arg[ARGINDEX_CURRENT_STEP]);
  const float   distance    = (propertyValue - refValue);
  // step is actual 1.0f / step so can multiply instead of dividing
  const int32_t newStep = static_cast<int32_t>(floorf(distance * step));

  if(newStep != currentStep)
  {
    // in new section
    arg[ARGINDEX_CURRENT_STEP] = static_cast<float>(newStep);
    return true;
  }
  return false;
}

bool Step::EvalInteger(const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg)
{
  const float propertyValue = static_cast<float>(value.GetInteger());
  return Evaluate(propertyValue, arg);
}

bool Step::EvalFloat(const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg)
{
  const float propertyValue = value.GetFloat();
  return Evaluate(propertyValue, arg);
}

bool Step::EvalVector2(const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg)
{
  const float propertyValue = value.GetVector2().Length();
  return Evaluate(propertyValue, arg);
}

bool Step::EvalVector3(const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg)
{
  float propertyValue = value.GetVector3().Length();
  return Evaluate(propertyValue, arg);
}

bool Step::EvalAndCompareVector3(const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg)
{
  float propertyValue = value.GetVector3().Length();
  bool  result        = Evaluate(propertyValue, arg);
  if(result == false)
  {
    const float step = 1.0f / arg[ARGINDEX_STEP_SIZE];
    if((fabsf(arg[ARGINDEX_FIRST_VALUE] - value.GetVector3().x) > step) || (fabsf(arg[ARGINDEX_SECOND_VALUE] - value.GetVector3().y) > step) || (fabsf(arg[ARGINDEX_THIRD_VALUE] - value.GetVector3().z) > step))
    {
      result = true;
    }
  }
  arg[ARGINDEX_FIRST_VALUE]  = value.GetVector3().x;
  arg[ARGINDEX_SECOND_VALUE] = value.GetVector3().y;
  arg[ARGINDEX_THIRD_VALUE]  = value.GetVector3().z;
  return result;
}

bool Step::EvalVector4(const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg)
{
  const float propertyValue = value.GetVector4().Length();
  return Evaluate(propertyValue, arg);
}

bool Step::EvalAndCompareQuaternion(const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg)
{
  // TODO : Make some meaningfule calculation here

  Quaternion propertyValue = value.GetQuaternion();

  Vector4     v          = propertyValue.EulerAngles();
  const float checkValue = v.LengthSquared3();
  bool        result     = Evaluate(checkValue, arg);

  if(result == false)
  {
    const float step = 1.0f / arg[ARGINDEX_STEP_SIZE];
    if((AngleDifference(arg[ARGINDEX_FIRST_VALUE], v.x, Dali::Math::PI) > step) ||
       (AngleDifference(arg[ARGINDEX_SECOND_VALUE], v.y, Dali::Math::PI_2) > step) ||
       (AngleDifference(arg[ARGINDEX_THIRD_VALUE], v.z, Dali::Math::PI) > step))
    {
      result = true;
    }
  }
  arg[ARGINDEX_FIRST_VALUE]  = v.x;
  arg[ARGINDEX_SECOND_VALUE] = v.y;
  arg[ARGINDEX_THIRD_VALUE]  = v.z;
  return result;
}

bool Step::EvalDefault(const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg)
{
  return false;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
