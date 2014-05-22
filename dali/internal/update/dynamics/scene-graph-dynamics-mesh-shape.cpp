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

// CLASS HEADER
#include <dali/internal/update/dynamics/scene-graph-dynamics-mesh-shape.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>
#include <dali/integration-api/dynamics/dynamics-shape-intf.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>
#include <dali/internal/update/modeling/scene-graph-mesh.h>
#include <dali/internal/update/modeling/internal-mesh-data.h>
#include <dali/public-api/dynamics/dynamics-shape.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DynamicsMeshShape::DynamicsMeshShape(DynamicsWorld& world, ResourceManager& resourceManager, const Integration::ResourceId meshId )
: DynamicsShape(world),
  mResourceManager(resourceManager),
  mMeshId(meshId)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - meshId:%d\n", __PRETTY_FUNCTION__, static_cast<int>(mMeshId));
}


DynamicsMeshShape::~DynamicsMeshShape()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

void DynamicsMeshShape::Initialize()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  mShape = mWorld.GetDynamicsFactory().CreateDynamicsShape();

  Mesh* mesh( GetMesh() );
  DALI_ASSERT_DEBUG( mesh );

  MeshData& meshData( mesh->GetMeshData(Mesh::UPDATE_THREAD ) );
  mShape->Initialize( Dali::DynamicsShape::MESH, meshData.GetVertices(), meshData.GetFaces() );

  DynamicsShape::Initialize();
}

Mesh* DynamicsMeshShape::GetMesh() const
{
  return mResourceManager.GetMesh(mMeshId);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
