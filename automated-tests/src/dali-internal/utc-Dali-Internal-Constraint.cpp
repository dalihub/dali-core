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
#include <dali/internal/update/animation/scene-graph-constraint-base.h>

using namespace Dali;

using Dali::Internal::SceneGraph::ConstraintBase;

int UtcDaliConstraintNewInput1OffStage(void)
{
  /**
   * Test that the Constraint is correctly added/removed when an object
   * providing the input property is added/removed from the stage
   */
  TestApplication application;

  Actor parent = Actor::New();
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  parent.Add( actor );

  Actor sibling1 = Actor::New();
  sibling1.SetPosition( Vector3(1.0f, 2.0f, 3.0f) );
  parent.Add( sibling1 );

  Vector3 startValue( 0.0f, 0.0f, 0.0f );
  DALI_TEST_EQUALS( 0u, ConstraintBase::GetCurrentInstanceCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 0u, ConstraintBase::GetTotalInstanceCount(),   TEST_LOCATION );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::Property::Position ), startValue, TEST_LOCATION );

  // Apply constraint with a parent input property

  Constraint constraint = Constraint::New<Vector3>( Actor::Property::Position,
                                                    Source( sibling1, Actor::Property::Position ),
                                                    EqualToConstraint() );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::Property::Position ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::Property::Position ), Vector3(1.0f, 2.0f, 3.0f)/*from sibling1*/, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, ConstraintBase::GetCurrentInstanceCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, ConstraintBase::GetTotalInstanceCount(),   TEST_LOCATION );

  // Remove sibling1 providing the input property

  parent.Remove( sibling1 );
  actor.SetPosition( Vector3(2.0f, 2.0f, 2.0f) ); // This should be effective

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::Property::Position ), Vector3(2.0f, 2.0f, 2.0f)/*from SetPosition*/, TEST_LOCATION );
  DALI_TEST_EQUALS( 0u/*should have been removed*/, ConstraintBase::GetCurrentInstanceCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, ConstraintBase::GetTotalInstanceCount(), TEST_LOCATION );

  // Add sibling1 back again (re-enables constraint)

  parent.Add( sibling1 );
  actor.SetPosition( Vector3(3.0f, 3.0f, 3.0f) ); // This should NOT be effective

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::Property::Position ), Vector3(1.0f, 2.0f, 3.0f)/*from sibling1*/, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, ConstraintBase::GetCurrentInstanceCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 2u/*recreated once*/, ConstraintBase::GetTotalInstanceCount(), TEST_LOCATION );

  END_TEST;
}

