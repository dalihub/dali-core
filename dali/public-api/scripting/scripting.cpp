/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/public-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/property-array.h>
#include <dali/internal/common/image-attributes.h>
#include <dali/internal/event/images/resource-image-impl.h>
#include <dali/internal/event/images/frame-buffer-image-impl.h>
#include <dali/internal/event/images/buffer-image-impl.h>
#include <dali/internal/event/effects/shader-effect-impl.h>

namespace Dali
{

namespace Scripting
{

namespace
{

// Tables used here for converting strings to the enumerations and vice versa

const StringEnum< ColorMode > COLOR_MODE_TABLE[] =
{
  { "USE_OWN_COLOR",                    USE_OWN_COLOR                    },
  { "USE_PARENT_COLOR",                 USE_PARENT_COLOR                 },
  { "USE_OWN_MULTIPLY_PARENT_COLOR",    USE_OWN_MULTIPLY_PARENT_COLOR    },
  { "USE_OWN_MULTIPLY_PARENT_ALPHA",    USE_OWN_MULTIPLY_PARENT_ALPHA    },
};
const unsigned int COLOR_MODE_TABLE_COUNT = sizeof( COLOR_MODE_TABLE ) / sizeof( COLOR_MODE_TABLE[0] );

const StringEnum< PositionInheritanceMode > POSITION_INHERITANCE_MODE_TABLE[] =
{
  { "INHERIT_PARENT_POSITION",                    INHERIT_PARENT_POSITION                    },
  { "USE_PARENT_POSITION",                        USE_PARENT_POSITION                        },
  { "USE_PARENT_POSITION_PLUS_LOCAL_POSITION",    USE_PARENT_POSITION_PLUS_LOCAL_POSITION    },
  { "DONT_INHERIT_POSITION",                      DONT_INHERIT_POSITION                      },
};
const unsigned int POSITION_INHERITANCE_MODE_TABLE_COUNT = sizeof( POSITION_INHERITANCE_MODE_TABLE ) / sizeof( POSITION_INHERITANCE_MODE_TABLE[0] );

const StringEnum< DrawMode::Type > DRAW_MODE_TABLE[] =
{
  { "NORMAL",     DrawMode::NORMAL     },
  { "OVERLAY",    DrawMode::OVERLAY    },
  { "STENCIL",    DrawMode::STENCIL    },
};
const unsigned int DRAW_MODE_TABLE_COUNT = sizeof( DRAW_MODE_TABLE ) / sizeof( DRAW_MODE_TABLE[0] );

const StringEnum< Vector3 > ANCHOR_CONSTANT_TABLE[] =
{
  { "TOP_LEFT",               ParentOrigin::TOP_LEFT               },
  { "TOP_CENTER",             ParentOrigin::TOP_CENTER             },
  { "TOP_RIGHT",              ParentOrigin::TOP_RIGHT              },
  { "CENTER_LEFT",            ParentOrigin::CENTER_LEFT            },
  { "CENTER",                 ParentOrigin::CENTER                 },
  { "CENTER_RIGHT",           ParentOrigin::CENTER_RIGHT           },
  { "BOTTOM_LEFT",            ParentOrigin::BOTTOM_LEFT            },
  { "BOTTOM_CENTER",          ParentOrigin::BOTTOM_CENTER          },
  { "BOTTOM_RIGHT",           ParentOrigin::BOTTOM_RIGHT           },
};
const unsigned int ANCHOR_CONSTANT_TABLE_COUNT = sizeof( ANCHOR_CONSTANT_TABLE ) / sizeof( ANCHOR_CONSTANT_TABLE[0] );

const StringEnum< ResourceImage::LoadPolicy > IMAGE_LOAD_POLICY_TABLE[] =
{
  { "IMMEDIATE", ResourceImage::IMMEDIATE },
  { "ON_DEMAND", ResourceImage::ON_DEMAND },
};
const unsigned int IMAGE_LOAD_POLICY_TABLE_COUNT = sizeof( IMAGE_LOAD_POLICY_TABLE ) / sizeof( IMAGE_LOAD_POLICY_TABLE[0] );

const StringEnum< Image::ReleasePolicy > IMAGE_RELEASE_POLICY_TABLE[] =
{
  { "UNUSED", Image::UNUSED },
  { "NEVER",  Image::NEVER  },
};
const unsigned int IMAGE_RELEASE_POLICY_TABLE_COUNT = sizeof( IMAGE_RELEASE_POLICY_TABLE ) / sizeof( IMAGE_RELEASE_POLICY_TABLE[0] );

const StringEnum< Pixel::Format > PIXEL_FORMAT_TABLE[] =
{
  { "A8",                                           Pixel::A8                                           },
  { "L8",                                           Pixel::L8                                           },
  { "LA88",                                         Pixel::LA88                                         },
  { "RGB565",                                       Pixel::RGB565                                       },
  { "BGR565",                                       Pixel::BGR565                                       },
  { "RGBA4444",                                     Pixel::RGBA4444                                     },
  { "BGRA4444",                                     Pixel::BGRA4444                                     },
  { "RGBA5551",                                     Pixel::RGBA5551                                     },
  { "BGRA5551",                                     Pixel::BGRA5551                                     },
  { "RGB888",                                       Pixel::RGB888                                       },
  { "RGB8888",                                      Pixel::RGB8888                                      },
  { "BGR8888",                                      Pixel::BGR8888                                      },
  { "RGBA8888",                                     Pixel::RGBA8888                                     },
  { "BGRA8888",                                     Pixel::BGRA8888                                     },
  { "COMPRESSED_R11_EAC",                           Pixel::COMPRESSED_R11_EAC                           },
  { "COMPRESSED_SIGNED_R11_EAC",                    Pixel::COMPRESSED_SIGNED_R11_EAC                    },
  { "COMPRESSED_SIGNED_RG11_EAC",                   Pixel::COMPRESSED_SIGNED_RG11_EAC                   },
  { "COMPRESSED_RG11_EAC",                          Pixel::COMPRESSED_RG11_EAC                          },
  { "COMPRESSED_RGB8_ETC2",                         Pixel::COMPRESSED_RGB8_ETC2                         },
  { "COMPRESSED_SRGB8_ETC2",                        Pixel::COMPRESSED_SRGB8_ETC2                        },
  { "COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2",     Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2     },
  { "COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2",    Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2    },
  { "COMPRESSED_RGBA8_ETC2_EAC",                    Pixel::COMPRESSED_RGBA8_ETC2_EAC                    },
  { "COMPRESSED_SRGB8_ALPHA8_ETC2_EAC",             Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC             },
  { "COMPRESSED_RGB8_ETC1",                         Pixel::COMPRESSED_RGB8_ETC1                         },
  { "COMPRESSED_RGB_PVRTC_4BPPV1",                  Pixel::COMPRESSED_RGB_PVRTC_4BPPV1                  },
};
const unsigned int PIXEL_FORMAT_TABLE_COUNT = sizeof( PIXEL_FORMAT_TABLE ) / sizeof( PIXEL_FORMAT_TABLE[0] );

const StringEnum< FittingMode::Type > IMAGE_FITTING_MODE_TABLE[] =
{
  { "SHRINK_TO_FIT", FittingMode::SHRINK_TO_FIT },
  { "SCALE_TO_FILL", FittingMode::SCALE_TO_FILL },
  { "FIT_WIDTH",     FittingMode::FIT_WIDTH    },
  { "FIT_HEIGHT",    FittingMode::FIT_HEIGHT   },
};
const unsigned int IMAGE_FITTING_MODE_TABLE_COUNT = sizeof( IMAGE_FITTING_MODE_TABLE ) / sizeof( IMAGE_FITTING_MODE_TABLE[0] );

const StringEnum< SamplingMode::Type > IMAGE_SAMPLING_MODE_TABLE[] =
{
  { "BOX",              SamplingMode::BOX            },
  { "NEAREST",          SamplingMode::NEAREST        },
  { "LINEAR",           SamplingMode::LINEAR         },
  { "BOX_THEN_NEAREST", SamplingMode::BOX_THEN_NEAREST },
  { "BOX_THEN_LINEAR",  SamplingMode::BOX_THEN_LINEAR  },
  { "NO_FILTER",        SamplingMode::NO_FILTER       },
  { "DONT_CARE",        SamplingMode::DONT_CARE       },
};
const unsigned int IMAGE_SAMPLING_MODE_TABLE_COUNT = sizeof( IMAGE_SAMPLING_MODE_TABLE ) / sizeof( IMAGE_SAMPLING_MODE_TABLE[0] );

} // unnamed namespace

bool CompareEnums( const char * a, const char * b )
{
  while( ( *a != '\0' ) && ( *b != '\0' ) )
  {
    char ca = *a;
    char cb = *b;

    if( ( ( ca == '-' ) || ( ca == '_') ) &&
        ( ( cb == '-' ) || ( cb == '_') ) )
    {
      ++a;
      ++b;
      continue;
    }

    if( ( 'A' <= ca ) && ( ca <= 'Z') )
    {
      ca = ca + ( 'a' - 'A' );
    }

    if( ( 'A' <= cb ) && ( cb <= 'Z') )
    {
      cb = cb + ( 'a' - 'A' );
    }

    if( ca != cb )
    {
      return false;
    }

    ++a;
    ++b;
  }

  if( ( *a == '\0' ) && ( *b == '\0' ) )
  {
    return true;
  }

  return false;
}


ColorMode GetColorMode( const std::string& value )
{
  return GetEnumeration< ColorMode >( value.c_str(), COLOR_MODE_TABLE, COLOR_MODE_TABLE_COUNT );
}


std::string GetColorMode( ColorMode value )
{
  return GetEnumerationName< ColorMode >( value, COLOR_MODE_TABLE, COLOR_MODE_TABLE_COUNT );
}

PositionInheritanceMode GetPositionInheritanceMode( const std::string& value )
{
  return GetEnumeration< PositionInheritanceMode >( value.c_str(), POSITION_INHERITANCE_MODE_TABLE, POSITION_INHERITANCE_MODE_TABLE_COUNT );
}


std::string GetPositionInheritanceMode( PositionInheritanceMode value )
{
  return GetEnumerationName< PositionInheritanceMode >( value, POSITION_INHERITANCE_MODE_TABLE, POSITION_INHERITANCE_MODE_TABLE_COUNT );
}


DrawMode::Type GetDrawMode( const std::string& value )
{
  return GetEnumeration< DrawMode::Type >( value.c_str(), DRAW_MODE_TABLE, DRAW_MODE_TABLE_COUNT );
}


std::string GetDrawMode( DrawMode::Type value )
{
  return GetEnumerationName< DrawMode::Type >( value, DRAW_MODE_TABLE, DRAW_MODE_TABLE_COUNT );
}


Vector3 GetAnchorConstant( const std::string& value )
{
  return GetEnumeration< Vector3 >( value.c_str(), ANCHOR_CONSTANT_TABLE, ANCHOR_CONSTANT_TABLE_COUNT );
}


Image NewImage( const Property::Value& map )
{
  Image ret;

  std::string filename;
  ResourceImage::LoadPolicy loadPolicy    = Dali::Internal::IMAGE_LOAD_POLICY_DEFAULT;
  Image::ReleasePolicy releasePolicy = Dali::Internal::IMAGE_RELEASE_POLICY_DEFAULT;
  Internal::ImageAttributes attributes = Internal::ImageAttributes::New();

  if( Property::MAP == map.GetType() )
  {
    std::string field = "filename";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image filename property is not a string" );
      filename = map.GetValue(field).Get<std::string>();
    }

    field = "load-policy";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image load-policy property is not a string" );
      std::string v(map.GetValue(field).Get<std::string>());
      loadPolicy = GetEnumeration< ResourceImage::LoadPolicy >( v.c_str(), IMAGE_LOAD_POLICY_TABLE, IMAGE_LOAD_POLICY_TABLE_COUNT );
    }

