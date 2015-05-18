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

template <typename T>
struct GetCheckP
{
  GetCheckP(T value)
  {
    Property::Value v(value);
    DALI_TEST_CHECK( v.Get<T>() == value );
  }
};

template <typename T>
struct CheckCopyCtorP
{
  CheckCopyCtorP(Property::Value value)
  {
    Property::Value copy( value );
    DALI_TEST_CHECK( value.Get<T>() == copy.Get<T>() );
  }
};

template <>
struct CheckCopyCtorP<AngleAxis>
{
  CheckCopyCtorP(Property::Value value)
  {
    Property::Value copy( value );
    AngleAxis a = value.Get<AngleAxis>();
    AngleAxis b = copy.Get<AngleAxis>();
    DALI_TEST_CHECK( a.angle == b.angle );
    DALI_TEST_CHECK( a.axis == b.axis );
  }
};

template <>
struct CheckCopyCtorP<Property::Array>
{
  CheckCopyCtorP(Property::Value value)
  {
    Property::Value copy( value );
    Property::Array a = value.Get<Property::Array>();
    Property::Array b = copy.Get<Property::Array>();
    DALI_TEST_CHECK( a.Size() == b.Size() );
  }
};

template <>
struct CheckCopyCtorP<Property::Map>
{
  CheckCopyCtorP(Property::Value value)
  {
    Property::Value copy( value );
    Property::Map a = value.Get<Property::Map>();
    Property::Map b = copy.Get<Property::Map>();
    DALI_TEST_CHECK( a.Count() == b.Count() );
  }
};


template <typename T>
struct GetCheckN
{
  GetCheckN()
  {
    Property::Value v;
    try
    {
      T got = v.Get<T>();
      (void)got;
      tet_result(TET_FAIL);
    }
    catch( Dali::DaliException& e )
    {
      DALI_TEST_PRINT_ASSERT( e );
      DALI_TEST_ASSERT( e, "Property type invalid", TEST_LOCATION );
    }
    catch( ... )
    {
      tet_printf("Assertion test failed - wrong Exception\n" );
      tet_result(TET_FAIL);
    }
  }
};

template<>
struct GetCheckN<Property::Array>
{
  GetCheckN()
  {
    Property::Value value;
    try
    {
      Property::Array array = value.Get<Property::Array>();
      tet_result(TET_FAIL);
    }
    catch( Dali::DaliException& e )
    {
      DALI_TEST_PRINT_ASSERT( e );
      DALI_TEST_ASSERT( e, "Property type invalid", TEST_LOCATION );
    }
    catch( ... )
    {
      tet_printf("Assertion test failed - wrong Exception\n" );
      tet_result(TET_FAIL);
    }
  }
};

template<>
struct GetCheckN<Property::Map>
{
  GetCheckN()
  {
    Property::Value value;
    try
    {
      Property::Map map = value.Get<Property::Map>();
      tet_result(TET_FAIL);
    }
    catch( Dali::DaliException& e )
    {
      DALI_TEST_PRINT_ASSERT( e );
      DALI_TEST_ASSERT( e, "Property type invalid", TEST_LOCATION );
    }
    catch( ... )
    {
      tet_printf("Assertion test failed - wrong Exception\n" );
      tet_result(TET_FAIL);
    }
  }
};


} // unnamed namespace

void utc_dali_property_value_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_property_value_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliPropertyValueConstructors01P(void)
{
  TestApplication application;
  Property::Value value;

  DALI_TEST_CHECK( value.GetType() == Property::NONE );

  END_TEST;
}

int UtcDaliPropertyValueConstructors02P(void)
{
  TestApplication application;
  Property::Value value(true);

  DALI_TEST_CHECK( value.GetType() == Property::BOOLEAN );
  DALI_TEST_CHECK( value.Get<bool>() == true );

  END_TEST;
}

