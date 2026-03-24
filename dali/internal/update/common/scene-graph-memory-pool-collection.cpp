/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/common/scene-graph-memory-pool-collection.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLDUES
#include <dali/internal/common/memory-pool-object-allocator.h>

#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>

#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-texture.h>
#include <dali/internal/render/renderers/uniform-buffer-view.h>

namespace Dali::Internal::SceneGraph
{
namespace
{
constexpr uint32_t INITIAL_CAMERA_BLOCK_CAPACITY           = 4u;
constexpr uint32_t INITIAL_RENDER_TASK_LIST_BLOCK_CAPACITY = 4u;
constexpr uint32_t INITIAL_UBO_BLOCK_CAPACITY              = 4u;
} //namespace
/**
 * Structure to contain MemoryPoolCollection internal data
 */
struct MemoryPoolCollection::Impl
{
  Impl()  = default;
  ~Impl() = default;

  // UpdateManager
  Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Animation>      mAnimationMemoryPool;
  Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Camera>         mCameraMemoryPool{INITIAL_CAMERA_BLOCK_CAPACITY};
  Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Node>           mNodeMemoryPool;
  Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Renderer>       mRendererMemoryPool;
  Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::RenderItem>     mRenderItemMemoryPool;
  Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::RenderTaskList> mRenderTaskListMemoryPool{INITIAL_RENDER_TASK_LIST_BLOCK_CAPACITY};
  Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::TextureSet>     mTextureSetMemoryPool;

