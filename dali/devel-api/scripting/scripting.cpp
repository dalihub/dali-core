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
#include <dali/devel-api/scripting/scripting.h>

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
struct AnchorValue
{
  const char* name;
  const Vector3 value;
};
const AnchorValue ANCHOR_CONSTANT_TABLE[] =
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

const StringEnum COLOR_MODE_TABLE[] =
{
  { "USE_OWN_COLOR",                    USE_OWN_COLOR                    },
  { "USE_PARENT_COLOR",                 USE_PARENT_COLOR                 },
  { "USE_OWN_MULTIPLY_PARENT_COLOR",    USE_OWN_MULTIPLY_PARENT_COLOR    },
  { "USE_OWN_MULTIPLY_PARENT_ALPHA",    USE_OWN_MULTIPLY_PARENT_ALPHA    },
};
const unsigned int COLOR_MODE_TABLE_COUNT = sizeof( COLOR_MODE_TABLE ) / sizeof( COLOR_MODE_TABLE[0] );

const StringEnum POSITION_INHERITANCE_MODE_TABLE[] =
{
  { "INHERIT_PARENT_POSITION",                    INHERIT_PARENT_POSITION                    },
  { "USE_PARENT_POSITION",                        USE_PARENT_POSITION                        },
  { "USE_PARENT_POSITION_PLUS_LOCAL_POSITION",    USE_PARENT_POSITION_PLUS_LOCAL_POSITION    },
  { "DONT_INHERIT_POSITION",                      DONT_INHERIT_POSITION                      },
};
const unsigned int POSITION_INHERITANCE_MODE_TABLE_COUNT = sizeof( POSITION_INHERITANCE_MODE_TABLE ) / sizeof( POSITION_INHERITANCE_MODE_TABLE[0] );

const StringEnum DRAW_MODE_TABLE[] =
{
  { "NORMAL",     DrawMode::NORMAL     },
  { "OVERLAY",    DrawMode::OVERLAY    },
  { "STENCIL",    DrawMode::STENCIL    },
};
const unsigned int DRAW_MODE_TABLE_COUNT = sizeof( DRAW_MODE_TABLE ) / sizeof( DRAW_MODE_TABLE[0] );

const StringEnum IMAGE_LOAD_POLICY_TABLE[] =
{
  { "IMMEDIATE", ResourceImage::IMMEDIATE },
  { "ON_DEMAND", ResourceImage::ON_DEMAND },
};
const unsigned int IMAGE_LOAD_POLICY_TABLE_COUNT = sizeof( IMAGE_LOAD_POLICY_TABLE ) / sizeof( IMAGE_LOAD_POLICY_TABLE[0] );

const StringEnum IMAGE_RELEASE_POLICY_TABLE[] =
{
  { "UNUSED", Image::UNUSED },
  { "NEVER",  Image::NEVER  },
};
const unsigned int IMAGE_RELEASE_POLICY_TABLE_COUNT = sizeof( IMAGE_RELEASE_POLICY_TABLE ) / sizeof( IMAGE_RELEASE_POLICY_TABLE[0] );

const StringEnum PIXEL_FORMAT_TABLE[] =
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

const StringEnum IMAGE_FITTING_MODE_TABLE[] =
{
  { "SHRINK_TO_FIT", FittingMode::SHRINK_TO_FIT },
  { "SCALE_TO_FILL", FittingMode::SCALE_TO_FILL },
  { "FIT_WIDTH",     FittingMode::FIT_WIDTH    },
  { "FIT_HEIGHT",    FittingMode::FIT_HEIGHT   },
};
const unsigned int IMAGE_FITTING_MODE_TABLE_COUNT = sizeof( IMAGE_FITTING_MODE_TABLE ) / sizeof( IMAGE_FITTING_MODE_TABLE[0] );

const StringEnum IMAGE_SAMPLING_MODE_TABLE[] =
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

const char* ImageTypeName[] = { "ResourceImage", "FrameBufferImage", "BufferImage" };
enum ImageType                { RESOURCE_IMAGE,  FRAME_BUFFER_IMAGE, BUFFER_IMAGE };
const unsigned int imageTypeCount = sizeof( ImageTypeName ) / sizeof( const char* );

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

} // unnamed namespace

unsigned int FindEnumIndex( const char* value, const StringEnum* table, unsigned int tableCount )
{
  unsigned int index = 0;
  bool found = false;
  for ( unsigned int i = 0; i < tableCount; ++i, ++index )
  {
    if( CompareEnums( value, table->string ) )
    {
      found = true;
      break;
    }
    ++table;
  }
  if ( !found )
  {
    DALI_LOG_ERROR( "Unknown enumeration string %s\n", value );
  }
  return index;
}

