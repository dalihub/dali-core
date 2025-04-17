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
 *
 */

// CLASS HEADER
#include <dali/public-api/rendering/visual-renderer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/visual-renderer-impl.h>

namespace Dali
{
VisualRenderer VisualRenderer::New(Geometry& geometry, Shader& shader)
{
  Internal::VisualRendererPtr renderer = Internal::VisualRenderer::New();
  renderer->SetGeometry(GetImplementation(geometry));
  renderer->SetShader(GetImplementation(shader));
  return VisualRenderer(renderer.Get());
}

VisualRenderer::VisualRenderer() = default;

VisualRenderer::~VisualRenderer() = default;

VisualRenderer::VisualRenderer(const VisualRenderer& handle) = default;

VisualRenderer VisualRenderer::DownCast(BaseHandle handle)
{
  return VisualRenderer(dynamic_cast<Dali::Internal::VisualRenderer*>(handle.GetObjectPtr()));
}

VisualRenderer& VisualRenderer::operator=(const VisualRenderer& handle) = default;

VisualRenderer::VisualRenderer(VisualRenderer&& rhs) noexcept = default;

VisualRenderer& VisualRenderer::operator=(VisualRenderer&& rhs) noexcept = default;

void VisualRenderer::RegisterVisualTransformUniform()
{
  GetImplementation(*this).RegisterVisualTransformUniform();
}

VisualRenderer::VisualRenderer(Internal::VisualRenderer* pointer)
: Dali::Renderer(pointer)
{
}

} //namespace Dali
