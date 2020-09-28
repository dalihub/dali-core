/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-value.h>

// EXTERNAL INCLUDES
#include <ostream>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/math/angle-axis.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-types.h>

namespace Dali
{

struct Property::Value::Impl
{
  ~Impl()
  {
    // Destroy the current object stored in Data union memory.
    Destroy();
  }

  Impl(const Impl&) = delete;

  Impl(Impl&&) = delete;

  Impl& operator=(Impl&&) = delete;

  Impl()
  {
    static_assert(sizeof(Impl) == 16);
    static_assert(alignof(Impl) == alignof(Impl*));

    SetType(Property::NONE);
  }

  Impl(bool booleanValue)
  {
    SetType(Property::BOOLEAN);
    mData.mBool.member = booleanValue;
  }

  Impl(float floatValue)
  {
    SetType(Property::FLOAT);
    mData.mFloat.member = floatValue;
  }

  Impl(int32_t integerValue)
  {
    SetType(Property::INTEGER);
    mData.mInt.member = integerValue;
  }

  Impl(Vector2 vectorValue)
  {
    SetType(Property::VECTOR2);
    ConstructInplace(mData.mVector2.member, std::move(vectorValue));
  }

  Impl(Vector3 vectorValue)
  {
    SetType(Property::VECTOR3);
    ConstructInplace(mData.mVector3.member, std::move(vectorValue));
  }

  Impl(Extents extentsValue)
  {
    SetType(Property::EXTENTS);
    ConstructInplace(mData.mExtents.member, std::move(extentsValue));
  }

  Impl(Property::Map mapValue)
  {
    SetType(Property::MAP);
    ConstructInplace(mData.mMap.member, std::move(mapValue));
  }

  Impl(Property::Array arrayValue)
  {
    SetType(Property::ARRAY);
    ConstructInplace(mData.mArray.member, std::move(arrayValue));
  }

  Impl(std::string stringValue)
  {
    SetType(Property::STRING);
    mData.mString.member = new std::string(std::move(stringValue));
  }

  Impl(Rect<int32_t> rectValue)
  {
    SetType(Property::RECTANGLE);
    mData.mRect.member = new Rect<int32_t>(std::move(rectValue));
  }

  Impl(Vector4 vectorValue)
  {
    SetType(Property::VECTOR4);
    mData.mVector4.member = new Vector4(std::move(vectorValue));
  }

  Impl(Matrix3 matrixValue)
  {
    SetType(Property::MATRIX3);
    mData.mMatrix3.member = new Matrix3(std::move(matrixValue));
  }

  Impl(Matrix matrixValue)
  {
    SetType(Property::MATRIX);
    mData.mMatrix.member = new Matrix(std::move(matrixValue));
  }

  Impl(AngleAxis angleAxisValue)
  {
    SetType(Property::ROTATION);
    mData.mAngleAxis.member = new AngleAxis(std::move(angleAxisValue));
  }

  Type GetType() const
  {
    return mData.mType.type;
  }

  bool GetBool() const
  {
    return mData.mBool.member;
  }

  int32_t GetInt() const
  {
    return mData.mInt.member;
  }

  float GetFloat() const
  {
    return mData.mFloat.member;
  }

  const Extents& GetExtents() const
  {
    return mData.mExtents.member;
  }

  const Vector2& GetVector2() const
  {
    return mData.mVector2.member;
  }

  const Vector3& GetVector3() const
  {
    return mData.mVector3.member;
  }

  const Property::Map& GetMap() const
  {
    return mData.mMap.member;
  }

  const Property::Array& GetArray() const
  {
    return mData.mArray.member;
  }

  const Vector4& GetVector4() const
  {
    return *(mData.mVector4.member);
  }

  const Matrix3& GetMatrix3() const
  {
    return *(mData.mMatrix3.member);
  }

  const Matrix& GetMatrix() const
  {
    return *(mData.mMatrix.member);
  }

  const AngleAxis& GetAngleAxis() const
  {
    return *(mData.mAngleAxis.member);
  }

  const std::string& GetString() const
  {
    return *(mData.mString.member);
  }

  const Rect<int32_t>& GetRect() const
  {
    return *(mData.mRect.member);
  }

