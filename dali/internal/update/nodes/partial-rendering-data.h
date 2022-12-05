#ifndef DALI_INTERNAL_SCENE_GRAPH_PARTIAL_RENDERING_DATA_H
#define DALI_INTERNAL_SCENE_GRAPH_PARTIAL_RENDERING_DATA_H

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
 */

#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

namespace Dali::Internal::SceneGraph
{
class Node;
class TextureSet;

/**
 * Structure contains partial rendering data used in order to determine
 * whether anything has changed and node has to be updated
 */
struct PartialRenderingData
{
  Node*             node{nullptr};                  /// Node associated with the entry
  Renderer*         renderer{nullptr};              /// Renderer object
  const TextureSet* textureSet{nullptr};            /// TextureSet object
  Matrix            matrix{};                       /// Model-view matrix
  Vector4           color{};                        /// Color
  Vector4           updatedPositionSize{};          /// Updated position/size (x, y, width, height)
  Vector3           size{};                         /// Size
  uint32_t          depthIndex{0u};                 /// Depth index
  uint32_t          hash;                           /// Last frame's hash
  bool              isOpaque{};                     /// Opacity state

  bool mVisible{true};   /// Visible state (Not hashed)
  bool mRendered{false}; /// Rendering state (Not hashed)

  /**
   * Calculate a hash from the cache data
   */
  void CalculateHash()
  {
    hash = Dali::INITIAL_HASH_VALUE;
    AddToHash(hash, &node, sizeof(decltype(node)));
    AddToHash(hash, &renderer, sizeof(decltype(renderer)));
    AddToHash(hash, &textureSet, sizeof(decltype(textureSet)));
    AddToHash(hash, &matrix, sizeof(decltype(matrix)));
    AddToHash(hash, &color, sizeof(decltype(color)));
    AddToHash(hash, &updatedPositionSize, sizeof(decltype(updatedPositionSize)));
    AddToHash(hash, &size, sizeof(decltype(size)));
    AddToHash(hash, &depthIndex, sizeof(decltype(depthIndex)));
    AddToHash(hash, &isOpaque, sizeof(decltype(isOpaque)));
  }

  /**
   * @brief Tests whether cache changed since last frame
   * @return True if changed
   */
  bool IsUpdated(PartialRenderingData& frameCache)
  {
    frameCache.CalculateHash();

    return hash != frameCache.hash ||
           node != frameCache.node ||
           renderer != frameCache.renderer ||
           textureSet != frameCache.textureSet ||
           matrix != frameCache.matrix ||
           color != frameCache.color ||
           updatedPositionSize != frameCache.updatedPositionSize ||
           size != frameCache.size ||
           depthIndex != frameCache.depthIndex ||
           isOpaque != frameCache.isOpaque ||

           !mRendered; // If everything is the same, check if we didn't render last frame.
  }

  void Update(const PartialRenderingData& frameCache)
  {
    node                = frameCache.node;
    renderer            = frameCache.renderer;
    textureSet          = frameCache.textureSet;
    matrix              = frameCache.matrix;
    color               = frameCache.color;
    updatedPositionSize = frameCache.updatedPositionSize;
    size                = frameCache.size;
    depthIndex          = frameCache.depthIndex;
    isOpaque            = frameCache.isOpaque;
    hash                = frameCache.hash;

    mRendered = true;
    // Don't change mVisible.
  }

private:
  void AddToHash(uint32_t& aHash, void* el, size_t numBytes)
  {
    uint8_t* elBytes = static_cast<uint8_t*>(el);
    for(size_t i = 0; i < numBytes; ++i)
    {
      aHash = aHash * 33 + *elBytes++;
    }
  }
};

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_SCENE_GRAPH_PARTIAL_RENDERING_DATA_H
