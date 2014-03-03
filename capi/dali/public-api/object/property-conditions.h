#ifndef __DALI_PROPERTY_CONDITIONS_H__
#define __DALI_PROPERTY_CONDITIONS_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class PropertyCondition;
}

/**
 * PropertyCondition class represents a condition that
 * can be evaluated on a Property::Value.
 */
class PropertyCondition : public BaseHandle
{
public:

  /**
   * Argument container, represents the arguments to be supplied
   * to the condition function.
   */
  typedef std::vector<Property::Value> ArgumentContainer;
  typedef ArgumentContainer::iterator ArgumentIter;
  typedef ArgumentContainer::const_iterator ArgumentConstIter;

public:

  /**
   * Constructor for condition clause
   */
  PropertyCondition();

  /**
   * Destructor
   */
  ~PropertyCondition();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public:

  /**
   * Retrieve the arguments that this condition uses.
   * @return The arguments used for this condition
   */
  ArgumentContainer GetArguments();

  /**
   * Retrieve the arguments that this condition uses.
   * @return The arguments used for this condition
   * @note The container will only be valid as long PropertyCondition is valid.
   */
  const ArgumentContainer& GetArguments() const;

};

/**
 * LessThanCondition compares
 * whether property is less than arg:
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @param[in] arg The argument for the condition
 */
PropertyCondition LessThanCondition(float arg);

/**
 * GreaterThanCondition compares
 * whether property is greater than arg:
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @param[in] arg The argument for the condition
 */
PropertyCondition GreaterThanCondition(float arg);

/**
 * InsideCondition compares
 * whether property is greater than arg0 and less than arg1:
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @param[in] arg0 The first argument for the condition
 * @param[in] arg1 The second argument for the condition
 */
PropertyCondition InsideCondition(float arg0, float arg1);

/**
 * OutsideCondition compares
 * whether property is less than arg0 or greater than arg1:
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @param[in] arg0 The first argument for the condition
 * @param[in] arg1 The second argument for the condition
 */
PropertyCondition OutsideCondition(float arg0, float arg1);

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_PROPERTY_CONDITIONS_H__
