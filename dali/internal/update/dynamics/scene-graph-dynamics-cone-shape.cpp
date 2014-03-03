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
// TODO: Change this to use #pragma GCC diagnostic push / pop when the compiler is updated to 4.6.0+
#include <dali/internal/update/dynamics/scene-graph-dynamics-cone-shape.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>
#include <dali/integration-api/dynamics/dynamics-shape-intf.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>
#include <dali/public-api/dynamics/dynamics-shape.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DynamicsConeShape::DynamicsConeShape(DynamicsWorld& world)
: DynamicsShape(world)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

DynamicsConeShape::~DynamicsConeShape()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

void DynamicsConeShape::Initialize(const float radius, const float length)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (radius: %.1f length: %.1f)\n", __PRETTY_FUNCTION__, radius, length);

  mShape = mWorld.GetDynamicsFactory().CreateDynamicsShape();
  const float worldScale((1.0f / mWorld.GetWorldScale()) );
  mShape->Initialize( Dali::DynamicsShape::CONE, Vector3( radius * worldScale, length * worldScale, 0.0f ) );
//  const float worldScale(1.0f / mWorld.GetWorldScale());
//  mShape = new btConeShape(radius * worldScale, length * worldScale);

  DynamicsShape::Initialize();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
