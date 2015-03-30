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

void utc_dali_internal_text_culling_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_text_culling_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{
#define NUM_ROWS 9
#define NUM_COLS 9
#define NUM_ROWS_PER_PANE 3
#define NUM_COLS_PER_PANE 3


TextActor CreateOnStageActor(TestApplication& application, Text text, int width, int height, bool testDraw)
{
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TestPlatformAbstraction& platform = application.GetPlatform();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  TextActor textActor = TextActor::New(text);
  textActor.SetParentOrigin(ParentOrigin::CENTER);
  textActor.SetSize(width, height);
  Stage::GetCurrent().Add(textActor);

  application.SendNotification();
  application.Render(16);

  Integration::ResourceRequest* request = platform.GetRequest();
  DALI_TEST_CHECK( request != NULL );
  DALI_TEST_CHECK( request->GetType() != NULL );
  DALI_TEST_CHECK( request->GetType()->id == Integration::ResourceText );

  Integration::TextResourceType* textRequest = static_cast<Integration::TextResourceType*>(request->GetType());

  std::string font("Font");
  Integration::GlyphSet* set = platform.GetGlyphData(*textRequest, font, true);
  platform.SetResourceLoaded( request->GetId(), Integration::ResourceText, Integration::ResourcePointer(set) );

  application.SendNotification();
  application.Render(16);

  platform.ClearReadyResources();

  if(testDraw)
  {
    DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );
  }
  return textActor;
}


void TestTextInside( TestApplication& application, int width, int height )
{
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  std::string text("Text");

  TextActor textActor = CreateOnStageActor(application, text, width, height, true);
  textActor.SetPosition(0.0f, 0.0f, 0.0f);

  Vector3 textSize = textActor.GetCurrentSize();
  DALI_TEST_EQUALS( textSize, Vector3(width, height, std::min(width, height)), TEST_LOCATION);

  drawTrace.Reset();
  textActor.SetParentOrigin(ParentOrigin::TOP_LEFT);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  drawTrace.Reset();
  textActor.SetParentOrigin(ParentOrigin::TOP_RIGHT);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  drawTrace.Reset();
  textActor.SetParentOrigin(ParentOrigin::BOTTOM_RIGHT);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );

  drawTrace.Reset();
  textActor.SetParentOrigin(ParentOrigin::BOTTOM_LEFT);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );
}


bool RepositionActor(TestApplication& application, Actor actor, float x, float y, bool inside)
{
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();

  drawTrace.Reset();
  actor.SetPosition( x, y, 0.0f);
  application.SendNotification();
  application.Render(16);

  bool found = drawTrace.FindMethod( "DrawElements" );
  bool result = (inside && found) || (!inside && !found);
  return result;
}


void RepositionActorWithAngle(TestApplication& application, Actor actor, float x, float y, float angle, bool inside)
{
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();

  drawTrace.Reset();
  actor.SetPosition( x, y, 0.0f);
  actor.SetOrientation( Degree(angle), Vector3::ZAXIS );
  application.SendNotification();
  application.Render(16);
  if( inside )
  {
    bool found = drawTrace.FindMethod( "DrawElements" );
    if( ! found ) tet_printf( "Not drawn: Position:(%3.0f, %3.0f)\n", x, y );
    DALI_TEST_CHECK( found );
  }
  else
  {
    bool found = drawTrace.FindMethod( "DrawElements" );
    if( found ) tet_printf( "Drawn when not needed: Position:(%3.0f, %3.0f)\n", x, y );
    DALI_TEST_CHECK( ! found );
  }
}

void RepositionActorOutside(TestApplication& application, Actor actor, float x, float y, bool drawn )
{
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();

  drawTrace.Reset();
  actor.SetPosition( x, y, 0.0f);
  application.SendNotification();
  application.Render(16);
  if( drawn )
  {
    bool found = drawTrace.FindMethod( "DrawElements" );
    if( ! found ) tet_printf( "Not drawn: Position:(%3.0f, %3.0f)\n", x, y );
    DALI_TEST_CHECK( found );
  }
  else
  {
    bool found = drawTrace.FindMethod( "DrawElements" );
    if( found ) tet_printf( "Drawn unnecessarily: Position:(%3.0f, %3.0f)\n", x, y );
    DALI_TEST_CHECK( ! found );
  }
}

