/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-input.h>
#include <dali/internal/update/common/property-condition-step-functions.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace
{

const int ARGINDEX_REF_VALUE = 0;
const int ARGINDEX_STEP_SIZE = 1;
const int ARGINDEX_CURRENT_STEP = 2;

} // namespace

ConditionFunction Step::GetFunction(Property::Type valueType)
{
  ConditionFunction function = NULL;

  switch(valueType)
  {
    case Property::INTEGER:
    {
      function = EvalInteger;
      break;
    }
    case Property::UNSIGNED_INTEGER:
    {
      function = EvalUnsignedInteger;
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

bool Step::Evaluate( const float propertyValue, PropertyNotification::RawArgumentContainer& arg )
{
  const float refValue = arg[ARGINDEX_REF_VALUE];
  const float step = arg[ARGINDEX_STEP_SIZE];
  const int currentStep = static_cast<int>(arg[ARGINDEX_CURRENT_STEP]);
  const float distance = (propertyValue - refValue);
  // step is actual 1.0f / step so can multiply instead of dividing
  const int newStep = static_cast<int>(floorf(distance * step));

  if( newStep != currentStep )
  {
    // in new section
    arg[ARGINDEX_CURRENT_STEP] = static_cast<float>(newStep);
    return true;
  }
  return false;
}


bool Step::EvalInteger( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float propertyValue = static_cast<float>( value.GetInteger() );
  return Evaluate( propertyValue, arg );
}

bool Step::EvalUnsignedInteger( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float propertyValue = static_cast<float>( value.GetUnsignedInteger() );
  return Evaluate( propertyValue, arg );
}

bool Step::EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float propertyValue = value.GetFloat();
  return Evaluate( propertyValue, arg );
}
bool Step::EvalVector2( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float propertyValue = value.GetVector2().LengthSquared();
  return Evaluate( propertyValue, arg );
}

bool Step::EvalVector3( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  float propertyValue = value.GetVector3().LengthSquared();
  return Evaluate( propertyValue, arg );
}

bool Step::EvalVector4( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float propertyValue = value.GetVector4().LengthSquared();
  return Evaluate( propertyValue, arg );
}

bool Step::EvalDefault( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  return false;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
