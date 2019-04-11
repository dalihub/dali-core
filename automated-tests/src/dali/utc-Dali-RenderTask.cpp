/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali-test-suite-utils.h>
#include <dali/integration-api/debug.h>
#include <test-native-image.h>

#include <mesh-builder.h>

#define BOOLSTR(x) ((x)?"T":"F")

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

const int RENDER_FRAME_INTERVAL = 16;                           ///< Duration of each frame in ms. (at approx 60FPS)

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

  for(int i = 0; i <= ( duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
}

struct RenderTaskFinished
{
  RenderTaskFinished( bool& finished )
  : finished( finished )
  {
  }

  void operator()( RenderTask& renderTask )
  {
    finished = true;
  }

  bool& finished;
};

struct RenderTaskFinishedRemoveSource
{
  RenderTaskFinishedRemoveSource( bool& finished )
  : finished( finished ),
    finishedOnce(false)
  {
  }

  void operator()( RenderTask& renderTask )
  {
    DALI_TEST_CHECK(finishedOnce == false);
    finished = true;
    finishedOnce = true;
    Actor srcActor = renderTask.GetSourceActor();
    UnparentAndReset(srcActor);
  }

  bool& finished;
  bool finishedOnce;
};

struct RenderTaskFinishedRenderAgain
{
  RenderTaskFinishedRenderAgain( bool& finished )
  : finished( finished ),
    finishedOnce(false)
  {
  }

  void operator()( RenderTask& renderTask )
  {
    DALI_TEST_CHECK(finishedOnce == false);
    finished = true;
    finishedOnce = true;
    renderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  }

  bool& finished;
  bool finishedOnce;
};


bool TestScreenToFrameBufferFunction( Vector2& coordinates )
{
  coordinates = coordinates + Vector2( 1, 2 );

  return true;
}

Actor CreateRenderableActorSuccess(TestApplication& application, std::string filename)
{
  PrepareResourceImage( application, 80u, 80u, Pixel::RGBA8888 );
  Image image = ResourceImage::New(filename);
  Actor actor = CreateRenderableActor(image);
  actor.SetSize( 80, 80 );
  return actor;
}

Actor CreateRenderableActorFailed(TestApplication& application, std::string filename)
{
  Image image = ResourceImage::New(filename);
  DALI_TEST_CHECK( image );
  Actor actor = CreateRenderableActor(image);
  actor.SetSize( 80, 80 );
  return actor;
}

Image CreateResourceImage(TestApplication& application, std::string filename)
{
  PrepareResourceImage( application, 80u, 80u, Pixel::RGBA8888 );
  Image image = ResourceImage::New(filename);
  DALI_TEST_CHECK( image );
  return image;
}

RenderTask CreateRenderTask(TestApplication& application,
                            CameraActor offscreenCamera,
                            Actor rootActor,       // Reset default render task to point at this actor
                            Actor secondRootActor, // Source actor
                            unsigned int refreshRate,
                            bool glSync)
{
  // Change main render task to use a different root
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  taskList.GetTask(0u).SetSourceActor( rootActor );

  FrameBufferImage frameBufferImage;
  if( glSync )
  {
    NativeImageInterfacePtr testNativeImagePtr = TestNativeImage::New(10, 10);
    frameBufferImage= FrameBufferImage::New( *(testNativeImagePtr.Get()) );
  }
  else
  {
    frameBufferImage = FrameBufferImage::New( 10, 10 );
  }

  // Don't draw output framebuffer // '

  RenderTask newTask = taskList.CreateTask();
  newTask.SetCameraActor( offscreenCamera );
  newTask.SetSourceActor( secondRootActor );
  newTask.SetInputEnabled( false );
  newTask.SetClearColor( Vector4( 0.f, 0.f, 0.f, 0.f ) );
  newTask.SetClearEnabled( true );
  newTask.SetExclusive( true );
  newTask.SetRefreshRate( refreshRate );
  newTask.SetTargetFrameBuffer( frameBufferImage );
  newTask.SetProperty( RenderTask::Property::REQUIRES_SYNC, glSync );
  return newTask;
}

bool UpdateRender(TestApplication& application, TraceCallStack& callStack, bool testDrawn, bool& finishedSig, bool testFinished, bool testKeepUpdating, int lineNumber )
{
  finishedSig = false;
  callStack.Reset();

  tet_printf("TestApplication::UpdateRender().\n");

  application.Render(16);
  application.SendNotification();

  bool sigPassed = false;
  if( testFinished )
  {
    sigPassed = finishedSig;
  }
  else
  {
    sigPassed = ! finishedSig;
  }

  bool drawResult = callStack.FindMethod("DrawElements") || callStack.FindMethod("DrawArrays");

  bool drawPassed = false;
  if( testDrawn )
  {
    drawPassed = drawResult;
  }
  else
  {
    drawPassed = !drawResult;
  }

  bool keepUpdating = (application.GetUpdateStatus() != 0);
  bool keepUpdatingPassed = false;
  if( testKeepUpdating )
  {
    keepUpdatingPassed = keepUpdating;
  }
  else
  {
    keepUpdatingPassed = !keepUpdating;
  }

  bool result = (sigPassed && drawPassed && keepUpdatingPassed);

  tet_printf("UpdateRender: Expected: Draw:%s Signal:%s KeepUpdating: %s  Actual: Draw:%s  Signal:%s KeepUpdating: %s  %s, line %d\n",
             BOOLSTR(testDrawn), BOOLSTR(testFinished), BOOLSTR(testKeepUpdating),
             BOOLSTR(drawResult), BOOLSTR(finishedSig), BOOLSTR(keepUpdating),
             result ? "Passed":"Failed",
             lineNumber );

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

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  BaseHandle base = taskList.GetTask( 0u );
  DALI_TEST_CHECK( base );

  RenderTask task = RenderTask::DownCast( base );
  DALI_TEST_CHECK( task );

  // Try calling a method
  DALI_TEST_CHECK( task.GetSourceActor() );
  END_TEST;
}

int UtcDaliRenderTaskDownCast02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::DownCast()");

  Actor actor = Actor::New();

  RenderTask task = RenderTask::DownCast( actor );
  DALI_TEST_CHECK( ! task );

  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorN(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::SetSourceActor() Negative - try with empty actor handle");
  Stage stage = Stage::GetCurrent();

  Actor srcActor;

  RenderTaskList taskList = stage.GetRenderTaskList();
  RenderTask renderTask = taskList.CreateTask();
  renderTask.SetSourceActor(srcActor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( ! renderTask.GetSourceActor() );
  END_TEST;
}


int UtcDaliRenderTaskSetSourceActorP01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor() Positive - check that setting a non-renderable actor stops existing source actor being rendered ");

  Stage stage = Stage::GetCurrent();
  RenderTaskList taskList = stage.GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK( actor );

  BufferImage img = BufferImage::New( 1,1 );
  Actor newActor = CreateRenderableActor( img );
  newActor.SetSize(1,1);
  stage.Add( newActor );

  Actor nonRenderableActor = Actor::New();
  stage.Add( nonRenderableActor );

  // Stop the newActor from being rendered by changing the source actor
  DALI_TEST_CHECK( nonRenderableActor );
  task.SetSourceActor( nonRenderableActor );
  DALI_TEST_CHECK( task.GetSourceActor() != actor );
  DALI_TEST_CHECK( task.GetSourceActor() == nonRenderableActor );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render nothing!
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that nothing was rendered
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliRenderTaskSetSourceActorP02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor() Positive - check that switching source from a non-renderable to a renderable actor causes the texture to be drawn");

  Stage stage = Stage::GetCurrent();

  RenderTaskList taskList = stage.GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK( actor );


  BufferImage img = BufferImage::New( 1,1 );
  Actor newActor = CreateRenderableActor( img );
  newActor.SetSize(1,1);
  stage.Add( newActor );

  Actor nonRenderableActor = Actor::New();
  stage.Add( nonRenderableActor );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Stop the newActor from being rendered by changing the source actor
  DALI_TEST_CHECK( nonRenderableActor );
  task.SetSourceActor( nonRenderableActor );
  DALI_TEST_CHECK( task.GetSourceActor() != actor );
  DALI_TEST_CHECK( task.GetSourceActor() == nonRenderableActor );

  // Update & Render nothing!
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that nothing was rendered
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION );
  drawTrace.Reset();

  // Set newActor as the new source Actor
  task.SetSourceActor( newActor );
  DALI_TEST_CHECK( task.GetSourceActor() != actor );
  DALI_TEST_CHECK( task.GetSourceActor() == newActor );

  // Update & Render the newActor
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the newActor was rendered
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorOffStage(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor (on/off stage testing)");

  Stage stage = Stage::GetCurrent();
  RenderTaskList taskList = stage.GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  BufferImage img = BufferImage::New( 1,1 );
  Actor newActor = CreateRenderableActor( img );
  newActor.SetSize(1,1);
  task.SetSourceActor( newActor );
  // Don't add newActor to stage yet   //'

  // Update & Render with the actor initially off-stage
  application.SendNotification();
  application.Render();

  // Check that nothing was rendered
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION );

  // Now add to stage
  stage.Add( newActor );

  // Update & Render with the actor on-stage
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the newActor was rendered
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION );
  drawTrace.Reset();

  // Now remove from stage
  stage.Remove( newActor );

  // Update & Render with the actor off-stage
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorEmpty(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor (empty handle case)");

  Stage stage = Stage::GetCurrent();
  RenderTaskList taskList = stage.GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK( actor );

  BufferImage img = BufferImage::New( 1,1 );
  Actor newActor = CreateRenderableActor( img );
  newActor.SetSize(1,1);
  stage.Add( newActor );

  Actor nonRenderableActor = Actor::New();
  stage.Add( nonRenderableActor );

  // Set with empty handle
  task.SetSourceActor( Actor() );
  DALI_TEST_CHECK( ! task.GetSourceActor() );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render nothing!
  application.SendNotification();
  application.Render();

  // Check that nothing was rendered
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION );

  // Set with non-empty handle
  task.SetSourceActor( newActor );
  DALI_TEST_CHECK( task.GetSourceActor() == newActor );

  // Update & Render the newActor
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the newActor was rendered
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRenderTaskGetSourceActorP01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetSourceActor() Check the default render task has a valid source actor");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK( actor );

  // By default the entire scene should be rendered
  Actor root = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_CHECK( root == actor );
  END_TEST;
}

