/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
// Size of the VertexAttributeArray enables
// GLES specification states that there's a minimum of 8
const unsigned int TEST_MAX_ATTRIBUTE_CACHE_SIZE = 8;

enum TestAttribType
{
  ATTRIB_UNKNOWN = -1,
  ATTRIB_POSITION,
  ATTRIB_NORMAL,
  ATTRIB_TEXCOORD,
  ATTRIB_COLOR,
  ATTRIB_BONE_WEIGHTS,
  ATTRIB_BONE_INDICES,
  ATTRIB_TYPE_LAST
};

// Create bitmap actor
#if 0
static Actor CreateBitmapActor()
{
  BufferImage image = BufferImage::New(4,4,Pixel::RGBA8888);
  Actor actor = CreateRenderableActor( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("Test Image Rendering Actor");
  return actor;
}
#endif
} // anonymous namespace


// Positive test case for a method
int UtcDaliContextVertexAttribStartup(void)
{
  tet_infoline("Testing vertex attrib initial state in context");
  tet_infoline("   Test requires GraphicsController");
#if 0

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // check the locations
  for (unsigned int i = 0; i < TEST_MAX_ATTRIBUTE_CACHE_SIZE; i++)
  {
    DALI_TEST_CHECK( application.GetGlAbstraction().GetVertexAttribArrayState(i) == false);
  }

  tet_result(TET_PASS);
#endif
  END_TEST;
}

// Tests to make the attribs only get set once when continually rendering an image actor
int UtcDaliContextVertexAttribImageRendering(void)
{
  tet_infoline("Testing vertex attrib rendering state in context with images");
  tet_infoline("   Test requires GraphicsController");
#if 0

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // the vertex attribs get modified on startup to set them to disabled
  // clear the flag to say they've changed
  application.GetGlAbstraction().ClearVertexAttribArrayChanged();


  // create a test bitmap actor
  Actor actor(CreateBitmapActor());
  Stage::GetCurrent().Add(actor);


  application.SendNotification();
  application.Render();
  application.Render();

  // check to make sure the state has changed (the image renderer will enable some
  // locations).
  DALI_TEST_CHECK(application.GetGlAbstraction().GetVertexAttribArrayChanged());

  // Now check to make sure the state is cached, and isn't being set each frame.
  application.GetGlAbstraction().ClearVertexAttribArrayChanged();

  application.Render();
  application.Render();
  application.Render();

  // if it has changed then the caching has failed
  DALI_TEST_CHECK(application.GetGlAbstraction().GetVertexAttribArrayChanged() == false);


  tet_result(TET_PASS);
#endif
  END_TEST;
}
