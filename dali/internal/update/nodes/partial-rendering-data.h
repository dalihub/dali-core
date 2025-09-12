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
  struct NodeInfomations
  {
    Matrix  modelMatrix{};         /// Model matrix
    Vector4 worldColor{};          /// World Color
    Vector4 updatedPositionSize{}; /// Updated position/size (x, y, width, height)
    Vector3 size{};                /// Size

    mutable size_t hash{0u}; /// Last frame's hash

    static size_t CalculateHash(const Vector4& worldColor, const Vector4& updatedPositionSize, const Vector3& size, const Matrix& matrix)
    {
      size_t hash = Dali::Internal::HashUtils::INITIAL_HASH_VALUE;
      Dali::Internal::HashUtils::HashRawBuffer<float>(worldColor.AsFloat(), 4, hash);
      Dali::Internal::HashUtils::HashRawBuffer<float>(updatedPositionSize.AsFloat(), 4, hash);
      Dali::Internal::HashUtils::HashRawBuffer<float>(size.AsFloat(), 3, hash);
      Dali::Internal::HashUtils::HashRawBuffer<float>(matrix.AsFloat(), 16, hash);
      return hash;
    }

    size_t GetHash() const
    {
      if(hash == 0u)
      {
        hash = CalculateHash(worldColor, updatedPositionSize, size, modelMatrix);
      }
      return hash;
    }

  public:
    NodeInfomations() = default; // Default constructor

    NodeInfomations(const Matrix& modelMatrix, const Vector4& worldColor, const Vector4& updatedPositionSize, const Vector3& size, size_t hash = 0u)
    : modelMatrix(modelMatrix),
      worldColor(worldColor),
      updatedPositionSize(updatedPositionSize),
      size(size),
      hash(hash)
    {
    }
    NodeInfomations(NodeInfomations&& rhs)
    {
      (*this) = std::move(rhs);
    }

    NodeInfomations& operator=(NodeInfomations&& rhs)
    {
      if(this != &rhs)
      {
        modelMatrix         = std::move(rhs.modelMatrix);
        worldColor          = std::move(rhs.worldColor);
        updatedPositionSize = std::move(rhs.updatedPositionSize);
        size                = std::move(rhs.size);

        hash     = rhs.hash;
        rhs.hash = 0u;
      }
      return *this;
    }

  private:
    // Let we don't allow to copy the matrix values.
    NodeInfomations(const NodeInfomations&)            = delete;
    NodeInfomations& operator=(const NodeInfomations&) = delete;
  } mNodeInfomations;

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

      mNodeInfomations = NodeInfomations(modelMatrix, worldColor, updatedPositionSize, size, 0u);
    }
    else
    {
      size_t hash = NodeInfomations::CalculateHash(worldColor, updatedPositionSize, size, modelMatrix);

      mUpdated = !(mNodeInfomations.GetHash() == hash &&        ///< Hash comparision first
                   mNodeInfomations.worldColor == worldColor && ///< Full comparision one more time.
                   mNodeInfomations.updatedPositionSize == updatedPositionSize &&
                   mNodeInfomations.size == size &&
                   mNodeInfomations.modelMatrix == modelMatrix); ///< Compare matrix last order

      if(mUpdated)
      {
        mNodeInfomations = NodeInfomations(modelMatrix, worldColor, updatedPositionSize, size, hash);
      }
    }

    mUpdateDecay = Decay::UPDATED_CURRENT_FRAME;

    // Don't change mVisible.
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
};

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_SCENE_GRAPH_PARTIAL_RENDERING_DATA_H