int UtcDaliRenderTaskGetSourceActorP02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetSourceActor() Create a new render task, Add a new actor to the stage and set it as the source of the new render task. Get its source actor and check that it is equivalent to what was set.");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.CreateTask();
  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  task.SetSourceActor( actor );

  DALI_TEST_EQUALS( actor, task.GetSourceActor(), TEST_LOCATION );

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
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRenderTaskSetExclusive(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetExclusive() Check that exclusion works");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // Manipulate the GenTextures behaviour, to identify different actors

  std::vector<GLuint> ids;
  ids.push_back( 8 ); // 8 = actor1
  ids.push_back( 9 ); // 9 = actor2
  ids.push_back( 10 ); // 10 = actor3
  application.GetGlAbstraction().SetNextTextureIds( ids );

  BufferImage img1 = BufferImage::New( 1,1 );
  Actor actor1 = CreateRenderableActor( img1 );
  actor1.SetSize(1,1);
  Stage::GetCurrent().Add( actor1 );

  // Update & Render actor1
  application.SendNotification();
  application.Render();

  // Check that the actor1 was rendered
  const std::vector<GLuint>& boundTextures = application.GetGlAbstraction().GetBoundTextures( GL_TEXTURE0 );
  DALI_TEST_GREATER( boundTextures.size(), static_cast<std::vector<GLuint>::size_type>( 0 ), TEST_LOCATION );

  if ( boundTextures.size() )
  {
    int c = 0;
    DALI_TEST_EQUALS( boundTextures[c++], 8u/*unique to actor1*/, TEST_LOCATION );
    if( boundTextures.size() > 1 )
    {
      DALI_TEST_EQUALS( boundTextures[c++], 8u/*unique to actor1*/, TEST_LOCATION );
    }
  }

  BufferImage img2 = BufferImage::New( 1,1 );
  Actor actor2 = CreateRenderableActor( img2 );
  actor2.SetSize(1,1);

  // Force actor2 to be rendered before actor1
  Layer layer = Layer::New();
  Stage::GetCurrent().Add( layer );
  layer.Add( actor2 );
  layer.LowerToBottom();

  // Update & Render
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the actors were rendered
  DALI_TEST_GREATER( boundTextures.size(), static_cast<std::vector<GLuint>::size_type>( 1 ), TEST_LOCATION );

  if ( boundTextures.size() )
  {
    int c = 0;
    DALI_TEST_EQUALS( boundTextures[c++], 9u/*unique to actor2*/, TEST_LOCATION );
    if( boundTextures.size() > 2 )
    {
      DALI_TEST_EQUALS( boundTextures[c++], 9u/*unique to actor1*/, TEST_LOCATION );
    }
    DALI_TEST_EQUALS( boundTextures[c++], 8u/*unique to actor1*/, TEST_LOCATION );
  }

  BufferImage img3 = BufferImage::New( 1,1 );
  Actor actor3 = CreateRenderableActor( img3 );
  actor3.SetSize(1,1);

  // Force actor3 to be rendered before actor2
  layer = Layer::New();
  Stage::GetCurrent().Add( layer );
  layer.Add( actor3 );
  layer.LowerToBottom();

  // Update & Render all actors
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the actors were rendered
  DALI_TEST_GREATER( boundTextures.size(), static_cast<std::vector<GLuint>::size_type>( 2 ), TEST_LOCATION );

  if ( boundTextures.size() )
  {
    int c = 0;
    DALI_TEST_EQUALS( boundTextures[c++], 10u/*unique to actor3*/, TEST_LOCATION );
    if( boundTextures.size() > 3 )
    {
      DALI_TEST_EQUALS( boundTextures[c++], 10u/*unique to actor2*/, TEST_LOCATION );
    }
    DALI_TEST_EQUALS( boundTextures[c++], 9u/*unique to actor2*/, TEST_LOCATION );
    DALI_TEST_EQUALS( boundTextures[c++], 8u/*unique to actor1*/, TEST_LOCATION );
  }

  // Both actors are now connected to the root node
  // Setup 2 render-tasks - the first will render from the root-node, and the second from actor2

  // Not exclusive is the default
  RenderTask task1 = taskList.GetTask( 0u );
  DALI_TEST_CHECK( false == task1.IsExclusive() );

  RenderTask task2 = taskList.CreateTask();
  DALI_TEST_CHECK( false == task2.IsExclusive() );
  task2.SetSourceActor( actor2 );

  // Task1 should render all actors, and task 2 should render only actor2

  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( boundTextures.size(), 4u, TEST_LOCATION );

  if ( boundTextures.size() == 4 )
  {
    // Test that task 1 renders actor3, then actor2 & then actor1
    DALI_TEST_CHECK( boundTextures[0] == 10u );
    DALI_TEST_CHECK( boundTextures[1] == 9u );
    DALI_TEST_CHECK( boundTextures[2] == 8u );

    // Test that task 2 renders actor2
    DALI_TEST_EQUALS( boundTextures[3], 9u, TEST_LOCATION );
  }

  // Make actor2 exclusive to task2

  task2.SetExclusive( true );
  DALI_TEST_CHECK( true == task2.IsExclusive() );

  // Task1 should render only actor1, and task 2 should render only actor2

  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( boundTextures.size(), 3u, TEST_LOCATION );
  if ( boundTextures.size() == 3 )
  {
    // Test that task 1 renders actor3 & actor1
    DALI_TEST_CHECK( boundTextures[0] == 10u );
    DALI_TEST_CHECK( boundTextures[1] == 8u );

    // Test that task 2 renders actor2
    DALI_TEST_CHECK( boundTextures[2] == 9u );
  }
  END_TEST;
}

