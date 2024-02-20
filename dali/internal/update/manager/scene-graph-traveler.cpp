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
#include <dali/internal/update/manager/scene-graph-traveler.h>

// INTERNAL INCLUDES
#include <dali/integration-api/trace.h>
#include <dali/internal/update/nodes/node.h>

namespace
{
// TODO : The name of trace marker name is from VD specific. We might need to change it future.
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_COMBINED, false);
} // namespace

namespace Dali
{
namespace Internal
{
SceneGraphTraveler::SceneGraphTraveler(SceneGraph::Node& rootNode)
: mRootNode(rootNode),
  mNodeStack{},
  mInvalidated{false}
{
  mRootNode.AddObserver(*this);
  Clear();
}

SceneGraphTraveler::~SceneGraphTraveler()
{
  if(!mInvalidated)
  {
    mRootNode.RemoveObserver(*this);
  }
}

SceneGraph::Node* SceneGraphTraveler::FindNode(uint32_t id)
{
  SceneGraph::Node* node = nullptr;

  // Find node in cached map
  auto iter = mTravledNodeMap.find(id);
  if(iter != mTravledNodeMap.end())
  {
    node = iter->second;
  }
  else
  {
    while(!FullSearched())
    {
      SceneGraph::Node& currentNode = GetCurrentNode();
      IterateNextNode();

      // Cache traveled node and id pair.
      mTravledNodeMap.insert({currentNode.mId, &currentNode});

      if(currentNode.mId == id)
      {
        node = &currentNode;
        break;
      }
    }
#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      DALI_LOG_DEBUG_INFO("END: DALI_SCENE_GRAPH_TRAVELER [%zu,found:%d]\n", mTravledNodeMap.size(), (node == nullptr ? 0 : 1));
    }
#endif
  }

  return node;
}

void SceneGraphTraveler::Clear()
{
  mTravledNodeMap.clear();
  mTravledNodeMap.rehash(0u); ///< Note : We have to reduce capacity of hash map. Without this line, clear() API will be slow downed.
  mNodeStack.clear();
  if(!mInvalidated)
  {
    mNodeStack.emplace_back(&mRootNode, 0u);
  }
}

bool SceneGraphTraveler::FullSearched() const
{
  return mNodeStack.empty();
}

SceneGraph::Node& SceneGraphTraveler::GetCurrentNode()
{
  DALI_ASSERT_DEBUG(!FullSearched());

  return *(mNodeStack.back().first);
}

void SceneGraphTraveler::IterateNextNode()
{
  while(!mNodeStack.empty())
  {
    auto&    currentNode       = *(mNodeStack.back().first);
    uint32_t currentChildIndex = mNodeStack.back().second;

    if(currentNode.GetChildren().Count() <= currentChildIndex)
    {
      mNodeStack.pop_back();
      continue;
    }
    else
    {
      // Stack current child, and increase index
      ++mNodeStack.back().second;
      mNodeStack.emplace_back(currentNode.GetChildren()[currentChildIndex], 0u);
      break;
    }
  }
}

} // namespace Internal

} // namespace Dali
