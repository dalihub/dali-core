#ifndef DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali/internal/render/data-providers/uniform-map-data-provider.h>
#include <dali/public-api/math/matrix.h>
#include <cstring>

namespace Dali
{
struct Vector4;
class Matrix;

namespace Internal
{
namespace SceneGraph
{
class Node;
class Renderer;
class TextureSet;

/**
 * Structure to store partial rendering cache data
 */
struct PartialRenderingCacheInfo
{
  Node*             node{nullptr};         /// Node associated with the entry
  const Renderer*   renderer{nullptr};     /// Renderer object
  const TextureSet* textureSet{nullptr};   /// TextureSet object
  Matrix            matrix{};              /// Model-view matrix
  Vector4           color{};               /// Color
  Vector3           size{};                /// Size
  Vector4           updatedPositionSize{}; /// Updated position/size (x, y, width, height)
  bool              isOpaque{};            /// Opacity state
  uint32_t          depthIndex{0u};        /// Depth index
};

/**
 * Structure contains partial rendering data used in order to determine
 * whether anything has changed and node has to be updated
 */
struct PartialRenderingNodeData
{
  /**
   * @brief Retrieves current PartialDataCacheInfo structure
   * @return Current PartialDataCacheInfo structure
   */
  PartialRenderingCacheInfo& GetCurrentCacheInfo()
  {
    return mData[mCurrentIndex];
  }

  /**
   * @brief Tests whether cache changed since last frame
   * @return True if changed
   */
  bool IsUpdated()
  {
    return (0 != memcmp(&mData[0], &mData[1], sizeof(PartialRenderingCacheInfo)) || !mRendered);
  }

  /**
   * @brief Swaps cache buffers
   */
  void SwapBuffers()
  {
    mCurrentIndex = static_cast<uint8_t>((~mCurrentIndex) & 1);
  }

  PartialRenderingCacheInfo mData[2u];         /// Double-buffered data
  uint8_t                   mCurrentIndex{0u}; /// Current buffer index
  bool                      mVisible{true};    /// Visible state
  bool                      mRendered{false};  /// Rendering state
};

/**
 * An interface to provide partial rendering data
 */
class PartialRenderingDataProvider
{
public:
  /**
   * Constructor
   */
  PartialRenderingDataProvider() = default;

  /**
   * Destructor
   */
  virtual ~PartialRenderingDataProvider() = default;

  /**
   * @brief Returns partial rendering data associated with the node.
   * @return A valid pointer to the partial rendering data or nullptr
   */
  PartialRenderingNodeData& GetPartialRenderingData()
  {
    return mPartialRenderingData;
  }

protected:
  PartialRenderingNodeData mPartialRenderingData;
};

/**
 * An interface to provide data for a Renderer
 */
class NodeDataProvider : public PartialRenderingDataProvider
{
public:
  /**
   * Constructor. Nothing to do as a pure interface.
   */
  NodeDataProvider() = default;

  /**
   * @param bufferIndex to use
   * @return a reference to the model matrix
   */
  virtual const Matrix& GetModelMatrix(BufferIndex bufferIndex) const = 0;

  /**
   * @param bufferIndex to use
   * @return a reference to the color
   */
  virtual const Vector4& GetRenderColor(BufferIndex bufferIndex) const = 0;

  /**
   * Get the map of uniforms to property value addresses (for node only)
   */
  virtual const UniformMap& GetNodeUniformMap() const = 0;

protected:
  /**
   * Virtual destructor, this is an interface, no deletion through this interface
   */
  ~NodeDataProvider() override = default;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H
