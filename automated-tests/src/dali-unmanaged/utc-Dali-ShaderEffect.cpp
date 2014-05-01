//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{

static const char* VertexSource =
"void main()\n"
"{\n"
"  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
"  vTexCoord = aTexCoord;\n"
"}\n";

static const char* FragmentSource =
"void main()\n"
"{\n"
"  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n"
"}\n";

const int GETSOURCE_BUFFER_SIZE = 0x10000;

static const char* TestImageFilename = "icon_wrt.png";

} // Anonymous namespace


int UtcDaliShaderEffectFromProperties01(void)
{
  TestApplication application;
  tet_infoline("UtcDaliShaderEffectFromProperties01()");

  std::string fragmentShaderPrefix = "#define TEST_FS 1\n#extension GL_OES_standard_derivatives : enable";
  std::string vertexShaderPrefix = "#define TEST_VS 1";

  // Call render to compile default shaders.
  application.SendNotification();
  application.Render();
  application.Render();
  application.Render();

  GLuint lastShaderCompiledBefore = application.GetGlAbstraction().GetLastShaderCompiled();

  // create from type registry

  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "ShaderEffect" );
  DALI_TEST_CHECK( typeInfo );
  ShaderEffect effect = ShaderEffect::DownCast( typeInfo.CreateInstance() );
  DALI_TEST_CHECK( effect );

  Property::Value programMap = Property::Value(Property::MAP);

  programMap.SetValue("vertex", std::string(VertexSource));
  programMap.SetValue("fragment", std::string(FragmentSource));

  programMap.SetValue("vertex-prefix", std::string(fragmentShaderPrefix));
  programMap.SetValue("fragment-prefix", std::string(vertexShaderPrefix));

  programMap.SetValue("geometry-type", "GEOMETRY_TYPE_IMAGE");

  effect.SetProperty(effect.GetPropertyIndex("program"), programMap);

  Property::Value imageMap = Property::Value(Property::MAP);
  imageMap.SetValue("filename", Property::Value(TestImageFilename));

  effect.SetProperty(effect.GetPropertyIndex("image"), imageMap);

  BitmapImage image(CreateBitmapImage());

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  GLuint lastShaderCompiledAfter = application.GetGlAbstraction().GetLastShaderCompiled();
  bool testResult = false;

  // we should have compiled 4 shaders.
  DALI_TEST_CHECK(lastShaderCompiledAfter - lastShaderCompiledBefore == 4);
  if (lastShaderCompiledAfter - lastShaderCompiledBefore == 4)
  {
    char testVertexSourceResult[GETSOURCE_BUFFER_SIZE];
    char testFragmentSourceResult[GETSOURCE_BUFFER_SIZE];

    // we are interested in the first two.
    GLuint vertexShaderId = lastShaderCompiledBefore + 1;
    GLuint fragmentShaderId = lastShaderCompiledBefore + 2;

    GLsizei lengthVertexResult;
    GLsizei lengthFragmentResult;

    application.GetGlAbstraction().GetShaderSource(vertexShaderId, GETSOURCE_BUFFER_SIZE, &lengthVertexResult, testVertexSourceResult);
    application.GetGlAbstraction().GetShaderSource(fragmentShaderId, GETSOURCE_BUFFER_SIZE, &lengthFragmentResult, testFragmentSourceResult);

    int vertexShaderHasPrefix = strncmp(testVertexSourceResult, "#define ", strlen("#define "));
    int fragmentShaderHasPrefix = strncmp(testFragmentSourceResult, "#define ", strlen("#define "));
    testResult = (vertexShaderHasPrefix == 0) && (fragmentShaderHasPrefix == 0);
  }
  DALI_TEST_CHECK(testResult);
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
