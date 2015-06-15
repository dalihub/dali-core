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

} // unnamed namespace

void utc_dali_property_value_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_property_value_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliPropertyValueConstructorsNoneP(void)
{
  Property::Value value;

  DALI_TEST_CHECK( value.GetType() == Property::NONE );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsNoneTypeP(void)
{
  Property::Value value( Property::NONE );

  DALI_TEST_CHECK( value.GetType() == Property::NONE );

  Property::Value value2( Property::TYPE_COUNT );

  DALI_TEST_CHECK( value2.GetType() == Property::NONE );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsBoolP(void)
{
  Property::Value value(true);

  DALI_TEST_CHECK( value.GetType() == Property::BOOLEAN );
  DALI_TEST_CHECK( value.Get<bool>() == true );

  END_TEST;
}

int UtcDaliPropertyValueConstructorBoolTypeP(void)
{
  Property::Value value(Property::BOOLEAN);

  DALI_TEST_CHECK( value.GetType() == Property::BOOLEAN );
  DALI_TEST_CHECK( value.Get<bool>() == false );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsFloatP(void)
{
  Property::Value value(2.f);

  DALI_TEST_CHECK( value.GetType() == Property::FLOAT );
  DALI_TEST_CHECK( value.Get<float>() == 2.f );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsFloatTypeP(void)
{
  Property::Value value(Property::FLOAT);

  DALI_TEST_CHECK( value.GetType() == Property::FLOAT );
  DALI_TEST_CHECK( value.Get<float>() == 0.f );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsIntP(void)
{
  Property::Value value(1);

  DALI_TEST_CHECK( value.GetType() == Property::INTEGER );
  DALI_TEST_CHECK( value.Get<int>() == 1 );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsIntTypeP(void)
{
  Property::Value value(Property::INTEGER);

  DALI_TEST_CHECK( value.GetType() == Property::INTEGER );
  DALI_TEST_CHECK( value.Get<int>() == 0 );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsUnsignedIntP(void)
{
  Property::Value value(1u);

  DALI_TEST_CHECK( value.GetType() == Property::UNSIGNED_INTEGER );
  DALI_TEST_CHECK( value.Get<unsigned int>() == 1u );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsUnsignedIntTypeP(void)
{
  Property::Value value( Property::UNSIGNED_INTEGER );

  DALI_TEST_CHECK( value.GetType() == Property::UNSIGNED_INTEGER );
  DALI_TEST_CHECK( value.Get<unsigned int>() == 0u );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsVector2P(void)
{
  Vector2 v(1,1);
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::VECTOR2 );
  DALI_TEST_CHECK( value.Get<Vector2>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsVector2TypeP(void)
{
  Property::Value value( Property::VECTOR2 );

  DALI_TEST_CHECK( value.GetType() == Property::VECTOR2 );
  DALI_TEST_CHECK( value.Get<Vector2>() == Vector2::ZERO );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsVector3P(void)
{
  Vector3 v(1.f,2.f,3.f);
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::VECTOR3 );
  DALI_TEST_CHECK( value.Get<Vector3>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsVector3TypeP(void)
{
  Property::Value value( Property::VECTOR3 );

  DALI_TEST_CHECK( value.GetType() == Property::VECTOR3 );
  DALI_TEST_CHECK( value.Get<Vector3>() == Vector3() );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsVector4P(void)
{
  Vector4 v(1.f,1.f,1.f,0.9f);
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::VECTOR4 );
  DALI_TEST_CHECK( value.Get<Vector4>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsVector4TypeP(void)
{
  Property::Value value( Property::VECTOR4 );

  DALI_TEST_CHECK( value.GetType() == Property::VECTOR4 );
  DALI_TEST_CHECK( value.Get<Vector4>() == Vector4() );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsMatrix3P(void)
{
  Matrix3 v(1.0,1.0,1.0, 1.0,1.0,1.0, 1.0,1.0,1.0);
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::MATRIX3 );
  DALI_TEST_CHECK( value.Get<Matrix3>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsMatrix3TypeP(void)
{
  Property::Value value( Property::MATRIX3 );

  DALI_TEST_CHECK( value.GetType() == Property::MATRIX3 );
  DALI_TEST_CHECK( value.Get<Matrix3>() == Matrix3() );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsMatrixP(void)
{
  float a[] = {1.0,1.0,1.0,1.0, 1.0,1.0,1.0,1.0, 1.0,1.0,1.0,1.0, 1.0,1.0,1.0,1.0};
  Matrix v(a);
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::MATRIX );
  DALI_TEST_CHECK( value.Get<Matrix>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsMatrixTypeP(void)
{
  Property::Value value( Property::MATRIX );

  DALI_TEST_CHECK( value.GetType() == Property::MATRIX );
  DALI_TEST_CHECK( value.Get<Matrix>() == Matrix() );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsRectP(void)
{
  Rect<int> v(1.0,1.0,1.0,1.0);
  Property::Value value(v);

  DALI_TEST_EQUALS( value.GetType(), Property::RECTANGLE, TEST_LOCATION );
  DALI_TEST_CHECK( value.Get<Rect<int> >() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsRectTypeP(void)
{
  Property::Value value( Property::RECTANGLE );

  DALI_TEST_CHECK( value.GetType() == Property::RECTANGLE );
  DALI_TEST_CHECK( value.Get<Rect<int> >() == Rect<int>(0,0,0,0) );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsAngleAxisP(void)
{
  AngleAxis input( Dali::ANGLE_90, Vector3::XAXIS );
  Property::Value value( input );

  DALI_TEST_CHECK( value.GetType() == Property::ROTATION );
  AngleAxis result = value.Get<AngleAxis>();
  DALI_TEST_EQUALS( result.angle, input.angle, TEST_LOCATION );
  DALI_TEST_EQUALS( result.axis, input.axis, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsQuaternionP(void)
{
  Quaternion v( Vector4(1.0,1.0,1.0,1.0) );
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::ROTATION );
  DALI_TEST_CHECK( value.Get<Quaternion>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsRotationTypeP(void)
{
  Property::Value value( Property::ROTATION );

  DALI_TEST_CHECK( value.GetType() == Property::ROTATION );
  AngleAxis result = value.Get<AngleAxis>();
  DALI_TEST_EQUALS( result.angle, Radian(0.f), TEST_LOCATION );
  DALI_TEST_EQUALS( result.axis, Vector3::ZERO, TEST_LOCATION ); // identity quaternion returns a zero angle-axis

  END_TEST;
}

int UtcDaliPropertyValueConstructorsStringP(void)
{
  std::string v("1");
  Property::Value value(v);

  DALI_TEST_CHECK( value.GetType() == Property::STRING );
  DALI_TEST_CHECK( value.Get<std::string>() == v );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsStringTypeP(void)
{
  Property::Value value( Property::STRING );

  DALI_TEST_CHECK( value.GetType() == Property::STRING );
  DALI_TEST_CHECK( value.Get<std::string>() == std::string() );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsArrayP(void)
{
  Property::Array foo;
  Property::Value value( foo );

  DALI_TEST_CHECK( value.GetType() == Property::ARRAY );
  DALI_TEST_CHECK( value.Get<Property::Array>().Count() == 0 );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsArray2P(void)
{
  Property::Array foo;
  foo.PushBack( Property::Value() );
  Property::Value value( foo );

  DALI_TEST_CHECK( value.GetType() == Property::ARRAY );
  DALI_TEST_CHECK( value.Get<Property::Array>().Count() == 1 );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsArrayTypeP(void)
{
  Property::Value value(Property::ARRAY);

  DALI_TEST_CHECK( value.GetType() == Property::ARRAY );
  DALI_TEST_CHECK( value.Get<Property::Array>().Count() == 0 );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsMapP(void)
{
  Property::Map map;
  Property::Value value( map );

  DALI_TEST_CHECK( value.GetType() == Property::MAP );
  DALI_TEST_CHECK( value.Get<Property::Map>().Count() == 0 );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsMap2P(void)
{
  Property::Map map;
  map.Insert( "", "" );
  Property::Value value( map );

  DALI_TEST_CHECK( value.GetType() == Property::MAP );
  DALI_TEST_CHECK( value.Get<Property::Map>().Count() == 1 );

  END_TEST;
}

int UtcDaliPropertyValueConstructorsMapTypeP(void)
{
  Property::Value value(Property::MAP);

  DALI_TEST_CHECK( value.GetType() == Property::MAP );
  DALI_TEST_CHECK( value.Get<Property::Map>().Count() == 0 );

  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorP(void)
{
  Property::Value value;
  Property::Value value2( value );
  DALI_TEST_EQUALS( value.GetType(), value2.GetType(), TEST_LOCATION );
  DALI_TEST_EQUALS( value.IsMap(), value2.IsMap(), TEST_LOCATION );
  DALI_TEST_EQUALS( value.IsArray(), value2.IsArray(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorBoolP(void)
{
  CheckCopyCtorP<bool> check(true);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorFloatP(void)
{
  CheckCopyCtorP<float> check(1.f);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorIntP(void)
{
  CheckCopyCtorP<int> check(1);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorUnsignedIntP(void)
{
  CheckCopyCtorP<unsigned int> check(1u);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructoVector2P(void)
{
  CheckCopyCtorP<Vector2> check( Vector2(2,1) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorVector3P(void)
{
  CheckCopyCtorP<Vector3> check( Vector3(3.f,2.f,1.f) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorVector4P(void)
{
  CheckCopyCtorP<Vector3> check( Vector4(4.f,3.f,2.f,1.f) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorMatrix3P(void)
{
  CheckCopyCtorP<Matrix3> check( Matrix3::IDENTITY );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorMatrixP(void)
{
  CheckCopyCtorP<Matrix> check(Matrix::IDENTITY);
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorRectP(void)
{
  CheckCopyCtorP<Rect<int> > check( Rect<int>(1.0,1.0,1.0,1.0) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorAngleAxisP(void)
{
  CheckCopyCtorP<AngleAxis> check( AngleAxis(Degree(1.0), Vector3(1.0,1.0,1.0)) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorQuaternionP(void)
{
  CheckCopyCtorP<Quaternion> check( Quaternion( Vector4(1.0, 1.0, 1.0, 1.0) ) );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorStringP(void)
{
  CheckCopyCtorP<std::string> check( std::string("1") );
  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorArrayP(void)
{
  Property::Value value1(Property::ARRAY);
  Property::Array* array= value1.GetArray();
  array->PushBack(Property::Value(1));

  Property::Value value2( value1 );
  DALI_TEST_EQUALS( value1.GetType(), value2.GetType(), TEST_LOCATION );
  DALI_TEST_EQUALS( value1.GetArray()->Count(), value2.GetArray()->Count(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyValueCopyConstructorMapP(void)
{
  Property::Value value1(Property::MAP);
  Property::Map* map = value1.GetMap();
  (*map)["1"] = Property::Value(1);

  Property::Value value2( value1 );
  DALI_TEST_EQUALS( value1.GetType(), value2.GetType(), TEST_LOCATION );
  DALI_TEST_EQUALS( value1.GetMap()->Count(), value2.GetMap()->Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( value1.GetMap()->GetKey( 0 ), value2.GetMap()->GetKey( 0 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyValueAssignmentSelfP(void)
{
  Property::Value value;
  Property::Value* self = &value;
  value = *self;
  DALI_TEST_EQUALS( value.GetType(), Property::NONE, TEST_LOCATION );
  DALI_TEST_EQUALS( value.IsMap(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( value.IsArray(), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorNoneP(void)
{
  Property::Value value;
  value = Property::Value(); // type match
  DALI_TEST_EQUALS( value.GetType(), Property::NONE, TEST_LOCATION );
  Property::Value copy( false );
  copy = value; // type mismatch
  DALI_TEST_EQUALS( value.GetType(), Property::NONE, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorBoolP(void)
{
  Property::Value value;
  value = Property::Value(true); // type mismatch
  DALI_TEST_CHECK( true == value.Get<bool>() );
  Property::Value copy( false );
  copy = value; // type match
  DALI_TEST_CHECK( true == copy.Get<bool>() );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorIntP(void)
{
  Property::Value value;
  value = Property::Value(10); // type mismatch
  DALI_TEST_CHECK( 10 == value.Get<int>() );
  Property::Value copy( 99 );
  copy = value; // type match
  DALI_TEST_CHECK( 10 == copy.Get<int>() );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorUnsignedIntP(void)
{
  Property::Value value;
  value = Property::Value(10U); // type mismatch
  DALI_TEST_CHECK( 10U == value.Get<unsigned int>() );
  Property::Value copy( 123U);
  copy = value; // type match
  DALI_TEST_CHECK( 10U == copy.Get<unsigned int>() );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorFloatP(void)
{
  Property::Value value;
  value = Property::Value(10.f); // mismatch
  DALI_TEST_CHECK( Dali::Equals(10.f, value.Get<float>() ) );
  Property::Value copy(321.f);
  copy = value; // match
  DALI_TEST_CHECK( Dali::Equals(10.f, copy.Get<float>() ) );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorVector2P(void)
{
  Property::Value value;
  value = Property::Value( Vector2(1,2) ); // mismatch
  DALI_TEST_CHECK( Vector2(1,2) == value.Get<Vector2>() );
  Property::Value copy( Property::VECTOR2 );
  copy = value; // match
  DALI_TEST_CHECK( Vector2(1,2) == copy.Get<Vector2>() );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorVector3P(void)
{
  Property::Value value;
  value = Property::Value( Vector3(1.f,2.f,3.f) ); // mismatch
  DALI_TEST_CHECK( Vector3(1.f,2.f,3.f) == value.Get<Vector3>() );
  Property::Value copy( Property::VECTOR3 );
  copy = value; // match
  DALI_TEST_CHECK( Vector3(1.f,2.f,3.f) == copy.Get<Vector3>() );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorVector4P(void)
{
  Property::Value value;
  value = Property::Value( Vector4(1,2,3,4) ); // mismatch
  DALI_TEST_CHECK( Vector4(1,2,3,4) == value.Get<Vector4>() );
  Property::Value copy( Vector4(0,1,2,3) );
  copy = value; // match
  DALI_TEST_CHECK( Vector4(1,2,3,4) == copy.Get<Vector4>() );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorMatrix3P(void)
{
  Property::Value value;
  value = Property::Value( Matrix3::IDENTITY ); // mismatch
  DALI_TEST_CHECK( Matrix3::IDENTITY == value.Get<Matrix3>() );
  Property::Value copy( Property::MATRIX3 );
  copy = value; // match
  DALI_TEST_CHECK( Matrix3::IDENTITY == copy.Get<Matrix3>() );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorMatrixP(void)
{
  Property::Value value;
  value = Property::Value( Matrix::IDENTITY ); // mismatch
  DALI_TEST_CHECK( Matrix::IDENTITY == value.Get<Matrix>()  );
  Matrix foo;
  Property::Value copy( foo );
  copy = value; // match
  DALI_TEST_CHECK( Matrix::IDENTITY == copy.Get<Matrix>()  );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorRectP(void)
{
  Property::Value value;
  typedef Dali::Rect<int> Rectangle;
  value = Property::Value( Rectangle(4,3,2,1) ); // mismatch
  DALI_TEST_CHECK( Rectangle(4,3,2,1) == value.Get<Rectangle>() );
  Property::Value copy( Property::RECTANGLE );
  copy = value; // match
  Rectangle copyRect;
  copy.Get(copyRect);
  DALI_TEST_CHECK( Rectangle(4,3,2,1) == copyRect );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorQuaternionP(void)
{
  Property::Value value;
  value = Property::Value( Quaternion(1,1,1,1) ); // mismatch
  DALI_TEST_CHECK( Quaternion(1,1,1,1) == value.Get<Quaternion>() );
  Property::Value copy( Property::ROTATION );
  copy = value; // match
  DALI_TEST_CHECK( Quaternion(1,1,1,1) == copy.Get<Quaternion>() );
  END_TEST;
}


int UtcDaliPropertyValueAssignmentOperatorAngleAxisP(void)
{
  Property::Value value;
  value = Property::Value( AngleAxis( Radian(Math::PI_2), Vector3::XAXIS ) ); // mismatch
  DALI_TEST_EQUALS( value.Get<AngleAxis>().axis, Vector3::XAXIS, TEST_LOCATION );
  DALI_TEST_EQUALS( value.Get<AngleAxis>().angle, Radian(Math::PI_2), TEST_LOCATION );
  Property::Value copy( Property::ROTATION );
  copy = value; // match
  DALI_TEST_EQUALS( value.Get<AngleAxis>().axis, copy.Get<AngleAxis>().axis, TEST_LOCATION );
  DALI_TEST_EQUALS( value.Get<AngleAxis>().angle, copy.Get<AngleAxis>().angle, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorStringP(void)
{
  Property::Value value;
  value = Property::Value("yes"); // mismatch
  DALI_TEST_CHECK( "yes" == value.Get<std::string>() );
  Property::Value copy("no");
  copy = value; // match
  DALI_TEST_CHECK( "yes" == copy.Get<std::string>() );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorArrayP(void)
{
  Property::Value value;
  value = Property::Value(Property::ARRAY); // mismatch
  value.GetArray()->PushBack(10);
  DALI_TEST_CHECK( value.GetArray() );
  Property::Value copy(Property::ARRAY);
  copy = value; // match
  Property::Array array;
  copy.Get( array );
  int getItem = 0;
  array[0].Get( getItem );
  DALI_TEST_CHECK( getItem == 10 );
  END_TEST;
}

int UtcDaliPropertyValueAssignmentOperatorMapP(void)
{
  Property::Value value;
  value = Property::Value(Property::MAP); // mismatch
  value.GetMap()->Insert("key", "value");
  Property::Value copy( Property::MAP ); // match
  copy = value;
  Property::Map map;
  copy.Get( map );
  DALI_TEST_CHECK( map.GetKey(0) == "key" );
  END_TEST;
}

int UtcDaliPropertyValueGetTypeP(void)
{
  Property::Value value;
  DALI_TEST_CHECK( value.GetType() == Property::NONE );
  END_TEST;
}

int UtcDaliPropertyValueGetBoolP(void)
{
  Property::Value value(true);
  DALI_TEST_CHECK( value.Get<bool>() == true );
  value = Property::Value(1.f);
  DALI_TEST_CHECK( value.Get<float>() == 1.f );
  END_TEST;
}

int UtcDaliPropertyValueGetBoolN(void)
{
  Property::Value value;
  DALI_TEST_CHECK( value.Get<bool>() == false );
  END_TEST;
}

int UtcDaliPropertyValueGetFloatP(void)
{
  Property::Value value(1.1f);
  DALI_TEST_EQUALS( 1.1f, value.Get<float>(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetFloatN(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( 0.0f, value.Get<float>(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetIntP(void)
{
  Property::Value value(123);
  DALI_TEST_EQUALS( 123, value.Get<int>(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetIntN(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( 0, value.Get<int>(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetUnsignedIntP(void)
{
  Property::Value value(123u);
  DALI_TEST_EQUALS( 123u, value.Get<unsigned int>(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetUnsignedIntN(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( 0u, value.Get<unsigned int>(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetRectP(void)
{
  Property::Value value( Rect<int>(1,2,3,4) );
  DALI_TEST_EQUALS( Rect<int>(1,2,3,4), value.Get< Rect<int> >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetRectN(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( Rect<int>(0,0,0,0), value.Get< Rect<int> >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetVector2P(void)
{
  Property::Value value( Vector2(1.0f,2.0f) );
  DALI_TEST_EQUALS( Vector2(1.0f,2.0f), value.Get< Vector2 >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetVector2N(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( Vector2(0.f,0.f), value.Get< Vector2 >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetVector3P(void)
{
  Property::Value value( Vector3(1.0f,2.0f,-1.f) );
  DALI_TEST_EQUALS( Vector3(1.0f,2.0f,-1.f), value.Get< Vector3 >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetVector3N(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( Vector3(0.f,0.f,0.f), value.Get< Vector3 >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetVector4P(void)
{
  Property::Value value( Vector4(1.f,2.f,-1.f,-3.f) );
  DALI_TEST_EQUALS( Vector4(1.f,2.f,-1.f,-3.f), value.Get< Vector4 >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetVector4N(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( Vector4(0.f,0.f,0.f,0.f), value.Get< Vector4 >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetMatrix3P(void)
{
  Property::Value value( Matrix3(1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f) );
  DALI_TEST_EQUALS( Matrix3(1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f), value.Get< Matrix3 >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetMatrix3N(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( Matrix3::IDENTITY, value.Get< Matrix3 >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetMatrixP(void)
{
  float matrixValues[16] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
  Matrix input( matrixValues );
  Property::Value value( input );
  DALI_TEST_EQUALS( input, value.Get< Matrix >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetMatrixN(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( Matrix::IDENTITY, value.Get< Matrix >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetAngleAxisP(void)
{
  AngleAxis input( Dali::ANGLE_90, Vector3::XAXIS );
  Property::Value value( input );
  AngleAxis result = value.Get<AngleAxis>();
  DALI_TEST_EQUALS( input.angle, result.angle, TEST_LOCATION );
  DALI_TEST_EQUALS( input.axis, result.axis, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetAngleAxisN(void)
{
  Property::Value value;
  AngleAxis b = value.Get<AngleAxis>();
  DALI_TEST_EQUALS( 0.f, b.angle, TEST_LOCATION );
  DALI_TEST_EQUALS( Vector3::ZERO, b.axis, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetQuaternionP(void)
{
  Property::Value value( Quaternion(1.f,2.f,3.f,4.f) );
  DALI_TEST_EQUALS( Quaternion(1.f,2.f,3.f,4.f), value.Get< Quaternion >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetQuaternionN(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( Quaternion::IDENTITY, value.Get< Quaternion >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetStringP(void)
{
  Property::Value value( std::string("hello") );
  DALI_TEST_EQUALS( std::string("hello"), value.Get< std::string >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetStringN(void)
{
  Property::Value value;
  DALI_TEST_EQUALS( std::string(""), value.Get< std::string >(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyValueGetArrayP(void)
{
  Property::Value value( Property::ARRAY );
  DALI_TEST_CHECK( NULL != value.GetArray() );
  value.GetArray()->PushBack( Property::Value(1) );
  Property::Array got = value.Get<Property::Array>();
  DALI_TEST_CHECK( got[0].Get<int>() == 1);
  END_TEST;
}

int UtcDaliPropertyValueGetArrayN(void)
{
  Property::Value value;
  DALI_TEST_CHECK( NULL == value.GetArray() );
  Property::Array array;
  array.PushBack( Property::Value( 10 ) );
  value.Get( array );
  DALI_TEST_CHECK( 0 == array.Count() );
  END_TEST;
}

int UtcDaliPropertyValueGetMapP(void)
{
  Property::Value value(Property::MAP);
  DALI_TEST_CHECK( NULL == value.GetArray() );
  DALI_TEST_CHECK( NULL != value.GetMap() );
  value.GetMap()->Insert("key", Property::Value(1));
  Property::Map got = value.Get<Property::Map>();
  DALI_TEST_CHECK(got.Find("key")->Get<int>() == 1);
  END_TEST;
}

int UtcDaliPropertyValueGetMapN(void)
{
  Property::Value value;
  DALI_TEST_CHECK( NULL == value.GetMap() );
  Property::Map map = value.Get<Property::Map>();
  DALI_TEST_CHECK( 0 == map.Count() );
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
    DALI_TEST_CHECK( stream.str() == "1" )
  }

  {
    Property::Value empty;
    std::ostringstream stream;
    stream << empty;
    DALI_TEST_EQUALS( stream.str(), "empty type", TEST_LOCATION );
  }

  {
    Property::Value empty( Property::NONE );
    std::ostringstream stream;
    stream << empty;
    DALI_TEST_CHECK( stream.str() == "undefined type" )
  }

  {
    Property::Value empty( Property::TYPE_COUNT );
    std::ostringstream stream;
    stream << empty;
    DALI_TEST_CHECK( stream.str() == "undefined type" )
  }

  {
    value = Property::Value(20.2f);
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "20.2" )
  }

  {
    value = Property::Value(-25);
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "-25" )
  }

  {
    value = Property::Value(25U);
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "25" )
  }

  {
    value = Property::Value( Vector2(1.f,1.f) );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "[1, 1]" );
  }

  {
    value = Property::Value( Vector3(1.f,1.f,1.f) );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "[1, 1, 1]" );
  }

  {
    value = Property::Value( Vector4(-4.f,-3.f,-2.f,-1.f) );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_EQUALS( stream.str(), "[-4, -3, -2, -1]", TEST_LOCATION );
  }

  {
    value = Property::Value( Matrix3::IDENTITY );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "[ [1, 0, 0], [0, 1, 0], [0, 0, 1] ]" );
  }

  {
    value = Property::Value( Matrix::IDENTITY );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "[ [1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1] ]" );
  }

  {
    value = Property::Value( Rectangle(1,2,3,4) );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "[1, 2, 3, 4]" );
  }

  {
    value = Property::Value( AngleAxis( Dali::ANGLE_120, Vector3::XAXIS ) );
    std::ostringstream stream;
    stream <<  value;
    tet_printf("angle axis = %s \n", stream.str().c_str() );
    DALI_TEST_EQUALS( stream.str(), "[ Axis: [1, 0, 0], Angle: 120 degrees ]", TEST_LOCATION );
  }

  {
    value = Property::Value( std::string( "Foo" ) );
    std::ostringstream stream;
    stream <<  value;
    DALI_TEST_CHECK( stream.str() == "Foo" );
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
