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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace {

Integration::Bitmap* CreateBitmap( unsigned int imageWidth, unsigned int imageHeight, unsigned int initialColor, Pixel::Format pixelFormat )
{
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  Integration::PixelBuffer* pixbuffer = bitmap->GetPackedPixelsProfile()->ReserveBuffer( pixelFormat, imageWidth, imageHeight, imageWidth, imageHeight );
  unsigned int bytesPerPixel = GetBytesPerPixel( pixelFormat );

  memset( pixbuffer, initialColor, imageHeight * imageWidth * bytesPerPixel );

  return bitmap;
}

void InitialiseRegionsToZeroAlpha( Integration::Bitmap* image, unsigned int imageWidth, unsigned int imageHeight, Pixel::Format pixelFormat )
{
  PixelBuffer* pixbuffer = image->GetBuffer();
  unsigned int bytesPerPixel = GetBytesPerPixel( pixelFormat );

  for( unsigned int row = 0; row < imageWidth; ++row )
  {
    unsigned int pixelOffset = row * bytesPerPixel;
    pixbuffer[ pixelOffset + 3 ] = 0x00;
    pixelOffset += ( imageHeight - 1 ) * imageWidth * bytesPerPixel;
    pixbuffer[ pixelOffset + 3 ] = 0x00;
  }

  for ( unsigned int column = 0; column < imageHeight; ++column )
  {
    unsigned int pixelOffset = column * imageWidth * bytesPerPixel;
    pixbuffer[ pixelOffset + 3 ] = 0x00;
    pixelOffset += ( imageWidth -1 ) * bytesPerPixel;
    pixbuffer[ pixelOffset + 3 ] = 0x00;
  }
}

void AddStretchRegionsToImage( Integration::Bitmap* image, unsigned int imageWidth, unsigned int imageHeight, const Vector4& requiredStretchBorder, Pixel::Format pixelFormat )
{
  PixelBuffer* pixbuffer = image->GetBuffer();
  unsigned int bytesPerPixel = GetBytesPerPixel( pixelFormat );

  for( unsigned int column = requiredStretchBorder.x; column < imageWidth - requiredStretchBorder.z; ++column )
  {
    unsigned int pixelOffset = column * bytesPerPixel;
    pixbuffer[ pixelOffset ] = 0x00;
    pixbuffer[ pixelOffset + 1 ] = 0x00;
    pixbuffer[ pixelOffset + 2 ] = 0x00;
    pixbuffer[ pixelOffset + 3 ] = 0xFF;
  }

  for( unsigned int row = requiredStretchBorder.y; row < imageHeight - requiredStretchBorder.w; ++row )
  {
    unsigned int pixelOffset = row * imageWidth * bytesPerPixel;
    pixbuffer[ pixelOffset ] = 0x00;
    pixbuffer[ pixelOffset + 1 ] = 0x00;
    pixbuffer[ pixelOffset + 2 ] = 0x00;
    pixbuffer[ pixelOffset + 3 ] = 0xFF;
  }
}

void AddChildRegionsToImage( Integration::Bitmap* image, unsigned int imageWidth, unsigned int imageHeight, const Vector4& requiredChildRegion, Pixel::Format pixelFormat )
{
  PixelBuffer* pixbuffer = image->GetBuffer();
  unsigned int bytesPerPixel = GetBytesPerPixel( pixelFormat );

  Integration::Bitmap::PackedPixelsProfile* srcProfile = image->GetPackedPixelsProfile();
  unsigned int bufferStride = srcProfile->GetBufferStride();

  // Add bottom child region
  for( unsigned int column = requiredChildRegion.x; column < imageWidth - requiredChildRegion.z; ++column )
  {
    unsigned int pixelOffset = column * bytesPerPixel;
    pixelOffset += ( imageHeight - 1 ) * bufferStride;
    pixbuffer[ pixelOffset ] = 0x00;
    pixbuffer[ pixelOffset + 1 ] = 0x00;
    pixbuffer[ pixelOffset + 2 ] = 0x00;
    pixbuffer[ pixelOffset + 3 ] = 0xFF;
  }

  // Add right child region
  for ( unsigned int row = requiredChildRegion.y; row < imageHeight - requiredChildRegion.w; ++row )
  {
    unsigned int pixelOffset = row * bufferStride + ( imageWidth - 1 ) * bytesPerPixel;
    pixbuffer[ pixelOffset ] = 0x00;
    pixbuffer[ pixelOffset + 1 ] = 0x00;
    pixbuffer[ pixelOffset + 2 ] = 0x00;
    pixbuffer[ pixelOffset + 3 ] = 0xFF;
  }
}

NinePatchImage CustomizeNinePatch( TestApplication& application,
                                unsigned int ninePatchImageWidth,
                                unsigned int ninePatchImageHeight,
                                const Vector4& requiredStretchBorder,
                                bool addChildRegion = false,
                                Vector4 requiredChildRegion = Vector4::ZERO )
{
  TestPlatformAbstraction& platform = application.GetPlatform();

  Pixel::Format pixelFormat = Pixel::RGBA8888;

  tet_infoline("Create Bitmap");
  platform.SetClosestImageSize(Vector2( ninePatchImageWidth, ninePatchImageHeight));
  Integration::Bitmap* bitmap = CreateBitmap( ninePatchImageWidth, ninePatchImageHeight, 0xFF, pixelFormat );

  tet_infoline("Clear border regions");
  InitialiseRegionsToZeroAlpha( bitmap, ninePatchImageWidth, ninePatchImageHeight, pixelFormat );

  tet_infoline("Add Stretch regions to Bitmap");
  AddStretchRegionsToImage( bitmap, ninePatchImageWidth, ninePatchImageHeight, requiredStretchBorder, pixelFormat );

  if( addChildRegion )
  {
    tet_infoline("Add Child regions to Bitmap");
    AddChildRegionsToImage( bitmap, ninePatchImageWidth, ninePatchImageHeight, requiredChildRegion, pixelFormat );
  }

  tet_infoline("Getting resource");
  Integration::ResourcePointer resourcePtr(bitmap);
  platform.SetResourceLoaded( 0, Dali::Integration::ResourceBitmap, resourcePtr );

  Image image = ResourceImage::New( "blah.#.png" );

  tet_infoline("Assign image to ImageActor");
  ImageActor imageActor = ImageActor::New( image );
  Stage::GetCurrent().Add( imageActor );

  tet_infoline("Downcast Image to a nine-patch image\n");
  NinePatchImage ninePatchImage = NinePatchImage::DownCast( image );

  return ninePatchImage;

}

} // namespace

