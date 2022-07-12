/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/internal/event/rendering/texture-impl.h>

namespace Dali
{
namespace DevelTexture
{
bool IsNative(Dali::Texture texture)
{
  auto& impl = GetImplementation(texture);
  return impl.IsNative();
}

bool ApplyNativeFragmentShader(Dali::Texture texture, std::string& shader)
{
  auto& impl = GetImplementation(texture);
  return impl.ApplyNativeFragmentShader(shader);
}

bool UploadSubPixelData(Texture   texture,
                        PixelData pixelData,
                        uint32_t  dataXOffset,
                        uint32_t  dataYOffset,
                        uint32_t  dataWidth,
                        uint32_t  dataHeight)
{
  Internal::PixelData& internalPixelData = GetImplementation(pixelData);
  return GetImplementation(texture).UploadSubPixelData(&internalPixelData, dataXOffset, dataYOffset, dataWidth, dataHeight);
}

bool UploadSubPixelData(Texture   texture,
                        PixelData pixelData,
                        uint32_t  dataXOffset,
                        uint32_t  dataYOffset,
                        uint32_t  dataWidth,
                        uint32_t  dataHeight,
                        uint32_t  layer,
                        uint32_t  mipmap,
                        uint32_t  xOffset,
                        uint32_t  yOffset,
                        uint32_t  width,
                        uint32_t  height)
{
  Internal::PixelData& internalPixelData = GetImplementation(pixelData);
  return GetImplementation(texture).UploadSubPixelData(&internalPixelData, dataXOffset, dataYOffset, dataWidth, dataHeight, layer, mipmap, xOffset, yOffset, width, height);
}

} // namespace DevelTexture
} // namespace Dali
