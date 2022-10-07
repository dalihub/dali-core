#ifndef DALI_INDIRECT_VALUE_H
#define DALI_INDIRECT_VALUE_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

class Handle;

/**
 * @brief Dali::IndirectValue is an intermediate object that enables a simpler
 * syntax for addressing properties.
 *
 * @SINCE_1_9.27
 * object["property"] = value;
 * float value = object["property"];
 *
 * It is not intended to be directly instantiated, instead, any Handle type
 * will generate a temporary object using the above syntax.
 */
class DALI_CORE_API IndirectValue
{
public:
  /**
   * @brief Assignment operator
   *
   * @SINCE_1_9.27
   * Enables "handle[property] = value" syntax.
   * @param[in] value The value to assign
   */
  void operator=(Property::Value value);

  /**
   * @brief Explicit cast operator for property value.
   *
   * @SINCE_1_9.27
   * Enables implicit promotion of this to a Property::Value type parameter
   * @return The property value
   */
  operator Property::Value()
  {
    return GetProperty();
  }

  /**
   * @brief Cast operator
   *
   * @SINCE_1_9.27
   * Enables "value = handle[property]" syntax.
   * @tparam Type The type of the associated property
   * @return The associated property cast to the desired type
   */
  template<typename Type>
  inline operator Type()
  {
    Property::Value value = GetProperty();
    return value.Get<Type>();
  }

private:
  /// @cond internal

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.27
   * Making this private to prevent construction of auto type or IndirectValue type.
   * @param[in] rhs The object to move
   */
  DALI_INTERNAL IndirectValue(IndirectValue&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.27
   * Making this private to prevent assignment to auto type or IndirectValue type.
   * @param[in] rhs The object to move
   */
  DALI_INTERNAL IndirectValue& operator=(IndirectValue&& rhs) noexcept;

  /**
   * @brief Accessor for handle property.
   *
   * @SINCE_1_9.27
   * @return The handle's property value
   * @note Asserts if the handle is empty
   */
  Property::Value GetProperty();

  friend class Handle; ///< Only Handle types can construct this object

  /**
   * @brief Private constructor
   *
   * @SINCE_1_9.27
   * @param[in] handle A reference to the associated handle
   * @param[in] index The index to the associated property
   */
  DALI_INTERNAL IndirectValue(Handle& handle, Property::Index index);

private:
  IntrusivePtr<Dali::RefObject> mHandle; ///< A handle to the property owner
  Property::Index               mIndex;  ///< Index of the property in the property owner.

  struct Extension;      ///< Reserved for future use
  Extension* mExtension; ///< Reserved for future use

  /// @endcond
};

/**
 * @}
 */
} // Namespace Dali

#endif // DALI_INDIRECT_VALUE_H
