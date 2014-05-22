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
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_text_style_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_style_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static const std::string DEFAULT_FONT_NAME_STYLE( "" );
static const PointSize DEFAULT_FONT_POINT_SIZE( 0.f );

static const std::string FONT_FAMILY( "Arial" );
static const std::string FONT_STYLE( "Bold" );
static const PointSize FONT_POINT_SIZE( 12.f );
static const Vector4 TEXT_COLOR( Color::RED );

static const TextStyle::Weight TEXT_WEIGHT( TextStyle::EXTRALIGHT );
static const float SMOOTH_EDGE( 5.0f );

static const bool ITALICS( true );
static const Degree ITALICS_ANGLE( 10.f );

static const bool UNDERLINE( true );
static const float UNDERLINE_THICKNESS( 5.0f );
static const float UNDERLINE_POSITION( 60.0f );

static const bool SHADOW( true );
static const Vector4 SHADOW_COLOR( Color::BLUE );
static const Vector2 SHADOW_OFFSET( 2.f, 2.f );
static const float SHADOW_SIZE( 55.f );

static const bool GLOW( true );
static const Vector4 GLOW_COLOR( Color::BLACK );
static const float GLOW_INTENSITY( 10.0f );

static const bool OUTLINE( true );
static const Vector4 OUTLINE_COLOR( Color::MAGENTA );
static const Vector2 OUTLINE_THICKNESS( 15.f, 14.f );

static const bool GRADIENT( true );
static const Vector4 GRADIENT_COLOR( Color::YELLOW );
static const Vector2 GRADIENT_START_POINT( 1.f, 1.f );
static const Vector2 GRADIENT_END_POINT( 2.f, 2.f );
} // anon namespace

