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

// CLASS HEADER
#include <dali/internal/update/manager/frame-callback-processor.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);
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
  mTravelerMap{},
  mNodeHierarchyChanged(true)
{
}

FrameCallbackProcessor::~FrameCallbackProcessor() = default;

void FrameCallbackProcessor::AddFrameCallback(OwnerPointer<FrameCallback>& frameCallback, const Node* rootNode)
{
  Node& node = const_cast<Node&>(*rootNode); // Was sent as const from event thread, we need to be able to use non-const version here.

  SceneGraphTravelerPtr traveler = GetSceneGraphTraveler(&node);

  frameCallback->ConnectToSceneGraph(mUpdateManager, mTransformManager, node, traveler);

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

  if(mNodeHierarchyChanged)
  {
    DALI_LOG_DEBUG_INFO("Node hierarchy changed. Update traveler map\n");
    // Clear node traveler
    for(auto iter = mTravelerMap.begin(); iter != mTravelerMap.end();)
    {
      // We don't need to erase invalidated traveler always. Just erase now.
      // Note : ReferenceCount == 1 mean, no frame callbacks use this traveler now. We can erase it.
      if(iter->second->IsInvalidated() || iter->second->ReferenceCount() == 1u)
      {
        iter = mTravelerMap.erase(iter);
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
    DALI_TRACE_SCOPE(gTraceFilter, "DALI_FRAME_CALLBACK_UPDATE");

    // If any of the FrameCallback::Update calls returns false, then they are no longer required & can be removed.
    auto iter = std::remove_if(
      mFrameCallbacks.begin(), mFrameCallbacks.end(), [&](OwnerPointer<FrameCallback>& frameCallback) {
        FrameCallback::RequestFlags requests = frameCallback->Update(bufferIndex, elapsedSeconds, mNodeHierarchyChanged);
        keepRendering |= (requests & FrameCallback::KEEP_RENDERING);
        return (requests & FrameCallback::CONTINUE_CALLING) == 0;
      });
    mFrameCallbacks.erase(iter, mFrameCallbacks.end());
  }

  mNodeHierarchyChanged = false;

  return keepRendering;
}

SceneGraphTravelerPtr FrameCallbackProcessor::GetSceneGraphTraveler(Node* rootNode)
{
  auto iter = mTravelerMap.find(rootNode);

  if(iter != mTravelerMap.end())
  {
    // Check wheter traveler is invalidated or not
    if(!iter->second->IsInvalidated())
    {
      return iter->second;
    }
    else
    {
      mTravelerMap.erase(iter);
    }
  }

  // Create new traveler and keep it.
  return (mTravelerMap.insert({rootNode, new SceneGraphTraveler(*rootNode)})).first->second;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
