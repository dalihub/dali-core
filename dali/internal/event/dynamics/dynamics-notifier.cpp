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
#include <dali/internal/event/dynamics/dynamics-notifier.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-collision-data.h>

#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>

namespace Dali
{

namespace Internal
{

DynamicsNotifier::DynamicsNotifier()
{
}

DynamicsNotifier::~DynamicsNotifier()
{
}

void DynamicsNotifier::CollisionImpact( Integration::DynamicsCollisionData* collisionData )
{
  if( Stage::GetCurrent()->GetDynamicsWorld() )
  {
    Stage::GetCurrent()->GetDynamicsWorld()->CollisionImpact(collisionData);
  }
  delete collisionData;
}

void DynamicsNotifier::CollisionScrape( Integration::DynamicsCollisionData* collisionData )
{
}

void DynamicsNotifier::CollisionDisperse( Integration::DynamicsCollisionData* collisionData )
{
  if( Stage::GetCurrent()->GetDynamicsWorld() )
  {
    Stage::GetCurrent()->GetDynamicsWorld()->CollisionDisperse(collisionData);
  }
  delete collisionData;
}

} // namespace Internal

} // namespace Dali
