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

#include <dali-test-suite-utils.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/images/pixel.h>

#include <dali/integration-api/pixel-data-integ.h>

#include <cstdlib>

using namespace Dali;

int UtcDaliPixelData01(void)
{
  TestApplication application;

  unsigned int width      = 10u;
  unsigned int height     = 10u;
  unsigned int bufferSize = width * height * Pixel::GetBytesPerPixel(Pixel::RGB888);

  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGB888, PixelData::FREE);

  DALI_TEST_CHECK(pixelData);
  DALI_TEST_CHECK(pixelData.GetWidth() == width);
  DALI_TEST_CHECK(pixelData.GetHeight() == height);
  DALI_TEST_CHECK(pixelData.GetPixelFormat() == Pixel::RGB888);

  END_TEST;
}

int UtcDaliPixelData02(void)
{
  TestApplication application;

  unsigned int   width      = 10u;
  unsigned int   height     = 10u;
  unsigned int   bufferSize = width * height * Pixel::GetBytesPerPixel(Pixel::L8);
  unsigned char* buffer     = new unsigned char[bufferSize];
  buffer[0]                 = 'a';

  PixelData pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY);

  DALI_TEST_CHECK(pixelData);
  DALI_TEST_CHECK(pixelData.GetWidth() == width);
  DALI_TEST_CHECK(pixelData.GetHeight() == height);
  DALI_TEST_CHECK(pixelData.GetStride() == 0); ///< Legacy code. Can we remove it?
  DALI_TEST_CHECK(pixelData.GetStrideBytes() == 0);
  DALI_TEST_CHECK(pixelData.GetPixelFormat() == Pixel::L8);

  END_TEST;
}

int UtcDaliPixelData03(void)
{
  TestApplication application;

  uint32_t width       = 10u;
  uint32_t height      = 10u;
  uint32_t strideBytes = (width * Pixel::GetBytesPerPixel(Pixel::RGB888) + 7) / 8 * 8; /// = 32;
  uint32_t bufferSize  = strideBytes * height;

  uint8_t*  buffer    = reinterpret_cast<uint8_t*>(malloc(bufferSize));
  PixelData pixelData = PixelData::New(buffer, bufferSize, width, height, strideBytes, Pixel::RGB888, PixelData::FREE);

  DALI_TEST_CHECK(pixelData);
  DALI_TEST_CHECK(pixelData.GetWidth() == width);
  DALI_TEST_CHECK(pixelData.GetHeight() == height);
  DALI_TEST_CHECK(pixelData.GetStride() == width); ///< Legacy code. Can we remove it?
  DALI_TEST_CHECK(pixelData.GetStrideBytes() == strideBytes);
  DALI_TEST_CHECK(pixelData.GetPixelFormat() == Pixel::RGB888);

  END_TEST;
}

int UtcDaliPixelData04(void)
{
  TestApplication application;

  uint32_t width       = 10u;
  uint32_t height      = 10u;
  uint32_t strideBytes = (width * Pixel::GetBytesPerPixel(Pixel::L8) + 7) / 8 * 8; /// = 16;
  uint32_t bufferSize  = strideBytes * height;
  uint8_t* buffer      = new uint8_t[bufferSize];
  buffer[0]            = 'a';

  PixelData pixelData = PixelData::New(buffer, bufferSize, width, height, strideBytes, Pixel::L8, PixelData::DELETE_ARRAY);

  DALI_TEST_CHECK(pixelData);
  DALI_TEST_CHECK(pixelData.GetWidth() == width);
  DALI_TEST_CHECK(pixelData.GetHeight() == height);
  DALI_TEST_CHECK(pixelData.GetStrideBytes() == strideBytes);
  DALI_TEST_CHECK(pixelData.GetPixelFormat() == Pixel::L8);

  END_TEST;
}

