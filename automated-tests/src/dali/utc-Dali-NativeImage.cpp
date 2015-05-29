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
#include <algorithm>
#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <test-native-image.h>
#include <test-intrusive-ptr.h>

using namespace Dali;

void utc_dali_native_image_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_native_image_cleanup(void)
{
  test_return_value = TET_PASS;
}

IntrusivePtr<TestNativeImage> Creator()
{
  return TestNativeImage::New(10,10);
}

int UtcDaliIntrusivePtrTestNativeImage(void)
{
  UtcCoverageIntrusivePtr<TestNativeImage> pointer;

  pointer.Check(Creator);

  END_TEST;
}

int UtcDaliNativeImageNew(void)
{
  TestApplication application;

  tet_infoline("UtcDaliNativeImageNew - NativeImage::New(NativeImageInterface&)");

  // invoke default handle constructor
  NativeImage image;
  TestNativeImagePointer nativeImage = TestNativeImage::New(16, 16);

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = NativeImage::New(*(nativeImage.Get()));

  DALI_TEST_CHECK( image );
  END_TEST;
}

int UtcDaliNativeImageCopyConstructor(void)
{
  TestApplication application;

  tet_infoline("UtcDaliNativeImageCopyConstructor - NativeImage::NativeImage( const NativeImage& )");

  NativeImage image1;
  DALI_TEST_CHECK( !image1 );

  TestNativeImagePointer nativeImage = TestNativeImage::New(16, 16);
  image1 = NativeImage::New(*(nativeImage.Get()));
  NativeImage image2( image1 );

  DALI_TEST_CHECK( image2 );
  DALI_TEST_EQUALS( image1, image2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliNativeImageDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Image::DownCast()");

  TestNativeImagePointer nativeImage = TestNativeImage::New(16, 16);
  NativeImage image = NativeImage::New(*(nativeImage.Get()));

  BaseHandle object(image);

  NativeImage image2 = NativeImage::DownCast(object);
  DALI_TEST_CHECK(image2);

  NativeImage image3 = DownCast< NativeImage >(object);
  DALI_TEST_CHECK(image3);

  BaseHandle unInitializedObject;
  NativeImage image4 = NativeImage::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!image4);

  NativeImage image5 = DownCast< NativeImage >(unInitializedObject);
  DALI_TEST_CHECK(!image5);

  Image image6 = NativeImage::New(*(nativeImage.Get()));
  NativeImage image7 = NativeImage::DownCast(image6);
  DALI_TEST_CHECK(image7);
  END_TEST;
}

int UtcDaliNativeImageCreateGlTextureN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::NativeImage::GenerateGlTexture()" );

  NativeImage image;
  try
  {
    image.CreateGlTexture();
    tet_printf( "Assertion test failed - no Exception\n" );
    tet_result( TET_FAIL );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "image &&", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliNativeImageCreateGlTextureP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::NativeImage::GenerateGlTexture()" );

  TestNativeImagePointer imageInterface = TestNativeImage::New( 16, 16 );
  NativeImage image = NativeImage::New( *(imageInterface.Get()) );
  DALI_TEST_CHECK( image );

  image.CreateGlTexture();

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS( imageInterface->mExtensionCreateCalls, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( imageInterface->mTargetTextureCalls, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliNativeImageContextLoss(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::NativeImage behaviour through a context lost/regained cycle." );

  // Build an image that is expected to have a GL texture created for it and
  // recreated in a GL context recovery:
  TestNativeImagePointer eagerImageInterface = TestNativeImage::New( 16, 16 );
  NativeImage eagerImage = NativeImage::New( *(eagerImageInterface.Get()) );
  DALI_TEST_CHECK( eagerImage );

  // Build a regular lazy-allocated image for comparison:
  TestNativeImagePointer lazyImageInterface = TestNativeImage::New( 16, 16 );
  NativeImage lazyImage = NativeImage::New( *(lazyImageInterface.Get()) );
  DALI_TEST_CHECK( lazyImage );

  eagerImage.CreateGlTexture();

  application.SendNotification();
  application.Render(16);

  // Cycle through a context loss and regain, asserting that the texture is
  // not reallocated if it already existed before the cycle and is never allocated
  // throughout the cycle if of the regular lazy kind:

  // Call render thread context destroyed / created functions:
  application.ResetContext();

  // Call event thread function:
  application.GetCore().RecoverFromContextLoss();

  // Run update/render loop
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS( eagerImageInterface->mExtensionCreateCalls, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( eagerImageInterface->mTargetTextureCalls, 1, TEST_LOCATION );

  DALI_TEST_EQUALS( lazyImageInterface->mExtensionCreateCalls, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( lazyImageInterface->mTargetTextureCalls, 0, TEST_LOCATION );

  END_TEST;
}
