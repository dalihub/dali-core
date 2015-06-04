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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void renderable_actor_startup(void)
{
  test_return_value = TET_UNDEF;
}

void renderable_actor_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static bool gIsActor1SortModifierCorrect;
static bool gIsActor2SortModifierCorrect;
const float gActor1SortModifierValue = 96.0f;
const float gActor2SortModifierValue = 53.0f;

static float TestSortFunction(const Vector3& position, float sortModifier)
{
  if ( fabs(sortModifier - gActor1SortModifierValue) < 0.01)
    gIsActor1SortModifierCorrect = true;

  if ( fabs(sortModifier - gActor2SortModifierValue) < 0.01)
    gIsActor2SortModifierCorrect = true;

  return 0.0f;
}

} // anon namespace

int UtcDaliRenderableActorDownCast(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::DownCast()");

  ImageActor imageActor = ImageActor::New();

  Actor anActor = Actor::New();
  anActor.Add( imageActor );

  Actor child = anActor.GetChildAt(0);
  RenderableActor renderableActor = RenderableActor::DownCast( child );
  DALI_TEST_CHECK( renderableActor );

  renderableActor.Reset();
  DALI_TEST_CHECK( !renderableActor );

  renderableActor = DownCast< RenderableActor >( child );
  DALI_TEST_CHECK( renderableActor );

  renderableActor = DownCast< RenderableActor >( anActor );
  DALI_TEST_CHECK( !renderableActor );

  Actor unInitialzedActor;
  renderableActor = RenderableActor::DownCast( unInitialzedActor );
  DALI_TEST_CHECK( !renderableActor );

  renderableActor = DownCast< RenderableActor >( unInitialzedActor );
  DALI_TEST_CHECK( !renderableActor );
  END_TEST;
}

int UtcDaliRenderableActorSetSortModifier(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetSortModifier()");

  float val = -500.0f;

  ImageActor actor = ImageActor::New();
  Stage::GetCurrent().Add(actor);

  actor.SetSortModifier( val );

  float returnValue = actor.GetSortModifier();

  //Verify through actor api
  DALI_TEST_EQUALS(returnValue, val, TEST_LOCATION);

  Stage::GetCurrent().Remove(actor);


  //Verify through layer SetSortFunction
  gIsActor1SortModifierCorrect = false;
  gIsActor2SortModifierCorrect = false;
  BufferImage img = BufferImage::New( 1,1 );
  // create two transparent actors so there is something to sort
  ImageActor actor1 = ImageActor::New( img );
  ImageActor actor2 = ImageActor::New( img );
  actor1.SetSize(1,1);
  actor1.SetPosition( 0, 0, 0);
  actor1.SetSortModifier( gActor1SortModifierValue );
  actor1.SetColor( Vector4(1, 1, 1, 0.5f ) ); // 50% transparent
  actor2.SetSize(1,1);
  actor2.SetPosition( 0, 0, 1);
  actor2.SetSortModifier( gActor2SortModifierValue );
  actor2.SetColor( Vector4(1, 1, 1, 0.5f ) ); // 50% transparent

  // add to stage
   Stage::GetCurrent().Add( actor1 );
   Stage::GetCurrent().Add( actor2 );

   Layer root = Stage::GetCurrent().GetLayer( 0 );
   root.SetSortFunction( TestSortFunction );

   // flush the queue and render once
   application.SendNotification();
   application.Render();

   DALI_TEST_CHECK( gIsActor1SortModifierCorrect && gIsActor2SortModifierCorrect );
   END_TEST;

}

int UtcDaliRenderableActorGetSortModifier(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::GetSortModifier()");

  ImageActor actor = ImageActor::New();
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS(actor.GetSortModifier(), 0.0f, TEST_LOCATION);

  Stage::GetCurrent().Remove(actor);
  END_TEST;
}

