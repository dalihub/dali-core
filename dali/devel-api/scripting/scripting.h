#ifndef DALI_SCRIPTING_H
#define DALI_SCRIPTING_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/devel-api/animation/animation-data.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-value.h>

namespace Dali
{

class Actor;

/**
 * @brief Utilities for scripting support.
 */
namespace Scripting
{

/**
 * @brief Structure which stores an enumeration and its string equivalent.
 */
struct StringEnum
{
  const char* string; ///< The string representation
  const int value;    ///< The enumeration value wrapped in int
};

/**
 * @brief Find the given enum index from the table
 *
 * @param[in]  value       The string equivalent (case-insensitive).
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 * @return     The index of the enumeration. If enumeration is not found, logs an error and returns tableCount.
 */
DALI_IMPORT_API unsigned int FindEnumIndex( const char* value, const StringEnum* table, unsigned int tableCount );

/**
 * @brief Find the enum as an integer from the table
 *
 * @SINCE_1_1.12
 *
 * @param[in]  value       The string equivalent (case-insensitive, comma separate to OR values).
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 * @param[out] integerEnum The value of the enum.
 * @return     true if one or more enums in value.
 */
DALI_IMPORT_API bool EnumStringToInteger( const char* const value, const StringEnum* const table, unsigned int tableCount, int& integerEnum );

/**
 * @brief Chooses the appropriate enumeration for the provided string from the given table.
 *
 * @param[in]  value       The string equivalent (case-insensitive, comma separate to OR values).
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 * @param[out] result      The enum value
 *
 * @return     True if the value was found from the table
 */
template< typename T >
bool GetEnumeration( const char* value, const StringEnum* table, unsigned int tableCount, T& result )
{
  bool retVal( false );
  if( table )
  {
    int integerEnum = 0;
    // check to avoid crash, not asserting on purpose, error is logged instead
    if( EnumStringToInteger( value, table, tableCount, integerEnum ) )
    {
      result = static_cast<T>( integerEnum );
      retVal = true;
    }
  }
  return retVal;
}

/**
 * @brief Gets the enumeration value from an enumeration property.
 * An enumeration property is a property that can be set with either an INTEGER or STRING.
 *
 * @param[in]  propertyValue The property containing the int or string value.
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 * @param[out] result      The enum value. This is not modified if the enumeration could not be converted.
 * @return     True if the value was found successfully AND the value has changed. This is to allow the caller to do nothing if there is no change.
 */
template< typename T >
bool GetEnumerationProperty( const Property::Value& propertyValue, const StringEnum* table, unsigned int tableCount, T& result )
{
  int newValue;
  bool set = false;
  Property::Type type = propertyValue.GetType();

  if( type == Property::INTEGER )
  {
    // Attempt to fetch the property as an INTEGER type.
    if( propertyValue.Get( newValue ) )
    {
      // Success.
      set = true;
    }
  }
  else if( type == Property::STRING )
  {
    // Attempt to fetch the property as an STRING type, and convert it from string to enumeration value.
    std::string propertyString;
    if( table && propertyValue.Get( propertyString ) && EnumStringToInteger( propertyString.c_str(), table, tableCount, newValue ) )
    {
      // Success.
      set = true;
    }
  }

  // If the property was converted OK, AND the value has changed, update the result and return true.
  if( set && ( result != static_cast<T>( newValue ) ) )
  {
    result = static_cast<T>( newValue );
    return true;
  }

  // No change.
  return false;
}

/**
 * @brief Gets the enumeration value from a bitmask enumeration property.
 * An enumeration property is a property that can be set with either an INTEGER, STRING or an ARRAY of STRING.
 *
 * @param[in]  propertyValue The property containing the int, string or and array of string values.
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 * @param[out] result      The enum value. This is not modified if the enumeration could not be converted.
 * @return     True if the value was found successfully AND the value has changed. This is to allow the caller to do nothing if there is no change.
 */
template< typename T >
bool GetBitmaskEnumerationProperty( const Property::Value& propertyValue, const Scripting::StringEnum* table, unsigned int tableCount, T& result )
{
  bool returnValue = true;

  // Evaluate as a single INTEGER or STRING first.
  if( !GetEnumerationProperty( propertyValue, table, tableCount, result ) )
  {
    // If not, then check if it's an ARRAY
    if ( propertyValue.GetType() == Property::ARRAY )
    {
      int newValue = 0;
      Property::Array array;
      propertyValue.Get( array );
      for( Property::Array::SizeType i = 0; i < array.Count(); ++i )
      {
        Property::Value currentValue = array[ i ];
        // Use an initial value of -1 so any successful property conversion
        // causes a change (and true to be returned).
        T current = static_cast< T >( -1 );
        if( GetEnumerationProperty( currentValue, table, tableCount, current ) )
        {
          newValue |= current;
        }
        else
        {
          // We hit an invalid type.
          returnValue = false;
          break;
        }
      }

      // If we didn't hit an invalid type and the value has changed, update the result.
      if( returnValue && ( result != static_cast<T>( newValue ) ) )
      {
        result = static_cast<T>( newValue );
      }
    }
    else
    {
      // Property type was not ARRAY, and the single property evaluation also failed.
      returnValue = false;
    }
  }

  return returnValue;
}

/**
 * @brief Chooses the appropriate string for the provided enumeration from the given table.
 *
 * @param[in]  value       The enumeration.
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 *
 * @return     The equivalent enumeration for the given string. Will return NULL if the value does not exist
 *
 * @note The caller is NOT responsible for cleaning up the returned pointer as it is statically allocated.
 */
template< typename T >
const char* GetEnumerationName( T value, const StringEnum* table, unsigned int tableCount )
{
  if( table )
  {
    for ( unsigned int i = 0; i < tableCount; ++i )
    {
      if ( value == T(table[ i ].value) )
      {
        return table[ i ].string;
      }
    }
  }
  return NULL;
}

/**
 * @brief Chooses the appropriate string for the provided enumeration from the given table.
 * This is an optimised version that handles enumerations that start at 0 and are linear only.
 *
 * @param[in]  value       The enumeration.
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 *
 * @return     The equivalent enumeration for the given string. Will return NULL if the value does not exist
 *
 * @note The caller is NOT responsible for cleaning up the returned pointer as it is statically allocated.
 */
template< typename T >
const char * GetLinearEnumerationName( T value, const StringEnum* table, unsigned int tableCount )
{
  if ( table && ( value > 0 || value <= (int)tableCount ) )
  {
    return table[value].string;
  }
  return NULL;
}

/**
 * @brief Creates object with data from the property value map.
 *
 * @param[in] property The property value map with the following valid fields:
 * @code
 * "filename":      type std::string
 * "loadPolicy"     type std::string (enum)
 * "releasePolicy"  type std::string (enum)
 * "width"          type float
 * "height"         type float
 * "pixelFormat"    type std::string (enum)
 * "fittingMode"    type std::string (enum)
 * "samplingMode"   type std::string (enum)
 * "orientation"    type bool
 * "type"           type std::string (FrameBufferImage|BufferImage|ResourceImage(default))
 * @endcode
 * Some fields are optional and some only pertain to a specific type.
 *
 * @return A pointer to a newly created object.
 */
DALI_IMPORT_API Image NewImage( const Property::Value& property );

/**
 * @brief Creates an actor with the date from the property value map.
 *
 * @param[in] map The property value map with the properties (and hierarchy) of the actor required
 *                 For example:
 * @code
 * {
 *   "type": "Actor",
 *   "position": [ 100, 100, 0 ],
 *   "actors":
 *   [
 *     {
 *       "type":"Actor",
 *       "position":[0,0,0]
 *     }
 *   ]
 * }
 * @endcode
 *
 * @return A handle to the newly created actor.
 */
DALI_IMPORT_API Actor NewActor( const Property::Map& map );

/**
 * @brief Creates a Property::Map from the actor provided.
 *
 * @param[in]  actor The base-actor from which a Property::Map should be created
 * @param[out] map This map is cleared and a property map of actor and its children is filled in
 */
DALI_IMPORT_API void CreatePropertyMap( Actor actor, Property::Map& map );

/**
 * @brief Creates a Property::Map from the image provided.
 *
 * @param[in]  image The image from which a Property::Map should be created
 * @param[out] map This map is cleared and a property map of the image is filled in
 */
DALI_IMPORT_API void CreatePropertyMap( Image image, Property::Map& map );

/**
 * @brief Creates description data required to create an Animation object from a property map.
 *
 * @param[in]  map The property value map containing the animation description
 * @param[out] outputAnimationData Resultant data retrieved from the property map is written here
 */
DALI_IMPORT_API void NewAnimation( const Property::Map& map, Dali::AnimationData& outputAnimationData );

} // namespace Scripting

} // namespace Dali

#endif // DALI_SCRIPTING_H
