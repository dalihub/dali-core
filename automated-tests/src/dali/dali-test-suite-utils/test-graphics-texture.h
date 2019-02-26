#ifndef TEST_GRAPHICS_TEXTURE_H
#define TEST_GRAPHICS_TEXTURE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-texture.h>
#include <memory>

namespace Test
{
class GraphicsController;

struct GraphicsTextureCreateInfo
{
  Dali::Graphics::TextureDetails::Type type;
  Dali::Graphics::Extent2D size;
  Dali::Graphics::TextureDetails::Format format;
  Dali::Graphics::TextureDetails::MipMapFlag mipMapFlag;
  Dali::Graphics::TextureDetails::Usage usage;
  void* pData;
  uint32_t dataSizeInBytes;
};

class GraphicsTexture : public Dali::Graphics::Texture
{
public:
  explicit GraphicsTexture(GraphicsController& controller, const GraphicsTextureCreateInfo& createInfo);

  ~GraphicsTexture() override;

  void CopyMemory( const void *srcMemory, uint32_t srcMemorySize, Dali::Graphics::Extent2D srcExtent,
                   Dali::Graphics::Offset2D dstOffset, uint32_t layer, uint32_t level,
                   Dali::Graphics::TextureDetails::UpdateMode updateMode ) override;

  void CopyTexture( const Dali::Graphics::Texture &srcTexture, Dali::Graphics::Rect2D srcRegion,
                    Dali::Graphics::Offset2D dstOffset, uint32_t layer, uint32_t level,
                    Dali::Graphics::TextureDetails::UpdateMode updateMode ) override;

  void CopyBuffer( const Dali::Graphics::Buffer &srcBuffer,
                   uint32_t bufferOffset,
                   Dali::Graphics::Extent2D srcExtent,
                   Dali::Graphics::Offset2D dstOffset,
                   uint32_t layer,
                   uint32_t level,
                   Dali::Graphics::TextureUpdateFlags flags ) override;

  Dali::Graphics::MemoryRequirements GetMemoryRequirements() const override;

  const Dali::Graphics::TextureProperties& GetProperties() override;

public:
  GraphicsController& mController;
  GraphicsTextureCreateInfo mCreateInfo;
  std::unique_ptr<Dali::Graphics::TextureProperties> mProperties;
};

} // namespace Test

#endif // TEST_GRAPHICS_TEXTURE_H
