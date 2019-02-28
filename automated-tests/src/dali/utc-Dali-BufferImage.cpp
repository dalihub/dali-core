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

#include <iostream>
#include <algorithm>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using std::max;
using namespace Dali;

void utc_dali_buffer_image_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_buffer_image_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliBufferImageNew01(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBufferImageNew01 - BufferImage::New(unsigned int, unsigned int, Pixel::Format)");

  // invoke default handle constructor
  BufferImage image;

  // initialise handle
  image = BufferImage::New(16, 16);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( image.GetWidth() == 16);
  END_TEST;
}

int UtcDaliBufferImageNew02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBufferImageNew02 - BufferImage::New(PixelBuffer*, unsigned int, unsigned int, Pixel::Format, unsigned int)");

  PixelBuffer* buffer = new PixelBuffer[16 * 16];
  BufferImage image = BufferImage::New(buffer, 16, 16, Pixel::A8);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( image.GetWidth() == 16);

  delete [] buffer;
  END_TEST;
}

int UtcDaliBufferImageDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::BufferImage::DownCast()");

  Image image = BufferImage::New(1, 1, Dali::Pixel::BGRA8888);
  BufferImage bufferImage = BufferImage::DownCast( image );

  DALI_TEST_CHECK(bufferImage);
  END_TEST;
}

int UtcDaliBufferImageDownCast2(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::BufferImage::DownCast()");

  Image image = ResourceImage::New("IncorrectImageName");
  Actor actor = CreateRenderableActor( image );
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  BufferImage bufferImage = BufferImage::DownCast( image );
  DALI_TEST_CHECK(!bufferImage);

  Actor unInitialzedActor;
  bufferImage = BufferImage::DownCast( unInitialzedActor );
  DALI_TEST_CHECK(!bufferImage);
  END_TEST;
}

int UtcDaliBufferImageWHITE(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBufferImageWHITE - BufferImage::WHITE()");

  BufferImage image = BufferImage::WHITE();               // creates a 1x1 RGBA white pixel
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  PixelBuffer* buffer = image.GetBuffer();

  DALI_TEST_CHECK( image.GetWidth() == 1 &&               // 1 pixel wide
                   buffer != NULL &&                      // valid buffer
                   *buffer == 0xff);                       // r component is 255
  END_TEST;
}

int UtcDaliBufferImageGetBuffer(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBufferImageGetBuffer");

  BufferImage image = BufferImage::WHITE();               // creates a 1x1 RGBA white pixel

  PixelBuffer* buffer = image.GetBuffer();
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK( image.GetWidth() == 1 &&               // 1 pixel wide
                   buffer != NULL &&                      // valid buffer
                   *((unsigned int*)buffer) == 0xffffffff); // all component are 255
  END_TEST;
}

int UtcDaliBufferImageGetBufferSize(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBufferImageGetBufferSize");

  BufferImage image = BufferImage::WHITE();               // creates a 1x1 RGBA white pixel
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  PixelBuffer* buffer = image.GetBuffer();
  unsigned int bufferSize = image.GetBufferSize();
  unsigned int pixelSize = Pixel::GetBytesPerPixel(image.GetPixelFormat());

  DALI_TEST_CHECK( image.GetWidth() == 1 &&               // 1 pixel wide
                   buffer != NULL &&                      // valid buffer
                   bufferSize == pixelSize);              // r component is 255
  END_TEST;
}

int UtcDaliBufferImageGetBufferStride(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBufferImageGetBufferStride");

  BufferImage image = BufferImage::WHITE();               // creates a 1x1 RGBA white pixel
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  unsigned int pixelSize = Pixel::GetBytesPerPixel(image.GetPixelFormat());
  unsigned int bufferStride = image.GetBufferStride();
  DALI_TEST_CHECK( bufferStride == pixelSize );
  DALI_TEST_CHECK( !image.IsDataExternal() );

  PixelBuffer* buffer = new PixelBuffer[20 * 16];
  image = BufferImage::New(buffer, 16, 16, Pixel::A8, 20);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  bufferStride = image.GetBufferStride();

  DALI_TEST_CHECK( bufferStride == 20);
  DALI_TEST_CHECK( image.IsDataExternal() );

  delete [] buffer;
  END_TEST;
}

