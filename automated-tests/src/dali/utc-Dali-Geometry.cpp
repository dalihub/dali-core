/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/public-api/dali-core.h>

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
struct TexturedQuadVertex
{
  Vector2 position;
  Vector2 textureCoordinates;
};

VertexBuffer CreateVertexBuffer(const std::string& aPosition, const std::string& aTexCoord)
{
  const float        halfQuadSize              = .5f;
  TexturedQuadVertex texturedQuadVertexData[4] = {
    {Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f)},
    {Vector2(halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f)},
    {Vector2(-halfQuadSize, halfQuadSize), Vector2(0.f, 1.f)},
    {Vector2(halfQuadSize, halfQuadSize), Vector2(1.f, 1.f)}};

  Property::Map vertexFormat;
  vertexFormat[aPosition] = Property::VECTOR2;
  vertexFormat[aTexCoord] = Property::VECTOR2;

  VertexBuffer vertexData = VertexBuffer::New(vertexFormat);
  vertexData.SetData(texturedQuadVertexData, 4);

  return vertexData;
}

} // namespace

int UtcDaliGeometryNew01(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  DALI_TEST_EQUALS((bool)geometry, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeometryNew02(void)
{
  TestApplication application;
  Geometry        geometry;
  DALI_TEST_EQUALS((bool)geometry, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeometryCopyConstructor(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  Geometry geometryCopy(geometry);

  DALI_TEST_EQUALS((bool)geometryCopy, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeometryAssignmentOperator(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  Geometry geometry2;
  DALI_TEST_EQUALS((bool)geometry2, false, TEST_LOCATION);

  geometry2 = geometry;
  DALI_TEST_EQUALS((bool)geometry2, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGeometryMoveConstructor(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();
  DALI_TEST_CHECK(geometry);
  DALI_TEST_EQUALS(1, geometry.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, geometry.GetNumberOfVertexBuffers(), TEST_LOCATION);

  VertexBuffer vertexBuffer = CreateVertexBuffer("aPosition", "aTexCoord");
  geometry.AddVertexBuffer(vertexBuffer);
  DALI_TEST_EQUALS(1u, geometry.GetNumberOfVertexBuffers(), TEST_LOCATION);

  Geometry move = std::move(geometry);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, move.GetNumberOfVertexBuffers(), TEST_LOCATION);
  DALI_TEST_CHECK(!geometry);

  END_TEST;
}

int UtcDaliGeometryMoveAssignment(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();
  DALI_TEST_CHECK(geometry);
  DALI_TEST_EQUALS(1, geometry.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, geometry.GetNumberOfVertexBuffers(), TEST_LOCATION);

  VertexBuffer vertexBuffer = CreateVertexBuffer("aPosition", "aTexCoord");
  geometry.AddVertexBuffer(vertexBuffer);
  DALI_TEST_EQUALS(1u, geometry.GetNumberOfVertexBuffers(), TEST_LOCATION);

  Geometry move;
  move = std::move(geometry);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, move.GetNumberOfVertexBuffers(), TEST_LOCATION);
  DALI_TEST_CHECK(!geometry);

  END_TEST;
}

int UtcDaliGeometryDownCast01(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  BaseHandle handle(geometry);
  Geometry   geometry2 = Geometry::DownCast(handle);
  DALI_TEST_EQUALS((bool)geometry2, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeometryDownCast02(void)
{
  TestApplication application;

  Handle   handle   = Handle::New(); // Create a custom object
  Geometry geometry = Geometry::DownCast(handle);
  DALI_TEST_EQUALS((bool)geometry, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeometryAddVertexBuffer(void)
{
  TestApplication application;

  tet_infoline("Test AddVertexBuffer");
  auto& bufferTrace = application.GetGlAbstraction().GetBufferTrace();
  bufferTrace.Enable(true);
  bufferTrace.EnableLogging(true);

  VertexBuffer vertexBuffer1 = CreateVertexBuffer("aPosition1", "aTexCoord1");
  Geometry     geometry      = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer1);

  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();
    DALI_TEST_EQUALS(bufferDataCalls.size(), 1u, TEST_LOCATION);

    DALI_TEST_EQUALS(bufferDataCalls[0], 4 * sizeof(TexturedQuadVertex), TEST_LOCATION);
  }

  // add the second vertex buffer
  application.GetGlAbstraction().ResetBufferDataCalls();

  VertexBuffer vertexBuffer2 = CreateVertexBuffer("aPosition2", "aTexCoord2");
  geometry.AddVertexBuffer(vertexBuffer2);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

    //Check that only the new buffer gets uploaded
    DALI_TEST_EQUALS(bufferDataCalls.size(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(bufferDataCalls[0], 4 * sizeof(TexturedQuadVertex), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliGeometryGetNumberOfVertexBuffers(void)
{
  TestApplication application;
  auto&           bufferTrace = application.GetGlAbstraction().GetBufferTrace();
  bufferTrace.Enable(true);
  bufferTrace.EnableLogging(true);

  tet_infoline("Test GetNumberOfVertexBuffers");
  VertexBuffer vertexBuffer1 = CreateVertexBuffer("aPosition1", "aTexCoord1");
  VertexBuffer vertexBuffer2 = CreateVertexBuffer("aPosition2", "aTexCoord2");
  VertexBuffer vertexBuffer3 = CreateVertexBuffer("aPosition3", "aTexCoord3");

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer1);
  DALI_TEST_EQUALS(geometry.GetNumberOfVertexBuffers(), 1u, TEST_LOCATION);

  geometry.AddVertexBuffer(vertexBuffer2);
  geometry.AddVertexBuffer(vertexBuffer3);
  DALI_TEST_EQUALS(geometry.GetNumberOfVertexBuffers(), 3u, TEST_LOCATION);

  geometry.RemoveVertexBuffer(2u);
  DALI_TEST_EQUALS(geometry.GetNumberOfVertexBuffers(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGeometryRemoveVertexBuffer(void)
{
  TestApplication application;
  auto&           bufferTrace = application.GetGlAbstraction().GetBufferTrace();
  bufferTrace.Enable(true);
  bufferTrace.EnableLogging(true);

  tet_infoline("Test RemoveVertexBuffer");

  VertexBuffer vertexBuffer1 = CreateVertexBuffer("aPosition1", "aTexCoord1");
  VertexBuffer vertexBuffer2 = CreateVertexBuffer("aPosition2", "aTexCoord2");

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer1);

  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  DALI_TEST_EQUALS(geometry.GetNumberOfVertexBuffers(), 1u, TEST_LOCATION);

  geometry.RemoveVertexBuffer(0);
  geometry.AddVertexBuffer(vertexBuffer2);
  DALI_TEST_EQUALS(geometry.GetNumberOfVertexBuffers(), 1u, TEST_LOCATION);

  geometry.RemoveVertexBuffer(0);
  DALI_TEST_EQUALS(geometry.GetNumberOfVertexBuffers(), 0u, TEST_LOCATION);

  //Todo: test by checking the BufferDataCalls
  // make sure the vertex buffer in actually removed from gl

  END_TEST;
}

int UtcDaliGeometrySetIndexBuffer(void)
{
  TestApplication application;
  auto&           bufferTrace = application.GetGlAbstraction().GetBufferTrace();
  bufferTrace.Enable(true);
  bufferTrace.EnableLogging(true);

  tet_infoline("Test SetIndexBuffer");

  VertexBuffer vertexBuffer = CreateVertexBuffer("aPosition", "aTexCoord");

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);

  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

    DALI_TEST_EQUALS(bufferDataCalls.size(), 1u, TEST_LOCATION);

    DALI_TEST_EQUALS(bufferDataCalls[0], 4 * sizeof(TexturedQuadVertex), TEST_LOCATION);
  }

  // Set index buffer
  application.GetGlAbstraction().ResetBufferDataCalls();

  const unsigned short indexData[6] = {0, 3, 1, 0, 2, 3};
  geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

    //Only the index buffer should be uploaded
    DALI_TEST_EQUALS(bufferDataCalls.size(), 1u, TEST_LOCATION);

    // should be unsigned short instead of unsigned int
    DALI_TEST_EQUALS(bufferDataCalls[0], 6 * sizeof(unsigned short), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliGeometrySetIndexBuffer32Bits(void)
{
  TestApplication application;
  auto&           bufferTrace = application.GetGlAbstraction().GetBufferTrace();
  bufferTrace.Enable(true);
  bufferTrace.EnableLogging(true);

  tet_infoline("Test SetIndexBuffer 32Bits");

  VertexBuffer vertexBuffer = CreateVertexBuffer("aPosition", "aTexCoord");

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);

  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

    DALI_TEST_EQUALS(bufferDataCalls.size(), 1u, TEST_LOCATION);

    DALI_TEST_EQUALS(bufferDataCalls[0], 4 * sizeof(TexturedQuadVertex), TEST_LOCATION);
  }

  // Set index buffer
  application.GetGlAbstraction().ResetBufferDataCalls();

  const uint32_t indexData32Bits[6] = {0, 3, 1, 0, 2, 3};
  geometry.SetIndexBuffer(indexData32Bits, sizeof(indexData32Bits) / sizeof(indexData32Bits[0]));
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

    //Only the index buffer should be uploaded
    DALI_TEST_EQUALS(bufferDataCalls.size(), 1u, TEST_LOCATION);

    // should be unsigned int instead of unsigned short
    DALI_TEST_EQUALS(bufferDataCalls[0], 6 * sizeof(uint32_t), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliGeometrySetGetGeometryType01(void)
{
  TestApplication application;
  auto&           bufferTrace = application.GetGlAbstraction().GetBufferTrace();
  bufferTrace.Enable(true);
  bufferTrace.EnableLogging(true);

  tet_infoline("Test SetType and GetType: without index buffer");

  unsigned int numVertex    = 4u;
  VertexBuffer vertexBuffer = CreateVertexBuffer("aPosition", "aTexCoord");

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);

  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack&    drawTrace     = glAbstraction.GetDrawTrace();

  /****************************************************/
  // Default (TRIANGLES), no index buffer
  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable(false);

  // Test the default geometry type is GL_TRIANGLE
  // no index buffer, call glDrawArrays,
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawArrays"), 2, TEST_LOCATION);
  std::stringstream out;
  out << GL_TRIANGLES << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS(drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS(geometry.GetType(), Geometry::TRIANGLES, TEST_LOCATION);

  /*********************************************************/
  // LINES, no index buffer
  geometry.SetType(Geometry::LINES);

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable(false);

  // geometry type is set as GL_LINES
  // no index buffer, call glDrawArrays,
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawArrays"), 2, TEST_LOCATION);
  out.str("");
  out << GL_LINES << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS(drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS(geometry.GetType(), Geometry::LINES, TEST_LOCATION);

  /*****************************************************/
  //POINTS
  geometry.SetType(Geometry::POINTS);

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable(false);

  // geometry type is set as GL_POINTS
  // no index buffer, call glDrawArrays,
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawArrays"), 2, TEST_LOCATION);
  out.str("");
  out << GL_POINTS << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS(drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS(geometry.GetType(), Geometry::POINTS, TEST_LOCATION);

  /*****************************************************/
  //TRIANGLE_STRIP, no index buffer
  geometry.SetType(Geometry::TRIANGLE_STRIP);

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable(false);

  // geometry type is set as GL_TRIANGLE_STRIP
  // no index buffer, call glDrawArrays,
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawArrays"), 2, TEST_LOCATION);
  out.str("");
  out << GL_TRIANGLE_STRIP << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS(drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS(geometry.GetType(), Geometry::TRIANGLE_STRIP, TEST_LOCATION);

  /*****************************************************/
  //TRIANGLE_FAN, no index buffer
  geometry.SetType(Geometry::TRIANGLE_FAN);

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable(false);

  // geometry type is set as GL_TRIANGLE_FAN
  // no index buffer, call glDrawArrays,
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawArrays"), 2, TEST_LOCATION);
  out.str("");
  out << GL_TRIANGLE_FAN << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS(drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS(geometry.GetType(), Geometry::TRIANGLE_FAN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGeometrySetGetGeometryType02(void)
{
  TestApplication application;
  auto&           bufferTrace = application.GetGlAbstraction().GetBufferTrace();
  bufferTrace.Enable(true);
  bufferTrace.EnableLogging(true);

  tet_infoline("Test SetType and GetType: with index buffer");

  unsigned int numVertex    = 4u;
  unsigned int numIndex     = 6u; // 6 unsigned short
  VertexBuffer vertexBuffer = CreateVertexBuffer("aPosition", "aTexCoord");

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  const unsigned short indexData[6] = {0, 3, 1, 0, 2, 3};
  geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));

  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack&    drawTrace     = glAbstraction.GetDrawTrace();

  /****************************************************/
  // Default (TRIANGLES), with index buffer
  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable(false);

  // Test the default geometry type is GL_TRIANGLE
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION);
  std::stringstream out;
  out << GL_TRIANGLES << ", " << numIndex << ", " << GL_UNSIGNED_SHORT << ", "
      << "indices";
  DALI_TEST_EQUALS(drawTrace.TestMethodAndParams(1, "DrawElements", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS(geometry.GetType(), Geometry::TRIANGLES, TEST_LOCATION);

  /*********************************************************/
  // LINES, with index buffer
  geometry.SetType(Geometry::LINES);

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable(false);

  // geometry type is set as GL_LINES
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION);
  out.str("");
  out << GL_LINES << ", " << numIndex << ", " << GL_UNSIGNED_SHORT << ", "
      << "indices";
  DALI_TEST_EQUALS(drawTrace.TestMethodAndParams(1, "DrawElements", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS(geometry.GetType(), Geometry::LINES, TEST_LOCATION);

  /*****************************************************/
  //POINTS
  geometry.SetType(Geometry::POINTS);

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable(false);

  // geometry type is set as GL_POINTS
  // As Points does not use the index buffer, call glDrawArrays,
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawArrays"), 2, TEST_LOCATION);
  out.str("");
  out << GL_POINTS << ", " << 0 << ", " << numVertex;
  DALI_TEST_EQUALS(drawTrace.TestMethodAndParams(1, "DrawArrays", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS(geometry.GetType(), Geometry::POINTS, TEST_LOCATION);

  /*****************************************************/
  //TRIANGLE_STRIP
  geometry.SetType(Geometry::TRIANGLE_STRIP);

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable(false);

  // geometry type is set as GL_TRIANGLE_STRIP
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION);
  out.str("");
  out << GL_TRIANGLE_STRIP << ", " << numIndex << ", " << GL_UNSIGNED_SHORT << ", "
      << "indices";
  DALI_TEST_EQUALS(drawTrace.TestMethodAndParams(1, "DrawElements", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS(geometry.GetType(), Geometry::TRIANGLE_STRIP, TEST_LOCATION);

  /*****************************************************/
  //TRIANGLE_FAN
  geometry.SetType(Geometry::TRIANGLE_FAN);

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  drawTrace.Enable(false);

  // geometry type is set as GL_TRIANGLE_FAN
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION);
  out.str("");
  out << GL_TRIANGLE_FAN << ", " << numIndex << ", " << GL_UNSIGNED_SHORT << ", "
      << "indices";
  DALI_TEST_EQUALS(drawTrace.TestMethodAndParams(1, "DrawElements", out.str()), true, TEST_LOCATION);

  DALI_TEST_EQUALS(geometry.GetType(), Geometry::TRIANGLE_FAN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGeometrySetIndexBufferNegative(void)
{
  TestApplication application;
  Dali::Geometry  instance;
  try
  {
    unsigned short* arg1(nullptr);
    unsigned long   arg2(0u);
    instance.SetIndexBuffer(arg1, arg2);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliGeometryAddVertexBufferNegative(void)
{
  TestApplication application;
  Dali::Geometry  instance;
  try
  {
    Dali::VertexBuffer arg1;
    instance.AddVertexBuffer(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliGeometryRemoveVertexBufferNegative(void)
{
  TestApplication application;
  Dali::Geometry  instance;
  try
  {
    unsigned long arg1(0u);
    instance.RemoveVertexBuffer(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliGeometrySetTypeNegative(void)
{
  TestApplication application;
  Dali::Geometry  instance;
  try
  {
    Dali::Geometry::Type arg1(Geometry::POINTS);
    instance.SetType(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliGeometryGetNumberOfVertexBuffersNegative(void)
{
  TestApplication application;
  Dali::Geometry  instance;
  try
  {
    instance.GetNumberOfVertexBuffers();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliGeometryGetTypeNegative(void)
{
  TestApplication application;
  Dali::Geometry  instance;
  try
  {
    instance.GetType();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliGeometryDestructWorkerThreadN(void)
{
  TestApplication application;
  tet_infoline("UtcDaliGeometryDestructWorkerThreadN Test, for line coverage");

  try
  {
    class TestThread : public Thread
    {
    public:
      virtual void Run()
      {
        tet_printf("Run TestThread\n");
        // Destruct at worker thread.
        mGeometry.Reset();
      }

      Dali::Geometry mGeometry;
    };
    TestThread thread;

    Dali::Geometry geometry = Dali::Geometry::New();
    thread.mGeometry        = std::move(geometry);
    geometry.Reset();

    thread.Start();

    thread.Join();
  }
  catch(...)
  {
  }

  // Always success
  DALI_TEST_CHECK(true);

  END_TEST;
}

namespace
{
void EnsureDirtyRectIsEmpty(TestApplication& application, const char* location)
{
  Rect<int>              clippingRect = TestApplication::DEFAULT_SURFACE_RECT;
  std::vector<Rect<int>> damagedRects;

  // Try render several frames as full surface.
  for(int i = 0; i < 3; i++)
  {
    application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
    application.RenderWithPartialUpdate(damagedRects, clippingRect);

    DALI_TEST_EQUALS(damagedRects.size(), 0, location);
  }
}
} // namespace

int utcDaliGeometryPartialUpdateChangeIndicies(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged rect with changing uniform");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  // First render pass, nothing to render, adaptor would just do swap buffer.
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  Shader shader = Shader::New("VertexSource", "FragmentSource");

  VertexBuffer vertexData   = CreateVertexBuffer();
  const float  halfQuadSize = .5f;
  struct TexturedQuadVertex
  {
    Vector2 position;
    Vector2 textureCoordinates;
  };
  TexturedQuadVertex texturedQuadVertexData[5] = {
    {Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f)},
    {Vector2(halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f)},
    {Vector2(-halfQuadSize, halfQuadSize), Vector2(0.f, 1.f)},
    {Vector2(halfQuadSize, halfQuadSize), Vector2(1.f, 1.f)},
    {Vector2(0.0f, 0.0f), Vector2(0.5f, 0.5f)}};
  vertexData.SetData(texturedQuadVertexData, 4);

  unsigned short indexData[6] = {0, 3, 1, 0, 2, 3};

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexData);
  geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));

  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  // 1. Actor added, damaged rect is added size of actor
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(0, 752, 48, 48); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();

  // Ensure the damaged rect is empty
  EnsureDirtyRectIsEmpty(application, TEST_LOCATION);

  // 2. Change the index buffer
  geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]) - 1u);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(0, 752, 48, 48); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();

  // Ensure the damaged rect is empty
  EnsureDirtyRectIsEmpty(application, TEST_LOCATION);

  // 3. Change vertex buffer data
  vertexData.SetData(texturedQuadVertexData, 5);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(0, 752, 48, 48); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();

  // Ensure the damaged rect is empty
  EnsureDirtyRectIsEmpty(application, TEST_LOCATION);

  // 4. Change geometry type
  geometry.SetType(Geometry::LINES);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(0, 752, 48, 48); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();

  // Ensure the damaged rect is empty
  EnsureDirtyRectIsEmpty(application, TEST_LOCATION);

  // 5. Add vertex data
  VertexBuffer vertexData2 = CreateVertexBuffer();
  vertexData2.SetData(texturedQuadVertexData, 4);
  geometry.AddVertexBuffer(vertexData2);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(0, 752, 48, 48); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();

  // Ensure the damaged rect is empty
  EnsureDirtyRectIsEmpty(application, TEST_LOCATION);

  // 5. Remove vertex data
  geometry.RemoveVertexBuffer(0);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(0, 752, 48, 48); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();

  // Ensure the damaged rect is empty
  EnsureDirtyRectIsEmpty(application, TEST_LOCATION);

  END_TEST;
}
