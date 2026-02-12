#ifndef DALI_INTERNAL_SCENE_GRAPH_TRAVELER_H
#define DALI_INTERNAL_SCENE_GRAPH_TRAVELER_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
#include <dali/devel-api/common/map-wrapper.h>
#else
#include <unordered_map>
#endif

// INTERNAL INCLUDES
#include <dali/internal/update/manager/scene-graph-traveler-interface.h>

namespace Dali
{
namespace Internal
{
class SceneGraphTraveler;
using SceneGraphTravelerPtr = IntrusivePtr<SceneGraphTraveler>;

/**
 * @brief Helper class to travel scene graph under root node.
 */
class SceneGraphTraveler : public SceneGraphTravelerInterface, public SceneGraph::PropertyOwner::Observer
{
public:
  /**
   * @brief Construct
   * @param[in] updateManager The update manager.
   * @param[in] rootNode The root node of this traveler. The traveler will find only under this rootNode.
   */
  SceneGraphTraveler(SceneGraph::UpdateManager& updateManager, SceneGraph::Node& rootNode);

  /**
   * @brief Destructor
   */
  ~SceneGraphTraveler() override;

public:
  /**
   * @brief Call this method if hierarchy was changed under root node.
   */
  void NodeHierarchyChanged()
  {
    Clear();
  }

  /**
   * @brief Whether root node is invalidated or not.
   *
   * @return True if root node is invalidated.
   */
  bool IsInvalidated() const
  {
    return mInvalidated;
  }

public: // From SceneGraphTravelerInterface
  /**
   * @copydoc Dali::Internal::SceneGraphTravelerInterface::FindNode()
   */
  SceneGraph::Node* FindNode(uint32_t id) override;

private: // From SceneGraph::PropertyOwner::Observer
  /**
   * @copydoc SceneGraph::PropertyOwner::Observer::PropertyOwnerConnected()
   */
  void PropertyOwnerConnected(SceneGraph::PropertyOwner& owner) override
  { /* Nothing to do */
  }

  /**
   * @copydoc SceneGraph::PropertyOwner::Observer::PropertyOwnerDisconnected()
   */
  NotifyReturnType PropertyOwnerDisconnected(SceneGraph::PropertyOwner& owner) override
  { /* Nothing to do */
    return NotifyReturnType::KEEP_OBSERVING;
  }

  /**
   * @copydoc SceneGraph::PropertyOwner::Observer::PropertyOwnerDisconnected()
   */
  void PropertyOwnerDestroyed(SceneGraph::PropertyOwner& owner) override
  {
    // Invalidate this traveler
    mInvalidated = true;
    Clear();
  }

private:
  void Clear();

private:
  SceneGraph::Node& mRootNode;

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  using TraveledNodeMap = std::map<uint32_t, SceneGraph::Node*>;
#else
  using TraveledNodeMap = std::unordered_map<uint32_t, SceneGraph::Node*>;
#endif
  TraveledNodeMap mTravledNodeMap; ///< Used to store cached pointers to already searched for Nodes.

  bool mInvalidated : 1; ///< True if root node was destroyed.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_PROXY_SCENE_GRAPH_TRAVELER_H
