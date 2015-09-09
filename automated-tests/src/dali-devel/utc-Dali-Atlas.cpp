/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/bitmap.h>
#include <dali/devel-api/images/atlas.h>
#include <dali-test-suite-utils.h>
#include <test-native-image.h>

using namespace Dali;

namespace
{
static const char* gTestImageFilename = "icon_wrt.png";

void PrepareResourceImage( TestApplication& application, unsigned int imageHeight, unsigned int imageWidth, Pixel::Format pixelFormat )
{
  TestPlatformAbstraction& platform = application.GetPlatform();
  platform.SetClosestImageSize(Vector2( 16, 16));

  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  Integration::PixelBuffer* pixbuffer = bitmap->GetPackedPixelsProfile()->ReserveBuffer( pixelFormat, imageWidth, imageHeight, imageWidth, imageHeight );
  unsigned int bytesPerPixel = GetBytesPerPixel(  pixelFormat );
  unsigned int initialColor = 0xFF;
  memset( pixbuffer, initialColor, imageHeight*imageWidth*bytesPerPixel);

  Integration::ResourcePointer resourcePtr(bitmap);
  platform.SetResourceLoaded( 0, Dali::Integration::ResourceBitmap, resourcePtr );
}

}

void utc_dali_atlas_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_atlas_cleanup(void)
{
  test_return_value = TET_PASS;
}

// 1.1
int UtcDaliAtlasNew01(void)
{
  TestApplication application;

  // invoke default handle constructor
  Atlas atlas;

  DALI_TEST_CHECK( !atlas );

  // initialise handle
  atlas = Atlas::New( 16, 16 );

  DALI_TEST_CHECK( atlas );
  END_TEST;
}


// 1.2
int UtcDaliAtlasUpload01(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 16, 16, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas );

  // Using correct pixel format
  PixelBuffer* buffer = new PixelBuffer[16 * 16 * 4];
  BufferImage image = BufferImage::New( buffer, 16, 16, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas.Upload( image, 0, 0 ) );

  PrepareResourceImage( application, 16, 16, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas.Upload( gTestImageFilename, 0, 0 ) );

  END_TEST;
}

// 1.3
int UtcDaliAtlasUpload02(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 10, 10, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas );

  // Using INCORRECT pixel format
  PixelBuffer* buffer = new PixelBuffer[16 * 16];
  BufferImage image = BufferImage::New( buffer, 16, 16, Pixel::A8 );
  DALI_TEST_CHECK( !atlas.Upload( image, 0, 0 ) );

  PrepareResourceImage( application, 16, 16, Pixel::A8 );
  DALI_TEST_CHECK( !atlas.Upload( gTestImageFilename, 0, 0 ) );

  END_TEST;
}

// 1.4
int UtcDaliAtlasUpload03(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 10, 10, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas );

  // Using image too big for atlas
  PixelBuffer* buffer = new PixelBuffer[16 * 16 * 4];
  BufferImage image = BufferImage::New( buffer, 16, 16, Pixel::RGBA8888 );
  DALI_TEST_CHECK( !atlas.Upload( image, 0, 0 ) );

  PrepareResourceImage( application, 16, 16, Pixel::RGBA8888 );
  DALI_TEST_CHECK( !atlas.Upload( gTestImageFilename, 0, 0 ) );

  END_TEST;
}

// 1.5
int UtcDaliAtlasUpload04(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 32, 32, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas );

  // Using valid offsets
  PixelBuffer* buffer = new PixelBuffer[16 * 16 * 4];
  BufferImage image = BufferImage::New( buffer, 16, 16, Pixel::RGBA8888 );

  DALI_TEST_CHECK( atlas.Upload( image,  0,  0 ) );
  DALI_TEST_CHECK( atlas.Upload( image, 16,  0 ) );
  DALI_TEST_CHECK( atlas.Upload( image,  0, 16 ) );
  DALI_TEST_CHECK( atlas.Upload( image, 16, 16 ) );

  PrepareResourceImage( application, 16, 16, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas.Upload( gTestImageFilename,  0,  0 ) );
  DALI_TEST_CHECK( atlas.Upload( gTestImageFilename, 16,  0 ) );
  DALI_TEST_CHECK( atlas.Upload( gTestImageFilename,  0, 16 ) );
  DALI_TEST_CHECK( atlas.Upload( gTestImageFilename, 16, 16 ) );

  END_TEST;
}

// 1.6
int UtcDaliAtlasUpload05(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 32, 32, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas );

  // Using invalid offsets
  PixelBuffer* buffer = new PixelBuffer[16 * 16 * 4];
  BufferImage image = BufferImage::New( buffer, 16, 16, Pixel::RGBA8888 );

  DALI_TEST_CHECK( !atlas.Upload( image,  0, 17 ) );
  DALI_TEST_CHECK( !atlas.Upload( image, 17,  0 ) );
  DALI_TEST_CHECK( !atlas.Upload( image, 17, 17 ) );
  DALI_TEST_CHECK( !atlas.Upload( image, 99,  0 ) );
  DALI_TEST_CHECK( !atlas.Upload( image,  0, 99 ) );
  DALI_TEST_CHECK( !atlas.Upload( image, 99, 99 ) );

  PrepareResourceImage( application, 16, 16, Pixel::RGBA8888 );

  DALI_TEST_CHECK( !atlas.Upload( gTestImageFilename,  0, 17 ) );
  DALI_TEST_CHECK( !atlas.Upload( gTestImageFilename, 17,  0 ) );
  DALI_TEST_CHECK( !atlas.Upload( gTestImageFilename, 17, 17 ) );
  DALI_TEST_CHECK( !atlas.Upload( gTestImageFilename, 99,  0 ) );
  DALI_TEST_CHECK( !atlas.Upload( gTestImageFilename,  0, 99 ) );
  DALI_TEST_CHECK( !atlas.Upload( gTestImageFilename, 99, 99 ) );

  END_TEST;
}

