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
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/dali-core.h>
#include <mesh-builder.h>
#include <stdlib.h>
#include <test-actor-utils.h>
#include <test-native-image.h>

#include <iostream>
#include "test-application.h"

#define BOOLSTR(x) ((x) ? "T" : "F")

//& set: DaliRenderTask

using namespace Dali;

void utc_dali_render_task_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_render_task_cleanup(void)
{
  test_return_value = TET_PASS;
}

/**
 * APIs:
 *
 * Constructor, Destructor, DownCast, New, copy constructor, assignment operator
 *
 * SetSourceActor                      2+ve, 1-ve
 * GetSourceActor                      1+ve, 1-ve
 * SetExclusive                        2+ve, 0-ve
 * IsExclusive                         2+ve, 0-ve
 * SetInputEnabled                     1+ve, 0-ve
 * GetInputEnabled                     1+ve, 0-ve
 * SetCameraActor                      1+ve, 1-ve
 * GetCameraActor                      1+ve, 1-ve
 * SetTargetFrameBuffer                1+ve, 1-ve
 * GetTargetFrameBuffer                1+ve, 1-ve
 * SetScreenToFrameBufferFunction      1+ve, 1-ve
 * GetScreenToFrameBufferFunction      1+ve, 1-ve
 * SetScreenToFrameBufferMappingActor  1+ve, 1-ve
 * GetScreenToFrameBufferMappingActor  1+ve, 1-ve
 * SetViewportPosition                 1+ve
 * GetCurrentViewportPosition          1+ve
 * SetViewportSize                     1+ve
 * GetCurrentViewportSize              1+ve
 * SetViewport                         2+ve, 1-ve
 * GetViewport                         2+ve, 1-ve
 * SetClearColor                       1+ve, 1-ve
 * GetClearColor                       1+ve, 1-ve
 * SetClearEnabled                     1+ve, 1-ve
 * GetClearEnabled                     1+ve, 1-ve
 * SetCullMode
 * GetCullMode
 * SetRefreshRate                      Many
 * GetRefreshRate                      1+ve
 * FinishedSignal                      1+ve
 */

namespace // unnamed namespace
{
const int RENDER_FRAME_INTERVAL = 16; ///< Duration of each frame in ms. (at approx 60FPS)

// Test shader codes
const std::string_view SHADER_COLOR_TEST_SHADER_VERT1{
  R"(INPUT mediump vec2 aPosition;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
uniform mediump vec2 extraSize;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw ) + extraSize;
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);
  mediump vec2 vPosition = aPosition * visualSize;
  return vec4(vPosition + anchorPoint * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0);
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}
)"};

// Test shader codes
const std::string_view SHADER_COLOR_TEST_SHADER_VERT2{
  R"(INPUT mediump vec2 aPosition;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
uniform mediump vec2 extraSize;

vec4 ComputeVertexPosition2()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw ) + extraSize;
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);
  mediump vec2 vPosition = aPosition * visualSize;
  return vec4(vPosition + anchorPoint * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0);
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition2();
}
)"};

const std::string_view SHADER_COLOR_TEST_SHADER_FRAG{
  R"(
void main()
{
  OUT_COLOR = vec4(0.0, 0.0, 1.0, 1.0);
}
)"};

/*
 * Simulate time passed by.
 *
 * @note this will always process at least 1 frame (1/60 sec)
 *
 * @param application Test application instance
 * @param duration Time to pass in milliseconds.
 * @return The actual time passed in milliseconds
 */
int Wait(TestApplication& application, int duration = 0)
{
  int time = 0;

  for(int i = 0; i <= (duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
}

struct RenderTaskFinished
{
  RenderTaskFinished(bool& finished)
  : finished(finished)
  {
  }

  void operator()(RenderTask& renderTask)
  {
    finished = true;
  }

  bool& finished;
};

struct RenderTaskFinishedRemoveSource
{
  RenderTaskFinishedRemoveSource(bool& finished)
  : finished(finished),
    finishedOnce(false)
  {
  }

  void operator()(RenderTask& renderTask)
  {
    DALI_TEST_CHECK(finishedOnce == false);
    finished       = true;
    finishedOnce   = true;
    Actor srcActor = renderTask.GetSourceActor();
    UnparentAndReset(srcActor);
  }

  bool& finished;
  bool  finishedOnce;
};

struct RenderTaskFinishedRenderAgain
{
  RenderTaskFinishedRenderAgain(bool& finished)
  : finished(finished),
    finishedOnce(false)
  {
  }

  void operator()(RenderTask& renderTask)
  {
    DALI_TEST_CHECK(finishedOnce == false);
    finished     = true;
    finishedOnce = true;
    renderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  }

  bool& finished;
  bool  finishedOnce;
};

bool TestScreenToFrameBufferFunction(Vector2& coordinates)
{
  coordinates = coordinates + Vector2(1, 2);

  return true;
}

Actor CreateRenderableActorSuccess(TestApplication& application, std::string filename)
{
  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(80.0f, 80.0f));
  return actor;
}

Texture CreateTexture(void)
{
  return Dali::CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 80, 80);
}

RenderTask CreateRenderTask(TestApplication& application,
                            CameraActor      offscreenCamera,
                            Actor            rootActor,       // Reset default render task to point at this actor
                            Actor            secondRootActor, // Source actor
                            unsigned int     refreshRate,
                            bool             glSync,
                            uint32_t         frameBufferWidth  = 10,
                            uint32_t         frameBufferHeight = 10)
{
  // Change main render task to use a different root
  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  taskList.GetTask(0u).SetSourceActor(rootActor);

  FrameBuffer frameBuffer = FrameBuffer::New(frameBufferWidth, frameBufferHeight);
  if(glSync)
  {
    NativeImageInterfacePtr testNativeImagePtr = TestNativeImage::New(frameBufferWidth, frameBufferHeight);
    Texture                 texture            = Texture::New(*testNativeImagePtr);
    frameBuffer.AttachColorTexture(texture);
  }

  // Don't draw output framebuffer // '

  RenderTask newTask = taskList.CreateTask();
  newTask.SetCameraActor(offscreenCamera);
  newTask.SetSourceActor(secondRootActor);
  newTask.SetInputEnabled(false);
  newTask.SetClearColor(Vector4(0.f, 0.f, 0.f, 0.f));
  newTask.SetClearEnabled(true);
  newTask.SetExclusive(true);
  newTask.SetRefreshRate(refreshRate);
  newTask.SetFrameBuffer(frameBuffer);
  newTask.SetProperty(RenderTask::Property::REQUIRES_SYNC, glSync);
  return newTask;
}

bool UpdateRender(TestApplication& application, TraceCallStack& callStack, bool testDrawn, bool& finishedSig, bool testFinished, bool testKeepUpdating, int lineNumber)
{
  finishedSig = false;
  callStack.Reset();

  tet_printf("TestApplication::UpdateRender().\n");

  application.Render(16);
  application.SendNotification();

  bool sigPassed = false;
  if(testFinished)
  {
    sigPassed = finishedSig;
  }
  else
  {
    sigPassed = !finishedSig;
  }

  bool drawResult = callStack.FindMethod("DrawElements") || callStack.FindMethod("DrawArrays");

  bool drawPassed = false;
  if(testDrawn)
  {
    drawPassed = drawResult;
  }
  else
  {
    drawPassed = !drawResult;
  }

  bool keepUpdating       = (application.GetUpdateStatus() != 0);
  bool keepUpdatingPassed = false;
  if(testKeepUpdating)
  {
    keepUpdatingPassed = keepUpdating;
  }
  else
  {
    keepUpdatingPassed = !keepUpdating;
  }

  bool result = (sigPassed && drawPassed && keepUpdatingPassed);

  tet_printf("UpdateRender: Expected: Draw:%s Signal:%s KeepUpdating: %s  Actual: Draw:%s  Signal:%s KeepUpdating: %s  %s, line %d\n",
             BOOLSTR(testDrawn),
             BOOLSTR(testFinished),
             BOOLSTR(testKeepUpdating),
             BOOLSTR(drawResult),
             BOOLSTR(finishedSig),
             BOOLSTR(keepUpdating),
             result ? "Passed" : "Failed",
             lineNumber);

  return result;
}

} // unnamed namespace

/****************************************************************************************************/
/****************************************************************************************************/
/********************************   TEST CASES BELOW   **********************************************/
/****************************************************************************************************/
/****************************************************************************************************/

int UtcDaliRenderTaskDownCast01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::DownCast()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  BaseHandle base = taskList.GetTask(0u);
  DALI_TEST_CHECK(base);

  RenderTask task = RenderTask::DownCast(base);
  DALI_TEST_CHECK(task);

  // Try calling a method
  DALI_TEST_CHECK(task.GetSourceActor());
  END_TEST;
}

int UtcDaliRenderTaskDownCast02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::DownCast()");

  Actor actor = Actor::New();

  RenderTask task = RenderTask::DownCast(actor);
  DALI_TEST_CHECK(!task);

  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorN(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::SetSourceActor() Negative - try with empty actor handle");
  Integration::Scene stage = application.GetScene();

  Actor srcActor;

  RenderTaskList taskList   = stage.GetRenderTaskList();
  RenderTask     renderTask = taskList.CreateTask();
  renderTask.SetSourceActor(srcActor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!renderTask.GetSourceActor());
  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorP01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor() Positive - check that setting a non-renderable actor stops existing source actor being rendered ");

  Integration::Scene stage    = application.GetScene();
  RenderTaskList     taskList = stage.GetRenderTaskList();
  RenderTask         task     = taskList.GetTask(0u);

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK(actor);

  Texture img      = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);
  Actor   newActor = CreateRenderableActor(img);
  newActor.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  stage.Add(newActor);

  Actor nonRenderableActor = Actor::New();
  stage.Add(nonRenderableActor);

  // Stop the newActor from being rendered by changing the source actor
  DALI_TEST_CHECK(nonRenderableActor);
  task.SetSourceActor(nonRenderableActor);
  DALI_TEST_CHECK(task.GetSourceActor() != actor);
  DALI_TEST_CHECK(task.GetSourceActor() == nonRenderableActor);

  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render nothing!
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that nothing was rendered
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorP02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor() Positive - check that switching source from a non-renderable to a renderable actor causes the texture to be drawn");

  Integration::Scene stage = application.GetScene();

  RenderTaskList taskList = stage.GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK(actor);

  Texture img      = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);
  Actor   newActor = CreateRenderableActor(img);
  newActor.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  stage.Add(newActor);

  Actor nonRenderableActor = Actor::New();
  stage.Add(nonRenderableActor);

  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Stop the newActor from being rendered by changing the source actor
  DALI_TEST_CHECK(nonRenderableActor);
  task.SetSourceActor(nonRenderableActor);
  DALI_TEST_CHECK(task.GetSourceActor() != actor);
  DALI_TEST_CHECK(task.GetSourceActor() == nonRenderableActor);

  // Update & Render nothing!
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that nothing was rendered
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION);
  drawTrace.Reset();

  // Set newActor as the new source Actor
  task.SetSourceActor(newActor);
  DALI_TEST_CHECK(task.GetSourceActor() != actor);
  DALI_TEST_CHECK(task.GetSourceActor() == newActor);

  // Update & Render the newActor
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the newActor was rendered
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorOffScene(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor (on/off stage testing)");

  Integration::Scene stage    = application.GetScene();
  RenderTaskList     taskList = stage.GetRenderTaskList();
  RenderTask         task     = taskList.GetTask(0u);

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK(actor);

  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  Texture img      = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);
  Actor   newActor = CreateRenderableActor(img);
  newActor.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  task.SetSourceActor(newActor);
  // Don't add newActor to stage yet   //'

  // Update & Render with the actor initially off-stage
  application.SendNotification();
  application.Render();

  // Check that nothing was rendered
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION);

  // Now add to stage
  stage.Add(newActor);

  // Update & Render with the actor on-stage
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the newActor was rendered
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);
  drawTrace.Reset();

  // Now remove from stage
  stage.Remove(newActor);

  // Update & Render with the actor off-stage
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorEmpty(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor (empty handle case)");

  Integration::Scene stage    = application.GetScene();
  RenderTaskList     taskList = stage.GetRenderTaskList();
  RenderTask         task     = taskList.GetTask(0u);

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK(actor);

  Texture img      = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);
  Actor   newActor = CreateRenderableActor(img);
  newActor.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  stage.Add(newActor);

  Actor nonRenderableActor = Actor::New();
  stage.Add(nonRenderableActor);

  // Set with empty handle
  task.SetSourceActor(Actor());
  DALI_TEST_CHECK(!task.GetSourceActor());

  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render nothing!
  application.SendNotification();
  application.Render();

  // Check that nothing was rendered
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION);

  // Set with non-empty handle
  task.SetSourceActor(newActor);
  DALI_TEST_CHECK(task.GetSourceActor() == newActor);

  // Update & Render the newActor
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the newActor was rendered
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorDestroyed(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor - Set a source actor and destroy the source actor");

  Integration::Scene stage    = application.GetScene();
  RenderTaskList     taskList = stage.GetRenderTaskList();
  RenderTask         task     = taskList.GetTask(0u);

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK(actor);

  Texture img = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);

  Actor newActor = CreateRenderableActor(img);
  newActor.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  stage.Add(newActor);

  task.SetSourceActor(newActor);

  DALI_TEST_CHECK(task.GetSourceActor() != actor);
  DALI_TEST_CHECK(task.GetSourceActor() == newActor);

  application.SendNotification();
  application.Render();

  // Destroy the source actor
  stage.Remove(newActor);
  newActor.Reset();

  DALI_TEST_CHECK(!task.GetSourceActor()); // The source actor should be an empty handle.

  END_TEST;
}

int UtcDaliRenderTaskGetSourceActorP01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetSourceActor() Check the default render task has a valid source actor");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK(actor);

  // By default the entire scene should be rendered
  Actor root = application.GetScene().GetLayer(0);
  DALI_TEST_CHECK(root == actor);
  END_TEST;
}

