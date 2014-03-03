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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali-test-suite-utils.h>
#include "../../../build/slp/dali-core/dali-shaders.h"

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

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

static const char* FragmentSourceUsingExtensions =
"void main()\n"
"{\n"
"  float floatValue = 0.5f;\n"
"  float test = fwidth(floatValue);\n"
"  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n"
"  gl_FragColor.a *= test;\n"
"}\n";

const int GETSOURCE_BUFFER_SIZE = 0x10000;

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

static const char* TestImageFilename = "icon_wrt.png";

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliShaderEffectMethodNew01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodNew02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodNew03, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodNew04, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodNew05, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodNew06, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodDelete01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodSetUniformFloat, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodSetUniformVector2, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodSetUniformVector3, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodSetUniformVector4, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodSetUniformMatrix, POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliShaderEffectMethodSetUniformMatrix3, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodSetUniformViewport, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodSetEffectImage, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodSetEffectImageAndDelete, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodApplyConstraint, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodApplyConstraintFromActor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodApplyConstraintFromActor2, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodApplyConstraintCallback, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodRemoveConstraints, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodRemoveConstraints2, POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliShaderEffectMethodCreateExtension, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodCreateExtension2, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectMethodNoExtension, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectFromProperties01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectFromProperties02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliShaderEffectFromProperties03, NEGATIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

// Create bitmap image
BitmapImage CreateBitmapImage()
{
  BitmapImage image = BitmapImage::New(4,4,Pixel::RGBA8888);

  PixelBuffer* pixbuf = image.GetBuffer();

  // Using a 4x4 image gives a better blend with the GL implementation
  // than a 3x3 image
  for(size_t i=0; i<16; i++)
  {
    pixbuf[i*4+0] = 0xFF;
    pixbuf[i*4+1] = 0xFF;
    pixbuf[i*4+2] = 0xFF;
    pixbuf[i*4+3] = 0xFF;
  }

  return image;
}

struct TestConstraintToVector3
{
  TestConstraintToVector3(Vector3 target)
  : mTarget(target)
  {
  }

  Vector3 operator()(const Vector3& current)
  {
    return mTarget;
  }

  Vector3 mTarget;
};

struct TestConstraintFromPositionToVector3
{
  TestConstraintFromPositionToVector3()
  {
  }

  Vector3 operator()(const Vector3& current, const PropertyInput& position)
  {

    return position.GetVector3();
  }
};

struct TestConstraintToVector3Double
{
  TestConstraintToVector3Double(Vector3 target)
  : mTarget(target)
  {
  }

  Vector3 operator()(const Vector3& current)
  {
    return mTarget * 2.0f;
  }

  Vector3 mTarget;
};

static void UtcDaliShaderEffectMethodNew01()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK(effect);
}

static void UtcDaliShaderEffectMethodNew02()
{
  TestApplication application;

  ShaderEffect effect;

  try
  {
    // New must be called to create a ShaderEffect or it wont be valid.
    effect.SetUniform( "uUniform", 0 );
    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK( !effect );
  }
}

static void UtcDaliShaderEffectMethodNew03()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource, VertexSource, FragmentSource, ShaderEffect::HINT_NONE );
  DALI_TEST_CHECK(effect);
}

static void UtcDaliShaderEffectMethodNew04()
{
  TestApplication application;
  tet_infoline("Testing prefixed version of Dali::ShaderEffect::New()");

  std::string fragmentShaderPrefix = "#define TEST_FS 1\n#extension GL_OES_standard_derivatives : enable";
  std::string vertexShaderPrefix = "#define TEST_VS 1";

  try
  {
    // Call render to compile default shaders.
    application.SendNotification();
    application.Render();
    application.Render();
    application.Render();

    GLuint lastShaderCompiledBefore = application.GetGlAbstraction().GetLastShaderCompiled();
    ShaderEffect effect = ShaderEffect::NewWithPrefix( vertexShaderPrefix, VertexSource,
                                                       fragmentShaderPrefix, FragmentSourceUsingExtensions,
                                                       GEOMETRY_TYPE_IMAGE, ShaderEffect::HINT_NONE );

    BitmapImage image = CreateBitmapImage();
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
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    tet_result( TET_FAIL );
  }
}

