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

// EXTERNAL INCLUDES
#include <array>
#include <cstdlib>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/devel-api/rendering/frame-buffer-devel.h>
#include <dali/integration-api/texture-integ.h>
#include <dali/public-api/dali-core.h>
#include <test-native-image.h>

using namespace Dali;

namespace
{
void SetGpuMemoryProfileEnvironment(const char* value)
{
#if defined(_MSC_VER)
  _putenv_s("DALI_GPU_MEMORY_PROFILE_ENABLED", value ? value : "");
#else
  if(value)
  {
    setenv("DALI_GPU_MEMORY_PROFILE_ENABLED", value, 1);
  }
  else
  {
    unsetenv("DALI_GPU_MEMORY_PROFILE_ENABLED");
  }
#endif
}
} // namespace

void texture_gpu_memory_profile_startup(void)
{
  test_return_value = TET_UNDEF;
  SetGpuMemoryProfileEnvironment("1");
}

void texture_gpu_memory_profile_cleanup(void)
{
  SetGpuMemoryProfileEnvironment(nullptr);
  test_return_value = TET_PASS;
}

int UtcDaliTextureGpuMemoryProfilePrint(void)
{
  TestApplication application;

  const std::array<Integration::TextureContextTypeHint::Type, 8u> hints = {
    Integration::TextureContextTypeHint::UNKNOWN,
    Integration::TextureContextTypeHint::STANDARD_IMAGE,
    Integration::TextureContextTypeHint::TEXT_SIMPLE_LABEL,
    Integration::TextureContextTypeHint::NATIVE_IMAGE,
    Integration::TextureContextTypeHint::FBO_ATTACHED_COLOR_TEXTURE,
    Integration::TextureContextTypeHint::SCENE3D_STANDARD_IMAGE,
    Integration::TextureContextTypeHint::GRADIENT_TEXTURE,
    static_cast<Integration::TextureContextTypeHint::Type>(10000)};

  std::vector<Texture> textures;
  textures.reserve(hints.size());
  for(auto hint : hints)
  {
    Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
    Integration::TextureUploadWithContent(texture, PixelData(), std::string(90u, 'x').c_str(), hint, true);
    textures.push_back(texture);
  }

  auto*     pixels = static_cast<unsigned char*>(std::calloc(64u, 1u));
  PixelData data   = PixelData::New(pixels, 64u, 4u, 4u, Pixel::RGBA8888, PixelData::FREE);
  DALI_TEST_CHECK(Integration::TextureUploadWithContent(textures[1], data, "Image upload", Integration::TextureContextTypeHint::STANDARD_IMAGE));
  Integration::TextureUploadWithContent(textures[1], PixelData(), "ignored", Integration::TextureContextTypeHint::UNKNOWN, true, true);

  TestNativeImagePointer nativeImage   = TestNativeImage::New(4u, 4u);
  Texture                nativeTexture = Texture::New(*nativeImage);
  DALI_TEST_CHECK(nativeTexture);
  DALI_TEST_CHECK(!Integration::TextureUploadWithContent(nativeTexture, data, "Native image", Integration::TextureContextTypeHint::NATIVE_IMAGE));

  Texture resourceTexture = Integration::NewTextureWithResourceId(TextureType::TEXTURE_2D, 1234u);
  DALI_TEST_CHECK(resourceTexture);
  DALI_TEST_CHECK(!Integration::TextureUploadWithContent(resourceTexture, data, "Resource id", Integration::TextureContextTypeHint::STANDARD_IMAGE));
  Integration::SetTextureSize(resourceTexture, 4u, 4u);
  Integration::SetTexturePixelFormat(resourceTexture, Pixel::RGBA8888);

  Texture repeatedUpload = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  DALI_TEST_CHECK(Integration::TextureUploadWithContent(repeatedUpload, data, "(TextAtlas)", Integration::TextureContextTypeHint::TEXT_ATLAS));
  DALI_TEST_CHECK(repeatedUpload.Upload(data));
  DALI_TEST_CHECK(repeatedUpload.Upload(data, 0u, 0u, 0u, 0u, 4u, 4u));

  FrameBuffer colorBuffer = FrameBuffer::New(4u, 4u, FrameBuffer::Attachment::NONE);
  colorBuffer.AttachColorTexture(textures[1]);
  FrameBuffer depthBuffer  = FrameBuffer::New(4u, 4u, FrameBuffer::Attachment::DEPTH);
  Texture     depthTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::DEPTH_FLOAT, 4u, 4u);
  DevelFrameBuffer::AttachDepthTexture(depthBuffer, depthTexture);
  FrameBuffer stencilBuffer  = FrameBuffer::New(4u, 4u, FrameBuffer::Attachment::STENCIL);
  Texture     stencilTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::DEPTH_STENCIL, 4u, 4u);
  DevelFrameBuffer::AttachDepthStencilTexture(stencilBuffer, stencilTexture);

  // Each fake timer tick advances the profiler's three-second print cycle.
  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
    Integration::TextureUploadWithContent(textures[0], PixelData(), "Timer cycle", Integration::TextureContextTypeHint::UNKNOWN, true);
  }

  textures.clear();
  application.GetPlatform().TriggerTimer();
  DALI_TEST_CHECK(true);
  END_TEST;
}
