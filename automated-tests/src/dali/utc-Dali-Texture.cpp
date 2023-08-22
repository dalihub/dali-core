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

#include <dali-test-suite-utils.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/integration-api/texture-integ.h>
#include <dali/public-api/dali-core.h>
#include <test-native-image.h>

using namespace Dali;

#include <mesh-builder.h>

void texture_set_startup(void)
{
  test_return_value = TET_UNDEF;
}

void texture_set_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextureNew01(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  Texture      texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  DALI_TEST_CHECK(texture);
  END_TEST;
}

int UtcDaliTextureNew02(void)
{
  TestApplication application;
  Texture         texture;
  DALI_TEST_CHECK(!texture);
  END_TEST;
}

int UtcDaliTextureNew03(void)
{
  TestApplication application;

  // Create a native image source.
  TestNativeImagePointer testNativeImage = TestNativeImage::New(64u, 64u);

  // Create a texture from the native image source.
  Texture nativeTexture = Texture::New(*testNativeImage);

  // Check the texture was created OK.
  DALI_TEST_CHECK(nativeTexture);

  END_TEST;
}

int UtcDaliTextureNew04(void)
{
  TestApplication application;

  Texture texture = Texture::New(TextureType::TEXTURE_2D);

  DALI_TEST_CHECK(texture);
  END_TEST;
}

int UtcDaliTextureNew05(void)
{
  TestApplication application;

  uint32_t expectResourceId = 11u;

  Texture texture = Integration::NewTextureWithResourceId(TextureType::TEXTURE_2D, expectResourceId);

  DALI_TEST_CHECK(texture);

  uint32_t currentResourceId = Integration::GetTextureResourceId(texture);

  DALI_TEST_EQUALS(currentResourceId, expectResourceId, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureCopyConstructor(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  Texture      texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  Texture textureCopy(texture);

  DALI_TEST_CHECK(textureCopy);

  END_TEST;
}

int UtcDaliTextureAssignmentOperator(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);
  Texture         texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  Texture texture2;
  DALI_TEST_CHECK(!texture2);

  texture2 = texture;
  DALI_TEST_CHECK(texture2);

  END_TEST;
}

int UtcDaliTextureMoveConstructor(void)
{
  TestApplication application;

  uint32_t width   = 64;
  uint32_t height  = 64;
  Texture  texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUALS(1, texture.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetHeight(), height, TEST_LOCATION);

  Texture move = std::move(texture);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetHeight(), height, TEST_LOCATION);
  DALI_TEST_CHECK(!texture);

  END_TEST;
}

int UtcDaliTextureMoveAssignment(void)
{
  TestApplication application;

  uint32_t width   = 64;
  uint32_t height  = 64;
  Texture  texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUALS(1, texture.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetHeight(), height, TEST_LOCATION);

  Texture move;
  move = std::move(texture);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetHeight(), height, TEST_LOCATION);
  DALI_TEST_CHECK(!texture);

  END_TEST;
}

int UtcDaliTextureDownCast01(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);
  Texture         texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  BaseHandle handle(texture);
  Texture    texture2 = Texture::DownCast(handle);
  DALI_TEST_CHECK(texture2);

  END_TEST;
}

int UtcDaliTextureDownCast02(void)
{
  TestApplication application;

  Handle  handle  = Handle::New(); // Create a custom object
  Texture texture = Texture::DownCast(handle);
  DALI_TEST_CHECK(!texture);
  END_TEST;
}

int UtcDaliTextureUpload01(void)
{
  TestApplication application;

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);
  Texture      texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  application.GetGlAbstraction().EnableTextureCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

  //Upload data to the texture
  callStack.Reset();

  unsigned int   bufferSize(width * height * 4);
  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::FREE);
  texture.Upload(pixelData);
  application.SendNotification();
  application.Render();

  //TexImage2D should be called to upload the data
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  //Upload part of the texture
  callStack.Reset();
  bufferSize                  = width * height;
  buffer                      = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData pixelDataSubImage = PixelData::New(buffer, bufferSize, width / 2, height / 2, Pixel::RGBA8888, PixelData::FREE);
  texture.Upload(pixelDataSubImage, 0u, 0u, width / 2, height / 2, width / 2, height / 2);
  application.SendNotification();
  application.Render();

  //TexSubImage2D should be called to upload the data
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width / 2 << ", " << height / 2 << ", " << width / 2 << ", " << height / 2;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexSubImage2D", out.str().c_str()));
  }

  END_TEST;
}

