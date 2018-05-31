#ifndef __DALI_PROPERTY_CONDITIONS_H__
#define __DALI_PROPERTY_CONDITIONS_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

namespace Internal DALI_INTERNAL
{
class PropertyCondition;
}

/**
 * @brief This represents a condition that can be evaluated on a Property::Value.
 * @SINCE_1_0.0
 */
class DALI_CORE_API PropertyCondition : public BaseHandle
{
public:

  /**
   * @brief Constructor for condition clause.
   * @SINCE_1_0.0
   */
  PropertyCondition();

  /**
   * @brief Destructor.
   * @SINCE_1_0.0
   */
  ~PropertyCondition();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  PropertyCondition( const PropertyCondition& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  PropertyCondition& operator=( const PropertyCondition& rhs );

public:

  /**
   * @brief Retrieves the arguments that this condition uses.
   *
   * @SINCE_1_0.0
   * @return The arguments used for this condition
   */
  std::size_t GetArgumentCount() const;

  /**
   * @brief Retrieves the arguments that this condition uses.
   *
   * @SINCE_1_0.0
   * @param[in] index The condition index to get the argument
   * @return The arguments used for this condition
   * @note The container will only be valid as long PropertyCondition is valid.
   */
  float GetArgument( std::size_t index ) const;

};

/**
 * @brief LessThanCondition compares whether property is less than arg.
 *
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @SINCE_1_0.0
 * @param[in] arg The argument for the condition
 * @return A property condition function object
 */
DALI_CORE_API PropertyCondition LessThanCondition( float arg );

/**
 * @brief GreaterThanCondition compares whether property is greater than arg.
 *
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @SINCE_1_0.0
 * @param[in] arg The argument for the condition
 * @return A property condition function object
 */
DALI_CORE_API PropertyCondition GreaterThanCondition( float arg );

/**
 * @brief InsideCondition compares whether property is greater than arg0 and less than arg1.
 *
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @SINCE_1_0.0
 * @param[in] arg0 The first argument for the condition
 * @param[in] arg1 The second argument for the condition
 * @return A property condition function object
 */
DALI_CORE_API PropertyCondition InsideCondition( float arg0, float arg1 );

/**
 * @brief OutsideCondition compares whether property is less than arg0 or greater than arg1.
 *
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @SINCE_1_0.0
 * @param[in] arg0 The first argument for the condition
 * @param[in] arg1 The second argument for the condition
 * @return A property condition function object
 */
DALI_CORE_API PropertyCondition OutsideCondition( float arg0, float arg1 );

/**
 * @brief Detects when a property changes by stepAmount from initialValue, in both positive and negative directions. This will continue checking for multiples of stepAmount.
 *
 * property type:
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @SINCE_1_0.0
 * @param[in] stepAmount The step size required to trigger condition
 * @param[in] initialValue The initial value to step from
 * @return A property condition function object
 */
DALI_CORE_API PropertyCondition StepCondition( float stepAmount, float initialValue = 0.0f );

/**
 * @brief Receives notifications as a property goes above/below the inputted values. Values must be ordered and can be either ascending or descending.
 *
 * property type:
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @SINCE_1_0.0
 * @param[in] steps List of values to receive notifications for as a property crosses them
 * @return A property condition function object
 */
DALI_CORE_API PropertyCondition VariableStepCondition( const Dali::Vector<float>& steps );

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PROPERTY_CONDITIONS_H__
