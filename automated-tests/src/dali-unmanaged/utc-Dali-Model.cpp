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
#include "mesh-builder.h"

namespace
{

static std::string      gModelFile = "blah";


// Functor to test whether a Finish signal is emitted
struct AnimationFinishCheck
{
  AnimationFinishCheck(bool& signalReceived)
  : mSignalReceived(signalReceived)
  {
  }

  void operator()(Animation& animation)
  {
    mSignalReceived = true;
  }

  void Reset()
  {
    mSignalReceived = false;
  }

  void CheckSignalReceived()
  {
    if (!mSignalReceived)
    {
      tet_printf("Expected Finish signal was not received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  void CheckSignalNotReceived()
  {
    if (mSignalReceived)
    {
      tet_printf("Unexpected Finish signal was received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  bool& mSignalReceived; // owned by individual tests
};

}


int UtcDaliModelConstructorVoid(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Model::Model()");

  Model model;

  DALI_TEST_CHECK(!model);
  END_TEST;
}

int UtcDaliModelNew(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::Model::New()");

  Model model = Model::New(gModelFile);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc));
  DALI_TEST_CHECK(model);
  END_TEST;
}

int UtcDaliModelDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Model::DownCast()");

  Model model = Model::New(gModelFile);

  BaseHandle object(model);

  Model model2 = Model::DownCast(object);
  DALI_TEST_CHECK(model2);

  Model model3 = DownCast< Model >(object);
  DALI_TEST_CHECK(model3);

  BaseHandle unInitializedObject;
  Model model4 = Model::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!model4);

  Model model5 = DownCast< Model >(unInitializedObject);
  DALI_TEST_CHECK(!model5);
  END_TEST;
}


int UtcDaliModelGetLoadingState01(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::Model::New()");
  Model model = Model::New(gModelFile);
  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest();
  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoading);
  DALI_TEST_EQUALS(request->GetPath(), gModelFile, TEST_LOCATION);
  DALI_TEST_EQUALS(request->GetType()->id, Integration::ResourceModel, TEST_LOCATION);
  END_TEST;
}

int UtcDaliModelGetLoadingState02(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::Model::New()");
  Dali::ModelData modelData = BuildTreeModel();
  Model model = Model::New(gModelFile);
  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest();
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }

  // Get resources into resource cache
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  END_TEST;
}

int UtcDaliModelGetLoadingState03(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::Model::New()");
  Model model = Model::New(gModelFile);
  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest();
  if(request)
  {
    platform.SetResourceLoadFailed(request->GetId(), Integration::FailureFileNotFound);
  }
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingFailed);
  END_TEST;
}


// Could probably test the signals, too!

int UtcDaliModelNumberOfAnimations(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Model::NumberOfAnimations()");

  int animationCount = -1;
  Model model = Model::New("Blah"); // Probably need to create a real model...
  animationCount = model.NumberOfAnimations();

  DALI_TEST_CHECK( -1 != animationCount );
  END_TEST;
}

int UtcDaliModelActorFactory(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::ModelActorFactory");

  Dali::ModelData modelData = BuildTreeModel();

  // Raise a request
  Model model = Model::New("Tree");

  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();
  application.SendNotification();

  Actor modelRootActor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  DALI_TEST_CHECK(modelRootActor);
  DALI_TEST_CHECK(modelRootActor.GetName().compare("root") == 0);

  Actor trunk = modelRootActor.FindChildByName("trunk");
  DALI_TEST_CHECK(trunk);
  Actor branch = modelRootActor.FindChildByName("branch");
  DALI_TEST_CHECK(branch);
  Actor twig = modelRootActor.FindChildByName("twig");
  DALI_TEST_CHECK(twig);
  MeshActor twigMeshActor = MeshActor::DownCast(twig);
  DALI_TEST_CHECK(!twigMeshActor);
  Actor leaf = modelRootActor.FindChildByName("leaf");
  DALI_TEST_CHECK(leaf);
  MeshActor leafMeshActor = MeshActor::DownCast(leaf);
  DALI_TEST_CHECK(leafMeshActor);

  Material leafMaterial = leafMeshActor.GetMaterial();
  DALI_TEST_CHECK(leafMaterial);
  END_TEST;
}

