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

// CLASS HEADER
#include <dali/internal/update/manager/scene-graph-traveler.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_UPDATE_PROCESS, false);

DALI_INIT_TIME_CHECKER_FILTER_WITH_DEFAULT_THRESHOLD(gTimeCheckerFilter, DALI_UPDATE_PROCESS_THRESHOLD_TIME, 48);
} // namespace

SceneGraphTraveler::SceneGraphTraveler(SceneGraph::UpdateManager& updateManager, SceneGraph::Node& rootNode)
: SceneGraphTravelerInterface(updateManager),
  mRootNode(rootNode),
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

  if(!mInvalidated)
  {
    // Find node in cached map
    auto iter = mTravledNodeMap.find(id);
    if(iter != mTravledNodeMap.end())
    {
      node = iter->second;
    }
    else
    {
      DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_TRAVELER_FIND_NEW_NODE", [&](std::ostringstream& oss)
      { oss << "root[" << mRootNode.GetId() << "] input [" << id << "] cached count[" << mTravledNodeMap.size() << "]"; });

      DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);

      SceneGraph::Node* currentNode = mUpdateManager.GetNodePointerById(id);

      bool isNodeUnderRootNode = false;

      std::vector<std::pair<uint32_t, SceneGraph::Node*>> nodeStack;

      SceneGraph::Node* iterateNode = currentNode;

      while(iterateNode)
      {
        uint32_t iterateNodeId = iterateNode->GetId();

        auto iter = mTravledNodeMap.find(iterateNodeId);
        if(iter != mTravledNodeMap.end())
        {
          isNodeUnderRootNode = true;
          break;
        }
        nodeStack.emplace_back(iterateNodeId, iterateNode);

        // Go to parent.
        iterateNode = iterateNode->GetParent();
      }

      // Store current found result.
      // Note : We don't cache failed cases, to avoid memory increasement.
      if(isNodeUnderRootNode)
      {
        for(auto&& idPair : nodeStack)
        {
          mTravledNodeMap.insert({idPair.first, idPair.second});
        }
        node = currentNode;
      }

      DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
      {
        oss << "Traveler Check. root[" << mRootNode.GetId() << "] input [" << id << "] valid:" << (isNodeUnderRootNode ? "T" : "F") << " iter count[" << nodeStack.size() << "] cached count[" << mTravledNodeMap.size() << "]";
      });

      DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_TRAVELER_FIND_NEW_NODE", [&](std::ostringstream& oss)
      { oss << "root[" << mRootNode.GetId() << "] input [" << id << "] valid:" << (isNodeUnderRootNode ? "T" : "F") << " iter count[" << nodeStack.size() << "] cached count[" << mTravledNodeMap.size() << "]"; });
    }
  }

  return node;
}

void SceneGraphTraveler::Clear()
{
  mTravledNodeMap.clear();
#if !defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  mTravledNodeMap.rehash(0u); ///< Note : We have to reduce capacity of hash map. Without this line, clear() API will be slow downed.
#endif
  if(!mInvalidated)
  {
    mTravledNodeMap.insert({mRootNode.GetId(), &mRootNode});
  }
}

} // namespace Internal

} // namespace Dali
