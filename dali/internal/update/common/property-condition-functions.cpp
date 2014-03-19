//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-input.h>
#include <dali/internal/update/common/property-condition-functions.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

// LessThan ///////////////////////////////////////////////////////////////////

ConditionFunction LessThan::GetFunction(Property::Type valueType)
{
  ConditionFunction function = NULL;

  switch(valueType)
  {
    case Property::BOOLEAN:
    {
      function = LessThan::EvalBoolean;
      break;
    }
    case Property::FLOAT:
    {
      function = LessThan::EvalFloat;
      break;
    }
    case Property::VECTOR2:
    {
      function = LessThan::EvalVector2;
      break;
    }
    case Property::VECTOR3:
    {
      function = LessThan::EvalVector3;
      break;
    }
    case Property::VECTOR4:
    {
      function = LessThan::EvalVector4;
      break;
    }
    default:
    {
      function = LessThan::EvalDefault;
      break;
    }
  } // end switch

  return function;
}

bool LessThan::EvalBoolean( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float arg0 = arg[0];
  return (value.GetBoolean() < arg0);
}

bool LessThan::EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float arg0 = arg[0];
  return (value.GetFloat() < arg0);
}

bool LessThan::EvalVector2( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float arg0 = arg[0];
  return (value.GetVector2().LengthSquared() < arg0 * arg0);
}

bool LessThan::EvalVector3( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float arg0 = arg[0];
  return (value.GetVector3().LengthSquared() < arg0 * arg0);
}

bool LessThan::EvalVector4( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float arg0 = arg[0];
  return (value.GetVector4().LengthSquared() < arg0 * arg0);
}

bool LessThan::EvalDefault( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  return false;
}

// GreaterThan ///////////////////////////////////////////////////////////////////

ConditionFunction GreaterThan::GetFunction(Property::Type valueType)
{
  ConditionFunction function = NULL;

  switch(valueType)
  {
    case Property::BOOLEAN:
    {
      function = GreaterThan::EvalBoolean;
      break;
    }
    case Property::FLOAT:
    {
      function = GreaterThan::EvalFloat;
      break;
    }
    case Property::VECTOR2:
    {
      function = GreaterThan::EvalVector2;
      break;
    }
    case Property::VECTOR3:
    {
      function = GreaterThan::EvalVector3;
      break;
    }
    case Property::VECTOR4:
    {
      function = GreaterThan::EvalVector4;
      break;
    }
    default:
    {
      function = GreaterThan::EvalDefault;
      break;
    }
  } // end switch

  return function;
}

bool GreaterThan::EvalBoolean( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float arg0 = arg[0];
  return (value.GetBoolean() > arg0);
}

bool GreaterThan::EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float arg0 = arg[0];
  return (value.GetFloat() > arg0);
}

bool GreaterThan::EvalVector2( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float arg0 = arg[0];
  return (value.GetVector2().LengthSquared() > arg0 * arg0);
}

bool GreaterThan::EvalVector3( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float arg0 = arg[0];
  return (value.GetVector3().LengthSquared() > arg0 * arg0);
}

bool GreaterThan::EvalVector4( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float arg0 = arg[0];
  return (value.GetVector4().LengthSquared() > arg0 * arg0);
}

bool GreaterThan::EvalDefault( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  return false;
}

// Inside /////////////////////////////////////////////////////////////////////

ConditionFunction Inside::GetFunction(Property::Type valueType)
{
  ConditionFunction function = NULL;

  switch(valueType)
  {
    case Property::BOOLEAN:
    {
      function = Inside::EvalBoolean;
      break;
    }
    case Property::FLOAT:
    {
      function = Inside::EvalFloat;
      break;
    }
    case Property::VECTOR2:
    {
      function = Inside::EvalVector2;
      break;
    }
    case Property::VECTOR3:
    {
      function = Inside::EvalVector3;
      break;
    }
    case Property::VECTOR4:
    {
      function = Inside::EvalVector4;
      break;
    }
    default:
    {
      function = Inside::EvalDefault;
      break;
    }
  } // end switch

  return function;
}

bool Inside::EvalBoolean( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const bool valueBoolean = value.GetBoolean();
  return ( (valueBoolean > arg[0]) && (valueBoolean < arg[1]) );
}

bool Inside::EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float valueFloat = value.GetFloat();
  return ( (valueFloat > arg[0]) && (valueFloat < arg[1]) );
}

bool Inside::EvalVector2( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float length2 = value.GetVector2().LengthSquared();
  return ( (length2 > arg[0]*arg[0]) && (length2 < arg[1]*arg[1]) );
}

bool Inside::EvalVector3( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float length2 = value.GetVector3().LengthSquared();
  return ( (length2 > arg[0]*arg[0]) && (length2 < arg[1]*arg[1]) );
}

bool Inside::EvalVector4( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float length2 = value.GetVector4().LengthSquared();
  return ( (length2 > arg[0]*arg[0]) && (length2 < arg[1]*arg[1]) );
}

bool Inside::EvalDefault( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  return false;
}

// Outside ////////////////////////////////////////////////////////////////////

ConditionFunction Outside::GetFunction(Property::Type valueType)
{
  ConditionFunction function = NULL;

  switch(valueType)
  {
    case Property::BOOLEAN:
    {
      function = Outside::EvalBoolean;
      break;
    }
    case Property::FLOAT:
    {
      function = Outside::EvalFloat;
      break;
    }
    case Property::VECTOR2:
    {
      function = Outside::EvalVector2;
      break;
    }
    case Property::VECTOR3:
    {
      function = Outside::EvalVector3;
      break;
    }
    case Property::VECTOR4:
    {
      function = Outside::EvalVector4;
      break;
    }
    default:
    {
      function = Outside::EvalDefault;
      break;
    }
  } // end switch

  return function;
}

bool Outside::EvalBoolean( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const bool valueBoolean = value.GetBoolean();
  return ( (valueBoolean < arg[0]) || (valueBoolean > arg[1]) );
}

bool Outside::EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float valueFloat = value.GetFloat();
  return ( (valueFloat < arg[0]) || (valueFloat > arg[1]) );
}

bool Outside::EvalVector2( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float length2 = value.GetVector2().LengthSquared();
  return ( (length2 < arg[0]*arg[0]) || (length2 > arg[1]*arg[1]) );
}

bool Outside::EvalVector3( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float length2 = value.GetVector3().LengthSquared();
  return ( (length2 < arg[0]*arg[0]) || (length2 > arg[1]*arg[1]) );
}

bool Outside::EvalVector4( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  const float length2 = value.GetVector4().LengthSquared();
  return ( (length2 < arg[0]*arg[0]) || (length2 > arg[1]*arg[1]) );
}

bool Outside::EvalDefault( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg )
{
  return false;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
