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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;
#include <mesh-builder.h>

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

TEST_FUNCTION( UtcDaliModelConstructorVoid,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelConstructorRefObject,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelNew,                       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelDownCast,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelGetLoadingState01,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelGetLoadingState02,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelGetLoadingState03,         NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelActorFactory,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelActorFactoryTwoMesh,       NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelNumberOfAnimations,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelBuildAnimation01,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelBuildAnimation02,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelBuildAnimation03,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelBuildAnimation04,          NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelBuildAnimation05,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelBuildAnimation06,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelBuildAnimation07,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliModelBuildAnimation08,          NEGATIVE_TC_IDX );

static std::string      gModelFile = "blah";

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

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



static void UtcDaliModelConstructorVoid()
{
  TestApplication application;
  tet_infoline("Testing Dali::Model::Model()");

  Model model;

  DALI_TEST_CHECK(!model);
}

static void UtcDaliModelConstructorRefObject()
{
  TestApplication application;
  tet_infoline("Testing Dali::Model::Model(Internal::Model*)");

  Model model(NULL);

  DALI_TEST_CHECK(!model);
}

static void UtcDaliModelNew()
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
}

static void UtcDaliModelDownCast()
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
}


static void UtcDaliModelGetLoadingState01()
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
}

static void UtcDaliModelGetLoadingState02()
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
}

static void UtcDaliModelGetLoadingState03()
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
}


// Could probably test the signals, too!

static void UtcDaliModelNumberOfAnimations()
{
  TestApplication application;
  tet_infoline("Testing Dali::Model::NumberOfAnimations()");

  int animationCount = -1;
  Model model = Model::New("Blah"); // Probably need to create a real model...
  animationCount = model.NumberOfAnimations();

  DALI_TEST_CHECK( -1 != animationCount );
}

static void UtcDaliModelActorFactory()
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
}

static void UtcDaliModelActorFactoryTwoMesh()
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
}

static void UtcDaliModelBuildAnimation01()
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
}



static void UtcDaliModelBuildAnimation02()
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
}


static void UtcDaliModelBuildAnimation03()
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
}


static void UtcDaliModelBuildAnimation04()
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
}


static void UtcDaliModelBuildAnimation05()
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
}


static void UtcDaliModelBuildAnimation06()
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
}


static void UtcDaliModelBuildAnimation07()
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
}

static void UtcDaliModelBuildAnimation08()
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
}
