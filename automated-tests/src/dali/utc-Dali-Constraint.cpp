/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <mesh-builder.h>
#include <stdlib.h>

#include <iostream>

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
template<typename T>
void BasicFunction(T& /* current */, const PropertyInputContainer& /* inputs */)
{
}

/**
 * A functor which sets a given boolean when the functor is called.
 */
template<typename T>
struct BasicCalledFunctor
{
  BasicCalledFunctor(bool& functorCalled)
  : mCalled(functorCalled)
  {
  }

  void operator()(T& /* current */, const PropertyInputContainer& /* inputs */)
  {
    mCalled = true;
  }

  bool& mCalled;
};

/**
 * A functor which increments a given integer when the functor is called.
 */
template<typename T>
struct CalledCountFunctor
{
  CalledCountFunctor(int& callCount)
  : mCallCount(callCount)
  {
  }

  void operator()(T& /* current */, const PropertyInputContainer& /* inputs */)
  {
    ++mCallCount;
  }

  int& mCallCount;
};

/**
 * A functor which sets the given value as the value required when the functor is called.
 */
template<typename T>
struct SetValueFunctor
{
  SetValueFunctor(const T& value)
  : mValue(value)
  {
  }

  void operator()(T& current, const PropertyInputContainer& /* inputs */)
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
void ConstraintFunction(Vector3& /* current */, const PropertyInputContainer& /* inputs */)
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
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(UtcDaliConstraintNewFunction::gConstraintFunctionCalled, false, TEST_LOCATION);

