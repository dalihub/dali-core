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
#include <dali/internal/event/dynamics/dynamics-mesh-shape-impl.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-mesh-shape.h>

namespace Dali
{

namespace Internal
{

DynamicsMeshShape::DynamicsMeshShape(Mesh& mesh)
: DynamicsShape(Dali::DynamicsShape::MESH),
  mMesh( &mesh )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  StagePtr stage( Stage::GetCurrent() );
  DynamicsWorldPtr world( stage->GetDynamicsWorld() );
  DALI_ASSERT_ALWAYS( world && "No Dynamics World !");
  ResourceManager& resourceManager( ThreadLocalStorage::Get().GetResourceManager() );

  SceneGraph::DynamicsMeshShape* meshShape( new SceneGraph::DynamicsMeshShape( *world->GetSceneObject(), resourceManager, mesh.GetResourceId()) );
  mDynamicsShape = meshShape;

  // Queue a message to ensure the underlying dynamics object is created in the update thread
  InitializeDynamicsMeshShapeMessage( stage->GetUpdateInterface(), *meshShape );
}

DynamicsMeshShape::~DynamicsMeshShape()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

Vector3 DynamicsMeshShape::GetAABB() const
{
  return Vector3();
}

} // namespace Internal

} // namespace Dali
