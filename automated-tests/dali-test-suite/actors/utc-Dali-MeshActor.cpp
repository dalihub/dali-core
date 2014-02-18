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
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/update/nodes/node.h>

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

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliMeshActorConstructorVoid,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorConstructorRefObject,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorNew01,                     NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorNew02,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorNew03,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorDownCast,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorDownCast2,                 NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorCreateNoMeshData,          NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorCreateSetData01,           NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorCreateSetData02,           NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorCreateSetData03,           NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorCreateSetData04,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorSetMaterial01,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorSetMaterial01b,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorSetMaterial02,             NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorSetMaterial02b,             NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorSetMaterial03,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorSetMaterial03b,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorGetMaterial01,             NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorGetMaterial02,             NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorSetLighting01,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorSetLighting02,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorBlend01,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorBlend02,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorBlend03,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorBlend04,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorBlend05,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorBlend06,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorBlend07,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorBlend08,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorBlend09,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorBoneUpdate01,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMeshActorIndices,                   POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


static Mesh NewMesh()
{
  MeshData meshData;
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 60);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);
  Mesh mesh = Mesh::New(meshData);
  return mesh;
}

static void UtcDaliMeshActorConstructorVoid()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::MeshActor()");

  MeshActor actor;
  DALI_TEST_CHECK(!actor);
}

static void UtcDaliMeshActorConstructorRefObject()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::MeshActor(Internal::MeshActor*)");

  MeshActor actor(NULL);
  DALI_TEST_CHECK(!actor);

  MeshActor* actorPtr = new MeshActor();
  DALI_TEST_CHECK( ! *actorPtr );
  delete actorPtr;
}

static void UtcDaliMeshActorNew01()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::New()");

  MeshData meshData;
  MeshData::VertexContainer vertices;
  MeshData::FaceIndices faces;
  BoneContainer bones;
  ConstructVertices(vertices, 60);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);
  Mesh mesh = Mesh::New(meshData);

  MeshActor actor = MeshActor::New(mesh);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(actor);
}

static void UtcDaliMeshActorNew02()
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("Testing Dali::MeshActor::New()");

  std::string modelName("AModel");
  Dali::ModelData modelData = Dali::ModelData::New(modelName);
  Dali::Entity rootEntity = Dali::Entity::New("root");
  modelData.SetRootEntity(rootEntity);
  rootEntity.SetType(Dali::Entity::OBJECT);

  Model model = Model::New("Fake model");

  // Run Core - will query using TestPlatformAbstraction::GetResources().
  application.SendNotification();
  application.Render();

  Integration::ResourceRequest* request = platform.GetRequest();
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }

  application.Render();
  application.SendNotification();

  Actor actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded

  DALI_TEST_CHECK(model.GetLoadingState() == ResourceLoadingSucceeded);
  DALI_TEST_CHECK(actor);
  DALI_TEST_CHECK(actor.GetName().compare("root") == 0)
}


static void UtcDaliMeshActorNew03()
{
  TestApplication application;
  tet_infoline("Testing Dali::Mesh::New() - Create with no mesh");

  MeshActor actor = MeshActor::New(); // Shouldn't assert
  tet_result(TET_PASS);
}

static void UtcDaliMeshActorCreateNoMeshData()
{
  TestApplication application;
  tet_infoline("Testing Dali::Mesh::New() - Create with no mesh data");

  try
  {
    MeshData meshData;
    Mesh mesh = Mesh::New(meshData);
    MeshActor actor1 = MeshActor::New(mesh);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "object", TEST_LOCATION);
  }
}


static void UtcDaliMeshActorCreateSetData01()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshData::SetData() - Create with no verts");
  try
  {
    MeshData meshData;
    MeshData::VertexContainer vertices;
    MeshData::FaceIndices     faces;
    BoneContainer             bones;
    Material                  customMaterial;
    meshData.SetData(vertices, faces, bones, customMaterial);
    Mesh mesh = Mesh::New(meshData);
    MeshActor actor1 = MeshActor::New(mesh);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "!vertices.empty()", TEST_LOCATION );
  }
}

static void UtcDaliMeshActorCreateSetData02()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshData::SetData - Create with no faces");
  try
  {
    MeshData meshData;
    MeshData::VertexContainer    vertices;
    MeshData::FaceIndices        faces;
    BoneContainer                bones;
    Material                     customMaterial;
    ConstructVertices(vertices, 60);
    meshData.SetData(vertices, faces, bones, customMaterial);
    Mesh mesh = Mesh::New(meshData);
    MeshActor actor1 = MeshActor::New(mesh);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "!faceIndices.empty", TEST_LOCATION );
  }
}

