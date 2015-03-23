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

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{

void CheckTypeName(const Property::Type& type)
{
  switch(type)
  {
    case Property::NONE:
    {
      DALI_TEST_CHECK( "NONE" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::BOOLEAN:
    {
      DALI_TEST_CHECK( "BOOLEAN" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::FLOAT:
    {
      DALI_TEST_CHECK( "FLOAT" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::INTEGER:
    {
      DALI_TEST_CHECK( "INTEGER" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::UNSIGNED_INTEGER:
    {
      DALI_TEST_CHECK( "UNSIGNED_INTEGER" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::VECTOR2:
    {
      DALI_TEST_CHECK( "VECTOR2" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::VECTOR3:
    {
      DALI_TEST_CHECK( "VECTOR3" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::VECTOR4:
    {
      DALI_TEST_CHECK( "VECTOR4" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::MATRIX3:
    {
      DALI_TEST_CHECK( "MATRIX3" == std::string(PropertyTypes::GetName( type  ) ) );
      break;
    }
    case Property::MATRIX:
    {
      DALI_TEST_CHECK( "MATRIX" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::RECTANGLE:
    {
      DALI_TEST_CHECK( "RECTANGLE" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::ROTATION:
    {
      DALI_TEST_CHECK( "ROTATION" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::STRING:
    {
      DALI_TEST_CHECK( "STRING" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::ARRAY:
    {
      DALI_TEST_CHECK( "ARRAY" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::MAP:
    {
      DALI_TEST_CHECK( "MAP" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
    case Property::TYPE_COUNT:
    {
      DALI_TEST_CHECK( "NONE" == std::string(PropertyTypes::GetName( type ) ) );
      break;
    }
  } // switch(type)

} // CheckTypeName

} // unnamed namespace

void utc_dali_property_value_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_property_value_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliPropertyValueConstructors01(void)
{
  // Testing PropertyValue construction using Property::Type
  // also testing the PropertyTypes::GetName is correct for each type

  TestApplication application;
  tet_infoline("Testing Property::Value construction using  Property::Type");

  Property::Value value;
  DALI_TEST_CHECK( value.GetType() == Property::NONE );

  Property::Type type = Property::NONE;
  CheckTypeName(type);
  // Value(Value&) ctor and Value(Type&) ctor
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( Property::NONE == type );

  // PropertyTypes
  type = Property::BOOLEAN;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<bool>()            == type );

  type = Property::FLOAT;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<float>()           == type );

  type = Property::INTEGER;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<int>()             == type );

  type = Property::UNSIGNED_INTEGER;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<unsigned int>()    == type );

  type = Property::VECTOR2;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<Vector2>()         == type );

  type = Property::VECTOR3;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<Vector3>()         == type );

  type = Property::VECTOR4;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<Vector4>()         == type );

  type = Property::MATRIX3;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<Matrix3>()         == type );

  type = Property::MATRIX;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<Matrix>()          == type );

  typedef Dali::Rect<int> Rectangle;
  type = Property::RECTANGLE;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<Rectangle>()       == type );

  type = Property::ROTATION;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<Quaternion>()      == type );

  type = Property::ROTATION;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<AngleAxis>()       == type );

  type = Property::STRING;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<std::string>()     == type );

  type = Property::ARRAY;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<Property::Array>() == type );

  type = Property::MAP;
  CheckTypeName(type);
  DALI_TEST_CHECK( Property::Value(Property::Value(type)).GetType() == type );
  DALI_TEST_CHECK( PropertyTypes::Get<Property::Map>()   == type );
  END_TEST;

}

int UtcDaliPropertyValueConstructors02(void)
{
  // Testing PropertyValue construction using value / reference types
  // also testing the PropertyTypes::Get return same value as constructed

  TestApplication application;
  tet_infoline("Testing Property::Value construction using values / references");

  Property::Value value;

  bool b = false;
  value = Property::Value(true);
  value.Get(b);
  DALI_TEST_CHECK( true == b );

  float f = 5.f;
  value = Property::Value(10.f);
  value.Get(f);
  DALI_TEST_CHECK( Dali::Equals(10.f, f) );

  int i = 5;
  value = Property::Value(10);
  value.Get(i);
  DALI_TEST_CHECK( 10 == i );

  unsigned int ui = 5;
  value = Property::Value(10U);
  value.Get(ui);
  DALI_TEST_CHECK( 10 == ui );

  Vector2 v2 = Vector2(0,0);
  value = Property::Value( Vector2(1,1) );
  value.Get(v2);
  DALI_TEST_CHECK( Vector2(1,1) == v2 );

  Vector3 v3 = Vector3(0.f,0.f,0.f);
  value = Property::Value( Vector3(1.f,1.f,1.f) );
  value.Get(v3);
  DALI_TEST_CHECK( Vector3(1.f,1.f,1.f) == v3 );

  Vector4 v4 = Vector4(0,0,0,0);
  value = Property::Value( Vector4(1,1,1,1) );
  value.Get(v4);
  DALI_TEST_CHECK( Vector4(1,1,1,1) == v4 );

  Matrix3 m3 = Matrix3(0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f);
  value = Property::Value( Matrix3::IDENTITY );
  value.Get(m3);
  DALI_TEST_CHECK( Matrix3::IDENTITY == m3 );

  Matrix m = Matrix(true);
  value = Property::Value( Matrix::IDENTITY );
  value.Get(m);
  DALI_TEST_CHECK( Matrix::IDENTITY == m );

  typedef Dali::Rect<int> Rectangle;
  Rectangle r = Rectangle(0,0,0,0);
  value = Property::Value( Rectangle(1,1,1,1) );
  value.Get(r);
  DALI_TEST_CHECK( Rectangle(1,1,1,1) == r );

  Quaternion q = Quaternion(0,0,0,0);
  value = Property::Value( Quaternion(1,1,1,1) );
  value.Get(q);
  DALI_TEST_CHECK( Quaternion(1,1,1,1) == q );

  AngleAxis aa = AngleAxis( Degree(0), Vector3(0.f,0.f,0.f) );
  value = Property::Value( AngleAxis( Radian(Math::PI_2), Vector3::XAXIS  ));
  value.Get(aa);
  Quaternion r8(Radian(Degree(aa.angle)), aa.axis);
  DALI_TEST_EQUALS(r8, Quaternion(Math::PI_2, Vector3::XAXIS), 0.001, TEST_LOCATION);

  std::string s = "no";
  value = Property::Value("yes");
  value.Get(s);
  DALI_TEST_CHECK( "yes" == s );

  Property::Array array;
  value = Property::Value(Property::ARRAY);
  value.AppendItem(10);
  value.Get(array);
  int getItem = 0;
  array[0].Get(getItem);
  DALI_TEST_CHECK( getItem == 10 );

  Property::Map map;
  value = Property::Value(Property::MAP);
  value.SetValue("key", "value");
  value.Get(map);
  DALI_TEST_CHECK( map.GetKey(0) == "key" );

  END_TEST;
}


int UtcDaliPropertyValueCopyConstructors01(void)
{
  TestApplication application;
  tet_infoline("Testing Property::Value copy construction using values / references");

  Property::Value value;

  value = Property::Value(true);
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( true == copy.Get<bool>() );
  }

  value = Property::Value(10.f);
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( Dali::Equals(10.f, copy.Get<float>() ) );
  }

  value = Property::Value(10);
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( 10 == copy.Get<int>() );
  }

  value = Property::Value(10U);
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( 10 == copy.Get< unsigned int>() );
  }

  value = Property::Value( Vector2(1,1) );
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( Vector2(1,1) == copy.Get<Vector2>() );
  }

  value = Property::Value( Vector3(1.f,1.f,1.f) );
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( Vector3(1.f,1.f,1.f) == copy.Get<Vector3>() );
  }

  value = Property::Value( Vector4(1,1,1,1) );
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( Vector4(1,1,1,1) == copy.Get<Vector4>() );
  }

  value = Property::Value( Matrix3::IDENTITY );
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( Matrix3::IDENTITY == copy.Get<Matrix3>() );
  }

  value = Property::Value( Matrix::IDENTITY );
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( Matrix::IDENTITY == copy.Get<Matrix>()  );
  }

  typedef Dali::Rect<int> Rectangle;

  value = Property::Value( Rectangle(1,1,1,1) );
  {
    Property::Value copy( value );
    Rectangle copyRect;
    copy.Get(copyRect);
    DALI_TEST_CHECK(  Rectangle(1,1,1,1) == copyRect);
  }

  value = Property::Value( Quaternion(1,1,1,1) );
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( Quaternion(1,1,1,1) == copy.Get<Quaternion>() );
  }

  value = Property::Value( AngleAxis( Radian(Math::PI_2), Vector3::XAXIS  ));
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( value.Get<AngleAxis>().axis == copy.Get<AngleAxis>().axis );
    DALI_TEST_CHECK( value.Get<AngleAxis>().angle == copy.Get<AngleAxis>().angle );
  }

  value = Property::Value("yes");
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( "yes" == copy.Get<std::string>() );
  }

  Property::Array array;
  value = Property::Value(Property::ARRAY);
  value.AppendItem(10);
  {
    Property::Value copy( value );
    copy.Get( array );
    int getItem = 0;
    array[0].Get( getItem );
    DALI_TEST_CHECK( getItem == 10 );
  }
  Property::Map map;
  value = Property::Value(Property::MAP);
  value.SetValue("key", "value");
  {
    Property::Value copy( value );
    copy.Get( map );
    DALI_TEST_CHECK( map.GetKey(0) == "key" );
  }

  END_TEST;
}


int UtcDaliPropertyValueAssignmentOperator01(void)
{
  // Testing Property Value assignment
  TestApplication application;
  tet_infoline("Testing Property::Value assignment operator");

  Property::Value value;


  value = Property::Value(true);
  {
    Property::Value copy( false );
    copy = value;
    DALI_TEST_CHECK( true == copy.Get<bool>() );
  }

  value = Property::Value(10.f);
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Dali::Equals(10.f, copy.Get<float>() ) );
  }

  value = Property::Value(10);
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( 10 == copy.Get<int>() );
  }

  value = Property::Value(10U);
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( 10 == copy.Get< unsigned int>() );
  }

  value = Property::Value( Vector2(1,1) );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Vector2(1,1) == copy.Get<Vector2>() );
  }

  value = Property::Value( Vector3(1.f,1.f,1.f) );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Vector3(1.f,1.f,1.f) == copy.Get<Vector3>() );
  }

  value = Property::Value( Vector4(1,1,1,1) );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Vector4(1,1,1,1) == copy.Get<Vector4>() );
  }

  value = Property::Value( Matrix3::IDENTITY );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Matrix3::IDENTITY == copy.Get<Matrix3>() );
  }

  value = Property::Value( Matrix::IDENTITY );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Matrix::IDENTITY == copy.Get<Matrix>()  );
  }

  typedef Dali::Rect<int> Rectangle;

  value = Property::Value( Rectangle(1,1,1,1) );
  {
    Property::Value copy(false);
    copy = value;
    Rectangle copyRect;
    copy.Get(copyRect);
    DALI_TEST_CHECK(  Rectangle(1,1,1,1) == copyRect);
  }

  value = Property::Value( Quaternion(1,1,1,1) );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Quaternion(1,1,1,1) == copy.Get<Quaternion>() );
  }

  value = Property::Value( AngleAxis( Radian(Math::PI_2), Vector3::XAXIS  ));
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( value.Get<AngleAxis>().axis == copy.Get<AngleAxis>().axis );
    DALI_TEST_CHECK( value.Get<AngleAxis>().angle == copy.Get<AngleAxis>().angle );
  }

  value = Property::Value("yes");
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( "yes" == copy.Get<std::string>() );
  }

  Property::Array array;
  value = Property::Value(Property::ARRAY);
  value.AppendItem(10);
  {
    Property::Value copy(false);
    copy = value;
    copy.Get( array );
    int getItem = 0;
    array[0].Get( getItem );
    DALI_TEST_CHECK( getItem == 10 );
  }
  Property::Map map;
  value = Property::Value(Property::MAP);
  value.SetValue("key", "value");
  {
    Property::Value copy(false);
    copy = value;
    copy.Get( map );
    DALI_TEST_CHECK( map.GetKey(0) == "key" );
  }

  END_TEST;
}


