#ifndef DALI_INTERNAL_SCENE_GRAPH_TRAVELER_H
#define DALI_INTERNAL_SCENE_GRAPH_TRAVELER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <unordered_map>

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
} // namespace SceneGraph

class SceneGraphTraveler;
using SceneGraphTravelerPtr = IntrusivePtr<SceneGraphTraveler>;

/**
 * @brief Helper class to travel scene graph incrementally.
 */
class SceneGraphTraveler : public Dali::RefObject, public SceneGraph::PropertyOwner::Observer
{
public:
  SceneGraphTraveler(SceneGraph::Node& rootNode);

  ~SceneGraphTraveler();

public:
  void NodeHierarchyChanged()
  {
    Clear();
  }

  bool IsInvalidated() const
  {
    return mInvalidated;
  }

  SceneGraph::Node* FindNode(uint32_t id);

private:
  // From SceneGraph::PropertyOwner::Observer

  /**
   * @copydoc SceneGraph::PropertyOwner::Observer::PropertyOwnerConnected()
   */
  void PropertyOwnerConnected(SceneGraph::PropertyOwner& owner) override
  { /* Nothing to do */
  }

  /**
   * @copydoc SceneGraph::PropertyOwner::Observer::PropertyOwnerDisconnected()
   */
  void PropertyOwnerDisconnected(BufferIndex updateBufferIndex, SceneGraph::PropertyOwner& owner) override
  { /* Nothing to do */
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

  bool FullSearched() const;

  SceneGraph::Node& GetCurrentNode();

  void IterateNextNode();

private:
  SceneGraph::Node&                                   mRootNode;
  std::vector<std::pair<SceneGraph::Node*, uint32_t>> mNodeStack; ///< Depth first search stack. Pair of node, and it's index of children that we need to iterate next.

  std::unordered_map<uint32_t, SceneGraph::Node*> mTravledNodeMap; ///< Used to store cached pointers to already searched for Nodes.

  bool mInvalidated : 1; ///< Troe if root node was destroyed.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_PROXY_SCENE_GRAPH_TRAVELER_H
