#ifndef __DALI_PROPERTY_HELPER_H__
#define __DALI_PROPERTY_HELPER_H__

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

// INTERNAL INCLUDES
#include <dali/integration-api/bitmap.h>

namespace Dali
{

namespace Internal
{

/**
 * @brief Structure for setting up default properties and their details.
 */
struct PropertyDetails
{
  const char* name;             ///< The name of the property.
  Property::Type type;          ///< The property type.
  bool writable:1;              ///< Whether the property is writable
  bool animatable:1;            ///< Whether the property is animatable.
  bool constraintInput:1;       ///< Whether the property can be used as an input to a constraint.
#ifdef DEBUG_ENABLED
  Property::Index enumIndex;    ///< Used to check the index is correct within a debug build.
#endif
};

/**
 * These macros are used to define a table of property details per Actor object.
 * The index property is only compiled in for DEBUG_ENABLED builds and allows checking the table index VS the property enum index.
 * DALI_PROPERTY_TABLE_END Forces a run-time check that will happen once.
 */
#define DALI_PROPERTY_TABLE_BEGIN const Internal::PropertyDetails DEFAULT_PROPERTY_DETAILS[] = {
#ifdef DEBUG_ENABLED
#define DALI_PROPERTY_TABLE_END( startIndex )   }; const int DEFAULT_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_DETAILS ) / sizeof( Internal::PropertyDetails ); \
  struct PROPERTY_CHECK \
  { \
    PROPERTY_CHECK() \
    { \
      for( int i = 0; i < DEFAULT_PROPERTY_COUNT; i++ ) \
      { \
        if ( DEFAULT_PROPERTY_DETAILS[i].enumIndex != ( startIndex + i ) ) \
        { \
          DALI_LOG_ERROR( "Checking property failed: index:%d, enumIndex:%d == index+start:%d, (name:%s)\n", i, \
                          DEFAULT_PROPERTY_DETAILS[i].enumIndex, (startIndex + i), DEFAULT_PROPERTY_DETAILS[i].name ); \
          DALI_ASSERT_DEBUG( false && "Property enumeration mismatch" ); \
        } \
      } \
    } \
  }; \
  static PROPERTY_CHECK PROPERTY_CHECK_INSTANCE;
#else
#define DALI_PROPERTY_TABLE_END( startIndex )   }; const int DEFAULT_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_DETAILS ) / sizeof( Internal::PropertyDetails );
#endif
#ifdef DEBUG_ENABLED
#define DALI_PROPERTY( text, type, writable, animatable, constraint, index ) { text, Dali::Property::type, writable, animatable, constraint, index },
#else
#define DALI_PROPERTY( text, type, writable, animatable, constraint, index ) { text, Dali::Property::type, writable, animatable, constraint },
#endif

/**
 * Macros for creating enumeration to string tables.
 * Example:
 *
 * DALI_ENUM_TO_STRING_TABLE_BEGIN( SizeMode )
 * DALI_ENUM_TO_STRING( USE_OWN_SIZE )
 * DALI_ENUM_TO_STRING( SIZE_EQUAL_TO_PARENT )
 * DALI_ENUM_TO_STRING_TABLE_END( SizeMode )
 *
 * Creates:
 * const Scripting::StringEnum< SizeMode > SizeModeTable[] = {
 * { "USE_OWN_SIZE", USE_OWN_SIZE },
 * { "SIZE_EQUAL_TO_PARENT", SIZE_EQUAL_TO_PARENT },
 * }; const unsigned int SizeModeTableCount = sizeof( SizeModeTable ) / sizeof( SizeModeTable[0] );
 */
#define DALI_ENUM_TO_STRING_TABLE_BEGIN( t ) const Scripting::StringEnum< t > t##Table[] = {
#define DALI_ENUM_TO_STRING_TABLE_END( t )   }; const unsigned int t##TableCount = sizeof( t##Table ) / sizeof( t##Table[0] );
#define DALI_ENUM_TO_STRING( s ) { #s, s },


} // namespace Internal

} // namespace Dali

#endif // __DALI_PROPERTY_HELPER_H__

