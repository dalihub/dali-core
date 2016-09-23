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
#include <string>
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
  map[ 10 ] = "DALi";
  map[ "world" ] = "world";
  map[ 100 ] = 9;
  map[ "world" ] = 3; // same item as line above
  DALI_TEST_CHECK( !map.Empty() ); // Should no longer be empty
  DALI_TEST_CHECK( map.Count() == 4 ); // Should only have four items, not five!!
  DALI_TEST_CHECK( map["hello"].Get<int>() == 1 );
  DALI_TEST_CHECK( map["world"].Get<int>() == 3 );
  DALI_TEST_EQUALS( "DALi", map[ 10 ].Get<std::string>(), TEST_LOCATION );
  DALI_TEST_CHECK( map[100].Get<int>() == 9 );

  map.Clear();
  DALI_TEST_CHECK( map.Empty() );
  END_TEST;
}

int UtcDaliPropertyMapCopyAndAssignment(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;
  map[ 10 ] = "DALi";

  Property::Map assignedMap;
  assignedMap[ "foo" ] = 3;
  assignedMap[ 100 ] = 9;
  DALI_TEST_CHECK( assignedMap.Count() == 2 );
  assignedMap = map;
  DALI_TEST_CHECK( assignedMap.Count() == 3 );

  Property::Map copiedMap( map );
  DALI_TEST_CHECK( copiedMap.Count() == 3 );

  // Self assignment
  DALI_TEST_CHECK( map.Count() == 3 );
  map = map;
  DALI_TEST_CHECK( map.Count() == 3 );

  END_TEST;
}

int UtcDaliPropertyMapConstOperator(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ 10 ] = "DALi";
  map[ "world" ] = 2;
  DALI_TEST_CHECK( map.Count() == 3 );

  const Property::Map& constMap( map );
  DALI_TEST_CHECK( constMap[ "world" ].Get<int>() == 2 );
  DALI_TEST_CHECK( constMap.Count() == 3 ); // Ensure count hasn't gone up

  DALI_TEST_EQUALS( "DALi", map[ 10 ].Get<std::string>(), TEST_LOCATION );
  DALI_TEST_CHECK( constMap.Count() == 3 ); // Ensure count hasn't gone up

  // Invalid Key
  try
  {
    constMap[ "invalidKey" ];
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
  map[ Actor::Property::COLOR ] = Color::MAGENTA;

  Property::Value& value = map.GetValue( 0 );
  DALI_TEST_CHECK( value.Get<int>() == 1 );
  value = 10; // Allows the actual changing of the value as we have a ref
  DALI_TEST_CHECK( map[ "hello" ].Get<int>() == 10 );

  Property::Value& value2 = map.GetValue( 2 );
  DALI_TEST_CHECK( value2.Get<Vector4>() == Color::MAGENTA );
  value2 = Color::CYAN;
  DALI_TEST_EQUALS( map[ Actor::Property::COLOR].Get<Vector4>(), Color::CYAN, TEST_LOCATION);

  // Out of bounds
  try
  {
    map.GetValue( 3 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "position", TEST_LOCATION );
  }

  END_TEST;
}

// deprecated API, only retrieve the key from the string-value pairs
int UtcDaliPropertyMapGetKey(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;
  map[ Actor::Property::COLOR ] = Color::MAGENTA;

  DALI_TEST_CHECK( map.GetKey( 0 ) == "hello" );
  DALI_TEST_CHECK( map.GetKey( 1 ) == "world" );

  // Wrong type
  try
  {
    map.GetKey( 2 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "position", TEST_LOCATION );
  }

  // Out of bounds
  try
  {
    map.GetKey( 3 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "position", TEST_LOCATION );
  }

  END_TEST;
}


