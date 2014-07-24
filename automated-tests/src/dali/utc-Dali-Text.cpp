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

void utc_dali_text_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextConstructor(void)
{
  TestApplication application;

  Text text;

  DALI_TEST_CHECK( text.IsEmpty() );

  Text someEmptyText1( "" );

  DALI_TEST_CHECK( someEmptyText1.IsEmpty() );

  Text someEmptyText2( std::string( "" ) );

  DALI_TEST_CHECK( someEmptyText2.IsEmpty() );

  Text someEmptyText3( text );

  DALI_TEST_CHECK( someEmptyText3.IsEmpty() );

  Text someText1( "Some text" );

  DALI_TEST_CHECK( !someText1.IsEmpty() );

  Text someText2( std::string( "Some text" ) );

  DALI_TEST_CHECK( !someText2.IsEmpty() );

  Text someText3( Text( std::string( "Some text" ) ) );

  DALI_TEST_CHECK( !someText3.IsEmpty() );

  Character c = someText1[0];

  Text cText( c );

  DALI_TEST_CHECK( !cText.IsEmpty() );
  END_TEST;
}

int UtcDaliTextCopyConstructor(void)
{
  TestApplication application;

  Text someText1( std::string( "Some text1" ) );
  Text someText2( std::string( "Some text2" ) );

  Text someText3( someText1 );
  Text someText4 = someText2;

  DALI_TEST_CHECK( (someText1.GetText()==someText3.GetText()) && (someText2.GetText()==someText4.GetText()) );
  END_TEST;
}

int UtcDaliTextAssignmentOperator(void)
{
  TestApplication application;

  // check for assignment when current is NULL

  Text someText1( std::string( "Some text1" ) );
  Text someText2;
  someText2 = someText1;

  DALI_TEST_CHECK( (someText1.GetText()==someText2.GetText()) );

  // check for assignment when current object already has text
  Text someText3( std::string( "Some text3" ) );
  someText2 = someText3;
  DALI_TEST_CHECK( (someText3.GetText()==someText2.GetText()) );

  Text someText4;

  // check for assignment of empty text
  someText2 = someText4;
  DALI_TEST_CHECK( someText2.IsEmpty() );

  // check for self assignment
  someText3 = someText3;
  DALI_TEST_CHECK( ! someText3.IsEmpty() );


  END_TEST;
}

