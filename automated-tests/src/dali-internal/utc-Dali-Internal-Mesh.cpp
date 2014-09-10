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

// Internal headers are allowed here

#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/update/modeling/scene-graph-mesh.h>

using namespace Dali;
#include "mesh-builder.h"

void utc_dali_mesh_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_mesh_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

unsigned int GetNextResourceId(TestApplication& application)
{
  // Attempt to determine what the next resource ID will be
  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Internal::ResourceTicketPtr resourceTicket = resourceClient.AllocateTexture( 80, 80, Pixel::RGBA8888 );
  unsigned int nextResourceId = resourceTicket->GetId();
  nextResourceId++;

  // Clear allocation request through the system
  application.SendNotification();
  application.Render();
  application.SendNotification();

  return nextResourceId;
}

Mesh CreateMesh( TestApplication& application )
{
  Dali::MeshData publicMeshData;
  {
    MeshData::VertexContainer    publicVertices;
    MeshData::FaceIndices        publicFaces;
    BoneContainer                bones;
    ConstructVertices(publicVertices, 60);
    ConstructFaces(publicVertices, publicFaces);
    Dali::Material material  = ConstructMaterial();
    publicMeshData.SetData(publicVertices, publicFaces, bones, material);
  }
  Mesh publicMesh = Mesh::New(publicMeshData); // Will generate new request

  application.SendNotification();
  application.Render(); // Should allocate mesh, pass ownership to ResourceManager
  application.SendNotification();
  return publicMesh;
}

Model CreateModel(TestApplication& application)
{
  TestPlatformAbstraction& platform = application.GetPlatform();

  // Raise a request
  Model model = Model::New("Tree");

  application.SendNotification();
  application.Render();

  Dali::ModelData modelData = BuildTreeModel();
  Integration::ResourceRequest* request = platform.GetRequest(); // Return modelData
  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(&(modelData.GetBaseObject())));
  }
  application.Render();           // This will perform the GetResources() to get the model data
  application.SendNotification(); // This will handle model loaded (triggering mesh allocation)
  application.Render();           // This will allocate meshes in update manager
  application.SendNotification();

  return model;
}


Internal::MeshData& GetStagedMeshData( unsigned int resourceId )
{
    // ResourceManager::GetMesh() will give us the scene graph mesh from resource id
  Internal::ResourceManager& resourceManager  = Internal::ThreadLocalStorage::Get().GetResourceManager();
  Internal::SceneGraph::Mesh* internalMesh = resourceManager.GetMesh(resourceId);
  DALI_TEST_CHECK( internalMesh != NULL );
  return internalMesh->GetMeshData(Internal::SceneGraph::Mesh::UPDATE_THREAD);
}


void TestMeshDiscard( ResourcePolicy::DataRetention policy, bool expectedDiscardResult )
{
  TestApplication application( TestApplication::DEFAULT_SURFACE_WIDTH,
                               TestApplication::DEFAULT_SURFACE_HEIGHT,
                               TestApplication::DEFAULT_HORIZONTAL_DPI,
                               TestApplication::DEFAULT_VERTICAL_DPI,
                               policy );

  // run through startup, clearing all requests/messages
  application.SendNotification();
  application.Render();
  application.SendNotification();

  unsigned int nextResourceId = GetNextResourceId(application);
  Mesh publicMesh = CreateMesh(application);
  Internal::MeshData& meshData = GetStagedMeshData(nextResourceId);

  // Check that the vertex data is allocated
  const Internal::MeshData::VertexContainer& internalVertices = meshData.GetVertices();
  const Internal::MeshData::FaceIndices& internalFaces = meshData.GetFaces();
  DALI_TEST_CHECK( meshData.GetVertexCount() > 0 );
  DALI_TEST_CHECK( meshData.GetFaceCount() > 0 );
  DALI_TEST_CHECK( internalVertices.size() > 0 );
  DALI_TEST_CHECK( internalFaces.size() > 0 );

  // Create an actor that will render the mesh
  MeshActor actor = MeshActor::New( publicMesh );
  std::string name = "AMeshActor";
  actor.SetName(name);
  actor.SetAffectedByLighting(false);
  Stage::GetCurrent().Add(actor);

  // render it
  application.SendNotification();
  application.Render();
  application.SendNotification();

  // Check that the vertex data has been discarded
  DALI_TEST_CHECK( meshData.GetVertexCount() > 0 );
  DALI_TEST_CHECK( meshData.GetFaceCount() > 0 );
  if( expectedDiscardResult == false )
  {
    DALI_TEST_CHECK( internalVertices.size() > 0 );
    DALI_TEST_CHECK( internalFaces.size() > 0 );
  }
  else
  {
    DALI_TEST_CHECK( internalVertices.size() == 0 );
    DALI_TEST_CHECK( internalFaces.size() == 0 );
  }
}



