/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/any.h>
#include <dali/public-api/math/angle-axis.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-types.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

struct Property::Value::Impl
{
  Impl()
  : mType( Property::NONE )
  {
  }

  Impl(bool boolValue)
  : mType( PropertyTypes::Get<bool>() ),
    mValue( boolValue )
  {
  }

  Impl(float floatValue)
  : mType( PropertyTypes::Get<float>() ),
    mValue( floatValue )
  {
  }

  Impl(int integerValue)
  : mType( PropertyTypes::Get<int>() ),
    mValue( integerValue )
  {
  }

  Impl(unsigned int unsignedIntegerValue)
  : mType( PropertyTypes::Get<unsigned int>() ),
    mValue( unsignedIntegerValue )
  {
  }

  Impl(const Vector2& vectorValue)
  : mType( PropertyTypes::Get<Vector2>() ),
    mValue( vectorValue )
  {
  }

  Impl(const Vector3& vectorValue)
  : mType( PropertyTypes::Get<Vector3>() ),
    mValue( vectorValue )
  {
  }

  Impl(const Vector4& vectorValue)
  : mType( PropertyTypes::Get<Vector4>() ),
    mValue( vectorValue )
  {
  }

  Impl(const Matrix3& matrixValue)
  : mType(PropertyTypes::Get<Matrix3>()),
    mValue(matrixValue)
  {
  }

  Impl(const Matrix& matrixValue)
  : mType(PropertyTypes::Get<Matrix>()),
    mValue(matrixValue)
  {
  }

  Impl(const AngleAxis& angleAxisValue)
  : mType( PropertyTypes::Get<AngleAxis>() ),
    mValue( angleAxisValue )
  {
  }

  Impl(const Quaternion& quaternionValue)
  : mType( PropertyTypes::Get<Quaternion>() ),
    mValue( quaternionValue )
  {
  }

  Impl(const std::string& stringValue)
    : mType( PropertyTypes::Get<std::string>() ),
      mValue( stringValue )
  {
  }

  Impl(const Rect<int>& rect)
    : mType( PropertyTypes::Get<Rect<int> >() ),
      mValue( rect )
  {
  }

  Impl(Property::Map container)
    : mType( PropertyTypes::Get<Property::Map >() ),
      mValue( container )
  {
  }

  Impl(Property::Array container)
    : mType( PropertyTypes::Get<Property::Array >() ),
      mValue( container )
  {
  }

  Type mType;

  typedef Any AnyValue;
  AnyValue mValue;
};

Property::Value::Value()
: mImpl( NULL )
{
  mImpl = new Impl();
}

Property::Value::Value(bool boolValue)
: mImpl( NULL )
{
  mImpl = new Impl( boolValue );
}

Property::Value::Value(float floatValue)
: mImpl( NULL )
{
  mImpl = new Impl( floatValue );
}

Property::Value::Value(int integerValue)
: mImpl( NULL )
{
  mImpl = new Impl( integerValue );
}

Property::Value::Value(unsigned int unsignedIntegerValue)
: mImpl( NULL )
{
  mImpl = new Impl( unsignedIntegerValue );
}

Property::Value::Value(const Vector2& vectorValue)
: mImpl( NULL )
{
  mImpl = new Impl( vectorValue );
}

Property::Value::Value(const Vector3& vectorValue)
: mImpl( NULL )
{
  mImpl = new Impl( vectorValue );
}

Property::Value::Value(const Vector4& vectorValue)
: mImpl( NULL )
{
  mImpl = new Impl( vectorValue );
}

Property::Value::Value(const Matrix3& matrixValue)
: mImpl( NULL )
{
  mImpl = new Impl( matrixValue );
}

Property::Value::Value(const Matrix& matrixValue)
: mImpl( NULL )
{
  mImpl = new Impl( matrixValue );
}

Property::Value::Value(const Rect<int>& rect)
: mImpl( NULL )
{
  mImpl = new Impl( rect );
}

Property::Value::Value(const AngleAxis& angleAxisValue)
: mImpl( NULL )
{
  mImpl = new Impl( angleAxisValue );
}

Property::Value::Value(const Quaternion& quaternionValue)
{
  mImpl = new Impl( quaternionValue );
}

