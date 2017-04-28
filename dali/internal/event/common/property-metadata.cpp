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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property.h>

namespace Dali
{

namespace Internal
{

void PropertyMetadata::SetPropertyValue( const Property::Value& propertyValue )
{
  switch ( GetType() )
  {
    case Property::NONE:
    case Property::RECTANGLE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::BOOLEAN:
    case Property::INTEGER:
    case Property::FLOAT:
    case Property::ROTATION:
    case Property::MATRIX:
    case Property::MATRIX3:
    {
      value = propertyValue;
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

} // namespace Internal

} // namespace Dali