int UtcDaliRenderTaskGetSourceActorP02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetSourceActor() Create a new render task, Add a new actor to the stage and set it as the source of the new render task. Get its source actor and check that it is equivalent to what was set.");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.CreateTask();
  Actor          actor    = Actor::New();
  application.GetScene().Add(actor);
  task.SetSourceActor(actor);

  DALI_TEST_EQUALS(actor, task.GetSourceActor(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskGetSourceActorN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetSourceActor() Try with empty handle");

  RenderTask task;
  try
  {
    Actor actor = task.GetSourceActor();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRenderTaskGetStopperActorP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetStopperActor() Create a new render task, Add a new actor to the stage and set RenderTask::RenderUntil(actor). Get its stopper actor and check it is equivalent to what was set.");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.CreateTask();
  Actor          actor    = Actor::New();
  application.GetScene().Add(actor);
  task.RenderUntil(actor);

  DALI_TEST_EQUALS(actor, task.GetStopperActor(), TEST_LOCATION);

  // Cancel render until, by set empty handle
  task.RenderUntil(Dali::Actor());

  DALI_TEST_EQUALS(Dali::Actor(), task.GetStopperActor(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskGetStopperActorN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetStopperActor() Try with empty handle");

  RenderTask task;

  try
  {
    Actor actor = task.GetStopperActor();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRenderTaskRenderUntil01(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::RenderUntil(actor) Check that rendering stops at the renderable actor.");

  // Get default rendertask
  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);

  Integration::Scene stage = application.GetScene();

  Actor secondChild;
  for(int i = 0; i < 5; i++)
  {
    Actor parent = CreateRenderableActor();
    parent.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
    parent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    Actor child = CreateRenderableActor();
    child.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

    stage.Add(parent);
    parent.Add(child);

    if(i == 1)
    {
      secondChild = child;
    }
  }
  task.RenderUntil(secondChild);

  // Update & Render with the actor on-stage
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut.
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 3, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskRenderUntil02(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::RenderUntil(actor) Check that rendering stops at the empty actor.");

  // Get default rendertask
  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);

  Integration::Scene stage = application.GetScene();

  Actor secondChild;
  for(int i = 0; i < 5; i++)
  {
    Actor parent = CreateRenderableActor();
    parent.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
    parent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    Actor child = Actor::New(); // Has no renderer
    child.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

    stage.Add(parent);
    parent.Add(child);

    if(i == 1)
    {
      secondChild = child;
    }
  }
  task.RenderUntil(secondChild);

  // Update & Render with the actor on-stage
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut.
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskRenderUntil03(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::RenderUntil(actor) Check that other preceding layers are rendered.");

  // Get default rendertask
  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);

  Integration::Scene stage = application.GetScene();

  auto CreateRenderableActorWithName = [](const char* name) -> Actor
  {
    Actor actor = CreateRenderableActor();
    actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    actor.SetProperty(Actor::Property::NAME, name);

    return actor;
  };

  // Compose a tree
  //
  // stage (depth = 0)
  // |- a0 (renderable)
  //     |- l0 (depth = 1)
  //         |- (renderable)
  //         |- (renderable)
  // |- a1 (renderable)
  //     |- target (stopper node)
  // |- a2 (renderable)
  //     |- l2  (depth = 2)
  //         |- (renderable)
  //         |- (renderable)
  //         |- (renderable)
  Actor a0 = CreateRenderableActorWithName("a0");

  Layer l0 = Layer::New();
  l0.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  l0.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  Actor a1 = CreateRenderableActorWithName("a1");

  Actor target = CreateRenderableActorWithName("target");

  Layer l2 = Layer::New();
  l2.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  l2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  Actor a2 = CreateRenderableActorWithName("a2");

  stage.Add(a0);
  a0.Add(l0);
  l0.Add(CreateRenderableActorWithName("l0-c0"));
  l0.Add(CreateRenderableActorWithName("l0-c1"));

  stage.Add(a1);
  a1.Add(target);

  stage.Add(a2);
  a2.Add(l2);
  l2.Add(CreateRenderableActorWithName("l2-c0"));
  l2.Add(CreateRenderableActorWithName("l2-c1"));
  l2.Add(CreateRenderableActorWithName("l2-c2"));

  // draw only a0 and a1 (2 items)
  // l0, l2 and children is cut(high depth index than stage)
  // a2 and children are cut(added after target)
  task.RenderUntil(target);

  // Update & Render with the actor on-stage
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION);

  drawTrace.Reset();

  // Move l2 lower than stage.
  //
  // stage (depth = 1)
  // |- a0 (renderable)
  //     |- l0 (depth = 2)
  //         |- (renderable)
  //         |- (renderable)
  // |- a1 (renderable)
  //     |- target (stopper node)
  // |- a2 (renderable)
  //     |- l2  (depth = 0)
  //         |- (renderable)
  //         |- (renderable)
  //         |- (renderable)
  l2.LowerToBottom();

  // After now, draw a0, a1, and child of l2 (2 + 3 items)
  // l0 and children is cut(high depth index than stage)
  // a2 and children are cut(added after target)

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 5, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskRenderUntil04(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::RenderUntil(actor) Check that other preceding layers are rendered, for more complex cases");

  // Get default rendertask
  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);

  Integration::Scene stage = application.GetScene();

  auto CreateRenderableActorWithName = [](const char* name) -> Actor
  {
    Actor actor = CreateRenderableActor();
    actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    actor.SetProperty(Actor::Property::NAME, name);

    return actor;
  };

  // Compose a tree
  //
  // stage (depth = 0)
  // |- a0 (renderable)
  //     |- l0 (depth = 1)
  //         |- (renderable)
  //         |- (renderable)
  // |- a1 (renderable)
  //     |- l1 (depth = 2)
  //         |- b0 (renderable)
  //         |- target (stopper node)
  //         |- b1 (renderable)
  // |- a2 (renderable)
  //     |- l2  (depth = 3)
  //         |- (renderable)
  //         |- (renderable)
  //         |- (renderable)
  Actor a0 = CreateRenderableActorWithName("a0");

  Layer l0 = Layer::New();
  l0.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  l0.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  Actor a1 = CreateRenderableActorWithName("a1");

  Layer l1 = Layer::New();
  l1.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  l1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  Actor b0 = CreateRenderableActorWithName("b0");

  Actor target = CreateRenderableActorWithName("target");

  Actor b1 = CreateRenderableActorWithName("b1");

  Layer l2 = Layer::New();
  l2.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  l2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  Actor a2 = CreateRenderableActorWithName("a2");

  stage.Add(a0);
  a0.Add(l0);
  l0.Add(CreateRenderableActorWithName("l0-c0"));
  l0.Add(CreateRenderableActorWithName("l0-c1"));

  stage.Add(a1);
  a1.Add(l1);
  l1.Add(b0);
  l1.Add(target);
  l1.Add(b1);

  stage.Add(a2);
  a2.Add(l2);
  l2.Add(CreateRenderableActorWithName("l2-c0"));
  l2.Add(CreateRenderableActorWithName("l2-c1"));
  l2.Add(CreateRenderableActorWithName("l2-c2"));

  // draw a0, a1, a2, and l0, and b0 (3 + 2 + 1 items)
  // l2 and children is cut(high depth index than l1)
  // b1 is cut out (added after target)
  task.RenderUntil(target);

  // Update & Render with the actor on-stage
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 6, TEST_LOCATION);

  drawTrace.Reset();

  // Move l1 lower than stage.
  //
  // stage (depth = 1)
  // |- a0 (renderable)
  //     |- l0 (depth = 2)
  //         |- (renderable)
  //         |- (renderable)
  // |- a1 (renderable)
  //     |- l1 (depth = 0)
  //         |- b0 (renderable)
  //         |- target (stopper node)
  //         |- b1 (renderable)
  // |- a2 (renderable)
  //     |- l2  (depth = 3)
  //         |- (renderable)
  //         |- (renderable)
  //         |- (renderable)
  l1.LowerToBottom();

  // After now, draw b0 only.
  // root layer, l1, l2 and children is cut(high depth index than l1, what target hold)
  // b1 is cut out (added after target)

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  drawTrace.Reset();

  // Move l2 lower than stage.
  //
  // stage (depth = 2)
  // |- a0 (renderable)
  //     |- l0 (depth = 3)
  //         |- (renderable)
  //         |- (renderable)
  // |- a1 (renderable)
  //     |- l1 (depth = 1)
  //         |- b0 (renderable)
  //         |- target (stopper node)
  //         |- b1 (renderable)
  // |- a2 (renderable)
  //     |- l2  (depth = 0)
  //         |- (renderable)
  //         |- (renderable)
  //         |- (renderable)
  l2.LowerToBottom();

  // After now, draw b0 and l2 (1 + 3 items).
  // root layer, l1 and children is cut(high depth index than l1, what target hold)
  // b1 is cut out (added after target)

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 4, TEST_LOCATION);

  drawTrace.Reset();

  // Make b0 and a2 draw mode as overlay
  //
  // stage (depth = 2)
  // |- a0 (renderable)
  //     |- l0 (depth = 3)
  //         |- (renderable)
  //         |- (renderable)
  // |- a1 (renderable)
  //     |- l1 (depth = 1)
  //         |- b0 (renderable) (overlay)
  //         |- target (stopper node)
  //         |- b1 (renderable)
  // |- a2 (renderable) (overlay)
  //     |- l2  (depth = 0)
  //         |- (renderable) (overlay)
  //         |- (renderable)
  //         |- (renderable)
  b0.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  a2.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  l2.GetChildAt(0).SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);

  // After now, draw l2 (3 items).
  // root layer, l1 and children is cut(high depth index than l1, what target hold)
  // b1 is cut out (added after target)
  // b0 is cut out (overlay mode)

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 3, TEST_LOCATION);

  drawTrace.Reset();

  // Make target draw mode as overlay
  //
  // stage (depth = 2)
  // |- a0 (renderable)
  //     |- l0 (depth = 3)
  //         |- (renderable)
  //         |- (renderable)
  // |- a1 (renderable)
  //     |- l1 (depth = 1)
  //         |- b0 (renderable) (overlay)
  //         |- target (stopper node) (overlay)
  //         |- b1 (renderable)
  // |- a2 (renderable) (overlay)
  //     |- l2  (depth = 0)
  //         |- (renderable) (overlay)
  //         |- (renderable)
  //         |- (renderable)
  target.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);

  // After now, draw b0, b1, and l2 (2 + 3 items).
  // root layer, l1 and children is cut(high depth index than l1, what target hold)
  // b1 is not be cut out due to target is overlay mode

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 5, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskRenderUntil05(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::RenderUntil(actor) Check that stopper actor scene off and scene on again");

  // Get default rendertask
  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);

  Integration::Scene stage = application.GetScene();

  auto CreateRenderableActorWithName = [](const char* name) -> Actor
  {
    Actor actor = CreateRenderableActor();
    actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    actor.SetProperty(Actor::Property::NAME, name);

    return actor;
  };

  // Compose a tree
  //
  // stage (depth = 0)
  // |- a0 (renderable)
  // |- a1 (renderable)
  //     |- target (stopper node)
  // |- a2 (renderable)
  Actor a0     = CreateRenderableActorWithName("a0");
  Actor a1     = CreateRenderableActorWithName("a1");
  Actor a2     = CreateRenderableActorWithName("a2");
  Actor target = CreateRenderableActorWithName("target");

  stage.Add(a0);
  stage.Add(a1);
  stage.Add(a2);
  a1.Add(target);

  // draw a0, a1 (2 items)
  task.RenderUntil(target);

  // Update & Render with the actor on-stage
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION);

  drawTrace.Reset();

  // Unparent target
  //
  // stage (depth = 0)
  // |- a0 (renderable)
  // |- a1 (renderable)
  // |- a2 (renderable)
  target.Unparent();

  // After now, draw all actors, a0, a1, and a2.

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 3, TEST_LOCATION);

  drawTrace.Reset();

  // Add target under a0.
  //
  // stage (depth = 0)
  // |- a0 (renderable)
  //     |- target (stopper node)
  // |- a1 (renderable)
  // |- a2 (renderable)
  a0.Add(target);

  // After now, draw a0 only.

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  drawTrace.Reset();

  // Cancel RenderUntil.
  //
  // stage (depth = 0)
  // |- a0 (renderable)
  //     |- target (renderable)
  // |- a1 (renderable)
  // |- a2 (renderable)
  task.RenderUntil(Dali::Actor());

  // After now, draw all actors (4 items).

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskRenderUntil06(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::RenderUntil(actor) Check that stopper actor scene off and destroyed");

  // Get default rendertask
  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);

  Integration::Scene stage = application.GetScene();

  auto CreateRenderableActorWithName = [](const char* name) -> Actor
  {
    Actor actor = CreateRenderableActor();
    actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    actor.SetProperty(Actor::Property::NAME, name);

    return actor;
  };

  // Compose a tree
  //
  // stage (depth = 0)
  // |- a0 (renderable)
  // |- a1 (renderable)
  //     |- target (stopper node)
  // |- a2 (renderable)
  Actor a0     = CreateRenderableActorWithName("a0");
  Actor a1     = CreateRenderableActorWithName("a1");
  Actor a2     = CreateRenderableActorWithName("a2");
  Actor target = CreateRenderableActorWithName("target");

  stage.Add(a0);
  stage.Add(a1);
  stage.Add(a2);
  a1.Add(target);

  // draw a0, a1 (2 items)
  task.RenderUntil(target);

  // Update & Render with the actor on-stage
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION);

  drawTrace.Reset();

  // Destroy target
  //
  // stage (depth = 0)
  // |- a0 (renderable)
  // |- a1 (renderable)
  // |- a2 (renderable)
  target.Unparent();
  target.Reset();

  DALI_TEST_EQUALS(task.GetStopperActor(), Actor(), TEST_LOCATION);

  // After now, draw all actors, a0, a1, and a2.

  // Update & Render
  application.SendNotification();
  application.Render();

  // Check that rendering was cut
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 3, TEST_LOCATION);

  drawTrace.Reset();

  // To make ensure the destroyed target has same pointer with new one, try this test multiple times
  for(int tryCount = 0; tryCount < 50; tryCount++)
  {
    // Create new target, which might have same pointer with previous one.
    target = CreateRenderableActorWithName("target");

    // Add target under a0.
    //
    // stage (depth = 0)
    // |- a0 (renderable)
    //     |- target (renderable)
    // |- a1 (renderable)
    // |- a2 (renderable)
    a0.Add(target);

    // After now, draw all actors (4 items).

    // Update & Render
    application.SendNotification();
    application.Render();

    // Check that rendering was cut
    DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 4, TEST_LOCATION);

    drawTrace.Reset();
    target.Unparent();
  }

  END_TEST;
}

