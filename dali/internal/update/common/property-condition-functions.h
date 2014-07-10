#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_CONDITION_FUNCTIONS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_CONDITION_FUNCTIONS_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/property-value.h>
#include <dali/internal/update/common/scene-graph-property-notification.h>

// OTHER CONDITITION INCLUDES
#include <dali/internal/update/common/property-condition-step-functions.h>
#include <dali/internal/update/common/property-condition-variable-step-functions.h>

namespace Dali
{

namespace Internal
{

class ProxyObject;
class PropertyNotification;

namespace SceneGraph
{

// LessThan ///////////////////////////////////////////////////////////////////

/**
 * LessThan condition class,
 * Checks if a Property is "Less Than" the argument:
 *
 * bool       => false (0.0) or true (1.0) is less than arg0.
 * float      => value is less than arg0.
 * Vector2    => 2 dimensional length of vector is less than arg0.
 * Vector3    => 3 dimensional length of vector is less than arg0.
 * Vector4    => 4 dimensional length of vector is less than arg0.
 * Default    => return false.
 */
class LessThan
{

public:

  /**
   * @return function pointer to the correct condition function, based on
   * the type of value being examined.
   */
  static ConditionFunction GetFunction(Property::Type valueType);

private:

  /**
   * Checks if bool is LessThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalBoolean( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if float is LessThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if integer is LessThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalInteger( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector2.Length() is LessThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector2( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector3.Length() is LessThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector3( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector4.Length() is LessThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector4( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Default check for other types
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalDefault( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

};

// GreaterThan ////////////////////////////////////////////////////////////////

/**
 * GreaterThan condition class,
 * Checks if a Property is "Greater Than" the argument:
 *
 * bool       => false (0.0) or true (1.0) is greater than arg0.
 * float      => value is greater than arg0.
 * Vector2    => 2 dimensional length of vector is greater than arg0.
 * Vector3    => 3 dimensional length of vector is greater than arg0.
 * Vector4    => 4 dimensional length of vector is greater than arg0.
 * Default    => return false.
 */
class GreaterThan
{

public:

  /**
   * @returns function pointer to the correct condition function, based on
   * the type of value being examined.
   */
  static ConditionFunction GetFunction(Property::Type valueType);

private:

  /**
   * Checks if bool is GreaterThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalBoolean( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if float is GreaterThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if integer is GreaterThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalInteger( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector2.Length() is GreaterThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector2( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector3.Length() is GreaterThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector3( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector4.Length() is GreaterThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector4( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Default check for other types.
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalDefault( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

};

// Inside /////////////////////////////////////////////////////////////////////

/**
 * Inside condition class,
 * Checks if a Property is "Inside" the two arguments:
 *
 * bool       => false (0.0) or true (1.0) is inside arg0.
 * float      => value is between arg0 and arg1.
 * Vector2    => 2 dimensional length of vector is between arg0 and arg1.
 * Vector3    => 3 dimensional length of vector is between arg0 and arg1.
 * Vector4    => 4 dimensional length of vector is between arg0 and arg1.
 * Default    => return false.
 */
class Inside
{

public:

  /**
   * @return function pointer to the correct condition function, based on
   * the type of value being examined.
   */
  static ConditionFunction GetFunction(Property::Type valueType);

private:

  /**
   * Checks if bool is Inside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalBoolean( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if float is Inside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if integer is Inside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalInteger( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector2.Length() is Inside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector2( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector3.Length() is Inside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector3( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector4.Length() is Inside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector4( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Default check for other types.
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalDefault( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

};

// Outside ////////////////////////////////////////////////////////////////////

/**
 * Outside condition class,
 * Checks if a Property is "Outside" the two arguments:
 *
 * bool       => false (0.0) or true (1.0) is outside arg0.
 * float      => value is outside arg0 and arg1.
 * Vector2    => 2 dimensional length of vector is outside arg0 and arg1.
 * Vector3    => 3 dimensional length of vector is outside arg0 and arg1.
 * Vector4    => 4 dimensional length of vector is outside arg0 and arg1.
 * Default    => return false.
 */
class Outside
{

public:

  /**
   * @return function pointer to the correct condition function, based on
   * the type of value being examined.
   */
  static ConditionFunction GetFunction(Property::Type valueType);

private:

  /**
   * Checks if bool is Outside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalBoolean( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if float is Outside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if integer is Outside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalInteger( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector2.Length() is Outside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector2( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector3.Length() is Outside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector3( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if Vector4.Length() is Outside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalVector4( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Default check for other types.
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalDefault( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_CONDITION_FUNCTIONS_H__
