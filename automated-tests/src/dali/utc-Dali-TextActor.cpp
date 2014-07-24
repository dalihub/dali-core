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

void text_actor_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void text_actor_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static const char* TestTextHello = "Hello";
static const char* TestTextHelloWorld = "Hello World";
static const char* LongTestText = "This is a very long piece of text, and is sure not to fit into any box presented to it";

static const std::string DEFAULT_NAME_STYLE( "" );
static const PointSize DEFAULT_FONT_POINT_SIZE( 0.f );

static const std::string FONT_FAMILY( "Arial" );
static const std::string FONT_STYLE( "Bold" );
static const PointSize FONT_POINT_SIZE( 12.f );
static const Vector4 TEXT_COLOR( Color::RED );

static const TextStyle::Weight TEXT_WEIGHT( TextStyle::EXTRALIGHT );
static const float SMOOTH_EDGE( 5.0f );

static const bool ITALICS( true );
static const Degree ITALICS_ANGLE( 10.f );
static const Radian ITALICS_RADIAN_ANGLE(0.4f);

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

int UtcDaliTextActorConstructorVoid(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::TextActor()");

  TextActor actor;

  DALI_TEST_CHECK(!actor);
  END_TEST;
}

int UtcDaliTextActorNew01(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)));

  DALI_TEST_CHECK(actor);
  END_TEST;
}


int UtcDaliTextActorNew02(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  TextActor actor = TextActor::New(TestTextHello, false);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)), false);

  DALI_TEST_CHECK(actor);
  END_TEST;
}

int UtcDaliTextActorNew03(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  TextActor actor = TextActor::New(TestTextHello, false, false);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)), false, false);

  DALI_TEST_CHECK(actor);
  END_TEST;
}


int UtcDaliTextActorNew04(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  FontParameters parameters( "FreeSerif", "Book", PointSize(8) );
  Font freeSerif = Font::New( parameters );

  TextActor actor = TextActor::New(TestTextHello, freeSerif);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)), freeSerif);

  DALI_TEST_CHECK(actor);
  END_TEST;
}

int UtcDaliTextActorNew05(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  FontParameters parameters( "FreeSerif", "Book", PointSize(8) );
  Font freeSerif = Font::New( parameters );

  TextActor actor = TextActor::New(TestTextHello, freeSerif, false);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)), freeSerif, false);

  DALI_TEST_CHECK(actor);
  END_TEST;
}

int UtcDaliTextActorNew06(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  FontParameters parameters( "FreeSerif", "Book", PointSize(8) );
  Font freeSerif = Font::New( parameters );

  TextActor actor = TextActor::New(TestTextHello, freeSerif, false, false);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)), freeSerif, false, false);

  DALI_TEST_CHECK(actor);
  END_TEST;
}

int UtcDaliTextActorNew07(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  TextStyle style;

  TextActor actor = TextActor::New(Text(TestTextHello), style, false, false);

  DALI_TEST_CHECK(actor);
  END_TEST;
}


int UtcDaliTextActorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::TextActor::DownCast()");

  TextActor actor1 = TextActor::New("Hello, World!");
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  TextActor textActor = TextActor::DownCast(child);

  DALI_TEST_CHECK(textActor);
  DALI_TEST_CHECK(!textActor.GetText().compare("Hello, World!"));
  END_TEST;
}

int UtcDaliTextActorDownCast2(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::TextActor::DownCast()");

  Actor actor1 = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  TextActor textActor = TextActor::DownCast(child);
  DALI_TEST_CHECK(!textActor);

  Actor unInitialzedActor;
  textActor = DownCast< TextActor >( unInitialzedActor );
  DALI_TEST_CHECK(!textActor);
  END_TEST;
}

