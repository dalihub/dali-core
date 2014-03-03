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

TEST_FUNCTION( UtcDaliTextStyleDefaultConstructor,  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleCopyConstructor,     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleComparisonOperator,  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleCopy,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetFontName,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetFontStyle,     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetFontPointSize, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetWeight,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetTextColor,     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetItalics,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetItalicsAngle,  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetUnderline,     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetShadow,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetGlow,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetOutline,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextStyleSetGetSmoothEdge,    POSITIVE_TC_IDX );


static const std::string FONT_FAMILY="Arial";
static const std::string FONT_STYLE="Bold";
static const PointSize FONT_POINT=PointSize(12.f);
static const TextStyle::Weight FONT_TEXT_STYLE_WEIGHT = TextStyle::EXTRALIGHT;

static const Vector4 FONT_TEXT_COLOR = Color::RED;
static const Degree FONT_ITALICS_ANGLE(10.f);
static const bool FONT_ITALICS = true;
static const bool FONT_UNDERLINE = true;
static const bool FONT_SHADOW = true;
static const bool FONT_GLOW = true;
static const bool FONT_OUTLINE = true;
static const float FONT_UNDERLINE_THICKNESS = 5.0f;
static const float FONT_UNDERLINE_POSITION = 60.0f;
static const Vector4 FONT_SHADOW_COLOR = Color::BLUE;
static const Vector2 FONT_SHADOW_OFFSET(2.f, 2.f );
static const float FONT_SHADOW_SIZE = 55.f;
static const Vector4 FONT_TEXT_GLOW_COLOR = Color::BLACK;
static const float FONT_GLOW_INTENSITY = 10.0f;
static const float FONT_SMOOTH_EDGE = 5.0f;
static const Vector4 FONT_OUTLINE_COLOR = Color::MAGENTA;
static const Vector2 FONT_OUTLINE_THICKNESS(15.f, 14.f );


// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliTextStyleDefaultConstructor()
{
  tet_infoline(" UtcDaliTextStyleDefaultConstructor ");

  TestApplication application;

  TextStyle defaultStyle;

  DALI_TEST_CHECK( defaultStyle.GetFontName().empty() );
  DALI_TEST_CHECK( PointSize( 0.f ) == defaultStyle.GetFontPointSize() );
  DALI_TEST_CHECK( TextStyle::REGULAR == defaultStyle.GetWeight() );
  DALI_TEST_CHECK( Color::WHITE == defaultStyle.GetTextColor() );
  DALI_TEST_CHECK( !defaultStyle.GetItalics() );

  DALI_TEST_CHECK( !defaultStyle.GetUnderline() );
  DALI_TEST_CHECK( fabs( TextStyle::DEFAULT_UNDERLINE_THICKNESS - defaultStyle.GetUnderlineThickness() ) < GetRangedEpsilon( TextStyle::DEFAULT_UNDERLINE_THICKNESS, defaultStyle.GetUnderlineThickness() ) );
  DALI_TEST_CHECK( fabs( TextStyle::DEFAULT_UNDERLINE_POSITION - defaultStyle.GetUnderlinePosition() ) < GetRangedEpsilon( TextStyle::DEFAULT_UNDERLINE_POSITION, defaultStyle.GetUnderlinePosition() ) );

  DALI_TEST_CHECK( TextStyle::DEFAULT_ITALICS_ANGLE == defaultStyle.GetItalicsAngle() );
  DALI_TEST_CHECK( TextStyle::DEFAULT_SHADOW_COLOR == defaultStyle.GetShadowColor() );
  DALI_TEST_CHECK( TextStyle::DEFAULT_GLOW_COLOR == defaultStyle.GetGlowColor() );
  DALI_TEST_CHECK( TextStyle::DEFAULT_OUTLINE_COLOR == defaultStyle.GetOutlineColor() );

  DALI_TEST_CHECK( TextStyle::DEFAULT_SHADOW_OFFSET == defaultStyle.GetShadowOffset() );
  DALI_TEST_CHECK( TextStyle::DEFAULT_OUTLINE_THICKNESS == defaultStyle.GetOutlineThickness() );
  DALI_TEST_CHECK( fabs( TextStyle::DEFAULT_GLOW_INTENSITY - defaultStyle.GetGlowIntensity() ) < GetRangedEpsilon( TextStyle::DEFAULT_GLOW_INTENSITY, defaultStyle.GetGlowIntensity() ) );
  DALI_TEST_CHECK( fabs( TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD - defaultStyle.GetSmoothEdge() ) < GetRangedEpsilon( TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, defaultStyle.GetSmoothEdge() ) );
}

