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

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace {

Integration::Bitmap* CreateBitmap( unsigned int imageHeight, unsigned int imageWidth, unsigned int initialColor, Pixel::Format pixelFormat )
{
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::RETAIN );
  Integration::PixelBuffer* pixbuffer = bitmap->GetPackedPixelsProfile()->ReserveBuffer( pixelFormat,  imageWidth,imageHeight,imageWidth,imageHeight );
  unsigned int bytesPerPixel = GetBytesPerPixel(  pixelFormat );

  memset( pixbuffer,  initialColor , imageHeight*imageWidth*bytesPerPixel);

  return bitmap;
}

void InitialiseRegionsToZeroAlpha( Integration::Bitmap* image, unsigned int imageWidth, unsigned int imageHeight, Pixel::Format pixelFormat  )
{
  PixelBuffer* pixbuffer = image->GetBuffer();
  unsigned int bytesPerPixel = GetBytesPerPixel(  pixelFormat );

  for ( unsigned int row = 0; row < imageWidth; ++row )
  {
    unsigned int pixelOffset = (row*bytesPerPixel);
    pixbuffer[pixelOffset+3] = 0x00;
    pixelOffset += ( imageHeight-1 ) * imageWidth * bytesPerPixel;
    pixbuffer[pixelOffset+3] = 0x00;
  }

  for ( unsigned int column = 0; column < imageHeight; ++column )
  {
    unsigned int pixelOffset = (column*imageWidth*bytesPerPixel);
    pixbuffer[pixelOffset+3] = 0x00;
    pixelOffset += (imageWidth-1)*bytesPerPixel;
    pixbuffer[pixelOffset+3] = 0x00;
  }
}

void AddStretchRegionsToImage( Integration::Bitmap* image, unsigned int imageWidth, unsigned int imageHeight, const Vector4& requiredStretchBorder , Pixel::Format pixelFormat )
{
  PixelBuffer* pixbuffer = image->GetBuffer();
  unsigned int bytesPerPixel = GetBytesPerPixel(  pixelFormat );

  for ( unsigned int column = requiredStretchBorder.x; column < imageWidth - requiredStretchBorder.z; ++column )
  {
    unsigned int pixelOffset = (column*bytesPerPixel);
    pixbuffer[pixelOffset] = 0x00;
    pixbuffer[pixelOffset+1] = 0x00;
    pixbuffer[pixelOffset+2] = 0x00;
    pixbuffer[pixelOffset+3] = 0xFF;
  }

  for ( unsigned int row = requiredStretchBorder.y; row < imageHeight - requiredStretchBorder.w; ++row )
  {
    unsigned int pixelOffset = (row*imageWidth*bytesPerPixel);
    pixbuffer[pixelOffset] = 0x00;
    pixbuffer[pixelOffset+1] = 0x00;
    pixbuffer[pixelOffset+2] = 0x00;
    pixbuffer[pixelOffset+3] = 0xFF;
  }
}

void AddChildRegionsToImage( Integration::Bitmap* image, unsigned int imageWidth, unsigned int imageHeight, const Vector4& requiredChildRegion, Pixel::Format pixelFormat )
{
  PixelBuffer* pixbuffer = image->GetBuffer();
  unsigned int bytesPerPixel = GetBytesPerPixel(  pixelFormat );

  Integration::Bitmap::PackedPixelsProfile* srcProfile = image->GetPackedPixelsProfile();
  unsigned int bufferStride = srcProfile->GetBufferStride();

  // Add bottom child region
  for ( unsigned int column = requiredChildRegion.x; column < imageWidth - requiredChildRegion.z; ++column )
  {
    unsigned int pixelOffset = (column*bytesPerPixel);
    pixelOffset += ( imageHeight-1 ) * bufferStride;
    pixbuffer[pixelOffset] = 0x00;
    pixbuffer[pixelOffset+1] = 0x00;
    pixbuffer[pixelOffset+2] = 0x00;
    pixbuffer[pixelOffset+3] = 0xFF;
  }

  // Add right child region
  for ( unsigned int row = requiredChildRegion.y; row < imageHeight - requiredChildRegion.w; ++row )
  {
    unsigned int pixelOffset = row*bufferStride + (imageWidth-1)*bytesPerPixel;
    pixbuffer[pixelOffset] = 0x00;
    pixbuffer[pixelOffset+1] = 0x00;
    pixbuffer[pixelOffset+2] = 0x00;
    pixbuffer[pixelOffset+3] = 0xFF;
  }
}

} // namespace

