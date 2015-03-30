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

void utc_dali_property_map_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_property_map_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliPropertyMapPopulate(void)
{
  Property::Map map;
  DALI_TEST_CHECK( map.Empty() );

  map[ "hello" ] = 1;
  map[ "world" ] = "world";
  map[ "world" ] = 3; // same item as line above
  DALI_TEST_CHECK( !map.Empty() ); // Should no longer be empty
  DALI_TEST_CHECK( map.Count() == 2 ); // Should only have two items, not three!!
  DALI_TEST_CHECK( map["hello"].Get<int>() == 1 );
  DALI_TEST_CHECK( map["world"].Get<int>() == 3 );

  map.Clear();
  DALI_TEST_CHECK( map.Empty() );
  END_TEST;
}

int UtcDaliPropertyMapCopyAndAssignment(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;

  Property::Map assignedMap;
  assignedMap[ "foo" ] = 3;
  DALI_TEST_CHECK( assignedMap.Count() == 1 );
  assignedMap = map;
  DALI_TEST_CHECK( assignedMap.Count() == 2 );

  Property::Map copiedMap( map );
  DALI_TEST_CHECK( copiedMap.Count() == 2 );

  // Self assignment
  DALI_TEST_CHECK( map.Count() == 2 );
  map = map;
  DALI_TEST_CHECK( map.Count() == 2 );

  END_TEST;
}

int UtcDaliPropertyMapConstOperator(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;
  DALI_TEST_CHECK( map.Count() == 2 );

  const Property::Map& constMap( map );
  DALI_TEST_CHECK( constMap[ "world" ].Get<int>() == 2 );
  DALI_TEST_CHECK( constMap.Count() == 2 ); // Ensure count hasn't gone up

  // Invalid Key
  try
  {
    constMap[ "invalid-key" ];
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "! \"Invalid Key\"", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliPropertyMapGetValue(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;

  Property::Value& value = map.GetValue( 0 );
  DALI_TEST_CHECK( value.Get<int>() == 1 );
  value = 10; // Allows the actual changing of the value as we have a ref
  DALI_TEST_CHECK( map[ "hello" ].Get<int>() == 10 );

  // Out of bounds
  try
  {
    map.GetValue( 2 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "position", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliPropertyMapGetKey(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;

  DALI_TEST_CHECK( map.GetKey( 0 ) == "hello" );
  DALI_TEST_CHECK( map.GetKey( 1 ) == "world" );

  // Out of bounds
  try
  {
    map.GetKey( 2 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "position", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliPropertyMapGetPair(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;

  DALI_TEST_CHECK( map.GetPair( 0 ).first == "hello" );
  DALI_TEST_CHECK( map.GetPair( 0 ).second.Get< int >() == 1 );
  DALI_TEST_CHECK( map.GetPair( 1 ).first == "world" );
  DALI_TEST_CHECK( map.GetPair( 1 ).second.Get< int >() == 2 );

  // Out of bounds
  try
  {
    map.GetPair( 2 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "position", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliPropertyMapFind(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;

  Property::Value* value = NULL;

  value = map.Find( "hello" );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 1 );

  value = map.Find( "world" );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 2 );

  value = map.Find( "invalid-key" );
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliPropertyMapMerge(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;

  DALI_TEST_CHECK( map.Count() == 2 );

  // Create another map with the same keys but different values
  Property::Map map2;
  map[ "hello" ] = 3;
  map[ "world" ] = 4;

  // Merge map2 into map1, count should still be 2, map values should be from map2
  map.Merge( map2 );
  DALI_TEST_CHECK( map.Count() == 2 );
  DALI_TEST_CHECK( map[ "hello" ].Get< int >() == 3 );
  DALI_TEST_CHECK( map[ "world"].Get< int >() == 4 );

  // Create another map with different keys
  Property::Map map3;
  map3[ "foo" ] = 5;
  map3[ "bar" ] = 6;

  // Merge map3 into map1, count should increase, existing values should match previous and new values should match map3
  map.Merge( map3 );
  DALI_TEST_CHECK( map.Count() == 4 );
  DALI_TEST_CHECK( map[ "hello" ].Get< int >() == 3 );
  DALI_TEST_CHECK( map[ "world"].Get< int >() == 4 );
  DALI_TEST_CHECK( map[ "foo"].Get< int >() == 5 );
  DALI_TEST_CHECK( map[ "bar"].Get< int >() == 6 );

  // Create an empty map and attempt to merge, should be successful, nothing should change
  Property::Map map4;
  DALI_TEST_CHECK( map4.Empty() );
  map.Merge( map4 );
  DALI_TEST_CHECK( map4.Empty() );
  DALI_TEST_CHECK( map.Count() == 4 );
  DALI_TEST_CHECK( map[ "hello" ].Get< int >() == 3 );
  DALI_TEST_CHECK( map[ "world"].Get< int >() == 4 );
  DALI_TEST_CHECK( map[ "foo"].Get< int >() == 5 );
  DALI_TEST_CHECK( map[ "bar"].Get< int >() == 6 );

  // Merge map into map4, map4 should be the same as map now.
  map4.Merge( map );
  DALI_TEST_CHECK( map4.Count() == 4 );
  DALI_TEST_CHECK( map4[ "hello" ].Get< int >() == 3 );
  DALI_TEST_CHECK( map4[ "world"].Get< int >() == 4 );
  DALI_TEST_CHECK( map4[ "foo"].Get< int >() == 5 );
  DALI_TEST_CHECK( map4[ "bar"].Get< int >() == 6 );

  // Attempt to merge into itself, should be successful, nothing should change
  map.Merge( map );
  DALI_TEST_CHECK( map.Count() == 4 );
  DALI_TEST_CHECK( map[ "hello" ].Get< int >() == 3 );
  DALI_TEST_CHECK( map[ "world"].Get< int >() == 4 );
  DALI_TEST_CHECK( map[ "foo"].Get< int >() == 5 );
  DALI_TEST_CHECK( map[ "bar"].Get< int >() == 6 );

  END_TEST;
}