int UtcDaliRenderTaskSetExclusive(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetExclusive() Check that exclusion works");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  // Creates:
  //           Root
  //  Actor1   Layer    Layer
  //           Actor2  Actor3

  // Task 1 is the default render task, should render from Root, incl Actor2
  // Task 2 uses Actor2 as a source actor (texture id 9)

  // Manipulate the GenTextures behaviour, to identify different actors

  std::vector<GLuint> ids;
  ids.push_back(8);  // 8 = actor1
  ids.push_back(9);  // 9 = actor2
  ids.push_back(10); // 10 = actor3
  application.GetGlAbstraction().SetNextTextureIds(ids);

  Texture img1   = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);
  Actor   actor1 = CreateRenderableActor(img1);
  actor1.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  application.GetScene().Add(actor1);

  // Update & Render actor1
  application.SendNotification();
  application.Render();

  // Check that the actor1 was rendered
  const std::vector<GLuint>& boundTextures = application.GetGlAbstraction().GetBoundTextures(GL_TEXTURE0);
  DALI_TEST_GREATER(boundTextures.size(), static_cast<std::vector<GLuint>::size_type>(0), TEST_LOCATION);

  if(boundTextures.size())
  {
    int a = boundTextures.size() - 1;
    DALI_TEST_EQUALS(boundTextures[a], 8u /*unique to actor1*/, TEST_LOCATION);
  }

  Texture img2 = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);

  Actor actor2 = CreateRenderableActor(img2);
  actor2.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  // Force actor2 to be rendered before actor1
  Layer layer = Layer::New();
  application.GetScene().Add(layer);
  layer.Add(actor2);
  layer.LowerToBottom();

  // Update & Render
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the actors were rendered
  DALI_TEST_GREATER(boundTextures.size(), static_cast<std::vector<GLuint>::size_type>(1), TEST_LOCATION);

  if(boundTextures.size() >= 2)
  {
    int a = boundTextures.size() - 2;
    int b = boundTextures.size() - 1;
    DALI_TEST_EQUALS(boundTextures[a], 9u /*unique to actor2*/, TEST_LOCATION);
    DALI_TEST_EQUALS(boundTextures[b], 8u /*unique to actor1*/, TEST_LOCATION);
  }

  Texture img3   = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);
  Actor   actor3 = CreateRenderableActor(img3);
  actor3.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));

  // Force actor3 to be rendered before actor2
  layer = Layer::New();
  application.GetScene().Add(layer);
  layer.Add(actor3);
  layer.LowerToBottom();

  // Update & Render all actors
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the actors were rendered
  DALI_TEST_GREATER(boundTextures.size(), static_cast<std::vector<GLuint>::size_type>(2), TEST_LOCATION);

  if(boundTextures.size() >= 3)
  {
    int a = boundTextures.size() - 3;
    int b = boundTextures.size() - 2;
    int c = boundTextures.size() - 1;
    DALI_TEST_EQUALS(boundTextures[a], 10u /*unique to actor3*/, TEST_LOCATION);
    DALI_TEST_EQUALS(boundTextures[b], 9u /*unique to actor2*/, TEST_LOCATION);
    DALI_TEST_EQUALS(boundTextures[c], 8u /*unique to actor1*/, TEST_LOCATION);
  }

  // Both actors are now connected to the root node
  // Setup 2 render-tasks - the first will render from the root-node, and the second from actor2

  // Not exclusive is the default
  RenderTask task1 = taskList.GetTask(0u);
  DALI_TEST_CHECK(false == task1.IsExclusive());

  RenderTask task2 = taskList.CreateTask();
  DALI_TEST_CHECK(false == task2.IsExclusive());
  task2.SetSourceActor(actor2);

  // Task1 should render all actors, and task 2 should render only actor2

  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  if(boundTextures.size() >= 4)
  {
    // Test that task 1 renders actor3, then actor2 & then actor1
    int a = boundTextures.size() - 4;
    int b = boundTextures.size() - 3;
    int c = boundTextures.size() - 2;
    int d = boundTextures.size() - 1;
    DALI_TEST_EQUALS(boundTextures[a], 10u /*unique to actor3*/, TEST_LOCATION);
    DALI_TEST_EQUALS(boundTextures[b], 9u /*unique to actor2*/, TEST_LOCATION);
    DALI_TEST_EQUALS(boundTextures[c], 8u /*unique to actor1*/, TEST_LOCATION);

    // Test that task 2 renders actor2
    DALI_TEST_EQUALS(boundTextures[d], 9u, TEST_LOCATION);
  }

  // Make actor2 exclusive to task2

  task2.SetExclusive(true);
  DALI_TEST_CHECK(true == task2.IsExclusive());

  // Task1 should render only actor1, and task 2 should render only actor2

  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(boundTextures.size(), 3u, TEST_LOCATION);
  if(boundTextures.size() == 3)
  {
    // Test that task 1 renders actor3 & actor1
    DALI_TEST_CHECK(boundTextures[0] == 10u);
    DALI_TEST_CHECK(boundTextures[1] == 8u);

    // Test that task 2 renders actor2
    DALI_TEST_CHECK(boundTextures[2] == 9u);
  }

  // Create a renderable actor and replace the source actor in task2
  auto actor4 = CreateRenderableActor();
  task2.SetSourceActor(actor3);
  DALI_TEST_EQUALS(actor3, task2.GetSourceActor(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetExclusive02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetExclusive() Check that changing from exclusive to not-exclusive works");

  std::vector<GLuint> ids;
  ids.push_back(8); // 8 = actor1
  application.GetGlAbstraction().SetNextTextureIds(ids);

  Texture img1   = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);
  Actor   actor1 = CreateRenderableActor(img1);
  actor1.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  application.GetScene().Add(actor1);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.CreateTask();

  task.SetSourceActor(actor1);
  task.SetExclusive(true); // Actor should only render once

  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render actor1
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  // Set task to non-exclusive - actor1 should render twice:
  drawTrace.Reset();
  task.SetExclusive(false);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetExclusive03(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetExclusive() Check that changing from exclusive to not-exclusive works");

  std::vector<GLuint> ids;
  ids.push_back(8); // 8 = actor1
  application.GetGlAbstraction().SetNextTextureIds(ids);

  Texture img1   = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);
  Actor   actor1 = CreateRenderableActor(img1);
  actor1.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  application.GetScene().Add(actor1);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.CreateTask();

  task.SetSourceActor(actor1);
  task.SetExclusive(true); // Actor should only render once

  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render actor1
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  // Set task to non-exclusive - actor1 should render twice:
  drawTrace.Reset();

  RenderTask task2 = taskList.CreateTask();
  task2.SetSourceActor(actor1);
  task2.SetExclusive(true); // Actor should only render once

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION);

  // Set task to non-exclusive - actor1 should render twice:
  drawTrace.Reset();
  task.SetExclusive(false);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  // Set task to non-exclusive - actor1 should render twice:
  drawTrace.Reset();
  task2.SetExclusive(false);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 3, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetExclusiveN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetExclusive() on empty handle");

  RenderTask task;
  try
  {
    task.SetExclusive(true);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskIsExclusive01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::IsExclusive() Check default values are non-exclusive");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  // Not exclusive is the default
  RenderTask task = taskList.GetTask(0u);
  DALI_TEST_CHECK(false == task.IsExclusive());

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK(false == newTask.IsExclusive());

  END_TEST;
}

