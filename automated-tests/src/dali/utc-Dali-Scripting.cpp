/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/scripting/scripting.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Scripting;

namespace
{

const StringEnum COLOR_MODE_VALUES[] =
{
    { "USE_OWN_COLOR", USE_OWN_COLOR },
    { "USE_PARENT_COLOR", USE_PARENT_COLOR },
    { "USE_OWN_MULTIPLY_PARENT_COLOR", USE_OWN_MULTIPLY_PARENT_COLOR },
    { "USE_OWN_MULTIPLY_PARENT_ALPHA", USE_OWN_MULTIPLY_PARENT_ALPHA },
};
const unsigned int COLOR_MODE_VALUES_COUNT = sizeof( COLOR_MODE_VALUES ) / sizeof( COLOR_MODE_VALUES[0] );

const StringEnum POSITION_INHERITANCE_MODE_VALUES[] =
{
    { "INHERIT_PARENT_POSITION", INHERIT_PARENT_POSITION },
    { "USE_PARENT_POSITION", USE_PARENT_POSITION },
    { "USE_PARENT_POSITION_PLUS_LOCAL_POSITION", USE_PARENT_POSITION_PLUS_LOCAL_POSITION },
    { "DONT_INHERIT_POSITION", DONT_INHERIT_POSITION },
};
const unsigned int POSITION_INHERITANCE_MODE_VALUES_COUNT = sizeof( POSITION_INHERITANCE_MODE_VALUES ) / sizeof( POSITION_INHERITANCE_MODE_VALUES[0] );

const StringEnum DRAW_MODE_VALUES[] =
{
    { "NORMAL", DrawMode::NORMAL },
    { "OVERLAY_2D", DrawMode::OVERLAY_2D }
};
const unsigned int DRAW_MODE_VALUES_COUNT = sizeof( DRAW_MODE_VALUES ) / sizeof( DRAW_MODE_VALUES[0] );


////////////////////////////////////////////////////////////////////////////////
// Helpers for string to enum comparisons for Image and Image loading parameters
////////////////////////////////////////////////////////////////////////////////

/**
 * Template to check enumerations of type T, with a class of type X
 */
template< typename T, typename X >
void TestEnumStrings(
  Property::Map& map,                       // The map used to create instance of type X
  const char * const keyName,               // the name of the key to iterate through
  const StringEnum* values,                 // An array of string values
  unsigned int num,                         // Number of items in the array
  T ( X::*method )() const,                 // The member method of X to call to get the enum
  X ( *creator ) ( const Property::Value& ) // The method which creates an instance of type X
)
{
  // get the key reference so we can change its value
  Property::Value* value = map.Find( keyName );
  for ( unsigned int i = 0; i < num; ++i )
  {
    *value = values[i].string;
    tet_printf("Checking: %s: %s\n", keyName, values[i].string );
    X instance = creator( map );
    DALI_TEST_EQUALS( values[i].value, (int)( instance.*method )(), TEST_LOCATION );
  }
}

/// Helper method to create BufferImage using property
BufferImage NewBufferImage( const Property::Value& map )
{
  BufferImage image = BufferImage::DownCast( NewImage( map ) );
  return image;
}

//////////////////////////////////////////////////////////////////////////////
// Helpers for string to enum comparisons for Actor to Property::Map
//////////////////////////////////////////////////////////////////////////////

/**
 * Template to check enumerations of type T
 */
template< typename T >
void TestEnumStrings(
  const char * const keyName,               // The name of the key to check
  TestApplication& application,             // Reference to the application class
  const StringEnum* values,                 // An array of string values
  unsigned int num,                         // Number of items in the array
  void ( Actor::*method )( T )              // The Actor member method to set the enumeration
)
{
  for ( unsigned int i = 0; i < num; ++i )
  {
    tet_printf("Checking: %s: %s\n", keyName, values[i].string );

    Actor actor = Actor::New();
    (actor.*method)( ( T ) values[i].value );

    Stage::GetCurrent().Add( actor );
    application.SendNotification();
    application.Render();

    Property::Map map;
    CreatePropertyMap( actor, map );

    DALI_TEST_CHECK( 0 < map.Count() );
    DALI_TEST_CHECK( NULL != map.Find( keyName ) );
    DALI_TEST_EQUALS( map.Find( keyName )->Get< std::string >(), values[i].string, TEST_LOCATION );

    Stage::GetCurrent().Remove( actor );
  }
}

//////////////////////////////////////////////////////////////////////////////


} // anon namespace

