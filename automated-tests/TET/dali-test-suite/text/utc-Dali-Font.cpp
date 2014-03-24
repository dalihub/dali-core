//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>
#include <errno.h>
#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliFontNew01,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontNew02,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontNew03,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontNew04,              NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontNew05,              NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontNew06,              NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontDownCast,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontGetPixelSize,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontGetPointSize,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontPointsToPixels,     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontMeasureTextWidth,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontMeasureTextHeight,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontMeasureText,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontGetFamilyForText,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontGetFontLineHeightFromCapsHeight, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontAllGlyphsSupported,     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontGetMetrics,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontIsDefault,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontGetInstalledFonts,      POSITIVE_TC_IDX );
// Metrics
TEST_FUNCTION( UtcFontMetricsDefaultConstructor,    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontMetricsCopyConstructor,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcFontMetricsAssignmentOperator,    POSITIVE_TC_IDX );

const std::string FAMILY_NAME = "Arial";
const std::string STYLE = "Bold";
const unsigned int PIXEL_SIZE = 20;
const unsigned int POINT_SIZE = 11.f;
// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static Font CreateFont( PointSize size )
{
  // Don't use a font which could be cached otherwise cached values will be used making measure text test to fail.
  return Font::New(FontParameters("TET-FreeSans", "Book", size));
}

static Font CreateFont( PixelSize size )
{
  // Don't use a font which could be cached otherwise cached values will be used making measure text test to fail.
  return Font::New(FontParameters("TET-FreeSans", "Book", size));
}

static Font CreateFont( CapsHeight size )
{
  // Don't use a font which could be cached otherwise cached values will be used making measure text test to fail.
  return Font::New(FontParameters("TET-FreeSans", "Book", size));
}

static void UtcDaliFontNew01()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::New() - with specific font family and  pixel size");

  Font font;      // invoke default constructor (creates an empty handle)
  font = CreateFont(PixelSize(25.0f));  // This does not call platform abstraction until some text is displayed or measured

  DALI_TEST_CHECK(font);

  tet_infoline("Testing Dali::Font::New() - with default font name and pixel size");

  Font font2 = Font::New(FontParameters("", "", PixelSize(0.0f)));

  DALI_TEST_CHECK(font2);

  Font* ptrFont = new Font;
  *ptrFont = Font::New(FontParameters("", "", PixelSize(0.0f)));
  delete ptrFont;
}

static void UtcDaliFontNew02()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::New() - with specific font family and point size");

  Font font = CreateFont(PointSize(8));

  DALI_TEST_CHECK(font);

  tet_infoline("Testing Dali::Font::New() - with default font family and point size");

  Font font2;
  font2 = Font::New(FontParameters("", "", PointSize(0)));

  DALI_TEST_CHECK(font2);
}

static void UtcDaliFontNew03()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::New() - with specific font family and caps-height");

  Font font = CreateFont(CapsHeight(8));

  DALI_TEST_CHECK(font);
  DALI_TEST_CHECK(font.GetPixelSize() > 8.0f);    // Pixel size should be bigger than requested CapsHeight

  tet_infoline("Testing Dali::Font::New() - with default font family and point size");

  Font font2 = Font::New(FontParameters("", "", CapsHeight(0)));

  DALI_TEST_CHECK(font2);
}

static void UtcDaliFontNew04()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::New() - with wrong font family or font style");

  Font font = Font::New(FontParameters("gfagag", "fgafgafga",PointSize(0)));

  DALI_TEST_CHECK(font);
}

static void UtcDaliFontNew05()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::New() - with pixel size and weight");

  PixelSize pixelSize(PIXEL_SIZE);
  FontParameters fontParams(FAMILY_NAME, "", pixelSize);
  Font font = Font::New( fontParams );

  DALI_TEST_CHECK( font );
  DALI_TEST_CHECK( font.GetName() == FAMILY_NAME );
  DALI_TEST_CHECK( font.GetStyle().empty() );
}

