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
#include <dali/integration-api/constraint-integ.h>
#include <dali/public-api/dali-core.h>

using namespace Dali;

void utc_dali_internal_constraint_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_constraint_cleanup(void)
{
  test_return_value = TET_PASS;
}

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
 * TestConstraint reference.
 * When constraint is called, the resultRef is updated
 * with the value supplied.
 */
template<typename T>
struct TestConstraintRef
{
  TestConstraintRef(unsigned int& resultRef, unsigned int value)
  : mResultRef(resultRef),
    mValue(value)
  {
  }

  void operator()(T& current, const PropertyInputContainer& /* inputs */)
  {
    mResultRef = mValue;
  }

  unsigned int& mResultRef;
  unsigned int  mValue;
};
} // namespace

int UtcDaliInternalConstraintSetInternalTag(void)
{
  TestApplication application;

  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_EQUALS(constraint.GetTag(), 0u, TEST_LOCATION);

  const uint32_t tag = Dali::ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START;
  Dali::Integration::ConstraintSetInternalTag(constraint, tag);
  DALI_TEST_EQUALS(constraint.GetTag(), tag, TEST_LOCATION);

  const uint32_t tag2 = Dali::ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_MAX;
  Dali::Integration::ConstraintSetInternalTag(constraint, tag2);
  DALI_TEST_EQUALS(constraint.GetTag(), tag2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalConstraintSetInternalTagN01(void)
{
  TestApplication application;

  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_EQUALS(constraint.GetTag(), 0u, TEST_LOCATION);

  try
  {
    const uint32_t tag = Dali::ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START - 1u;
    Dali::Integration::ConstraintSetInternalTag(constraint, tag);
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }
  END_TEST;
}

int UtcDaliInternalConstraintSetInternalTagN02(void)
{
  TestApplication application;

  Actor      actor      = Actor::New();
  Constraint constraint = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_EQUALS(constraint.GetTag(), 0u, TEST_LOCATION);

  try
  {
    const uint32_t tag = Dali::ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_MAX + 1u;
    Dali::Integration::ConstraintSetInternalTag(constraint, tag);
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }
  END_TEST;
}

int UtcDaliInternalConstraintSetInternalTagN03(void)
{
  tet_infoline("Do not allow to set cross-tag between custom and internal.");
  TestApplication application;

  Actor      actor       = Actor::New();
  Constraint constraint1 = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_EQUALS(constraint1.GetTag(), Dali::ConstraintTagRanges::DEFAULT_TAG, TEST_LOCATION);

  Constraint constraint2 = Constraint::New<Vector3>(actor, Actor::Property::POSITION, &BasicFunction<Vector3>);
  DALI_TEST_EQUALS(constraint2.GetTag(), Dali::ConstraintTagRanges::DEFAULT_TAG, TEST_LOCATION);

  const uint32_t internalTag = Dali::ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START;
  Dali::Integration::ConstraintSetInternalTag(constraint1, internalTag);
  DALI_TEST_EQUALS(constraint1.GetTag(), internalTag, TEST_LOCATION);

  const uint32_t customTag = Dali::ConstraintTagRanges::CUSTOM_CONSTRAINT_TAG_START;
  constraint2.SetTag(customTag);
  DALI_TEST_EQUALS(constraint2.GetTag(), customTag, TEST_LOCATION);
  try
  {
    constraint1.SetTag(customTag);
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "\"Cross tag setting is not allowed!\"", TEST_LOCATION);
  }

  try
  {
    Dali::Integration::ConstraintSetInternalTag(constraint2, internalTag);
    DALI_TEST_CHECK(false); // Should not reach here!
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "\"Cross tag setting is not allowed!\"", TEST_LOCATION);
  }

  // But allow to set Default tag, which is 0
  constraint1.SetTag(Dali::ConstraintTagRanges::DEFAULT_TAG);
  DALI_TEST_EQUALS(constraint1.GetTag(), Dali::ConstraintTagRanges::DEFAULT_TAG, TEST_LOCATION);
  constraint2.SetTag(Dali::ConstraintTagRanges::DEFAULT_TAG);
  DALI_TEST_EQUALS(constraint2.GetTag(), Dali::ConstraintTagRanges::DEFAULT_TAG, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalConstraintHandleRemoveConstraints(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  uint32_t result1 = 0u;
  uint32_t result2 = 0u;
  uint32_t result3 = 0u;
  uint32_t result4 = 0u;

  uint32_t   constraint1Tag = 1u;
  Constraint constraint1    = Constraint::New<Vector4>(actor, Actor::Property::COLOR, TestConstraintRef<Vector4>(result1, 1));
  constraint1.SetTag(constraint1Tag);
  constraint1.Apply();

  uint32_t   constraint2Tag = 2u;
  Constraint constraint2    = Constraint::New<Vector4>(actor, Actor::Property::COLOR, TestConstraintRef<Vector4>(result2, 2));
  constraint2.SetTag(constraint2Tag);
  constraint2.Apply();

  uint32_t   internalConstraint3Tag = Dali::ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START + 1u;
  Constraint internalConstraint3    = Constraint::New<Vector4>(actor, Actor::Property::COLOR, TestConstraintRef<Vector4>(result3, 3));
  Dali::Integration::ConstraintSetInternalTag(internalConstraint3, internalConstraint3Tag);
  internalConstraint3.Apply();

  uint32_t   internalConstraint4Tag = Dali::ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START + 2u;
  Constraint internalConstraint4    = Constraint::New<Vector4>(actor, Actor::Property::COLOR, TestConstraintRef<Vector4>(result4, 4));
  Dali::Integration::ConstraintSetInternalTag(internalConstraint4, internalConstraint4Tag);
  internalConstraint4.Apply();

  application.GetScene().Add(actor);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result2, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result3, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(result4, 4u, TEST_LOCATION);

  tet_printf("Test 1 : Handle::RemoveConstraints() didnt remove internal constraints\n");

  result1 = result2 = result3 = result4 = 0u;

  actor.RemoveConstraints();
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 0u, TEST_LOCATION); // constraint1 didnt applied
  DALI_TEST_EQUALS(result2, 0u, TEST_LOCATION); // constraint2 didnt applied
  DALI_TEST_EQUALS(result3, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(result4, 4u, TEST_LOCATION);

  // Re-apply removed constraints for next tests.
  result1 = result2 = result3 = result4 = 0u;

  constraint1.Apply();
  constraint2.Apply();
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result2, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result3, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(result4, 4u, TEST_LOCATION);

  tet_printf("Test 2 : Integration::HandleRemoveConstraints(tag) to remove internal constraints\n");

  result1 = result2 = result3 = result4 = 0u;

  Dali::Integration::HandleRemoveConstraints(actor, internalConstraint4Tag);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result2, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result3, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(result4, 0u, TEST_LOCATION); // internalConstraint4 didnt applied

  result1 = result2 = result3 = result4 = 0u;

  Dali::Integration::HandleRemoveConstraints(actor, internalConstraint3Tag);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result2, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result3, 0u, TEST_LOCATION); // internalConstraint3 didnt applied
  DALI_TEST_EQUALS(result4, 0u, TEST_LOCATION); // internalConstraint4 didnt applied

  // Re-apply removed constraints for next tests.
  result1 = result2 = result3 = result4 = 0u;

  internalConstraint3.Apply();
  internalConstraint4.Apply();
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result2, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result3, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(result4, 4u, TEST_LOCATION);

  tet_printf("Test 3 : Integration::HandleRemoveConstraints(tagBegin, tagEnd) to remove multiple internal constraints\n");

  result1 = result2 = result3 = result4 = 0u;

  Dali::Integration::HandleRemoveConstraints(actor, internalConstraint3Tag, internalConstraint4Tag + 100u);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result2, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result3, 0u, TEST_LOCATION); // internalConstraint3 didnt applied
  DALI_TEST_EQUALS(result4, 0u, TEST_LOCATION); // internalConstraint4 didnt applied

  // Re-apply removed constraints for next tests.
  result1 = result2 = result3 = result4 = 0u;

  internalConstraint3.Apply();
  internalConstraint4.Apply();
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result2, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result3, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(result4, 4u, TEST_LOCATION);

  tet_printf("Test 4 : Integration::HandleRemoveAllConstraints() to remove every constraints both custom and internal\n");

  result1 = result2 = result3 = result4 = 0u;

  Dali::Integration::HandleRemoveAllConstraints(actor);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 0u, TEST_LOCATION); // constraint1 didnt applied
  DALI_TEST_EQUALS(result2, 0u, TEST_LOCATION); // constraint2 didnt applied
  DALI_TEST_EQUALS(result3, 0u, TEST_LOCATION); // internalConstraint3 didnt applied
  DALI_TEST_EQUALS(result4, 0u, TEST_LOCATION); // internalConstraint4 didnt applied

  END_TEST;
}