    field = "release-policy";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image release-policy property is not a string" );
      std::string v(map.GetValue(field).Get<std::string>());
      releasePolicy = GetEnumeration< Image::ReleasePolicy >( v.c_str(), IMAGE_RELEASE_POLICY_TABLE, IMAGE_RELEASE_POLICY_TABLE_COUNT );
    }

    // Width and height can be set individually. Dali derives the unspecified
    // dimension from the aspect ratio of the raw image.
    unsigned int width = 0, height = 0;

    field = "width";
    if( map.HasKey( field ) )
    {
      Property::Value &value = map.GetValue( field );

      // handle floats and integer the same for json script
      if( value.GetType() == Property::FLOAT )
      {
        width = static_cast<unsigned int>( value.Get<float>() );
      }
      else
      {
        DALI_ASSERT_ALWAYS( value.GetType() == Property::INTEGER && "Image width property is not a number" );
        width = value.Get<int>();
      }
    }

    field = "height";
    if( map.HasKey( field ) )
    {
      Property::Value &value = map.GetValue( field );
      if( value.GetType() == Property::FLOAT )
      {
        height = static_cast<unsigned int>( value.Get<float>() );
      }
      else
      {
        DALI_ASSERT_ALWAYS( value.GetType() == Property::INTEGER && "Image width property is not a number" );
        height = value.Get<int>();
      }
    }

