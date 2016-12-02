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

// CLASS HEADER
#include <dali/devel-api/object/csharp-type-registry.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/type-registry-impl.h>

namespace Dali
{

namespace CSharpTypeRegistry
{

bool RegisterType( const std::string& name, const std::type_info& baseType, CSharpTypeInfo::CreateFunction f )
{
  Internal::TypeRegistry *impl = Internal::TypeRegistry::Get();

  return impl->Register( name, baseType, f );
}

bool RegisterProperty( const std::string& objectName,
                       const std::string& name,
                       Property::Index index,
                       Property::Type type,
                       CSharpTypeInfo::SetPropertyFunction setFunc,
                       CSharpTypeInfo::GetPropertyFunction getFunc )
{

  Internal::TypeRegistry *impl = Internal::TypeRegistry::Get();

  return impl->RegisterProperty( objectName, name, index, type, setFunc, getFunc );
}

}

} // namespace Dali
