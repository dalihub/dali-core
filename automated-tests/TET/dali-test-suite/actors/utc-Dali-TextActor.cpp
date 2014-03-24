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

static const char* TestTextHello = "Hello";
static const char* TestTextHelloWorld = "Hello World";
static const char* LongTestText = "This is a very long piece of text, and is sure not to fit into any box presented to it";

const std::string FAMILY_NAME = "Arial";
const std::string STYLE = "Bold";
const unsigned int POINT_SIZE = 11.f;
static const Vector4 FONT_TEXT_COLOR = Color::RED;
static const Degree FONT_ITALICS_ANGLE(10.f);
static const Radian FONT_ITALICS_RADIAN_ANGLE(0.4f);
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

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliTextActorConstructorVoid,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorConstructorRefObject,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorNew01,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorNew02,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorNew03,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorNew04,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorNew05,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorNew06,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorNew07,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorDownCast,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorDownCast2,                 NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorSetText,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorSetTextIndividualStyles,   POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliTextActorSetFont,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorSetFontDetection,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorChangingText,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorGetLoadingState,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorSetItalics,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorSetUnderline,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorSetWeight,                 POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorSetStyle,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorDefaultProperties,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorSetGradientColor,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorSetGradientStartPoint,     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorSetGradientEndPoint,       POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliTextActorSynchronousGlyphLoading,   POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliTextActorAutomaticSizeSet,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextActorAutomaticSizeSetAnimation, POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliTextActorPropertyIndices,           POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliTextActorConstructorVoid()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::TextActor()");

  TextActor actor;

  DALI_TEST_CHECK(!actor);
}

static void UtcDaliTextActorConstructorRefObject()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::TextActor(Internal::TextActor*)");

  TextActor actor(NULL);

  DALI_TEST_CHECK(!actor);
}

static void UtcDaliTextActorNew01()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)));

  DALI_TEST_CHECK(actor);
}


static void UtcDaliTextActorNew02()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  TextActor actor = TextActor::New(TestTextHello, false);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)), false);

  DALI_TEST_CHECK(actor);
}

static void UtcDaliTextActorNew03()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  TextActor actor = TextActor::New(TestTextHello, false, false);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)), false, false);

  DALI_TEST_CHECK(actor);
}


static void UtcDaliTextActorNew04()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  FontParameters parameters( "FreeSerif", "Book", PointSize(8) );
  Font freeSerif = Font::New( parameters );

  TextActor actor = TextActor::New(TestTextHello, freeSerif);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)), freeSerif);

  DALI_TEST_CHECK(actor);
}

static void UtcDaliTextActorNew05()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  FontParameters parameters( "FreeSerif", "Book", PointSize(8) );
  Font freeSerif = Font::New( parameters );

  TextActor actor = TextActor::New(TestTextHello, freeSerif, false);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)), freeSerif, false);

  DALI_TEST_CHECK(actor);
}

static void UtcDaliTextActorNew06()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  FontParameters parameters( "FreeSerif", "Book", PointSize(8) );
  Font freeSerif = Font::New( parameters );

  TextActor actor = TextActor::New(TestTextHello, freeSerif, false, false);

  DALI_TEST_CHECK(actor);

  actor = TextActor::New(Text(std::string(TestTextHello)), freeSerif, false, false);

  DALI_TEST_CHECK(actor);
}

static void UtcDaliTextActorNew07()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::New()");

  TextStyle style;

  TextActor actor = TextActor::New(Text(TestTextHello), style, false, false);

  DALI_TEST_CHECK(actor);
}


static void UtcDaliTextActorDownCast()
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
}

static void UtcDaliTextActorDownCast2()
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
}

static void UtcDaliTextActorSetText()
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
}

static void UtcDaliTextActorSetFont()
{
  TestApplication application;

  TextActor actor = TextActor::New(TestTextHello);

  PointSize pointSize( POINT_SIZE );
  FontParameters params( FAMILY_NAME, STYLE, pointSize);

  Font font = Font::New( params );

  actor.SetFont( font );

  DALI_TEST_CHECK( actor.GetFont().GetName() == FAMILY_NAME );
}

