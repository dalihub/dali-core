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

void utc_dali_internal_image_culling_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_image_culling_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{
#define NUM_ROWS 9
#define NUM_COLS 9
#define NUM_ROWS_PER_PANE 3
#define NUM_COLS_PER_PANE 3
const unsigned int TEXTURE_ID_OFFSET = 23;

Image LoadImage( TestApplication& application, GLuint textureId, int width, int height )
{
  Image image;
  char* filename = NULL;
  int numChars = asprintf(&filename, "image%u.png", textureId );

  if( numChars > 0 )
  {
    const Vector2 closestImageSize( width, height );
    application.GetPlatform().SetClosestImageSize(closestImageSize);

    image = Image::New( filename, Image::Immediate, Image::Never );
    free (filename);
  }
  DALI_TEST_CHECK(image);
  application.SendNotification();
  application.Render(16);

  std::vector<GLuint> ids;
  ids.push_back( textureId );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, width, height, width, height);
  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  DALI_TEST_CHECK( request != NULL );
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
  application.SendNotification();
  application.Render(16);
  application.GetPlatform().ClearReadyResources();
  application.GetPlatform().DiscardRequest();
  application.SendNotification();
  application.Render(16);

  return image;
}

ImageActor CreateOnStageActor(TestApplication& application, Image image, int width, int height, bool testDraw)
{
  ImageActor imageActor = ImageActor::New(image);
  Stage::GetCurrent().Add(imageActor);

  imageActor.SetParentOrigin(ParentOrigin::CENTER);
  application.SendNotification();
  application.Render(16);
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  if(testDraw)
  {
    DALI_TEST_CHECK( drawTrace.FindMethod( "DrawArrays" ) );
    DALI_TEST_CHECK( textureTrace.FindMethod( "BindTexture" ) );
    const std::vector<GLuint>& textures = glAbstraction.GetBoundTextures(GL_TEXTURE0);
    DALI_TEST_CHECK( textures.size() > 0 );
    if( textures.size() > 0 )
    {
      DALI_TEST_CHECK( textures[0] == 23 );
    }
  }
  return imageActor;
}


void TestImageInside( TestApplication& application, int width, int height )
{
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  Image image = LoadImage( application, 23, width, height );

  ImageActor imageActor = CreateOnStageActor(application, image, width, height, true);
  imageActor.SetPosition(0.0f, 0.0f, 0.0f);

  Vector3 imageSize = imageActor.GetCurrentSize();
  DALI_TEST_EQUALS( imageSize, Vector3(width, height, std::min(width, height)), TEST_LOCATION);

  drawTrace.Reset();
  imageActor.SetParentOrigin(ParentOrigin::TOP_LEFT);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawArrays" ) );

  drawTrace.Reset();
  imageActor.SetParentOrigin(ParentOrigin::TOP_RIGHT);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawArrays" ) );

  drawTrace.Reset();
  imageActor.SetParentOrigin(ParentOrigin::BOTTOM_RIGHT);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawArrays" ) );

  drawTrace.Reset();
  imageActor.SetParentOrigin(ParentOrigin::BOTTOM_LEFT);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawArrays" ) );
}


bool RepositionActor(TestApplication& application, Actor actor, float x, float y, bool inside)
{
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();

  drawTrace.Reset();
  actor.SetPosition( x, y, 0.0f);
  application.SendNotification();
  application.Render(16);

  bool found = drawTrace.FindMethod( "DrawArrays" );
  return (inside && found) || (!inside && !found);
}


void RepositionActorWithAngle(TestApplication& application, Actor actor, float x, float y, float angle, bool inside)
{
  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();

  drawTrace.Reset();
  actor.SetPosition( x, y, 0.0f);
  actor.SetRotation( Degree(angle), Vector3::ZAXIS );
  application.SendNotification();
  application.Render(16);
  if( inside )
  {
    bool found = drawTrace.FindMethod( "DrawArrays" );
    if( ! found ) tet_printf( "Not drawn: Position:(%3.0f, %3.0f)\n", x, y );
    DALI_TEST_CHECK( found );
  }
  else
  {
    bool found = drawTrace.FindMethod( "DrawArrays" );
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
    bool found = drawTrace.FindMethod( "DrawArrays" );
    if( ! found ) tet_printf( "Not drawn: Position:(%3.0f, %3.0f)\n", x, y );
    DALI_TEST_CHECK( found );
  }
  else
  {
    bool found = drawTrace.FindMethod( "DrawArrays" );
    if( found ) tet_printf( "Drawn unnecessarily: Position:(%3.0f, %3.0f)\n", x, y );
    DALI_TEST_CHECK( ! found );
  }

}

