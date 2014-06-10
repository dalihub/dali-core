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

// CLASS HEADER
#include <dali/internal/event/modeling/mesh-impl.h>

// EXTERNAL INCLUDES
#include <stdio.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/geometry/mesh.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/internal/update/modeling/scene-graph-mesh.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/modeling/internal-mesh-data.h>

using namespace std;

namespace Dali
{
namespace Internal
{

MeshIPtr Mesh::New( const Dali::MeshData& meshData, bool discardable, bool scalingRequired )
{
  MeshIPtr mesh( new Mesh( meshData, discardable, scalingRequired ) );

  return mesh;
}

Mesh::Mesh( const Dali::MeshData& publicMeshData, bool discardable, bool scalingRequired )
: mBoneContainer( publicMeshData.GetBones() )
{
  ResourceClient& resourceClient = ThreadLocalStorage::Get().GetResourceClient();

  // Copy the mesh-data into an internal structure, and pass ownership to the resourceClient
  OwnerPointer<MeshData> meshDataPtr( new MeshData( publicMeshData, discardable, scalingRequired ) );
  const ResourceTicketPtr& ticket = resourceClient.AllocateMesh( meshDataPtr );

  mTicket = ticket.Get();
  mTicket->AddObserver( *this );

  Dali::Material materialHandle( publicMeshData.GetMaterial() );
  Material& material( GetImplementation( materialHandle ) );
  mMaterial = &material;
}

Mesh::~Mesh()
{
  mTicket->RemoveObserver(*this);
}

ResourceId Mesh::GetResourceId() const
{
  return mTicket->GetId();
}

MaterialIPtr Mesh::GetMaterial() const
{
  return mMaterial;
}

const BoneContainer& Mesh::GetBones() const
{
  return mBoneContainer;
}

void Mesh::UpdateMeshData( const Dali::MeshData& meshData )
{
  DALI_ASSERT_DEBUG(meshData.GetFaceCount());

  ResourceClient &resourceClient = ThreadLocalStorage::Get().GetResourceClient();
  resourceClient.UpdateMesh( mTicket, meshData );
}

void Mesh::ResourceLoadingFailed(const ResourceTicket& ticket)
{
  // This class is for programmatic meshes defined by the application
  // and is not currently used by the model resource loader
}

void Mesh::ResourceLoadingSucceeded(const ResourceTicket& ticket)
{
  // This class is for programmatic meshes defined by the application
  // and is not currently used by the model resource loader
}

void Mesh::ResourceUploaded(const ResourceTicket& ticket)
{
}

void Mesh::ResourceSavingSucceeded( const ResourceTicket& ticket )
{
  // do nothing
}

void Mesh::ResourceSavingFailed( const ResourceTicket& ticket )
{
  // do nothing
}

} // namespace Internal

} // namespace Dali