static void UtcDaliTextStyleCopyConstructor()
{
  tet_infoline(" UtcDaliTextStyleCopyConstructor ");

  TestApplication application;

  const std::string fontName( "Arial" );
  const std::string fontStyle( "Book" );

  TextStyle style;
  style.SetFontName( fontName );
  style.SetFontStyle( fontStyle );
  style.SetFontPointSize( PointSize( 16.f ) );
  style.SetWeight( TextStyle::EXTRABLACK );
  style.SetTextColor( Color::BLUE );
  style.SetItalics( true );
  style.SetUnderline( true );
  style.SetItalicsAngle( Degree( 15.f ) );
  style.SetUnderlineThickness( 3.5f );
  style.SetUnderlinePosition( 12.f );
  style.SetShadow( true, Color::RED, Vector2( 0.5f, 0.5f ), TextStyle::DEFAULT_SHADOW_SIZE );
  style.SetGlow( true, Color::GREEN, 0.75f );
  style.SetSmoothEdge( 0.8f );
  style.SetOutline( true, Color::BLUE, Vector2( 0.9f, 0.9f ) );

  TextStyle style1( style );
  TextStyle style2 = style;

  DALI_TEST_CHECK( fontName == style1.GetFontName() );
  DALI_TEST_CHECK( fontStyle == style.GetFontStyle() );
  DALI_TEST_CHECK( PointSize( 16.f ) == style1.GetFontPointSize() );
  DALI_TEST_CHECK( TextStyle::EXTRABLACK == style1.GetWeight() );
  DALI_TEST_CHECK( Vector4( 0.f, 0.f, 1.f, 1.f ) == style1.GetTextColor() );
  DALI_TEST_CHECK( style1.GetItalics() );
  DALI_TEST_CHECK( style1.GetUnderline() );

  DALI_TEST_CHECK( Degree( 15.f ) == style1.GetItalicsAngle() );
  DALI_TEST_CHECK( Color::RED == style1.GetShadowColor() );
  DALI_TEST_CHECK( Color::GREEN == style1.GetGlowColor() );
  DALI_TEST_CHECK( Color::BLUE == style1.GetOutlineColor() );

  DALI_TEST_CHECK( Vector2( 0.5f, 0.5f ) == style1.GetShadowOffset() );
  DALI_TEST_CHECK( Vector2( 0.9f, 0.9f ) == style1.GetOutlineThickness() );
  DALI_TEST_CHECK( fabs( 0.75f - style1.GetGlowIntensity() ) < GetRangedEpsilon( 0.75f, style1.GetGlowIntensity() ) );
  DALI_TEST_CHECK( fabs( 0.8f - style1.GetSmoothEdge() ) < GetRangedEpsilon( 0.8f, style1.GetSmoothEdge() ) );

  DALI_TEST_CHECK( fontName == style2.GetFontName() );
  DALI_TEST_CHECK( PointSize( 16.f ) == style2.GetFontPointSize() );
  DALI_TEST_CHECK( TextStyle::EXTRABLACK == style2.GetWeight() );
  DALI_TEST_CHECK( Vector4( 0.f, 0.f, 1.f, 1.f ) == style2.GetTextColor() );
  DALI_TEST_CHECK( style2.GetItalics() );

  DALI_TEST_CHECK( style2.GetUnderline() );
  DALI_TEST_CHECK( fabs( 3.5f - style2.GetUnderlineThickness() ) < GetRangedEpsilon( 3.5f, style2.GetUnderlineThickness() ) );
  DALI_TEST_CHECK( fabs( 12.f - style2.GetUnderlinePosition() ) < GetRangedEpsilon( 12.f, style2.GetUnderlinePosition() ) );

  DALI_TEST_CHECK( Color::RED == style2.GetShadowColor() );
  DALI_TEST_CHECK( Color::GREEN == style2.GetGlowColor() );
  DALI_TEST_CHECK( Color::BLUE == style2.GetOutlineColor() );

  DALI_TEST_CHECK( Vector2( 0.5f, 0.5f ) == style2.GetShadowOffset() );
  DALI_TEST_CHECK( Vector2( 0.9f, 0.9f ) == style2.GetOutlineThickness() );
  DALI_TEST_CHECK( fabs( 0.75f - style2.GetGlowIntensity() ) < GetRangedEpsilon( 0.75f, style2.GetGlowIntensity() ) );
  DALI_TEST_CHECK( fabs( 0.8f - style2.GetSmoothEdge() ) < GetRangedEpsilon( 0.8f, style2.GetSmoothEdge() ) );
}