  Property::Map* GetMapPtr()
  {
    return &(mData.mMap.member);
  }

  Property::Array* GetArrayPtr()
  {
    return &(mData.mArray.member);
  }

  Impl& operator=(const Impl& other)
  {
    const bool isSameType = GetType() == other.GetType();

    if(!isSameType)
    {
      Destroy();
      SetType(other.GetType());
    }

    switch(GetType())
    {
      case Property::NONE:
      {
        break;
      }
      case Property::BOOLEAN:
      {
        mData.mBool.member = other.GetBool();
        break;
      }
      case Property::FLOAT:
      {
        mData.mFloat.member = other.GetFloat();
        break;
      }
      case Property::INTEGER:
      {
        mData.mInt.member = other.GetInt();
        break;
      }
      case Property::EXTENTS:
      {
        auto obj = other.GetExtents();
        ConstructInplace(mData.mExtents.member, std::move(obj));
        break;
      }
      case Property::VECTOR2:
      {
        auto obj = other.GetVector2();
        ConstructInplace(mData.mVector2.member, std::move(obj));
        break;
      }
      case Property::VECTOR3:
      {
        auto obj = other.GetVector3();
        ConstructInplace(mData.mVector3.member, std::move(obj));
        break;
      }
      case Property::ARRAY:
      {
        auto obj = other.GetArray();
        ConstructInplace(mData.mArray.member, std::move(obj));
        break;
      }
      case Property::MAP:
      {
        auto obj = other.GetMap();
        ConstructInplace(mData.mMap.member, std::move(obj));
        break;
      }
      case Property::VECTOR4:
      {
        if(isSameType)
        {
          *mData.mVector4.member = other.GetVector4();
        }
        else
        {
          mData.mVector4.member = new Vector4(other.GetVector4());
        }
        break;
      }
      case Property::MATRIX3:
      {
        if(isSameType)
        {
          *mData.mMatrix3.member = other.GetMatrix3();
        }
        else
        {
          mData.mMatrix3.member = new Matrix3(other.GetMatrix3());
        }
        break;
      }
      case Property::MATRIX:
      {
        if(isSameType)
        {
          *mData.mMatrix.member = other.GetMatrix();
        }
        else
        {
          mData.mMatrix.member = new Matrix(other.GetMatrix());
        }
        break;
      }
      case Property::RECTANGLE:
      {
        if(isSameType)
        {
          *mData.mRect.member = other.GetRect();
        }
        else
        {
          mData.mRect.member = new Rect<int32_t>(other.GetRect());
        }
        break;
      }
      case Property::ROTATION:
      {
        if(isSameType)
        {
          *mData.mAngleAxis.member = other.GetAngleAxis();
        }
        else
        {
          mData.mAngleAxis.member = new AngleAxis(other.GetAngleAxis());
        }
        break;
      }
      case Property::STRING:
      {
        if(isSameType)
        {
          *mData.mString.member = other.GetString();
        }
        else
        {
          mData.mString.member = new std::string(other.GetString());
        }
        break;
      }
    }
    return *this;
  }

private:
  void SetType(Type typeValue)
  {
    mData.mType.type = typeValue;
  }

  /**
   * This helper function takes a typed(Tp) memory location( member)
   * and a object of same type( val ) and move constructs a new object of
   * same type(Tp) in the memory location( member) using placement new.
   * after this function call member location will have a object of type Tp.
   */
  template<typename Tp>
  void ConstructInplace(Tp& member, Tp&& val)
  {
    new(&member) Tp(std::forward<Tp>(val));
  }

