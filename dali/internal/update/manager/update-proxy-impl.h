#ifndef DALI_INTERNAL_UPDATE_PROXY_IMPL_H
#define DALI_INTERNAL_UPDATE_PROXY_IMPL_H

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

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/manager/transform-manager.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali
{

namespace Internal
{

/**
 * @brief The implementation of Dali::UpdateProxy.
 *
 * Ref-counting is not required for this object.
 *
 * @see Dali::UpdateProxy
 */
class UpdateProxy
{
public:

  /**
   * @brief Constructor.
   * @param[in]  transformManager   Ref to the TransformManager in order to set/get transform properties of nodes
   * @param[in]  rootNode           The root node for this proxy
   */
  UpdateProxy( SceneGraph::TransformManager& transformManager, SceneGraph::Node& rootNode );

  /**
   * @brief Destructor.
   */
  ~UpdateProxy();

  // Movable but not copyable

  UpdateProxy( const UpdateProxy& )            = delete;  ///< Deleted copy constructor.
  UpdateProxy( UpdateProxy&& )                 = default; ///< Default move constructor.
  UpdateProxy& operator=( const UpdateProxy& ) = delete;  ///< Deleted copy assignment operator.
  UpdateProxy& operator=( UpdateProxy&& )      = default; ///< Default move assignment operator.

  /**
   * @copydoc Dali::UpdateProxy::GetPosition()
   */
  bool GetPosition( uint32_t id, Vector3& position) const;

  /**
   * @copydoc Dali::UpdateProxy::SetPosition()
   */
  bool SetPosition( uint32_t id, const Vector3& position );

  /**
   * @copydoc Dali::UpdateProxy::BakePosition()
   */
  bool BakePosition( uint32_t id, const Vector3& position );

  /**
   * @copydoc Dali::UpdateProxy::GetSize()
   */
  bool GetSize( uint32_t id, Vector3& size ) const;

  /**
   * @copydoc Dali::UpdateProxy::SetSize()
   */
  bool SetSize( uint32_t id, const Vector3& size );

  /**
   * @copydoc Dali::UpdateProxy::BakeSize()
   */
  bool BakeSize( uint32_t id, const Vector3& size );

  /**
   * @copydoc Dali::UpdateProxy::GetPositionAndSize()
   */
  bool GetPositionAndSize( uint32_t id, Vector3& position, Vector3& size ) const;

  /**
   * @copydoc Dali::UpdateProxy::GetScale()
   */
  bool GetScale( uint32_t id, Vector3& scale ) const;

  /**
   * @copydoc Dali::UpdateProxy::SetScale()
   */
  bool SetScale( uint32_t id, const Vector3& scale );

  /**
   * @copydoc Dali::UpdateProxy::BakeScale()
   */
  bool BakeScale( uint32_t id, const Vector3& scale );

  /**
   * @copydoc Dali::UpdateProxy::GetColor()
   */
  bool GetColor( uint32_t id, Vector4& color ) const;

  /**
   * @copydoc Dali::UpdateProxy::SetColor()
   */
  bool SetColor( uint32_t id, const Vector4& color ) const;

  /**
   * @copydoc Dali::UpdateProxy::BakeColor()
   */
  bool BakeColor( uint32_t id, const Vector4& color ) const;

  /**
   * @brief Retrieves the root-node used by this class
   * @return The root node used by this class.
   */
  SceneGraph::Node& GetRootNode() const
  {
    return mRootNode;
  }

  /**
   * @brief Sets the buffer index to use when processing the next callback.
   * @param[in]  bufferIndex  The current buffer index
   */
  void SetCurrentBufferIndex( BufferIndex bufferIndex )
  {
    mCurrentBufferIndex = bufferIndex;
  }

  /**
   * @brief Informs the update-proxy that the node hierarchy has changed.
   */
  void NodeHierarchyChanged();

private:

  /**
   * @brief Retrieves the node with the specified ID.
   * @param[in]  id  The ID of the node required
   * @return A pointer to the required node if found.
   * @note This caches the last accessed node.
   */
  SceneGraph::Node* GetNodeWithId( uint32_t id ) const;

private:

  /**
   * Structure to store the ID & Node pair
   */
  struct IdNodePair
  {
    uint32_t id; ///< The ID of the node
    SceneGraph::Node* node; ///< The node itself
  };

  mutable std::vector< IdNodePair > mNodeContainer; ///< Used to store cached pointers to already searched for Nodes.
  mutable IdNodePair mLastCachedIdNodePair; ///< Used to cache the last retrieved id-node pair.
  BufferIndex mCurrentBufferIndex;

  SceneGraph::TransformManager& mTransformManager; ///< Reference to the Transform Manager.
  SceneGraph::Node& mRootNode; ///< The root node of this update proxy.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_PROXY_IMPL_H