Property::Value::Value(const std::string& stringValue)
{
  mImpl = new Impl( stringValue );
}

Property::Value::Value(const char *stringValue)
{
  mImpl = new Impl( std::string(stringValue) );
}

Property::Value::Value(Property::Array &arrayValue)
{
  mImpl = new Impl( arrayValue );
}

Property::Value::Value(Property::Map &mapValue)
{
  mImpl = new Impl( mapValue );
}


Property::Value::~Value()
{
  delete mImpl;
}

Property::Value::Value(const Value& value)
{
  switch (value.GetType())
  {
    case Property::BOOLEAN:
    {
      mImpl = new Impl( value.Get<bool>() );
      break;
    }

    case Property::FLOAT:
    {
      mImpl = new Impl( value.Get<float>() );
      break;
    }

    case Property::INTEGER:
    {
      mImpl = new Impl( value.Get<int>() );
      break;
    }

    case Property::UNSIGNED_INTEGER:
    {
      mImpl = new Impl( value.Get<unsigned int>() );
      break;
    }

    case Property::VECTOR2:
    {
      mImpl = new Impl( value.Get<Vector2>() );
      break;
    }

    case Property::VECTOR3:
    {
      mImpl = new Impl( value.Get<Vector3>() );
      break;
    }

    case Property::VECTOR4:
    {
      mImpl = new Impl( value.Get<Vector4>() );
      break;
    }

    case Property::RECTANGLE:
    {
      mImpl = new Impl( value.Get<Rect<int> >() );
      break;
    }

    case Property::ROTATION:
    {
      // Orientations have two representations
      DALI_ASSERT_DEBUG( typeid(Quaternion) == value.mImpl->mValue.GetType() ||
                         typeid(AngleAxis)  == value.mImpl->mValue.GetType() );

      if ( typeid(Quaternion) == value.mImpl->mValue.GetType() )
      {
        mImpl = new Impl( value.Get<Quaternion>() );
      }
      else
      {
        mImpl = new Impl( value.Get<AngleAxis>() );
      }
      break;
    }

    case Property::MATRIX3:
    {
      mImpl = new Impl( value.Get<Matrix3>());
      break;
    }

    case Property::MATRIX:
    {
      mImpl = new Impl( value.Get<Matrix>());
      break;
    }

    case Property::STRING:
    {
      mImpl = new Impl( value.Get<std::string>() );
      break;
    }

    case Property::MAP:
    {
      mImpl = new Impl( value.Get<Property::Map>() );
      break;
    }

    case Property::ARRAY:
    {
      mImpl = new Impl( value.Get<Property::Array>() );
      break;
    }

    case Property::NONE: // fall
    default:
    {
      mImpl = new Impl();
      break;
    }
  }
}

Property::Value::Value(Type type)
{
  switch (type)
  {
    case Property::BOOLEAN:
    {
      mImpl = new Impl( false );
      break;
    }

    case Property::FLOAT:
    {
      mImpl = new Impl( 0.f );
      break;
    }

    case Property::INTEGER:
    {
      mImpl = new Impl( 0 );
      break;
    }

    case Property::UNSIGNED_INTEGER:
    {
      mImpl = new Impl( 0U );
      break;
    }

    case Property::VECTOR2:
    {
      mImpl = new Impl( Vector2::ZERO );
      break;
    }

    case Property::VECTOR3:
    {
      mImpl = new Impl( Vector3::ZERO );
      break;
    }

    case Property::VECTOR4:
    {
      mImpl = new Impl( Vector4::ZERO );
      break;
    }

    case Property::RECTANGLE:
    {
      mImpl = new Impl( Rect<int>(0,0,0,0) );
      break;
    }

    case Property::ROTATION:
    {
      mImpl = new Impl( Quaternion( Radian(0.f), Vector3::YAXIS) );
      break;
    }

    case Property::STRING:
    {
      mImpl = new Impl( std::string() );
      break;
    }

    case Property::MAP:
    {
      mImpl = new Impl( Property::Map() );
      break;
    }

    case Property::MATRIX:
    {
      mImpl = new Impl( Matrix() );
      break;
    }

    case Property::MATRIX3:
    {
      mImpl = new Impl( Matrix3() );
      break;
    }

    case Property::ARRAY:
    {
      mImpl = new Impl( Property::Array() );
      break;
    }

    case Property::NONE: // fall
    default:
    {
      mImpl = new Impl();
      break;
    }
  }
}