int UtcDaliTextSetGetText(void)
{
  TestApplication application;

  const char* someText = "Some text";
  const std::string someText2( "Some text2" );

  Text text;
  text.SetText( someText );

  DALI_TEST_EQUALS( someText, text.GetText(), TEST_LOCATION );

  text.SetText( someText2 );

  DALI_TEST_EQUALS( someText2, text.GetText(), TEST_LOCATION );

  Character c = text[0];
  text.SetText( c );

  DALI_TEST_EQUALS( std::string( "S" ), text.GetText(), TEST_LOCATION );

  Text text2;
  text2.SetText( text );

  DALI_TEST_EQUALS( text2.GetText(), text.GetText(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextAccessOperator01(void)
{
  TestApplication application;

  std::string someText;

  Text text;
  text.SetText( someText );

  bool assert1 = false;
  bool assert2 = false;
  try
  {
    Character c = text[0];
  }
  catch( DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "NULL != mImpl && \"Text::operator[]: Text is uninitialized\"", TEST_LOCATION );

    assert1 = true;
  }

  someText = std::string( "some text" );
  text.SetText( someText );

  try
  {
    Character c = text[100];
  }
  catch( DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "position < mString.size() && \"Text::operator[]: Character position is out of bounds\"", TEST_LOCATION );

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
  END_TEST;
}

int UtcDaliTextAccessOperator02(void)
{
  TestApplication application;

  const std::string someText( "Some text");

  Text text;
  text.SetText( someText );

  DALI_TEST_CHECK( someText == text.GetText() );

  Character c = text[0];
  text.SetText( c );

  DALI_TEST_CHECK( std::string("S") == text.GetText() );
  END_TEST;
}

int UtcDaliTextIsEmpty(void)
{
  TestApplication application;

  Text text;

  DALI_TEST_CHECK( text.IsEmpty() );

  text.SetText( std::string( "Some text") );

  DALI_TEST_CHECK( !text.IsEmpty() );
  END_TEST;
}

int UtcDaliTextGetLength(void)
{
  TestApplication application;

  const std::string someText( "Some text");

  Text text( someText );

  DALI_TEST_CHECK( someText.size() == text.GetLength() );
  END_TEST;
}

int UtcDaliTextAppend(void)
{
  TestApplication application;

  Text text( std::string( "Some text") );

  text.Append( "A" );

  DALI_TEST_CHECK( std::string( "Some textA" ) == text.GetText() );

  text.Append( std::string( "B" ) );

  DALI_TEST_CHECK( std::string( "Some textAB" ) == text.GetText() );

  Character c = text[0];
  text.Append( c );

  DALI_TEST_CHECK( std::string( "Some textABS" ) == text.GetText() );

  Text text2( std::string("C") );
  text.Append( text2 );

  DALI_TEST_CHECK( std::string( "Some textABSC" ) == text.GetText() );

  // append to a null text

  Text emptyText;
  emptyText.Append( text2 );
  DALI_TEST_CHECK( text2.GetText() == emptyText.GetText() );

  // append a null text

  Text emptyText2;
  emptyText.Append( emptyText2 );
  DALI_TEST_CHECK( text2.GetText() == emptyText.GetText() );

  END_TEST;
}

int UtcDaliTextRemove01(void)
{
  TestApplication application;

  Text text( std::string( "Some text") );

  bool assert1 = false;
  bool assert2 = false;
  bool assert3 = false;

  try
  {
    text.Remove( 100, 3 );
  }
  catch( DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "position < mString.size() && \"Text::Remove: Character position is out of bounds\"", TEST_LOCATION );
    assert1 = true;
  }

  try
  {
    text.Remove( 1, 300 );
  }
  catch( DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "position + numberOfCharacters <= mString.size() && \"Text::Remove: Character position + numberOfCharacters is out of bounds\"", TEST_LOCATION );
    assert2 = true;
  }

  try
  {
    text.SetText( std::string( "" ) );
    text.Remove( 1, 300 );
  }
  catch( DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "NULL != mImpl && \"Text::Remove: Text is uninitialized\"", TEST_LOCATION );
    assert3 = true;
  }

  if( assert1 && assert2 && assert3 )
  {
    tet_result( TET_PASS );
  }
  else
  {
    tet_result( TET_FAIL );
  }
  END_TEST;
}

