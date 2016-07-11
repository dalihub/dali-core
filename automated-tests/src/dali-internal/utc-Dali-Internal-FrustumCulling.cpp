/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali/devel-api/object/property-buffer.h>
#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/rendering/texture-set.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/rendering/sampler.h>
#include <dali/devel-api/rendering/shader.h>
#include <dali/devel-api/images/texture-set-image.h>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>

using namespace Dali;

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec2    aPosition;
attribute mediump vec2    aTexCoord;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
varying   mediump vec2    vTexCoord;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  vTexCoord = aTexCoord;
  gl_Position = vertexPosition;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
uniform Sampler2D sTexture;
varying mediump vec2 vTexCoord;
void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord );
}
);

Actor CreateMeshActorToStage( TestApplication& application, Vector3 parentOrigin = ParentOrigin::CENTER, Vector3 anchorPoint = AnchorPoint::CENTER, Shader::ShaderHints shaderHints = Shader::HINT_NONE )
{
  PixelBuffer* pixelBuffer = new PixelBuffer[ 4 ];
  BufferImage image = BufferImage::New( pixelBuffer, 1, 1 );

  Geometry geometry = CreateQuadGeometry();
  Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER, shaderHints );
  TextureSet textureSet = TextureSet::New();
  TextureSetImage( textureSet, 0u, image );
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  meshActor.SetSize( Vector3( 400.0f, 400.0f, 0.1f ) );
  meshActor.SetParentOrigin( parentOrigin );
  meshActor.SetAnchorPoint( anchorPoint );
  Stage::GetCurrent().Add( meshActor );

  application.SendNotification();
  application.Render( 16 );

  return meshActor;
}

bool GetCameraDepths( TestApplication& application, float& nearPlane, float& farPlane, float& cameraDepth )
{
  RenderTaskList renderTasks = Stage::GetCurrent().GetRenderTaskList();
  CameraActor cameraActor;
  for( unsigned int i = 0; i < renderTasks.GetTaskCount(); ++i )
  {
    RenderTask task = renderTasks.GetTask( i );
    cameraActor = task.GetCameraActor();
    if( cameraActor )
    {
      break;
    }
  }
  if( cameraActor )
  {
    application.SendNotification();
    application.Render( 16 );

    nearPlane = cameraActor.GetNearClippingPlane();
    farPlane = cameraActor.GetFarClippingPlane();
    cameraDepth = cameraActor.GetCurrentPosition().z;
  }

  return !!cameraActor;
}

int UtcFrustumCullN(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  CreateMeshActorToStage( application );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumLeftCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float offset = -0.01f;
  Actor meshActor = CreateMeshActorToStage( application, Vector3( offset, 0.5f, 0.5f ), AnchorPoint::CENTER_RIGHT );

  float radius = meshActor.GetTargetSize().Length() * 0.5f;
  Vector2 stageSize = Stage::GetCurrent().GetSize();
  meshActor.SetParentOrigin( Vector3( -radius / stageSize.width + offset, 0.5f, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumLeftCullN(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float offset = 0.01f;
  Actor meshActor = CreateMeshActorToStage( application, Vector3( offset, 0.5f, 0.5f ), AnchorPoint::CENTER_RIGHT );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumRightCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float offset = 1.01f;
  Actor meshActor = CreateMeshActorToStage( application, Vector3( offset, 0.5f, 0.5f ), AnchorPoint::CENTER_LEFT );

  float radius = meshActor.GetTargetSize().Length() * 0.5f;
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  meshActor.SetParentOrigin( Vector3( radius / stageSize.width + offset, 0.5f, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumRightCullN(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float offset = 0.99f;
  Actor meshActor = CreateMeshActorToStage( application, Vector3( offset, 0.5f, 0.5f ), AnchorPoint::CENTER_LEFT );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumTopCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float offset = -0.01f;
  Actor meshActor = CreateMeshActorToStage( application, Vector3( 0.5f, offset, 0.5f ), AnchorPoint::BOTTOM_CENTER );

  float radius = meshActor.GetTargetSize().Length() * 0.5f;
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  meshActor.SetParentOrigin( Vector3( 0.5f, -radius / stageSize.width + offset, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumTopCullN(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float offset = 0.01f;
  Actor meshActor = CreateMeshActorToStage( application, Vector3( 0.5f, offset, 0.5f ), AnchorPoint::BOTTOM_CENTER );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  // This will be box culled
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumBottomCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float offset = 1.01f;
  Actor meshActor = CreateMeshActorToStage( application, Vector3( 0.5f, offset, 0.5f ), AnchorPoint::TOP_CENTER );

  float radius = meshActor.GetTargetSize().Length() * 0.5f;
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  meshActor.SetParentOrigin( Vector3( 0.5f, radius / stageSize.width + offset, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumBottomCullN(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float offset = 0.99f;
  Actor meshActor = CreateMeshActorToStage( application, Vector3( 0.5f, offset, 0.5f ), AnchorPoint::TOP_CENTER );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumNearCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float nearPlane, farPlane, cameraDepth;
  DALI_TEST_CHECK( GetCameraDepths( application, nearPlane, farPlane, cameraDepth ) );

  Actor meshActor = CreateMeshActorToStage( application );
  Vector3 meshPosition = meshActor.GetCurrentPosition();

  float radius = meshActor.GetTargetSize().Length() * 0.5f;
  float offset = radius + 0.1f;
  meshPosition.z = cameraDepth - nearPlane + offset;
  meshActor.SetPosition( meshPosition );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumNearCullN(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float nearPlane, farPlane, cameraDepth;
  DALI_TEST_CHECK( GetCameraDepths( application, nearPlane, farPlane, cameraDepth ) );

  Actor meshActor = CreateMeshActorToStage( application );
  Vector3 meshPosition = meshActor.GetCurrentPosition();

  float offset = meshActor.GetTargetSize().z - 0.1f;
  meshPosition.z = cameraDepth - nearPlane + offset;
  meshActor.SetPosition( meshPosition );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumFarCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float nearPlane, farPlane, cameraDepth;
  DALI_TEST_CHECK( GetCameraDepths( application, nearPlane, farPlane, cameraDepth ) );

  Actor meshActor = CreateMeshActorToStage( application );
  Vector3 meshPosition = meshActor.GetCurrentPosition();

  float radius = meshActor.GetTargetSize().Length() * 0.5f;
  float offset = radius + 0.1f;
  meshPosition.z = cameraDepth - farPlane - offset;
  meshActor.SetPosition( meshPosition );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumFarCullN(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  float nearPlane, farPlane, cameraDepth;
  DALI_TEST_CHECK( GetCameraDepths( application, nearPlane, farPlane, cameraDepth ) );

  Actor meshActor = CreateMeshActorToStage( application );
  Vector3 meshPosition = meshActor.GetCurrentPosition();

  float offset = meshActor.GetTargetSize().z - 0.1f;
  meshPosition.z = cameraDepth - farPlane - offset;
  meshActor.SetPosition( meshPosition );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumCullDisabledP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable( true );

  CreateMeshActorToStage( application, Vector3( 7.0f, 0.5f, 0.5f ), AnchorPoint::CENTER, Shader::HINT_MODIFIES_GEOMETRY );

  drawTrace.Reset();
  application.SendNotification();
  application.Render( 16 );

  // This should not be culled
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}
