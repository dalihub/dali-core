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
#include <dali/dali.h>
#include <dali/integration-api/glyph-set.h>

#include <dali-test-suite-utils.h>

// Internal headers are allowed here

#include <dali/internal/event/text/font-impl.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/common/thread-local-storage.h>

using namespace Dali;

// Called only once before first test is run.
void utc_dali_internal_font_startup()
{
  test_return_value = TET_UNDEF;
}

// Called only once after last test is run
void utc_dali_internal_font_cleanup()
{
  test_return_value = TET_PASS;
}


namespace
{

static const char* TestText = "Some text";


Integration::GlyphMetrics characters[] =
    {
        {' ', 1,  0.0f,  0.0f, 0.0f, 0.0f, 10.0f},
        {'S', 1, 10.0f, 20.0f, 0.0f, 1.0f, 12.0f},
        {'o', 1, 11.0f, 20.0f, 0.0f, 1.0f, 13.0f},
        {'m', 1, 12.0f, 20.0f, 0.0f, 1.0f, 14.0f},
        {'e', 1, 13.0f, 20.0f, 0.0f, 1.0f, 15.0f},
        {'t', 1, 14.0f, 20.0f, 0.0f, 1.0f, 16.0f},
        {'x', 1, 15.0f, 20.0f, 0.0f, 1.0f, 17.0f}   };

static Integration::GlyphSet* BuildGlyphSet()
{
  Integration::GlyphSet* set = new Integration::GlyphSet();
  Integration::BitmapPtr bitmapData;

  for (unsigned int index = 0; index < sizeof(characters)/sizeof(characters[0]); index++)
  {
    set->AddCharacter(bitmapData, characters[index]);
  }

  set->mLineHeight = 20.0f;
  set->mUnitsPerEM = 2048.0f/64.0f;

  return set;
}

static Font CreateFont(TestApplication& application)
{
  Integration::GlyphSet* glyphSet = BuildGlyphSet();
  Integration::ResourcePointer resourcePtr(glyphSet); // reference it

  // Don't use a font which could be cached otherwise cached values will be used making measure text test to fail.
  Font font = Font::New(FontParameters("TET-FreeSans", "Book", PointSize(8)));
  application.SendNotification(); // Send to update thread
  application.Render(16);         // Process request
  application.Render(16);         // Resource complete
  application.SendNotification(); // Update event objects
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  return font;
}

} //anonymous namespace


int UtcDaliFontMeasureTextWidth(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::MeasureTextWidth()");

  Font testFont = CreateFont(application);
  float width = testFont.MeasureTextWidth(TestText, 30.0f);

  DALI_TEST_EQUALS(width, 270.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontMeasureTextWidthNegative(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::MeasureTextWidth() with negative height");

  Font testFont = CreateFont(application);
  float width = testFont.MeasureTextWidth(TestText, -30.0f);

  DALI_TEST_EQUALS(width, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontMeasureTextHeight(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::MeasureTextHeight()");

  Font testFont = CreateFont(application);
  float height = testFont.MeasureTextHeight(TestText, 200.0f);

  DALI_TEST_EQUALS(height, 22.2222f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontMeasureTextHeightNegative(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::MeasureTextHeight() with negative width");

  Font testFont = CreateFont(application);
  float height = testFont.MeasureTextHeight(TestText, -200.0f);

  DALI_TEST_EQUALS(height, 0.0f, TEST_LOCATION);
  END_TEST;
}
