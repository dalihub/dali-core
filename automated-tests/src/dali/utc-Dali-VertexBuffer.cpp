/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <chrono>
using namespace std::chrono_literals;
using namespace Dali;

#include <mesh-builder.h>
#include <future>

struct VertexBufferUpdater
{
  struct Diagnostics
  {
    bool     success{false};
    uint32_t counter{0};
    void*    lastPtr{nullptr};
    size_t   lastSize{0};
    size_t   lastReturned{0};
  };

  VertexBufferUpdater() = default;

  uint32_t UpdateVertices(void* ptr, size_t size)
  {
    diagnostics.success      = true;
    diagnostics.lastPtr      = ptr;
    diagnostics.lastSize     = size;
    diagnostics.lastReturned = returnSize;
    diagnostics.counter++;

    promise.set_value(diagnostics);
    return returnSize;
  }

  void SetCallbackReturnValue(size_t size)
  {
    returnSize = size;
  }

  void Reset()
  {
    promise = std::promise<Diagnostics>();
  }

  std::unique_ptr<VertexBufferUpdateCallback> CreateCallback()
  {
    return VertexBufferUpdateCallback::New(this, &VertexBufferUpdater::UpdateVertices);
  }

  Diagnostics GetValue()
  {
    auto value = promise.get_future().get();

    // reset promise automatically
    promise = {};
    return value;
  }

  Diagnostics GetValueWithTimeout()
  {
    auto future = promise.get_future();
    auto status = future.wait_for(1s);
    if(status == std::future_status::ready)
    {
      promise = {};
      return promise.get_future().get();
    }

    promise = {};
    // reset promise automatically
    Diagnostics failure{};
    failure.success = false;
    return failure;
  }

  bool IsValueReady()
  {
    // fake-wait for two frames
    auto status = promise.get_future().wait_for(32ms);
    return status == std::future_status::ready;
  }

  Diagnostics               diagnostics;
  size_t                    returnSize{0u};
  std::promise<Diagnostics> promise;
};

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

    auto& drawTrace = application.GetGlAbstraction().GetDrawTrace();
    drawTrace.Enable(true);

    application.SendNotification();
    application.Render(0);
    application.Render();
    application.SendNotification();

    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

    DALI_TEST_CHECK(drawTrace.FindMethod("DrawArrays"));

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

  END_TEST;
}