  // RenderManager
  Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::Render::Renderer>          mRenderRendererMemoryPool;
  Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::Render::Texture>           mRenderTextureMemoryPool;
  Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::Render::UniformBufferView> mRenderUboViewMemoryPool{INITIAL_UBO_BLOCK_CAPACITY};
};

MemoryPoolCollection::MemoryPoolCollection()
: mImpl(new Impl())
{
}

MemoryPoolCollection::~MemoryPoolCollection() = default;

void* MemoryPoolCollection::AllocateRaw(MemoryPoolCollection::MemoryPoolType type)
{
  switch(type)
  {
    case MemoryPoolCollection::MemoryPoolType::RENDER_ITEM:
    {
      return mImpl->mRenderItemMemoryPool.AllocateRaw();
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_UBO_VIEW:
    {
      return mImpl->mRenderUboViewMemoryPool.AllocateRaw();
    }
    default:
    {
      DALI_ASSERT_ALWAYS(0 && "Invalid memory pool type AllocateRaw!");
      return nullptr;
    }
  }
}

void* MemoryPoolCollection::AllocateRawThreadSafe(MemoryPoolCollection::MemoryPoolType type)
{
  switch(type)
  {
    case MemoryPoolCollection::MemoryPoolType::ANIMATION:
    {
      return mImpl->mAnimationMemoryPool.AllocateRawThreadSafe();
    }
    case MemoryPoolCollection::MemoryPoolType::CAMERA:
    {
      return mImpl->mCameraMemoryPool.AllocateRawThreadSafe();
    }
    case MemoryPoolCollection::MemoryPoolType::NODE:
    {
      return mImpl->mNodeMemoryPool.AllocateRawThreadSafe();
    }
    case MemoryPoolCollection::MemoryPoolType::RENDERER:
    {
      return mImpl->mRendererMemoryPool.AllocateRawThreadSafe();
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_TASK_LIST:
    {
      return mImpl->mRenderTaskListMemoryPool.AllocateRawThreadSafe();
    }
    case MemoryPoolCollection::MemoryPoolType::TEXTURE_SET:
    {
      return mImpl->mTextureSetMemoryPool.AllocateRawThreadSafe();
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_RENDERER:
    {
      return mImpl->mRenderRendererMemoryPool.AllocateRawThreadSafe();
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_TEXTURE:
    {
      return mImpl->mRenderTextureMemoryPool.AllocateRawThreadSafe();
    }
    default:
    {
      DALI_ASSERT_ALWAYS(0 && "Invalid memory pool type AllocateRawThreadSafe!");
      return nullptr;
    }
  }
}
void MemoryPoolCollection::Free(MemoryPoolCollection::MemoryPoolType type, void* object)
{
  switch(type)
  {
    case MemoryPoolCollection::MemoryPoolType::RENDER_ITEM:
    {
      mImpl->mRenderItemMemoryPool.Free(static_cast<Dali::Internal::SceneGraph::RenderItem*>(object));
      break;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_UBO_VIEW:
    {
      mImpl->mRenderUboViewMemoryPool.Free(static_cast<Dali::Internal::Render::UniformBufferView*>(object));
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(0 && "Invalid memory pool type Free!");
    }
  }
}

void MemoryPoolCollection::FreeThreadSafe(MemoryPoolCollection::MemoryPoolType type, void* object)
{
  switch(type)
  {
    case MemoryPoolCollection::MemoryPoolType::ANIMATION:
    {
      mImpl->mAnimationMemoryPool.FreeThreadSafe(static_cast<Dali::Internal::SceneGraph::Animation*>(object));
      break;
    }
    case MemoryPoolCollection::MemoryPoolType::CAMERA:
    {
      mImpl->mCameraMemoryPool.FreeThreadSafe(static_cast<Dali::Internal::SceneGraph::Camera*>(object));
      break;
    }
    case MemoryPoolCollection::MemoryPoolType::NODE:
    {
      mImpl->mNodeMemoryPool.FreeThreadSafe(static_cast<Dali::Internal::SceneGraph::Node*>(object));
      break;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDERER:
    {
      mImpl->mRendererMemoryPool.FreeThreadSafe(static_cast<Dali::Internal::SceneGraph::Renderer*>(object));
      break;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_TASK_LIST:
    {
      mImpl->mRenderTaskListMemoryPool.FreeThreadSafe(static_cast<Dali::Internal::SceneGraph::RenderTaskList*>(object));
      break;
    }
    case MemoryPoolCollection::MemoryPoolType::TEXTURE_SET:
    {
      mImpl->mTextureSetMemoryPool.FreeThreadSafe(static_cast<Dali::Internal::SceneGraph::TextureSet*>(object));
      break;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_RENDERER:
    {
      mImpl->mRenderRendererMemoryPool.FreeThreadSafe(static_cast<Dali::Internal::Render::Renderer*>(object));
      break;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_TEXTURE:
    {
      mImpl->mRenderTextureMemoryPool.FreeThreadSafe(static_cast<Dali::Internal::Render::Texture*>(object));
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(0 && "Invalid memory pool type FreeThreadSafe!");
    }
  }
}

void* MemoryPoolCollection::GetPtrFromKey(MemoryPoolCollection::MemoryPoolType type, MemoryPoolInterface::KeyType key)
{
  switch(type)
  {
    case MemoryPoolCollection::MemoryPoolType::RENDERER:
    {
      return mImpl->mRendererMemoryPool.GetPtrFromKey(key);
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_ITEM:
    {
      return mImpl->mRenderItemMemoryPool.GetPtrFromKey(key);
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_RENDERER:
    {
      return mImpl->mRenderRendererMemoryPool.GetPtrFromKey(key);
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_TEXTURE:
    {
      return mImpl->mRenderTextureMemoryPool.GetPtrFromKey(key);
    }
    default:
    {
      DALI_ASSERT_ALWAYS(0 && "Invalid memory pool type GetPtrFromKey!");
      return nullptr;
    }
  }
}

MemoryPoolInterface::KeyType MemoryPoolCollection::GetKeyFromPtr(MemoryPoolCollection::MemoryPoolType type, void* ptr)
{
  switch(type)
  {
    case MemoryPoolCollection::MemoryPoolType::RENDERER:
    {
      return mImpl->mRendererMemoryPool.GetKeyFromPtr(static_cast<Dali::Internal::SceneGraph::Renderer*>(ptr));
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_ITEM:
    {
      return mImpl->mRenderItemMemoryPool.GetKeyFromPtr(static_cast<Dali::Internal::SceneGraph::RenderItem*>(ptr));
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_RENDERER:
    {
      return mImpl->mRenderRendererMemoryPool.GetKeyFromPtr(static_cast<Dali::Internal::Render::Renderer*>(ptr));
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_TEXTURE:
    {
      return mImpl->mRenderTextureMemoryPool.GetKeyFromPtr(static_cast<Dali::Internal::Render::Texture*>(ptr));
    }
    default:
    {
      DALI_ASSERT_ALWAYS(0 && "Invalid memory pool type GetKeyFromPtr!");
      return MemoryPoolInterface::KeyType();
    }
  }
}

void MemoryPoolCollection::GetCapacity(MemoryPoolCollection::MemoryPoolType type, uint32_t& cap, uint32_t& size) const
{
  switch(type)
  {
    case MemoryPoolCollection::MemoryPoolType::ANIMATION:
    {
      mImpl->mAnimationMemoryPool.GetCapacity(cap, size);
      return;
    }
    case MemoryPoolCollection::MemoryPoolType::CAMERA:
    {
      mImpl->mCameraMemoryPool.GetCapacity(cap, size);
      return;
    }
    case MemoryPoolCollection::MemoryPoolType::NODE:
    {
      mImpl->mNodeMemoryPool.GetCapacity(cap, size);
      return;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDERER:
    {
      mImpl->mRendererMemoryPool.GetCapacity(cap, size);
      return;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_ITEM:
    {
      mImpl->mRenderItemMemoryPool.GetCapacity(cap, size);
      return;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_TASK_LIST:
    {
      mImpl->mRenderTaskListMemoryPool.GetCapacity(cap, size);
      return;
    }
    case MemoryPoolCollection::MemoryPoolType::TEXTURE_SET:
    {
      mImpl->mTextureSetMemoryPool.GetCapacity(cap, size);
      return;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_RENDERER:
    {
      mImpl->mRenderRendererMemoryPool.GetCapacity(cap, size);
      return;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_TEXTURE:
    {
      mImpl->mRenderTextureMemoryPool.GetCapacity(cap, size);
      return;
    }
    case MemoryPoolCollection::MemoryPoolType::RENDER_UBO_VIEW:
    {
      mImpl->mRenderUboViewMemoryPool.GetCapacity(cap, size);
      return;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(0 && "Invalid memory pool type capacity");
      return;
    }
  }
}

template<typename T>
std::string SizeConvertor(const Dali::Internal::MemoryPoolObjectAllocator<T>& pool)
{
  uint32_t cap, size;
  pool.GetCapacity(cap, size);
  std::ostringstream oss;
  oss.width(12);
  oss << std::right << cap << " byte (";
  oss.width(9);
  oss << ((cap + (1 << 9)) >> 10) << " KB) (";
  oss.width(4);
  oss << TypeSizeWithAlignment<T>::size << " byte * " << (cap / TypeSizeWithAlignment<T>::size) << " items] ";
  oss.width(3);
  oss << (size * 100 / cap) << "%\n";
  return oss.str();
}

std::string MemoryPoolCollection::LogPools() const
{
  std::ostringstream oss;
  oss << "Animations:        " << SizeConvertor<Dali::Internal::SceneGraph::Animation>(mImpl->mAnimationMemoryPool) << std::endl
      << "RenderItems:       " << SizeConvertor<Dali::Internal::SceneGraph::RenderItem>(mImpl->mRenderItemMemoryPool) << std::endl
      << "Renderers:         " << SizeConvertor<Dali::Internal::SceneGraph::Renderer>(mImpl->mRendererMemoryPool) << std::endl
      << "TextureSets:       " << SizeConvertor<Dali::Internal::SceneGraph::TextureSet>(mImpl->mTextureSetMemoryPool) << std::endl
      << "RenderTasks:       " << SizeConvertor<Dali::Internal::SceneGraph::RenderTaskList>(mImpl->mRenderTaskListMemoryPool) << std::endl
      << "Nodes:             " << SizeConvertor<Dali::Internal::SceneGraph::Node>(mImpl->mNodeMemoryPool) << std::endl
      << "Cameras:           " << SizeConvertor<Dali::Internal::SceneGraph::Camera>(mImpl->mCameraMemoryPool) << std::endl
      << "Render::Renderers: " << SizeConvertor<Dali::Internal::Render::Renderer>(mImpl->mRenderRendererMemoryPool) << std::endl
      << "Render::Textures:  " << SizeConvertor<Dali::Internal::Render::Texture>(mImpl->mRenderTextureMemoryPool) << std::endl
      << "Render::UboViews:  " << SizeConvertor<Dali::Internal::Render::UniformBufferView>(mImpl->mRenderUboViewMemoryPool) << std::endl;
  return oss.str();
}

} // namespace Dali::Internal::SceneGraph
