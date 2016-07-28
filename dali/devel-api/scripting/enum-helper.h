#ifndef DALI_ENUM_HELPER_H
#define DALI_ENUM_HELPER_H

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
#include <dali/devel-api/scripting/scripting.h>

namespace Dali
{

/**
 * Macros for creating value, typically enumerations, to string tables.
 * Example:
 *
 * DALI_ENUM_TO_STRING_TABLE_BEGIN( SIZE_MODE )
 * DALI_ENUM_TO_STRING( USE_OWN_SIZE )
 * DALI_ENUM_TO_STRING( SIZE_EQUAL_TO_PARENT )
 * DALI_ENUM_TO_STRING_TABLE_END( SIZE_MODE )
 *
 * Creates:
 * const Scripting::StringEnum< SizeMode > SIZE_MODE_TABLE[] = {
 * { "USE_OWN_SIZE", USE_OWN_SIZE },
 * { "SIZE_EQUAL_TO_PARENT", SIZE_EQUAL_TO_PARENT },
 * }; const unsigned int SIZE_MODE_TABLE_COUNT = sizeof( SIZE_MODE_TABLE ) / sizeof( SIZE_MODE_TABLE[0] );
 *
 * By default, Dali::Scripting::StringEnum is used as the type for the table, however, a different type can be specified by using
 * DALI_ENUM_TO_STRING_TABLE_BEGIN_WITH_TYPE.
 */
#define DALI_ENUM_TO_STRING_TABLE_BEGIN_WITH_TYPE( type, t ) const type t##_TABLE[] = {
#define DALI_ENUM_TO_STRING_TABLE_BEGIN( t ) DALI_ENUM_TO_STRING_TABLE_BEGIN_WITH_TYPE( Dali::Scripting::StringEnum, t )
#define DALI_ENUM_TO_STRING_TABLE_END( t )   }; const unsigned int t##_TABLE_COUNT = sizeof( t##_TABLE ) / sizeof( t##_TABLE[0] );
#define DALI_ENUM_TO_STRING( s ) { #s, s },

/**
 * Adds a value, typically an enum, to the table within a scope but without the scope name
 * Example converts ( Layer, LAYER_2D ) to ( "LAYER_2D", Layer::Layer2D )
 */
#define DALI_ENUM_TO_STRING_WITH_SCOPE( className, enumName ) { #enumName, className::enumName },

} // namespace Dali

#endif // DALI_ENUM_HELPER_H