void SphereTestImageAtBoundary( TestApplication& application, int width, int height )
{
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  Vector2 stageSize = Stage::GetCurrent().GetSize();

  Image image = LoadImage(application, 23, width, height);
  ImageActor imageActor = CreateOnStageActor(application, image, width, height, true);

  Vector3 imageSize = imageActor.GetCurrentSize();
  DALI_TEST_EQUALS( imageSize, Vector3(width, height, std::min(width, height)), TEST_LOCATION);

  imageSize.z = 0.0f;
  float radius = imageSize.Length() * 0.5f; // Radius of bounding box
  tet_printf("Testing Stage Size: (%3.0f, %3.0f) image size:(%3.0f, %3.0f) := radius=%3.0f\n",
             stageSize.x, stageSize.y, imageSize.x, imageSize.y, radius);

  for( int i=0; i<=radius; i++ )
  {
    float x1 = -stageSize.x/2.0f - i;
    float x2 =  stageSize.x/2.0f + i;
    float y1 = -stageSize.y/2.0f - i;
    float y2 =  stageSize.y/2.0f + i;

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
      float x = ((stageSize.x+2*radius)/21.0f) * j;
      float y = ((stageSize.y+2*radius)/21.0f) * j;

      RepositionActor( application, imageActor, x1, y, true );
      RepositionActor( application, imageActor, x2, y, true );
      RepositionActor( application, imageActor, x, y1, true );
      RepositionActor( application, imageActor, x, y2, true );
    }
  }
}

void OBBTestImageAtBoundary( TestApplication& application, int width, int height )
{
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  Vector2 stageSize = Stage::GetCurrent().GetSize();

  Image image = LoadImage(application, 23, width, height);
  ImageActor imageActor = CreateOnStageActor(application, image, width, height, true);

  Vector3 imageSize = imageActor.GetCurrentSize();
  DALI_TEST_EQUALS( imageSize, Vector3(width, height, std::min(width, height)), TEST_LOCATION);

  imageSize.z = 0.0f;
  tet_printf("Testing Stage Size: (%3.0f, %3.0f) image size:(%3.0f, %3.0f) \n",
             stageSize.x, stageSize.y, imageSize.x, imageSize.y);

  int successCount = 0;
  int totalCount = 0;
  for( int i=0; i<100; i++ )
  {
    float x1 = -stageSize.x/2.0f - imageSize.x*i/200.0f;
    float x2 =  stageSize.x/2.0f + imageSize.x*i/200.0f;
    float y1 = -stageSize.y/2.0f - imageSize.y*i/200.0f;
    float y2 =  stageSize.y/2.0f + imageSize.y*i/200.0f;

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
      float x = ((stageSize.x+imageSize.x/2.0f)/21.0f) * j;
      float y = ((stageSize.y+imageSize.y/2.0f)/21.0f) * j;

      if(RepositionActor( application, imageActor, x1, y, true )) successCount++;
      if(RepositionActor( application, imageActor, x2, y, true )) successCount++;
      if(RepositionActor( application, imageActor, x, y1, true )) successCount++;
      if(RepositionActor( application, imageActor, x, y2, true )) successCount++;

      totalCount += 4;
    }
  }
  DALI_TEST_EQUALS(successCount, totalCount, TEST_LOCATION);
  tet_printf( "Test succeeded with %d passes out of %d tests\n", successCount, totalCount);
}


