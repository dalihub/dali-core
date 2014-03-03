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
#include <dali/internal/event/text/character-impl.h>
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
TEST_FUNCTION( UtcDaliCharacterCopyConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCharacterComparissonOperators, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCharacterAssignmentOperator, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCharacterIsLeftToRight, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliGetCharacterDirection, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCharacterIsWhiteSpace, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCharacterIsNewLine, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCharacterGetImplementation,POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliCharacterCopyConstructor()
{
  tet_infoline("UtcDaliCharacterCopyConstructor ");
  TestApplication application;

  Text text( std::string("12") );

  Character c1 = text[0];
  Character c2 = text[1];

  Character c3( c1 );
  Character c4 = c2;

  DALI_TEST_CHECK( (c1==c3) && (c2==c4) );
}

static void UtcDaliCharacterComparissonOperators()
{
  tet_infoline("UtcDaliCharacterComparissonOperators ");
  TestApplication application;

  Text text( std::string("12") );

  Character c1 = text[0];
  Character c2 = text[1];

  Character c3( c1 );
  Character c4 = c2;

  DALI_TEST_CHECK( (c1==c3) && (c2==c4) && (c1!=c2) && (c1!=c4) );
}

static void UtcDaliCharacterAssignmentOperator()
{
  tet_infoline("UtcDaliCharacterAssignmentOperator ");
  TestApplication application;

  Text text( std::string("12") );
  Character c1 = text[0];
  Character c2 = text[1];
  c1 = c2; //assign
  DALI_TEST_CHECK( c1 == c1);

  // this should do nothing
  c1 = c1;
  DALI_TEST_CHECK(  c1 == c2);

}

static void UtcDaliCharacterIsLeftToRight()
{
  tet_infoline("UtcDaliCharacterIsLeftToRight ");
  TestApplication application;

  Text text( std::string("12בא") );

  Character c1 = text[0];
  Character c2 = text[1];
  Character c3 = text[2];
  Character c4 = text[3];

  DALI_TEST_CHECK( c1.GetCharacterDirection() != Character::RightToLeft &&
                   c2.GetCharacterDirection() != Character::RightToLeft &&
                   c3.GetCharacterDirection() == Character::RightToLeft &&
                   c4.GetCharacterDirection() == Character::RightToLeft );
}

static void UtcDaliGetCharacterDirection()
{
  tet_infoline("UtcDaliGetCharacterDirection ");
  TestApplication application;

  Text text( std::string("Aבא ") );

  Character c1 = text[0];
  Character c2 = text[1];
  Character c3 = text[2];
  Character c4 = text[3];

  DALI_TEST_EQUALS( c1.GetCharacterDirection(), Character::LeftToRight, TEST_LOCATION );
  DALI_TEST_EQUALS( c2.GetCharacterDirection(), Character::RightToLeft, TEST_LOCATION );
  DALI_TEST_EQUALS( c3.GetCharacterDirection(), Character::RightToLeft, TEST_LOCATION );
  DALI_TEST_EQUALS( c4.GetCharacterDirection(), Character::Neutral, TEST_LOCATION );
}

static void UtcDaliCharacterIsWhiteSpace()
{
  tet_infoline("UtcDaliCharacterIsWhiteSpace ");
  TestApplication application;

  Text text( std::string("ab c\n\tdef") );

  DALI_TEST_CHECK(!text[0].IsWhiteSpace()); // 'a'
  DALI_TEST_CHECK(!text[1].IsWhiteSpace()); // 'b'
  DALI_TEST_CHECK(text[2].IsWhiteSpace());  // ' '
  DALI_TEST_CHECK(!text[3].IsWhiteSpace()); // 'c'
  DALI_TEST_CHECK(text[4].IsWhiteSpace());  // '\n'
  DALI_TEST_CHECK(text[5].IsWhiteSpace());  // '\t'
  DALI_TEST_CHECK(!text[6].IsWhiteSpace()); // 'd'
  DALI_TEST_CHECK(!text[7].IsWhiteSpace()); // 'e'
  DALI_TEST_CHECK(!text[8].IsWhiteSpace()); // 'f'
}

static void UtcDaliCharacterIsNewLine()
{
  tet_infoline("UtcDaliCharacterIsWhiteSpace ");
  TestApplication application;

  Text text( std::string("ab c\n\tdef") );

  DALI_TEST_CHECK(!text[0].IsNewLine()); // 'a'
  DALI_TEST_CHECK(!text[1].IsNewLine()); // 'b'
  DALI_TEST_CHECK(!text[2].IsNewLine());  // ' '
  DALI_TEST_CHECK(!text[3].IsNewLine()); // 'c'
  DALI_TEST_CHECK(text[4].IsNewLine());  // '\n'
  DALI_TEST_CHECK(!text[5].IsNewLine());  // '\t'
  DALI_TEST_CHECK(!text[6].IsNewLine()); // 'd'
  DALI_TEST_CHECK(!text[7].IsNewLine()); // 'e'
  DALI_TEST_CHECK(!text[8].IsNewLine()); // 'f'
}

static void UtcDaliCharacterGetImplementation()
{
  tet_infoline("UtcDaliCharacterIsWhiteSpace ");
  TestApplication application;

  Text text( std::string("a") );
  Character c1 = text[0];
  const Dali::Internal::Character& impl = c1.GetImplementation();
  DALI_TEST_CHECK( impl.GetCharacter() == 'a' );
}