  // Add a constraint
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &UtcDaliConstraintNewFunction::ConstraintFunction);
  DALI_TEST_CHECK(constraint);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(UtcDaliConstraintNewFunction::gConstraintFunctionCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintNewFunctionN(void)
{
  // Create a constraint with an uninitialised handle

  TestApplication application;

  // Add a constraint with an uninitialised handle
  try
  {
    Constraint constraint = Constraint::New<Vector3>(Actor(), Actor::Property::POSITION, &UtcDaliConstraintNewFunction::ConstraintFunction);
    DALI_TEST_CHECK(false); // Should not reach here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // Should assert!
  }

  END_TEST;
}

// helper for next test
void StringConstraintFunction(std::string& /* current */, const PropertyInputContainer& /* inputs */)
{
}

int UtcDaliConstraintNewFunctionNonConstrainableTypeN(void)
{
  // Ensure that we can create a constraint using a C function and that it is called.

  TestApplication application;
  UtcDaliConstraintNewFunction::gConstraintFunctionCalled = false;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  try
  {
    // Add a constraint
    Constraint constraint = Constraint::New<std::string>(actor, Actor::Property::COLOR_MODE, &StringConstraintFunction);
    DALI_TEST_CHECK(constraint);
    constraint.Apply();
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_ASSERT(e, "Property not constrainable", TEST_LOCATION);
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
  bool            functorCalled = false;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(functorCalled, false, TEST_LOCATION);

  // Add a constraint
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, BasicCalledFunctor<Vector3>(functorCalled));
  DALI_TEST_CHECK(constraint);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintNewFunctorN(void)
{
  // Create a constraint with an uninitialised handle

  TestApplication application;
  bool            functorCalled = false;

  // Add a constraint with an uninitialised handle
  try
  {
    Constraint constraint = Constraint::New<Vector3>(Actor(), Actor::Property::POSITION, BasicCalledFunctor<Vector3>(functorCalled));
    DALI_TEST_CHECK(false); // Should not reach here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // Should assert!
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
  Functor(bool& positionCalled, bool& scaleCalled)
  : mPositionCalled(positionCalled),
    mScaleCalled(scaleCalled)
  {
  }

  void Position(Vector3& /* current */, const PropertyInputContainer& /* inputs */)
  {
    mPositionCalled = true;
  }

  void Scale(Vector3& /* current */, const PropertyInputContainer& /* inputs */)
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
  bool            positionFunctorCalled = false;
  bool            sizeFunctorCalled     = false;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(positionFunctorCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeFunctorCalled, false, TEST_LOCATION);

  // Add a constraint that calls Functor::Position
  Constraint constraint = Constraint::New<Vector3>(
    actor,
    Actor::Property::POSITION,
    UtcDaliConstraintNewFunctorMember::Functor(positionFunctorCalled, sizeFunctorCalled),
    &UtcDaliConstraintNewFunctorMember::Functor::Position);
  DALI_TEST_CHECK(constraint);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(positionFunctorCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeFunctorCalled, false, TEST_LOCATION);

  // Add another constraint that calls Functor::Size
  Constraint constraint2 = Constraint::New<Vector3>(
    actor,
    Actor::Property::SCALE,
    UtcDaliConstraintNewFunctorMember::Functor(positionFunctorCalled, sizeFunctorCalled),
    &UtcDaliConstraintNewFunctorMember::Functor::Scale);
  DALI_TEST_CHECK(constraint2);
  constraint2.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(positionFunctorCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeFunctorCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintNewFunctorMemberN(void)
{
  // Create a constraint with an uninitialised handle

  TestApplication application;
  bool            positionFunctorCalled = false;
  bool            sizeFunctorCalled     = false;

  // Add a constraint with an uninitialised handle
  try
  {
    Constraint constraint = Constraint::New<Vector3>(
      Actor(),
      Actor::Property::POSITION,
      UtcDaliConstraintNewFunctorMember::Functor(positionFunctorCalled, sizeFunctorCalled),
      &UtcDaliConstraintNewFunctorMember::Functor::Position);
    DALI_TEST_CHECK(false); // Should not reach here
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_CHECK(true); // Should assert!
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
  int             calledCount = 0;

  Actor actor = Actor::New();
  Actor clone = Actor::New();

  Integration::Scene stage = application.GetScene();
  stage.Add(actor);
  stage.Add(clone);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);

  // Add a constraint to actor
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, CalledCountFunctor<Vector3>(calledCount));
  DALI_TEST_CHECK(constraint);
  constraint.Apply();

  // Create a clone but don't apply
  Constraint constraintClone = constraint.Clone(clone);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);

  // Reset
  calledCount = 0;

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);

  // Apply the clone constraint
  constraintClone.Apply();

  application.SendNotification();
  application.Render();

  // Should be called once for the new constraint clone and once for the original constraint
  DALI_TEST_EQUALS(calledCount, 3, TEST_LOCATION);

  // Reset
  calledCount = 0;

  // Change the position of both actors
  actor.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 100.0f));
  clone.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 100.0f));

  application.SendNotification();
  application.Render();

  // Functor should have been called twice
  DALI_TEST_EQUALS(calledCount, 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintCloneN(void)
{
  // Attempt to clone an uninitialised constraint should cause an assert

  TestApplication application;

  Constraint constraint;

  try
  {
    Actor      actor = Actor::New();
    Constraint clone = constraint.Clone(actor);
    DALI_TEST_CHECK(false);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}

namespace UtcDaliConstraintClone
{
void Function(Vector3& /* current */, const PropertyInputContainer& inputs)
{
  DALI_TEST_EQUALS(inputs[0]->GetType(), Property::VECTOR3, TEST_LOCATION);
  DALI_TEST_EQUALS(inputs[1]->GetType(), Property::ROTATION, TEST_LOCATION);
  DALI_TEST_EQUALS(inputs[2]->GetType(), Property::VECTOR4, TEST_LOCATION);
  DALI_TEST_EQUALS(inputs[3]->GetType(), Property::BOOLEAN, TEST_LOCATION);
}
} // namespace UtcDaliConstraintClone

int UtcDaliConstraintCloneCheckSourcesAndSetters(void)
{
  // Ensure all sources, the tag and remove-action are cloned appropriately

  TestApplication application;

  Actor actor = Actor::New();
  Actor clone = Actor::New();

  Integration::Scene stage = application.GetScene();
  stage.Add(actor);
  stage.Add(clone);

  application.SendNotification();
  application.Render();

  // Create a constraint, DON'T Apply it though
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &UtcDaliConstraintClone::Function);
  constraint.AddSource(LocalSource(Actor::Property::SIZE));
  constraint.AddSource(LocalSource(Actor::Property::ORIENTATION));
  constraint.AddSource(LocalSource(Actor::Property::COLOR));
  constraint.AddSource(LocalSource(Actor::Property::VISIBLE));
  constraint.SetRemoveAction(Constraint::DISCARD);
  constraint.SetApplyRate(3u);
  constraint.SetTag(123);

  // Clone the constraint & apply the clone
  Constraint constraintClone = constraint.Clone(clone);
  constraintClone.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(constraint.GetRemoveAction(), constraintClone.GetRemoveAction(), TEST_LOCATION);
  DALI_TEST_EQUALS(constraint.GetApplyRate(), constraintClone.GetApplyRate(), TEST_LOCATION);
  DALI_TEST_EQUALS(constraint.GetTag(), constraintClone.GetTag(), TEST_LOCATION);

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
  application.GetScene().Add(actor);

  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  Constraint copied(constraint);
  Constraint assigned;
  DALI_TEST_CHECK(constraint == copied);
  DALI_TEST_CHECK(copied != assigned);

  assigned = constraint;
  DALI_TEST_CHECK(constraint == assigned);

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

int UtcDaliConstraintMoveConstructor(void)
{
  // Ensure copy constructor & assignment operators work

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_CHECK(constraint);
  DALI_TEST_EQUALS(1, constraint.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(constraint.GetTargetObject() == actor);

  Constraint moved = std::move(constraint);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(moved.GetTargetObject() == actor);
  DALI_TEST_CHECK(!constraint);

  END_TEST;
}

int UtcDaliConstraintMoveAssignment(void)
{
  // Ensure copy constructor & assignment operators work

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_CHECK(constraint);
  DALI_TEST_EQUALS(1, constraint.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(constraint.GetTargetObject() == actor);

  Constraint moved;
  moved = std::move(constraint);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(moved.GetTargetObject() == actor);
  DALI_TEST_CHECK(!constraint);

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////
// Constraint::DownCast
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintDownCast(void)
{
  // Ensure DownCast works as expected

  TestApplication application;

  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);

  // Another BaseHandle type
  Constraint downCast = Constraint::DownCast(actor);
  DALI_TEST_CHECK(!downCast);

  // A constraint
  downCast = Constraint::DownCast(constraint);
  DALI_TEST_CHECK(downCast);

  // An empty constraint
  downCast = Constraint::DownCast(Constraint());
  DALI_TEST_CHECK(!downCast);

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::GetTargetObject
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintGetTargetObjectP(void)
{
  TestApplication application;

  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_CHECK(constraint.GetTargetObject() == actor);

  Actor actor2 = Actor::New();
  DALI_TEST_CHECK(constraint.GetTargetObject() != actor2);

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
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
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

  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_EQUALS(constraint.GetTargetProperty(), (Property::Index)Actor::Property::POSITION, TEST_LOCATION);

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
    (void)propertyIndex;
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
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

  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_EQUALS(constraint.GetTag(), 0u, TEST_LOCATION);

  const unsigned int tag = 123;
  constraint.SetTag(tag);
  DALI_TEST_EQUALS(constraint.GetTag(), tag, TEST_LOCATION);

  const unsigned int tag2 = Dali::ConstraintTagRanges::CUSTOM_CONSTRAINT_TAG_MAX;
  constraint.SetTag(tag2);
  DALI_TEST_EQUALS(constraint.GetTag(), tag2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintSetTagN1(void)
{
  // Attempt to set from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    constraint.SetTag(123);
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}

int UtcDaliConstraintSetTagN2(void)
{
  // Attempt to set out of custom tag ranges

  TestApplication application;

  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_EQUALS(constraint.GetTag(), 0u, TEST_LOCATION);

  try
  {
    const uint32_t tag = Dali::ConstraintTagRanges::CUSTOM_CONSTRAINT_TAG_MAX + 1u;
    constraint.SetTag(tag);
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
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
    (void)tag;
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
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

  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_EQUALS(constraint.GetRemoveAction(), Constraint::DEFAULT_REMOVE_ACTION, TEST_LOCATION);

  constraint.SetRemoveAction(Constraint::DISCARD);
  DALI_TEST_EQUALS(constraint.GetRemoveAction(), Constraint::DISCARD, TEST_LOCATION);

  constraint.SetRemoveAction(Constraint::BAKE);
  DALI_TEST_EQUALS(constraint.GetRemoveAction(), Constraint::BAKE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintSetRemoveActionN(void)
{
  // Attempt to set from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    constraint.SetRemoveAction(Constraint::DISCARD);
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
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
    (void)removeAction;
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}

int UtcDaliConstraintBakeRemoveAction(void)
{
  // Ensure value is baked when constraint is removed

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Should not equal position by default
  Vector3 position(10.0f, 20.0f, 30.0f);
  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION) != position);

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, SetValueFunctor<Vector3>(position));
  constraint.SetRemoveAction(Constraint::BAKE);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), position, TEST_LOCATION);

  // Remove the constraint, it should still be at position
  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), position, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), position, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintDiscardRemoveAction(void)
{
  // Ensure value is baked when constraint is removed

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Get and store current position
  Vector3 originalPosition = actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);

  // Should not equal position by default
  Vector3 position(10.0f, 20.0f, 30.0f);
  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION) != position);

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, SetValueFunctor<Vector3>(position));
  constraint.SetRemoveAction(Constraint::DISCARD);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), position, TEST_LOCATION);

  // Remove the constraint, it should still be at position
  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), originalPosition, TEST_LOCATION);
  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION) != position);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), originalPosition, TEST_LOCATION);
  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION) != position);

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::SetApplyRate
// Constraint::GetApplyRate
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintApplyRateP(void)
{
  TestApplication application;

  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_EQUALS(constraint.GetApplyRate(), Constraint::APPLY_ALWAYS, TEST_LOCATION);

  constraint.SetApplyRate(Constraint::APPLY_ONCE);
  DALI_TEST_EQUALS(constraint.GetApplyRate(), Constraint::APPLY_ONCE, TEST_LOCATION);

  constraint.SetApplyRate(7u);
  DALI_TEST_EQUALS(constraint.GetApplyRate(), 7u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintSetApplyRateN(void)
{
  // Attempt to set from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    constraint.SetApplyRate(Constraint::APPLY_ONCE);
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}

int UtcDaliConstraintGetApplyRateN(void)
{
  // Attempt to retrieve from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    uint32_t applyRate = constraint.GetApplyRate();
    (void)applyRate;
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}

int UtcDaliConstraintSetApplyRateCallbackCount01(void)
{
  // Ensure the apply callback called only once

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  // Dummy animation to make actor is always dirty.
  Animation animation = Animation::New(1);
  animation.AnimateTo(Property(actor, Actor::Property::OPACITY), 0.5f);
  animation.SetLooping(true);
  animation.Play();

  application.SendNotification();
  application.Render();

  int calledCount = 0;

  DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, CalledCountFunctor<Vector3>(calledCount));
  constraint.SetRemoveAction(Constraint::BAKE);
  constraint.SetApplyRate(Constraint::APPLY_ONCE);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);
  calledCount = 0;

  // Check apply function called only once.
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);
  calledCount = 0;

  constraint.SetApplyRate(Constraint::APPLY_ONCE);

  // Check changing apply-rate call function.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);
  calledCount = 0;

  // Check apply function called only once.
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);
  calledCount = 0;

  constraint.SetApplyRate(Constraint::APPLY_ALWAYS);

  for(int trialCount = 0; trialCount < 10; ++trialCount)
  {
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);
    calledCount = 0;
  }

  // Check function call per every 3 frames. It will be called apply rate changed frame.
  constraint.SetApplyRate(3u);

  for(int trialCount = 0; trialCount < 10; ++trialCount)
  {
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);
    calledCount = 0;

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);
    calledCount = 0;
  }

  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);
  calledCount = 0;

  END_TEST;
}

