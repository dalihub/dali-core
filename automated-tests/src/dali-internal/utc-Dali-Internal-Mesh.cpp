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
