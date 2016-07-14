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
#include <algorithm>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_gesture_detector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gesture_detector_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliGestureDetectorConstructorN(void)
{
  TestApplication application;

  GestureDetector detector;

  Actor actor = Actor::New();

  try
  {
    detector.Attach(actor);
    tet_result(TET_FAIL);
  }
  catch (DaliException& e)
  {
    DALI_TEST_ASSERT( e, "detector", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliGestureDetectorConstructorP(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();

  try
  {
    detector.Attach(actor);
    tet_result(TET_PASS);
  }
  catch (DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorAssignP(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();
  GestureDetector detector2;

  detector2 = detector;

  Actor actor = Actor::New();

  try
  {
    detector2.Attach(actor);
    tet_result(TET_PASS);
  }
  catch (DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorDownCastP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::GestureDetector::DownCast()");

  GestureDetector detector = PanGestureDetector::New();

  BaseHandle object(detector);

  GestureDetector detector2 = GestureDetector::DownCast(object);
  DALI_TEST_CHECK(detector2);

  GestureDetector detector3 = DownCast< GestureDetector >(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle unInitializedObject;
  GestureDetector detector4 = GestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  GestureDetector detector5 = DownCast< GestureDetector >(unInitializedObject);
  DALI_TEST_CHECK(!detector5);
  END_TEST;
}

int UtcDaliGestureDetectorAttachP(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();

  detector.Attach(actor);

  bool found = false;
  for(size_t i = 0; i < detector.GetAttachedActorCount(); i++)
  {
    if( detector.GetAttachedActor(i) == actor )
    {
      tet_result(TET_PASS);
      found = true;
    }
  }

  if(!found)
  {
    tet_result(TET_FAIL);
  }

  // Scoped gesture detector. GestureDetectors connect to a destroy signal of the actor. If the
  // actor is still alive when the gesture detector is destroyed, then it should disconnect from
  // this signal.  If it does not, then when this function ends, there will be a segmentation fault
  // thus, a TET case failure.
  {
    GestureDetector detector2 = PanGestureDetector::New();
    detector2.Attach(actor);
  }
  END_TEST;
}

int UtcDaliGestureDetectorAttachN(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  // Do not initialise actor
  Actor actor;

  try
  {
    detector.Attach(actor);
    tet_result(TET_FAIL);
  }
  catch (DaliException& e)
  {
    DALI_TEST_ASSERT( e, "actor", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachP(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();
  detector.Attach(actor);

  bool found = false;
  for(size_t i = 0; i < detector.GetAttachedActorCount(); i++)
  {
    if( detector.GetAttachedActor(i) == actor )
    {
      tet_result(TET_PASS);
      found = true;
    }
  }

  if(!found)
  {
    tet_result(TET_FAIL);
  }

  // Detach and retrieve attached actors again, the vector should be empty.
  detector.Detach(actor);

  found = false;
  for(size_t i = 0; i < detector.GetAttachedActorCount(); i++)
  {
    if( detector.GetAttachedActor(i) == actor )
    {
      found = true;
    }
  }

  if(found)
  {
    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }

  END_TEST;
}

int UtcDaliGestureDetectorDetachN01(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  // Do not initialise actor
  Actor actor;

  try
  {
    detector.Detach(actor);
    tet_result(TET_FAIL);
  }
  catch (DaliException& e)
  {
    DALI_TEST_ASSERT( e, "actor", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachN02(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();
  detector.Attach(actor);

  // Detach an actor that hasn't been attached.  This should not cause an exception, it's a no-op.
  try
  {
    Actor actor2 = Actor::New();
    detector.Detach(actor2);
    tet_result(TET_PASS);
  }
  catch (DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachN03(void)
{
  TestApplication application;
  TestGestureManager& gestureManager = application.GetGestureManager();

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();
  detector.Attach(actor);

  // Detach an actor twice - no exception should happen.
  try
  {
    detector.Detach(actor);
    DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

    gestureManager.Initialize(); // Reset values
    detector.Detach(actor);
    DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
  }
  catch (DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachAllP(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  const unsigned int actorsToAdd = 5;
  std::vector<Actor> myActors;

  for (unsigned int i = 0; i < actorsToAdd; ++i)
  {
    Actor actor = Actor::New();
    myActors.push_back(actor);
    detector.Attach(actor);
  }

  DALI_TEST_EQUALS(actorsToAdd, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach and retrieve attached actors again, the vector should be empty.
  detector.DetachAll();

  DALI_TEST_EQUALS(0u, detector.GetAttachedActorCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGestureDetectorDetachAllN(void)
{
  TestApplication application;
  TestGestureManager& gestureManager = application.GetGestureManager();

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  const unsigned int actorsToAdd = 5;
  std::vector<Actor> myActors;

  for (unsigned int i = 0; i < actorsToAdd; ++i)
  {
    Actor actor = Actor::New();
    myActors.push_back(actor);
    detector.Attach(actor);
  }

  DALI_TEST_EQUALS(actorsToAdd, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach and retrieve attached actors again, the vector should be empty.
  detector.DetachAll();

  DALI_TEST_EQUALS(0u, detector.GetAttachedActorCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

  // Call DetachAll again, there should not be any exception
  try
  {
    gestureManager.Initialize(); // Reset values
    detector.DetachAll();
    DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
  }
  catch (DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorGetAttachedActors(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  // Initially there should not be any actors.
  DALI_TEST_EQUALS(0u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Attach one actor
  Actor actor1 = Actor::New();
  detector.Attach(actor1);
  DALI_TEST_EQUALS(1u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Attach another actor
  Actor actor2 = Actor::New();
  detector.Attach(actor2);
  DALI_TEST_EQUALS(2u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Attach another five actors
  std::vector<Actor> myActors;
  for (unsigned int i = 0; i < 5; ++i)
  {
    Actor actor = Actor::New();
    myActors.push_back(actor);
    detector.Attach(actor);
  }
  DALI_TEST_EQUALS(7u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach actor2
  detector.Detach(actor2);
  DALI_TEST_EQUALS(6u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Attach actor1 again, count should not increase.
  detector.Attach(actor1);
  DALI_TEST_EQUALS(6u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach actor2 again, count should not decrease.
  detector.Detach(actor2);
  DALI_TEST_EQUALS(6u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach actor1.
  detector.Detach(actor1);
  DALI_TEST_EQUALS(5u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Create scoped actor, actor should be automatically removed from the detector when it goes out
  // of scope.
  {
    Actor scopedActor = Actor::New();
    detector.Attach(scopedActor);
    DALI_TEST_EQUALS(6u, detector.GetAttachedActorCount(), TEST_LOCATION);
  }
  DALI_TEST_EQUALS(5u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach all so nothing remains.
  detector.DetachAll();
  DALI_TEST_EQUALS(0u, detector.GetAttachedActorCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGestureDetectorProperties(void)
{
  TestApplication application;

  // Use pinch gesture as that doen't currently have any properties. Will need to change it if default properties are added.
  GestureDetector detector = PinchGestureDetector::New();

  DALI_TEST_EQUALS( detector.GetPropertyCount(), 0u, TEST_LOCATION );

  Property::IndexContainer indices;
  detector.GetPropertyIndices( indices );
  DALI_TEST_EQUALS( indices.Size(), 0u, TEST_LOCATION );

  DALI_TEST_EQUALS( detector.IsPropertyWritable( DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( detector.IsPropertyAnimatable( DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( detector.IsPropertyAConstraintInput( DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( detector.GetPropertyType( DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX ), Property::NONE, TEST_LOCATION );
  DALI_TEST_EQUALS( detector.GetPropertyIndex( "InvalidIndex" ), Property::INVALID_INDEX, TEST_LOCATION );

  Property::Value propValue = detector.GetProperty( DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX );
  DALI_TEST_EQUALS( propValue.GetType(), Property::NONE, TEST_LOCATION );

  DALI_TEST_CHECK( detector.GetPropertyName( DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX ).empty() );

  // For coverage only, not testable
  detector.SetProperty( DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX, true );

  END_TEST;
}