int UtcDaliPixelDataCopyConstructor(void)
{
  TestApplication application;

  unsigned int   width      = 10u;
  unsigned int   height     = 10u;
  unsigned int   bufferSize = width * height * Pixel::GetBytesPerPixel(Pixel::L8);
  unsigned char* buffer     = new unsigned char[bufferSize];
  PixelData      pixelData  = PixelData::New(buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY);

  PixelData pixelDataCopy(pixelData);

  DALI_TEST_EQUALS((bool)pixelDataCopy, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPixelDataAssignmentOperator(void)
{
  TestApplication application;

  unsigned int   width      = 10u;
  unsigned int   height     = 10u;
  unsigned int   bufferSize = width * height * Pixel::GetBytesPerPixel(Pixel::L8);
  unsigned char* buffer     = new unsigned char[bufferSize];
  PixelData      pixelData  = PixelData::New(buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY);

  PixelData pixelData2;
  DALI_TEST_EQUALS((bool)pixelData2, false, TEST_LOCATION);

  pixelData2 = pixelData;
  DALI_TEST_EQUALS((bool)pixelData2, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelDataMoveConstructor(void)
{
  TestApplication application;

  unsigned int   width      = 10u;
  unsigned int   height     = 10u;
  unsigned int   bufferSize = width * height * Pixel::GetBytesPerPixel(Pixel::L8);
  unsigned char* buffer     = new unsigned char[bufferSize];

  PixelData pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY);
  DALI_TEST_CHECK(pixelData);
  DALI_TEST_EQUALS(width, pixelData.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(height, pixelData.GetHeight(), TEST_LOCATION);

  PixelData moved = std::move(pixelData);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(width, moved.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(height, moved.GetHeight(), TEST_LOCATION);
  DALI_TEST_CHECK(!pixelData);

  END_TEST;
}

int UtcDaliPixelDataMoveAssignment(void)
{
  TestApplication application;

  unsigned int   width      = 10u;
  unsigned int   height     = 10u;
  unsigned int   bufferSize = width * height * Pixel::GetBytesPerPixel(Pixel::L8);
  unsigned char* buffer     = new unsigned char[bufferSize];

  PixelData pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY);
  DALI_TEST_CHECK(pixelData);
  DALI_TEST_EQUALS(width, pixelData.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(height, pixelData.GetHeight(), TEST_LOCATION);

  PixelData moved;
  moved = std::move(pixelData);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(width, moved.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(height, moved.GetHeight(), TEST_LOCATION);
  DALI_TEST_CHECK(!pixelData);

  END_TEST;
}

int UtcDaliPixelDataGetPixelFormatNegative(void)
{
  TestApplication application;
  Dali::PixelData instance;
  try
  {
    instance.GetPixelFormat();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliPixelDataGetWidthNegative(void)
{
  TestApplication application;
  Dali::PixelData instance;
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

int UtcDaliPixelDataGetHeightNegative(void)
{
  TestApplication application;
  Dali::PixelData instance;
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

int UtcDaliPixelDataGetPixelDataBuffer(void)
{
  TestApplication application;

  uint32_t width       = 10u;
  uint32_t height      = 10u;
  uint32_t strideBytes = (width * Pixel::GetBytesPerPixel(Pixel::L8) + 7) / 8 * 8; /// = 16;
  uint32_t bufferSize  = strideBytes * height;
  uint8_t* buffer      = new uint8_t[bufferSize];
  buffer[0]            = 'a';

  PixelData pixelData = PixelData::New(buffer, bufferSize, width, height, strideBytes, Pixel::L8, PixelData::DELETE_ARRAY);

  DALI_TEST_CHECK(pixelData);
  DALI_TEST_CHECK(pixelData.GetWidth() == width);
  DALI_TEST_CHECK(pixelData.GetHeight() == height);
  DALI_TEST_CHECK(pixelData.GetStrideBytes() == strideBytes);
  DALI_TEST_CHECK(pixelData.GetPixelFormat() == Pixel::L8);

  Dali::Integration::PixelDataBuffer pixelDataBuffer = Dali::Integration::GetPixelDataBuffer(pixelData);

  DALI_TEST_CHECK(pixelData);

  DALI_TEST_EQUALS(pixelDataBuffer.bufferSize, bufferSize, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelDataBuffer.buffer[0], static_cast<uint8_t>('a'), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPixelDataReleasePixelDataBuffer(void)
{
  TestApplication application;

  uint32_t width       = 10u;
  uint32_t height      = 10u;
  uint32_t strideBytes = (width * Pixel::GetBytesPerPixel(Pixel::L8) + 7) / 8 * 8; /// = 16;
  uint32_t bufferSize  = strideBytes * height;
  uint8_t* buffer      = new uint8_t[bufferSize];
  buffer[0]            = 'a';

  PixelData pixelData = PixelData::New(buffer, bufferSize, width, height, strideBytes, Pixel::L8, PixelData::DELETE_ARRAY);

  DALI_TEST_CHECK(pixelData);
  DALI_TEST_CHECK(pixelData.GetWidth() == width);
  DALI_TEST_CHECK(pixelData.GetHeight() == height);
  DALI_TEST_CHECK(pixelData.GetStrideBytes() == strideBytes);
  DALI_TEST_CHECK(pixelData.GetPixelFormat() == Pixel::L8);

  Dali::Integration::ReleasePixelDataBuffer(pixelData);

  Dali::Integration::PixelDataBuffer pixelDataBuffer = Dali::Integration::GetPixelDataBuffer(pixelData);

  DALI_TEST_CHECK(pixelDataBuffer.buffer == nullptr);

  END_TEST;
}

int UtcDaliPixelDataNewPixelDataWithReleaseAfterUpload(void)
{
  TestApplication application;

  uint32_t width       = 10u;
  uint32_t height      = 10u;
  uint32_t strideBytes = (width * Pixel::GetBytesPerPixel(Pixel::L8) + 7) / 8 * 8; /// = 16;
  uint32_t bufferSize  = strideBytes * height;
  uint8_t* buffer      = new uint8_t[bufferSize];
  buffer[0]            = 'a';

  PixelData pixelData = Dali::Integration::NewPixelDataWithReleaseAfterUpload(buffer, bufferSize, width, height, strideBytes, Pixel::L8, PixelData::DELETE_ARRAY);

  DALI_TEST_CHECK(pixelData);
  DALI_TEST_CHECK(pixelData.GetWidth() == width);
  DALI_TEST_CHECK(pixelData.GetHeight() == height);
  DALI_TEST_CHECK(pixelData.GetStrideBytes() == strideBytes);
  DALI_TEST_CHECK(pixelData.GetPixelFormat() == Pixel::L8);
  DALI_TEST_EQUALS(Dali::Integration::IsPixelDataReleaseAfterUpload(pixelData), true, TEST_LOCATION);

  Dali::Integration::PixelDataBuffer pixelDataBuffer = Dali::Integration::GetPixelDataBuffer(pixelData);

  DALI_TEST_EQUALS(pixelDataBuffer.bufferSize, bufferSize, TEST_LOCATION);
  DALI_TEST_EQUALS(pixelDataBuffer.buffer[0], static_cast<uint8_t>('a'), TEST_LOCATION);

  END_TEST;
}
