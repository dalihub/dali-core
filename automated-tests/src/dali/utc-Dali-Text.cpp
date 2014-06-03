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
#include <dali/dali.h>
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

  Text someText( std::string( "Some text" ) );

  DALI_TEST_CHECK( !someText.IsEmpty() );

  Character c = someText[0];

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

  printf(" is text empty ? ...... %d \n", someText4.IsEmpty());
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

  END_TEST;
}

int UtcDaliTextRemove01(void)
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
  END_TEST;
}

int UtcDaliTextRemove02(void)
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
  END_TEST;
}