  /**
  * Destroy the object created in the Data union memory by probing the
  * type and calling the appropriate destructor.
  * and also reset the type and memory location to reflect that .
  */
  void Destroy()
  {
    switch(GetType())
    {
      case Property::NONE:
      case Property::BOOLEAN:
      case Property::FLOAT:
      case Property::INTEGER:
      {
        break; // nothing to do
      }
      case Property::EXTENTS:
      {
        mData.mExtents.member.~Extents();
        break;
      }
      case Property::VECTOR2:
      {
        mData.mVector2.member.~Vector2();
        break;
      }
      case Property::VECTOR3:
      {
        mData.mVector3.member.~Vector3();
        break;
      }
      case Property::ARRAY:
      {
        using array = Property::Array;
        mData.mArray.member.~array();
        break;
      }
      case Property::MAP:
      {
        using map = Property::Map;
        mData.mMap.member.~map();
        break;
      }
      case Property::VECTOR4:
      {
        delete mData.mVector4.member;
        break;
      }
      case Property::MATRIX3:
      {
        delete mData.mMatrix3.member;
        break;
      }
      case Property::MATRIX:
      {
        delete mData.mMatrix.member;
        break;
      }
      case Property::RECTANGLE:
      {
        delete mData.mRect.member;
        break;
      }
      case Property::ROTATION:
      {
        delete mData.mAngleAxis.member;
        break;
      }
      case Property::STRING:
      {
        delete mData.mString.member;
        break;
      }
    }
  }

  /*
   * This wrapper struct is used for
   * storing Type in every union member
   * and can acess it from non active member
   * of the uninon without invoking UB. this is
   * possible because of CIS(common initial sequence)
   * http://eel.is/c++draft/class.mem#general-25
   */
  template<typename T>
  struct UnionMember
  {
    Type type;
    T    member;
  };

  /**
   * Tagged union implementation.
   *
   * This Data union contains non trivial data
   * types Map and Array, the default constructor
   * and destructors are deleted by the compiler
   * so we provided empty constructor and destructor
   * just to pacify the compiler.
   * The only job of this union to give a typed memory buffer to the
   * Impl class which can construct the appropriate object
   * using placement new.
   * As Impl class explicitly construct the object and keeps track of the
   * object it creates and then destroys them in the ~Impl() this will not leak
   * any memory.
   */
  union Data
  {
    Data()
    {
    }
    ~Data()
    {
    }

    UnionMember<bool>            mBool;
    UnionMember<int32_t>         mInt;
    UnionMember<float>           mFloat;
    UnionMember<Extents>         mExtents;
    UnionMember<Vector2>         mVector2;
    UnionMember<Vector3>         mVector3;
    UnionMember<Property::Map>   mMap;
    UnionMember<Property::Array> mArray;
    UnionMember<Vector4*>        mVector4;
    UnionMember<Matrix3*>        mMatrix3;
    UnionMember<Matrix*>         mMatrix;
    UnionMember<AngleAxis*>      mAngleAxis;
    UnionMember<std::string*>    mString;
    UnionMember<Rect<int32_t>*>  mRect;
    struct
    {
      Type type;
    } mType;
  };