void OBBTestTextAtBoundary( TestApplication& application, int width, int height )
{
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  Vector2 stageSize = Stage::GetCurrent().GetSize();

  std::string text("Text");
  TextActor textActor = CreateOnStageActor(application, text, width, height, true);

  Vector3 textSize = textActor.GetCurrentSize();
  DALI_TEST_EQUALS( textSize, Vector3(width, height, std::min(width, height)), TEST_LOCATION);

  textSize.z = 0.0f;
  tet_printf("Testing Stage Size: (%3.0f, %3.0f) text size:(%3.0f, %3.0f) \n",
             stageSize.x, stageSize.y, textSize.x, textSize.y);

  int successCount = 0;
  int totalCount = 0;
  for( int i=0; i<100; i++ )
  {
    float x1 = -stageSize.x/2.0f - textSize.x*i/200.0f;
    float x2 =  stageSize.x/2.0f + textSize.x*i/200.0f;
    float y1 = -stageSize.y/2.0f - textSize.y*i/200.0f;
    float y2 =  stageSize.y/2.0f + textSize.y*i/200.0f;

    //tet_printf("Testing i=%d\n",i);

    // Test paths marked with dots
    //  + . . . . . .
    //  .\_     ^
    //  .  \_   | within radius
    //  .    \  v
    //  .     +-----
    //  .     | Stage

    for( int j=-10; j<=10; j++ )
    {
      float x = ((stageSize.x+textSize.x/2.0f)/21.0f) * j;
      float y = ((stageSize.y+textSize.y/2.0f)/21.0f) * j;

      if(RepositionActor( application, textActor, x1, y, true )) successCount++;
      if(RepositionActor( application, textActor, x2, y, true )) successCount++;
      if(RepositionActor( application, textActor, x, y1, true )) successCount++;
      if(RepositionActor( application, textActor, x, y2, true )) successCount++;

      totalCount += 4;
    }
  }
  DALI_TEST_EQUALS(successCount, totalCount, TEST_LOCATION);
  tet_printf( "Test succeeded with %d passes out of %d tests\n", successCount, totalCount);
}


void OBBTestTextOutsideBoundary( TestApplication& application, int width, int height )
{
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  Vector2 stageSize = Stage::GetCurrent().GetSize();

  std::string text("Text");

  TextActor textActor = CreateOnStageActor(application, text, width, height, true);
  Vector3 textSize = textActor.GetCurrentSize();
  DALI_TEST_EQUALS( textSize, Vector3(width, height, std::min(width, height)), TEST_LOCATION);

  textSize.z = 0.0f;
  tet_printf("Testing Stage Size: (%3.0f, %3.0f) text size:(%3.0f, %3.0f)\n",
             stageSize.x, stageSize.y, textSize.x, textSize.y);

  int successCount=0;
  int totalCount=0;

  for( int i=0; i<=100; i++ )
  {
    float x1 = -stageSize.x/2.0f - textSize.x * (1.5f + i/100.0f);
    float x2 =  stageSize.x/2.0f + textSize.x * (1.5f + i/100.0f);
    float y1 = -stageSize.y/2.0f - textSize.y * (1.5f + i/100.0f);
    float y2 =  stageSize.y/2.0f + textSize.y * (1.5f + i/100.0f);

    for( int j=-10; j<=10; j++ )
    {
      float x = (stageSize.x/17.0f) * j; // use larger intervals to test more area
      float y = (stageSize.y/17.0f) * j;

      if(RepositionActor( application, textActor, x1, y, false )) successCount++;
      if(RepositionActor( application, textActor, x2, y, false )) successCount++;
      if(RepositionActor( application, textActor, x, y1, false )) successCount++;
      if(RepositionActor( application, textActor, x, y2, false )) successCount++;
      totalCount+=4;
    }
  }
  DALI_TEST_EQUALS(successCount, totalCount, TEST_LOCATION);
  tet_printf( "Test succeeded with %d passes out of %d tests\n", successCount, totalCount);
}


} // namespace

int UtcDaliTextCulling_Inside01(void)
{
  tet_infoline( "Testing that 80x80 text positioned inside the stage is drawn\n");

  TestApplication application;

  TestTextInside(application, 80, 80);

  END_TEST;
}

int UtcDaliTextCulling_Inside02(void)
{
  tet_infoline( "Testing that 120x40 text positioned inside the stage is drawn\n");

  TestApplication application;

  TestTextInside(application, 120, 40);

  END_TEST;
}

