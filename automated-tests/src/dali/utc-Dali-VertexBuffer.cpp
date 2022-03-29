/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/dali-core.h>

using namespace Dali;

#include <mesh-builder.h>

void vertexBuffer_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void vertexBuffer_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliVertexBufferNew01(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"]    = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  VertexBuffer vertexBuffer = VertexBuffer::New(texturedQuadVertexFormat);

  DALI_TEST_EQUALS((bool)vertexBuffer, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVertexBufferNew02(void)
{
  TestApplication application;
  VertexBuffer    vertexBuffer;
  DALI_TEST_EQUALS((bool)vertexBuffer, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVertexBufferDownCast01(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"]    = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  VertexBuffer vertexBuffer = VertexBuffer::New(texturedQuadVertexFormat);

  BaseHandle   handle(vertexBuffer);
  VertexBuffer vertexBuffer2 = VertexBuffer::DownCast(handle);
  DALI_TEST_EQUALS((bool)vertexBuffer2, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVertexBufferDownCast02(void)
{
  TestApplication application;

  Handle       handle       = Handle::New(); // Create a custom object
  VertexBuffer vertexBuffer = VertexBuffer::DownCast(handle);
  DALI_TEST_EQUALS((bool)vertexBuffer, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVertexBufferCopyConstructor(void)
{
  TestApplication application;

  VertexBuffer vertexBuffer = CreateVertexBuffer();

  VertexBuffer vertexBufferCopy(vertexBuffer);

  DALI_TEST_EQUALS((bool)vertexBufferCopy, true, TEST_LOCATION);
  DALI_TEST_EQUALS(vertexBufferCopy.GetSize(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVertexBufferAssignmentOperator(void)
{
  TestApplication application;

  VertexBuffer vertexBuffer = CreateVertexBuffer();

  VertexBuffer vertexBuffer2;
  DALI_TEST_EQUALS((bool)vertexBuffer2, false, TEST_LOCATION);

  vertexBuffer2 = vertexBuffer;
  DALI_TEST_EQUALS((bool)vertexBuffer2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(vertexBuffer2.GetSize(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVertexBufferMoveConstructor(void)
{
  TestApplication application;

  VertexBuffer vertexBuffer = CreateVertexBuffer();
  DALI_TEST_CHECK(vertexBuffer);
  DALI_TEST_EQUALS(1, vertexBuffer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, vertexBuffer.GetSize(), TEST_LOCATION);

  VertexBuffer move = std::move(vertexBuffer);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, move.GetSize(), TEST_LOCATION);
  DALI_TEST_CHECK(!vertexBuffer);

  END_TEST;
}

int UtcDaliVertexBufferMoveAssignment(void)
{
  TestApplication application;

  VertexBuffer vertexBuffer = CreateVertexBuffer();
  DALI_TEST_CHECK(vertexBuffer);
  DALI_TEST_EQUALS(1, vertexBuffer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, vertexBuffer.GetSize(), TEST_LOCATION);

  VertexBuffer move;
  move = std::move(vertexBuffer);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, move.GetSize(), TEST_LOCATION);
  DALI_TEST_CHECK(!vertexBuffer);

  END_TEST;
}

int UtcDaliVertexBufferSetData01(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"]    = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  {
    VertexBuffer vertexBuffer = VertexBuffer::New(texturedQuadVertexFormat);
    DALI_TEST_EQUALS((bool)vertexBuffer, true, TEST_LOCATION);

    const float halfQuadSize = .5f;
    struct TexturedQuadVertex
    {
      Vector2 position;
      Vector2 textureCoordinates;
    };
    TexturedQuadVertex texturedQuadVertexData[4] = {
      {Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f)},
      {Vector2(halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f)},
      {Vector2(-halfQuadSize, halfQuadSize), Vector2(0.f, 1.f)},
      {Vector2(halfQuadSize, halfQuadSize), Vector2(1.f, 1.f)}};

    vertexBuffer.SetData(texturedQuadVertexData, 4);

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

    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

    DALI_TEST_EQUALS(bufferDataCalls.size(), 3u, TEST_LOCATION);

    DALI_TEST_EQUALS(bufferDataCalls[0], sizeof(texturedQuadVertexData), TEST_LOCATION);
  }
  // end of scope to let the buffer and geometry die; do another notification and render to get the deletion processed
  application.SendNotification();
  application.Render(0);

  END_TEST;
}

int UtcDaliVertexBufferSetData02(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"]    = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  VertexBuffer vertexBuffer = VertexBuffer::New(texturedQuadVertexFormat);
  DALI_TEST_EQUALS((bool)vertexBuffer, true, TEST_LOCATION);

  const float halfQuadSize = .5f;
  struct TexturedQuadVertex
  {
    Vector2 position;
    Vector2 textureCoordinates;
  };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    {Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f)},
    {Vector2(halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f)},
    {Vector2(-halfQuadSize, halfQuadSize), Vector2(0.f, 1.f)},
    {Vector2(halfQuadSize, halfQuadSize), Vector2(1.f, 1.f)}};

  vertexBuffer.SetData(texturedQuadVertexData, 4);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);

  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  {
    const TestGlAbstraction::BufferSubDataCalls& bufferSubDataCalls =
      application.GetGlAbstraction().GetBufferSubDataCalls();

    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

    // Should be 1 (Flush standalone uniform buffer per each RenderScene)
    DALI_TEST_EQUALS(bufferSubDataCalls.size(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(bufferDataCalls.size(), 2u, TEST_LOCATION);

    DALI_TEST_EQUALS(bufferDataCalls[0], sizeof(texturedQuadVertexData), TEST_LOCATION);
  }

  // Re-upload the data on the vertexBuffer
  vertexBuffer.SetData(texturedQuadVertexData, 4);

  application.SendNotification();
  application.Render(0);

  {
    const TestGlAbstraction::BufferSubDataCalls& bufferSubDataCalls =
      application.GetGlAbstraction().GetBufferSubDataCalls();

    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

    // Should be 3 (2 Render + 1 vertexBuffer reload)
    DALI_TEST_EQUALS(bufferSubDataCalls.size(), 3u, TEST_LOCATION);
    DALI_TEST_EQUALS(bufferDataCalls.size(), 3u, TEST_LOCATION);

    if(bufferSubDataCalls.size() >= 2)
    {
      DALI_TEST_EQUALS(bufferSubDataCalls[1], sizeof(texturedQuadVertexData), TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliVertexBufferInvalidTypeN(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"]    = Property::MAP;
  texturedQuadVertexFormat["aVertexCoord"] = Property::STRING;

  try
  {
    VertexBuffer vertexBuffer = VertexBuffer::New(texturedQuadVertexFormat);
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_ASSERT(e, "Property::Type not supported in VertexBuffer", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliVertexBufferSetDataNegative(void)
{
  TestApplication    application;
  Dali::VertexBuffer instance;
  try
  {
    void*         arg1(nullptr);
    unsigned long arg2(0u);
    instance.SetData(arg1, arg2);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliVertexBufferGetSizeNegative(void)
{
  TestApplication    application;
  Dali::VertexBuffer instance;
  try
  {
    instance.GetSize();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}