void TestModelDiscard( ResourcePolicy::DataRetention policy, bool expectedDiscardResult )
{
  TestApplication application( TestApplication::DEFAULT_SURFACE_WIDTH,
                               TestApplication::DEFAULT_SURFACE_HEIGHT,
                               TestApplication::DEFAULT_HORIZONTAL_DPI,
                               TestApplication::DEFAULT_VERTICAL_DPI,
                               policy );

  // run through startup, clearing all requests/messages
  application.SendNotification();
  application.Render();
  application.SendNotification();

  unsigned int nextResourceId = GetNextResourceId(application);
  Model model = CreateModel(application);

  // 2 resource id's should be generated, 1st for the model, 2nd for the only mesh
  Internal::MeshData& meshData = GetStagedMeshData(nextResourceId+1);

  // Check that the vertex data is allocated
  const Internal::MeshData::VertexContainer& internalVertices = meshData.GetVertices();
  const Internal::MeshData::FaceIndices& internalFaces = meshData.GetFaces();
  DALI_TEST_CHECK( meshData.GetVertexCount() > 0 );
  DALI_TEST_CHECK( meshData.GetFaceCount() > 0 );
  DALI_TEST_CHECK( internalVertices.size() > 0 );
  DALI_TEST_CHECK( internalFaces.size() > 0 );

  // Create an actor that will render the mesh
  Actor actor = ModelActorFactory::BuildActorTree(model, ""); // model should be loaded
  Light light = Light::New("KeyLight");
  light.SetFallOff(Vector2(10000.0f, 10000.0f));
  LightActor keyLightActor = LightActor::New();
  keyLightActor.SetParentOrigin(ParentOrigin::CENTER);
  keyLightActor.SetPosition(200.0f, 500.0f, 300.0f);
  keyLightActor.SetName(light.GetName());
  keyLightActor.SetLight(light);
  keyLightActor.SetActive(true);
  Stage::GetCurrent().Add(keyLightActor);
  Stage::GetCurrent().Add(actor);

  // render it
  application.SendNotification();
  application.Render();
  application.SendNotification();

  // Check that the vertex data has been discarded
  DALI_TEST_CHECK( meshData.GetVertexCount() > 0 );
  DALI_TEST_CHECK( meshData.GetFaceCount() > 0 );
  if( expectedDiscardResult == false )
  {
    DALI_TEST_CHECK( internalVertices.size() > 0 );
    DALI_TEST_CHECK( internalFaces.size() > 0 );
  }
  else
  {
    DALI_TEST_CHECK( internalVertices.size() == 0 );
    DALI_TEST_CHECK( internalFaces.size() == 0 );
  }
}


} // anon namespace


int utcDaliInternalMeshDiscard01(void)
{
  tet_infoline("Test that internal mesh data is discarded after rendering with policy=DISCARD_ALL");
  TestMeshDiscard( ResourcePolicy::DALI_DISCARDS_ALL_DATA, true /* Not discarded */);
  END_TEST;
}

int utcDaliInternalMeshDiscard02(void)
{
  tet_infoline("Test that internal mesh data is not discarded after rendering with policy=RETAIN_ALL");
  TestMeshDiscard( ResourcePolicy::DALI_RETAINS_ALL_DATA, false /* Not discarded */);
  END_TEST;
}

int utcDaliInternalMeshDiscard03(void)
{
  tet_infoline("Test that internal mesh data is not discarded after rendering with policy=RETAIN_MESH");
  TestMeshDiscard( ResourcePolicy::DALI_RETAINS_MESH_DATA, false /* Not discarded */);
  END_TEST;
}

int utcDaliInternalModelDiscard01(void)
{
  tet_infoline("Test that model's internal mesh data is discarded after rendering with policy=DISCARD_ALL");
  TestModelDiscard( ResourcePolicy::DALI_DISCARDS_ALL_DATA, true /* Not discarded */);
  END_TEST;
}

int utcDaliInternalModelDiscard02(void)
{
  tet_infoline("Test that model's internal mesh data is not discarded after rendering with policy=RETAIN_ALL");
  TestModelDiscard( ResourcePolicy::DALI_RETAINS_ALL_DATA, false /* Not discarded */);
  END_TEST;
}

int utcDaliInternalModelDiscard03(void)
{
  tet_infoline("Test that model's internal mesh data is not discarded after rendering with policy=RETAIN_MESH");
  TestModelDiscard( ResourcePolicy::DALI_RETAINS_MESH_DATA, false /* Not discarded */);
  END_TEST;
}
