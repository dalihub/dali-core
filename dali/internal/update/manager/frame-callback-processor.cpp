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

// INTERNAL INCLUDES
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>
#include <dali/internal/update/manager/update-proxy-impl.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace
{

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

  PropertyOwner& mRootNode;
};

} // unnamed namespace

FrameCallbackProcessor::FrameCallbackProcessor( TransformManager& transformManager, Node& rootNode )
: mFrameCallbacks(),
  mTransformManager( transformManager ),
  mRootNode( rootNode )
{
}

FrameCallbackProcessor::~FrameCallbackProcessor()
{
}

void FrameCallbackProcessor::AddFrameCallback( FrameCallbackInterface* frameCallback, const Node* rootNode )
{
  Node& node = const_cast< Node& >( *rootNode ); // Was sent as const from event thread, we need to be able to use non-const version here.

  FrameCallbackInfo info;
  info.frameCallback = frameCallback;
  info.updateProxyImpl = new UpdateProxy( mTransformManager, node );

  // We want to be notified when the node is destroyed (if we're not observing it already)
  auto iter = std::find_if( mFrameCallbacks.begin(), mFrameCallbacks.end(), MatchRootNode< FrameCallbackInfo >( node ) );
  if( iter == mFrameCallbacks.end() )
  {
    node.AddObserver( *this );
  }

  mFrameCallbacks.push_back( info );
}

void FrameCallbackProcessor::RemoveFrameCallback( FrameCallbackInterface* frameCallback )
{
  auto iter = std::remove_if( mFrameCallbacks.begin(), mFrameCallbacks.end(),
                              [ this, frameCallback ]
                                ( const FrameCallbackInfo& info )
                                {
                                  info.updateProxyImpl->GetRootNode().RemoveObserver( *this );
                                  delete info.updateProxyImpl;
                                  return info.frameCallback == frameCallback;
                                } );
  mFrameCallbacks.erase( iter, mFrameCallbacks.end() );
}

void FrameCallbackProcessor::Update( BufferIndex bufferIndex, float elapsedSeconds )
{
  for( auto&& iter : mFrameCallbacks )
  {
    UpdateProxy& updateProxyImpl = *iter.updateProxyImpl;
    updateProxyImpl.SetCurrentBufferIndex( bufferIndex );
    Dali::UpdateProxy updateProxy( updateProxyImpl );
    iter.frameCallback->Update( updateProxy, elapsedSeconds );
  }
}

void FrameCallbackProcessor::PropertyOwnerDestroyed( PropertyOwner& owner )
{
  auto iter = std::remove_if( mFrameCallbacks.begin(), mFrameCallbacks.end(), MatchRootNode< FrameCallbackInfo >( owner )  );
  mFrameCallbacks.erase( iter, mFrameCallbacks.end() );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
