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
#include <dali/devel-api/object/property-buffer.h>
#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/rendering/material.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/rendering/sampler.h>
#include <dali/devel-api/rendering/shader.h>

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

Geometry CreateGeometry()
{
  const float halfQuadSize = .5f;
  struct TexturedQuadVertex { Vector2 position; Vector2 textureCoordinates; };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    { Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f) },
    { Vector2( halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f) },
    { Vector2(-halfQuadSize,  halfQuadSize), Vector2(0.f, 1.f) },
    { Vector2( halfQuadSize,  halfQuadSize), Vector2(1.f, 1.f) } };

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aTexCoord"] = Property::VECTOR2;
  PropertyBuffer texturedQuadVertices = PropertyBuffer::New( texturedQuadVertexFormat, 4 );
  texturedQuadVertices.SetData(texturedQuadVertexData);

  // Create indices
  unsigned int indexData[6] = { 0, 3, 1, 0, 2, 3 };
  Property::Map indexFormat;
  indexFormat["indices"] = Property::INTEGER;
  PropertyBuffer indices = PropertyBuffer::New( indexFormat, sizeof(indexData)/sizeof(indexData[0]) );
  indices.SetData(indexData);

  // Create the geometry object
  Geometry texturedQuadGeometry = Geometry::New();
  texturedQuadGeometry.AddVertexBuffer( texturedQuadVertices );
  texturedQuadGeometry.SetIndexBuffer( indices );

  return texturedQuadGeometry;
}

int UtcFrustumCullN(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  PixelBuffer* pixelBuffer = new PixelBuffer[ 4 ];
  BufferImage image = BufferImage::New( pixelBuffer, 1, 1 );

  Geometry geometry = CreateGeometry();
  Material material = Material::New( Shader::New( VERTEX_SHADER, FRAGMENT_SHADER ) );
  Sampler sampler = Sampler::New( image, "sTexture" );
  material.AddSampler( sampler );
  Renderer renderer = Renderer::New( geometry, material );

  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  meshActor.SetSize( Vector3( 400.0f, 400.0f, 0.1f) );
  drawTrace.Reset();

  meshActor.SetParentOrigin( ParentOrigin::CENTER );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );
  Stage::GetCurrent().Add( meshActor );

  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );
  END_TEST;
}

int UtcFrustumLeftCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  PixelBuffer* pixelBuffer = new PixelBuffer[ 4 ];
  BufferImage image = BufferImage::New( pixelBuffer, 1, 1 );

  Geometry geometry = CreateGeometry();
  Material material = Material::New( Shader::New( VERTEX_SHADER, FRAGMENT_SHADER ) );
  Sampler sampler = Sampler::New( image, "sTexture" );
  material.AddSampler( sampler );
  Renderer renderer = Renderer::New( geometry, material );

  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  meshActor.SetSize( Vector3( 400.0f, 400.0f, 0.1f) );
  drawTrace.Reset();

  float offset = -0.01f;

  meshActor.SetParentOrigin( Vector3( offset, 0.5f, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER_RIGHT );
  Stage::GetCurrent().Add( meshActor );

  application.SendNotification();
  application.Render(16);

  // This will be box culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  float radius = meshActor.GetTargetSize().Length() * 0.5f;
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  drawTrace.Reset();
  meshActor.SetParentOrigin( Vector3( -radius/stageSize.width + offset, 0.5f, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );
  application.SendNotification();
  application.Render(16);

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumRightCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  PixelBuffer* pixelBuffer = new PixelBuffer[ 4 ];
  BufferImage image = BufferImage::New( pixelBuffer, 1, 1 );

  Geometry geometry = CreateGeometry();
  Material material = Material::New( Shader::New( VERTEX_SHADER, FRAGMENT_SHADER ) );
  Sampler sampler = Sampler::New( image, "sTexture" );
  material.AddSampler( sampler );
  Renderer renderer = Renderer::New( geometry, material );

  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  meshActor.SetSize( Vector3( 400.0f, 400.0f, 0.1f) );
  drawTrace.Reset();

  float offset = 1.01f;

  meshActor.SetParentOrigin( Vector3( offset, 0.5f, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  Stage::GetCurrent().Add( meshActor );

  application.SendNotification();
  application.Render(16);

  // This will be box culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  float radius = meshActor.GetTargetSize().Length() * 0.5f;
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  drawTrace.Reset();
  meshActor.SetParentOrigin( Vector3( radius/stageSize.width + offset, 0.5f, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );
  application.SendNotification();
  application.Render(16);

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumTopCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  PixelBuffer* pixelBuffer = new PixelBuffer[ 4 ];
  BufferImage image = BufferImage::New( pixelBuffer, 1, 1 );

  Geometry geometry = CreateGeometry();
  Material material = Material::New( Shader::New( VERTEX_SHADER, FRAGMENT_SHADER ) );
  Sampler sampler = Sampler::New( image, "sTexture" );
  material.AddSampler( sampler );
  Renderer renderer = Renderer::New( geometry, material );

  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  meshActor.SetSize( Vector3( 400.0f, 400.0f, 0.1f) );
  drawTrace.Reset();

  float offset = -0.01f;

  meshActor.SetParentOrigin( Vector3( 0.5f, offset, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  Stage::GetCurrent().Add( meshActor );

  application.SendNotification();
  application.Render(16);

  // This will be box culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  float radius = meshActor.GetTargetSize().Length() * 0.5f;
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  drawTrace.Reset();
  meshActor.SetParentOrigin( Vector3( 0.5f, -radius/stageSize.width + offset, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );
  application.SendNotification();
  application.Render(16);

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumBottomCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  PixelBuffer* pixelBuffer = new PixelBuffer[ 4 ];
  BufferImage image = BufferImage::New( pixelBuffer, 1, 1 );

  Geometry geometry = CreateGeometry();
  Material material = Material::New( Shader::New( VERTEX_SHADER, FRAGMENT_SHADER ) );
  Sampler sampler = Sampler::New( image, "sTexture" );
  material.AddSampler( sampler );
  Renderer renderer = Renderer::New( geometry, material );

  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  meshActor.SetSize( Vector3( 400.0f, 400.0f, 0.1f) );
  drawTrace.Reset();

  float offset = 1.01f;

  meshActor.SetParentOrigin( Vector3( 0.5f, offset, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  Stage::GetCurrent().Add( meshActor );

  application.SendNotification();
  application.Render(16);

  // This will be box culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  float radius = meshActor.GetTargetSize().Length() * 0.5f;
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  drawTrace.Reset();
  meshActor.SetParentOrigin( Vector3( 0.5f, radius/stageSize.width + offset, 0.5f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );
  application.SendNotification();
  application.Render(16);

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );

  END_TEST;
}

int UtcFrustumNearCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  PixelBuffer* pixelBuffer = new PixelBuffer[ 4 ];
  BufferImage image = BufferImage::New( pixelBuffer, 1, 1 );

  Geometry geometry = CreateGeometry();
  Material material = Material::New( Shader::New( VERTEX_SHADER, FRAGMENT_SHADER ) );
  Sampler sampler = Sampler::New( image, "sTexture" );
  material.AddSampler( sampler );
  Renderer renderer = Renderer::New( geometry, material );

  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  meshActor.SetSize( Vector3( 400.0f, 400.0f, 0.1f) );
  drawTrace.Reset();

  meshActor.SetParentOrigin( Vector3( 0.5f, 0.5f, 7.0f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );
  Stage::GetCurrent().Add( meshActor );

  application.SendNotification();
  application.Render(16);

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );
  END_TEST;
}

int UtcFrustumFarCullP(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  PixelBuffer* pixelBuffer = new PixelBuffer[ 4 ];
  BufferImage image = BufferImage::New( pixelBuffer, 1, 1 );

  Geometry geometry = CreateGeometry();
  Material material = Material::New( Shader::New( VERTEX_SHADER, FRAGMENT_SHADER ) );
  Sampler sampler = Sampler::New( image, "sTexture" );
  material.AddSampler( sampler );
  Renderer renderer = Renderer::New( geometry, material );

  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  meshActor.SetSize( Vector3( 400.0f, 400.0f, 0.1f) );
  drawTrace.Reset();

  meshActor.SetParentOrigin( Vector3( 0.5f, 0.5f, -7.0f ) );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );
  Stage::GetCurrent().Add( meshActor );

  application.SendNotification();
  application.Render(16);

  // This will be sphere culled
  DALI_TEST_CHECK( !drawTrace.FindMethod( "DrawElements" ) );
  END_TEST;
}

int UtcFrustumCullDisabledP(void)
{
    TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  PixelBuffer* pixelBuffer = new PixelBuffer[ 4 ];
  BufferImage image = BufferImage::New( pixelBuffer, 1, 1 );

  Geometry geometry = CreateGeometry();
  Material material = Material::New( Shader::New( VERTEX_SHADER, FRAGMENT_SHADER, Shader::HINT_MODIFIES_GEOMETRY ) );
  Sampler sampler = Sampler::New( image, "sTexture" );
  material.AddSampler( sampler );
  Renderer renderer = Renderer::New( geometry, material );

  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  meshActor.SetSize( Vector3( 400.0f, 400.0f, 0.1f) );
  drawTrace.Reset();

  meshActor.SetParentOrigin( ParentOrigin::CENTER );
  meshActor.SetAnchorPoint( AnchorPoint::CENTER );
  Stage::GetCurrent().Add( meshActor );

  application.SendNotification();
  application.Render(16);

  // This should not be culled
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );
  END_TEST;
}
