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
#include <dali/internal/update/controllers/render-message-dispatcher.h>

// INTERNAL INCLUDES
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/common/message.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderMessageDispatcher::RenderMessageDispatcher( RenderManager& renderManager, RenderQueue& renderQueue, const SceneGraphBuffers& buffers )
: mRenderManager( renderManager ),
  mRenderQueue( renderQueue ),
  mBuffers( buffers )
{
}

RenderMessageDispatcher::~RenderMessageDispatcher()
{
}

void RenderMessageDispatcher::AddRenderer( Renderer& renderer )
{
  typedef MessageValue1< RenderManager, Renderer* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( mBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mRenderManager, &RenderManager::AddRenderer, &renderer );
}

void RenderMessageDispatcher::RemoveRenderer( Renderer& renderer )
{
  typedef MessageValue1< RenderManager, Renderer* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( mBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mRenderManager, &RenderManager::RemoveRenderer, &renderer );
}

void RenderMessageDispatcher::AddGeometry( RenderGeometry& renderGeometry )
{
  typedef MessageValue1< RenderManager, RenderGeometry* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( mBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mRenderManager, &RenderManager::AddGeometry, &renderGeometry );
}

void RenderMessageDispatcher::RemoveGeometry( RenderGeometry& renderGeometry )
{
  typedef MessageValue1< RenderManager, RenderGeometry* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( mBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mRenderManager, &RenderManager::RemoveGeometry, &renderGeometry );
}

void RenderMessageDispatcher::AddPropertyBuffer( RenderGeometry& renderGeometry, PropertyBufferDataProvider* propertyBuffer, GpuBuffer::Target target, GpuBuffer::Usage usage )
{
  typedef MessageValue4< RenderManager, RenderGeometry*, PropertyBufferDataProvider*, GpuBuffer::Target, GpuBuffer::Usage > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( mBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mRenderManager, &RenderManager::AddPropertyBuffer, &renderGeometry, propertyBuffer, target, usage );
}

void RenderMessageDispatcher::RemovePropertyBuffer( RenderGeometry& renderGeometry, PropertyBufferDataProvider* propertyBuffer )
{
  typedef MessageValue2< RenderManager, RenderGeometry*, PropertyBufferDataProvider* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( mBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mRenderManager, &RenderManager::RemovePropertyBuffer, &renderGeometry, propertyBuffer );
}

void RenderMessageDispatcher::AddRenderTracker( RenderTracker& renderTracker )
{
  typedef MessageValue1< RenderManager, RenderTracker* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( mBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mRenderManager, &RenderManager::AddRenderTracker, &renderTracker );
}

void RenderMessageDispatcher::RemoveRenderTracker( RenderTracker& renderTracker )
{
  typedef MessageValue1< RenderManager, RenderTracker* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( mBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mRenderManager, &RenderManager::RemoveRenderTracker, &renderTracker );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
