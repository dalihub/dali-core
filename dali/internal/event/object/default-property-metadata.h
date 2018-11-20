#ifndef DALI_DEFAULT_PROPERTY_METADATA_H
#define DALI_DEFAULT_PROPERTY_METADATA_H

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
#include <dali/public-api/object/property.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

/**
 * @brief Structure for setting up default properties and their details.
 */
struct PropertyDetails
{
  const char* name;           ///< The name of the property.
  Property::Index enumIndex;  ///< Used to check the index is correct within a debug build.
  Property::Type type;        ///< The property type.
  bool writable;              ///< Whether the property is writable
  bool animatable;            ///< Whether the property is animatable.
  bool constraintInput;       ///< Whether the property can be used as an input to a constraint.
};

/**
 * Struct to capture the address of the default property table and count of them.
 */
struct DefaultPropertyMetadata
{
  const PropertyDetails* propertyTable; ///< address of the table defining property meta-data.
  Property::Index propertyCount;        ///< count of the default properties.
};

} // namespace Dali

#endif // DALI_DEFAULT_PROPERTY_METADATA_H