    attributes.SetSize( width, height );

    field = "pixel-format";
    Pixel::Format pixelFormat = Pixel::RGBA8888;
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS( map.GetValue(field).GetType() == Property::STRING && "Image release-policy property is not a string" );
      std::string s( map.GetValue(field).Get<std::string>() );
      pixelFormat = GetEnumeration< Pixel::Format >( s.c_str(), PIXEL_FORMAT_TABLE, PIXEL_FORMAT_TABLE_COUNT );
    }

    field = "fitting-mode";
    if( map.HasKey( field ) )
    {
      Property::Value& value = map.GetValue( field );
      DALI_ASSERT_ALWAYS( value.GetType() == Property::STRING && "Image fitting-mode property is not a string" );
      std::string s( value.Get<std::string>() );
      attributes.SetScalingMode( GetEnumeration< FittingMode::Type >( s.c_str(), IMAGE_FITTING_MODE_TABLE, IMAGE_FITTING_MODE_TABLE_COUNT ) );
    }

    field = "sampling-mode";
    if( map.HasKey( field ) )
    {
      Property::Value& value = map.GetValue( field );
      DALI_ASSERT_ALWAYS( value.GetType() == Property::STRING && "Image sampling-mode property is not a string" );
      std::string s( value.Get<std::string>() );
      attributes.SetFilterMode( GetEnumeration< SamplingMode::Type >( s.c_str(), IMAGE_SAMPLING_MODE_TABLE, IMAGE_SAMPLING_MODE_TABLE_COUNT ) );
    }

    field = "orientation";
    if( map.HasKey( field ) )
    {
      Property::Value& value = map.GetValue( field );
      DALI_ASSERT_ALWAYS( value.GetType() == Property::BOOLEAN && "Image orientation property is not a boolean" );
      bool b = value.Get<bool>();
      attributes.SetOrientationCorrection( b );
    }

    if( map.HasKey("type") )
    {
      DALI_ASSERT_ALWAYS( map.GetValue("type").GetType() == Property::STRING );
      std::string s(map.GetValue("type").Get<std::string>());
      if("FrameBufferImage" == s)
      {
        ret = FrameBufferImage::New(attributes.GetWidth(),
                                    attributes.GetHeight(),
                                    pixelFormat,
                                    releasePolicy);
      }
      else if("BufferImage" == s)
      {
        ret = BufferImage::New(attributes.GetWidth(),
                               attributes.GetHeight(),
                               pixelFormat,
                               releasePolicy);
      }
      else if("ResourceImage" == s)
      {
        ret = ResourceImage::New( filename, loadPolicy, releasePolicy, ImageDimensions( attributes.GetSize().x, attributes.GetSize().y ), attributes.GetScalingMode(), attributes.GetFilterMode(), attributes.GetOrientationCorrection() );
      }
      else
      {
        DALI_ASSERT_ALWAYS( !"Unknown image type" );
      }
    }
    else
    {
      ret = ResourceImage::New( filename, loadPolicy, releasePolicy, ImageDimensions( attributes.GetSize().x, attributes.GetSize().y ), attributes.GetScalingMode(), attributes.GetFilterMode(), attributes.GetOrientationCorrection() );
    }
  }

  return ret;

} // Image NewImage( Property::Value map )


