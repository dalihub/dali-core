/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DiscardQueue::DiscardQueue( RenderQueue& renderQueue )
: mRenderQueue( renderQueue )
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
  if ( 0u == updateBufferIndex )
  {
    mNodeQueue0.PushBack( node );
  }
  else
  {
    mNodeQueue1.PushBack( node );
  }
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, NodeAttachment* attachment )
{
  DALI_ASSERT_DEBUG( NULL != attachment );

  // The GL resources will now be freed in Render frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the attachment for destruction in Update frame N+2
  if ( 0u == updateBufferIndex )
  {
    mAttachmentQueue0.PushBack( attachment );
  }
  else
  {
    mAttachmentQueue1.PushBack( attachment );
  }
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, Geometry* geometry )
{
  DALI_ASSERT_DEBUG( NULL != geometry );

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mGeometryQueue0.PushBack( geometry );
  }
  else
  {
    mGeometryQueue1.PushBack( geometry );
  }
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, Material* material )
{
  DALI_ASSERT_DEBUG( NULL != material );

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mMaterialQueue0.PushBack( material );
  }
  else
  {
    mMaterialQueue1.PushBack( material );
  }
}


void DiscardQueue::Add( BufferIndex updateBufferIndex, Shader* shader )
{
  DALI_ASSERT_DEBUG( NULL != shader );

  // Programs are cached for the lifetime of DALi so no need for GL cleanup for shader for now.

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mShaderQueue0.PushBack( shader );
  }
  else
  {
    mShaderQueue1.PushBack( shader );
  }
}

void DiscardQueue::Clear( BufferIndex updateBufferIndex )
{
  // Destroy some discarded objects; these should no longer own any GL resources

  if ( 0u == updateBufferIndex )
  {
    mNodeQueue0.Clear();
    mAttachmentQueue0.Clear();
    mShaderQueue0.Clear();
    mGeometryQueue0.Clear();
    mMaterialQueue0.Clear();
  }
  else
  {
    mNodeQueue1.Clear();
    mAttachmentQueue1.Clear();
    mShaderQueue1.Clear();
    mGeometryQueue1.Clear();
    mMaterialQueue1.Clear();
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
