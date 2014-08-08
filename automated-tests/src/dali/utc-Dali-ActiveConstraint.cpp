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

void utc_dali_active_constraint_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_active_constraint_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static const Vector3 TEST_CONSTRAINT_TARGET = Vector3( 10.0f, 10.0f, 10.0f );

struct TestConstraintVector3
{
  Vector3 operator()( const Vector3& current )
  {
    return TEST_CONSTRAINT_TARGET;
  }
};

static bool constraintSignalled=false;
static void ConstraintCallback( ActiveConstraint& constraint )
{
  constraintSignalled = true;
}

} // anon namespace

int UtcDaliConstraintGetTargetObject(void)
{
  TestApplication application;

  // Apply a constraint to an actor

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, TestConstraintVector3() );

  Actor actor = Actor::New();

  ActiveConstraint active = actor.ApplyConstraint( constraint );

  // Retrieve the actor back from the active-constraint

  Handle object = active.GetTargetObject();

  DALI_TEST_CHECK( object );

  DALI_TEST_CHECK( object.GetObjectPtr() == actor.GetObjectPtr() );

  // Throw-away the actor, and check GetTargetObject returns NULL

  object.Reset();
  actor.Reset();

  object = active.GetTargetObject();

  DALI_TEST_CHECK( !object );
  END_TEST;
}

int UtcDaliConstraintGetTargetProperty(void)
{
  TestApplication application;

  // Apply a constraint to an actor

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, TestConstraintVector3() );

  Actor actor = Actor::New();

  ActiveConstraint active = actor.ApplyConstraint( constraint );

  // Check the property index

  Property::Index index = active.GetTargetProperty();

  DALI_TEST_CHECK( Actor::SIZE == index );
  END_TEST;
}

int UtcDaliConstraintSetWeight(void)
{
  TestApplication application;

  // Apply a constraint to an actor

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, TestConstraintVector3() );

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  ActiveConstraint active = actor.ApplyConstraint( constraint );

  // Apply the constraint manually

  active.SetWeight( 0.0f ); // start at zero

  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f)); // 1 elapsed second

  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  float weight( 0.25f );
  active.SetWeight( weight );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f)); // 1 elapsed second
  DALI_TEST_EQUALS( actor.GetCurrentSize(), TEST_CONSTRAINT_TARGET * weight, TEST_LOCATION );

  weight = 0.5f;
  active.SetWeight( weight );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f)); // 2 elapsed seconds
  DALI_TEST_EQUALS( actor.GetCurrentSize(), TEST_CONSTRAINT_TARGET * weight, TEST_LOCATION );

  weight = 0.75f;
  active.SetWeight( weight );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f)); // 2 elapsed seconds
  DALI_TEST_EQUALS( actor.GetCurrentSize(), TEST_CONSTRAINT_TARGET * weight, TEST_LOCATION );

  weight = 1.0f;
  active.SetWeight( weight );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f)); // 2 elapsed seconds
  DALI_TEST_EQUALS( actor.GetCurrentSize(), TEST_CONSTRAINT_TARGET, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintGetCurrentWeight(void)
{
  TestApplication application;

  // Apply a constraint to an actor

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, TestConstraintVector3() );

  Actor actor = Actor::New();

  ActiveConstraint active = actor.ApplyConstraint( constraint );

  // Check default weight

  DALI_TEST_CHECK( ActiveConstraint::DEFAULT_WEIGHT == active.GetCurrentWeight() );
  END_TEST;
}

