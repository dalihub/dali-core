#ifndef DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_H
#define DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <stdint.h> ///< For size_t

// INTERNAL INCLUDES
#include <dali/internal/update/nodes/node-helper.h> ///< For property wrapper macro
#include <dali/internal/update/rendering/scene-graph-visual-renderer-property.h>

namespace Dali::Internal::SceneGraph
{
class Node;
namespace VisualRenderer
{
struct DecoratedVisualProperties
{
  DecoratedVisualProperties(VisualRendererPropertyObserver& owner)
  : mCoefficient(owner),
    mBorderlineWidth(0.0f),
    mBorderlineOffset(0.0f),
    mBlurRadius(0.0f),
    mBorderlineColor(Color::BLACK),
    mCornerRadius(Vector4::ZERO),
    mCornerSquareness(Vector4::ZERO),
    mCornerRadiusPolicy(1.0f)
  {
  }

  ~DecoratedVisualProperties()
  {
  }

public: // Public API
  /**
   * @brief Get the update area after visual properties applied.
   * @param[in] bufferIndex The current buffer index.
   * @param[in, out] updateArea The original update area before apply the visual properties. Stored into this value after calculated
   */
  void GetVisualTransformedUpdateArea(BufferIndex updateBufferIndex, Vector4& updateArea) noexcept;

public:
  /**
   * @brief Prepare properties and ready to render sequence
   *
   * @return True if we need to render this frame.
   */
  bool PrepareProperties();

public:
  /**
   * @brief Cached coefficient value when we calculate visual transformed update size.
   * It can reduce complexity of calculate the vertex position.
   *
   * Vector2 vertexPosition += Vector2(D, D) * aPosition
   */
  struct DecoratedVisualTransformedUpdateSizeCoefficientCache : public VisualRendererCoefficientCacheBase
  {
    DecoratedVisualTransformedUpdateSizeCoefficientCache(VisualRendererPropertyObserver& owner)
    : VisualRendererCoefficientCacheBase(owner),
      coefD(0.0f)
    {
    }

    ~DecoratedVisualTransformedUpdateSizeCoefficientCache() override = default;

    float coefD;
  };

public: // Default properties
  // Define a base offset for the following wrappers. The wrapper macros calculate offsets from the previous
  // element such that each wrapper type generates a compile time offset to the CoefficientCache data.
  BASE(DecoratedVisualTransformedUpdateSizeCoefficientCache, mCoefficient); ///< Coefficient value to calculate visual transformed update size by VisualProperties more faster.

  PROPERTY_WRAPPER(mCoefficient, VisualRendererProperty, float, mBorderlineWidth);
  PROPERTY_WRAPPER(mBorderlineWidth, VisualRendererProperty, float, mBorderlineOffset);
  PROPERTY_WRAPPER(mBorderlineOffset, VisualRendererProperty, float, mBlurRadius);

  // Properties that don't give any effort to coefficient.
  AnimatableProperty<Vector4> mBorderlineColor;
  AnimatableProperty<Vector4> mCornerRadius;
  AnimatableProperty<Vector4> mCornerSquareness;
  AnimatableProperty<float>   mCornerRadiusPolicy;
};

struct VisualProperties
{
  VisualProperties(VisualRendererPropertyObserver& owner)
  : mCoefficient(owner),
    mTransformOffset(Vector2::ZERO),
    mTransformSize(Vector2::ONE),
    mTransformOrigin(Vector2::ZERO),
    mTransformAnchorPoint(Vector2::ZERO),
    mTransformOffsetSizeMode(Vector4::ZERO),
    mExtraSize(Vector2::ZERO)
  {
  }

  ~VisualProperties()
  {
  }

public: // Public API
  /**
   * @brief Get the update area after visual properties applied.
   * @param[in] bufferIndex The current buffer index.
   * @param[in, out] updateArea The original update area before apply the visual properties. Stored into this value after calculated
   */
  void GetVisualTransformedUpdateArea(BufferIndex updateBufferIndex, Vector4& updateArea) noexcept;

public:
  /**
   * @brief Prepare properties and ready to render sequence
   *
   * @return True if we need to render this frame.
   */
  bool PrepareProperties();

public:
  /**
   * @brief Cached coefficient value when we calculate visual transformed update size.
   * It can reduce complexity of calculate the vertex position.
   *
   * Vector2 vertexPosition = (XA * aPosition + XB) * originalSize + (CA * aPosition + CB)
   */
  struct VisualTransformedUpdateSizeCoefficientCache : public VisualRendererCoefficientCacheBase
  {
    VisualTransformedUpdateSizeCoefficientCache(VisualRendererPropertyObserver& owner)
    : VisualRendererCoefficientCacheBase(owner),
      coefXA(Vector2::ZERO),
      coefXB(Vector2::ZERO),
      coefCA(Vector2::ZERO),
      coefCB(Vector2::ZERO)
    {
    }

    ~VisualTransformedUpdateSizeCoefficientCache() override = default;

    Vector2 coefXA;
    Vector2 coefXB;
    Vector2 coefCA;
    Vector2 coefCB;
  };

public: // Default properties
  // Define a base offset for the following wrappers. The wrapper macros calculate offsets from the previous
  // element such that each wrapper type generates a compile time offset to the CoefficientCache data.
  BASE(VisualTransformedUpdateSizeCoefficientCache, mCoefficient); ///< Coefficient value to calculate visual transformed update size by VisualProperties more faster.

  PROPERTY_WRAPPER(mCoefficient, VisualRendererProperty, Vector2, mTransformOffset);
  PROPERTY_WRAPPER(mTransformOffset, VisualRendererProperty, Vector2, mTransformSize);
  PROPERTY_WRAPPER(mTransformSize, VisualRendererProperty, Vector2, mTransformOrigin);
  PROPERTY_WRAPPER(mTransformOrigin, VisualRendererProperty, Vector2, mTransformAnchorPoint);
  PROPERTY_WRAPPER(mTransformAnchorPoint, VisualRendererProperty, Vector4, mTransformOffsetSizeMode);
  PROPERTY_WRAPPER(mTransformOffsetSizeMode, VisualRendererProperty, Vector2, mExtraSize);

  // Properties that don't give any effort to coefficient.
};
} // namespace VisualRenderer
} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_H
