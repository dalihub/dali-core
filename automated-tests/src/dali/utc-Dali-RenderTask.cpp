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
#include <dali-test-suite-utils.h>
#include <dali/integration-api/debug.h>

using namespace Dali;

void utc_dali_render_task_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_render_task_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace // unnamed namespace
{

class TestNativeImage : public NativeImage
{
public:
  int mWidth;
  int mHeight;
  TestNativeImage(int width, int height)
  : mWidth(width),
    mHeight(height)
  {}

  virtual bool GlExtensionCreate() {return true;};

  /**
   * Destroy the GL resource for the NativeImage.
   * e.g. For the EglImageKHR extension, this corresponds to calling eglDestroyImageKHR()
   * @pre There is a GL context for the current thread.
   */
  virtual void GlExtensionDestroy() {};

  /**
   * Use the NativeImage as a texture for rendering
   * @pre There is a GL context for the current thread.
   * @return A GL error code
   */
  virtual unsigned int TargetTexture() {return 0;};

  /**
   * Called in each NativeTexture::Bind() call to allow implementation specific operations.
   * The correct texture sampler has already been bound before the function gets called.
   * @pre glAbstraction is being used by context in current thread
   */
  virtual void PrepareTexture() {}

  /**
   * Returns the width of the NativeImage
   * @return width
   */
  virtual unsigned int GetWidth() const {return mWidth;}

  /**
   * Returns the height of the NativeImage
   * @return height
   */
  virtual unsigned int GetHeight() const {return mHeight;}

  /**
   * Returns the internal pixel NativeImage::PixelFormat of the NativeImage
   * @return pixel format
   */
  virtual Pixel::Format GetPixelFormat() const { return Pixel::RGBA8888; }
protected:
  ~TestNativeImage(){}
};


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

ImageActor CreateLoadingImage(TestApplication& application, std::string filename, Image::LoadPolicy loadPolicy, Image::ReleasePolicy releasePolicy)
{
  Image image = Image::New(filename, loadPolicy, releasePolicy);
  DALI_TEST_CHECK( image );
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
  ImageActor actor = ImageActor::New(image);
  actor.SetSize( 80, 80 );
  application.SendNotification();
  application.Render(16);
  return actor;
}

void CompleteImageLoad(TestApplication& application, Integration::ResourceId resourceId, Integration::ResourceTypeId requestType)
{
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);

  application.GetPlatform().SetResourceLoaded(resourceId, requestType, resource);
}

void FailImageLoad(TestApplication& application, Integration::ResourceId resourceId )
{
  application.GetPlatform().SetResourceLoadFailed(resourceId, Integration::FailureUnknown);
}

void ReloadImage(TestApplication& application, Image image)
{
  application.GetPlatform().ClearReadyResources();
  application.GetPlatform().DiscardRequest();
  application.GetPlatform().ResetTrace();
  application.GetPlatform().SetClosestImageSize(Vector2(80.0f, 80.0f)); // Ensure reload is called.
  image.Reload();
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
    NativeImagePtr testNativeImagePtr = new TestNativeImage(10, 10);
    frameBufferImage= FrameBufferImage::New( *testNativeImagePtr.Get() );
  }
  else
  {
    frameBufferImage = FrameBufferImage::New( 10, 10 );
  }

  // Don't draw output framebuffer

  RenderTask newTask = taskList.CreateTask();
  newTask.SetCameraActor( offscreenCamera );
  newTask.SetSourceActor( secondRootActor );
  newTask.SetInputEnabled( false );
  newTask.SetClearColor( Vector4( 0.f, 0.f, 0.f, 0.f ) );
  newTask.SetClearEnabled( true );
  newTask.SetExclusive( true );
  newTask.SetRefreshRate( refreshRate );
  newTask.SetTargetFrameBuffer( frameBufferImage );
  return newTask;
}


