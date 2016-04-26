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

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/rendering/renderer.h>

using namespace Dali;

#include <mesh-builder.h>

void geometry_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void geometry_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

void TestConstraintNoBlue( Vector4& current, const PropertyInputContainer& inputs )
{
  current.b = 0.0f;
}

struct TexturedQuadVertex { Vector2 position; Vector2 textureCoordinates; };

PropertyBuffer CreateVertexBuffer( const std::string& aPosition, const std::string& aTexCoord )
{
  const float halfQuadSize = .5f;
  TexturedQuadVertex texturedQuadVertexData[4] = {
    { Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f) },
    { Vector2( halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f) },
    { Vector2(-halfQuadSize,  halfQuadSize), Vector2(0.f, 1.f) },
    { Vector2( halfQuadSize,  halfQuadSize), Vector2(1.f, 1.f) } };

  Property::Map vertexFormat;
  vertexFormat[aPosition] = Property::VECTOR2;
  vertexFormat[aTexCoord] = Property::VECTOR2;

  PropertyBuffer vertexData = PropertyBuffer::New( vertexFormat );
  vertexData.SetData( texturedQuadVertexData, 4 );

  return vertexData;
}


}


int UtcDaliGeometryNew01(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  DALI_TEST_EQUALS( (bool)geometry, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryNew02(void)
{
  TestApplication application;
  Geometry geometry;
  DALI_TEST_EQUALS( (bool)geometry, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryCopyConstructor(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  Geometry geometryCopy(geometry);

  DALI_TEST_EQUALS( (bool)geometryCopy, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryAssignmentOperator(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  Geometry geometry2;
  DALI_TEST_EQUALS( (bool)geometry2, false, TEST_LOCATION );

  geometry2 = geometry;
  DALI_TEST_EQUALS( (bool)geometry2, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliGeometryDownCast01(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  BaseHandle handle(geometry);
  Geometry geometry2 = Geometry::DownCast(handle);
  DALI_TEST_EQUALS( (bool)geometry2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  Geometry geometry = Geometry::DownCast(handle);
  DALI_TEST_EQUALS( (bool)geometry, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryAddVertexBuffer(void)
{
  TestApplication application;

  tet_infoline("Test AddVertexBuffer");

  PropertyBuffer vertexBuffer1 = CreateVertexBuffer("aPosition1", "aTexCoord1" );
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer1 );

  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor actor = Actor::New();
  actor.SetSize(Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
        application.GetGlAbstraction().GetBufferDataCalls();

    DALI_TEST_EQUALS( bufferDataCalls.size(), 1u, TEST_LOCATION );

    DALI_TEST_EQUALS( bufferDataCalls[0], 4*sizeof( TexturedQuadVertex ), TEST_LOCATION );
  }

  // add the second vertex buffer
  application.GetGlAbstraction().ResetBufferDataCalls();

  PropertyBuffer vertexBuffer2 = CreateVertexBuffer( "aPosition2", "aTexCoord2" );
  geometry.AddVertexBuffer( vertexBuffer2 );
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
        application.GetGlAbstraction().GetBufferDataCalls();

    //Check that only the new buffer gets uploaded
    DALI_TEST_EQUALS( bufferDataCalls.size(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( bufferDataCalls[0], 4*sizeof( TexturedQuadVertex ), TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliGeometryGetNumberOfVertexBuffers(void)
{
  TestApplication application;

  tet_infoline("Test GetNumberOfVertexBuffers");
  PropertyBuffer vertexBuffer1 = CreateVertexBuffer("aPosition1", "aTexCoord1" );
  PropertyBuffer vertexBuffer2 = CreateVertexBuffer("aPosition2", "aTexCoord2" );
  PropertyBuffer vertexBuffer3 = CreateVertexBuffer("aPosition3", "aTexCoord3" );

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer1 );
  DALI_TEST_EQUALS( geometry.GetNumberOfVertexBuffers(), 1u, TEST_LOCATION );

  geometry.AddVertexBuffer( vertexBuffer2 );
  geometry.AddVertexBuffer( vertexBuffer3 );
  DALI_TEST_EQUALS( geometry.GetNumberOfVertexBuffers(), 3u, TEST_LOCATION );

  geometry.RemoveVertexBuffer( 2u );
  DALI_TEST_EQUALS( geometry.GetNumberOfVertexBuffers(), 2u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliGeometryRemoveVertexBuffer(void)
{
  TestApplication application;

  tet_infoline("Test RemoveVertexBuffer");

  PropertyBuffer vertexBuffer1 = CreateVertexBuffer("aPosition1", "aTexCoord1" );
  PropertyBuffer vertexBuffer2 = CreateVertexBuffer("aPosition2", "aTexCoord2" );

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer1 );

  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor actor = Actor::New();
  actor.SetSize(Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( geometry.GetNumberOfVertexBuffers(), 1u, TEST_LOCATION );

  geometry.RemoveVertexBuffer( 0 );
  geometry.AddVertexBuffer( vertexBuffer2 );
  DALI_TEST_EQUALS( geometry.GetNumberOfVertexBuffers(), 1u, TEST_LOCATION );

  geometry.RemoveVertexBuffer( 0 );
  DALI_TEST_EQUALS( geometry.GetNumberOfVertexBuffers(), 0u, TEST_LOCATION );

  //Todo: test by checking the BufferDataCalls
  // make sure the vertex buffer in actually removed from gl

   END_TEST;
}

int UtcDaliGeometrySetIndexBuffer(void)
{
  TestApplication application;

  tet_infoline("Test SetIndexBuffer");

  PropertyBuffer vertexBuffer = CreateVertexBuffer("aPosition", "aTexCoord" );

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );

  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor actor = Actor::New();
  actor.SetSize(Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
        application.GetGlAbstraction().GetBufferDataCalls();

    DALI_TEST_EQUALS( bufferDataCalls.size(), 1u, TEST_LOCATION );

    DALI_TEST_EQUALS( bufferDataCalls[0], 4*sizeof( TexturedQuadVertex ), TEST_LOCATION );
  }

  // Set index buffer
  application.GetGlAbstraction().ResetBufferDataCalls();

  const unsigned short indexData[6] = { 0, 3, 1, 0, 2, 3 };
  geometry.SetIndexBuffer( indexData, sizeof(indexData)/sizeof(indexData[0]) );
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
        application.GetGlAbstraction().GetBufferDataCalls();

    //Only the index buffer should be uploaded
    DALI_TEST_EQUALS( bufferDataCalls.size(), 1u, TEST_LOCATION );

    // should be unsigned short instead of unsigned int
    DALI_TEST_EQUALS( bufferDataCalls[0], 6*sizeof( unsigned short ), TEST_LOCATION );
  }


  END_TEST;
}

int UtcDaliGeometrySetGetGeometryType01(void)
{
  TestApplication application;

  tet_infoline("Test SetGeometryType and GetGeometryType: without index buffer");

  unsigned int numVertex = 4u;
  PropertyBuffer vertexBuffer = CreateVertexBuffer("aPosition", "aTexCoord" );

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );

  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor actor = Actor::New();
  actor.SetSize(Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  Stage::GetCurrent().Add(actor);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  /****************************************************/
  // Default (TRIANGLES), no index buffer
  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable( false );

  // Test the default geometry type is GL_TRIANGLE
  // no index buffer, call glDrawArrays,
  DALI_TEST_EQUALS( drawTrace.CountMethod( "DrawArrays" ), 2, TEST_LOCATION);
  std::stringstream out;
  out << GL_TRIANGLES << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS( drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS( geometry.GetGeometryType(), Geometry::TRIANGLES, TEST_LOCATION);

  /*********************************************************/
  // LINES, no index buffer
  geometry.SetGeometryType( Geometry::LINES );

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable( false );

  // geometry type is set as GL_LINES
  // no index buffer, call glDrawArrays,
  DALI_TEST_EQUALS( drawTrace.CountMethod( "DrawArrays" ), 2, TEST_LOCATION);
  out.str("");
  out << GL_LINES << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS( drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS( geometry.GetGeometryType(), Geometry::LINES, TEST_LOCATION);

  /*****************************************************/
  //POINTS
  geometry.SetGeometryType( Geometry::POINTS );

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable( false );

  // geometry type is set as GL_POINTS
  // no index buffer, call glDrawArrays,
  DALI_TEST_EQUALS( drawTrace.CountMethod( "DrawArrays" ), 2, TEST_LOCATION);
  out.str("");
  out << GL_POINTS << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS( drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS( geometry.GetGeometryType(), Geometry::POINTS, TEST_LOCATION);

  /*****************************************************/
  //TRIANGLE_STRIP, no index buffer
  geometry.SetGeometryType( Geometry::TRIANGLE_STRIP );

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable( false );

  // geometry type is set as GL_POINTS
  // no index buffer, call glDrawArrays,
  DALI_TEST_EQUALS( drawTrace.CountMethod( "DrawArrays" ), 2, TEST_LOCATION);
  out.str("");
  out << GL_TRIANGLE_STRIP << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS( drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS( geometry.GetGeometryType(), Geometry::TRIANGLE_STRIP, TEST_LOCATION);

  /*****************************************************/
  //TRIANGLE_FAN, no index buffer
  geometry.SetGeometryType( Geometry::TRIANGLE_FAN );

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable( false );

  // geometry type is set as GL_POINTS
  // no index buffer, call glDrawArrays,
  DALI_TEST_EQUALS( drawTrace.CountMethod( "DrawArrays" ), 2, TEST_LOCATION);
  out.str("");
  out << GL_TRIANGLE_FAN << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS( drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS( geometry.GetGeometryType(), Geometry::TRIANGLE_FAN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGeometrySetGetGeometryType02(void)
{
  TestApplication application;

  tet_infoline("Test SetGeometryType and GetGeometryType: with index buffer");

  unsigned int numVertex = 4u;
  unsigned int numIndex = 6u; // 6 unsigned short
  PropertyBuffer vertexBuffer = CreateVertexBuffer("aPosition", "aTexCoord" );


  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  const unsigned short indexData[6] = { 0, 3, 1, 0, 2, 3 };
  geometry.SetIndexBuffer( indexData, sizeof(indexData)/sizeof(indexData[0]) );

  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor actor = Actor::New();
  actor.SetSize(Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  Stage::GetCurrent().Add(actor);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  /****************************************************/
  // Default (TRIANGLES), no index buffer
  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable( false );

  // Test the default geometry type is GL_TRIANGLE
  DALI_TEST_EQUALS( drawTrace.CountMethod( "DrawElements" ), 2, TEST_LOCATION);
  std::stringstream out;
  out << GL_TRIANGLES << ", " << numIndex << ", " << GL_UNSIGNED_SHORT<<", "<<"indices";
  DALI_TEST_EQUALS( drawTrace.TestMethodAndParams(1, "DrawElements", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS( geometry.GetGeometryType(), Geometry::TRIANGLES, TEST_LOCATION);

  /*********************************************************/
  // LINES, no index buffer
  geometry.SetGeometryType( Geometry::LINES );

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable( false );

  // geometry type is set as GL_LINES
  DALI_TEST_EQUALS( drawTrace.CountMethod( "DrawElements" ), 2, TEST_LOCATION);
  out.str("");
  out << GL_LINES << ", " << numIndex << ", " << GL_UNSIGNED_SHORT<<", "<<"indices";
  DALI_TEST_EQUALS( drawTrace.TestMethodAndParams(1, "DrawElements", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS( geometry.GetGeometryType(), Geometry::LINES, TEST_LOCATION);

  /*****************************************************/
  //POINTS
  geometry.SetGeometryType( Geometry::POINTS );

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable( false );

  // geometry type is set as GL_POINTS
  // As Points does not use the index buffer, call glDrawArrays,
  DALI_TEST_EQUALS( drawTrace.CountMethod( "DrawArrays" ), 2, TEST_LOCATION);
  out.str("");
  out << GL_POINTS << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS( drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS( geometry.GetGeometryType(), Geometry::POINTS, TEST_LOCATION);

  /*****************************************************/
  //TRIANGLE_STRIP
  geometry.SetGeometryType( Geometry::TRIANGLE_STRIP );

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable( false );

  // geometry type is set as GL_POINTS
  DALI_TEST_EQUALS( drawTrace.CountMethod( "DrawElements" ), 2, TEST_LOCATION);
  out.str("");
  out << GL_TRIANGLE_STRIP << ", " << numIndex << ", " << GL_UNSIGNED_SHORT<<", "<<"indices";
  DALI_TEST_EQUALS( drawTrace.TestMethodAndParams(1, "DrawElements", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS( geometry.GetGeometryType(), Geometry::TRIANGLE_STRIP, TEST_LOCATION);

  /*****************************************************/
  //TRIANGLE_FAN
  geometry.SetGeometryType( Geometry::TRIANGLE_FAN );

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable( false );

  // geometry type is set as GL_POINTS
  DALI_TEST_EQUALS( drawTrace.CountMethod( "DrawElements" ), 2, TEST_LOCATION);
  out.str("");
  out << GL_TRIANGLE_FAN << ", " << numIndex << ", " << GL_UNSIGNED_SHORT<<", "<<"indices";
  DALI_TEST_EQUALS( drawTrace.TestMethodAndParams(1, "DrawElements", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS( geometry.GetGeometryType(), Geometry::TRIANGLE_FAN, TEST_LOCATION);

  END_TEST;
}

