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
#include <dali/devel-api/images/pixel-buffer-devel.h>
#include <dali/public-api/images/pixel-buffer.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/images/pixel.h>

#include <dali/integration-api/pixel-buffer-integ.h>
#include <dali/integration-api/pixel-data-integ.h>

#include <mesh-builder.h>

#if defined(__linux__)
#include <sys/resource.h>
#include <unistd.h>

#include <cstdio>
#endif

#include <cstdlib>
#include <cstring>

using namespace Dali;

void utc_dali_pixel_buffer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_pixel_buffer_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
uint32_t gBlurCallCount   = 0;
float    gBlurRadius      = 0.0f;
uint32_t gResizeCallCount = 0;
uint16_t gResizeWidth     = 0u;
uint16_t gResizeHeight    = 0u;

bool TestGaussianBlurOperation(Dali::PixelBuffer& pixelBuffer, float blurRadius)
{
  ++gBlurCallCount;
  gBlurRadius = blurRadius;
  return true;
}

bool TestResizeOperation(Dali::PixelBuffer& pixelBuffer, uint16_t width, uint16_t height)
{
  ++gResizeCallCount;
  gResizeWidth  = width;
  gResizeHeight = height;
  return true;
}

// Uploads the given data to a texture and draws it, returning whether the texture got bound
bool UploadAndDraw(TestApplication& application, PixelData pixelData)
{
  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.Reset();

  Texture texture = Texture::New(TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
  texture.Upload(pixelData);
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("v", "f");
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  return textureTrace.FindMethod("BindTexture");
}
} // namespace

int UtcDaliPixelBufferNew01(void)
{
  TestApplication application;

  uint32_t width  = 20u;
  uint32_t height = 10u;

  PixelBuffer pixelBuffer = PixelBuffer::New(width, height, Pixel::RGBA8888);

  DALI_TEST_CHECK(pixelBuffer);
  DALI_TEST_EQUALS(pixelBuffer.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelBuffer.GetHeight(), height, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelBuffer.GetStrideBytes(), width * Pixel::GetBytesPerPixel(Pixel::RGBA8888), TEST_LOCATION);
  DALI_TEST_EQUALS(pixelBuffer.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelBuffer.GetBufferSize(), width * height * Pixel::GetBytesPerPixel(Pixel::RGBA8888), TEST_LOCATION);
  DALI_TEST_CHECK(pixelBuffer.GetBuffer() != nullptr);

  // The buffer is writable
  memset(pixelBuffer.GetBuffer(), 0xab, pixelBuffer.GetBufferSize());
  DALI_TEST_EQUALS(pixelBuffer.GetBuffer()[pixelBuffer.GetBufferSize() - 1u], 0xab, TEST_LOCATION);

  // The const accessor returns the same buffer
  const PixelBuffer& constPixelBuffer = pixelBuffer;
  DALI_TEST_CHECK(constPixelBuffer.GetBuffer() == pixelBuffer.GetBuffer());

  END_TEST;
}

int UtcDaliPixelBufferNew02(void)
{
  TestApplication application;

  // A compressed format has no fixed bytes-per-pixel, so no buffer is allocated
  PixelBuffer pixelBuffer = PixelBuffer::New(4u, 4u, Pixel::COMPRESSED_RGB8_ETC2);

  DALI_TEST_CHECK(pixelBuffer);
  DALI_TEST_CHECK(pixelBuffer.GetBuffer() == nullptr);
  DALI_TEST_EQUALS(pixelBuffer.GetBufferSize(), 0u, TEST_LOCATION);

  // Dimensions whose buffer size exceeds the uint32_t range allocate nothing
  PixelBuffer tooBig = PixelBuffer::New(65536u, 65536u, Pixel::RGBA8888);

  DALI_TEST_CHECK(tooBig);
  DALI_TEST_CHECK(tooBig.GetBuffer() == nullptr);
  DALI_TEST_EQUALS(tooBig.GetBufferSize(), 0u, TEST_LOCATION);

  // Zero dimensions allocate nothing either
  PixelBuffer empty = PixelBuffer::New(0u, 0u, Pixel::RGBA8888);

  DALI_TEST_CHECK(empty);
  DALI_TEST_CHECK(empty.GetBuffer() == nullptr);
  DALI_TEST_EQUALS(empty.GetBufferSize(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferCopyConstructor(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);
  PixelBuffer copy(pixelBuffer);

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy.GetBuffer() == pixelBuffer.GetBuffer());

  END_TEST;
}

int UtcDaliPixelBufferAssignmentOperator(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);
  PixelBuffer copy;
  DALI_TEST_CHECK(!copy);

  copy = pixelBuffer;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy.GetBuffer() == pixelBuffer.GetBuffer());

  END_TEST;
}

