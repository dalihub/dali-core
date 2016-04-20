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
#include <dali/internal/render/renderers/render-renderer.h>
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

void DiscardQueue::Add( BufferIndex updateBufferIndex, Renderer* renderer )
{
  DALI_ASSERT_DEBUG( NULL != renderer );

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mRendererQueue0.PushBack( renderer );
  }
  else
  {
    mRendererQueue1.PushBack( renderer );
  }
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, TextureSet* textureSet )
{
  DALI_ASSERT_DEBUG( NULL != textureSet );

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mTextureSetQueue0.PushBack( textureSet );
  }
  else
  {
    mTextureSetQueue1.PushBack( textureSet );
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
    mShaderQueue0.Clear();
    mTextureSetQueue0.Clear();
    mRendererQueue0.Clear();
  }
  else
  {
    mNodeQueue1.Clear();
    mShaderQueue1.Clear();
    mTextureSetQueue1.Clear();
    mRendererQueue1.Clear();
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