void SphereTestImageOutsideBoundary( TestApplication& application, int width, int height )
{
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  Vector2 stageSize = Stage::GetCurrent().GetSize();

  Image image = LoadImage( application, 23, width, height );

  ImageActor imageActor = CreateOnStageActor(application, image, width, height, true);
  Vector3 imageSize = imageActor.GetCurrentSize();
  DALI_TEST_EQUALS( imageSize, Vector3(width, height, std::min(width, height)), TEST_LOCATION);

  imageSize.z = 0.0f;
  float radius = imageSize.Length() * 0.5f; // Radius of bounding box
  tet_printf("Testing Stage Size: (%3.0f, %3.0f) image size:(%3.0f, %3.0f) := radius=%3.0f\n",
             stageSize.x, stageSize.y, imageSize.x, imageSize.y, radius);

  for( int i=0; i<100; i++ )
  {
    // Try from 3 times
    float x1 = -stageSize.x/2.0f - imageSize.x*i/200.0f;
    float x2 =  stageSize.x/2.0f + imageSize.x*i/200.0f;
    float y1 = -stageSize.y/2.0f - imageSize.y*i/200.0f;
    float y2 =  stageSize.y/2.0f + imageSize.y*i/200.0f;


    //tet_printf("Testing i=%d\n",i);
    for( int j=-10; j<=10; j++ )
    {
      float x = (stageSize.x/17.0f) * j; // use larger intervals to test more area
      float y = (stageSize.y/17.0f) * j;

      RepositionActor( application, imageActor, x1, y, false );
      RepositionActor( application, imageActor, x2, y, false );
      RepositionActor( application, imageActor, x, y1, false );
      RepositionActor( application, imageActor, x, y2, false );
    }
  }
}

void OBBTestImageOutsideBoundary( TestApplication& application, int width, int height )
{
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  Vector2 stageSize = Stage::GetCurrent().GetSize();

  Image image = LoadImage( application, 23, width, height );

  ImageActor imageActor = CreateOnStageActor(application, image, width, height, true);
  Vector3 imageSize = imageActor.GetCurrentSize();
  DALI_TEST_EQUALS( imageSize, Vector3(width, height, std::min(width, height)), TEST_LOCATION);

  imageSize.z = 0.0f;
  tet_printf("Testing Stage Size: (%3.0f, %3.0f) image size:(%3.0f, %3.0f)\n",
             stageSize.x, stageSize.y, imageSize.x, imageSize.y);

  int successCount=0;
  int totalCount=0;

  for( int i=0; i<=100; i++ )
  {
    float x1 = -stageSize.x/2.0f - imageSize.x * (1.5f + i/100.0f);
    float x2 =  stageSize.x/2.0f + imageSize.x * (1.5f + i/100.0f);
    float y1 = -stageSize.y/2.0f - imageSize.y * (1.5f + i/100.0f);
    float y2 =  stageSize.y/2.0f + imageSize.y * (1.5f + i/100.0f);

    for( int j=-10; j<=10; j++ )
    {
      float x = (stageSize.x/17.0f) * j; // use larger intervals to test more area
      float y = (stageSize.y/17.0f) * j;

      if(RepositionActor( application, imageActor, x1, y, false )) successCount++;
      if(RepositionActor( application, imageActor, x2, y, false )) successCount++;
      if(RepositionActor( application, imageActor, x, y1, false )) successCount++;
      if(RepositionActor( application, imageActor, x, y2, false )) successCount++;
      totalCount+=4;
    }
  }
  DALI_TEST_EQUALS(successCount, totalCount, TEST_LOCATION);
  tet_printf( "Test succeeded with %d passes out of %d tests\n", successCount, totalCount);
}

