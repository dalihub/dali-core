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

using namespace Dali;

namespace
{
// The functor to be used in the hit-test algorithm to check whether the actor is hittable.
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

          hittable = true;
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

} // anonymous namespace


int UtcDaliRenderTaskSetScreenToFrameBufferMappingActor(void)
{
  TestApplication application;
  tet_infoline("Testing RenderTask::SetScreenToFrameBufferMappingActor ");

  Stage stage = Stage::GetCurrent();
  Size stageSize = stage.GetSize();
  Actor mappingActor = Actor::New();
  Vector2 scale( 0.6f, 0.75f);
  Vector2 offset( stageSize.x*0.1f, stageSize.y*0.15f);
  mappingActor.SetSize( stageSize * scale );
  mappingActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mappingActor.SetPosition( offset.x, offset.y );
  stage.Add( mappingActor );

  Actor offscreenActor = Actor::New();
  offscreenActor.SetSize( stageSize );
  offscreenActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  stage.Add( offscreenActor );

  RenderTaskList taskList = stage.GetRenderTaskList();
  RenderTask renderTask = taskList.CreateTask();
  FrameBufferImage frameBufferImage =  FrameBufferImage::New(stageSize.width*scale.x, stageSize.height*scale.y, Pixel::A8, Image::Never);
  renderTask.SetSourceActor( offscreenActor );
  renderTask.SetExclusive( true );
  renderTask.SetInputEnabled( true );
  renderTask.SetTargetFrameBuffer( frameBufferImage );
  renderTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
  renderTask.SetScreenToFrameBufferMappingActor( mappingActor );
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  Vector2 screenCoordinates( stageSize.x * 0.05f, stageSize.y * 0.05f );
  Dali::HitTestAlgorithm::Results results;
  Dali::HitTestAlgorithm::HitTest( renderTask, screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( !results.actor);
  DALI_TEST_EQUALS( Vector2::ZERO, results.actorCoordinates, 0.1f, TEST_LOCATION );

  screenCoordinates.x = stageSize.x * 0.265f;
  screenCoordinates.y = stageSize.y * 0.33f;
  results.actor = Actor();
  results.actorCoordinates = Vector2::ZERO;
  Dali::HitTestAlgorithm::HitTest( renderTask, screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( results.actor  == offscreenActor);
  DALI_TEST_EQUALS( (screenCoordinates-offset)/scale , results.actorCoordinates, 0.1f, TEST_LOCATION );

  screenCoordinates.x = stageSize.x * 0.435f;
  screenCoordinates.y = stageSize.y * 0.52f;
  Dali::HitTestAlgorithm::HitTest( renderTask, screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( results.actor  == offscreenActor);
  DALI_TEST_EQUALS( (screenCoordinates-offset)/scale , results.actorCoordinates, 0.1f, TEST_LOCATION );

  screenCoordinates.x = stageSize.x * 0.65f;
  screenCoordinates.y = stageSize.y * 0.95f;
  Dali::HitTestAlgorithm::HitTest( renderTask, screenCoordinates, results, IsActorHittableFunction );
  DALI_TEST_CHECK( !results.actor);
  DALI_TEST_EQUALS( Vector2::ZERO, results.actorCoordinates, 0.1f, TEST_LOCATION );
  END_TEST;
}