int UtcDaliTextRemove02(void)
{
  TestApplication application;

  Text text01( std::string( "Some text") );
  Text text02( std::string( "Somext") );
  Text text03( std::string( "" ) );
  Text text04( std::string( "Hello world" ) );
  Text text05( std::string( "world" ) );

  text01.Remove( 3u, 3u );

  DALI_TEST_EQUALS( text01.GetLength(), text02.GetLength(), TEST_LOCATION );
  DALI_TEST_EQUALS( text01.GetText(), text02.GetText(), TEST_LOCATION );

  text01.Remove( 0u, 0u );

  DALI_TEST_EQUALS( text01.GetLength(), text02.GetLength(), TEST_LOCATION );
  DALI_TEST_EQUALS( text01.GetText(), text02.GetText(), TEST_LOCATION );

  text01.Remove( 0u, 6u );

  DALI_TEST_EQUALS( text01.GetLength(), text03.GetLength(), TEST_LOCATION );
  DALI_TEST_EQUALS( text01.GetText(), text03.GetText(), TEST_LOCATION );

  text04.Remove( 0u, 6u );

  DALI_TEST_EQUALS( text04.GetLength(), text05.GetLength(), TEST_LOCATION );
  DALI_TEST_EQUALS( text04.GetText(), text05.GetText(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextFind(void)
{
  TestApplication application;

  Text text;
  Character c1 = Text( std::string( "c" ) )[0u];
  Character c2 = Text( std::string( "o" ) )[0u];
  Vector<std::size_t> positions;

  // Find in void text.

  positions.Clear();
  text.Find( c1, 0u, 0u, positions );
  DALI_TEST_EQUALS( positions.Count(), 0u, TEST_LOCATION );

  positions.Clear();
  text.Find( Text::WHITE_SPACE, 0u, 0u, positions );
  DALI_TEST_EQUALS( positions.Count(), 0u, TEST_LOCATION );

  positions.Clear();
  text.Find( Text::NEW_LINE, 0u, 0u, positions );
  DALI_TEST_EQUALS( positions.Count(), 0u, TEST_LOCATION );

  // Find in text.

  // Find 'c' and 'o'
  text.SetText( std::string( "Hello world" ) );

  positions.Clear();
  text.Find( c1, 0u, 11u, positions );
  DALI_TEST_EQUALS( positions.Count(), 0u, TEST_LOCATION );

  positions.Clear();
  text.Find( c2, 0u, 11u, positions );
  DALI_TEST_EQUALS( positions.Count(), 2u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[0u], 4u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[1u], 7u, TEST_LOCATION );

  // Find white space
  text.SetText( std::string( "   Hello  world  \n" ) );

  positions.Clear();
  text.Find( Text::WHITE_SPACE, 0u, 17u, positions );
  DALI_TEST_EQUALS( positions.Count(), 8u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[0u], 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[1u], 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[2u], 2u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[3u], 8u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[4u], 9u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[5u], 15u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[6u], 16u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[7u], 17u, TEST_LOCATION );

  // Find new line character
  text.SetText( std::string( "\n\nHello\nworld\n\n" ) );

  positions.Clear();
  text.Find( Text::NEW_LINE, 0u, 14u, positions );
  DALI_TEST_EQUALS( positions.Count(), 5u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[0u], 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[1u], 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[2u], 7u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[3u], 13u, TEST_LOCATION );
  DALI_TEST_EQUALS( positions[4u], 14u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextGetSubText(void)
{
  TestApplication application;

  Text text;
  Text subText;

  // Get sub-text from a void text.

  subText.SetText( "Hello" );
  text.GetSubText( 0u, 1u, subText );
  DALI_TEST_EQUALS( subText.GetText(), std::string( "Hello" ), TEST_LOCATION );

  // Get sub-text.

  text.SetText( std::string( "Hello world" ) );

  // Access out of bounds
  subText.SetText( "Hello" );
  text.GetSubText( 30u, 31u, subText );
  DALI_TEST_EQUALS( subText.GetText(), std::string( "Hello" ), TEST_LOCATION );
  text.GetSubText( 0u, 31u, subText );
  DALI_TEST_EQUALS( subText.GetText(), std::string( "Hello" ), TEST_LOCATION );
  text.GetSubText( 30u, 1u, subText );
  DALI_TEST_EQUALS( subText.GetText(), std::string( "Hello" ), TEST_LOCATION );

  // Check it swaps the indices.
  text.GetSubText( 8u, 2u, subText );
  DALI_TEST_EQUALS( subText.GetText(), std::string( "row oll" ), TEST_LOCATION );

  // Normal access.
  subText.SetText( std::string( "" ) );
  text.GetSubText( 4u, 6u, subText );
  DALI_TEST_EQUALS( subText.GetText(), std::string( "o w" ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextIsWhiteSpaceNewLine(void)
{
  TestApplication application;

  Text text;

  // Query a void text.

  DALI_TEST_CHECK( !text.IsWhiteSpace( 0u ) );
  DALI_TEST_CHECK( !text.IsNewLine( 0u ) );

  // Set a text
  text.SetText( "Hello world\n" );

  // Query out of bounds

  DALI_TEST_CHECK( !text.IsWhiteSpace( 30u ) );
  DALI_TEST_CHECK( !text.IsNewLine( 30u ) );

  // Normal query.

  DALI_TEST_CHECK( !text.IsWhiteSpace( 1u ) );
  DALI_TEST_CHECK( !text.IsNewLine( 1u ) );
  DALI_TEST_CHECK( text.IsWhiteSpace( 5u ) );
  DALI_TEST_CHECK( text.IsNewLine( 11u ) );

  END_TEST;
}
