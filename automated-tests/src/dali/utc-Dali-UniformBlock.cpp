/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <mesh-builder.h>
#include <stdlib.h>

#include <iostream>

#include <test-platform-abstraction.h>

using namespace Dali;

void utc_dali_uniform_block_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_uniform_block_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static const char* VertexSource =
  "This is a custom vertex shader\n"
  "made on purpose to look nothing like a normal vertex shader inside dali\n";

static const char* FragmentSource =
  "This is a custom fragment shader\n"
  "made on purpose to look nothing like a normal fragment shader inside dali\n";

void TestConstraintNoBlue(Vector4& current, const PropertyInputContainer& inputs)
{
  current.b = 0.0f;
}

} // namespace

int UtcDaliUniformBlockMethodNewP1(void)
{
  TestApplication application;

  UniformBlock uniformBlock = UniformBlock::New("testBlock");
  DALI_TEST_EQUALS((bool)uniformBlock, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliUniformBlockMethodNewN1(void)
{
  TestApplication application;

  UniformBlock uniformBlock;
  DALI_TEST_EQUALS((bool)uniformBlock, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliUniformBlockAssignmentOperator(void)
{
  TestApplication application;

  UniformBlock uniformBlock1 = UniformBlock::New("testBlock");

  UniformBlock uniformBlock2;

  DALI_TEST_CHECK(!(uniformBlock1 == uniformBlock2));

  uniformBlock2 = uniformBlock1;

  DALI_TEST_CHECK(uniformBlock1 == uniformBlock2);

  uniformBlock2 = UniformBlock::New("testBlock");

  DALI_TEST_CHECK(!(uniformBlock1 == uniformBlock2));

  END_TEST;
}

int UtcDaliUniformBlockMoveConstructor(void)
{
  TestApplication application;

  UniformBlock uniformBlock = UniformBlock::New("testBlock");
  DALI_TEST_CHECK(uniformBlock);
  DALI_TEST_EQUALS(1, uniformBlock.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  // Register a custom property
  Vector2         vec(1.0f, 2.0f);
  Property::Index customIndex = uniformBlock.RegisterProperty("custom", vec);
  DALI_TEST_EQUALS(uniformBlock.GetProperty<Vector2>(customIndex), vec, TEST_LOCATION);

  UniformBlock move = std::move(uniformBlock);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector2>(customIndex), vec, TEST_LOCATION);
  DALI_TEST_CHECK(!uniformBlock);

  END_TEST;
}

int UtcDaliUniformBlockMoveAssignment(void)
{
  TestApplication application;

  UniformBlock uniformBlock = UniformBlock::New("testBlock");
  DALI_TEST_CHECK(uniformBlock);
  DALI_TEST_EQUALS(1, uniformBlock.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  // Register a custom property
  Vector2         vec(1.0f, 2.0f);
  Property::Index customIndex = uniformBlock.RegisterProperty("custom", vec);
  DALI_TEST_EQUALS(uniformBlock.GetProperty<Vector2>(customIndex), vec, TEST_LOCATION);

  UniformBlock move;
  move = std::move(uniformBlock);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector2>(customIndex), vec, TEST_LOCATION);
  DALI_TEST_CHECK(!uniformBlock);

  END_TEST;
}

int UtcDaliUniformBlockDownCast01(void)
{
  TestApplication application;

  UniformBlock uniformBlock1 = UniformBlock::New("testBlock");

  BaseHandle   handle(uniformBlock1);
  UniformBlock uniformBlock2 = UniformBlock::DownCast(handle);
  DALI_TEST_EQUALS((bool)uniformBlock2, true, TEST_LOCATION);
  DALI_TEST_CHECK(uniformBlock1 == uniformBlock2);
  END_TEST;
}

int UtcDaliUniformBlockDownCast02(void)
{
  TestApplication application;

  Handle       handle       = Handle::New(); // Create a custom object
  UniformBlock uniformBlock = UniformBlock::DownCast(handle);
  DALI_TEST_EQUALS((bool)uniformBlock, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliUniformBlockGetUniformBlockNameP(void)
{
  TestApplication application;

  UniformBlock uniformBlock1 = UniformBlock::New("testBlock");
  UniformBlock uniformBlock2 = UniformBlock::New("testBlock2");

  DALI_TEST_EQUALS(uniformBlock1.GetUniformBlockName(), std::string_view("testBlock"), TEST_LOCATION);
  DALI_TEST_EQUALS(uniformBlock2.GetUniformBlockName(), std::string_view("testBlock2"), TEST_LOCATION);

  UniformBlock move;
  move = std::move(uniformBlock1);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(move.GetUniformBlockName(), std::string_view("testBlock"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUniformBlockGetUniformBlockNameN(void)
{
  TestApplication application;

  UniformBlock uniformBlock;
  try
  {
    std::string name = std::string(uniformBlock.GetUniformBlockName());
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }

  END_TEST;
}

int UtcDaliUniformBlockConnectToShader(void)
{
  TestApplication application;

  Shader   shader   = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  UniformBlock uniformBlock = UniformBlock::New("testBlock");

  tet_printf("Connect to shader\n");
  bool ret = uniformBlock.ConnectToShader(shader);
  DALI_TEST_EQUALS(ret, true, TEST_LOCATION);

  tet_printf("Re-connect to already connected uniform block will be failed\n");
  ret = uniformBlock.ConnectToShader(shader);
  DALI_TEST_EQUALS(ret, false, TEST_LOCATION);

  Shader shader2 = Shader::New(VertexSource, FragmentSource);
  tet_printf("Connect to new shader with same source code.\n");
  ret = uniformBlock.ConnectToShader(shader2);
  DALI_TEST_EQUALS(ret, true, TEST_LOCATION);

  tet_printf("connect empty shader handle will be failed\n");
  ret = uniformBlock.ConnectToShader(Dali::Shader());
  DALI_TEST_EQUALS(ret, false, TEST_LOCATION);

  tet_printf("disconnect from shader\n");
  uniformBlock.DisconnectFromShader(shader);
  uniformBlock.DisconnectFromShader(Dali::Shader());

  tet_printf("Connect to shader\n");
  ret = uniformBlock.ConnectToShader(shader);
  DALI_TEST_EQUALS(ret, true, TEST_LOCATION);

  application.SendNotification();
  application.Render(0);

  shader2.Reset();

  application.SendNotification();
  application.Render(0);

  END_TEST;
}

int UtcDaliUniformBlockConnectToShaderStrong(void)
{
  TestApplication application;

  Shader   shader   = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  UniformBlock uniformBlock1 = UniformBlock::New("testBlock1");
  UniformBlock uniformBlock2 = UniformBlock::New("testBlock2");

  tet_printf("Connect to shader as strong\n");
  bool ret = uniformBlock1.ConnectToShader(shader, true);
  DALI_TEST_EQUALS(ret, true, TEST_LOCATION);

  tet_printf("Re-connect to already connected uniform block will be failed\n");
  ret = uniformBlock1.ConnectToShader(shader);
  DALI_TEST_EQUALS(ret, false, TEST_LOCATION);

  tet_printf("Connect to shader as strong\n");
  ret = uniformBlock2.ConnectToShader(shader, true);
  DALI_TEST_EQUALS(ret, true, TEST_LOCATION);

  tet_printf("Re-connect to already connected uniform block will be failed\n");
  ret = uniformBlock2.ConnectToShader(shader, false);
  DALI_TEST_EQUALS(ret, false, TEST_LOCATION);

  tet_printf("Disconnect and then Re-connect as weak\n");
  uniformBlock2.DisconnectFromShader(shader);
  ret = uniformBlock2.ConnectToShader(shader, false);
  DALI_TEST_EQUALS(ret, true, TEST_LOCATION);

  struct TestObjectDestroyedCallback
  {
    TestObjectDestroyedCallback(bool& signalReceived, const Dali::RefObject*& objectPointer)
    : mSignalVerified(signalReceived),
      mObjectPointer(objectPointer)
    {
    }
    void operator()(const Dali::RefObject* objectPointer)
    {
      tet_infoline("Verifying TestObjectDestroyedCallback()");
      if(objectPointer == mObjectPointer)
      {
        mSignalVerified = true;
      }
    }
    bool&                   mSignalVerified;
    const Dali::RefObject*& mObjectPointer;
  };
  // Test whether ubo2 is destroyed and ubo1 is alive
  const Dali::RefObject* ubo1Impl      = uniformBlock1.GetObjectPtr();
  const Dali::RefObject* ubo2Impl      = uniformBlock2.GetObjectPtr();
  bool                   ubo1Destroyed = false;
  bool                   ubo2Destroyed = false;
  ObjectRegistry         registry      = application.GetCore().GetObjectRegistry();
  DALI_TEST_CHECK(registry);
  registry.ObjectDestroyedSignal().Connect(&application, TestObjectDestroyedCallback(ubo1Destroyed, ubo1Impl));
  registry.ObjectDestroyedSignal().Connect(&application, TestObjectDestroyedCallback(ubo2Destroyed, ubo2Impl));

  DALI_TEST_EQUALS(ubo1Destroyed, false, TEST_LOCATION);
  DALI_TEST_EQUALS(ubo2Destroyed, false, TEST_LOCATION);

  uniformBlock1.Reset();
  uniformBlock2.Reset();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // Now ubo2 is destroyed, but ubo1 still alive.
  DALI_TEST_EQUALS(ubo1Destroyed, false, TEST_LOCATION);
  DALI_TEST_EQUALS(ubo2Destroyed, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUniformBlockGetPropertyFromGraphics(void)
{
  TestApplication application;

  const std::string uniformBlockName("testBlock");
  const std::string uniformValue1Name("uValue1");
  const std::string uniformValue2Name("uValue2");

  // Values for actor
  const float   value1ForActor = 1.0f;
  const Vector2 value2ForActor(-2.0f, -3.0f);

  // Values for UniformBlock
  const float   value1ForUniformBlock = 10.0f;
  const Vector2 value2ForUniformBlock(20.0f, 30.0f);

  const uint32_t uniformAlign     = sizeof(float) * 4;
  const uint32_t uniformBlockSize = (uniformAlign) * 2;

  tet_infoline("Prepare graphics to check UTC for testBlock\n");
  auto& graphics = application.GetGraphicsController();
  auto& gl       = application.GetGlAbstraction();
  gl.mBufferTrace.EnableLogging(true);

  const uint32_t UNIFORM_BLOCK_ALIGNMENT(512);
  gl.SetUniformBufferOffsetAlignment(UNIFORM_BLOCK_ALIGNMENT);

  // Add custom uniform block
  TestGraphicsReflection::TestUniformBlockInfo block{
    uniformBlockName,
    0,
    0,
    uniformBlockSize,
    {
      {uniformValue1Name, Graphics::UniformClass::UNIFORM, 0, 0, {0}, {1}, 0, Property::Type::FLOAT, uniformAlign, 0},
      {uniformValue2Name, Graphics::UniformClass::UNIFORM, 0, 0, {uniformAlign}, {2}, 0, Property::Type::VECTOR2, uniformAlign, 0},
    }};
  graphics.AddCustomUniformBlock(block);
  tet_infoline("Prepare done\n");

  Shader   shader   = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  // Register a custom property
  actor.RegisterProperty(uniformValue1Name, value1ForActor);
  actor.RegisterProperty(uniformValue2Name, value2ForActor);

  UniformBlock uniformBlock = UniformBlock::New("testBlock");
  DALI_TEST_CHECK(uniformBlock);
  DALI_TEST_EQUALS(uniformBlock.ConnectToShader(shader), true, TEST_LOCATION);

  // Register a custom property
  uniformBlock.RegisterProperty(uniformValue1Name, value1ForUniformBlock);
  uniformBlock.RegisterProperty(uniformValue2Name, value2ForUniformBlock);

  // TODO : For now, we should connect to shader before first rendering.
  // We should resolve this bug in future.

  TraceCallStack& graphicsTrace = graphics.mCallStack;
  TraceCallStack& cmdTrace      = graphics.mCommandBufferCallStack;
  graphicsTrace.EnableLogging(true);
  cmdTrace.EnableLogging(true);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS(cmdTrace.CountMethod("BindUniformBuffers"), 1, TEST_LOCATION);
  DALI_TEST_CHECK(graphics.mLastUniformBinding.buffer != nullptr);
  DALI_TEST_CHECK(graphics.mLastUniformBinding.emulated == false);

  auto TestRawBuffer = [&](const float expectValue1, const Vector2& expectValue2)
  {
    DALI_TEST_CHECK(graphics.mLastUniformBinding.buffer != nullptr);
    DALI_TEST_CHECK(graphics.mLastUniformBinding.emulated == false);

    tet_printf("Expect value : %f, %fx%f\n", expectValue1, expectValue2.x, expectValue2.y);

    auto data = graphics.mLastUniformBinding.buffer->memory.data();
    data += graphics.mLastUniformBinding.offset;
    const float* fdata = reinterpret_cast<const float*>(data);
    for(uint32_t i = 0u; i < uniformBlockSize / sizeof(float); i++)
    {
      tet_printf("%f ", fdata[i]);
    }
    tet_printf("\n");

    DALI_TEST_EQUALS(fdata[0], expectValue1, TEST_LOCATION);
    DALI_TEST_EQUALS(fdata[(uniformAlign / sizeof(float))], expectValue2.x, TEST_LOCATION);
    DALI_TEST_EQUALS(fdata[(uniformAlign / sizeof(float)) + 1], expectValue2.y, TEST_LOCATION);
  };

  // Test the value
  {
    tet_printf("The result after connected!\n");
    TestRawBuffer(value1ForUniformBlock, value2ForUniformBlock);
  }

  uniformBlock.DisconnectFromShader(shader);

  application.SendNotification();
  application.Render(0);

  // Test the value
  {
    tet_printf("The result after disconnected!\n");
    TestRawBuffer(value1ForActor, value2ForActor);
  }

  uniformBlock.ConnectToShader(shader);

  application.SendNotification();
  application.Render(0);

  // Test the value
  {
    tet_printf("The result after connected again\n");
    TestRawBuffer(value1ForUniformBlock, value2ForUniformBlock);
  }
  actor.RegisterProperty(uniformValue1Name, value1ForActor * 3.0f);
  actor.RegisterProperty(uniformValue2Name, value2ForActor * 3.0f);

  application.SendNotification();
  application.Render(0);

  // Test the value
  {
    tet_printf("The result when we change actor property\n");
    TestRawBuffer(value1ForUniformBlock, value2ForUniformBlock);
  }

  uniformBlock.DisconnectFromShader(shader);

  application.SendNotification();
  application.Render(0);

  // Test the value
  {
    tet_printf("The result after disconnected after change actor property\n");
    TestRawBuffer(value1ForActor * 3.0f, value2ForActor * 3.0f);
  }

  uniformBlock.ConnectToShader(shader);

  application.SendNotification();
  application.Render(0);

  // Test the value
  {
    tet_printf("The result after connected again\n");
    TestRawBuffer(value1ForUniformBlock, value2ForUniformBlock);
  }

  uniformBlock.Reset();

  application.SendNotification();
  application.Render(0);

  // Test the value
  {
    tet_printf("Destroy uniform block without disconnect shader. It will disconnect from shader automatically\n");
    TestRawBuffer(value1ForActor * 3.0f, value2ForActor * 3.0f);
  }

  END_TEST;
}

int UtcDaliUniformBlockConstraint01(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform block property can be constrained");

  Shader   shader   = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  UniformBlock uniformBlock = UniformBlock::New("testBlock");
  uniformBlock.ConnectToShader(shader);

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = uniformBlock.RegisterProperty("uFadeColor", initialColor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(uniformBlock.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION);

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>(uniformBlock, colorIndex, TestConstraintNoBlue);
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS(uniformBlock.GetCurrentProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION);
  application.Render(0);
  DALI_TEST_EQUALS(uniformBlock.GetCurrentProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION);

  uniformBlock.RemoveConstraints();
  uniformBlock.SetProperty(colorIndex, Color::WHITE);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(uniformBlock.GetCurrentProperty<Vector4>(colorIndex), Color::WHITE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUniformBlockAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform block property can be animated");

  UniformBlock uniformBlock = UniformBlock::New("testBlock");

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = uniformBlock.RegisterProperty("uFadeColor", initialColor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(uniformBlock.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween(Property(uniformBlock, colorIndex), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS(uniformBlock.GetCurrentProperty<Vector4>(colorIndex), Color::WHITE * 0.5f, TEST_LOCATION);

  application.Render(500);

  DALI_TEST_EQUALS(uniformBlock.GetCurrentProperty<Vector4>(colorIndex), Color::TRANSPARENT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUniformBlockDestructWorkerThreadN(void)
{
  TestApplication application;
  tet_infoline("UtcDaliUniformBlockDestructWorkerThreadN Test, for line coverage");

  try
  {
    class TestThread : public Thread
    {
    public:
      virtual void Run()
      {
        tet_printf("Run TestThread\n");
        // Destruct at worker thread.
        mUniformBlock.Reset();
      }

      Dali::UniformBlock mUniformBlock;
    };
    TestThread thread;

    Dali::UniformBlock uniformBlock = Dali::UniformBlock::New("testBlock");
    thread.mUniformBlock            = std::move(uniformBlock);
    uniformBlock.Reset();

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

int utcDaliUniformBlockPartialUpdate(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged rect when UniformBlock's property changed");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  UniformBlock uniformBlock = UniformBlock::New("testBlock");
  uniformBlock.ConnectToShader(shader);

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = uniformBlock.RegisterProperty("uFadeColor", initialColor);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;

  // 1. Actor added, damaged rect is added size of actor
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates

  DirtyRectChecker(damagedRects, {clippingRect}, true, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  // Check dirty rect empty
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  clippingRect = TestApplication::DEFAULT_SURFACE_RECT;
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Change uniform block property
  uniformBlock.SetProperty(colorIndex, Color::RED);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates

  DirtyRectChecker(damagedRects, {clippingRect}, true, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  END_TEST;
}