int UtcDaliTextCulling_Inside03(void)
{
  tet_infoline( "Testing that 40x120 text positioned inside the stage is drawn\n");

  TestApplication application;

  TestTextInside(application, 40, 120);

  END_TEST;
}

int UtcDaliTextCulling_Inside04(void)
{
  tet_infoline( "Testing that 500x2 text positioned inside the stage is drawn\n");
  TestApplication application;
  TestTextInside(application, 500, 2);
  END_TEST;
}

int UtcDaliTextCulling_Inside05(void)
{
  tet_infoline( "Testing that 2x500 text positioned inside the stage is drawn\n");
  TestApplication application;
  TestTextInside(application, 2, 500);
  END_TEST;
}


int UtcDaliTextCulling_WithinBoundary01(void)
{
  tet_infoline("Test that 80x80 text positioned outside the stage but with bounding box intersecting the stage is drawn\n");

  TestApplication application;
  OBBTestTextAtBoundary( application, 80, 80);
  END_TEST;
}
int UtcDaliTextCulling_WithinBoundary02(void)
{
  tet_infoline("Test that 120x40 text positioned outside the stage but with bounding box intersecting the stage is drawn\n");

  TestApplication application;
  OBBTestTextAtBoundary( application, 120, 40 );
  END_TEST;
}
int UtcDaliTextCulling_WithinBoundary03(void)
{
  tet_infoline("Test that 40x120 text positioned outside the stage but with bounding box intersecting the stage is drawn\n");

  TestApplication application;
  OBBTestTextAtBoundary( application, 40, 120);
  END_TEST;
}

int UtcDaliTextCulling_WithinBoundary04(void)
{
  tet_infoline("Test that 500x2 texts positioned outside the stage but with bounding box intersecting the stage is drawn\n");

  TestApplication application;
  OBBTestTextAtBoundary( application, 500, 2 );
  END_TEST;
}

int UtcDaliTextCulling_WithinBoundary05(void)
{
  tet_infoline("Test that 2x500 texts positioned outside the stage but with bounding box intersecting the stage is drawn\n");

  TestApplication application;
  OBBTestTextAtBoundary( application, 2, 500 );
  END_TEST;
}

int UtcDaliTextCulling_OutsideBoundary01(void)
{
  tet_infoline("Test that 80x80 text positioned outside the stage by more than 2 times\n"
               "the radius of the bounding circle  is not drawn\n");

  TestApplication application;
  OBBTestTextOutsideBoundary( application, 80, 80 );
  END_TEST;
}

int UtcDaliTextCulling_OutsideBoundary02(void)
{
  tet_infoline("Test that 120x40 text positioned outside the stage by more than 2 times\n"
               "the radius of the bounding circle  is not drawn\n");

  TestApplication application;
  OBBTestTextOutsideBoundary( application, 120, 40 );
  END_TEST;
}
int UtcDaliTextCulling_OutsideBoundary03(void)
{
  tet_infoline("Test that 40x120 text positioned outside the stage by more than 2 times\n"
               "the radius of the bounding circle  is not drawn\n");

  TestApplication application;
  OBBTestTextOutsideBoundary( application, 40, 120 );
  END_TEST;
}

int UtcDaliTextCulling_OutsideBoundary04(void)
{
  tet_infoline("Test that 500x2 text positioned outside the stage by more than 2 times\n"
               "the radius of the bounding circle  is not drawn\n");

  TestApplication application;
  OBBTestTextOutsideBoundary( application, 500, 2 );
  END_TEST;
}

int UtcDaliTextCulling_OutsideBoundary05(void)
{
  tet_infoline("Test that 2x500 text positioned outside the stage by more than 2 times\n"
               "the radius of the bounding circle  is not drawn\n");

  TestApplication application;
  OBBTestTextOutsideBoundary( application, 2, 500 );
  END_TEST;
}

int UtcDaliTextCulling_OutsideIntersect01(void)
{
  TestApplication application;

  tet_infoline("Test that actors positioned outside the stage with bounding boxes also\n"
               "outside the stage but intersecting it are still drawn");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  float width = stageSize.x*5.0f;
  float height = stageSize.y*0.2f;
  std::string text("Text");
  TextActor textActor = CreateOnStageActor(application, text, width, height, true);

  RepositionActor( application, textActor, stageSize.x*1.2f, 0.0f, true);
  RepositionActor( application, textActor, stageSize.x*1.2f, -stageSize.y*0.55f, true);
  RepositionActor( application, textActor, stageSize.x*1.2f, stageSize.y*0.55f, true);
  END_TEST;
}

