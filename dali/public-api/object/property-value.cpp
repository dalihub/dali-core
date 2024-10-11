/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <algorithm>
#include <cstring>
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

/**
 * As the Implementation is bit complex because of Small Buffer Optimization (SBO)
 * In the Property::Value class and Tagged Union implementation in the Impl class.
 * Here is the brief description of the motivation and internal of this implementation.
 *
 * The main motivation is to keep the value semantic without doing any heap allocation.
 * 1. In the public class we keep a aligned buffer of 16byte instead of keeping the Pimpl pointer.
 * 2. we create the Impl object inplace in that buffer to avoid the memory allocation.
 * 3. we don't store the Impl* along with the SBO storage to save 8byte from the Property::Value object.
 * 4. Every time we need to access  Impl object we access it through Read() and Write() which retrieves
 *    the already constructed Impl object from the SBO storage.
 * 5. with SBO , the move and assignment operator are tricky , so we use std::copy to move the object
 *    and update the moved from object to NONE to keep it in a safe state.
 * 6. In the Impl class, we keep a Uninitialized Union and manage lifecycle of objects by doing construct/destroy manually.
 * 7. We keep the Type information along with the Object to keep the size as 16byte (using common initial sequence(CIS) ).
 */

namespace Dali
{
struct Property::Value::Impl
{
  template<typename... Args>
  static Impl* New(Property::Value::Storage& buffer, Args... args)
  {
    return new(&buffer) Impl(std::forward<Args>(args)...);
  }

  static void Delete(Property::Value::Impl& impl)
  {
    impl.~Impl();
  }

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

  const Property::Map* GetMapPtr() const
  {
    return &(mData.mMap.member);
  }

  Property::Array* GetArrayPtr()
  {
    return &(mData.mArray.member);
  }