int UtcDaliNinePatch01(void)
{
  /* Stretch region left(3) top(2) right (3) bottom (2)
   *    ss
   *  OOOOOO
   *  OOOOOOc
   * sOOooOOc
   * sOOooOOc
   *  OOOOOOc
   *  OOOOOO
   *   cccc
   */

  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("UtcDaliNinePatchImage - Stretch Regions");

  const unsigned int ninePatchImageHeight = 8;
  const unsigned int ninePatchImageWidth = 8;
  Pixel::Format pixelFormat = Pixel::RGBA8888;
  const Vector4 requiredStretchBorder( 3, 3, 3, 3);

  tet_infoline("Create Bitmap");
  platform.SetClosestImageSize(Vector2( ninePatchImageWidth, ninePatchImageHeight));
  Integration::Bitmap* bitmap = CreateBitmap( ninePatchImageWidth, ninePatchImageHeight, 0xFF, pixelFormat );

  tet_infoline("Clear border regions");
  InitialiseRegionsToZeroAlpha( bitmap, ninePatchImageWidth, ninePatchImageHeight, pixelFormat );

  tet_infoline("Add Stretch regions to Bitmap");
  AddStretchRegionsToImage( bitmap, ninePatchImageWidth, ninePatchImageHeight, requiredStretchBorder, pixelFormat );

  tet_infoline("Getting resource");
  Integration::ResourcePointer resourcePtr(bitmap);
  platform.SetResourceLoaded( 0, Dali::Integration::ResourceBitmap, resourcePtr );

  Image image = ResourceImage::New( "blah.#.png" );
  DALI_TEST_CHECK( image );

  tet_infoline("Assign image to ImageActor");
  ImageActor imageActor = ImageActor::New( image );
  DALI_TEST_CHECK( imageActor );
  Stage::GetCurrent().Add( imageActor );

  tet_infoline("Downcast Image to a nine-patch image\n");
  NinePatchImage ninePatchImage = NinePatchImage::DownCast( image );
  DALI_TEST_CHECK( ninePatchImage );

  if ( ninePatchImage )
  {
    tet_infoline("Get Stretch regions from NinePatch");
    Vector4 stretchBorders = ninePatchImage.GetStretchBorders();
    tet_printf("stretchBorders left(%f) right(%f) top(%f) bottom(%f)\n", stretchBorders.x, stretchBorders.z, stretchBorders.y, stretchBorders.w );
    DALI_TEST_CHECK( stretchBorders == requiredStretchBorder );
  }
  else
  {
    tet_infoline("Image not NinePatch");
    test_return_value = TET_FAIL;
  }

  END_TEST;
}

int UtcDaliNinePatch02(void)
{
  /* Child region x(2) y(2) width (4) height (4)
   *
   *    ss
   *  OOOOOO
   *  OOOOOOc
   * sOOooOOc
   * sOOooOOc
   *  OOOOOOc
   *  OOOOOO
   *   cccc
   */

  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("UtcDaliNinePatchImage - Child Regions");

  const unsigned int ninePatchImageHeight = 8;
  const unsigned int ninePatchImageWidth = 8;
  Pixel::Format pixelFormat = Pixel::RGBA8888;
  const Vector4 requiredChildRegion( 2, 2, 2, 2 );
  const Vector4 requiredStretchBorder( 3, 3, 3, 3);

  tet_infoline("Create Bitmap");
  platform.SetClosestImageSize(Vector2( ninePatchImageWidth, ninePatchImageHeight));
  Integration::Bitmap* bitmap = CreateBitmap( ninePatchImageWidth, ninePatchImageHeight, 0xFF, pixelFormat );

  tet_infoline("Clear border regions");
  InitialiseRegionsToZeroAlpha( bitmap, ninePatchImageWidth, ninePatchImageHeight, pixelFormat );

  tet_infoline("Add Stretch regions to Bitmap");
  AddStretchRegionsToImage( bitmap, ninePatchImageWidth, ninePatchImageHeight, requiredStretchBorder, pixelFormat );

  tet_infoline("Add Child regions to Bitmap");
  AddChildRegionsToImage( bitmap, ninePatchImageWidth, ninePatchImageHeight, requiredChildRegion, pixelFormat );

  tet_infoline("Getting resource");
  Integration::ResourcePointer resourcePtr(bitmap);
  platform.SetResourceLoaded( 0, Dali::Integration::ResourceBitmap, resourcePtr );
  Image image = ResourceImage::New( "blah.#.png" );
  DALI_TEST_CHECK( image );

  tet_infoline("Assign image to ImageActor");
  ImageActor imageActor = ImageActor::New( image );
  DALI_TEST_CHECK( imageActor );
  Stage::GetCurrent().Add( imageActor );

  tet_infoline("Downcast Image to a nine-patch image\n");
  NinePatchImage ninePatchImage = NinePatchImage::DownCast( image );
  DALI_TEST_CHECK( ninePatchImage );

  if ( ninePatchImage )
  {
    tet_infoline("Get Child regions from NinePatch");
    Rect<int> childRectangle = ninePatchImage.GetChildRectangle();
    tet_printf("childRectange x(%d) y(%d) width(%d) height(%d)\n", childRectangle.x, childRectangle.y, childRectangle.width, childRectangle.height );
    Rect<int> childRegion(requiredChildRegion.x, requiredChildRegion.y, ninePatchImageWidth - requiredChildRegion.x - requiredChildRegion.z, ninePatchImageHeight - requiredChildRegion.y - requiredChildRegion.w );
    DALI_TEST_CHECK( childRegion == childRectangle );
  }
  else
  {
    tet_infoline("Image not NinePatch");
    test_return_value = TET_FAIL;
  }

  END_TEST;
}
