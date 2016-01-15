#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H__

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
}

namespace SceneGraph
{

class RenderItem;

typedef std::vector< RenderItem > RendererItemContainer;

/**
 * A render-item is a renderer & modelView matrix pair.
 */
class RenderItem
{
public:

  /**
   * Construct a new RenderItem
   */
  static RenderItem* New();

  /**
   * Non-virtual destructor; RenderItem is not suitable as a base class.
   */
  ~RenderItem();

  /**
   * Overriden delete operator.
   * Deletes the RenderItem from its global memory pool
   */
  void operator delete( void* ptr );

  /**
   * Reset the render-item.
   */
  void Reset();

  /**
   * Retrieve the renderer.
   * @return The renderer.
   */
  Render::Renderer& GetRenderer() const;

  /**
   * Set the renderer
   * @param[in] renderer The renderer
   */
  void SetRenderer( Render::Renderer* renderer );

  /**
   * Set the node
   * @param[in] node The node
   */
  void SetNode( Node* node );

  /**
   * Retrieve the node
   * @return The node
   */
  const Node& GetNode() const
  {
    return *mNode;
  }
  /**
   * Retrieve the modelView matrix.
   * @return The modelView matrix.
   */
  Matrix& GetModelViewMatrix();

  /**
   * Retrieve the modelView matrix.
   * @return The modelView matrix.
   */
  const Matrix& GetModelViewMatrix() const;

  /**
   * Retrieve the depth index
   */
  int GetDepthIndex() const
  {
    return mDepthIndex;
  }

  /**
   * Set the depth index
   */
  void SetDepthIndex( int depthIndex );

  /**
   * Set if the RenderItem is opaque
   * @param[in] isOpaque true if the RenderItem is opaque, false otherwise
   */
  void SetIsOpaque( bool isOpaque );

  /**
   * Check if the RenderItem is opaque
   * @return True if it is opaque, false otherwise
   */
  bool IsOpaque() const
  {
    return mIsOpaque;
  }

private:

  /**
   * Private constructor. See RenderItem::New
   */
  RenderItem();

  // RenderItems should not be copied as they are heavy
  RenderItem( const RenderItem& item );
  RenderItem& operator = ( const RenderItem& item );

  Matrix            mModelViewMatrix;
  Render::Renderer* mRenderer;
  Node*             mNode;
  int               mDepthIndex;
  bool              mIsOpaque:1;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H__