int UtcDaliModelActorFactoryTwoMesh(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::ModelActorFactory with 2 meshes in an entity");

  Dali::ModelData modelData = BuildTreeModel();
  Dali::Entity twoMeshEntity = Dali::Entity::New("2Mesh");
  MeshData meshData;
  CreateMeshData(meshData);
  MeshData meshData2;
  CreateMeshData(meshData2);
  unsigned int meshIndex = modelData.NumberOfMeshes();
  modelData.AddMesh(meshData);
  modelData.AddMesh(meshData2);
  twoMeshEntity.AddMeshIndex(meshIndex);
  twoMeshEntity.AddMeshIndex(meshIndex+1);
  modelData.GetRootEntity().Add(twoMeshEntity);

  // Raise a request
  Model model = Model::New("Tree");

  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();
  application.SendNotification();

  Actor actor;
  try
  {
    actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded
  }
  catch( Dali::DaliException& e )
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "entity.NumberOfMeshes() == 1", TEST_LOCATION);
    DALI_TEST_CHECK( !actor );
  }
  END_TEST;
}

int UtcDaliModelBuildAnimation01(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::MeshActor::New()");

  Dali::ModelData modelData = BuildTreeModel();

  // Raise a request
  Model model = Model::New("Tree");

  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();
  application.SendNotification();

  Actor actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded
  Stage::GetCurrent().Add(actor);

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  DALI_TEST_CHECK(actor);
  DALI_TEST_CHECK(actor.GetName().compare("root") == 0);

  DALI_TEST_EQUALS(model.NumberOfAnimations(), static_cast<size_t>(1), TEST_LOCATION);
  unsigned int animIndex=0;
  bool found = model.FindAnimation("Anim1", animIndex);
  DALI_TEST_CHECK(found);

  Animation twigAnim = ModelActorFactory::BuildAnimation(model, actor, animIndex);
  DALI_TEST_CHECK(twigAnim);
  DALI_TEST_EQUALS(twigAnim.GetDuration(), 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_CHECK(twigAnim.GetDefaultAlphaFunction() == Dali::AlphaFunctions::Linear);

  Actor twigActor = actor.FindChildByName("twig");
  DALI_TEST_CHECK(twigActor);

  // Start the animation
  twigAnim.Play();

  float durationSeconds = 10.0f;

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  twigAnim.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( twigActor.GetCurrentPosition(), Vector3(2.0f, 1.0f, 0.0f), 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( twigActor.GetCurrentPosition(), Vector3(2.5f, 1.0f, 2.5f), 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( twigActor.GetCurrentPosition(), Vector3(3.5f, 1.0f, 7.5f), 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* Past Finished */);
  application.SendNotification();
  DALI_TEST_EQUALS( twigActor.GetCurrentPosition(), Vector3(4.0f, 1.0f, 10.0f), 0.01f, TEST_LOCATION );

  finishCheck.CheckSignalReceived();
  END_TEST;
}



int UtcDaliModelBuildAnimation02(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::MeshActor::New()");

  Dali::ModelData modelData = BuildTreeModel();

  // Raise a request
  Model model = Model::New("Tree");

  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();
  application.SendNotification();

  Actor actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  DALI_TEST_CHECK(actor);
  DALI_TEST_CHECK(actor.GetName().compare("root") == 0);

  DALI_TEST_EQUALS(model.NumberOfAnimations(), static_cast<size_t>(1), TEST_LOCATION);
  unsigned int animIndex=0;
  bool found = model.FindAnimation("Anim1", animIndex);
  DALI_TEST_CHECK(found);
  Animation twigAnim = ModelActorFactory::BuildAnimation(model, actor, animIndex, 5.0f);
  DALI_TEST_CHECK(twigAnim);
  DALI_TEST_EQUALS(twigAnim.GetDuration(), 5.0f, 0.001, TEST_LOCATION);
  DALI_TEST_CHECK(twigAnim.GetDefaultAlphaFunction() == Dali::AlphaFunctions::Linear);
  END_TEST;
}


int UtcDaliModelBuildAnimation03(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::MeshActor::New()");

  Dali::ModelData modelData = BuildTreeModel();

  // Raise a request
  Model model = Model::New("Tree");

  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();
  application.SendNotification();

  Actor actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  DALI_TEST_CHECK(actor);
  DALI_TEST_CHECK(actor.GetName().compare("root") == 0);

  DALI_TEST_EQUALS(model.NumberOfAnimations(), static_cast<size_t>(1), TEST_LOCATION);
  unsigned int animIndex=0;
  bool found = model.FindAnimation("Anim1", animIndex);
  DALI_TEST_CHECK(found);

  Animation twigAnim = ModelActorFactory::BuildAnimation(model, actor, animIndex, Dali::AlphaFunctions::EaseInOut, 5.0f);
  DALI_TEST_CHECK(twigAnim);
  DALI_TEST_EQUALS(twigAnim.GetDuration(), 5.0f, 0.001, TEST_LOCATION);
  DALI_TEST_CHECK(twigAnim.GetDefaultAlphaFunction() == Dali::AlphaFunctions::EaseInOut);
  END_TEST;
}


int UtcDaliModelBuildAnimation04(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::MeshActor::New()");

  Dali::ModelData modelData = BuildTreeModel();

  // Raise a request
  Model model = Model::New("Tree");

  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();
  application.SendNotification();

  Actor actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  DALI_TEST_CHECK(actor);
  DALI_TEST_CHECK(actor.GetName().compare("root") == 0);

  DALI_TEST_EQUALS(model.NumberOfAnimations(), static_cast<size_t>(1), TEST_LOCATION);

  unsigned int animIndex=0;
  bool found = model.FindAnimation("Fred", animIndex);
  DALI_TEST_CHECK(!found);
  END_TEST;
}


int UtcDaliModelBuildAnimation05(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::MeshActor::New()");

  Dali::ModelData modelData = BuildTreeModel();

  // Raise a request
  Model model = Model::New("Tree");

  // Run Core - will query using TestPlatformAbstraction::GetResources().
  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();
  application.SendNotification();

  Actor actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  DALI_TEST_CHECK(actor);
  DALI_TEST_CHECK(actor.GetName().compare("root") == 0);

  DALI_TEST_EQUALS(model.NumberOfAnimations(), static_cast<size_t>(1), TEST_LOCATION);
  Animation twigAnim = ModelActorFactory::BuildAnimation(model, actor, 0);
  DALI_TEST_CHECK(twigAnim);
  DALI_TEST_EQUALS(twigAnim.GetDuration(), 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_CHECK(twigAnim.GetDefaultAlphaFunction() == Dali::AlphaFunctions::Linear);
  END_TEST;
}


int UtcDaliModelBuildAnimation06(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::MeshActor::New()");

  Dali::ModelData modelData = BuildTreeModel();

  // Raise a request
  Model model = Model::New("Tree");

  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();
  application.SendNotification();

  Actor actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  DALI_TEST_CHECK(actor);
  DALI_TEST_CHECK(actor.GetName().compare("root") == 0);

  DALI_TEST_EQUALS(model.NumberOfAnimations(), static_cast<size_t>(1), TEST_LOCATION);
  Animation twigAnim = ModelActorFactory::BuildAnimation(model, actor, 0, 5.0f);
  DALI_TEST_CHECK(twigAnim);
  DALI_TEST_EQUALS(twigAnim.GetDuration(), 5.0f, 0.001, TEST_LOCATION);
  DALI_TEST_CHECK(twigAnim.GetDefaultAlphaFunction() == Dali::AlphaFunctions::Linear);
  END_TEST;
}


int UtcDaliModelBuildAnimation07(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::MeshActor::New()");

  Dali::ModelData modelData = BuildTreeModel();

  // Raise a request
  Model model = Model::New("Tree");

  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();
  application.SendNotification();

  Actor actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  DALI_TEST_CHECK(actor);
  DALI_TEST_CHECK(actor.GetName().compare("root") == 0);

  DALI_TEST_EQUALS(model.NumberOfAnimations(), static_cast<size_t>(1), TEST_LOCATION);
  Animation twigAnim = ModelActorFactory::BuildAnimation(model, actor, 0, Dali::AlphaFunctions::Bounce);
  DALI_TEST_CHECK(twigAnim);
  DALI_TEST_EQUALS(twigAnim.GetDuration(), 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_CHECK(twigAnim.GetDefaultAlphaFunction() == Dali::AlphaFunctions::Bounce);
  END_TEST;
}

int UtcDaliModelBuildAnimation08(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::MeshActor::New()");

  Dali::ModelData modelData = BuildTreeModel();

  // Raise a request
  Model model = Model::New("Tree");

  application.SendNotification();
  application.Render();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();
  application.SendNotification();

  Actor actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  DALI_TEST_CHECK(actor);
  DALI_TEST_CHECK(actor.GetName().compare("root") == 0);

  DALI_TEST_EQUALS(model.NumberOfAnimations(), static_cast<size_t>(1), TEST_LOCATION);

  Animation twigAnim = ModelActorFactory::BuildAnimation(model, actor, 10);
  DALI_TEST_CHECK(!twigAnim);
  END_TEST;
}
