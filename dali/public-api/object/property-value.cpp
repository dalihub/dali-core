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

namespace
{
/**
 * Helper to check if the property value can be read as int/unsigned int/bool
 */
inline bool IsIntegerType( Property::Type type )
{
  return ( Property::BOOLEAN == type )||( Property::INTEGER == type )||(Property::UNSIGNED_INTEGER == type );
}
}

struct Property::Value::Impl
{
  Impl()
  : type( Property::NONE ),
    integerValue( 0 )
  { }

  Impl( bool booleanValue )
  : type( Property::BOOLEAN ),
    integerValue( booleanValue )
  { }

  Impl( float floatValue )
  : type( Property::FLOAT ),
    floatValue( floatValue )
  { }

  Impl( int integerValue )
  : type( Property::INTEGER ),
    integerValue( integerValue )
  { }

  Impl( unsigned int unsignedIntegerValue )
  : type( Property::UNSIGNED_INTEGER ),
    unsignedIntegerValue( unsignedIntegerValue )
  { }

  Impl( const Vector2& vectorValue )
  : type( Property::VECTOR2 ),
    vector2Value( new Vector2( vectorValue ) )
  { }

  Impl( const Vector3& vectorValue )
  : type( Property::VECTOR3 ),
    vector3Value( new Vector3( vectorValue ) )
  { }

  Impl( const Vector4& vectorValue )
  : type( Property::VECTOR4 ),
    vector4Value( new Vector4( vectorValue ) )
  { }

  Impl( const Matrix3& matrixValue )
  : type( Property::MATRIX3 ),
    matrix3Value( new Matrix3( matrixValue ) )
  {
  }

  Impl( const Matrix& matrixValue )
  : type( Property::MATRIX ),
    matrixValue( new Matrix( matrixValue ) )
  {
  }

  Impl( const AngleAxis& angleAxisValue )
  : type( Property::ROTATION ),
    quaternionValue( new Quaternion( angleAxisValue.angle, angleAxisValue.axis ) )
  {
  }

  Impl( const Quaternion& quaternionValue )
  : type( Property::ROTATION ),
    quaternionValue( new Quaternion( quaternionValue ) )
  {
  }

  Impl(const std::string& stringValue)
  : type( Property::STRING ),
    stringValue( new std::string( stringValue ) )
  {
  }

  Impl( const Rect<int>& rectValue )
  : type( Property::RECTANGLE ),
    rectValue( new Rect<int>( rectValue ) )
  {
  }

  Impl( const Property::Array& arrayValue )
  : type( Property::ARRAY ),
    arrayValue( new Property::Array( arrayValue ) )
  {
  }

  Impl( const Property::Map& mapValue )
  : type( Property::MAP ),
    mapValue( new Property::Map( mapValue ) )
  {
  }

  /**
   * Destructor, takes care of releasing the dynamically allocated types
   */
  ~Impl()
  {
    switch( type )
    {
      case Property::NONE :             // FALLTHROUGH
      case Property::TYPE_COUNT :       // FALLTHROUGH
      case Property::BOOLEAN :          // FALLTHROUGH
      case Property::FLOAT :            // FALLTHROUGH
      case Property::INTEGER :          // FALLTHROUGH
      case Property::UNSIGNED_INTEGER :
      {
        break; // nothing to do
      }
      case Property::VECTOR2 :
      {
        delete vector2Value;
        break;
      }
      case Property::VECTOR3:
      {
        delete vector3Value;
        break;
      }
      case Property::VECTOR4:
      {
        delete vector4Value;
        break;
      }
      case Property::MATRIX3:
      {
        delete matrix3Value;
        break;
      }
      case Property::MATRIX:
      {
        delete matrixValue;
        break;
      }
      case Property::RECTANGLE:
      {
        delete rectValue;
        break;
      }
      case Property::ROTATION:
      {
        delete quaternionValue;
        break;
      }
      case Property::STRING:
      {
        delete stringValue;
        break;
      }
      case Property::ARRAY:
      {
        delete arrayValue;
        break;
      }
      case Property::MAP:
      {
        delete mapValue;
        break;
      }
    }
  }

public: // Data

  Type type;
  union
  {
    int integerValue;
    float floatValue;
    unsigned int unsignedIntegerValue;
    // must use pointers for any class value pre c++ 11
    Vector2* vector2Value;
    Vector3* vector3Value;
    Vector4* vector4Value;
    Matrix3* matrix3Value;
    Matrix* matrixValue;
    Quaternion* quaternionValue;
    std::string* stringValue;
    Rect<int>* rectValue;
    Property::Array* arrayValue;
    Property::Map* mapValue;
  };
};

Property::Value::Value()
: mImpl( NULL )
{
}

