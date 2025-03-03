#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES

#include <dali/internal/render/common/render-item-key.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/matrix.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
class Renderer;
}

namespace SceneGraph
{
/**
 * A RenderItem contains all the data needed for rendering
 */
struct RenderItem
{
  /**
   * Construct a new RenderItem
   * @return A pointer to a new RenderItem.
   */
  static RenderItem* New();

  /**
   * Construct a new RenderItem
   * @return A key to a new RenderItem
   */
  static RenderItemKey NewKey();

  /**
   * Clear memory pool of render item.
   * This should be called at the begin of Core.
   * (Since Core could be recreated, we need to reset the memory pool.)
   * After this API call, all SceneGraph::RenderItem classes are invalid.
   */
  static void ResetMemoryPool();

  /**
   * Non-virtual destructor; RenderItem is not suitable as a base class.
   */
  ~RenderItem();

  /**
   * Get a pointer to the given object in the associated memory pool.
   * @param[in] key A key to the memory pool object
   * @return a ptr to the given object, or nullptr if not found/invalid
   */
  static RenderItem* Get(RenderItemKey::KeyType key);

  /**
   * Get the key of the given renderer in the associated memory pool.
   * @param[in] renderer the given renderer
   * @return The key in the associated memory pool.
   */
  static RenderItemKey GetKey(const RenderItem& renderer);

  /**
   * Get the key of the given renderer in the associated memory pool.
   * @param[in] renderer the given renderer
   * @return The key in the associated memory pool, or -1 if not
   * found.
   */
  static RenderItemKey GetKey(RenderItem* renderer);

  /**
   * Produce a 2D AABB in transformed space
   * See below for caveats.
   *
   * @param[in]    transformMatrix   The matrix for converting to a different space
   * @param[in]    position          The center position of the render item
   * @param[in]    size              The size of the render item
   */
  static ClippingBox CalculateTransformSpaceAABB(const Matrix& transformMatrix, const Vector3& position, const Vector3& size);

  /**
   * @brief This method is an optimized calculation of a viewport-space AABB (Axis-Aligned-Bounding-Box).
   *
   * We use the model-view-matrix, but we do not use projection. Therefore we assume Z = 0.
   * As the box is Axis-Aligned (in viewport space) rotations on Z axis are correctly delt with by expanding the box as necessary.
   * Rotations on X & Y axis will resize the AABB, but it will not handle the projection error due to the new coordinates having non-zero Z values.
   *
   * Note: We pass in the viewport dimensions rather than allow the caller to modify the raw AABB in order to optimally generate the final result.
   *
   * Note: ASSUMES THAT THE VIEWPORT COVERS THE SCREEN AND THAT THE CANVAS SIZE AND VIEWPORT SIZE ARE THE SAME!!!!!  (Not the case for magnifier)
   *
   * @param[in]    modelViewMatrix   The model view matrix
   * @param[in]    position          The center position of the render item
   * @param[in]    size              The size of the render item
   * @param[in]    viewportWidth     The width of the viewport to calculate for
   * @param[in]    viewportHeight    The height of the viewport to calculate for
   * @return                         The AABB coordinates in viewport-space (x, y, width, height)
   */
  static ClippingBox CalculateViewportSpaceAABB(const Matrix& modelViewMatrix, const Vector3& position, const Vector3& size, const int viewportWidth, const int viewportHeight);

  /**
   * @brief Returns true if this node/renderer uses the depth buffer (read or write)
   * @param[in] depthTestEnabled True if depth testing is currently on
   * @return true if this node/renderer pair uses the depth buffer
   */
  bool UsesDepthBuffer(bool depthTestEnabled);

  /**
   * @brief Returns true if this node/renderer uses the stencil buffer (read or write)
   * @return true if this node/renderer pair uses the stencil buffer
   */
  bool UsesStencilBuffer() const;

  /**
   * @brief Get PartialRenderingData::NodeInfomations from node
   * @return Node infomations from node's partial rendering data.
   */
  const PartialRenderingData::NodeInfomations& GetPartialRenderingDataNodeInfomations() const
  {
    return mNode->GetPartialRenderingData().mNodeInfomations;
  }

  /**
   * Overriden delete operator.
   * Deletes the RenderItem from its global memory pool
   * @param[in] A pointer to the RenderItem to delete.
   */
  void operator delete(void* ptr);

  Matrix              mModelViewMatrix;
  Render::RendererKey mRenderer;
  const Node*         mNode;
  const void*         mTextureSet; ///< Used for sorting only
  int                 mDepthIndex;

  bool mIsOpaque : 1;
  bool mIsUpdated : 1;

  /**
   * Get the capacity of the global pool.
   */
  static uint32_t GetMemoryPoolCapacity();

  /**
   * @return true if this render item uses the depth buffer (reads or writes to it)
   */
  bool UsesDepthBuffer();

private:
  /**
   * Private constructor. See RenderItem::New
   */
  RenderItem();

  // RenderItems should not be copied as they are heavy
  RenderItem(const RenderItem& item) = delete;
  RenderItem& operator=(const RenderItem& item) = delete;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H