int UtcDaliPropertyValueOutputStream(void)
{
  TestApplication application;
  tet_infoline("Testing Property::Value output stream");
  typedef Dali::Rect<int> Rectangle;

  Property::Value value(true);
  {
    std::ostringstream stream;
    stream << value;
    DALI_TEST_CHECK( stream.str() == "1")
  }

  {
    value = Property::Value(20.2f);
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "20.2")
  }

  {
    value = Property::Value(-25);
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "-25")
  }

  {
    value = Property::Value(25U);
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "25")
  }

  {
    value = Property::Value( Vector2(1.f,1.f) );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "[1, 1]");
  }

  {
    value = Property::Value( Vector3(1.f,1.f,1.f) );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "[1, 1, 1]");
  }

  {
    value = Property::Value( Matrix3::IDENTITY );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "[ [1, 0, 0], [0, 1, 0], [0, 0, 1] ]");
  }

  {
    value = Property::Value( Matrix::IDENTITY );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "[ [1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1] ]");
  }

  {
    value = Property::Value( Rectangle(1,2,3,4) );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "[1, 2, 3, 4]");
  }

  {
    value = Property::Value( AngleAxis( Radian(1.2f), Vector3::XAXIS ));
    std::ostringstream stream;
    stream <<  value;
    tet_printf("angle axis = %s \n", stream.str().c_str() );
    DALI_TEST_CHECK( stream.str() == "[1.2, -0, 0, 0]");
  }

  // Maps and arrays currently not supported, we just check a message is output
  {
    Property::Map map;
    value = Property::Value( map );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( !stream.str().empty() );
  }
  {
    Property::Array array;
    value = Property::Value( array );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( !stream.str().empty() );
  }


  END_TEST;

}
