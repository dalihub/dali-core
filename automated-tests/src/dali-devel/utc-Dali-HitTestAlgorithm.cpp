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
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{

/**
 * The functor to be used in the hit-test algorithm to check whether the actor is hittable.
 */
bool IsActorHittableFunction(Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch (type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      // Check whether the actor is visible and not fully transparent.
      if( actor.IsVisible()
       && actor.GetCurrentWorldColor().a > 0.01f) // not FULLY_TRANSPARENT
      {
        // Check whether the actor has the specific name "HittableActor"
        if(actor.GetName() == "HittableActor")
        {
          hittable = true;
        }
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if( actor.IsVisible() ) // Actor is visible, if not visible then none of its children are visible.
      {
        hittable = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return hittable;
};


bool DefaultIsActorTouchableFunction(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch (type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      if( actor.IsVisible() &&
          actor.IsSensitive() &&
          actor.GetCurrentWorldColor().a > 0.01f)
      {
        hittable = true;
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if( actor.IsVisible() && // Actor is visible, if not visible then none of its children are visible.
          actor.IsSensitive()) // Actor is sensitive, if insensitive none of its children should be hittable either.
      {
        hittable = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return hittable;
};

} // anonymous namespace


// Positive test case for a method
int UtcDaliHitTestAlgorithmWithFunctor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm functor");

  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  actor.SetName("NonHittableActor");
  stage.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Vector2 localCoordinates;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );

  // Perform a hit-test at the given screen coordinates
  Dali::HitTestAlgorithm::Results results;
  Dali::HitTestAlgorithm::HitTest( stage, screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( results.actor != actor );

  actor.SetName("HittableActor");

  results.actor = Actor();
  results.actorCoordinates = Vector2::ZERO;

  // Perform a hit-test at the given screen coordinates
  Dali::HitTestAlgorithm::HitTest( stage, screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( results.actor == actor );
  DALI_TEST_EQUALS( localCoordinates, results.actorCoordinates, 0.1f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHitTestAlgorithmWithFunctorOnRenderTask(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm functor, specific to a given render task");

  Stage stage = Stage::GetCurrent();
  Size stageSize = stage.GetSize();
  RenderTaskList taskList = stage.GetRenderTaskList();

  Actor actor[2];

  for( int i=0; i<2; i++ )
  {
    actor[i] = Actor::New();
    actor[i].SetSize(100.f, 100.f);
    actor[i].SetParentOrigin(ParentOrigin::TOP_LEFT);
    actor[i].SetAnchorPoint(AnchorPoint::TOP_LEFT);
    actor[i].SetName("HittableActor");
    stage.Add(actor[i]);
  }
  Vector2 position( 50.f, 40.f );
  actor[1].SetPosition( position.x, position.y );

  RenderTask renderTask[2];
  renderTask[0] = taskList.GetTask( 0u );

  FrameBufferImage frameBufferImage =  FrameBufferImage::New(stageSize.width, stageSize.height, Pixel::A8, Image::NEVER);
  renderTask[1] = taskList.CreateTask();
  renderTask[1].SetSourceActor( actor[1] );
  renderTask[1].SetExclusive( true );
  renderTask[1].SetInputEnabled( true );
  renderTask[1].SetTargetFrameBuffer( frameBufferImage );
  renderTask[1].SetRefreshRate( RenderTask::REFRESH_ONCE );
  renderTask[1].SetScreenToFrameBufferFunction( RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION );
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  // Perform a hit-test at the given screen coordinates with different render tasks

  Dali::HitTestAlgorithm::Results results;
  Vector2 screenCoordinates( 25.f, 25.f );

  Dali::HitTestAlgorithm::HitTest( renderTask[0], screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( results.actor == actor[0] );
  DALI_TEST_EQUALS( screenCoordinates, results.actorCoordinates, 0.1f, TEST_LOCATION );

  results.actor = Actor();
  results.actorCoordinates = Vector2::ZERO;
  Dali::HitTestAlgorithm::HitTest( renderTask[1], screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( !results.actor );
  DALI_TEST_EQUALS( Vector2::ZERO, results.actorCoordinates, 0.1f, TEST_LOCATION );

  screenCoordinates.x = 80.f;
  screenCoordinates.y = 70.f;

  results.actor = Actor();
  results.actorCoordinates = Vector2::ZERO;
  Dali::HitTestAlgorithm::HitTest( renderTask[0], screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( results.actor == actor[0] );
  DALI_TEST_EQUALS( screenCoordinates, results.actorCoordinates, 0.1f, TEST_LOCATION );

  results.actor = Actor();
  results.actorCoordinates = Vector2::ZERO;
  Dali::HitTestAlgorithm::HitTest( renderTask[1], screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( results.actor == actor[1]);
  DALI_TEST_EQUALS( screenCoordinates - position, results.actorCoordinates, 0.1f, TEST_LOCATION );

  screenCoordinates.x = 120.f;
  screenCoordinates.y = 130.f;

  results.actor = Actor();
  results.actorCoordinates = Vector2::ZERO;
  Dali::HitTestAlgorithm::HitTest( renderTask[0], screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( !results.actor );
  DALI_TEST_EQUALS( Vector2::ZERO, results.actorCoordinates, 0.1f, TEST_LOCATION );

  results.actor = Actor();
  results.actorCoordinates = Vector2::ZERO;
  Dali::HitTestAlgorithm::HitTest( renderTask[1], screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( results.actor == actor[1]);
  DALI_TEST_EQUALS( screenCoordinates - position, results.actorCoordinates, 0.1f, TEST_LOCATION );
  END_TEST;
}


int UtcDaliHitTestAlgorithmOrtho01(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with parallel Ortho camera()");

  Stage stage = Stage::GetCurrent();
  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask defaultRenderTask = renderTaskList.GetTask(0u);
  Dali::CameraActor cameraActor = defaultRenderTask.GetCameraActor();

  Vector2 stageSize ( stage.GetSize() );
  cameraActor.SetOrthographicProjection( stageSize );
  cameraActor.SetPosition(0.0f, 0.0f, 1600.0f);

  Vector2 actorSize( stageSize * 0.5f );
  // Create two actors with half the size of the stage and set them to be partially overlapping
  Actor blue = Actor::New();
  blue.SetName( "Blue" );
  blue.SetAnchorPoint( AnchorPoint::CENTER );
  blue.SetParentOrigin( Vector3(1.0f/3.0f, 1.0f/3.0f, 0.5f) );
  blue.SetSize( actorSize );
  blue.SetZ(30.0f);

  Actor green = Actor::New( );
  green.SetName( "Green" );
  green.SetAnchorPoint( AnchorPoint::CENTER );
  green.SetParentOrigin( Vector3(2.0f/3.0f, 2.0f/3.0f, 0.5f) );
  green.SetSize( actorSize );

  // Add the actors to the view
  stage.Add( blue );
  stage.Add( green );

  // Render and notify
  application.SendNotification();
  application.Render(0);
  application.Render(10);

  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK( results.actor == green );
  DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 1.0f/6.0f, TEST_LOCATION );

  HitTest(stage, stageSize / 3.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK( results.actor == blue );
  DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION );

  HitTest(stage, stageSize * 2.0f / 3.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK( results.actor == green );
  DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION );
  END_TEST;
}


int UtcDaliHitTestAlgorithmOrtho02(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with offset Ortho camera()");

  Stage stage = Stage::GetCurrent();
  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask defaultRenderTask = renderTaskList.GetTask(0u);
  Dali::CameraActor cameraActor = defaultRenderTask.GetCameraActor();

  Vector2 stageSize ( stage.GetSize() );
  cameraActor.SetOrthographicProjection(-stageSize.x * 0.3f,  stageSize.x * 0.7f,
                                         stageSize.y * 0.3f, -stageSize.y * 0.7f,
                                         800.0f, 4895.0f);
  cameraActor.SetPosition(0.0f, 0.0f, 1600.0f);

  Vector2 actorSize( stageSize * 0.5f );
  // Create two actors with half the size of the stage and set them to be partially overlapping
  Actor blue = Actor::New();
  blue.SetName( "Blue" );
  blue.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  blue.SetParentOrigin( Vector3(0.2f, 0.2f, 0.5f) );
  blue.SetSize( actorSize );
  blue.SetZ(30.0f);

  Actor green = Actor::New( );
  green.SetName( "Green" );
  green.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  green.SetParentOrigin( Vector3(0.4f, 0.4f, 0.5f) );
  green.SetSize( actorSize );

  // Add the actors to the view
  stage.Add( blue );
  stage.Add( green );

  // Render and notify
  application.SendNotification();
  application.Render(0);
  application.Render(10);

  {
    HitTestAlgorithm::Results results;
    HitTest(stage, Vector2( 240.0f, 400.0f ), results, &DefaultIsActorTouchableFunction);
    DALI_TEST_CHECK( results.actor == green );
    DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 0.6f, 0.01f, TEST_LOCATION );
  }

  {
    HitTestAlgorithm::Results results;
    HitTest(stage, Vector2( 0.001f, 0.001f ), results, &DefaultIsActorTouchableFunction);
    DALI_TEST_CHECK( results.actor == blue );
    DALI_TEST_EQUALS( results.actorCoordinates, Vector2( 0.001f, 0.001f ), 0.001f, TEST_LOCATION );
  }

  {
    HitTestAlgorithm::Results results;
    HitTest(stage, stageSize, results, &DefaultIsActorTouchableFunction);
    DALI_TEST_CHECK( ! results.actor );
    DALI_TEST_EQUALS( results.actorCoordinates, Vector2::ZERO, TEST_LOCATION );
  }

  // Just inside green
  {
    HitTestAlgorithm::Results results;
    HitTest(stage, stageSize*0.69f, results, &DefaultIsActorTouchableFunction);
    DALI_TEST_CHECK( results.actor == green );
    DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 0.98f, 0.01f, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliHitTestAlgorithmStencil(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with a stencil");

  Stage stage = Stage::GetCurrent();
  Actor rootLayer = stage.GetRootLayer();
  rootLayer.SetName( "RootLayer" );

  // Create a layer
  Layer layer = Layer::New();
  layer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  layer.SetParentOrigin( ParentOrigin::TOP_LEFT );
  layer.SetName( "layer" );
  stage.Add( layer );

  // Create a stencil and add that to the layer
  Actor stencil = ImageActor::New(Dali::BufferImage::WHITE() );
  stencil.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  stencil.SetParentOrigin( ParentOrigin::TOP_LEFT );
  stencil.SetSize( 50.0f, 50.0f );
  stencil.SetDrawMode( DrawMode::STENCIL );
  stencil.SetName( "stencil" );
  layer.Add( stencil );

  // Create a renderable actor and add that to the layer
  Actor layerHitActor = ImageActor::New( Dali::BufferImage::WHITE() );
  layerHitActor.SetSize( 100.0f, 100.0f );
  layerHitActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  layerHitActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  layerHitActor.SetName( "layerHitActor" );
  layer.Add( layerHitActor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Hit within stencil and actor
  {
    HitTestAlgorithm::Results results;
    HitTest(stage, Vector2( 10.0f, 10.0f ), results, &DefaultIsActorTouchableFunction);
    DALI_TEST_CHECK( results.actor == layerHitActor );
    tet_printf( "Hit: %s\n", ( results.actor ? results.actor.GetName().c_str() : "NULL" ) );
  }

  // Hit within actor but outside of stencil, should hit the root-layer
  {
    HitTestAlgorithm::Results results;
    HitTest(stage, Vector2( 60.0f, 60.0f ), results, &DefaultIsActorTouchableFunction);
    DALI_TEST_CHECK( results.actor == rootLayer );
    tet_printf( "Hit: %s\n", ( results.actor ? results.actor.GetName().c_str() : "NULL" ) );
  }
  END_TEST;
}

int UtcDaliHitTestAlgorithmOverlay(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with overlay actors");

  Stage stage = Stage::GetCurrent();
  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask defaultRenderTask = renderTaskList.GetTask(0u);
  Dali::CameraActor cameraActor = defaultRenderTask.GetCameraActor();

  Vector2 stageSize ( stage.GetSize() );
  cameraActor.SetOrthographicProjection( stageSize );
  cameraActor.SetPosition(0.0f, 0.0f, 1600.0f);

  Vector2 actorSize( stageSize * 0.5f );
  // Create two actors with half the size of the stage and set them to be partially overlapping
  Actor blue = Actor::New();
  blue.SetDrawMode( DrawMode::OVERLAY_2D );
  blue.SetName( "Blue" );
  blue.SetAnchorPoint( AnchorPoint::CENTER );
  blue.SetParentOrigin( Vector3(1.0f/3.0f, 1.0f/3.0f, 0.5f) );
  blue.SetSize( actorSize );
  blue.SetZ(30.0f);

  Actor green = Actor::New( );
  green.SetName( "Green" );
  green.SetAnchorPoint( AnchorPoint::CENTER );
  green.SetParentOrigin( Vector3(2.0f/3.0f, 2.0f/3.0f, 0.5f) );
  green.SetSize( actorSize );

  // Add the actors to the view
  stage.Add( blue );
  stage.Add( green );

  // Render and notify
  application.SendNotification();
  application.Render(0);
  application.Render(10);

  HitTestAlgorithm::Results results;

  //Hit in the intersection. Should pick the blue actor since it is an overlay.
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK( results.actor == blue );
  DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 5.0f/6.0f, TEST_LOCATION );

  //Hit in the blue actor
  HitTest(stage, stageSize / 3.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK( results.actor == blue );
  DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION );

  //Hit in the green actor
  HitTest(stage, stageSize * 2.0f / 3.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK( results.actor == green );
  DALI_TEST_EQUALS( results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION );
  END_TEST;
}
