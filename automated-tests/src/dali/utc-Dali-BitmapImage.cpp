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
#include <dali-test-suite-utils.h>

using std::max;
using namespace Dali;

void utc_dali_bitmap_image_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_bitmap_image_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliBitmapImageNew01(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageNew01 - BitmapImage::New(unsigned int, unsigned int, Pixel::Format)");

  // invoke default handle constructor
  BitmapImage image;

  // initialise handle
  image = BitmapImage::New(16, 16);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  ImageAttributes attributes = image.GetAttributes();

  DALI_TEST_CHECK( attributes.GetWidth() == 16);
  END_TEST;
}

int UtcDaliBitmapImageNew02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageNew02 - BitmapImage::New(PixelBuffer*, unsigned int, unsigned int, Pixel::Format, unsigned int)");

  PixelBuffer* buffer = new PixelBuffer[16 * 16];
  BitmapImage image = BitmapImage::New(buffer, 16, 16, Pixel::A8);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  Dali::ImageAttributes attributes = image.GetAttributes();

  DALI_TEST_CHECK( attributes.GetWidth() == 16);

  delete [] buffer;
  END_TEST;
}

int UtcDaliBitmapImageNewWithPolicy01(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageNewWithPolicy01 - BitmapImage::New(unsigned int, unsigned int, Pixel::Format, LoadPolicy, ReleasePolicy)");

  // Force texture id's
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  // invoke default handle constructor
  BitmapImage image;

  // initialise handle
  image = BitmapImage::New(16, 16, Pixel::A8, Image::OnDemand, Image::Unused);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  ImageAttributes attributes = image.GetAttributes();
  DALI_TEST_CHECK( attributes.GetWidth() == 16);
  /// @todo: how to test OnDemand? - resource id would be 0 if buffer only allocated on first call to ::GetBuffer()

  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);
  // testing ReleasePolicy::Unused
  // fake loading image
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK ( !application.GetGlAbstraction().CheckTextureDeleted(23) );

  // discard texture when actor comes off stage
  Stage::GetCurrent().Remove(actor);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK ( application.GetGlAbstraction().CheckTextureDeleted(23) );
  END_TEST;
}

int UtcDaliBitmapImageNewWithPolicy02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageNewWithPolicy02 - BitmapImage::New(PixelBuffer*, unsigned int, unsigned int, Pixel::Format, unsigned int, ReleasePolicy)");

  // Force texture id's
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  PixelBuffer* buffer = new PixelBuffer[16 * 16];
  BitmapImage image = BitmapImage::New(buffer, 16, 16, Pixel::A8, 16, Image::Unused);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  ImageAttributes attributes = image.GetAttributes();
  DALI_TEST_CHECK( attributes.GetWidth() == 16);
  /// @todo: how to test OnDemand? - resource id would be 0 if buffer only allocated on first call to ::GetBuffer()

  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);
  // testing ReleasePolicy::Unused
  // fake loading image
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK ( !application.GetGlAbstraction().CheckTextureDeleted(23) );

  // discard texture when actor comes off stage
  Stage::GetCurrent().Remove(actor);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK ( application.GetGlAbstraction().CheckTextureDeleted(23) );
  END_TEST;
}

int UtcDaliBitmapImageDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::BitmapImage::DownCast()");

  BitmapImage bitmap = BitmapImage::New(1, 1, Dali::Pixel::BGRA8888);
  ImageActor imageActor = ImageActor::New(bitmap);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  Image image = imageActor.GetImage();
  BitmapImage bitmapImage = BitmapImage::DownCast( image );

  DALI_TEST_CHECK(bitmapImage);
  END_TEST;
}

int UtcDaliBitmapImageDownCast2(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::BitmapImage::DownCast()");

  Image image = Image::New("IncorrectImageName");
  ImageActor imageActor = ImageActor::New(image);
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  Image image1 = imageActor.GetImage();

  BitmapImage bitmapImage = BitmapImage::DownCast( image1 );
  DALI_TEST_CHECK(!bitmapImage);

  Actor unInitialzedActor;
  bitmapImage = BitmapImage::DownCast( unInitialzedActor );
  DALI_TEST_CHECK(!bitmapImage);
  END_TEST;
}

int UtcDaliBitmapImageWHITE(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageWHITE - BitmapImage::WHITE()");

  BitmapImage image = BitmapImage::WHITE();               // creates a 1x1 RGBA white pixel
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  Dali::ImageAttributes attributes = image.GetAttributes();
  PixelBuffer* buffer = image.GetBuffer();

  DALI_TEST_CHECK( attributes.GetWidth() == 1 &&               // 1 pixel wide
                   buffer != NULL &&                      // valid buffer
                   *buffer == 0xff);                       // r component is 255
  END_TEST;
}

int UtcDaliBitmapImageGetBuffer(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageGetBuffer");

  BitmapImage image = BitmapImage::WHITE();               // creates a 1x1 RGBA white pixel

  PixelBuffer* buffer = image.GetBuffer();
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  ImageAttributes attributes = image.GetAttributes();
  DALI_TEST_CHECK( attributes.GetWidth() == 1 &&               // 1 pixel wide
                   buffer != NULL &&                      // valid buffer
                   *((unsigned int*)buffer) == 0xffffffff); // all component are 255
  END_TEST;
}

