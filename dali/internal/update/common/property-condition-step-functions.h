#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_CONDITION_STEP_FUNCTIONS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_CONDITION_STEP_FUNCTIONS_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/property-value.h>
#include <dali/internal/update/common/scene-graph-property-notification.h>

namespace Dali
{

namespace Internal
{

class ProxyObject;
class PropertyNotification;

namespace SceneGraph
{

/**
 * Step condition class,
 * Checks if a Property has stepped a certain amount from the reference point
 *
 * float      => value has stepped arg1 amount from arg0.
 * Vector2    => 2 dimensional length of vector has stepped arg1 amount from arg0.
 * Vector3    => 3 dimensional length of vector has stepped arg1 amount from arg0.
 * Vector4    => 4 dimensional length of vector has stepped arg1 amount from arg0.
 * Default    => return false.
 */
class Step
{

public:

  /**
   * @return function pointer to the correct condition function, based on
   * the type of value being examined.
   */
  static ConditionFunction GetFunction( Property::Type valueType );

private:

  static bool Evaluate( const float propertyValue, PropertyNotification::RawArgumentContainer& arg );

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

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_CONDITION_STEP_FUNCTIONS_H__