  Data mData;
};

Property::Value::Value()
: mImpl(nullptr)
{
}

Property::Value::Value(bool booleanValue)
: mImpl(new Impl(booleanValue))
{
}

Property::Value::Value(float floatValue)
: mImpl(new Impl(floatValue))
{
}

Property::Value::Value(int32_t integerValue)
: mImpl(new Impl(integerValue))
{
}

Property::Value::Value(const Vector2& vectorValue)
: mImpl(new Impl(vectorValue))
{
}

Property::Value::Value(const Vector3& vectorValue)
: mImpl(new Impl(vectorValue))
{
}

Property::Value::Value(const Vector4& vectorValue)
: mImpl(new Impl(vectorValue))
{
}

Property::Value::Value(const Matrix3& matrixValue)
: mImpl(new Impl(matrixValue))
{
}

Property::Value::Value(const Matrix& matrixValue)
: mImpl(new Impl(matrixValue))
{
}

Property::Value::Value(const Rect<int32_t>& rectValue)
: mImpl(new Impl(rectValue))
{
}

Property::Value::Value(const Rect<float>& rectValue)
: mImpl(new Impl(Vector4(rectValue.x, rectValue.y, rectValue.width, rectValue.height)))
{
}

Property::Value::Value(const AngleAxis& angleAxisValue)
: mImpl(new Impl(angleAxisValue))
{
}

Property::Value::Value(const Quaternion& quaternionValue)
{
  AngleAxis angleAxisValue;
  quaternionValue.ToAxisAngle(angleAxisValue.axis, angleAxisValue.angle);
  mImpl = new Impl(std::move(angleAxisValue));
}

Property::Value::Value(std::string stringValue)
: mImpl(new Impl(std::move(stringValue)))
{
}

Property::Value::Value(const char* stringValue)
: mImpl(nullptr)
{
  if(stringValue) // string constructor is undefined with nullptr
  {
    mImpl = new Impl(std::string(stringValue));
  }
  else
  {
    mImpl = new Impl(std::string());
  }
}

Property::Value::Value(Property::Array arrayValue)
: mImpl(new Impl(std::move(arrayValue)))
{
}

Property::Value::Value(Property::Map mapValue)
: mImpl(new Impl(std::move(mapValue)))
{
}

Property::Value::Value(const Extents& extentsValue)
: mImpl(new Impl(extentsValue))
{
}

Property::Value::Value(const std::initializer_list<KeyValuePair>& values)
: mImpl(new Impl(Property::Map(values)))
{
}

Property::Value::Value(Type type)
: mImpl(nullptr)
{
  switch(type)
  {
    case Property::BOOLEAN:
    {
      mImpl = new Impl(false);
      break;
    }
    case Property::FLOAT:
    {
      mImpl = new Impl(0.f);
      break;
    }
    case Property::INTEGER:
    {
      mImpl = new Impl(0);
      break;
    }
    case Property::VECTOR2:
    {
      mImpl = new Impl(Vector2::ZERO);
      break;
    }
    case Property::VECTOR3:
    {
      mImpl = new Impl(Vector3::ZERO);
      break;
    }
    case Property::VECTOR4:
    {
      mImpl = new Impl(Vector4::ZERO);
      break;
    }
    case Property::RECTANGLE:
    {
      mImpl = new Impl(Rect<int32_t>());
      break;
    }
    case Property::ROTATION:
    {
      mImpl = new Impl(AngleAxis());
      break;
    }
    case Property::STRING:
    {
      mImpl = new Impl(std::string());
      break;
    }
    case Property::MATRIX:
    {
      mImpl = new Impl(Matrix());
      break;
    }
    case Property::MATRIX3:
    {
      mImpl = new Impl(Matrix3());
      break;
    }
    case Property::ARRAY:
    {
      mImpl = new Impl(Property::Array());
      break;
    }
    case Property::MAP:
    {
      mImpl = new Impl(Property::Map());
      break;
    }
    case Property::EXTENTS:
    {
      mImpl = new Impl(Extents());
      break;
    }
    case Property::NONE:
    {
      // No need to create an Impl
      break;
    }
  }
}

Property::Value::Value(const Property::Value& value)
: mImpl(nullptr)
{
  // reuse assignment operator
  operator=(value);
}

Property::Value::Value(Property::Value&& value) noexcept
: mImpl(value.mImpl)
{
  value.mImpl = nullptr;
}

Property::Value& Property::Value::operator=(const Property::Value& value)
{
  if(this == &value)
  {
    // skip self assignment
    return *this;
  }

  if(value.mImpl)
  {
    if(!mImpl)
    {
      mImpl = new Impl();
    }

    *mImpl = *(value.mImpl);
  }
  else
  {
    delete mImpl;
    mImpl = nullptr;
  }

  return *this;
}

Property::Value& Property::Value::operator=(Property::Value&& value) noexcept
{
  if(this != &value)
  {
    delete mImpl;
    mImpl       = value.mImpl;
    value.mImpl = nullptr;
  }

  return *this;
}

Property::Value::~Value()
{
  delete mImpl;
}

Property::Type Property::Value::GetType() const
{
  return mImpl ? mImpl->GetType() : Property::NONE;
}

bool Property::Value::Get(bool& booleanValue) const
{
  bool converted = false;
  if(mImpl)
  {
    if(mImpl->GetType() == BOOLEAN)
    {
      booleanValue = mImpl->GetBool();
      converted    = true;
    }
    else if(mImpl->GetType() == INTEGER)
    {
      booleanValue = mImpl->GetInt();
      converted    = true;
    }
  }
  return converted;
}

bool Property::Value::Get(float& floatValue) const
{
  bool converted = false;
  if(mImpl)
  {
    if(mImpl->GetType() == FLOAT)
    {
      floatValue = mImpl->GetFloat();
      converted  = true;
    }
    else if(mImpl->GetType() == BOOLEAN)
    {
      floatValue = static_cast<float>(mImpl->GetBool());
      converted  = true;
    }
    else if(mImpl->GetType() == INTEGER)
    {
      floatValue = static_cast<float>(mImpl->GetInt());
      converted  = true;
    }
  }
  return converted;
}

bool Property::Value::Get(int32_t& integerValue) const
{
  bool converted = false;
  if(mImpl)
  {
    if(mImpl->GetType() == INTEGER)
    {
      integerValue = mImpl->GetInt();
      converted    = true;
    }
    else if(mImpl->GetType() == BOOLEAN)
    {
      integerValue = mImpl->GetBool();
      converted    = true;
    }
    else if(mImpl->GetType() == FLOAT)
    {
      integerValue = static_cast<int32_t>(mImpl->GetFloat());
      converted    = true;
    }
  }
  return converted;
}

bool Property::Value::Get(Vector2& vectorValue) const
{
  bool converted = false;
  if(mImpl)
  {
    if(mImpl->GetType() == VECTOR4)
    {
      vectorValue = mImpl->GetVector4();
      converted   = true;
    }
    else if(mImpl->GetType() == VECTOR2)
    {
      vectorValue = mImpl->GetVector2();
      converted   = true;
    }
    else if(mImpl->GetType() == VECTOR3)
    {
      vectorValue = mImpl->GetVector3();
      converted   = true;
    }
  }
  return converted;
}

bool Property::Value::Get(Vector3& vectorValue) const
{
  bool converted = false;
  if(mImpl)
  {
    if(mImpl->GetType() == VECTOR4)
    {
      vectorValue = mImpl->GetVector4();
      converted   = true;
    }
    else if(mImpl->GetType() == VECTOR2)
    {
      vectorValue = mImpl->GetVector2();
      converted   = true;
    }
    else if(mImpl->GetType() == VECTOR3)
    {
      vectorValue = mImpl->GetVector3();
      converted   = true;
    }
  }
  return converted;
}

bool Property::Value::Get(Vector4& vectorValue) const
{
  bool converted = false;
  if(mImpl)
  {
    if(mImpl->GetType() == VECTOR4)
    {
      vectorValue = mImpl->GetVector4();
      converted   = true;
    }
    else if(mImpl->GetType() == VECTOR2)
    {
      vectorValue = mImpl->GetVector2();
      converted   = true;
    }
    else if(mImpl->GetType() == VECTOR3)
    {
      vectorValue = mImpl->GetVector3();
      converted   = true;
    }
  }
  return converted;
}

bool Property::Value::Get(Matrix3& matrixValue) const
{
  bool converted = false;
  if(mImpl && (mImpl->GetType() == MATRIX3))
  {
    matrixValue = mImpl->GetMatrix3();
    converted   = true;
  }
  return converted;
}

bool Property::Value::Get(Matrix& matrixValue) const
{
  bool converted = false;
  if(mImpl && (mImpl->GetType() == MATRIX))
  {
    matrixValue = mImpl->GetMatrix();
    converted   = true;
  }
  return converted;
}

bool Property::Value::Get(Rect<int32_t>& rectValue) const
{
  bool converted = false;
  if(mImpl && (mImpl->GetType() == RECTANGLE))
  {
    rectValue = mImpl->GetRect();
    converted = true;
  }
  return converted;
}

bool Property::Value::Get(AngleAxis& angleAxisValue) const
{
  bool converted = false;
  if(mImpl && (mImpl->GetType() == ROTATION))
  {
    angleAxisValue = mImpl->GetAngleAxis();
    converted      = true;
  }
  return converted;
}

bool Property::Value::Get(Quaternion& quaternionValue) const
{
  bool converted = false;
  if(mImpl && (mImpl->GetType() == ROTATION))
  {
    auto& obj       = mImpl->GetAngleAxis();
    quaternionValue = Quaternion(obj.angle, obj.axis);
    converted       = true;
  }
  return converted;
}

bool Property::Value::Get(std::string& stringValue) const
{
  bool converted = false;
  if(mImpl && (mImpl->GetType() == STRING))
  {
    stringValue.assign(mImpl->GetString());
    converted = true;
  }
  return converted;
}

bool Property::Value::Get(Property::Array& arrayValue) const
{
  bool converted = false;
  if(mImpl && (mImpl->GetType() == ARRAY))
  {
    arrayValue = mImpl->GetArray();
    converted  = true;
  }
  return converted;
}

bool Property::Value::Get(Property::Map& mapValue) const
{
  bool converted = false;
  if(mImpl && (mImpl->GetType() == MAP))
  {
    mapValue  = mImpl->GetMap();
    converted = true;
  }
  return converted;
}

Property::Array const* Property::Value::GetArray() const
{
  if(mImpl && (mImpl->GetType() == ARRAY))
  {
    return mImpl->GetArrayPtr();
  }
  return nullptr;
}

Property::Array* Property::Value::GetArray()
{
  Property::Array* array = nullptr;
  if(mImpl && (mImpl->GetType() == ARRAY)) // type cannot change in mImpl so array is allocated
  {
    array = mImpl->GetArrayPtr();
  }
  return array;
}

Property::Map const* Property::Value::GetMap() const
{
  Property::Map* map = nullptr;
  if(mImpl && (mImpl->GetType() == MAP)) // type cannot change in mImpl so map is allocated
  {
    map = mImpl->GetMapPtr();
  }
  return map;
}

Property::Map* Property::Value::GetMap()
{
  if(mImpl && (mImpl->GetType() == MAP))
  {
    return mImpl->GetMapPtr();
  }
  return nullptr;
}

bool Property::Value::Get(Extents& extentsValue) const
{
  bool converted = false;
  if(mImpl)
  {
    if(mImpl->GetType() == EXTENTS)
    {
      extentsValue = mImpl->GetExtents();
      converted    = true;
    }
    else if(mImpl->GetType() == VECTOR4)
    {
      auto& obj           = mImpl->GetVector4();
      extentsValue.start  = static_cast<uint16_t>(obj.x);
      extentsValue.end    = static_cast<uint16_t>(obj.y);
      extentsValue.top    = static_cast<uint16_t>(obj.z);
      extentsValue.bottom = static_cast<uint16_t>(obj.w);
      converted           = true;
    }
  }
  return converted;
}

std::ostream& operator<<(std::ostream& stream, const Property::Value& value)
{
  if(value.mImpl)
  {
    auto obj = value.mImpl;

    switch(obj->GetType())
    {
      case Dali::Property::BOOLEAN:
      {
        stream << obj->GetBool();
        break;
      }
      case Dali::Property::FLOAT:
      {
        stream << obj->GetFloat();
        break;
      }
      case Dali::Property::INTEGER:
      {
        stream << obj->GetInt();
        break;
      }
      case Dali::Property::VECTOR2:
      {
        stream << obj->GetVector2();
        break;
      }
      case Dali::Property::VECTOR3:
      {
        stream << obj->GetVector3();
        break;
      }
      case Dali::Property::VECTOR4:
      {
        stream << obj->GetVector4();
        break;
      }
      case Dali::Property::MATRIX3:
      {
        stream << obj->GetMatrix3();
        break;
      }
      case Dali::Property::MATRIX:
      {
        stream << obj->GetMatrix();
        break;
      }
      case Dali::Property::RECTANGLE:
      {
        stream << obj->GetRect();
        break;
      }
      case Dali::Property::ROTATION:
      {
        stream << obj->GetAngleAxis();
        break;
      }
      case Dali::Property::STRING:
      {
        stream << obj->GetString();
        break;
      }
      case Dali::Property::ARRAY:
      {
        stream << obj->GetArray();
        break;
      }
      case Dali::Property::MAP:
      {
        stream << obj->GetMap();
        break;
      }
      case Dali::Property::EXTENTS:
      {
        stream << obj->GetExtents();
        break;
      }
      case Dali::Property::NONE:
      { // mImpl will be a nullptr, there's no way to get to this case
      }
    }
  }
  else
  {
    stream << "undefined type";
  }
  return stream;
}

} // namespace Dali
