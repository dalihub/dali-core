#ifndef DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_H
#define DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_H

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

// EXTERNAL INCLUDES
#include <stdint.h> ///< For size_t

// INTERNAL INCLUDES
#include <dali/internal/update/nodes/node-helper.h> ///< For property wrapper macro
#include <dali/internal/update/rendering/scene-graph-visual-renderer-property.h>

namespace Dali::Internal::SceneGraph::VisualRenderer
{
struct AnimatableVisualProperties
{
  AnimatableVisualProperties()
  : mTransformOffset(Vector2::ZERO),
    mTransformSize(Vector2::ONE),
    mTransformOrigin(Vector2::ZERO),
    mTransformAnchorPoint(Vector2::ZERO),
    mTransformOffsetSizeMode(Vector4::ZERO),
    mExtraSize(Vector2::ZERO),
    mMixColor(Vector3::ONE),
    mPreMultipliedAlpha(0.0f),
    mExtendedPropertiesDeleteFunction(nullptr)
  {
  }

  ~AnimatableVisualProperties()
  {
    if(mExtendedProperties && mExtendedPropertiesDeleteFunction)
    {
      mExtendedPropertiesDeleteFunction(mExtendedProperties);
    }
  }

public: // Public API
  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::RequestResetToBaseValues
   */
  void RequestResetToBaseValues();

  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::Updated
   */
  bool Updated() const;

  /**
   * @copydoc RenderDataProvider::GetVisualTransformedUpdateArea()
   */
  Vector4 GetVisualTransformedUpdateArea(BufferIndex updateBufferIndex, const Vector4& originalUpdateArea) noexcept;

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
    VisualTransformedUpdateSizeCoefficientCache()
    : VisualRendererCoefficientCacheBase(),
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
  AnimatableProperty<Vector3> mMixColor;
  AnimatableProperty<float>   mPreMultipliedAlpha;

public:                                                      // Extended properties
  void* mExtendedProperties{nullptr};                        // Enable derived class to extend properties further
  void (*mExtendedPropertiesDeleteFunction)(void*){nullptr}; // Derived class's custom delete functor
};

struct AnimatableDecoratedVisualProperties
{
  AnimatableDecoratedVisualProperties()
  : mBorderlineWidth(0.0f),
    mBorderlineOffset(0.0f),
    mBlurRadius(0.0f),
    mBorderlineColor(Color::BLACK),
    mCornerRadius(Vector4::ZERO),
    mCornerRadiusPolicy(1.0f)
  {
  }
  ~AnimatableDecoratedVisualProperties()
  {
  }

public: // Public API
  // Delete function of AnimatableDecoratedVisualProperties* converted as void*
  static void DeleteFunction(void* data)
  {
    delete static_cast<AnimatableDecoratedVisualProperties*>(data);
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::RequestResetToBaseValues
   */
  void RequestResetToBaseValues();

  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::Updated
   */
  bool Updated() const;

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
    DecoratedVisualTransformedUpdateSizeCoefficientCache()
    : VisualRendererCoefficientCacheBase(),
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
  AnimatableProperty<float>   mCornerRadiusPolicy;
};
} // namespace Dali::Internal::SceneGraph::VisualRenderer

#endif // DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_H