int UtcDaliPropertyMapGetKeyAt(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;
  map[ Actor::Property::COLOR ] = Color::MAGENTA;

  DALI_TEST_CHECK( map.GetKeyAt( 0 ) == "hello" );
  DALI_TEST_CHECK( map.GetKeyAt( 1 ) == "world" );
  DALI_TEST_CHECK( map.GetKeyAt( 2 ) == Actor::Property::COLOR );

  // Out of bounds
  try
  {
    map.GetKey( 3 );
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
  map[ Actor::Property::COLOR ] = Color::MAGENTA;

  DALI_TEST_CHECK( map.GetPair( 0 ).first == "hello" );
  DALI_TEST_CHECK( map.GetPair( 0 ).second.Get< int >() == 1 );
  DALI_TEST_CHECK( map.GetPair( 1 ).first == "world" );
  DALI_TEST_CHECK( map.GetPair( 1 ).second.Get< int >() == 2 );

  // Wrong Type
  try
  {
    map.GetPair( 2 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "position", TEST_LOCATION );
  }

  // Out of bounds
  try
  {
    map.GetPair( 3 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "position", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliPropertyMapGetKeyValue(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ "world" ] = 2;
  map[ Actor::Property::COLOR ] = Color::MAGENTA;

  DALI_TEST_CHECK( map.GetKeyValue( 0 ).first == "hello" );
  DALI_TEST_CHECK( map.GetKeyValue( 0 ).second.Get< int >() == 1 );
  DALI_TEST_CHECK( map.GetKeyValue( 1 ).first == "world" );
  DALI_TEST_CHECK( map.GetKeyValue( 1 ).second.Get< int >() == 2 );
  DALI_TEST_CHECK( map.GetKeyValue( 2 ).first == Actor::Property::COLOR );
  DALI_TEST_CHECK( map.GetKeyValue( 2 ).second.Get< Vector4 >() == Color::MAGENTA );

  // Out of bounds
  try
  {
    map.GetPair( 3 );
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
  map[ 10 ] = "DALi";
  map[ "world" ] = 2;
  map[ 100 ] = 9;

  Property::Value* value = NULL;

  value = map.Find( "hello" );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 1 );

  const std::string world("world");
  value = map.Find( world );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 2 );

  value = map.Find( 100 );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 9 );

  value = map.Find( 10, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "DALi", value->Get<std::string>(), TEST_LOCATION );

  value = map.Find( 10, Property::INTEGER );
  DALI_TEST_CHECK( value == NULL );

  value = map.Find( "invalidKey" );
  DALI_TEST_CHECK( !value );

  END_TEST;
}

int UtcDaliPropertyMapFindIndexThenString(void)
{
  // Define the valid keys and values to test with.
  std::string stringKeyValid = "bar";
  std::string stringKeyInvalid = "aardvark";
  int indexKeyValid = 100;
  int indexKeyInvalid = 101;

  // Define invalid key and value to test with.
  std::string stringValueValid = "DALi";
  int indexValueValid = 3;

  // Set up a property map containing the valid keys and values defined above.
  Property::Map map;
  map[ "foo" ] = 1;
  map[ 10 ] = "string";
  map[ stringKeyValid ] = stringValueValid;
  map[ indexKeyValid ] = indexValueValid;

  Property::Value* value = NULL;

  // TEST: If both index and string are valid, the Property::Value of the index is returned.
  value = map.Find( indexKeyValid, stringKeyValid );

  DALI_TEST_EQUALS( value->Get<int>(), indexValueValid, TEST_LOCATION );


  // TEST: If only the index is valid, the Property::Value of the index is returned.
  value = map.Find( indexKeyValid, stringKeyInvalid );

  DALI_TEST_EQUALS( value->Get<int>(), indexValueValid, TEST_LOCATION );


  // TEST: If only the string is valid, the Property::Value of the string is returned.
  value = map.Find( indexKeyInvalid, stringKeyValid );

  DALI_TEST_EQUALS( value->Get<std::string>(), stringValueValid, TEST_LOCATION );


  // TEST: If neither the index or string are valid, then a NULL pointer is returned.
  value = map.Find( indexKeyInvalid, stringKeyInvalid );

  DALI_TEST_CHECK( value == NULL );

  END_TEST;
}

int UtcDaliPropertyMapOperatorIndex(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ 10 ] = "DALi";
  map[ "world" ] = 2;
  map[ 100 ] = 9;

  const Property::Map map2 = map;
  const Property::Value& value10 = map2[10];
  DALI_TEST_EQUALS( value10.Get<std::string>(), "DALi", TEST_LOCATION );

  const Property::Value& value100 = map2[100];
  DALI_TEST_EQUALS( value100.Get<int>(), 9, TEST_LOCATION );

  const Property::Value& valueHello = map2["hello"];
  DALI_TEST_EQUALS( valueHello.Get<int>(), 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyMapInsertP(void)
{
  Property::Map map;
  DALI_TEST_EQUALS( 0u, map.Count(), TEST_LOCATION );
  map.Insert( "foo", "bar");
  DALI_TEST_EQUALS( 1u, map.Count(), TEST_LOCATION );
  Property::Value* value = map.Find( "foo" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "bar", value->Get<std::string>(), TEST_LOCATION );

  map.Insert( std::string("foo2"), "testing" );
  DALI_TEST_EQUALS( 2u, map.Count(), TEST_LOCATION );
  value = map.Find( "foo2" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "testing", value->Get<std::string>(), TEST_LOCATION );

  map.Insert( 10, "DALi" );
  DALI_TEST_EQUALS( 3u, map.Count(), TEST_LOCATION );
  value = map.Find( 10 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "DALi", value->Get<std::string>(), TEST_LOCATION );

  map.Insert( 100, 9 );
  DALI_TEST_EQUALS( 4u, map.Count(), TEST_LOCATION );
  value = map.Find( 100 );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 9 );

  END_TEST;
}


int UtcDaliPropertyMapAddP(void)
{
  Property::Map map;
  DALI_TEST_EQUALS( 0u, map.Count(), TEST_LOCATION );
  map.Add( "foo", "bar");
  DALI_TEST_EQUALS( 1u, map.Count(), TEST_LOCATION );
  Property::Value* value = map.Find( "foo" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "bar", value->Get<std::string>(), TEST_LOCATION );

  map.Add( std::string("foo2"), "testing" );
  DALI_TEST_EQUALS( 2u, map.Count(), TEST_LOCATION );
  value = map.Find( "foo2" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "testing", value->Get<std::string>(), TEST_LOCATION );

  map.Add( 10, "DALi" );
  DALI_TEST_EQUALS( 3u, map.Count(), TEST_LOCATION );
  value = map.Find( 10 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "DALi", value->Get<std::string>(), TEST_LOCATION );

  map.Add( 100, 9 );
  DALI_TEST_EQUALS( 4u, map.Count(), TEST_LOCATION );
  value = map.Find( 100 );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 9 );

  END_TEST;
}