class ShaderEffectExtension : public ShaderEffect::Extension {};
static void UtcDaliShaderEffectMethodNew05()
{
  TestApplication application;

  // heap constructor / destructor
  DefaultFunctionCoverage<ShaderEffect> shaderEffect;
  DefaultFunctionCoverage<ShaderEffectExtension> shaderEffectExtension;

}

static void UtcDaliShaderEffectMethodNew06()
{
  TestApplication application;
  tet_infoline("Testing Dali::ShaderEffect::New() with shader sources for different geometry types");

  ShaderEffect effect = ShaderEffect::New( "imageVertexShader", "imageFragmentShader",
                                           "textVertexShader", "textFragmentShader",
                                           "texturedMeshVertexShader", "texturedMeshFragmentShader",
                                           "meshVertexShader", "meshFragmentShader",
                                           ShaderEffect::HINT_NONE );
  DALI_TEST_CHECK(effect);
}


static void UtcDaliShaderEffectMethodDownCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::ShaderEffect::DownCast()");

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );

  BaseHandle object(effect);

  ShaderEffect effect2 = ShaderEffect::DownCast(object);
  DALI_TEST_CHECK(effect2);

  ShaderEffect effect3 = DownCast< ShaderEffect >(object);
  DALI_TEST_CHECK(effect3);

  BaseHandle unInitializedObject;
  ShaderEffect effect4 = ShaderEffect::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!effect4);

  ShaderEffect effect5 = DownCast< ShaderEffect >(unInitializedObject);
  DALI_TEST_CHECK(!effect5);
}

static void UtcDaliShaderEffectMethodDelete01()
{
   TestApplication application;

   // get the default shaders built, this is not required but makes it
   // easier to debug the TET case and isolate the custom shader compilation.
   application.SendNotification();
   application.Render();

   application.SendNotification();
   application.Render();

   // create a new shader effect
   // the vertex and fragment shader will be cached in the ShaderFactory
   ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource);

   // destroy the shader effect
   effect.Reset();

   // Create the same shader effect again, this should now use the cached version
   // held in the shader factory
   effect= ShaderEffect::New( VertexSource, FragmentSource );

   // Compile the shader effect
   application.SendNotification();
   application.Render();

   GLuint lastShaderCompiled = application.GetGlAbstraction().GetLastShaderCompiled();

   // get the vertex shader (compiled before fragment shader).
   // this last shaders compiled is for text.
   GLuint vertexShaderId = lastShaderCompiled-1;
   GLsizei lengthVertexResult;

   char testVertexSourceResult[GETSOURCE_BUFFER_SIZE];
   application.GetGlAbstraction().GetShaderSource(vertexShaderId, GETSOURCE_BUFFER_SIZE, &lengthVertexResult, testVertexSourceResult);

   // compare the first 10 bytes of the vertex shader sent to be compiled, with
   // the shader string that ended up being compiled (in the render task)
   // this is to confirm the string hasn't been deleted / corrupted.
   int testPassed = strncmp( testVertexSourceResult , Dali::Internal::CustomFontPrefixVertex.c_str(), 10 ) ;

   DALI_TEST_CHECK( testPassed == 0 );
}

static void UtcDaliShaderEffectMethodSetUniformFloat()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uFloat", 1.0f );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uFloat", 1.0f ) );
}

static void UtcDaliShaderEffectMethodSetUniformVector2()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uVec2", Vector2( 2.0f, 3.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec2", Vector2( 2.0f, 3.0f ) ) );
}

static void UtcDaliShaderEffectMethodSetUniformVector3()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uVec3", Vector3( 4.0f, 5.0f, 6.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 4.0f, 5.0f, 6.0f ) ) );
}

static void UtcDaliShaderEffectMethodSetUniformVector4()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uVec4", Vector4( 7.0f, 8.0f, 9.0f, 10.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec4", Vector4( 7.0f, 8.0f, 9.0f, 10.0f ) ) );
}

static void UtcDaliShaderEffectMethodSetUniformMatrix()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uModelView", Matrix::IDENTITY );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uModelView", Matrix::IDENTITY ) );
}

static void UtcDaliShaderEffectMethodSetUniformMatrix3()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  Matrix3 matIdentity(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  effect.SetUniform( "uMatrix3", matIdentity );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue("uMatrix3", matIdentity) );
}

static void UtcDaliShaderEffectMethodSetUniformViewport()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  effect.SetUniform( "uVec2", Vector2( 0.0f, 0.0f ), ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION );

  application.SendNotification();
  application.Render();

  const Vector2& stageSize(Stage::GetCurrent().GetSize());

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec2", Vector2( -stageSize.x/2, stageSize.y/2 ) ) );
}

