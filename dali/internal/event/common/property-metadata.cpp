/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/extents.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property.h>

namespace Dali
{
namespace Internal
{
namespace
{
/// Helper to adjust the property value by an amount specified in another property-value
template<typename PropertyType>
inline void AdjustProperty(Property::Value& currentPropertyValue, const Property::Value& relativePropertyValue)
{
  PropertyType currentValue;
  PropertyType relativeValue;
  if(currentPropertyValue.Get(currentValue) && relativePropertyValue.Get(relativeValue))
  {
    currentPropertyValue = currentValue + relativeValue;
  }
}

/// Helper to convert and then assign the property value
template<typename PropertyType, typename Value>
inline void SetValue(const Property::Value& propertyValue, Value& value)
{
  PropertyType convertedValue;
  if(propertyValue.Get(convertedValue))
  {
    value = convertedValue;
  }
}

/// Helper to check container property type and set appropriately
template<typename ContainerType>
inline void SetContainerValue(const ContainerType* containerPtr, Property::Value& value)
{
  if(containerPtr)
  {
    value = *containerPtr;
  }
}
} // unnamed namespace

void PropertyMetadata::SetPropertyValue(const Property::Value& propertyValue)
{
  switch(GetType())
  {
    case Property::NONE:
    {
      // NOOP
      break;
    }

    case Property::RECTANGLE:
    {
      SetValue<Rect<int32_t>>(propertyValue, value);
      break;
    }

    case Property::STRING:
    {
      SetValue<std::string>(propertyValue, value);
      break;
    }

    case Property::ARRAY:
    {
      SetContainerValue(propertyValue.GetArray(), value);
      break;
    }

    case Property::MAP:
    {
      SetContainerValue(propertyValue.GetMap(), value);
      break;
    }

    case Property::EXTENTS:
    {
      SetValue<Extents>(propertyValue, value);
      break;
    }

    case Property::BOOLEAN:
    {
      SetValue<bool>(propertyValue, value);
      break;
    }

    case Property::INTEGER:
    {
      SetValue<int32_t>(propertyValue, value);
      break;
    }

    case Property::FLOAT:
    {
      SetValue<float>(propertyValue, value);
      break;
    }

    case Property::ROTATION:
    {
      SetValue<Quaternion>(propertyValue, value);
      break;
    }

    case Property::MATRIX:
    {
      SetValue<Matrix>(propertyValue, value);
      break;
    }

    case Property::MATRIX3:
    {
      SetValue<Matrix3>(propertyValue, value);
      break;
    }

    case Property::VECTOR2:
    {
      Vector2 vector2Value;
      value.Get(vector2Value);

      switch(componentIndex)
      {
        case 0:
        {
          SetValue<float>(propertyValue, vector2Value.x);
          break;
        }

        case 1:
        {
          SetValue<float>(propertyValue, vector2Value.y);
          break;
        }

        default:
        {
          SetValue<Vector2>(propertyValue, vector2Value);
          break;
        }
      }

      value = vector2Value;
      break;
    }

    case Property::VECTOR3:
    {
      Vector3 vector3Value;
      value.Get(vector3Value);

      switch(componentIndex)
      {
        case 0:
        {
          SetValue<float>(propertyValue, vector3Value.x);
          break;
        }

        case 1:
        {
          SetValue<float>(propertyValue, vector3Value.y);
          break;
        }

        case 2:
        {
          SetValue<float>(propertyValue, vector3Value.z);
          break;
        }

        default:
        {
          SetValue<Vector3>(propertyValue, vector3Value);
          break;
        }
      }

      value = vector3Value;
      break;
    }

    case Property::VECTOR4:
    {
      Vector4 vector4Value;
      value.Get(vector4Value);

      switch(componentIndex)
      {
        case 0:
        {
          SetValue<float>(propertyValue, vector4Value.x);
          break;
        }

        case 1:
        {
          SetValue<float>(propertyValue, vector4Value.y);
          break;
        }

        case 2:
        {
          SetValue<float>(propertyValue, vector4Value.z);
          break;
        }

        case 3:
        {
          SetValue<float>(propertyValue, vector4Value.w);
          break;
        }

        default:
        {
          SetValue<Vector4>(propertyValue, vector4Value);
          break;
        }
      }

      value = vector4Value;
      break;
    }
  }
}

Property::Value PropertyMetadata::GetPropertyValue() const
{
  Property::Value propertyValue;

  if(!IsAnimatable())
  {
    propertyValue = value;
  }
  else
  {
    switch(GetType())
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
        value.Get(vector2Value);

        if(componentIndex == 0)
        {
          propertyValue = vector2Value.x;
        }
        else if(componentIndex == 1)
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
        value.Get(vector3Value);

        if(componentIndex == 0)
        {
          propertyValue = vector3Value.x;
        }
        else if(componentIndex == 1)
        {
          propertyValue = vector3Value.y;
        }
        else if(componentIndex == 2)
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
        value.Get(vector4Value);

        if(componentIndex == 0)
        {
          propertyValue = vector4Value.x;
        }
        else if(componentIndex == 1)
        {
          propertyValue = vector4Value.y;
        }
        else if(componentIndex == 2)
        {
          propertyValue = vector4Value.z;
        }
        else if(componentIndex == 3)
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

void PropertyMetadata::AdjustPropertyValueBy(const Property::Value& relativePropertyValue)
{
  switch(GetType())
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
      bool currentValue  = false;
      bool relativeValue = false;
      if(value.Get(currentValue) && relativePropertyValue.Get(relativeValue))
      {
        value = currentValue || relativeValue;
      }
      break;
    }

    case Property::INTEGER:
    {
      AdjustProperty<int>(value, relativePropertyValue);
      break;
    }

    case Property::FLOAT:
    {
      AdjustProperty<float>(value, relativePropertyValue);
      break;
    }

    case Property::ROTATION:
    {
      Quaternion currentValue;
      Quaternion relativeValue;
      if(value.Get(currentValue) && relativePropertyValue.Get(relativeValue))
      {
        value = currentValue * relativeValue;
      }
      break;
    }

    case Property::VECTOR2:
    {
      if(componentIndex == Property::INVALID_COMPONENT_INDEX)
      {
        AdjustProperty<Vector2>(value, relativePropertyValue);
      }
      else
      {
        Vector2 vector2Value;
        value.Get(vector2Value);

        if(componentIndex == 0)
        {
          vector2Value.x += relativePropertyValue.Get<float>();
        }
        else if(componentIndex == 1)
        {
          vector2Value.y += relativePropertyValue.Get<float>();
        }

        value = vector2Value;
      }

      break;
    }

    case Property::VECTOR3:
    {
      if(componentIndex == Property::INVALID_COMPONENT_INDEX)
      {
        AdjustProperty<Vector3>(value, relativePropertyValue);
      }
      else
      {
        Vector3 vector3Value;
        value.Get(vector3Value);

        if(componentIndex == 0)
        {
          vector3Value.x += relativePropertyValue.Get<float>();
        }
        else if(componentIndex == 1)
        {
          vector3Value.y += relativePropertyValue.Get<float>();
        }
        else if(componentIndex == 2)
        {
          vector3Value.z += relativePropertyValue.Get<float>();
        }

        value = vector3Value;
      }
      break;
    }

    case Property::VECTOR4:
    {
      if(componentIndex == Property::INVALID_COMPONENT_INDEX)
      {
        AdjustProperty<Vector4>(value, relativePropertyValue);
      }
      else
      {
        Vector4 vector4Value;
        value.Get(vector4Value);

        if(componentIndex == 0)
        {
          vector4Value.x += relativePropertyValue.Get<float>();
        }
        else if(componentIndex == 1)
        {
          vector4Value.y += relativePropertyValue.Get<float>();
        }
        else if(componentIndex == 2)
        {
          vector4Value.z += relativePropertyValue.Get<float>();
        }
        else if(componentIndex == 3)
        {
          vector4Value.w += relativePropertyValue.Get<float>();
        }

        value = vector4Value;
      }
      break;
    }
  }
}

} // namespace Internal

} // namespace Dali