static void UtcDaliFontNew06()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::New() - with caps height");

  CapsHeight capsHeight(10.f);
  FontParameters fontParams(FAMILY_NAME, "", capsHeight);
  Font font = Font::New( fontParams );

  DALI_TEST_CHECK( font );
  DALI_TEST_CHECK( font.GetName() == FAMILY_NAME );
  DALI_TEST_CHECK( font.GetStyle().empty() );
}


static void UtcDaliFontDownCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::Font::DownCast()");

  Font font = CreateFont(PixelSize(25.0f));

  BaseHandle object(font);

  Font font2 = Font::DownCast(object);
  DALI_TEST_CHECK(font2);

  Font font3 = DownCast< Font >(object);
  DALI_TEST_CHECK(font3);

  BaseHandle unInitializedObject;
  Font font4 = Font::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!font4);

  Font font5 = DownCast< Font >(unInitializedObject);
  DALI_TEST_CHECK(!font5);
}

static void UtcDaliFontGetPixelSize()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::GetPixelSize()");

  Font font = CreateFont(PixelSize(32));

  DALI_TEST_CHECK(32 == font.GetPixelSize());
}

static void UtcDaliFontGetPointSize()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::GetPointSize)");

  Font font = CreateFont(PointSize(8.0f));

  DALI_TEST_EQUALS(8.f, font.GetPointSize(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
}

static void UtcDaliFontPointsToPixels()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::PointsToPixels)");

  unsigned int points= Font::PointsToPixels( 12.0f );

  DALI_TEST_CHECK(  points == 36  );
}