int UtcDaliConstraintSetApplyRateCallbackCount02(void)
{
  // Ensure the apply callback called only once

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  // Dummy animation to make actor is always dirty.
  Animation animation = Animation::New(1);
  animation.AnimateTo(Property(actor, Actor::Property::OPACITY), 0.5f);
  animation.SetLooping(true);
  animation.Play();

  application.SendNotification();
  application.Render();

  int calledCount = 0;

  DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);

  Property::Index customIndex = actor.RegisterProperty("customProperty", 0.0f);

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New<float>(actor, customIndex, CalledCountFunctor<float>(calledCount));
  constraint.SetRemoveAction(Constraint::BAKE);
  constraint.SetApplyRate(Constraint::APPLY_ONCE);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);
  calledCount = 0;

  // Check apply function called only once.
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);
  calledCount = 0;

  constraint.SetApplyRate(Constraint::APPLY_ONCE);

  // Check changing apply-rate call function.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);
  calledCount = 0;

  // Check apply function called only once.
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);
  calledCount = 0;

  constraint.SetApplyRate(Constraint::APPLY_ALWAYS);

  for(int trialCount = 0; trialCount < 10; ++trialCount)
  {
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);
    calledCount = 0;
  }

  // Check function call per every 3 frames. It will be called apply rate changed frame.
  constraint.SetApplyRate(3u);

  for(int trialCount = 0; trialCount < 10; ++trialCount)
  {
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);
    calledCount = 0;

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);
    calledCount = 0;
  }

  // Change to APPLY_ONCE again (for line coverage)
  constraint.SetApplyRate(Constraint::APPLY_ONCE);

  // Check changing apply-rate call function.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 1, TEST_LOCATION);
  calledCount = 0;

  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(calledCount, 0, TEST_LOCATION);
  calledCount = 0;

  END_TEST;
}

