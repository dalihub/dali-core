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
#include <mesh-builder.h>

using namespace Dali;


void utc_dali_animatable_mesh_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_animatable_mesh_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

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

} // anon namespace

// Negative test case for a method
int UtcDaliAnimatableMeshConstructor01(void)
{
  TestApplication application;

  AnimatableMesh mesh;

  DALI_TEST_CHECK( ! mesh );
  END_TEST;
}

int UtcDaliAnimatableMeshConstructor02(void)
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
  END_TEST;
}

int UtcDaliAnimatableMeshConstructor03(void)
{
  TestApplication application;

  // Heap allocate a handle. Don't do this in real code!
  AnimatableMesh* mesh = new AnimatableMesh();
  DALI_TEST_CHECK( ! *mesh );
  delete mesh;
  END_TEST;
}


// Positive test case for a method
int UtcDaliAnimatableMeshNew01(void)
{
  TestApplication application;

  Dali::AnimatableMesh::Faces faces;
  CreateFaces(faces, 10);

  AnimatableMesh mesh = AnimatableMesh::New(10, faces);
  DALI_TEST_CHECK( mesh );
  END_TEST;
}

// Positive test case for a method
int UtcDaliAnimatableMeshNew02(void)
{
  TestApplication application;

  Dali::AnimatableMesh::Faces faces;
  CreateFaces(faces, 10);

  Dali::Material mat = Dali::Material::New("dummy mat");
  AnimatableMesh mesh = AnimatableMesh::New(10, faces, mat);
  DALI_TEST_CHECK( mesh );
  END_TEST;
}


// Negative test case for a method
int UtcDaliAnimatableMeshNew03(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "numVertices > 0", TEST_LOCATION);
  }
  END_TEST;
}

// Negative test case for a method
int UtcDaliAnimatableMeshNew04(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "faceIndices.size() > 0", TEST_LOCATION);
  }
  END_TEST;
}

// Negative test case for a method
int UtcDaliAnimatableMeshNew05(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "faceIndex < numVertices", TEST_LOCATION);
  }
  END_TEST;
}

// Negative test case for a method
int UtcDaliAnimatableMeshNew06(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "material", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimatableMeshDownCast01(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::DownCast()");

  Dali::AnimatableMesh::Faces faces;
  CreateFaces(faces, 10);

  AnimatableMesh mesh = AnimatableMesh::New(10, faces);
  BaseHandle* bh = &mesh;

  AnimatableMesh mesh2 = AnimatableMesh::DownCast(*bh);
  DALI_TEST_CHECK( mesh2 );
  END_TEST;
}

int UtcDaliAnimatableMeshDownCast02(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::DownCast()");

  MeshData meshData;
  CreateMeshData(meshData);
  Mesh mesh = Mesh::New(meshData);
  BaseHandle* bh = &mesh;

  AnimatableMesh mesh2 = AnimatableMesh::DownCast(*bh);
  DALI_TEST_CHECK( ! mesh2 );
  END_TEST;
}

int UtcDaliAnimatableMeshGetPropertyIndex01(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::operator[]");
  AnimatableMesh mesh = CreateMesh();

  Property::Index i = mesh.GetPropertyIndex(0, AnimatableVertex::Property::Position );
  DALI_TEST_EQUALS( i, 0*3+0, TEST_LOCATION );

  i = mesh.GetPropertyIndex(5, AnimatableVertex::Property::Position );
  DALI_TEST_EQUALS( i, 5*3+0, TEST_LOCATION );

  i = mesh.GetPropertyIndex(7, AnimatableVertex::Property::Color );
  DALI_TEST_EQUALS( i, 7*3+1, TEST_LOCATION );

  i = mesh.GetPropertyIndex(9, AnimatableVertex::Property::TextureCoords );
  DALI_TEST_EQUALS( i, 9*3+2, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimatableMeshGetPropertyIndex02(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::GetPropertyIndex");

  AnimatableMesh mesh = CreateMesh();
  try
  {
    Property::Index i = mesh.GetPropertyIndex(12, AnimatableVertex::Property::Position );
    DALI_TEST_CHECK( i==0 );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "index < GetNumberOfVertices()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimatableMeshGetPropertyIndex03(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::GetPropertyIndex");

  AnimatableMesh mesh = CreateMesh();
  try
  {
    Property::Index i = mesh.GetPropertyIndex(12, AnimatableVertex::Property::Color );
    DALI_TEST_CHECK( i==0 );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "index < GetNumberOfVertices()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimatableMeshGetPropertyIndex04(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::GetPropertyIndexa");

  AnimatableMesh mesh = CreateMesh();
  try
  {
    Property::Index i = mesh.GetPropertyIndex(12342343, AnimatableVertex::Property::TextureCoords );
    DALI_TEST_CHECK( i==0 );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "index < GetNumberOfVertices()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimatableMeshOperatorArray01(void)
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
  END_TEST;
}

int UtcDaliAnimatableMeshOperatorArray02(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "index < GetNumberOfVertices()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimatableMeshAnimateVertex01(void)
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
    anim.AnimateBy(mesh.GetVertexProperty(0, AnimatableVertex::Property::Position), Vector3(  0.0f, 100.0f, 0.0f));
    anim.AnimateTo(mesh.GetVertexProperty(1, AnimatableVertex::Property::Position), Vector3(100.0f,   0.0f, 0.0f));
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
  END_TEST;
}

int UtcDaliAnimatableVertexSettersAndGetters(void)
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
  END_TEST;
}

int UtcDaliAnimatableMeshProperties(void)
{
  TestApplication application;
  AnimatableMesh mesh = CreateMesh();

  Property::IndexContainer indices;
  mesh.GetPropertyIndices( indices );
  DALI_TEST_CHECK( ! indices.empty() );
  DALI_TEST_EQUALS( indices.size(), mesh.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimatableMeshExceedVertices(void)
{
  TestApplication application;

  AnimatableMesh::Faces faces;
  CreateFaces(faces, 10);

  try
  {
    AnimatableMesh mesh = AnimatableMesh::New(3333334, faces);
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( numVertices * 3 ) < DEFAULT_PROPERTY_MAX_COUNT", TEST_LOCATION );
  }
  END_TEST;
}
