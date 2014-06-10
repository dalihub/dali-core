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

#include <iostream>
#include <stdlib.h>
#include <dali/dali.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{

const Dali::VectorBase::SizeType ZERO(0);

}


int UtcDaliEmptyVectorInt(void)
{
  tet_infoline("Testing Dali::Vector<int>");

  Vector< int > intvector;

  DALI_TEST_EQUALS( ZERO, intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intvector.Capacity(), TEST_LOCATION );

  intvector.Clear();
  DALI_TEST_EQUALS( ZERO, intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intvector.Capacity(), TEST_LOCATION );

  intvector.Release();
  DALI_TEST_EQUALS( ZERO, intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intvector.Capacity(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliVectorInt(void)
{
  tet_infoline("Testing Dali::Vector<int>");

  Vector< int > intvector;

  DALI_TEST_EQUALS( ZERO, intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intvector.Capacity(), TEST_LOCATION );

  intvector.PushBack( 11 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(1), intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(2), intvector.Capacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( 11, intvector[ 0 ], TEST_LOCATION );

  intvector.PushBack( 99 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(2), intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(2), intvector.Capacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( 99, intvector[ 1 ], TEST_LOCATION );

  intvector.PushBack( 34 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(3), intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(6), intvector.Capacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( 11, intvector[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 99, intvector[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 34, intvector[ 2 ], TEST_LOCATION );

  intvector.Clear();
  DALI_TEST_EQUALS( ZERO, intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(6), intvector.Capacity(), TEST_LOCATION );
  intvector.PushBack( 123 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(1), intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 123, intvector[ 0 ], TEST_LOCATION );
  END_TEST;
}

int UtcDaliVectorIntCopy(void)
{
  tet_infoline("Testing Dali::Vector<int>::Copy");

  Vector< int > intvector;
  DALI_TEST_EQUALS( ZERO, intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intvector.Capacity(), TEST_LOCATION );

  intvector.PushBack( 99 );
  intvector.PushBack( 11 );
  intvector.PushBack( 34 );

  // copy construct
  Vector< int > intvector2( intvector );

  DALI_TEST_EQUALS( intvector2.Count(), intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( intvector2.Capacity(), intvector.Capacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( intvector2[ 0 ], intvector[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( intvector2[ 1 ], intvector[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( intvector2[ 2 ], intvector[ 2 ], TEST_LOCATION );

  // assign
  Vector< int > intvector3;
  DALI_TEST_EQUALS( ZERO, intvector3.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intvector3.Capacity(), TEST_LOCATION );
  intvector2 = intvector3;
  DALI_TEST_EQUALS( intvector2.Count(), intvector3.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( intvector2.Capacity(), intvector3.Capacity(), TEST_LOCATION );

  // copy empty
  Vector< int > intvector4;
  intvector4.Reserve( 100 );
  DALI_TEST_EQUALS( ZERO, intvector4.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(100), intvector4.Capacity(), TEST_LOCATION );
  intvector3 = intvector4;
  DALI_TEST_EQUALS( ZERO, intvector3.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(100), intvector3.Capacity(), TEST_LOCATION );

  // self copy
  intvector4 = intvector4;
  DALI_TEST_EQUALS( ZERO, intvector4.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(100), intvector4.Capacity(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliVectorIntResize(void)
{
  tet_infoline("Testing Dali::Vector<short>::Resize");

  Vector< short > vector;
  DALI_TEST_EQUALS( ZERO, vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, vector.Capacity(), TEST_LOCATION );

  vector.Resize( 10u );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(10), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(10), vector.Capacity(), TEST_LOCATION );

  vector.Resize( 4u );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(4), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(10), vector.Capacity(), TEST_LOCATION );

  vector.Resize( 4u );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(4), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(10), vector.Capacity(), TEST_LOCATION );

  vector.Resize( 0u );
  DALI_TEST_EQUALS( ZERO, vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(10), vector.Capacity(), TEST_LOCATION );

  vector.Resize( 12u, 123 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(12), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(12), vector.Capacity(), TEST_LOCATION );

  DALI_TEST_EQUALS( vector[ 0 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 1 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 2 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 3 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 4 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 5 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 6 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 7 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 8 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 9 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 10 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 11 ], 123, TEST_LOCATION );

  vector.Resize( 13u, 321 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(13), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(13), vector.Capacity(), TEST_LOCATION );

  DALI_TEST_EQUALS( vector[ 0 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 1 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 2 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 3 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 4 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 5 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 6 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 7 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 8 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 9 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 10 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 11 ], 123, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 12 ], 321, TEST_LOCATION );
  END_TEST;
}

int UtcDaliVectorIntErase(void)
{
  tet_infoline("Testing Dali::Vector<short>::Erase");

  Vector< char > vector;
  DALI_TEST_EQUALS( ZERO, vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, vector.Capacity(), TEST_LOCATION );
  vector.PushBack( 1 );
  vector.PushBack( 2 );
  vector.PushBack( 3 );
  vector.PushBack( 4 );
  vector.PushBack( 5 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(5), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 1, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 1 ], 2, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 2 ], 3, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 3 ], 4, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 4 ], 5, TEST_LOCATION );

  vector.Erase( vector.Begin() );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(4), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 2, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 1 ], 3, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 2 ], 4, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 3 ], 5, TEST_LOCATION );

  Vector< char >::Iterator ret = vector.Erase( std::find( vector.Begin(), vector.End(), 4 ) );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(3), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 2, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 1 ], 3, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 2 ], 5, TEST_LOCATION );
  DALI_TEST_EQUALS( *ret, 5, TEST_LOCATION );

  // try erasing last
  vector.PushBack( 99 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(4), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 3 ], 99, TEST_LOCATION );
  ret = vector.Erase( vector.End() - 1 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(3), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ret, vector.End(), TEST_LOCATION );

  try
  {
    // illegal erase, one past the end
    vector.Erase( vector.End() );
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "(iterator < End())", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(3), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 2, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 1 ], 3, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 2 ], 5, TEST_LOCATION );

  vector.Erase( vector.Begin() + 1 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(2), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 2, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 1 ], 5, TEST_LOCATION );

  vector.Erase( vector.Begin() + 1 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(1), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 2, TEST_LOCATION );

  try
  {
    // illegal erase, one past the end
    vector.Erase( vector.Begin() + 1 );
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "(iterator < End())", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(1), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 2, TEST_LOCATION );

  vector.Erase( vector.Begin() );
  DALI_TEST_EQUALS( ZERO, vector.Count(), TEST_LOCATION );

  try
  {
    // illegal erase, one before the beginning
    vector.Erase( vector.Begin() - 1 );
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "(iterator < End())", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  DALI_TEST_EQUALS( ZERO, vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.Begin(), vector.End(), TEST_LOCATION );

  Vector< char >::Iterator endIter = vector.End();
  for( Vector< char >::Iterator iter = vector.Begin(); iter != endIter; ++iter )
  {
    tet_result(TET_FAIL);
  }

  vector.PushBack( 3 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(1), vector.Count(), TEST_LOCATION );

  vector.Clear();
  DALI_TEST_EQUALS( ZERO, vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.Begin(), vector.End(), TEST_LOCATION );

  endIter = vector.End();
  for( Vector< char >::Iterator iter = vector.Begin(); iter != endIter; ++iter )
  {
    tet_result(TET_FAIL);
  }

  // test a vector of pointers
  Vector< int* > ptrVector;
  DALI_TEST_EQUALS( ZERO, ptrVector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ptrVector.Begin(), ptrVector.End(), TEST_LOCATION );

  int* pointer = NULL;
  ptrVector.PushBack( pointer );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(1), ptrVector.Count(), TEST_LOCATION );

  Vector< int* >::Iterator ptriter = std::find( ptrVector.Begin(), ptrVector.End(), pointer );
  ptriter = ptrVector.Erase( ptriter );
  DALI_TEST_EQUALS( ZERO, ptrVector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ptrVector.Begin(), ptrVector.End(), TEST_LOCATION );
  DALI_TEST_EQUALS( ptrVector.Begin(), ptriter, TEST_LOCATION );
  END_TEST;
}