bool UpdateRender(TestApplication& application, TraceCallStack& callStack, bool testDrawn, bool& finishedSig, bool testFinished )
{
  finishedSig = false;
  callStack.Reset();
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

  bool drawPassed = false;
  if( testDrawn )
  {
    drawPassed = callStack.FindMethod("DrawElements") || callStack.FindMethod("DrawArrays");
  }
  else
  {
    drawPassed = ! callStack.FindMethod("DrawElements") && ! callStack.FindMethod("DrawArrays") ;
  }

  tet_printf("UpdateRender: drawPassed:%s  sigPassed:%s (Expected: %s %s)\n", drawPassed?"T":"F", sigPassed?"T":"F", testDrawn?"T":"F", testFinished?"T":"F");

  return (sigPassed && drawPassed);
}

} // unnamed namespace


/****************************************************************************************************/
/****************************************************************************************************/
/********************************   TEST CASES BELOW   **********************************************/
/****************************************************************************************************/
/****************************************************************************************************/

int UtcDaliRenderTaskDownCast(void)
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

int UtcDaliRenderTaskSetSourceActor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor()");

  Stage stage = Stage::GetCurrent();

  const std::vector<GLuint>& boundTextures = application.GetGlAbstraction().GetBoundTextures( GL_TEXTURE0 );

  RenderTaskList taskList = stage.GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK( actor );

  std::vector<GLuint> ids;
  ids.push_back( 7 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  BitmapImage img = BitmapImage::New( 1,1 );
  ImageActor newActor = ImageActor::New( img );
  newActor.SetSize(1,1);
  stage.Add( newActor );

  Actor nonRenderableActor = Actor::New();
  stage.Add( nonRenderableActor );

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
  DALI_TEST_EQUALS( boundTextures.size(), 0u, TEST_LOCATION );

  // Set newActor as the new source Actor
  task.SetSourceActor( newActor );
  DALI_TEST_CHECK( task.GetSourceActor() != actor );
  DALI_TEST_CHECK( task.GetSourceActor() == newActor );

  // Update & Render the newActor
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the newActor was rendered
  DALI_TEST_EQUALS( boundTextures.size(), 1u, TEST_LOCATION );
  if ( boundTextures.size() )
  {
    DALI_TEST_EQUALS( boundTextures[0], 7u, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorOffStage(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor (on/off stage testing)");

  Stage stage = Stage::GetCurrent();

  const std::vector<GLuint>& boundTextures = application.GetGlAbstraction().GetBoundTextures( GL_TEXTURE0 );

  RenderTaskList taskList = stage.GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK( actor );

  std::vector<GLuint> ids;
  GLuint expectedTextureId( 3 );
  ids.push_back( expectedTextureId );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  BitmapImage img = BitmapImage::New( 1,1 );
  ImageActor newActor = ImageActor::New( img );
  newActor.SetSize(1,1);
  task.SetSourceActor( newActor );
  // Don't add newActor to stage yet

  // Update & Render with the actor initially off-stage
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that nothing was rendered
  DALI_TEST_EQUALS( boundTextures.size(), 0u, TEST_LOCATION );

  // Now add to stage
  stage.Add( newActor );

  // Update & Render with the actor on-stage
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the newActor was rendered
  DALI_TEST_EQUALS( boundTextures.size(), 1u, TEST_LOCATION );
  if ( boundTextures.size() )
  {
    DALI_TEST_EQUALS( boundTextures[0], expectedTextureId, TEST_LOCATION );
  }

  // Now remove from stage
  stage.Remove( newActor );

  // Update & Render with the actor off-stage
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();
  END_TEST;
}

int UtcDaliRenderTaskSetSourceActorEmpty(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetSourceActor (empty handle case)");

  Stage stage = Stage::GetCurrent();

  const std::vector<GLuint>& boundTextures = application.GetGlAbstraction().GetBoundTextures( GL_TEXTURE0 );

  RenderTaskList taskList = stage.GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK( actor );

  std::vector<GLuint> ids;
  GLuint expectedTextureId( 5 );
  ids.push_back( expectedTextureId );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  BitmapImage img = BitmapImage::New( 1,1 );
  ImageActor newActor = ImageActor::New( img );
  newActor.SetSize(1,1);
  stage.Add( newActor );

  Actor nonRenderableActor = Actor::New();
  stage.Add( nonRenderableActor );

  // Set with empty handle
  task.SetSourceActor( Actor() );
  DALI_TEST_CHECK( ! task.GetSourceActor() );

  // Update & Render nothing!
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that nothing was rendered
  DALI_TEST_EQUALS( boundTextures.size(), 0u, TEST_LOCATION );

  // Set with non-empty handle
  task.SetSourceActor( newActor );
  DALI_TEST_CHECK( task.GetSourceActor() == newActor );

  // Update & Render the newActor
  application.GetGlAbstraction().ClearBoundTextures();
  application.SendNotification();
  application.Render();

  // Check that the newActor was rendered
  DALI_TEST_EQUALS( boundTextures.size(), 1u, TEST_LOCATION );
  if ( boundTextures.size() )
  {
    DALI_TEST_EQUALS( boundTextures[0], expectedTextureId, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliRenderTaskGetSourceActor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetSourceActor()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetSourceActor();
  DALI_TEST_CHECK( actor );

  // By default the entire scene should be rendered
  Actor root = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_CHECK( root == actor );
  END_TEST;
}

int UtcDaliRenderTaskSetExclusive(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetExclusive()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // Manipulate the GenTextures behaviour, to identify different ImageActors

  std::vector<GLuint> ids;
  ids.push_back( 8 ); // 8 = actor1
  ids.push_back( 9 ); // 9 = actor2
  ids.push_back( 10 ); // 10 = actor3
  application.GetGlAbstraction().SetNextTextureIds( ids );

  BitmapImage img1 = BitmapImage::New( 1,1 );
  ImageActor actor1 = ImageActor::New( img1 );
  actor1.SetSize(1,1);
  Stage::GetCurrent().Add( actor1 );

  // Update & Render actor1
  application.SendNotification();
  application.Render();

  // Check that the actor1 was rendered
  const std::vector<GLuint>& boundTextures = application.GetGlAbstraction().GetBoundTextures( GL_TEXTURE0 );
  DALI_TEST_EQUALS( boundTextures.size(), 1u, TEST_LOCATION );

  if ( boundTextures.size() )
  {
    DALI_TEST_EQUALS( boundTextures[0], 8u/*unique to actor1*/, TEST_LOCATION );
  }

  BitmapImage img2 = BitmapImage::New( 1,1 );
  ImageActor actor2 = ImageActor::New( img2 );
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
  DALI_TEST_EQUALS( boundTextures.size(), 2u, TEST_LOCATION );

  if ( boundTextures.size() )
  {
    DALI_TEST_EQUALS( boundTextures[0], 9u/*unique to actor2*/, TEST_LOCATION );
    DALI_TEST_EQUALS( boundTextures[1], 8u/*unique to actor1*/, TEST_LOCATION );
  }

  BitmapImage img3 = BitmapImage::New( 1,1 );
  ImageActor actor3 = ImageActor::New( img3 );
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
  DALI_TEST_EQUALS( boundTextures.size(), 3u, TEST_LOCATION );

  if ( boundTextures.size() )
  {
    DALI_TEST_EQUALS( boundTextures[0], 10u/*unique to actor3*/, TEST_LOCATION );
    DALI_TEST_EQUALS( boundTextures[1], 9u/*unique to actor2*/, TEST_LOCATION );
    DALI_TEST_EQUALS( boundTextures[2], 8u/*unique to actor1*/, TEST_LOCATION );
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

int UtcDaliRenderTaskIsExclusive(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::IsExclusive()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // Not exclusive is the default
  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_CHECK( false == task.IsExclusive() );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK( false == newTask.IsExclusive() );
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
  DALI_TEST_CHECK( true == task.GetInputEnabled() );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK( true == newTask.GetInputEnabled() );
  END_TEST;
}

int UtcDaliRenderTaskSetCameraActor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetCameraActor()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetCameraActor();
  DALI_TEST_CHECK( actor );

  CameraActor newActor = CameraActor::New();
  DALI_TEST_CHECK( newActor );

  task.SetCameraActor( newActor );
  DALI_TEST_CHECK( task.GetCameraActor() != actor );
  DALI_TEST_CHECK( task.GetCameraActor() == newActor );
  END_TEST;
}

int UtcDaliRenderTaskGetCameraActor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetCameraActor()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Actor actor = task.GetCameraActor();
  DALI_TEST_CHECK( actor );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK( actor == newTask.GetCameraActor() );
  END_TEST;
}

int UtcDaliRenderTaskSetTargetFrameBuffer(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetTargetFrameBuffer()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  // By default render-tasks do not render off-screen
  FrameBufferImage image = task.GetTargetFrameBuffer();
  DALI_TEST_CHECK( !image );

  FrameBufferImage newImage = FrameBufferImage::New();

  task.SetTargetFrameBuffer( newImage );
  DALI_TEST_CHECK( task.GetTargetFrameBuffer() == newImage );
  END_TEST;
}

int UtcDaliRenderTaskGetTargetFrameBuffer(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetTargetFrameBuffer()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  // By default render-tasks do not render off-screen
  FrameBufferImage image = task.GetTargetFrameBuffer();
  DALI_TEST_CHECK( !image );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK( !newTask.GetTargetFrameBuffer() );
  END_TEST;
}

int UtcDaliRenderTaskSetScreenToFrameBufferFunction(void)
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

int UtcDaliRenderTaskGetScreenToFrameBufferFunction(void)
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


int UtcDaliRenderTaskGetScreenToFrameBufferMappingActor(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::GetScreenToFrameBufferMappingActor ");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask renderTask = taskList.CreateTask();
  Actor mappingActor = Actor::New();
  renderTask.SetScreenToFrameBufferMappingActor(mappingActor);

  DALI_TEST_CHECK( mappingActor == renderTask.GetScreenToFrameBufferMappingActor() );
  END_TEST;
}

int UtcDaliRenderTaskSetViewport(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::SetViewport()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Viewport viewport = task.GetViewport();

  // By default the viewport should match the stage width/height

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Viewport expectedViewport( 0, 0, stageSize.width, stageSize.height );
  DALI_TEST_CHECK( viewport == expectedViewport );

  Viewport newViewport( 0, 0, stageSize.width * 0.5f, stageSize.height * 0.5f );
  task.SetViewport( newViewport );

  // Update (viewport is a property)
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( task.GetViewport() == newViewport );
  END_TEST;
}

int UtcDaliRenderTaskGetViewport(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetViewport()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );

  Viewport viewport = task.GetViewport();

  // By default the viewport should match the stage width/height

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Viewport expectedViewport( 0, 0, stageSize.width, stageSize.height );
  DALI_TEST_CHECK( viewport == expectedViewport );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK( newTask.GetViewport() == expectedViewport );
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
  task.SetProperty( RenderTask::VIEWPORT_POSITION, newPosition2 );

  // Update
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( task.GetCurrentViewportPosition(), newPosition2, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  Vector2 newPosition3(64.0f, 0.0f);
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo( Property( task, RenderTask::VIEWPORT_POSITION ), newPosition3, AlphaFunctions::Linear );
  animation.Play();

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS( task.GetCurrentViewportPosition(), newPosition3, Math::MACHINE_EPSILON_1, TEST_LOCATION );
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
  task.SetProperty( RenderTask::VIEWPORT_SIZE, newSize2 );

  // Update
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( task.GetCurrentViewportSize(), newSize2, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  Vector2 newSize3(10.0f, 10.0f);
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo( Property( task, RenderTask::VIEWPORT_SIZE ), newSize3, AlphaFunctions::Linear );
  animation.Play();

  // Perform 1000ms worth of updates at which point animation should have completed.
  Wait(application, 1000);
  DALI_TEST_EQUALS( task.GetCurrentViewportSize(), newSize3, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRenderTaskSetClearColor(void)
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

  task.SetProperty( RenderTask::CLEAR_COLOR, testColor2 );

  // Wait a frame.
  Wait(application);

  DALI_TEST_EQUALS( task.GetClearColor(), testColor2, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRenderTaskGetClearColor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetClearColor()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_EQUALS( task.GetClearColor(), RenderTask::DEFAULT_CLEAR_COLOR, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRenderTaskSetClearEnabled(void)
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

int UtcDaliRenderTaskGetClearEnabled(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask::GetClearEnabled()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  RenderTask task = taskList.GetTask( 0u );
  DALI_TEST_CHECK( !task.GetClearEnabled() ); // defaults to false
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

  BitmapImage image = BitmapImage::New( 10, 10 );
  ImageActor rootActor = ImageActor::New( image );
  rootActor.SetSize( 10, 10 );
  Stage::GetCurrent().Add( rootActor );

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  NativeImagePtr testNativeImagePtr = new TestNativeImage(10, 10);
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
  DALI_TEST_EQUALS( (application.GetUpdateStatus() & Integration::KeepUpdating::RENDER_TASK_SYNC), Integration::KeepUpdating::RENDER_TASK_SYNC, TEST_LOCATION );
  application.SendNotification();
  DALI_TEST_CHECK( !finished );

  application.Render();
  DALI_TEST_EQUALS( (application.GetUpdateStatus() & Integration::KeepUpdating::RENDER_TASK_SYNC), Integration::KeepUpdating::RENDER_TASK_SYNC, TEST_LOCATION );
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

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );

  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.SendNotification();

  // START PROCESS/RENDER                     Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, false,   finished, false ) );
  application.GetPlatform().ClearReadyResources();

  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, false,   finished, false ) );

  // ADD SOURCE ACTOR TO STAGE - expect continuous renders to start, no finished signal
  Stage::GetCurrent().Add(secondRootActor);
  application.SendNotification();

  // CONTINUE PROCESS/RENDER                  Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
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

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );

  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  Stage::GetCurrent().Add(secondRootActor);
  secondRootActor.SetVisible(false);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  // MAKE SOURCE ACTOR VISIBLE - expect continuous renders to start, no finished signal
  secondRootActor.SetVisible(true);
  application.SendNotification();

  // CONTINUE PROCESS/RENDER                 Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
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

  CameraActor offscreenCameraActor = CameraActor::New();
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  // ADD CAMERA ACTOR TO STAGE - expect continuous renders to start, no finished signal
  Stage::GetCurrent().Add( offscreenCameraActor );
  application.SendNotification();

  // CONTINUE PROCESS/RENDER                 Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  END_TEST;
}


int UtcDaliRenderTaskContinuous04(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous using loading image\nPRE: Resource not ready\nPOST:continuous renders, no Finished signal");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  // FINISH RESOURCE LOADING - expect continuous renders to start, no finished signal
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  application.GetPlatform().ClearReadyResources();

  // CONTINUE PROCESS/RENDER                 Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  END_TEST;
}

int UtcDaliRenderTaskContinuous05(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous, every 3 frames using loading image\nPRE: Resources not ready\nPOST:continuous renders, no Finished signal");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, 3, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  // FINISH RESOURCE LOADING - expect continuous renders to start, no finished signal
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}

int UtcDaliRenderTaskContinuous06(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Continuous, every 3 frames using loading image\nPRE: Render Always, Resources not ready\nPOST:continuous renders at reduced freq, no Finished signal");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                     Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, false,   finished, false ) );

  // FINISH RESOURCE LOADING - expect continuous renders to start, no finished signal
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );

  // CHANGE FREQUENCY
  newTask.SetRefreshRate(3);  //              Input,    Expected  Input,    Expected
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, false,   finished, false ) );

  newTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS); // Should render every frame immediately
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application,  drawTrace, true,    finished, false ) );
  END_TEST;
}