Property::Value& Property::Value::operator=(const Property::Value& value)
{
  if (this == &value)
  {
    // skip self assignment
    return *this;
  }

  mImpl->mType = value.GetType();

  switch (mImpl->mType)
  {
    case Property::BOOLEAN:
    {
      mImpl->mValue = value.Get<bool>();
      break;
    }

    case Property::FLOAT:
    {
      mImpl->mValue = value.Get<float>();
      break;
    }

    case Property::INTEGER:
    {
      mImpl->mValue = value.Get<int>();
      break;
    }

    case Property::UNSIGNED_INTEGER:
    {
      mImpl->mValue = value.Get<unsigned int>();
      break;
    }

    case Property::VECTOR2:
    {
      mImpl->mValue = value.Get<Vector2>();
      break;
    }

    case Property::VECTOR3:
    {
      mImpl->mValue = value.Get<Vector3>();
      break;
    }

    case Property::VECTOR4:
    {
      mImpl->mValue = value.Get<Vector4>();
      break;
    }

    case Property::RECTANGLE:
    {
      mImpl->mValue = value.Get<Rect<int> >();
      break;
    }

    case Property::ROTATION:
    {
      // Orientations have two representations
      DALI_ASSERT_DEBUG( typeid(Quaternion) == value.mImpl->mValue.GetType() ||
                         typeid(AngleAxis)  == value.mImpl->mValue.GetType() );

      if ( typeid(Quaternion) == value.mImpl->mValue.GetType() )
      {
        mImpl = new Impl( value.Get<Quaternion>() );
      }
      else
      {
        mImpl = new Impl( value.Get<AngleAxis>() );
      }
      break;
    }

    case Property::STRING:
    {
      mImpl->mValue = value.Get<std::string>();
      break;
    }

    case Property::MATRIX:
    {
      mImpl->mValue = value.Get<Matrix>();
      break;
    }

    case Property::MATRIX3:
    {
      mImpl->mValue = value.Get<Matrix3>();
      break;
    }

    case Property::MAP:
    {
      mImpl->mValue = value.Get<Property::Map>();
      break;
    }

    case Property::ARRAY:
    {
      mImpl->mValue = value.Get<Property::Array>();
      break;
    }

    case Property::NONE: // fall
    default:
    {
      mImpl->mValue = Impl::AnyValue(0);
      break;
    }
  }

  return *this;
}

Property::Type Property::Value::GetType() const
{
  return mImpl->mType;
}

void Property::Value::Get(bool& boolValue) const
{
  DALI_ASSERT_DEBUG( Property::BOOLEAN == GetType() && "Property type invalid" );  // AnyCast does asserted type checking

  boolValue = AnyCast<bool>(mImpl->mValue);
}

void Property::Value::Get(float& floatValue) const
{
  DALI_ASSERT_DEBUG( Property::FLOAT == GetType() && "Property type invalid" );

  floatValue = AnyCast<float>(mImpl->mValue);
}

void Property::Value::Get(int& integerValue) const
{
  DALI_ASSERT_DEBUG( Property::INTEGER == GetType() && "Property type invalid" );

  integerValue = AnyCast<int>(mImpl->mValue);
}

void Property::Value::Get(unsigned int& unsignedIntegerValue) const
{
  DALI_ASSERT_DEBUG( Property::UNSIGNED_INTEGER == GetType() && "Property type invalid" );

  unsignedIntegerValue = AnyCast<unsigned int>(mImpl->mValue);
}

void Property::Value::Get(Vector2& vectorValue) const
{
  DALI_ASSERT_DEBUG( Property::VECTOR2 == GetType() && "Property type invalid" );

  vectorValue = AnyCast<Vector2>(mImpl->mValue);
}

void Property::Value::Get(Vector3& vectorValue) const
{
  DALI_ASSERT_DEBUG( Property::VECTOR3 == GetType() && "Property type invalid" );

  vectorValue = AnyCast<Vector3>(mImpl->mValue);
}

void Property::Value::Get(Vector4& vectorValue) const
{
  DALI_ASSERT_DEBUG( Property::VECTOR4 == GetType() && "Property type invalid" );

  vectorValue = AnyCast<Vector4>(mImpl->mValue);
}

