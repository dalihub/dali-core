#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_LIST_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_LIST_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/devel-api/common/owner-container.h>
#include <dali/public-api/math/rect.h>

#include <dali/graphics-api/graphics-controller.h>
#include <dali/internal/common/owner-key-container.h>
#include <dali/internal/render/common/render-item.h>

namespace Dali
{
using ClippingBox = Rect<int>;

namespace Internal
{
namespace Render
{
class Renderer;
}

namespace SceneGraph
{
class Layer;

using RenderItemContainer = OwnerKeyContainer<RenderItem>;

struct RenderList;
using RenderListContainer = OwnerContainer<RenderList*>;

/**
 * The RenderList structure provides the renderer manager with a list of renderers.
 */
struct RenderList
{
public:
  /**
   * Constructor
   */
  RenderList()
  : mNextFree(0),
    mClippingBox(nullptr),
    mSourceLayer(nullptr),
    mHasColorRenderItems(false)
  {
  }

  /**
   * Destructor
   */
  ~RenderList()
  {
    // Pointer container deletes the render items
    delete mClippingBox;
  }

  /*
   * Copy constructor and assignment operator not defined
   */
  RenderList(const RenderList& rhs) = delete;
  const RenderList& operator=(const RenderList& rhs) = delete;

  /**
   * Reset the render list for next frame
   */
  void Reset()
  {
    // We don't want to delete and re-create the render items every frame
    mNextFree = 0;

    delete mClippingBox;
    mClippingBox = nullptr;
  }

  /**
   * Reserve space in the render list
   * @param size to reserve
   */
  void Reserve(RenderItemContainer::SizeType size)
  {
    mNextFree = 0;
    mItems.Reserve(size);
  }

  /**
   * @return the capacity of the render list
   */
  RenderItemContainer::SizeType Capacity()
  {
    return mItems.Capacity();
  }

  /**
   * Get next free render item
   * @return reference to the next available RenderItem
   */
  RenderItem& GetNextFreeItem()
  {
    // check if we have enough items, we can only be one behind at worst
    if(mItems.Count() <= mNextFree)
    {
      mItems.PushBack(RenderItem::NewKey()); // Push a new empty render item
    }
    // get the item mNextFree points to and increase by one
    RenderItem* item = mItems[mNextFree++].Get();
    return *item;
  }

  /**
   * Get item at a given position in the list
   */
  RenderItem& GetItem(uint32_t index) const
  {
    DALI_ASSERT_DEBUG(index < GetCachedItemCount());
    RenderItem* item = mItems[index].Get();
    return *item;
  }

  RenderItemKey GetItemKey(uint32_t index) const
  {
    DALI_ASSERT_DEBUG(index < GetCachedItemCount());
    return mItems[index];
  }

  /**
   * Get renderer from an item in the list
   */
  Render::RendererKey GetRenderer(uint32_t index) const
  {
    DALI_ASSERT_DEBUG(index < GetCachedItemCount());
    return mItems[index]->mRenderer;
  }

  /**
   * Get the number of real items
   * Because of caching, the actual size may be bit more
   * @return The number of items
   */
  uint32_t Count() const
  {
    return mNextFree;
  }

  /**
   * @return the number of items cached by the list
   */
  uint32_t GetCachedItemCount() const
  {
    return static_cast<uint32_t>(mItems.Count());
  }

  /**
   * Tells the render list to reuse the items from the cache
   */
  void ReuseCachedItems()
  {
    mNextFree = static_cast<uint32_t>(mItems.Count());
  }

  /**
   * Predicate to inform if the list is empty
   */
  bool IsEmpty() const
  {
    return (mNextFree == 0);
  }

  /**
   * Set clipping
   * @param clipping on/off
   * @param box for clipping
   */
  void SetClipping(bool clipping, const ClippingBox& box)
  {
    if(clipping)
    {
      delete mClippingBox;
      mClippingBox = new ClippingBox(box);
    }
  }

  /**
   * @return true if clipping is on
   */
  bool IsClipping() const
  {
    return (nullptr != mClippingBox);
  }

  /**
   * @return the clipping box
   */
  const ClippingBox& GetClippingBox() const
  {
    return *mClippingBox;
  }

  /**
   * @return the container (for sorting)
   */
  RenderItemContainer& GetContainer()
  {
    return mItems;
  }

  /**
   * Do some housekeeping to keep memory consumption low
   */
  void ReleaseUnusedItems()
  {
    // release any non-used RenderItems
    if(mItems.Count() > mNextFree)
    {
      mItems.Resize(mNextFree);
    }
  }

  /**
   * @return the source layer these renderitems originate from
   */
  Layer* GetSourceLayer() const
  {
    return mSourceLayer;
  }

  /**
   * @param layer The layer these RenderItems originate from
   */
  void SetSourceLayer(Layer* layer)
  {
    mSourceLayer = layer;
  }

  /**
   * Set if the RenderList contains color RenderItems
   * @param[in] hasColorRenderItems True if it contains color RenderItems, false otherwise
   */
  void SetHasColorRenderItems(bool hasColorRenderItems)
  {
    mHasColorRenderItems = hasColorRenderItems;
  }

  /**
   * Check if the RenderList contains color RenderItems
   * @return true if the RenderList contains color RenderItems, false otherwise
   */
  bool HasColorRenderItems() const
  {
    return mHasColorRenderItems;
  }

  Graphics::CommandBuffer& GetCommandBuffer(Graphics::Controller& controller)
  {
    if(!mGraphicsCommandBuffer)
    {
      mGraphicsCommandBuffer = controller.CreateCommandBuffer(
        Graphics::CommandBufferCreateInfo().SetLevel(Graphics::CommandBufferLevel::SECONDARY), nullptr);
    }
    return *mGraphicsCommandBuffer.get();
  }

  const Graphics::CommandBuffer* GetCommandBuffer() const
  {
    return mGraphicsCommandBuffer.get();
  }

private:
  RenderItemContainer mItems;    ///< Container of render items
  uint32_t            mNextFree; ///< index for the next free item to use

  mutable Graphics::UniquePtr<Graphics::CommandBuffer> mGraphicsCommandBuffer{nullptr};

  ClippingBox* mClippingBox;             ///< The clipping box, in window coordinates, when clipping is enabled
  Layer*       mSourceLayer;             ///< The originating layer where the renderers are from
  bool         mHasColorRenderItems : 1; ///< True if list contains color render items
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_LIST_H