static void UtcDaliShaderEffectMethodSetEffectImage()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetEffectImage(image);

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "sEffect", 1 ) );
}

static void UtcDaliShaderEffectMethodSetEffectImageAndDelete()
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );

  BitmapImage effectImage = CreateBitmapImage();
  effect.SetEffectImage(effectImage);

  Integration::ResourceId imageId = GetImplementation(effectImage).GetResourceId();
  tet_printf("Effect Image id %d \n ",imageId);


  ImageActor actor = ImageActor::New();

  actor.SetShaderEffect(effect);
  effect.Reset();

  Stage::GetCurrent().Add(actor);

  // do an update / render cycle
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  printf("removing image actor from stage and Reseting handle\n");
  Stage::GetCurrent().Remove(actor);
  actor.Reset();

  tet_printf("### Update & Render  \n");

  application.SendNotification();
  application.Render(16);

  tet_printf("#### Update Only  \n");

  tet_printf("effectImage.Reset \n");

  // this releases the effect texture resource,
  // Update will send a DispatchDiscardTexture message to render
  effectImage.Reset();
  application.SendNotification();
  application.UpdateOnly(16);

  tet_printf("#### Update Only \n");

  // at this point shader is deleted, during clear discard queue
  // and it sends a Shader:: DispatchRemoveObserver message to render thread
  application.UpdateOnly(16);

  tet_printf("#### Render Only  \n");
  // This is where it used to crash, there is a message in the queue to perform DispatchDiscardTexture
  // which tries to call observer->TextureDiscarded, where observer == shader that was deleted
  // in previous update.
  application.RenderOnly();


  // process the discard texture message
  application.RenderOnly();
  application.SendNotification();
  application.Render(16);

  tet_result(TET_PASS);

}
static void UtcDaliShaderEffectMethodApplyConstraint()
{
  // Test whether Shader's uniform can be constrained to a stationary constraint.
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  application.SendNotification();
  application.Render();

  // Test effects of SetUniform...
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );

  Constraint constraint = Constraint::New<Vector3>( uVecProperty,
                                                    TestConstraintToVector3(Vector3(4.0f, 9.0f, 16.0f)) );

  effect.ApplyConstraint(constraint);

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 4.0f, 9.0f, 16.0f ) ) );
}


static void UtcDaliShaderEffectMethodApplyConstraintFromActor()
{
  // Test whether Shader's uniform can be constrained to Actor's position.
  TestApplication application;

  const Vector3 targetPosition = Vector3( 100.0f, 70.0f, 20.0f);

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uVec3", Vector3( 50.0f, 25.0f, 0.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetPosition(targetPosition);
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  Constraint constraint = Constraint::New<Vector3>( uVecProperty,
                                                    Source(actor, Actor::POSITION),
                                                    TestConstraintFromPositionToVector3() );

  effect.ApplyConstraint(constraint);

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", targetPosition ) );
}

static void UtcDaliShaderEffectMethodApplyConstraintFromActor2()
{
  // Test whether Shader's uniform can be constrained to Actor's position.
  // While Actor's position is constrained to another point * 2.0f
  TestApplication application;

  const Vector3 targetPosition = Vector3( 25.0f, 36.0f, 49.0f );

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uVec3", Vector3( 50.0f, 25.0f, 0.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetPosition(Vector3( 100.0f, 70.0f, 20.0f));
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  Constraint shaderConstraint = Constraint::New<Vector3>( uVecProperty,
                                                    Source(actor, Actor::POSITION),
                                                    TestConstraintFromPositionToVector3() );

  effect.ApplyConstraint(shaderConstraint);

  Constraint actorConstraint = Constraint::New<Vector3>( Actor::POSITION,
                                                         TestConstraintToVector3Double(targetPosition) );

  actor.ApplyConstraint(actorConstraint);

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", targetPosition * 2.0f ) );
}