static void UtcDaliTextActorSetFontDetection()
{
  TestApplication application;

  TextActor actor = TextActor::New(TestTextHello);

  actor.SetFontDetectionAutomatic( true );

  DALI_TEST_CHECK( true == actor.IsFontDetectionAutomatic() );

}

static void UtcDaliTextActorSetTextIndividualStyles()
{
  TestApplication application;

  TextActor actor = TextActor::New(TestTextHello);

  actor.SetTextColor( FONT_TEXT_COLOR);

  DALI_TEST_CHECK( actor.GetTextColor() == FONT_TEXT_COLOR );

  actor.SetSmoothEdge( FONT_SMOOTH_EDGE  );

  actor.SetOutline( FONT_OUTLINE, FONT_OUTLINE_COLOR, FONT_OUTLINE_THICKNESS );

  actor.SetShadow( FONT_SHADOW, FONT_SHADOW_COLOR, FONT_SHADOW_OFFSET, FONT_SHADOW_SIZE );

  actor.SetItalics( FONT_ITALICS, FONT_ITALICS_ANGLE );

  actor.SetGlow( FONT_GLOW, FONT_TEXT_GLOW_COLOR, FONT_GLOW_INTENSITY );

  TextStyle style = actor.GetTextStyle();

  DALI_TEST_CHECK( FONT_TEXT_COLOR == style.GetTextColor() );

  DALI_TEST_CHECK( FONT_SMOOTH_EDGE == style.GetSmoothEdge() );

  DALI_TEST_CHECK( FONT_OUTLINE == style.GetOutline() );
  DALI_TEST_CHECK( FONT_OUTLINE_COLOR == style.GetOutlineColor() );
  DALI_TEST_CHECK( FONT_OUTLINE_THICKNESS == style.GetOutlineThickness() );


  DALI_TEST_CHECK( FONT_SHADOW == style.GetShadow() );
  DALI_TEST_CHECK( FONT_SHADOW_COLOR == style.GetShadowColor() );
  DALI_TEST_CHECK( FONT_SHADOW_OFFSET == style.GetShadowOffset() );
  DALI_TEST_CHECK( FONT_SHADOW_SIZE == style.GetShadowSize() );

  DALI_TEST_CHECK( FONT_ITALICS == style.GetItalics() );
  DALI_TEST_CHECK( FONT_ITALICS_ANGLE == style.GetItalicsAngle() );

  DALI_TEST_CHECK( FONT_GLOW == style.GetGlow() );
  DALI_TEST_CHECK( FONT_OUTLINE == style.GetOutline() );
  DALI_TEST_CHECK( FONT_TEXT_GLOW_COLOR == style.GetGlowColor() );
  DALI_TEST_CHECK( FONT_GLOW_INTENSITY == style.GetGlowIntensity() );


  actor.SetItalics( FONT_ITALICS, FONT_ITALICS_RADIAN_ANGLE );
  style = actor.GetTextStyle();
  DALI_TEST_CHECK( FONT_ITALICS_RADIAN_ANGLE == style.GetItalicsAngle() );


}

static void UtcDaliTextActorChangingText()
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
}

static void UtcDaliTextActorGetLoadingState()
{
  TestApplication application;

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK( ResourceLoading == actor.GetLoadingState());

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( ResourceLoadingSucceeded == actor.GetLoadingState());

}

static void UtcDaliTextActorSetItalics()
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
}

static void UtcDaliTextActorSetUnderline()
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
}

static void UtcDaliTextActorSetWeight()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetWeight()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor.SetWeight( TextStyle::EXTRABOLD );

  DALI_TEST_CHECK( TextStyle::EXTRABOLD == actor.GetWeight() );

  actor.SetWeight( TextStyle::BOLD );

  DALI_TEST_CHECK( TextStyle::BOLD == actor.GetWeight() );
}