int UtcDaliRenderTaskIsExclusive02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::IsExclusive() Check the getter returns set values");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  // Not exclusive is the default
  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_EQUALS(newTask.IsExclusive(), false, TEST_LOCATION);

  newTask.SetExclusive(true);
  DALI_TEST_EQUALS(newTask.IsExclusive(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskIsExclusiveN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::IsExclusive() on empty handle");

  RenderTask task;
  try
  {
    bool x = task.IsExclusive();
    (void)x;
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskSetInputEnabled(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetInputEnabled()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  // Input is enabled by default
  RenderTask task = taskList.GetTask(0u);
  DALI_TEST_CHECK(true == task.GetInputEnabled());

  task.SetInputEnabled(false);
  DALI_TEST_CHECK(false == task.GetInputEnabled());

  task.SetInputEnabled(true);
  DALI_TEST_CHECK(true == task.GetInputEnabled());
  END_TEST;
}

int UtcDaliRenderTaskGetInputEnabled(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetInputEnabled()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  // Input is enabled by default
  RenderTask task = taskList.GetTask(0u);
  DALI_TEST_EQUALS(true, task.GetInputEnabled(), TEST_LOCATION);

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_EQUALS(true, newTask.GetInputEnabled(), TEST_LOCATION);

  newTask.SetInputEnabled(false);
  DALI_TEST_EQUALS(false, newTask.GetInputEnabled(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetCameraActorP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetCameraActor()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  Actor defaultCameraActor = task.GetCameraActor();
  DALI_TEST_CHECK(defaultCameraActor);

  CameraActor newCameraActor = CameraActor::New();
  DALI_TEST_CHECK(newCameraActor);

  task.SetCameraActor(newCameraActor);
  DALI_TEST_CHECK(task.GetCameraActor() != defaultCameraActor);
  DALI_TEST_EQUALS(task.GetCameraActor(), newCameraActor, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskSetCameraActorN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetCameraActor() with empty actor handle");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  Actor actor = task.GetCameraActor();
  DALI_TEST_CHECK(actor);

  CameraActor cameraActor;

  task.SetCameraActor(cameraActor);
  DALI_TEST_EQUALS((bool)task.GetCameraActor(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCameraActor(), cameraActor, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskSetCameraActorDestroyed(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetCameraActor - Set a camera actor and destroy the camera actor");

  Integration::Scene stage    = application.GetScene();
  RenderTaskList     taskList = stage.GetRenderTaskList();
  RenderTask         task     = taskList.GetTask(0u);

  CameraActor newCameraActor = CameraActor::New();
  task.SetCameraActor(newCameraActor);

  DALI_TEST_EQUALS(task.GetCameraActor(), newCameraActor, TEST_LOCATION);

  // Destroy the camera actor
  newCameraActor.Reset();

  CameraActor camera = task.GetCameraActor();
  DALI_TEST_CHECK(!camera); // The camera actor should be an empty handle.

  END_TEST;
}

int UtcDaliRenderTaskGetCameraActorP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetCameraActor()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  CameraActor actor = task.GetCameraActor();
  DALI_TEST_CHECK(actor);
  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);
  DALI_TEST_GREATER(actor.GetFieldOfView(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskGetCameraActorN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetCameraActor() with empty handle");
  RenderTask task;

  try
  {
    Actor actor = task.GetCameraActor();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRenderTaskSetFrameBufferP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetFrameBuffer()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  FrameBuffer newFrameBuffer = FrameBuffer::New(128u, 128u, FrameBuffer::Attachment::NONE);
  task.SetFrameBuffer(newFrameBuffer);
  DALI_TEST_CHECK(task.GetFrameBuffer() == newFrameBuffer);
  END_TEST;
}

int UtcDaliRenderTaskSetFrameBufferN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetFrameBuffer()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask  task = taskList.GetTask(0u);
  FrameBuffer newFrameBuffer; // Empty handle
  task.SetFrameBuffer(newFrameBuffer);
  DALI_TEST_EQUALS((bool)task.GetFrameBuffer(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskGetFrameBufferP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetFrameBuffer()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  FrameBuffer newFrameBuffer = FrameBuffer::New(1u, 1u, FrameBuffer::Attachment::NONE);
  task.SetFrameBuffer(newFrameBuffer);
  DALI_TEST_CHECK(task.GetFrameBuffer() == newFrameBuffer);
  END_TEST;
}

int UtcDaliRenderTaskGetFrameBufferN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetFrameBuffer()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  // By default render-tasks do not render off-screen
  FrameBuffer frameBuffer = task.GetFrameBuffer();
  DALI_TEST_CHECK(!frameBuffer);

  END_TEST;
}

int UtcDaliRenderTaskSetScreenToFrameBufferFunctionP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetScreenToFrameBufferFunction()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  task.SetScreenToFrameBufferFunction(TestScreenToFrameBufferFunction);

  Vector2 coordinates(5, 10);
  Vector2 convertedCoordinates(6, 12); // + Vector(1, 2)

  RenderTask::ScreenToFrameBufferFunction func = task.GetScreenToFrameBufferFunction();
  DALI_TEST_CHECK(func(coordinates));
  DALI_TEST_CHECK(coordinates == convertedCoordinates);

  task.SetScreenToFrameBufferFunction(RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION);
  func = task.GetScreenToFrameBufferFunction();
  DALI_TEST_CHECK(func(coordinates));

  task.SetScreenToFrameBufferFunction(RenderTask::DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION);
  func = task.GetScreenToFrameBufferFunction();
  DALI_TEST_CHECK(!func(coordinates));
  END_TEST;
}

int UtcDaliRenderTaskSetScreenToFrameBufferFunctionN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetScreenToFrameBufferFunction()");

  RenderTask task; // Empty handle
  try
  {
    task.SetScreenToFrameBufferFunction(TestScreenToFrameBufferFunction);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetScreenToFrameBufferFunctionP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetScreenToFrameBufferFunction()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  Vector2 originalCoordinates(5, 10);
  Vector2 coordinates(5, 10);

  RenderTask::ScreenToFrameBufferFunction func = task.GetScreenToFrameBufferFunction();
  DALI_TEST_CHECK(!func(coordinates));                 // conversion should fail by default
  DALI_TEST_CHECK(coordinates == originalCoordinates); // coordinates should not be modified
  END_TEST;
}

int UtcDaliRenderTaskGetScreenToFrameBufferFunctionN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetScreenToFrameBufferFunction() on empty handle");

  RenderTask task;
  try
  {
    RenderTask::ScreenToFrameBufferFunction func = task.GetScreenToFrameBufferFunction();
    (void)func;
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetScreenToFrameBufferMappingActorP(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::GetScreenToFrameBufferMappingActor ");

  RenderTaskList taskList     = application.GetScene().GetRenderTaskList();
  RenderTask     renderTask   = taskList.CreateTask();
  Actor          mappingActor = Actor::New();
  renderTask.SetScreenToFrameBufferMappingActor(mappingActor);

  DALI_TEST_EQUALS(mappingActor, renderTask.GetScreenToFrameBufferMappingActor(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskGetScreenToFrameBufferMappingActorN(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::GetScreenToFrameBufferMappingActor with empty task handle");

  RenderTask task;
  try
  {
    Actor mappingActor;
    task.SetScreenToFrameBufferMappingActor(mappingActor);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetScreenToFrameBufferMappingActor02N(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::GetScreenToFrameBufferMappingActor with empty task handle");

  RenderTaskList taskList   = application.GetScene().GetRenderTaskList();
  RenderTask     renderTask = taskList.CreateTask();
  Actor          actor;
  renderTask.SetScreenToFrameBufferMappingActor(actor);

  DALI_TEST_EQUALS((bool)renderTask.GetScreenToFrameBufferMappingActor(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskGetViewportP01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetViewport() on default task");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);
  Viewport       viewport = task.GetViewport();

  // By default the viewport should match the stage width/height
  Vector2  stageSize = application.GetScene().GetSize();
  Viewport expectedViewport(0, 0, stageSize.width, stageSize.height);
  DALI_TEST_CHECK(viewport == expectedViewport);
  END_TEST;
}

int UtcDaliRenderTaskGetViewportP02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetViewport() on new task");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.CreateTask();
  Viewport       viewport = task.GetViewport();

  // By default the viewport should match the stage width/height
  Vector2  stageSize = application.GetScene().GetSize();
  Viewport expectedViewport(0, 0, stageSize.width, stageSize.height);
  DALI_TEST_CHECK(viewport == expectedViewport);
  END_TEST;
}

int UtcDaliRenderTaskGetViewportN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetViewport() on empty handle");

  RenderTask task;
  try
  {
    Viewport viewport = task.GetViewport();
    (void)viewport;
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskSetViewportP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetViewport()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task      = taskList.GetTask(0u);
  Vector2    stageSize = application.GetScene().GetSize();
  Viewport   newViewport(0, 0, stageSize.width * 0.5f, stageSize.height * 0.5f);
  task.SetViewport(newViewport);

  // Update (viewport is a property)
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(task.GetViewport() == newViewport);
  END_TEST;
}

int UtcDaliRenderTaskSetViewportN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetViewport()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task;
  try
  {
    Vector2  stageSize = application.GetScene().GetSize();
    Viewport newViewport(0, 0, stageSize.width * 0.5f, stageSize.height * 0.5f);
    task.SetViewport(newViewport);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRenderTaskSetViewportPosition(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetViewportPosition()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  Viewport viewport = task.GetViewport();

  // By default the viewport should match the stage width/height

  Vector2  stageSize = application.GetScene().GetSize();
  Viewport expectedViewport(0, 0, stageSize.width, stageSize.height);
  DALI_TEST_CHECK(viewport == expectedViewport);

  // 'Setter' test
  Vector2 newPosition(25.0f, 50.0f);
  task.SetViewportPosition(newPosition);

  // Update (viewport is a property)
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(task.GetCurrentViewportPosition(), newPosition, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  // Set by Property test
  Vector2 newPosition2(32.0f, 32.0f);
  task.SetProperty(RenderTask::Property::VIEWPORT_POSITION, newPosition2);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition, TEST_LOCATION); // still the old position

  // Update
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(task.GetCurrentViewportPosition(), newPosition2, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition2, TEST_LOCATION);

  Vector2   newPosition3(64.0f, 0.0f);
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(task, RenderTask::Property::VIEWPORT_POSITION), newPosition3, AlphaFunction::LINEAR);
  animation.Play();

  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition3, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition2, TEST_LOCATION);

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS(task.GetCurrentViewportPosition(), newPosition3, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition3, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition3, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  // Create another animation which animates by a certain value
  const Vector2 newPosition4(75.0f, 45.0f);
  const Vector2 relativePosition(newPosition4 - newPosition3);
  animation = Animation::New(1.0f);
  animation.AnimateBy(Property(task, RenderTask::Property::VIEWPORT_POSITION), relativePosition);
  animation.Play();

  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition4, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition3, TEST_LOCATION);

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS(task.GetCurrentViewportPosition(), newPosition4, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition4, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_POSITION), newPosition4, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetViewportSize(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetViewportSize()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);

  Viewport viewport = task.GetViewport();

  // By default the viewport should match the stage width/height

  Vector2  stageSize = application.GetScene().GetSize();
  Viewport expectedViewport(0, 0, stageSize.width, stageSize.height);
  DALI_TEST_CHECK(viewport == expectedViewport);

  Vector2 newSize(128.0f, 64.0f);
  task.SetViewportSize(newSize);

  // Update (viewport is a property)
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(task.GetCurrentViewportSize(), newSize, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  // Set by Property test
  Vector2 newSize2(50.0f, 50.0f);
  task.SetProperty(RenderTask::Property::VIEWPORT_SIZE, newSize2);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize, TEST_LOCATION); // still the old position

  // Update
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(task.GetCurrentViewportSize(), newSize2, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize2, TEST_LOCATION);

  Vector2   newSize3(10.0f, 10.0f);
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(task, RenderTask::Property::VIEWPORT_SIZE), newSize3, AlphaFunction::LINEAR);
  animation.Play();

  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize3, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize2, TEST_LOCATION);

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS(task.GetCurrentViewportSize(), newSize3, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize3, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize3, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  // Create another animation which animates by a certain value
  const Vector2 newSize4(75.0f, 45.0f);
  const Vector2 relativeSize(newSize4 - newSize3);
  animation = Animation::New(1.0f);
  animation.AnimateBy(Property(task, RenderTask::Property::VIEWPORT_SIZE), relativeSize);
  animation.Play();

  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize4, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize3, TEST_LOCATION);

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS(task.GetCurrentViewportSize(), newSize4, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize4, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::VIEWPORT_SIZE), newSize4, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetClearColorP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetClearColor()");

  Vector4 testColor(1.0f, 2.0f, 3.0f, 4.0f);
  Vector4 testColor2(5.0f, 6.0f, 7.0f, 8.0f);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);
  DALI_TEST_CHECK(task.GetClearColor() != testColor);

  task.SetClearColor(testColor);

  // Wait a frame.
  Wait(application);

  DALI_TEST_EQUALS(task.GetClearColor(), testColor, TEST_LOCATION);

  task.SetProperty(RenderTask::Property::CLEAR_COLOR, testColor2);
  DALI_TEST_EQUALS(task.GetProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), testColor2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), testColor, TEST_LOCATION); // still the old color

  // Wait a frame.
  Wait(application);

  DALI_TEST_EQUALS(task.GetClearColor(), testColor2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), testColor2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), testColor2, TEST_LOCATION);

  Vector4   newColor3(10.0f, 10.0f, 20.0f, 30.0f);
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(task, RenderTask::Property::CLEAR_COLOR), newColor3, AlphaFunction::LINEAR);
  animation.Play();

  DALI_TEST_EQUALS(task.GetProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), newColor3, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), testColor2, TEST_LOCATION);

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), newColor3, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), newColor3, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  // Create another animation which animates by a certain value
  const Vector4 newColor4(0.45f, 0.35f, 0.25f, 0.1f);
  const Vector4 relativeColor(newColor4 - newColor3);
  animation = Animation::New(1.0f);
  animation.AnimateBy(Property(task, RenderTask::Property::CLEAR_COLOR), relativeColor);
  animation.Play();

  DALI_TEST_EQUALS(task.GetProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), newColor4, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), newColor3, Math::MACHINE_EPSILON_10, TEST_LOCATION);

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), newColor4, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector4>(RenderTask::Property::CLEAR_COLOR), newColor4, Math::MACHINE_EPSILON_10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetClearColorN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetClearColor() on empty handle");

  RenderTask task;
  try
  {
    task.SetClearColor(Vector4::ZERO);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetClearColorP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetClearColor()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);
  DALI_TEST_EQUALS(task.GetClearColor(), RenderTask::DEFAULT_CLEAR_COLOR, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskGetClearColorN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetClearColor()");

  RenderTask task;
  try
  {
    Vector4 color = task.GetClearColor();
    (void)color;
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskSetClearEnabledP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetClearEnabled()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);
  DALI_TEST_CHECK(task.GetClearEnabled()); // defaults to true

  task.SetClearEnabled(false);
  DALI_TEST_EQUALS(task.GetClearEnabled(), false, TEST_LOCATION);

  task.SetClearEnabled(true);
  DALI_TEST_EQUALS(task.GetClearEnabled(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskSetClearEnabledN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetClearEnabled() with empty handle");

  RenderTask task;
  try
  {
    task.SetClearEnabled(true);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetClearEnabledP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetClearEnabled()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  RenderTask task = taskList.GetTask(0u);
  DALI_TEST_CHECK(task.GetClearEnabled()); // defaults to true
  END_TEST;
}

int UtcDaliRenderTaskGetClearEnabledN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetClearEnabled() with empty handle");

  RenderTask task;
  try
  {
    bool x = task.GetClearEnabled();
    (void)x;
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskSetCullModeP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetCullMode()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);
  DALI_TEST_EQUALS(task.GetCullMode(), true, TEST_LOCATION);

  task.SetCullMode(false);

  DALI_TEST_EQUALS(task.GetCullMode(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetCullModeN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetCullMode() on empty handle");

  RenderTask task;
  try
  {
    task.SetCullMode(false);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetCullModeP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetCullMode()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);
  DALI_TEST_EQUALS(task.GetCullMode(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskGetCullModeN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetCullMode() with empty handle");

  RenderTask task;
  try
  {
    bool x = task.GetCullMode();
    (void)x;
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskSetRefreshRate(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetRefreshRate()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  // By default tasks will be processed every frame
  RenderTask task = taskList.GetTask(0u);
  DALI_TEST_CHECK(RenderTask::REFRESH_ALWAYS == task.GetRefreshRate());

  task.SetRefreshRate(2u); // every-other frame
  DALI_TEST_CHECK(2u == task.GetRefreshRate());

  task.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
  DALI_TEST_CHECK(RenderTask::REFRESH_ALWAYS == task.GetRefreshRate());
  END_TEST;
}

int UtcDaliRenderTaskGetRefreshRate(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetRefreshRate()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  // By default tasks will be processed every frame
  RenderTask task = taskList.GetTask(0u);
  DALI_TEST_CHECK(RenderTask::REFRESH_ALWAYS == task.GetRefreshRate());

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK(RenderTask::REFRESH_ALWAYS == newTask.GetRefreshRate());
  END_TEST;
}

int UtcDaliRenderTaskSignalFinished(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SignalFinished()");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  auto& sync = application.GetGraphicsSyncImpl();

  CameraActor offscreenCameraActor = CameraActor::New();

  application.GetScene().Add(offscreenCameraActor);

  Texture image     = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 10, 10);
  Actor   rootActor = CreateRenderableActor(image);
  rootActor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  application.GetScene().Add(rootActor);

  RenderTaskList          taskList           = application.GetScene().GetRenderTaskList();
  NativeImageInterfacePtr testNativeImagePtr = TestNativeImage::New(10, 10);
  Texture                 frameBufferTexture = Texture::New(*testNativeImagePtr);
  FrameBuffer             frameBuffer        = FrameBuffer::New(frameBufferTexture.GetWidth(), frameBufferTexture.GetHeight());
  frameBuffer.AttachColorTexture(frameBufferTexture);

  RenderTask newTask = taskList.CreateTask();
  newTask.SetCameraActor(offscreenCameraActor);
  newTask.SetSourceActor(rootActor);
  newTask.SetInputEnabled(false);
  newTask.SetClearColor(Vector4(0.f, 0.f, 0.f, 0.f));
  newTask.SetClearEnabled(true);
  newTask.SetExclusive(true);
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  newTask.SetFrameBuffer(frameBuffer);
  newTask.SetProperty(RenderTask::Property::REQUIRES_SYNC, true);

  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);

  // Flush the queue and render.
  application.SendNotification();

  // 1 render to process render task, then wait for sync before finished msg is sent
  // from update to the event thread.

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK(!finished);

  Integration::GraphicsSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK(lastSyncObj != NULL);

  application.Render();
  DALI_TEST_EQUALS((Integration::KeepUpdating::Reasons)(application.GetUpdateStatus() & Integration::KeepUpdating::RENDER_TASK_SYNC), Integration::KeepUpdating::RENDER_TASK_SYNC, TEST_LOCATION);
  application.SendNotification();
  DALI_TEST_CHECK(!finished);

  application.Render();
  DALI_TEST_EQUALS((Integration::KeepUpdating::Reasons)(application.GetUpdateStatus() & Integration::KeepUpdating::RENDER_TASK_SYNC), Integration::KeepUpdating::RENDER_TASK_SYNC, TEST_LOCATION);
  application.SendNotification();
  DALI_TEST_CHECK(!finished);

  sync.SetObjectSynced(lastSyncObj, true);

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK(!finished);

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK(finished);

  DALI_TEST_EQUALS(application.GetUpdateStatus(), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskContinuous01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous using loading image\nPRE: render task not ready (source actor not staged)\nPOST:continuous renders, no Finished signal");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);

  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  application.SendNotification();

  // START PROCESS/RENDER                     Input,    Expected  Input, Expected, KeepUpdating
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, false, false, __LINE__));
  application.GetPlatform().ClearReadyResources();

  // ADD SOURCE ACTOR TO STAGE - expect continuous renders to start, no finished signal
  application.GetScene().Add(secondRootActor);
  application.SendNotification();

  // CONTINUE PROCESS/RENDER                  Input,    Expected  Input,    Expected
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, false, __LINE__));
  END_TEST;
}

int UtcDaliRenderTaskContinuous02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous using loading image\nPRE: render task not ready (source actor not visible)\nPOST:continuous renders, no Finished signal");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);

  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  application.GetScene().Add(secondRootActor);
  secondRootActor.SetProperty(Actor::Property::VISIBLE, false);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected, KeepUpdating
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, false, false, __LINE__));
  application.GetPlatform().ClearReadyResources();

  // MAKE SOURCE ACTOR VISIBLE - expect continuous renders to start, no finished signal
  secondRootActor.SetProperty(Actor::Property::VISIBLE, true);
  application.SendNotification();

  // CONTINUE PROCESS/RENDER                 Input,    Expected  Input,    Expected
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, false, __LINE__));
  END_TEST;
}

int UtcDaliRenderTaskContinuous03(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous using loading image\nPRE: render task not ready (camera actor not staged)\nPOST:continuous renders, no Finished signal");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  Actor       secondRootActor      = CreateRenderableActorSuccess(application, "aFile.jpg");
  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, false, false, __LINE__));
  application.GetPlatform().ClearReadyResources();

  // ADD CAMERA ACTOR TO STAGE - expect continuous renders to start, no finished signal
  application.GetScene().Add(offscreenCameraActor);
  application.SendNotification();

  // CONTINUE PROCESS/RENDER                 Input,    Expected  Input,    Expected
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, false, __LINE__));
  END_TEST;
}

int UtcDaliRenderTaskContinuous04(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous using loaded image");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, false, __LINE__));
  END_TEST;
}

int UtcDaliRenderTaskOnce01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync, using loaded image");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  auto&           sync      = application.GetGraphicsSyncImpl();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");

  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, true);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, true, __LINE__));

  Integration::GraphicsSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK(lastSyncObj != NULL);
  sync.SetObjectSynced(lastSyncObj, true);

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, false, true, __LINE__));
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, true, false, __LINE__));
  END_TEST;
}

int UtcDaliRenderTaskOnce02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync, using Mesh which accesses texture through sampler with loaded image.\n");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  auto&           sync      = application.GetGraphicsSyncImpl();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);

  Shader     shader     = CreateShader();
  Texture    image      = CreateTexture();
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);
  Actor secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, true);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, true, __LINE__));

  Integration::GraphicsSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK(lastSyncObj != NULL);
  sync.SetObjectSynced(lastSyncObj, true);

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, false, true, __LINE__));
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, true, false, __LINE__));

  END_TEST;
}

int UtcDaliRenderTaskOnce03(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync, using loaded image. Switch from render always after ready to render once\n");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  auto&           sync      = application.GetGraphicsSyncImpl();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, false, __LINE__));

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  //                                                   drawn   sig    finished  Keep updating
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, true, __LINE__));

  Integration::GraphicsSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK(lastSyncObj != NULL);
  sync.SetObjectSynced(lastSyncObj, true);

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, false, true, __LINE__));
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, true, false, __LINE__));

  END_TEST;
}

int UtcDaliRenderTaskOnce04(void)
{
  TestApplication application;
  tet_infoline(
    "Testing RenderTask Render Once GlSync, using Mesh which accesses texture through sampler with loaded image.\n"
    "Switch from render always after ready to render once\n");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  auto&           sync      = application.GetGraphicsSyncImpl();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);

  Shader     shader     = CreateShader();
  Texture    image      = CreateTexture();
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);
  Actor secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, false, __LINE__));

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  //   FAILS                                          drawn   sig    finished  Keep updating
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, true, __LINE__));

  Integration::GraphicsSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK(lastSyncObj != NULL);
  sync.SetObjectSynced(lastSyncObj, true);

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, false, true, __LINE__));
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, true, false, __LINE__));

  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once, \nPRE: Resources ready\nPOST: Finished signal sent once only");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, false);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, true, __LINE__));
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, true, false, __LINE__));
  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync02(void)
{
  TestApplication application;

  tet_infoline(
    "Testing RenderTask Render Once, using Mesh which accesses texture through sampler with loaded image.\n"
    "PRE: Resources ready\nPOST: Finished signal sent once only");
  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);

  Shader     shader     = CreateShader();
  Texture    image      = CreateTexture();
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);
  Actor secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, false);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, true, __LINE__));
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, true, false, __LINE__));

  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync03(void)
{
  TestApplication application;

  tet_infoline(
    "Testing RenderTask Render Once, using loaded image. Switch from render always after ready to render once\n"
    "PRE: Render task ready, Image loaded\n"
    "POST: Finished signal sent only once");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, false, __LINE__));

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification(); //         Input,    Expected  Input,    Expected
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, true, __LINE__));
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, true, false, __LINE__));
  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync04(void)
{
  TestApplication application;

  tet_infoline(
    "Testing RenderTask Render Once, using Mesh which accesses texture through sampler with loading image.\n"
    "Switch from render always after ready to render once\n"
    "PRE: Render task ready, Image not loaded\n"
    "POST: Finished signal sent only once");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);

  Shader     shader     = CreateShader();
  Texture    image      = CreateTexture();
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);
  Actor secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, false, __LINE__));
  auto&                                             sync        = application.GetGraphicsSyncImpl();
  Integration::GraphicsSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK(lastSyncObj == NULL);

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification(); //         Input,    Expected  Input,    Expected
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, true, __LINE__));
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, finished, true, false, __LINE__));

  lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK(lastSyncObj == NULL);

  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync05(void)
{
  TestApplication application;

  tet_infoline(
    "Testing RenderTask Render Once\n"
    "SetRefreshRate(ONCE), resource load failed, completes render task.\n"
    "PRE: resources failed to load\n"
    "POST: No finished signal sent.");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  // START PROCESS/RENDER                    Input,     Expected  Input,    Expected
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, false, __LINE__));

  // CHANGE TO RENDER ONCE,
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, false, __LINE__));

  END_TEST;
}

