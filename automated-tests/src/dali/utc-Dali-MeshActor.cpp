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


void mesh_actor_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void mesh_actor_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static AnimatableMesh NewMesh()
{
  AnimatableMesh::Faces faces;
  faces.push_back(0);
  faces.push_back(1);
  faces.push_back(2);

  Material customMaterial = Material::New("CustomMaterial");
  customMaterial.SetOpacity(.76f);
  customMaterial.SetDiffuseColor(Vector4(0.8f, 0.0f, 0.4f, 1.0f));
  customMaterial.SetAmbientColor(Vector4(0.2f, 1.0f, 0.6f, 1.0f));
  customMaterial.SetSpecularColor(Vector4(0.5f, 0.6f, 0.7f, 1.0f));

  AnimatableMesh mesh = AnimatableMesh::New( 10u, faces, customMaterial );
  return mesh;
}
} // anonymous namespace

int UtcDaliMeshActorConstructorVoid(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::MeshActor()");

  MeshActor actor;
  DALI_TEST_CHECK(!actor);
  END_TEST;
}

int UtcDaliMeshActorNew01(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::New()");

  AnimatableMesh mesh = NewMesh();
  MeshActor actor = MeshActor::New(mesh);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(actor);
  END_TEST;
}

int UtcDaliMeshActorNew03(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Mesh::New() - Create with no mesh");

  try
  {
    MeshActor actor = MeshActor::New(); // Shouldn't assert
    tet_result(TET_PASS);
  }
  catch (Dali::DaliException& e)
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliMeshActorIndices(void)
{
  TestApplication application;
  Actor basicActor = Actor::New();
  AnimatableMesh mesh = NewMesh();
  MeshActor meshActor = MeshActor::New(mesh);

  Property::IndexContainer indices;
  meshActor.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.size() == basicActor.GetPropertyCount() ); // Mesh Actor does not have any properties
  DALI_TEST_EQUALS( indices.size(), meshActor.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}