static void UtcDaliMeshActorCreateSetData03()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshData::SetData - Create with no mats");
  try
  {
    MeshData meshData;
    MeshData::VertexContainer    vertices;
    MeshData::FaceIndices        faces;
    BoneContainer                bones;
    Material                     customMaterial;
    ConstructVertices(vertices, 60);
    ConstructFaces(vertices, faces);
    meshData.SetData(vertices, faces, bones, customMaterial);
    Mesh mesh = Mesh::New(meshData);
    MeshActor actor1 = MeshActor::New(mesh);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "material", TEST_LOCATION );
  }
}

static void UtcDaliMeshActorCreateSetData04()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::SetData()");

  MeshData meshData;
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 60);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);

  Mesh mesh = Mesh::New(meshData);
  MeshActor actor1 = MeshActor::New(mesh);
  DALI_TEST_CHECK(actor1);
}


static void UtcDaliMeshActorDownCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::DownCast()");

  MeshData meshData;
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 60);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);
  Mesh mesh = Mesh::New(meshData);

  MeshActor actor1 = MeshActor::New(mesh);
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  MeshActor meshActor = MeshActor::DownCast(child);

  DALI_TEST_CHECK(meshActor);
}

static void UtcDaliMeshActorDownCast2()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::DownCast()");

  Actor actor1 = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  MeshActor meshActor = MeshActor::DownCast(child);
  DALI_TEST_CHECK(!meshActor);

  Actor unInitialzedActor;
  meshActor = DownCast< MeshActor >( unInitialzedActor );
  DALI_TEST_CHECK(!meshActor);
}

static void UtcDaliMeshActorSetMaterial01()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::SetMaterial()");

  Mesh mesh = NewMesh();

  MeshActor actor = MeshActor::New(mesh);
  std::string name = "AMeshActor";
  Stage::GetCurrent().Add(actor);
  actor.SetName(name);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  Material customMaterial = Material::New("CustomMaterial");
  customMaterial.SetDiffuseColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  MeshActor::SetMaterial(actor, name, customMaterial);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK( actor.GetMaterial() == customMaterial );
}

static void UtcDaliMeshActorSetMaterial01b()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::SetMaterial()");

  Mesh mesh = NewMesh();

  Actor rootActor = Actor::New();
  MeshActor meshActor = MeshActor::New(mesh);
  rootActor.Add(meshActor);

  std::string name = "AMeshActor";
  meshActor.SetName(name);

  Stage::GetCurrent().Add(rootActor);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  Material customMaterial = Material::New("CustomMaterial");
  customMaterial.SetDiffuseColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  MeshActor::SetMaterial(rootActor, name, customMaterial);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(meshActor.GetMaterial() == customMaterial );
}


static void UtcDaliMeshActorSetMaterial02()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::SetMaterial()");

  Mesh mesh = NewMesh();
  MeshActor actor = MeshActor::New(mesh);

  std::string name = "AMeshActor";
  actor.SetName(name);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  Material baseMat = actor.GetMaterial();
  Material customMaterial = Material::New("CustomMaterial");
  customMaterial.SetDiffuseColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  MeshActor::SetMaterial(actor, "NoName", customMaterial);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK( actor.GetMaterial() == baseMat );
  DALI_TEST_CHECK( actor.GetMaterial() != customMaterial );
}

static void UtcDaliMeshActorSetMaterial02b()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::SetMaterial()");

  Mesh mesh = NewMesh();

  MeshActor actor = MeshActor::New(mesh);
  Stage::GetCurrent().Add(actor);

  std::string name = "AMeshActor";
  actor.SetName(name);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  Material baseMat = actor.GetMaterial();
  Material customMaterial = Material::New("CustomMaterial");
  customMaterial.SetDiffuseColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  MeshActor::SetMaterial(actor, "NoName", customMaterial);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK( actor.GetMaterial() == baseMat );
  DALI_TEST_CHECK( actor.GetMaterial() != customMaterial );
}


static void UtcDaliMeshActorSetMaterial03()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::SetMaterial()");

  Mesh mesh = NewMesh();

  MeshActor actor = MeshActor::New(mesh);
  std::string name = "AMeshActor";
  actor.SetName(name);
  Stage::GetCurrent().Add(actor);

  Material customMaterial = Material::New("CustomMaterial");
  customMaterial.SetDiffuseColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  actor.SetMaterial(customMaterial);
  application.SendNotification();
  application.Render(0);
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK(actor.GetMaterial() == customMaterial );
}