ColorMode GetColorMode( const std::string& value )
{
  // return default on error
  ColorMode mode( USE_OWN_MULTIPLY_PARENT_ALPHA );
  GetEnumeration< ColorMode >( value.c_str(), COLOR_MODE_TABLE, COLOR_MODE_TABLE_COUNT, mode );
  return mode;
}


std::string GetColorMode( ColorMode value )
{
  const char* name = GetEnumerationName< ColorMode >( value, COLOR_MODE_TABLE, COLOR_MODE_TABLE_COUNT );
  if( name )
  {
    return std::string( name );
  }
  return std::string();
}

PositionInheritanceMode GetPositionInheritanceMode( const std::string& value )
{
  // return default on error
  PositionInheritanceMode mode( INHERIT_PARENT_POSITION );
  GetEnumeration< PositionInheritanceMode >( value.c_str(), POSITION_INHERITANCE_MODE_TABLE, POSITION_INHERITANCE_MODE_TABLE_COUNT, mode );
  return mode;
}


std::string GetPositionInheritanceMode( PositionInheritanceMode value )
{
  const char* name = GetEnumerationName< PositionInheritanceMode >( value, POSITION_INHERITANCE_MODE_TABLE, POSITION_INHERITANCE_MODE_TABLE_COUNT );
  if( name )
  {
    return std::string( name );
  }
  return std::string();
}


DrawMode::Type GetDrawMode( const std::string& value )
{
  // return default on error
  DrawMode::Type mode( DrawMode::NORMAL );
  GetEnumeration< DrawMode::Type >( value.c_str(), DRAW_MODE_TABLE, DRAW_MODE_TABLE_COUNT, mode );
  return mode;
}


std::string GetDrawMode( DrawMode::Type value )
{
  const char* name = GetEnumerationName< DrawMode::Type >( value, DRAW_MODE_TABLE, DRAW_MODE_TABLE_COUNT );
  if( name )
  {
    return std::string( name );
  }
  return std::string();
}


Vector3 GetAnchorConstant( const std::string& value )
{
  for( unsigned int i = 0; i < ANCHOR_CONSTANT_TABLE_COUNT; ++i )
  {
    if( CompareEnums( value.c_str(), ANCHOR_CONSTANT_TABLE[ i ].name ) )
    {
      return ANCHOR_CONSTANT_TABLE[ i ].value;
    }
  }
  return Vector3();
}