int UtcDaliPixelBufferMoveConstructor(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);
  DALI_TEST_CHECK(pixelBuffer);

  PixelBuffer moved = std::move(pixelBuffer);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!pixelBuffer);

  END_TEST;
}

int UtcDaliPixelBufferMoveAssignment(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);
  DALI_TEST_CHECK(pixelBuffer);

  PixelBuffer moved;
  moved = std::move(pixelBuffer);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!pixelBuffer);

  END_TEST;
}

int UtcDaliPixelBufferConvert(void)
{
  TestApplication application;

  uint32_t width  = 10u;
  uint32_t height = 10u;

  PixelBuffer pixelBuffer = PixelBuffer::New(width, height, Pixel::RGB888);
  memset(pixelBuffer.GetBuffer(), 0x5a, pixelBuffer.GetBufferSize());

  // Another handle to the same object
  PixelBuffer otherHandle = pixelBuffer;

  PixelData pixelData = PixelBuffer::Convert(pixelBuffer);

  DALI_TEST_CHECK(pixelData);
  DALI_TEST_EQUALS(pixelData.GetWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelData.GetHeight(), height, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelData.GetPixelFormat(), Pixel::RGB888, TEST_LOCATION);

  // The converted handle is reset
  DALI_TEST_CHECK(!pixelBuffer);

  // Any other handle is left without a buffer
  DALI_TEST_CHECK(otherHandle.GetBuffer() == nullptr);
  DALI_TEST_EQUALS(otherHandle.GetBufferSize(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(otherHandle.GetWidth(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(otherHandle.GetHeight(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(otherHandle.GetStrideBytes(), 0u, TEST_LOCATION);

  // The buffer has been handed over, not copied
  Integration::PixelDataBuffer pixelDataBuffer = Integration::GetPixelDataBuffer(pixelData);
  DALI_TEST_EQUALS(pixelDataBuffer.buffer[0], 0x5a, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferConvertUpload(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGB565);
  memset(pixelBuffer.GetBuffer(), 0xff, pixelBuffer.GetBufferSize());

  PixelData pixelData = PixelBuffer::Convert(pixelBuffer);
  DALI_TEST_EQUALS(pixelData.GetStrideBytes(), 20u, TEST_LOCATION);

  // The converted data can be uploaded to a texture and drawn
  DALI_TEST_EQUALS(UploadAndDraw(application, pixelData), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferConvertNegative(void)
{
  TestApplication   application;
  Dali::PixelBuffer instance;
  try
  {
    PixelBuffer::Convert(instance);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliPixelBufferDevelConvert(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);
  PixelData   pixelData   = DevelPixelBuffer::Convert(pixelBuffer, true);

  DALI_TEST_CHECK(pixelData);
  DALI_TEST_CHECK(!pixelBuffer);
  DALI_TEST_EQUALS(Integration::IsPixelDataReleaseAfterUpload(pixelData), true, TEST_LOCATION);

  PixelBuffer pixelBuffer2 = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);
  PixelData   pixelData2   = DevelPixelBuffer::Convert(pixelBuffer2, false);

  DALI_TEST_CHECK(pixelData2);
  DALI_TEST_CHECK(!pixelBuffer2);
  DALI_TEST_EQUALS(Integration::IsPixelDataReleaseAfterUpload(pixelData2), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferDevelConvertUpload(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGB565);
  memset(pixelBuffer.GetBuffer(), 0xff, pixelBuffer.GetBufferSize());

  PixelData pixelData = DevelPixelBuffer::Convert(pixelBuffer, true);
  DALI_TEST_EQUALS(pixelData.GetStrideBytes(), 20u, TEST_LOCATION);

  // Data released after upload can still be uploaded once and drawn
  DALI_TEST_EQUALS(UploadAndDraw(application, pixelData), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferDevelCreatePixelData(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);
  memset(pixelBuffer.GetBuffer(), 0x3c, pixelBuffer.GetBufferSize());

  PixelData pixelData = DevelPixelBuffer::CreatePixelData(pixelBuffer);

  DALI_TEST_CHECK(pixelData);
  DALI_TEST_EQUALS(pixelData.GetWidth(), 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelData.GetHeight(), 10u, TEST_LOCATION);

  // Unlike Convert(), the pixel buffer keeps its own buffer
  DALI_TEST_CHECK(pixelBuffer);
  DALI_TEST_CHECK(pixelBuffer.GetBuffer() != nullptr);

  // The data is a copy
  Integration::PixelDataBuffer pixelDataBuffer = Integration::GetPixelDataBuffer(pixelData);
  DALI_TEST_CHECK(pixelDataBuffer.buffer != pixelBuffer.GetBuffer());
  DALI_TEST_EQUALS(pixelDataBuffer.buffer[0], 0x3c, TEST_LOCATION);

  // A pixel buffer without a buffer copies nothing
  PixelBuffer emptied     = PixelBuffer::New(4u, 4u, Pixel::COMPRESSED_RGB8_ETC2);
  PixelData   copyOfEmpty = DevelPixelBuffer::CreatePixelData(emptied);
  DALI_TEST_CHECK(copyOfEmpty);
  DALI_TEST_EQUALS(Integration::GetPixelDataBuffer(copyOfEmpty).bufferSize, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferDevelMetadata(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);

  Property::Map metadata;
  DALI_TEST_EQUALS(DevelPixelBuffer::GetMetadata(pixelBuffer, metadata), false, TEST_LOCATION);

  Property::Map exifData;
  exifData["Orientation"] = 1;
  Integration::SetPixelBufferMetadata(pixelBuffer, exifData);

  DALI_TEST_EQUALS(DevelPixelBuffer::GetMetadata(pixelBuffer, metadata), true, TEST_LOCATION);
  DALI_TEST_EQUALS(metadata.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(metadata["Orientation"].Get<int>(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferDevelIsAlphaPreMultiplied(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);
  DALI_TEST_EQUALS(DevelPixelBuffer::IsAlphaPreMultiplied(pixelBuffer), false, TEST_LOCATION);

  Integration::SetPixelBufferPreMultiplied(pixelBuffer, true);
  DALI_TEST_EQUALS(DevelPixelBuffer::IsAlphaPreMultiplied(pixelBuffer), true, TEST_LOCATION);

  Integration::SetPixelBufferPreMultiplied(pixelBuffer, false);
  DALI_TEST_EQUALS(DevelPixelBuffer::IsAlphaPreMultiplied(pixelBuffer), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferIntegNewPixelBuffer(void)
{
  TestApplication application;

  uint32_t width      = 10u;
  uint32_t height     = 10u;
  uint32_t bufferSize = width * height * Pixel::GetBytesPerPixel(Pixel::RGB888);
  uint8_t* buffer     = static_cast<uint8_t*>(malloc(bufferSize));

  // A stride of 0 means the buffer is tightly packed
  PixelBuffer pixelBuffer = Integration::NewPixelBuffer(buffer, bufferSize, width, height, 0u, Pixel::RGB888);

  DALI_TEST_CHECK(pixelBuffer);
  DALI_TEST_CHECK(pixelBuffer.GetBuffer() == buffer);
  DALI_TEST_EQUALS(pixelBuffer.GetStrideBytes(), width * Pixel::GetBytesPerPixel(Pixel::RGB888), TEST_LOCATION);

  // An explicit stride is kept
  uint32_t strideBytes = (width + 2u) * Pixel::GetBytesPerPixel(Pixel::RGB888);
  uint8_t* buffer2     = static_cast<uint8_t*>(malloc(strideBytes * height));

  PixelBuffer pixelBuffer2 = Integration::NewPixelBuffer(buffer2, strideBytes * height, width, height, strideBytes, Pixel::RGB888);

  DALI_TEST_EQUALS(pixelBuffer2.GetStrideBytes(), strideBytes, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferIntegResetPixelBufferData(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);

  Property::Map exifData;
  exifData["Orientation"] = 6;
  Integration::SetPixelBufferMetadata(pixelBuffer, exifData);
  Integration::SetPixelBufferPreMultiplied(pixelBuffer, true);

  uint32_t newWidth  = 4u;
  uint32_t newHeight = 2u;
  uint32_t newSize   = newWidth * newHeight * Pixel::GetBytesPerPixel(Pixel::L8);
  uint8_t* newBuffer = static_cast<uint8_t*>(malloc(newSize));

  Integration::ResetPixelBufferData(pixelBuffer, newBuffer, newSize, newWidth, newHeight, 0u, Pixel::L8);

  DALI_TEST_CHECK(pixelBuffer.GetBuffer() == newBuffer);
  DALI_TEST_EQUALS(pixelBuffer.GetWidth(), newWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelBuffer.GetHeight(), newHeight, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelBuffer.GetBufferSize(), newSize, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelBuffer.GetStrideBytes(), newWidth * Pixel::GetBytesPerPixel(Pixel::L8), TEST_LOCATION);
  DALI_TEST_EQUALS(pixelBuffer.GetPixelFormat(), Pixel::L8, TEST_LOCATION);

  // The metadata and the pre-multiplied flag are kept
  Property::Map metadata;
  DALI_TEST_EQUALS(DevelPixelBuffer::GetMetadata(pixelBuffer, metadata), true, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelPixelBuffer::IsAlphaPreMultiplied(pixelBuffer), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferIntegTakePixelBufferOwnership(void)
{
  TestApplication application;

  PixelBuffer destination = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);
  PixelBuffer source      = PixelBuffer::New(4u, 2u, Pixel::L8);
  memset(source.GetBuffer(), 0x77, source.GetBufferSize());

  Property::Map exifData;
  exifData["Orientation"] = 3;
  Integration::SetPixelBufferMetadata(source, exifData);
  Integration::SetPixelBufferPreMultiplied(source, true);

  uint8_t* sourceBuffer = source.GetBuffer();

  Integration::TakePixelBufferOwnership(destination, source);

  DALI_TEST_CHECK(destination.GetBuffer() == sourceBuffer);
  DALI_TEST_EQUALS(destination.GetWidth(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(destination.GetHeight(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(destination.GetPixelFormat(), Pixel::L8, TEST_LOCATION);

  // The metadata and the pre-multiplied flag are moved too
  Property::Map metadata;
  DALI_TEST_EQUALS(DevelPixelBuffer::GetMetadata(destination, metadata), true, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelPixelBuffer::IsAlphaPreMultiplied(destination), true, TEST_LOCATION);

  // The source is left without a buffer
  DALI_TEST_CHECK(source.GetBuffer() == nullptr);

  END_TEST;
}

int UtcDaliPixelBufferIntegAllocateFixedSize(void)
{
  TestApplication application;

  // A compressed format allocates no buffer up front
  PixelBuffer pixelBuffer = PixelBuffer::New(4u, 4u, Pixel::COMPRESSED_RGB8_ETC2);
  DALI_TEST_CHECK(pixelBuffer.GetBuffer() == nullptr);

  Integration::AllocatePixelBufferFixedSize(pixelBuffer, 128u);

  DALI_TEST_CHECK(pixelBuffer.GetBuffer() != nullptr);
  DALI_TEST_EQUALS(pixelBuffer.GetBufferSize(), 128u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferIntegGetAllocationTotal(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);

  // Non-zero only when the library is built with DEBUG_ENABLED
  Integration::GetPixelBufferAllocationTotal();
  DALI_TEST_CHECK(true);

  END_TEST;
}

int UtcDaliPixelBufferImageOperations(void)
{
  TestApplication application;

  PixelBuffer pixelBuffer = PixelBuffer::New(10u, 10u, Pixel::RGBA8888);

  // Without a platform layer, no image operations are registered, so the
  // deprecated member functions fail
  DALI_TEST_EQUALS(pixelBuffer.ApplyGaussianBlur(1.0f), false, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelBuffer.Resize(5u, 5u), false, TEST_LOCATION);

  // With operations registered, the member functions route to them
  gBlurCallCount = gResizeCallCount = 0u;
  Integration::RegisterPixelBufferImageOperations(TestGaussianBlurOperation, TestResizeOperation);

  DALI_TEST_EQUALS(pixelBuffer.ApplyGaussianBlur(3.0f), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gBlurCallCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(gBlurRadius, 3.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(pixelBuffer.Resize(5u, 7u), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResizeCallCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(gResizeWidth, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(gResizeHeight, 7u, TEST_LOCATION);

  // Unregister, so that other test cases are not affected
  Integration::RegisterPixelBufferImageOperations(nullptr, nullptr);
  DALI_TEST_EQUALS(pixelBuffer.ApplyGaussianBlur(1.0f), false, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelBuffer.Resize(5u, 5u), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelBufferGetWidthNegative(void)
{
  TestApplication   application;
  Dali::PixelBuffer instance;
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

int UtcDaliPixelBufferGetBufferNegative(void)
{
  TestApplication   application;
  Dali::PixelBuffer instance;
  try
  {
    instance.GetBuffer();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliPixelBufferMallocFailure(void)
{
#if defined(__linux__)
  TestApplication application;

  // Cap the address space just above the current usage, so that a large allocation
  // fails deterministically while small allocations (logging etc.) keep working.
  constexpr uint32_t BIG_BUFFER_SIZE = 512u * 1024u * 1024u;
  constexpr uint64_t HEADROOM        = 64u * 1024u * 1024u;

  unsigned long usedPages = 0;
  FILE*         statm     = fopen("/proc/self/statm", "r");
  bool          limited   = statm && (fscanf(statm, "%lu", &usedPages) == 1);
  if(statm)
  {
    fclose(statm);
  }

  struct rlimit oldLimit{};
  if(limited)
  {
    getrlimit(RLIMIT_AS, &oldLimit);
    struct rlimit newLimit = oldLimit;
    newLimit.rlim_cur      = static_cast<rlim_t>(usedPages) * static_cast<rlim_t>(getpagesize()) + HEADROOM;
    limited                = (setrlimit(RLIMIT_AS, &newLimit) == 0);
  }

  if(limited)
  {
    // 16384 x 8192 x 4 bytes = 512MB, within uint32_t but over the limit
    PixelBuffer failedNew = PixelBuffer::New(16384u, 8192u, Pixel::RGBA8888);
    bool        newFailed = (failedNew.GetBuffer() == nullptr);
    DALI_TEST_CHECK(newFailed);
    DALI_TEST_EQUALS(failedNew.GetBufferSize(), 0u, TEST_LOCATION);

    PixelBuffer fixedSize = PixelBuffer::New(4u, 4u, Pixel::COMPRESSED_RGB8_ETC2);
    Integration::AllocatePixelBufferFixedSize(fixedSize, BIG_BUFFER_SIZE);
    DALI_TEST_CHECK(fixedSize.GetBuffer() == nullptr);
    DALI_TEST_EQUALS(fixedSize.GetBufferSize(), 0u, TEST_LOCATION);

    if(newFailed)
    {
      // A buffer whose claimed size is over the limit: the copy allocation in
      // CreatePixelData() fails before any pixel is copied, which the check on
      // newFailed above guarantees
      uint8_t*    smallBuffer = static_cast<uint8_t*>(malloc(4u));
      PixelBuffer claimedBig  = Integration::NewPixelBuffer(smallBuffer, BIG_BUFFER_SIZE, 1u, 1u, 0u, Pixel::RGBA8888);
      PixelData   pixelData   = DevelPixelBuffer::CreatePixelData(claimedBig);
      DALI_TEST_CHECK(!pixelData);
    }

    setrlimit(RLIMIT_AS, &oldLimit);
  }
  else
  {
    tet_printf("Could not limit the address space, skipping\n");
    DALI_TEST_CHECK(true);
  }
#else
  // Forcing an allocation failure needs the POSIX address-space limit
  tet_printf("setrlimit is not available on this platform, skipping\n");
  DALI_TEST_CHECK(true);
#endif

  END_TEST;
}
