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
#include <dali/internal/update/manager/frame-callback-processor.h>

// EXTERNAL INCLUDES
#include <algorithm>

#ifdef TRACE_ENABLED
#include <chrono>
#include <cmath>
#include <sstream> ///< for std::ostringstream
#endif

// INTERNAL INCLUDES
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/update/manager/global-scene-graph-traveler.h>
#include <dali/internal/update/manager/scene-graph-traveler.h>

namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);

#ifdef TRACE_ENABLED
uint64_t GetNanoseconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch = std::chrono::steady_clock::now().time_since_epoch();

  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);

  return static_cast<uint64_t>(duration.count());
}
#endif
} // namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
FrameCallbackProcessor::FrameCallbackProcessor(UpdateManager& updateManager, TransformManager& transformManager)
: mFrameCallbacks(),
  mUpdateManager(updateManager),
  mTransformManager(transformManager),
  mRootNodeTravelerMap{},
  mNodeHierarchyChanged(true)
{
}

FrameCallbackProcessor::~FrameCallbackProcessor() = default;

void FrameCallbackProcessor::AddFrameCallback(OwnerPointer<FrameCallback>& frameCallback, const Node* rootNode)
{
  // We allow to input root node as nullptr.
  // In this case, let we use global scene graph traveler, instead of node traveler
  if(rootNode)
  {
    Node& node = const_cast<Node&>(*rootNode); // Was sent as const from event thread, we need to be able to use non-const version here.

    auto traveler = GetSceneGraphTraveler(&node);

    frameCallback->ConnectToSceneGraph(mUpdateManager, mTransformManager, node, traveler);
  }
  else
  {
    if(!mGlobalTraveler)
    {
      mGlobalTraveler = new GlobalSceneGraphTraveler(mUpdateManager);
    }
    frameCallback->ConnectToSceneGraph(mUpdateManager, mTransformManager, mGlobalTraveler);
  }

  mFrameCallbacks.emplace_back(frameCallback);
}

void FrameCallbackProcessor::RemoveFrameCallback(FrameCallbackInterface* frameCallback)
{
  // Find and remove all frame-callbacks that use the given frame-callback-interface
  auto iter = std::remove(mFrameCallbacks.begin(), mFrameCallbacks.end(), frameCallback);
  mFrameCallbacks.erase(iter, mFrameCallbacks.end());
}

void FrameCallbackProcessor::NotifyFrameCallback(FrameCallbackInterface* frameCallback, Dali::UpdateProxy::NotifySyncPoint syncPoint)
{
  // Ensure that frame callback is still valid before sending notification
  auto iter = std::find(mFrameCallbacks.begin(), mFrameCallbacks.end(), frameCallback);
  if(iter != mFrameCallbacks.end())
  {
    (*iter)->Notify(syncPoint);
  }
}

bool FrameCallbackProcessor::Update(BufferIndex bufferIndex, float elapsedSeconds)
{
  bool keepRendering = false;

  if(mNodeHierarchyChanged && !mRootNodeTravelerMap.empty())
  {
    DALI_LOG_DEBUG_INFO("Node hierarchy changed. Update traveler map\n");
    // Clear node traveler
    for(auto iter = mRootNodeTravelerMap.begin(); iter != mRootNodeTravelerMap.end();)
    {
      // We don't need to erase invalidated traveler always. Just erase now.
      // Note : ReferenceCount == 1 mean, no frame callbacks use this traveler now. We can erase it.
      if(iter->second->IsInvalidated() || iter->second->ReferenceCount() == 1u)
      {
        iter = mRootNodeTravelerMap.erase(iter);
      }
      else
      {
        if(iter->first->IsDescendentHierarchyChanged())
        {
          iter->second->NodeHierarchyChanged();
        }
        ++iter;
      }
    }
  }

  if(!mFrameCallbacks.empty())
  {
#ifdef TRACE_ENABLED
    std::vector<std::pair<uint64_t, uint32_t>> frameCallbackTimeChecker;

    uint32_t frameIndex = 0u;
    uint64_t start      = 0u;
    uint64_t end        = 0u;
#endif

    DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_FRAME_CALLBACK_UPDATE", [&](std::ostringstream& oss) {
      oss << "[" << mFrameCallbacks.size() << "]";
    });

    // If any of the FrameCallback::Update calls returns false, then they are no longer required & can be removed.
    auto iter = std::remove_if(
      mFrameCallbacks.begin(), mFrameCallbacks.end(), [&](OwnerPointer<FrameCallback>& frameCallback) {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          start = GetNanoseconds();
        }
#endif
        FrameCallback::RequestFlags requests = frameCallback->Update(bufferIndex, elapsedSeconds, mNodeHierarchyChanged);
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          end = GetNanoseconds();
          frameCallbackTimeChecker.emplace_back(end - start, ++frameIndex);
        }
#endif
        keepRendering |= (requests & FrameCallback::KEEP_RENDERING);
        return (requests & FrameCallback::CONTINUE_CALLING) == 0;
      });
    mFrameCallbacks.erase(iter, mFrameCallbacks.end());

    DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_FRAME_CALLBACK_UPDATE", [&](std::ostringstream& oss) {
      oss << "[" << mFrameCallbacks.size() << ",";

      std::sort(frameCallbackTimeChecker.rbegin(), frameCallbackTimeChecker.rend());
      auto topCount = std::min(5u, static_cast<uint32_t>(frameCallbackTimeChecker.size()));

      oss << "top" << topCount;
      for(auto i = 0u; i < topCount; ++i)
      {
        oss << "(" << static_cast<float>(frameCallbackTimeChecker[i].first) / 1000000.0f << "ms,";
        oss << frameCallbackTimeChecker[i].second << ")";
      }
      oss << "]";
    });
  }

  mNodeHierarchyChanged = false;

  return keepRendering;
}

SceneGraphTravelerPtr FrameCallbackProcessor::GetSceneGraphTraveler(Node* rootNode)
{
  auto iter = mRootNodeTravelerMap.find(rootNode);

  if(iter != mRootNodeTravelerMap.end())
  {
    // Check wheter traveler is invalidated or not
    if(!iter->second->IsInvalidated())
    {
      return iter->second;
    }
    else
    {
      mRootNodeTravelerMap.erase(iter);
    }
  }

  // Create new traveler and keep it.
  return (mRootNodeTravelerMap.insert({rootNode, new SceneGraphTraveler(mUpdateManager, *rootNode)})).first->second;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
