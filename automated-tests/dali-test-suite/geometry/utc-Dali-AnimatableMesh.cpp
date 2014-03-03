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

TEST_FUNCTION( UtcDaliAnimatableMeshConstructor01, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshConstructor02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshConstructor03, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshNew01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshNew02, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshNew03, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshNew04, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshNew05, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshNew06, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshDownCast01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshDownCast02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshGetPropertyIndex01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshGetPropertyIndex02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshGetPropertyIndex03, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshGetPropertyIndex04, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshOperatorArray01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshOperatorArray02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableMeshAnimateVertex01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnimatableVertexSettersAndGetters, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

void CreateFaces(Dali::AnimatableMesh::Faces& faces, int numVerts)
{
  for(int i=0; i<numVerts-3; i++)
  {
    faces.push_back(i);
    faces.push_back(i+1);
    faces.push_back(i+2);
  }
}

void CreateOutOfRangeFaces(Dali::AnimatableMesh::Faces& faces, int numVerts)
{
  for(int i=numVerts; i<numVerts*2-3; i++)
  {
    faces.push_back(i);
    faces.push_back(i+1);
    faces.push_back(i+2);
  }
}

AnimatableMesh CreateMesh()
{
  AnimatableMesh::Faces faces;
  CreateFaces(faces, 10);
  AnimatableMesh mesh = AnimatableMesh::New(10, faces);
  return mesh;
}

// Negative test case for a method
static void UtcDaliAnimatableMeshConstructor01()
{
  TestApplication application;

  AnimatableMesh mesh;

  DALI_TEST_CHECK( ! mesh );
}

static void UtcDaliAnimatableMeshConstructor02()
{
  TestApplication application;

  Dali::AnimatableMesh::Faces faces;
  CreateFaces(faces, 10);

  AnimatableMesh mesh = AnimatableMesh::New(10, faces);
  DALI_TEST_CHECK( mesh );

  AnimatableMesh mesh2 = mesh;
  DALI_TEST_CHECK( mesh2 );

  AnimatableMesh mesh3 ( mesh2 );
  DALI_TEST_CHECK( mesh3 );
}

static void UtcDaliAnimatableMeshConstructor03()
{
  TestApplication application;

  // Heap allocate a handle. Don't do this in real code!
  AnimatableMesh* mesh = new AnimatableMesh();
  DALI_TEST_CHECK( ! *mesh );
  delete mesh;
}


// Positive test case for a method
static void UtcDaliAnimatableMeshNew01()
{
  TestApplication application;

  Dali::AnimatableMesh::Faces faces;
  CreateFaces(faces, 10);

  AnimatableMesh mesh = AnimatableMesh::New(10, faces);
  DALI_TEST_CHECK( mesh );
}

// Positive test case for a method
static void UtcDaliAnimatableMeshNew02()
{
  TestApplication application;

  Dali::AnimatableMesh::Faces faces;
  CreateFaces(faces, 10);

  Dali::Material mat = Dali::Material::New("dummy mat");
  AnimatableMesh mesh = AnimatableMesh::New(10, faces, mat);
  DALI_TEST_CHECK( mesh );
}


// Negative test case for a method
static void UtcDaliAnimatableMeshNew03()
{
  TestApplication application;
  Dali::AnimatableMesh::Faces faces;
  try
  {
    AnimatableMesh mesh = AnimatableMesh::New(0, faces);
    DALI_TEST_CHECK( !mesh );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "numVertices > 0", TEST_LOCATION);
  }
}

// Negative test case for a method
static void UtcDaliAnimatableMeshNew04()
{
  TestApplication application;

  Dali::AnimatableMesh::Faces faces;

  try
  {
    AnimatableMesh mesh = AnimatableMesh::New(10, faces);
    DALI_TEST_CHECK( !mesh );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "faceIndices.size() > 0", TEST_LOCATION);
  }
}

// Negative test case for a method
static void UtcDaliAnimatableMeshNew05()
{
  TestApplication application;

  Dali::AnimatableMesh::Faces faces;
  CreateOutOfRangeFaces(faces, 10);

  try
  {
    AnimatableMesh mesh = AnimatableMesh::New(10, faces);
    DALI_TEST_CHECK( !mesh );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "faceIndex < numVertices", TEST_LOCATION);
  }
}

// Negative test case for a method
static void UtcDaliAnimatableMeshNew06()
{
  TestApplication application;

  Dali::AnimatableMesh::Faces faces;
  CreateFaces(faces, 10);

  try
  {
    AnimatableMesh mesh = AnimatableMesh::New(10, faces, Dali::Material() );
    DALI_TEST_CHECK( !mesh );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "material", TEST_LOCATION);
  }
}

static void UtcDaliAnimatableMeshDownCast01()
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::DownCast()");

  Dali::AnimatableMesh::Faces faces;
  CreateFaces(faces, 10);

  AnimatableMesh mesh = AnimatableMesh::New(10, faces);
  BaseHandle* bh = &mesh;

  AnimatableMesh mesh2 = AnimatableMesh::DownCast(*bh);
  DALI_TEST_CHECK( mesh2 );
}

static void UtcDaliAnimatableMeshDownCast02()
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::DownCast()");

  MeshData meshData;
  CreateMeshData(meshData);
  Mesh mesh = Mesh::New(meshData);
  BaseHandle* bh = &mesh;

  AnimatableMesh mesh2 = AnimatableMesh::DownCast(*bh);
  DALI_TEST_CHECK( ! mesh2 );
}