void Property::Value::Get(Matrix3& matrixValue) const
{
  DALI_ASSERT_DEBUG( Property::MATRIX3 == GetType() && "Property type invalid" );
  matrixValue = AnyCast<Matrix3>(mImpl->mValue);
}

void Property::Value::Get(Matrix& matrixValue) const
{
  DALI_ASSERT_DEBUG( Property::MATRIX == GetType() && "Property type invalid" );
  matrixValue = AnyCast<Matrix>(mImpl->mValue);
}

void Property::Value::Get(Rect<int>& rect) const
{
  DALI_ASSERT_DEBUG( Property::RECTANGLE == GetType() && "Property type invalid" );

  rect = AnyCast<Rect<int> >(mImpl->mValue);
}

void Property::Value::Get(AngleAxis& angleAxisValue) const
{
  DALI_ASSERT_ALWAYS( Property::ROTATION == GetType() && "Property type invalid" );

  // Orientations have two representations
  DALI_ASSERT_DEBUG( typeid(Quaternion) == mImpl->mValue.GetType() ||
                     typeid(AngleAxis)  == mImpl->mValue.GetType() );

  if ( typeid(Quaternion) == mImpl->mValue.GetType() )
  {
    Quaternion quaternion = AnyCast<Quaternion>(mImpl->mValue);

    quaternion.ToAxisAngle( angleAxisValue.axis, angleAxisValue.angle );
  }
  else
  {
    angleAxisValue = AnyCast<AngleAxis>(mImpl->mValue);
  }
}

void Property::Value::Get(Quaternion& quaternionValue) const
{
  DALI_ASSERT_DEBUG( Property::ROTATION == GetType() && "Property type invalid" );

  // Orientations have two representations
  DALI_ASSERT_DEBUG( typeid(Quaternion) == mImpl->mValue.GetType() ||
               typeid(AngleAxis)  == mImpl->mValue.GetType() );

  if ( typeid(Quaternion) == mImpl->mValue.GetType() )
  {
    quaternionValue = AnyCast<Quaternion>(mImpl->mValue);
  }
  else
  {
    AngleAxis angleAxis = AnyCast<AngleAxis>(mImpl->mValue);

    quaternionValue = Quaternion( Radian(angleAxis.angle), angleAxis.axis );
  }
}

void Property::Value::Get(std::string &out) const
{
  DALI_ASSERT_DEBUG(Property::STRING == GetType() && "Property type invalid");

  out = AnyCast<std::string>(mImpl->mValue);
}

void Property::Value::Get(Property::Array &out) const
{
  DALI_ASSERT_DEBUG(Property::ARRAY == GetType() && "Property type invalid");

  out = AnyCast<Property::Array>(mImpl->mValue);
}

void Property::Value::Get(Property::Map &out) const
{
  DALI_ASSERT_DEBUG(Property::MAP == GetType() && "Property type invalid");

  out = AnyCast<Property::Map>(mImpl->mValue);
}

Property::Value& Property::Value::GetValue(const std::string& key) const
{
  DALI_ASSERT_DEBUG(Property::MAP == GetType() && "Property type invalid");

  Property::Map *container = AnyCast<Property::Map>(&(mImpl->mValue));

  DALI_ASSERT_DEBUG(container);

  if(container)
  {
    Property::Value* value = container->Find( key );
    if ( value )
    {
      return *value;
    }
  }

  DALI_LOG_WARNING("Cannot find property map key %s", key.c_str());
  DALI_ASSERT_ALWAYS(!"Cannot find property map key");
}

bool Property::Value::HasKey(const std::string& key) const
{
  bool has = false;

  if( Property::MAP == GetType() )
  {
    Property::Map *container = AnyCast<Property::Map>(&(mImpl->mValue));

    DALI_ASSERT_DEBUG(container && "Property::Map has no container?");

    if(container)
    {
      Property::Value* value = container->Find( key );
      if ( value )
      {
        has = true;
      }
    }
  }

  return has;
}


