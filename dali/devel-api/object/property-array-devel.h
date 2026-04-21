#ifndef DALI_DEVEL_API_OBJECT_PROPERTY_ARRAY_DEVEL_H
#define DALI_DEVEL_API_OBJECT_PROPERTY_ARRAY_DEVEL_H

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
#include <dali/public-api/object/property-array.h>

namespace Dali
{
/**
 * @brief Creates a Property::Array from an initializer_list.
 *
 * This devel-api function allows convenient initialization of Property::Array
 * using brace-enclosed initializer lists.
 *
 * @param[in] values An initializer_list of Property::Value objects
 * @return A Property::Array containing the provided values
 *
 * @code
 * auto array = Dali::CreatePropertyArray({1.0f, 2.0f, 3.0f});
 * @endcode
 */
DALI_CORE_API Property::Array CreatePropertyArray(std::initializer_list<Property::Value> values);
} // namespace Dali

#endif // DALI_DEVEL_API_OBJECT_PROPERTY_ARRAY_DEVEL_H
