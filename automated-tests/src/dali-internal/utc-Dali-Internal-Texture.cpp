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

#include <dali-test-suite-utils.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/texture-integ.h>
#include <dali/public-api/dali-core.h>

using namespace Dali;

void utc_dali_internal_texture_set_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_texture_set_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextureUploadByResourceId(void)
{
  TestApplication application;

  uint32_t  width(4);
  uint32_t  height(4);
  uint32_t  bufferSize(width * height * 4);
  uint8_t*  buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::FREE);

  // Run two cases.
  for(int tc = 0; tc < 2; tc++)
  {
    uint32_t expectResourceId = 11u + tc;

    Texture texture = Dali::Integration::NewTextureWithResourceId(TextureType::TEXTURE_2D, expectResourceId);
    Actor   actor   = CreateRenderableActor(texture);

    application.GetScene().Add(actor);

    DALI_TEST_CHECK(texture);

    uint32_t currentResourceId = Dali::Integration::GetTextureResourceId(texture);

    DALI_TEST_EQUALS(currentResourceId, expectResourceId, TEST_LOCATION);

    if(tc == 0)
    {
      // Scene on before upload.
      application.SendNotification();
      application.Render(16);
    }

    auto& graphicsController = application.GetGraphicsController();

    Graphics::Texture* graphicsTexture = nullptr;

    tet_printf("CreateTextureByResourceId\n");
    {
      auto createInfo = Graphics::TextureCreateInfo();
      createInfo
        .SetTextureType(Dali::Graphics::ConvertTextureType(Dali::TextureType::TEXTURE_2D))
        .SetUsageFlags(static_cast<Graphics::TextureUsageFlags>(Graphics::TextureUsageFlagBits::SAMPLE))
        .SetFormat(Dali::Graphics::ConvertPixelFormat(pixelData.GetPixelFormat()))
        .SetSize({pixelData.GetWidth(), pixelData.GetHeight()})
        .SetLayout(Graphics::TextureLayout::LINEAR)
        .SetData(nullptr)
        .SetDataSize(0u)
        .SetNativeImage(nullptr)
        .SetMipMapFlag(Graphics::TextureMipMapFlag::DISABLED);

      graphicsTexture = graphicsController.CreateTextureByResourceId(currentResourceId, createInfo);
    }

    DALI_TEST_CHECK(graphicsTexture != nullptr);

    tet_printf("Upload\n");
    if(graphicsTexture)
    {
      Graphics::TextureUpdateInfo info{};

      info.dstTexture   = graphicsTexture;
      info.dstOffset2D  = {0u, 0u};
      info.layer        = 0u;
      info.level        = 0u;
      info.srcReference = 0;
      info.srcExtent2D  = {pixelData.GetWidth(), pixelData.GetHeight()};
      info.srcOffset    = 0;
      info.srcSize      = Dali::Integration::GetPixelDataBuffer(pixelData).bufferSize;
      info.srcStride    = pixelData.GetStrideBytes() ? pixelData.GetStrideBytes() / Dali::Pixel::GetBytesPerPixel(pixelData.GetPixelFormat()) : 0u;
      info.srcFormat    = Dali::Graphics::ConvertPixelFormat(pixelData.GetPixelFormat());

      Graphics::TextureUpdateSourceInfo updateSourceInfo{};
      updateSourceInfo.sourceType                = Graphics::TextureUpdateSourceInfo::Type::PIXEL_DATA;
      updateSourceInfo.pixelDataSource.pixelData = pixelData;

      graphicsController.UpdateTextures({info}, {updateSourceInfo});
    }

    tet_printf("Flush\n");
    {
      Graphics::SubmitInfo submitInfo;
      submitInfo.cmdBuffer.clear(); // Only flush
      submitInfo.flags = 0 | Graphics::SubmitFlagBits::FLUSH;
      graphicsController.SubmitCommandBuffers(submitInfo);
    }

    if(tc == 1)
    {
      // Scene on after upload.
      application.SendNotification();
      application.Render(16);
    }

    actor.Unparent();
  }

  END_TEST;
}