static void UtcDaliTextActorSetStyle()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetTextStyle()");

  TextActor actor = TextActor::New(TestTextHello);

  TextStyle defaultStyle = actor.GetTextStyle();
  DALI_TEST_CHECK( defaultStyle.GetFontName().empty() );
  DALI_TEST_CHECK( TextStyle::REGULAR == defaultStyle.GetWeight() );
  DALI_TEST_CHECK( Color::WHITE == defaultStyle.GetTextColor() );
  DALI_TEST_CHECK( !defaultStyle.GetItalics() );
  DALI_TEST_CHECK( !defaultStyle.GetUnderline() );

  TextStyle style;
  style.SetFontPointSize(PointSize( 16.f ));
  style.SetWeight(TextStyle::EXTRABLACK);
  style.SetTextColor(Color::BLUE);
  style.SetItalics(true);
  style.SetUnderline(true);
  style.SetShadow(false);
  style.SetGlow(false);
  style.SetOutline(false);

  actor.SetTextStyle( style );
  // This is necessary since SetColor (via TextStyle) is asynchronous
  application.SendNotification();
  application.Render();
  style = actor.GetTextStyle();

  DALI_TEST_CHECK( style.GetFontName().empty() );
  DALI_TEST_CHECK( style.GetFontStyle().empty() );
  DALI_TEST_EQUALS( static_cast<float>( PointSize( 16.f ) ),
                    static_cast<float>( style.GetFontPointSize() ),
                    GetRangedEpsilon( PointSize( 16.f ), style.GetFontPointSize() ),
                    TEST_LOCATION );
  DALI_TEST_CHECK( TextStyle::EXTRABLACK == style.GetWeight() );
  DALI_TEST_EQUALS( Vector4( 0.f, 0.f, 1.f, 1.f ), style.GetTextColor(), TEST_LOCATION );
  DALI_TEST_CHECK( style.GetItalics() );
  DALI_TEST_CHECK( style.GetUnderline() );
}

static void UtcDaliTextActorDefaultProperties()
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
}

static void UtcDaliTextActorSetGradientColor()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetGradientColor()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor.SetGradientColor( Color::RED );
  DALI_TEST_EQUALS( actor.GetGradientColor(), Color::RED, TEST_LOCATION );

  actor.SetGradientColor( Color::BLUE );
  DALI_TEST_EQUALS( actor.GetGradientColor(), Color::BLUE, TEST_LOCATION );
}

static void UtcDaliTextActorSetGradientStartPoint()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetGradientStartPoint()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor.SetGradientStartPoint( Vector2(0.5f, 0.5f) );
  DALI_TEST_EQUALS( actor.GetGradientStartPoint(), Vector2(0.5f, 0.5f), TEST_LOCATION );

  actor.SetGradientStartPoint( Vector2(1.0f, 0.0f) );
  DALI_TEST_EQUALS( actor.GetGradientStartPoint(), Vector2(1.0f, 0.0f), TEST_LOCATION );
}

static void UtcDaliTextActorSetGradientEndPoint()
{
  TestApplication application;

  tet_infoline("Testing Dali::TextActor::SetGradientEndPoint()");

  TextActor actor = TextActor::New(TestTextHello);

  DALI_TEST_CHECK(actor);

  actor.SetGradientEndPoint( Vector2(0.25f, 0.25f) );
  DALI_TEST_EQUALS( actor.GetGradientEndPoint(), Vector2(0.25f, 0.25f), TEST_LOCATION );

  actor.SetGradientEndPoint( Vector2(0.0f, 1.0f) );
  DALI_TEST_EQUALS( actor.GetGradientEndPoint(), Vector2(0.0f, 1.0f), TEST_LOCATION );
}

static void UtcDaliTextActorSynchronousGlyphLoading()
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
}

static void UtcDaliTextActorAutomaticSizeSet()
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
}

static void UtcDaliTextActorAutomaticSizeSetAnimation()
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
}

void UtcDaliTextActorPropertyIndices()
{
  TestApplication application;
  Actor basicActor = Actor::New();
  TextActor textActor = TextActor::New("Text");

  Property::IndexContainer indices;
  textActor.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.size() > basicActor.GetPropertyCount() );
  DALI_TEST_EQUALS( indices.size(), textActor.GetPropertyCount(), TEST_LOCATION );
}
