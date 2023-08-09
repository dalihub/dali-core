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
 *
 */

// FILE HEADER
#include <dali/integration-api/texture-integ.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/texture-impl.h>

namespace Dali::Integration
{
Dali::Texture NewTextureWithResourceId(Dali::TextureType::Type type, uint32_t resourceId)
{
  Internal::TexturePtr texture = Internal::Texture::New(type, resourceId);
  return Texture(texture.Get());
}

uint32_t GetTextureResourceId(Dali::Texture texture)
{
  return GetImplementation(texture).GetResourceId();
}

Dali::TextureType::Type GetTextureType(Dali::Texture texture)
{
  return GetImplementation(texture).GetTextureType();
}

void SetTextureSize(Dali::Texture texture, const Dali::ImageDimensions& size)
{
  GetImplementation(texture).SetSize(size);
}

void SetTexturePixelFormat(Dali::Texture texture, Dali::Pixel::Format format)
{
  GetImplementation(texture).SetPixelFormat(format);
}

} // namespace Dali::Integration
