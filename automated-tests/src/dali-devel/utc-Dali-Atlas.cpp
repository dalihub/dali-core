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

void PrepareResourceImage( TestApplication& application, unsigned int imageWidth, unsigned int imageHeight, Pixel::Format pixelFormat )
{
  TestPlatformAbstraction& platform = application.GetPlatform();
  platform.SetClosestImageSize(Vector2( imageWidth, imageHeight));

  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  Integration::PixelBuffer* pixbuffer = bitmap->GetPackedPixelsProfile()->ReserveBuffer( pixelFormat, imageWidth, imageHeight, imageWidth, imageHeight );
  unsigned int bytesPerPixel = GetBytesPerPixel(  pixelFormat );
  unsigned int initialColor = 0xFF;
  memset( pixbuffer, initialColor, imageHeight*imageWidth*bytesPerPixel);

  Integration::ResourcePointer resourcePtr(bitmap);
  platform.SetSynchronouslyLoadedResource( resourcePtr );
}

PixelData CreatePixelData(unsigned int width, unsigned int height, Pixel::Format pixelFormat)
{
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( pixelFormat );
  unsigned char* buffer = new unsigned char [ bufferSize ];

  return PixelData::New( buffer, bufferSize, width, height, pixelFormat, PixelData::DELETE_ARRAY );
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

int UtcDaliAtlasNew(void)
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

int UtcDaliAtlasCopyConstructor(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 16, 16);
  Atlas atlasCopy(atlas);


  DALI_TEST_EQUALS( (bool)atlasCopy, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAtlasAssignmentOperator(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 16, 16);

  Atlas atlas2;
  DALI_TEST_EQUALS( (bool)atlas2, false, TEST_LOCATION );

  atlas2 = atlas;
  DALI_TEST_EQUALS( (bool)atlas2, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAtlasDownCast(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 16, 16);

  BaseHandle handle(atlas);
  Atlas atlasDowncast = Atlas::DownCast(handle);
  DALI_TEST_EQUALS( (bool)atlasDowncast, true, TEST_LOCATION );

  Handle handle2 = Handle::New(); // Create a custom object
  Atlas atlas2 = Atlas::DownCast(handle2);
  DALI_TEST_EQUALS( (bool)atlas2, false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAtlasClear(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 16, 32, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas );

  atlas.Clear(Color::TRANSPARENT);

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  callStack.Enable(false);

  std::stringstream out;
  out << GL_TEXTURE_2D <<", "<< 0u << ", " << 0u << ", " << 0u << ", " << 16u <<", "<< 32u;
  DALI_TEST_CHECK( callStack.FindMethodAndParams("TexSubImage2D", out.str().c_str() ) );

  END_TEST;
}


// Upload resource image, buffer image & pixel data with same pixel format
int UtcDaliAtlasUpload01P(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 16, 40, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas );

  // Using same pixel format
  PixelBuffer* buffer = new PixelBuffer[16 * 16 * 4];
  BufferImage image = BufferImage::New( buffer, 16, 16, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas.Upload( image, 0, 0 ) );

  PrepareResourceImage( application, 16, 16, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas.Upload( gTestImageFilename, 0, 16 ) );

  PixelData pixelData = CreatePixelData( 6,8,Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas.Upload( pixelData, 2, 32 ) );

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  callStack.Enable(false);

  {
    std::stringstream out;
    out << GL_TEXTURE_2D <<", "<< 0u << ", " << 0u << ", " << 0u << ", " << 16u <<", "<< 16u;
    DALI_TEST_CHECK( callStack.FindMethodAndParams("TexSubImage2D", out.str().c_str() ) );
  }
  {
    std::stringstream out;
    out << GL_TEXTURE_2D <<", "<< 0u << ", " << 0u << ", " << 16u << ", " << 16u <<", "<< 16u;
    DALI_TEST_CHECK( callStack.FindMethodAndParams("TexSubImage2D", out.str().c_str() ) );
  }
  {
    std::stringstream out;
    out << GL_TEXTURE_2D <<", "<< 0u << ", " << 2u << ", " << 32u << ", " << 6u <<", "<< 8u;
    DALI_TEST_CHECK( callStack.FindMethodAndParams("TexSubImage2D", out.str().c_str() ) );
  }

  END_TEST;
}

// Upload resource image, buffer image & pixel data with different pixel format
int UtcDaliAtlasUpload02P(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 20, 20, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas );

  // Using different pixel format
  PixelBuffer* buffer = new PixelBuffer[16 * 16 * 3];
  BufferImage image = BufferImage::New( buffer, 16, 16, Pixel::RGB888 );
  DALI_TEST_CHECK( atlas.Upload( image, 0, 0 ) );

  PrepareResourceImage( application, 12, 12, Pixel::A8 );
  DALI_TEST_CHECK( atlas.Upload( gTestImageFilename, 6, 6 ) );

  PixelData pixelData = CreatePixelData( 8,8,Pixel::LA88 );
  DALI_TEST_CHECK( atlas.Upload( pixelData, 10, 10 ) );

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  callStack.Enable(false);

  {
    std::stringstream out;
    out << GL_TEXTURE_2D <<", "<< 0u << ", " << 0u << ", " << 0u << ", " << 16u <<", "<< 16u;
    DALI_TEST_CHECK( callStack.FindMethodAndParams("TexSubImage2D", out.str().c_str() ) );
  }
  {
    std::stringstream out;
    out << GL_TEXTURE_2D <<", "<< 0u << ", " << 6u << ", " << 6u << ", " << 12u <<", "<< 12u;
    DALI_TEST_CHECK( callStack.FindMethodAndParams("TexSubImage2D", out.str().c_str() ) );
  }
  {
    std::stringstream out;
    out << GL_TEXTURE_2D <<", "<< 0u << ", " << 10u << ", " << 10u << ", " << 8u <<", "<< 8u;
    DALI_TEST_CHECK( callStack.FindMethodAndParams("TexSubImage2D", out.str().c_str() ) );
  }

  END_TEST;
}

// Upload resource image, buffer image & pixel data which cannot fit into the atlas with given offset
int UtcDaliAtlasUploadN(void)
{
  TestApplication application;

  Atlas atlas = Atlas::New( 16, 16, Pixel::RGBA8888 );
  DALI_TEST_CHECK( atlas );

  // Using image cannot fit into atlas at the given offsets
  PixelBuffer* buffer = new PixelBuffer[24 * 24 * 4];
  BufferImage image = BufferImage::New( buffer, 24, 24, Pixel::RGBA8888 );
  DALI_TEST_CHECK( !atlas.Upload( image, 0, 0 ) );

  PrepareResourceImage( application, 16, 16, Pixel::RGBA8888 );
  DALI_TEST_CHECK( !atlas.Upload( gTestImageFilename, 10, 10 ) );

  PixelData pixelData = CreatePixelData( 6,6,Pixel::RGBA8888 );
  DALI_TEST_CHECK( !atlas.Upload( pixelData, 11, 11 ) );

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  callStack.Enable(false);

  // none of these three upload() call sends texture to GPU
  DALI_TEST_CHECK( ! callStack.FindMethodAndParams("TexSubImage2D", "0, 0, 24, 24") );
  DALI_TEST_CHECK( ! callStack.FindMethodAndParams("TexSubImage2D", "10, 10, 16, 16") );
  DALI_TEST_CHECK( ! callStack.FindMethodAndParams("TexSubImage2D", "11, 11, 6, 6") );

  END_TEST;
}




