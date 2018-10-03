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
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
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
  Vector3 GetPosition( unsigned int id ) const;

  /**
   * @copydoc Dali::UpdateProxy::SetPosition()
   */
  void SetPosition( unsigned int id, const Vector3& position );

  /**
   * @copydoc Dali::UpdateProxy::BakePosition()
   */
  void BakePosition( unsigned int id, const Vector3& position );

  /**
   * @copydoc Dali::UpdateProxy::GetSize()
   */
  const Vector3& GetSize( unsigned int id ) const;

  /**
   * @copydoc Dali::UpdateProxy::SetSize()
   */
  void SetSize( unsigned int id, const Vector3& size );

  /**
   * @copydoc Dali::UpdateProxy::BakeSize()
   */
  void BakeSize( unsigned int id, const Vector3& size );

  /**
   * @copydoc Dali::UpdateProxy::GetPositionAndSize()
   */
  void GetPositionAndSize( unsigned int id, Vector3& position, Vector3& size ) const;

  /**
   * @copydoc Dali::UpdateProxy::GetScale()
   */
  Vector3 GetScale( unsigned int id ) const;

  /**
   * @copydoc Dali::UpdateProxy::SetScale()
   */
  void SetScale( unsigned int id, const Vector3& scale );

  /**
   * @copydoc Dali::UpdateProxy::BakeScale()
   */
  void BakeScale( unsigned int id, const Vector3& scale );

  /**
   * @copydoc Dali::UpdateProxy::GetColor()
   */
  Vector4 GetColor( unsigned int id ) const;

  /**
   * @copydoc Dali::UpdateProxy::SetColor()
   */
  void SetColor( unsigned int id, const Vector4& color ) const;

  /**
   * @copydoc Dali::UpdateProxy::BakeColor()
   */
  void BakeColor( unsigned int id, const Vector4& color ) const;

  /**
   * @brief Retrieves the root-node used by this class
   * @return The root node used by this class.
   */
  SceneGraph::Node& GetRootNode() const
  {
    return mRootNode;
  }

  void SetCurrentBufferIndex( BufferIndex bufferIndex )
  {
    mCurrentBufferIndex = bufferIndex;
  }

private:

  /**
   * @brief Retrieves the node with the specified ID.
   * @param[in]  id  The ID of the node required
   * @return A pointer to the required node if found.
   * @note This caches the last accessed node.
   */
  SceneGraph::Node* GetNodeWithId( unsigned int id ) const;

private:

  /**
   * Structure to store the ID & Node pair
   */
  struct IdNodePair
  {
    unsigned int id; ///< The ID of the node
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