int UtcDaliNinePatchImageNew(void)
{
  TestApplication application;
  tet_infoline("UtcDaliNinePatchImageNew - NinePatchImage::New(const std::string&)");

  // invoke default handle constructor
  NinePatchImage image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = NinePatchImage::New( "blah.#.png" );

  DALI_TEST_CHECK( image );
  END_TEST;
}

int UtcDaliNinePatchImageDowncast(void)
{
  TestApplication application;
  tet_infoline("UtcDaliNinePatchImageDowncast - NinePatchImage::DownCast(BaseHandle)");

  NinePatchImage image = NinePatchImage::New( "blah.#.png" );

  BaseHandle object( image );

  NinePatchImage image2 = NinePatchImage::DownCast( object );
  DALI_TEST_CHECK( image2 );

  NinePatchImage image3 = DownCast< NinePatchImage >( object );
  DALI_TEST_CHECK( image3 );

  BaseHandle unInitializedObject;
  NinePatchImage image4 = NinePatchImage::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !image4 );

  NinePatchImage image5 = DownCast< NinePatchImage >( unInitializedObject );
  DALI_TEST_CHECK( !image5 );

  Image image6 = NinePatchImage::New( "blah.#.png" );
  NinePatchImage image7 = NinePatchImage::DownCast( image6 );
  DALI_TEST_CHECK( image7 );
  END_TEST;
}