int UtcDaliConstraintSignalApplied(void)
{
  TestApplication application;

  // Apply a constraint to an actor

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, TestConstraintVector3() );

  float duration( 10.0f );
  constraint.SetApplyTime( duration );

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  ActiveConstraint active = actor.ApplyConstraint( constraint );

  // Check signal is received after duration

  bool constraintCheck( false );
  ConstraintAppliedCheck appliedCheck( constraintCheck );

  active.AppliedSignal().Connect( &application, appliedCheck );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f)); // 1 elapsed second

  // Check signal has not fired
  application.SendNotification();
  appliedCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(4000.0f)); // 5 elapsed seconds

  // Check signal has not fired
  application.SendNotification();
  appliedCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(5000.0f - 1.0f)); // <10 elapsed seconds

  // Check signal has not fired
  application.SendNotification();
  appliedCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(2.0f)); // >10 elapsed seconds

  // Signal should have fired
  application.SendNotification();
  appliedCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliConstraintRemove(void)
{
  TestApplication application;

  // Apply a constraint to an actor

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, TestConstraintVector3() );

  float duration( 1.0f );
  constraint.SetApplyTime( duration );

  Actor actor = Actor::New();
  const Vector3 startSize( 1, 2, 3 );
  actor.SetSize( startSize );
  Stage::GetCurrent().Add( actor );

  ActiveConstraint active = actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(0.0f)); // 0 elapsed seconds

  DALI_TEST_CHECK( 0 == active.GetCurrentWeight() );
  DALI_TEST_CHECK( startSize == actor.GetCurrentSize() );

  bool constraintCheck( false );
  ConstraintAppliedCheck appliedCheck( constraintCheck );

  active.AppliedSignal().Connect( &application, appliedCheck );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(2000.0f)); // 2 elapsed seconds

  application.SendNotification();
  appliedCheck.CheckSignalReceived();

  DALI_TEST_CHECK( ActiveConstraint::DEFAULT_WEIGHT == active.GetCurrentWeight() );
  DALI_TEST_CHECK( TEST_CONSTRAINT_TARGET == actor.GetCurrentSize() );

  // This should be NOOP while constraint is applied
  actor.SetSize( startSize );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f));
  DALI_TEST_CHECK( ActiveConstraint::DEFAULT_WEIGHT == active.GetCurrentWeight() );
  DALI_TEST_CHECK( TEST_CONSTRAINT_TARGET == actor.GetCurrentSize() );

  // Remove constraint & try again
  actor.RemoveConstraint( active );
  actor.SetSize( startSize );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f));
  DALI_TEST_CHECK( ActiveConstraint::DEFAULT_WEIGHT == active.GetCurrentWeight() );
  DALI_TEST_CHECK( startSize == actor.GetCurrentSize() );

  // Try setting the weight after removal
  active.SetProperty( ActiveConstraint::WEIGHT, 0.5f );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f));
  DALI_TEST_CHECK( 0.5f == active.GetCurrentWeight() );
  END_TEST;
}

int UtcDaliConstraintCallback(void)
{
  TestApplication application;
  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, TestConstraintVector3() );
  constraint.SetApplyTime(2.0f);
  Actor actor = Actor::New();
  ActiveConstraint active = actor.ApplyConstraint( constraint );
  active.AppliedSignal().Connect( ConstraintCallback );
  application.SendNotification();
  application.Render(0);
  application.Render(1000);
  application.SendNotification();
  DALI_TEST_CHECK( ! constraintSignalled );
  application.Render(1016);
  application.SendNotification();

  DALI_TEST_CHECK( constraintSignalled );
  END_TEST;
}

int UtcDaliConstraintProperties(void)
{
  TestApplication application;

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, TestConstraintVector3() );
  Actor actor = Actor::New();
  ActiveConstraint active = actor.ApplyConstraint( constraint );

  Property::IndexContainer indices;
  active.GetPropertyIndices( indices );
  DALI_TEST_CHECK( ! indices.empty() );
  DALI_TEST_EQUALS( indices.size(), active.GetPropertyCount(), TEST_LOCATION );

  // Valid property
  DALI_TEST_EQUALS( active.GetPropertyName( 0 ), "weight", TEST_LOCATION );
  DALI_TEST_EQUALS( active.GetPropertyIndex( "weight" ), 0, TEST_LOCATION );
  DALI_TEST_CHECK( active.IsPropertyWritable( 0 ) );
  DALI_TEST_CHECK( active.IsPropertyAnimatable( 0 ) );
  DALI_TEST_EQUALS( active.GetPropertyType( 0 ), Property::FLOAT, TEST_LOCATION );
  DALI_TEST_CHECK( active.GetCurrentWeight() != 21312.0f );
  active.SetProperty( 0, 21312.0f );
  DALI_TEST_EQUALS( active.GetCurrentWeight(), 21312.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( active.GetProperty< float >( 0 ), 21312.0f, TEST_LOCATION );

  END_TEST;
}
