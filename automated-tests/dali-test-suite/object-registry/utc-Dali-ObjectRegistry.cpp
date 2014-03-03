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

static void UtcDaliObjectRegistryGet();
static void UtcDaliObjectRegistrySignalActorCreated();
static void UtcDaliObjectRegistrySignalCameraCreated();
static void UtcDaliObjectRegistrySignalImageActorCreated();
static void UtcDaliObjectRegistrySignalLayerCreated();
static void UtcDaliObjectRegistrySignalLightActorCreated();
static void UtcDaliObjectRegistrySignalMeshActorCreated();
static void UtcDaliObjectRegistrySignalModelCreated();
static void UtcDaliObjectRegistrySignalTextActorCreated();
static void UtcDaliObjectRegistrySignalAnimationCreated();
static void UtcDaliObjectRegistrySignalShaderEffectCreated();



enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliObjectRegistryGet, POSITIVE_TC_IDX },
    { UtcDaliObjectRegistrySignalActorCreated, POSITIVE_TC_IDX },
    { UtcDaliObjectRegistrySignalCameraCreated, POSITIVE_TC_IDX },
    { UtcDaliObjectRegistrySignalImageActorCreated, POSITIVE_TC_IDX },
    { UtcDaliObjectRegistrySignalLayerCreated, POSITIVE_TC_IDX },
    { UtcDaliObjectRegistrySignalLightActorCreated, POSITIVE_TC_IDX },
    { UtcDaliObjectRegistrySignalMeshActorCreated, POSITIVE_TC_IDX },
    { UtcDaliObjectRegistrySignalModelCreated, POSITIVE_TC_IDX },
    { UtcDaliObjectRegistrySignalTextActorCreated, POSITIVE_TC_IDX },
    { UtcDaliObjectRegistrySignalAnimationCreated, POSITIVE_TC_IDX },
    { UtcDaliObjectRegistrySignalShaderEffectCreated, POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

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

struct TestImageActorCallback
{
  TestImageActorCallback(bool& signalReceived)
  : mSignalVerified(signalReceived)
  {
  }
  void operator()(BaseHandle object)
  {
    tet_infoline("Verifying TestImageActorCallback()");
    ImageActor actor = ImageActor::DownCast(object);
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

struct TestLightActorCallback
{
  TestLightActorCallback(bool& signalReceived)
  : mSignalVerified(signalReceived)
  {
  }
  void operator()(BaseHandle object)
  {
    tet_infoline("Verifying TestLightActorCallback()");
    LightActor actor = LightActor::DownCast(object);
    if(actor)
    {
      mSignalVerified = true;
    }
  }
  bool& mSignalVerified;
};

struct TestMeshActorCallback
{
  TestMeshActorCallback(bool& signalReceived)
  : mSignalVerified(signalReceived)
  {
  }
  void operator()(BaseHandle object)
  {
    tet_infoline("Verifying TestMeshActorCallback()");
    MeshActor actor = MeshActor::DownCast(object);
    if(actor)
    {
      mSignalVerified = true;
    }
  }
  bool& mSignalVerified;
};

struct TestModelCallback
{
  TestModelCallback(bool& signalReceived)
  : mSignalVerified(signalReceived)
  {
  }
  void operator()(BaseHandle object)
  {
    tet_infoline("Verifying TestModelCallback()");
    Model actor = Model::DownCast(object);
    if(actor)
    {
      mSignalVerified = true;
    }
  }
  bool& mSignalVerified;
};

struct TestTextActorCallback
{
  TestTextActorCallback(bool& signalReceived)
  : mSignalVerified(signalReceived)
  {
  }
  void operator()(BaseHandle object)
  {
    tet_infoline("Verifying TestTextActorCallback()");
    TextActor actor = TextActor::DownCast(object);
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

struct TestShaderEffectCallback
{
  TestShaderEffectCallback(bool& signalReceived)
  : mSignalVerified(signalReceived)
  {
  }
  void operator()(BaseHandle object)
  {
    tet_infoline("Verifying TestShaderEffectCallback()");
    ShaderEffect actor = ShaderEffect::DownCast(object);
    if(actor)
    {
      mSignalVerified = true;
    }
  }
  bool& mSignalVerified;
};

static void UtcDaliObjectRegistryGet()
{
  TestApplication application;

  ObjectRegistry registry; //  like this for ctor code coverage
  registry= Stage::GetCurrent().GetObjectRegistry();

  DALI_TEST_CHECK( registry );
}



static void UtcDaliObjectRegistrySignalActorCreated()
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
}

void UtcDaliObjectRegistrySignalCameraCreated()
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
}

void UtcDaliObjectRegistrySignalImageActorCreated()
{
  TestApplication application;
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();

  static const char* TestImageFilename = "icon_wrt.png";
  Image image = Image::New(TestImageFilename);

  bool verified = false;
  TestImageActorCallback test(verified);

  Dali::RefObject* objectPointer = NULL;
  TestObjectDestroyedCallback test2(verified, objectPointer);

  registry.ObjectCreatedSignal().Connect(&application, test);
  registry.ObjectDestroyedSignal().Connect(&application, test2);

  {
    ImageActor actor = ImageActor::New(image);
    DALI_TEST_CHECK( test.mSignalVerified );

    verified = false;
    objectPointer = actor.GetObjectPtr();
  }
  DALI_TEST_CHECK( test.mSignalVerified );
}

void UtcDaliObjectRegistrySignalLayerCreated()
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
}

void UtcDaliObjectRegistrySignalLightActorCreated()
{
  TestApplication application;
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();

  bool verified = false;
  TestLightActorCallback test(verified);

  Dali::RefObject* objectPointer = NULL;
  TestObjectDestroyedCallback test2(verified, objectPointer);

  registry.ObjectCreatedSignal().Connect(&application, test);
  registry.ObjectDestroyedSignal().Connect(&application, test2);

  {
    LightActor actor = LightActor::New();
    DALI_TEST_CHECK( test.mSignalVerified );

    verified = false;
    objectPointer = actor.GetObjectPtr();
  }
  DALI_TEST_CHECK( test.mSignalVerified );
}

void UtcDaliObjectRegistrySignalMeshActorCreated()
{
  TestApplication application;
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();

  bool verified = false;
  TestMeshActorCallback test(verified);

  Dali::RefObject* objectPointer = NULL;
  TestObjectDestroyedCallback test2(verified, objectPointer);

  registry.ObjectCreatedSignal().Connect(&application, test);
  registry.ObjectDestroyedSignal().Connect(&application, test2);

  Mesh mesh = ConstructMesh(60);

  {
    MeshActor actor = MeshActor::New(mesh);

    DALI_TEST_CHECK(actor);
    DALI_TEST_CHECK( test.mSignalVerified );

    verified = false;
    objectPointer = actor.GetObjectPtr();
  }
  DALI_TEST_CHECK( test.mSignalVerified );
}

void UtcDaliObjectRegistrySignalModelCreated()
{
  TestApplication application;
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();

  bool verified = false;
  TestModelCallback test(verified);

  Dali::RefObject* objectPointer = NULL;
  TestObjectDestroyedCallback test2(verified, objectPointer);

  registry.ObjectCreatedSignal().Connect(&application, test);
  registry.ObjectDestroyedSignal().Connect(&application, test2);

  {
    Model model = Model::New("blah");
    DALI_TEST_CHECK( test.mSignalVerified );

    verified = false;
    objectPointer = model.GetObjectPtr();
  }
  DALI_TEST_CHECK( test.mSignalVerified );
}

void UtcDaliObjectRegistrySignalTextActorCreated()
{
  TestApplication application;
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();

  bool verified = false;
  TestTextActorCallback test(verified);

  Dali::RefObject* objectPointer = NULL;
  TestObjectDestroyedCallback test2(verified, objectPointer);

  registry.ObjectCreatedSignal().Connect(&application, test);
  registry.ObjectDestroyedSignal().Connect(&application, test2);

  {
    TextActor actor = TextActor::New("Hello");
    DALI_TEST_CHECK( test.mSignalVerified );

    verified = false;
    objectPointer = actor.GetObjectPtr();
  }
  DALI_TEST_CHECK( test.mSignalVerified );
}

void UtcDaliObjectRegistrySignalAnimationCreated()
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
}

void UtcDaliObjectRegistrySignalShaderEffectCreated()
{
  TestApplication application;
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();

  static const char* VertexSource =
  "void main()\n"
  "{\n"
  "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
  "  vTexCoord = aTexCoord;\n"
  "}\n";

  static const char* FragmentSource =
  "void main()\n"
  "{\n"
  "  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n"
  "}\n";

  bool verified = false;
  TestShaderEffectCallback test(verified);

  Dali::RefObject* objectPointer = NULL;
  TestObjectDestroyedCallback test2(verified, objectPointer);

  registry.ObjectCreatedSignal().Connect(&application, test);
  registry.ObjectDestroyedSignal().Connect(&application, test2);

  {
    ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
    DALI_TEST_CHECK( test.mSignalVerified );

    verified = false;
    objectPointer = effect.GetObjectPtr();
  }
  DALI_TEST_CHECK( test.mSignalVerified );
}
