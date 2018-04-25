#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/math/matrix.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali
{

namespace Internal
{

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
   * Non-virtual destructor; RenderItem is not suitable as a base class.
   */
  ~RenderItem();

  /**
   * @brief This method is an optimized calculation of a viewport-space AABB (Axis-Aligned-Bounding-Box).
   *
   * We use the model-view-matrix, but we do not use projection. Therefore we assume Z = 0.
   * As the box is Axis-Aligned (in viewport space) rotations on Z axis are correctly delt with by expanding the box as necessary.
   * Rotations on X & Y axis will resize the AABB, but it will not handle the projection error due to the new coordinates having non-zero Z values.
   *
   * Note: We pass in the viewport dimensions rather than allow the caller to modify the raw AABB in order to optimally generate the final result.
   *
   * @param[in]    viewportWidth     The width of the viewport to calculate for
   * @param[in]    viewportHeight    The height of the viewport to calculate for
   * @return                         The AABB coordinates in viewport-space (x, y, width, height)
   */
  ClippingBox CalculateViewportSpaceAABB( const int viewportWidth, const int viewportHeight ) const;

  /**
   * Overriden delete operator.
   * Deletes the RenderItem from its global memory pool
   * @param[in] A pointer to the RenderItem to delete.
   */
  void operator delete( void* ptr );

  Matrix            mModelMatrix;
  Matrix            mModelViewMatrix;
  Vector3           mSize;
  Node*             mNode;
  const void*       mTextureSet;        //< Used for sorting only
  int               mDepthIndex;
  bool              mIsOpaque:1;

private:

  /**
   * Private constructor. See RenderItem::New
   */
  RenderItem();

  // RenderItems should not be copied as they are heavy
  RenderItem( const RenderItem& item );
  RenderItem& operator = ( const RenderItem& item );

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H