int UtcDaliRenderableActorSetGetBlendMode(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetBlendMode() / Dali::RenderableActor::GetBlendMode()");

  ImageActor actor = ImageActor::New();

  actor.SetBlendMode( BlendingMode::OFF );
  DALI_TEST_CHECK( BlendingMode::OFF == actor.GetBlendMode() );

  actor.SetBlendMode( BlendingMode::AUTO );
  DALI_TEST_CHECK( BlendingMode::AUTO == actor.GetBlendMode() );

  actor.SetBlendMode( BlendingMode::ON );
  DALI_TEST_CHECK( BlendingMode::ON == actor.GetBlendMode() );
  END_TEST;
}

int UtcDaliRenderableActorSetCullFace(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetCullFace()");

  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );
  ImageActor actor2 = ImageActor::New( img );

  actor.SetSize(100.0f, 100.0f);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetAnchorPoint(AnchorPoint::CENTER);

  actor2.SetSize(100.0f, 100.0f);
  actor2.SetParentOrigin(ParentOrigin::CENTER);
  actor2.SetAnchorPoint(AnchorPoint::CENTER);

  Stage::GetCurrent().Add(actor);
  Stage::GetCurrent().Add(actor2);

  //Verify whether the correct GL calls are made when actor is face culled in front and back, and
  // face culling is disabled for actor2
  TraceCallStack& cullFaceTrace = application.GetGlAbstraction().GetCullFaceTrace();
  cullFaceTrace.Enable(true);
  actor.SetCullFace( CullFrontAndBack );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  cullFaceTrace.Enable(false);
  std::stringstream out;

  //Verify actor gl state
  out.str("");
  out << GL_BLEND;
  DALI_TEST_EQUALS( cullFaceTrace.TestMethodAndParams(0, "Enable", out.str()), true, TEST_LOCATION);

  out.str("");
  out << GL_CULL_FACE;
  DALI_TEST_EQUALS( cullFaceTrace.TestMethodAndParams(1, "Enable", out.str()), true, TEST_LOCATION);

  out.str("");
  out << GL_FRONT_AND_BACK;
  DALI_TEST_EQUALS( cullFaceTrace.TestMethodAndParams(2, "CullFace", out.str()), true, TEST_LOCATION);

  //Verify actor2 gl state
  out.str("");
  out << GL_CULL_FACE;
  DALI_TEST_EQUALS( cullFaceTrace.TestMethodAndParams(3, "Disable", out.str()), true, TEST_LOCATION);

  //Verify state through the actor api
  DALI_TEST_CHECK( CullFrontAndBack == actor.GetCullFace() );
  DALI_TEST_CHECK( CullNone == actor2.GetCullFace() );

  /**************************************************************/

  //Verify whether the correct GL calls are made when actor2 is face culled in the front
  cullFaceTrace.Reset();
  cullFaceTrace.Enable(true);
  actor2.SetCullFace( CullFront );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  cullFaceTrace.Enable(false);

  //Verify actor gl state
  out.str("");
  out << GL_CULL_FACE;
  DALI_TEST_EQUALS( cullFaceTrace.TestMethodAndParams(0, "Enable", out.str()), true, TEST_LOCATION);

  out.str("");
  out << GL_FRONT_AND_BACK;
  DALI_TEST_EQUALS( cullFaceTrace.TestMethodAndParams(1, "CullFace", out.str()), true, TEST_LOCATION);

  //Verify actor2 gl state
  out.str("");
  out << GL_CULL_FACE;
  DALI_TEST_EQUALS( cullFaceTrace.TestMethodAndParams(2, "Enable", out.str()), true, TEST_LOCATION);

  out.str("");
  out << GL_FRONT;
  DALI_TEST_EQUALS( cullFaceTrace.TestMethodAndParams(3, "CullFace", out.str()), true, TEST_LOCATION);

  //Verify state through the actor api
  DALI_TEST_CHECK( CullFrontAndBack == actor.GetCullFace() );
  DALI_TEST_CHECK( CullFront == actor2.GetCullFace() );

  /**************************************************************/
  //Verify whether the correct GL calls are made when face culling is disabled for both actors
  cullFaceTrace.Reset();
  cullFaceTrace.Enable(true);
  actor.SetCullFace( CullNone );
  actor2.SetCullFace( CullNone );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  cullFaceTrace.Enable(false);

  out.str("");
  out << GL_CULL_FACE;
  DALI_TEST_EQUALS( cullFaceTrace.TestMethodAndParams(0, "Disable", out.str()), true, TEST_LOCATION);

  //Verify state through the actor api
  DALI_TEST_CHECK( CullNone == actor.GetCullFace() );
  DALI_TEST_CHECK( CullNone == actor2.GetCullFace() );

  Stage::GetCurrent().Remove(actor);
  Stage::GetCurrent().Remove(actor2);
  END_TEST;
}

