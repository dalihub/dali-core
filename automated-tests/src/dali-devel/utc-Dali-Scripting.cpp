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
    { "OVERLAY_2D", DrawMode::OVERLAY_2D },
    { "STENCIL", DrawMode::STENCIL },
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

/// Helper method to create ResourceImage using property
ResourceImage NewResourceImage( const Property::Value& map )
{
  ResourceImage image = ResourceImage::DownCast( NewImage( map ) );
  return image;
}

/// Helper method to create ResourceImage using property
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



int UtcDaliScriptingGetColorMode(void)
{
  for ( unsigned int i = 0; i < COLOR_MODE_VALUES_COUNT; ++i )
  {
    tet_printf( "Checking %s == %d\n", COLOR_MODE_VALUES[i].string, COLOR_MODE_VALUES[i].value );
    DALI_TEST_EQUALS( COLOR_MODE_VALUES[i].value, (int)GetColorMode( COLOR_MODE_VALUES[i].string ), TEST_LOCATION );
    DALI_TEST_EQUALS( COLOR_MODE_VALUES[i].string, GetColorMode( (ColorMode) COLOR_MODE_VALUES[i].value ), TEST_LOCATION );
  }

  DALI_TEST_EQUALS( USE_OWN_MULTIPLY_PARENT_ALPHA, GetColorMode("INVALID_ARG"), TEST_LOCATION );
  END_TEST;
}

int UtcDaliScriptingGetPositionInheritanceMode(void)
{
  for ( unsigned int i = 0; i < POSITION_INHERITANCE_MODE_VALUES_COUNT; ++i )
  {
    tet_printf( "Checking %s == %d\n", POSITION_INHERITANCE_MODE_VALUES[i].string, POSITION_INHERITANCE_MODE_VALUES[i].value );
    DALI_TEST_EQUALS( POSITION_INHERITANCE_MODE_VALUES[i].value, (int)GetPositionInheritanceMode( POSITION_INHERITANCE_MODE_VALUES[i].string ), TEST_LOCATION );
    DALI_TEST_EQUALS( POSITION_INHERITANCE_MODE_VALUES[i].string, GetPositionInheritanceMode( (PositionInheritanceMode) POSITION_INHERITANCE_MODE_VALUES[i].value ), TEST_LOCATION );
  }

  DALI_TEST_EQUALS( POSITION_INHERITANCE_MODE_VALUES[0].value, (int)GetPositionInheritanceMode("INVALID_ARG"), TEST_LOCATION );
  END_TEST;
}