ShaderEffect NewShaderEffect( const Property::Value& map )
{
  Internal::ShaderEffectPtr ret;

  if( map.GetType() == Property::MAP )
  {
    ret = Internal::ShaderEffect::New(Dali::ShaderEffect::HINT_NONE); // hint can be reset if in map

    if( map.HasKey("program") )
    {
      Property::Index index = ret->GetPropertyIndex("program");
      DALI_ASSERT_DEBUG( map.GetValue("program").GetType() == Property::MAP );
      ret->SetProperty(index, map.GetValue("program"));
    }

    for(int i = 0; i < map.GetSize(); ++i)
    {
      const std::string& key = map.GetKey(i);
      if(key != "program")
      {
        Property::Index index = ret->GetPropertyIndex( key );

        if( Property::INVALID_INDEX != index )
        {
          ret->SetProperty(index, map.GetItem(i));
        }
        else
        {
          // if its not a property then register it as a uniform (making a custom property)

          if(map.GetItem(i).GetType() == Property::INTEGER)
          {
            // valid uniforms are floats, vec3's etc so we recast if the user accidentally
            // set as integer. Note the map could have come from json script.
            Property::Value asFloat( static_cast<float>( map.GetItem(i).Get<int>() ) );
            ret->SetUniform( key, asFloat, Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
          }
          else
          {
            ret->SetUniform( key, map.GetItem(i), Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
          }
        }
      }
    }
  }

  return Dali::ShaderEffect(ret.Get());
}


Actor NewActor( const Property::Map& map )
{
  BaseHandle handle;

  // First find type and create Actor
  Property::Value* typeValue = map.Find( "type" );
  if ( typeValue )
  {
    TypeInfo type = TypeRegistry::Get().GetTypeInfo( typeValue->Get< std::string >() );
    if ( type )
    {
      handle = type.CreateInstance();
    }
  }

  if ( !handle )
  {
    DALI_LOG_ERROR( "Actor type not provided, returning empty handle" );
    return Actor();
  }

  Actor actor( Actor::DownCast( handle ) );

  if ( actor )
  {
    // Now set the properties, or create children
    for ( unsigned int i = 0, mapCount = map.Count(); i < mapCount; ++i )
    {
      const StringValuePair& pair( map.GetPair( i ) );
      const std::string& key( pair.first );
      if ( key == "type" )
      {
        continue;
      }

      const Property::Value& value( pair.second );

      if ( key == "actors" )
      {
        // Create children

        Property::Array actorArray = value.Get< Property::Array >();
        for ( size_t i = 0; i < actorArray.Size(); ++i)
        {
          actor.Add( NewActor( actorArray[i].Get< Property::Map >() ) );
        }
      }
      else if ( key == "signals" )
      {
        DALI_LOG_ERROR( "signals not supported" );
      }
      else if( key ==  "parent-origin" )
      {
        // Parent Origin can be a string constant as well as a Vector3

        const Property::Type type( value.GetType() );
        if ( type == Property::VECTOR3 )
        {
          actor.SetParentOrigin( value.Get< Vector3 >() );
        }
        else if( type == Property::STRING )
        {
          actor.SetParentOrigin( GetAnchorConstant( value.Get< std::string >() ) );
        }
      }
      else if( key ==  "anchor-point" )
      {
        // Anchor Point can be a string constant as well as a Vector3

        const Property::Type type( value.GetType() );
        if ( type == Property::VECTOR3 )
        {
          actor.SetAnchorPoint( value.Get< Vector3 >() );
        }
        else if( type == Property::STRING )
        {
          actor.SetAnchorPoint( GetAnchorConstant( value.Get< std::string >() ) );
        }
      }
      else
      {
        Property::Index index( actor.GetPropertyIndex( key ) );

        if ( index != Property::INVALID_INDEX )
        {
          actor.SetProperty( index, value );
        }
      }
    }
  }

  return actor;
}

void CreatePropertyMap( Actor actor, Property::Map& map )
{
  map.Clear();

  if ( actor )
  {
    map[ "type" ] = actor.GetTypeName();

    // Default properties
    Property::IndexContainer indices;
    actor.GetPropertyIndices( indices );
    const Property::IndexContainer::const_iterator endIter = indices.end();
    for ( Property::IndexContainer::iterator iter = indices.begin(); iter != endIter; ++iter )
    {
      map[ actor.GetPropertyName( *iter ) ] = actor.GetProperty( *iter );
    }

    // Children
    unsigned int childCount( actor.GetChildCount() );
    if ( childCount )
    {
      Property::Array childArray;
      for ( unsigned int child = 0; child < childCount; ++child )
      {
        Property::Map childMap;
        CreatePropertyMap( actor.GetChildAt( child ), childMap );
        childArray.PushBack( childMap );
      }
      map[ "actors" ] = childArray;
    }
  }
}

void CreatePropertyMap( Image image, Property::Map& map )
{
  map.Clear();

  if ( image )
  {
    std::string imageType( "ResourceImage" );

    // Get Type - cannot use TypeRegistry as Image is not an Object and thus, not registered
    BufferImage bufferImage = BufferImage::DownCast( image );
    if ( bufferImage )
    {
      imageType = "BufferImage";
      map[ "pixel-format" ] = GetEnumerationName< Pixel::Format >( bufferImage.GetPixelFormat(), PIXEL_FORMAT_TABLE, PIXEL_FORMAT_TABLE_COUNT );
    }
    else if ( FrameBufferImage::DownCast( image ) )
    {
      imageType = "FrameBufferImage";
    }

    map[ "type" ] = imageType;
    map[ "release-policy" ] = GetEnumerationName< Image::ReleasePolicy >( image.GetReleasePolicy(), IMAGE_RELEASE_POLICY_TABLE, IMAGE_RELEASE_POLICY_TABLE_COUNT );

    ResourceImage resourceImage = ResourceImage::DownCast( image );
    if( resourceImage )
    {
      map[ "filename" ] = resourceImage.GetUrl();
      map[ "load-policy" ] = GetEnumerationName< ResourceImage::LoadPolicy >( resourceImage.GetLoadPolicy(), IMAGE_LOAD_POLICY_TABLE, IMAGE_LOAD_POLICY_TABLE_COUNT );
    }

    int width( image.GetWidth() );
    int height( image.GetHeight() );

    if ( width && height )
    {
      map[ "width" ] = width;
      map[ "height" ] = height;
    }
  }
}

} // namespace scripting

} // namespace Dali




