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
 *
 */

// CLASS HEADER
#include <dali/internal/update/manager/frame-callback-processor.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

FrameCallbackProcessor::FrameCallbackProcessor( TransformManager& transformManager )
: mFrameCallbacks(),
  mTransformManager( transformManager ),
  mNodeHierarchyChanged( true )
{
}

FrameCallbackProcessor::~FrameCallbackProcessor()
{
}

void FrameCallbackProcessor::AddFrameCallback( OwnerPointer< FrameCallback >& frameCallback, const Node* rootNode )
{
  Node& node = const_cast< Node& >( *rootNode ); // Was sent as const from event thread, we need to be able to use non-const version here.

  frameCallback->ConnectToSceneGraph( mTransformManager, node );

  mFrameCallbacks.emplace_back( frameCallback );
}

void FrameCallbackProcessor::RemoveFrameCallback( FrameCallbackInterface* frameCallback )
{
  // Find and remove all frame-callbacks that use the given frame-callback-interface
  auto iter = std::remove( mFrameCallbacks.begin(), mFrameCallbacks.end(), frameCallback );
  mFrameCallbacks.erase( iter, mFrameCallbacks.end() );
}

void FrameCallbackProcessor::Update( BufferIndex bufferIndex, float elapsedSeconds )
{
  // If any of the FrameCallback::Update calls returns false, then they are no longer required & can be removed.
  auto iter = std::remove_if(
    mFrameCallbacks.begin(), mFrameCallbacks.end(),
    [ & ]( OwnerPointer< FrameCallback >& frameCallback )
    {
      return ! frameCallback->Update( bufferIndex, elapsedSeconds, mNodeHierarchyChanged );
    }
  );
  mFrameCallbacks.erase( iter, mFrameCallbacks.end() );

  mNodeHierarchyChanged = false;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