static void UtcDaliTextStyleComparisonOperator()
{
  tet_infoline(" UtcDaliTextStyleComparisonOperator ");

  TestApplication application;

  TextStyle style1;
  TextStyle style2;

  const std::string fontName( "Arial" );

  style1.SetFontName( fontName );
  DALI_TEST_CHECK( style1 != style2 );

  style2.SetFontName( fontName );
  DALI_TEST_CHECK( style1 == style2 );
}

static void UtcDaliTextStyleCopy()
{
  tet_infoline(" UtcDaliTextStyleCopy ");

  TestApplication application;

  TextStyle style;

  // Set a style different than default.
  TextStyle style2;
  style2.SetFontName( "Arial" );
  style2.SetFontStyle( "Bold" );
  style2.SetFontPointSize( PointSize( 24.f ) );
  style2.SetWeight( TextStyle::BOLD );
  style2.SetTextColor( Color::RED );
  style2.SetItalics( true );
  style2.SetUnderline( true );
  style2.SetItalicsAngle( Degree( 15.f ) );
  style2.SetUnderlineThickness( 3.5f );
  style2.SetUnderlinePosition( 12.f );
  style2.SetShadow( true, Color::GREEN, Vector2( 0.5f, 0.5f ), 0.5f );
  style2.SetGlow( true, Color::BLUE, 0.5f );
  style2.SetSmoothEdge( 0.5f );
  style2.SetOutline( true, Color::WHITE, Vector2( 0.05f, 0.5f ) );

  // Test mask.

  // All values are copied.
  style.Copy( style2 );

  DALI_TEST_CHECK( style == style2 );

  // Test FONT
  style = TextStyle();

  style.Copy( style2, TextStyle::FONT );

  DALI_TEST_CHECK( style.GetFontName() != "" );

  // Test STYLE
  style = TextStyle();

  style.Copy( style2, TextStyle::STYLE );

  DALI_TEST_CHECK( style.GetFontStyle() != "" );

  // Test SIZE
  style = TextStyle();

  style.Copy( style2, TextStyle::SIZE );

  DALI_TEST_CHECK( style.GetFontPointSize() > 0.f );

  // Test WEIGHT
  style = TextStyle();

  style.Copy( style2, TextStyle::WEIGHT );

  DALI_TEST_CHECK( style.GetWeight() != TextStyle::REGULAR );
  DALI_TEST_CHECK( fabsf( style.GetSmoothEdge() - TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD ) > Math::MACHINE_EPSILON_1000 );

  // Test COLOR
  style = TextStyle();

  style.Copy( style2, TextStyle::COLOR );

  DALI_TEST_CHECK( style.GetTextColor() != TextStyle::DEFAULT_TEXT_COLOR );

  // Test ITALICS
  style = TextStyle();

  style.Copy( style2, TextStyle::ITALICS );

  DALI_TEST_CHECK( style.GetItalics() );
  DALI_TEST_CHECK( Degree( 15.f ) == style.GetItalicsAngle() );

  // Test UNDERLINE
  style = TextStyle();

  style.Copy( style2, TextStyle::UNDERLINE );

  DALI_TEST_CHECK( style.GetUnderline() );
  DALI_TEST_CHECK( fabs( 3.5f - style.GetUnderlineThickness() ) < GetRangedEpsilon( 3.5f, style.GetUnderlineThickness() ) );
  DALI_TEST_CHECK( fabs( 12.f - style.GetUnderlinePosition() ) < GetRangedEpsilon( 12.f, style.GetUnderlinePosition() ) );

  // Test SHADOW
  style = TextStyle();

  style.Copy( style2, TextStyle::SHADOW );

  DALI_TEST_CHECK( style.GetShadow() );
  DALI_TEST_CHECK( style.GetShadowColor() != TextStyle::DEFAULT_SHADOW_COLOR );
  DALI_TEST_CHECK( style.GetShadowOffset() != TextStyle::DEFAULT_SHADOW_OFFSET );
  DALI_TEST_CHECK( fabsf( style.GetShadowSize() - TextStyle::DEFAULT_SHADOW_SIZE ) > Math::MACHINE_EPSILON_1000 );

  // Test GLOW
  style = TextStyle();

  style.Copy( style2, TextStyle::GLOW );

  DALI_TEST_CHECK( style.GetGlow() );
  DALI_TEST_CHECK( style.GetGlowColor() != TextStyle::DEFAULT_GLOW_COLOR );
  DALI_TEST_CHECK( fabsf( style.GetGlowIntensity() - TextStyle::DEFAULT_GLOW_INTENSITY ) > Math::MACHINE_EPSILON_1000 );

  // Test OUTLINE
  style = TextStyle();

  style.Copy( style2, TextStyle::OUTLINE );

  DALI_TEST_CHECK( style.GetOutline() );
  DALI_TEST_CHECK( style.GetOutlineColor() != TextStyle::DEFAULT_OUTLINE_COLOR );
  DALI_TEST_CHECK( style.GetOutlineThickness() != TextStyle::DEFAULT_OUTLINE_THICKNESS );
}

