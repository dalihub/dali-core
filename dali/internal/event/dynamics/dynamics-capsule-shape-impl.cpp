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
#include <dali/internal/event/dynamics/dynamics-capsule-shape-impl.h>

// EXTERNAL HEADERS

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-capsule-shape.h>

namespace Dali
{

namespace Internal
{

DynamicsCapsuleShape::DynamicsCapsuleShape(const float radius, const float length)
: DynamicsShape(Dali::DynamicsShape::CAPSULE)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (radius:%.1f length:%.1f)\n", __PRETTY_FUNCTION__, radius, length);

  StagePtr stage( Stage::GetCurrent() );
  DynamicsWorldPtr world( stage->GetDynamicsWorld() );
  DALI_ASSERT_ALWAYS( world && "No Dynamics World !");

  SceneGraph::DynamicsCapsuleShape* capsuleShape = new SceneGraph::DynamicsCapsuleShape( *world->GetSceneObject() );
  mDynamicsShape = capsuleShape;

  // Queue a message to ensure the underlying dynamics object is created in the update thread
  InitializeDynamicsCapsuleShapeMessage( stage->GetUpdateInterface(), *capsuleShape, radius, length );
}

DynamicsCapsuleShape::~DynamicsCapsuleShape()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

Vector3 DynamicsCapsuleShape::GetAABB() const
{
  return Vector3();
}

} // namespace Internal

} // namespace Dali
