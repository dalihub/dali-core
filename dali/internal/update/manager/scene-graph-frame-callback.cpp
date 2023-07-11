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
#include <dali/internal/update/manager/scene-graph-frame-callback.h>

// INTERNAL INCLUDES
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>
#include <dali/internal/event/update/frame-callback-interface-impl.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
FrameCallback* FrameCallback::New(FrameCallbackInterface& frameCallbackInterface)
{
  return new FrameCallback(&frameCallbackInterface);
}

FrameCallback::~FrameCallback()
{
  if(mUpdateProxy)
  {
    mUpdateProxy->GetRootNode().RemoveObserver(*this);
    mUpdateProxy->AddNodeResetters();
  }

  Invalidate();
}

void FrameCallback::ConnectToSceneGraph(UpdateManager& updateManager, TransformManager& transformManager, Node& rootNode)
{
  mUpdateProxy = std::unique_ptr<UpdateProxy>(new UpdateProxy(updateManager, transformManager, rootNode));
  rootNode.AddObserver(*this);
}

FrameCallback::RequestFlags FrameCallback::Update(BufferIndex bufferIndex, float elapsedSeconds, bool nodeHierarchyChanged)
{
  bool continueCalling = false;
  bool keepRendering   = false;

  if(mUpdateProxy)
  {
    mUpdateProxy->SetCurrentBufferIndex(bufferIndex);

    if(nodeHierarchyChanged)
    {
      mUpdateProxy->NodeHierarchyChanged();
    }

    Mutex::ScopedLock lock(mMutex);
    if(mFrameCallbackInterface && mValid)
    {
      Dali::UpdateProxy updateProxy(*mUpdateProxy);
      keepRendering   = mFrameCallbackInterface->Update(updateProxy, elapsedSeconds);
      continueCalling = true;
    }
  }

  return static_cast<FrameCallback::RequestFlags>(continueCalling | (keepRendering << 1));
}

void FrameCallback::Invalidate()
{
  Mutex::ScopedLock lock(mMutex);
  if(mFrameCallbackInterface && mValid)
  {
    FrameCallbackInterface::Impl::Get(*mFrameCallbackInterface).DisconnectFromSceneGraphObject();
    mValid = false;
    // Do not set mFrameCallbackInterface to nullptr as it is used for comparison checks by the comparison operator
  }
}

void FrameCallback::PropertyOwnerDestroyed(PropertyOwner& owner)
{
  mUpdateProxy.reset(); // Root node is being destroyed so no point keeping the update-proxy either

  Invalidate();
}

FrameCallback::FrameCallback(FrameCallbackInterface* frameCallbackInterface)
: mMutex(),
  mFrameCallbackInterface(frameCallbackInterface)
{
  if(frameCallbackInterface)
  {
    FrameCallbackInterface::Impl::Get(*mFrameCallbackInterface).ConnectToSceneGraphObject(*this);
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