  const Property::Array* GetArrayPtr() const
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
        if(isSameType)
        {
          mData.mArray.member = other.GetArray();
        }
        else
        {
          auto obj = other.GetArray();
          ConstructInplace(mData.mArray.member, std::move(obj));
        }
        break;
      }
      case Property::MAP:
      {
        if(isSameType)
        {
          mData.mMap.member = other.GetMap();
        }
        else
        {
          auto obj = other.GetMap();
          ConstructInplace(mData.mMap.member, std::move(obj));
        }
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

  bool operator==(const Impl& other) const
  {
    const bool isSameType = GetType() == other.GetType();

    if(!isSameType)
    {
      // We don't support to compare with different type.
      return false;
    }

    switch(GetType())
    {
      case Property::NONE:
      {
        return true;
      }
      case Property::BOOLEAN:
      {
        return mData.mBool.member == other.mData.mBool.member;
      }
      case Property::FLOAT:
      {
        return Equals(mData.mFloat.member, other.mData.mFloat.member);
      }
      case Property::INTEGER:
      {
        return mData.mInt.member == other.mData.mInt.member;
      }
      case Property::VECTOR2:
      {
        return mData.mVector2.member == other.mData.mVector2.member;
      }
      case Property::VECTOR3:
      {
        return mData.mVector3.member == other.mData.mVector3.member;
      }
      case Property::VECTOR4:
      {
        return *mData.mVector4.member == *other.mData.mVector4.member;
      }
      case Property::MATRIX3:
      {
        return *mData.mMatrix3.member == *other.mData.mMatrix3.member;
      }
      case Property::MATRIX:
      {
        return *mData.mMatrix.member == *other.mData.mMatrix.member;
      }
      case Property::RECTANGLE:
      {
        return *mData.mRect.member == *other.mData.mRect.member;
      }
      case Property::ROTATION:
      {
        return *mData.mAngleAxis.member == *other.mData.mAngleAxis.member;
      }
      case Property::STRING:
      {
        return *mData.mString.member == *other.mData.mString.member;
      }
      case Property::EXTENTS:
      {
        return mData.mExtents.member == other.mData.mExtents.member;
      }
      case Property::ARRAY:
      case Property::MAP:
      {
        // TODO : Need to support this case
        return false;
      }
    }
    return false;
  }

  void SetType(Type typeValue)
  {
    mData.mType.type = typeValue;
  }

  bool ConvertType(const Property::Type targetType)
  {
    bool                 converted = false;
    const Property::Type inputType = mData.mType.type;

    if(inputType == targetType)
    {
      // We don't need conversion for same type.
      return true;
    }

    switch(inputType)
    {
      // Try to convert only for scalar types
      case Property::BOOLEAN:
      {
        bool value = mData.mBool.member;
        switch(targetType)
        {
          case Property::FLOAT:
          {
            SetType(targetType);
            mData.mFloat.member = static_cast<float>(value);
            converted           = true;
            break;
          }
          case Property::INTEGER:
          {
            SetType(targetType);
            mData.mInt.member = static_cast<int32_t>(value);
            converted         = true;
            break;
          }
          default:
          {
            break;
          }
        }
        break;
      }
      case Property::FLOAT:
      {
        float value = mData.mFloat.member;
        switch(targetType)
        {
          case Property::BOOLEAN:
          {
            SetType(targetType);
            mData.mBool.member = static_cast<bool>(!Dali::EqualsZero(value));
            converted          = true;
            break;
          }
          case Property::INTEGER:
          {
            SetType(targetType);
            mData.mInt.member = static_cast<int32_t>(value);
            converted         = true;
            break;
          }
          default:
          {
            break;
          }
        }
        break;
      }
      case Property::INTEGER:
      {
        int32_t value = mData.mInt.member;
        switch(targetType)
        {
          case Property::BOOLEAN:
          {
            SetType(targetType);
            mData.mBool.member = static_cast<bool>(value);
            converted          = true;
            break;
          }
          case Property::FLOAT:
          {
            SetType(targetType);
            mData.mFloat.member = static_cast<float>(value);
            converted           = true;
            break;
          }
          default:
          {
            break;
          }
        }
        break;
      }
      default:
      {
        break;
      }
    }
    return converted;
  }

private:
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

/*
 * Safe way to read an already constructed Object from a buffer.
 */
const Property::Value::Impl& Property::Value::Read() const
{
  return *(std::launder(reinterpret_cast<const Property::Value::Impl*>(mStorage.buffer)));
}

Property::Value::Impl& Property::Value::Write()
{
  return *(std::launder(reinterpret_cast<Property::Value::Impl*>(mStorage.buffer)));
}

Property::Value::Value()
{
  Impl::New(mStorage);

  static_assert(sizeof(Value) == 16);
  static_assert(alignof(Value) == alignof(Value*));
}

Property::Value::Value(bool booleanValue)
{
  Impl::New(mStorage, booleanValue);
}

Property::Value::Value(float floatValue)
{
  Impl::New(mStorage, floatValue);
}

Property::Value::Value(int32_t integerValue)
{
  Impl::New(mStorage, integerValue);
}

Property::Value::Value(const Vector2& vectorValue)
{
  Impl::New(mStorage, vectorValue);
}

Property::Value::Value(const Vector3& vectorValue)
{
  Impl::New(mStorage, vectorValue);
}

Property::Value::Value(const Vector4& vectorValue)
{
  Impl::New(mStorage, vectorValue);
}

Property::Value::Value(const Matrix3& matrixValue)
{
  Impl::New(mStorage, matrixValue);
}

Property::Value::Value(const Matrix& matrixValue)
{
  Impl::New(mStorage, matrixValue);
}

Property::Value::Value(const Rect<int32_t>& rectValue)
{
  Impl::New(mStorage, rectValue);
}

Property::Value::Value(const Rect<float>& rectValue)
{
  Impl::New(mStorage, Vector4(rectValue.x, rectValue.y, rectValue.width, rectValue.height));
}

Property::Value::Value(const AngleAxis& angleAxisValue)
{
  Impl::New(mStorage, angleAxisValue);
}

Property::Value::Value(const Quaternion& quaternionValue)
{
  AngleAxis angleAxisValue;
  quaternionValue.ToAxisAngle(angleAxisValue.axis, angleAxisValue.angle);
  Impl::New(mStorage, std::move(angleAxisValue));
}

Property::Value::Value(std::string stringValue)
{
  Impl::New(mStorage, std::move(stringValue));
}

Property::Value::Value(const char* stringValue)
{
  if(stringValue) // string constructor is undefined with nullptr
  {
    Impl::New(mStorage, std::string(stringValue));
  }
  else
  {
    Impl::New(mStorage, std::string());
  }
}

Property::Value::Value(Property::Array arrayValue)
{
  Impl::New(mStorage, std::move(arrayValue));
}

Property::Value::Value(Property::Map mapValue)
{
  Impl::New(mStorage, std::move(mapValue));
}

Property::Value::Value(const Extents& extentsValue)
{
  Impl::New(mStorage, extentsValue);
}

Property::Value::Value(const std::initializer_list<KeyValuePair>& values)
{
  Impl::New(mStorage, Property::Map(values));
}

Property::Value::Value(Type type)
{
  switch(type)
  {
    case Property::BOOLEAN:
    {
      Impl::New(mStorage, false);
      break;
    }
    case Property::FLOAT:
    {
      Impl::New(mStorage, 0.f);
      break;
    }
    case Property::INTEGER:
    {
      Impl::New(mStorage, 0);
      break;
    }
    case Property::VECTOR2:
    {
      Impl::New(mStorage, Vector2::ZERO);
      break;
    }
    case Property::VECTOR3:
    {
      Impl::New(mStorage, Vector3::ZERO);
      break;
    }
    case Property::VECTOR4:
    {
      Impl::New(mStorage, Vector4::ZERO);
      break;
    }
    case Property::RECTANGLE:
    {
      Impl::New(mStorage, Rect<int32_t>());
      break;
    }
    case Property::ROTATION:
    {
      Impl::New(mStorage, AngleAxis());
      break;
    }
    case Property::STRING:
    {
      Impl::New(mStorage, std::string());
      break;
    }
    case Property::MATRIX:
    {
      Impl::New(mStorage, Matrix());
      break;
    }
    case Property::MATRIX3:
    {
      Impl::New(mStorage, Matrix3());
      break;
    }
    case Property::ARRAY:
    {
      Impl::New(mStorage, Property::Array());
      break;
    }
    case Property::MAP:
    {
      Impl::New(mStorage, Property::Map());
      break;
    }
    case Property::EXTENTS:
    {
      Impl::New(mStorage, Extents());
      break;
    }
    case Property::NONE:
    {
      Impl::New(mStorage);
      break;
    }
  }
}

Property::Value::Value(const Property::Value& value)
{
  Impl::New(mStorage);

  // reuse assignment operator
  operator=(value);
}

Property::Value::Value(Property::Value&& value) noexcept
{
  // steal the Impl object by doing a copy.
  std::copy(std::begin(value.mStorage.buffer), std::end(value.mStorage.buffer), std::begin(mStorage.buffer));

  // update the moved from object to NONE state.
  value.Write().SetType(Property::NONE);
}

Property::Value& Property::Value::operator=(const Property::Value& value)
{
  if(this == &value)
  {
    // skip self assignment
    return *this;
  }

  // this will call the Impl operator=()
  Write() = value.Read();

  return *this;
}

Property::Value& Property::Value::operator=(Property::Value&& value) noexcept
{
  if(this != &value)
  {
    // delete the existing Impl object
    Impl::Delete(Write());

    // steal the Impl object by doing a copy.
    std::copy(std::begin(value.mStorage.buffer), std::end(value.mStorage.buffer), std::begin(mStorage.buffer));

    // update the moved from object to NONE state.
    value.Write().SetType(Property::NONE);
  }

  return *this;
}

bool Property::Value::operator==(const Property::Value& rhs) const
{
  // this will call the Impl operator==()
  return Read() == rhs.Read();
}

Property::Value::~Value()
{
  Impl::Delete(Write());
}

Property::Type Property::Value::GetType() const
{
  return Read().GetType();
}

bool Property::Value::ConvertType(const Property::Type targetType)
{
  return Write().ConvertType(targetType);
}

bool Property::Value::Get(bool& booleanValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == BOOLEAN)
  {
    booleanValue = obj.GetBool();
    converted    = true;
  }
  else if(obj.GetType() == INTEGER)
  {
    booleanValue = obj.GetInt();
    converted    = true;
  }

  return converted;
}

bool Property::Value::Get(float& floatValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == FLOAT)
  {
    floatValue = obj.GetFloat();
    converted  = true;
  }
  else if(obj.GetType() == BOOLEAN)
  {
    floatValue = static_cast<float>(obj.GetBool());
    converted  = true;
  }
  else if(obj.GetType() == INTEGER)
  {
    floatValue = static_cast<float>(obj.GetInt());
    converted  = true;
  }