static void UtcDaliMeshActorSetMaterial03b()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::SetMaterial()");

  Mesh mesh = NewMesh();

  MeshActor actor = MeshActor::New(mesh);
  std::string name = "AMeshActor";
  actor.SetName(name);
  Stage::GetCurrent().Add(actor);

  Material customMaterial = Material::New("CustomMaterial");
  customMaterial.SetDiffuseColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  actor.SetMaterial(customMaterial);
  application.SendNotification();
  application.Render(0);
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK(actor.GetMaterial() == customMaterial );
}



static void UtcDaliMeshActorGetMaterial01()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::SetMaterial()");

  MeshData meshData;
  MeshData::VertexContainer vertices;
  MeshData::FaceIndices faces;
  BoneContainer bones;
  ConstructVertices(vertices, 60);
  ConstructFaces(vertices, faces);
  Material material = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, material);
  Mesh mesh = Mesh::New(meshData);

  MeshActor actor = MeshActor::New(mesh);
  std::string name = "AMeshActor";
  actor.SetName(name);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  Material gotMaterial = actor.GetMaterial();

  DALI_TEST_EQUALS( material.GetOpacity(), gotMaterial.GetOpacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( material.GetAmbientColor(), gotMaterial.GetAmbientColor(), TEST_LOCATION );
  DALI_TEST_EQUALS( material.GetDiffuseColor(), gotMaterial.GetDiffuseColor(), TEST_LOCATION );
  DALI_TEST_EQUALS( material.GetSpecularColor(), gotMaterial.GetSpecularColor(), TEST_LOCATION );
}


static void UtcDaliMeshActorGetMaterial02()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::SetMaterial()");

  MeshData meshData;
  MeshData::VertexContainer vertices;
  MeshData::FaceIndices faces;
  BoneContainer bones;
  ConstructVertices(vertices, 60);
  ConstructFaces(vertices, faces);
  Material material = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, material);
  Mesh mesh = Mesh::New(meshData);

  MeshActor actor = MeshActor::New(mesh);
  std::string name = "AMeshActor";
  actor.SetName(name);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  Material gotMaterial = actor.GetMaterial();

  DALI_TEST_EQUALS( material.GetOpacity(), gotMaterial.GetOpacity(), TEST_LOCATION );
  DALI_TEST_EQUALS( material.GetAmbientColor(), gotMaterial.GetAmbientColor(), TEST_LOCATION );
  DALI_TEST_EQUALS( material.GetDiffuseColor(), gotMaterial.GetDiffuseColor(), TEST_LOCATION );
  DALI_TEST_EQUALS( material.GetSpecularColor(), gotMaterial.GetSpecularColor(), TEST_LOCATION );
}

static void UtcDaliMeshActorSetLighting01()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::GetLighting()");

  Mesh mesh = NewMesh();

  MeshActor actor = MeshActor::New(mesh);
  Stage::GetCurrent().Add(actor);

  // Mesh actors should be lit by default
  DALI_TEST_EQUALS(actor.IsAffectedByLighting(), true, TEST_LOCATION);
}

static void UtcDaliMeshActorSetLighting02()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::SetLighting()");

  Mesh mesh = NewMesh();
  MeshActor actor = MeshActor::New(mesh);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  Light light = Light::New("KeyLight");
  light.SetFallOff(Vector2(10000.0f, 10000.0f));
  LightActor keyLightActor = LightActor::New();
  keyLightActor.SetParentOrigin(ParentOrigin::CENTER);
  keyLightActor.SetPosition(200.0f, 500.0f, 300.0f);
  keyLightActor.SetName(light.GetName());

  Stage::GetCurrent().Add(keyLightActor);
  keyLightActor.SetLight(light);
  keyLightActor.SetActive(true);

  actor.SetAffectedByLighting(true);
  DALI_TEST_EQUALS(actor.IsAffectedByLighting(), true, TEST_LOCATION);

  // Test rendering to ensure that the correct shader setup is used in renderer
  // (check in debugger or via coverage)
  application.Render(1);
  application.SendNotification();
  application.Render(1);
  application.SendNotification();
  application.Render(1);
  application.SendNotification();
  application.Render(1);
  application.SendNotification();
  application.Render(1);

  actor.SetAffectedByLighting(false);
  DALI_TEST_EQUALS(actor.IsAffectedByLighting(), false, TEST_LOCATION);

  // Test rendering to ensure that the correct shader setup is used in renderer
  // (check in debugger or via coverage)
  application.Render(1);
  application.SendNotification();
  application.Render(1);
  application.SendNotification();
  application.Render(1);
  application.SendNotification();
  application.Render(1);
  application.SendNotification();
  application.Render(1);
}


