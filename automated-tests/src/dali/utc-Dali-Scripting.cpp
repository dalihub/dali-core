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
using namespace Dali::Scripting;

namespace
{

const StringEnum< int > COLOR_MODE_VALUES[] =
{
    { "USE_OWN_COLOR", USE_OWN_COLOR },
    { "USE_PARENT_COLOR", USE_PARENT_COLOR },
    { "USE_OWN_MULTIPLY_PARENT_COLOR", USE_OWN_MULTIPLY_PARENT_COLOR },
    { "USE_OWN_MULTIPLY_PARENT_ALPHA", USE_OWN_MULTIPLY_PARENT_ALPHA },
};
const unsigned int COLOR_MODE_VALUES_COUNT = sizeof( COLOR_MODE_VALUES ) / sizeof( COLOR_MODE_VALUES[0] );

const StringEnum< int > POSITION_INHERITANCE_MODE_VALUES[] =
{
    { "INHERIT_PARENT_POSITION", INHERIT_PARENT_POSITION },
    { "USE_PARENT_POSITION", USE_PARENT_POSITION },
    { "USE_PARENT_POSITION_PLUS_LOCAL_POSITION", USE_PARENT_POSITION_PLUS_LOCAL_POSITION },
    { "DONT_INHERIT_POSITION", DONT_INHERIT_POSITION },
};
const unsigned int POSITION_INHERITANCE_MODE_VALUES_COUNT = sizeof( POSITION_INHERITANCE_MODE_VALUES ) / sizeof( POSITION_INHERITANCE_MODE_VALUES[0] );

const StringEnum< int > DRAW_MODE_VALUES[] =
{
    { "NORMAL", DrawMode::NORMAL },
    { "OVERLAY", DrawMode::OVERLAY },
    { "STENCIL", DrawMode::STENCIL },
};
const unsigned int DRAW_MODE_VALUES_COUNT = sizeof( DRAW_MODE_VALUES ) / sizeof( DRAW_MODE_VALUES[0] );


//////////////////////////////////////////////////////////////////////////////
// Helpers for string to enum comparisons for Image and ImageAttributes
//////////////////////////////////////////////////////////////////////////////

/**
 * Template to check enumerations of type T, with a class of type X
 */
template< typename T, typename X >
void TestEnumStrings(
  Property::Map& map,                       // The map used to create instance of type X
  const StringEnum< int >* values,          // An array of string values
  unsigned int num,                         // Number of items in the array
  T ( X::*method )() const,                 // The member method of X to call to get the enum
  X ( *creator ) ( const Property::Value& ) // The method which creates an instance of type X
)
{
  const unsigned int lastIndex( map.Count() - 1 );
  const std::string& key = map.GetKey( lastIndex );
  Property::Value& value = map.GetValue( lastIndex );

  for ( unsigned int i = 0; i < num; ++i )
  {
    value = values[i].string;
    tet_printf("Checking: %s: %s\n", key.c_str(), values[i].string );
    X instance = creator( map );
    DALI_TEST_EQUALS( values[i].value, ( instance.*method )(), TEST_LOCATION );
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

/// Helper method to create ImageAttributes using an Image
ImageAttributes NewImageAttributes( const Property::Value& map )
{
  ResourceImage image = ResourceImage::DownCast( NewImage( map ) );
  return image.GetAttributes();
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
  const StringEnum< int >* values,          // An array of string values
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

    DALI_TEST_CHECK( map.Count() );
    Property::Value value( map );
    DALI_TEST_CHECK( value.HasKey( keyName ) );
    DALI_TEST_EQUALS( value.GetValue( keyName ).Get< std::string >(), values[i].string, TEST_LOCATION );

    Stage::GetCurrent().Remove( actor );
  }
}

//////////////////////////////////////////////////////////////////////////////


} // anon namespace



int UtcDaliScriptingGetColorMode(void)
{
  TestApplication application;

  for ( unsigned int i = 0; i < COLOR_MODE_VALUES_COUNT; ++i )
  {
    tet_printf( "Checking %s == %d\n", COLOR_MODE_VALUES[i].string, COLOR_MODE_VALUES[i].value );
    DALI_TEST_EQUALS( COLOR_MODE_VALUES[i].value, GetColorMode( COLOR_MODE_VALUES[i].string ), TEST_LOCATION );
    DALI_TEST_EQUALS( COLOR_MODE_VALUES[i].string, GetColorMode( (ColorMode) COLOR_MODE_VALUES[i].value ), TEST_LOCATION );
  }

  try
  {
    (void)GetColorMode("INVALID_ARG");
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliScriptingGetPositionInheritanceMode(void)
{
  TestApplication application;

  for ( unsigned int i = 0; i < POSITION_INHERITANCE_MODE_VALUES_COUNT; ++i )
  {
    tet_printf( "Checking %s == %d\n", POSITION_INHERITANCE_MODE_VALUES[i].string, POSITION_INHERITANCE_MODE_VALUES[i].value );
    DALI_TEST_EQUALS( POSITION_INHERITANCE_MODE_VALUES[i].value, GetPositionInheritanceMode( POSITION_INHERITANCE_MODE_VALUES[i].string ), TEST_LOCATION );
    DALI_TEST_EQUALS( POSITION_INHERITANCE_MODE_VALUES[i].string, GetPositionInheritanceMode( (PositionInheritanceMode) POSITION_INHERITANCE_MODE_VALUES[i].value ), TEST_LOCATION );
  }

  try
  {
    (void)GetPositionInheritanceMode("INVALID_ARG");
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
  }
  END_TEST;
}


int UtcDaliScriptingGetDrawMode(void)
{
  TestApplication application;

  for ( unsigned int i = 0; i < DRAW_MODE_VALUES_COUNT; ++i )
  {
    tet_printf( "Checking %s == %d\n", DRAW_MODE_VALUES[i].string, DRAW_MODE_VALUES[i].value );
    DALI_TEST_EQUALS( DRAW_MODE_VALUES[i].value, GetDrawMode( DRAW_MODE_VALUES[i].string ), TEST_LOCATION );
    DALI_TEST_EQUALS( DRAW_MODE_VALUES[i].string, GetDrawMode( (DrawMode::Type) DRAW_MODE_VALUES[i].value ), TEST_LOCATION );
  }

  try
  {
    (void)GetDrawMode("INVALID_ARG");
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliScriptingGetAnchorConstant(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( Dali::ParentOrigin::TOP_LEFT, GetAnchorConstant( "TOP_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::TOP_CENTER, GetAnchorConstant( "TOP_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::TOP_RIGHT, GetAnchorConstant( "TOP_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::CENTER_LEFT, GetAnchorConstant( "CENTER_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::CENTER, GetAnchorConstant( "CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::CENTER_RIGHT, GetAnchorConstant( "CENTER_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BOTTOM_LEFT, GetAnchorConstant( "BOTTOM_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BOTTOM_CENTER, GetAnchorConstant( "BOTTOM_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BOTTOM_RIGHT, GetAnchorConstant( "BOTTOM_RIGHT" ), TEST_LOCATION );

  try
  {
    (void)GetAnchorConstant("INVALID_ARG");
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliScriptingNewImageNegative(void)
{
  TestApplication application;

  // Invalid filename
  try
  {
    Property::Map map;
    map[ "filename" ] = Vector3::ZERO;
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );
  }

  // Invalid load-policy
  try
  {
    Property::Map map;
    map[ "load-policy" ] = Vector3::ZERO;
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );

    // Invalid value
    try
    {
      Property::Map map;
      map[ "load-policy" ] = "INVALID";
      Image image = NewImage( map );
      tet_result( TET_FAIL );
    }
    catch ( DaliException& e )
    {
      DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
    }
  }

  // Invalid release-policy
  try
  {
    Property::Map map;
    map[ "release-policy" ] = Vector3::ZERO;
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );

    // Invalid value
    try
    {
      Property::Map map;
      map[ "release-policy" ] = "INVALID";
      Image image = NewImage( map );
      tet_result( TET_FAIL );
    }
    catch ( DaliException& e )
    {
      DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
    }
  }

  // Invalid width
  try
  {
    Property::Map map;
    map[ "width" ] = "Invalid";
    map[ "height" ] = "Invalid";
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "value.GetType()", TEST_LOCATION );
  }

  // Invalid height
  try
  {
    Property::Map map;
    map[ "width" ] = 10;
    map[ "height" ] = "Invalid";
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "value.GetType()", TEST_LOCATION );
  }

  // Invalid pixel-format
  try
  {
    Property::Map map;
    map[ "pixel-format" ] = Vector3::ZERO;
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );

    // Invalid value
    try
    {
      Property::Map map;
      map[ "pixel-format" ] = "INVALID";
      Image image = NewImage( map );
      tet_result( TET_FAIL );
    }
    catch ( DaliException& e )
    {
      DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
    }
  }

  // Invalid scaling-mode
  try
  {
    Property::Map map;
    map[ "scaling-mode" ] = Vector3::ZERO;
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );

    // Invalid value
    try
    {
      Property::Map map;
      map[ "scaling-mode" ] = "INVALID";
      Image image = NewImage( map );
      tet_result( TET_FAIL );
    }
    catch ( DaliException& e )
    {
      DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
    }
  }

  // Invalid type
  try
  {
    Property::Map map;
    map[ "type" ] = Vector3::ZERO;
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(\"type\").GetType()", TEST_LOCATION );

    // Invalid value
    try
    {
      Property::Map map;
      map[ "type" ] = "INVALID";
      Image image = NewImage( map );
      tet_result( TET_FAIL );
    }
    catch ( DaliException& e )
    {
      DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
    }
  }
  END_TEST;
}


int UtcDaliScriptingNewImage(void)
{
  TestApplication application;

  Property::Map map;
  map[ "filename" ] = "TEST_FILE";

  // Filename only
  {
    ResourceImage image = ResourceImage::DownCast( NewImage( map ) );
    DALI_TEST_EQUALS( "TEST_FILE", image.GetUrl(), TEST_LOCATION );
  }

  // load-policy
  map[ "load-policy" ] = "";
  {
    const StringEnum< int > values[] =
    {
        { "IMMEDIATE", ResourceImage::IMMEDIATE },
        { "ON_DEMAND", ResourceImage::ON_DEMAND }
    };
    TestEnumStrings< ResourceImage::LoadPolicy, ResourceImage >( map, values, ( sizeof( values ) / sizeof ( values[0] ) ), &ResourceImage::GetLoadPolicy, &NewResourceImage );
  }

  // release-policy
  map[ "release-policy" ] = "";
  {
    const StringEnum< int > values[] =
    {
        { "UNUSED", Image::UNUSED },
        { "NEVER", Image::NEVER }
    };
    TestEnumStrings< Image::ReleasePolicy, Image >( map, values, ( sizeof( values ) / sizeof ( values[0] ) ), &Image::GetReleasePolicy, &NewImage );
  }

  // float width and height
  map[ "width" ] = (float) 10.0f;
  map[ "height" ] = (float) 20.0f;
  {
    Image image = NewImage( map );
    DALI_TEST_EQUALS( image.GetWidth(), 10.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( image.GetHeight(), 20.0f, TEST_LOCATION );
  }

  // int width and height
  map[ "width"] = (int) 50;
  map[ "height" ] = (int) 70;
  {
    Image image = NewImage( map );
    DALI_TEST_EQUALS( image.GetWidth(), 50u, TEST_LOCATION );
    DALI_TEST_EQUALS( image.GetHeight(), 70u, TEST_LOCATION );
  }

  //map.erase( map.end() - 2, map.end() );

  // scaling-mode
  map[ "scaling-mode" ] = "";
  {
    const StringEnum< int > values[] =
    {
        { "SHRINK_TO_FIT", ImageAttributes::ShrinkToFit },
        { "SCALE_TO_FILL", ImageAttributes::ScaleToFill },
        { "FIT_WIDTH", ImageAttributes::FitWidth },
        { "FIT_HEIGHT", ImageAttributes::FitHeight },
    };
    TestEnumStrings< ImageAttributes::ScalingMode, ImageAttributes >( map, values, ( sizeof( values ) / sizeof ( values[0] ) ), &ImageAttributes::GetScalingMode, &NewImageAttributes );
  }

  // type FrameBufferImage
  map[ "type" ] = "FrameBufferImage";
  {
    Image image = NewImage( map );
    DALI_TEST_CHECK( FrameBufferImage::DownCast( image ) );
  }
  // type BufferImage
   map[ "type" ] = "BufferImage";
   {
     Image image = NewImage( map );
     DALI_TEST_CHECK( BufferImage::DownCast( image ) );
     DALI_TEST_CHECK((BufferImage::DownCast( image )).GetPixelFormat()== Pixel::RGBA8888);
   }

   // pixel-format
   map[ "pixel-format" ] = "";
   {
     const StringEnum< int > values[] =
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
       /*{ "COMPRESSED_R11_EAC", Pixel::COMPRESSED_R11_EAC },
         { "COMPRESSED_SIGNED_R11_EAC", Pixel::COMPRESSED_SIGNED_R11_EAC },
         { "COMPRESSED_RG11_EAC", Pixel::COMPRESSED_RG11_EAC },
         { "COMPRESSED_SIGNED_RG11_EAC", Pixel::COMPRESSED_SIGNED_RG11_EAC },
         { "COMPRESSED_RGB8_ETC2", Pixel::COMPRESSED_RGB8_ETC2 },
         { "COMPRESSED_SRGB8_ETC2", Pixel::COMPRESSED_SRGB8_ETC2 },
         { "COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2", Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 },
         { "COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2", Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 },
         { "COMPRESSED_RGBA8_ETC2_EAC", Pixel::COMPRESSED_RGBA8_ETC2_EAC },
         { "COMPRESSED_SRGB8_ALPHA8_ETC2_EAC", Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC },
         { "COMPRESSED_RGB8_ETC1", Pixel::COMPRESSED_RGB8_ETC1 },
         { "COMPRESSED_RGB_PVRTC_4BPPV1", Pixel::COMPRESSED_RGB_PVRTC_4BPPV1 },*/
         // BufferImage doesnot support compressed format
     };

     TestEnumStrings< Pixel::Format, BufferImage >( map, values, ( sizeof( values ) / sizeof ( values[0] ) ), &BufferImage::GetPixelFormat, &NewBufferImage );
   }

  // type Image
  map[ "type" ] = "ResourceImage";
  {
    Image image = NewImage( map );
    DALI_TEST_CHECK( ResourceImage::DownCast( image ) );
    DALI_TEST_CHECK( !FrameBufferImage::DownCast( image ) );
    DALI_TEST_CHECK( !BufferImage::DownCast( image ) );
  }
  END_TEST;
}

int UtcDaliScriptingNewShaderEffect(void)
{
  TestApplication application;

  Property::Map programMap;
  programMap[ "vertex-filename" ] = "bump.vert";
  programMap[ "fragment-filename" ] = "bump.frag";

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
    map[ "parent-origin" ] = ParentOrigin::TOP_CENTER;
    map[ "anchor-point" ] = AnchorPoint::TOP_CENTER;
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
    DALI_TEST_CHECK( !handle.MouseWheelEventSignal().GetConnectionCount() );
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
  map[ "color-mode" ] = "USE_PARENT_COLOR";
  map[ "inherit-shader-effect" ] = false;
  map[ "sensitive" ] = false;
  map[ "leave-required" ] = true;
  map[ "position-inheritance" ] = "DONT_INHERIT_POSITION";
  map[ "draw-mode" ] = "STENCIL";
  map[ "inherit-rotation" ] = false;
  map[ "inherit-scale" ] = false;

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
    DALI_TEST_EQUALS( handle.IsRotationInherited(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.IsScaleInherited(), false, TEST_LOCATION );

    Stage::GetCurrent().Remove( handle );
  }

  // Check Anchor point and parent origin vector3s
  map[ "parent-origin" ] = ParentOrigin::TOP_CENTER;
  map[ "anchor-point" ] = AnchorPoint::TOP_LEFT;
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
  map[ "parent-origin" ] = "TOP_LEFT";
  map[ "anchor-point" ] = "CENTER_LEFT";
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

  Property::Map child2Map;
  child2Map[ "type" ] = "TextActor";
  child2Map[ "position" ] = Vector3::ZAXIS;

  Property::Array childArray;
  childArray.push_back( child1Map );
  childArray.push_back( child2Map );
  map[ "actors" ] = childArray;

  // Create
  Actor handle = NewActor( map );
  DALI_TEST_CHECK( handle );

  Stage::GetCurrent().Add( handle );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( handle.GetCurrentPosition(), Vector3::XAXIS, TEST_LOCATION );
  DALI_TEST_EQUALS( handle.GetChildCount(), 2u, TEST_LOCATION );

  Actor child1 = handle.GetChildAt(0);
  DALI_TEST_CHECK( child1 );
  DALI_TEST_CHECK( ImageActor::DownCast( child1 ) );
  DALI_TEST_EQUALS( child1.GetCurrentPosition(), Vector3::YAXIS, TEST_LOCATION );
  DALI_TEST_EQUALS( child1.GetChildCount(), 0u, TEST_LOCATION );

  Actor child2 = handle.GetChildAt(1);
  DALI_TEST_CHECK( child2 );
  DALI_TEST_CHECK( TextActor::DownCast( child2 ) );
  DALI_TEST_EQUALS( child2.GetCurrentPosition(), Vector3::ZAXIS, TEST_LOCATION );
  DALI_TEST_EQUALS( child2.GetChildCount(), 0u, TEST_LOCATION );

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
    Property::Value value( map );
    DALI_TEST_CHECK( value.HasKey( "type" ) );
    DALI_TEST_EQUALS( value.GetValue( "type").Get< std::string >(), "Actor", TEST_LOCATION );

    Stage::GetCurrent().Remove( actor );
  }

  // ImageActor Type
  {
    Actor actor = ImageActor::New();

    Property::Map map;
    CreatePropertyMap( actor, map );
    DALI_TEST_CHECK( !map.Empty() );
    Property::Value value( map );
    DALI_TEST_CHECK( value.HasKey( "type" ) );
    DALI_TEST_EQUALS( value.GetValue( "type").Get< std::string >(), "ImageActor", TEST_LOCATION );

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
    actor.SetInheritRotation( false );
    actor.SetInheritScale( false );
    actor.SetSizeMode( USE_OWN_SIZE );
    actor.SetSizeModeFactor( Vector3::ONE );

    Stage::GetCurrent().Add( actor );
    application.SendNotification();
    application.Render();

    Property::Map map;
    CreatePropertyMap( actor, map );

    DALI_TEST_CHECK( !map.Empty() );
    Property::Value value( map );
    DALI_TEST_CHECK( value.HasKey( "size" ) );
    DALI_TEST_EQUALS( value.GetValue( "size" ).Get< Vector3 >(), Vector3::ONE, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "position" ) );
    DALI_TEST_EQUALS( value.GetValue( "position" ).Get< Vector3 >(), Vector3::XAXIS, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "scale" ) );
    DALI_TEST_EQUALS( value.GetValue( "scale" ).Get< Vector3 >(), Vector3::ZAXIS, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "visible" ) );
    DALI_TEST_EQUALS( value.GetValue( "visible" ).Get< bool >(), false, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "color" ) );
    DALI_TEST_EQUALS( value.GetValue( "color" ).Get< Vector4 >(), Color::MAGENTA, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "name" ) );
    DALI_TEST_EQUALS( value.GetValue( "name").Get< std::string >(), "MyActor", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "anchor-point" ) );
    DALI_TEST_EQUALS( value.GetValue( "anchor-point" ).Get< Vector3 >(), AnchorPoint::CENTER_LEFT, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "parent-origin" ) );
    DALI_TEST_EQUALS( value.GetValue( "parent-origin" ).Get< Vector3 >(), ParentOrigin::TOP_RIGHT, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "sensitive" ) );
    DALI_TEST_EQUALS( value.GetValue( "sensitive" ).Get< bool >(), false, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "leave-required" ) );
    DALI_TEST_EQUALS( value.GetValue( "leave-required" ).Get< bool >(), true, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "inherit-rotation" ) );
    DALI_TEST_EQUALS( value.GetValue( "inherit-rotation" ).Get< bool >(), false, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "inherit-scale" ) );
    DALI_TEST_EQUALS( value.GetValue( "inherit-scale" ).Get< bool >(), false, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "size-mode-factor" ) );
    DALI_TEST_EQUALS( value.GetValue( "size-mode-factor" ).Get< Vector3 >(), Vector3::ONE, TEST_LOCATION );

    Stage::GetCurrent().Remove( actor );
  }

  // ColorMode
  TestEnumStrings< ColorMode >( "color-mode", application, COLOR_MODE_VALUES, COLOR_MODE_VALUES_COUNT, &Actor::SetColorMode );

  // PositionInheritanceMode
  TestEnumStrings< PositionInheritanceMode >( "position-inheritance", application, POSITION_INHERITANCE_MODE_VALUES, POSITION_INHERITANCE_MODE_VALUES_COUNT, &Actor::SetPositionInheritanceMode );

  // DrawMode
  TestEnumStrings< DrawMode::Type >( "draw-mode", application, DRAW_MODE_VALUES, DRAW_MODE_VALUES_COUNT, &Actor::SetDrawMode );

  // Children
  {
    Actor actor = Actor::New();
    Actor child = ImageActor::New();
    Actor grandChild = TextActor::New();

    actor.Add( child );
    child.Add( grandChild );

    Stage::GetCurrent().Add( actor );
    application.SendNotification();
    application.Render();

    Property::Map map;
    CreatePropertyMap( actor, map );
    DALI_TEST_CHECK( !map.Empty() );

    Property::Value value( map );
    DALI_TEST_CHECK( value.HasKey( "type" ) );
    DALI_TEST_EQUALS( value.GetValue( "type" ).Get< std::string >(), "Actor", TEST_LOCATION );

    DALI_TEST_CHECK( value.HasKey( "actors" ) );
    Property::Array children( value.GetValue( "actors").Get< Property::Array >() );
    DALI_TEST_CHECK( !children.empty() );
    Property::Map childMap( children[0].Get< Property::Map >() );
    DALI_TEST_CHECK( !childMap.Empty() );
    Property::Value childValue( childMap );
    DALI_TEST_CHECK( childValue.HasKey( "type" ) );
    DALI_TEST_EQUALS( childValue.GetValue( "type" ).Get< std::string >(), "ImageActor", TEST_LOCATION );

    DALI_TEST_CHECK( childValue.HasKey( "actors" ) );
    Property::Array grandChildren( childValue.GetValue( "actors").Get< Property::Array >() );
    DALI_TEST_CHECK( grandChildren.size() == 1u );

    Property::Map grandChildMap( grandChildren[0].Get< Property::Map >() );
    DALI_TEST_CHECK( !grandChildMap.Empty() );
    Property::Value grandChildValue( grandChildMap );
    DALI_TEST_CHECK( grandChildValue.HasKey( "type" ) );
    DALI_TEST_EQUALS( grandChildValue.GetValue( "type" ).Get< std::string >(), "TextActor", TEST_LOCATION );


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

    Property::Value value( map );
    DALI_TEST_CHECK( value.HasKey( "type" ) );
    DALI_TEST_EQUALS( value.GetValue( "type" ).Get< std::string >(), "ResourceImage", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "filename" ) );
    DALI_TEST_EQUALS( value.GetValue( "filename" ).Get< std::string >(), "MY_PATH", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "load-policy") );
    DALI_TEST_EQUALS( value.GetValue( "load-policy" ).Get< std::string >(), "IMMEDIATE", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "release-policy") );
    DALI_TEST_EQUALS( value.GetValue( "release-policy" ).Get< std::string >(), "NEVER", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "scaling-mode") );
    DALI_TEST_EQUALS( value.GetValue( "scaling-mode" ).Get< std::string >(), "SHRINK_TO_FIT", TEST_LOCATION );
    DALI_TEST_CHECK( !value.HasKey( "width" ) );
    DALI_TEST_CHECK( !value.HasKey( "height" ) );
  }

  // Change values
  {
    ImageAttributes attributes;
    attributes.SetScalingMode( ImageAttributes::FitWidth );
    attributes.SetSize( 300, 400 );
    Image image = ResourceImage::New( "MY_PATH", attributes, ResourceImage::ON_DEMAND, Image::UNUSED );

    Property::Map map;
    CreatePropertyMap( image, map );
    DALI_TEST_CHECK( !map.Empty() );

    Property::Value value( map );
    DALI_TEST_CHECK( value.HasKey( "type" ) );
    DALI_TEST_EQUALS( value.GetValue( "type" ).Get< std::string >(), "ResourceImage", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "filename" ) );
    DALI_TEST_EQUALS( value.GetValue( "filename" ).Get< std::string >(), "MY_PATH", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "load-policy") );
    DALI_TEST_EQUALS( value.GetValue( "load-policy" ).Get< std::string >(), "ON_DEMAND", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "release-policy") );
    DALI_TEST_EQUALS( value.GetValue( "release-policy" ).Get< std::string >(), "UNUSED", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "scaling-mode") );
    DALI_TEST_EQUALS( value.GetValue( "scaling-mode" ).Get< std::string >(), "FIT_WIDTH", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "width" ) );
    DALI_TEST_EQUALS( value.GetValue( "width" ).Get< int >(), 300, TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "height" ) );
    DALI_TEST_EQUALS( value.GetValue( "height" ).Get< int >(), 400, TEST_LOCATION );
  }

  // BufferImage
  {
    Image image = BufferImage::New( 200, 300, Pixel::A8 );
    Property::Map map;
    CreatePropertyMap( image, map );
    Property::Value value( map );
    DALI_TEST_CHECK( value.HasKey( "type" ) );
    DALI_TEST_EQUALS( value.GetValue( "type" ).Get< std::string >(), "BufferImage", TEST_LOCATION );
    DALI_TEST_CHECK( value.HasKey( "pixel-format") );
    DALI_TEST_EQUALS( value.GetValue( "pixel-format" ).Get< std::string >(), "A8", TEST_LOCATION );
  }

  // FrameBufferImage
  {
    Image image = FrameBufferImage::New( 200, 300, Pixel::RGBA8888 );
    Property::Map map;
    CreatePropertyMap( image, map );
    Property::Value value( map );
    DALI_TEST_CHECK( value.HasKey( "type" ) );
    DALI_TEST_EQUALS( value.GetValue( "type" ).Get< std::string >(), "FrameBufferImage", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliScriptingGetEnumerationTemplates(void)
{
  TestApplication application;

  const Scripting::StringEnum< int > myTable[] =
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
    DALI_TEST_EQUALS( myTable[ i ].value, GetEnumeration( myTable[ i ].string, myTable, myTableCount ), TEST_LOCATION );
  }

  for ( unsigned int i = 0; i < myTableCount; ++i )
  {
    tet_printf("Checking: %d\n", myTable[ i ].value );
    DALI_TEST_EQUALS( myTable[ i ].string, GetEnumerationName( myTable[ i ].value, myTable, myTableCount ), TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliScriptingCompareEnums(void)
{
  // EQUAL
  DALI_TEST_CHECK( CompareEnums( "", "" ) );
  DALI_TEST_CHECK( CompareEnums( "HELLO", "HELLO" ) );
  DALI_TEST_CHECK( CompareEnums( "HELLO", "hello" ) );
  DALI_TEST_CHECK( CompareEnums( "hello", "HELLO" ) );
  DALI_TEST_CHECK( CompareEnums( "hello-world", "HELLO_WORLD" ) );
  DALI_TEST_CHECK( CompareEnums( "hello_WORLD", "HELLO-world" ) );
  DALI_TEST_CHECK( CompareEnums( "hello_WORLD-", "HELLO-world_" ) );
  DALI_TEST_CHECK( CompareEnums( "_hello_WORLD-", "-HELLO-world_" ) );
  DALI_TEST_CHECK( CompareEnums( "-hello_WORLD-", "_HELLO-world_" ) );
  DALI_TEST_CHECK( CompareEnums( "hello123", "HELLO123" ) );

  // NOT EQUAL
  DALI_TEST_CHECK( ! CompareEnums( "hello", "HELLOWORLD" ) );

  END_TEST;
}