int UtcDaliBufferImageGetPixelFormat(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBufferImageGetPixelFormat");

  // Set pixel format to a non-default
  BufferImage image = BufferImage::New( 16, 16, Pixel::A8 );
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( image.GetPixelFormat() == Pixel::A8 );
  END_TEST;
}


int UtcDaliBufferImageIsDataExternal(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBufferImageIsDataExternal - BufferImage::IsDataExternal()");

  PixelBuffer* buffer = new PixelBuffer[16 * 16];
  BufferImage image = BufferImage::New(buffer, 16, 16, Pixel::A8);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK( image.IsDataExternal() );
  END_TEST;
}

int UtcDaliBufferImageUpdate01(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBufferImageUpdate01 - single empty rect");
  tet_infoline("   Test requires GraphicsController");
#if 0

  PixelBuffer* buffer = new PixelBuffer[16 * 16];

  BufferImage image = BufferImage::New(buffer, 16, 16, Pixel::A8);
  Actor actor = CreateRenderableActor( image );
  Stage::GetCurrent().Add(actor);
  actor.SetVisible(true);

  std::vector<GLuint> ids;
  ids.push_back(200);
  ids.push_back(201);
  ids.push_back(202);
  application.GetGlAbstraction().SetNextTextureIds(ids);

  // Allow actor to be staged and rendered
  application.SendNotification();
  application.Render(0);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( image.IsDataExternal() );
  application.GetGlAbstraction().EnableTextureCallTrace(true);

  image.Update();//(RectArea()); // notify Core that the image has been updated
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  const TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

  std::stringstream out;
  out << GL_TEXTURE_2D <<", "<< 0u << ", " << 16u <<", "<< 16u;
  DALI_TEST_EQUALS( callStack.TestMethodAndParams(0, "TexImage2D", out.str().c_str() ), true, TEST_LOCATION);

#endif
  END_TEST;
}

int UtcDaliBufferImageUpdate02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBufferImageUpdate02 - Multiple rects");
  tet_infoline("   Test requires GraphicsController");
#if 0

  PixelBuffer* buffer = new PixelBuffer[16 * 16];
  BufferImage image = BufferImage::New(buffer, 16, 16, Pixel::A8);
  Actor actor = CreateRenderableActor( image );
  Stage::GetCurrent().Add(actor);
  actor.SetVisible(true);

  application.SendNotification();
  application.Render(0);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( image.IsDataExternal() );
  application.GetGlAbstraction().EnableTextureCallTrace(true);

  // Check that multiple updates in a frame will be properly uploaded
  image.Update(RectArea(9,9,5,5));
  image.Update(RectArea(2,2,4,4));
  image.Update(RectArea(3,3,1,6));

  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  const TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();

  {
    std::stringstream out;
    out << GL_TEXTURE_2D <<", "<< 0u << ", " << 9u << ", " << 9u << ", " << 5u <<", "<< 5u;
    DALI_TEST_EQUALS( callStack.TestMethodAndParams(0, "TexSubImage2D", out.str().c_str()), true, TEST_LOCATION);
  }

  {
    std::stringstream out;
    out << GL_TEXTURE_2D <<", "<< 0u << ", " << 2u << ", " << 2u << ", " << 4u <<", "<< 4u;
    DALI_TEST_EQUALS( callStack.TestMethodAndParams(1, "TexSubImage2D", out.str().c_str()), true, TEST_LOCATION);
  }

  {
    std::stringstream out;
    out << GL_TEXTURE_2D <<", "<< 0u << ", " << 3u << ", " << 3u << ", " << 1u <<", "<< 6u;
    DALI_TEST_EQUALS( callStack.TestMethodAndParams(2, "TexSubImage2D", out.str().c_str()), true, TEST_LOCATION);
  }

#endif
  END_TEST;
}
