#ifndef DALI_INTERNAL_UPDATE_PROXY_IMPL_H
#define DALI_INTERNAL_UPDATE_PROXY_IMPL_H

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

// EXTERNAL INCLUDES
#include <cstdint>
#include <memory>

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/manager/transform-manager.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/public-api/common/list-wrapper.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>

#include <dali/internal/update/manager/scene-graph-traveler-interface.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class UpdateManager;
}

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
   * @param[in]  updateManager      Ref to the UpdateManager in order to add property resetters
   * @param[in]  transformManager   Ref to the TransformManager in order to set/get transform properties of nodes
   * @param[in]  traveler           The cache of traversal.
   */
  UpdateProxy(SceneGraph::UpdateManager& updateManager, SceneGraph::TransformManager& transformManager, SceneGraphTravelerInterfacePtr traveler);

  /**
   * @brief Destructor.
   */
  ~UpdateProxy();

  // Movable but not copyable

  UpdateProxy(const UpdateProxy&)            = delete;  ///< Deleted copy constructor.
  UpdateProxy(UpdateProxy&&)                 = default; ///< Default move constructor.
  UpdateProxy& operator=(const UpdateProxy&) = delete;  ///< Deleted copy assignment operator.
  UpdateProxy& operator=(UpdateProxy&&)      = delete;  ///< Deleted move assignment operator.

  /**
   * @copydoc Dali::UpdateProxy::GetPosition()
   */
  bool GetPosition(uint32_t id, Vector3& position) const;

  /**
   * @copydoc Dali::UpdateProxy::SetPosition()
   */
  bool SetPosition(uint32_t id, const Vector3& position);

  /**
   * @copydoc Dali::UpdateProxy::BakePosition()
   */
  bool BakePosition(uint32_t id, const Vector3& position);

  /**
   * @copydoc Dali::UpdateProxy::GetOrientation()
   */
  bool GetOrientation(uint32_t id, Quaternion& orientation) const;

  /**
   * @copydoc Dali::UpdateProxy::SetOrientation()
   */
  bool SetOrientation(uint32_t id, const Quaternion& orientation);

  /**
   * @copydoc Dali::UpdateProxy::BakeOrientation()
   */
  bool BakeOrientation(uint32_t id, const Quaternion& orientation);

  /**
   * @copydoc Dali::UpdateProxy::GetSize()
   */
  bool GetSize(uint32_t id, Vector3& size) const;

  /**
   * @copydoc Dali::UpdateProxy::SetSize()
   */
  bool SetSize(uint32_t id, const Vector3& size);

  /**
   * @copydoc Dali::UpdateProxy::BakeSize()
   */
  bool BakeSize(uint32_t id, const Vector3& size);

  /**
   * @copydoc Dali::UpdateProxy::GetPositionAndSize()
   */
  bool GetPositionAndSize(uint32_t id, Vector3& position, Vector3& size) const;

  /**
   * @copydoc Dali::UpdateProxy::GetWorldPositionScaleAndSize()
   */
  bool GetWorldPositionScaleAndSize(uint32_t id, Vector3& position, Vector3& scale, Vector3& size) const;

  /**
   * @copydoc Dali::UpdateProxy::GetWorldTransformAndSize()
   */
  bool GetWorldTransformAndSize(uint32_t id, Vector3& position, Vector3& scale, Quaternion& orientation, Vector3& size) const;

  /**
   * @copydoc Dali::UpdateProxy::GetScale()
   */
  bool GetScale(uint32_t id, Vector3& scale) const;

  /**
   * @copydoc Dali::UpdateProxy::SetScale()
   */
  bool SetScale(uint32_t id, const Vector3& scale);

  /**
   * @copydoc Dali::UpdateProxy::BakeScale()
   */
  bool BakeScale(uint32_t id, const Vector3& scale);

  /**
   * @copydoc Dali::UpdateProxy::GetColor()
   */
  bool GetColor(uint32_t id, Vector4& color) const;

  /**
   * @copydoc Dali::UpdateProxy::SetColor()
   */
  bool SetColor(uint32_t id, const Vector4& color);

  /**
   * @copydoc Dali::UpdateProxy::BakeColor()
   */
  bool BakeColor(uint32_t id, const Vector4& color);

  /**
   * @brief Sets the buffer index to use when processing the next callback.
   * @param[in]  bufferIndex  The current buffer index
   */
  void SetCurrentBufferIndex(BufferIndex bufferIndex)
  {
    mCurrentBufferIndex = bufferIndex;
  }

  /**
   * @brief Informs the update-proxy that the node hierarchy has changed.
   */
  void NodeHierarchyChanged();

  /**
   * @brief Inform the proxy that a sync point was requested.
   *
   * @param[in] syncPoint The sync point to notify with.
   */
  void Notify(Dali::UpdateProxy::NotifySyncPoint syncPoint);

  /**
   * @brief Get the sync point from the proxy. If there was no sync point,
   * this returns INVALID_SYNC. It is intended that this method is only called
   * once per Update(), so clears any stored sync point.
   * @return Valid sync point or INVALID_SYNC
   */
  Dali::UpdateProxy::NotifySyncPoint PopSyncPoint();

  /**
   * @brief Adds node resetter for each dirty node whose animatable properties have been changed.
   */
  void AddNodeResetters();

  /**
   * @copydoc Dali::UpdateProxy::GetUpdateArea()
   */
  bool GetUpdateArea(uint32_t id, Vector4& updateArea) const;

  /**
   * @copydoc Dali::UpdateProxy::SetUpdateArea()
   */
  bool SetUpdateArea(uint32_t id, const Vector4& updateArea);

  /**
   * @copydoc Dali::UpdateProxy::SetIgnored()
   */
  bool SetIgnored(uint32_t id, bool ignored);

  /**
   * @copydoc Dali::UpdateProxy::GetIgnored()
   */
  bool GetIgnored(uint32_t id, bool& ignored) const;

private:
  /**
   * @brief Retrieves the node with the specified ID.
   * @param[in]  id  The ID of the node required
   * @return A pointer to the required node if found.
   * @note This caches the last accessed node.
   */
  SceneGraph::Node* GetNodeWithId(uint32_t id) const;

  /**
   * @brief Adds a property-resetter for non-transform properties so that they can be reset to their base value every frame.
   * @param[in]  node          The node the property belongs to
   * @param[in]  propertyBase  The property itself
   */
  void AddResetter(SceneGraph::Node& node, SceneGraph::PropertyBase& propertyBase);

private:
  /**
   * Structure to store the ID & Node pair
   */
  struct IdNodePair
  {
    uint32_t          id;   ///< The ID of the node
    SceneGraph::Node* node; ///< The node itself
  };

  class PropertyModifier;
  using PropertyModifierPtr = std::unique_ptr<PropertyModifier>;

  mutable IdNodePair    mLastCachedIdNodePair; ///< Used to cache the last retrieved id-node pair.
  std::vector<uint32_t> mDirtyNodes;           ///< Used to store the ID of the dirty nodes with non-transform property modifications.
  BufferIndex           mCurrentBufferIndex;

  SceneGraph::UpdateManager&     mUpdateManager;      ///< Reference to the Update Manager.
  SceneGraph::TransformManager&  mTransformManager;   ///< Reference to the Transform Manager.
  SceneGraphTravelerInterfacePtr mSceneGraphTraveler; ///< The cache system when we travel scene graph. (Not owned)

  std::list<Dali::UpdateProxy::NotifySyncPoint> mSyncPoints;

  PropertyModifierPtr mPropertyModifier; ///< To ensure non-transform property modifications reset to base values.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_PROXY_IMPL_H