int UtcDaliPropertyMapAddChainP(void)
{
  Property::Map map;
  DALI_TEST_EQUALS( 0u, map.Count(), TEST_LOCATION );
  map
    .Add( "foo", "bar")
    .Add( std::string("foo2"), "testing" )
    .Add( 10, "DALi" )
    .Add( 100, 9 );

  DALI_TEST_EQUALS( 4u, map.Count(), TEST_LOCATION );

  Property::Value* value = map.Find( "foo" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "bar", value->Get<std::string>(), TEST_LOCATION );

  value = map.Find( "foo2" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "testing", value->Get<std::string>(), TEST_LOCATION );

  value = map.Find( 10 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "DALi", value->Get<std::string>(), TEST_LOCATION );

  value = map.Find( 100 );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 9 );

  END_TEST;
}

int UtcDaliPropertyMapAnonymousAddChainP(void)
{
  class TestMap
  {
  public:
    TestMap(Property::Map map)
    : mMap(map)
    {
    }
    Property::Map mMap;
  };

  TestMap mapTest( Property::Map().Add( "foo", "bar")
                                  .Add( std::string("foo2"), "testing" )
                                  .Add( 10, "DALi" )
                                  .Add( 100, 9 ));


  Property::Value* value = mapTest.mMap.Find( "foo" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "bar", value->Get<std::string>(), TEST_LOCATION );

  value = mapTest.mMap.Find( "foo2" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "testing", value->Get<std::string>(), TEST_LOCATION );

  value = mapTest.mMap.Find( 10 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( "DALi", value->Get<std::string>(), TEST_LOCATION );

  value = mapTest.mMap.Find( 100 );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 9 );

  END_TEST;
}