int UtcDaliScriptingNewImageNegative01(void)
{
  // Invalid filename
  Property::Map map;
  map[ "filename" ] = Vector3::ZERO;
  // will give us an empty image handle
  Image image = NewImage( map );
  DALI_TEST_CHECK( !image );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative06(void)
{
  TestApplication application; // Image needs application
  // Invalid width and height
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "width" ] = "Invalid";
  map[ "height" ] = 100;
  // will give us a valid image
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  DALI_TEST_EQUALS( resImage.GetWidth(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( resImage.GetHeight(), 100u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative07(void)
{
  TestApplication application; // Image needs application
  // Invalid height
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "width" ] = 10;
  map[ "height" ] = "Invalid";
  // will give us a valid image
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  DALI_TEST_EQUALS( resImage.GetWidth(), 10u, TEST_LOCATION );
  DALI_TEST_EQUALS( resImage.GetHeight(), 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative08(void)
{
  TestApplication application; // Image needs application
  // Invalid fitting-mode
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "fittingMode" ] = Vector3::ZERO;
  // will give us a valid image
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative09(void)
{
  TestApplication application; // Image needs application
  // Invalid value
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "fittingMode" ] = "INVALID";
  // will give us a valid image
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative10(void)
{
  TestApplication application; // Image needs application
  // Invalid scaling-mode
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "samplingMode" ] = Vector3::ZERO;
  // will give us a valid image
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative12(void)
{
  TestApplication application; // Image needs application
  // Invalid orientation-correction
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "orientation" ] = Vector3::ZERO;
  // will give us a valid image
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative13(void)
{
  TestApplication application; // Image needs application
  // Invalid type
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "type" ] = Vector3::ZERO;
  // will give us a valid image
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative14(void)
{
  // Invalid value
  Property::Map map;
  map[ "type" ] = "INVALID";
  Image image = NewImage( map );
  DALI_TEST_CHECK( !image );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative15(void)
{
  // Invalid pixel-format
  Property::Map map;
  map[ "pixelFormat" ] = Vector3::ZERO;
  Image image = NewImage( map );
  DALI_TEST_CHECK( !image );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative16(void)
{
  // Invalid value
  Property::Map map;
  map[ "pixelFormat" ] = "INVALID";
  Image image = NewImage( map );
  DALI_TEST_CHECK( !image );
  END_TEST;
}

int UtcDaliScriptingNewImage01P(void)
{
  TestApplication application; // Image needs application

  Property::Map map;
  map[ "filename" ] = "TEST_FILE";

  // Filename only
  ResourceImage image = ResourceImage::DownCast( NewImage( map ) );
  DALI_TEST_EQUALS( "TEST_FILE", image.GetUrl(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliScriptingNewImage04P(void)
{
  TestApplication application;

  Property::Map map;
  map[ "filename" ] = "TEST_FILE";

  // float width and height
  map[ "width" ] = (float) 10.0f;
  map[ "height" ] = (float) 20.0f;
  Image image = NewImage( map );
  DALI_TEST_EQUALS( image.GetWidth(), 10u, TEST_LOCATION );
  DALI_TEST_EQUALS( image.GetHeight(), 20u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliScriptingNewImage05P(void)
{
  TestApplication application;

  Property::Map map;
  map[ "filename" ] = "TEST_FILE";

  // width and height
  map[ "width"] = 50;
  map[ "height" ] = 70;
  Image image = NewImage( map );
  DALI_TEST_EQUALS( image.GetWidth(), 50u, TEST_LOCATION );
  DALI_TEST_EQUALS( image.GetHeight(), 70u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliScriptingNewImage06P(void)
{
  TestApplication application;

  Property::Map map;
  // type FrameBufferImage
  map[ "type" ] = "FrameBufferImage";
  // width and height
  map[ "width"] = 50;
  map[ "height" ] = 70;
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  DALI_TEST_CHECK( FrameBufferImage::DownCast( image ) );
  END_TEST;
}

int UtcDaliScriptingNewImage07P(void)
{
  TestApplication application;

  Property::Map map;
  // type BufferImage
  map[ "type" ] = "BufferImage";
  // width and height
  map[ "width"] = 50;
  map[ "height" ] = 70;
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  DALI_TEST_CHECK( BufferImage::DownCast( image ) );
  DALI_TEST_EQUALS( (BufferImage::DownCast( image )).GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );
  END_TEST;
}

int UtcDaliScriptingNewImage08P(void)
{
  TestApplication application;

  Property::Map map;
  map[ "type" ] = "BufferImage";
  // width and height
  map[ "width"] = 66;
  map[ "height" ] = 99;
  // pixel-format
  map[ "pixelFormat" ] = "";
  const StringEnum values[] =
  {
    { "A8", Pixel::A8 },
    { "L8", Pixel::L8 },
    { "LA88", Pixel::LA88 },
    { "RGB565", Pixel::RGB565 },
    { "BGR565", Pixel::BGR565 },
    { "RGBA4444", Pixel::RGBA4444 },
    { "BGRA4444", Pixel::BGRA4444 },
    { "RGBA5551", Pixel::RGBA5551 },
    { "BGRA5551", Pixel::BGRA5551 },
    { "RGB888", Pixel::RGB888 },
    { "RGB8888", Pixel::RGB8888 },
    { "BGR8888", Pixel::BGR8888 },
    { "RGBA8888", Pixel::RGBA8888 },
    { "BGRA8888", Pixel::BGRA8888 },
    // BufferImage does not support compressed formats
  };
  TestEnumStrings< Pixel::Format, BufferImage >( map, "pixelFormat",  values, ( sizeof( values ) / sizeof ( values[0] ) ), &BufferImage::GetPixelFormat, &NewBufferImage );

  END_TEST;
}

int UtcDaliScriptingNewImage09P(void)
{
  TestApplication application;

  Property::Map map;
  // type Image
  map[ "type" ] = "ResourceImage";
  map[ "filename" ] = "TEST_FILE";

  {
    Image image = NewImage( map );
    DALI_TEST_CHECK( ResourceImage::DownCast( image ) );
    DALI_TEST_CHECK( !FrameBufferImage::DownCast( image ) );
    DALI_TEST_CHECK( !BufferImage::DownCast( image ) );
  }
  END_TEST;
}

int UtcDaliScriptingNewImage10P(void)
{
  TestApplication application;

  Property::Map map;
  // type FrameBufferImage, empty size gives us stage size
  map[ "type" ] = "FrameBufferImage";
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  END_TEST;
}

int UtcDaliScriptingNewActorNegative(void)
{
  TestApplication application;

  // Empty map
  {
    Actor handle = NewActor( Property::Map() );
    DALI_TEST_CHECK( !handle );
  }

  // Map with only properties
  {
    Property::Map map;
    map[ "parentOrigin" ] = ParentOrigin::TOP_CENTER;
    map[ "anchorPoint" ] = AnchorPoint::TOP_CENTER;
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( !handle );
  }

  // Add some signals to the map, we should have no signal connections as its not yet supported
  {
    Property::Map map;
    map[ "type" ] = "Actor";
    map[ "signals" ] = Property::MAP;
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( handle );
    DALI_TEST_CHECK( !handle.WheelEventSignal().GetConnectionCount() );
    DALI_TEST_CHECK( !handle.OffStageSignal().GetConnectionCount() );
    DALI_TEST_CHECK( !handle.OnStageSignal().GetConnectionCount() );
    DALI_TEST_CHECK( !handle.TouchedSignal().GetConnectionCount() );
  }
  END_TEST;
}

int UtcDaliScriptingNewActorProperties(void)
{
  TestApplication application;

  Property::Map map;
  map[ "type" ] = "Actor";
  map[ "size" ] = Vector3::ONE;
  map[ "position" ] = Vector3::XAXIS;
  map[ "scale" ] = Vector3::ONE;
  map[ "visible" ] = false;
  map[ "color" ] = Color::MAGENTA;
  map[ "name" ] = "MyActor";
  map[ "colorMode" ] = "USE_PARENT_COLOR";
  map[ "sensitive" ] = false;
  map[ "leaveRequired" ] = true;
  map[ "positionInheritance" ] = "DONT_INHERIT_POSITION";
  map[ "drawMode" ] = "OVERLAY_2D";
  map[ "inheritOrientation" ] = false;
  map[ "inheritScale" ] = false;

  // Default properties
  {
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( handle );

    Stage::GetCurrent().Add( handle );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( handle.GetCurrentSize(), Vector3::ONE, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetCurrentPosition(), Vector3::XAXIS, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.IsVisible(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetCurrentColor(), Color::MAGENTA, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetName(), "MyActor", TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetColorMode(), USE_PARENT_COLOR, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.IsSensitive(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetLeaveRequired(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetPositionInheritanceMode(), DONT_INHERIT_POSITION, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetDrawMode(), DrawMode::OVERLAY_2D, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.IsOrientationInherited(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.IsScaleInherited(), false, TEST_LOCATION );

    Stage::GetCurrent().Remove( handle );
  }

  // Check Anchor point and parent origin vector3s
  map[ "parentOrigin" ] = ParentOrigin::TOP_CENTER;
  map[ "anchorPoint" ] = AnchorPoint::TOP_LEFT;
  {
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( handle );

    Stage::GetCurrent().Add( handle );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( handle.GetCurrentParentOrigin(), ParentOrigin::TOP_CENTER, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetCurrentAnchorPoint(), AnchorPoint::TOP_LEFT, TEST_LOCATION );

    Stage::GetCurrent().Remove( handle );
  }

  // Check Anchor point and parent origin STRINGS
  map[ "parentOrigin" ] = "TOP_LEFT";
  map[ "anchorPoint" ] = "CENTER_LEFT";
  {
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( handle );

    Stage::GetCurrent().Add( handle );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( handle.GetCurrentParentOrigin(), ParentOrigin::TOP_LEFT, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetCurrentAnchorPoint(), AnchorPoint::CENTER_LEFT, TEST_LOCATION );

    Stage::GetCurrent().Remove( handle );
  }
  END_TEST;
}

int UtcDaliScriptingNewAnimation(void)
{
  TestApplication application;

  Property::Map map;
  map["actor"] = "Actor1";
  map["property"] = "color";
  map["value"] = Color::MAGENTA;
  map["alphaFunction"] = "EASE_IN_OUT";

  Property::Map timePeriod;
  timePeriod["delay"] = 0.5f;
  timePeriod["duration"] = 1.0f;
  map["timePeriod"] = timePeriod;

  Dali::AnimationData data;
  Scripting::NewAnimation( map, data );

  Actor actor = Actor::New();
  actor.SetName("Actor1");
  actor.SetColor(Color::CYAN);
  Stage::GetCurrent().Add(actor);

  Animation anim = data.CreateAnimation( actor, 0.5f );
  anim.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(500); // Start animation
  application.Render(500); // Halfway thru anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), (Color::MAGENTA+Color::CYAN)*0.5f, TEST_LOCATION);

  application.Render(500); // Halfway thru anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::MAGENTA, TEST_LOCATION );

  END_TEST;
}

int UtcDaliScriptingNewActorChildren(void)
{
  TestApplication application;

  Property::Map map;
  map[ "type" ] = "Actor";
  map[ "position" ] = Vector3::XAXIS;

  Property::Map child1Map;
  child1Map[ "type" ] = "CameraActor";
  child1Map[ "position" ] = Vector3::YAXIS;

  Property::Array childArray;
  childArray.PushBack( child1Map );
  map[ "actors" ] = childArray;

  // Create
  Actor handle = NewActor( map );
  DALI_TEST_CHECK( handle );

  Stage::GetCurrent().Add( handle );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( handle.GetCurrentPosition(), Vector3::XAXIS, TEST_LOCATION );
  DALI_TEST_EQUALS( handle.GetChildCount(), 1u, TEST_LOCATION );

  Actor child1 = handle.GetChildAt(0);
  DALI_TEST_CHECK( child1 );
  DALI_TEST_CHECK( CameraActor::DownCast( child1 ) );
  DALI_TEST_EQUALS( child1.GetCurrentPosition(), Vector3::YAXIS, TEST_LOCATION );
  DALI_TEST_EQUALS( child1.GetChildCount(), 0u, TEST_LOCATION );

  Stage::GetCurrent().Remove( handle );
  END_TEST;
}


int UtcDaliScriptingCreatePropertyMapActor(void)
{
  TestApplication application;

  // Actor Type
  {
    Actor actor = Actor::New();

    Property::Map map;
    CreatePropertyMap( actor, map );
    DALI_TEST_CHECK( !map.Empty() );
    DALI_TEST_CHECK( NULL != map.Find( "type" ) );
    DALI_TEST_EQUALS( map.Find( "type")->Get< std::string >(), "Actor", TEST_LOCATION );

    Stage::GetCurrent().Remove( actor );
  }

  // Layer Type
  {
    Actor actor = Layer::New();

    Property::Map map;
    CreatePropertyMap( actor, map );
    DALI_TEST_CHECK( !map.Empty() );
    DALI_TEST_CHECK( NULL != map.Find( "type" ) );
    DALI_TEST_EQUALS( map.Find( "type" )->Get< std::string >(), "Layer", TEST_LOCATION );

    Stage::GetCurrent().Remove( actor );
  }

  // Default properties
  {
    Actor actor = Actor::New();
    actor.SetSize( Vector3::ONE );
    actor.SetPosition( Vector3::XAXIS );
    actor.SetScale( Vector3::ZAXIS );
    actor.SetVisible( false );
    actor.SetColor( Color::MAGENTA );
    actor.SetName( "MyActor" );
    actor.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    actor.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    actor.SetSensitive( false );
    actor.SetLeaveRequired( true );
    actor.SetInheritOrientation( false );
    actor.SetInheritScale( false );
    actor.SetSizeModeFactor( Vector3::ONE );

    Stage::GetCurrent().Add( actor );
    application.SendNotification();
    application.Render();

    Property::Map map;
    CreatePropertyMap( actor, map );

    DALI_TEST_CHECK( !map.Empty() );
    DALI_TEST_CHECK( NULL != map.Find( "size" ) );
    DALI_TEST_EQUALS( map.Find( "size" )->Get< Vector3 >(), Vector3::ONE, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "position" ) );
    DALI_TEST_EQUALS( map.Find( "position" )->Get< Vector3 >(), Vector3::XAXIS, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "scale" ) );
    DALI_TEST_EQUALS( map.Find( "scale" )->Get< Vector3 >(), Vector3::ZAXIS, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "visible" ) );
    DALI_TEST_EQUALS( map.Find( "visible" )->Get< bool >(), false, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "color" ) );
    DALI_TEST_EQUALS( map.Find( "color" )->Get< Vector4 >(), Color::MAGENTA, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "name" ) );
    DALI_TEST_EQUALS( map.Find( "name")->Get< std::string >(), "MyActor", TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "anchorPoint" ) );
    DALI_TEST_EQUALS( map.Find( "anchorPoint" )->Get< Vector3 >(), AnchorPoint::CENTER_LEFT, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "parentOrigin" ) );
    DALI_TEST_EQUALS( map.Find( "parentOrigin" )->Get< Vector3 >(), ParentOrigin::TOP_RIGHT, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "sensitive" ) );
    DALI_TEST_EQUALS( map.Find( "sensitive" )->Get< bool >(), false, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "leaveRequired" ) );
    DALI_TEST_EQUALS( map.Find( "leaveRequired" )->Get< bool >(), true, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "inheritOrientation" ) );
    DALI_TEST_EQUALS( map.Find( "inheritOrientation" )->Get< bool >(), false, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "inheritScale" ) );
    DALI_TEST_EQUALS( map.Find( "inheritScale" )->Get< bool >(), false, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "sizeModeFactor" ) );
    DALI_TEST_EQUALS( map.Find( "sizeModeFactor" )->Get< Vector3 >(), Vector3::ONE, TEST_LOCATION );

    Stage::GetCurrent().Remove( actor );
  }

  // ColorMode
  TestEnumStrings< ColorMode >( "colorMode",  application, COLOR_MODE_VALUES, COLOR_MODE_VALUES_COUNT, &Actor::SetColorMode );

  // PositionInheritanceMode
  TestEnumStrings< PositionInheritanceMode >( "positionInheritance",  application, POSITION_INHERITANCE_MODE_VALUES, POSITION_INHERITANCE_MODE_VALUES_COUNT, &Actor::SetPositionInheritanceMode );

  // DrawMode
  TestEnumStrings< DrawMode::Type >( "drawMode",  application, DRAW_MODE_VALUES, DRAW_MODE_VALUES_COUNT, &Actor::SetDrawMode );

  // Children
  {
    Actor actor = Actor::New();
    Actor child = Layer::New();
    actor.Add( child );

    Stage::GetCurrent().Add( actor );
    application.SendNotification();
    application.Render();

    Property::Map map;
    CreatePropertyMap( actor, map );
    DALI_TEST_CHECK( !map.Empty() );

    DALI_TEST_CHECK( NULL != map.Find( "type" ) );
    DALI_TEST_EQUALS( map.Find( "type" )->Get< std::string >(), "Actor", TEST_LOCATION );

    DALI_TEST_CHECK( NULL != map.Find( "actors" ) );
    Property::Array children( map.Find( "actors")->Get< Property::Array >() );
    DALI_TEST_CHECK( !children.Empty() );
    Property::Map childMap( children[0].Get< Property::Map >() );
    DALI_TEST_CHECK( !childMap.Empty() );
    DALI_TEST_CHECK( childMap.Find( "type" ) );
    DALI_TEST_EQUALS( childMap.Find( "type" )->Get< std::string >(), "Layer", TEST_LOCATION );

    Stage::GetCurrent().Remove( actor );
  }
  END_TEST;
}

int UtcDaliScriptingCreatePropertyMapImage(void)
{
  TestApplication application;

  // Empty
  {
    Image image;
    Property::Map map;
    CreatePropertyMap( image, map );
    DALI_TEST_CHECK( map.Empty() );
  }

  // Default
  {
    Image image = ResourceImage::New( "MY_PATH" );

    Property::Map map;
    CreatePropertyMap( image, map );
    DALI_TEST_CHECK( !map.Empty() );

    DALI_TEST_CHECK( NULL != map.Find( "type" ) );
    DALI_TEST_EQUALS( map.Find( "type" )->Get< std::string >(), "ResourceImage", TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "filename" ) );
    DALI_TEST_EQUALS( map.Find( "filename" )->Get< std::string >(), "MY_PATH", TEST_LOCATION );
    DALI_TEST_CHECK( NULL == map.Find( "width" ) );
    DALI_TEST_CHECK( NULL == map.Find( "height" ) );
  }

  // Change values
  {
    ResourceImage image = ResourceImage::New( "MY_PATH", ImageDimensions( 300, 400 ), FittingMode::FIT_WIDTH );

    Property::Map map;
    CreatePropertyMap( image, map );
    DALI_TEST_CHECK( !map.Empty() );

    DALI_TEST_CHECK( NULL != map.Find( "type" ) );
    DALI_TEST_EQUALS( map.Find( "type" )->Get< std::string >(), "ResourceImage", TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "filename" ) );
    DALI_TEST_EQUALS( map.Find( "filename" )->Get< std::string >(), "MY_PATH", TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "width" ) );
    DALI_TEST_EQUALS( map.Find( "width" )->Get< int >(), 300, TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "height" ) );
    DALI_TEST_EQUALS( map.Find( "height" )->Get< int >(), 400, TEST_LOCATION );
  }

  // BufferImage
  {
    Image image = BufferImage::New( 200, 300, Pixel::A8 );
    Property::Map map;
    CreatePropertyMap( image, map );
    DALI_TEST_CHECK( NULL != map.Find( "type" ) );
    DALI_TEST_EQUALS( map.Find( "type" )->Get< std::string >(), "BufferImage", TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "pixelFormat") );
    DALI_TEST_EQUALS( map.Find( "pixelFormat" )->Get< std::string >(), "A8", TEST_LOCATION );
  }

  // FrameBufferImage
  {
    Image image = FrameBufferImage::New( 200, 300, Pixel::RGBA8888 );
    Property::Map map;
    CreatePropertyMap( image, map );
    DALI_TEST_CHECK( NULL != map.Find( "type" ) );
    DALI_TEST_EQUALS( map.Find( "type" )->Get< std::string >(), "FrameBufferImage", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliScriptingGetEnumerationTemplates(void)
{
  const Scripting::StringEnum myTable[] =
  {
    { "ONE",    1 },
    { "TWO",    2 },
    { "THREE",  3 },
    { "FOUR",   4 },
    { "FIVE",   5 },
  };
  const unsigned int myTableCount = sizeof( myTable ) / sizeof( myTable[0] );

  for ( unsigned int i = 0; i < myTableCount; ++i )
  {
    tet_printf("Checking: %s\n", myTable[ i ].string );
    int value;
    DALI_TEST_CHECK( GetEnumeration<int>( myTable[ i ].string, myTable, myTableCount, value ) );
    DALI_TEST_EQUALS( myTable[ i ].value, value, TEST_LOCATION );
  }

  for ( unsigned int i = 0; i < myTableCount; ++i )
  {
    tet_printf("Checking: %d\n", myTable[ i ].value );
    DALI_TEST_EQUALS( myTable[ i ].string, GetEnumerationName( myTable[ i ].value, myTable, myTableCount ), TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliScriptingGetEnumerationNameN(void)
{
  const char* value = GetEnumerationName( 10, NULL, 0 );
  DALI_TEST_CHECK( NULL == value );

  value = GetEnumerationName( 10, NULL, 1 );
  DALI_TEST_CHECK( NULL == value );

  END_TEST;
}

int UtcDaliScriptingGetLinearEnumerationNameN(void)
{
  const char* value = GetLinearEnumerationName( 10, NULL, 0 );
  DALI_TEST_CHECK( NULL == value );

  value = GetLinearEnumerationName( 10, NULL, 1 );
  DALI_TEST_CHECK( NULL == value );

  END_TEST;
}

int UtcDaliScriptingGetEnumerationProperty(void)
{
  /*
   * This test function performs the following checks:
   *  - An enum can be looked up from a Property::Value of type INTEGER.
   *  - An enum can be looked up from a Property::Value of type STRING.
   *  - An enum can NOT be looked up for other Property::Value types.
   *  - The return value is "true" if the property can be successfully converted AND it has changed.
   *  - The return value is "false" if the property can be successfully converted BUT it has NOT changed.
   *  - The return value is "false" if the property can not be successfully converted.
   *  - The result value is only updated if the return value is "true" (IE. successful conversion and property value has changed).
   */

  // String to Enum property table to test with (equivalent to ones used within DALi).
  const Dali::Scripting::StringEnum  testTable[] = {
      { "NONE",           FaceCullingMode::NONE },
      { "FRONT",          FaceCullingMode::FRONT },
      { "BACK",           FaceCullingMode::BACK },
      { "FRONT_AND_BACK", FaceCullingMode::FRONT_AND_BACK }
  }; const unsigned int testTableCount = sizeof( testTable ) / sizeof( testTable[0] );

  // TEST: An enum can be looked up from a Property::Value of type INTEGER.
  // Initialise to first element.
  FaceCullingMode::Type result = FaceCullingMode::NONE;
  // Set the input property value to a different value (to emulate a change).
  Property::Value propertyValueInteger( FaceCullingMode::FRONT );

  // Perform the lookup.
  bool returnValue = GetEnumerationProperty< FaceCullingMode::Type >( propertyValueInteger, testTable, testTableCount, result );

  // TEST: The return value is "true" if the property can be successfully converted AND it has changed
  // Check the property could be converted.
  DALI_TEST_CHECK( returnValue );

  DALI_TEST_EQUALS( static_cast<int>( result ), static_cast<int>( FaceCullingMode::FRONT ), TEST_LOCATION );

  // Now emulate a property-set with the same value. false should be returned.
  returnValue = GetEnumerationProperty< FaceCullingMode::Type >( propertyValueInteger, testTable, testTableCount, result );

  // TEST: The return value is "false" if the property can be successfully converted BUT it has NOT changed.
  DALI_TEST_CHECK( !returnValue );

  // The result should remain the same.
  DALI_TEST_EQUALS( static_cast<int>( result ), static_cast<int>( FaceCullingMode::FRONT ), TEST_LOCATION );

  // TEST: An enum can be looked up from a Property::Value of type STRING.
  // Set the input property value to a different value (to emulate a change).
  Property::Value propertyValueString( "BACK" );

  returnValue = GetEnumerationProperty< FaceCullingMode::Type >( propertyValueString, testTable, testTableCount, result );

  DALI_TEST_CHECK( returnValue );

  // The result should remain the same.
  DALI_TEST_EQUALS( static_cast<int>( result ), static_cast<int>( FaceCullingMode::BACK ), TEST_LOCATION );

  returnValue = GetEnumerationProperty< FaceCullingMode::Type >( propertyValueString, testTable, testTableCount, result );

  DALI_TEST_CHECK( !returnValue );

  // The result should remain the same.
  DALI_TEST_EQUALS( static_cast<int>( result ), static_cast<int>( FaceCullingMode::BACK ), TEST_LOCATION );

  // TEST: An enum can NOT be looked up for other Property::Value types.
  Property::Value propertyValueBoolean( true );

  returnValue = GetEnumerationProperty< FaceCullingMode::Type >( propertyValueBoolean, testTable, testTableCount, result );

  // TEST: The return value is "false" if the property can not be successfully converted.
  // Return value should be false as Property::Value was of an unsupported type for enum properties.
  DALI_TEST_CHECK( !returnValue );

  // TEST: The result value is only updated if the return value is "true" (IE. successful conversion and property value has changed).
  // The result should remain the same.
  DALI_TEST_EQUALS( static_cast<int>( result ), static_cast<int>( FaceCullingMode::BACK ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliScriptingGetBitmaskEnumerationProperty(void)
{
  /*
   * This test function performs the following checks:
   *  - An enum can be looked up from a Property::Value of type INTEGER.
   *  - An enum can be looked up from a Property::Value of type STRING.
   *  - An enum can NOT be looked up from other Property::Value types.
   *  - The return value is "true" if the property can be successfully converted AND it has changed.
   *  - The return value is "false" if the property can not be successfully converted.
   *  - The result value is only updated if the return value is "true" (IE. successful conversion and property value has changed).
   *  PropertyArrays:
   *  - The return value when checking an array with 2 INTEGERS is "true" if the properties can be successfully converted.
   *  - The result value when checking an array with 2 INTEGERS is the ORd value of the 2 integers.
   *  - The return value when checking an array with 2 STRINGS is "true" if the properties can be successfully converted.
   *  - The result value when checking an array with 2 STRINGS is the ORd value of the 2 integer equivalents of the strings.
   *  - The return value when checking an array with an INTEGER and a STRING is "true" if the properties can be successfully converted.
   *  - The result value when checking an array with an INTEGER and a STRING is the ORd value of the 2 integer equivalents of the strings.
   *  - The return value when checking an array with an INTEGER and a Vector3 is "false" as the properties can not be successfully converted.
   *  - The result value when checking an array with an INTEGER and a Vector3 is unchanged.
   */

  // String to Enum property table to test with (equivalent to ones used within DALi).
  const Dali::Scripting::StringEnum  testTable[] = {
      { "NONE",           FaceCullingMode::NONE },
      { "FRONT",          FaceCullingMode::FRONT },
      { "BACK",           FaceCullingMode::BACK },
      { "FRONT_AND_BACK", FaceCullingMode::FRONT_AND_BACK }
  }; const unsigned int testTableCount = sizeof( testTable ) / sizeof( testTable[0] );

  // TEST: An enum can be looked up from a Property::Value of type INTEGER.
  // Initialise to first element.
  FaceCullingMode::Type result = FaceCullingMode::NONE;
  // Set the input property value to a different value (to emulate a change).
  Property::Value propertyValueInteger( FaceCullingMode::FRONT );

  // Perform the lookup.
  bool returnValue = GetBitmaskEnumerationProperty< FaceCullingMode::Type >( propertyValueInteger, testTable, testTableCount, result );

  // TEST: The return value is "true" if the property can be successfully converted AND it has changed
  // Check the property could be converted.
  DALI_TEST_CHECK( returnValue );

  DALI_TEST_EQUALS( static_cast<int>( result ), static_cast<int>( FaceCullingMode::FRONT ), TEST_LOCATION );

  // TEST: An enum can be looked up from a Property::Value of type STRING.
  // Set the input property value to a different value (to emulate a change).
  Property::Value propertyValueString( "BACK" );

  returnValue = GetBitmaskEnumerationProperty< FaceCullingMode::Type >( propertyValueString, testTable, testTableCount, result );

  DALI_TEST_CHECK( returnValue );

  DALI_TEST_EQUALS( static_cast<int>( result ), static_cast<int>( FaceCullingMode::BACK ), TEST_LOCATION );

  // TEST: An enum can NOT be looked up from other Property::Value types.
  Property::Value propertyValueVector( Vector3::ZERO );

  returnValue = GetBitmaskEnumerationProperty< FaceCullingMode::Type >( propertyValueVector, testTable, testTableCount, result );

  // TEST: The return value is "false" if the property can not be successfully converted.
  // Return value should be false as Property::Value was of an unsupported type for enum properties.
  DALI_TEST_CHECK( !returnValue );

  // TEST: The result value is only updated if the return value is "true" (IE. successful conversion and property value has changed).
  // The result should remain the same.
  DALI_TEST_EQUALS( static_cast<int>( result ), static_cast<int>( FaceCullingMode::BACK ), TEST_LOCATION );

  // Test PropertyArrays:

  // Property array of 2 integers.
  Property::Array propertyArrayIntegers;
  propertyArrayIntegers.PushBack( FaceCullingMode::FRONT );
  propertyArrayIntegers.PushBack( FaceCullingMode::BACK );
  result = FaceCullingMode::NONE;

  returnValue = GetBitmaskEnumerationProperty< FaceCullingMode::Type >( propertyArrayIntegers, testTable, testTableCount, result );

  // TEST: The return value when checking an array with 2 INTEGERS is "true" if the properties can be successfully converted.
  DALI_TEST_CHECK( returnValue );
  // TEST: The result value when checking an array with 2 INTEGERS is the ORd value of the 2 integers.
  DALI_TEST_CHECK( result == ( FaceCullingMode::FRONT | FaceCullingMode::BACK ) );

  // Property array of 2 strings.
  Property::Array propertyArrayStrings;
  propertyArrayStrings.PushBack( "FRONT" );
  propertyArrayStrings.PushBack( "BACK" );
  result = FaceCullingMode::NONE;

  returnValue = GetBitmaskEnumerationProperty< FaceCullingMode::Type >( propertyArrayStrings, testTable, testTableCount, result );

  // TEST: The return value when checking an array with 2 STRINGS is "true" if the properties can be successfully converted.
  DALI_TEST_CHECK( returnValue );
  // TEST: The result value when checking an array with 2 STRINGS is the ORd value of the 2 integer equivalents of the strings.
  DALI_TEST_CHECK( result == ( FaceCullingMode::FRONT | FaceCullingMode::BACK ) );

  // Property array of an int and a string.
  Property::Array propertyArrayMixed;
  propertyArrayMixed.PushBack( FaceCullingMode::FRONT );
  propertyArrayMixed.PushBack( "BACK" );
  result = FaceCullingMode::NONE;

  returnValue = GetBitmaskEnumerationProperty< FaceCullingMode::Type >( propertyArrayMixed, testTable, testTableCount, result );

  // TEST: The return value when checking an array with an INTEGER and a STRING is "true" if the properties can be successfully converted.
  DALI_TEST_CHECK( returnValue );
  // TEST: The result value when checking an array with an INTEGER and a STRING is the ORd value of the 2 integer equivalents of the strings.
  DALI_TEST_CHECK( result == ( FaceCullingMode::FRONT | FaceCullingMode::BACK ) );

  // Property array of an int and a string.
  Property::Array propertyArrayInvalid;
  propertyArrayInvalid.PushBack( FaceCullingMode::FRONT );
  propertyArrayInvalid.PushBack( Vector3::ZERO );

  // Set the initial value to non-zero, so we can test it does not change.
  result = FaceCullingMode::FRONT_AND_BACK;

  returnValue = GetBitmaskEnumerationProperty< FaceCullingMode::Type >( propertyArrayInvalid, testTable, testTableCount, result );

  // TEST: The return value when checking an array with an INTEGER and a Vector3 is "false" as the properties can not be successfully converted.
  DALI_TEST_CHECK( !returnValue );
  // TEST: The result value when checking an array with an INTEGER and a Vector3 is unchanged.
  DALI_TEST_CHECK( result == FaceCullingMode::FRONT_AND_BACK );

  END_TEST;
}

int UtcDaliScriptingFindEnumIndexN(void)
{
  const Scripting::StringEnum myTable[] =
    {
      { "ONE",    (1<<1) },
      { "TWO",    (1<<2) },
      { "THREE",  (1<<3) },
      { "FOUR",   (1<<4) },
      { "FIVE",   (1<<5) },
    };
  const unsigned int myTableCount = sizeof( myTable ) / sizeof( myTable[0] );
  DALI_TEST_EQUALS( myTableCount, FindEnumIndex( "Foo", myTable, myTableCount ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliScriptingEnumStringToIntegerP(void)
{
  const Scripting::StringEnum myTable[] =
    {
      { "ONE",    (1<<1) },
      { "TWO",    (1<<2) },
      { "THREE",  (1<<3) },
      { "FOUR",   (1<<4) },
      { "FIVE",   (1<<5) },
    };
  const unsigned int myTableCount = sizeof( myTable ) / sizeof( myTable[0] );

  int integerEnum = 0;
  DALI_TEST_CHECK( EnumStringToInteger( "ONE", myTable, myTableCount, integerEnum ) );

  DALI_TEST_EQUALS( integerEnum, (1<<1), TEST_LOCATION );

  integerEnum = 0;
  DALI_TEST_CHECK( EnumStringToInteger( "ONE,TWO", myTable, myTableCount, integerEnum ) );
  DALI_TEST_EQUALS( integerEnum, (1<<1) | (1<<2), TEST_LOCATION );

  DALI_TEST_CHECK( EnumStringToInteger( "ONE,,TWO", myTable, myTableCount, integerEnum ) );
  DALI_TEST_EQUALS( integerEnum, (1<<1) | (1<<2), TEST_LOCATION );

  DALI_TEST_CHECK( EnumStringToInteger( "ONE,TWO,THREE", myTable, myTableCount, integerEnum ) );
  DALI_TEST_EQUALS( integerEnum, (1<<1) | (1<<2) | (1<<3), TEST_LOCATION );

  DALI_TEST_CHECK( EnumStringToInteger( "ONE,TWO,THREE,FOUR,FIVE", myTable, myTableCount, integerEnum ) );
  DALI_TEST_EQUALS( integerEnum, (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5), TEST_LOCATION );

  DALI_TEST_CHECK( EnumStringToInteger( "TWO,ONE", myTable, myTableCount, integerEnum ) );
  DALI_TEST_EQUALS( integerEnum, (1<<1) | (1<<2), TEST_LOCATION );

  DALI_TEST_CHECK( EnumStringToInteger( "TWO,ONE,FOUR,THREE,FIVE", myTable, myTableCount, integerEnum ) );
  DALI_TEST_EQUALS( integerEnum, (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5), TEST_LOCATION );

  DALI_TEST_CHECK( EnumStringToInteger( "ONE,SEVEN", myTable, myTableCount, integerEnum ) );
  DALI_TEST_EQUALS( integerEnum, (1<<1), TEST_LOCATION );

  DALI_TEST_CHECK( EnumStringToInteger( "ONE,", myTable, myTableCount, integerEnum ) );
  DALI_TEST_EQUALS( integerEnum, (1<<1), TEST_LOCATION );


  END_TEST;
}

int UtcDaliScriptingEnumStringToIntegerN(void)
{
  const Scripting::StringEnum myTable[] =
  {
    { "ONE",    1 },
    { "TWO",    2 },
    { "THREE",  3 },
    { "FOUR",   4 },
    { "FIVE",   5 },
  };
  const unsigned int myTableCount = sizeof( myTable ) / sizeof( myTable[0] );

  int integerEnum = 0;
  DALI_TEST_CHECK( !EnumStringToInteger( "Foo", myTable, myTableCount, integerEnum ) );

  DALI_TEST_CHECK( !EnumStringToInteger( "", myTable, myTableCount, integerEnum ) );

  DALI_TEST_CHECK( !EnumStringToInteger( ",", myTable, myTableCount, integerEnum ) );

  DALI_TEST_CHECK( !EnumStringToInteger( ",ONE,SEVEN", myTable, myTableCount, integerEnum ) );

  DALI_TEST_CHECK( !EnumStringToInteger( ",", myTable, myTableCount, integerEnum ) );

  DALI_TEST_CHECK( !EnumStringToInteger( "ONE", myTable, 0, integerEnum ) );

  DALI_TEST_EQUALS( integerEnum, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliScriptingEnumStringToIntegerInvalidEnumP(void)
{
  const Scripting::StringEnum myTable[] =
  {
    { "",    1 },
    { "",    2 },
    { "",    3 },
  };

  const unsigned int myTableCount = sizeof( myTable ) / sizeof( myTable[0] );

  int integerEnum = 0;
  DALI_TEST_CHECK( EnumStringToInteger( "", myTable, myTableCount, integerEnum ) );
  DALI_TEST_EQUALS( integerEnum, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliScriptingEnumStringToIntegerInvalidEnumN(void)
{
  const Scripting::StringEnum myTable[] =
  {
    { "",    1 },
    { "",    1 },
    { "",    1 },
  };

  const unsigned int myTableCount = sizeof( myTable ) / sizeof( myTable[0] );

  int integerEnum = 0;
  DALI_TEST_CHECK( !EnumStringToInteger( NULL, NULL, 0, integerEnum ) );

  DALI_TEST_CHECK( !EnumStringToInteger( "ONE", NULL, 0, integerEnum ) );

  DALI_TEST_CHECK( !EnumStringToInteger( NULL, myTable, 0, integerEnum ) );

  DALI_TEST_CHECK( !EnumStringToInteger( NULL, myTable, myTableCount, integerEnum ) );

  DALI_TEST_CHECK( !EnumStringToInteger( "ONE", NULL, myTableCount, integerEnum ) );

  DALI_TEST_EQUALS( integerEnum, 0, TEST_LOCATION );

  END_TEST;
}
