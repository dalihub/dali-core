#ifndef DALI_INTERNAL_SCENE_GRAPH_NODE_DECLARATIONS_H
#define DALI_INTERNAL_SCENE_GRAPH_NODE_DECLARATIONS_H

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
#include <dali/devel-api/common/bitwise-enum.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Node;

using NodeContainer = Dali::Vector<Node*>;
using NodeIter      = NodeContainer::Iterator;
using NodeConstIter = NodeContainer::ConstIterator;

/**
 * Flag whether property has changed, during the Update phase.
 */
enum class NodePropertyFlags : uint8_t
// 8 bits is enough for 7 flags (compiler will check it)
{
  NOTHING                      = 0x000,
  TRANSFORM                    = 0x001,
  VISIBLE                      = 0x002,
  COLOR                        = 0x004,
  CHILD_DELETED                = 0x008,
  CHILDREN_REORDER             = 0x010,
  DESCENDENT_HIERARCHY_CHANGED = 0x020,
  CLIPPING_MODE                = 0x040,
  ALL                          = (CLIPPING_MODE << 1) - 1 // all the flags
};

struct NodeDepthPair
{
  Node*    node;
  uint32_t sortedDepth;
  NodeDepthPair(Node* node, uint32_t sortedDepth)
  : node(node),
    sortedDepth(sortedDepth)
  {
  }
};

struct NodeDepths
{
  NodeDepths() = default;

  void Add(Node* node, uint32_t sortedDepth)
  {
    nodeDepths.push_back(NodeDepthPair(node, sortedDepth));
  }

  std::vector<NodeDepthPair> nodeDepths;
};

} // namespace SceneGraph

} // namespace Internal

// specialization has to be done in the same namespace
template<>
struct EnableBitMaskOperators<Internal::SceneGraph::NodePropertyFlags>
{
  static const bool ENABLE = true;
};

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_NODE_DECLARATIONS_H
