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
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_character_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_character_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliCharacterCopyConstructor(void)
{
  tet_infoline("UtcDaliCharacterCopyConstructor ");
  TestApplication application;

  Text text( std::string("12") );

  Character c1 = text[0];
  Character c2 = text[1];

  Character c3( c1 );
  Character c4 = c2;

  DALI_TEST_CHECK( (c1==c3) && (c2==c4) );
  END_TEST;
}

int UtcDaliCharacterComparissonOperators(void)
{
  tet_infoline("UtcDaliCharacterComparissonOperators ");
  TestApplication application;

  Text text( std::string("12") );

  Character c1 = text[0];
  Character c2 = text[1];

  Character c3( c1 );
  Character c4 = c2;

  DALI_TEST_CHECK( (c1==c3) && (c2==c4) && (c1!=c2) && (c1!=c4) );
  END_TEST;
}

int UtcDaliCharacterAssignmentOperator(void)
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

  END_TEST;
}

int UtcDaliCharacterIsLeftToRight(void)
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
  END_TEST;
}

int UtcDaliGetCharacterDirection(void)
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
  END_TEST;
}

int UtcDaliCharacterIsWhiteSpace(void)
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
  END_TEST;
}

int UtcDaliCharacterIsNewLine(void)
{
  tet_infoline("UtcDaliCharacterIsWhiteSpace ");
  TestApplication application;

  Text text( std::string("ab c\n\tdef") );

  DALI_TEST_CHECK(!text[0].IsNewLine()); // 'a'
  DALI_TEST_CHECK(!text[1].IsNewLine()); // 'b'
  DALI_TEST_CHECK(!text[2].IsNewLine()); // ' '
  DALI_TEST_CHECK(!text[3].IsNewLine()); // 'c'
  DALI_TEST_CHECK( text[4].IsNewLine()); // '\n'
  DALI_TEST_CHECK(!text[5].IsNewLine()); // '\t'
  DALI_TEST_CHECK(!text[6].IsNewLine()); // 'd'
  DALI_TEST_CHECK(!text[7].IsNewLine()); // 'e'
  DALI_TEST_CHECK(!text[8].IsNewLine()); // 'f'
  END_TEST;
}