int UtcDaliBitmapImageGetBufferSize(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageGetBufferSize");

  BitmapImage image = BitmapImage::WHITE();               // creates a 1x1 RGBA white pixel
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  Dali::ImageAttributes attributes = image.GetAttributes();
  PixelBuffer* buffer = image.GetBuffer();
  unsigned int bufferSize = image.GetBufferSize();
  unsigned int pixelSize = Pixel::GetBytesPerPixel(attributes.GetPixelFormat());

  DALI_TEST_CHECK( attributes.GetWidth() == 1 &&               // 1 pixel wide
                   buffer != NULL &&                      // valid buffer
                   bufferSize == pixelSize);              // r component is 255
  END_TEST;
}

int UtcDaliBitmapImageGetBufferStride(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageGetBufferStride");

  BitmapImage image = BitmapImage::WHITE();               // creates a 1x1 RGBA white pixel
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  Dali::ImageAttributes attributes = image.GetAttributes();
  unsigned int pixelSize = Pixel::GetBytesPerPixel(attributes.GetPixelFormat());
  unsigned int bufferStride = image.GetBufferStride();
  DALI_TEST_CHECK( bufferStride == pixelSize );
  DALI_TEST_CHECK( !image.IsDataExternal() );

  PixelBuffer* buffer = new PixelBuffer[20 * 16];
  image = BitmapImage::New(buffer, 16, 16, Pixel::A8, 20);
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

int UtcDaliBitmapImageGetPixelFormat(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageGetPixelFormat");

  // Set pixel format to a non-default
  BitmapImage image = BitmapImage::New( 16, 16, Pixel::A8 );
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( image.GetPixelFormat() == Pixel::A8 );
  END_TEST;
}


int UtcDaliBitmapImageIsDataExternal(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageIsDataExternal - BitmapImage::IsDataExternal()");

  PixelBuffer* buffer = new PixelBuffer[16 * 16];
  BitmapImage image = BitmapImage::New(buffer, 16, 16, Pixel::A8);
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK( image.IsDataExternal() );
  END_TEST;
}

namespace
{

static bool SignalReceived;
static void ImageUploaded(Image image)
{
  tet_infoline("Received image uploaded signal");
  SignalReceived = true;
}
}

int UtcDaliBitmapImageUpdate01(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageUpdate01 - single empty rect");

  PixelBuffer* buffer = new PixelBuffer[16 * 16];

  BitmapImage image = BitmapImage::New(buffer, 16, 16, Pixel::A8);
  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);
  actor.SetVisible(true);

  SignalReceived = false;
  image.UploadedSignal().Connect( ImageUploaded );

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
  DALI_TEST_EQUALS( callStack.TestMethodAndParams(0, "TexSubImage2D", "0, 0, 16, 16"), true, TEST_LOCATION);

  DALI_TEST_CHECK( SignalReceived == true );
  SignalReceived = false;
  END_TEST;
}

int UtcDaliBitmapImageUpdate02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageUpdate02 - Multiple rects");

  PixelBuffer* buffer = new PixelBuffer[16 * 16];
  BitmapImage image = BitmapImage::New(buffer, 16, 16, Pixel::A8);
  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);
  actor.SetVisible(true);

  SignalReceived = false;
  image.UploadedSignal().Connect( ImageUploaded );

  std::vector<GLuint> ids;
  ids.push_back(200);
  ids.push_back(201);
  ids.push_back(202);
  application.GetGlAbstraction().SetNextTextureIds(ids);

  application.SendNotification();
  application.Render(0);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( image.IsDataExternal() );
  application.GetGlAbstraction().EnableTextureCallTrace(true);

  image.Update(RectArea(9,9,5,5));              // notify Core that the image has been updated

  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  const TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  DALI_TEST_EQUALS( callStack.TestMethodAndParams(0, "TexSubImage2D", "9, 9, 5, 1"), true, TEST_LOCATION);
  DALI_TEST_EQUALS( callStack.TestMethodAndParams(1, "TexSubImage2D", "9, 10, 5, 1"), true, TEST_LOCATION);
  DALI_TEST_EQUALS( callStack.TestMethodAndParams(2, "TexSubImage2D", "9, 11, 5, 1"), true, TEST_LOCATION);
  DALI_TEST_EQUALS( callStack.TestMethodAndParams(3, "TexSubImage2D", "9, 12, 5, 1"), true, TEST_LOCATION);
  DALI_TEST_EQUALS( callStack.TestMethodAndParams(4, "TexSubImage2D", "9, 13, 5, 1"), true, TEST_LOCATION);

  DALI_TEST_CHECK( SignalReceived == true );
  SignalReceived = false;
  END_TEST;
}

int UtcDaliBitmapImageUploadedSignal01(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageUploadedSignal - Test that Uploaded signal is sent when image is staged");

  PixelBuffer* buffer = new PixelBuffer[16 * 16];
  BitmapImage image = BitmapImage::New(buffer, 16, 16, Pixel::A8);
  SignalReceived = false;
  image.UploadedSignal().Connect( ImageUploaded );

  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  Dali::ImageActor imageActor = ImageActor::New(image);
  Stage::GetCurrent().Add(imageActor);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( SignalReceived == true );
  END_TEST;
}

int UtcDaliBitmapImageUploadedSignal02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliBitmapImageUploadedSignal - Test that Uploaded signal is sent after Update");

  PixelBuffer* buffer = new PixelBuffer[16 * 16];
  BitmapImage image = BitmapImage::New(buffer, 16, 16, Pixel::A8);
  SignalReceived = false;
  //ScopedConnection connection =
  image.UploadedSignal().Connect( ImageUploaded );

  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  Dali::ImageActor imageActor = ImageActor::New(image);
  Stage::GetCurrent().Add(imageActor);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK( SignalReceived == true );
  SignalReceived = false;

  image.Update(RectArea());              // notify Core that the whole image has been updated
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK( SignalReceived == true );
  END_TEST;
}