static void UtcDaliAnimatableMeshGetPropertyIndex01()
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::operator[]");
  AnimatableMesh mesh = CreateMesh();

  Property::Index i = mesh.GetPropertyIndex(0, AnimatableVertex::POSITION );
  DALI_TEST_EQUALS( i, 0*3+0, TEST_LOCATION );

  i = mesh.GetPropertyIndex(5, AnimatableVertex::POSITION );
  DALI_TEST_EQUALS( i, 5*3+0, TEST_LOCATION );

  i = mesh.GetPropertyIndex(7, AnimatableVertex::COLOR );
  DALI_TEST_EQUALS( i, 7*3+1, TEST_LOCATION );

  i = mesh.GetPropertyIndex(9, AnimatableVertex::TEXTURE_COORDS );
  DALI_TEST_EQUALS( i, 9*3+2, TEST_LOCATION );
}

static void UtcDaliAnimatableMeshGetPropertyIndex02()
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::GetPropertyIndexa");

  AnimatableMesh mesh = CreateMesh();
  try
  {
    Property::Index i = mesh.GetPropertyIndex(12, AnimatableVertex::POSITION );
    DALI_TEST_CHECK( i==0 );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "index < GetNumberOfVertices()", TEST_LOCATION);
  }
}

static void UtcDaliAnimatableMeshGetPropertyIndex03()
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::GetPropertyIndexa");

  AnimatableMesh mesh = CreateMesh();
  try
  {
    Property::Index i = mesh.GetPropertyIndex(12, AnimatableVertex::COLOR );
    DALI_TEST_CHECK( i==0 );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "index < GetNumberOfVertices()", TEST_LOCATION);
  }
}

static void UtcDaliAnimatableMeshGetPropertyIndex04()
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::GetPropertyIndexa");

  AnimatableMesh mesh = CreateMesh();
  try
  {
    Property::Index i = mesh.GetPropertyIndex(12342343, AnimatableVertex::TEXTURE_COORDS );
    DALI_TEST_CHECK( i==0 );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "index < GetNumberOfVertices()", TEST_LOCATION);
  }
}

static void UtcDaliAnimatableMeshOperatorArray01()
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::operator[]");

  AnimatableMesh mesh = CreateMesh();
  {
    Vector3 initialPos1(0.0f, 200.0f, 0.0f);
    Vector3 initialPos2(100.0f, 300.0f, 0.0f);

    mesh[1].SetPosition(initialPos1);
    mesh[3].SetPosition(initialPos2);

    application.Render(0);
    application.SendNotification();
    application.Render(16);
    application.SendNotification();
    DALI_TEST_EQUALS( mesh[1].GetCurrentPosition(), initialPos1, TEST_LOCATION );

    Vector3 pos = mesh[3].GetCurrentPosition();
    DALI_TEST_EQUALS( pos, initialPos2, TEST_LOCATION );
  }
}

static void UtcDaliAnimatableMeshOperatorArray02()
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::operator[]");

  AnimatableMesh mesh = CreateMesh();
  try
  {
    mesh[20].SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "index < GetNumberOfVertices()", TEST_LOCATION);
  }
}

static void UtcDaliAnimatableMeshAnimateVertex01()
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh Animating properties");

  AnimatableMesh mesh = CreateMesh();
  MeshActor meshActor = MeshActor::New(mesh);
  Stage::GetCurrent().Add(meshActor);
  {
    mesh[0].SetPosition(Vector3(0.0f, 200.0f, 0.0f));
    mesh[1].SetPosition(Vector3(100.0f, 300.0f, 0.0f));

    Animation anim = Animation::New(1);
    anim.AnimateBy(mesh.GetVertexProperty(0, AnimatableVertex::POSITION), Vector3(  0.0f, 100.0f, 0.0f));
    anim.AnimateTo(mesh.GetVertexProperty(1, AnimatableVertex::POSITION), Vector3(100.0f,   0.0f, 0.0f));
    anim.Play();

    application.SendNotification();
    application.Render(0);
    application.Render(500);
    application.SendNotification();

    // 50% progress
    DALI_TEST_EQUALS( mesh[0].GetCurrentPosition(), Vector3(  0.0f, 250.0f, 0.0f), TEST_LOCATION );
    DALI_TEST_EQUALS( mesh[1].GetCurrentPosition(), Vector3(100.0f, 150.0f, 0.0f), TEST_LOCATION );

    application.SendNotification();
    application.Render(501);
    application.SendNotification();

    DALI_TEST_EQUALS( mesh[0].GetCurrentPosition(), Vector3(  0.0f, 300.0f, 0.0f), TEST_LOCATION );
    DALI_TEST_EQUALS( mesh[1].GetCurrentPosition(), Vector3(100.0f,   0.0f, 0.0f), TEST_LOCATION );
  }
}

static void UtcDaliAnimatableVertexSettersAndGetters()
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableVertex constructors");
  AnimatableMesh mesh = CreateMesh();
  Vector3 v1Pos(0.0f, 200.0f, 0.0f);
  Vector3 v2Pos(100.0f, 300.0f, 0.0f);
  Vector2 uvs(0.1f, 0.2f);
  mesh[0].SetPosition(v1Pos);
  mesh[1].SetPosition(v2Pos);
  mesh[2].SetColor(Color::BLACK);
  mesh[3].SetTextureCoords(uvs);

  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  DALI_TEST_EQUALS(mesh[0].GetCurrentPosition(), v1Pos, TEST_LOCATION);
  DALI_TEST_EQUALS(mesh[1].GetCurrentPosition(), v2Pos, TEST_LOCATION);
  DALI_TEST_EQUALS(mesh[2].GetCurrentColor(), Color::BLACK, TEST_LOCATION);
  DALI_TEST_EQUALS(mesh[3].GetCurrentTextureCoords(), uvs, TEST_LOCATION);
}