static void UtcFontMeasureTextWidth()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::MeasureTextWidth()");

  TraceCallStack& trace = application.GetPlatform().GetTrace();
  trace.Enable(true);

  Font font = CreateFont(PointSize(8));

  float width = font.MeasureTextWidth("test me", 24.0f);

  // No cache

  DALI_TEST_CHECK(trace.FindMethod("ReadMetricsFromCacheFile"));
  DALI_TEST_CHECK(trace.FindMethod("WriteMetricsToCacheFile"));
  DALI_TEST_CHECK(trace.FindMethod("GetGlyphData"));
  trace.Reset();

  DALI_TEST_EQUALS(width, 168.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  width = font.MeasureTextWidth(Text(std::string("test me")), 24.0f);

  // Should now be cached in memory
  DALI_TEST_CHECK( ! trace.FindMethod("ReadMetricsFromCacheFile"));
  DALI_TEST_CHECK( ! trace.FindMethod("WriteMetricsToCacheFile"));
  DALI_TEST_CHECK( ! trace.FindMethod("GetGlyphData"));

  DALI_TEST_EQUALS(width, 168.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Text text = Text(std::string("t"))[0];
  Character c = text[0];
  width = font.MeasureTextWidth(c, 24.0f);
  DALI_TEST_EQUALS(width, 24.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

}

static void UtcFontMeasureTextHeight()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::MeasureTextHeight()");

  TraceCallStack& trace = application.GetPlatform().GetTrace();
  trace.Enable(true);

  Font font = CreateFont(PointSize(8));

  float height = font.MeasureTextHeight("test me", 48.0f);

  DALI_TEST_CHECK(trace.FindMethod("ReadMetricsFromCacheFile"));
  DALI_TEST_CHECK(trace.FindMethod("WriteMetricsToCacheFile"));
  DALI_TEST_CHECK(trace.FindMethod("GetGlyphData"));
  trace.Reset();

  DALI_TEST_EQUALS(height, 6.8571f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  height = font.MeasureTextHeight(Text(std::string("test me")), 48.0f);

  DALI_TEST_CHECK( ! trace.FindMethod("ReadMetricsFromCacheFile"));
  DALI_TEST_CHECK( ! trace.FindMethod("WriteMetricsToCacheFile"));
  DALI_TEST_CHECK( ! trace.FindMethod("GetGlyphData"));

  DALI_TEST_EQUALS(height, 6.8571f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Text text = Text(std::string("t"))[0];
  Character c = text[0];
  height = font.MeasureTextHeight(c, 24.0f);
  DALI_TEST_EQUALS(height, 24.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

}

static void UtcFontMeasureText()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::MeasureText()");

  TraceCallStack& trace = application.GetPlatform().GetTrace();
  trace.Enable(true);

  Font font = CreateFont(PointSize(8));

  Vector3 size = font.MeasureText("test me");

  DALI_TEST_CHECK(trace.FindMethod("ReadMetricsFromCacheFile"));
  DALI_TEST_CHECK(trace.FindMethod("WriteMetricsToCacheFile"));
  DALI_TEST_CHECK(trace.FindMethod("GetGlyphData"));
  trace.Reset();

  DALI_TEST_EQUALS(size.width, 53.1076f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 7.5868f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  size = font.MeasureText(Text(std::string("test me")));

  DALI_TEST_CHECK( ! trace.FindMethod("ReadMetricsFromCacheFile"));
  DALI_TEST_CHECK( ! trace.FindMethod("WriteMetricsToCacheFile"));
  DALI_TEST_CHECK( ! trace.FindMethod("GetGlyphData"));

  DALI_TEST_EQUALS(size.width, 53.1076f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 7.5868f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);


  Text text = Text(std::string("t"))[0];
  Character c = text[0];
  size = font.MeasureText(c);

  // character size is square
  DALI_TEST_EQUALS(size.width, 7.5868f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 7.5868f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

}

static void UtcFontGetFamilyForText()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::GetFamilyForText()");

  std::string latinFont = Font::GetFamilyForText("Hello world");
  DALI_TEST_CHECK( latinFont.size() != 0 );

  latinFont = Font::GetFamilyForText( Text(std::string("Hello world")) );
  DALI_TEST_CHECK( latinFont.size() != 0 );

  std::string asianFont = Font::GetFamilyForText("繁體中文");
  DALI_TEST_CHECK( asianFont.size() != 0 );

  asianFont = Font::GetFamilyForText(Text(std::string("繁體中文")));
  DALI_TEST_CHECK( asianFont.size() != 0 );

  Text text = Text(std::string("繁體中文"))[0];
  Character c = text[0];

  asianFont = Font::GetFamilyForText(c );
  DALI_TEST_CHECK( asianFont.size() != 0 );

}

static void UtcFontGetFontLineHeightFromCapsHeight()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::GetLineHeightFromCapsHeight()");

  CapsHeight capsHeight(10);
  PixelSize pixelSize = Font::GetLineHeightFromCapsHeight("", "", capsHeight);
  DALI_TEST_CHECK( capsHeight < pixelSize );

  pixelSize = Font::GetLineHeightFromCapsHeight(Font::GetFamilyForText("Hello world"), "", capsHeight);
  DALI_TEST_CHECK( capsHeight < pixelSize );

  pixelSize = Font::GetLineHeightFromCapsHeight(Font::GetFamilyForText(Text(std::string("Hello world"))), "", capsHeight);
  DALI_TEST_CHECK( capsHeight < pixelSize );
}

static void UtcFontAllGlyphsSupported()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::AllGlyphsSupported()");

  Font font = Font::New();

  font.AllGlyphsSupported("Hello World\n");

  DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::AllGlyphsSupportedFunc));
  application.GetPlatform().ResetTrace();

  font.AllGlyphsSupported(Text(std::string("Hello World\n")));

  DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::AllGlyphsSupportedFunc));

  application.GetPlatform().ResetTrace();

  Text text = Text(std::string("t"))[0];
  Character c = text[0];
  font.AllGlyphsSupported(c);

  DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::AllGlyphsSupportedFunc));
}

static void UtcFontGetMetrics()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::UtcFontGetMetrics()");

  Font font = Font::New();
  font.MeasureText(Text(std::string("Hello World"))); // Builds fake metrics in TestPlatformAbstraction.

  float lineHeight = font.GetLineHeight();
  float ascender = font.GetAscender();
  float underlineThickness = font.GetUnderlineThickness();
  float underlinePosition = font.GetUnderlinePosition();
  Font::Metrics metrics = font.GetMetrics( Text("H")[0] );

  // TODO VCC This TET case fails if there are some metrics cached.

  DALI_TEST_EQUALS( lineHeight, 11.380209f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( ascender, 10.242188f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( underlineThickness, 2.276042f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( underlinePosition, 9.104167f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetAdvance(), 11.380209f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetBearing(), 10.242188f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetWidth(), 11.380209f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
}

static void UtcFontIsDefault()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::UtcFontIsDefault()");

  FontParameters fontParams("FreeSans", "", PointSize(10.f));
  Font font1 = Font::New(fontParams);

  DALI_TEST_CHECK( !font1.IsDefaultSystemFont() );
  DALI_TEST_CHECK( !font1.IsDefaultSystemSize() );

  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::ValidateFontFamilyNameFunc ) );
  application.GetPlatform().ResetTrace();

  Font font2 = Font::New();

  DALI_TEST_CHECK( !font2.IsDefaultSystemFont() );
  DALI_TEST_CHECK( font2.IsDefaultSystemSize() );

  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::ValidateFontFamilyNameFunc ) );
}