int UtcDaliRenderTaskSetExclusive02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetExclusive() Check that changing from exclusive to not-exclusive works");

  std::vector<GLuint> ids;
  ids.push_back( 8 ); // 8 = actor1
  application.GetGlAbstraction().SetNextTextureIds( ids );

  BufferImage img1 = BufferImage::New( 1,1 );
  Actor actor1 = CreateRenderableActor( img1 );
  actor1.SetSize(1,1);
  Stage::GetCurrent().Add( actor1 );

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.CreateTask();

  task.SetSourceActor( actor1 );
  task.SetExclusive(true); // Actor should only render once

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  // Update & Render actor1
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION );

  // Set task to non-exclusive - actor1 should render twice:
  drawTrace.Reset();
  task.SetExclusive(false);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 2, TEST_LOCATION );

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
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskIsExclusive01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::IsExclusive() Check default values are non-exclusive");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // Not exclusive is the default
  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_CHECK( false == task.IsExclusive() );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK( false == newTask.IsExclusive() );

  END_TEST;
}

int UtcDaliRenderTaskIsExclusive02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::IsExclusive() Check the getter returns set values");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // Not exclusive is the default
  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_EQUALS( newTask.IsExclusive(), false, TEST_LOCATION );

  newTask.SetExclusive(true);
  DALI_TEST_EQUALS( newTask.IsExclusive(), true, TEST_LOCATION );
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
    (void) x;
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskSetInputEnabled(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetInputEnabled()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // Input is enabled by default
  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_CHECK( true == task.GetInputEnabled() );

  task.SetInputEnabled( false );
  DALI_TEST_CHECK( false == task.GetInputEnabled() );

  task.SetInputEnabled( true );
  DALI_TEST_CHECK( true == task.GetInputEnabled() );
  END_TEST;
}

int UtcDaliRenderTaskGetInputEnabled(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetInputEnabled()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // Input is enabled by default
  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_EQUALS( true, task.GetInputEnabled(), TEST_LOCATION );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_EQUALS( true, newTask.GetInputEnabled(), TEST_LOCATION );

  newTask.SetInputEnabled(false);
  DALI_TEST_EQUALS( false, newTask.GetInputEnabled(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliRenderTaskSetCameraActorP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetCameraActor()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Actor defaultCameraActor = task.GetCameraActor();
  DALI_TEST_CHECK( defaultCameraActor );

  CameraActor newCameraActor = CameraActor::New();
  DALI_TEST_CHECK( newCameraActor );

  task.SetCameraActor( newCameraActor );
  DALI_TEST_CHECK( task.GetCameraActor() != defaultCameraActor );
  DALI_TEST_EQUALS( task.GetCameraActor(), newCameraActor, TEST_LOCATION );
  END_TEST;
}


int UtcDaliRenderTaskSetCameraActorN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetCameraActor() with empty actor handle");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetCameraActor();
  DALI_TEST_CHECK( actor );

  CameraActor cameraActor;

  task.SetCameraActor( cameraActor );
  DALI_TEST_EQUALS( (bool)task.GetCameraActor(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCameraActor(), cameraActor, TEST_LOCATION );
  END_TEST;
}


int UtcDaliRenderTaskGetCameraActorP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetCameraActor()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  CameraActor actor = task.GetCameraActor();
  DALI_TEST_CHECK( actor );
  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION );
  DALI_TEST_GREATER( actor.GetFieldOfView(), 0.0f, TEST_LOCATION );
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
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRenderTaskSetTargetFrameBufferP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetTargetFrameBuffer()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  FrameBufferImage newImage = FrameBufferImage::New();
  task.SetTargetFrameBuffer( newImage );
  DALI_TEST_CHECK( task.GetTargetFrameBuffer() == newImage );
  END_TEST;
}