int UtcDaliRenderableActorGetCullFace(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::GetCullFace()");

  ImageActor imageActor = ImageActor::New();

  DALI_TEST_CHECK( CullNone == imageActor.GetCullFace() );

  imageActor.SetCullFace( CullBack );

  DALI_TEST_CHECK( CullBack == imageActor.GetCullFace() );

  END_TEST;
}

int UtcDaliRenderableActorSetGetBlendFunc(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  tet_infoline("Testing Dali::RenderableActor::UtcDaliRenderableActorSetGetBlendFunc()");

  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );
  Stage::GetCurrent().Add( actor );
  application.SendNotification();
  application.Render();

  // Test the defaults as documented int blending.h
  {
    BlendingFactor::Type srcFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type srcFactorAlpha( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorAlpha( BlendingFactor::ZERO );
    actor.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
    DALI_TEST_EQUALS( BlendingFactor::SRC_ALPHA,           srcFactorRgb,    TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_SRC_ALPHA, destFactorRgb,   TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE,                 srcFactorAlpha,  TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_SRC_ALPHA, destFactorAlpha, TEST_LOCATION );
  }

  // Set to non-default values
  actor.SetBlendFunc( BlendingFactor::ONE_MINUS_SRC_COLOR, BlendingFactor::SRC_ALPHA_SATURATE, BlendingFactor::ONE_MINUS_SRC_COLOR, BlendingFactor::SRC_ALPHA_SATURATE );

  // Test that Set was successful
  {
    BlendingFactor::Type srcFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type srcFactorAlpha( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorAlpha( BlendingFactor::ZERO );
    actor.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_SRC_COLOR, srcFactorRgb,    TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::SRC_ALPHA_SATURATE,  destFactorRgb,   TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_SRC_COLOR, srcFactorAlpha,  TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::SRC_ALPHA_SATURATE,  destFactorAlpha, TEST_LOCATION );
  }

  // Render & check GL commands
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_COLOR, glAbstraction.GetLastBlendFuncSrcRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA_SATURATE,  glAbstraction.GetLastBlendFuncDstRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_COLOR, glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA_SATURATE,  glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION );

  // Set using separate alpha settings
  actor.SetBlendFunc( BlendingFactor::CONSTANT_COLOR, BlendingFactor::ONE_MINUS_CONSTANT_COLOR,
                      BlendingFactor::CONSTANT_ALPHA, BlendingFactor::ONE_MINUS_CONSTANT_ALPHA );

  // Test that Set was successful
  {
    BlendingFactor::Type srcFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type srcFactorAlpha( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorAlpha( BlendingFactor::ZERO );
    actor.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
    DALI_TEST_EQUALS( BlendingFactor::CONSTANT_COLOR,            srcFactorRgb,    TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_CONSTANT_COLOR,  destFactorRgb,   TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::CONSTANT_ALPHA,            srcFactorAlpha,  TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_CONSTANT_ALPHA,  destFactorAlpha, TEST_LOCATION );
  }

  // Render & check GL commands
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( (GLenum)GL_CONSTANT_COLOR,           glAbstraction.GetLastBlendFuncSrcRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_CONSTANT_COLOR, glAbstraction.GetLastBlendFuncDstRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_CONSTANT_ALPHA,           glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_CONSTANT_ALPHA, glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliRenderableActorSetGetAlpha(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetGetAlpha()");

  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );
  Stage::GetCurrent().Add( actor );
  application.SendNotification();
  application.Render();

  // use the image alpha on actor
  actor.SetBlendMode(BlendingMode::ON);

  // Test that Set was successful
  DALI_TEST_EQUALS( BlendingMode::ON, actor.GetBlendMode(), TEST_LOCATION );

  // Now test that it can be set to false
  actor.SetBlendMode(BlendingMode::OFF);
  DALI_TEST_EQUALS(BlendingMode::OFF, actor.GetBlendMode(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliRenderableActorCreateDestroy(void)
{
  tet_infoline("Testing Dali::RenderableActor::CreateDestroy()");
  RenderableActor* ractor = new RenderableActor;
  RenderableActor ractor2( *ractor );
  DALI_TEST_CHECK( ractor );
  delete ractor;
  END_TEST;
}

int UtcDaliRenderableActorSetGetFilterModes(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetFilterMode() / Dali::RenderableActor::GetFilterMode()");

  ImageActor actor = ImageActor::New();

  FilterMode::Type minifyFilter = FilterMode::NEAREST;
  FilterMode::Type magnifyFilter = FilterMode::NEAREST;

  // Default test
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::DEFAULT == minifyFilter );
  DALI_TEST_CHECK( FilterMode::DEFAULT == magnifyFilter );

  // Default/Default
  actor.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::DEFAULT == minifyFilter );
  DALI_TEST_CHECK( FilterMode::DEFAULT == magnifyFilter );

  // Nearest/Nearest
  actor.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::NEAREST == minifyFilter );
  DALI_TEST_CHECK( FilterMode::NEAREST == magnifyFilter );

  // Linear/Linear
  actor.SetFilterMode( FilterMode::LINEAR, FilterMode::LINEAR );
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::LINEAR == minifyFilter );
  DALI_TEST_CHECK( FilterMode::LINEAR == magnifyFilter );

  // Nearest/Linear
  actor.SetFilterMode( FilterMode::NEAREST, FilterMode::LINEAR );
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::NEAREST == minifyFilter );
  DALI_TEST_CHECK( FilterMode::LINEAR == magnifyFilter );

  // Linear/Nearest
  actor.SetFilterMode( FilterMode::LINEAR, FilterMode::NEAREST );
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::LINEAR == minifyFilter );
  DALI_TEST_CHECK( FilterMode::NEAREST == magnifyFilter );

  END_TEST;
}

