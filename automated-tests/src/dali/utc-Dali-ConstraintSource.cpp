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
void utc_dali_constraint_source_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_constraint_source_cleanup(void)
{
  test_return_value = TET_PASS;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// LocalSource
///////////////////////////////////////////////////////////////////////////////
int UtcDaliLocalSource(void)
{
  LocalSource source( Actor::Property::POSITION );

  DALI_TEST_EQUALS( source.propertyIndex, (Property::Index)Actor::Property::POSITION, TEST_LOCATION );

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ParentSource
///////////////////////////////////////////////////////////////////////////////
int UtcDaliParentSource(void)
{
  ParentSource source( Actor::Property::POSITION );

  DALI_TEST_EQUALS( source.propertyIndex, (Property::Index)Actor::Property::POSITION, TEST_LOCATION );

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Source
///////////////////////////////////////////////////////////////////////////////
int UtcDaliSource1(void)
{
  Actor actor;
  Source source( actor, Actor::Property::SIZE );

  DALI_TEST_CHECK( ! source.object );
  DALI_TEST_EQUALS( source.propertyIndex, (Property::Index)Actor::Property::SIZE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSource2(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Source source( actor, Actor::Property::SIZE );
  DALI_TEST_EQUALS( source.object, actor, TEST_LOCATION );
  DALI_TEST_EQUALS( source.propertyIndex, (Property::Index)Actor::Property::SIZE, TEST_LOCATION );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ConstraintSource
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintSourceWithSource1(void)
{
  Actor actor;

  ConstraintSource source( Source( actor, Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( ! source.object );
  DALI_TEST_EQUALS( source.propertyIndex, (Property::Index)Actor::Property::PARENT_ORIGIN, TEST_LOCATION );
  DALI_TEST_EQUALS( source.sourceType, OBJECT_PROPERTY, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintSourceWithSource2(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  ConstraintSource source( Source( actor, Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_EQUALS( source.object, actor, TEST_LOCATION );
  DALI_TEST_EQUALS( source.propertyIndex, (Property::Index)Actor::Property::PARENT_ORIGIN, TEST_LOCATION );
  DALI_TEST_EQUALS( source.sourceType, OBJECT_PROPERTY, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintSourceWithLocalSource(void)
{
  Actor actor;

  ConstraintSource source( LocalSource( Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( ! source.object );
  DALI_TEST_EQUALS( source.propertyIndex, (Property::Index)Actor::Property::PARENT_ORIGIN, TEST_LOCATION );
  DALI_TEST_EQUALS( source.sourceType, LOCAL_PROPERTY, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintSourceWithParentSource(void)
{
  Actor actor;

  ConstraintSource source( ParentSource( Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( ! source.object );
  DALI_TEST_EQUALS( source.propertyIndex, (Property::Index)Actor::Property::PARENT_ORIGIN, TEST_LOCATION );
  DALI_TEST_EQUALS( source.sourceType, PARENT_PROPERTY, TEST_LOCATION );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////