int UtcDaliRenderTaskOnce01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync, using loading image\nPRE: Resources not ready, Source not visible\nPOST: Finished signal sent once only");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;

  Stage::GetCurrent().Add(secondRootActor);
  secondRootActor.SetVisible(false);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  // MAKE SOURCE VISIBLE
  secondRootActor.SetVisible(true);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  // FINISH RESOURCE LOADING - expect no rendering yet
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  application.GetPlatform().ClearReadyResources();
  sync.SetObjectSynced( lastSyncObj, true );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );

  // Finished rendering - expect no more renders, no more signals:
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}

int UtcDaliRenderTaskOnce02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync, using loading image. Switch from render always after ready to render once\n"
               "PRE: Render task ready, Image not loaded\n"
               "POST: Finished signal sent only once");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  // FINISH RESOURCE LOADING
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification(); //         Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  sync.SetObjectSynced( lastSyncObj, true );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );

  // Finished rendering - expect no more renders, no more signals:
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}


int UtcDaliRenderTaskOnce03(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync\n"
               "Switch from Render always after ready to render once with resources unready\n"
               "PRE: Everything ready to render\n"
               "POST: Finished signal sent once");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  // CHANGE TO RENDER ONCE
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification(); //         Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  // FINISH RESOURCE LOADING
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );
  application.GetPlatform().ClearReadyResources();

  sync.SetObjectSynced( lastSyncObj, true );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );

  // Finished rendering - expect no more renders, no more signals:
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}

