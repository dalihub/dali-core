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
#include <dali/internal/update/common/property-condition-variable-step-functions.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace
{

const int ARGINDEX_STEP_INDEX = 0;
const int ARGINDEX_LIST_SIZE = 1;
const int ARGINDEX_LIST_START = 2;

}

ConditionFunction VariableStep::GetFunction( Property::Type valueType )
{
  ConditionFunction function = NULL;

  switch( valueType )
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

bool VariableStep::Evaluate( const float propertyValue, PropertyNotification::RawArgumentContainer& arg )
{
  const int currentIndex = arg[ARGINDEX_STEP_INDEX];
  const int numSteps = arg[ARGINDEX_LIST_SIZE];
  const float first = arg[ARGINDEX_LIST_START];
  const float last = arg[ARGINDEX_LIST_START + (numSteps - 1)];
  const bool ascending = (last > first) ? true : false;
  int newIndex = currentIndex;

  // avoid loop if property currently not within any of the range values
  if( ascending )
  {
    if( propertyValue < first )
    {
      newIndex = -1;
    }
    else if( propertyValue >= last )
    {
      newIndex = numSteps - 1;
    }
  }
  else
  {
    // increments are in negative direction
    if( propertyValue > first )
    {
      newIndex = -1;
    }
    else if( propertyValue <= last )
    {
      newIndex = numSteps - 1;
    }
  }
  int i = 0;
  for( i = 0 ; i < numSteps - 1 ; ++i )
  {
    const float arg1 = arg[ARGINDEX_LIST_START + i];
    const float arg2 = arg[ARGINDEX_LIST_START + (i + 1)];
    if( ascending )
    {
      if( ( propertyValue >= arg1 )
          && ( propertyValue < arg2 ) )
      {
        newIndex = i;
        break;
      }
    }
    else
    {
      // increments are in negative direction
      if( ( propertyValue > arg2 )
          && ( propertyValue <= arg1 ) )
      {
        newIndex = i;
        break;
      }
    }
  }
  if( newIndex != currentIndex )
  {
    // have changed to new step
    arg[ARGINDEX_STEP_INDEX] = static_cast<float>(newIndex);
    return true;
  }
  return false;
}

bool VariableStep::EvalInteger( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float propertyValue = static_cast<float>( value.GetInteger() );
  return Evaluate( propertyValue, arg );
}

bool VariableStep::EvalUnsignedInteger( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float propertyValue = static_cast<float>( value.GetUnsignedInteger() );
  return Evaluate( propertyValue, arg );
}

bool VariableStep::EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float propertyValue = value.GetFloat();
  return Evaluate( propertyValue, arg );
}

bool VariableStep::EvalVector2( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float propertyValue = value.GetVector2().LengthSquared();
  return Evaluate( propertyValue, arg );
}

bool VariableStep::EvalVector3( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  float propertyValue = value.GetVector3().LengthSquared();
  return Evaluate( propertyValue, arg );
}

bool VariableStep::EvalVector4( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float propertyValue = value.GetVector4().LengthSquared();
  return Evaluate( propertyValue, arg );
}

bool VariableStep::EvalDefault( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  return false;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