const std::string& Property::Value::GetKey(const int index) const
{
  switch( GetType() )
  {
    case Property::MAP:
    {
      Property::Map *container = AnyCast<Property::Map>(&(mImpl->mValue));
      DALI_ASSERT_DEBUG(container && "Property::Map has no container?");
      if(container)
      {
        if(0 <= index && index < static_cast<int>(container->Count()))
        {
          return container->GetKey( index );
        }
      }
    }
    break;
    case Property::NONE:
    case Property::ARRAY:
    case Property::BOOLEAN:
    case Property::FLOAT:
    case Property::UNSIGNED_INTEGER:
    case Property::INTEGER:
    case Property::VECTOR2:
    case Property::VECTOR3:
    case Property::VECTOR4:
    case Property::MATRIX:
    case Property::MATRIX3:
    case Property::RECTANGLE:
    case Property::ROTATION:
    case Property::STRING:
    case Property::TYPE_COUNT:
    {
      break;
    }
  }


  // should never return this
  static std::string null;
  return null;
}


void Property::Value::SetValue(const std::string& key, const Property::Value &value)
{
  DALI_ASSERT_DEBUG(Property::MAP == GetType() && "Property type invalid");

  Property::Map *container = AnyCast<Property::Map>(&(mImpl->mValue));

  if(container)
  {
    (*container)[ key ] = value;
  }
}

Property::Value& Property::Value::GetItem(const int index) const
{
  switch( GetType() )
  {
    case Property::MAP:
    {
      Property::Map *container = AnyCast<Property::Map>(&(mImpl->mValue));

      DALI_ASSERT_DEBUG(container && "Property::Map has no container?");
      if(container)
      {
        DALI_ASSERT_ALWAYS(index < static_cast<int>(container->Count()) && "Property array index invalid");
        DALI_ASSERT_ALWAYS(index >= 0 && "Property array index invalid");

        return container->GetValue( index );
      }
    }
    break;

    case Property::ARRAY:
    {
      Property::Array *container = AnyCast<Property::Array>(&(mImpl->mValue));

      DALI_ASSERT_DEBUG(container && "Property::Map has no container?");
      if(container)
      {
        DALI_ASSERT_ALWAYS(index < static_cast<int>(container->Size()) && "Property array index invalid");
        DALI_ASSERT_ALWAYS(index >= 0 && "Property array index invalid");

        return (*container)[index];
      }
    }
    break;

    case Property::NONE:
    case Property::BOOLEAN:
    case Property::FLOAT:
    case Property::INTEGER:
    case Property::UNSIGNED_INTEGER:
    case Property::VECTOR2:
    case Property::VECTOR3:
    case Property::VECTOR4:
    case Property::MATRIX3:
    case Property::MATRIX:
    case Property::RECTANGLE:
    case Property::ROTATION:
    case Property::STRING:
    case Property::TYPE_COUNT:
    {
      DALI_ASSERT_ALWAYS(!"Cannot GetItem on property Type; not a container");
      break;
    }

  } // switch GetType()


  DALI_ASSERT_ALWAYS(!"Property value index not valid");
}

Property::Value& Property::Value::GetItem(const int index, std::string& key) const
{
  Property::Value& ret( GetItem(index) );

  if( Property::MAP == GetType() )
  {
    Property::Map *container = AnyCast<Property::Map>(&(mImpl->mValue));
    if( container && index < static_cast<int>(container->Count()) )
    {
      key = container->GetKey( index );
    }
  }

  return ret;
}

void Property::Value::SetItem(const int index, const Property::Value &value)
{
  switch( GetType() )
  {
    case Property::MAP:
    {
      Property::Map *container = AnyCast<Property::Map>(&(mImpl->mValue));
      if( container && index < static_cast<int>(container->Count()) )
      {
        Property::Value& indexValue = container->GetValue( index );
        indexValue = value;
      }
    }
    break;

    case Property::ARRAY:
    {
      Property::Array *container = AnyCast<Property::Array>(&(mImpl->mValue));
      if( container && index < static_cast<int>(container->Size()) )
      {
        (*container)[index] = value;
      }
    }
    break;

    case Property::NONE:
    case Property::BOOLEAN:
    case Property::FLOAT:
    case Property::INTEGER:
    case Property::UNSIGNED_INTEGER:
    case Property::VECTOR2:
    case Property::VECTOR3:
    case Property::VECTOR4:
    case Property::MATRIX3:
    case Property::MATRIX:
    case Property::RECTANGLE:
    case Property::ROTATION:
    case Property::STRING:
    case Property::TYPE_COUNT:
    {
      DALI_ASSERT_ALWAYS(!"Cannot SetItem on property Type; not a container");
      break;
    }
  }
}