#if 0
//int UtcDaliRenderTaskOnce04(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync\n"
               "During RenderOnce, make ready resources unready before sending first finished signal\n"
               "PRE: Everything ready.\n"
               "POST: Finished signal sent only once");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.Render();

  Stage::GetCurrent().Add(secondRootActor);
  application.GetPlatform().ClearReadyResources();

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );

  // CHANGE TO RENDER ONCE, RESOURCES BECOME NOT READY
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);

  // Doesn't work...
  ReloadImage(application, secondRootActor.GetImage());
  application.SendNotification(); //         Input,    Expected  Input,    Expected

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_EQUALS( secondRootActor.GetImage().GetLoadingState(), Dali::ResourceLoading, TEST_LOCATION);
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  // FINISH RESOURCE LOADING
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  application.GetPlatform().ClearReadyResources();
  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  sync.SetObjectSynced( lastSyncObj, true );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );

  // Finished rendering - expect no more renders, no more signals:
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}
#endif

int UtcDaliRenderTaskOnce05(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GLSync\n"
               "Render once, Second call to SetRefreshRate(ONCE) triggers only one more finished signal\n"
               "PRE: Everything ready\n"
               "POST: exactly 1 finished signal per call to SetRefreshRate(ONCE)");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.Render();
  application.GetPlatform().ClearReadyResources();

  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );

  // CHANGE TO RENDER ONCE,
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  sync.SetObjectSynced( lastSyncObj, true );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  sync.SetObjectSynced( lastSyncObj, true );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}