int UtcDaliScriptingGetDrawMode(void)
{
  for ( unsigned int i = 0; i < DRAW_MODE_VALUES_COUNT; ++i )
  {
    tet_printf( "Checking %s == %d\n", DRAW_MODE_VALUES[i].string, DRAW_MODE_VALUES[i].value );
    DALI_TEST_EQUALS( DRAW_MODE_VALUES[i].value, (int)GetDrawMode( DRAW_MODE_VALUES[i].string ), TEST_LOCATION );
    DALI_TEST_EQUALS( DRAW_MODE_VALUES[i].string, GetDrawMode( (DrawMode::Type) DRAW_MODE_VALUES[i].value ), TEST_LOCATION );
  }

  DALI_TEST_EQUALS( DRAW_MODE_VALUES[0].value, (int)GetDrawMode( "INVALID_ARG" ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliScriptingGetAnchorConstant(void)
{
  DALI_TEST_EQUALS( Dali::ParentOrigin::TOP_LEFT, GetAnchorConstant( "TOP_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::TOP_CENTER, GetAnchorConstant( "TOP_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::TOP_RIGHT, GetAnchorConstant( "TOP_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::CENTER_LEFT, GetAnchorConstant( "CENTER_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::CENTER, GetAnchorConstant( "CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::CENTER_RIGHT, GetAnchorConstant( "CENTER_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BOTTOM_LEFT, GetAnchorConstant( "BOTTOM_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BOTTOM_CENTER, GetAnchorConstant( "BOTTOM_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BOTTOM_RIGHT, GetAnchorConstant( "BOTTOM_RIGHT" ), TEST_LOCATION );

  DALI_TEST_EQUALS( Vector3(), GetAnchorConstant("INVALID_ARG"), TEST_LOCATION );
  END_TEST;
}

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

int UtcDaliScriptingNewImageNegative02(void)
{
  TestApplication application; // Image needs application
  // Invalid load-policy value type
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "loadPolicy" ] = Vector3::ZERO;
  // will give us a valid image handle with default load policy
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  DALI_TEST_EQUALS( resImage.GetLoadPolicy(), ResourceImage::IMMEDIATE, TEST_LOCATION );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative03(void)
{
  TestApplication application; // Image needs application
  // Invalid load-policy value
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "loadPolicy" ] = "INVALID";
  // will give us a valid image with default load policy
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  DALI_TEST_EQUALS( resImage.GetLoadPolicy(), ResourceImage::IMMEDIATE, TEST_LOCATION );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative04(void)
{
  TestApplication application; // Image needs application
  // Invalid release-policy value type
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "releasePolicy" ] = Vector3::ZERO;
  // will give us a valid image with default release policy
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  DALI_TEST_EQUALS( resImage.GetReleasePolicy(), Image::NEVER, TEST_LOCATION );
  END_TEST;
}

int UtcDaliScriptingNewImageNegative05(void)
{
  TestApplication application; // Image needs application
  // Invalid release-policy value
  Property::Map map;
  map[ "filename" ] = "TEST_FILE";
  map[ "releasePolicy" ] = "INVALID";
  // will give us a valid image with default release policy
  Image image = NewImage( map );
  DALI_TEST_CHECK( image );
  ResourceImage resImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resImage );
  DALI_TEST_EQUALS( resImage.GetReleasePolicy(), Image::NEVER, TEST_LOCATION );
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

int UtcDaliScriptingNewImage02P(void)
{
  TestApplication application;

  Property::Map map;
  map[ "filename" ] = "TEST_FILE";

  // load-policy
  map[ "loadPolicy" ] = "";
  const StringEnum values[] =
  {
    { "IMMEDIATE", ResourceImage::IMMEDIATE },
    { "ON_DEMAND", ResourceImage::ON_DEMAND }
  };
  TestEnumStrings< ResourceImage::LoadPolicy, ResourceImage >( map, "loadPolicy",  values, ( sizeof( values ) / sizeof ( values[0] ) ), &ResourceImage::GetLoadPolicy, &NewResourceImage );
  END_TEST;
}

int UtcDaliScriptingNewImage03P(void)
{
  TestApplication application;

  Property::Map map;
  map[ "filename" ] = "TEST_FILE";

  // release-policy
  map[ "releasePolicy" ] = "";
  const StringEnum values[] =
  {
    { "UNUSED", Image::UNUSED },
    { "NEVER", Image::NEVER }
  };
  TestEnumStrings< Image::ReleasePolicy, Image >( map, "releasePolicy",  values, ( sizeof( values ) / sizeof ( values[0] ) ), &Image::GetReleasePolicy, &NewImage );
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

int UtcDaliScriptingNewShaderEffect(void)
{
  TestApplication application;

  Property::Map programMap;
  programMap[ "vertexFilename" ] = "bump.vert";
  programMap[ "fragmentFilename" ] = "bump.frag";

  Property::Map imageMap;
  imageMap[ "filename" ] = "image.png";

  Property::Map map;
  map[ "image" ] = imageMap;
  map[ "program" ] = programMap;
  map[ "uLightPosition" ] = Vector3( 0.0, 0.0, -1.5);
  map[ "uAmbientLight" ] = (int)10;

  ShaderEffect shader = NewShaderEffect( map );

  DALI_TEST_CHECK( shader );
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
  map[ "inheritShaderEffect" ] = false;
  map[ "sensitive" ] = false;
  map[ "leaveRequired" ] = true;
  map[ "positionInheritance" ] = "DONT_INHERIT_POSITION";
  map[ "drawMode" ] = "STENCIL";
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
    DALI_TEST_EQUALS( handle.GetDrawMode(), DrawMode::STENCIL, TEST_LOCATION );
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

int UtcDaliScriptingNewActorChildren(void)
{
  TestApplication application;

  Property::Map map;
  map[ "type" ] = "Actor";
  map[ "position" ] = Vector3::XAXIS;

  Property::Map child1Map;
  child1Map[ "type" ] = "ImageActor";
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
  DALI_TEST_CHECK( ImageActor::DownCast( child1 ) );
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

  // ImageActor Type
  {
    Actor actor = ImageActor::New();

    Property::Map map;
    CreatePropertyMap( actor, map );
    DALI_TEST_CHECK( !map.Empty() );
    DALI_TEST_CHECK( NULL != map.Find( "type" ) );
    DALI_TEST_EQUALS( map.Find( "type" )->Get< std::string >(), "ImageActor", TEST_LOCATION );

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
    Actor child = ImageActor::New();
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
    DALI_TEST_EQUALS( childMap.Find( "type" )->Get< std::string >(), "ImageActor", TEST_LOCATION );

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
    DALI_TEST_CHECK( NULL != map.Find( "loadPolicy") );
    DALI_TEST_EQUALS( map.Find( "loadPolicy" )->Get< std::string >(), "IMMEDIATE", TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "releasePolicy") );
    DALI_TEST_EQUALS( map.Find( "releasePolicy" )->Get< std::string >(), "NEVER", TEST_LOCATION );
    DALI_TEST_CHECK( NULL == map.Find( "width" ) );
    DALI_TEST_CHECK( NULL == map.Find( "height" ) );
  }

  // Change values
  {
    ResourceImage image = ResourceImage::New( "MY_PATH", ResourceImage::ON_DEMAND, Image::UNUSED, ImageDimensions( 300, 400 ), FittingMode::FIT_WIDTH );

    Property::Map map;
    CreatePropertyMap( image, map );
    DALI_TEST_CHECK( !map.Empty() );

    DALI_TEST_CHECK( NULL != map.Find( "type" ) );
    DALI_TEST_EQUALS( map.Find( "type" )->Get< std::string >(), "ResourceImage", TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "filename" ) );
    DALI_TEST_EQUALS( map.Find( "filename" )->Get< std::string >(), "MY_PATH", TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "loadPolicy") );
    DALI_TEST_EQUALS( map.Find( "loadPolicy" )->Get< std::string >(), "ON_DEMAND", TEST_LOCATION );
    DALI_TEST_CHECK( NULL != map.Find( "releasePolicy") );
    DALI_TEST_EQUALS( map.Find( "releasePolicy" )->Get< std::string >(), "UNUSED", TEST_LOCATION );
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

int UtcDaliScriptingFindEnumIndexN(void)
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
  DALI_TEST_EQUALS( myTableCount, FindEnumIndex( "Foo", myTable, myTableCount ), TEST_LOCATION );

  END_TEST;
}