Property::Value::Value( bool booleanValue )
: mImpl( new Impl( booleanValue ) )
{
}

Property::Value::Value( float floatValue )
: mImpl( new Impl( floatValue ) )
{
}

Property::Value::Value( int integerValue )
: mImpl( new Impl( integerValue ) )
{
}

Property::Value::Value( unsigned int unsignedIntegerValue )
: mImpl( new Impl( unsignedIntegerValue ) )
{
}

Property::Value::Value( const Vector2& vectorValue )
: mImpl( new Impl( vectorValue ) )
{
}

Property::Value::Value( const Vector3& vectorValue )
: mImpl( new Impl( vectorValue ) )
{
}

Property::Value::Value( const Vector4& vectorValue )
: mImpl( new Impl( vectorValue ) )
{
}

Property::Value::Value( const Matrix3& matrixValue )
: mImpl( new Impl( matrixValue ) )
{
}

Property::Value::Value( const Matrix& matrixValue )
: mImpl( new Impl( matrixValue ) )
{
}

Property::Value::Value( const Rect<int>& rectValue )
: mImpl( new Impl( rectValue ) )
{
}

Property::Value::Value( const AngleAxis& angleAxisValue )
: mImpl( new Impl( angleAxisValue ) )
{
}

Property::Value::Value( const Quaternion& quaternionValue )
: mImpl( new Impl( quaternionValue ) )
{
}

Property::Value::Value( const std::string& stringValue )
: mImpl( new Impl( stringValue ) )
{
}

Property::Value::Value( const char* stringValue )
: mImpl( new Impl( std::string(stringValue) ) )
{
}

Property::Value::Value( Property::Array& arrayValue )
: mImpl( new Impl( arrayValue ) )
{
}

Property::Value::Value( Property::Map& mapValue )
: mImpl( new Impl( mapValue ) )
{
}


Property::Value::~Value()
{
  delete mImpl;
}

Property::Value::Value( const Property::Value& value )
: mImpl( NULL )
{
  // reuse assignment operator
  operator=( value );
}

Property::Value::Value( Type type )
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
      mImpl = new Impl( Quaternion() );
      break;
    }
    case Property::STRING:
    {
      mImpl = new Impl( std::string() );
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
    case Property::MAP:
    {
      mImpl = new Impl( Property::Map() );
      break;
    }
    case Property::NONE:
    case Property::TYPE_COUNT:
    {
      mImpl = new Impl();
      break;
    }
  }
}

