#ifndef __DALI_CSHARP_TYPE_REGISTRY_H__
#define __DALI_CSHARP_TYPE_REGISTRY_H__

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


// EXTERNAL INCLUDES
#include <typeinfo>


// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/csharp-type-info.h>

namespace Dali
{


/**
 * Custom static functions for registering C# custom controls and properties
 *
 */
namespace CSharpTypeRegistry
{

/**
 * @brief Constructor registers the type creation function for a named class or type.
 *
 * This allows types to be created dynamically from script. The name must be
 * unique for successful registration.
 * @param [in] name the name of the type to be registered
 * @param [in] baseType the base type info of registerType
 * @param [in] f registerType instance creation function
 * @return true if the name could be registered.
 */
  DALI_IMPORT_API bool RegisterType( const std::string& name,
                                     const std::type_info& baseType,
                                     CSharpTypeInfo::CreateFunction f );

  /**
   * Register an event-thread only property with a type (used by C# Custom controls)
   * @param [in] objectName name of the object used to register the type
   * @param [in] name Property name
   * @param [in] index Property index
   * @param [in] type Property type
   * @param [in] setFunc The function to set the property (Can be NULL).
   * @param [in] getFunc The function to get the value of a property.
   * @return true if the property could be registered.
  */
  DALI_IMPORT_API bool RegisterProperty( const std::string& objectName,
                                         const std::string& name,
                                         Property::Index index,
                                         Property::Type type,
                                         CSharpTypeInfo::SetPropertyFunction setFunc,
                                         CSharpTypeInfo::GetPropertyFunction getFunc );
}

} // namespace Dali

#endif // header