int UtcDaliConstraintSetApplyRateOnceBackEndAction(void)
{
  // Ensure value is baked when constraint is removed

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Should not equal position by default
  Vector3 position(10.0f, 20.0f, 30.0f);
  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION) != position);

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, SetValueFunctor<Vector3>(position));
  constraint.SetRemoveAction(Constraint::BAKE);
  constraint.SetApplyRate(Constraint::APPLY_ONCE);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), position, TEST_LOCATION);

  // Remove the constraint, it should still be at position
  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), position, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), position, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintSetApplyRateOnceDiscardEndAction(void)
{
  // Ensure value is baked when constraint is removed

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Get and store current position
  Vector3 originalPosition = actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);

  // Should not equal position by default
  Vector3 position(10.0f, 20.0f, 30.0f);
  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION) != position);

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, SetValueFunctor<Vector3>(position));
  constraint.SetRemoveAction(Constraint::DISCARD);
  constraint.SetApplyRate(Constraint::APPLY_ONCE);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), position, TEST_LOCATION);

  // Remove the constraint, it should still be at position
  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), originalPosition, TEST_LOCATION);
  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION) != position);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), originalPosition, TEST_LOCATION);
  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION) != position);

  END_TEST;
}

int UtcDaliConstraintSetApplyRateOnceAfterChangeValue(void)
{
  // Ensure value is baked when constraint is removed

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Should not equal position by default
  Vector3 originalValue(1.0f, 2.0f, 3.0f);
  Vector3 targetValue(10.0f, 20.0f, 30.0f);

  Property::Index customIndex = actor.RegisterProperty("customProperty", originalValue);

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New<Vector3>(actor, customIndex, SetValueFunctor<Vector3>(targetValue));
  constraint.SetRemoveAction(Constraint::BAKE);
  constraint.SetApplyRate(Constraint::APPLY_ONCE);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(customIndex), targetValue, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(customIndex), targetValue, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(customIndex), targetValue, TEST_LOCATION);

  tet_printf("Change value by set property\n");
  Vector3 changedValue(4.0f, 5.0f, 6.0f);
  actor.SetProperty(customIndex, changedValue);
  DALI_TEST_EQUALS(actor.GetProperty<Vector3>(customIndex), changedValue, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(customIndex), targetValue, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(customIndex), changedValue, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(customIndex), changedValue, TEST_LOCATION);

  tet_printf("Set APPLY_ONCE again\n");
  constraint.SetApplyRate(Constraint::APPLY_ONCE);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(customIndex), targetValue, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(customIndex), targetValue, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(customIndex), targetValue, TEST_LOCATION);

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
  bool            functorCalled = false;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(functorCalled, false, TEST_LOCATION);

  // Create a constraint and apply, functor should be called
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, BasicCalledFunctor<Vector3>(functorCalled));
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);

  // Reset
  functorCalled = false;

  // Remove the constraint, functor should not be called
  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(functorCalled, false, TEST_LOCATION);

  // Re-apply the constraint, functor should be called again
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintApplyBeforeAddedToStage(void)
{
  // Constraint gets applied to an off-stage actor.
  // Constraint should be automatically applied when the actor is added to the stage and not before

  TestApplication application;
  bool            functorCalled = false;

  // Create an actor and a constraint and apply, DON'T add to stage just yet
  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, BasicCalledFunctor<Vector3>(functorCalled));
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Should NOT be called
  DALI_TEST_EQUALS(functorCalled, false, TEST_LOCATION);

  // Add actor to stage
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Should now be called
  DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintApplyAndRemoveBeforeAddedToStage(void)
{
  // Constraint gets applied to an off-stage actor, then gets removed before it's added to the stage
  // Constraint should NOT be called at all

  TestApplication application;
  bool            functorCalled = false;

  // Create an actor and a constraint and apply, DON'T add to stage just yet
  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, BasicCalledFunctor<Vector3>(functorCalled));
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Should NOT be called
  DALI_TEST_EQUALS(functorCalled, false, TEST_LOCATION);

  // Remove the constraint
  constraint.Remove();

  // Add actor to stage
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Still should NOT be called
  DALI_TEST_EQUALS(functorCalled, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintApplyActorStagedUnstaged(void)
{
  // Apply a constraint to an actor which is staged and unstaged.
  // Functor should only be called while the actor is staged.

  TestApplication application;
  bool            functorCalled = false;

  // Create an actor and add to stage
  Actor              actor = Actor::New();
  Integration::Scene stage = application.GetScene();
  stage.Add(actor);

  // Create a constraint and apply
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, BasicCalledFunctor<Vector3>(functorCalled));
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Constraint should be called
  DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);

  // Reset
  functorCalled = false;

  // Remove actor from stage
  stage.Remove(actor);

  application.SendNotification();
  application.Render();

  // Constraint should NOT be called
  DALI_TEST_EQUALS(functorCalled, false, TEST_LOCATION);

  // Re-add to stage
  stage.Add(actor);

  application.SendNotification();
  application.Render();

  // Constraint should be called
  DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintApplySeveralTimes(void)
{
  // Apply the same constraint several times.
  // Should not cause any problems (subsequent attempts should be no-ops)

  TestApplication application;
  int             count = 0;

  // Create an actor and add to stage
  Actor              actor = Actor::New();
  Integration::Scene stage = application.GetScene();
  stage.Add(actor);

  // Create a constraint and apply
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, CalledCountFunctor<Vector3>(count));
  constraint.Apply();

  // Apply again
  constraint.Apply(); // no-op

  application.SendNotification();
  application.Render();

  // Should only have been called once
  DALI_TEST_EQUALS(count, 1, TEST_LOCATION);

  // Reset
  count = 0;

  // Apply again
  constraint.Apply(); // no-op

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(count, 1, TEST_LOCATION);

  // Reset
  count = 0;

  // Change the position property, apply again
  actor.SetProperty(Actor::Property::POSITION, Vector2(10.0f, 10.0f));
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Constraint should have been called once
  DALI_TEST_EQUALS(count, 1, TEST_LOCATION);

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::AddSource
///////////////////////////////////////////////////////////////////////////////
namespace UtcDaliConstraintAddSource
{
void Function(Vector3& /* current */, const PropertyInputContainer& inputs)
{
  DALI_TEST_EQUALS(inputs.Size(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(inputs[0]->GetType(), Property::VECTOR3, TEST_LOCATION);
  DALI_TEST_EQUALS(inputs[1]->GetType(), Property::ROTATION, TEST_LOCATION);
  DALI_TEST_EQUALS(inputs[2]->GetType(), Property::VECTOR4, TEST_LOCATION);
  DALI_TEST_EQUALS(inputs[3]->GetType(), Property::BOOLEAN, TEST_LOCATION);
}
} // namespace UtcDaliConstraintAddSource

int UtcDaliConstraintAddSourceP(void)
{
  // Ensure all sources are in the correct order in the functor

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  // Create a constraint, add sources
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &UtcDaliConstraintAddSource::Function);
  constraint.AddSource(LocalSource(Actor::Property::SIZE));
  constraint.AddSource(LocalSource(Actor::Property::ORIENTATION));
  constraint.AddSource(LocalSource(Actor::Property::COLOR));
  constraint.AddSource(LocalSource(Actor::Property::VISIBLE));
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
    constraint.AddSource(LocalSource(Actor::Property::POSITION));
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace TestChaining
{
const Vector3 gFunction1Output(Vector3::ONE);
void          Function1(Vector3& current, const PropertyInputContainer& /* inputs */)
{
  // current is original position
  DALI_TEST_EQUALS(current, Vector3::ZERO, TEST_LOCATION);
  current = gFunction1Output;
}

const Vector3 gFunction2Output(10.0f, 20.0f, 30.0f);
void          Function2(Vector3& current, const PropertyInputContainer& /* inputs */)
{
  // current is output from Function1
  DALI_TEST_EQUALS(current, gFunction1Output, TEST_LOCATION);

  current = gFunction2Output;
}

const Vector3 gFunction3Output(10.0f, 20.0f, 30.0f);
void          Function3(Vector3& current, const PropertyInputContainer& /* inputs */)
{
  // current is output from Function2
  DALI_TEST_EQUALS(current, gFunction2Output, TEST_LOCATION);

  current = gFunction3Output;
}

const Vector3 gFunction4Output(10.0f, 20.0f, 30.0f);
void          Function4(Vector3& current, const PropertyInputContainer& /* inputs */)
{
  // current is output from Function3
  DALI_TEST_EQUALS(current, gFunction3Output, TEST_LOCATION);

  current = gFunction4Output;
}

void Function5(Vector3& current, const PropertyInputContainer& /* inputs */)
{
  // current is output from Function4
  DALI_TEST_EQUALS(current, gFunction4Output, TEST_LOCATION);

  current = Vector3::ZERO;
}

} // namespace TestChaining

int UtcDaliConstraintChaining(void)
{
  // Apply several constraints to the same property and ensure the functors are called in the correct order.

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  Constraint constraint1 = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &TestChaining::Function1);
  Constraint constraint2 = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &TestChaining::Function2);
  Constraint constraint3 = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &TestChaining::Function3);
  Constraint constraint4 = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &TestChaining::Function4);
  Constraint constraint5 = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &TestChaining::Function5);

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
template<typename T>
void Execute(T value)
{
  TestApplication application;
  bool            functorCalled = false;

  Actor           actor = Actor::New();
  Property::Index index = actor.RegisterProperty("TEMP_PROPERTY_NAME", value);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(functorCalled, false, TEST_LOCATION);

  // Add a constraint
  Constraint constraint = Constraint::New<T>(actor, index, BasicCalledFunctor<T>(functorCalled));
  DALI_TEST_CHECK(constraint);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);
}
} // namespace TestPropertyTypes

