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
#include <dali/internal/update/common/discard-queue.h>

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/rendering/scene-graph-shader.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DiscardQueue::DiscardQueue()
: mNodeQueue(),
  mShaderQueue(),
  mRendererQueue(),
  mCameraQueue()
{
}

DiscardQueue::~DiscardQueue()
{
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, Node* node )
{
  DALI_ASSERT_DEBUG( NULL != node );

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  mNodeQueue[ updateBufferIndex ].PushBack( node );
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, Shader* shader )
{
  DALI_ASSERT_DEBUG( NULL != shader );

  // Programs are cached for the lifetime of DALi so no need for GL cleanup for shader for now.

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  mShaderQueue[ updateBufferIndex ].PushBack( shader );
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, Renderer* renderer )
{
  DALI_ASSERT_DEBUG( NULL != renderer );

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  mRendererQueue[ updateBufferIndex ].PushBack( renderer );
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, Camera* camera )
{
  DALI_ASSERT_DEBUG( NULL != camera );

  mCameraQueue[ updateBufferIndex ].PushBack( camera );
}

void DiscardQueue::Clear( BufferIndex updateBufferIndex )
{
  // Destroy some discarded objects; these should no longer own any GL resources
  mNodeQueue[ updateBufferIndex ].Clear();
  mShaderQueue[ updateBufferIndex ].Clear();
  mRendererQueue[ updateBufferIndex ].Clear();
  mCameraQueue[ updateBufferIndex ].Clear();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
