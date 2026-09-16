#ifndef DALI_DEVEL_API_OBJECT_PROPERTY_DEVEL_H
#define DALI_DEVEL_API_OBJECT_PROPERTY_DEVEL_H

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

// INTERNAL INCLUDES
#include <dali/public-api/object/property.h>

namespace DALI_NAMESPACE
{
/**
 * @brief Property types that are not part of Property::Type in the public API.
 *
 * They continue the numbering of Property::Type, so a type added to Property::Type must move them.
 * @note They are not enumerators of Property::Type, so a switch over a Property::Type cannot have a case label for them.
 */
namespace DevelProperty
{
constexpr Property::Type EXTENTS = static_cast<Property::Type>(Property::INSETS + 1); ///< A collection of 4 x int16_t, see Dali::Extents.

} // namespace DevelProperty

} // namespace DALI_NAMESPACE

#endif // DALI_DEVEL_API_OBJECT_PROPERTY_DEVEL_H
