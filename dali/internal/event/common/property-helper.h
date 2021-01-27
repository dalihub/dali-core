#ifndef DALI_PROPERTY_HELPER_H
#define DALI_PROPERTY_HELPER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/event/object/default-property-metadata.h>

namespace Dali
{
namespace Internal
{
/**
 * These macros are used to define a table of property details per object.
 * Checking of the table index VS the property enum index happens during compile time.
 * the macros define an instance of PropertyMetadata with the name that is passed to DALI_PROPERTY_TABLE_END
 */
#define DALI_PROPERTY_TABLE_BEGIN static constexpr Dali::PropertyDetails DEFAULT_PROPERTY_DETAILS[] = {
#define DALI_PROPERTY_TABLE_END(startIndex, tableName)                                  \
  }                                                                                     \
  ;                                                                                     \
  static constexpr auto tableName = GeneratePropertyMetadata(DEFAULT_PROPERTY_DETAILS); \
  static_assert(CheckPropertyMetadata(tableName, startIndex), "Property enumeration mismatch");

#define DALI_PROPERTY(text, type, writable, animatable, constraint, index) {text, index, Dali::Property::type, writable, animatable, constraint},

/**
 * @brief Case insensitive string comparison.
 *
 * Additionally, '-' and '_' can be used interchangeably as well.
 * Returns if both strings have a ',' or a '\0' at the same point.
 *
 * @param[in]   first   The first string.
 * @param[in]   second  The string to compare it to.
 * @param[out]  size    The size of the string.
 *
 * @return true if strings are the same
 */
bool CompareTokens(const char* first, const char* second, uint32_t& size);

/**
 * @brief Helper to adjust the current value of a variable from the given property-value
 * @param[in] currentValue The current value of the property
 * @param[in] value The relative value as a Property::Value
 * @return true if value adjusted, false otherwise
 */
template<typename PropertyType>
bool AdjustValue(PropertyType& currentValue, const Property::Value& value)
{
  PropertyType relativeValue;
  if(value.Get(relativeValue))
  {
    currentValue += relativeValue;
    return true;
  }
  return false;
}

} // namespace Internal

} // namespace Dali

#endif // DALI_PROPERTY_HELPER_H