int UtcDaliRenderableActorSetFilterMode(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetFilterMode()");

  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );

  actor.SetSize(100.0f, 100.0f);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetAnchorPoint(AnchorPoint::CENTER);

  Stage::GetCurrent().Add(actor);

  /**************************************************************/

  // Default/Default
  TraceCallStack& texParameterTrace = application.GetGlAbstraction().GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  std::stringstream out;

  // Verify actor gl state

  // There are two calls to TexParameteri when the texture is first created
  // Texture mag filter is not called as the first time set it uses the system default
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 3, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(2, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/

  // Default/Default
  texParameterTrace = application.GetGlAbstraction().GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state

  // Should not make any calls when settings are the same
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 0, TEST_LOCATION);

  /**************************************************************/

  // Nearest/Nearest
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 2, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MAG_FILTER << ", " << GL_NEAREST;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(1, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/

  // Linear/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::LINEAR, FilterMode::LINEAR );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 2, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MAG_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(1, "TexParameteri", out.str()), true, TEST_LOCATION);


  /**************************************************************/

  // Nearest/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::NEAREST, FilterMode::LINEAR );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 1, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/

  // Default/Default
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 1, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/

  // None/None
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::NONE, FilterMode::NONE );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 1, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST_MIPMAP_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/

  Stage::GetCurrent().Remove(actor);

  END_TEST;
}