int UtcDaliRenderTaskOnceChain01(void)
{
  TestApplication application;

  tet_infoline(
    "Testing RenderTask Render Once Chained render tasks\n"
    "SetRefreshRate(ONCE), resource load completes, both render tasks render.\n"
    "PRE: resources ready\n"
    "POST: 2 finished signals sent.");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor defaultRootActor = Actor::New(); // Root for default RT
  application.GetScene().Add(defaultRootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);
  Actor firstRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  application.GetScene().Add(firstRootActor);

  // first render task
  RenderTask         firstTask     = CreateRenderTask(application, offscreenCameraActor, defaultRootActor, firstRootActor, RenderTask::REFRESH_ONCE, false);
  bool               firstFinished = false;
  RenderTaskFinished renderTask1Finished(firstFinished);
  firstTask.FinishedSignal().Connect(&application, renderTask1Finished);

  // Second render task
  FrameBuffer fbo             = firstTask.GetFrameBuffer();
  Actor       secondRootActor = CreateRenderableActor(fbo.GetColorTexture());
  application.GetScene().Add(secondRootActor);
  RenderTask         secondTask     = CreateRenderTask(application, offscreenCameraActor, defaultRootActor, secondRootActor, RenderTask::REFRESH_ONCE, false);
  bool               secondFinished = false;
  RenderTaskFinished renderTask2Finished(secondFinished);
  secondTask.FinishedSignal().Connect(&application, renderTask2Finished);

  application.SendNotification();

  // Both render tasks are executed.
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, firstFinished, false, true, __LINE__));
  DALI_TEST_CHECK(firstFinished == false);
  DALI_TEST_CHECK(secondFinished == false);

  // Nothing else to render and both render task should have finished now
  DALI_TEST_CHECK(UpdateRender(application, drawTrace, false, firstFinished, true, false, __LINE__));
  DALI_TEST_CHECK(firstFinished == true);
  DALI_TEST_CHECK(secondFinished == true);

  END_TEST;
}

int UtcDaliRenderTaskProperties(void)
{
  TestApplication application;

  RenderTask task = application.GetScene().GetRenderTaskList().CreateTask();

  Property::IndexContainer indices;
  task.GetPropertyIndices(indices);
  DALI_TEST_CHECK(indices.Size());
  DALI_TEST_EQUALS(indices.Size(), task.GetPropertyCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskFinishInvisibleSourceActor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::FinishInvisibleSourceActor()");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  auto& sync = application.GetGraphicsSyncImpl();

  CameraActor offscreenCameraActor = CameraActor::New();

  application.GetScene().Add(offscreenCameraActor);

  Texture image     = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 10, 10);
  Actor   rootActor = CreateRenderableActor(image);
  rootActor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  rootActor.SetProperty(Actor::Property::VISIBLE, false);
  application.GetScene().Add(rootActor);

  RenderTaskList          taskList           = application.GetScene().GetRenderTaskList();
  NativeImageInterfacePtr testNativeImagePtr = TestNativeImage::New(10, 10);
  Texture                 frameBufferTexture = Texture::New(*testNativeImagePtr);
  FrameBuffer             frameBuffer        = FrameBuffer::New(frameBufferTexture.GetWidth(), frameBufferTexture.GetHeight());
  frameBuffer.AttachColorTexture(frameBufferTexture);

  // Flush all outstanding messages
  application.SendNotification();
  application.Render();

  RenderTask newTask = taskList.CreateTask();
  newTask.SetCameraActor(offscreenCameraActor);
  newTask.SetSourceActor(rootActor);
  newTask.SetInputEnabled(false);
  newTask.SetClearColor(Vector4(0.f, 0.f, 0.f, 0.f));
  newTask.SetClearEnabled(true);
  newTask.SetExclusive(true);
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  newTask.SetFrameBuffer(frameBuffer);
  newTask.SetProperty(RenderTask::Property::REQUIRES_SYNC, true);

  // Framebuffer doesn't actually get created until Connected, i.e. by previous line

  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);

  // Flush the queue and render.
  application.SendNotification();

  // 1 render to process render task, then wait for sync before finished msg is sent
  // from update to the event thread.

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK(!finished);

  Integration::GraphicsSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK(lastSyncObj != NULL);

  application.Render();
  DALI_TEST_EQUALS((Integration::KeepUpdating::Reasons)(application.GetUpdateStatus() & Integration::KeepUpdating::RENDER_TASK_SYNC), Integration::KeepUpdating::RENDER_TASK_SYNC, TEST_LOCATION);
  application.SendNotification();
  DALI_TEST_CHECK(!finished);

  application.Render();
  DALI_TEST_EQUALS((Integration::KeepUpdating::Reasons)(application.GetUpdateStatus() & Integration::KeepUpdating::RENDER_TASK_SYNC), Integration::KeepUpdating::RENDER_TASK_SYNC, TEST_LOCATION);
  application.SendNotification();
  DALI_TEST_CHECK(!finished);

  sync.SetObjectSynced(lastSyncObj, true);

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK(!finished);

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK(finished);
  finished = false;

  application.Render(); // Double check no more finished signal
  application.SendNotification();
  DALI_TEST_CHECK(!finished);

  END_TEST;
}

int UtcDaliRenderTaskFinishMissingImage(void)
{
  TestApplication application;

  // Previously we had bugs where not having a resource ID would cause render-tasks to wait forever
  tet_infoline("Testing RenderTask::SignalFinished() when an Actor has no Image set");

  Integration::Scene stage = application.GetScene();

  Texture image     = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 10, 10);
  Actor   rootActor = CreateRenderableActor(image);
  rootActor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  stage.Add(rootActor);

  Actor actorWithMissingImage = CreateRenderableActor(Texture());
  actorWithMissingImage.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  stage.Add(actorWithMissingImage);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     newTask  = taskList.CreateTask();
  newTask.SetInputEnabled(false);
  newTask.SetClearColor(Vector4(0.f, 0.f, 0.f, 0.f));
  newTask.SetClearEnabled(true);
  newTask.SetExclusive(true);
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);

  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);

  // 1 render to process render task, then 1 before finished msg is sent from update to the event thread.
  application.SendNotification();
  application.Render();
  application.Render();

  application.SendNotification();
  DALI_TEST_CHECK(finished);

  END_TEST;
}