int UtcDaliRenderTaskOnce06(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GLSync\n"
               "Render once, Call to SetRefreshRate(ONCE) in Finished signal callback triggers "
               "another render & another finished signal\n"
               "PRE: Everything ready\n"
               "POST: exactly 1 finished signal per call to SetRefreshRate(ONCE)");


  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.Render();
  application.GetPlatform().ClearReadyResources();

  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;

  ConnectionTracker connectionTracker;
  RenderTaskFinishedRenderAgain renderTaskFinishedRenderAgain( finished );
  newTask.FinishedSignal().Connect( &connectionTracker, renderTaskFinishedRenderAgain );

  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj == NULL );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );

  // CHANGE TO RENDER ONCE,
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  sync.SetObjectSynced( lastSyncObj, true );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );
  application.SendNotification();

  // Expect SetRefreshRate to have been called again
  // Prevent next finished signal calling refresh once again
  RenderTaskFinished renderTaskFinished( finished );
  connectionTracker.DisconnectAll();
  newTask.FinishedSignal().Connect( &connectionTracker, renderTaskFinished );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  sync.SetObjectSynced( lastSyncObj, true );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}


int UtcDaliRenderTaskOnce07(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync\n"
               "SetRefreshRate(ONCE) again before first finished signal has been sent.\n"
               "PRE: resources ready\n"
               "POST: Only 1 finished signal sent.");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.Render();
  application.GetPlatform().ClearReadyResources();

  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );

  // CHANGE TO RENDER ONCE,
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  sync.SetObjectSynced( lastSyncObj, true );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}