void TestPlaneOfImages(TestApplication& application, float z)
{
  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Vector2 imageSize = stageSize/3.0f;

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  // Create a grid of 9 x 9 actors; only the central 3x3 are in viewport

  ActorContainer actors;
  for( int i = 0; i < NUM_ROWS*NUM_COLS; i++ )
  {
    GLuint textureId = TEXTURE_ID_OFFSET+i;
    Image image = LoadImage( application, textureId, imageSize.x, imageSize.y );
    ImageActor imageActor = CreateOnStageActor(application, image, imageSize.x, imageSize.y, false );
    actors.push_back(imageActor);
  }
  application.SendNotification();
  application.Render(16);

  drawTrace.Reset();
  textureTrace.Reset();
  drawTrace.Enable(true);
  textureTrace.Enable(true);
  application.GetGlAbstraction().ClearBoundTextures();

  for( int row=0; row<NUM_ROWS; row++)
  {
    for( int col=0; col<NUM_COLS; col++)
    {
      // col:    0   1   2  | 3   4   5 | 6   7   8
      //
      //        -4/3 -1 -2/3 -1/3  0 1/3 |2/3 1  4/3
      // col*2 -5
      //         0   2   4
      // NUM_COLS = 9 NUM_COLS_PER_PANE = 3   (A pane is stage sized)
      //
      // NUM_COLS / NUM_COLS_PER_PANE => number of panes
      // 3 per pane, splits into -1/3, 0, +1/3
      // number of lh planes * number of cols per plane
      // Center column maps to zero, index of center column = num_cols / 2 rounded down
      // index - that  and divide by number of cols per pane.
      // 0 1 2 3 4  5 6 7 8  9 10 11 12
      float xOffset = (int)(col - (NUM_COLS/2));
      float yOffset = (int)(row - (NUM_ROWS/2));
      float x = stageSize.x*xOffset / (float)(NUM_COLS_PER_PANE);
      float y = stageSize.y*yOffset / (float)(NUM_ROWS_PER_PANE);
      actors[row*NUM_COLS+col].SetPosition( x, y, z );
    }
  }

  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawArrays" ) );

  typedef std::vector<GLuint> TexVec;
  const TexVec& textures = glAbstraction.GetBoundTextures(GL_TEXTURE0);
  DALI_TEST_CHECK( textures.size() >= NUM_ROWS_PER_PANE * NUM_COLS_PER_PANE );
  if( textures.size() > 0 )
  {
    int foundCount=0;
    int expectedCount = 0;
    for(unsigned int row=0; row<NUM_ROWS; row++)
    {
      for(unsigned int col=0; col<NUM_ROWS; col++)
      {
        Vector3 pos = actors[row*NUM_COLS+col].GetCurrentPosition();
        if( -stageSize.x/2.0f <= pos.x && pos.x <= stageSize.x/2.0f &&
            -stageSize.y/2.0f <= pos.y && pos.y <= stageSize.y/2.0f )
        {
          expectedCount++;
          GLuint texId = (row*NUM_COLS+col)+TEXTURE_ID_OFFSET;

          for(TexVec::const_iterator iter = textures.begin(); iter != textures.end(); iter++)
          {
            if(*iter == texId)
            {
              foundCount++;
            }
          }
        }
      }
    }
    tet_printf("Number of expected draws: %u\n", expectedCount);
    DALI_TEST_EQUALS(foundCount, expectedCount, TEST_LOCATION);
  }
  int numDrawCalls = drawTrace.CountMethod("DrawArrays");
  DALI_TEST_CHECK( numDrawCalls >= NUM_ROWS_PER_PANE * NUM_COLS_PER_PANE );

  tet_printf("Number of bound textures: %u\n", textures.size());
  tet_printf("Number of draw calls: %d\n", numDrawCalls);
}

} // namespace

int UtcDaliImageCulling_Inside01(void)
{
  tet_infoline( "Testing that 80x80 image positioned inside the stage is drawn\n");

  TestApplication application;

  TestImageInside(application, 80, 80);

  END_TEST;
}

int UtcDaliImageCulling_Inside02(void)
{
  tet_infoline( "Testing that 120x40 image positioned inside the stage is drawn\n");

  TestApplication application;

  TestImageInside(application, 120, 40);

  END_TEST;
}

int UtcDaliImageCulling_Inside03(void)
{
  tet_infoline( "Testing that 40x120 image positioned inside the stage is drawn\n");

  TestApplication application;

  TestImageInside(application, 40, 120);

  END_TEST;
}

int UtcDaliImageCulling_Inside04(void)
{
  tet_infoline( "Testing that 500x2 image positioned inside the stage is drawn\n");
  TestApplication application;
  TestImageInside(application, 500, 2);
  END_TEST;
}

int UtcDaliImageCulling_Inside05(void)
{
  tet_infoline( "Testing that 2x500 image positioned inside the stage is drawn\n");
  TestApplication application;
  TestImageInside(application, 2, 500);
  END_TEST;
}


int UtcDaliImageCulling_WithinBoundary01(void)
{
  tet_infoline("Test that 80x80 image positioned outside the stage but with bounding box intersecting the stage is drawn\n");

  TestApplication application;
  OBBTestImageAtBoundary( application, 80, 80);
  END_TEST;
}
int UtcDaliImageCulling_WithinBoundary02(void)
{
  tet_infoline("Test that 120x40 image positioned outside the stage but with bounding box intersecting the stage is drawn\n");

  TestApplication application;
  OBBTestImageAtBoundary( application, 120, 40 );
  END_TEST;
}
int UtcDaliImageCulling_WithinBoundary03(void)
{
  tet_infoline("Test that 40x120 image positioned outside the stage but with bounding box intersecting the stage is drawn\n");

  TestApplication application;
  OBBTestImageAtBoundary( application, 40, 120);
  END_TEST;
}