int UtcDaliVertexBufferMapInitializerList(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat = Property::Map{{"aPosition", Property::VECTOR2},
                                                         {"aTexCoord", Property::VECTOR2},
                                                         {"aColor", Property::VECTOR4}};

  try
  {
    VertexBuffer vertexBuffer = VertexBuffer::New(texturedQuadVertexFormat);
    tet_result(TET_PASS);
  }
  catch(Dali::DaliException& e)
  {
    // Shouldn't assert any more
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliVertexBufferInvalidTypeN01(void)
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

int UtcDaliVertexBufferInvalidTypeN02(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat = Property::Map{{"aPosition", Property::MAP},
                                                         {"aTexCoord", Property::STRING},
                                                         {"aColor", Property::VECTOR4}};

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

int UtcDaliVertexBufferSetDivisor(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aTexCoord"] = Property::VECTOR2;

  Property::Map instanceFormat{{"aTranslate", Property::VECTOR2}, {"aColor", Property::VECTOR4}};

  VertexBuffer vertexBuffer = VertexBuffer::New(texturedQuadVertexFormat);
  DALI_TEST_EQUALS((bool)vertexBuffer, true, TEST_LOCATION);

  DALI_TEST_EQUALS(0, vertexBuffer.GetDivisor(), TEST_LOCATION);

  VertexBuffer instanceBuffer = VertexBuffer::New(instanceFormat);
  DALI_TEST_EQUALS((bool)instanceBuffer, true, TEST_LOCATION);

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

  struct InstanceData
  {
    Vector2 translate;
    Vector4 color;
  };

  InstanceData instanceData[] = {{Vector2(12, 33), Color::WHITE},
                                 {Vector2(-2000, 43), Color::BLUE},
                                 {Vector2(200, 43), Color::GREEN},
                                 {Vector2(-243, 43), Color::TURQUOISE},
                                 {Vector2(192, 43), Color::CYAN},
                                 {Vector2(-2000, 43), Color::MAGENTA},
                                 {Vector2(-292, 393), Color::BLUE},
                                 {Vector2(-499, 128), Color::BLUE},
                                 {Vector2(328, 43), Color::BLUE},
                                 {Vector2(726, 43), Color::BLUE}};
  instanceBuffer.SetData(instanceData, sizeof(instanceData) / sizeof(InstanceData));
  instanceBuffer.SetDivisor(1);
  DALI_TEST_EQUALS(1, instanceBuffer.GetDivisor(), TEST_LOCATION);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.AddVertexBuffer(instanceBuffer);

  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  TestGlAbstraction& gl          = application.GetGlAbstraction();
  auto&              bufferTrace = gl.GetBufferTrace();
  auto&              drawTrace   = gl.GetDrawTrace();
  bufferTrace.Enable(true);
  drawTrace.Enable(true);

  application.SendNotification();
  application.Render();

  TraceCallStack::NamedParams params;
  params["divisor"] << "1";
  DALI_TEST_CHECK(bufferTrace.FindMethodAndParams("VertexAttribDivisor", params));

  tet_infoline("Test that by default, instancing isn't used");
  TraceCallStack::NamedParams params2;
  params2["instanceCount"] << 0;
  DALI_TEST_CHECK(!drawTrace.FindMethodAndGetParameters("DrawArraysInstanced", params2));
  DALI_TEST_CHECK(drawTrace.FindMethod("DrawArrays"));

  tet_infoline("Test that instancing is used if Renderer requests an instance count");
  drawTrace.Reset();

  int instanceCount         = sizeof(instanceData) / sizeof(InstanceData);
  renderer["instanceCount"] = instanceCount;
  application.SendNotification();
  application.Render();

  TraceCallStack::NamedParams params3;
  params3["instanceCount"] << instanceCount;
  DALI_TEST_CHECK(drawTrace.FindMethodAndGetParameters("DrawArraysInstanced", params3));
  DALI_TEST_CHECK(!drawTrace.FindMethod("DrawArrays"));
  END_TEST;
}

int UtcDaliVertexBufferUpdateCallback(void)
{
  TestApplication application;

  // Create vertex buffer
  VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map() = {
                                                  {"aPosition", Property::Type::VECTOR2},
                                                  {"aTexCoord", Property::Type::VECTOR2}});

  // set callback
  auto callback = std::make_unique<VertexBufferUpdater>();
  vertexBuffer.SetVertexBufferUpdateCallback(callback->CreateCallback());

  struct Vertex
  {
    Vector2 pos;
    Vector2 uv;
  };

  std::vector<Vertex> vertices;
  vertices.resize(16);
  vertexBuffer.SetData(vertices.data(), 16);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  auto& gl    = application.GetGlAbstraction();
  auto& trace = gl.GetDrawTrace();
  trace.Enable(true);
  trace.EnableLogging(true);

  callback->SetCallbackReturnValue(16 * sizeof(Vertex));

  application.SendNotification();
  application.Render();

  auto value = callback->GetValue();

  // Test whether callback ran
  DALI_TEST_EQUALS(value.counter, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(value.lastSize, 16 * sizeof(Vertex), TEST_LOCATION);
  DALI_TEST_EQUALS(value.lastReturned, 16 * sizeof(Vertex), TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(value.lastPtr, (void*)nullptr, 0, TEST_LOCATION);

  // test whether draw call has been issued (return value indicates end of array to be drawn)
  auto result = trace.FindMethod("DrawArrays");
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);
  result = trace.FindMethodAndParams("DrawArrays", "4, 0, 16");
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);

  // Test 2. Update and render only half of vertex buffer
  callback->SetCallbackReturnValue(8 * sizeof(Vertex));
  trace.Reset();

  application.SendNotification();
  application.Render();

  value = callback->GetValue();
  // Test whether callback ran
  DALI_TEST_EQUALS(value.counter, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(value.lastSize, 16 * sizeof(Vertex), TEST_LOCATION);
  DALI_TEST_EQUALS(value.lastReturned, 8 * sizeof(Vertex), TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(value.lastPtr, (void*)nullptr, 0, TEST_LOCATION);
  result = trace.FindMethod("DrawArrays");
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);
  result = trace.FindMethodAndParams("DrawArrays", "4, 0, 8");
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);

  // Test 3. callback returns 0 elements to render, the draw call shouldn't happen.
  callback->SetCallbackReturnValue(0);
  trace.Reset();

  application.SendNotification();
  application.Render();

  value = callback->GetValue();
  // Test whether callback ran
  DALI_TEST_EQUALS(value.counter, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(value.lastSize, 16 * sizeof(Vertex), TEST_LOCATION);
  DALI_TEST_EQUALS(value.lastReturned, 0, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(value.lastPtr, (void*)nullptr, 0, TEST_LOCATION);
  result = trace.FindMethod("DrawArrays");
  DALI_TEST_EQUALS(result, false, TEST_LOCATION);

  // Test 4. removing callback, original behaviour should kick in
  vertexBuffer.SetVertexBufferUpdateCallback(nullptr);
  trace.Reset();
  callback->Reset();

  application.SendNotification();
  application.Render();

  auto valueReady = callback->IsValueReady();
  DALI_TEST_EQUALS(valueReady, false, TEST_LOCATION);
  DALI_TEST_EQUALS(callback->diagnostics.counter, 3, TEST_LOCATION);
  result = trace.FindMethod("DrawArrays");
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);
  result = trace.FindMethodAndParams("DrawArrays", "4, 0, 16");
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSetAndRemoveVertexBufferUpdateCallback(void)
{
  TestApplication application;

  // Create vertex buffer
  VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map() = {
                                                  {"aPosition", Property::Type::VECTOR2},
                                                  {"aTexCoord", Property::Type::VECTOR2}});

  // set callback
  auto callback = std::make_unique<VertexBufferUpdater>();
  vertexBuffer.SetVertexBufferUpdateCallback(callback->CreateCallback());

  struct Vertex
  {
    Vector2 pos;
    Vector2 uv;
  };

  std::vector<Vertex> vertices;
  vertices.resize(16);
  vertexBuffer.SetData(vertices.data(), 16);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  auto& gl    = application.GetGlAbstraction();
  auto& trace = gl.GetDrawTrace();
  trace.Enable(true);
  trace.EnableLogging(true);

  callback->SetCallbackReturnValue(16 * sizeof(Vertex));

  application.SendNotification();
  application.Render();

  auto value = callback->GetValue();

  // Test whether callback ran
  DALI_TEST_EQUALS(value.counter, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(value.lastSize, 16 * sizeof(Vertex), TEST_LOCATION);
  DALI_TEST_EQUALS(value.lastReturned, 16 * sizeof(Vertex), TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(value.lastPtr, (void*)nullptr, 0, TEST_LOCATION);

  // test whether draw call has been issued (return value indicates end of array to be drawn)
  auto result = trace.FindMethod("DrawArrays");
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);
  result = trace.FindMethodAndParams("DrawArrays", "4, 0, 16");
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);

  // Test 2. Update and render only half of vertex buffer
  callback->SetCallbackReturnValue(8 * sizeof(Vertex));
  trace.Reset();

  // Remove the callback
  vertexBuffer.ClearVertexBufferUpdateCallback();

  application.SendNotification();
  application.Render();

  // Use 1sec timeout as callback won't be executed and future won't be filled
  value = callback->GetValueWithTimeout();
  // Test whether callback ran
  DALI_TEST_EQUALS(value.success, false, TEST_LOCATION);

  END_TEST;
}