int UtcDaliRenderableActorSetShaderEffect(void)
{
  TestApplication application;
  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );
  Stage::GetCurrent().Add( actor );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  GLuint lastShaderCompiledBefore = application.GetGlAbstraction().GetLastShaderCompiled();

  application.GetGlAbstraction().EnableShaderCallTrace( true );

  const std::string vertexShader = "UtcDaliRenderableActorSetShaderEffect-VertexSource";
  const std::string fragmentShader = "UtcDaliRenderableActorSetShaderEffect-FragmentSource";
  ShaderEffect effect = ShaderEffect::New(vertexShader, fragmentShader );
  DALI_TEST_CHECK( effect != actor.GetShaderEffect() );

  actor.SetShaderEffect( effect );
  DALI_TEST_CHECK( effect == actor.GetShaderEffect() );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  GLuint lastShaderCompiledAfter = application.GetGlAbstraction().GetLastShaderCompiled();
  DALI_TEST_EQUALS( lastShaderCompiledAfter, lastShaderCompiledBefore + 2, TEST_LOCATION );

  std::string actualVertexShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 1 );
  DALI_TEST_EQUALS( vertexShader,
                    actualVertexShader.substr( actualVertexShader.length() - vertexShader.length() ), TEST_LOCATION );
  std::string actualFragmentShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 2 );
  DALI_TEST_EQUALS( fragmentShader,
                    actualFragmentShader.substr( actualFragmentShader.length() - fragmentShader.length() ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliRenderableActorGetShaderEffect(void)
{
  TestApplication application;
  ImageActor actor = ImageActor::New();

  ShaderEffect effect = ShaderEffect::New("UtcDaliRenderableActorGetShaderEffect-VertexSource", "UtcDaliRenderableActorGetShaderEffect-FragmentSource" );
  actor.SetShaderEffect(effect);

  DALI_TEST_CHECK(effect == actor.GetShaderEffect());
  END_TEST;
}

int UtcDaliRenderableActorRemoveShaderEffect01(void)
{
  TestApplication application;
  ImageActor actor = ImageActor::New();

  ShaderEffect defaultEffect = actor.GetShaderEffect();

  ShaderEffect effect = ShaderEffect::New("UtcDaliRenderableActorRemoveShaderEffect-VertexSource", "UtcDaliRenderableActorRemoveShaderEffect-FragmentSource" );
  actor.SetShaderEffect(effect);

  DALI_TEST_CHECK(effect == actor.GetShaderEffect());

  actor.RemoveShaderEffect();

  DALI_TEST_CHECK(defaultEffect == actor.GetShaderEffect());
  END_TEST;
}

int UtcDaliRenderableActorRemoveShaderEffect02(void)
{
  TestApplication application;
  ImageActor actor = ImageActor::New();

  ShaderEffect defaultEffect = actor.GetShaderEffect();

  actor.RemoveShaderEffect();

  DALI_TEST_CHECK(defaultEffect == actor.GetShaderEffect());
  END_TEST;
}

int UtcDaliSetShaderEffectRecursively(void)
{
  TestApplication application;
  /**
   * create a tree
   *                 actor1
   *           actor2       actor4
   *       actor3 imageactor1
   * imageactor2
   */
  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor1 = ImageActor::New( img );
  Actor actor2 = Actor::New();
  actor1.Add( actor2 );
  Actor actor3 = Actor::New();
  actor2.Add( actor3 );
  ImageActor imageactor1 = ImageActor::New( img );
  actor2.Add( imageactor1 );
  ImageActor imageactor2 = ImageActor::New( img );
  actor3.Add( imageactor2 );
  Actor actor4 = Actor::New();
  actor1.Add( actor4 );
  Stage::GetCurrent().Add( actor1 );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  GLuint lastShaderCompiledBefore = application.GetGlAbstraction().GetLastShaderCompiled();

  application.GetGlAbstraction().EnableShaderCallTrace( true );

  const std::string vertexShader = "UtcDaliRenderableActorSetShaderEffect-VertexSource";
  const std::string fragmentShader = "UtcDaliRenderableActorSetShaderEffect-FragmentSource";
  // test with empty effect
  ShaderEffect effect;
  SetShaderEffectRecursively( actor1, effect );

  effect = ShaderEffect::New(vertexShader, fragmentShader );

  DALI_TEST_CHECK( effect != actor1.GetShaderEffect() );
  DALI_TEST_CHECK( effect != imageactor1.GetShaderEffect() );
  DALI_TEST_CHECK( effect != imageactor2.GetShaderEffect() );

  SetShaderEffectRecursively( actor1, effect );
  DALI_TEST_CHECK( effect == imageactor1.GetShaderEffect() );
  DALI_TEST_CHECK( effect == imageactor2.GetShaderEffect() );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  GLuint lastShaderCompiledAfter = application.GetGlAbstraction().GetLastShaderCompiled();
  DALI_TEST_EQUALS( lastShaderCompiledAfter, lastShaderCompiledBefore + 2, TEST_LOCATION );

  std::string actualVertexShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 1 );
  DALI_TEST_EQUALS( vertexShader,
                    actualVertexShader.substr( actualVertexShader.length() - vertexShader.length() ), TEST_LOCATION );
  std::string actualFragmentShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 2 );
  DALI_TEST_EQUALS( fragmentShader,
                    actualFragmentShader.substr( actualFragmentShader.length() - fragmentShader.length() ), TEST_LOCATION );

  // remove from one that does not have shader
  RemoveShaderEffectRecursively( actor4 );

  // remove partially
  RemoveShaderEffectRecursively( actor3 );
  DALI_TEST_CHECK( effect == imageactor1.GetShaderEffect() );
  DALI_TEST_CHECK( effect != imageactor2.GetShaderEffect() );

  // test with empty actor just to check it does not crash
  Actor empty;
  SetShaderEffectRecursively( empty, effect );
  RemoveShaderEffectRecursively( empty );

  // test with actor with no children just to check it does not crash
  Actor loner = Actor::New();
  Stage::GetCurrent().Add( loner );
  SetShaderEffectRecursively( loner, effect );
  RemoveShaderEffectRecursively( loner );

  END_TEST;
}

