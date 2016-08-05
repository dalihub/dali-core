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
#include <dali/internal/render/common/render-item.h>

// INTERNAL INCLUDES
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/renderers/render-renderer.h>

namespace
{
//Memory pool used to allocate new RenderItems. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::RenderItem> gRenderItemPool;
}
namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderItem* RenderItem::New()
{
  return new ( gRenderItemPool.AllocateRaw() ) RenderItem();
}

RenderItem::RenderItem()
: mModelMatrix( false ),
  mModelViewMatrix( false ),
  mSize(),
  mRenderer( NULL ),
  mNode( NULL ),
  mBatchRenderGeometry( NULL ),
  mDepthIndex( 0 ),
  mIsOpaque( true ),
  mBatched( false )
{
}

RenderItem::~RenderItem()
{
}

void RenderItem::operator delete( void* ptr )
{
  gRenderItemPool.Free( static_cast<RenderItem*>( ptr ) );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