int UtcDaliPropertyMapMerge(void)
{
  Property::Map map;
  map[ "hello" ] = 1;
  map[ 10 ] = "DALi";
  map[ "world" ] = 2;

  DALI_TEST_CHECK( map.Count() == 3 );

  // Create another map with the same keys but different values
  Property::Map map2;
  map2[ "hello" ] = 3;
  map2[ "world" ] = 4;
  map[ 10 ] = "3DEngine";

  // Merge map2 into map1, count should still be 2, map values should be from map2
  map.Merge( map2 );
  DALI_TEST_CHECK( map.Count() == 3 );
  DALI_TEST_CHECK( map[ "hello" ].Get< int >() == 3 );
  DALI_TEST_CHECK( map[ "world"].Get< int >() == 4 );
  DALI_TEST_EQUALS( "3DEngine", map[ 10 ].Get<std::string>(), TEST_LOCATION );

  // Create another map with different keys
  Property::Map map3;
  map3[ "foo" ] = 5;
  map3[ 100 ] = 6;

  // Merge map3 into map1, count should increase, existing values should match previous and new values should match map3
  map.Merge( map3 );
  DALI_TEST_CHECK( map.Count() == 5 );
  DALI_TEST_CHECK( map[ "hello" ].Get< int >() == 3 );
  DALI_TEST_CHECK( map[ "world"].Get< int >() == 4 );
  DALI_TEST_CHECK( map[ "foo"].Get< int >() == 5 );
  DALI_TEST_EQUALS( "3DEngine", map[ 10 ].Get<std::string>(), TEST_LOCATION );
  DALI_TEST_CHECK( map[ 100].Get< int >() == 6 );

  // Create an empty map and attempt to merge, should be successful, nothing should change
  Property::Map map4;
  DALI_TEST_CHECK( map4.Empty() );
  map.Merge( map4 );
  DALI_TEST_CHECK( map4.Empty() );
  DALI_TEST_CHECK( map.Count() == 5 );
  DALI_TEST_CHECK( map[ "hello" ].Get< int >() == 3 );
  DALI_TEST_CHECK( map[ "world"].Get< int >() == 4 );
  DALI_TEST_CHECK( map[ "foo"].Get< int >() == 5 );
  DALI_TEST_EQUALS( "3DEngine", map[ 10 ].Get<std::string>(), TEST_LOCATION );
  DALI_TEST_CHECK( map[ 100 ].Get< int >() == 6 );

  // Merge map into map4, map4 should be the same as map now.
  map4.Merge( map );
  DALI_TEST_CHECK( map4.Count() == 5 );
  DALI_TEST_CHECK( map4[ "hello" ].Get< int >() == 3 );
  DALI_TEST_CHECK( map4[ "world"].Get< int >() == 4 );
  DALI_TEST_CHECK( map4[ "foo"].Get< int >() == 5 );
  DALI_TEST_EQUALS( "3DEngine", map[ 10 ].Get<std::string>(), TEST_LOCATION );
  DALI_TEST_CHECK( map4[ 100 ].Get< int >() == 6 );

  // Attempt to merge into itself, should be successful, nothing should change
  map.Merge( map );
  DALI_TEST_CHECK( map.Count() == 5 );
  DALI_TEST_CHECK( map[ "hello" ].Get< int >() == 3 );
  DALI_TEST_CHECK( map[ "world"].Get< int >() == 4 );
  DALI_TEST_CHECK( map[ "foo"].Get< int >() == 5 );
  DALI_TEST_EQUALS( "3DEngine", map[ 10 ].Get<std::string>(), TEST_LOCATION );
  DALI_TEST_CHECK( map[ 100 ].Get< int >() == 6 );

  END_TEST;
}