int UtcDaliConstraintTestPropertyTypesP(void)
{
  // Ensure we can use a constraint functor with all supported property types

  TestPropertyTypes::Execute<bool>(false);
  TestPropertyTypes::Execute<int>(0);
  TestPropertyTypes::Execute<float>(0.0f);
  TestPropertyTypes::Execute<Vector2>(Vector2::ZERO);
  TestPropertyTypes::Execute<Vector3>(Vector3::ZERO);
  TestPropertyTypes::Execute<Vector4>(Vector4::ZERO);
  TestPropertyTypes::Execute<Quaternion>(Quaternion::IDENTITY);
  TestPropertyTypes::Execute<Matrix>(Matrix::IDENTITY);
  TestPropertyTypes::Execute<Matrix3>(Matrix3::IDENTITY);

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace
{
void SetHalfOpacity(Vector4& current, const PropertyInputContainer& inputs)
{
  current.a = 0.5f;
}
} // unnamed namespace

int UtcDaliConstraintEnsureResetterAppliedOnSceneRemoval(void)
{
  // Ensure BOTH double-buffered values of our color property is reset when a constraint is applied to it.

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  // Check initial value is fully opaque
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 1.0f, TEST_LOCATION);

  // Create a constraint whose value is discarded when it is removed
  Constraint constraint = Constraint::New<Vector4>(actor, Actor::Property::COLOR, SetHalfOpacity);
  constraint.SetRemoveAction(Constraint::RemoveAction::DISCARD);
  constraint.Apply();

  // Check value after one render, it should be constrained
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 0.5f, TEST_LOCATION);

  // Render another frame, ensure the other value has also been updated
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 0.5f, TEST_LOCATION);

  // Remove the actor from the stage and delete the constraint
  actor.Unparent();
  constraint.Remove();
  constraint.Reset();

  // Check value while off-stage, it should be fully opaque
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 1.0f, TEST_LOCATION);

  // Add the actor back to the stage and check the value, it should be fully opaque again
  application.GetScene().Add(actor);

  // Check value when back on-stage, it should be fully opaque as the constraint is no longer applied to it.
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 1.0f, TEST_LOCATION);

  // Render for another frame to ensure both buffers have the correct value
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintOnActorAddedAndRemoved(void)
{
  // Ensure adding and removing an actor from stage with a constraint still has it applied when it is re-added back to the stage

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  // Check initial value is fully opaque
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 1.0f, TEST_LOCATION);

  // Create a constraint whose value is discarded when it is removed
  Constraint constraint = Constraint::New<Vector4>(actor, Actor::Property::COLOR, SetHalfOpacity);
  constraint.SetRemoveAction(Constraint::RemoveAction::DISCARD);
  constraint.Apply();

  // Check value after one render, it should be constrained
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 0.5f, TEST_LOCATION);

  // Render another frame, ensure the other value has also been updated
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 0.5f, TEST_LOCATION);

  // Remove the actor from the stage
  actor.Unparent();

  // Check value while off-stage, the constraint is no longer being applied as it's off-stage
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 1.0f, TEST_LOCATION);

  // Check the other buffer, the constraint should not be applied to this either.
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 1.0f, TEST_LOCATION);

  // Add the actor back to the stage and check the value, the constraint should have been re-applied
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 0.5f, TEST_LOCATION);

  // Render for another frame to ensure both buffers have the correct value
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 0.5f, TEST_LOCATION);

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