int UtcDaliRenderTaskOnce08(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once GlSync\n"
               "SetRefreshRate(ONCE), resource load failed completes render task.\n"
               "PRE: resources not ready\n"
               "POST: Only 1 finished signal sent.");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TestGlSyncAbstraction& sync = application.GetGlSyncAbstraction();
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, true);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  // CHANGE TO RENDER ONCE,
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  FailImageLoad(application, imageRequestId); // Need to run Update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) ); // nothing to draw
  Integration::GlSyncAbstraction::SyncObject* lastSyncObj = sync.GetLastSyncObject();
  DALI_TEST_CHECK( lastSyncObj != NULL );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  sync.SetObjectSynced( lastSyncObj, true );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}



int UtcDaliRenderTaskOnceNoSync01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once, using loading image\nPRE: Resources not ready, Source not visible\nPOST: Finished signal sent once only");

  // SETUP AN OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;

  Stage::GetCurrent().Add(secondRootActor);
  secondRootActor.SetVisible(false);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ONCE, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  // MAKE SOURCE VISIBLE
  secondRootActor.SetVisible(true);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  // FINISH RESOURCE LOADING - expect immediate rendering yet
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync02(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once, using loading image. Switch from render always after ready to render once\n"
               "PRE: Render task ready, Image not loaded\n"
               "POST: Finished signal sent only once");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  // FINISH RESOURCE LOADING
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification(); //         Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}