int UtcDaliRenderTaskSetTargetFrameBufferN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetTargetFrameBuffer()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );
  FrameBufferImage newImage; // Empty handle
  task.SetTargetFrameBuffer( newImage );
  DALI_TEST_EQUALS( (bool)task.GetTargetFrameBuffer(), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRenderTaskGetTargetFrameBufferP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetTargetFrameBuffer()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask newTask = taskList.CreateTask();
  FrameBufferImage fb = FrameBufferImage::New(128, 128, Pixel::RGBA8888);
  newTask.SetTargetFrameBuffer( fb );
  DALI_TEST_EQUALS( newTask.GetTargetFrameBuffer(), fb, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRenderTaskGetTargetFrameBufferN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetTargetFrameBuffer()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  // By default render-tasks do not render off-screen
  FrameBufferImage image = task.GetTargetFrameBuffer();
  DALI_TEST_CHECK( !image );

  END_TEST;
}

int UtcDaliRenderTaskSetFrameBufferP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetFrameBuffer()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  FrameBuffer newFrameBuffer = FrameBuffer::New( 128u, 128u, FrameBuffer::Attachment::NONE );
  task.SetFrameBuffer( newFrameBuffer );
  DALI_TEST_CHECK( task.GetFrameBuffer() == newFrameBuffer );
  END_TEST;
}

int UtcDaliRenderTaskSetFrameBufferN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetFrameBuffer()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );
  FrameBuffer newFrameBuffer; // Empty handle
  task.SetFrameBuffer( newFrameBuffer );
  DALI_TEST_EQUALS( (bool)task.GetFrameBuffer(), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRenderTaskGetFrameBufferP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetFrameBuffer()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  FrameBuffer newFrameBuffer = FrameBuffer::New( 1u, 1u, FrameBuffer::Attachment::NONE  );
  task.SetFrameBuffer( newFrameBuffer );
  DALI_TEST_CHECK( task.GetFrameBuffer() == newFrameBuffer );
  END_TEST;
}

int UtcDaliRenderTaskGetFrameBufferN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetFrameBuffer()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  // A scene creates frame buffer by default
  FrameBuffer frameBuffer = task.GetFrameBuffer();
  DALI_TEST_CHECK( frameBuffer );

  END_TEST;
}

int UtcDaliRenderTaskSetScreenToFrameBufferFunctionP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetScreenToFrameBufferFunction()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  task.SetScreenToFrameBufferFunction( TestScreenToFrameBufferFunction );

  Vector2 coordinates( 5, 10 );
  Vector2 convertedCoordinates( 6, 12 ); // + Vector(1, 2)

  RenderTask::ScreenToFrameBufferFunction func = task.GetScreenToFrameBufferFunction();
  DALI_TEST_CHECK( func( coordinates ) );
  DALI_TEST_CHECK( coordinates == convertedCoordinates );

  task.SetScreenToFrameBufferFunction( RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION );
  func = task.GetScreenToFrameBufferFunction();
  DALI_TEST_CHECK( func( coordinates ) );

  task.SetScreenToFrameBufferFunction( RenderTask::DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION );
  func = task.GetScreenToFrameBufferFunction();
  DALI_TEST_CHECK( ! func( coordinates ) );
  END_TEST;
}

int UtcDaliRenderTaskSetScreenToFrameBufferFunctionN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetScreenToFrameBufferFunction()");

  RenderTask task; // Empty handle
  try
  {
    task.SetScreenToFrameBufferFunction( TestScreenToFrameBufferFunction );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetScreenToFrameBufferFunctionP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetScreenToFrameBufferFunction()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Vector2 originalCoordinates( 5, 10 );
  Vector2 coordinates( 5, 10 );

  RenderTask::ScreenToFrameBufferFunction func = task.GetScreenToFrameBufferFunction();
  DALI_TEST_CHECK( !func( coordinates ) ); // conversion should fail by default
  DALI_TEST_CHECK( coordinates == originalCoordinates ); // coordinates should not be modified
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
    (void) func;
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}


int UtcDaliRenderTaskGetScreenToFrameBufferMappingActorP(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::GetScreenToFrameBufferMappingActor ");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask renderTask = taskList.CreateTask();
  Actor mappingActor = Actor::New();
  renderTask.SetScreenToFrameBufferMappingActor(mappingActor);

  DALI_TEST_EQUALS( mappingActor, renderTask.GetScreenToFrameBufferMappingActor(), TEST_LOCATION );
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
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetScreenToFrameBufferMappingActor02N(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::GetScreenToFrameBufferMappingActor with empty task handle");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask renderTask = taskList.CreateTask();
  Actor actor;
  renderTask.SetScreenToFrameBufferMappingActor(actor);

  DALI_TEST_EQUALS( (bool)renderTask.GetScreenToFrameBufferMappingActor(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRenderTaskGetViewportP01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetViewport() on default task");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );
  Viewport viewport = task.GetViewport();

  // By default the viewport should match the stage width/height
  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Viewport expectedViewport( 0, 0, stageSize.width, stageSize.height );
  DALI_TEST_CHECK( viewport == expectedViewport );
  END_TEST;
}

int UtcDaliRenderTaskGetViewportP02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetViewport() on new task");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.CreateTask();
  Viewport viewport = task.GetViewport();

  // By default the viewport should match the stage width/height
  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Viewport expectedViewport( 0, 0, stageSize.width, stageSize.height );
  DALI_TEST_CHECK( viewport == expectedViewport );
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
    (void) viewport;
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}


int UtcDaliRenderTaskSetViewportP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetViewport()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );
  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Viewport newViewport( 0, 0, stageSize.width * 0.5f, stageSize.height * 0.5f );
  task.SetViewport( newViewport );

  // Update (viewport is a property)
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( task.GetViewport() == newViewport );
  END_TEST;
}

int UtcDaliRenderTaskSetViewportN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetViewport()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task;
  try
  {
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    Viewport newViewport( 0, 0, stageSize.width * 0.5f, stageSize.height * 0.5f );
    task.SetViewport( newViewport );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }

  END_TEST;
}