int UtcDaliNinePatchImageCopyConstructor(void)
{
  TestApplication application;

  tet_infoline("UtcDaliNinePatchImageCopyConstructor - NinePatchImage::NinePatchImage( const NinePatchImage& )");

  NinePatchImage image1;
  DALI_TEST_CHECK( !image1 );

  image1 = NinePatchImage::New( "blah.#.png" );
  NinePatchImage image2( image1 );

  DALI_TEST_CHECK( image2 );
  DALI_TEST_EQUALS( image1, image2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliNinePatchImageGetStrechBorders(void)
{
  TestApplication application;
  tet_infoline("UtcDaliNinePatchImageGetStrechBorders - NinePatchImage::GetStretchBorders()");

  /* Stretch region left(2) top(2) right (2) bottom (2)
    *    ss
    *  OOOOOO
    *  OOOOOOc
    * sOOooOOc
    * sOOooOOc
    *  OOOOOOc
    *  OOOOOO
    *   cccc
    */

   const unsigned int ninePatchImageHeight = 18;
   const unsigned int ninePatchImageWidth = 28;
   const Vector4 requiredStretchBorder( 3, 4, 5, 6 );

   NinePatchImage ninePatchImage = CustomizeNinePatch( application, ninePatchImageWidth, ninePatchImageHeight, requiredStretchBorder );
   DALI_TEST_CHECK( ninePatchImage );

   if( ninePatchImage )
   {
     tet_infoline("Get Stretch regions from NinePatch");

     const NinePatchImage::StretchRanges& stretchPixelsX = ninePatchImage.GetStretchPixelsX();
     const NinePatchImage::StretchRanges& stretchPixelsY = ninePatchImage.GetStretchPixelsY();

     DALI_TEST_CHECK( stretchPixelsX.Size() == 1 );
     DALI_TEST_CHECK( stretchPixelsY.Size() == 1 );

     Vector4 stretchBorders;
     //The NinePatchImage stretch pixels are in the cropped image space, inset by 1 to get it to uncropped image space
     stretchBorders.x = stretchPixelsX[ 0 ].GetX() + 1;
     stretchBorders.y = stretchPixelsY[ 0 ].GetX() + 1;
     stretchBorders.z = ninePatchImageWidth - stretchPixelsX[ 0 ].GetY() - 1;
     stretchBorders.w = ninePatchImageHeight - stretchPixelsY[ 0 ].GetY() - 1;

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

int UtcDaliNinePatchImageGetChildRectangle(void)
{
  TestApplication application;
  tet_infoline("UtcDaliNinePatchImageGetChildRectangle - NinePatchImage::GetChildRectangle()");

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

  const unsigned int ninePatchImageHeight = 18;
  const unsigned int ninePatchImageWidth = 28;
  const Vector4 requiredChildRegion( 2, 2, 2, 2 );
  const Vector4 requiredStretchBorder( 3, 4, 5, 6 );

  NinePatchImage ninePatchImage = CustomizeNinePatch( application,ninePatchImageWidth, ninePatchImageHeight, requiredStretchBorder, true, requiredChildRegion );
  DALI_TEST_CHECK( ninePatchImage );

  if ( ninePatchImage )
  {
    tet_infoline("Get Child regions from NinePatch");
    Rect< int > childRectangle = ninePatchImage.GetChildRectangle();
    tet_printf("childRectange x(%d) y(%d) width(%d) height(%d)\n", childRectangle.x, childRectangle.y, childRectangle.width, childRectangle.height );
    Rect< int > childRegion(requiredChildRegion.x, requiredChildRegion.y, ninePatchImageWidth - requiredChildRegion.x - requiredChildRegion.z, ninePatchImageHeight - requiredChildRegion.y - requiredChildRegion.w );
    DALI_TEST_CHECK( childRegion == childRectangle );
  }
  else
  {
    tet_infoline("Image not NinePatch");
    test_return_value = TET_FAIL;
  }

  END_TEST;
}

int UtcDaliNinePatchImageCreateCroppedBufferImage(void)
{
  TestApplication application;
  tet_infoline("UtcDaliNinePatchImageCreateCroppedBufferImage - NinePatchImage::CreateCroppedBufferImage()");

  const unsigned int ninePatchImageHeight = 8;
  const unsigned int ninePatchImageWidth = 8;
  const Vector4 requiredStretchBorder( 1, 1, 1, 1 );

  NinePatchImage ninePatchImage = CustomizeNinePatch( application, ninePatchImageWidth, ninePatchImageHeight, requiredStretchBorder  );
  DALI_TEST_CHECK( ninePatchImage );

  if ( ninePatchImage )
  {
    BufferImage newImage = ninePatchImage.CreateCroppedBufferImage();
    DALI_TEST_CHECK( newImage );

    DALI_TEST_EQUALS( newImage.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

    DALI_TEST_EQUALS( newImage.GetBufferSize(), 144u/* 36*4 */, TEST_LOCATION );
  }
  else
  {
    tet_infoline("Image not NinePatch");
    test_return_value = TET_FAIL;
  }

  END_TEST;
}

int UtcDaliNinePatchImageIsNinePatchUrl(void)
{
  TestApplication application;
  tet_infoline("UtcDaliNinePatchImageIsNinePatchUrl - NinePatchImage::IsNinePatchUrl(const std::string&)");

  DALI_TEST_CHECK( NinePatchImage::IsNinePatchUrl( "test.9.jpg" ) );
  DALI_TEST_CHECK( NinePatchImage::IsNinePatchUrl( "test.#.jpg" ) );
  DALI_TEST_CHECK( !NinePatchImage::IsNinePatchUrl( "test.9" ) );
  DALI_TEST_CHECK( !NinePatchImage::IsNinePatchUrl( "test.#" ) );
  DALI_TEST_CHECK( !NinePatchImage::IsNinePatchUrl( "test" ) );

  END_TEST;
}