int UtcDaliPropertyMapOstream01(void)
{
  Property::Map map;

  map.Insert("duration", 5.0f);
  map.Insert( 10, "DALi" );
  map.Insert("delay", 1.0f);
  map.Insert( 100, 9 );
  map.Insert("value", 100);

  std::ostringstream oss;
  oss << map;

  tet_printf("Testing ouput of map: %s\n", oss.str().c_str());

  // string-value pairs first, then index-value pairs
  DALI_TEST_EQUALS( oss.str().compare("Map(5) = {duration:5, delay:1, value:100, 10:DALi, 100:9}"), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliPropertyMapOstream02(void)
{
  Property::Map map, map2;

  map2.Insert("duration", 5.0f);
  map2.Insert("delay", 1.0f);
  map2.Insert( 10, "DALi" );
  map.Insert("timePeriod", map2);
  map.Insert( 100, 9 );
  map.Insert("value", 100);

  std::ostringstream oss;
  oss << map;

  tet_printf("Testing ouput of map: %s\n", oss.str().c_str());

  // string-value pairs first, then index-value pairs
  DALI_TEST_EQUALS( oss.str().compare("Map(3) = {timePeriod:Map(3) = {duration:5, delay:1, 10:DALi}, value:100, 100:9}"), 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyKeyConstructorP(void)
{
  Property::Key key1( "aKey" );
  DALI_TEST_EQUALS( key1.type, Property::Key::STRING, TEST_LOCATION );
  DALI_TEST_EQUALS( key1.stringKey, "aKey", TEST_LOCATION );
  DALI_TEST_EQUALS( key1.indexKey, Property::INVALID_INDEX, TEST_LOCATION );

  Property::Key key2( Actor::Property::COLOR );
  DALI_TEST_EQUALS( key2.type, Property::Key::INDEX, TEST_LOCATION );
  DALI_TEST_EQUALS( key2.indexKey, (Dali::Property::Index)Actor::Property::COLOR, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyKeyEqualityOperatorP(void)
{
  Property::Key key1( "aKey" );
  Property::Key key2( 113 );

  DALI_TEST_CHECK( key1 == "aKey" );
  DALI_TEST_CHECK( ! (key1 == "notTheKey") );
  DALI_TEST_CHECK( ! (key1 == 1) );

  DALI_TEST_CHECK( key2 == 113 );
  DALI_TEST_CHECK( ! (key2 == 0) );
  DALI_TEST_CHECK( ! (key2 == "One hundred and thirteen" ) );

  DALI_TEST_CHECK( ! (key1 == key2) );
  DALI_TEST_CHECK( key1 != key2 );

  Property::Key key1B( "aKey" );
  Property::Key key2B( 113 );

  DALI_TEST_CHECK( key1 == key1B );
  DALI_TEST_CHECK( key2 == key2B );

  END_TEST;
}

int UtcDaliPropertyKeyInequalityOperatorP(void)
{
  Property::Key key1( "aKey" );
  Property::Key key2( 113 );

  DALI_TEST_CHECK( key1 != "notTheKey" );
  DALI_TEST_CHECK( key1 != 1 );

  DALI_TEST_CHECK( key2 != 0 );
  DALI_TEST_CHECK( key2 != "One hundred and thirteen" );

  DALI_TEST_CHECK( key1 != key2 );

  END_TEST;
}


int UtcDaliPropertyKeyOutputStream(void)
{
  Property::Key key1( "aKey" );
  Property::Key key2( 113 );

  std::ostringstream oss;
  oss << key1;
  DALI_TEST_EQUALS( oss.str(), "aKey", TEST_LOCATION );

  std::ostringstream oss2;
  oss2 << key2;
  DALI_TEST_EQUALS( oss2.str(), "113", TEST_LOCATION );

  END_TEST;
}