int UtcDaliRenderTaskSetViewportPosition(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetViewportPosition()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Viewport viewport = task.GetViewport();

  // By default the viewport should match the stage width/height

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Viewport expectedViewport( 0, 0, stageSize.width, stageSize.height );
  DALI_TEST_CHECK( viewport == expectedViewport );

  // 'Setter' test
  Vector2 newPosition(25.0f, 50.0f);
  task.SetViewportPosition( newPosition );

  // Update (viewport is a property)
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( task.GetCurrentViewportPosition(), newPosition, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  // Set by Property test
  Vector2 newPosition2(32.0f, 32.0f);
  task.SetProperty( RenderTask::Property::VIEWPORT_POSITION, newPosition2 );
  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition2, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition, TEST_LOCATION ); // still the old position

  // Update
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( task.GetCurrentViewportPosition(), newPosition2, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition2, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition2, TEST_LOCATION );

  Vector2 newPosition3(64.0f, 0.0f);
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo( Property( task, RenderTask::Property::VIEWPORT_POSITION ), newPosition3, AlphaFunction::LINEAR );
  animation.Play();

  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition3, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition2, TEST_LOCATION );

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS( task.GetCurrentViewportPosition(), newPosition3, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition3, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition3, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  // Create another animation which animates by a certain value
  const Vector2 newPosition4( 75.0f, 45.0f );
  const Vector2 relativePosition( newPosition4 - newPosition3 );
  animation = Animation::New( 1.0f );
  animation.AnimateBy( Property( task, RenderTask::Property::VIEWPORT_POSITION ), relativePosition );
  animation.Play();

  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition4, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition3, TEST_LOCATION );

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS( task.GetCurrentViewportPosition(), newPosition4, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition4, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_POSITION ), newPosition4, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRenderTaskSetViewportSize(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetViewportSize()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Viewport viewport = task.GetViewport();

  // By default the viewport should match the stage width/height

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Viewport expectedViewport( 0, 0, stageSize.width, stageSize.height );
  DALI_TEST_CHECK( viewport == expectedViewport );

  Vector2 newSize(128.0f, 64.0f);
  task.SetViewportSize( newSize );

  // Update (viewport is a property)
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( task.GetCurrentViewportSize(), newSize, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  // Set by Property test
  Vector2 newSize2(50.0f, 50.0f);
  task.SetProperty( RenderTask::Property::VIEWPORT_SIZE, newSize2 );
  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE ), newSize2, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE ), newSize, TEST_LOCATION ); // still the old position

  // Update
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( task.GetCurrentViewportSize(), newSize2, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE ), newSize2, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE ), newSize2, TEST_LOCATION );

  Vector2 newSize3(10.0f, 10.0f);
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo( Property( task, RenderTask::Property::VIEWPORT_SIZE ), newSize3, AlphaFunction::LINEAR );
  animation.Play();

  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE  ), newSize3, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE ), newSize2, TEST_LOCATION );

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS( task.GetCurrentViewportSize(), newSize3, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE ), newSize3, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE  ), newSize3, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  // Create another animation which animates by a certain value
  const Vector2 newSize4( 75.0f, 45.0f );
  const Vector2 relativeSize( newSize4 - newSize3 );
  animation = Animation::New( 1.0f );
  animation.AnimateBy( Property( task, RenderTask::Property::VIEWPORT_SIZE ), relativeSize );
  animation.Play();

  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE ), newSize4, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE ), newSize3, TEST_LOCATION );

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS( task.GetCurrentViewportSize(), newSize4, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE ), newSize4, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetProperty< Vector2 >( RenderTask::Property::VIEWPORT_SIZE ), newSize4, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRenderTaskSetClearColorP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetClearColor()");

  Vector4 testColor( 1.0f, 2.0f, 3.0f, 4.0f );
  Vector4 testColor2( 5.0f, 6.0f, 7.0f, 8.0f );

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_CHECK( task.GetClearColor() != testColor );

  task.SetClearColor( testColor );

  // Wait a frame.
  Wait(application);

  DALI_TEST_EQUALS( task.GetClearColor(), testColor, TEST_LOCATION );

  task.SetProperty( RenderTask::Property::CLEAR_COLOR, testColor2 );
  DALI_TEST_EQUALS( task.GetProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR ), testColor2, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR ), testColor, TEST_LOCATION ); // still the old color

  // Wait a frame.
  Wait(application);

  DALI_TEST_EQUALS( task.GetClearColor(), testColor2, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR ), testColor2, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR ), testColor2, TEST_LOCATION );

  Vector4 newColor3(10.0f, 10.0f, 20.0f, 30.0f);
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo( Property( task, RenderTask::Property::CLEAR_COLOR ), newColor3, AlphaFunction::LINEAR );
  animation.Play();

  DALI_TEST_EQUALS( task.GetProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR  ), newColor3, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR ), testColor2, TEST_LOCATION );

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR ), newColor3, Math::MACHINE_EPSILON_1, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR  ), newColor3, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  // Create another animation which animates by a certain value
  const Vector4 newColor4( 0.45f, 0.35f, 0.25f, 0.1f );
  const Vector4 relativeColor( newColor4 - newColor3 );
  animation = Animation::New( 1.0f );
  animation.AnimateBy( Property( task, RenderTask::Property::CLEAR_COLOR ), relativeColor );
  animation.Play();

  DALI_TEST_EQUALS( task.GetProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR ), newColor4, Math::MACHINE_EPSILON_10, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR ), newColor3, Math::MACHINE_EPSILON_10, TEST_LOCATION );

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS( task.GetCurrentProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR ), newColor4, Math::MACHINE_EPSILON_10, TEST_LOCATION );
  DALI_TEST_EQUALS( task.GetProperty< Vector4 >( RenderTask::Property::CLEAR_COLOR ), newColor4, Math::MACHINE_EPSILON_10, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRenderTaskSetClearColorN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetClearColor() on empty handle");

  RenderTask task;
  try
  {
    task.SetClearColor( Vector4::ZERO );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetClearColorP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetClearColor()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_EQUALS( task.GetClearColor(), RenderTask::DEFAULT_CLEAR_COLOR, TEST_LOCATION );
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
    (void) color;
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskSetClearEnabledP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetClearEnabled()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_CHECK( !task.GetClearEnabled() ); // defaults to false

  task.SetClearEnabled( true );
  DALI_TEST_EQUALS( task.GetClearEnabled(), true, TEST_LOCATION );

  task.SetClearEnabled( false );
  DALI_TEST_EQUALS( task.GetClearEnabled(), false, TEST_LOCATION );
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
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetClearEnabledP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetClearEnabled()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_CHECK( !task.GetClearEnabled() ); // defaults to false
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
    (void) x;
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskSetCullModeP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetCullMode()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_EQUALS( task.GetCullMode(), true, TEST_LOCATION );

  task.SetCullMode( false );

  DALI_TEST_EQUALS( task.GetCullMode(), false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRenderTaskSetCullModeN(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetCullMode() on empty handle");

  RenderTask task;
  try
  {
    task.SetCullMode( false );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRenderTaskGetCullModeP(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetCullMode()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_EQUALS( task.GetCullMode(), true, TEST_LOCATION );
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
    (void) x;
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "RenderTask handle is empty", TEST_LOCATION);
  }
  END_TEST;
}


int UtcDaliRenderTaskSetRefreshRate(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetRefreshRate()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // By default tasks will be processed every frame
  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_CHECK( RenderTask::REFRESH_ALWAYS == task.GetRefreshRate() );

  task.SetRefreshRate( 2u ); // every-other frame
  DALI_TEST_CHECK( 2u == task.GetRefreshRate() );

  task.SetRefreshRate( RenderTask::REFRESH_ALWAYS );
  DALI_TEST_CHECK( RenderTask::REFRESH_ALWAYS == task.GetRefreshRate() );
  END_TEST;
}

int UtcDaliRenderTaskGetRefreshRate(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetRefreshRate()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // By default tasks will be processed every frame
  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_CHECK( RenderTask::REFRESH_ALWAYS == task.GetRefreshRate() );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK( RenderTask::REFRESH_ALWAYS == newTask.GetRefreshRate() );
  END_TEST;
}

int UtcDaliRenderTaskSignalFinished(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SignalFinished()");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();

  CameraActor offscreenCameraActor = CameraActor::New();

  Stage::GetCurrent().Add( offscreenCameraActor );

  BufferImage image = BufferImage::New( 10, 10 );
  image.Update();
  Actor rootActor = CreateRenderableActor( image );
  rootActor.SetSize( 10, 10 );
  Stage::GetCurrent().Add( rootActor );

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  NativeImageInterfacePtr testNativeImagePtr = TestNativeImage::New(10, 10);
  FrameBufferImage frameBufferImage = FrameBufferImage::New( *testNativeImagePtr.Get() );

  RenderTask newTask = taskList.CreateTask();
  newTask.SetCameraActor( offscreenCameraActor );
  newTask.SetSourceActor( rootActor );
  newTask.SetInputEnabled( false );
  newTask.SetClearColor( Vector4( 0.f, 0.f, 0.f, 0.f ) );
  newTask.SetClearEnabled( true );
  newTask.SetExclusive( true );
  newTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
  newTask.SetTargetFrameBuffer( frameBufferImage );
  newTask.SetProperty( RenderTask::Property::REQUIRES_SYNC, true );

  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );

  // Flush the queue and render.
  application.SendNotification();

  // 1 render to process render task, then wait for sync before finished msg is sent
  // from update to the event thread.

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK( !finished );

  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  application.Render();
  DALI_TEST_EQUALS( (Integration::KeepUpdating::Reasons)(application.GetUpdateStatus() & Integration::KeepUpdating::RENDER_TASK_SYNC), Integration::KeepUpdating::RENDER_TASK_SYNC, TEST_LOCATION );
  application.SendNotification();
  DALI_TEST_CHECK( !finished );

  application.Render();
  DALI_TEST_EQUALS( (Integration::KeepUpdating::Reasons)(application.GetUpdateStatus() & Integration::KeepUpdating::RENDER_TASK_SYNC), Integration::KeepUpdating::RENDER_TASK_SYNC, TEST_LOCATION );
  application.SendNotification();
  DALI_TEST_CHECK( ! finished );

  sync.SetObjectSynced( lastSyncObj, true );

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK( !finished );

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK( finished );

  DALI_TEST_EQUALS( application.GetUpdateStatus(), 0, TEST_LOCATION );
  END_TEST;
}


int UtcDaliRenderTaskContinuous01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous using loading image\nPRE: render task not ready (source actor not staged)\nPOST:continuous renders, no Finished signal");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );

  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  application.SendNotification();

  // START PROCESS/RENDER                     Input,    Expected  Input, Expected, KeepUpdating
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, false,   finished, false, false, __LINE__ ) );
  application.GetPlatform().ClearReadyResources();

  // ADD SOURCE ACTOR TO STAGE - expect continuous renders to start, no finished signal
  Stage::GetCurrent().Add(secondRootActor);
  application.SendNotification();

  // CONTINUE PROCESS/RENDER                  Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false, false, __LINE__ ) );
  END_TEST;
}


