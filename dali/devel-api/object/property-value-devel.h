#ifndef DALI_DEVEL_API_OBJECT_PROPERTY_VALUE_DEVEL_H
#define DALI_DEVEL_API_OBJECT_PROPERTY_VALUE_DEVEL_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <initializer_list>

// INTERNAL INCLUDES
#include <dali/public-api/object/property-value.h>

namespace Dali
{
/**
 * @brief Creates a Property::Value (as a Map) from an initializer_list.
 *
 * This devel-api function allows convenient initialization of Property::Value
 * as a map using brace-enclosed initializer lists.
 *
 * @param[in] values An initializer_list of key-value pairs
 * @return A Property::Value containing a Property::Map with the provided key-value pairs
 *
 * @code
 * auto value = Dali::CreatePropertyValue({
 *   {"key1", value1},
 *   {"key2", value2}
 * });
 * @endcode
 */
DALI_CORE_API Property::Value CreatePropertyValue(std::initializer_list<KeyValuePair> values);
} // namespace Dali

#endif // DALI_DEVEL_API_OBJECT_PROPERTY_VALUE_DEVEL_H
