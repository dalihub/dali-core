#ifndef __DALI_INTERNAL_RESOURCE_CLIENT_DECLARATIONS_H__
#define __DALI_INTERNAL_RESOURCE_CLIENT_DECLARATIONS_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/common/type-abstraction.h>

namespace Dali
{

namespace Internal
{

template <> struct ParameterType< Integration::LoadStatus >
: public BasicType< Integration::LoadStatus > {};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_CLIENT_DECLARATIONS_H__
