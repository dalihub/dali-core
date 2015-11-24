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
void utc_dali_constraint_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_constraint_cleanup(void)
{
  test_return_value = TET_PASS;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace
{

/**
 * A function to use for a constraint, no data collected.
 */
template< typename T >
void BasicFunction( T& /* current */, const PropertyInputContainer& /* inputs */ )
{
}

/**
 * A functor which sets a given boolean when the functor is called.
 */
template< typename T >
struct BasicCalledFunctor
{
  BasicCalledFunctor( bool& functorCalled ) : mCalled( functorCalled ) { }

  void operator()( T& /* current */, const PropertyInputContainer& /* inputs */ )
  {
    mCalled = true;
  }

  bool& mCalled;
};

/**
 * A functor which increments a given integer when the functor is called.
 */
template< typename T >
struct CalledCountFunctor
{
  CalledCountFunctor( int& callCount ) : mCallCount( callCount ) { }

  void operator()( T& /* current */, const PropertyInputContainer& /* inputs */ )
  {
    ++mCallCount;
  }

  int& mCallCount;
};

/**
 * A functor which sets the given value as the value required when the functor is called.
 */
template< typename T >
struct SetValueFunctor
{
  SetValueFunctor( const T& value ) : mValue( value ) { }

  void operator()( T& current, const PropertyInputContainer& /* inputs */ )
  {
    current = mValue;
  }

  T mValue;
};

} // unnamed namespace
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::New(
//   Handle,
//   Property::Index,
//   void( *function )( T&, const PropertyInputContainer& ) )
///////////////////////////////////////////////////////////////////////////////
namespace UtcDaliConstraintNewFunction
{
bool gConstraintFunctionCalled = false;
void ConstraintFunction( Vector3& /* current */, const PropertyInputContainer& /* inputs */ )
{
  gConstraintFunctionCalled = true;
}
} // namespace UtcDaliConstraintNewFunction

int UtcDaliConstraintNewFunctionP(void)
{
  // Ensure that we can create a constraint using a C function and that it is called.

  TestApplication application;
  UtcDaliConstraintNewFunction::gConstraintFunctionCalled = false;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( UtcDaliConstraintNewFunction::gConstraintFunctionCalled, false, TEST_LOCATION );

  // Add a constraint
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &UtcDaliConstraintNewFunction::ConstraintFunction );
  DALI_TEST_CHECK( constraint );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( UtcDaliConstraintNewFunction::gConstraintFunctionCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintNewFunctionN(void)
{
  // Create a constraint with an uninitialised handle

  TestApplication application;

  // Add a constraint with an uninitialised handle
  try
  {
    Constraint constraint = Constraint::New< Vector3 >( Actor(), Actor::Property::POSITION, &UtcDaliConstraintNewFunction::ConstraintFunction );
    DALI_TEST_CHECK( false ); // Should not reach here
  }
  catch ( ... )
  {
    DALI_TEST_CHECK( true ); // Should assert!
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::New(
//   Handle,
//   Property::Index,
//   const T& object )
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintNewFunctorP(void)
{
  // Ensure that we can create a constraint using a functor and that it is called.

  TestApplication application;
  bool functorCalled = false;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( functorCalled, false, TEST_LOCATION );

  // Add a constraint
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicCalledFunctor< Vector3 >( functorCalled ) );
  DALI_TEST_CHECK( constraint );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( functorCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintNewFunctorN(void)
{
  // Create a constraint with an uninitialised handle

  TestApplication application;
  bool functorCalled = false;

  // Add a constraint with an uninitialised handle
  try
  {
    Constraint constraint = Constraint::New< Vector3 >( Actor(), Actor::Property::POSITION, BasicCalledFunctor< Vector3 >( functorCalled ) );
    DALI_TEST_CHECK( false ); // Should not reach here
  }
  catch ( ... )
  {
    DALI_TEST_CHECK( true ); // Should assert!
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::New(
//   Handle,
//   Property::Index,
//   const T& object,
//   void ( T::*memberFunction ) ( P&, const PropertyInputContainer& ) )
///////////////////////////////////////////////////////////////////////////////
namespace UtcDaliConstraintNewFunctorMember
{
struct Functor
{
  Functor( bool& positionCalled, bool& scaleCalled )
  : mPositionCalled( positionCalled ),
    mScaleCalled( scaleCalled )
  {
  }

  void Position( Vector3& /* current */, const PropertyInputContainer& /* inputs */ )
  {
    mPositionCalled = true;
  }

  void Scale( Vector3& /* current */, const PropertyInputContainer& /* inputs */ )
  {
    mScaleCalled = true;
  }

  bool& mPositionCalled;
  bool& mScaleCalled;
};
} // namespace UtcDaliConstraintNewFunctorMember

int UtcDaliConstraintNewFunctorMemberP(void)
{
  // Ensure that we can create a constraint using a functor and that it is called.

  TestApplication application;
  bool positionFunctorCalled = false;
  bool sizeFunctorCalled = false;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( positionFunctorCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( sizeFunctorCalled, false, TEST_LOCATION );

  // Add a constraint that calls Functor::Position
  Constraint constraint = Constraint::New< Vector3 >(
      actor,
      Actor::Property::POSITION,
      UtcDaliConstraintNewFunctorMember::Functor( positionFunctorCalled, sizeFunctorCalled ),
      &UtcDaliConstraintNewFunctorMember::Functor::Position );
  DALI_TEST_CHECK( constraint );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( positionFunctorCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( sizeFunctorCalled, false, TEST_LOCATION );

  // Add another constraint that calls Functor::Size
  Constraint constraint2 = Constraint::New< Vector3 >(
      actor,
      Actor::Property::SCALE,
      UtcDaliConstraintNewFunctorMember::Functor( positionFunctorCalled, sizeFunctorCalled ),
      &UtcDaliConstraintNewFunctorMember::Functor::Scale );
  DALI_TEST_CHECK( constraint2 );
  constraint2.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( positionFunctorCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( sizeFunctorCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintNewFunctorMemberN(void)
{
  // Create a constraint with an uninitialised handle

  TestApplication application;
  bool positionFunctorCalled = false;
  bool sizeFunctorCalled = false;

  // Add a constraint with an uninitialised handle
  try
  {
    Constraint constraint = Constraint::New< Vector3 >(
        Actor(),
        Actor::Property::POSITION,
        UtcDaliConstraintNewFunctorMember::Functor( positionFunctorCalled, sizeFunctorCalled ),
        &UtcDaliConstraintNewFunctorMember::Functor::Position );
    DALI_TEST_CHECK( false ); // Should not reach here
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_CHECK( true ); // Should assert!
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::Clone
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintCloneP(void)
{
  // Ensure we can clone for another actor and it's called appropriately

  TestApplication application;
  int calledCount = 0;

  Actor actor = Actor::New();
  Actor clone = Actor::New();

  Stage stage = Stage::GetCurrent();
  stage.Add( actor );
  stage.Add( clone );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( calledCount, 0, TEST_LOCATION );

  // Add a constraint to actor
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, CalledCountFunctor< Vector3 >( calledCount ) );
  DALI_TEST_CHECK( constraint );
  constraint.Apply();

  // Create a clone but don't apply
  Constraint constraintClone = constraint.Clone( clone );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( calledCount, 1, TEST_LOCATION );

  // Reset
  calledCount = 0;

  // Ensure constraint isn't called again if scene doesn't change
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( calledCount, 0, TEST_LOCATION );

  // Apply the clone constraint
  constraintClone.Apply();

  application.SendNotification();
  application.Render();

  // Should only be called once for the new constraint clone ONLY
  DALI_TEST_EQUALS( calledCount, 1, TEST_LOCATION );

  // Reset
  calledCount = 0;

  // Change the position of both actors
  actor.SetPosition( 100.0f, 100.0f );
  clone.SetPosition( 100.0f, 100.0f );

  application.SendNotification();
  application.Render();

  // Functor should have been called twice
  DALI_TEST_EQUALS( calledCount, 2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintCloneN(void)
{
  // Attempt to clone an uninitialised constraint should cause an assert

  TestApplication application;

  Constraint constraint;

  try
  {
    Actor actor = Actor::New();
    Constraint clone = constraint.Clone( actor );
    DALI_TEST_CHECK( false );
  }
  catch ( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

namespace UtcDaliConstraintClone
{
void Function( Vector3& /* current */, const PropertyInputContainer& inputs )
{
  DALI_TEST_EQUALS( inputs[0]->GetType(), Property::VECTOR3, TEST_LOCATION );
  DALI_TEST_EQUALS( inputs[1]->GetType(), Property::ROTATION, TEST_LOCATION );
  DALI_TEST_EQUALS( inputs[2]->GetType(), Property::VECTOR4, TEST_LOCATION );
  DALI_TEST_EQUALS( inputs[3]->GetType(), Property::BOOLEAN, TEST_LOCATION );
}
} // namespace UtcDaliConstraintClone

int UtcDaliConstraintCloneCheckSourcesAndSetters(void)
{
  // Ensure all sources, the tag and remove-action are cloned appropriately

  TestApplication application;

  Actor actor = Actor::New();
  Actor clone = Actor::New();

  Stage stage = Stage::GetCurrent();
  stage.Add( actor );
  stage.Add( clone );

  application.SendNotification();
  application.Render();

  // Create a constraint, DON'T Apply it though
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &UtcDaliConstraintClone::Function );
  constraint.AddSource( LocalSource( Actor::Property::SIZE ) );
  constraint.AddSource( LocalSource( Actor::Property::ORIENTATION ) );
  constraint.AddSource( LocalSource( Actor::Property::COLOR ) );
  constraint.AddSource( LocalSource( Actor::Property::VISIBLE ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.SetTag( 123 );

  // Clone the constraint & apply the clone
  Constraint constraintClone = constraint.Clone( clone );
  constraintClone.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( constraint.GetRemoveAction(), constraintClone.GetRemoveAction(), TEST_LOCATION );
  DALI_TEST_EQUALS( constraint.GetTag(),          constraintClone.GetTag(),          TEST_LOCATION );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::Constraint( const Constraint& )
// Constraint::operator=
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintCopyAndAssignment(void)
{
  // Ensure copy constructor & assignment operators work

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &BasicFunction< Vector3 > );
  Constraint copied( constraint );
  Constraint assigned;
  DALI_TEST_CHECK( constraint == copied );
  DALI_TEST_CHECK( copied != assigned );

  assigned = constraint;
  DALI_TEST_CHECK( constraint == assigned );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::DownCast
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintDownCast(void)
{
  // Ensure DownCast works as expected

  TestApplication application;

  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &BasicFunction< Vector3 > );

  // Another BaseHandle type
  Constraint downCast = Constraint::DownCast( actor );
  DALI_TEST_CHECK( ! downCast );

  // A constraint
  downCast = Constraint::DownCast( constraint );
  DALI_TEST_CHECK( downCast );

  // An empty constraint
  downCast = Constraint::DownCast( Constraint() );
  DALI_TEST_CHECK( ! downCast );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::GetTargetObject
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintGetTargetObjectP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &BasicFunction< Vector3 > );
  DALI_TEST_CHECK( constraint.GetTargetObject() == actor );

  Actor actor2 = Actor::New();
  DALI_TEST_CHECK( constraint.GetTargetObject() != actor2 );

  END_TEST;
}

int UtcDaliConstraintGetTargetObjectN(void)
{
  // Attempt to retrieve from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    Handle handle = constraint.GetTargetObject();
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::GetTargetProperty
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintGetTargetPropertyP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &BasicFunction< Vector3 > );
  DALI_TEST_EQUALS( constraint.GetTargetProperty(), (Property::Index)Actor::Property::POSITION, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintGetTargetPropertyN(void)
{
  // Attempt to retrieve from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    Property::Index propertyIndex = constraint.GetTargetProperty();
    ( void )propertyIndex;
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::SetTag
// Constraint::GetTag
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintTagP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &BasicFunction< Vector3 > );
  DALI_TEST_EQUALS( constraint.GetTag(), 0u, TEST_LOCATION );

  const unsigned int tag = 123;
  constraint.SetTag( tag );
  DALI_TEST_EQUALS( constraint.GetTag(), tag, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintSetTagN(void)
{
  // Attempt to set from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    constraint.SetTag( 123 );
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliConstraintGetTagN(void)
{
  // Attempt to retrieve from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    int tag = constraint.GetTag();
    ( void )tag;
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::SetRemoveAction
// Constraint::GetRemoveAction
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintRemoveActionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &BasicFunction< Vector3 > );
  DALI_TEST_EQUALS( constraint.GetRemoveAction(), Constraint::DEFAULT_REMOVE_ACTION, TEST_LOCATION );

  constraint.SetRemoveAction( Constraint::Discard );
  DALI_TEST_EQUALS( constraint.GetRemoveAction(), Constraint::Discard, TEST_LOCATION );

  constraint.SetRemoveAction( Constraint::Bake );
  DALI_TEST_EQUALS( constraint.GetRemoveAction(), Constraint::Bake, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintSetRemoveActionN(void)
{
  // Attempt to set from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    constraint.SetRemoveAction( Constraint::Discard );
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliConstraintGetRemoveActionN(void)
{
  // Attempt to retrieve from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    Constraint::RemoveAction removeAction = constraint.GetRemoveAction();
    ( void )removeAction;
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliConstraintBakeRemoveAction(void)
{
  // Ensure value is baked when constraint is removed

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // Should not equal position by default
  Vector3 position( 10.0f, 20.0f, 30.0f );
  DALI_TEST_CHECK( actor.GetCurrentPosition() != position );

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, SetValueFunctor< Vector3 >( position ) );
  constraint.SetRemoveAction( Constraint::Bake );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  // Remove the constraint, it should still be at position
  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintDiscardRemoveAction(void)
{
  // Ensure value is baked when constraint is removed

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // Get and store current position
  Vector3 originalPosition = actor.GetCurrentPosition();

  // Should not equal position by default
  Vector3 position( 10.0f, 20.0f, 30.0f );
  DALI_TEST_CHECK( actor.GetCurrentPosition() != position );

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, SetValueFunctor< Vector3 >( position ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  // Remove the constraint, it should still be at position
  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), originalPosition, TEST_LOCATION );
  DALI_TEST_CHECK( actor.GetCurrentPosition() != position );

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::Apply
// Constraint::Remove
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintApplyRemove(void)
{
  // Ensure constraint functors are called appropriately

  TestApplication application;
  bool functorCalled = false;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( functorCalled, false, TEST_LOCATION );

  // Create a constraint and apply, functor should be called
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicCalledFunctor< Vector3 >( functorCalled ) );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( functorCalled, true, TEST_LOCATION );

  // Reset
  functorCalled = false;

  // Remove the constraint, functor should not be called
  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( functorCalled, false, TEST_LOCATION );

  // Re-apply the constraint, functor should be called again
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( functorCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintApplyBeforeAddedToStage(void)
{
  // Constraint gets applied to an off-stage actor.
  // Constraint should be automatically applied when the actor is added to the stage and not before

  TestApplication application;
  bool functorCalled = false;

  // Create an actor and a constraint and apply, DON'T add to stage just yet
  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicCalledFunctor< Vector3 >( functorCalled ) );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Should NOT be called
  DALI_TEST_EQUALS( functorCalled, false, TEST_LOCATION );

  // Add actor to stage
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // Should now be called
  DALI_TEST_EQUALS( functorCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintApplyAndRemoveBeforeAddedToStage(void)
{
  // Constraint gets applied to an off-stage actor, then gets removed before it's added to the stage
  // Constraint should NOT be called at all

  TestApplication application;
  bool functorCalled = false;

  // Create an actor and a constraint and apply, DON'T add to stage just yet
  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicCalledFunctor< Vector3 >( functorCalled ) );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Should NOT be called
  DALI_TEST_EQUALS( functorCalled, false, TEST_LOCATION );

  // Remove the constraint
  constraint.Remove();

  // Add actor to stage
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // Still should NOT be called
  DALI_TEST_EQUALS( functorCalled, false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintApplyActorStagedUnstaged(void)
{
  // Apply a constraint to an actor which is staged and unstaged.
  // Functor should only be called while the actor is staged.

  TestApplication application;
  bool functorCalled = false;

  // Create an actor and add to stage
  Actor actor = Actor::New();
  Stage stage = Stage::GetCurrent();
  stage.Add( actor );

  // Create a constraint and apply
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicCalledFunctor< Vector3 >( functorCalled ) );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Constraint should be called
  DALI_TEST_EQUALS( functorCalled, true, TEST_LOCATION );

  // Reset
  functorCalled = false;

  // Remove actor from stage
  stage.Remove( actor );

  application.SendNotification();
  application.Render();

  // Constraint should NOT be called
  DALI_TEST_EQUALS( functorCalled, false, TEST_LOCATION );

  // Re-add to stage
  stage.Add( actor );

  application.SendNotification();
  application.Render();

  // Constraint should be called
  DALI_TEST_EQUALS( functorCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintApplySeveralTimes(void)
{
  // Apply the same constraint several times.
  // Should not cause any problems (subsequent attempts should be no-ops)

  TestApplication application;
  int count = 0;

  // Create an actor and add to stage
  Actor actor = Actor::New();
  Stage stage = Stage::GetCurrent();
  stage.Add( actor );

  // Create a constraint and apply
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, CalledCountFunctor< Vector3 >( count ) );
  constraint.Apply();

  // Apply again
  constraint.Apply(); // no-op

  application.SendNotification();
  application.Render();

  // Should only have been called once
  DALI_TEST_EQUALS( count, 1, TEST_LOCATION );

  // Reset
  count = 0;

  // Apply again
  constraint.Apply(); // no-op

  application.SendNotification();
  application.Render();

  // Constraint should not have been called as the input-properties (none) have not changed for the constraint
  DALI_TEST_EQUALS( count, 0, TEST_LOCATION );

  // Reset
  count = 0;

  // Change the position property, apply again
  actor.SetPosition( 10.0f, 10.0f );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Constraint should have been called once
  DALI_TEST_EQUALS( count, 1, TEST_LOCATION );

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::AddSource
///////////////////////////////////////////////////////////////////////////////
namespace UtcDaliConstraintAddSource
{
void Function( Vector3& /* current */, const PropertyInputContainer& inputs )
{
  DALI_TEST_EQUALS( inputs.Size(), 4u, TEST_LOCATION );
  DALI_TEST_EQUALS( inputs[0]->GetType(), Property::VECTOR3, TEST_LOCATION );
  DALI_TEST_EQUALS( inputs[1]->GetType(), Property::ROTATION, TEST_LOCATION );
  DALI_TEST_EQUALS( inputs[2]->GetType(), Property::VECTOR4, TEST_LOCATION );
  DALI_TEST_EQUALS( inputs[3]->GetType(), Property::BOOLEAN, TEST_LOCATION );
}
} // namespace UtcDaliConstraintAddSource

int UtcDaliConstraintAddSourceP(void)
{
  // Ensure all sources are in the correct order in the functor

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  // Create a constraint, add sources
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &UtcDaliConstraintAddSource::Function );
  constraint.AddSource( LocalSource( Actor::Property::SIZE ) );
  constraint.AddSource( LocalSource( Actor::Property::ORIENTATION ) );
  constraint.AddSource( LocalSource( Actor::Property::COLOR ) );
  constraint.AddSource( LocalSource( Actor::Property::VISIBLE ) );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliConstraintAddSourceN(void)
{
  // Attempt to set from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    constraint.AddSource( LocalSource( Actor::Property::POSITION ) );
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace TestChaining
{

const Vector3 gFunction1Output( Vector3::ONE );
void Function1( Vector3& current, const PropertyInputContainer& /* inputs */ )
{
  // current is original position
  DALI_TEST_EQUALS( current, Vector3::ZERO, TEST_LOCATION );
  current = gFunction1Output;
}

const Vector3 gFunction2Output( 10.0f, 20.0f, 30.0f );
void Function2( Vector3& current, const PropertyInputContainer& /* inputs */ )
{
  // current is output from Function1
  DALI_TEST_EQUALS( current, gFunction1Output, TEST_LOCATION );

  current = gFunction2Output;
}

const Vector3 gFunction3Output( 10.0f, 20.0f, 30.0f );
void Function3( Vector3& current, const PropertyInputContainer& /* inputs */ )
{
  // current is output from Function2
  DALI_TEST_EQUALS( current, gFunction2Output, TEST_LOCATION );

  current = gFunction3Output;
}

const Vector3 gFunction4Output( 10.0f, 20.0f, 30.0f );
void Function4( Vector3& current, const PropertyInputContainer& /* inputs */ )
{
  // current is output from Function3
  DALI_TEST_EQUALS( current, gFunction3Output, TEST_LOCATION );

  current = gFunction4Output;
}

void Function5( Vector3& current, const PropertyInputContainer& /* inputs */ )
{
  // current is output from Function4
  DALI_TEST_EQUALS( current, gFunction4Output, TEST_LOCATION );

  current = Vector3::ZERO;
}

} // namespace TestChaining

int UtcDaliConstraintChaining(void)
{
  // Apply several constraints to the same property and ensure the functors are called in the correct order.

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  Constraint constraint1 = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &TestChaining::Function1 );
  Constraint constraint2 = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &TestChaining::Function2 );
  Constraint constraint3 = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &TestChaining::Function3 );
  Constraint constraint4 = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &TestChaining::Function4 );
  Constraint constraint5 = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &TestChaining::Function5 );

  constraint1.Apply();
  constraint2.Apply();
  constraint3.Apply();
  constraint4.Apply();
  constraint5.Apply();

  application.SendNotification();
  application.Render();

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace TestPropertyTypes
{
template< typename T >
void Execute( T value )
{
  TestApplication application;
  bool functorCalled = false;

  Actor actor = Actor::New();
  Property::Index index = actor.RegisterProperty( "TEMP_PROPERTY_NAME", value );

  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( functorCalled, false, TEST_LOCATION );

  // Add a constraint
  Constraint constraint = Constraint::New< T >( actor, index, BasicCalledFunctor< T >( functorCalled ) );
  DALI_TEST_CHECK( constraint );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( functorCalled, true, TEST_LOCATION );
}
} // namespace UtcDaliConstraintNewFunctor

int UtcDaliConstraintTestPropertyTypesP(void)
{
  // Ensure we can use a constraint functor with all supported property types

  TestPropertyTypes::Execute< bool >( false );
  TestPropertyTypes::Execute< int >( 0 );
  TestPropertyTypes::Execute< float >( 0.0f );
  TestPropertyTypes::Execute< Vector2 >( Vector2::ZERO );
  TestPropertyTypes::Execute< Vector3 >( Vector3::ZERO );
  TestPropertyTypes::Execute< Vector4 >( Vector4::ZERO );
  TestPropertyTypes::Execute< Quaternion >( Quaternion::IDENTITY );
  TestPropertyTypes::Execute< Matrix >( Matrix::IDENTITY );
  TestPropertyTypes::Execute< Matrix3 >( Matrix3::IDENTITY );

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