Material ConstructMaterial(float opacity, float diffuseOpacity)
{
  Material customMaterial = Material::New("CustomMaterial");
  customMaterial.SetOpacity(opacity);
  customMaterial.SetDiffuseColor(Vector4(0.8f, 0.0f, 0.4f, diffuseOpacity));
  customMaterial.SetAmbientColor(Vector4(0.2f, 1.0f, 0.6f, 1.0f));
  customMaterial.SetSpecularColor(Vector4(0.5f, 0.6f, 0.7f, 1.0f));
  return customMaterial;
}

static void TestBlending( TestApplication& application, Material material, float actorOpacity, BlendingMode::Type blendingMode, bool expectedBlend )
{
  MeshData meshData;
  MeshData::VertexContainer vertices;
  MeshData::FaceIndices faces;
  BoneContainer bones;
  ConstructVertices(vertices, 60);
  ConstructFaces(vertices, faces);
  meshData.SetData(vertices, faces, bones, material);
  Mesh mesh = Mesh::New(meshData);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  MeshActor actor = MeshActor::New(mesh);
  Stage::GetCurrent().Add(actor);

  actor.SetAffectedByLighting(false);
  actor.SetBlendMode(blendingMode);
  actor.SetOpacity(actorOpacity);

  TraceCallStack& cullFaceTrace = application.GetGlAbstraction().GetCullFaceTrace();
  cullFaceTrace.Enable(true);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( BlendEnabled( cullFaceTrace ), expectedBlend, TEST_LOCATION );
}


static void UtcDaliMeshActorBlend01()
{
  // Set Material with translucent color, actor color opaque, Set Use image alpha to true
  // Expect blending

  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::Blend01()");

  TestBlending( application, ConstructMaterial(0.5f, 0.5f), 1.0f, BlendingMode::AUTO, true );
}


static void UtcDaliMeshActorBlend02()
{
  // Set material to translucent, set use image alpha to false, set actor opacity to 1.0f
  // Expect no blending

  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::Blend02()");
  TestBlending( application, ConstructMaterial(0.5f, 0.5f), 1.0f, BlendingMode::OFF, false );
}

static void UtcDaliMeshActorBlend03()
{
  // Set material to opaque, set use image alpha to true, set actor opacity to 1.0f
  // Expect no blending

  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::Blend03()");
  TestBlending( application, ConstructMaterial(1.0f, 1.0f), 1.0f, BlendingMode::AUTO, false );
}


static void UtcDaliMeshActorBlend04()
{
  // Set material to have image with alpha, set use image alpha to true, set actor opacity to 1.0f
  // Expect blending
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::Blend04()");

  Material material = ConstructMaterial(1.0f, 1.0f);
  BitmapImage image = BitmapImage::New( 100, 50, Pixel::RGBA8888 );
  material.SetDiffuseTexture( image );
  application.SendNotification();
  application.Render(0);

  TestBlending( application, material, 1.0f, BlendingMode::AUTO, true );
}

static void UtcDaliMeshActorBlend05()
{
  // Set material to have image with alpha, set use image alpha to false, set actor opacity to 1.0f
  // Expect no blending

  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::Blend05()");

  Material material = ConstructMaterial(1.0f, 1.0f);
  BitmapImage image = BitmapImage::New( 100, 50, Pixel::RGBA8888 );
  material.SetDiffuseTexture( image );
  application.SendNotification();
  application.Render(0);

  TestBlending( application, material, 1.0f, BlendingMode::ON, true );
}


static void UtcDaliMeshActorBlend06()
{
  // Set material to have image without alpha, set use image alpha to true, set actor opacity to 1.0f
  // Expect no blending

  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::Blend()");

  Material material = ConstructMaterial(1.0f, 1.0f);
  BitmapImage image = BitmapImage::New( 100, 50, Pixel::RGB888 );
  material.SetDiffuseTexture( image );
  application.SendNotification();
  application.Render(0);

  TestBlending( application, material, 1.0f, BlendingMode::AUTO, false );
}

static void UtcDaliMeshActorBlend07()
{
  // Set material to have framebuffer with alpha, set use image alpha to true, set actor opacity to 1.0f
  // Expect blending
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::Blend07()");
  application.Render(0);

  Material material = ConstructMaterial(1.0f, 1.0f);
  FrameBufferImage image = FrameBufferImage::New( 100, 50, Pixel::RGBA8888 );
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );
  task.SetTargetFrameBuffer( image ); // To ensure frame buffer is connected
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  application.SendNotification();
  application.Render();

  material.SetDiffuseTexture( image ); // (to render from)
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  TestBlending( application, material, 1.0f, BlendingMode::AUTO, true );
}

