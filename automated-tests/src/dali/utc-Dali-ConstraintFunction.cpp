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

using namespace Dali;

///////////////////////////////////////////////////////////////////////////////
void utc_dali_constraint_function_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_constraint_function_cleanup(void)
{
  test_return_value = TET_PASS;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace
{
bool gFunctionCalled = false;

template< typename T >
void TestCallbackFunction( T& /* current*/ , const PropertyInputContainer& /* inputs */ )
{
  gFunctionCalled = true;
}

template< typename T >
struct TestCallbackFunctor
{
  TestCallbackFunctor( bool& functorCalled ) : mFunctorCalled( functorCalled ) { }

  void operator()( T& /* current*/ , const PropertyInputContainer& /* inputs */ )
  {
    mFunctorCalled = true;
  }

  bool& mFunctorCalled;
};

template< typename T >
struct TestFunctorMethod
{
  TestFunctorMethod( bool& functorCalled ) : mFunctorCalled( functorCalled ) { }

  void Method( T& /* current*/ , const PropertyInputContainer& /* inputs */ )
  {
    mFunctorCalled = true;
  }

  bool& mFunctorCalled;
};

} // unnamed namespace
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::Function::Function( void( *function )( P&, const PropertyInputContainer& ) )
///////////////////////////////////////////////////////////////////////////////
namespace
{
template< typename T >
void TestFunctionConstructor()
{
  gFunctionCalled = false;
  Constraint::Function< T > function( &TestCallbackFunction< T > );
  T current;
  PropertyInputContainer inputs;

  DALI_TEST_EQUALS( gFunctionCalled, false, TEST_LOCATION );
  CallbackBase::Execute< T&, const PropertyInputContainer& >( function, current, inputs );
  DALI_TEST_EQUALS( gFunctionCalled, true, TEST_LOCATION );
}
} // unnamed namespace

int UtcDaliConstraintFunctionWithFunction(void)
{
  TestFunctionConstructor< bool >();
  TestFunctionConstructor< int >();
  TestFunctionConstructor< unsigned int >();
  TestFunctionConstructor< float >();
  TestFunctionConstructor< Vector2 >();
  TestFunctionConstructor< Vector3 >();
  TestFunctionConstructor< Vector4 >();
  TestFunctionConstructor< Quaternion >();
  TestFunctionConstructor< Matrix >();
  TestFunctionConstructor< Matrix3 >();
  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::Function::Function( const T& object )
///////////////////////////////////////////////////////////////////////////////
namespace
{
template< typename T >
void TestFunctorConstructor()
{
  bool called = false;
  TestCallbackFunctor< T > functor( called );
  Constraint::Function< T > callback( functor );
  T current;
  PropertyInputContainer inputs;

  DALI_TEST_EQUALS( called, false, TEST_LOCATION );
  CallbackBase::Execute< T&, const PropertyInputContainer& >( callback, current, inputs );
  DALI_TEST_EQUALS( called, true, TEST_LOCATION );
}
} // unnamed namespace

int UtcDaliConstraintFunctionWithFunctor(void)
{
  TestFunctorConstructor< bool >();
  TestFunctorConstructor< int >();
  TestFunctorConstructor< unsigned int >();
  TestFunctorConstructor< float >();
  TestFunctorConstructor< Vector2 >();
  TestFunctorConstructor< Vector3 >();
  TestFunctorConstructor< Vector4 >();
  TestFunctorConstructor< Quaternion >();
  TestFunctorConstructor< Matrix >();
  TestFunctorConstructor< Matrix3 >();
  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::Function::Function( const T& object, void ( T::*memberFunction ) ( P&, const PropertyInputContainer& ) )
///////////////////////////////////////////////////////////////////////////////
namespace
{
template< typename T >
void TestFunctorMethodConstructor()
{
  bool called = false;
  TestFunctorMethod< T > functor( called );
  Constraint::Function< T > callback( functor, &TestFunctorMethod< T >::Method );
  T current;
  PropertyInputContainer inputs;

  DALI_TEST_EQUALS( called, false, TEST_LOCATION );
  CallbackBase::Execute< T&, const PropertyInputContainer& >( callback, current, inputs );
  DALI_TEST_EQUALS( called, true, TEST_LOCATION );
}
} // unnamed namespace

int UtcDaliConstraintFunctionWithMethodFunctor(void)
{
  TestFunctorMethodConstructor< bool >();
  TestFunctorMethodConstructor< int >();
  TestFunctorMethodConstructor< unsigned int >();
  TestFunctorMethodConstructor< float >();
  TestFunctorMethodConstructor< Vector2 >();
  TestFunctorMethodConstructor< Vector3 >();
  TestFunctorMethodConstructor< Vector4 >();
  TestFunctorMethodConstructor< Quaternion >();
  TestFunctorMethodConstructor< Matrix >();
  TestFunctorMethodConstructor< Matrix3 >();
  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::Function::Clone
///////////////////////////////////////////////////////////////////////////////
namespace
{
template< typename T >
void TestFunctionClone()
{
  gFunctionCalled = false;
  Constraint::Function< T > callback( &TestCallbackFunction< T > );
  CallbackBase* clone = callback.Clone();

  T current;
  PropertyInputContainer inputs;

  DALI_TEST_EQUALS( gFunctionCalled, false, TEST_LOCATION );
  CallbackBase::Execute< T&, const PropertyInputContainer& >( *clone, current, inputs );
  DALI_TEST_EQUALS( gFunctionCalled, true, TEST_LOCATION );
  delete clone;
}

template< typename T >
void TestFunctorClone()
{
  bool called = false;
  TestCallbackFunctor< T > functor( called );
  Constraint::Function< T > callback( functor );
  CallbackBase* clone = callback.Clone();

  T current;
  PropertyInputContainer inputs;

  DALI_TEST_EQUALS( called, false, TEST_LOCATION );
  CallbackBase::Execute< T&, const PropertyInputContainer& >( *clone, current, inputs );
  DALI_TEST_EQUALS( called, true, TEST_LOCATION );
  delete clone;
}

template< typename T >
void TestMethodFunctorClone()
{
  bool called = false;
  TestFunctorMethod< T > functor( called );
  Constraint::Function< T > callback( functor, &TestFunctorMethod< T >::Method );
  CallbackBase* clone = callback.Clone();

  T current;
  PropertyInputContainer inputs;

  DALI_TEST_EQUALS( called, false, TEST_LOCATION );
  CallbackBase::Execute< T&, const PropertyInputContainer& >( *clone, current, inputs );
  DALI_TEST_EQUALS( called, true, TEST_LOCATION );
  delete clone;
}

} // unnamed namespace

int UtcDaliConstraintFunctionFunctionClone(void)
{
  TestFunctionClone< bool >();
  TestFunctionClone< int >();
  TestFunctionClone< unsigned int >();
  TestFunctionClone< float >();
  TestFunctionClone< Vector2 >();
  TestFunctionClone< Vector3 >();
  TestFunctionClone< Vector4 >();
  TestFunctionClone< Quaternion >();
  TestFunctionClone< Matrix >();
  TestFunctionClone< Matrix3 >();
  END_TEST;
}

int UtcDaliConstraintFunctionFunctorClone(void)
{
  TestFunctorClone< bool >();
  TestFunctorClone< int >();
  TestFunctorClone< unsigned int >();
  TestFunctorClone< float >();
  TestFunctorClone< Vector2 >();
  TestFunctorClone< Vector3 >();
  TestFunctorClone< Vector4 >();
  TestFunctorClone< Quaternion >();
  TestFunctorClone< Matrix >();
  TestFunctorClone< Matrix3 >();
  END_TEST;
}

int UtcDaliConstraintFunctionMethodFunctorClone(void)
{
  TestMethodFunctorClone< bool >();
  TestMethodFunctorClone< int >();
  TestMethodFunctorClone< unsigned int >();
  TestMethodFunctorClone< float >();
  TestMethodFunctorClone< Vector2 >();
  TestMethodFunctorClone< Vector3 >();
  TestMethodFunctorClone< Vector4 >();
  TestMethodFunctorClone< Quaternion >();
  TestMethodFunctorClone< Matrix >();
  TestMethodFunctorClone< Matrix3 >();
  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace
{
struct CountFunctor
{
  CountFunctor( int& count )
  : mCount( count )
  {
    ++mCount;
  }

  CountFunctor( const CountFunctor& other )
  : mCount( other.mCount )
  {
    ++mCount;
  }

  CountFunctor& operator=( const CountFunctor& other )
  {
    return *this;
  }

  ~CountFunctor()
  {
    --mCount;
  }

  void operator()( bool& /* current*/ , const PropertyInputContainer& /* inputs */ )
  {
  }

  int& mCount;
};
} // unnamed namespace

int UtcDaliConstraintFunctionEnsureMemoryCleanup(void)
{
  // Functors are new'd in Constraint::Function, so check that all memory is released at the end

  int count = 0;

  {
    CountFunctor functor( count );
    Constraint::Function< bool > callback1( functor );
    Constraint::Function< bool > callback2( functor );
    Constraint::Function< bool > callback3( functor );
    Constraint::Function< bool > callback4( functor );
    Constraint::Function< bool > callback5( functor );
    Constraint::Function< bool > callback6( functor );
    Constraint::Function< bool > callback7( functor );
    Constraint::Function< bool > callback8( functor );
    Constraint::Function< bool > callback9( functor );
    DALI_TEST_EQUALS( count, 10, TEST_LOCATION );
  }

  DALI_TEST_EQUALS( count, 0, TEST_LOCATION );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////