int UtcDaliRenderTaskWorldToViewport(void)
{
  TestApplication application(400u, 400u); // square surface

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0, 0.0, 0.0));

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5, 0.5, 0.5));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, Vector3(0.5, 0.5, 0.5));

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();
  application.SendNotification();

  RenderTask task = taskList.GetTask(0u);

  CameraActor camera = task.GetCameraActor();

  Vector2 screenSize = task.GetCurrentViewportSize();

  float screenX = 0.0;
  float screenY = 0.0;

  bool ok = task.WorldToViewport(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), screenX, screenY);
  DALI_TEST_CHECK(ok == true);

  DALI_TEST_EQUALS(screenX, screenSize.x / 2, Math::MACHINE_EPSILON_10000, TEST_LOCATION);
  DALI_TEST_EQUALS(screenY, screenSize.y / 2, Math::MACHINE_EPSILON_10000, TEST_LOCATION);

  Actor actor2     = Actor::New();
  float actor2Size = 100.f;
  actor2.SetProperty(Actor::Property::SIZE, Vector2(actor2Size, actor2Size));
  actor2.SetProperty(Actor::Property::POSITION, Vector3(0.0, 0.0, 0.0));
  actor2.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5, 0.5, 0.0));
  actor2.SetProperty(Actor::Property::ANCHOR_POINT, Vector3(0.5, 0.5, 0.0));
  application.GetScene().Add(actor2);
  actor2.Add(actor);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0, 0, 0));

  application.SendNotification();
  application.Render();
  application.SendNotification();

  ok = task.WorldToViewport(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), screenX, screenY);
  DALI_TEST_CHECK(ok == true);

  DALI_TEST_EQUALS(screenX, screenSize.x / 2 - actor2Size / 2, Math::MACHINE_EPSILON_10000, TEST_LOCATION);
  DALI_TEST_EQUALS(screenY, screenSize.y / 2 - actor2Size / 2, Math::MACHINE_EPSILON_10000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskViewportToLocal(void)
{
  TestApplication application;
  Actor           actor = Actor::New();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::POSITION, Vector2(10.0f, 10.0f));
  application.GetScene().Add(actor);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  float localX;
  float localY;

  float rtLocalX;
  float rtLocalY;

  float screenX = 50.0f;
  float screenY = 50.0f;

  DALI_TEST_CHECK(actor.ScreenToLocal(localX, localY, screenX, screenY));

  DALI_TEST_CHECK(task.ViewportToLocal(actor, screenX, screenY, rtLocalX, rtLocalY));

  DALI_TEST_EQUALS(localX, rtLocalX, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(localY, rtLocalY, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskOffscreenViewportToLocal(void)
{
  TestApplication application;
  Actor           actor = Actor::New();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::POSITION, Vector2(10.0f, 10.0f));
  application.GetScene().Add(actor);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.CreateTask();

  FrameBuffer newFrameBuffer = FrameBuffer::New(10, 10);
  task.SetFrameBuffer(newFrameBuffer);
  task.SetSourceActor(actor);
  task.SetScreenToFrameBufferMappingActor(actor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);
  task.SetCameraActor(offscreenCameraActor);

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  float localX;
  float localY;

  float rtLocalX;
  float rtLocalY;

  float screenX = 50.0f;
  float screenY = 50.0f;

  DALI_TEST_CHECK(actor.ScreenToLocal(localX, localY, screenX, screenY));

  DALI_TEST_CHECK(task.ViewportToLocal(actor, screenX, screenY, rtLocalX, rtLocalY));

  DALI_TEST_EQUALS(localX, rtLocalX, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(localY, rtLocalY, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskRequiresSync(void)
{
  TestApplication application;
  RenderTaskList  taskList = application.GetScene().GetRenderTaskList();

  RenderTask newTask = taskList.CreateTask();
  newTask.SetProperty(RenderTask::Property::REQUIRES_SYNC, false);

  DALI_TEST_EQUALS(newTask.GetProperty<bool>(RenderTask::Property::REQUIRES_SYNC), false, TEST_LOCATION);
  DALI_TEST_EQUALS(newTask.GetCurrentProperty<bool>(RenderTask::Property::REQUIRES_SYNC), false, TEST_LOCATION);

  newTask.SetProperty(RenderTask::Property::REQUIRES_SYNC, true);

  DALI_TEST_EQUALS(newTask.GetProperty<bool>(RenderTask::Property::REQUIRES_SYNC), true, TEST_LOCATION);
  DALI_TEST_EQUALS(newTask.GetCurrentProperty<bool>(RenderTask::Property::REQUIRES_SYNC), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetClearEnabled(void)
{
  TestApplication application;

  tet_infoline("UtcDaliRenderTaskSetClearEnabled");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TestGlAbstraction& gl = application.GetGlAbstraction();

  Actor renderableActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  application.GetScene().Add(renderableActor);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);

  Actor sourceActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  application.GetScene().Add(sourceActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, sourceActor, RenderTask::REFRESH_ALWAYS, false);

  DALI_TEST_EQUALS(gl.GetClearCountCalled(), 0, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // glClear should be called twice - default task and the new task.
  DALI_TEST_EQUALS(gl.GetClearCountCalled(), 2, TEST_LOCATION);

  newTask.SetClearEnabled(false);

  application.SendNotification();
  application.Render();

  // The count should increase by 1 - default task only.
  DALI_TEST_EQUALS(gl.GetClearCountCalled(), 3, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskMoveConstrctor(void)
{
  TestApplication application;

  Vector4 testColor(1.0f, 2.0f, 3.0f, 4.0f);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);
  DALI_TEST_CHECK(task);
  DALI_TEST_EQUALS(2, task.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(task.GetClearColor() != testColor);

  task.SetClearColor(testColor);

  // Wait a frame.
  Wait(application);

  DALI_TEST_EQUALS(task.GetClearColor(), testColor, TEST_LOCATION);

  RenderTask move = std::move(task);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(2, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetClearColor(), testColor, TEST_LOCATION);
  DALI_TEST_CHECK(!task);

  END_TEST;
}

int UtcDaliRenderTaskMoveAssignment(void)
{
  TestApplication application;

  Vector4 testColor(1.0f, 2.0f, 3.0f, 4.0f);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.GetTask(0u);
  DALI_TEST_CHECK(task);
  DALI_TEST_EQUALS(2, task.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(task.GetClearColor() != testColor);

  task.SetClearColor(testColor);

  // Wait a frame.
  Wait(application);

  DALI_TEST_EQUALS(task.GetClearColor(), testColor, TEST_LOCATION);

  RenderTask move;
  move = std::move(task);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(2, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetClearColor(), testColor, TEST_LOCATION);
  DALI_TEST_CHECK(!task);

  END_TEST;
}

int UtcDaliRenderTaskSetCullModeNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    bool arg1(false);
    instance.SetCullMode(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetViewportNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    Dali::Rect<int> arg1;
    instance.SetViewport(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetExclusiveNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    bool arg1(false);
    instance.SetExclusive(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetClearColorNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    Dali::Vector4 arg1;
    instance.SetClearColor(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskFinishedSignalNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.FinishedSignal();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetCameraActorNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    Dali::CameraActor arg1;
    instance.SetCameraActor(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetFrameBufferNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    Dali::FrameBuffer arg1;
    instance.SetFrameBuffer(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetRefreshRateNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    unsigned int arg1(0u);
    instance.SetRefreshRate(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    Dali::Actor arg1;
    instance.SetSourceActor(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetClearEnabledNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    bool arg1(false);
    instance.SetClearEnabled(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetInputEnabledNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    bool arg1(false);
    instance.SetInputEnabled(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetViewportSizeNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    Dali::Vector2 arg1;
    instance.SetViewportSize(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetViewportPositionNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    Dali::Vector2 arg1;
    instance.SetViewportPosition(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetScreenToFrameBufferFunctionNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    RenderTask::ScreenToFrameBufferFunction arg1(nullptr);
    instance.SetScreenToFrameBufferFunction(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskSetScreenToFrameBufferMappingActorNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    Dali::Actor arg1;
    instance.SetScreenToFrameBufferMappingActor(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetCullModeNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetCullMode();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetViewportNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetViewport();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskIsExclusiveNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.IsExclusive();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetClearColorNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetClearColor();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetCameraActorNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetCameraActor();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetFrameBufferNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetFrameBuffer();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetRefreshRateNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetRefreshRate();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetSourceActorNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetSourceActor();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetClearEnabledNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetClearEnabled();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetInputEnabledNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetInputEnabled();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskViewportToLocalNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    Dali::Actor arg1(Actor::New());
    float       arg2(0.0f);
    float       arg3(0.0f);
    float       arg4(0.0f);
    float       arg5(0.0f);
    instance.ViewportToLocal(arg1, arg2, arg3, arg4, arg5);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskWorldToViewportNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    Dali::Vector3 arg1;
    float         arg2(0.0f);
    float         arg3(0.0f);
    instance.WorldToViewport(arg1, arg2, arg3);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetCurrentViewportSizeNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetCurrentViewportSize();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetCurrentViewportPositionNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetCurrentViewportPosition();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetScreenToFrameBufferFunctionNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetScreenToFrameBufferFunction();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskGetScreenToFrameBufferMappingActorNegative(void)
{
  TestApplication  application;
  Dali::RenderTask instance;
  try
  {
    instance.GetScreenToFrameBufferMappingActor();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskClippingMode01(void)
{
  TestApplication application;

  tet_infoline("Testing clipping mode: CLIP_TO_BOUNDING_BOX.\n");

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();
  TraceCallStack&    scissorTrace        = glAbstraction.GetScissorTrace();

  enabledDisableTrace.Enable(true);
  scissorTrace.Enable(true);

  // SETUP AN OFFSCREEN RENDER TASK
  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  offscreenCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  offscreenCameraActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  application.GetScene().Add(offscreenCameraActor);

  Shader     shader     = CreateShader();
  Texture    image      = CreateTexture();
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Vector2 position(100.0f, 100.0f);
  Vector2 size(200.0f, 200.0f);
  Actor   secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetProperty(Actor::Property::POSITION, position);
  secondRootActor.SetProperty(Actor::Property::SIZE, size);
  secondRootActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  secondRootActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  secondRootActor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  application.GetScene().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, true, TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);

  application.SendNotification();
  application.Render();

  std::ostringstream scissor;
  scissor << std::hex << GL_SCISSOR_TEST;
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", scissor.str()));

  // Check the scissor was set, and the coordinates are correct.
  Vector4           expectResults(position.x, TestApplication::DEFAULT_SURFACE_HEIGHT - size.height - position.y, size.width, size.height); // (100, 500, 200, 200)
  std::stringstream compareParametersString;
  compareParametersString << expectResults.x << ", " << expectResults.y << ", " << expectResults.z << ", " << expectResults.w;
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", compareParametersString.str())); // Compare with the expected result

  END_TEST;
}

int UtcDaliRenderTaskClippingMode02(void)
{
  TestApplication application;

  tet_infoline("Testing clipping mode with the inverted camera: CLIP_TO_BOUNDING_BOX.\n");

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();
  TraceCallStack&    scissorTrace        = glAbstraction.GetScissorTrace();

  enabledDisableTrace.Enable(true);
  scissorTrace.Enable(true);

  // SETUP AN OFFSCREEN RENDER TASK
  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  offscreenCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  offscreenCameraActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  offscreenCameraActor.SetInvertYAxis(true);
  application.GetScene().Add(offscreenCameraActor);

  Shader     shader     = CreateShader();
  Texture    image      = CreateTexture();
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Vector2 position(100.0f, 100.0f);
  Vector2 size(200.0f, 200.0f);
  Actor   secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetProperty(Actor::Property::POSITION, position);
  secondRootActor.SetProperty(Actor::Property::SIZE, size);
  secondRootActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  secondRootActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  secondRootActor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  application.GetScene().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, true, TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);

  application.SendNotification();
  application.Render();

  std::ostringstream scissor;
  scissor << std::hex << GL_SCISSOR_TEST;
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", scissor.str()));

  // Check the scissor was set, and the coordinates are correct.
  Vector4           expectResults(position.x, position.y, size.width, size.height); // (100, 100, 200, 200)
  std::stringstream compareParametersString;
  compareParametersString << expectResults.x << ", " << expectResults.y << ", " << expectResults.z << ", " << expectResults.w;
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", compareParametersString.str())); // Compare with the expected result

  END_TEST;
}

int UtcDaliRenderTaskClippingMode03(void)
{
  TestApplication application;

  tet_infoline("Testing clipping mode: CLIP_TO_BOUNDING_BOX. with RenderedScaleFactor\n");

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();
  TraceCallStack&    scissorTrace        = glAbstraction.GetScissorTrace();

  enabledDisableTrace.Enable(true);
  scissorTrace.Enable(true);

  // SETUP AN OFFSCREEN RENDER TASK
  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  offscreenCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  offscreenCameraActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  application.GetScene().Add(offscreenCameraActor);

  Shader     shader     = CreateShader();
  Texture    image      = CreateTexture();
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Vector2 position(100.0f, 100.0f);
  Vector2 size(200.0f, 200.0f);
  Actor   secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetProperty(Actor::Property::POSITION, position);
  secondRootActor.SetProperty(Actor::Property::SIZE, size);
  secondRootActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  secondRootActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  secondRootActor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  application.GetScene().Add(secondRootActor);

  float downscaledFactor = 0.5f;

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, true, static_cast<uint32_t>(TestApplication::DEFAULT_SURFACE_WIDTH * downscaledFactor), static_cast<uint32_t>(TestApplication::DEFAULT_SURFACE_HEIGHT * downscaledFactor));
  newTask.SetRenderedScaleFactor(Vector2(downscaledFactor, downscaledFactor));

  application.SendNotification();
  application.Render();

  std::ostringstream scissor;
  scissor << std::hex << GL_SCISSOR_TEST;
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", scissor.str()));

  // Check the scissor was set, and the coordinates are correct.
  Vector4           expectResults = Vector4(position.x, TestApplication::DEFAULT_SURFACE_HEIGHT - size.height - position.y, size.width, size.height) * downscaledFactor; // (50, 250, 100, 100)
  std::stringstream compareParametersString;
  compareParametersString << expectResults.x << ", " << expectResults.y << ", " << expectResults.z << ", " << expectResults.w;
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", compareParametersString.str())); // Compare with the expected result

  END_TEST;
}

int UtcDaliRenderTaskClippingMode04(void)
{
  TestApplication application;

  tet_infoline("Testing clipping mode with the inverted camera: CLIP_TO_BOUNDING_BOX. with RenderedScaleFactor\n");

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();
  TraceCallStack&    scissorTrace        = glAbstraction.GetScissorTrace();

  enabledDisableTrace.Enable(true);
  scissorTrace.Enable(true);

  // SETUP AN OFFSCREEN RENDER TASK
  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  offscreenCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  offscreenCameraActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  offscreenCameraActor.SetInvertYAxis(true);
  application.GetScene().Add(offscreenCameraActor);

  Shader     shader     = CreateShader();
  Texture    image      = CreateTexture();
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Vector2 position(100.0f, 100.0f);
  Vector2 size(200.0f, 200.0f);
  Actor   secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetProperty(Actor::Property::POSITION, position);
  secondRootActor.SetProperty(Actor::Property::SIZE, size);
  secondRootActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  secondRootActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  secondRootActor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  application.GetScene().Add(secondRootActor);

  float downscaledFactor = 0.5f;

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, true, static_cast<uint32_t>(TestApplication::DEFAULT_SURFACE_WIDTH * downscaledFactor), static_cast<uint32_t>(TestApplication::DEFAULT_SURFACE_HEIGHT * downscaledFactor));
  newTask.SetRenderedScaleFactor(Vector2(downscaledFactor, downscaledFactor));

  application.SendNotification();
  application.Render();

  std::ostringstream scissor;
  scissor << std::hex << GL_SCISSOR_TEST;
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", scissor.str()));

  // Check the scissor was set, and the coordinates are correct.
  Vector4           expectResults = Vector4(position.x, position.y, size.width, size.height) * downscaledFactor; // (50, 50, 100, 100)
  std::stringstream compareParametersString;
  compareParametersString << expectResults.x << ", " << expectResults.y << ", " << expectResults.z << ", " << expectResults.w;
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", compareParametersString.str())); // Compare with the expected result

  END_TEST;
}

int UtcDaliRenderTaskUploadOnly(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync, using loaded image");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  auto&           sync      = application.GetGraphicsSyncImpl();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  application.GetScene().Add(rootActor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  application.GetScene().Add(offscreenCameraActor);
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");

  application.GetScene().Add(secondRootActor);

  RenderTask         newTask  = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, true);
  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);
  application.SendNotification();

  DALI_TEST_CHECK(UpdateRender(application, drawTrace, true, finished, false, true, __LINE__));

  Integration::GraphicsSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK(lastSyncObj != NULL);
  sync.SetObjectSynced(lastSyncObj, true);

  application.SendNotification();
  application.Render(16, nullptr, true);

  DALI_TEST_CHECK(!finished);

  application.Render(16, nullptr, true);
  application.SendNotification();

  DALI_TEST_CHECK(!finished);
  END_TEST;
}

int UtcDaliRenderTaskSetGetViewportGuideActor(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask with Set/Get ViewportGuideActor");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  Actor blue                                 = Actor::New();
  blue[Dali::Actor::Property::NAME]          = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  blue[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  blue[Dali::Actor::Property::SIZE]          = Vector2(300, 300);
  blue[Dali::Actor::Property::POSITION]      = Vector2(0, 0);

  stage.Add(blue);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     renderTask     = renderTaskList.CreateTask();

  renderTask.SetViewportGuideActor(blue);

  Actor actor = renderTask.GetViewportGuideActor();
  DALI_TEST_EQUALS(actor, blue, TEST_LOCATION);

  renderTask.ResetViewportGuideActor();
  actor = renderTask.GetViewportGuideActor();

  DALI_TEST_CHECK(!actor);

  END_TEST;
}

int UtcDaliRenderTaskViewportGuideActor(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask with ViewportGuideActor");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  Actor blue                                 = Actor::New();
  blue[Dali::Actor::Property::NAME]          = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  blue[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  blue[Dali::Actor::Property::SIZE]          = Vector2(300, 300);
  blue[Dali::Actor::Property::POSITION]      = Vector2(0, 0);

  Geometry geometry = Geometry::New();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);
  blue.AddRenderer(renderer);

  stage.Add(blue);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     renderTask     = renderTaskList.CreateTask();

  Dali::CameraActor cameraActor                     = Dali::CameraActor::New(stageSize);
  cameraActor[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  cameraActor[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(cameraActor);

  renderTask.SetExclusive(true);
  renderTask.SetInputEnabled(true);
  renderTask.SetCameraActor(cameraActor);
  renderTask.SetSourceActor(blue);
  renderTask.SetViewportGuideActor(blue);

  // Render and notify
  application.SendNotification();
  application.Render(16);

  Vector2 viewportPosition = renderTask.GetCurrentViewportPosition();
  Vector2 viewportSize     = renderTask.GetCurrentViewportSize();

  DALI_TEST_EQUALS(viewportSize, Vector2(300, 300), TEST_LOCATION);
  DALI_TEST_EQUALS(viewportPosition, Vector2(90, 250), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskViewportGuideActor02(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack&    callStack     = glAbstraction.GetViewportTrace();
  glAbstraction.EnableViewportCallTrace(true);
  tet_infoline("Testing RenderTask with ViewportGuideActor02");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  // Render and notify
  application.SendNotification();
  application.Render(16);
  glAbstraction.ResetViewportCallStack();

  Geometry geometry = Geometry::New();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  Actor blue                                 = Actor::New();
  blue[Dali::Actor::Property::NAME]          = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_LEFT;
  blue[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
  blue[Dali::Actor::Property::SIZE]          = Vector2(400, 300);
  blue[Dali::Actor::Property::POSITION]      = Vector2(100, 50);
  blue.AddRenderer(renderer);
  stage.Add(blue);

  Actor green                                 = Actor::New();
  green[Dali::Actor::Property::NAME]          = "Green";
  green[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_LEFT;
  green[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
  green[Dali::Actor::Property::SIZE]          = Vector2(400, 300);
  green[Dali::Actor::Property::POSITION]      = Vector2(100, 50);
  green.AddRenderer(renderer);
  stage.Add(green);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     renderTask     = renderTaskList.CreateTask();

  Dali::CameraActor cameraActor                     = Dali::CameraActor::New(stageSize);
  cameraActor[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  cameraActor[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(cameraActor);

  renderTask.SetExclusive(true);
  renderTask.SetInputEnabled(true);
  renderTask.SetCameraActor(cameraActor);
  renderTask.SetSourceActor(blue);
  renderTask.SetViewportGuideActor(blue);

  application.GetScene().SurfaceRotated(static_cast<float>(TestApplication::DEFAULT_SURFACE_HEIGHT),
                                        static_cast<float>(TestApplication::DEFAULT_SURFACE_WIDTH),
                                        90,
                                        0);

  // Render and notify
  application.SendNotification();
  application.Render(16);

  std::string viewportParams1("50, 100, 300, 400"); // to match newSize
  std::string viewportParams2("0, 0, 480, 800");    // to match newSize

  // Check that the viewport is handled properly
  DALI_TEST_CHECK(callStack.FindIndexFromMethodAndParams("Viewport", viewportParams1) >= 0);
  DALI_TEST_CHECK(callStack.FindIndexFromMethodAndParams("Viewport", viewportParams2) >= 0);

  END_TEST;
}

int UtcDaliRenderTaskViewportGuideActor03(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack&    callStack     = glAbstraction.GetViewportTrace();
  glAbstraction.EnableViewportCallTrace(true);
  tet_infoline("Testing that adding a viewport guide actor to RenderTask will change the viewport");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  // Render and notify
  application.SendNotification();
  application.Render(16);
  glAbstraction.ResetViewportCallStack();

  Geometry geometry = Geometry::New();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  Actor blue                                 = Actor::New();
  blue[Dali::Actor::Property::NAME]          = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_LEFT;
  blue[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
  blue[Dali::Actor::Property::SIZE]          = Vector2(400, 300);
  blue[Dali::Actor::Property::POSITION]      = Vector2(100, 50);
  blue.AddRenderer(renderer);
  stage.Add(blue);

  Actor green                                 = Actor::New();
  green[Dali::Actor::Property::NAME]          = "Green";
  green[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_LEFT;
  green[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
  green[Dali::Actor::Property::SIZE]          = Vector2(400, 300);
  green[Dali::Actor::Property::POSITION]      = Vector2(100, 50);
  green.AddRenderer(renderer);
  stage.Add(green);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     renderTask     = renderTaskList.CreateTask();

  Dali::CameraActor cameraActor                     = Dali::CameraActor::New(stageSize);
  cameraActor[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  cameraActor[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(cameraActor);

  renderTask.SetExclusive(true);
  renderTask.SetInputEnabled(true);
  renderTask.SetCameraActor(cameraActor);
  renderTask.SetSourceActor(green);

  Viewport viewport(75, 55, 150, 250);
  renderTask.SetViewport(viewport);

  // Render and notify
  application.SendNotification();
  application.Render(16);

  // Note Y pos: 800 - (250+55) = 495
  std::string viewportParams1("75, 495, 150, 250");
  DALI_TEST_CHECK(callStack.FindIndexFromMethodAndParams("Viewport", viewportParams1) >= 0);
  glAbstraction.ResetViewportCallStack();

  // Update to use viewport guide actor instead.
  renderTask.SetViewportGuideActor(blue);

  // Render and notify
  application.SendNotification();
  application.Render(16);

  // Note: Y pos: 800 - (300+50) = 450
  std::string viewportParams2("100, 450, 400, 300");
  DALI_TEST_CHECK(callStack.FindIndexFromMethodAndParams("Viewport", viewportParams2) >= 0);
  tet_infoline("Testing that removing viewport guide actor from RenderTask will revert the viewport");
  glAbstraction.ResetViewportCallStack();

  // Remove guide actor, expect that the viewport is reset to its original values
  renderTask.SetViewportGuideActor(Actor());
  application.SendNotification();
  application.Render(16);

  // Currently, update manager does not consider that added Resetters should cause another
  // update; this is probably right. But, we have to then force another update for the resetter to trigger, and this will register as un-necessary in the test output.
  //
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(callStack.FindIndexFromMethodAndParams("Viewport", viewportParams1) >= 0);

  END_TEST;
}

int UtcDaliRenderTaskViewportGuideActor04(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack&    callStack     = glAbstraction.GetViewportTrace();
  glAbstraction.EnableViewportCallTrace(true);
  tet_infoline("Testing that adding a viewport guide actor to RenderTask will change the viewport");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  // Render and notify
  application.SendNotification();
  application.Render(16);
  glAbstraction.ResetViewportCallStack();

  Geometry geometry = Geometry::New();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  Actor blue                                 = Actor::New();
  blue[Dali::Actor::Property::NAME]          = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_LEFT;
  blue[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
  blue[Dali::Actor::Property::SIZE]          = Vector2(400, 300);
  blue[Dali::Actor::Property::POSITION]      = Vector2(100, 50);
  blue.AddRenderer(renderer);
  stage.Add(blue);

  Actor green                                 = Actor::New();
  green[Dali::Actor::Property::NAME]          = "Green";
  green[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_LEFT;
  green[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
  green[Dali::Actor::Property::SIZE]          = Vector2(400, 300);
  green[Dali::Actor::Property::POSITION]      = Vector2(100, 50);
  green.AddRenderer(renderer);
  stage.Add(green);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     renderTask     = renderTaskList.CreateTask();

  Dali::CameraActor cameraActor                     = Dali::CameraActor::New(stageSize);
  cameraActor[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  cameraActor[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(cameraActor);

  renderTask.SetExclusive(true);
  renderTask.SetInputEnabled(true);
  renderTask.SetCameraActor(cameraActor);
  renderTask.SetSourceActor(green);

  Viewport viewport(75, 55, 150, 250);
  renderTask.SetViewport(viewport);

  // Render and notify
  application.SendNotification();
  application.Render(16);

  // Note Y pos: 800 - (250+55) = 495
  std::string viewportParams1("75, 495, 150, 250");
  DALI_TEST_CHECK(callStack.FindIndexFromMethodAndParams("Viewport", viewportParams1) >= 0);
  glAbstraction.ResetViewportCallStack();

  // Update to use viewport guide actor instead.
  renderTask.SetViewportGuideActor(blue);

  // Render and notify
  application.SendNotification();
  application.Render(16);

  std::string viewportParams2("100, 450, 400, 300");
  DALI_TEST_CHECK(callStack.FindIndexFromMethodAndParams("Viewport", viewportParams2) >= 0);
  tet_infoline("Testing that removing viewport guide actor from RenderTask will revert the viewport");

  glAbstraction.ResetViewportCallStack();

  // Remove guide actor, expect that the viewport is reset to it's original values
  renderTask.ResetViewportGuideActor();
  application.SendNotification();
  application.Render(16);

  // Currently, update manager does not consider that added Resetters should cause another
  // update; this is probably right. But, we have to then force another update for the resetter
  // to trigger, and this will register as un-necessary in the test output.
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(callStack.FindIndexFromMethodAndParams("Viewport", viewportParams1) >= 0);

  // This should remove the baking resetters, but is again going to show up
  // as unnecessary. Also try and figure out if we can test the dirty flags
  // here, somehow (Can at least check the property's dirty flags in the debugger).
  application.SendNotification();
  application.Render(16);

  END_TEST;
}

int UtcDaliRenderTaskViewportGuideActorDestroyed(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask with ViewportGuideActor, and check viewport if guide actor destroyed");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  Actor blue                                 = Actor::New();
  blue[Dali::Actor::Property::NAME]          = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  blue[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  blue[Dali::Actor::Property::SIZE]          = Vector2(300, 300);
  blue[Dali::Actor::Property::POSITION]      = Vector2(0, 0);

  Actor red                                 = Actor::New();
  red[Dali::Actor::Property::NAME]          = "Red";
  red[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  red[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  red[Dali::Actor::Property::SIZE]          = Vector2(300, 300);
  red[Dali::Actor::Property::POSITION]      = Vector2(0, 0);

  Geometry geometry = Geometry::New();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);
  red.AddRenderer(renderer);

  stage.Add(blue);
  stage.Add(red);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     renderTask     = renderTaskList.CreateTask();

  Dali::CameraActor cameraActor                     = Dali::CameraActor::New(stageSize);
  cameraActor[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  cameraActor[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(cameraActor);

  renderTask.SetExclusive(true);
  renderTask.SetInputEnabled(true);
  renderTask.SetCameraActor(cameraActor);
  renderTask.SetSourceActor(red);
  application.SendNotification();
  application.Render(16);

  Vector2 viewportPosition = renderTask.GetCurrentViewportPosition();
  Vector2 viewportSize     = renderTask.GetCurrentViewportSize();

  // Check viewport return 0 without guide actor
  DALI_TEST_EQUALS(viewportSize, Vector2(0, 0), TEST_LOCATION);
  DALI_TEST_EQUALS(viewportPosition, Vector2(0, 0), TEST_LOCATION);

  // Set ViewportGuideActor
  renderTask.SetViewportGuideActor(blue);

  // Render and notify
  application.SendNotification();
  application.Render(16);

  viewportPosition = renderTask.GetCurrentViewportPosition();
  viewportSize     = renderTask.GetCurrentViewportSize();

  DALI_TEST_EQUALS(viewportSize, Vector2(300, 300), TEST_LOCATION);
  DALI_TEST_EQUALS(viewportPosition, Vector2(90, 250), TEST_LOCATION);

  // Render several frames.
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  viewportPosition = renderTask.GetCurrentViewportPosition();
  viewportSize     = renderTask.GetCurrentViewportSize();

  DALI_TEST_EQUALS(viewportSize, Vector2(300, 300), TEST_LOCATION);
  DALI_TEST_EQUALS(viewportPosition, Vector2(90, 250), TEST_LOCATION);

  tet_printf("Destroy blue. RenderTask don't hold actor's reference, so ViewportGuideActor invalide now\n");

  blue.Unparent();
  blue.Reset();

  DALI_TEST_EQUALS(renderTask.GetViewportGuideActor(), Actor(), TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render(16);

  viewportPosition = renderTask.GetCurrentViewportPosition();
  viewportSize     = renderTask.GetCurrentViewportSize();

  // Check view port revert as origin well.
  DALI_TEST_EQUALS(viewportSize, Vector2(0, 0), TEST_LOCATION);
  DALI_TEST_EQUALS(viewportPosition, Vector2(0, 0), TEST_LOCATION);

  // Render several frames.
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  viewportPosition = renderTask.GetCurrentViewportPosition();
  viewportSize     = renderTask.GetCurrentViewportSize();

  DALI_TEST_EQUALS(viewportSize, Vector2(0, 0), TEST_LOCATION);
  DALI_TEST_EQUALS(viewportPosition, Vector2(0, 0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskSetPartialUpdate(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged rects with render task");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  Actor rootActor = CreateRenderableActor();
  rootActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  rootActor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  rootActor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  rootActor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(rootActor);

  CameraActor cameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  cameraActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  cameraActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(cameraActor);

  Texture     frameBufferTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGB888, 16, 16);
  FrameBuffer frameBuffer        = FrameBuffer::New(frameBufferTexture.GetWidth(), frameBufferTexture.GetHeight());
  frameBuffer.AttachColorTexture(frameBufferTexture);

  // Create a RenderTask and set a framebuffer
  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     newTask  = taskList.CreateTask();
  newTask.SetCameraActor(cameraActor);
  newTask.SetSourceActor(rootActor);
  newTask.SetInputEnabled(false);
  newTask.SetClearColor(Vector4(0.f, 0.f, 0.f, 0.f));
  newTask.SetClearEnabled(true);
  newTask.SetExclusive(true);
  newTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
  newTask.SetFrameBuffer(frameBuffer);

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;

  // First frame result.
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  clippingRect = Rect<int>(16, 768, 32, 32);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  DirtyRectChecker(damagedRects, {clippingRect}, true, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();

  // Second frame result. No dirty rects.
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Attach FBO texture to actor
  DALI_TEST_GREATER(actor.GetRendererCount(), 0u, TEST_LOCATION);
  Renderer renderer = actor.GetRendererAt(0);
  DALI_TEST_CHECK(renderer);
  TextureSet textureSet = TextureSet::New();
  DALI_TEST_CHECK(textureSet);
  textureSet.SetTexture(0u, frameBufferTexture);

  renderer.SetTextures(textureSet);

  // Next frame after FBO. actor is drty always.
  for(int i = 0; i < 10; ++i)
  {
    application.SendNotification();
    application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

    clippingRect = Rect<int>(16, 768, 32, 32);
    DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
    DirtyRectChecker(damagedRects, {clippingRect}, true, TEST_LOCATION);
    DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

    application.RenderWithPartialUpdate(damagedRects, clippingRect);
    DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
    DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
    DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
    DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

    damagedRects.clear();
  }

  // Remove framebuffer texture
  TextureSet newTextureSet = CreateTextureSet(Pixel::RGBA8888, 2, 2);
  renderer.SetTextures(newTextureSet);

  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  clippingRect = Rect<int>(16, 768, 32, 32);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  DirtyRectChecker(damagedRects, {clippingRect}, true, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();

  // Second frame after remove fbo. No dirty rects.
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Set invalid viewport of the render task
  newTask.SetFrameBuffer(FrameBuffer());
  newTask.SetViewportSize(Vector2(-100.0f, -100.0f));

  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  // Full update because the task viewport is invalid
  clippingRect = Rect<int>(0, 0, TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  newTask.SetViewportSize(Vector2(0.0f, 0.0f));

  // Change orientation of offscreen camera
  cameraActor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(90.0f), Vector3::XAXIS));

  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  // Full update because the camera orientation is changed
  clippingRect = Rect<int>(0, 0, TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Change camera target
  cameraActor.SetTargetPosition(Vector3(10.0f, 10.0f, 0.0f));

  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  // Full update because the camera is moved
  clippingRect = Rect<int>(0, 0, TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  END_TEST;
}

int UtcDaliRenderTaskRenderPassTag(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask with RenderPassTag");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  Actor blue                                 = Actor::New();
  blue[Dali::Actor::Property::NAME]          = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  blue[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  blue[Dali::Actor::Property::SIZE]          = Vector2(300, 300);
  blue[Dali::Actor::Property::POSITION]      = Vector2(0, 0);

  Geometry geometry = Geometry::New();

  Property::Map map[2];
  map[0]["vertex"]        = SHADER_COLOR_TEST_SHADER_VERT1.data();
  map[0]["fragment"]      = SHADER_COLOR_TEST_SHADER_FRAG.data();
  map[0]["renderPassTag"] = 0;

  map[1]["vertex"]        = SHADER_COLOR_TEST_SHADER_VERT2.data();
  map[1]["fragment"]      = SHADER_COLOR_TEST_SHADER_FRAG.data();
  map[1]["renderPassTag"] = 1;

  Property::Array array;
  array.PushBack(map[0]);
  array.PushBack(map[1]);

  Shader   shader   = Shader::New(array);
  Renderer renderer = Renderer::New(geometry, shader);
  blue.AddRenderer(renderer);

  stage.Add(blue);

  auto& gfx = application.GetGraphicsController();
  gfx.mCallStack.EnableLogging(true);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  DALI_TEST_EQUALS(0u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);
  // Render and notify
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(gfx.mCallStack.FindMethod("CreatePipeline"));
  gfx.mCallStack.Reset();
  DALI_TEST_EQUALS(0u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);

  renderTaskList.GetTask(0u).SetRenderPassTag(1u);
  DALI_TEST_EQUALS(1u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);
  // Render and notify
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(gfx.mCallStack.FindMethod("CreatePipeline"));
  gfx.mCallStack.Reset();
  DALI_TEST_EQUALS(1u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);

  renderTaskList.GetTask(0u).SetRenderPassTag(0u);
  DALI_TEST_EQUALS(0u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);
  // Render and notify
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(!gfx.mCallStack.FindMethod("CreatePipeline"));
  gfx.mCallStack.Reset();
  DALI_TEST_EQUALS(0u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);

  renderTaskList.GetTask(0u).SetRenderPassTag(1u);
  DALI_TEST_EQUALS(1u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);
  // Render and notify
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(!gfx.mCallStack.FindMethod("CreatePipeline"));
  gfx.mCallStack.Reset();
  DALI_TEST_EQUALS(1u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskWithWrongShaderData01(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask with wrong shader data");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  Actor blue                                 = Actor::New();
  blue[Dali::Actor::Property::NAME]          = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  blue[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  blue[Dali::Actor::Property::SIZE]          = Vector2(300, 300);
  blue[Dali::Actor::Property::POSITION]      = Vector2(0, 0);

  Geometry geometry = Geometry::New();

  Shader   shader   = Shader::New(Property::Value(10.0f));
  Renderer renderer = Renderer::New(geometry, shader);
  blue.AddRenderer(renderer);

  stage.Add(blue);

  auto& gfx = application.GetGraphicsController();

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  DALI_TEST_EQUALS(0u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);
  // Render and notify
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(!gfx.mCallStack.FindMethod("CreatePipeline"));
  gfx.mCallStack.Reset();
  DALI_TEST_EQUALS(0u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskWithWrongShaderData02(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask with wrong shader data. This UTC is for line coverage");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  Actor blue                                 = Actor::New();
  blue[Dali::Actor::Property::NAME]          = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  blue[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  blue[Dali::Actor::Property::SIZE]          = Vector2(300, 300);
  blue[Dali::Actor::Property::POSITION]      = Vector2(0, 0);

  Geometry geometry = Geometry::New();

  Shader   validShader = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer    = Renderer::New(geometry, validShader);
  blue.AddRenderer(renderer);

  stage.Add(blue);

  auto& gfx = application.GetGraphicsController();

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  DALI_TEST_EQUALS(0u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);
  // Render and notify
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(gfx.mCallStack.FindMethod("CreatePipeline"));
  gfx.mCallStack.Reset();
  DALI_TEST_EQUALS(0u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);

  // Change the shader as invalid.
  Shader invalidShader = Shader::New(Property::Value(10.0f));
  renderer.SetShader(invalidShader);

  // Render and notify
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(!gfx.mCallStack.FindMethod("CreatePipeline"));
  gfx.mCallStack.Reset();
  DALI_TEST_EQUALS(0u, renderTaskList.GetTask(0u).GetRenderPassTag(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskOrderIndex01(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask with OrderIndex");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     renderTask1    = renderTaskList.CreateTask();

  application.SendNotification();
  int32_t answer1[2] = {INT32_MIN, 0};
  DALI_TEST_EQUALS(2, renderTaskList.GetTaskCount(), TEST_LOCATION);
  for(uint32_t i = 0; i < 2; ++i)
  {
    DALI_TEST_EQUALS(answer1[i], renderTaskList.GetTask(i).GetOrderIndex(), TEST_LOCATION);
  }

  RenderTask renderTask2 = renderTaskList.CreateTask();
  application.SendNotification();
  int32_t answer2[3] = {INT32_MIN, 0, 0};
  DALI_TEST_EQUALS(3, renderTaskList.GetTaskCount(), TEST_LOCATION);
  for(uint32_t i = 0; i < 3; ++i)
  {
    DALI_TEST_EQUALS(answer2[i], renderTaskList.GetTask(i).GetOrderIndex(), TEST_LOCATION);
  }

  RenderTask renderTask3 = renderTaskList.CreateTask();
  application.SendNotification();
  int32_t answer3[4] = {INT32_MIN, 0, 0, 0};
  DALI_TEST_EQUALS(4, renderTaskList.GetTaskCount(), TEST_LOCATION);
  for(uint32_t i = 0; i < 4; ++i)
  {
    DALI_TEST_EQUALS(answer3[i], renderTaskList.GetTask(i).GetOrderIndex(), TEST_LOCATION);
  }

  renderTask1.SetOrderIndex(3);
  application.SendNotification();
  int32_t answer4[4] = {INT32_MIN, 0, 0, 3};
  for(uint32_t i = 0; i < 4; ++i)
  {
    DALI_TEST_EQUALS(answer4[i], renderTaskList.GetTask(i).GetOrderIndex(), TEST_LOCATION);
  }

  renderTask2.SetOrderIndex(7);
  application.SendNotification();
  int32_t answer5[4] = {INT32_MIN, 0, 3, 7};
  for(uint32_t i = 0; i < 4; ++i)
  {
    DALI_TEST_EQUALS(answer5[i], renderTaskList.GetTask(i).GetOrderIndex(), TEST_LOCATION);
  }

  Dali::Integration::Scene scene = application.GetScene();
  scene.GetOverlayLayer();
  application.SendNotification();
  DALI_TEST_EQUALS(5, renderTaskList.GetTaskCount(), TEST_LOCATION);
  int32_t answer6[5] = {INT32_MIN, 0, 3, 7, INT32_MAX};
  for(uint32_t i = 0; i < 5; ++i)
  {
    DALI_TEST_EQUALS(answer6[i], renderTaskList.GetTask(i).GetOrderIndex(), TEST_LOCATION);
  }

  renderTask3.SetOrderIndex(4);
  application.SendNotification();
  int32_t answer7[5] = {INT32_MIN, 3, 4, 7, INT32_MAX};
  for(uint32_t i = 0; i < 5; ++i)
  {
    DALI_TEST_EQUALS(answer7[i], renderTaskList.GetTask(i).GetOrderIndex(), TEST_LOCATION);
  }

  renderTask2.SetOrderIndex(2);
  application.SendNotification();
  int32_t answer8[5] = {INT32_MIN, 2, 3, 4, INT32_MAX};
  for(uint32_t i = 0; i < 5; ++i)
  {
    DALI_TEST_EQUALS(answer8[i], renderTaskList.GetTask(i).GetOrderIndex(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRenderTaskOrderIndex02(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask with OrderIndex");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     renderTask1    = renderTaskList.CreateTask();
  application.SendNotification();
  DALI_TEST_EQUALS(renderTask1, renderTaskList.GetTask(1u), TEST_LOCATION);

  RenderTask renderTask2 = renderTaskList.CreateTask();
  application.SendNotification();
  DALI_TEST_EQUALS(renderTask1, renderTaskList.GetTask(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(renderTask2, renderTaskList.GetTask(2u), TEST_LOCATION);

  RenderTask renderTask3 = renderTaskList.CreateTask();
  application.SendNotification();
  DALI_TEST_EQUALS(renderTask1, renderTaskList.GetTask(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(renderTask2, renderTaskList.GetTask(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(renderTask3, renderTaskList.GetTask(3u), TEST_LOCATION);

  RenderTask renderTask4 = renderTaskList.CreateTask();
  application.SendNotification();
  DALI_TEST_EQUALS(renderTask1, renderTaskList.GetTask(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(renderTask2, renderTaskList.GetTask(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(renderTask3, renderTaskList.GetTask(3u), TEST_LOCATION);
  DALI_TEST_EQUALS(renderTask4, renderTaskList.GetTask(4u), TEST_LOCATION);

  renderTask2.SetOrderIndex(2);
  application.SendNotification();
  DALI_TEST_EQUALS(renderTask1, renderTaskList.GetTask(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(renderTask3, renderTaskList.GetTask(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(renderTask4, renderTaskList.GetTask(3u), TEST_LOCATION);
  DALI_TEST_EQUALS(renderTask2, renderTaskList.GetTask(4u), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskGetRenderTaskId(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask Id get");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  RenderTaskList renderTaskList = stage.GetRenderTaskList();

  RenderTask renderTask1 = renderTaskList.CreateTask();
  RenderTask renderTask2 = renderTaskList.CreateTask();
  RenderTask renderTask3 = renderTaskList.CreateTask();

  DALI_TEST_CHECK(renderTask1.GetRenderTaskId() != 0u);
  DALI_TEST_CHECK(renderTask2.GetRenderTaskId() != 0u);
  DALI_TEST_CHECK(renderTask3.GetRenderTaskId() != 0u);

  DALI_TEST_CHECK(renderTask1.GetRenderTaskId() != renderTask2.GetRenderTaskId());
  DALI_TEST_CHECK(renderTask2.GetRenderTaskId() != renderTask3.GetRenderTaskId());
  DALI_TEST_CHECK(renderTask3.GetRenderTaskId() != renderTask1.GetRenderTaskId());

  END_TEST;
}

int UtcDaliRenderTaskDestructWorkerThreadN(void)
{
  TestApplication application;
  tet_infoline("UtcDaliRenderTaskDestructWorkerThreadN Test, for line coverage");

  try
  {
    class TestThread : public Thread
    {
    public:
      virtual void Run()
      {
        tet_printf("Run TestThread\n");
        // Destruct at worker thread.
        mRenderTask.Reset();
      }

      Dali::RenderTask mRenderTask;
    };
    TestThread thread;
    Stage      stage = Stage::GetCurrent();
    Vector2    stageSize(stage.GetSize());

    RenderTaskList renderTaskList = stage.GetRenderTaskList();

    RenderTask renderTask = renderTaskList.CreateTask();

    renderTaskList.RemoveTask(renderTask);

    thread.mRenderTask = std::move(renderTask);
    renderTask.Reset();

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

int UtcDaliRenderTaskKeepRenderResult(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SignalFinished()");

  CameraActor offscreenCameraActor = CameraActor::New();

  application.GetScene().Add(offscreenCameraActor);

  Texture image     = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 10, 10);
  Actor   rootActor = CreateRenderableActor(image);
  rootActor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  application.GetScene().Add(rootActor);

  RenderTaskList taskList           = application.GetScene().GetRenderTaskList();
  Texture        frameBufferTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, 10, 10);
  FrameBuffer    frameBuffer        = FrameBuffer::New(frameBufferTexture.GetWidth(), frameBufferTexture.GetHeight());
  frameBuffer.AttachColorTexture(frameBufferTexture);

  RenderTask newTask = taskList.CreateTask();
  newTask.SetCameraActor(offscreenCameraActor);
  newTask.SetSourceActor(rootActor);
  newTask.SetInputEnabled(false);
  newTask.SetClearColor(Vector4(0.f, 0.f, 0.f, 0.f));
  newTask.SetClearEnabled(true);
  newTask.SetExclusive(true);
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  newTask.SetFrameBuffer(frameBuffer);
  newTask.KeepRenderResult();

  bool               finished = false;
  RenderTaskFinished renderTaskFinished(finished);
  newTask.FinishedSignal().Connect(&application, renderTaskFinished);

  DALI_TEST_CHECK(!newTask.GetRenderResult());

  // Flush the queue and render.
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK(finished);
  DALI_TEST_CHECK(newTask.GetRenderResult());

  newTask.ClearRenderResult();

  DALI_TEST_CHECK(newTask.GetRenderResult());

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!newTask.GetRenderResult());

  END_TEST;
}

int UtcDaliRenderTaskSetGetRenderedScaleFactor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetRenderedScaleFactor() and GetRenderedScaleFactor()");

  // Create a render task
  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.CreateTask();

  // Default value should be Vector2::ONE
  Vector2 defaultScale = task.GetRenderedScaleFactor();
  DALI_TEST_EQUALS(defaultScale, Vector2::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), Vector2::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), Vector2::ONE, TEST_LOCATION);

  // Test setting and getting a new scale factor
  Vector2 testScale(0.5f, 0.5f);
  task.SetRenderedScaleFactor(testScale);
  DALI_TEST_EQUALS(task.GetRenderedScaleFactor(), testScale, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), testScale, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), testScale, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Test setting and getting another scale factor
  Vector2 anotherTestScale(1.5f, 2.0f);
  task.SetProperty(RenderTask::Property::RENDERED_SCALE_FACTOR, anotherTestScale);
  DALI_TEST_EQUALS(task.GetRenderedScaleFactor(), anotherTestScale, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), anotherTestScale, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), anotherTestScale, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Test setting zero scale (should be allowed)
  Vector2 zeroScale(0.0f, 0.0f);
  task.SetRenderedScaleFactor(zeroScale);
  DALI_TEST_EQUALS(task.GetRenderedScaleFactor(), zeroScale, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), zeroScale, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), zeroScale, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Test setting negative scale (should be allowed)
  Vector2 negativeScale(-1.0f, -2.0f);
  task.SetRenderedScaleFactor(negativeScale);
  DALI_TEST_EQUALS(task.GetRenderedScaleFactor(), negativeScale, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), negativeScale, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), negativeScale, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Test setting float type value (single float value allowed)
  float   singleFloat = 4.0f;
  Vector2 singleFloatV2(singleFloat, singleFloat);
  task.SetProperty(RenderTask::Property::RENDERED_SCALE_FACTOR, singleFloat);
  DALI_TEST_EQUALS(task.GetRenderedScaleFactor(), singleFloatV2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), singleFloatV2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), singleFloatV2, TEST_LOCATION);

  // Test setting non-scalar type value (value should not be updated)
  Property::Array invalidArray;
  task.SetProperty(RenderTask::Property::RENDERED_SCALE_FACTOR, invalidArray);
  DALI_TEST_EQUALS(task.GetRenderedScaleFactor(), singleFloatV2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), singleFloatV2, TEST_LOCATION);
  DALI_TEST_EQUALS(task.GetCurrentProperty<Vector2>(RenderTask::Property::RENDERED_SCALE_FACTOR), singleFloatV2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderTaskExclusiveAddCacheRenderer(void)
{
  TestApplication application;

  tet_infoline("Testing that exclusive RenderTask with AddCacheRenderer calls draw, and still draws after RenderTask removed");

  // Setup GL abstraction for tracking draw calls
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Create scene and actors
  Integration::Scene stage    = application.GetScene();
  RenderTaskList     taskList = stage.GetRenderTaskList();

  // Create a renderable actor
  Texture image           = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  Actor   renderableActor = CreateRenderableActor(image);
  renderableActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  stage.Add(renderableActor);

  // Create camera for offscreen rendering
  CameraActor offscreenCameraActor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  stage.Add(offscreenCameraActor);

  // Create framebuffer for offscreen rendering
  FrameBuffer frameBuffer        = FrameBuffer::New(100, 100);
  Texture     frameBufferTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  frameBuffer.AttachColorTexture(frameBufferTexture);

  // Create exclusive render task
  RenderTask exclusiveTask = taskList.CreateTask();
  exclusiveTask.SetCameraActor(offscreenCameraActor);
  exclusiveTask.SetSourceActor(renderableActor);
  exclusiveTask.SetInputEnabled(false);
  exclusiveTask.SetClearColor(Vector4(0.f, 0.f, 0.f, 0.f));
  exclusiveTask.SetClearEnabled(true);
  exclusiveTask.SetExclusive(true);
  exclusiveTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
  exclusiveTask.SetFrameBuffer(frameBuffer);

  // Add cache renderer to the exclusive task
  Shader   cachedShader  = CreateShader();
  Geometry quadGeometry  = CreateQuadGeometry();
  Renderer cacheRenderer = Renderer::New(quadGeometry, cachedShader);
  DALI_TEST_CHECK(cacheRenderer);
  TextureSet textureSet = TextureSet::New();
  DALI_TEST_CHECK(textureSet);
  textureSet.SetTexture(0u, frameBufferTexture);
  cacheRenderer.SetTextures(textureSet);
  renderableActor.AddCacheRenderer(cacheRenderer);

  // Initial render - should draw the actor
  application.SendNotification();
  application.Render();

  // Check that draw was called for the exclusive task
  DALI_TEST_CHECK(drawTrace.FindMethod("DrawElements") || drawTrace.FindMethod("DrawArrays"));
  int initialDrawCount = drawTrace.CountMethod("DrawElements") + drawTrace.CountMethod("DrawArrays");
  DALI_TEST_GREATER(initialDrawCount, 0, TEST_LOCATION);

  drawTrace.Reset();

  // Second render - should still draw due to REFRESH_ALWAYS
  application.SendNotification();
  application.Render();

  // Check that draw was called again
  int secondDrawCount = drawTrace.CountMethod("DrawElements") + drawTrace.CountMethod("DrawArrays");
  DALI_TEST_GREATER(secondDrawCount, 0, TEST_LOCATION);

  drawTrace.Reset();

  // Remove the exclusive render task
  renderableActor.RemoveCacheRenderer(cacheRenderer);
  taskList.RemoveTask(exclusiveTask);

  // Render after removing the exclusive task - should still draw the cache renderer
  application.SendNotification();
  application.Render();

  // Check that draw is still called for the cache renderer even after task removal
  int afterRemovalDrawCount = drawTrace.CountMethod("DrawElements") + drawTrace.CountMethod("DrawArrays");
  DALI_TEST_GREATER(afterRemovalDrawCount, 0, TEST_LOCATION);

  tet_printf("Initial draws: %d, Second draws: %d, After removal draws: %d\n", initialDrawCount, secondDrawCount, afterRemovalDrawCount);

  END_TEST;
}