int Property::Value::AppendItem(const Property::Value &value)
{
  DALI_ASSERT_DEBUG(Property::ARRAY == GetType() && "Property type invalid");

  Property::Array *container = AnyCast<Property::Array>(&(mImpl->mValue));

  if(container)
  {
    container->PushBack(value);
    return container->Size() - 1;
  }
  else
  {
    return -1;
  }

}

int Property::Value::GetSize() const
{
  int ret = 0;

  switch(GetType())
  {
    case Property::MAP:
    {
      Property::Map *container = AnyCast<Property::Map>(&(mImpl->mValue));
      if(container)
      {
        ret = container->Count();
      }
    }
    break;

    case Property::ARRAY:
    {
      Property::Array *container = AnyCast<Property::Array>(&(mImpl->mValue));
      if(container)
      {
        ret = container->Size();
      }
    }
    break;

    case Property::NONE:
    case Property::BOOLEAN:
    case Property::FLOAT:
    case Property::INTEGER:
    case Property::UNSIGNED_INTEGER:
    case Property::VECTOR2:
    case Property::VECTOR3:
    case Property::VECTOR4:
    case Property::MATRIX3:
    case Property::MATRIX:
    case Property::RECTANGLE:
    case Property::ROTATION:
    case Property::STRING:
    case Property::TYPE_COUNT:
    {
      break;
    }

  }

  return ret;
}

std::ostream& operator<< (std::ostream& stream, const Property::Value& value )
{

  const Property::Value::Impl& impl( *value.mImpl );

  switch( impl.mType )
  {
    case Dali::Property::STRING:
    {
      stream <<  AnyCast<std::string>(impl.mValue).c_str();
      break;
    }
    case Dali::Property::VECTOR2:
    {
      stream << AnyCast<Vector2>(impl.mValue);
      break;
    }
    case Dali::Property::VECTOR3:
    {
      stream << AnyCast<Vector3>(impl.mValue);
      break;
    }
    case Dali::Property::VECTOR4:
    {
      stream << AnyCast<Vector4>(impl.mValue);
      break;
    }
    case Dali::Property::MATRIX:
    {
      stream << AnyCast<Matrix>(impl.mValue);
      break;
    }
    case Dali::Property::BOOLEAN:
    {
      stream << AnyCast<bool>(impl.mValue);
      break;
    }
    case Dali::Property::FLOAT:
    {
      stream << AnyCast<float>(impl.mValue);
      break;
    }
    case Dali::Property::INTEGER:
    {
       stream << AnyCast<int>(impl.mValue);
       break;
    }
    case Dali::Property::UNSIGNED_INTEGER:
    {
      stream << AnyCast<unsigned int>(impl.mValue);
      break;
    }
    case Dali::Property::RECTANGLE:
    {
      Dali::Rect<int> rect; // Propery Value rectangles are currently integer based
      value.Get( rect );
      stream << rect;
      break;
    }
    case Dali::Property::MATRIX3:
    {
      stream << AnyCast<Matrix3>(impl.mValue);
      break;
    }
    case Dali::Property::ROTATION:
    {
      // @todo this may change to Quaternion
      Dali::Quaternion q;
      value.Get( q );
      Dali::Vector4 v4 = q.EulerAngles();
      stream << v4;
      break;
    }

    case Dali::Property::ARRAY:
    {
      // @todo Need to think about the best way to support array
      // E.g Do we want to create a JSON style array like:
      // [ {"property-name-0":"property-value-0", "property-name-1":"property-value-1"} ]
      stream << "ARRAY unsupported";
      break;
    }
    case Dali::Property::MAP:
    {
      Dali::Property::Map map;
      value.Get( map );
      stream << "Map containing " << map.Count() << " elements";
      break;
    }
    case Dali::Property::TYPE_COUNT:
    {
      stream << "unsupported TYPE_COUNT";
      break;
    }
    default:
    {
      stream << "unsupported type = " << value.GetType();
      break;
    }
  }
  return stream;
}


} // namespace Dali
