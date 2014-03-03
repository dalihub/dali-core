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
#include <dali/internal/event/dynamics/dynamics-cube-shape-impl.h>

// EXTERNAL HEADERS

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-cube-shape.h>

namespace Dali
{

namespace Internal
{

DynamicsCubeShape::DynamicsCubeShape(const Vector3& dimensions)
: DynamicsShape(Dali::DynamicsShape::CUBE)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (dimensions[%1.02f %1.02f %1.02f])\n", __PRETTY_FUNCTION__, dimensions.x, dimensions.y, dimensions.z);

  StagePtr stage( Stage::GetCurrent() );
  DynamicsWorldPtr world( stage->GetDynamicsWorld() );
  DALI_ASSERT_ALWAYS( world && "No Dynamics World !");

  SceneGraph::DynamicsCubeShape* cubeShape = new SceneGraph::DynamicsCubeShape( *world->GetSceneObject() );
  mDynamicsShape = cubeShape;

  // Queue a message to ensure the underlying dynamics object is created in the update thread
  InitializeDynamicsCubeShapeMessage( stage->GetUpdateInterface(), *cubeShape, dimensions );
}

DynamicsCubeShape::~DynamicsCubeShape()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

Vector3 DynamicsCubeShape::GetAABB() const
{
  return Vector3();
}

} // namespace Internal

} // namespace Dali
