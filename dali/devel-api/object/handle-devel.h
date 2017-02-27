#ifndef DALI_HANDLE_DEVEL_H
#define DALI_HANDLE_DEVEL_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/handle.h>

namespace Dali
{

namespace DevelHandle
{

/**
 * @brief Query the index of a custom property matching the given key.
 *
 * Returns the first custom property that matches the given integer key. This is
 * useful for other classes that know the key but not the name. Requires the property
 * to have been registered with the associated key.
 *
 * @note This key is not the same as the Property enum found in
 * objects such as Actor (which is a preset index).
 *
 * @param[in] handle The handle from where to retrieve the property index.
 * @param[in] key The integer key of the property
 *
 * @return The index of the property, or Property::INVALID_INDEX if no property exists with the given key.
 *
 * @note The key is not the same as the returned index, though it has the same type.
 */
DALI_IMPORT_API Property::Index GetPropertyIndex( const Handle& handle, Property::Index key );

/**
 * @brief Query the index of a property using the given key from a Property::Map
 *
 * @param[in] handle The handle from where to retrieve the property index.
 * @param[in] key The key of the property to search for.
 *
 * @return the matching property index of either the string key or the matching
 * custom property index of the index key, or Property::INVALID_INDEX if no
 * property matches the given key.
 *
 * @note See also, GetPropertyIndex(Property::Index) and GetPropertyIndex(const std::string&)
 */
DALI_IMPORT_API Property::Index GetPropertyIndex( const Handle& handle, Property::Key key );

/**
 * @brief Register a new animatable property with an integer key.
 *
 * @param[in] handle The handle where to register the property.
 * @param[in] key  The integer key of the property.
 * @param[in] name The text key of the property.
 * @param[in] propertyValue The new value of the property.
 *
 * @return The index of the property or Property::INVALID_INDEX if registration failed
 *
 * @pre The object supports dynamic properties i.e. Supports(Handle::DYNAMIC_PROPERTIES) returns true.
 * Property names and keys are expected to be unique, but this is not enforced.
 * Property indices are unique to each registered custom property in a given object.
 *
 * @note Returns Property::INVALID_INDEX if registration failed. This can happen if you try to register
 * animatable property on an object that does not have scene graph object.
 *
 * @note The returned property index is not the same as the integer key (though it shares a type)
 *
 * This version of RegisterProperty associates both an integer key
 * and the text key with the property, allowing for lookup of the
 * property index by either key or name ( which is useful when other
 * classes know the key but not the name )
 *
 * @note Only the following types can be animated:
 *       - Property::BOOLEAN
 *       - Property::FLOAT
 *       - Property::INTEGER
 *       - Property::VECTOR2
 *       - Property::VECTOR3
 *       - Property::VECTOR4
 *       - Property::MATRIX3
 *       - Property::MATRIX
 *       - Property::ROTATION
 * @note If a property with the desired name already exists, then the value given is just set.
 */
DALI_IMPORT_API Property::Index RegisterProperty( Handle handle, Property::Index key, const std::string& name, const Property::Value& propertyValue );

/**
 * @brief Set the type-info that the object is created by.
 *
 * @note This is particularly useful to link C# custom control with its correct type-info in the native side
 *
 * @param[in] handle The handle created by this TypeInfo.
 * @param[in] typeInfo The TypeInfo that creates the handle.
 */
DALI_IMPORT_API void SetTypeInfo( Handle& handle, const TypeInfo& typeInfo );

} // namespace DevelHandle

} // namespace Dali

#endif // DALI_HANDLE_DEVEL_H