  return converted;
}

bool Property::Value::Get(int32_t& integerValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == INTEGER)
  {
    integerValue = obj.GetInt();
    converted    = true;
  }
  else if(obj.GetType() == BOOLEAN)
  {
    integerValue = obj.GetBool();
    converted    = true;
  }
  else if(obj.GetType() == FLOAT)
  {
    integerValue = static_cast<int32_t>(obj.GetFloat());
    converted    = true;
  }

  return converted;
}

bool Property::Value::Get(Vector2& vectorValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == VECTOR4)
  {
    vectorValue = obj.GetVector4();
    converted   = true;
  }
  else if(obj.GetType() == VECTOR2)
  {
    vectorValue = obj.GetVector2();
    converted   = true;
  }
  else if(obj.GetType() == VECTOR3)
  {
    vectorValue = obj.GetVector3();
    converted   = true;
  }

  return converted;
}

bool Property::Value::Get(Vector3& vectorValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == VECTOR4)
  {
    vectorValue = obj.GetVector4();
    converted   = true;
  }
  else if(obj.GetType() == VECTOR2)
  {
    vectorValue = obj.GetVector2();
    converted   = true;
  }
  else if(obj.GetType() == VECTOR3)
  {
    vectorValue = obj.GetVector3();
    converted   = true;
  }

  return converted;
}