static void UtcDaliMeshActorBlend08()
{
  // Set material to have image with alpha, set use image alpha to false, set actor opacity to 0.5f
  // Expect blending
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::Blend08()");

  Material material = ConstructMaterial(1.0f, 1.0f);
  BitmapImage image = BitmapImage::New( 100, 50, Pixel::RGBA8888 );
  material.SetDiffuseTexture( image );
  application.SendNotification();
  application.Render(0);

  TestBlending( application, material, 0.5f, BlendingMode::AUTO, true );
}

static void UtcDaliMeshActorBlend09()
{
  // Set material to have image with no alpha, set material opacity to 0.5, set use image alpha to true, set actor opacity to 1.0f
  // Expect blending
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::Blend08()");

  Material material = ConstructMaterial(0.5f, 1.0f);
  BitmapImage image = BitmapImage::New( 100, 50, Pixel::RGB888 );
  material.SetDiffuseTexture( image );
  application.SendNotification();
  application.Render(0);

  TestBlending( application, material, 1.0f, BlendingMode::AUTO, true );
}

static void UtcDaliMeshActorBoneUpdate01()
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::BoneUpdate01()");

  // Set up a mesh with bones.
  // animate bones
  // ensure bone actor's world matrix is updated

  Actor trunk = Actor::New();
  trunk.SetName("trunk");
  trunk.SetPosition(Vector3(100.0f, 200.0f, 300.0f));
  trunk.SetRotation(Quaternion(M_PI*0.3f, Vector3::XAXIS));
  Actor branch = Actor::New();
  branch.SetName("branch");
  branch.SetPosition(Vector3(10.0f, 100.0f, 0.0f));
  branch.SetRotation(Quaternion(M_PI*0.2f, Vector3::YAXIS));
  Actor twig = Actor::New();
  twig.SetName("twig");
  branch.SetPosition(Vector3(20.0f, 30.0f, 40.0f));

  Actor bug = Actor::New(); // Not a bone
  bug.SetName("bug");
  bug.SetPosition(Vector3(10.0f, 10.0f, 10.0f));

  Stage::GetCurrent().Add(trunk);
  trunk.Add(branch);
  branch.Add(twig);
  twig.Add(bug);

  MeshData meshData;
  CreateMeshData(meshData); // Created with named bones (as above)
  Mesh mesh = Mesh::New(meshData);
  MeshActor meshActor = MeshActor::New(mesh);
  Stage::GetCurrent().Add(meshActor);

  meshActor.BindBonesToMesh(Stage::GetCurrent().GetRootLayer());

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  // How to test?
  // Need to see what bone actor's node has set as world matrix.

  Animation anim = Animation::New(1.0f);
  anim.RotateBy(trunk, Radian(M_PI*0.5f), Vector3::ZAXIS);
  anim.Play();
  application.SendNotification();
  application.Render(500);
  application.SendNotification();
  application.Render(500);
  application.SendNotification();
  application.Render(10);

  // All bones have moved.
  // Check that their world matrix has been updated: ( Isn't IDENTITY )

  const Internal::SceneGraph::Node *twigNode = static_cast<const Internal::SceneGraph::Node *>(GetImplementation(twig).GetSceneObject());
  Matrix worldMatrix = twigNode->GetWorldMatrix(0);

  DALI_TEST_CHECK( worldMatrix != Matrix::IDENTITY );

  Matrix calcWorldMatrix = twig.GetCurrentWorldMatrix();
  DALI_TEST_CHECK( worldMatrix == calcWorldMatrix );

  // Non-bones have moved. Check that their world matrix is identity:
  const Internal::SceneGraph::Node *bugNode = static_cast<const Internal::SceneGraph::Node *>(GetImplementation(bug).GetSceneObject());
  Matrix bugWorldMatrix = bugNode->GetWorldMatrix(0);
  DALI_TEST_CHECK( bugWorldMatrix == Matrix::IDENTITY );

  // But also check that property is calculated as needed:
  calcWorldMatrix = bug.GetCurrentWorldMatrix();
  DALI_TEST_CHECK( bugWorldMatrix != calcWorldMatrix );

}

void UtcDaliMeshActorIndices()
{
  TestApplication application;
  Actor basicActor = Actor::New();
  Mesh mesh = NewMesh();
  MeshActor meshActor = MeshActor::New(mesh);

  Property::IndexContainer indices;
  meshActor.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.size() == basicActor.GetPropertyCount() ); // Mesh Actor does not have any properties
  DALI_TEST_EQUALS( indices.size(), meshActor.GetPropertyCount(), TEST_LOCATION );
}