int UtcDaliRenderTaskOnceNoSync03(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once\n"
               "Switch from Render always after ready to render once with resources unready\n"
               "PRE: Everything ready to render\n"
               "POST: Finished signal sent once");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  // CHANGE TO RENDER ONCE
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification(); //         Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  // FINISH RESOURCE LOADING
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}

#if 0
//int UtcDaliRenderTaskOnceNoSync04(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once\n"
               "During RenderOnce, make ready resources unready before sending first finished signal\n"
               "PRE: Everything ready.\n"
               "POST: Finished signal sent only once");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.Render();
  application.GetPlatform().ClearReadyResources();

  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );

  // CHANGE TO RENDER ONCE, RESOURCES BECOME NOT READY
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);

  // Doesn't work...
  ReloadImage(application, secondRootActor.GetImage());
  application.SendNotification(); //         Input,    Expected  Input,    Expected

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_EQUALS( secondRootActor.GetImage().GetLoadingState(), Dali::ResourceLoading, TEST_LOCATION);
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  // FINISH RESOURCE LOADING
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, true ) );
  application.GetPlatform().ClearReadyResources();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}
#endif

int UtcDaliRenderTaskOnceNoSync05(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once\n"
               "Render once, Second call to SetRefreshRate(ONCE) triggers only one more finished signal\n"
               "PRE: Everything ready\n"
               "POST: exactly 1 finished signal per call to SetRefreshRate(ONCE)");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.Render();
  application.GetPlatform().ClearReadyResources();

  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );

  // CHANGE TO RENDER ONCE,
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync06(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once\n"
               "Render once, Call to SetRefreshRate(ONCE) in Finished signal callback triggers\n"
               "another render & another finished signal\n"
               "PRE: Everything ready\n"
               "POST: exactly 1 finished signal per call to SetRefreshRate(ONCE)");


  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.Render();
  application.GetPlatform().ClearReadyResources();

  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool finished = false;

  ConnectionTracker connectionTracker;
  RenderTaskFinishedRenderAgain renderTaskFinishedRenderAgain( finished );
  newTask.FinishedSignal().Connect( &connectionTracker, renderTaskFinishedRenderAgain );

  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );

  // CHANGE TO RENDER ONCE,
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true ) );

  // Expect SetRefreshRate to have been called again
  // Prevent next finished signal calling refresh once again
  RenderTaskFinished renderTaskFinished( finished );
  connectionTracker.DisconnectAll();
  newTask.FinishedSignal().Connect( &connectionTracker, renderTaskFinished );

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}


int UtcDaliRenderTaskOnceNoSync07(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once\n"
               "SetRefreshRate(ONCE) again before first finished signal has been sent.\n"
               "PRE: resources ready\n"
               "POST: Only 1 finished signal sent.");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  application.Render();
  application.GetPlatform().ClearReadyResources();

  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );

  // CHANGE TO RENDER ONCE,
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );

  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, true ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   finished, false ) );
  END_TEST;
}