int UtcDaliConstraintGetTargetObjectNegative(void)
{
  TestApplication  application;
  Dali::Constraint instance;
  try
  {
    instance.GetTargetObject();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliConstraintSetRemoveActionNegative(void)
{
  TestApplication  application;
  Dali::Constraint instance;
  try
  {
    Dali::Constraint::RemoveAction arg1(Constraint::BAKE);
    instance.SetRemoveAction(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliConstraintGetTargetPropertyNegative(void)
{
  TestApplication  application;
  Dali::Constraint instance;
  try
  {
    instance.GetTargetProperty();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliConstraintApplyNegative(void)
{
  TestApplication  application;
  Dali::Constraint instance;
  try
  {
    instance.Apply();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliConstraintCloneNegative(void)
{
  TestApplication  application;
  Dali::Constraint instance;
  try
  {
    Dali::Handle arg1;
    instance.Clone(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliConstraintRemoveNegative(void)
{
  TestApplication  application;
  Dali::Constraint instance;
  try
  {
    instance.Remove();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliConstraintSetTagNegative(void)
{
  TestApplication  application;
  Dali::Constraint instance;
  try
  {
    unsigned int arg1(0u);
    instance.SetTag(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliConstraintGetRemoveActionNegative(void)
{
  TestApplication  application;
  Dali::Constraint instance;
  try
  {
    instance.GetRemoveAction();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliConstraintGetTagNegative(void)
{
  TestApplication  application;
  Dali::Constraint instance;
  try
  {
    instance.GetTag();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

namespace ComponentTest
{
void CheckComponentProperty(TestApplication& application, Actor& actor, Property::Index property)
{
  float value = actor.GetCurrentProperty<float>(property);

  // Add a component 0 constraint
  RelativeToConstraintFloat relativeConstraint(2.0f);
  Constraint                constraint = Constraint::New<float>(actor, property, relativeConstraint);
  constraint.AddSource(Source{actor, property});
  DALI_TEST_CHECK(constraint);
  constraint.SetRemoveAction(Constraint::RemoveAction::DISCARD);
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<float>(property), value * 2.0f, TEST_LOCATION);

  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<float>(property), value, TEST_LOCATION);
}
} // namespace ComponentTest

int UtcDaliConstraintComponentTransformPropertyConstraintP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::POSITION, Vector3(100.0f, 100.0f, 100.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(100.0f, 100.0f, 100.0f), TEST_LOCATION);

  ComponentTest::CheckComponentProperty(application, actor, Actor::Property::POSITION_X); // Component 0
  ComponentTest::CheckComponentProperty(application, actor, Actor::Property::POSITION_Y); // Component 1
  ComponentTest::CheckComponentProperty(application, actor, Actor::Property::POSITION_Z); // Component 2

  END_TEST;
}

int UtcDaliConstraintComponentNonTransformPropertyConstraintP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::COLOR, Vector4(0.25f, 0.25f, 0.25f, 0.25f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR), Vector4(0.25f, 0.25f, 0.25f, 0.25f), TEST_LOCATION);

  ComponentTest::CheckComponentProperty(application, actor, Actor::Property::COLOR_RED);   // Component 0
  ComponentTest::CheckComponentProperty(application, actor, Actor::Property::COLOR_GREEN); // Component 1
  ComponentTest::CheckComponentProperty(application, actor, Actor::Property::COLOR_BLUE);  // Component 2
  ComponentTest::CheckComponentProperty(application, actor, Actor::Property::COLOR_ALPHA); // Component 3

  END_TEST;
}

namespace PostConstraintTest
{
void CheckComponentProperty(TestApplication& application, Actor& actor, Handle target)
{
  actor.SetProperty(Actor::Property::POSITION, Vector3::ONE);
  DALI_TEST_EQUALS(actor.GetProperty<Vector3>(Actor::Property::POSITION), Vector3::ONE, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  actor.SetProperty(Actor::Property::POSITION, Vector3::ONE * 2.0f);

  DALI_TEST_EQUALS(actor.GetProperty<Vector3>(Actor::Property::POSITION), Vector3::ONE * 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3::ONE, TEST_LOCATION);

  Property::Index prePropertyIndex = target.RegisterProperty("testPreProperty", Vector3::ZERO);
  Constraint      preConstraint    = Constraint::New<Vector3>(target, prePropertyIndex, [](Vector3& output, const PropertyInputContainer& inputs)
                                                      { output = inputs[0]->GetVector3(); });
  preConstraint.AddSource(Source{actor, Actor::Property::WORLD_POSITION});
  preConstraint.Apply();

  Property::Index postPropertyIndex = target.RegisterProperty("testPostProperty", Vector3::ZERO);
  Constraint      postConstraint    = Constraint::New<Vector3>(target, postPropertyIndex, [](Vector3& output, const PropertyInputContainer& inputs)
                                                       { output = inputs[0]->GetVector3(); });
  postConstraint.AddSource(Source{actor, Actor::Property::WORLD_POSITION});
  postConstraint.ApplyPost();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(target.GetCurrentProperty<Vector3>(prePropertyIndex), Vector3(-239.0, -399.0, 1.0), TEST_LOCATION);
  DALI_TEST_EQUALS(target.GetCurrentProperty<Vector3>(postPropertyIndex), Vector3(-238.0, -398.0, 2.0), TEST_LOCATION);

  preConstraint.Remove();
  postConstraint.Remove();
}
} // namespace PostConstraintTest

int UtcDaliConstraintApplyPost(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  Geometry       targetGeometry = CreateQuadGeometry();
  Shader         targetShader   = CreateShader();
  Renderer       targetRenderer = Renderer::New(targetGeometry, targetShader);
  Actor          targetActor    = Actor::New();
  RenderTaskList taskList       = application.GetScene().GetRenderTaskList();

  application.GetScene().Add(targetActor);
  PostConstraintTest::CheckComponentProperty(application, actor, targetShader);         // Shader
  PostConstraintTest::CheckComponentProperty(application, actor, targetRenderer);       // Renderer
  PostConstraintTest::CheckComponentProperty(application, actor, targetActor);          // Actor(Node)
  PostConstraintTest::CheckComponentProperty(application, actor, taskList.GetTask(0u)); // RenderTask

  END_TEST;
}