int UtcDaliRenderTaskContinuous02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous using loading image\nPRE: render task not ready (source actor not visible)\nPOST:continuous renders, no Finished signal");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );

  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  Stage::GetCurrent().Add(secondRootActor);
  secondRootActor.SetVisible(false);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected, KeepUpdating
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false, false, __LINE__ ) );
  application.GetPlatform().ClearReadyResources();

  // MAKE SOURCE ACTOR VISIBLE - expect continuous renders to start, no finished signal
  secondRootActor.SetVisible(true);
  application.SendNotification();

  // CONTINUE PROCESS/RENDER                 Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, false, __LINE__ ) );
  END_TEST;
}

int UtcDaliRenderTaskContinuous03(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous using loading image\nPRE: render task not ready (camera actor not staged)\nPOST:continuous renders, no Finished signal");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false, false, __LINE__ ) );
  application.GetPlatform().ClearReadyResources();

  // ADD CAMERA ACTOR TO STAGE - expect continuous renders to start, no finished signal
  Stage::GetCurrent().Add( offscreenCameraActor );
  application.SendNotification();

  // CONTINUE PROCESS/RENDER                 Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, false, __LINE__ ) );
  END_TEST;
}


int UtcDaliRenderTaskContinuous04(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous using loaded image");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );
  Actor secondRootActor = CreateRenderableActorFailed(application, "aFile.jpg");
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,   finished, false, false, __LINE__ ) );
  END_TEST;
}

int UtcDaliRenderTaskOnce01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync, using loaded image");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");

  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,   finished, false, true, __LINE__  ) );

  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );
  sync.SetObjectSynced( lastSyncObj, true );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false, true, __LINE__  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true, false, __LINE__  ) );
  END_TEST;
}

