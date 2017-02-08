#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/math/matrix.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali
{

namespace Internal
{

namespace Render
{
class Renderer;
class RenderGeometry;
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
   * Non-virtual destructor; RenderItem is not suitable as a base class.
   */
  ~RenderItem();

  /**
   * Overriden delete operator.
   * Deletes the RenderItem from its global memory pool
   * @param[in] A pointer to the RenderItem to delete.
   */
  void operator delete( void* ptr );

  Matrix            mModelMatrix;
  Matrix            mModelViewMatrix;
  Vector3           mSize;
  Render::Renderer* mRenderer;
  Node*             mNode;
  const void*       mTextureSet;        //< Used only for sorting

  mutable Render::Geometry* mBatchRenderGeometry;

  int               mDepthIndex;
  bool              mIsOpaque:1;
  bool              mBatched:1;

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
