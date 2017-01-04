#ifndef __DALI_CSHARP_TYPE_INFO_H__
#define __DALI_CSHARP_TYPE_INFO_H__

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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-value.h>

namespace Dali
{

namespace CSharpTypeInfo
{

  /**
   * @brief Call back used to create an instance of the associated object type
   *
   * @param[in] typeName The type name of the object to be created.
   * @return Pointer to a BaseHandle
   */
  typedef BaseHandle* (*CreateFunction)(const char* const typeName);

  /**
   * @brief Callback to set an event-thread only property.
   *
   * @param[in] object The object whose property should be set.
   * @param[in] propertyName The name of the property required.
   * @param[in] value The new value of the property for the object specified.
   * @see PropertyRegistration.
   */
  typedef void (*SetPropertyFunction)( BaseObject* object, const char* const propertyName , Property::Value* value );


  /**
   * @brief Callback to get the value of an event-thread only property.
   *
   * @param[in] object The object whose property value is required.
   * @param[in] propertyName The name of the property required.
   * @return The current value of the property for the object specified.
   * @see PropertyRegistration.
   */
  typedef Property::Value* (*GetPropertyFunction)( BaseObject* object, const char* const propertyName );
}


} // namespace Dali

#endif // __DALI_CSHARP_INFO_H__
