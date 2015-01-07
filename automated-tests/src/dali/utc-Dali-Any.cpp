/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <stdlib.h>

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

// Temp include
#include <dali/public-api/object/any.h>

namespace
{
struct MyStruct
{
  MyStruct()
  : mFloatValue( 0.f ),
    mIntValue( 0 )
  {}

  MyStruct( float fValue, int iValue )
  : mFloatValue( fValue ),
    mIntValue( iValue )
  {}

  MyStruct( const MyStruct& myStruct )
  : mFloatValue( myStruct.mFloatValue ),
    mIntValue( myStruct.mIntValue )
  {}

  MyStruct& operator=( const MyStruct& myStruct )
  {
    mFloatValue = myStruct.mFloatValue;
    mIntValue = myStruct.mIntValue;

    return *this;
  }

  float mFloatValue;
  int mIntValue;
};
}

using namespace Dali;

void utc_dali_any_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_any_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliAnyConstructors(void)
{
  TestApplication application;

  tet_infoline("Test Any constructors.");

  // Test default constructor.
  Any value;

  DALI_TEST_CHECK( typeid( void ) == value.GetType() );

  // Test constructor Any( const Type& )
  Any value1 = 4u;

  // Test constructor Any( const Any& )
  Any value2 = value1;

  // Test constructor Any( const Any& ) with a non initialized Any
  Any value3 = value;

  DALI_TEST_CHECK( typeid( unsigned int ) == value1.GetType() );
  DALI_TEST_CHECK( typeid( unsigned int ) == value2.GetType() );
  DALI_TEST_CHECK( typeid( void ) == value3.GetType() );

  unsigned int uiValue1 = 0u;
  unsigned int uiValue2 = 0u;
  value1.Get( uiValue1 );
  value2.Get( uiValue2 );

  DALI_TEST_EQUALS( uiValue1, uiValue2, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnyAssignmentOperators(void)
{
  TestApplication application;

  tet_infoline("Test assignment operators.");

  float fValue = 0.f;

  Any value1;

  value1 = 4.f; // Test operator=( const Type& ) when current object is not initialized.

  value1.Get( fValue );

  DALI_TEST_EQUALS( fValue, 4.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  Any value2 = 0.f;

  value2 = 9.f; // Test operator=( const Type& ).

  value2.Get( fValue );

  DALI_TEST_EQUALS( fValue, 9.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  Any value3 = 5.f;

  value1 = value3; // Test operator=( const Any& ).

  value1.Get( fValue );

  DALI_TEST_EQUALS( fValue, 5.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  Any value4;

  value4 = value3; // Test operator=( const Any& ) when current object is not initialized.

  value4.Get( fValue );

  DALI_TEST_EQUALS( fValue, 5.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test assign a value to value3 doesn't modify value1.
  value3 = 3.f;

  value1.Get( fValue );

  DALI_TEST_EQUALS( fValue, 5.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  value3.Get( fValue );

  DALI_TEST_EQUALS( fValue, 3.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test the branch when copying the same object.
  Any value5 = 3.f;
  Any& value6( value5 );

  value6 = value5;

  value6.Get( fValue );
  DALI_TEST_EQUALS( fValue, 3.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnyNegativeAssignmentOperators(void)
{
  TestApplication application;

  tet_infoline("Test assignment operators.");

  Any value1 = 4.f;
  Any value2 = 5u;

  bool assert = false;

  try
  {
    value1 = value2; // Test operator=( const Any& );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    assert = true;
  }

  if( assert )
  {
    tet_result( TET_PASS );
  }
  else
  {
    tet_result( TET_FAIL );
  }
  END_TEST;
}

int UtcDaliAnyGetType(void)
{
  TestApplication application;

  tet_infoline("Test GetType().");

  Any value;

  DALI_TEST_CHECK( typeid( void ) == value.GetType() );

  value = 5.f;

  DALI_TEST_CHECK( typeid( float ) == value.GetType() );
  END_TEST;
}

int UtcDaliAnyGet(void)
{
  TestApplication application;

  tet_infoline("Test Get().");

  Any value1( 5.f );

  float fValue = value1.Get<float>();

  DALI_TEST_EQUALS( fValue, 5.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  fValue = 0.f;
  value1.Get( fValue );
  DALI_TEST_EQUALS( fValue, 5.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  class MyClass
  {
  public:
    MyClass( float fValue, int iValue )
    : mAny( MyStruct( fValue, iValue ) )
    {
    }

    const MyStruct& Get() const
    {
      return AnyCastReference<MyStruct>( mAny );
    }

    MyStruct* GetPointer()
    {
      return AnyCast<MyStruct>( &mAny );
    }

    const MyStruct* GetPointer() const
    {
      return AnyCast<MyStruct>( &mAny );
    }

  private:
    Dali::Any mAny;
  };

  MyClass myClass( 3.25f, 3 );

  MyStruct myStruct1 = myClass.Get();
  const MyStruct& myStruct2 = myClass.Get();
  MyStruct* myStruct3 = myClass.GetPointer();
  const MyStruct* myStruct4 = myClass.GetPointer();

  DALI_TEST_EQUALS( myStruct1.mFloatValue, 3.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( myStruct2.mFloatValue, 3.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( myStruct3->mFloatValue, 3.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( myStruct4->mFloatValue, 3.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( myStruct1.mIntValue, 3, TEST_LOCATION );
  DALI_TEST_EQUALS( myStruct2.mIntValue, 3, TEST_LOCATION );
  DALI_TEST_EQUALS( myStruct3->mIntValue, 3, TEST_LOCATION );
  DALI_TEST_EQUALS( myStruct4->mIntValue, 3, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnyNegativeGet(void)
{
  TestApplication application;
  tet_infoline("Test Get().");

  Any value1;
  Any value2( 5.f );

  bool assert1 = false;
  bool assert2 = false;

  unsigned int uiValue = 0u;

  try
  {
    uiValue = value1.Get< unsigned int >();
  }

  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    assert1 = true;
  }

  try
  {
    uiValue = value2.Get< unsigned int >();
  }

  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    assert2 = true;
  }

  if( assert1 && assert2 )
  {
    tet_result( TET_PASS );
  }
  else
  {
    tet_result( TET_FAIL );
  }
  uiValue++;  // supresss warning from unused variable
  END_TEST;
}