int UtcDaliRenderTaskOnce02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync, using Mesh which accesses texture through sampler with loaded image.\n");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );

  Shader shader = CreateShader();
  Image image = CreateResourceImage(application, "aFile.jpg");
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures( textureSet );
  Actor secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetSize(100, 100);
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,   finished, false, true, __LINE__  ) );

  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );
  sync.SetObjectSynced( lastSyncObj, true );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false, true, __LINE__  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true, false, __LINE__  ) );

   END_TEST;
}

int UtcDaliRenderTaskOnce03(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync, using loaded image. Switch from render always after ready to render once\n");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, false, __LINE__ ) );

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,   finished, false, true, __LINE__  ) );

  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );
  sync.SetObjectSynced( lastSyncObj, true );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false, true, __LINE__  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true, false, __LINE__  ) );

  END_TEST;
}


int UtcDaliRenderTaskOnce04(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask Render Once GlSync, using Mesh which accesses texture through sampler with loaded image.\n"
               "Switch from render always after ready to render once\n"
              );

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );

  Shader shader = CreateShader();
  Image image = CreateResourceImage(application, "aFile.jpg");
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures( textureSet );
  Actor secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetSize(100, 100);
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, false, __LINE__ ) );

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,   finished, false, true, __LINE__  ) );

  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );
  sync.SetObjectSynced( lastSyncObj, true );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false, true, __LINE__  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true, false, __LINE__  ) );

  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once, \nPRE: Resources ready\nPOST: Finished signal sent once only");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, true, __LINE__ ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true, false, __LINE__ ) );
  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once, using Mesh which accesses texture through sampler with loaded image.\n"
               "PRE: Resources ready\nPOST: Finished signal sent once only");
  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );

  Shader shader = CreateShader();
  Image image = CreateResourceImage(application, "aFile.jpg");
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures( textureSet );
  Actor secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetSize(100, 100);
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, true, __LINE__ ) );
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true, false, __LINE__ ) );

  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync03(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once, using loaded image. Switch from render always after ready to render once\n"
               "PRE: Render task ready, Image loaded\n"
               "POST: Finished signal sent only once");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );
  Actor secondRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, false, __LINE__ ) );

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification(); //         Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, true, __LINE__ ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true, false, __LINE__ ) );
  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync04(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once, using Mesh which accesses texture through sampler with loading image.\n"
               "Switch from render always after ready to render once\n"
               "PRE: Render task ready, Image not loaded\n"
               "POST: Finished signal sent only once");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );

  Shader shader = CreateShader();
  Image image = CreateResourceImage(application, "aFile.jpg");
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures( textureSet );
  Actor secondRootActor = Actor::New();
  secondRootActor.AddRenderer(renderer);
  secondRootActor.SetSize(100, 100);
  Stage::GetCurrent().Add(secondRootActor);


  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, false, __LINE__ ) );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj == NULL );

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification(); //         Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, true, __LINE__ ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true, false, __LINE__ ) );

  lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj == NULL );

  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync05(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once\n"
               "SetRefreshRate(ONCE), resource load failed, completes render task.\n"
               "PRE: resources failed to load\n"
               "POST: No finished signal sent.");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );
  Actor secondRootActor = CreateRenderableActorFailed(application, "aFile.jpg");
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,     Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, false, __LINE__ ) );

  // CHANGE TO RENDER ONCE,
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false, false, __LINE__ ) );

  END_TEST;
}



int UtcDaliRenderTaskOnceChain01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once Chained render tasks\n"
               "SetRefreshRate(ONCE), resource load completes, both render tasks render.\n"
               "PRE: resources ready\n"
               "POST: 2 finished signals sent.");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor defaultRootActor = Actor::New(); // Root for default RT
  Stage::GetCurrent().Add( defaultRootActor );

  CameraActor offscreenCameraActor = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  Stage::GetCurrent().Add( offscreenCameraActor );
  Actor firstRootActor = CreateRenderableActorSuccess(application, "aFile.jpg");
  Stage::GetCurrent().Add(firstRootActor);

  // first render task
  RenderTask firstTask = CreateRenderTask(application, offscreenCameraActor, defaultRootActor, firstRootActor, RenderTask::REFRESH_ONCE, false);
  bool firstFinished = false;
  RenderTaskFinished renderTask1Finished( firstFinished );
  firstTask.FinishedSignal().Connect( &application, renderTask1Finished );

  // Second render task
  FrameBufferImage fbo = firstTask.GetTargetFrameBuffer();
  Actor secondRootActor = CreateRenderableActor( fbo );
  Stage::GetCurrent().Add(secondRootActor);
  RenderTask secondTask = CreateRenderTask(application, offscreenCameraActor, defaultRootActor, secondRootActor, RenderTask::REFRESH_ONCE, false);
  bool secondFinished = false;
  RenderTaskFinished renderTask2Finished( secondFinished );
  secondTask.FinishedSignal().Connect( &application, renderTask2Finished );

  application.SendNotification();

  //Both render tasks are executed.
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,  firstFinished, false, true, __LINE__ ) );
  DALI_TEST_CHECK( firstFinished == false );
  DALI_TEST_CHECK( secondFinished == false );

  //Nothing else to render and both render task should have finished now
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,  firstFinished, true, false, __LINE__ ) );
  DALI_TEST_CHECK( firstFinished == true );
  DALI_TEST_CHECK( secondFinished == true );

  END_TEST;
}