int UtcDaliTextActorSetText(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetText()");

  TextActor actor01 = TextActor::New(TestTextHello);

  actor01.SetText(TestTextHelloWorld);

  std::string text = actor01.GetText();

  DALI_TEST_EQUALS(text, TestTextHelloWorld, TEST_LOCATION);

  actor01.SetText(Text(std::string(TestTextHelloWorld)));

  text = actor01.GetText();

  DALI_TEST_EQUALS(text, TestTextHelloWorld, TEST_LOCATION);

  actor01.SetText("");

  text = actor01.GetText();

  DALI_TEST_EQUALS(text, "", TEST_LOCATION);

  TextActor actor02 = TextActor::New("");

  actor02.SetText( std::string() );

  text = actor02.GetText();

  DALI_TEST_EQUALS(text, "", TEST_LOCATION);

  actor02.SetText(TestTextHelloWorld);
  actor02.SetText( std::string() );

  text = actor02.GetText();

  DALI_TEST_EQUALS(text, "", TEST_LOCATION);

  TextActor actor03 = TextActor::New("");
  const Text voidText;
  actor03.SetText(voidText);

  text = actor03.GetText();

  DALI_TEST_EQUALS(text, "", TEST_LOCATION);

  actor03.SetText(TestTextHelloWorld);
  actor03.SetText(voidText);

  text = actor03.GetText();

  DALI_TEST_EQUALS(text, "", TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextActorSetFont(void)
{
  TestApplication application;

  TextActor actor = TextActor::New(TestTextHello);

  Font defaultFont = actor.GetFont();
  DALI_TEST_EQUALS( defaultFont.GetName(), DEFAULT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultFont.GetStyle(), DEFAULT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_CHECK( defaultFont.IsDefaultSystemSize() );

  TextStyle defaultStyle = actor.GetTextStyle();
  DALI_TEST_EQUALS( defaultStyle.GetFontName(), DEFAULT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetFontStyle(), DEFAULT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );

  FontParameters params( FONT_FAMILY, FONT_STYLE, FONT_POINT_SIZE );

  Font font = Font::New( params );

  actor.SetFont( font );

  Font font2 = actor.GetFont();

  DALI_TEST_EQUALS( font2.GetName(), FONT_FAMILY, TEST_LOCATION );
  DALI_TEST_EQUALS( font2.GetStyle(), FONT_STYLE, TEST_LOCATION );
  DALI_TEST_CHECK( !font2.IsDefaultSystemSize() );
  DALI_TEST_EQUALS( PointSize( font2.GetPointSize() ), FONT_POINT_SIZE, TEST_LOCATION );

  TextStyle style = actor.GetTextStyle();
  DALI_TEST_EQUALS( style.GetFontName(), FONT_FAMILY, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetFontStyle(), FONT_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetFontPointSize(), FONT_POINT_SIZE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextActorSetFontDetection(void)
{
  TestApplication application;

  TextActor actor = TextActor::New(TestTextHello);

  actor.SetFontDetectionAutomatic( true );

  DALI_TEST_CHECK( true == actor.IsFontDetectionAutomatic() );

  END_TEST;
}

int UtcDaliTextActorSetTextIndividualStyles(void)
{
  TestApplication application;

  TextActor actor = TextActor::New(TestTextHello);
  TextStyle defaultStyle = actor.GetTextStyle();

  DALI_TEST_EQUALS( actor.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );

  DALI_TEST_EQUALS( defaultStyle.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !actor.GetItalics() );
  DALI_TEST_EQUALS( actor.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );
  DALI_TEST_CHECK( defaultStyle.IsItalicsDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsItalicsEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( !actor.GetUnderline() );
  DALI_TEST_CHECK( defaultStyle.IsUnderlineDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( defaultStyle.IsShadowDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsShadowEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( defaultStyle.IsGlowDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsGlowEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( defaultStyle.IsOutlineDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsOutlineEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( defaultStyle.IsGradientDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsGradientEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );


  actor.SetTextColor( TEXT_COLOR );

  actor.SetWeight( TEXT_WEIGHT );
  actor.SetSmoothEdge( SMOOTH_EDGE  );

  actor.SetItalics( ITALICS, ITALICS_ANGLE );
  actor.SetUnderline( UNDERLINE );

  actor.SetShadow( SHADOW, SHADOW_COLOR, SHADOW_OFFSET, SHADOW_SIZE );
  actor.SetGlow( GLOW, GLOW_COLOR, GLOW_INTENSITY );
  actor.SetOutline( OUTLINE, OUTLINE_COLOR, OUTLINE_THICKNESS );
  actor.SetGradientColor( GRADIENT_COLOR );
  actor.SetGradientStartPoint( GRADIENT_START_POINT );
  actor.SetGradientEndPoint( GRADIENT_END_POINT );


  TextStyle style = actor.GetTextStyle();

  DALI_TEST_EQUALS( actor.GetTextColor(), TEXT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetTextColor(), TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetWeight(), TEXT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetWeight(), TEXT_WEIGHT, TEST_LOCATION );

  DALI_TEST_EQUALS( style.GetSmoothEdge(), SMOOTH_EDGE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( actor.GetItalics() );
  DALI_TEST_EQUALS( actor.GetItalicsAngle(), ITALICS_ANGLE, TEST_LOCATION );
  DALI_TEST_CHECK( !style.IsItalicsDefault() );
  DALI_TEST_CHECK( style.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style.GetItalicsAngle(), ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( actor.GetUnderline() );
  DALI_TEST_CHECK( !style.IsUnderlineDefault() );
  DALI_TEST_CHECK( style.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style.IsShadowDefault() );
  DALI_TEST_CHECK( style.IsShadowEnabled() );
  DALI_TEST_EQUALS( style.GetShadowColor(), SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetShadowOffset(), SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( !style.IsGlowDefault() );
  DALI_TEST_CHECK( style.IsGlowEnabled() );
  DALI_TEST_EQUALS( style.GetGlowColor(), GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGlowIntensity(), GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( !style.IsOutlineDefault() );
  DALI_TEST_CHECK( style.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style.GetOutlineColor(), OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetOutlineThickness(), OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetGradientColor(), GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetGradientStartPoint(), GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetGradientEndPoint(), GRADIENT_END_POINT, TEST_LOCATION );
  DALI_TEST_CHECK( !style.IsGradientDefault() );
  DALI_TEST_CHECK( style.IsGradientEnabled() );
  DALI_TEST_EQUALS( style.GetGradientColor(), GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGradientStartPoint(), GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style.GetGradientEndPoint(), GRADIENT_END_POINT, TEST_LOCATION );

  // Added to increase coverage.

  // Set a different color.
  actor.SetTextColor( TEXT_COLOR );
  actor.SetTextColor( Color::GREEN );
  DALI_TEST_EQUALS( actor.GetTextColor(), Color::GREEN, TEST_LOCATION );

  // Set a different weight
  actor.SetWeight( TEXT_WEIGHT );
  actor.SetWeight( TextStyle::BOLD );
  DALI_TEST_EQUALS( actor.GetWeight(), TextStyle::BOLD, TEST_LOCATION );

  // Set a different smooth edge
  actor.SetSmoothEdge( SMOOTH_EDGE );
  actor.SetSmoothEdge( 1.f );
  DALI_TEST_EQUALS( actor.GetTextStyle().GetSmoothEdge(), 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Set different italic parameters
  actor.SetItalics( true, ITALICS_ANGLE );
  actor.SetItalics( false );
  DALI_TEST_CHECK( !actor.GetItalics() );
  actor.SetItalics( true, Degree( 15.f ) );
  DALI_TEST_EQUALS( actor.GetItalicsAngle(), Degree( 15.f ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextActorChangingText(void)
{
  TestApplication application;

  TextActor actor = TextActor::New(TestTextHello);
  actor.SetSize(Vector3(200, 20, 0.0f));
  actor.SetPosition(20.0f, 400.0f, 40.0f);
  Stage::GetCurrent().Add(actor);

  tet_infoline("Testing Dali::TextActor::SetText() & Dali::TextActor::GetText()");
  actor.SetText(LongTestText);
  std::string text = actor.GetText();
  DALI_TEST_EQUALS(text, LongTestText, TEST_LOCATION);

  // do a render
  application.SendNotification();
  application.Render();

  // check that the size did not change
  DALI_TEST_EQUALS( Vector3(200, 20, 0.0f), actor.GetCurrentSize(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextActorGetLoadingState(void)
{
  TestApplication application;

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK( ResourceLoading == actor.GetLoadingState());

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( ResourceLoadingSucceeded == actor.GetLoadingState());

  END_TEST;
}

int UtcDaliTextActorSetItalics(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor.SetItalics( true );

  DALI_TEST_CHECK( actor.GetItalics() );

  DALI_TEST_EQUALS( static_cast<float>( Degree( actor.GetItalicsAngle() ) ), static_cast<float>(TextStyle::DEFAULT_ITALICS_ANGLE), 0.0001f, TEST_LOCATION );

  actor.SetItalics( false );

  DALI_TEST_CHECK( ! actor.GetItalics() );

  // TODO: Implement a why on the glAbstraction to check if the geometry was created correctly
  END_TEST;
}

int UtcDaliTextActorSetUnderline(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetUnderline()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor.SetUnderline( true );

  DALI_TEST_CHECK( actor.GetUnderline() );

  actor.SetUnderline( false );

  DALI_TEST_CHECK( ! actor.GetUnderline() );

  // TODO: Implement a why on the glAbstraction to check if the geometry was created correctly
  END_TEST;
}

int UtcDaliTextActorSetWeight(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetWeight()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor.SetWeight( TextStyle::EXTRABOLD );

  DALI_TEST_CHECK( TextStyle::EXTRABOLD == actor.GetWeight() );

  actor.SetWeight( TextStyle::BOLD );

  DALI_TEST_CHECK( TextStyle::BOLD == actor.GetWeight() );
  END_TEST;
}

int UtcDaliTextActorSetStyle(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetTextStyle()");

  TextActor actor = TextActor::New(TestTextHello);

  const TextStyle defaultStyle = actor.GetTextStyle();

  DALI_TEST_EQUALS( defaultStyle.GetFontName(), DEFAULT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetFontStyle(), DEFAULT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( defaultStyle.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( defaultStyle.IsItalicsDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsItalicsEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( defaultStyle.IsUnderlineDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( defaultStyle.IsShadowDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsShadowEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( defaultStyle.IsGlowDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsGlowEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( defaultStyle.IsOutlineDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsOutlineEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( defaultStyle.IsGradientDefault() );
  DALI_TEST_CHECK( !defaultStyle.IsGradientEnabled() );
  DALI_TEST_EQUALS( defaultStyle.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( defaultStyle.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );


  // Set a non default style.

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

  actor.SetTextStyle( style );

  // This is necessary since SetColor (via TextStyle) is asynchronous
  application.SendNotification();
  application.Render();

  TextStyle style2 = actor.GetTextStyle();

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


  // Set a default style
  actor.SetTextStyle( defaultStyle );

  TextStyle style3 = actor.GetTextStyle();

  DALI_TEST_EQUALS( style3.GetFontName(), DEFAULT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style3.GetFontStyle(), DEFAULT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style3.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );
  DALI_TEST_EQUALS( style3.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( style3.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( style3.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style3.IsItalicsDefault() );
  DALI_TEST_CHECK( !style3.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style3.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( style3.IsUnderlineDefault() );
  DALI_TEST_CHECK( !style3.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style3.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style3.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style3.IsShadowDefault() );
  DALI_TEST_CHECK( !style3.IsShadowEnabled() );
  DALI_TEST_EQUALS( style3.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style3.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( style3.IsGlowDefault() );
  DALI_TEST_CHECK( !style3.IsGlowEnabled() );
  DALI_TEST_EQUALS( style3.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style3.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style3.IsOutlineDefault() );
  DALI_TEST_CHECK( !style3.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style3.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style3.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( style3.IsGradientDefault() );
  DALI_TEST_CHECK( !style3.IsGradientEnabled() );
  DALI_TEST_EQUALS( style3.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style3.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style3.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );

  // Added to increase coverage.
  // Reset what is already reset.

  actor.SetTextStyle( style3 );

  TextStyle style4 = actor.GetTextStyle();

  DALI_TEST_EQUALS( style4.GetFontName(), DEFAULT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style4.GetFontStyle(), DEFAULT_NAME_STYLE, TEST_LOCATION );
  DALI_TEST_EQUALS( style4.GetFontPointSize(), DEFAULT_FONT_POINT_SIZE, TEST_LOCATION );
  DALI_TEST_EQUALS( style4.GetTextColor(), TextStyle::DEFAULT_TEXT_COLOR, TEST_LOCATION );

  DALI_TEST_EQUALS( style4.GetWeight(), TextStyle::DEFAULT_FONT_WEIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( style4.GetSmoothEdge(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style4.IsItalicsDefault() );
  DALI_TEST_CHECK( !style4.IsItalicsEnabled() );
  DALI_TEST_EQUALS( style4.GetItalicsAngle(), TextStyle::DEFAULT_ITALICS_ANGLE, TEST_LOCATION );

  DALI_TEST_CHECK( style4.IsUnderlineDefault() );
  DALI_TEST_CHECK( !style4.IsUnderlineEnabled() );
  DALI_TEST_EQUALS( style4.GetUnderlinePosition(), TextStyle::DEFAULT_UNDERLINE_POSITION, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( style4.GetUnderlineThickness(), TextStyle::DEFAULT_UNDERLINE_THICKNESS, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style4.IsShadowDefault() );
  DALI_TEST_CHECK( !style4.IsShadowEnabled() );
  DALI_TEST_EQUALS( style4.GetShadowColor(), TextStyle::DEFAULT_SHADOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style4.GetShadowOffset(), TextStyle::DEFAULT_SHADOW_OFFSET, TEST_LOCATION );

  DALI_TEST_CHECK( style4.IsGlowDefault() );
  DALI_TEST_CHECK( !style4.IsGlowEnabled() );
  DALI_TEST_EQUALS( style4.GetGlowColor(), TextStyle::DEFAULT_GLOW_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style4.GetGlowIntensity(), TextStyle::DEFAULT_GLOW_INTENSITY, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( style4.IsOutlineDefault() );
  DALI_TEST_CHECK( !style4.IsOutlineEnabled() );
  DALI_TEST_EQUALS( style4.GetOutlineColor(), TextStyle::DEFAULT_OUTLINE_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style4.GetOutlineThickness(), TextStyle::DEFAULT_OUTLINE_THICKNESS, TEST_LOCATION );

  DALI_TEST_CHECK( style4.IsGradientDefault() );
  DALI_TEST_CHECK( !style4.IsGradientEnabled() );
  DALI_TEST_EQUALS( style4.GetGradientColor(), TextStyle::DEFAULT_GRADIENT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( style4.GetGradientStartPoint(), TextStyle::DEFAULT_GRADIENT_START_POINT, TEST_LOCATION );
  DALI_TEST_EQUALS( style4.GetGradientEndPoint(), TextStyle::DEFAULT_GRADIENT_END_POINT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextActorDefaultProperties(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::TextActor DefaultProperties");

  TextActor actor = TextActor::New("@");

  std::vector<Property::Index> indices ;
  indices.push_back(TextActor::TEXT                     );
  indices.push_back(TextActor::FONT                     );
  indices.push_back(TextActor::FONT_STYLE               );
  indices.push_back(TextActor::OUTLINE_ENABLE           );
  indices.push_back(TextActor::OUTLINE_COLOR            );
  indices.push_back(TextActor::OUTLINE_THICKNESS_WIDTH  );
  indices.push_back(TextActor::SMOOTH_EDGE              );
  indices.push_back(TextActor::GLOW_ENABLE              );
  indices.push_back(TextActor::GLOW_COLOR               );
  indices.push_back(TextActor::GLOW_INTENSITY           );
  indices.push_back(TextActor::SHADOW_ENABLE            );
  indices.push_back(TextActor::SHADOW_COLOR             );
  indices.push_back(TextActor::SHADOW_OFFSET            );
  indices.push_back(TextActor::ITALICS_ANGLE            );
  indices.push_back(TextActor::UNDERLINE                );
  indices.push_back(TextActor::WEIGHT                   );
  indices.push_back(TextActor::FONT_DETECTION_AUTOMATIC );
  indices.push_back(TextActor::GRADIENT_COLOR           );
  indices.push_back(TextActor::GRADIENT_START_POINT     );
  indices.push_back(TextActor::GRADIENT_END_POINT       );
  indices.push_back(TextActor::SHADOW_SIZE              );
  indices.push_back(TextActor::TEXT_COLOR               );

  DALI_TEST_CHECK(actor.GetPropertyCount() == ( Actor::New().GetPropertyCount() + indices.size() ) );

  for(std::vector<Property::Index>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
  {
    DALI_TEST_CHECK( *iter == actor.GetPropertyIndex(actor.GetPropertyName(*iter)) );
    DALI_TEST_CHECK( actor.IsPropertyWritable(*iter) );
    DALI_TEST_CHECK( !actor.IsPropertyAnimatable(*iter) );
    DALI_TEST_CHECK( actor.GetPropertyType(*iter) == actor.GetPropertyType(*iter) );  // just checking call succeeds
  }

  // set/get one of them
  actor.SetUnderline(false);
  DALI_TEST_CHECK(actor.GetUnderline() != true);

  actor.SetProperty(TextActor::UNDERLINE, Property::Value(true));
  Property::Value v = actor.GetProperty(TextActor::UNDERLINE);
  DALI_TEST_CHECK(v.GetType() == Property::BOOLEAN);

  DALI_TEST_CHECK(v.Get<bool>() == true);
  END_TEST;
}

int UtcDaliTextActorSetGradientColor(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetGradientColor()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor.SetGradientColor( Color::RED );
  DALI_TEST_EQUALS( actor.GetGradientColor(), Color::RED, TEST_LOCATION );

  actor.SetGradientColor( Color::BLUE );
  DALI_TEST_EQUALS( actor.GetGradientColor(), Color::BLUE, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextActorSetGradientStartPoint(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetGradientStartPoint()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor.SetGradientStartPoint( Vector2(0.5f, 0.5f) );
  DALI_TEST_EQUALS( actor.GetGradientStartPoint(), Vector2(0.5f, 0.5f), TEST_LOCATION );

  actor.SetGradientStartPoint( Vector2(1.0f, 0.0f) );
  DALI_TEST_EQUALS( actor.GetGradientStartPoint(), Vector2(1.0f, 0.0f), TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextActorSetGradientEndPoint(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetGradientEndPoint()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor.SetGradientEndPoint( Vector2(0.25f, 0.25f) );
  DALI_TEST_EQUALS( actor.GetGradientEndPoint(), Vector2(0.25f, 0.25f), TEST_LOCATION );

  actor.SetGradientEndPoint( Vector2(0.0f, 1.0f) );
  DALI_TEST_EQUALS( actor.GetGradientEndPoint(), Vector2(0.0f, 1.0f), TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextActorSynchronousGlyphLoading(void)
{
  TestApplication application;

  tet_infoline( "Testing synchronous loading of glyphs");

  // All numerals 0 through 9 are 'fake' cached in the test abstraction glyphcache

  // create text actor containg "Hello"
  TextActor actor = TextActor::New(TestTextHello);

  // no glyphs will be cached

  // so..GetGlyphData should have been called to gather metrics
  DALI_TEST_CHECK( application.GetPlatform().GetTrace().FindMethodAndParams( "GetGlyphData", "getBitmap:false" ) );
  // ..but not to load glyph bitmap data
  DALI_TEST_CHECK( ! application.GetPlatform().GetTrace().FindMethodAndParams( "GetGlyphData", "getBitmap:true" ) );
  // ..also, cached high quality glyphs will not have been requested yet
  DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::GetCachedGlyphDataFunc) );

  // reset PlatformAbstraction function call traces
  application.GetPlatform().ResetTrace();

  // Invoke Core::ProcessEvent and tick the update/render threads
  application.SendNotification();
  application.Render();

  // An attempt to load high quality glyphs will have been requested and loaded nothing
  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::GetCachedGlyphDataFunc) );
  // low quality glyphs bitmap data will have now been generated
  DALI_TEST_CHECK( application.GetPlatform().GetTrace().FindMethodAndParams( "GetGlyphData", "getBitmap:true" ) );

  // request numerals
  actor.SetText( "0123456789" );

  // reset PlatformAbstraction function call traces
  application.GetPlatform().ResetTrace();

  application.SendNotification();
  application.Render();

  // An attempt to load high quality glyphs will have been requested and loaded all the numerals
  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::GetCachedGlyphDataFunc) );
  // ..therefore no low quality glyphs bitmap data will have been requested
  DALI_TEST_CHECK( !application.GetPlatform().GetTrace().FindMethodAndParams( "GetGlyphData", "getBitmap:true" ) );
  END_TEST;
}

int UtcDaliTextActorAutomaticSizeSet(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor getting size based on text automatically");

  // create empty text actor
  TextActor actor = TextActor::New();
  Stage::GetCurrent().Add(actor);

  // initial size is zero
  DALI_TEST_EQUALS( Vector3::ZERO, actor.GetCurrentSize(), TEST_LOCATION );

  // set some text
  actor.SetText( "a" );
  // render a frame
  application.SendNotification();
  application.Render();

  // dont care about the actual size as that is too hard to figure out, just want to validate that the size was changed to bigger
  Vector3 currentSize = actor.GetCurrentSize();
  DALI_TEST_GREATER( currentSize.width, 0.0f, TEST_LOCATION );
  DALI_TEST_GREATER( currentSize.height, 0.0f, TEST_LOCATION );

  // set some more text
  actor.SetText( "abba" );
  // render a frame
  application.SendNotification();
  application.Render();

  Vector3 biggerSize = actor.GetCurrentSize();
  DALI_TEST_GREATER( biggerSize.width, currentSize.width, TEST_LOCATION );

  // set some shorter text
  actor.SetText( "i" );
  // render a frame
  application.SendNotification();
  application.Render();

  // actor has shrunk
  DALI_TEST_GREATER( biggerSize.width, actor.GetCurrentSize().width, TEST_LOCATION );

  // set a size from application side, from this point onwards text actor no longer uses the "natural" size of the text
  actor.SetSize( Vector2( 10.0f, 11.0f ) );
  // render a frame
  application.SendNotification();
  application.Render();
  // actor has the user set size
  DALI_TEST_EQUALS( Vector2( 10.0f, 11.0f ), actor.GetCurrentSize().GetVectorXY(), TEST_LOCATION );

  // set some different text
  std::string longText( "jabba dabba duu" );
  actor.SetText( longText );
  // render a frame
  application.SendNotification();
  application.Render();
  // actor still has the user set size
  DALI_TEST_EQUALS( Vector2( 10.0f, 11.0f ), actor.GetCurrentSize().GetVectorXY(), TEST_LOCATION );

  // set text to its natural size
  actor.SetToNaturalSize();
  // render a frame
  application.SendNotification();
  application.Render();
  // actor has the natural size
  Font defaultFont = Font::New();
  Vector3 naturalSize = defaultFont.MeasureText( longText );
  DALI_TEST_EQUALS( naturalSize.GetVectorXY(), actor.GetCurrentSize().GetVectorXY(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextActorAutomaticSizeSetAnimation(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor getting size based on text automatically with animation");

  // create empty text actor
  TextActor actor = TextActor::New();
  Stage::GetCurrent().Add(actor);

  // initial size is zero
  DALI_TEST_EQUALS( Vector3::ZERO, actor.GetCurrentSize(), TEST_LOCATION );

  // set some text
  actor.SetText( "a" );
  // render a frame
  application.SendNotification();
  application.Render();

  // dont care about the actual size as that is too hard to figure out, just want to validate that the size was changed to bigger
  Vector3 currentSize = actor.GetCurrentSize();
  DALI_TEST_GREATER( currentSize.width, 0.0f, TEST_LOCATION );
  DALI_TEST_GREATER( currentSize.height, 0.0f, TEST_LOCATION );

  // animate size, from this point onwards text actor no longer uses the "natural" size of the text
  Animation sizeAnim = Animation::New( 0.1f ); // 0.1 seconds
  Vector3 animationTargetSize( 20.0f, 30.0f, 0.0f );
  sizeAnim.AnimateTo( Property( actor, Actor::SIZE ), animationTargetSize );
  sizeAnim.Play();

  // set some more text
  actor.SetText( "abba" );
  // render a frame
  application.SendNotification();
  application.Render( 1000 ); // 1 second to complete the animation

  DALI_TEST_EQUALS( animationTargetSize, actor.GetCurrentSize(), TEST_LOCATION );

  // set some more text
  std::string moreText( "something else" );
  actor.SetText( moreText );
  // render a frame
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( animationTargetSize, actor.GetCurrentSize(), TEST_LOCATION );

  // set text to its natural size
  actor.SetToNaturalSize();
  // render a frame
  application.SendNotification();
  application.Render();
  // actor has the natural size
  Font defaultFont = Font::New();
  Vector3 naturalSize = defaultFont.MeasureText( moreText );
  DALI_TEST_EQUALS( naturalSize.GetVectorXY(), actor.GetCurrentSize().GetVectorXY(), TEST_LOCATION );
  END_TEST;
}


int UtcDaliTextActorPropertyIndices(void)
{
  TestApplication application;
  Actor basicActor = Actor::New();
  TextActor textActor = TextActor::New("Text");

  Property::IndexContainer indices;
  textActor.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.size() > basicActor.GetPropertyCount() );
  DALI_TEST_EQUALS( indices.size(), textActor.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}