static void UtcDaliShaderEffectMethodApplyConstraintCallback()
{
  // Test whether Shader's uniform can be constrained to a stationary constraint.
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  application.SendNotification();
  application.Render();

  // Test effects of SetUniform...
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );

  Constraint constraint = Constraint::New<Vector3>( uVecProperty,
                                                    TestConstraintToVector3(Vector3(4.0f, 9.0f, 16.0f)) );

  constraint.SetApplyTime( 10.0f );

  bool constraintCheck( false );
  ConstraintAppliedCheck appliedCheck( constraintCheck );

  // We should receive the "Applied" signal after 10 seconds
  ActiveConstraint active = effect.ApplyConstraint(constraint);
  active.AppliedSignal().Connect( &application, appliedCheck );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f)); // 1 elapsed second

  // Check signal has not fired
  application.SendNotification();
  appliedCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(4000.0f)); // 5 elapsed seconds

  // Check signal has not fired
  application.SendNotification();
  appliedCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(5000.0f - 1.0f)); // <10 elapsed seconds

  // Check signal has not fired
  application.SendNotification();
  appliedCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(2.0f)); // >10 elapsed seconds

  // Signal should have fired
  application.SendNotification();
  appliedCheck.CheckSignalReceived();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 4.0f, 9.0f, 16.0f ) ) );
}

static void UtcDaliShaderEffectMethodRemoveConstraints()
{
  // Test if constrains can be removed before they are ever applyed.
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  application.SendNotification();
  application.Render();

  // Test effects of SetUniform...
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );

  Constraint constraint = Constraint::New<Vector3>( uVecProperty,
                                                    TestConstraintToVector3(Vector3(4.0f, 9.0f, 16.0f)) );

  effect.ApplyConstraint(constraint);

  // Remove the constraints
  effect.RemoveConstraints();

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );
}

static void UtcDaliShaderEffectMethodRemoveConstraints2()
{
  // Test whether Shader's uniform constrains can be removed after they are applyed.
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  effect.SetUniform( "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  application.SendNotification();
  application.Render();

  // Test effects of SetUniform...
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );

  Constraint constraint = Constraint::New<Vector3>( uVecProperty,
                                                    TestConstraintToVector3(Vector3(4.0f, 9.0f, 16.0f)) );

  effect.ApplyConstraint(constraint);

  application.SendNotification();
  application.Render();

  // Reset the value and remove the constraints
  effect.SetUniform( "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) );
  effect.RemoveConstraints();

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );
}


class TestExtension : public ShaderEffect::Extension
{
public:
  TestExtension( bool& deleted )
  : mDeleted(deleted)
  {
    mDeleted = false;
  }
  ~TestExtension()
  {
    mDeleted = true;
  }
  bool IsAlive() const
  {
    return !mDeleted;
  }
private:
  bool& mDeleted;
};

static void UtcDaliShaderEffectMethodCreateExtension()
{
  // Test creation of a shader extension
  TestApplication aplication;

  bool deleted;
  {
    ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
    DALI_TEST_CHECK( effect );

    TestExtension* extension = new TestExtension ( deleted );

    effect.AttachExtension( extension );

    DALI_TEST_CHECK( static_cast<TestExtension&>(effect.GetExtension()).IsAlive() );
  }

  DALI_TEST_CHECK( deleted );
}

static void UtcDaliShaderEffectMethodCreateExtension2()
{
  // Test creation of a shader extension
  bool deleted;
  {
    TestApplication application;

    ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
    DALI_TEST_CHECK( effect );

    BitmapImage image = CreateBitmapImage();

    effect.SetUniform( "uFloat", 1.0f );

    ImageActor actor = ImageActor::New( image );
    actor.SetSize( 100.0f, 100.0f );
    actor.SetName("TestImageFilenameActor");
    actor.SetShaderEffect(effect);
    Stage::GetCurrent().Add(actor);

    application.SendNotification();
    application.Render();

    TestExtension* extension = new TestExtension ( deleted );

    effect.AttachExtension( extension );

    const ShaderEffect& constEffect(effect);
    const TestExtension& ext( static_cast<const TestExtension&>(constEffect.GetExtension()) );

    DALI_TEST_CHECK( ext.IsAlive() );
  }

  DALI_TEST_CHECK( deleted );
}

static void UtcDaliShaderEffectMethodNoExtension()
{
  TestApplication application;

  ShaderEffect effect;

  try
  {
    ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
    DALI_TEST_CHECK( effect );

    // Don't attach extension
    ShaderEffect::Extension& extension = effect.GetExtension();
    (void) extension;

    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK( !effect );
  }
}

static void UtcDaliShaderEffectFromProperties01()
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

  BitmapImage image = CreateBitmapImage();
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

}

static void UtcDaliShaderEffectFromProperties02()
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
}

static void UtcDaliShaderEffectFromProperties03()
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
}