int UtcDaliTextCulling_OutsideIntersect02(void)
{
  TestApplication application;

  tet_infoline("Test that actors positioned outside the stage with bounding boxes also\n"
               "outside the stage that cross planes are not drawn");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  float width = stageSize.x*5.0f;
  float height = stageSize.y*0.2f;
  std::string text("Text");
  TextActor textActor = CreateOnStageActor(application, text, width, height, true);

  RepositionActor( application, textActor,  stageSize.x*10.0f,  stageSize.y*0.5f, false);
  RepositionActor( application, textActor, -stageSize.x*10.0f,  stageSize.y*0.5f, false);
  RepositionActor( application, textActor,  stageSize.x*10.0f, -stageSize.y*0.5f, false);
  RepositionActor( application, textActor, -stageSize.x*10.0f, -stageSize.y*0.5f, false);
  END_TEST;
}

int UtcDaliTextCulling_OutsideIntersect03(void)
{
  TestApplication application;

  tet_infoline("Test that text actor larger than the stage, positioned outside the stage \n"
               "with bounding boxes also outside the stage but intersecting it is still drawn\n");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  // Try an actor bigger than the stage, with center outside stage
  float width = stageSize.x*5.0f;
  float height = stageSize.y*5.0f;
  std::string text("Text");
  TextActor textActor = CreateOnStageActor(application, text, width, height, true);

  RepositionActor( application, textActor, stageSize.x*1.2f, 0.0f, true);
  RepositionActor( application, textActor, stageSize.x*1.2f, -stageSize.y*1.1f, true);
  RepositionActor( application, textActor, stageSize.x*1.2f, stageSize.y*1.1f, true);

  END_TEST;
}

int UtcDaliTextCulling_OutsideIntersect04(void)
{
  TestApplication application;

  tet_infoline("Test that text actors positioned outside the stage, with bounding boxes\n"
               "also outside the stage but intersecting it, and angled at 45 degrees to\n"
               "the corners are still drawn\n");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  // Test text at 45 degrees outside corners of stage
  float width = 400.0f;
  float height = 200.0f;
  std::string text("Text");
  TextActor textActor = CreateOnStageActor(application, text, width, height, true);

  RepositionActorWithAngle( application, textActor, -stageSize.x*0.55f, -stageSize.y*0.55, 135.0f, true);
  RepositionActorWithAngle( application, textActor, -stageSize.x*0.55f,  stageSize.y*0.55, 225.0f, true);
  RepositionActorWithAngle( application, textActor,  stageSize.x*0.55f, -stageSize.y*0.55,  45.0f, true);
  RepositionActorWithAngle( application, textActor,  stageSize.x*0.55f,  stageSize.y*0.55, 315.0f, true);

  END_TEST;
}


int UtcDaliTextCulling_Disable(void)
{
  tet_infoline("Test that culling can be disabled");

  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  float width=80;
  float height=80;
  std::string text("Text");


  TextActor textActor = CreateOnStageActor(application, text, width, height, true);
  Vector3 textSize = textActor.GetCurrentSize();
  DALI_TEST_EQUALS( textSize, Vector3(width, height, std::min(width, height)), TEST_LOCATION);

  textSize.z = 0.0f;

  tet_infoline("Setting cull mode to false\n");
  Stage::GetCurrent().GetRenderTaskList().GetTask(0).SetCullMode(false);

  float x1 = -stageSize.x - textSize.x;
  float x2 =  stageSize.x + textSize.x;
  float y1 = -stageSize.y - textSize.y;
  float y2 =  stageSize.y + textSize.y;

  // Positioning actors outside stage, with no culling, they should still be drawn.
  RepositionActorOutside( application, textActor, x1, y1, true );
  RepositionActorOutside( application, textActor, x2, y1, true );
  RepositionActorOutside( application, textActor, x1, y2, true );
  RepositionActorOutside( application, textActor, x2, y2, true );

  tet_infoline("Setting cull mode to true\n");
  Stage::GetCurrent().GetRenderTaskList().GetTask(0).SetCullMode(true);

  RepositionActorOutside( application, textActor, x1, y1, false );
  RepositionActorOutside( application, textActor, x2, y1, false );
  RepositionActorOutside( application, textActor, x1, y2, false );
  RepositionActorOutside( application, textActor, x2, y2, false );

  END_TEST;
}