int UtcDaliRenderTaskOnceNoSync08(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once\n"
               "SetRefreshRate(ONCE), resource load failed, completes render task.\n"
               "PRE: resources not ready\n"
               "POST: Only 1 finished signal sent.");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor secondRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Stage::GetCurrent().Add(secondRootActor);

  RenderTask newTask = CreateRenderTask(application, offscreenCameraActor, rootActor, secondRootActor, RenderTask::REFRESH_ALWAYS, false);
  bool finished = false;
  RenderTaskFinished renderTaskFinished( finished );
  newTask.FinishedSignal().Connect( &application, renderTaskFinished );
  application.SendNotification();

  // START PROCESS/RENDER                    Input,     Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  // CHANGE TO RENDER ONCE,
  newTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  application.SendNotification();
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );

  FailImageLoad(application, imageRequestId); // Need to run Update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) ); // nothing to draw
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, true  ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,    finished, false ) );
  END_TEST;
}



int UtcDaliRenderTaskOnceChain01(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTask Render Once Chained render tasks\n"
               "SetRefreshRate(ONCE), resource load completes, both render tasks render.\n"
               "PRE: resources not ready\n"
               "POST: 2 finished signals sent.");

  // SETUP A CONTINUOUS OFFSCREEN RENDER TASK
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);

  Actor defaultRootActor = Actor::New(); // Root for default RT
  Stage::GetCurrent().Add( defaultRootActor );

  CameraActor offscreenCameraActor = CameraActor::New();
  Stage::GetCurrent().Add( offscreenCameraActor );
  ImageActor firstRootActor = CreateLoadingImage(application, "aFile.jpg", Image::Immediate, Image::Unused);
  Integration::ResourceRequest* imageRequest = application.GetPlatform().GetRequest();
  Integration::ResourceId imageRequestId = imageRequest->GetId();
  Integration::ResourceTypeId imageType  = imageRequest->GetType()->id;
  Stage::GetCurrent().Add(firstRootActor);

  // first render task
  RenderTask firstTask = CreateRenderTask(application, offscreenCameraActor, defaultRootActor, firstRootActor, RenderTask::REFRESH_ONCE, false);
  bool firstFinished = false;
  RenderTaskFinished renderTask1Finished( firstFinished );
  firstTask.FinishedSignal().Connect( &application, renderTask1Finished );

  // Second render task
  FrameBufferImage fbo = firstTask.GetTargetFrameBuffer();
  ImageActor secondRootActor = ImageActor::New( fbo );
  Stage::GetCurrent().Add(secondRootActor);
  RenderTask secondTask = CreateRenderTask(application, offscreenCameraActor, defaultRootActor, secondRootActor, RenderTask::REFRESH_ONCE, false);
  bool secondFinished = false;
  RenderTaskFinished renderTask2Finished( secondFinished );
  secondTask.FinishedSignal().Connect( &application, renderTask2Finished );

  application.SendNotification();

  // START PROCESS/RENDER                    Input,    Expected  Input,    Expected
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,  firstFinished, false ) );
  DALI_TEST_CHECK( secondFinished == false );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,  firstFinished, false ) );
  DALI_TEST_CHECK( secondFinished == false );

  CompleteImageLoad(application, imageRequestId, imageType); // Need to run update again for this to complete
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, true,  firstFinished, false ) );
  DALI_TEST_CHECK( secondFinished == false );
  application.GetPlatform().ClearReadyResources();

  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,  firstFinished, true ) );
  DALI_TEST_CHECK( secondFinished == true );

  secondFinished = false;
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   firstFinished, false ) );
  DALI_TEST_CHECK( secondFinished == false );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   firstFinished, false ) );
  DALI_TEST_CHECK( secondFinished == false );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   firstFinished, false ) );
  DALI_TEST_CHECK( secondFinished == false );
  DALI_TEST_CHECK( UpdateRender(application, drawTrace, false,   firstFinished, false ) );
  DALI_TEST_CHECK( secondFinished == false );

  END_TEST;
}

int UtcDaliRenderTaskProperties(void)
{
  TestApplication application;

  RenderTask task = Stage::GetCurrent().GetRenderTaskList().CreateTask();

  Property::IndexContainer indices;
  task.GetPropertyIndices( indices );
  DALI_TEST_CHECK( ! indices.empty() );
  DALI_TEST_EQUALS( indices.size(), task.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}
