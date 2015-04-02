#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_CONDITION_VARIABLE_STEP_FUNCTIONS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_CONDITION_VARIABLE_STEP_FUNCTIONS_H__

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

namespace Dali
{

namespace Internal
{

class Object;
class PropertyNotification;

namespace SceneGraph
{

/**
 * VariableStep condition class,
 * Uses a list of values, this condition notifies when the Property crosses between different values
 *
 * float      => value has stepped arg1 amount from arg0.
 * Vector2    => 2 dimensional length of vector has stepped arg1 amount from arg0.
 * Vector3    => 3 dimensional length of vector has stepped arg1 amount from arg0.
 * Vector4    => 4 dimensional length of vector has stepped arg1 amount from arg0.
 * Default    => return false.
 */
class VariableStep
{

public:

  /**
   * @return function pointer to the correct condition function, based on
   * the type of value being examined.
   */
  static ConditionFunction GetFunction( Property::Type valueType );

private:

  /**
   * Helper function used to evaluate our property
   *
   *
   */
  static bool Evaluate( const float propertyValue, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if integer is Outside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalInteger( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if unsigned integer is Outside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalUnsignedInteger( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

  /**
   * Checks if float is Outside
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalFloat( const Dali::PropertyInput& value, PropertyNotification::RawArgumentContainer& arg );

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

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_CONDITION_VARIABLE_STEP_FUNCTIONS_H__
