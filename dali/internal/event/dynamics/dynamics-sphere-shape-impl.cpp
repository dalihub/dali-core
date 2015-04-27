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
#include <dali/internal/event/dynamics/dynamics-sphere-shape-impl.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-sphere-shape.h>

namespace Dali
{

namespace Internal
{

DynamicsSphereShape::DynamicsSphereShape(const float radius)
: DynamicsShape(Dali::DynamicsShape::SPHERE)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (radius: %f)\n", __PRETTY_FUNCTION__, radius);

  StagePtr stage( Stage::GetCurrent() );
  DynamicsWorldPtr world( stage->GetDynamicsWorld() );
  DALI_ASSERT_ALWAYS( world && "No Dynamics World !");

  SceneGraph::DynamicsSphereShape* sphereShape = new SceneGraph::DynamicsSphereShape(  *world->GetSceneObject() );
  mDynamicsShape = sphereShape;

  // Queue a message to ensure the underlying dynamics object is created in the update thread
  InitializeDynamicsSphereShapeMessage( *stage, *sphereShape, radius );
}

DynamicsSphereShape::~DynamicsSphereShape()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

Vector3 DynamicsSphereShape::GetAABB() const
{
  return Vector3();
}


} // namespace Internal

} // namespace Dali