Image NewImage( const Property::Value& property )
{
  Image ret;

  std::string filename;
  ResourceImage::LoadPolicy loadPolicy = Dali::Internal::IMAGE_LOAD_POLICY_DEFAULT;
  Image::ReleasePolicy releasePolicy   = Dali::Internal::IMAGE_RELEASE_POLICY_DEFAULT;
  Internal::ImageAttributes attributes = Internal::ImageAttributes::New();

  const Property::Map* map = property.GetMap();
  ImageType imageType = RESOURCE_IMAGE; // default to resource image
  if( map )
  {
    // first check the type as it determines, which other parameters are needed
    const Property::Value* value = map->Find( "type" );
    if( value )
    {
      std::string type;
      value->Get( type );
      for( unsigned int i = 0; i < imageTypeCount; ++i )
      {
        if( 0 == type.compare( ImageTypeName[ i ] ) )
        {
          imageType = static_cast<ImageType>( i );
          break;
        }
      }
    }

    // filename is only needed for resource images
    if( RESOURCE_IMAGE == imageType )
    {
      const Property::Value* value = map->Find( "filename" );
      if( value )
      {
        value->Get( filename );
      }
      // if empty file, no need to go further
      if( filename.size() == 0 )
      {
        DALI_LOG_ERROR( "No filename\n" );
        return Image();
      }
    }

    value = map->Find( "load-policy" );
    if( value )
    {
      std::string policy;
      value->Get( policy );
      // keep default value on error
      GetEnumeration< ResourceImage::LoadPolicy >( policy.c_str(), IMAGE_LOAD_POLICY_TABLE, IMAGE_LOAD_POLICY_TABLE_COUNT, loadPolicy );
    }

    value = map->Find( "release-policy" );
    if( value )
    {
      std::string policy;
      value->Get( policy );
      // keep default value on error
      GetEnumeration< Image::ReleasePolicy >( policy.c_str(), IMAGE_RELEASE_POLICY_TABLE, IMAGE_RELEASE_POLICY_TABLE_COUNT, releasePolicy );
    }

    // Width and height can be set individually. Dali derives the unspecified
    // dimension from the aspect ratio of the raw image.
    int width = 0, height = 0;

    value = map->Find( "width" );
    if( value )
    {
      // handle floats and integer the same for json script
      if( value->GetType() == Property::FLOAT )
      {
        width = static_cast<unsigned int>( value->Get<float>() );
      }
      else
      {
        value->Get( width );
      }
    }
    value = map->Find( "height" );
    if( value )
    {
      if( value->GetType() == Property::FLOAT )
      {
        height = static_cast<int>( value->Get<float>() );
      }
      else
      {
        value->Get( height );
      }
    }
    attributes.SetSize( width, height );

    Pixel::Format pixelFormat = Pixel::RGBA8888;
    value = map->Find( "pixel-format" );
    if( value )
    {
      std::string format;
      value->Get( format );
      GetEnumeration< Pixel::Format >( format.c_str(), PIXEL_FORMAT_TABLE, PIXEL_FORMAT_TABLE_COUNT, pixelFormat );
    }

    value = map->Find( "fitting-mode" );
    if( value )
    {
      std::string fitting;
      value->Get( fitting );
      FittingMode::Type mode;
      if( GetEnumeration< FittingMode::Type >( fitting.c_str(), IMAGE_FITTING_MODE_TABLE, IMAGE_FITTING_MODE_TABLE_COUNT, mode ) )
      {
        attributes.SetScalingMode( mode );
      }
    }

    value = map->Find( "sampling-mode" );
    if( value )
    {
      std::string sampling;
      value->Get( sampling );
      SamplingMode::Type mode;
      if( GetEnumeration< SamplingMode::Type >( sampling.c_str(), IMAGE_SAMPLING_MODE_TABLE, IMAGE_SAMPLING_MODE_TABLE_COUNT, mode ) )
      {
        attributes.SetFilterMode( mode );
      }
    }

    value = map->Find( "orientation" );
    if( value )
    {
      bool b = value->Get<bool>();
      attributes.SetOrientationCorrection( b );
    }

    switch( imageType )
    {
      case RESOURCE_IMAGE :
      {
        ret = ResourceImage::New( filename, loadPolicy, releasePolicy,
                                  ImageDimensions( attributes.GetSize().x, attributes.GetSize().y ),
                                  attributes.GetScalingMode(), attributes.GetFilterMode(), attributes.GetOrientationCorrection() );
        break;
      }
      case BUFFER_IMAGE :
      {
        ret = BufferImage::New( attributes.GetWidth(),
                                attributes.GetHeight(),
                                pixelFormat,
                                releasePolicy );
        break;
      }
      case FRAME_BUFFER_IMAGE :
      {
        ret = FrameBufferImage::New( attributes.GetWidth(),
                                     attributes.GetHeight(),
                                     pixelFormat,
                                     releasePolicy );
        break;
      }
    }
  }

  return ret;

} // Image NewImage( Property::Value map )


ShaderEffect NewShaderEffect( const Property::Value& property )
{
  Internal::ShaderEffectPtr ret;

  const Property::Map* map = property.GetMap();
  if( map )
  {
    ret = Internal::ShaderEffect::New( Dali::ShaderEffect::HINT_NONE ); // hint can be reset if in map

    const Property::Value* value = map->Find( "program" );
    if( value )
    {
      Property::Index index = ret->GetPropertyIndex( "program" );
      ret->SetProperty( index, *value );
    }

    for( unsigned int i = 0; i < map->Count(); ++i )
    {
      const std::string& key = map->GetKey( i );
      if( key != "program" )
      {
        Property::Index index = ret->GetPropertyIndex( key );

        const Property::Value& value = map->GetValue( i );
        if( Property::INVALID_INDEX != index )
        {
          ret->SetProperty( index, value );
        }
        else
        {
          // if its not a property then register it as a uniform (making a custom property)
          if( value.GetType() == Property::INTEGER )
          {
            // valid uniforms are floats, vec3's etc so we recast if the user accidentally
            // set as integer. Note the map could have come from json script.
            Property::Value asFloat( static_cast<float>( value.Get<int>() ) );
            ret->SetUniform( key, asFloat, Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
          }
          else
          {
            ret->SetUniform( key, value, Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
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
    DALI_LOG_ERROR( "Actor type not provided\n" );
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
        for ( Property::Array::SizeType i = 0; i < actorArray.Size(); ++i)
        {
          actor.Add( NewActor( actorArray[i].Get< Property::Map >() ) );
        }
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
    const Property::IndexContainer::ConstIterator endIter = indices.End();

    for ( Property::IndexContainer::Iterator iter = indices.Begin(); iter != endIter; ++iter )
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




