/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

namespace Dali::Internal::SceneGraph::VisualRenderer
{
void AnimatableVisualProperties::ResetToBaseValues(BufferIndex updateBufferIndex)
{
  mTransformOffset.ResetToBaseValue(updateBufferIndex);
  mTransformSize.ResetToBaseValue(updateBufferIndex);
  mTransformOrigin.ResetToBaseValue(updateBufferIndex);
  mTransformAnchorPoint.ResetToBaseValue(updateBufferIndex);
  mTransformOffsetSizeMode.ResetToBaseValue(updateBufferIndex);
  mExtraSize.ResetToBaseValue(updateBufferIndex);
  mMixColor.ResetToBaseValue(updateBufferIndex);
  mPreMultipliedAlpha.ResetToBaseValue(updateBufferIndex);
  if(mExtendedProperties)
  {
    auto* decoratedVisualProperties = static_cast<VisualRenderer::AnimatableDecoratedVisualProperties*>(mExtendedProperties);
    decoratedVisualProperties->ResetToBaseValues(updateBufferIndex);
  }
}

void AnimatableVisualProperties::MarkAsDirty()
{
  mTransformOffset.MarkAsDirty();
  mTransformSize.MarkAsDirty();
  mTransformOrigin.MarkAsDirty();
  mTransformAnchorPoint.MarkAsDirty();
  mTransformOffsetSizeMode.MarkAsDirty();
  mExtraSize.MarkAsDirty();
  mMixColor.MarkAsDirty();
  mPreMultipliedAlpha.MarkAsDirty();
  if(mExtendedProperties)
  {
    auto* decoratedVisualProperties = static_cast<VisualRenderer::AnimatableDecoratedVisualProperties*>(mExtendedProperties);
    decoratedVisualProperties->MarkAsDirty();
  }
}

void AnimatableDecoratedVisualProperties::ResetToBaseValues(BufferIndex updateBufferIndex)
{
  mCornerRadius.ResetToBaseValue(updateBufferIndex);
  mCornerRadiusPolicy.ResetToBaseValue(updateBufferIndex);
  mBorderlineWidth.ResetToBaseValue(updateBufferIndex);
  mBorderlineColor.ResetToBaseValue(updateBufferIndex);
  mBorderlineOffset.ResetToBaseValue(updateBufferIndex);
  mBlurRadius.ResetToBaseValue(updateBufferIndex);
}

void AnimatableDecoratedVisualProperties::MarkAsDirty()
{
  mCornerRadius.MarkAsDirty();
  mCornerRadiusPolicy.MarkAsDirty();
  mBorderlineWidth.MarkAsDirty();
  mBorderlineColor.MarkAsDirty();
  mBorderlineOffset.MarkAsDirty();
  mBlurRadius.MarkAsDirty();
}

} // namespace Dali::Internal::SceneGraph::VisualRenderer