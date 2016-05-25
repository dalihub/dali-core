/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/images/pixel-data.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/common/dali-vector.h>

using namespace Dali;

int UtcDaliPixelData01(void)
{
  TestApplication application;

  unsigned int width = 10u;
  unsigned int height = 10u;
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::RGB888 );

  unsigned char* buffer= reinterpret_cast<unsigned char*>( malloc( bufferSize ) );
  PixelData pixelData = PixelData::New( buffer, width, height, Pixel::RGB888, PixelData::FREE );

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

  PixelData pixelData = PixelData::New( buffer, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

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
  PixelData pixelData = PixelData::New( buffer, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

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
  PixelData pixelData = PixelData::New( buffer, width, height, Pixel::L8, PixelData::DELETE_ARRAY );

  PixelData pixelData2;
  DALI_TEST_EQUALS( (bool)pixelData2, false, TEST_LOCATION );

  pixelData2 = pixelData;
  DALI_TEST_EQUALS( (bool)pixelData2, true, TEST_LOCATION );

  END_TEST;
}

