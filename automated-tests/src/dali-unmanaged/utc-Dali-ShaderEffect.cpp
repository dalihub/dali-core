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

namespace
{

static const char* VertexSource = "VertexSource: this can be whatever you want it to be, but don't make it exact the same as default shader\n";

static const char* FragmentSource = "FragmentSource: this can be whatever you want it to be, but don't make it exact the same as default shader\n";

const int GETSOURCE_BUFFER_SIZE = 0x10000;

static const char* TestImageFilename = "icon_wrt.png";

Integration::Bitmap* CreateBitmap( unsigned int imageHeight, unsigned int imageWidth, unsigned int initialColor )
{
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::RETAIN );
  Integration::PixelBuffer* pixbuffer = bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  imageWidth,imageHeight,imageWidth,imageHeight );
  unsigned int bytesPerPixel = GetBytesPerPixel(  Pixel::RGBA8888 );

  memset( pixbuffer,  initialColor , imageHeight*imageWidth*bytesPerPixel);

  return bitmap;
}

} // Anonymous namespace


int UtcDaliShaderEffectFromProperties01(void)
{
  TestApplication application;
  tet_infoline("UtcDaliShaderEffectFromProperties01()");

  std::string fragmentShaderPrefix = "#define TEST_FS 1\n#extension GL_OES_standard_derivatives : enable";
  std::string vertexShaderPrefix = "#define TEST_VS 1";
  std::string vertexShader(VertexSource);
  std::string fragmentShader(FragmentSource);

  // Call render to compile default shaders.
  application.SendNotification();
  application.Render();

  GLuint lastShaderCompiledBefore = application.GetGlAbstraction().GetLastShaderCompiled();

  // create from type registry

  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "ShaderEffect" );
  DALI_TEST_CHECK( typeInfo );
  ShaderEffect effect = ShaderEffect::DownCast( typeInfo.CreateInstance() );
  DALI_TEST_CHECK( effect );

  Property::Value programMap = Property::Value(Property::MAP);

  programMap.SetValue("vertex", vertexShader);
  programMap.SetValue("fragment", fragmentShader);

  programMap.SetValue("vertex-prefix", vertexShaderPrefix);
  programMap.SetValue("fragment-prefix", fragmentShaderPrefix);

  programMap.SetValue("geometry-type", "GEOMETRY_TYPE_IMAGE");

  effect.SetProperty(effect.GetPropertyIndex("program"), programMap);

  Property::Value imageMap = Property::Value(Property::MAP);
  imageMap.SetValue("filename", Property::Value(TestImageFilename));
  effect.SetProperty(effect.GetPropertyIndex("image"), imageMap);

  // do a update & render to get the image request
  application.SendNotification();
  application.Render();

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  // create the image
  Integration::Bitmap* bitmap = CreateBitmap( 10, 10, 0xFF );
  Integration::ResourcePointer resourcePtr(bitmap);
  TestPlatformAbstraction& platform = application.GetPlatform();
  platform.SetResourceLoaded(request->GetId(), request->GetType()->id, resourcePtr);

  BitmapImage image(CreateBitmapImage());
  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  GLuint lastShaderCompiledAfter = application.GetGlAbstraction().GetLastShaderCompiled();

  // we should have compiled 2 shaders.
  DALI_TEST_EQUALS(lastShaderCompiledAfter, lastShaderCompiledBefore + 2, TEST_LOCATION );

  std::string actualVertexShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 1 );
  DALI_TEST_EQUALS( vertexShaderPrefix, actualVertexShader.substr( 0, vertexShaderPrefix.length() ), TEST_LOCATION );
  DALI_TEST_EQUALS( vertexShader, actualVertexShader.substr( actualVertexShader.length() - vertexShader.length() ), TEST_LOCATION );

  std::string actualFragmentShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 2 );
  DALI_TEST_EQUALS( fragmentShaderPrefix, actualFragmentShader.substr( 0, fragmentShaderPrefix.length() ), TEST_LOCATION );
  DALI_TEST_EQUALS( fragmentShader, actualFragmentShader.substr( actualFragmentShader.length() - fragmentShader.length() ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliShaderEffectFromProperties02(void)
{
  try
  {
    TestApplication application;
    tet_infoline("UtcDaliShaderEffectFromProperties02()");

    // Call render to compile default shaders.
    application.SendNotification();
    application.Render();
    application.Render();
    application.Render();

    // create from type registry (currently only way to get ShaderEffect with no shader setup in constructor
    TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "ShaderEffect" );
    DALI_TEST_CHECK( typeInfo );
    ShaderEffect effect = ShaderEffect::DownCast( typeInfo.CreateInstance() );
    DALI_TEST_CHECK( effect );

    Property::Value programMap = Property::Value(Property::MAP);

    programMap.SetValue("vertex",   std::string(VertexSource));
    programMap.SetValue("fragment", std::string(FragmentSource));

    // programMap.SetValue("geometry-type", "GEOMETRY_TYPE_IMAGE");
    // dont set by value
    programMap.SetValue("geometry-type", GeometryType( GEOMETRY_TYPE_IMAGE ));

    effect.SetProperty(effect.GetPropertyIndex("program"), programMap);

    tet_result( TET_FAIL );
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
  }
  END_TEST;
}

int UtcDaliShaderEffectFromProperties03(void)
{
  try
  {
    TestApplication application;
    tet_infoline("UtcDaliShaderEffectFromProperties03()");

    // Call render to compile default shaders.
    application.SendNotification();
    application.Render();
    application.Render();
    application.Render();

    // create from type registry (currently only way to get ShaderEffect with no shader setup in constructor
    TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "ShaderEffect" );
    DALI_TEST_CHECK( typeInfo );
    ShaderEffect effect = ShaderEffect::DownCast( typeInfo.CreateInstance() );
    DALI_TEST_CHECK( effect );

    // dont set unknown
    effect.SetProperty( effect.GetPropertyIndex("geometry-hints"), "HINT_2" );

    tet_result( TET_FAIL );
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
  }
  END_TEST;
}
