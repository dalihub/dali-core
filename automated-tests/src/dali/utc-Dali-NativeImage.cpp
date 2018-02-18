/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

int UtcDaliNativeImageExtensionP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::NativeImage::GenerateTexture()" );

  TestNativeImagePointer testNativeImage = TestNativeImage::New( 16, 16 );
  DALI_TEST_CHECK( testNativeImage );
  DALI_TEST_CHECK( NULL != testNativeImage->GetExtension() );

  TestNativeImageNoExtPointer testNativeImage2 = TestNativeImageNoExt::New( 16, 16 );
  DALI_TEST_CHECK( testNativeImage2 );
  DALI_TEST_CHECK( NULL == testNativeImage2->GetExtension() );

  END_TEST;
}

int UtcDaliNativeImageGetCustomFragmentPreFixP(void)
{
  TestApplication application;
  TestNativeImagePointer nativeImageInterface = TestNativeImage::New( 16, 16 );
  NativeImage nativeImage = NativeImage::New( *(nativeImageInterface.Get()) );

  const char* preFix = "#extension GL_OES_EGL_image_external:require\n";
  DALI_TEST_EQUALS( nativeImage.GetCustomFragmentPreFix(), preFix, TEST_LOCATION );
  END_TEST;
}

int UtcDaliNativeImageGetCustomSamplerTypenameP(void)
{
  TestApplication application;
  TestNativeImagePointer nativeImageInterface = TestNativeImage::New( 16, 16 );
  NativeImage nativeImage = NativeImage::New( *(nativeImageInterface.Get()) );

  const char* samplerTypename = "samplerExternalOES";
  DALI_TEST_EQUALS( nativeImage.GetCustomSamplerTypename(), samplerTypename, TEST_LOCATION );
  END_TEST;
}



int UtcDaliNativeImageTestCreationFailure(void)
{
  TestApplication application;
  TestNativeImagePointer nativeImageInterface = TestNativeImage::New( 16, 16 );
  NativeImage nativeImage = NativeImage::New( *(nativeImageInterface.Get()) );

  tet_printf("Test what happens when GlExtensionCreate is called, and returns false to indicate an error\n");

  nativeImageInterface->SetGlExtensionCreateResult( false );

  Actor actor = CreateRenderableActor( nativeImage );
  actor.SetParentOrigin( ParentOrigin::CENTER );
  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Reset();
  textureTrace.Enable(true);

  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Reset();
  drawTrace.Enable(true);

  application.SendNotification();
  application.Render();

  // Test that nothing was rendered
  //GlExtensionCreate() called twice, once at initialization and once when trying to bind the texture
  DALI_TEST_EQUALS( nativeImageInterface->mExtensionCreateCalls, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( nativeImageInterface->mTargetTextureCalls, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), false, TEST_LOCATION );
  DALI_TEST_EQUALS( drawTrace.FindMethod("DrawElements") || drawTrace.FindMethod("DrawArrays"), false, TEST_LOCATION );

  textureTrace.Reset();
  drawTrace.Reset();

  nativeImageInterface->SetGlExtensionCreateResult( true );
  actor.SetPosition( 0, 0, 1 );
  application.SendNotification();
  application.Render();

  // This time around, the bind and draw should occur following the call to nativeImage->GlExtensionCreate.
  DALI_TEST_EQUALS( nativeImageInterface->mExtensionCreateCalls, 3, TEST_LOCATION );
  DALI_TEST_EQUALS( nativeImageInterface->mTargetTextureCalls, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( drawTrace.FindMethod("DrawElements") || drawTrace.FindMethod("DrawArrays"), true, TEST_LOCATION );

  END_TEST;
}
