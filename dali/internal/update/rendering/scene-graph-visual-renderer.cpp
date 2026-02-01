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

// CLASS HEADER
#include <dali/internal/update/rendering/scene-graph-visual-renderer.h>

// INTERNAL INCLUDES
#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

namespace Dali::Internal::SceneGraph
{
#ifdef DEBUG_ENABLED
extern Debug::Filter* gSceneGraphRendererLogFilter; ///< Defined at scene-graph-renderer.cpp
#endif
namespace VisualRenderer
{
void VisualProperties::GetVisualTransformedUpdateArea(BufferIndex updateBufferIndex, Vector4& updateArea) noexcept
{
  auto& coefficient = mCoefficient;

  // Recalculate only if coefficient need to be updated.
  if(!coefficient.IsCoefficientCalculated())
  {
    // VisualProperty
    const Vector2 transformOffset         = mTransformOffset.Get(updateBufferIndex);
    const Vector4 transformOffsetSizeMode = mTransformOffsetSizeMode.Get(updateBufferIndex);
    const Vector2 transformSize           = mTransformSize.Get(updateBufferIndex);
    const Vector2 transformOrigin         = mTransformOrigin.Get(updateBufferIndex);
    const Vector2 transformAnchorPoint    = mTransformAnchorPoint.Get(updateBufferIndex);
    const Vector2 extraSize               = mExtraSize.Get(updateBufferIndex);

    DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "transform size   %5.3f %5.3f\n", transformSize.x, transformSize.y);
    DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "transform offset %5.3f %5.3f\n", transformOffset.x, transformOffset.y);
    DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "transform origin %5.3f %5.3f\n", transformOrigin.x, transformOrigin.y);
    DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "transform anchor %5.3f %5.3f\n", transformAnchorPoint.x, transformAnchorPoint.y);
    DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "extra size       %5.3f %5.3f\n", extraSize.x, extraSize.y);

    // const Vector2 visualSize = Vector2(Dali::Lerp(transformOffsetSizeMode.z, originalWH.x * transformSize.x, transformSize.x),
    //                                    Dali::Lerp(transformOffsetSizeMode.w, originalWH.y * transformSize.y, transformSize.y)) +
    //                            extraSize;
    // const Vector2 visualOffset = Vector2(Dali::Lerp(transformOffsetSizeMode.x, originalWH.x * transformOffset.x, transformOffset.x),
    //                                      Dali::Lerp(transformOffsetSizeMode.y, originalWH.y * transformOffset.y, transformOffset.y)) +
    //                              originalXY;

    // const float decoratedBorderlineWidth = std::max((1.0f + Dali::Clamp(borderlineOffset, -1.0f, 1.0f)) * borderlineWidth, 2.0f * blurRadius);
    // const Vector2 decoratedVisualSize    = visualSize + Vector2(decoratedBorderlineWidth, decoratedBorderlineWidth);

    // Note : vertexPosition.xy = aPosition * decoratedVisualSize
    //                          + anchorPoint * visualSize
    //                          + origin * uSize.xy
    //                          + visualOffset;

    // Calculate same logic of visual's vertex shader transform.
    // minVertexPosition = -0.5f * decoratedVisualSize + transformAnchorPoint * visualSize + transformOrigin * originalWH.xy + visualOffset
    // maxVertexPosition =  0.5f * decoratedVisualSize + transformAnchorPoint * visualSize + transformOrigin * originalWH.xy + visualOffset

    // Update cached VisualTransformedUpdateSizeCoefficientCache

    // Note : vertexPosition = (XA * aPosition + XB) * originalWH + (CA * aPosition + CB) + Vector2(D, D) * aPosition + originalXY

    // XA = transformSize * (1.0 - transformOffsetSizeMode.zw)
    // XB = transformSize * (1.0 - transformOffsetSizeMode.zw) * transformAnchorPoint
    //    + transformOffset * (1.0 - transformOffsetSizeMode.xy)
    //    + transformOrigin
    // CA = transformSize * transformOffsetSizeMode.zw + extraSize
    // CB = (transformSize * transformOffsetSizeMode.zw + extraSize) * transformAnchorPoint
    //    + transformOffset * transformOffsetSizeMode.xy
    // D = max((1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth, 2.0 * blurRadius)

    coefficient.coefXA = transformSize * Vector2(1.0f - transformOffsetSizeMode.z, 1.0f - transformOffsetSizeMode.w);
    coefficient.coefXB = coefficient.coefXA * transformAnchorPoint + transformOffset * Vector2(1.0f - transformOffsetSizeMode.x, 1.0f - transformOffsetSizeMode.y) + transformOrigin;
    coefficient.coefCA = transformSize * Vector2(transformOffsetSizeMode.z, transformOffsetSizeMode.w) + extraSize;
    coefficient.coefCB = coefficient.coefCA * transformAnchorPoint + transformOffset * Vector2(transformOffsetSizeMode.x, transformOffsetSizeMode.y);

    coefficient.MarkCoefficientCalculated();
  }

  // Calculate vertex position by coefficient
  // It will reduce the number of operations

  // const Vector2 minVertexPosition = (XA * -0.5 + XB) * originalWH + (CA * -0.5 + CB) + Vector2(D, D) * -0.5 + originalXY;
  // const Vector2 maxVertexPosition = (XA * +0.5 + XB) * originalWH + (CA * +0.5 + CB) + Vector2(D, D) * +0.5 + originalXY;

  // When we set
  // basicVertexPosition = XB * originalWH + CB + originalXY
  // scaleVertexPosition = XA * originalWH + CA + D

  // --> minVertexPosition = basicVertexPosition + scaleVertexPosition * -0.5
  //     maxVertexPosition = basicVertexPosition + scaleVertexPosition * +0.5

  // Then, resultPosition = basicVertexPosition
  //       resultSize     = scaleVertexPosition

  const Vector2 originalXY = Vector2(updateArea.x, updateArea.y);
  const Vector2 originalWH = Vector2(updateArea.z, updateArea.w);

  const Vector2 basicVertexPosition = coefficient.coefXB * originalWH + coefficient.coefCB + originalXY;
  const Vector2 scaleVertexPosition = coefficient.coefXA * originalWH + coefficient.coefCA;

  updateArea = Vector4(basicVertexPosition.x,
                       basicVertexPosition.y,
                       scaleVertexPosition.x,
                       scaleVertexPosition.y);

  DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "%f %f %f %f--> %f %f %f %f\n", originalXY.x, originalXY.y, originalWH.width, originalWH.height, updateArea.x, updateArea.y, updateArea.z, updateArea.w);
}

