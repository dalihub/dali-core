#ifndef DALI_INTERNAL_GLOBAL_SCENE_GRAPH_TRAVELER_H
#define DALI_INTERNAL_GLOBAL_SCENE_GRAPH_TRAVELER_H

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

// INTERNAL INCLUDES
#include <dali/internal/update/manager/scene-graph-traveler-interface.h>

namespace Dali
{
namespace Internal
{
class GlobalSceneGraphTraveler;
using GlobalSceneGraphTravelerPtr = IntrusivePtr<GlobalSceneGraphTraveler>;
/**
 * @brief Helper class to travel global scene graph.
 */
class GlobalSceneGraphTraveler : public SceneGraphTravelerInterface
{
public:
  /**
   * @brief Construct
   * @param[in] updateManager The update manager.
   */
  GlobalSceneGraphTraveler(SceneGraph::UpdateManager& updateManager);

  /**
   * @brief Destructor
   */
  ~GlobalSceneGraphTraveler() override
  {
  }

public: // From SceneGraphTravelerInterface
  /**
   * @copydoc Dali::Internal::SceneGraphTravelerInterface::FindNode()
   */
  SceneGraph::Node* FindNode(uint32_t id) override;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_PROXY_SCENE_GRAPH_TRAVELER_H
