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
 *
 */

// CLASS HEADER
#include <dali/public-api/rendering/decorated-visual-renderer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/decorated-visual-renderer-impl.h>

namespace Dali
{
DecoratedVisualRenderer DecoratedVisualRenderer::New(Geometry& geometry, Shader& shader)
{
  Internal::DecoratedVisualRendererPtr renderer = Internal::DecoratedVisualRenderer::New();
  renderer->SetGeometry(GetImplementation(geometry));
  renderer->SetShader(GetImplementation(shader));
  return DecoratedVisualRenderer(renderer.Get());
}

DecoratedVisualRenderer::DecoratedVisualRenderer() = default;

DecoratedVisualRenderer::~DecoratedVisualRenderer() = default;

DecoratedVisualRenderer::DecoratedVisualRenderer(const DecoratedVisualRenderer& handle) = default;

DecoratedVisualRenderer DecoratedVisualRenderer::DownCast(BaseHandle handle)
{
  return DecoratedVisualRenderer(dynamic_cast<Dali::Internal::DecoratedVisualRenderer*>(handle.GetObjectPtr()));
}

DecoratedVisualRenderer& DecoratedVisualRenderer::operator=(const DecoratedVisualRenderer& handle) = default;

DecoratedVisualRenderer::DecoratedVisualRenderer(DecoratedVisualRenderer&& rhs) noexcept = default;

DecoratedVisualRenderer& DecoratedVisualRenderer::operator=(DecoratedVisualRenderer&& rhs) noexcept = default;

void DecoratedVisualRenderer::RegisterCornerRadiusUniform()
{
  GetImplementation(*this).RegisterCornerRadiusUniform();
}

void DecoratedVisualRenderer::RegisterCornerSquarenessUniform()
{
  GetImplementation(*this).RegisterCornerSquarenessUniform();
}

void DecoratedVisualRenderer::RegisterBorderlineUniform()
{
  GetImplementation(*this).RegisterBorderlineUniform();
}

void DecoratedVisualRenderer::RegisterBlurRadiusUniform()
{
  GetImplementation(*this).RegisterBlurRadiusUniform();
}

DecoratedVisualRenderer::DecoratedVisualRenderer(Internal::DecoratedVisualRenderer* pointer)
: Dali::VisualRenderer(pointer)
{
}

} //namespace Dali
