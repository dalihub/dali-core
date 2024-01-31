#ifndef DALI_INTERNAL_SCENE_GRAPH_TRAVELER_INTERFACE_H
#define DALI_INTERNAL_SCENE_GRAPH_TRAVELER_INTERFACE_H

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
#include <dali/internal/update/common/property-owner.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Node;
class UpdateManager;
} // namespace SceneGraph

class SceneGraphTravelerInterface;
using SceneGraphTravelerInterfacePtr = IntrusivePtr<SceneGraphTravelerInterface>;

/**
 * @brief Interface of helper class to travel scene graph.
 */
class SceneGraphTravelerInterface : public Dali::RefObject
{
public:
  /**
   * @brief Construct
   * @param[in] updateManager The update manager.
   */
  SceneGraphTravelerInterface(SceneGraph::UpdateManager& updateManager)
  : mUpdateManager(updateManager)
  {
  }

  /**
   * @brief Destructor
   */
  virtual ~SceneGraphTravelerInterface() = default;

public:
  /**
   * @brief Get SceneGraph::Node pointer from node id.
   * The way of find & choose mechanism is depend on inherited class.
   *
   * @param[in] id The id of node what we want to find.
   * @return Node pointer, or nullptr if we fail to find it.
   */
  virtual SceneGraph::Node* FindNode(uint32_t id) = 0;

protected:
  SceneGraph::UpdateManager& mUpdateManager;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_PROXY_SCENE_GRAPH_TRAVELER_H
