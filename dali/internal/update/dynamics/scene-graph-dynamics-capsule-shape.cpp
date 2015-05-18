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
#include <dali/internal/update/dynamics/scene-graph-dynamics-capsule-shape.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>
#include <dali/integration-api/dynamics/dynamics-shape-intf.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>
#include <dali/devel-api/dynamics/dynamics-shape.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DynamicsCapsuleShape::DynamicsCapsuleShape(DynamicsWorld& world)
: DynamicsShape(world)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

DynamicsCapsuleShape::~DynamicsCapsuleShape()
{
}

void DynamicsCapsuleShape::Initialize(const float radius, const float length)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (radius: %.1f length: %.1f)\n", __PRETTY_FUNCTION__, radius, length);

  mShape = mWorld.GetDynamicsFactory().CreateDynamicsShape();

  const float worldScale((1.0f / mWorld.GetWorldScale()) );
  mShape->Initialize( Dali::DynamicsShape::CAPSULE, Vector3( radius * worldScale, length * worldScale, 0.0f ) );

  DynamicsShape::Initialize();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