int UtcDaliImageCulling_WithinBoundary04(void)
{
  tet_infoline("Test that 500x2 images positioned outside the stage but with bounding box intersecting the stage is drawn\n");

  TestApplication application;
  OBBTestImageAtBoundary( application, 500, 2 );
  END_TEST;
}

int UtcDaliImageCulling_WithinBoundary05(void)
{
  tet_infoline("Test that 2x500 images positioned outside the stage but with bounding box intersecting the stage is drawn\n");

  TestApplication application;
  OBBTestImageAtBoundary( application, 2, 500 );
  END_TEST;
}

int UtcDaliImageCulling_OutsideBoundary01(void)
{
  tet_infoline("Test that 80x80 image positioned outside the stage by more than 2 times\n"
               "the radius of the bounding circle  is not drawn\n");

  TestApplication application;
  OBBTestImageOutsideBoundary( application, 80, 80 );
  END_TEST;
}

int UtcDaliImageCulling_OutsideBoundary02(void)
{
  tet_infoline("Test that 120x40 image positioned outside the stage by more than 2 times\n"
               "the radius of the bounding circle  is not drawn\n");

  TestApplication application;
  OBBTestImageOutsideBoundary( application, 120, 40 );
  END_TEST;
}
int UtcDaliImageCulling_OutsideBoundary03(void)
{
  tet_infoline("Test that 40x120 image positioned outside the stage by more than 2 times\n"
               "the radius of the bounding circle  is not drawn\n");

  TestApplication application;
  OBBTestImageOutsideBoundary( application, 40, 120 );
  END_TEST;
}

int UtcDaliImageCulling_OutsideBoundary04(void)
{
  tet_infoline("Test that 500x2 image positioned outside the stage by more than 2 times\n"
               "the radius of the bounding circle  is not drawn\n");

  TestApplication application;
  OBBTestImageOutsideBoundary( application, 500, 2 );
  END_TEST;
}

int UtcDaliImageCulling_OutsideBoundary05(void)
{
  tet_infoline("Test that 2x500 image positioned outside the stage by more than 2 times\n"
               "the radius of the bounding circle  is not drawn\n");

  TestApplication application;
  OBBTestImageOutsideBoundary( application, 2, 500 );
  END_TEST;
}

int UtcDaliImageCulling_OutsideIntersect01(void)
{
  TestApplication application;

  tet_infoline("Test that actors positioned outside the stage with bounding boxes also\n"
               "outside the stage but intersecting it are still drawn");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  float width = stageSize.x*5.0f;
  float height = stageSize.y*0.2f;
  Image image = LoadImage( application, 23,  width, height);
  ImageActor imageActor = CreateOnStageActor(application, image, width, height, true);

  RepositionActor( application, imageActor, stageSize.x*1.2f, 0.0f, true);
  RepositionActor( application, imageActor, stageSize.x*1.2f, -stageSize.y*0.55f, true);
  RepositionActor( application, imageActor, stageSize.x*1.2f, stageSize.y*0.55f, true);
  END_TEST;
}

int UtcDaliImageCulling_OutsideIntersect02(void)
{
  TestApplication application;

  tet_infoline("Test that actors positioned outside the stage with bounding boxes also\n"
               "outside the stage that cross planes are not drawn");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  float width = stageSize.x*5.0f;
  float height = stageSize.y*0.2f;
  Image image = LoadImage( application, 23,  width, height);
  ImageActor imageActor = CreateOnStageActor(application, image, width, height, true);

  RepositionActor( application, imageActor,  stageSize.x*10.0f,  stageSize.y*0.5f, false);
  RepositionActor( application, imageActor, -stageSize.x*10.0f,  stageSize.y*0.5f, false);
  RepositionActor( application, imageActor,  stageSize.x*10.0f, -stageSize.y*0.5f, false);
  RepositionActor( application, imageActor, -stageSize.x*10.0f, -stageSize.y*0.5f, false);
  END_TEST;
}