bool Property::Value::Get(Vector4& vectorValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == VECTOR4)
  {
    vectorValue = obj.GetVector4();
    converted   = true;
  }
  else if(obj.GetType() == VECTOR2)
  {
    vectorValue = obj.GetVector2();
    converted   = true;
  }
  else if(obj.GetType() == VECTOR3)
  {
    vectorValue = obj.GetVector3();
    converted   = true;
  }

  return converted;
}

bool Property::Value::Get(Matrix3& matrixValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == MATRIX3)
  {
    matrixValue = obj.GetMatrix3();
    converted   = true;
  }
  return converted;
}

bool Property::Value::Get(Matrix& matrixValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == MATRIX)
  {
    matrixValue = obj.GetMatrix();
    converted   = true;
  }
  return converted;
}

bool Property::Value::Get(Rect<int32_t>& rectValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == RECTANGLE)
  {
    rectValue = obj.GetRect();
    converted = true;
  }
  return converted;
}

bool Property::Value::Get(AngleAxis& angleAxisValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == ROTATION)
  {
    angleAxisValue = obj.GetAngleAxis();
    converted      = true;
  }
  return converted;
}

bool Property::Value::Get(Quaternion& quaternionValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == ROTATION)
  {
    auto& angleAxis = obj.GetAngleAxis();
    quaternionValue = Quaternion(angleAxis.angle, angleAxis.axis);
    converted       = true;
  }
  return converted;
}

