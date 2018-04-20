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

// CLASS HEADER
#include <dali/internal/event/common/property-metadata.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-array.h>

namespace Dali
{

namespace Internal
{

namespace
{

/// Helper to adjust the property value by an amount specified in another property-value
template < typename PropertyType >
inline void AdjustProperty( Property::Value& currentPropertyValue, const Property::Value& relativePropertyValue )
{
  PropertyType currentValue;
  PropertyType relativeValue;
  if( currentPropertyValue.Get( currentValue ) && relativePropertyValue.Get( relativeValue ) )
  {
    currentPropertyValue = currentValue + relativeValue;
  }
}

} // unnamed namespace

void PropertyMetadata::SetPropertyValue( const Property::Value& propertyValue )
{
  switch ( GetType() )
  {
    case Property::NONE:
    {
      // NOOP
      break;
    }

    case Property::RECTANGLE:
    {
      Rect<int> convertedValue;
      if( propertyValue.Get( convertedValue ) )
      {
        value = convertedValue;
      }
      break;
    }

    case Property::STRING:
    {
      std::string convertedValue;
      if( propertyValue.Get( convertedValue ) )
      {
        value = convertedValue;
      }
      break;
    }

    case Property::ARRAY:
    {
      Property::Array* array = propertyValue.GetArray();
      if( array )
      {
        value = *array;
      }
      break;
    }

    case Property::MAP:
    {
      Property::Map* map = propertyValue.GetMap();
      if( map )
      {
        value = *map;
      }
      break;
    }

    case Property::EXTENTS:
    {
      Extents convertedValue;
      if( propertyValue.Get( convertedValue ) )
      {
        value = convertedValue;
      }
      break;
    }

    case Property::BOOLEAN:
    {
      bool convertedValue;
      if( propertyValue.Get( convertedValue ) )
      {
        value = convertedValue;
      }
      break;
    }

    case Property::INTEGER:
    {
      int convertedValue;
      if( propertyValue.Get( convertedValue ) )
      {
        value = convertedValue;
      }
      break;
    }

    case Property::FLOAT:
    {
      float convertedValue;
      if( propertyValue.Get( convertedValue ) )
      {
        value = convertedValue;
      }
      break;
    }

    case Property::ROTATION:
    {
      Quaternion convertedValue;
      if( propertyValue.Get( convertedValue ) )
      {
        value = convertedValue;
      }
      break;
    }

    case Property::MATRIX:
    {
      Matrix convertedValue;
      if( propertyValue.Get( convertedValue ) )
      {
        value = convertedValue;
      }
      break;
    }

    case Property::MATRIX3:
    {
      Matrix3 convertedValue;
      if( propertyValue.Get( convertedValue ) )
      {
        value = convertedValue;
      }
      break;
    }

    case Property::VECTOR2:
    {
      Vector2 vector2Value;
      value.Get( vector2Value );

      if( componentIndex == 0 )
      {
        vector2Value.x = propertyValue.Get< float >();
      }
      else if( componentIndex == 1 )
      {
        vector2Value.y = propertyValue.Get< float >();
      }
      else
      {
        propertyValue.Get( vector2Value );
      }

      value = vector2Value;
      break;
    }

    case Property::VECTOR3:
    {
      Vector3 vector3Value;
      value.Get( vector3Value );

      if( componentIndex == 0 )
      {
        vector3Value.x = propertyValue.Get< float >();
      }
      else if( componentIndex == 1 )
      {
        vector3Value.y = propertyValue.Get< float >();
      }
      else if( componentIndex == 2 )
      {
        vector3Value.z = propertyValue.Get< float >();
      }
      else
      {
        propertyValue.Get( vector3Value );
      }

      value = vector3Value;
      break;
    }

    case Property::VECTOR4:
    {
      Vector4 vector4Value;
      value.Get( vector4Value );

      if( componentIndex == 0 )
      {
        vector4Value.x = propertyValue.Get< float >();
      }
      else if( componentIndex == 1 )
      {
        vector4Value.y = propertyValue.Get< float >();
      }
      else if( componentIndex == 2 )
      {
        vector4Value.z = propertyValue.Get< float >();
      }
      else if( componentIndex == 3 )
      {
        vector4Value.w = propertyValue.Get< float >();
      }
      else
      {
        propertyValue.Get( vector4Value );
      }

      value = vector4Value;
      break;
    }
  }
}

Property::Value PropertyMetadata::GetPropertyValue() const
{
  Property::Value propertyValue;

  if( !IsAnimatable() )
  {
    propertyValue = value;
  }
  else
  {
    switch ( GetType() )
    {
      case Property::NONE:
      case Property::RECTANGLE:
      case Property::STRING:
      case Property::ARRAY:
      case Property::MAP:
      case Property::EXTENTS:
      case Property::BOOLEAN:
      case Property::INTEGER:
      case Property::FLOAT:
      case Property::MATRIX:
      case Property::MATRIX3:
      case Property::ROTATION:
      {
        propertyValue = value;
        break;
      }

      case Property::VECTOR2:
      {
        Vector2 vector2Value;
        value.Get( vector2Value );

        if( componentIndex == 0 )
        {
          propertyValue = vector2Value.x;
        }
        else if( componentIndex == 1 )
        {
          propertyValue = vector2Value.y;
        }
        else
        {
          propertyValue = vector2Value;
        }
        break;
      }

      case Property::VECTOR3:
      {
        Vector3 vector3Value;
        value.Get( vector3Value );

        if( componentIndex == 0 )
        {
          propertyValue = vector3Value.x;
        }
        else if( componentIndex == 1 )
        {
          propertyValue = vector3Value.y;
        }
        else if( componentIndex == 2 )
        {
          propertyValue = vector3Value.z;
        }
        else
        {
          propertyValue = vector3Value;
        }
        break;
      }

      case Property::VECTOR4:
      {
        Vector4 vector4Value;
        value.Get( vector4Value );

        if( componentIndex == 0 )
        {
          propertyValue = vector4Value.x;
        }
        else if( componentIndex == 1 )
        {
          propertyValue = vector4Value.y;
        }
        else if( componentIndex == 2 )
        {
          propertyValue = vector4Value.z;
        }
        else if( componentIndex == 3 )
        {
          propertyValue = vector4Value.w;
        }
        else
        {
          propertyValue = vector4Value;
        }
        break;
      }
    }
  }

  return propertyValue;
}

void PropertyMetadata::AdjustPropertyValueBy( const Property::Value& relativePropertyValue )
{
  switch ( GetType() )
  {
    case Property::NONE:
    case Property::RECTANGLE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::EXTENTS:
    case Property::MATRIX:
    case Property::MATRIX3:
    {
      // Not animated
      break;
    }

    case Property::BOOLEAN:
    {
      bool currentValue = false;
      bool relativeValue = false;
      if( value.Get( currentValue ) && relativePropertyValue.Get( relativeValue ) )
      {
        value = currentValue || relativeValue;
      }
      break;
    }

    case Property::INTEGER:
    {
      AdjustProperty< int >( value, relativePropertyValue );
      break;
    }

    case Property::FLOAT:
    {
      AdjustProperty< float >( value, relativePropertyValue );
      break;
    }

    case Property::ROTATION:
    {
      Quaternion currentValue;
      Quaternion relativeValue;
      if( value.Get( currentValue ) && relativePropertyValue.Get( relativeValue ) )
      {
        value = currentValue * relativeValue;
      }
      break;
    }

    case Property::VECTOR2:
    {
      if( componentIndex == Property::INVALID_COMPONENT_INDEX )
      {
        AdjustProperty< Vector2 >( value, relativePropertyValue );
      }
      else
      {
        Vector2 vector2Value;
        value.Get( vector2Value );

        if( componentIndex == 0 )
        {
          vector2Value.x += relativePropertyValue.Get< float >();
        }
        else if( componentIndex == 1 )
        {
          vector2Value.y += relativePropertyValue.Get< float >();
        }

        value = vector2Value;
      }

      break;
    }

    case Property::VECTOR3:
    {
      if( componentIndex == Property::INVALID_COMPONENT_INDEX )
      {
        AdjustProperty< Vector3 >( value, relativePropertyValue );
      }
      else
      {
        Vector3 vector3Value;
        value.Get( vector3Value );

        if( componentIndex == 0 )
        {
          vector3Value.x += relativePropertyValue.Get< float >();
        }
        else if( componentIndex == 1 )
        {
          vector3Value.y += relativePropertyValue.Get< float >();
        }
        else if( componentIndex == 2 )
        {
          vector3Value.z += relativePropertyValue.Get< float >();
        }

        value = vector3Value;
      }
      break;
    }

    case Property::VECTOR4:
    {
      if( componentIndex == Property::INVALID_COMPONENT_INDEX )
      {
        AdjustProperty< Vector4 >( value, relativePropertyValue );
      }
      else
      {
        Vector4 vector4Value;
        value.Get( vector4Value );

        if( componentIndex == 0 )
        {
          vector4Value.x += relativePropertyValue.Get< float >();
        }
        else if( componentIndex == 1 )
        {
          vector4Value.y += relativePropertyValue.Get< float >();
        }
        else if( componentIndex == 2 )
        {
          vector4Value.z += relativePropertyValue.Get< float >();
        }
        else if( componentIndex == 3 )
        {
          vector4Value.w += relativePropertyValue.Get< float >();
        }

        value = vector4Value;
      }
      break;
    }
  }
}

} // namespace Internal

} // namespace Dali