int UtcDaliRenderableActorTestClearCache01(void)
{
  // Testing the framebuffer state caching in frame-buffer-state-caching.cpp
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::ClearCache01()");

  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );

  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetAnchorPoint(AnchorPoint::CENTER);

  Stage::GetCurrent().Add(actor);

  /**************************************************************/
  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  // There should be a single call to Clear
  DALI_TEST_EQUALS( application.GetGlAbstraction().GetClearCountCalled() , 1u, TEST_LOCATION );

  // the last set clear mask should be COLOR, DEPTH & STENCIL which occurs at the start of each frame
  GLbitfield mask = application.GetGlAbstraction().GetLastClearMask();
  DALI_TEST_CHECK( mask & GL_DEPTH_BUFFER_BIT );
  DALI_TEST_CHECK( mask & GL_STENCIL_BUFFER_BIT );
  DALI_TEST_CHECK( mask & GL_COLOR_BUFFER_BIT );

  END_TEST;
}

int UtcDaliRenderableActorTestClearCache02(void)
{
  // Testing the framebuffer state caching in frame-buffer-state-caching.cpp
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::ClearCache02()");

  // use RGB so alpha is disabled and the actors are drawn opaque
  BufferImage img = BufferImage::New( 10,10 ,Pixel::RGB888 );

  // Without caching DALi perform clears in the following places
  // Root
  // | glClear #1 ( everything at start of frame )
  // |
  // |
  // | glClear #2 ( start of layer with opaque actors )
  // | ----> Layer1
  // |     -> Actor 1 ( opaque )
  // |     -> Actor 2 ( opaque )
  // |
  // |
  // | glClear  #3 ( start of layer with opaque actors )
  // |----> Layer 2
  // |     -> Actor 3 ( opaque )
  // |     -> Actor 4 ( opaque )
  //
  // With caching enabled glClear should only be called twice, at points #1 and #3.
  // At #1 with depth, color and stencil cleared
  // At #3 with depth cleared
  // #2 is not required because the buffer has already been cleared at #1

  Layer layer1 = Layer::New();
  layer1.Add( ImageActor::New( img ) );
  layer1.Add( ImageActor::New( img ) );

  Layer layer2 = Layer::New();
  layer2.Add( ImageActor::New( img ) );
  layer2.Add( ImageActor::New( img ) );

  Stage::GetCurrent().Add( layer1 );
  Stage::GetCurrent().Add( layer2 );

  /**************************************************************/

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  // There should be a 2 calls to Clear
  DALI_TEST_EQUALS( application.GetGlAbstraction().GetClearCountCalled() , 2u, TEST_LOCATION );

  // the last set clear mask should be  DEPTH & STENCIL & COLOR
  GLbitfield mask = application.GetGlAbstraction().GetLastClearMask();

  tet_printf(" clear count = %d \n",  application.GetGlAbstraction().GetClearCountCalled() );

  // The last clear should just be DEPTH BUFFER, not color and stencil which were cleared at the start of the frame
  DALI_TEST_CHECK( mask & GL_DEPTH_BUFFER_BIT );
  DALI_TEST_CHECK( ! ( mask & GL_COLOR_BUFFER_BIT ) );
  DALI_TEST_CHECK( ! ( mask & GL_STENCIL_BUFFER_BIT ) );

  END_TEST;
}

