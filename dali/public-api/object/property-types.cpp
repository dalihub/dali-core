//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/public-api/object/property-types.h>

namespace Dali DALI_EXPORT_API
{

namespace PropertyTypes
{

static const char* PROPERTY_TYPE_NAMES[Property::TYPE_COUNT] =
{
  "NONE",
  "BOOLEAN",
  "FLOAT",
  "INTEGER",
  "UNSIGNED_INTEGER",
  "VECTOR2",
  "VECTOR3",
  "VECTOR4",
  "MATRIX3",
  "MATRIX",
  "RECTANGLE",
  "ROTATION",
  "STRING",
  "ARRAY",
  "MAP",
};

const char* GetName(Property::Type type)
{
  if (type < Property::TYPE_COUNT)
  {
    return PROPERTY_TYPE_NAMES[type];
  }

  return PROPERTY_TYPE_NAMES[Property::NONE];
}

template <typename T> Property::Type Get()             { return Property::NONE;  }
template <>           Property::Type Get<bool>()       { return Property::BOOLEAN;  }
template <>           Property::Type Get<float>()      { return Property::FLOAT;    }
template <>           Property::Type Get<int>()        { return Property::INTEGER;  }
template <>           Property::Type Get<unsigned int>(){ return Property::UNSIGNED_INTEGER;  }
template <>           Property::Type Get<Vector2>()    { return Property::VECTOR2;  }
template <>           Property::Type Get<Vector3>()    { return Property::VECTOR3;  }
template <>           Property::Type Get<Vector4>()    { return Property::VECTOR4;  }
template <>           Property::Type Get<Matrix3>()    { return Property::MATRIX3;  }
template <>           Property::Type Get<Matrix>()     { return Property::MATRIX;  }
template <>           Property::Type Get<AngleAxis>()  { return Property::ROTATION; } // Rotation has two representations
template <>           Property::Type Get<Quaternion>() { return Property::ROTATION; } // Rotation has two representations
template <>           Property::Type Get<std::string>(){ return Property::STRING; }
template <>           Property::Type Get<Dali::Rect<int> >(){ return Property::RECTANGLE; }
template <>           Property::Type Get<Property::Map>() { return Property::MAP; }
template <>           Property::Type Get<Property::Array>() { return Property::ARRAY; }


}; // namespace PropertyTypes

} // namespace Dali
