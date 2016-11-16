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

// FILE HEADER
#include <dali/devel-api/object/handle-devel.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/property-key.h>
#include <dali/internal/event/common/object-impl.h>

namespace Dali
{

namespace DevelHandle
{

Property::Index GetPropertyIndex( const Handle& handle, Property::Index key )
{
  return GetImplementation( handle ).GetPropertyIndex( key );
}

Property::Index GetPropertyIndex( const Handle& handle, Property::Key key )
{
  return GetImplementation( handle ).GetPropertyIndex( key );
}

Property::Index RegisterProperty( Handle handle, Property::Index key, const std::string& name, const Property::Value& propertyValue )
{
  return GetImplementation( handle ).RegisterProperty( name, key, propertyValue );
}

} // namespace DevelHandle

} // namespace Dali
