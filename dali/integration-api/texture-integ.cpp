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
 *
 */

// FILE HEADER
#include <dali/integration-api/texture-integ.h>

// EXTERNAL INCLUDES
#include <cstdlib>

// INTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>
#include <dali/internal/event/rendering/texture-impl.h>

namespace DALI_NAMESPACE::Integration
{
bool IsGpuMemoryProfileEnabled()
{
  static const bool enabled = []
  {
#if defined(_MSC_VER)
    char*       value  = nullptr;
    std::size_t length = 0u;
    if(_dupenv_s(&value, &length, "DALI_GPU_MEMORY_PROFILE_ENABLED") == 0 && value != nullptr)
    {
      const bool enabled = value[0] == '1' && value[1] == '\0';
      std::free(value);
      return enabled;
    }
#else
    const char* value = std::getenv("DALI_GPU_MEMORY_PROFILE_ENABLED");
    if(value != nullptr)
    {
      return value[0] == '1' && value[1] == '\0';
    }
#endif
#if defined(GPU_MEMORY_PROFILE_ENABLED)
    // Keep the old behavior for backward compatibility, but it is recommended to use the environment variable instead.
    return true;
#else
    return false;
#endif
  }();
  return enabled;
}

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

void SetTextureSize(Dali::Texture texture, uint16_t width, uint16_t height)
{
  GetImplementation(texture).SetSize(Dali::Internal::ImageDimensions(width, height));
}

void SetTexturePixelFormat(Dali::Texture texture, Dali::Pixel::Format format)
{
  GetImplementation(texture).SetPixelFormat(format);
}

void SetTextureAlphaPreMultiplied(Dali::Texture texture, bool preMultiplied)
{
  GetImplementation(texture).SetAlphaPreMultiplied(preMultiplied);
}

bool TextureUploadWithContent(Dali::Texture texture, Dali::PixelData pixelData, Dali::String content, TextureContextTypeHint::Type typeHint, bool updateContextOnly, bool keepPreviousContext)
{
  Internal::PixelData* internalPixelData = pixelData ? &GetImplementation(pixelData) : nullptr;
  if(DALI_UNLIKELY(IsGpuMemoryProfileEnabled()))
  {
    return GetImplementation(texture).Upload(internalPixelData, Integration::ToStdString(std::move(content)), typeHint, updateContextOnly, keepPreviousContext);
  }
  return GetImplementation(texture).UploadWithoutContent(internalPixelData, updateContextOnly);
}

} //namespace DALI_NAMESPACE::Integration
