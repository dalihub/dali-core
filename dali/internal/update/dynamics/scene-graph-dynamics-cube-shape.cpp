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
#include <dali/internal/update/dynamics/scene-graph-dynamics-cube-shape.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>
#include <dali/integration-api/dynamics/dynamics-shape-intf.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>
#include <dali/public-api/math/vector3.h>
#include <dali/devel-api/dynamics/dynamics-shape.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DynamicsCubeShape::DynamicsCubeShape(DynamicsWorld& world)
: DynamicsShape(world)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

DynamicsCubeShape::~DynamicsCubeShape()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

void DynamicsCubeShape::Initialize(const Vector3& dimensions)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::General, "%s - (dimensions(%.1f %.1f, %.1f))\n", __PRETTY_FUNCTION__, dimensions.width, dimensions.height, dimensions.depth);

  mShape = mWorld.GetDynamicsFactory().CreateDynamicsShape();

  mShape->Initialize( Dali::DynamicsShape::CUBE, (dimensions * 0.5f) / mWorld.GetWorldScale() );

  DynamicsShape::Initialize();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
