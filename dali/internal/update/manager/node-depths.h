#ifndef DALI_INTERNAL_UPDATE_MANAGER_NODE_DEPTHS_H
#define DALI_INTERNAL_UPDATE_MANAGER_NODE_DEPTHS_H
/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Node;

struct NodeDepthPair
{
  SceneGraph::Node* node;
  uint32_t sortedDepth;
  NodeDepthPair( SceneGraph::Node* node, uint32_t sortedDepth )
  : node(node),
    sortedDepth(sortedDepth)
  {
  }
};

struct NodeDepths
{
  NodeDepths()
  {
  }

  void Add( SceneGraph::Node* node, uint32_t sortedDepth )
  {
    nodeDepths.push_back( NodeDepthPair( node, sortedDepth ) );
  }

  std::vector<NodeDepthPair> nodeDepths;
};



} // namespace SceneGraph

} // namespace Internal

} // namespace Dali


#endif //DALI_INTERNAL_UPDATE_MANAGER_NODE_DEPTHS_H
