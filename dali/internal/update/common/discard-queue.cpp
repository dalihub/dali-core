/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>
#include <dali/internal/update/manager/geometry-batcher.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DiscardQueue::DiscardQueue( RenderQueue& renderQueue )
: mRenderQueue( renderQueue ),
  mNodeQueue(),
  mShaderQueue(),
  mRendererQueue(),
  mCameraQueue(),
  mGeometryBatcher( NULL )
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

  // If batching, then mark corresponding batch to be destroyed too
  if( node->GetIsBatchParent() )
  {
    mGeometryBatcher->RemoveBatchParent( node );
  }
  else if( node->GetBatchParent() )
  {
    if( node->mBatchIndex != BATCH_NULL_HANDLE )
    {
      mGeometryBatcher->RemoveNode( node );
    }
  }

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

void DiscardQueue::SetGeometryBatcher( GeometryBatcher* geometryBatcher )
{
  mGeometryBatcher = geometryBatcher;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