int UtcDaliTextureUpload02(void)
{
  TestApplication application;

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);
  Texture      texture = CreateTexture(TextureType::TEXTURE_CUBE, Pixel::RGBA8888, width, height);

  application.GetGlAbstraction().EnableTextureCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

  tet_infoline("TexImage2D should be called six times with a null pointer to reserve storage for the six textures of the cube map");
  for(unsigned int i(0); i < 6; ++i)
  {
    std::stringstream out;
    out << GL_TEXTURE_CUBE_MAP_POSITIVE_X + i << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  unsigned int   bufferSize(width * height * 4);
  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::FREE);

  //Upload data to the POSITIVE_X face of the texture
  {
    callStack.Reset();

    texture.Upload(pixelData, CubeMapLayer::POSITIVE_X, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the POSITIVE_X face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_POSITIVE_X << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  //Upload data to the NEGATIVE_X face of the texture
  {
    callStack.Reset();

    texture.Upload(pixelData, CubeMapLayer::NEGATIVE_X, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the NEGATIVE_X face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_NEGATIVE_X << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  //Upload data to the POSITIVE_Y face of the texture
  {
    callStack.Reset();
    texture.Upload(pixelData, CubeMapLayer::POSITIVE_Y, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the POSITIVE_Y face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_POSITIVE_Y << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  //Upload data to the NEGATIVE_Y face of the texture
  {
    callStack.Reset();
    texture.Upload(pixelData, CubeMapLayer::NEGATIVE_Y, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the NEGATIVE_Y face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_NEGATIVE_Y << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  //Upload data to the POSITIVE_Z face of the texture
  {
    callStack.Reset();
    texture.Upload(pixelData, CubeMapLayer::POSITIVE_Z, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the POSITIVE_Z face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_POSITIVE_Z << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  //Upload data to the NEGATIVE_Z face of the texture
  {
    callStack.Reset();
    texture.Upload(pixelData, CubeMapLayer::NEGATIVE_Z, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the NEGATIVE_Z face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_NEGATIVE_Z << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  END_TEST;
}

int UtcDaliTextureUpload03(void)
{
  TestApplication application;

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);
  unsigned int widthMipmap1(32);
  unsigned int heightMipmap1(32);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  application.GetGlAbstraction().EnableTextureCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

  tet_infoline("TexImage2D should be called with a null pointer to reserve storage for the texture in the gpu");
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  //Upload data to the texture mipmap 0 and mipmap 1
  callStack.Reset();

  unsigned int   bufferSize(width * height * 4);
  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::FREE);
  texture.Upload(pixelData, 0u, 0u, 0u, 0u, width, height);

  bufferSize                 = widthMipmap1 * heightMipmap1 * 4;
  buffer                     = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData pixelDataMipmap1 = PixelData::New(buffer, bufferSize, widthMipmap1, heightMipmap1, Pixel::RGBA8888, PixelData::FREE);
  texture.Upload(pixelDataMipmap1, 0u, 1u, 0u, 0u, widthMipmap1, heightMipmap1);
  application.SendNotification();
  application.Render();

  //TexImage2D should be called to upload the data to mipmaps 0 and 1
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 1u << ", " << widthMipmap1 << ", " << heightMipmap1;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  END_TEST;
}

int UtcDaliTextureUpload04(void)
{
  TestApplication application;

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);
  unsigned int widthMipmap1(32);
  unsigned int heightMipmap1(32);

  Texture texture = CreateTexture(TextureType::TEXTURE_CUBE, Pixel::RGBA8888, width, height);

  application.GetGlAbstraction().EnableTextureCallTrace(true);
  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

  //Upload data to the NEGATIVE_X face mipmap 0 and mipmap 1
  unsigned int   bufferSize(width * height * 4);
  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::FREE);
  texture.Upload(pixelData, CubeMapLayer::NEGATIVE_X, 0u, 0u, 0u, width, height);

  bufferSize                 = widthMipmap1 * heightMipmap1 * 4;
  buffer                     = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData pixelDataMipmap1 = PixelData::New(buffer, bufferSize, widthMipmap1, heightMipmap1, Pixel::RGBA8888, PixelData::FREE);
  texture.Upload(pixelDataMipmap1, CubeMapLayer::NEGATIVE_X, 1u, 0u, 0u, widthMipmap1, heightMipmap1);
  application.SendNotification();
  application.Render();

  //TexImage2D should be called to upload the data to mipmaps 0 and 1
  {
    std::stringstream out;
    out << GL_TEXTURE_CUBE_MAP_NEGATIVE_X << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }
  {
    std::stringstream out;
    out << GL_TEXTURE_CUBE_MAP_NEGATIVE_X << ", " << 1u << ", " << widthMipmap1 << ", " << heightMipmap1;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  END_TEST;
}

int UtcDaliTextureUpload05(void)
{
  Pixel::Format COMPRESSED_PIXEL_FORMATS[] =
    {
      Pixel::COMPRESSED_R11_EAC,
      Pixel::COMPRESSED_SIGNED_R11_EAC,
      Pixel::COMPRESSED_RG11_EAC,
      Pixel::COMPRESSED_SIGNED_RG11_EAC,
      Pixel::COMPRESSED_RGB8_ETC2,
      Pixel::COMPRESSED_SRGB8_ETC2,
      Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
      Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
      Pixel::COMPRESSED_RGBA8_ETC2_EAC,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
      Pixel::COMPRESSED_RGB8_ETC1,
      Pixel::COMPRESSED_RGB_PVRTC_4BPPV1,
      Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR,
      Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,
      Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
    };
  const unsigned int NUMBER_OF_COMPRESSED_PIXEL_FORMATS = sizeof(COMPRESSED_PIXEL_FORMATS) / sizeof(Pixel::Format);

  for(unsigned int index = 0; index < NUMBER_OF_COMPRESSED_PIXEL_FORMATS; ++index)
  {
    TestApplication application;

    //Create a texture with a compressed format
    unsigned int width(64);
    unsigned int height(64);
    Texture      texture = CreateTexture(TextureType::TEXTURE_2D, COMPRESSED_PIXEL_FORMATS[index], width, height);

    application.GetGlAbstraction().EnableTextureCallTrace(true);

    application.SendNotification();
    application.Render();

    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

    tet_infoline("CompressedTexImage2D should be called with a null pointer to reserve storage for the texture in the gpu");
    {
      std::stringstream out;
      out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("CompressedTexImage2D", out.str().c_str()));
    }

    //Upload data to the texture
    callStack.Reset();

    unsigned int   bufferSize(width * height * 4);
    unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
    PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, COMPRESSED_PIXEL_FORMATS[index], PixelData::FREE);
    texture.Upload(pixelData);
    application.SendNotification();
    application.Render();

    //CompressedTexImage2D should be called to upload the data
    {
      std::stringstream out;
      out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("CompressedTexImage2D", out.str().c_str()));
    }

    //Upload part of the texture
    callStack.Reset();
    bufferSize                  = width * height;
    buffer                      = reinterpret_cast<unsigned char*>(malloc(bufferSize));
    PixelData pixelDataSubImage = PixelData::New(buffer, bufferSize, width / 2, height / 2, COMPRESSED_PIXEL_FORMATS[index], PixelData::FREE);
    texture.Upload(pixelDataSubImage, 0u, 0u, width / 2, height / 2, width / 2, height / 2);
    application.SendNotification();
    application.Render();

    //CompressedTexSubImage2D should be called to upload the data
    {
      std::stringstream out;
      out << GL_TEXTURE_2D << ", " << 0u << ", " << width / 2 << ", " << height / 2 << ", " << width / 2 << ", " << height / 2;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("CompressedTexSubImage2D", out.str().c_str()));
    }

    application.GetGlAbstraction().ResetTextureCallStack();
  }

  END_TEST;
}

int UtcDaliTextureUpload06(void)
{
  TestApplication application;

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);
  tet_infoline("Creating a Texure with an alpha channel");
  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  application.GetGlAbstraction().EnableTextureCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

  tet_infoline("TexImage2D should be called with a null pointer to reserve storage for the texture in the gpu");
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  tet_infoline("Upload data to the texture");
  callStack.Reset();

  tet_infoline("Creating a RGB pixel buffer and adding that to the texture to ensure it is handled correctly");
  unsigned int   bufferSize(width * height * 3);
  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGB888, PixelData::FREE);
  texture.Upload(pixelData);
  application.SendNotification();
  application.Render();

  tet_infoline("TexImage2D should be called to upload the data");
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  END_TEST;
}

int UtcDaliTextureUpload07(void)
{
  Pixel::Format FLOATING_POINT_PIXEL_FORMATS[] =
    {
      Pixel::RGB16F,
      Pixel::RGB32F,
    };
  const unsigned int NUMBER_OF_FLOATING_POINT_PIXEL_FORMATS = sizeof(FLOATING_POINT_PIXEL_FORMATS) / sizeof(Pixel::Format);

  for(unsigned int index = 0; index < NUMBER_OF_FLOATING_POINT_PIXEL_FORMATS; ++index)
  {
    TestApplication application;

    //Create the texture
    unsigned int width(64);
    unsigned int height(64);
    tet_infoline("Creating a floating point texture");
    Texture texture = CreateTexture(TextureType::TEXTURE_2D, FLOATING_POINT_PIXEL_FORMATS[index], width, height);

    application.GetGlAbstraction().EnableTextureCallTrace(true);

    application.SendNotification();
    application.Render();

    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

    tet_infoline("TexImage2D should be called with a null pointer to reserve storage for the texture in the gpu");
    {
      std::stringstream out;
      out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }

    tet_infoline("Upload data to the texture");
    callStack.Reset();

    tet_infoline("Creating a RGB pixel buffer and adding that to the texture to ensure it is handled correctly");
    unsigned int   bufferSize(width * height * 3);
    unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
    PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, FLOATING_POINT_PIXEL_FORMATS[index], PixelData::FREE);
    texture.Upload(pixelData);
    application.SendNotification();
    application.Render();

    tet_infoline("TexImage2D should be called to upload the data");
    {
      std::stringstream out;
      out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  END_TEST;
}

int UtcDaliTextureUpload08(void)
{
  TestApplication application;

  //Create the texture without pixel information
  tet_infoline("Creating a Texure without any size/format information");
  Texture texture = Texture::New(TextureType::TEXTURE_2D);

  application.GetGlAbstraction().EnableTextureCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

  tet_infoline("TexImage2D should not be called with a null pointer to reserve storage for the texture in the gpu");
  DALI_TEST_CHECK(!callStack.FindMethod("GenTextures"));
  DALI_TEST_CHECK(!callStack.FindMethod("TexImage2D"));

  tet_infoline("Upload data to the texture");
  unsigned int width(64);
  unsigned int height(64);
  callStack.Reset();

  tet_infoline("Creating a RGB pixel buffer and adding that to the texture to ensure it is handled correctly");
  unsigned int   bufferSize(width * height * 3);
  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGB888, PixelData::FREE);
  texture.Upload(pixelData);
  application.SendNotification();
  application.Render();

  tet_infoline("GetWidth / GetHeight / GetPixelFormat will return uploaded value");
  DALI_TEST_EQUALS(texture.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetHeight(), height, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::RGB888, TEST_LOCATION);

  tet_infoline("TexImage2D should be called to upload the data");
  DALI_TEST_CHECK(callStack.FindMethod("GenTextures"));
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  tet_infoline("Upload another data to the texture");
  width  = 40;
  height = 73;
  callStack.Reset();

  tet_infoline("Creating a RGB pixel buffer and adding that to the texture to ensure it is handled correctly");
  bufferSize = width * height * 4;
  buffer     = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  pixelData  = PixelData::New(buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::FREE);
  texture.Upload(pixelData);
  application.SendNotification();
  application.Render();

  tet_infoline("TexImage2D should be generate new graphics, and be called to upload the data");
  DALI_TEST_CHECK(callStack.FindMethod("GenTextures"));
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  tet_infoline("GetWidth / GetHeight / GetPixelFormat will return uploaded value");
  DALI_TEST_EQUALS(texture.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetHeight(), height, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureUploadSubPixelData01(void)
{
  TestApplication application;

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);
  Texture      texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  application.GetGlAbstraction().EnableTextureCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

  //Upload data to the texture
  callStack.Reset();

  uint32_t bufferWidth   = width * 2;
  uint32_t bufferHeight  = height * 2;
  uint32_t bufferXOffset = width;
  uint32_t bufferYOffset = height;

  unsigned int   bufferSize(bufferWidth * bufferHeight * 4);
  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, bufferWidth, bufferHeight, Pixel::RGBA8888, PixelData::FREE);
  DevelTexture::UploadSubPixelData(texture, pixelData, bufferXOffset, bufferYOffset, width, height);
  application.SendNotification();
  application.Render();

  //TexImage2D should be called to upload the data
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  //Upload part of the texture
  callStack.Reset();
  DevelTexture::UploadSubPixelData(texture, pixelData, bufferXOffset, bufferYOffset, width / 2, height / 2, 0u, 0u, width / 2, height / 2, width / 2, height / 2);
  application.SendNotification();
  application.Render();

  //TexSubImage2D should be called to upload the data
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width / 2 << ", " << height / 2 << ", " << width / 2 << ", " << height / 2;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexSubImage2D", out.str().c_str()));
  }

  END_TEST;
}

int UtcDaliTextureUploadSubPixelData02(void)
{
  TestApplication application;

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);
  Texture      texture = CreateTexture(TextureType::TEXTURE_CUBE, Pixel::RGBA8888, width, height);

  application.GetGlAbstraction().EnableTextureCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

  tet_infoline("TexImage2D should be called six times with a null pointer to reserve storage for the six textures of the cube map");
  for(unsigned int i(0); i < 6; ++i)
  {
    std::stringstream out;
    out << GL_TEXTURE_CUBE_MAP_POSITIVE_X + i << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  uint32_t bufferWidth   = width * 2;
  uint32_t bufferHeight  = height * 2;
  uint32_t bufferXOffset = width;
  uint32_t bufferYOffset = height;

  unsigned int   bufferSize(bufferWidth * bufferHeight * 4);
  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, bufferWidth, bufferHeight, Pixel::RGBA8888, PixelData::FREE);

  //Upload data to the POSITIVE_X face of the texture
  {
    callStack.Reset();

    DevelTexture::UploadSubPixelData(texture, pixelData, bufferXOffset, bufferYOffset, width, height, CubeMapLayer::POSITIVE_X, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the POSITIVE_X face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_POSITIVE_X << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  //Upload data to the NEGATIVE_X face of the texture
  {
    callStack.Reset();

    DevelTexture::UploadSubPixelData(texture, pixelData, bufferXOffset, bufferYOffset, width, height, CubeMapLayer::NEGATIVE_X, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the NEGATIVE_X face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_NEGATIVE_X << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  //Upload data to the POSITIVE_Y face of the texture
  {
    callStack.Reset();
    DevelTexture::UploadSubPixelData(texture, pixelData, bufferXOffset, bufferYOffset, width, height, CubeMapLayer::POSITIVE_Y, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the POSITIVE_Y face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_POSITIVE_Y << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  //Upload data to the NEGATIVE_Y face of the texture
  {
    callStack.Reset();
    DevelTexture::UploadSubPixelData(texture, pixelData, bufferXOffset, bufferYOffset, width, height, CubeMapLayer::NEGATIVE_Y, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the NEGATIVE_Y face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_NEGATIVE_Y << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  //Upload data to the POSITIVE_Z face of the texture
  {
    callStack.Reset();
    DevelTexture::UploadSubPixelData(texture, pixelData, bufferXOffset, bufferYOffset, width, height, CubeMapLayer::POSITIVE_Z, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the POSITIVE_Z face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_POSITIVE_Z << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  //Upload data to the NEGATIVE_Z face of the texture
  {
    callStack.Reset();
    DevelTexture::UploadSubPixelData(texture, pixelData, bufferXOffset, bufferYOffset, width, height, CubeMapLayer::NEGATIVE_Z, 0u, 0u, 0u, width, height);
    application.SendNotification();
    application.Render();

    //TexImage2D should be called to upload the data to the NEGATIVE_Z face
    {
      std::stringstream out;
      out << GL_TEXTURE_CUBE_MAP_NEGATIVE_Z << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
  }

  END_TEST;
}

int UtcDaliTextureUploadPixelFormats(void)
{
  TestApplication application;
  application.GetGlAbstraction().EnableTextureCallTrace(true);

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);

  std::vector<Pixel::Format> formats =
    {
      Pixel::A8,
      Pixel::L8,
      Pixel::LA88,
      Pixel::RGB565,
      Pixel::BGR565,
      Pixel::RGBA4444,
      Pixel::BGRA4444,
      Pixel::RGBA5551,
      Pixel::BGRA5551,
      Pixel::RGB888,
      Pixel::RGB8888,
      Pixel::BGR8888,
      Pixel::RGBA8888,
      Pixel::BGRA8888,
      Pixel::DEPTH_UNSIGNED_INT,
      Pixel::DEPTH_FLOAT,
      Pixel::DEPTH_STENCIL,
      Pixel::RGB16F,
      Pixel::RGB32F,
      Pixel::R11G11B10F,
      Pixel::CHROMINANCE_U,
      Pixel::CHROMINANCE_V};

  for(auto format : formats)
  {
    tet_infoline("Creating a Texure with a new or recent format");
    Texture texture = CreateTexture(TextureType::TEXTURE_2D, format, width, height);

    application.SendNotification();
    application.Render();

    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

    tet_infoline("TexImage2D should be called twice per texture");
    DALI_TEST_EQUALS(callStack.CountMethod("TexImage2D"), 2, TEST_LOCATION);
    {
      std::stringstream out;
      out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
      DALI_TEST_CHECK(callStack.FindMethodAndParams("TexImage2D", out.str().c_str()));
    }
    callStack.Reset();
  }

  END_TEST;
}

int UtcDaliTextureUploadSmallerThanSize(void)
{
  TestApplication application;

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);
  Texture      texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  application.GetGlAbstraction().EnableTextureCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.EnableLogging(true);
  TraceCallStack& texParamCallStack = application.GetGlAbstraction().GetTexParameterTrace();
  texParamCallStack.EnableLogging(true);

  tet_infoline("TexImage2D should be called with a null pointer to reserve storage for the texture in the gpu");
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    std::string params;
    DALI_TEST_CHECK(callStack.FindMethodAndGetParameters("TexImage2D", params));
    DALI_TEST_EQUALS(out.str(), params, TEST_LOCATION);
  }

  //Upload data to the texture
  callStack.Reset();

  unsigned int   bufferSize(width * height * 4);
  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, width / 2, height / 2, Pixel::RGBA8888, PixelData::FREE);
  texture.Upload(pixelData);
  application.SendNotification();
  application.Render();

  //TexSubImage2D should be called to upload the data
  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << 0u << ", " << 0u << ", " << width / 2 << ", " << height / 2;
    std::string params;
    DALI_TEST_CHECK(callStack.FindMethodAndGetParameters("TexSubImage2D", params));
    DALI_TEST_EQUALS(out.str(), params, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextureGenerateMipmaps(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  texture.GenerateMipmaps();

  Texture textureCubemap = CreateTexture(TextureType::TEXTURE_CUBE, Pixel::RGBA8888, width, height);
  textureCubemap.GenerateMipmaps();

  application.GetGlAbstraction().EnableTextureCallTrace(true);
  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  application.SendNotification();
  application.Render();

  {
    std::stringstream out;
    out << GL_TEXTURE_2D;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("GenerateMipmap", out.str().c_str()));
  }
  {
    std::stringstream out;
    out << GL_TEXTURE_CUBE_MAP;
    DALI_TEST_CHECK(callStack.FindMethodAndParams("GenerateMipmap", out.str().c_str()));
  }

  END_TEST;
}

int UtcDaliTextureGenerateMipmapsCompressedFormat(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::COMPRESSED_RGBA8_ETC2_EAC, width, height);
  texture.GenerateMipmaps();

  application.GetGlAbstraction().EnableTextureCallTrace(true);
  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  application.SendNotification();
  application.Render();

  // Check generate mipmap didn't called when we use compressed pixel format.
  DALI_TEST_CHECK(!callStack.FindMethod("GenerateMipmap"));

  END_TEST;
}

int UtcDaliTextureGetWidth(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  DALI_TEST_EQUALS(texture.GetWidth(), width, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextureGetHeight(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  DALI_TEST_EQUALS(texture.GetHeight(), height, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureGetTextureType(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  DALI_TEST_EQUALS(Integration::GetTextureType(texture), TextureType::TEXTURE_2D, TEST_LOCATION);

  texture = CreateTexture(TextureType::TEXTURE_CUBE, Pixel::RGBA8888, width, height);
  DALI_TEST_EQUALS(Integration::GetTextureType(texture), TextureType::TEXTURE_CUBE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetSize(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  DALI_TEST_EQUALS(texture.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetHeight(), height, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);

  width += 11u;
  height += 22u;

  Integration::SetTextureSize(texture, ImageDimensions(width, height));
  DALI_TEST_EQUALS(texture.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetHeight(), height, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextureSetPixelFormat(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  DALI_TEST_EQUALS(texture.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetHeight(), height, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);

  Integration::SetTexturePixelFormat(texture, Pixel::BGRA5551);
  DALI_TEST_EQUALS(texture.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetHeight(), height, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::BGRA5551, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextureContextLoss(void)
{
  tet_infoline("UtcDaliTextureContextLoss\n");
  TestApplication application;

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);
  Texture      texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  DALI_TEST_CHECK(texture);

  application.SendNotification();
  application.Render(16);

  // Lose & regain context (in render 'thread')
  application.ResetContext();
  DALI_TEST_CHECK(texture);

  END_TEST;
}

int UtcDaliNativeImageTexture01(void)
{
  TestApplication application;
  tet_infoline("UtcDaliNativeImageTexture01");

  TestNativeImagePointer imageInterface = TestNativeImage::New(16, 16);
  {
    Texture texture = Texture::New(*(imageInterface.Get()));
    Actor   actor   = CreateRenderableActor(texture, "", "");
    application.GetScene().Add(actor);

    DALI_TEST_CHECK(texture);

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(imageInterface->mExtensionCreateCalls, 1, TEST_LOCATION);
    DALI_TEST_EQUALS(imageInterface->mExtensionDestroyCalls, 0, TEST_LOCATION);
    DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE), Property::Value(Vector3(16, 16, 0)), TEST_LOCATION);

    UnparentAndReset(actor);

    application.SendNotification();
    application.Render(16);
  }
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(imageInterface->mExtensionCreateCalls, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(imageInterface->mExtensionDestroyCalls, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliNativeImageTexture02(void)
{
  TestApplication application;
  tet_infoline("UtcDaliNativeImageTexture02 - test error on TargetTexture");

  TestNativeImagePointer imageInterface = TestNativeImage::New(16, 16);
  imageInterface->mTargetTextureError   = 1u;
  {
    Texture texture = Texture::New(*(imageInterface.Get()));
    Actor   actor   = CreateRenderableActor(texture, "", "");
    application.GetScene().Add(actor);

    DALI_TEST_CHECK(texture);

    application.SendNotification();
    application.Render(16);

    // Expect 2 attempts to create the texture - once when adding the texture
    // to the scene-graph, and again since that failed, during the Bind.
    // The second one succeeds (TargetTexture only errors once)
    DALI_TEST_EQUALS(imageInterface->mExtensionCreateCalls, 2, TEST_LOCATION);
    DALI_TEST_EQUALS(imageInterface->mExtensionDestroyCalls, 1, TEST_LOCATION);

    UnparentAndReset(actor);

    application.SendNotification();
    application.Render(16);
  }
  application.SendNotification();
  application.Render(16);

  // Expect that there are no further calls to create/destroy resource
  DALI_TEST_EQUALS(imageInterface->mExtensionCreateCalls, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(imageInterface->mExtensionDestroyCalls, 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureGenerateMipmapsNegative(void)
{
  TestApplication application;
  Dali::Texture   instance;
  try
  {
    instance.GenerateMipmaps();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliTextureUploadNegative01(void)
{
  TestApplication application;
  Dali::Texture   instance;
  try
  {
    Dali::PixelData arg1;
    instance.Upload(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliTextureUploadNegative02(void)
{
  TestApplication application;
  Dali::Texture   instance;
  try
  {
    Dali::PixelData arg1;
    unsigned int    arg2(0u);
    unsigned int    arg3(0u);
    unsigned int    arg4(0u);
    unsigned int    arg5(0u);
    unsigned int    arg6(0u);
    unsigned int    arg7(0u);
    instance.Upload(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliTextureGetWidthNegative(void)
{
  TestApplication application;
  Dali::Texture   instance;
  try
  {
    instance.GetWidth();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliTextureGetHeightNegative(void)
{
  TestApplication application;
  Dali::Texture   instance;
  try
  {
    instance.GetHeight();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliTextureCheckNativeP(void)
{
  TestApplication        application;
  TestNativeImagePointer testNativeImage = TestNativeImage::New(64u, 64u);
  Texture                nativeTexture   = Texture::New(*testNativeImage);

  DALI_TEST_CHECK(nativeTexture);
  DALI_TEST_CHECK(DevelTexture::IsNative(nativeTexture));
  END_TEST;
}

int UtcDaliTextureCheckNativeN1(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);
  Texture         texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  DALI_TEST_CHECK(texture);
  DALI_TEST_CHECK(!DevelTexture::IsNative(texture));
  END_TEST;
}

int UtcDaliTextureCheckNativeN2(void)
{
  TestApplication application;
  Texture         texture;
  try
  {
    bool native = DevelTexture::IsNative(texture);
    DALI_TEST_CHECK(native != native);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }
  END_TEST;
}

int UtcDaliTextureApplyFragShaderP1(void)
{
  TestApplication        application;
  TestNativeImagePointer testNativeImage = TestNativeImage::New(64u, 64u);
  Texture                nativeTexture   = Texture::New(*testNativeImage);
  DALI_TEST_CHECK(nativeTexture);

  const std::string baseFragShader =
    "varying mediump vec4 uColor;\n"
    "void main(){\n"
    "  gl_FragColor=uColor;\n"
    "}\n";
  std::string fragShader = baseFragShader;
  bool        applied    = DevelTexture::ApplyNativeFragmentShader(nativeTexture, fragShader);

  std::string fragPrefix = "#extension GL_OES_EGL_image_external:require\n";

  DALI_TEST_CHECK(applied);
  DALI_TEST_CHECK(baseFragShader.compare(fragShader));
  DALI_TEST_CHECK(fragShader.compare(fragPrefix + baseFragShader) == 0);
  DALI_TEST_CHECK(!fragShader.empty());
  END_TEST;
}

int UtcDaliTextureApplyFragShaderP2(void)
{
  TestApplication        application;
  TestNativeImagePointer testNativeImage = TestNativeImage::New(64u, 64u);
  Texture                nativeTexture   = Texture::New(*testNativeImage);
  DALI_TEST_CHECK(nativeTexture);

  const std::string baseFragShader =
    "varying mediump vec4 uColor;\n"
    "varying vec2 vTexCoord;\n"
    "uniform sampler2D uNative;\n"
    "void main(){\n"
    "  gl_FragColor=uColor*texture2D(uNative, vTexCoord);\n"
    "}\n";
  std::string fragShader = baseFragShader;
  bool        applied    = DevelTexture::ApplyNativeFragmentShader(nativeTexture, fragShader);

  DALI_TEST_CHECK(applied);
  DALI_TEST_CHECK(baseFragShader.compare(fragShader));
  DALI_TEST_CHECK(!fragShader.empty());
  DALI_TEST_CHECK(fragShader.find("samplerExternalOES") < fragShader.length());
  END_TEST;
}

int UtcDaliTextureApplyFragShaderN1(void)
{
  TestApplication        application;
  TestNativeImagePointer testNativeImage = TestNativeImage::New(64u, 64u);
  Texture                nativeTexture   = Texture::New(*testNativeImage);
  DALI_TEST_CHECK(nativeTexture);

  std::string fragShader;
  bool        applied = DevelTexture::ApplyNativeFragmentShader(nativeTexture, fragShader);

  DALI_TEST_CHECK(!applied);
  DALI_TEST_CHECK(fragShader.empty());
  END_TEST;
}

int UtcDaliTextureApplyFragShaderN2(void)
{
  TestApplication application;
  unsigned int    width(64);
  unsigned int    height(64);
  Texture         texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  const std::string baseFragShader =
    "varying mediump vec4 uColor;\n"
    "void main(){\n"
    "  gl_FragColor=uColor;\n"
    "}\n";
  std::string fragShader = baseFragShader;
  bool        applied    = DevelTexture::ApplyNativeFragmentShader(texture, fragShader);

  DALI_TEST_CHECK(!applied);
  DALI_TEST_CHECK(!baseFragShader.compare(fragShader));
  END_TEST;
}

int UtcDaliTextureGetPixelFormat(void)
{
  TestApplication application;
  uint32_t        width(64);
  uint32_t        height(64);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);

  texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGB888, width, height);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::RGB888, TEST_LOCATION);

  texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::L8, width, height);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::L8, TEST_LOCATION);

  texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::CHROMINANCE_U, width, height);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::CHROMINANCE_U, TEST_LOCATION);

  END_TEST;
}

int utcDaliTexturePartialUpdate01(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged rect with partial update and texture change");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int32_t>> damagedRects;
  Rect<int32_t>              clippingRect;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  uint32_t   width(4);
  uint32_t   height(4);
  Texture    texture    = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  application.GetScene().Add(actor);

  damagedRects.clear();
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int32_t>(16, 768, 32, 32); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int32_t>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Ensure the damaged rect is empty
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  // Upload texture
  uint32_t  bufferSize(width * height * 4);
  uint8_t*  buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::FREE);
  texture.Upload(pixelData);

  damagedRects.clear();
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int32_t>(16, 768, 32, 32); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int32_t>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Ensure the damaged rect is empty
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  END_TEST;
}

int utcDaliTexturePartialUpdate02(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged rect with partial update and texture change");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int32_t>> damagedRects;
  Rect<int32_t>              clippingRect;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  uint32_t   width(4);
  uint32_t   height(4);
  Texture    texture1   = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  Texture    texture2   = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture1);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  application.GetScene().Add(actor);

  damagedRects.clear();
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int32_t>(16, 768, 32, 32); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int32_t>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Ensure the damaged rect is empty
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  // Set another texture
  textureSet.SetTexture(0u, texture2);

  damagedRects.clear();
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int32_t>(16, 768, 32, 32); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int32_t>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Ensure the damaged rect is empty
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  END_TEST;
}
