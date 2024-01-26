/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/manager/global-scene-graph-traveler.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali
{
namespace Internal
{
GlobalSceneGraphTraveler::GlobalSceneGraphTraveler(SceneGraph::UpdateManager& updateManager)
: SceneGraphTravelerInterface(updateManager)
{
}

SceneGraph::Node* GlobalSceneGraphTraveler::FindNode(uint32_t id)
{
  return mUpdateManager.GetNodePointerById(id);
}

} // namespace Internal

} // namespace Dali
