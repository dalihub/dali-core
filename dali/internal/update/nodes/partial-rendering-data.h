#ifndef DALI_INTERNAL_SCENE_GRAPH_PARTIAL_RENDERING_DATA_H
#define DALI_INTERNAL_SCENE_GRAPH_PARTIAL_RENDERING_DATA_H

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
 */

#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

namespace Dali::Internal::SceneGraph
{
/**
 * Structure contains partial rendering data used in order to determine
 * whether anything has changed and node has to be updated
 */
struct PartialRenderingData
{
  Matrix      matrix{};              /// Model-view matrix
  Vector4     color{};               /// Color
  Vector4     updatedPositionSize{}; /// Updated position/size (x, y, width, height)
  Vector3     size{};                /// Size
  std::size_t hash{0u};              /// Last frame's hash

  bool mVisible : 1; /// Visible state. It is depends on node's visibility (Not hashed)
  bool mUpdated : 1; /// IsUpdated return true at this frame. Will be reset at UpdateNodes time. (Not hashed)

  enum Decay
  {
    EXPIRED                = 0,
    UPDATED_PREVIOUS_FRAME = 1,
    UPDATED_CURRENT_FRAME  = 2
  };
  Decay mUpdateDecay : 3; ///< Update decay (aging, Not hashed)

  PartialRenderingData()
  : mVisible{true},
    mUpdated{false},
    mUpdateDecay{Decay::EXPIRED}
  {
  }

  /**
   * Calculate a hash from the cache data
   */
  void CalculateHash()
  {
    hash = Dali::Internal::HashUtils::INITIAL_HASH_VALUE;
    Dali::Internal::HashUtils::HashRawBuffer<float>(matrix.AsFloat(), 16, hash);
    Dali::Internal::HashUtils::HashRawBuffer<float>(color.AsFloat(), 4, hash);
    Dali::Internal::HashUtils::HashRawBuffer<float>(updatedPositionSize.AsFloat(), 4, hash);
    Dali::Internal::HashUtils::HashRawBuffer<float>(size.AsFloat(), 3, hash);
  }

  /**
   * @brief Tests whether cache changed since last frame
   * @return True if changed.
   */
  bool IsUpdated(PartialRenderingData& frameCache)
  {
    if(mUpdateDecay == Decay::UPDATED_CURRENT_FRAME)
    {
      return mUpdated;
    }

    frameCache.CalculateHash();

    mUpdated = hash != frameCache.hash ||
               mUpdateDecay == Decay::EXPIRED || ///< If current value is expired, so cached data is invalid.
               matrix != frameCache.matrix ||
               color != frameCache.color ||
               updatedPositionSize != frameCache.updatedPositionSize ||
               size != frameCache.size;

    mUpdateDecay = Decay::UPDATED_CURRENT_FRAME;

    return mUpdated;
  }

  /**
   * @brief Update cached value
   */
  void Update(const PartialRenderingData& frameCache)
  {
    matrix              = frameCache.matrix;
    color               = frameCache.color;
    updatedPositionSize = frameCache.updatedPositionSize;
    size                = frameCache.size;
    hash                = frameCache.hash;

    // Don't change mVisible.
  }

  /**
   * @brief Aging down for this data.
   */
  void Aging()
  {
    mUpdateDecay = static_cast<Decay>(static_cast<int>(mUpdateDecay) >> 1u);
  }

  /**
   * @brief Make this data expired.
   */
  void MakeExpired()
  {
    mUpdateDecay = Decay::EXPIRED;
  }
};

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_SCENE_GRAPH_PARTIAL_RENDERING_DATA_H