int UtcDaliTextStyleDefaultConstructor(void)
{
  tet_infoline(" UtcDaliTextStyleDefaultConstructor ");

  TestApplication application;

  TextStyle defaultStyle;

  DALI_TEST_CHECK( defaultStyle.IsFontNameDefault() );
  DALI_TEST_CHECK( defaultStyle.IsFontStyleDefault() );
  DALI_TEST_CHECK( defaultStyle.IsFontSizeDefault() );
  DALI_TEST_CHECK( defaultStyle.IsTextColorDefault() );
  DALI_TEST_CHECK( defaultStyle.IsFontWeightDefault() );
  DALI_TEST_CHECK( defaultStyle.IsSmoothEdgeDefault() );
  DALI_TEST_CHECK( defaultStyle.IsItalicsDefault() );
  DALI_TEST_CHECK( defaultStyle.IsUnderlineDefault() );
  DALI_TEST_CHECK( defaultStyle.IsShadowDefault() );
  DALI_TEST_CHECK( defaultStyle.IsGlowDefault() );
  DALI_TEST_CHECK( defaultStyle.IsOutlineDefault() );
  DALI_TEST_CHECK( defaultStyle.IsGradientDefault() );

  DALI_TEST_EQUALS( defaultStyle.GetFontName(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetFontStyle(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( defaultStyle.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !defaultStyle.IsItalicsEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( !defaultStyle.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !defaultStyle.IsShadowEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( !defaultStyle.IsGlowEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !defaultStyle.IsOutlineEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( !defaultStyle.IsGradientEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleCopyConstructor(void)
{
  tet_infoline(" UtcDaliTextStyleCopyConstructor ");

  TestApplication application;

  TextStyle style;
  style.SetFontName( FONT_FAMILY );
  style.SetFontStyle( FONT_STYLE );
  style.SetFontPointSize( FONT_POINT_SIZE );
  style.SetTextColor( TEXT_COLOR );

  style.SetWeight( TEXT_WEIGHT );
  style.SetSmoothEdge( SMOOTH_EDGE );

  style.SetItalics( ITALICS, ITALICS_ANGLE );
  style.SetUnderline( UNDERLINE, UNDERLINE_THICKNESS, UNDERLINE_POSITION );
  style.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );
  style.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );
  style.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  style.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );

  TextStyle style1( style );
  TextStyle style2 = style;

  DALI_TEST_CHECK( !style1.IsFontNameDefault() );
  DALI_TEST_CHECK( !style1.IsFontStyleDefault() );
  DALI_TEST_CHECK( !style1.IsFontSizeDefault() );
  DALI_TEST_CHECK( !style1.IsTextColorDefault() );
  DALI_TEST_CHECK( !style1.IsFontWeightDefault() );
  DALI_TEST_CHECK( !style1.IsSmoothEdgeDefault() );
  DALI_TEST_CHECK( !style1.IsItalicsDefault() );
  DALI_TEST_CHECK( !style1.IsUnderlineDefault() );
  DALI_TEST_CHECK( !style1.IsShadowDefault() );
  DALI_TEST_CHECK( !style1.IsGlowDefault() );
  DALI_TEST_CHECK( !style1.IsOutlineDefault() );
  DALI_TEST_CHECK( !style1.IsGradientDefault() );

  DALI_TEST_EQUALS( style1.GetFontName(), FONT_FAMILY, TEST_LOCATION );
  DALI_TEST_EQUALS( style1.GetFontStyle(), FONT_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style1.GetFontPointSize(), FONT_POINT_SIZE, TEST_LOCATION );
  DALI_TEST_EQUALS( style1.GetTextColor(), TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( style1.GetWeight(), TEXT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( style1.GetSmoothEdge(), SMOOTH_EDGE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style1.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style1.GetItalicsAngle(), ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( style1.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style1.GetUnderlineThickness(), UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style1.GetUnderlinePosition(), UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style1.IsShadowEnabled() );
  DALI_TEST_EQUALS( style1.GetShadowColor(), SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style1.GetShadowOffset(), SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( style1.IsGlowEnabled() );
  DALI_TEST_EQUALS( style1.GetGlowColor(), GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style1.GetGlowIntensity(), GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style1.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style1.GetOutlineColor(), OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style1.GetOutlineThickness(), OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( style1.IsGradientEnabled() );
  DALI_TEST_EQUALS( style1.GetGradientColor(), GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style1.GetGradientStartPoint(), GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style1.GetGradientEndPoint(), GRADIENT_END_POINT, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsFontNameDefault() );
  DALI_TEST_CHECK( !style2.IsFontStyleDefault() );
  DALI_TEST_CHECK( !style2.IsFontSizeDefault() );
  DALI_TEST_CHECK( !style2.IsTextColorDefault() );
  DALI_TEST_CHECK( !style2.IsFontWeightDefault() );
  DALI_TEST_CHECK( !style2.IsSmoothEdgeDefault() );
  DALI_TEST_CHECK( !style2.IsItalicsDefault() );
  DALI_TEST_CHECK( !style2.IsUnderlineDefault() );
  DALI_TEST_CHECK( !style2.IsShadowDefault() );
  DALI_TEST_CHECK( !style2.IsGlowDefault() );
  DALI_TEST_CHECK( !style2.IsOutlineDefault() );
  DALI_TEST_CHECK( !style2.IsGradientDefault() );

  DALI_TEST_EQUALS( style2.GetFontName(), FONT_FAMILY, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetFontStyle(), FONT_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetFontPointSize(), FONT_POINT_SIZE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetTextColor(), TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( style2.GetWeight(), TEXT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetSmoothEdge(), SMOOTH_EDGE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style2.GetItalicsAngle(), ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style2.GetUnderlineThickness(), UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetUnderlinePosition(), UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsShadowEnabled() );
  DALI_TEST_EQUALS( style2.GetShadowColor(), SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetShadowOffset(), SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsGlowEnabled() );
  DALI_TEST_EQUALS( style2.GetGlowColor(), GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGlowIntensity(), GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style2.GetOutlineColor(), OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetOutlineThickness(), OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsGradientEnabled() );
  DALI_TEST_EQUALS( style2.GetGradientColor(), GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientStartPoint(), GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientEndPoint(), GRADIENT_END_POINT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleAssignmentOperator(void)
{
  tet_infoline(" UtcDaliTextStyleAssignmentOperator ");

  TestApplication application;

  TextStyle style1;
  TextStyle style2;

  style1.SetFontName( FONT_FAMILY );
  style1.SetFontStyle( FONT_STYLE );
  style1.SetFontPointSize( FONT_POINT_SIZE );
  style1.SetTextColor( TEXT_COLOR );

  style1.SetWeight( TEXT_WEIGHT );
  style1.SetSmoothEdge( SMOOTH_EDGE );

  style1.SetItalics( ITALICS, ITALICS_ANGLE );

  style1.SetUnderline( UNDERLINE, UNDERLINE_THICKNESS, UNDERLINE_POSITION );

  style1.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );
  style1.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );
  style1.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  style1.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );

  // Test the copy of all parameters.
  style2 = style1;

  DALI_TEST_CHECK( !style2.IsFontNameDefault() );
  DALI_TEST_CHECK( !style2.IsFontStyleDefault() );
  DALI_TEST_CHECK( !style2.IsFontSizeDefault() );
  DALI_TEST_CHECK( !style2.IsTextColorDefault() );
  DALI_TEST_CHECK( !style2.IsFontWeightDefault() );
  DALI_TEST_CHECK( !style2.IsSmoothEdgeDefault() );
  DALI_TEST_CHECK( !style2.IsItalicsDefault() );
  DALI_TEST_CHECK( !style2.IsUnderlineDefault() );
  DALI_TEST_CHECK( !style2.IsShadowDefault() );
  DALI_TEST_CHECK( !style2.IsGlowDefault() );
  DALI_TEST_CHECK( !style2.IsOutlineDefault() );
  DALI_TEST_CHECK( !style2.IsGradientDefault() );

  DALI_TEST_EQUALS( style2.GetFontName(), FONT_FAMILY, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetFontStyle(), FONT_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetFontPointSize(), FONT_POINT_SIZE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetTextColor(), TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( style2.GetWeight(), TEXT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetSmoothEdge(), SMOOTH_EDGE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style2.GetItalicsAngle(), ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style2.GetUnderlineThickness(), UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetUnderlinePosition(), UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsShadowEnabled() );
  DALI_TEST_EQUALS( style2.GetShadowColor(), SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetShadowOffset(), SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsGlowEnabled() );
  DALI_TEST_EQUALS( style2.GetGlowColor(), GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGlowIntensity(), GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style2.GetOutlineColor(), OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetOutlineThickness(), OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( style2.IsGradientEnabled() );
  DALI_TEST_EQUALS( style2.GetGradientColor(), GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientStartPoint(), GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientEndPoint(), GRADIENT_END_POINT, TEST_LOCATION );

  // Test assignment of the same object.
  const TextStyle& style( style2 );

  style2 = style;

  // Test the assignment of the defaults.

  // Test copy all defaults.

  TextStyle defaultStyle;
  style2 = defaultStyle;

  DALI_TEST_CHECK( style2.IsFontNameDefault() );
  DALI_TEST_CHECK( style2.IsFontStyleDefault() );
  DALI_TEST_CHECK( style2.IsFontSizeDefault() );
  DALI_TEST_CHECK( style2.IsTextColorDefault() );
  DALI_TEST_CHECK( style2.IsFontWeightDefault() );
  DALI_TEST_CHECK( style2.IsSmoothEdgeDefault() );
  DALI_TEST_CHECK( style2.IsItalicsDefault() );
  DALI_TEST_CHECK( style2.IsUnderlineDefault() );
  DALI_TEST_CHECK( style2.IsShadowDefault() );
  DALI_TEST_CHECK( style2.IsGlowDefault() );
  DALI_TEST_CHECK( style2.IsOutlineDefault() );
  DALI_TEST_CHECK( style2.IsGradientDefault() );

  DALI_TEST_EQUALS( style2.GetFontName(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetFontStyle(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( style2.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style2.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style2.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsShadowEnabled() );
  DALI_TEST_EQUALS( style2.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsGlowEnabled() );
  DALI_TEST_EQUALS( style2.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style2.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsGradientEnabled() );
  DALI_TEST_EQUALS( style2.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );

  // Test copy some defaults.

  // set style2 with non defaults.
  style2 = style1;

  // create a style with all defaults excepd the font name.
  defaultStyle.SetFontName( FONT_FAMILY );

  // should reset all to defaults except the font name.
  style2 = defaultStyle;

  DALI_TEST_CHECK( !style2.IsFontNameDefault() );
  DALI_TEST_CHECK( style2.IsFontStyleDefault() );
  DALI_TEST_CHECK( style2.IsFontSizeDefault() );
  DALI_TEST_CHECK( style2.IsTextColorDefault() );
  DALI_TEST_CHECK( style2.IsFontWeightDefault() );
  DALI_TEST_CHECK( style2.IsSmoothEdgeDefault() );
  DALI_TEST_CHECK( style2.IsItalicsDefault() );
  DALI_TEST_CHECK( style2.IsUnderlineDefault() );
  DALI_TEST_CHECK( style2.IsShadowDefault() );
  DALI_TEST_CHECK( style2.IsGlowDefault() );
  DALI_TEST_CHECK( style2.IsOutlineDefault() );
  DALI_TEST_CHECK( style2.IsGradientDefault() );

  DALI_TEST_EQUALS( style2.GetFontName(), FONT_FAMILY, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetFontStyle(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( style2.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style2.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style2.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsShadowEnabled() );
  DALI_TEST_EQUALS( style2.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsGlowEnabled() );
  DALI_TEST_EQUALS( style2.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style2.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsGradientEnabled() );
  DALI_TEST_EQUALS( style2.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );

  // reset the text-style to default.
  defaultStyle = TextStyle();

  // test now the default font name.
  defaultStyle.SetFontStyle( FONT_STYLE );

  style2 = defaultStyle;

  DALI_TEST_CHECK( style2.IsFontNameDefault() );
  DALI_TEST_CHECK( !style2.IsFontStyleDefault() );
  DALI_TEST_CHECK( style2.IsFontSizeDefault() );
  DALI_TEST_CHECK( style2.IsTextColorDefault() );
  DALI_TEST_CHECK( style2.IsFontWeightDefault() );
  DALI_TEST_CHECK( style2.IsSmoothEdgeDefault() );
  DALI_TEST_CHECK( style2.IsItalicsDefault() );
  DALI_TEST_CHECK( style2.IsUnderlineDefault() );
  DALI_TEST_CHECK( style2.IsShadowDefault() );
  DALI_TEST_CHECK( style2.IsGlowDefault() );
  DALI_TEST_CHECK( style2.IsOutlineDefault() );
  DALI_TEST_CHECK( style2.IsGradientDefault() );

  DALI_TEST_EQUALS( style2.GetFontName(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetFontStyle(), FONT_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( style2.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style2.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style2.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsShadowEnabled() );
  DALI_TEST_EQUALS( style2.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsGlowEnabled() );
  DALI_TEST_EQUALS( style2.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style2.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( !style2.IsGradientEnabled() );
  DALI_TEST_EQUALS( style2.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleComparisonOperator(void)
{
  tet_infoline(" UtcDaliTextStyleComparisonOperator ");

  TestApplication application;

  TextStyle style1;
  TextStyle style2;

  // Compares equality and inequality operators

  // Compare two defaults.

  DALI_TEST_CHECK( style1 == style2 );
  DALI_TEST_CHECK( !( style1 != style2 ) );

  // Compare with defaults.

  style1.SetFontName( FONT_FAMILY );

  DALI_TEST_CHECK( !( style1 == style2 ) );
  DALI_TEST_CHECK( style1 != style2 );

  style1 = TextStyle();
  style2.SetFontName( FONT_FAMILY );

  DALI_TEST_CHECK( !( style1 == style2 ) );
  DALI_TEST_CHECK( style1 != style2 );

  // Compare non defaults.

  // Compare different flags

  style1.SetFontName( FONT_FAMILY );
  style2.SetFontStyle( FONT_STYLE );
  style1.SetFontPointSize( FONT_POINT_SIZE );
  style2.SetTextColor( TEXT_COLOR );

  style1.SetWeight( TEXT_WEIGHT );
  style2.SetSmoothEdge( SMOOTH_EDGE );

  style1.SetItalics( ITALICS, ITALICS_ANGLE );

  style2.SetUnderline( UNDERLINE, UNDERLINE_THICKNESS, UNDERLINE_POSITION );

  style1.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );
  style2.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );
  style1.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  style2.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );

  DALI_TEST_CHECK( !( style1 == style2 ) );
  DALI_TEST_CHECK( style1 != style2 );

  // Compare same styles
  style1 = style2;

  DALI_TEST_CHECK( style1 == style2 );
  DALI_TEST_CHECK( !( style1 != style2 ) );

  // Compare same flags but different values
  style1.SetTextColor( Color::GREEN );

  DALI_TEST_CHECK( !( style1 == style2 ) );
  DALI_TEST_CHECK( style1 != style2 );

  END_TEST;
}

int UtcDaliTextStyleCopy(void)
{
  tet_infoline(" UtcDaliTextStyleCopy ");

  TestApplication application;

  TextStyle style;

  // Set a style different than default.
  TextStyle style2;
  style2.SetFontName( FONT_FAMILY );
  style2.SetFontStyle( FONT_STYLE );
  style2.SetFontPointSize( FONT_POINT_SIZE );
  style2.SetTextColor( TEXT_COLOR );

  style2.SetWeight( TEXT_WEIGHT );
  style2.SetSmoothEdge( SMOOTH_EDGE );

  style2.SetItalics( ITALICS, ITALICS_ANGLE );
  style2.SetUnderline( UNDERLINE, UNDERLINE_THICKNESS, UNDERLINE_POSITION );
  style2.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );
  style2.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );
  style2.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  style2.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );

  // Test not to copy the same object. To increase coverage.

  const TextStyle& same( style2 );

  style2.Copy( same );

  DALI_TEST_CHECK( same == style2 );

  // Test mask.

  // All values are copied.
  style.Copy( style2 );

  DALI_TEST_CHECK( style == style2 );

  // Copy the default
  TextStyle defaultStyle;

  style.Copy( defaultStyle );

  // Test FONT
  style = TextStyle();

  style.Copy( style2, TextStyle::FONT );

  DALI_TEST_EQUALS( style.GetFontName(), FONT_FAMILY, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::FONT );

  DALI_TEST_EQUALS( style.GetFontName(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );

  // Test STYLE
  style = TextStyle();

  style.Copy( style2, TextStyle::STYLE );

  DALI_TEST_EQUALS( style.GetFontStyle(), FONT_STYLE, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::STYLE );

  DALI_TEST_EQUALS( style.GetFontStyle(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );

  // Test SIZE
  style = TextStyle();

  style.Copy( style2, TextStyle::SIZE );

  DALI_TEST_EQUALS( style.GetFontPointSize(), FONT_POINT_SIZE, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::SIZE );

  DALI_TEST_EQUALS( style.GetFontPointSize(), PointSize( 0.f ), TEST_LOCATION );

  // Test COLOR
  style = TextStyle();

  style.Copy( style2, TextStyle::COLOR );

  DALI_TEST_EQUALS( style.GetTextColor(), TEXT_COLOR, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::COLOR );

  DALI_TEST_EQUALS( style.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  // Test WEIGHT
  style = TextStyle();

  style.Copy( style2, TextStyle::WEIGHT );

  DALI_TEST_EQUALS( style.GetWeight(), TEXT_WEIGHT, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::WEIGHT );

  DALI_TEST_EQUALS( style.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );

  // Test SMOOTH
  style = TextStyle();

  style.Copy( style2, TextStyle::SMOOTH );

  DALI_TEST_EQUALS( style.GetSmoothEdge(), SMOOTH_EDGE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::SMOOTH );

  DALI_TEST_EQUALS( style.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test ITALICS
  style = TextStyle();

  style.Copy( style2, TextStyle::ITALICS );

  DALI_TEST_CHECK( style.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style.GetItalicsAngle(), ITALICS_ANGLE, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::ITALICS );

  DALI_TEST_CHECK( !style.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  // Test UNDERLINE
  style = TextStyle();

  style.Copy( style2, TextStyle::UNDERLINE );

  DALI_TEST_CHECK( style.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style.GetUnderlineThickness(), UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetUnderlinePosition(), UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::UNDERLINE );

  DALI_TEST_CHECK( !style.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test SHADOW
  style = TextStyle();

  style.Copy( style2, TextStyle::SHADOW );

  DALI_TEST_CHECK( style.IsShadowEnabled() );
  DALI_TEST_EQUALS( style.GetShadowColor(), SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetShadowOffset(), SHADOW_OFFSET, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetShadowSize(), SHADOW_SIZE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::SHADOW );

  DALI_TEST_CHECK( !style.IsShadowEnabled() );
  DALI_TEST_EQUALS( style.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetShadowSize(), TextStyle::DEFAULT_SHADOW_SIZE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test GLOW
  style = TextStyle();

  style.Copy( style2, TextStyle::GLOW );

  DALI_TEST_CHECK( style.IsGlowEnabled() );
  DALI_TEST_EQUALS( style.GetGlowColor(), GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGlowIntensity(), GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::GLOW );

  DALI_TEST_CHECK( !style.IsGlowEnabled() );
  DALI_TEST_EQUALS( style.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test OUTLINE
  style = TextStyle();

  style.Copy( style2, TextStyle::OUTLINE );

  DALI_TEST_CHECK( style.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style.GetOutlineColor(), OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetOutlineThickness(), OUTLINE_THICKNESS, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::OUTLINE );

  DALI_TEST_CHECK( !style.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  // Test GRADIENT
  style = TextStyle();

  style.Copy( style2, TextStyle::GRADIENT );

  DALI_TEST_CHECK( style.IsGradientEnabled() );
  DALI_TEST_EQUALS( style.GetGradientColor(), GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGradientStartPoint(), GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGradientEndPoint(), GRADIENT_END_POINT, TEST_LOCATION );

  style.Copy( defaultStyle, TextStyle::GRADIENT );

  DALI_TEST_CHECK( !style.IsGradientEnabled() );
  DALI_TEST_EQUALS( style.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );

  // Copy to a default.
  defaultStyle.Copy( style2 );
  DALI_TEST_CHECK( defaultStyle ==  style2);

  // Copy from default.
  defaultStyle = TextStyle();

  defaultStyle.SetItalics( ITALICS, ITALICS_ANGLE );
  defaultStyle.SetUnderline( UNDERLINE, UNDERLINE_THICKNESS, UNDERLINE_POSITION );
  defaultStyle.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );
  defaultStyle.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );
  defaultStyle.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  defaultStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );

  // Copy the default values
  style2.Copy( defaultStyle, static_cast<TextStyle::Mask>( TextStyle::FONT      |
                                                           TextStyle::STYLE     |
                                                           TextStyle::SIZE      |
                                                           TextStyle::COLOR     |
                                                           TextStyle::WEIGHT    |
                                                           TextStyle::SMOOTH ) );

  defaultStyle = TextStyle();

  defaultStyle.SetFontName( FONT_FAMILY );
  defaultStyle.SetFontStyle( FONT_STYLE );
  defaultStyle.SetFontPointSize( FONT_POINT_SIZE );
  defaultStyle.SetTextColor( TEXT_COLOR );
  defaultStyle.SetWeight( TEXT_WEIGHT );
  defaultStyle.SetSmoothEdge( SMOOTH_EDGE );

  // Copy the default values
  style2.Copy( defaultStyle, static_cast<TextStyle::Mask>( TextStyle::ITALICS   |
                                                           TextStyle::UNDERLINE |
                                                           TextStyle::SHADOW    |
                                                           TextStyle::GLOW      |
                                                           TextStyle::OUTLINE   |
                                                           TextStyle::GRADIENT ) );

  defaultStyle = TextStyle();

  DALI_TEST_CHECK( defaultStyle == style2 );

  END_TEST;
}

int UtcDaliTextStyleReset(void)
{
  tet_infoline(" UtcDaliTextStyleReset ");

  TestApplication application;

  const TextStyle defaultStyle;

  // Reset a default.
  TextStyle style;

  style.Reset();

  DALI_TEST_CHECK( defaultStyle == style );

  // Reset NONE
  style.Reset( TextStyle::NONE );

  DALI_TEST_CHECK( defaultStyle == style );

  // Reset ALL
  TextStyle style2;
  style2.SetFontName( FONT_FAMILY );
  style2.SetFontStyle( FONT_STYLE );
  style2.SetFontPointSize( FONT_POINT_SIZE );
  style2.SetTextColor( TEXT_COLOR );

  style2.SetWeight( TEXT_WEIGHT );
  style2.SetSmoothEdge( SMOOTH_EDGE );

  style2.SetItalics( ITALICS, ITALICS_ANGLE );
  style2.SetUnderline( UNDERLINE, UNDERLINE_THICKNESS, UNDERLINE_POSITION );
  style2.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );
  style2.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );
  style2.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  style2.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );

  style2.Reset( TextStyle::ALL );

  DALI_TEST_CHECK( defaultStyle == style2 );

  // Set all parameters
  style2.SetFontName( FONT_FAMILY );
  style2.SetFontStyle( FONT_STYLE );
  style2.SetFontPointSize( FONT_POINT_SIZE );
  style2.SetTextColor( TEXT_COLOR );

  style2.SetWeight( TEXT_WEIGHT );
  style2.SetSmoothEdge( SMOOTH_EDGE );

  style2.SetItalics( ITALICS, ITALICS_ANGLE );
  style2.SetUnderline( UNDERLINE, UNDERLINE_THICKNESS, UNDERLINE_POSITION );
  style2.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );
  style2.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );
  style2.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  style2.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );

  // Reset FONT
  style2.Reset( TextStyle::FONT );

  DALI_TEST_CHECK( style2.IsFontNameDefault() );
  DALI_TEST_EQUALS( style2.GetFontName(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );

  // Reset STYLE
  style2.Reset( TextStyle::STYLE );

  DALI_TEST_CHECK( style2.IsFontStyleDefault() );
  DALI_TEST_EQUALS( style2.GetFontStyle(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );

  // Reset SIZE
  style2.Reset( TextStyle::SIZE );

  DALI_TEST_CHECK( style2.IsFontSizeDefault() );
  DALI_TEST_EQUALS( style2.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );

  // Reset COLOR
  style2.Reset( TextStyle::COLOR );

  DALI_TEST_CHECK( style2.IsTextColorDefault() );
  DALI_TEST_EQUALS( style2.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  // Reset WEIGHT
  style2.Reset( TextStyle::WEIGHT );

  DALI_TEST_CHECK( style2.IsFontWeightDefault() );
  DALI_TEST_EQUALS( style2.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );

  // Reset SMOOTH
  style2.Reset( TextStyle::SMOOTH );

  DALI_TEST_CHECK( style2.IsSmoothEdgeDefault() );
  DALI_TEST_EQUALS( style2.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Reset ITALICS
  style2.Reset( TextStyle::ITALICS );

  DALI_TEST_CHECK( style2.IsItalicsDefault() );
  DALI_TEST_CHECK( !style2.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style2.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  // Reset UNDERLINE
  style2.Reset( TextStyle::UNDERLINE );

  DALI_TEST_CHECK( style2.IsUnderlineDefault() );
  DALI_TEST_CHECK( !style2.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style2.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Reset SHADOW
  style2.Reset( TextStyle::SHADOW );

  DALI_TEST_CHECK( style2.IsShadowDefault() );
  DALI_TEST_CHECK( !style2.IsShadowEnabled() );
  DALI_TEST_EQUALS( style2.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetShadowSize(), TextStyle::DEFAULT_SHADOW_SIZE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Reset GLOW
  style2.Reset( TextStyle::GLOW );

  DALI_TEST_CHECK( style2.IsGlowDefault() );
  DALI_TEST_CHECK( !style2.IsGlowEnabled() );
  DALI_TEST_EQUALS( style2.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Reset OUTLINE
  style2.Reset( TextStyle::OUTLINE );

  DALI_TEST_CHECK( style2.IsOutlineDefault() );
  DALI_TEST_CHECK( !style2.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style2.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  // Reset GRADIENT
  style2.Reset( TextStyle::GRADIENT );

  DALI_TEST_CHECK( style2.IsGradientDefault() );
  DALI_TEST_CHECK( !style2.IsGradientEnabled() );
  DALI_TEST_EQUALS( style2.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style2.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );


  // Added to increase coverage.

  {
  // Reset the font name when there is another parameter of a different struct.
  // Test when the struct storing the font name is in the last position of the vector and when is not.

  TextStyle nameStyle;

  nameStyle.SetFontName( FONT_FAMILY );
  nameStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  nameStyle.Reset( TextStyle::FONT );

  DALI_TEST_EQUALS( nameStyle.GetFontName(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );

  nameStyle.SetFontName( FONT_FAMILY );
  nameStyle.Reset( TextStyle::FONT );

  DALI_TEST_EQUALS( nameStyle.GetFontName(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );
  }

  {
  // Reset the font style when there is another parameter of a different struct.
  // Test when the struct storing the font style is in the last position of the vector and when is not.

  TextStyle styleStyle;

  styleStyle.SetFontStyle( FONT_STYLE );
  styleStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  styleStyle.Reset( TextStyle::STYLE );

  DALI_TEST_EQUALS( styleStyle.GetFontStyle(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );

  styleStyle.SetFontStyle( FONT_STYLE );
  styleStyle.Reset( TextStyle::STYLE );

  DALI_TEST_EQUALS( styleStyle.GetFontStyle(), DEFAULT_FONT_NAME_STYLE, TEST_LOCATION );
  }

  {
  // Reset the font size when there is another parameter of a different struct.
  // Test when the struct storing the font size is in the last position of the vector and when is not.

  TextStyle sizeStyle;

  sizeStyle.SetFontPointSize( FONT_POINT_SIZE );
  sizeStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  sizeStyle.Reset( TextStyle::SIZE );

  DALI_TEST_EQUALS( sizeStyle.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );

  sizeStyle.SetFontPointSize( FONT_POINT_SIZE );
  sizeStyle.Reset( TextStyle::SIZE );

  DALI_TEST_EQUALS( sizeStyle.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );
  }

  {
  // Reset the text color when there is another parameter of a different struct.
  // Test when the struct storing the text color is in the last position of the vector and when is not.

  TextStyle colorStyle;

  colorStyle.SetTextColor( TEXT_COLOR );
  colorStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  colorStyle.Reset( TextStyle::COLOR );

  DALI_TEST_EQUALS( colorStyle.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  colorStyle.SetTextColor( TEXT_COLOR );
  colorStyle.Reset( TextStyle::COLOR );

  DALI_TEST_EQUALS( colorStyle.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );
  }

  {
  // Reset the text weight when there is another parameter of a different struct.
  // Test when the struct storing the text weight is in the last position of the vector and when is not.

  TextStyle weightStyle;

  weightStyle.SetWeight( TEXT_WEIGHT );
  weightStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  weightStyle.Reset( TextStyle::WEIGHT );

  DALI_TEST_EQUALS( weightStyle.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );

  weightStyle.SetWeight( TEXT_WEIGHT );
  weightStyle.Reset( TextStyle::WEIGHT );

  DALI_TEST_EQUALS( weightStyle.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );
  }

  {
  // Reset the smooth edge when there is another parameter of a different struct.
  // Test when the struct storing the smooth edge is in the last position of the vector and when is not.

  TextStyle smoothStyle;

  smoothStyle.SetSmoothEdge( SMOOTH_EDGE );
  smoothStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  smoothStyle.Reset( TextStyle::SMOOTH );

  DALI_TEST_EQUALS( smoothStyle.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  smoothStyle.SetSmoothEdge( SMOOTH_EDGE );
  smoothStyle.Reset( TextStyle::SMOOTH );

  DALI_TEST_EQUALS( smoothStyle.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  }

  {
  // Reset the italics when there is another parameter of a different struct.
  // Test when the struct storing the italics is in the last position of the vector and when is not.

  TextStyle italicsStyle;

  italicsStyle.SetItalics( ITALICS, ITALICS_ANGLE );
  italicsStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  italicsStyle.Reset( TextStyle::ITALICS );

  DALI_TEST_EQUALS( italicsStyle.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  italicsStyle.SetItalics( ITALICS, ITALICS_ANGLE );
  italicsStyle.Reset( TextStyle::ITALICS );

  DALI_TEST_EQUALS( italicsStyle.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );
  }

  {
  // Reset the underline when there is another parameter of a different struct.
  // Test when the struct storing the underline is in the last position of the vector and when is not.

  TextStyle underlineStyle;

  underlineStyle.SetUnderline( UNDERLINE, UNDERLINE_THICKNESS, UNDERLINE_POSITION );
  underlineStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  underlineStyle.Reset( TextStyle::UNDERLINE );

  DALI_TEST_EQUALS( underlineStyle.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( underlineStyle.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  underlineStyle.SetUnderline( UNDERLINE, UNDERLINE_THICKNESS, UNDERLINE_POSITION );
  underlineStyle.Reset( TextStyle::UNDERLINE );

  DALI_TEST_EQUALS( underlineStyle.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( underlineStyle.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  }





  {
  // Reset the shadow when there is another parameter of a different struct.
  // Test when the struct storing the shadow is in the last position of the vector and when is not.

  TextStyle shadowStyle;

  shadowStyle.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );
  shadowStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  shadowStyle.Reset( TextStyle::SHADOW );

  DALI_TEST_CHECK( shadowStyle.IsShadowDefault() );

  shadowStyle.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );
  shadowStyle.Reset( TextStyle::SHADOW );

  DALI_TEST_CHECK( shadowStyle.IsShadowDefault() );
}

  {
  // Reset the glow when there is another parameter of a different struct.
  // Test when the struct storing the glow is in the last position of the vector and when is not.

  TextStyle glowStyle;

  glowStyle.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );
  glowStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  glowStyle.Reset( TextStyle::GLOW );

  DALI_TEST_CHECK( glowStyle.IsGlowDefault() );

  glowStyle.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );
  glowStyle.Reset( TextStyle::GLOW );

  DALI_TEST_CHECK( glowStyle.IsGlowDefault() );
  }

  {
  // Reset the outline when there is another parameter of a different struct.
  // Test when the struct storing the outline is in the last position of the vector and when is not.

  TextStyle outlineStyle;

  outlineStyle.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  outlineStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  outlineStyle.Reset( TextStyle::OUTLINE );

  DALI_TEST_CHECK( outlineStyle.IsOutlineDefault() );

  outlineStyle.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  outlineStyle.Reset( TextStyle::OUTLINE );

  DALI_TEST_CHECK( outlineStyle.IsOutlineDefault() );
  }

  {
  // Reset the gradient when there is another parameter of a different struct.
  // Test when the struct storing the gradient is in the last position of the vector and when is not.

  TextStyle gradientStyle;

  gradientStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  gradientStyle.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  gradientStyle.Reset( TextStyle::GRADIENT );

  DALI_TEST_CHECK( gradientStyle.IsGradientDefault() );

  gradientStyle.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );
  gradientStyle.Reset( TextStyle::GRADIENT );

  DALI_TEST_CHECK( gradientStyle.IsGradientDefault() );
  }

  END_TEST;
}

int UtcDaliTextStyleSetGetFontName(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetFontName ");

  TestApplication application;

  TextStyle style;

  style.SetFontName( FONT_FAMILY );

  DALI_TEST_EQUALS( style.GetFontName(), FONT_FAMILY, TEST_LOCATION );

  // Added to increase coverage
  style.SetFontStyle( FONT_STYLE );
  DALI_TEST_EQUALS( style.GetFontStyle(), FONT_STYLE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleSetGetFontStyle(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetFontStyle ");

  TestApplication application;

  TextStyle style;

  style.SetFontStyle( FONT_STYLE );

  DALI_TEST_EQUALS( style.GetFontStyle(), FONT_STYLE, TEST_LOCATION );

  // Added to increase coverage
  style.SetFontName( FONT_FAMILY );
  DALI_TEST_EQUALS( style.GetFontName(), FONT_FAMILY, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleSetGetFontPointSize(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetFontPointSize ");

  TestApplication application;

  TextStyle style;

  style.SetFontPointSize( FONT_POINT_SIZE );

  DALI_TEST_EQUALS( style.GetFontPointSize(), FONT_POINT_SIZE, TEST_LOCATION );

  // Added to increase coverage
  style.SetTextColor( TEXT_COLOR );
  DALI_TEST_EQUALS( style.GetTextColor(), TEXT_COLOR, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleSetGetTextColor(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetTextColor ");

  TestApplication application;

  TextStyle style;

  style.SetTextColor( TEXT_COLOR );

  DALI_TEST_EQUALS( style.GetTextColor(), TEXT_COLOR, TEST_LOCATION );

  // Added to increase coverage
  style.SetFontPointSize( FONT_POINT_SIZE );
  DALI_TEST_EQUALS( style.GetFontPointSize(), FONT_POINT_SIZE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleSetGetWeight(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetWeight ");

  TestApplication application;

  TextStyle style;

  style.SetWeight( TEXT_WEIGHT );

  DALI_TEST_EQUALS( style.GetWeight(), TEXT_WEIGHT, TEST_LOCATION );

  // Added to increase coverage
  style.SetSmoothEdge( SMOOTH_EDGE );
  DALI_TEST_EQUALS( style.GetSmoothEdge(), SMOOTH_EDGE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleSetGetSmoothEdge(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetGlow ");

  TestApplication application;

  TextStyle style;

  style.SetSmoothEdge( SMOOTH_EDGE );

  DALI_TEST_EQUALS( style.GetSmoothEdge(), SMOOTH_EDGE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Added to increase coverage
  style.SetWeight( TEXT_WEIGHT );
  DALI_TEST_EQUALS( style.GetWeight(), TEXT_WEIGHT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleSetGetItalics(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetItalics ");

  TestApplication application;

  TextStyle style;

  DALI_TEST_EQUALS( style.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  style.SetItalics( ITALICS, ITALICS_ANGLE );

  DALI_TEST_CHECK( style.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style.GetItalicsAngle(), ITALICS_ANGLE, TEST_LOCATION );

  style.SetItalics( !ITALICS, TextStyle::DEFAULT_ITALICS_ANGLE  );

  DALI_TEST_CHECK( !style.IsItalicsEnabled() );

  END_TEST;
}

int UtcDaliTextStyleSetGetUnderline(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetUnderline ");

  TestApplication application;

  TextStyle style;

  style.SetUnderline( UNDERLINE, UNDERLINE_THICKNESS, UNDERLINE_POSITION );

  DALI_TEST_CHECK( style.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style.GetUnderlineThickness(), UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetUnderlinePosition(), UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  style.SetUnderline( !UNDERLINE, TextStyle::DEFAULT_UNDERLINE_THICKNESS, TextStyle::DEFAULT_UNDERLINE_POSITION );

  DALI_TEST_CHECK( !style.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleSetGetShadow(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetShadow ");

  TestApplication application;

  TextStyle style;

  DALI_TEST_CHECK( !style.IsShadowEnabled() );

  style.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );

  DALI_TEST_CHECK( style.IsShadowEnabled() );
  DALI_TEST_EQUALS( style.GetShadowColor(), SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetShadowOffset(), SHADOW_OFFSET, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetShadowSize(), SHADOW_SIZE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  style.SetShadow( !SHADOW, TextStyle::DEFAULT_SHADOW_COLOR, TextStyle::DEFAULT_SHADOW_OFFSET, TextStyle::DEFAULT_SHADOW_SIZE );
  DALI_TEST_CHECK( !style.IsShadowEnabled() );
  DALI_TEST_EQUALS( style.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetShadowSize(), TextStyle::DEFAULT_SHADOW_SIZE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleSetGetGlow(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetGlow ");

  TestApplication application;

  TextStyle style;

  DALI_TEST_CHECK( !style.IsGlowEnabled() );

  style.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );

  DALI_TEST_CHECK( style.IsGlowEnabled() );
  DALI_TEST_EQUALS( style.GetGlowColor(), GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGlowIntensity(), GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  style.SetGlow( !GLOW, TextStyle::DEFAULT_GLOW_COLOR, TextStyle::DEFAULT_GLOW_INTENSITY );

  DALI_TEST_CHECK( !style.IsGlowEnabled() );
  DALI_TEST_EQUALS( style.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleSetGetOutline(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetOutline ");

  TestApplication application;

  TextStyle style;

  DALI_TEST_CHECK( !style.IsOutlineEnabled() );

  style.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );

  DALI_TEST_CHECK( style.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style.GetOutlineColor(), OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetOutlineThickness(), OUTLINE_THICKNESS, TEST_LOCATION );

  style.SetOutline( !OUTLINE, TextStyle::DEFAULT_OUTLINE_COLOR, TextStyle::DEFAULT_OUTLINE_THICKNESS );

  DALI_TEST_CHECK( !style.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextStyleSetGetGradient(void)
{
  tet_infoline(" UtcDaliTextStyleSetGetGradient ");

  TestApplication application;

  TextStyle style;

  DALI_TEST_CHECK( !style.IsGradientEnabled() );

  style.SetGradient( GRADIENT, GRADIENT_COLOR, GRADIENT_START_POINT, GRADIENT_END_POINT );

  DALI_TEST_CHECK( style.IsGradientEnabled() );
  DALI_TEST_EQUALS( style.GetGradientColor(), GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGradientStartPoint(), GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGradientEndPoint(), GRADIENT_END_POINT, TEST_LOCATION );

  style.SetGradient( !GRADIENT, TextStyle::DEFAULT_GRADIENT_COLOR, TextStyle::DEFAULT_GRADIENT_START_POINT, TextStyle::DEFAULT_GRADIENT_END_POINT );

  DALI_TEST_CHECK( !style.IsGradientEnabled() );
  DALI_TEST_EQUALS( style.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );

  END_TEST;
}
