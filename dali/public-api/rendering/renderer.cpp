/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/renderer.h> // Dali::Renderer

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/renderer-impl.h> // Dali::Internal::Renderer

namespace Dali
{
Renderer Renderer::New(Geometry& geometry, Shader& shader)
{
  Internal::RendererPtr renderer = Internal::Renderer::New();
  renderer->SetGeometry(GetImplementation(geometry));
  renderer->SetShader(GetImplementation(shader));
  return Renderer(renderer.Get());
}

Renderer::Renderer() = default;

Renderer::~Renderer() = default;

Renderer::Renderer(const Renderer& handle) = default;

Renderer Renderer::DownCast(BaseHandle handle)
{
  return Renderer(dynamic_cast<Dali::Internal::Renderer*>(handle.GetObjectPtr()));
}

Renderer& Renderer::operator=(const Renderer& handle) = default;

Renderer::Renderer(Renderer&& rhs) = default;

Renderer& Renderer::operator=(Renderer&& rhs) = default;

void Renderer::SetGeometry(Geometry& geometry)
{
  DALI_ASSERT_ALWAYS(geometry && "Geometry handle not initialized");
  GetImplementation(*this).SetGeometry(GetImplementation(geometry));
}

Geometry Renderer::GetGeometry() const
{
  return Dali::Geometry(GetImplementation(*this).GetGeometry().Get());
}

void Renderer::SetTextures(TextureSet& textureSet)
{
  DALI_ASSERT_ALWAYS(textureSet && "TextureSet handle not initialized");
  GetImplementation(*this).SetTextures(GetImplementation(textureSet));
}

TextureSet Renderer::GetTextures() const
{
  return Dali::TextureSet(GetImplementation(*this).GetTextures().Get());
}

void Renderer::SetShader(Shader& shader)
{
  DALI_ASSERT_ALWAYS(shader && "Shader handle not initialized");
  GetImplementation(*this).SetShader(GetImplementation(shader));
}

Shader Renderer::GetShader() const
{
  return Dali::Shader(GetImplementation(*this).GetShader().Get());
}

Renderer::Renderer(Internal::Renderer* pointer)
: Handle(pointer)
{
}

} //namespace Dali