int UtcDaliVectorDoubleRemove(void)
{
  tet_infoline("Testing Dali::Vector<double>::Remove");

  Vector< double > vector;
  DALI_TEST_EQUALS( ZERO, vector.Count(), TEST_LOCATION );

  vector.PushBack( 11.1 );
  vector.PushBack( 22.2 );
  vector.PushBack( 33.3 );
  vector.PushBack( 44.4 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(4), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 11.1, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 1 ], 22.2, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 2 ], 33.3, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 3 ], 44.4, TEST_LOCATION );

  Vector< double >::Iterator res = std::find( vector.Begin(), vector.End(), 22.2 );
  DALI_TEST_EQUALS( 22.2, *res, TEST_LOCATION );
  vector.Remove( res );
  res = std::find( vector.Begin(), vector.End(), 22.2 );
  DALI_TEST_EQUALS( vector.End(), res, TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(3), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 11.1, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 1 ], 44.4, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 2 ], 33.3, TEST_LOCATION );

  vector.Remove( vector.End() - 1 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(2), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 11.1, TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 1 ], 44.4, TEST_LOCATION );

  vector.Remove( vector.Begin() );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(1), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 44.4, TEST_LOCATION );

  try
  {
    // illegal erase, one past the end
    vector.Remove( vector.Begin() + 1 );
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "(iterator < end)", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(1), vector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( vector[ 0 ], 44.4, TEST_LOCATION );

  vector.Remove( vector.Begin() );
  DALI_TEST_EQUALS( ZERO, vector.Count(), TEST_LOCATION );

  try
  {
    // illegal erase, one before the beginning
    vector.Remove( vector.Begin() - 1 );
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException& e )
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "(iterator < end) && (iterator >= Begin()", TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliVectorIntSwap(void)
{
  tet_infoline("Testing Dali::Vector<int>::Swap");

  Vector< int > intvector;
  DALI_TEST_EQUALS( ZERO, intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intvector.Capacity(), TEST_LOCATION );

  intvector.PushBack( 11 );
  intvector.PushBack( 22 );
  intvector.PushBack( 33 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(3), intvector.Count(), TEST_LOCATION );

  Vector< int > intvector2;
  DALI_TEST_EQUALS( ZERO, intvector2.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intvector2.Capacity(), TEST_LOCATION );

  intvector2.Swap( intvector );
  DALI_TEST_EQUALS( ZERO, intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intvector.Capacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(3), intvector2.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 11, intvector2[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 22, intvector2[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 33, intvector2[ 2 ], TEST_LOCATION );

  intvector.PushBack( 99 );
  intvector.PushBack( 88 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(2), intvector.Count(), TEST_LOCATION );

  intvector.Swap( intvector2 );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(2), intvector2.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 99, intvector2[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 88, intvector2[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(3), intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 11, intvector[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 22, intvector[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 33, intvector[ 2 ], TEST_LOCATION );

  Vector< int > empty;
  intvector.Swap( empty );
  DALI_TEST_EQUALS( ZERO, intvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intvector.Capacity(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliVectorIterate(void)
{
  tet_infoline("Testing Dali::Vector<float>::Begin");

  Vector< float > floatvector;
  DALI_TEST_EQUALS( ZERO, floatvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, floatvector.Capacity(), TEST_LOCATION );

  floatvector.PushBack( 0.9f );
  floatvector.PushBack( 1.1f );
  floatvector.PushBack( 1.2f );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(3), floatvector.Count(), TEST_LOCATION );

  Vector< float >::Iterator iter = floatvector.Begin();
  int index = 0;
  for( ; iter != floatvector.End(); ++iter, ++index )
  {
    std::cout << "value " << *iter << std::endl;
    DALI_TEST_EQUALS( *iter, floatvector[ index ], TEST_LOCATION );
  }
  DALI_TEST_EQUALS( 3, index, TEST_LOCATION );

  iter = std::find( floatvector.Begin(), floatvector.End(), 1.1f );
  DALI_TEST_EQUALS( 1.1f, *iter, TEST_LOCATION );

  floatvector.Clear();
  iter = std::find( floatvector.Begin(), floatvector.End(), 1.1f );
  DALI_TEST_EQUALS( floatvector.End(), iter, TEST_LOCATION );
  END_TEST;
}

int UtcDaliVectorPair(void)
{
  tet_infoline("Testing Dali::Vector< std::pair< int, float > >");

  Vector< std::pair< int, float > > pairvector;
  DALI_TEST_EQUALS( ZERO, pairvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, pairvector.Capacity(), TEST_LOCATION );

  pairvector.PushBack( std::make_pair( 5, 0.1f ) );
  pairvector.PushBack( std::make_pair( 3, 0.2f ) );
  pairvector.PushBack( std::make_pair( 4, 0.3f ) );
  pairvector.PushBack( std::make_pair( 1, 0.4f ) );
  pairvector.PushBack( std::make_pair( 2, 0.5f ) );
  DALI_TEST_EQUALS( static_cast<Dali::VectorBase::SizeType>(5), pairvector.Count(), TEST_LOCATION );

  Vector< std::pair< int, float > >::Iterator iter = pairvector.Begin();
  int index = 0;
  for( ; iter != pairvector.End(); ++iter, ++index )
  {
    std::cout << "pair " << (*iter).first << ":" << (*iter).second << std::endl;
    DALI_TEST_EQUALS( (*iter).first, pairvector[ index ].first, TEST_LOCATION );
    DALI_TEST_EQUALS( (*iter).second, pairvector[ index ].second, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliVectorAsserts(void)
{
  tet_infoline("Testing Dali::Vector< int* > exception handling");

  // empty vector
  Vector< int* > pointervector;
  try
  {
    int* value = NULL;
    pointervector[ 1 ] = value;
    tet_printf("Assertion expected, but not occurred at %s\n", TEST_LOCATION );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "VectorBase::mData", TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  try
  {
    int* value = NULL;
    value = pointervector[ 0 ];
    (void)value; // to "use" the value
    tet_printf("Assertion expected, but not occurred at %s\n", TEST_LOCATION );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "VectorBase::mData", TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  Vector< int* >::Iterator iter = pointervector.Begin();
  if( iter != pointervector.End() )
  {
    tet_result(TET_FAIL);
  }

  try
  {
    pointervector.Erase( pointervector.Begin() );
    tet_printf("Assertion expected, but not occurred at %s\n", TEST_LOCATION );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "VectorBase::mData", TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  iter = pointervector.Begin();
  if( iter != pointervector.End() )
  {
    tet_result(TET_FAIL);
  }

  try
  {
    pointervector.Remove( pointervector.Begin() );
    tet_printf("Assertion expected, but not occurred at %s\n", TEST_LOCATION );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "VectorBase::mData", TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  iter = pointervector.Begin();
  if( iter != pointervector.End() )
  {
    tet_result(TET_FAIL);
  }

  // reserve 0 space
  pointervector.Reserve( 0 );
  iter = pointervector.Begin();
  if( iter != pointervector.End() )
  {
    tet_result(TET_FAIL);
  }

  // reserve 1 space
  pointervector.Reserve( 1 );
  iter = pointervector.Begin();
  if( iter != pointervector.End() )
  {
    tet_result(TET_FAIL);
  }

  try
  {
    int* value = NULL;
    pointervector[ 1 ] = value;
    tet_printf("Assertion expected, but not occurred at %s\n", TEST_LOCATION );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "index < VectorBase::Count()", TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  try
  {
    int* value = pointervector[ 1 ];
    (void)value; // to "use" the value
    tet_printf("Assertion expected, but not occurred at %s\n", TEST_LOCATION );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "index < VectorBase::Count()", TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliVectorAcidTest(void)
{
  tet_infoline("Testing multiple Dali::Vector's");

  // create multiple vectors
  Vector< std::pair< float, float > > pairvector;
  DALI_TEST_EQUALS( ZERO, pairvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, pairvector.Capacity(), TEST_LOCATION );
  Vector< double > doublevector;
  DALI_TEST_EQUALS( ZERO, doublevector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, doublevector.Capacity(), TEST_LOCATION );
  Vector< int* > intptrvector;
  DALI_TEST_EQUALS( ZERO, intptrvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intptrvector.Capacity(), TEST_LOCATION );
  Vector< Dali::Actor* > actorptrvector;
  DALI_TEST_EQUALS( ZERO, actorptrvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, actorptrvector.Capacity(), TEST_LOCATION );
  Vector< long > longvector;
  DALI_TEST_EQUALS( ZERO, longvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, longvector.Capacity(), TEST_LOCATION );
  Vector< char > charvector;
  DALI_TEST_EQUALS( ZERO, charvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, charvector.Capacity(), TEST_LOCATION );

  // add items
  static unsigned int acidCount = 10000;
  int* ptr = NULL;
  for( unsigned int i = 0; i < acidCount; ++i )
  {
    ++ptr;
    pairvector.PushBack( std::make_pair( i, i ) );
    doublevector.PushBack( (double)i );
    intptrvector.PushBack( (int*)ptr );
    actorptrvector.PushBack( (Dali::Actor*)ptr );
    longvector.PushBack( (long)i );
    charvector.PushBack( (char)i );
  }
  DALI_TEST_EQUALS( acidCount, pairvector.Count(), TEST_LOCATION );
  std::size_t pairCapacity = pairvector.Capacity();
  DALI_TEST_EQUALS( acidCount, doublevector.Count(), TEST_LOCATION );
  std::size_t doubleCapacity = doublevector.Capacity();
  DALI_TEST_EQUALS( acidCount, intptrvector.Count(), TEST_LOCATION );
  std::size_t intptrCapacity = intptrvector.Capacity();
  DALI_TEST_EQUALS( acidCount, actorptrvector.Count(), TEST_LOCATION );
  std::size_t actorptrCapacity = actorptrvector.Capacity();
  DALI_TEST_EQUALS( acidCount, longvector.Count(), TEST_LOCATION );
  std::size_t longCapacity = longvector.Capacity();
  DALI_TEST_EQUALS( acidCount, charvector.Count(), TEST_LOCATION );
  std::size_t charCapacity = charvector.Capacity();

  tet_printf("Dali::Vector< pair > capacity after %d pushbacks is %d", acidCount, pairCapacity );
  tet_printf("Dali::Vector< double > capacity after %d pushbacks is %d", acidCount, doubleCapacity );
  tet_printf("Dali::Vector< int* > capacity after %d pushbacks is %d", acidCount, intptrCapacity );
  tet_printf("Dali::Vector< Actor* > capacity after %d pushbacks is %d", acidCount, actorptrCapacity );
  tet_printf("Dali::Vector< long > capacity after %d pushbacks is %d", acidCount, longCapacity );
  tet_printf("Dali::Vector< char > capacity after %d pushbacks is %d", acidCount, charCapacity );

  // erase items
  for( unsigned int i = 0; i < acidCount; ++i )
  {
    pairvector.Erase( pairvector.Begin() + ( i % pairvector.Count() ) );
    doublevector.Erase( doublevector.Begin() + ( i % doublevector.Count() ) );
    intptrvector.Erase( intptrvector.Begin() + ( i % intptrvector.Count() ) );
    actorptrvector.Erase( actorptrvector.Begin() + ( i % actorptrvector.Count() ) );
    longvector.Erase( longvector.Begin() + ( i % longvector.Count() ) );
    charvector.Erase( charvector.Begin() + ( i % charvector.Count() ) );
  }
  DALI_TEST_EQUALS( ZERO, pairvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( pairCapacity, pairvector.Capacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, doublevector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( doubleCapacity, doublevector.Capacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, intptrvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( intptrCapacity, intptrvector.Capacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, actorptrvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( actorptrCapacity, actorptrvector.Capacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, longvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( longCapacity, longvector.Capacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, charvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( charCapacity, charvector.Capacity(), TEST_LOCATION );

  END_TEST;
}

namespace
{

bool gConstructorCalled = false;
bool gDestructorCalled = false;

struct ComplexType
{
  ComplexType()
  {
    gConstructorCalled = true;
  }
  ~ComplexType()
  {
    gDestructorCalled = true;
  }
};

} // anonymous namespace


int UtcDaliVectorComplex(void)
{
  tet_infoline("Testing Dali::Vector< int* > exception handling");

  // this does not compile at the moment
/*  Vector< ComplexType > classvector;
  DALI_TEST_EQUALS( ZERO, classvector.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( ZERO, classvector.Capacity(), TEST_LOCATION );

  DALI_TEST_EQUALS( false, gConstructorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, gDestructorCalled, TEST_LOCATION );
  classvector.PushBack( ComplexType() );
  DALI_TEST_EQUALS( true, gConstructorCalled, TEST_LOCATION );
  classvector.Clear();
  DALI_TEST_EQUALS( true, gDestructorCalled, TEST_LOCATION );
*/
//  Vector< Actor > classvector; this does not compile yet either
  tet_result(TET_PASS); // for now
  END_TEST;
}