int UtcDaliPropertyValueConstructors03P(void)
{
  TestApplication application;
  Property::Value value(2.f);

  DALI_TEST_CHECK( value.GetType() == Property::FLOAT );
  DALI_TEST_CHECK( value.Get<float>() == 2.f );

  END_TEST;
}

int UtcDaliPropertyValueConstructors04P(void)
{
  TestApplication application;
  Property::Value value(1);

  DALI_TEST_CHECK( value.GetType() == Property::INTEGER );
  DALI_TEST_CHECK( value.Get<int>() == 1 );

  END_TEST;
}

int UtcDaliPropertyValueConstructors05P(void)
{
  TestApplication application;
  Property::Value value(1u);

  DALI_TEST_CHECK( value.GetType() == Property::UNSIGNED_INTEGER );
  DALI_TEST_CHECK( value.Get<unsigned int>() == 1u );

  END_TEST;
}

int UtcDaliPropertyValueConstructors06P(void)
{
  TestApplication application;
  Vector2 v(1,1);
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::VECTOR2 );
  DALI_TEST_CHECK( value.Get<Vector2>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructors07P(void)
{
  TestApplication application;
  Vector3 v(1.0,1.0,1.0);
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::VECTOR3 );
  DALI_TEST_CHECK( value.Get<Vector3>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructors08P(void)
{
  TestApplication application;
  Matrix3 v(1.0,1.0,1.0, 1.0,1.0,1.0, 1.0,1.0,1.0);
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::MATRIX3 );
  DALI_TEST_CHECK( value.Get<Matrix3>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructors09P(void)
{
  TestApplication application;
  float a[] = {1.0,1.0,1.0,1.0, 1.0,1.0,1.0,1.0, 1.0,1.0,1.0,1.0, 1.0,1.0,1.0,1.0};
  Matrix v(a);
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::MATRIX );
  DALI_TEST_CHECK( value.Get<Matrix>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructors10P(void)
{
  TestApplication application;
  Rect<int> v(1.0,1.0,1.0,1.0);
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::RECTANGLE );
  DALI_TEST_CHECK( value.Get<Rect<int> >() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructors11P(void)
{
  TestApplication application;
  AngleAxis v(Degree(1.0), Vector3(1.0,1.0,1.0));
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::ROTATION );
  AngleAxis got = value.Get<AngleAxis>();
  DALI_TEST_CHECK( got.angle == v.angle );
  DALI_TEST_CHECK( got.axis == v.axis );

  END_TEST;
}

int UtcDaliPropertyValueConstructors12P(void)
{
  TestApplication application;
  Quaternion v( Vector4(1.0,1.0,1.0,1.0) );
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::ROTATION );
  DALI_TEST_CHECK( value.Get<Quaternion>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructors13P(void)
{
  TestApplication application;
  std::string v("1");
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::STRING );
  DALI_TEST_CHECK( value.Get<std::string>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructors14P(void)
{
  TestApplication application;

  Property::Value value(Property::ARRAY);
  value.AppendItem(Property::Value(1));

  DALI_TEST_CHECK( value.GetType() == Property::ARRAY );
  DALI_TEST_CHECK( value.GetSize() == 1);

  END_TEST;
}

int UtcDaliPropertyValueConstructors15P(void)
{
  TestApplication application;

  Property::Value value(Property::MAP);
  value.SetValue("1", Property::Value(1));

  DALI_TEST_CHECK( value.GetType() == Property::MAP );
  DALI_TEST_CHECK( value.GetSize() == 1);

  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor01P(void)
{
  TestApplication application;
  CheckCopyCtorP<float> check(1.f);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor02P(void)
{
  TestApplication application;
  CheckCopyCtorP<bool> check(true);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor03P(void)
{
  TestApplication application;
  CheckCopyCtorP<float> check(1.f);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor04P(void)
{
  TestApplication application;
  CheckCopyCtorP<int> check(1);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor05P(void)
{
  TestApplication application;
  CheckCopyCtorP<unsigned int> check(1u);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor06P(void)
{
  TestApplication application;
  CheckCopyCtorP<Vector2> check( Vector2(1,1) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor07P(void)
{
  TestApplication application;
  CheckCopyCtorP<Vector3> check( Vector3(1.0,1.0,1.0) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor08P(void)
{
  TestApplication application;
  CheckCopyCtorP<Matrix3> check( Matrix3::IDENTITY );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor09P(void)
{
  TestApplication application;
  CheckCopyCtorP<Matrix> check(Matrix::IDENTITY);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor10P(void)
{
  TestApplication application;
  CheckCopyCtorP<Rect<int> > check( Rect<int>(1.0,1.0,1.0,1.0) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor11P(void)
{
  TestApplication application;
  CheckCopyCtorP<AngleAxis> check( AngleAxis(Degree(1.0), Vector3(1.0,1.0,1.0)) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor12P(void)
{
  TestApplication application;
  CheckCopyCtorP<Quaternion> check( Quaternion( Vector4(1.0, 1.0, 1.0, 1.0) ) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor13P(void)
{
  TestApplication application;
  CheckCopyCtorP<std::string> check( std::string("1") );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor14P(void)
{
  TestApplication application;
  Property::Array value;
  value.PushBack(Property::Value(1));
  CheckCopyCtorP<Property::Array> check(value);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructor15P(void)
{
  TestApplication application;
  Property::Map value;
  value["1"] = Property::Value(1);
  CheckCopyCtorP<Property::Map> check(value);
  END_TEST;
}


int UtcDaliPropertyValueAssignmentOperator01P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value(true);
  {
    Property::Value copy( false );
    copy = value;
    DALI_TEST_CHECK( true == copy.Get<bool>() );
  }
  END_TEST;
}


int UtcDaliPropertyValueAssignmentOperator02P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value(10.f);
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Dali::Equals(10.f, copy.Get<float>() ) );
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator03P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value(10);
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( 10 == copy.Get<int>() );
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator04P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value(10U);
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( 10 == copy.Get< unsigned int>() );
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator05P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value( Vector2(1,1) );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Vector2(1,1) == copy.Get<Vector2>() );
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator06P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value( Vector3(1.f,1.f,1.f) );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Vector3(1.f,1.f,1.f) == copy.Get<Vector3>() );
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator07P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value( Vector4(1,1,1,1) );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Vector4(1,1,1,1) == copy.Get<Vector4>() );
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator08P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value( Matrix3::IDENTITY );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Matrix3::IDENTITY == copy.Get<Matrix3>() );
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator09P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value( Matrix::IDENTITY );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Matrix::IDENTITY == copy.Get<Matrix>()  );
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator10P(void)
{
  TestApplication application;
  Property::Value value;
  typedef Dali::Rect<int> Rectangle;

  value = Property::Value( Rectangle(1,1,1,1) );
  {
    Property::Value copy(false);
    copy = value;
    Rectangle copyRect;
    copy.Get(copyRect);
    DALI_TEST_CHECK(  Rectangle(1,1,1,1) == copyRect);
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator11P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value( Quaternion(1,1,1,1) );
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( Quaternion(1,1,1,1) == copy.Get<Quaternion>() );
  }
  END_TEST;
}


int UtcDaliPropertyValueAssignmentOperator12P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value( AngleAxis( Radian(Math::PI_2), Vector3::XAXIS  ));
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( value.Get<AngleAxis>().axis == copy.Get<AngleAxis>().axis );
    DALI_TEST_CHECK( value.Get<AngleAxis>().angle == copy.Get<AngleAxis>().angle );
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator13P(void)
{
  TestApplication application;
  Property::Value value;
  value = Property::Value("yes");
  {
    Property::Value copy(false);
    copy = value;
    DALI_TEST_CHECK( "yes" == copy.Get<std::string>() );
  }
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator14P(void)
{
  TestApplication application;
  Property::Value value;
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
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperator15P(void)
{
  TestApplication application;
  Property::Value value;
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


int UtcDaliPropertyValueGetTypeP(void)
{
  TestApplication application;

  Property::Value value;
  DALI_TEST_CHECK( value.GetType() == Property::NONE );
  END_TEST;
}

int UtcDaliPropertyValueGet01P(void)
{
  TestApplication application;
  Property::Value value(true);
  DALI_TEST_CHECK( value.Get<bool>() == true );
  value = Property::Value(1.f);
  DALI_TEST_CHECK( value.Get<float>() == 1.f );
  END_TEST;
}

int UtcDaliPropertyValueGet01N(void)
{
  TestApplication application;

  Property::Value value;
  try
  {
    value.Get<bool>() == true;
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "Property type invalid", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliPropertyValueGet02P(void)
{
  TestApplication application;
  Property::Value value(true);
  DALI_TEST_CHECK( value.Get<bool>() == true );
  END_TEST;
}

int UtcDaliPropertyValueGet02N(void)
{
  TestApplication application;
  GetCheckN<bool> check;
  END_TEST;
}


int UtcDaliPropertyValueGet03P(void)
{
  TestApplication application;
  GetCheckP<float> check(1.f);
  END_TEST;
}

int UtcDaliPropertyValueGet03N(void)
{
  TestApplication application;
  GetCheckN<float> check;
  END_TEST;
}

int UtcDaliPropertyValueGet04P(void)
{
  TestApplication application;
  GetCheckP<int> check(1);
  END_TEST;
}

int UtcDaliPropertyValueGet04N(void)
{
  TestApplication application;
  GetCheckN<int> check;
  END_TEST;
}

int UtcDaliPropertyValueGet05P(void)
{
  TestApplication application;
  GetCheckP<unsigned int> check(1u);
  END_TEST;
}

int UtcDaliPropertyValueGet05N(void)
{
  TestApplication application;
  GetCheckN<unsigned int> check;
  END_TEST;
}

int UtcDaliPropertyValueGet06P(void)
{
  TestApplication application;
  GetCheckP<Rect<int> > check(Rect<int>(1,1,1,1));
  END_TEST;
}

int UtcDaliPropertyValueGet06N(void)
{
  TestApplication application;
  GetCheckN<Rect<int> > check;
  END_TEST;
}

int UtcDaliPropertyValueGet07P(void)
{
  TestApplication application;
  GetCheckP<Vector2> check(Vector2(1.0,1.0));
  END_TEST;
}

int UtcDaliPropertyValueGet07N(void)
{
  TestApplication application;
  GetCheckN<Vector2> check;
  END_TEST;
}

int UtcDaliPropertyValueGet8P(void)
{
  TestApplication application;
  GetCheckP<Vector3> check(Vector3(1.0,1.0,1.0));
  END_TEST;
}

int UtcDaliPropertyValueGet08N(void)
{
  TestApplication application;
  GetCheckN<Vector3> check;
  END_TEST;
}

int UtcDaliPropertyValueGet09P(void)
{
  TestApplication application;
  GetCheckP<Vector4> check(Vector4(1.0,1.0,1.0,1.0));
  END_TEST;
}

int UtcDaliPropertyValueGet09N(void)
{
  TestApplication application;
  GetCheckN<Vector4> check;
  END_TEST;
}

int UtcDaliPropertyValueGet10P(void)
{
  TestApplication application;
  GetCheckP<Matrix3> check(Matrix3(Matrix3::IDENTITY));
  END_TEST;
}

int UtcDaliPropertyValueGet10N(void)
{
  TestApplication application;
  GetCheckN<Matrix3> check;
  END_TEST;
}

int UtcDaliPropertyValueGet11P(void)
{
  TestApplication application;
  GetCheckP<Matrix> check(Matrix(Matrix::IDENTITY));
  END_TEST;
}

int UtcDaliPropertyValueGet11N(void)
{
  TestApplication application;
  GetCheckN<Matrix> check;
  END_TEST;
}

int UtcDaliPropertyValueGet12P(void)
{
  TestApplication application;
  AngleAxis a(Degree(1.0), Vector3(1.0,1.0,1.0));
  Property::Value v(a);
  AngleAxis b = v.Get<AngleAxis>();
  DALI_TEST_CHECK(a.angle == b.angle);
  DALI_TEST_CHECK(a.axis == b.axis);
  END_TEST;
}

int UtcDaliPropertyValueGet12N(void)
{
  TestApplication application;
  GetCheckN<AngleAxis> check;
  END_TEST;
}

int UtcDaliPropertyValueGet13P(void)
{
  TestApplication application;
  GetCheckP<Quaternion> check(Quaternion(1.0,1.0,1.0,1.0));
  END_TEST;
}

int UtcDaliPropertyValueGet13N(void)
{
  TestApplication application;
  GetCheckN<Quaternion> check;
  END_TEST;
}

int UtcDaliPropertyValueGet14P(void)
{
  TestApplication application;
  GetCheckP<std::string> check(std::string("astring"));
  END_TEST;
}

int UtcDaliPropertyValueGet14N(void)
{
  TestApplication application;
  GetCheckN<std::string> check;
  END_TEST;
}

int UtcDaliPropertyValueGet15P(void)
{
  TestApplication application;
  Property::Value a(Property::ARRAY);
  a.AppendItem(Property::Value(1));
  Property::Array got = a.Get<Property::Array>();
  DALI_TEST_CHECK(got[0].Get<int>() == 1);
  END_TEST;
}

int UtcDaliPropertyValueGet15N(void)
{
  TestApplication application;
  GetCheckN<Property::Array> check;
  END_TEST;
}

int UtcDaliPropertyValueGet16P(void)
{
  TestApplication application;
  Property::Value a(Property::MAP);
  a.SetValue("key", Property::Value(1));
  Property::Map got = a.Get<Property::Map>();
  DALI_TEST_CHECK(got.GetValue(0).Get<int>() == 1);
  END_TEST;
}

int UtcDaliPropertyValueGet16N(void)
{
  TestApplication application;
  GetCheckN<Property::Map> check;
  END_TEST;
}

int UtcDaliPropertyValueGetValueP(void)
{
  TestApplication application;
  Property::Value map(Property::MAP);
  Property::Value a(1);
  map.SetValue("key", a);
  DALI_TEST_CHECK( map.GetValue("key").Get<int>() == 1 );
  END_TEST;
}

int UtcDaliPropertyValueGetValue01N(void)
{
  TestApplication application;
  Property::Value value(1);

  try
  {
    Property::Value got = value.GetValue("key");
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "Property type invalid", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliPropertyValueGetValue02N(void)
{
  TestApplication application;
  Property::Value value(Property::MAP);
  value.SetValue("key", Property::Value(1));

  try
  {
    Property::Value got = value.GetValue("nokey");
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "Cannot find property map key", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliPropertyValueHasKeyP(void)
{
  TestApplication application;
  Property::Value map(Property::MAP);
  Property::Value a(1);
  map.SetValue("key", a);
  DALI_TEST_CHECK( map.HasKey("key") == true );
  DALI_TEST_CHECK( map.HasKey("key2") == false );
  END_TEST;
}

int UtcDaliPropertyValueHasKeyN(void)
{
  TestApplication application;
  Property::Value a(1);
  DALI_TEST_CHECK( a.HasKey("key") == false );
  END_TEST;
}

int UtcDaliPropertyValueGetKeyP(void)
{
  TestApplication application;
  Property::Value map(Property::MAP);
  Property::Value a(1);
  map.SetValue("key", a);
  std::string empty;
  DALI_TEST_CHECK( map.GetKey(-1) == empty );
  DALI_TEST_CHECK( map.GetKey(1) == empty );
  END_TEST;
}

int UtcDaliPropertyValueGetKeyN(void)
{
  TestApplication application;
  Property::Value a(1);
  std::string empty;
  DALI_TEST_CHECK( a.GetKey(0) == empty );
  END_TEST;
}

int UtcDaliPropertyValueSetValueP(void)
{
  TestApplication application;
  Property::Value map(Property::MAP);
  Property::Value a(1);
  map.SetValue("key", a);
  DALI_TEST_CHECK( map.GetValue("key").Get<int>() == 1 );
  END_TEST;
}

int UtcDaliPropertyValueSetValueN(void)
{
  TestApplication application;
  Property::Value value(1);

  try
  {
    value.SetValue("key", Property::Value(1));
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "Property type invalid", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliPropertyValueGetItem01P(void)
{
  TestApplication application;
  Property::Value value(Property::ARRAY);
  value.AppendItem(Property::Value(0));
  value.SetItem(0, Property::Value(1));
  DALI_TEST_CHECK( value.GetItem(0).Get<int>() == 1 );

  value = Property::Value(Property::MAP);
  value.SetValue("key", Property::Value(0));
  value.SetItem(0, Property::Value(1));
  DALI_TEST_CHECK( value.GetItem(0).Get<int>() == 1 );

  END_TEST;
}

int UtcDaliPropertyValueGetItem01N(void)
{
  TestApplication application;

  Property::Value value(1);

  try
  {
    Property::Value item = value.GetItem(0);
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "Cannot GetItem on property Type; not a container", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliPropertyValueGetItem02P(void)
{
  TestApplication application;
  Property::Value value(Property::ARRAY);
  value.AppendItem(Property::Value(0));
  value.SetItem(0, Property::Value(1));
  std::string key;
  DALI_TEST_CHECK( value.GetItem(0, key).Get<int>() == 1 );
  DALI_TEST_CHECK( key.size() == 0u );

  value = Property::Value(Property::MAP);
  value.SetValue("key", Property::Value(0));
  value.SetItem(0, Property::Value(1));
  DALI_TEST_CHECK( value.GetItem(0, key).Get<int>() == 1 );
  DALI_TEST_CHECK( key.size() == 3 );
  DALI_TEST_CHECK( key == std::string("key") );

  END_TEST;
}

int UtcDaliPropertyValueGetItem02N(void)
{
  TestApplication application;

  Property::Value value(1);
  std::string key;
  try
  {
    Property::Value item = value.GetItem(0, key);
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "Cannot GetItem on property Type; not a container", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliPropertyValueSetItemP(void)
{
  TestApplication application;

  Property::Value value(Property::ARRAY);
  value.AppendItem(Property::Value(0));
  value.SetItem(0, Property::Value(1));
  DALI_TEST_CHECK( value.GetItem(0).Get<int>() == 1 );

  value = Property::Value(Property::MAP);
  value.SetValue("key", Property::Value(0));
  value.SetItem(0, Property::Value(1));
  DALI_TEST_CHECK( value.GetItem(0).Get<int>() == 1 );

  END_TEST;
}

int UtcDaliPropertyValueSetItemN(void)
{
  TestApplication application;

  Property::Value value(1);

  try
  {
    value.SetItem(0, Property::Value(2));
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "Cannot SetItem on property Type; not a container", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliPropertyValueAppendItemP(void)
{
  TestApplication application;

  Property::Value value(Property::ARRAY);
  value.AppendItem(Property::Value(1));
  DALI_TEST_CHECK( value.GetItem(0).Get<int>() == 1 );

  END_TEST;
}

int UtcDaliPropertyValueAppendItemN(void)
{
  TestApplication application;

  Property::Value value(1);

  try
  {
    Property::Value item = value.AppendItem(Property::Value(1));
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "Property type invalid", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
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
