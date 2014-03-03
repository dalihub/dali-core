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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliTextConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextCopyConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextAssignmentOperator, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextGetImplementation, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextSetGetText, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextAccessOperator01, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextAccessOperator02, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextIsEmpty, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextGetLength, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextAppend, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextRemove01, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextRemove02, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliTextConstructor()
{
  TestApplication application;

  Text text;

  DALI_TEST_CHECK( text.IsEmpty() );

  Text someText( std::string( "Some text" ) );

  DALI_TEST_CHECK( !someText.IsEmpty() );

  Character c = someText[0];

  Text cText( c );

  DALI_TEST_CHECK( !cText.IsEmpty() );
}

static void UtcDaliTextCopyConstructor()
{
  TestApplication application;

  Text someText1( std::string( "Some text1" ) );
  Text someText2( std::string( "Some text2" ) );

  Text someText3( someText1 );
  Text someText4 = someText2;

  DALI_TEST_CHECK( (someText1.GetText()==someText3.GetText()) && (someText2.GetText()==someText4.GetText()) );
}

static void UtcDaliTextAssignmentOperator()
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

  printf(" is text empty ? ...... %d \n", someText4.IsEmpty());
  // check for assignment of empty text
  someText2 = someText4;
  DALI_TEST_CHECK( someText2.IsEmpty() );

  // check for self assignment
  someText3 = someText3;
  DALI_TEST_CHECK( ! someText3.IsEmpty() );


}

static void UtcDaliTextGetImplementation()
{
  TestApplication application;

  Text text;

  bool assert1 = false;
  bool assert2 = false;

  try  // const GetImpl
  {
    const Internal::Text& impl = text.GetImplementation();
    (void)impl; // Avoid unused variable warning
  }
  catch( DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "NULL != mImpl && \"Text::GetImplementation: Text is uninitialized\"", TEST_LOCATION );

    assert1 = true;
  }

  try   // non const getImp
  {
    Internal::Text& impl = text.GetImplementation();
    (void)impl; // Avoid unused variable warning
  }
  catch( DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "NULL != mImpl && \"Text::GetImplementation: Text is uninitialized\"", TEST_LOCATION );

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
}

static void UtcDaliTextSetGetText()
{
  TestApplication application;

  const std::string someText( "Some text");

  Text text;
  text.SetText( someText );

  DALI_TEST_CHECK( someText == text.GetText() );

  Character c = text[0];
  text.SetText( c );

  DALI_TEST_CHECK( std::string("S") == text.GetText() );

  Text text2;
  text2.SetText( text );

  DALI_TEST_CHECK( text2.GetText() == text.GetText() );
}

static void UtcDaliTextAccessOperator01()
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
}

static void UtcDaliTextAccessOperator02()
{
  TestApplication application;

  const std::string someText( "Some text");

  Text text;
  text.SetText( someText );

  DALI_TEST_CHECK( someText == text.GetText() );

  Character c = text[0];
  text.SetText( c );

  DALI_TEST_CHECK( std::string("S") == text.GetText() );
}

static void UtcDaliTextIsEmpty()
{
  TestApplication application;

  Text text;

  DALI_TEST_CHECK( text.IsEmpty() );

  text.SetText( std::string( "Some text") );

  DALI_TEST_CHECK( !text.IsEmpty() );
}

static void UtcDaliTextGetLength()
{
  TestApplication application;

  const std::string someText( "Some text");

  Text text( someText );

  DALI_TEST_CHECK( someText.size() == text.GetLength() );
}

static void UtcDaliTextAppend()
{
  TestApplication application;

  Text text( std::string( "Some text") );

  text.Append( std::string( "A" ) );

  DALI_TEST_CHECK( std::string( "Some textA" ) == text.GetText() );

  Character c = text[0];
  text.Append( c );

  DALI_TEST_CHECK( std::string( "Some textAS" ) == text.GetText() );

  Text text2( std::string("B") );
  text.Append( text2 );

  DALI_TEST_CHECK( std::string( "Some textASB" ) == text.GetText() );

  // append to a null text

  Text emptyText;
  emptyText.Append( text2 );
  DALI_TEST_CHECK( text2.GetText() == emptyText.GetText() );

}

static void UtcDaliTextRemove01()
{
  TestApplication application;

  Text text( std::string( "Some text") );

  bool assert1 = false;
  bool assert2 = false;
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

  if( assert1 && assert2 )
  {
    tet_result( TET_PASS );
  }
  else
  {
    tet_result( TET_FAIL );
  }
}

static void UtcDaliTextRemove02()
{
  TestApplication application;

  Text text01( std::string( "Some text") );
  Text text02( std::string( "Somext") );
  Text text03( std::string( "") );

  text01.Remove( 3, 3 );

  DALI_TEST_EQUALS( text01.GetLength(), text02.GetLength(), TEST_LOCATION );
  DALI_TEST_EQUALS( text01.GetText(), text02.GetText(), TEST_LOCATION );

  text01.Remove( 0, 6 );

  DALI_TEST_EQUALS( text01.GetLength(), text03.GetLength(), TEST_LOCATION );
  DALI_TEST_EQUALS( text01.GetText(), text03.GetText(), TEST_LOCATION );
}