int UtcDaliImageCulling_OutsideIntersect03(void)
{
  TestApplication application;

  tet_infoline("Test that image actor larger than the stage, positioned outside the stage \n"
               "with bounding boxes also outside the stage but intersecting it is still drawn\n");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  // Try an actor bigger than the stage, with center outside stage
  float width = stageSize.x*5.0f;
  float height = stageSize.y*5.0f;
  Image image = LoadImage( application, 23,  width, height);
  ImageActor imageActor = CreateOnStageActor(application, image, width, height, true);

  RepositionActor( application, imageActor, stageSize.x*1.2f, 0.0f, true);
  RepositionActor( application, imageActor, stageSize.x*1.2f, -stageSize.y*1.1f, true);
  RepositionActor( application, imageActor, stageSize.x*1.2f, stageSize.y*1.1f, true);

  END_TEST;
}

int UtcDaliImageCulling_OutsideIntersect04(void)
{
  TestApplication application;

  tet_infoline("Test that image actors positioned outside the stage, with bounding boxes\n"
               "also outside the stage but intersecting it, and angled at 45 degrees to\n"
               "the corners are still drawn\n");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  // Test image at 45 degrees outside corners of stage
  float width = 400.0f;
  float height = 200.0f;
  Image image = LoadImage( application, 23,  width, height);
  ImageActor imageActor = CreateOnStageActor(application, image, width, height, true);

  RepositionActorWithAngle( application, imageActor, -stageSize.x*0.55f, -stageSize.y*0.55, 135.0f, true);
  RepositionActorWithAngle( application, imageActor, -stageSize.x*0.55f,  stageSize.y*0.55, 225.0f, true);
  RepositionActorWithAngle( application, imageActor,  stageSize.x*0.55f, -stageSize.y*0.55,  45.0f, true);
  RepositionActorWithAngle( application, imageActor,  stageSize.x*0.55f,  stageSize.y*0.55, 315.0f, true);

  END_TEST;
}

int UtcDaliImageCulling_Plane01(void)
{
  tet_infoline("Test that a set of image actors with different images are drawn appropriately");

  TestApplication application;

  TestPlaneOfImages(application, 0.0f);
  END_TEST;
}

int UtcDaliImageCulling_Plane02(void)
{
  tet_infoline("Test that a set of image actors with different images are drawn appropriately");

  TestApplication application;

  TestPlaneOfImages(application, 100.0f);
  END_TEST;
}

int UtcDaliImageCulling_Plane03(void)
{
  tet_infoline("Test that a set of image actors with different images are drawn appropriately");

  TestApplication application;

  TestPlaneOfImages(application, -100.0f);
  END_TEST;
}

int UtcDaliImageCulling_Plane04(void)
{
  tet_infoline("Test that a set of image actors with different images are drawn appropriately");

  TestApplication application;

  TestPlaneOfImages(application, -200.0f);
  END_TEST;
}

int UtcDaliImageCulling_Disable(void)
{
  tet_infoline("Test that culling can be disabled");

  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  float width=80;
  float height=80;
  Image image = LoadImage( application, 23, width, height );

  ImageActor imageActor = CreateOnStageActor(application, image, width, height, true);
  Vector3 imageSize = imageActor.GetCurrentSize();
  DALI_TEST_EQUALS( imageSize, Vector3(width, height, std::min(width, height)), TEST_LOCATION);

  imageSize.z = 0.0f;

  tet_infoline("Setting cull mode to false\n");
  Stage::GetCurrent().GetRenderTaskList().GetTask(0).SetCullMode(false);

  float x1 = -stageSize.x - imageSize.x;
  float x2 =  stageSize.x + imageSize.x;
  float y1 = -stageSize.y - imageSize.y;
  float y2 =  stageSize.y + imageSize.y;

  // Positioning actors outside stage, with no culling, they should still be drawn.
  RepositionActorOutside( application, imageActor, x1, y1, true );
  RepositionActorOutside( application, imageActor, x2, y1, true );
  RepositionActorOutside( application, imageActor, x1, y2, true );
  RepositionActorOutside( application, imageActor, x2, y2, true );

  tet_infoline("Setting cull mode to true\n");
  Stage::GetCurrent().GetRenderTaskList().GetTask(0).SetCullMode(true);

  RepositionActorOutside( application, imageActor, x1, y1, false );
  RepositionActorOutside( application, imageActor, x2, y1, false );
  RepositionActorOutside( application, imageActor, x1, y2, false );
  RepositionActorOutside( application, imageActor, x2, y2, false );

  END_TEST;
}
