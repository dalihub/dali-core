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
#include <dali/internal/update/dynamics/scene-graph-dynamics-shape.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-shape-intf.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DynamicsShape::DynamicsShape(DynamicsWorld& world)
: mShape(NULL),
  mWorld(world)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

DynamicsShape::~DynamicsShape()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  delete mShape;
}

void DynamicsShape::Initialize()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  mWorld.AddShape( *this );
}

void DynamicsShape::Delete()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  mWorld.DeleteShape( *this );
}

Integration::DynamicsShape* DynamicsShape::GetShape() const
{
  return mShape;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