static void UtcFontGetInstalledFonts()
{
  TestApplication application;

  tet_infoline("Testing Dali::Font::GetInstalledFonts()");

  // the default should only get installed and downloaded fonts
  std::vector<std::string> fontList;

  fontList = Font::GetInstalledFonts(Font::LIST_SYSTEM_FONTS);
  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::ValidateGetFontListFunc ) );
  DALI_TEST_CHECK( application.GetPlatform().GetLastFontListMode() == Dali::Integration::PlatformAbstraction::LIST_SYSTEM_FONTS );

  fontList = Font::GetInstalledFonts(Font::LIST_ALL_FONTS);
  DALI_TEST_CHECK( application.GetPlatform().GetLastFontListMode() == Dali::Integration::PlatformAbstraction::LIST_ALL_FONTS );

  fontList = Font::GetInstalledFonts(Font::LIST_APPLICATION_FONTS);
  DALI_TEST_CHECK( application.GetPlatform().GetLastFontListMode() == Dali::Integration::PlatformAbstraction::LIST_APPLICATION_FONTS );

}

static void UtcFontMetricsDefaultConstructor()
{
  TestApplication application;

  tet_infoline("Testing UtcFontMetricsDefaultConstructor");

  Font::Metrics metrics;

  DALI_TEST_EQUALS( metrics.GetAdvance() , 0.f , Math::MACHINE_EPSILON_10000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetBearing() , 0.f , Math::MACHINE_EPSILON_10000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetWidth() , 0.f , Math::MACHINE_EPSILON_10000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetHeight() , 0.f , Math::MACHINE_EPSILON_10000, TEST_LOCATION );

}
static void UtcFontMetricsCopyConstructor()
{
  TestApplication application;

  tet_infoline("Testing UtcFontMetricsCopyConstructor");

  Font font = Font::New();
  font.MeasureText(Text(std::string("Hello World"))); // Builds fake metrics in TestPlatformAbstraction.
  Font::Metrics metrics = font.GetMetrics( Text("H")[0] );
  Font::Metrics metrics2( metrics );

  DALI_TEST_EQUALS( metrics.GetAdvance() , metrics2.GetAdvance() , Math::MACHINE_EPSILON_10000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetBearing() , metrics2.GetBearing(), Math::MACHINE_EPSILON_10000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetWidth() , metrics2.GetWidth() , Math::MACHINE_EPSILON_10000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetHeight() , metrics2.GetHeight(), Math::MACHINE_EPSILON_10000, TEST_LOCATION );

}
static void UtcFontMetricsAssignmentOperator()
{
  TestApplication application;

  tet_infoline("Testing UtcFontMetricsAssignmentOperator");

  Font font = Font::New();
  font.MeasureText(Text(std::string("Hello World"))); // Builds fake metrics in TestPlatformAbstraction.
  Font::Metrics metrics = font.GetMetrics( Text("H")[0] );
  Font::Metrics metrics2;

  metrics2 = metrics;

  DALI_TEST_EQUALS( metrics.GetAdvance() , metrics2.GetAdvance() , Math::MACHINE_EPSILON_10000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetBearing() , metrics2.GetBearing(), Math::MACHINE_EPSILON_10000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetWidth() , metrics2.GetWidth() , Math::MACHINE_EPSILON_10000, TEST_LOCATION );
  DALI_TEST_EQUALS( metrics.GetHeight() , metrics2.GetHeight(), Math::MACHINE_EPSILON_10000, TEST_LOCATION );

}