static void UtcDaliTextStyleSetGetFontName()
{
  tet_infoline(" UtcDaliTextStyleSetGetFontName ");

  TestApplication application;

  TextStyle style;

  const std::string fontName( "Arial" );
  style.SetFontName( fontName );

  DALI_TEST_CHECK( fontName == style.GetFontName() );
}

static void UtcDaliTextStyleSetGetFontStyle()
{
  tet_infoline(" UtcDaliTextStyleSetGetFontStyle ");

  TestApplication application;

  TextStyle style;

  const std::string fontStyle( "Book" );
  style.SetFontStyle( fontStyle );

  DALI_TEST_CHECK( fontStyle == style.GetFontStyle() );
}

static void UtcDaliTextStyleSetGetFontPointSize()
{
  tet_infoline(" UtcDaliTextStyleSetGetFontPointSize ");

  TestApplication application;

  TextStyle style;

  style.SetFontPointSize( PointSize( 16.f ) );

  DALI_TEST_CHECK( PointSize( 16.f ) == style.GetFontPointSize() );
}

static void UtcDaliTextStyleSetGetWeight()
{
  tet_infoline(" UtcDaliTextStyleSetGetWeight ");

  TestApplication application;

  TextStyle style;

  style.SetWeight( TextStyle::EXTRABLACK );

  DALI_TEST_CHECK( TextStyle::EXTRABLACK == style.GetWeight() );
}

static void UtcDaliTextStyleSetGetTextColor()
{
  tet_infoline(" UtcDaliTextStyleSetGetTextColor ");

  TestApplication application;

  TextStyle style;

  style.SetTextColor( Color::GREEN );

  DALI_TEST_CHECK( Vector4( 0.f, 1.f, 0.f, 1.f ) == style.GetTextColor() );
}

static void UtcDaliTextStyleSetGetItalics()
{
  tet_infoline(" UtcDaliTextStyleSetGetItalics ");

  TestApplication application;

  TextStyle style;

  style.SetItalics( true );

  DALI_TEST_CHECK( style.GetItalics() );

  style.SetItalics( false );

  DALI_TEST_CHECK( !style.GetItalics() );
}

