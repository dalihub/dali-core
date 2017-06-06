/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <cstdlib>
#include <dali-test-suite-utils.h>

#include <dali/public-api/images/pixel.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/devel-api/images/pixel-data-mask.h>
#include <dali/public-api/common/dali-vector.h>

using namespace Dali;

int UtcDaliPixelData01(void)
{
  TestApplication application;

  unsigned int width = 10u;
  unsigned int height = 10u;
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::RGB888 );

  unsigned char* buffer= reinterpret_cast<unsigned char*>( malloc( bufferSize ) );
  PixelData pixelData = PixelData::New( buffer, bufferSize, width, height, Pixel::RGB888, PixelData::FREE );

  DALI_TEST_CHECK( pixelData );
  DALI_TEST_CHECK( pixelData.GetWidth() == width );
  DALI_TEST_CHECK( pixelData.GetHeight() == height );
  DALI_TEST_CHECK( pixelData.GetPixelFormat() == Pixel::RGB888 );

  END_TEST;
}

int UtcDaliPixelData02(void)
{
  TestApplication application;

  unsigned int width = 10u;
  unsigned int height = 10u;
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::L8 );
  unsigned char* buffer = new unsigned char [ bufferSize ];
  buffer[0] = 'a';

  PixelData pixelData = PixelData::New( buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

  DALI_TEST_CHECK( pixelData);
  DALI_TEST_CHECK( pixelData.GetWidth() == width );
  DALI_TEST_CHECK( pixelData.GetHeight() == height );
  DALI_TEST_CHECK( pixelData.GetPixelFormat() == Pixel::L8 );

  END_TEST;
}

int UtcDaliPixelDataCopyConstructor(void)
{
  TestApplication application;

  unsigned int width = 10u;
  unsigned int height = 10u;
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::L8 );
  unsigned char* buffer = new unsigned char [ bufferSize ];
  PixelData pixelData = PixelData::New( buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

  PixelData pixelDataCopy(pixelData);

  DALI_TEST_EQUALS( (bool)pixelDataCopy, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPixelDataAssignmentOperator(void)
{
  TestApplication application;

  unsigned int width = 10u;
  unsigned int height = 10u;
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::L8 );
  unsigned char* buffer = new unsigned char [ bufferSize ];
  PixelData pixelData = PixelData::New( buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

  PixelData pixelData2;
  DALI_TEST_EQUALS( (bool)pixelData2, false, TEST_LOCATION );

  pixelData2 = pixelData;
  DALI_TEST_EQUALS( (bool)pixelData2, true, TEST_LOCATION );

  END_TEST;
}


int UtcDaliPixelDataMask01(void)
{
  TestApplication application;

  unsigned int width = 10u;
  unsigned int height = 10u;
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::L8 );
  unsigned char* buffer = new unsigned char [ bufferSize ];
  PixelData maskData = PixelData::New( buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

  width = 20u;
  height = 20u;
  Pixel::Format pixelFormat = Pixel::RGBA5551;
  bufferSize = width*height*Pixel::GetBytesPerPixel( pixelFormat );
  buffer = new unsigned char [ bufferSize ];
  PixelData imageData = PixelData::New( buffer, bufferSize, width, height, pixelFormat, PixelData::DELETE_ARRAY );

  Dali::ApplyMask( imageData, maskData );

  // Test that the pixel format has been promoted to RGBA8888
  DALI_TEST_EQUALS( imageData.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPixelDataMask02(void)
{
  TestApplication application;

  unsigned int width = 10u;
  unsigned int height = 10u;
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::L8 );
  unsigned char* buffer = new unsigned char [ bufferSize ];
  PixelData maskData = PixelData::New( buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

  width = 20u;
  height = 20u;
  Pixel::Format pixelFormat = Pixel::RGBA4444;
  bufferSize = width*height*Pixel::GetBytesPerPixel( pixelFormat );
  buffer = new unsigned char [ bufferSize ];
  PixelData imageData = PixelData::New( buffer, bufferSize, width, height, pixelFormat, PixelData::DELETE_ARRAY );

  Dali::ApplyMask( imageData, maskData );

  // Test that the pixel format has been promoted to RGBA8888
  DALI_TEST_EQUALS( imageData.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPixelDataMask03(void)
{
  TestApplication application;

  unsigned int width = 20u;
  unsigned int height = 20u;
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::L8 );
  unsigned char* buffer = new unsigned char [ bufferSize ];
  PixelData maskData = PixelData::New( buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

  width = 10u;
  height = 10u;
  Pixel::Format format = Pixel::RGB565;
  bufferSize = width*height*Pixel::GetBytesPerPixel( format );
  buffer = new unsigned char [ bufferSize ];
  PixelData imageData = PixelData::New( buffer, bufferSize, width, height, format, PixelData::DELETE_ARRAY );

  Dali::ApplyMask( imageData, maskData );

  // Test that the pixel format has been promoted to RGBA8888
  DALI_TEST_EQUALS( imageData.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  // Can't test the final image directly...

  END_TEST;
}


int UtcDaliPixelDataMask04(void)
{
  TestApplication application;

  unsigned int width = 10u;
  unsigned int height = 10u;
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::L8 );
  unsigned char* buffer = new unsigned char [ bufferSize ];
  PixelData maskData = PixelData::New( buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

  width = 20u;
  height = 20u;
  bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::RGBA8888 );
  buffer = new unsigned char [ bufferSize ];
  PixelData imageData = PixelData::New( buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::DELETE_ARRAY );

  Dali::ApplyMask( imageData, maskData );

  // Test that the pixel format hasn't changed
  DALI_TEST_EQUALS( imageData.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  // Can't test the final image directly...

  END_TEST;
}

int UtcDaliPixelDataMask05(void)
{
  TestApplication application;

  unsigned int width = 20u;
  unsigned int height = 20u;
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::L8 );
  unsigned char* buffer = new unsigned char [ bufferSize ];
  PixelData maskData = PixelData::New( buffer, bufferSize, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

  width = 10u;
  height = 10u;
  bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::RGBA8888 );
  buffer = new unsigned char [ bufferSize ];
  PixelData imageData = PixelData::New( buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::DELETE_ARRAY );

  Dali::ApplyMask( imageData, maskData );

  // Test that the pixel format hasn't changed
  DALI_TEST_EQUALS( imageData.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  // Can't test the final image directly...

  END_TEST;
}