bool VisualProperties::PrepareProperties()
{
  bool rendererUpdated = mCoefficient.IsUpdated();
  mCoefficient.ResetFlag();
  return rendererUpdated;
}

void DecoratedVisualProperties::GetVisualTransformedUpdateArea(BufferIndex updateBufferIndex, Vector4& updateArea) noexcept
{
  auto& decoratedCoefficient = mCoefficient;

  // Recalculate only if coefficient need to be updated.
  if(!decoratedCoefficient.IsCoefficientCalculated())
  {
    // DecoratedVisualProperty
    const float borderlineWidth  = mBorderlineWidth.Get(updateBufferIndex);
    const float borderlineOffset = mBorderlineOffset.Get(updateBufferIndex);
    const float blurRadius       = mBlurRadius.Get(updateBufferIndex);

    // Extra padding information for anti-alias
    const float extraPadding = 2.0f;

    DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "borderline width  %5.3f\n", borderlineWidth);
    DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "borderline offset %5.3f\n", borderlineOffset);
    DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "blur radius       %5.3f\n", blurRadius);

    // D coefficients be used only decoratedVisual.
    // It can be calculated parallely with visual transform.
    decoratedCoefficient.coefD = (1.0f + Dali::Clamp(borderlineOffset, -1.0f, 1.0f)) * borderlineWidth + (2.0f * blurRadius) + extraPadding;

    decoratedCoefficient.MarkCoefficientCalculated();
  }

  DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "%f %f %f %f--> %f %f %f %f\n", updateArea.x, updateArea.y, updateArea.z, updateArea.w, updateArea.x, updateArea.y, updateArea.z + decoratedCoefficient.coefD, updateArea.w + decoratedCoefficient.coefD);

  updateArea.z += decoratedCoefficient.coefD;
  updateArea.w += decoratedCoefficient.coefD;
}

bool DecoratedVisualProperties::PrepareProperties()
{
  bool rendererUpdated = mCoefficient.IsUpdated();
  mCoefficient.ResetFlag();
  return rendererUpdated;
}

} // namespace VisualRenderer
} // namespace Dali::Internal::SceneGraph