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
#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>

using namespace Dali;

namespace
{

// Functors to test whether Object created/destroyed signal is emitted for different types of Objects

struct TestObjectDestroyedCallback
{
  TestObjectDestroyedCallback(bool& signalReceived, Dali::RefObject*& objectPointer)
  : mSignalVerified(signalReceived),
    mObjectPointer(objectPointer)
  {
  }

  void operator()(const Dali::RefObject* objectPointer)
  {
    tet_infoline("Verifying TestObjectDestroyedCallback()");

    if(objectPointer == mObjectPointer)
    {
      mSignalVerified = true;
    }
  }

  bool& mSignalVerified;
  Dali::RefObject*& mObjectPointer;
};

struct TestActorCallback
{
  TestActorCallback(bool& signalReceived)
  : mSignalVerified(signalReceived)
  {
  }

  void operator()(BaseHandle object)
  {
    tet_infoline("Verifying TestActorCallback()");
    Actor actor = Actor::DownCast(object);
    if(actor)
    {
      mSignalVerified = true;
    }
  }

  bool& mSignalVerified;
};

struct TestCameraActorCallback
{
  TestCameraActorCallback(bool& signalReceived)
  : mSignalVerified(signalReceived)
  {
  }
  void operator()(BaseHandle object)
  {
    tet_infoline("Verifying TestCameraActorCallback()");
    CameraActor actor = CameraActor::DownCast(object);
    if(actor)
    {
      mSignalVerified = true;
    }
  }
  bool& mSignalVerified;
};

struct TestLayerCallback
{
  TestLayerCallback(bool& signalReceived)
  : mSignalVerified(signalReceived)
  {
  }
  void operator()(BaseHandle object)
  {
    tet_infoline("Verifying TestLayerCallback()");
    Layer actor = Layer::DownCast(object);
    if(actor)
    {
      mSignalVerified = true;
    }
  }
  bool& mSignalVerified;
};

struct TestAnimationCallback
{
  TestAnimationCallback(bool& signalReceived)
  : mSignalVerified(signalReceived)
  {
  }
  void operator()(BaseHandle object)
  {
    tet_infoline("Verifying TestAnimationCallback()");
    Animation actor = Animation::DownCast(object);
    if(actor)
    {
      mSignalVerified = true;
    }
  }
  bool& mSignalVerified;
};

} // anonymous namespace

int UtcDaliObjectRegistryGet(void)
{
  TestApplication application;

  ObjectRegistry registry; //  like this for ctor code coverage
  registry= Stage::GetCurrent().GetObjectRegistry();

  DALI_TEST_CHECK( registry );
  END_TEST;
}

int UtcDaliObjectRegistryCopyConstructor(void)
{
  TestApplication application;

  ObjectRegistry myRegistry;
  ObjectRegistry anotherRegistry( myRegistry );

  DALI_TEST_EQUALS( myRegistry, anotherRegistry, TEST_LOCATION );
  END_TEST;
}

int UtcDaliObjectRegistrySignalActorCreated(void)
{
  tet_infoline("Testing GetObjectRegistry()");
  TestApplication application;
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  bool verified = false;
  TestActorCallback test(verified);

  Dali::RefObject* objectPointer = NULL;
  TestObjectDestroyedCallback test2(verified, objectPointer);

  registry.ObjectCreatedSignal().Connect(&application, test);
  registry.ObjectDestroyedSignal().Connect(&application, test2);

  {
    Actor actor = Actor::New();
    DALI_TEST_CHECK( test.mSignalVerified );

    verified = false;
    objectPointer = actor.GetObjectPtr();
  }
  DALI_TEST_CHECK( test.mSignalVerified );
  END_TEST;
}

int UtcDaliObjectRegistrySignalCameraCreated(void)
{
  TestApplication application;

  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();

  bool verified = false;
  TestCameraActorCallback test(verified);

  Dali::RefObject* objectPointer = NULL;
  TestObjectDestroyedCallback test2(verified, objectPointer);

  registry.ObjectCreatedSignal().Connect(&application, test);
  registry.ObjectDestroyedSignal().Connect(&application, test2);

  {
    CameraActor actor = CameraActor::New();
    DALI_TEST_CHECK( test.mSignalVerified );

    verified = false;
    objectPointer = actor.GetObjectPtr();
  }
  DALI_TEST_CHECK( test.mSignalVerified );
  END_TEST;
}

int UtcDaliObjectRegistrySignalLayerCreated(void)
{
  TestApplication application;
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();

  bool verified = false;
  TestLayerCallback test(verified);

  Dali::RefObject* objectPointer = NULL;
  TestObjectDestroyedCallback test2(verified, objectPointer);

  registry.ObjectCreatedSignal().Connect(&application, test);
  registry.ObjectDestroyedSignal().Connect(&application, test2);

  {
    Layer actor = Layer::New();
    DALI_TEST_CHECK( test.mSignalVerified );

    verified = false;
    objectPointer = actor.GetObjectPtr();
  }
  DALI_TEST_CHECK( test.mSignalVerified );
  END_TEST;
}

int UtcDaliObjectRegistrySignalAnimationCreated(void)
{
  TestApplication application;
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();

  bool verified = false;
  TestAnimationCallback test(verified);

  Dali::RefObject* objectPointer = NULL;
  TestObjectDestroyedCallback test2(verified, objectPointer);

  registry.ObjectCreatedSignal().Connect(&application, test);
  registry.ObjectDestroyedSignal().Connect(&application, test2);

  {
    Animation animation = Animation::New(1.0f);
    DALI_TEST_CHECK( test.mSignalVerified );

    verified = false;
    objectPointer = animation.GetObjectPtr();
  }
  DALI_TEST_CHECK( test.mSignalVerified );
  END_TEST;
}