int UtcDaliRenderableActorTestClearCache03(void)
{
  // Testing the framebuffer state caching in frame-buffer-state-caching.cpp
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::ClearCache03()");

  // use RGB so alpha is disabled and the actors are drawn opaque
  BufferImage img = BufferImage::New( 10,10 ,Pixel::RGB888 );

  // Without caching DALi perform clears in the following places
  // Root
  // | 1-## glClear ( COLOR, DEPTH, STENCIL )
  // |
  // | ----> Layer1
  // |     2-##  glClear  ( STENCIL )
  // |     -> Actor 1 ( stencil )
  // |     3-##  glClear  ( DEPTH )
  // |     -> Actor 2 ( opaque )  // need 2 opaque actors to bypass optimisation of turning off depth test
  // |     -> Actor 3 ( opaque )
  // |
  // |
  // |----> Layer 2
  // |     4-##  glClear  ( STENCIL )
  // |     -> Actor 4 ( stencil )
  // |     5-##  glClear  ( DEPTH )
  // |     -> Actor 5 ( opaque )  // need 2 opaque actors to bypass optimisation of turning off depth test
  // |     -> Actor 6 ( opaque )
  //
  // With caching enabled glClear will not be called at ## 2 and ## 3 ( because those buffers are already clear).
  //
  // @TODO Add further optimisation to look-ahead in the render-list to see if
  // When performing STENCIL clear, check if there another layer after it.
  // If there is, combine the STENCIL with a DEPTH clear.
  //

  Layer layer1 = Layer::New();
  ImageActor actor1 =  ImageActor::New( img );
  ImageActor actor2 =  ImageActor::New( img );
  ImageActor actor3 =  ImageActor::New( img );

  actor2.SetDrawMode( DrawMode::STENCIL );

  layer1.Add( actor1 );
  layer1.Add( actor2 );
  layer1.Add( actor3 );

  Layer layer2 = Layer::New();
  ImageActor actor4 =  ImageActor::New( img );
  ImageActor actor5 =  ImageActor::New( img );
  ImageActor actor6 =  ImageActor::New( img );

  actor4.SetDrawMode( DrawMode::STENCIL );

  layer2.Add( actor4 );
  layer2.Add( actor5 );
  layer2.Add( actor6 );

  Stage::GetCurrent().Add( layer1 );
  Stage::GetCurrent().Add( layer2 );


  /**************************************************************/

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  // There should be a 3 calls to Clear ( one for everything, one for stencil, one for depth buffer).
  DALI_TEST_EQUALS( application.GetGlAbstraction().GetClearCountCalled() , 3u, TEST_LOCATION );

  // the last set clear mask should be  DEPTH & STENCIL & COLOR
  GLbitfield mask = application.GetGlAbstraction().GetLastClearMask();

  tet_printf(" clear count = %d \n",  application.GetGlAbstraction().GetClearCountCalled() );
  tet_printf(" clear mask  = %x \n",  mask);

  // The last clear should just be DEPTH BUFFER and stencil
  DALI_TEST_CHECK(  !( mask & GL_COLOR_BUFFER_BIT ) );
  DALI_TEST_CHECK(  !( mask & GL_STENCIL_BUFFER_BIT ) );
  DALI_TEST_CHECK(  mask & GL_DEPTH_BUFFER_BIT );


  END_TEST;
}