Property::Value& Property::Value::operator=( const Property::Value& value )
{
  if ( this == &value )
  {
    // skip self assignment
    return *this;
  }
  // if we are assigned an empty value, just drop impl
  if( !value.mImpl )
  {
    delete mImpl;
    mImpl = NULL;
    return *this;
  }
  // first check if the type is the same, no need to change impl, just assign
  if( mImpl && ( mImpl->type == value.mImpl->type ) )
  {
    switch( mImpl->type )
    {
      case Property::BOOLEAN:
      {
        mImpl->integerValue = value.mImpl->integerValue;
        break;
      }
      case Property::FLOAT:
      {
        mImpl->floatValue = value.mImpl->floatValue;
        break;
      }
      case Property::INTEGER:
      {
        mImpl->integerValue = value.mImpl->integerValue;
        break;
      }
      case Property::UNSIGNED_INTEGER:
      {
        mImpl->unsignedIntegerValue = value.mImpl->unsignedIntegerValue;
        break;
      }
      case Property::VECTOR2:
      {
        *mImpl->vector2Value = *value.mImpl->vector2Value;
        break;
      }
      case Property::VECTOR3:
      {
        *mImpl->vector3Value = *value.mImpl->vector3Value;
        break;
      }
      case Property::VECTOR4:
      {
        *mImpl->vector4Value = *value.mImpl->vector4Value;
        break;
      }
      case Property::RECTANGLE:
      {
        *mImpl->rectValue = *value.mImpl->rectValue;
        break;
      }
      case Property::ROTATION:
      {
        *mImpl->quaternionValue = *value.mImpl->quaternionValue;
        break;
      }
      case Property::STRING:
      {
        *mImpl->stringValue = *value.mImpl->stringValue;
        break;
      }
      case Property::MATRIX:
      {
        *mImpl->matrixValue = *value.mImpl->matrixValue;
        break;
      }
      case Property::MATRIX3:
      {
        *mImpl->matrix3Value = *value.mImpl->matrix3Value;
        break;
      }
      case Property::ARRAY:
      {
        *mImpl->arrayValue = *value.mImpl->arrayValue;
        break;
      }
      case Property::MAP:
      {
        *mImpl->mapValue = *value.mImpl->mapValue;
        break;
      }
      case Property::NONE:
      case Property::TYPE_COUNT:
      { // mImpl will be NULL, there's no way to get to this case
      }
    }
  }
  else
  {
    // different type, release old impl and create new
    Impl* newImpl( NULL );
    switch ( value.mImpl->type )
    {
      case Property::BOOLEAN:
      {
        newImpl = new Impl( bool( value.mImpl->integerValue ) );
        break;
      }
      case Property::FLOAT:
      {
        newImpl = new Impl( value.mImpl->floatValue );
        break;
      }
      case Property::INTEGER:
      {
        newImpl = new Impl( value.mImpl->integerValue );
        break;
      }
      case Property::UNSIGNED_INTEGER:
      {
        newImpl = new Impl( value.mImpl->unsignedIntegerValue );
        break;
      }
      case Property::VECTOR2:
      {
        newImpl = new Impl( *value.mImpl->vector2Value );
        break;
      }
      case Property::VECTOR3:
      {
        newImpl = new Impl( *value.mImpl->vector3Value );
        break;
      }
      case Property::VECTOR4:
      {
        newImpl = new Impl( *value.mImpl->vector4Value );
        break;
      }
      case Property::RECTANGLE:
      {
        newImpl = new Impl( *value.mImpl->rectValue );
        break;
      }
      case Property::ROTATION:
      {
        newImpl = new Impl( *value.mImpl->quaternionValue );
        break;
      }
      case Property::MATRIX3:
      {
        newImpl = new Impl( *value.mImpl->matrix3Value );
        break;
      }
      case Property::MATRIX:
      {
        newImpl = new Impl( *value.mImpl->matrixValue );
        break;
      }
      case Property::STRING:
      {
        newImpl = new Impl( *value.mImpl->stringValue );
        break;
      }
      case Property::ARRAY:
      {
        newImpl = new Impl( *value.mImpl->arrayValue );
        break;
      }
      case Property::MAP:
      {
        newImpl = new Impl( *value.mImpl->mapValue );
        break;
      }
      case Property::NONE:
      case Property::TYPE_COUNT:
      { // NULL value will be used for "empty" value
      }
    }
    delete mImpl;
    mImpl = newImpl;
  }

  return *this;
}

Property::Type Property::Value::GetType() const
{
  Property::Type type( Property::NONE );
  if( mImpl )
  {
    type = mImpl->type;
  }
  return type;
}

void Property::Value::Get( bool& booleanValue ) const
{
  if( mImpl && IsIntegerType( mImpl->type ) )
  {
    booleanValue = mImpl->integerValue;
  }
  else
  {
    booleanValue = false;
  }
}

void Property::Value::Get( float& floatValue ) const
{
  if( mImpl && mImpl->type == FLOAT )
  {
    floatValue = mImpl->floatValue;
  }
  else
  {
    floatValue = 0.f;
  }
}

void Property::Value::Get( int& integerValue ) const
{
  if( mImpl && IsIntegerType( mImpl->type ) )
  {
    integerValue = mImpl->integerValue;
  }
  else
  {
    integerValue = 0;
  }
}

void Property::Value::Get( unsigned int& unsignedIntegerValue ) const
{
  if( mImpl && IsIntegerType( mImpl->type ) )
  {
    unsignedIntegerValue = mImpl->unsignedIntegerValue;
  }
  else
  {
    unsignedIntegerValue = 0u;
  }
}

void Property::Value::Get( Vector2& vectorValue ) const
{
  if( mImpl && mImpl->type == VECTOR2 && mImpl->vector2Value )
  {
    vectorValue = *(mImpl->vector2Value);
  }
  else
  {
    vectorValue.x = vectorValue.y = 0.f;
  }
}

void Property::Value::Get( Vector3& vectorValue ) const
{
  if( mImpl && mImpl->type == VECTOR3 && mImpl->vector3Value )
  {
    vectorValue = *(mImpl->vector3Value);
  }
  else
  {
    vectorValue.x = vectorValue.y = vectorValue.z = 0.f;
  }
}

void Property::Value::Get( Vector4& vectorValue ) const
{
  if( mImpl && mImpl->type == VECTOR4 && mImpl->vector4Value )
  {
    vectorValue = *(mImpl->vector4Value);
  }
  else
  {
    vectorValue.x = vectorValue.y = vectorValue.z = vectorValue.w = 0.f;
  }
}

void Property::Value::Get( Matrix3& matrixValue ) const
{
  if( mImpl && mImpl->type == MATRIX3 && mImpl->matrix3Value )
  {
    matrixValue = *(mImpl->matrix3Value);
  }
  else
  {
    matrixValue.SetIdentity();
  }
}