static void UtcDaliTextStyleSetGetItalicsAngle()
{
  tet_infoline(" UtcDaliTextStyleSetGetItalicsAngle ");

  TestApplication application;

  TextStyle style;

  DALI_TEST_CHECK( TextStyle::DEFAULT_ITALICS_ANGLE == style.GetItalicsAngle() );

  style.SetItalicsAngle( Degree( 15.f ) );

  DALI_TEST_CHECK( Degree( 15.f ) == style.GetItalicsAngle() );
}

static void UtcDaliTextStyleSetGetUnderline()
{
  tet_infoline(" UtcDaliTextStyleSetGetUnderline ");

  TestApplication application;

  TextStyle style;

  style.SetUnderline( true );

  DALI_TEST_CHECK( style.GetUnderline() );

  style.SetUnderline( false );

  DALI_TEST_CHECK( !style.GetUnderline() );

  style.SetUnderlineThickness( 3.5f );
  style.SetUnderlinePosition( 12.f );

  DALI_TEST_CHECK( fabs( 3.5f - style.GetUnderlineThickness() ) < GetRangedEpsilon( 3.5f, style.GetUnderlineThickness() ) );
  DALI_TEST_CHECK( fabs( 12.f - style.GetUnderlinePosition() ) < GetRangedEpsilon( 12.f, style.GetUnderlinePosition() ) );
}

static void UtcDaliTextStyleSetGetShadow()
{
  tet_infoline(" UtcDaliTextStyleSetGetShadow ");

  TestApplication application;

  TextStyle style;

  DALI_TEST_CHECK( !style.GetShadow() );

  style.SetShadow( true );

  DALI_TEST_CHECK( style.GetShadow() );

  style.SetShadow( true, Color::RED, Vector2( 0.5f, 0.5f ) );

  DALI_TEST_EQUALS( style.GetShadowColor(), Color::RED, TEST_LOCATION );
  DALI_TEST_CHECK( Vector2( 0.5f, 0.5f ) == style.GetShadowOffset() );

  style.SetShadow( false );

  DALI_TEST_CHECK( !style.GetShadow() );
}

static void UtcDaliTextStyleSetGetGlow()
{
  tet_infoline(" UtcDaliTextStyleSetGetGlow ");

  TestApplication application;

  TextStyle style;

  DALI_TEST_CHECK( !style.GetGlow() );

  style.SetGlow( true );

  DALI_TEST_CHECK( style.GetGlow() );

  style.SetGlow( true, Color::RED, 0.75f );

  DALI_TEST_EQUALS( style.GetGlowColor(), Color::RED, TEST_LOCATION );
  DALI_TEST_CHECK( fabs( 0.75f - style.GetGlowIntensity() ) < GetRangedEpsilon( 0.75f, style.GetGlowIntensity() ) );

  style.SetGlow( false );

  DALI_TEST_CHECK( !style.GetGlow() );
}

static void UtcDaliTextStyleSetGetOutline()
{
  tet_infoline(" UtcDaliTextStyleSetGetGlow ");

  TestApplication application;

  TextStyle style;

  DALI_TEST_CHECK( !style.GetOutline() );

  style.SetOutline( true );

  DALI_TEST_CHECK( style.GetOutline() );

  style.SetOutline( true, Color::RED, Vector2( 0.9f, 0.9f ) );

  DALI_TEST_EQUALS( style.GetOutlineColor(), Color::RED, TEST_LOCATION );
  DALI_TEST_CHECK( Vector2( 0.9f, 0.9f ) == style.GetOutlineThickness() );

  style.SetOutline( false );

  DALI_TEST_CHECK( !style.GetOutline() );
}

static void UtcDaliTextStyleSetGetSmoothEdge()
{
  tet_infoline(" UtcDaliTextStyleSetGetGlow ");

  TestApplication application;

  TextStyle style;

  style.SetSmoothEdge( 0.5f );

  DALI_TEST_CHECK( fabs( 0.5f - style.GetSmoothEdge() ) < GetRangedEpsilon( 0.5f, style.GetSmoothEdge() ) );
}