bool Property::Value::Get(std::string& stringValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == STRING)
  {
    stringValue.assign(obj.GetString());
    converted = true;
  }
  return converted;
}

bool Property::Value::Get(Property::Array& arrayValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == ARRAY)
  {
    arrayValue = obj.GetArray();
    converted  = true;
  }
  return converted;
}

bool Property::Value::Get(Property::Map& mapValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == MAP)
  {
    mapValue  = obj.GetMap();
    converted = true;
  }
  return converted;
}

Property::Array const* Property::Value::GetArray() const
{
  const auto& obj = Read();

  if(obj.GetType() == ARRAY)
  {
    return obj.GetArrayPtr();
  }
  return nullptr;
}

Property::Array* Property::Value::GetArray()
{
  auto& obj = Write();

  if(obj.GetType() == ARRAY)
  {
    return obj.GetArrayPtr();
  }
  return nullptr;
}

Property::Map const* Property::Value::GetMap() const
{
  const auto& obj = Read();

  if(obj.GetType() == MAP)
  {
    return obj.GetMapPtr();
  }
  return nullptr;
}

Property::Map* Property::Value::GetMap()
{
  auto& obj = Write();

  if(obj.GetType() == MAP)
  {
    return obj.GetMapPtr();
  }
  return nullptr;
}

bool Property::Value::Get(Extents& extentsValue) const
{
  bool converted = false;

  const auto& obj = Read();

  if(obj.GetType() == EXTENTS)
  {
    extentsValue = obj.GetExtents();
    converted    = true;
  }
  else if(obj.GetType() == VECTOR4)
  {
    auto& vec4          = obj.GetVector4();
    extentsValue.start  = static_cast<uint16_t>(vec4.x);
    extentsValue.end    = static_cast<uint16_t>(vec4.y);
    extentsValue.top    = static_cast<uint16_t>(vec4.z);
    extentsValue.bottom = static_cast<uint16_t>(vec4.w);
    converted           = true;
  }

  return converted;
}

std::ostream& operator<<(std::ostream& stream, const Property::Value& value)
{
  const auto& obj = value.Read();

  switch(obj.GetType())
  {
    case Dali::Property::BOOLEAN:
    {
      stream << obj.GetBool();
      break;
    }
    case Dali::Property::FLOAT:
    {
      stream << obj.GetFloat();
      break;
    }
    case Dali::Property::INTEGER:
    {
      stream << obj.GetInt();
      break;
    }
    case Dali::Property::VECTOR2:
    {
      stream << obj.GetVector2();
      break;
    }
    case Dali::Property::VECTOR3:
    {
      stream << obj.GetVector3();
      break;
    }
    case Dali::Property::VECTOR4:
    {
      stream << obj.GetVector4();
      break;
    }
    case Dali::Property::MATRIX3:
    {
      stream << obj.GetMatrix3();
      break;
    }
    case Dali::Property::MATRIX:
    {
      stream << obj.GetMatrix();
      break;
    }
    case Dali::Property::RECTANGLE:
    {
      stream << obj.GetRect();
      break;
    }
    case Dali::Property::ROTATION:
    {
      stream << obj.GetAngleAxis();
      break;
    }
    case Dali::Property::STRING:
    {
      stream << obj.GetString();
      break;
    }
    case Dali::Property::ARRAY:
    {
      stream << obj.GetArray();
      break;
    }
    case Dali::Property::MAP:
    {
      stream << obj.GetMap();
      break;
    }
    case Dali::Property::EXTENTS:
    {
      stream << obj.GetExtents();
      break;
    }
    case Dali::Property::NONE:
    {
      stream << "undefined type";
    }
  }
  return stream;
}

} // namespace Dali