void Property::Value::Get( Matrix& matrixValue ) const
{
  if( mImpl && mImpl->type == MATRIX && mImpl->matrixValue )
  {
    matrixValue = *(mImpl->matrixValue);
  }
  else
  {
    matrixValue.SetIdentity();
  }
}

void Property::Value::Get( Rect<int>& rectValue ) const
{
  if( mImpl && mImpl->type == RECTANGLE && mImpl->rectValue )
  {
    rectValue = *(mImpl->rectValue);
  }
  else
  {
    rectValue.x = rectValue.y = rectValue.width = rectValue.height = 0.f;
  }
}

void Property::Value::Get( AngleAxis& angleAxisValue ) const
{
  if( mImpl && mImpl->type == ROTATION && mImpl->quaternionValue )
  {
    mImpl->quaternionValue->ToAxisAngle( angleAxisValue.axis, angleAxisValue.angle );
  }
  else
  {
    angleAxisValue.angle = 0.f;
    angleAxisValue.axis = Vector3::ZERO; // identity quaternion
  }
}

void Property::Value::Get( Quaternion& quaternionValue ) const
{
  if( mImpl && mImpl->type == ROTATION && mImpl->quaternionValue )
  {
    quaternionValue = *(mImpl->quaternionValue);
  }
  else
  {
    quaternionValue = Quaternion::IDENTITY;
  }
}

void Property::Value::Get( std::string& stringValue ) const
{
  if( mImpl && mImpl->type == STRING && mImpl->stringValue )
  {
    stringValue.assign( *(mImpl->stringValue) );
  }
  else
  {
    stringValue.clear();
  }
}

void Property::Value::Get( Property::Array& arrayValue ) const
{
  if( mImpl && mImpl->type == ARRAY && mImpl->arrayValue )
  {
    arrayValue = *(mImpl->arrayValue);
  }
  else
  {
    arrayValue.Clear();
  }
}

void Property::Value::Get( Property::Map& mapValue ) const
{
  if( mImpl && mImpl->type == MAP && mImpl->mapValue )
  {
    mapValue = *(mImpl->mapValue);
  }
  else
  {
    mapValue.Clear();
  }
}

Property::Map* Property::Value::GetMap() const
{
  Property::Map* map = NULL;
  if( mImpl && mImpl->type == MAP && mImpl->mapValue )
  {
    map = mImpl->mapValue;
  }
  return map;
}

Property::Array* Property::Value::GetArray() const
{
  Property::Array* array = NULL;
  if( mImpl && mImpl->type == ARRAY && mImpl->arrayValue )
  {
    array = mImpl->arrayValue;
  }
  return array;
}

std::ostream& operator<<( std::ostream& stream, const Property::Value& value )
{
  if( value.mImpl )
  {
    const Property::Value::Impl& impl( *value.mImpl );

    switch( impl.type )
    {
      case Dali::Property::BOOLEAN:
      {
        stream << impl.integerValue;
        break;
      }
      case Dali::Property::FLOAT:
      {
        stream << impl.floatValue;
        break;
      }
      case Dali::Property::INTEGER:
      {
         stream << impl.integerValue;
         break;
      }
      case Dali::Property::UNSIGNED_INTEGER:
      {
        stream << impl.unsignedIntegerValue;
        break;
      }
      case Dali::Property::VECTOR2:
      {
        stream << *impl.vector2Value;
        break;
      }
      case Dali::Property::VECTOR3:
      {
        stream << *impl.vector3Value;
        break;
      }
      case Dali::Property::VECTOR4:
      {
        stream << *impl.vector4Value;
        break;
      }
      case Dali::Property::MATRIX3:
      {
        stream << *impl.matrix3Value;
        break;
      }
      case Dali::Property::MATRIX:
      {
        stream << *impl.matrixValue;
        break;
      }
      case Dali::Property::RECTANGLE:
      {
        stream << *impl.rectValue;
        break;
      }
      case Dali::Property::ROTATION:
      {
        stream << *impl.quaternionValue;
        break;
      }
      case Dali::Property::STRING:
      {
        stream << *impl.stringValue;
        break;
      }
      case Dali::Property::ARRAY:
      {
        stream << "Array containing" << impl.arrayValue->Count() << " elements"; // TODO add ostream<< operator in array
        break;
      }
      case Dali::Property::MAP:
      {
        stream << "Map containing " << impl.mapValue->Count() << " elements"; // TODO add ostream<< operator in map
        break;
      }
      case Dali::Property::NONE:
      case Dali::Property::TYPE_COUNT:
      {
        stream << "undefined type";
        break;
      }
    }
  }
  else
  {
    stream << "empty type";
  }
  return stream;
}


} // namespace Dali
