#ifndef DALI_INTERNAL_SCENE_GRAPH_PARTIAL_RENDERING_DATA_H
#define DALI_INTERNAL_SCENE_GRAPH_PARTIAL_RENDERING_DATA_H

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
   * @brief Tests whether cache changed since last frame, and update node infomations
   * @return True if changed.
   */
  bool UpdateNodeInfomations(const Matrix& modelMatrix, const Vector4& worldColor, const Vector4& updatedPositionSize, const Vector3& size)
  {
    if(mUpdateDecay == Decay::UPDATED_CURRENT_FRAME)
    {
      return mUpdated;
    }

    if(mUpdateDecay == Decay::EXPIRED)
    {
      mUpdated = true;

      mNodeInfomations = {modelMatrix, worldColor, updatedPositionSize, size, 0u, 0u};
    }
    else
    {
      mUpdated = true;

      size_t hash1 = NodeInfomations::CalculateHash1(worldColor, updatedPositionSize, size);
      size_t hash2 = 0u;
      if(mNodeInfomations.GetHash1() == hash1)
      {
        hash2 = NodeInfomations::CalculateHash2(modelMatrix); // Hash2 is expensive, so we calculate it only when necessary
        if(mNodeInfomations.GetHash2() == hash2)
        {
          // Full comparision one more time.
          mUpdated = !(mNodeInfomations.matrix == modelMatrix &&
                       mNodeInfomations.color == worldColor &&
                       mNodeInfomations.updatedPositionSize == updatedPositionSize &&
                       mNodeInfomations.size == size);
        }
      }

      if(mUpdated)
      {
        mNodeInfomations = {modelMatrix, worldColor, updatedPositionSize, size, hash1, hash2};

        // Don't change mVisible.
      }
    }

    mUpdateDecay = Decay::UPDATED_CURRENT_FRAME;

    return mUpdated;
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

private:
  struct NodeInfomations
  {
    Matrix  matrix{};              /// Model matrix
    Vector4 color{};               /// Color
    Vector4 updatedPositionSize{}; /// Updated position/size (x, y, width, height)
    Vector3 size{};                /// Size

    mutable size_t hash1{0u}; /// Last frame's hash for non-matrix
    mutable size_t hash2{0u}; /// Last frame's hash for matrix

    static size_t CalculateHash1(const Vector4& color, const Vector4& updatedPositionSize, const Vector3& size)
    {
      size_t hash = Dali::Internal::HashUtils::INITIAL_HASH_VALUE;
      Dali::Internal::HashUtils::HashRawBuffer<float>(color.AsFloat(), 4, hash);
      Dali::Internal::HashUtils::HashRawBuffer<float>(updatedPositionSize.AsFloat(), 4, hash);
      Dali::Internal::HashUtils::HashRawBuffer<float>(size.AsFloat(), 3, hash);
      return hash;
    }

    static size_t CalculateHash2(const Matrix& matrix)
    {
      size_t hash = Dali::Internal::HashUtils::INITIAL_HASH_VALUE;
      Dali::Internal::HashUtils::HashRawBuffer<float>(matrix.AsFloat(), 16, hash);
      return hash;
    }

    size_t GetHash1() const
    {
      if(hash1 == 0u)
      {
        hash1 = CalculateHash1(color, updatedPositionSize, size);
      }
      return hash1;
    }
    size_t GetHash2() const
    {
      if(hash2 == 0u)
      {
        hash2 = CalculateHash2(matrix);
      }
      return hash2;
    }
  } mNodeInfomations;
};

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_SCENE_GRAPH_PARTIAL_RENDERING_DATA_H
