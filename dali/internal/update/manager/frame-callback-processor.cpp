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

namespace
{

/**
 * Given a node, it matches all update-proxies using that node as their root-node.
 */
template< typename FrameCallbackInfoT >
class MatchRootNode
{
public:

  MatchRootNode( PropertyOwner& rootNode )
  : mRootNode( rootNode )
  {
  }

  bool operator() ( const FrameCallbackInfoT& info )
  {
    return &info.updateProxyImpl->GetRootNode() == &mRootNode;
  }

private:

  const PropertyOwner& mRootNode;
};

} // unnamed namespace

FrameCallbackProcessor::FrameCallbackProcessor( TransformManager& transformManager, Node& rootNode )
: mFrameCallbacks(),
  mTransformManager( transformManager ),
  mRootNode( rootNode ),
  mNodeHierarchyChanged( true )
{
}

FrameCallbackProcessor::~FrameCallbackProcessor()
{
}

void FrameCallbackProcessor::AddFrameCallback( FrameCallbackInterface* frameCallback, const Node* rootNode )
{
  Node& node = const_cast< Node& >( *rootNode ); // Was sent as const from event thread, we need to be able to use non-const version here.

  // We want to be notified when the node is destroyed (if we're not observing it already)
  auto iter = std::find_if( mFrameCallbacks.begin(), mFrameCallbacks.end(), MatchRootNode< FrameCallbackInfo >( node ) );
  if( iter == mFrameCallbacks.end() )
  {
    node.AddObserver( *this );
  }

  mFrameCallbacks.emplace_back( FrameCallbackInfo( frameCallback , new UpdateProxy( mTransformManager, node ) ) );
}

void FrameCallbackProcessor::RemoveFrameCallback( FrameCallbackInterface* frameCallback )
{
  std::vector< SceneGraph::Node* > nodesToStopObserving;

  // Find and remove all matching frame-callbacks
  auto iter =
    std::remove_if( mFrameCallbacks.begin(), mFrameCallbacks.end(),
                    [ frameCallback, &nodesToStopObserving ] ( FrameCallbackInfo& info )
                    {
                      bool match = false;
                      if( info.frameCallback == frameCallback )
                      {
                        nodesToStopObserving.push_back( &info.updateProxyImpl->GetRootNode() );
                        match = true;
                      }
                      return match;
                    } );
  mFrameCallbacks.erase( iter, mFrameCallbacks.end() );

  // Only stop observing the removed frame-callback nodes if none of the other frame-callbacks use them as root nodes
  for( auto&& node : nodesToStopObserving )
  {
    auto nodeMatchingIter = std::find_if( mFrameCallbacks.begin(), mFrameCallbacks.end(), MatchRootNode< FrameCallbackInfo >( *node ) );
    if( nodeMatchingIter == mFrameCallbacks.end() )
    {
      node->RemoveObserver( *this );
    }
  }
}

void FrameCallbackProcessor::Update( BufferIndex bufferIndex, float elapsedSeconds )
{
  for( auto&& iter : mFrameCallbacks )
  {
    UpdateProxy& updateProxyImpl = *iter.updateProxyImpl;
    updateProxyImpl.SetCurrentBufferIndex( bufferIndex );

    if( mNodeHierarchyChanged )
    {
      updateProxyImpl.NodeHierarchyChanged();
    }

    Dali::UpdateProxy updateProxy( updateProxyImpl );
    iter.frameCallback->Update( updateProxy, elapsedSeconds );
  }
  mNodeHierarchyChanged = false;
}

void FrameCallbackProcessor::PropertyOwnerDestroyed( PropertyOwner& owner )
{
  auto iter = std::remove_if( mFrameCallbacks.begin(), mFrameCallbacks.end(), MatchRootNode< FrameCallbackInfo >( owner )  );
  mFrameCallbacks.erase( iter, mFrameCallbacks.end() );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