int UtcDaliRenderTaskProperties(void)
{
  TestApplication application;

  RenderTask task = Stage::GetCurrent().GetRenderTaskList().CreateTask();

  Property::IndexContainer indices;
  task.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.Size() );
  DALI_TEST_EQUALS( indices.Size(), task.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliRenderTaskFinishInvisibleSourceActor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::FinishInvisibleSourceActor()");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();

  CameraActor offscreenCameraActor = CameraActor::New();

  Stage::GetCurrent().Add( offscreenCameraActor );

  BufferImage image = BufferImage::New( 10, 10 );
  Actor rootActor = CreateRenderableActor( image );
  rootActor.SetSize( 10, 10 );
  rootActor.SetVisible(false);
  Stage::GetCurrent().Add( rootActor );

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  NativeImageInterfacePtr testNativeImagePtr = TestNativeImage::New(10, 10);
  FrameBufferImage frameBufferImage = FrameBufferImage::New( *testNativeImagePtr.Get() );

  // Flush all outstanding messages
  application.SendNotification();
  application.Render();

  RenderTask newTask = taskList.CreateTask();
  newTask.SetCameraActor( offscreenCameraActor );
  newTask.SetSourceActor( rootActor );
  newTask.SetInputEnabled( false );
  newTask.SetClearColor( Vector4( 0.f, 0.f, 0.f, 0.f ) );
  newTask.SetClearEnabled( true );
  newTask.SetExclusive( true );
  newTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
  newTask.SetTargetFrameBuffer( frameBufferImage );
  newTask.SetProperty( RenderTask::Property::REQUIRES_SYNC, true );

  // Framebuffer doesn't actually get created until Connected, i.e. by previous line

  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );

  // Flush the queue and render.
  application.SendNotification();

  // 1 render to process render task, then wait for sync before finished msg is sent
  // from update to the event thread.

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK( !finished );

  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  application.Render();
  DALI_TEST_EQUALS( (Integration::KeepUpdating::Reasons)(application.GetUpdateStatus() & Integration::KeepUpdating::RENDER_TASK_SYNC), Integration::KeepUpdating::RENDER_TASK_SYNC, TEST_LOCATION );
  application.SendNotification();
  DALI_TEST_CHECK( !finished );

  application.Render();
  DALI_TEST_EQUALS( (Integration::KeepUpdating::Reasons)(application.GetUpdateStatus() & Integration::KeepUpdating::RENDER_TASK_SYNC), Integration::KeepUpdating::RENDER_TASK_SYNC, TEST_LOCATION );
  application.SendNotification();
  DALI_TEST_CHECK( ! finished );

  sync.SetObjectSynced( lastSyncObj, true );

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK( !finished );

  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK( finished );
  finished = false;

  application.Render(); // Double check no more finished signal
  application.SendNotification();
  DALI_TEST_CHECK( ! finished );

  END_TEST;
}

int UtcDaliRenderTaskFinishMissingImage(void)
{
  TestApplication application;

  // Previously we had bugs where not having a resource ID would cause render-tasks to wait forever
  tet_infoline("Testing RenderTask::SignalFinished() when an Actor has no Image set");

  Stage stage = Stage::GetCurrent();

  BufferImage image = BufferImage::New( 10, 10 );
  Actor rootActor = CreateRenderableActor( image );
  rootActor.SetSize( 10, 10 );
  stage.Add( rootActor );

  Actor actorWithMissingImage = CreateRenderableActor( Image() );
  actorWithMissingImage.SetSize( 10, 10 );
  stage.Add( actorWithMissingImage );

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask newTask = taskList.CreateTask();
  newTask.SetInputEnabled( false );
  newTask.SetClearColor( Vector4( 0.f, 0.f, 0.f, 0.f ) );
  newTask.SetClearEnabled( true );
  newTask.SetExclusive( true );
  newTask.SetRefreshRate( RenderTask::REFRESH_ONCE );

  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );

  // 1 render to process render task, then 1 before finished msg is sent from update to the event thread.
  application.SendNotification();
  application.Render();
  application.Render();

  application.SendNotification();
  DALI_TEST_CHECK( finished );

  END_TEST;
}

int UtcDaliRenderTaskWorldToViewport(void)
{
  TestApplication application( 400u, 400u ); // square surface

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetPosition( Vector3(0.0, 0.0, 0.0) );

  actor.SetParentOrigin( Vector3(0.5, 0.5, 0.5) );
  actor.SetAnchorPoint( Vector3(0.5, 0.5, 0.5) );

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  application.SendNotification();

  RenderTask task = taskList.GetTask( 0u );

  CameraActor camera = task.GetCameraActor();

  Vector2 screenSize = task.GetCurrentViewportSize();

  float screenX = 0.0;
  float screenY = 0.0;

  bool ok = task.WorldToViewport(actor.GetCurrentWorldPosition(), screenX, screenY);
  DALI_TEST_CHECK(ok == true);

  DALI_TEST_EQUALS(screenX, screenSize.x/2, Math::MACHINE_EPSILON_10000, TEST_LOCATION);
  DALI_TEST_EQUALS(screenY, screenSize.y/2, Math::MACHINE_EPSILON_10000, TEST_LOCATION);

  Actor actor2 = Actor::New();
  float actor2Size = 100.f;
  actor2.SetSize( actor2Size, actor2Size );
  actor2.SetPosition( Vector3(0.0, 0.0, 0.0) );
  actor2.SetParentOrigin( Vector3(0.5, 0.5, 0.0) );
  actor2.SetAnchorPoint( Vector3(0.5, 0.5, 0.0) );
  Stage::GetCurrent().Add( actor2 );
  actor2.Add(actor);
  actor.SetParentOrigin( Vector3(0,0,0) );

  application.SendNotification();
  application.Render();
  application.SendNotification();

  ok = task.WorldToViewport(actor.GetCurrentWorldPosition(), screenX, screenY);
  DALI_TEST_CHECK(ok == true);

  DALI_TEST_EQUALS(screenX, screenSize.x/2 - actor2Size/2, Math::MACHINE_EPSILON_10000, TEST_LOCATION);
  DALI_TEST_EQUALS(screenY, screenSize.y/2 - actor2Size/2, Math::MACHINE_EPSILON_10000, TEST_LOCATION);

  END_TEST;
}


int UtcDaliRenderTaskViewportToLocal(void)
{
  TestApplication application;
  Actor actor = Actor::New();
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  actor.SetSize(100.0f, 100.0f);
  actor.SetPosition(10.0f, 10.0f);
  Stage::GetCurrent().Add(actor);

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );

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

  DALI_TEST_CHECK( actor.ScreenToLocal(localX, localY, screenX, screenY) );

  DALI_TEST_CHECK( task.ViewportToLocal(actor, screenX, screenY, rtLocalX, rtLocalY ) );

  DALI_TEST_EQUALS(localX, rtLocalX, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(localY, rtLocalY, 0.01f, TEST_LOCATION);

  END_TEST;

}

int UtcDaliRenderTaskRequiresSync(void)
{
  TestApplication application;
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask newTask = taskList.CreateTask();
  newTask.SetProperty( RenderTask::Property::REQUIRES_SYNC, false );

  DALI_TEST_EQUALS( newTask.GetProperty< bool >( RenderTask::Property::REQUIRES_SYNC ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( newTask.GetCurrentProperty< bool >( RenderTask::Property::REQUIRES_SYNC ), false, TEST_LOCATION );

  newTask.SetProperty( RenderTask::Property::REQUIRES_SYNC, true );

  DALI_TEST_EQUALS( newTask.GetProperty< bool >( RenderTask::Property::REQUIRES_SYNC ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( newTask.GetCurrentProperty< bool >( RenderTask::Property::REQUIRES_SYNC ), true, TEST_LOCATION );

  END_TEST;
}
