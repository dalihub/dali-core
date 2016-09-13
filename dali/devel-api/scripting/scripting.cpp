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

// CLASS HEADER
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/property-array.h>
#include <dali/internal/common/image-attributes.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/images/resource-image-impl.h>
#include <dali/internal/event/images/frame-buffer-image-impl.h>
#include <dali/internal/event/images/buffer-image-impl.h>

namespace Dali
{

namespace Scripting
{

namespace
{

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

} // unnamed namespace

bool EnumStringToInteger( const char * const value, const StringEnum* const enumTable, unsigned int tableCount, int& integerEnum )
{
  int ret = 0;

  bool found = false;
  bool done = false;

  if( value && enumTable && tableCount )
  {
    const char* pValue = value;

    while(!done)
    {
      size_t size = 0;

      const StringEnum* table = enumTable;

      for ( unsigned int i = 0; i < tableCount; ++i )
      {
        if( Internal::CompareTokens( pValue, table->string, size ) )
        {
          found = true;
          ret |= table->value;
          break;
        }
        table++;
      }

      done = true;

      if(found)
      {
        // allow comma separated or'd value
        if( *(pValue+size) == ',' )
        {
          pValue += size + 1;
          done = false;
        }
      }

    }

    integerEnum = ret;
  }

  if ( !found )
  {
    DALI_LOG_ERROR( "Unknown enumeration string %s\n", value );
  }
  return found;
}

unsigned int FindEnumIndex( const char* value, const StringEnum* table, unsigned int tableCount )
{
  unsigned int index = 0;
  bool found = false;
  for ( unsigned int i = 0; i < tableCount; ++i, ++index )
  {
    size_t sizeIgnored = 0;
    if( Internal::CompareTokens( value, table->string, sizeIgnored ) )
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


Image NewImage( const Property::Value& property )
{
  Image ret;

  std::string filename;
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
    value = map->Find( "pixelFormat" );
    if( value )
    {
      std::string format;
      value->Get( format );
      GetEnumeration< Pixel::Format >( format.c_str(), PIXEL_FORMAT_TABLE, PIXEL_FORMAT_TABLE_COUNT, pixelFormat );
    }

    value = map->Find( "fittingMode" );
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

    value = map->Find( "samplingMode" );
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
        ret = ResourceImage::New( filename, ImageDimensions( attributes.GetSize().x, attributes.GetSize().y ),
                                  attributes.GetScalingMode(), attributes.GetFilterMode(), attributes.GetOrientationCorrection() );
        break;
      }
      case BUFFER_IMAGE :
      {
        ret = BufferImage::New( attributes.GetWidth(),
                                attributes.GetHeight(),
                                pixelFormat );
        break;
      }
      case FRAME_BUFFER_IMAGE :
      {
        ret = FrameBufferImage::New( attributes.GetWidth(),
                                     attributes.GetHeight(),
                                     pixelFormat );
        break;
      }
    }
  }

  return ret;

} // Image NewImage( Property::Value map )


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
      const KeyValuePair pair( map.GetKeyValue( i ) );
      if( pair.first.type == Property::Key::INDEX )
      {
        continue;
      }
      const std::string& key( pair.first.stringKey );
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
      map[ "pixelFormat" ] = GetEnumerationName< Pixel::Format >( bufferImage.GetPixelFormat(), PIXEL_FORMAT_TABLE, PIXEL_FORMAT_TABLE_COUNT );
    }
    else if ( FrameBufferImage::DownCast( image ) )
    {
      imageType = "FrameBufferImage";
    }

    map[ "type" ] = imageType;

    ResourceImage resourceImage = ResourceImage::DownCast( image );
    if( resourceImage )
    {
      map[ "filename" ] = resourceImage.GetUrl();
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

void NewAnimation( const Property::Map& map, Dali::AnimationData& outputAnimationData )
{
  // Note: Builder cannot currently pass generic Property::Maps "{" that are nested, so currently we can only have one AnimateTo per animation.
  Dali::AnimationData::AnimationDataElement* element = new Dali::AnimationData::AnimationDataElement();
  element->alphaFunction = AlphaFunction::LINEAR;
  element->timePeriodDelay = 0.0f;
  element->timePeriodDuration = 1.0f;

  // Now set the properties, or create children
  for( unsigned int i = 0, animationMapCount = map.Count(); i < animationMapCount; ++i )
  {
    const KeyValuePair pair( map.GetKeyValue( i ) );
    if( pair.first.type == Property::Key::INDEX )
    {
      continue; // We don't consider index keys.
    }
    const std::string& key( pair.first.stringKey );

    const Property::Value& value( pair.second );

    if( key == "actor" )
    {
      element->actor = value.Get< std::string >();
    }
    else if( key == "property" )
    {
      element->property = value.Get< std::string >();
    }
    else if( key == "value" )
    {
      element->value = value;
    }
    else if( key == "alphaFunction" )
    {
      std::string alphaFunctionValue = value.Get< std::string >();

      if( alphaFunctionValue == "LINEAR" )
      {
        element->alphaFunction = AlphaFunction::LINEAR;
      }
      else if( alphaFunctionValue == "REVERSE" )
      {
        element->alphaFunction = AlphaFunction::REVERSE;
      }
      else if( alphaFunctionValue == "EASE_IN_SQUARE" )
      {
        element->alphaFunction = AlphaFunction::EASE_IN_SQUARE;
      }
      else if( alphaFunctionValue == "EASE_OUT_SQUARE" )
      {
        element->alphaFunction = AlphaFunction::EASE_OUT_SQUARE;
      }
      else if( alphaFunctionValue == "EASE_IN" )
      {
        element->alphaFunction = AlphaFunction::EASE_IN;
      }
      else if( alphaFunctionValue == "EASE_OUT" )
      {
        element->alphaFunction = AlphaFunction::EASE_OUT;
      }
      else if( alphaFunctionValue == "EASE_IN_OUT" )
      {
        element->alphaFunction = AlphaFunction::EASE_IN_OUT;
      }
      else if( alphaFunctionValue == "EASE_IN_SINE" )
      {
        element->alphaFunction = AlphaFunction::EASE_IN_SINE;
      }
      else if( alphaFunctionValue == "EASE_OUT_SINE" )
      {
        element->alphaFunction = AlphaFunction::EASE_OUT_SINE;
      }
      else if( alphaFunctionValue == "EASE_IN_OUT_SINE" )
      {
        element->alphaFunction = AlphaFunction::EASE_IN_OUT_SINE;
      }
      else if( alphaFunctionValue == "BOUNCE" )
      {
        element->alphaFunction = AlphaFunction::BOUNCE;
      }
      else if( alphaFunctionValue == "SIN" )
      {
        element->alphaFunction = AlphaFunction::SIN;
      }
      else if( alphaFunctionValue == "EASE_OUT_BACK" )
      {
        element->alphaFunction = AlphaFunction::EASE_OUT_BACK;
      }
    }
    else if( key == "timePeriod" )
    {
      Property::Map timeMap = value.Get< Property::Map >();
      for( unsigned int i = 0; i < timeMap.Count(); ++i )
      {
        const KeyValuePair pair( timeMap.GetKeyValue( i ) );
        if( pair.first.type == Property::Key::INDEX )
        {
          continue;
        }
        const std::string& key( pair.first.stringKey );

        if( key == "delay" )
        {
          element->timePeriodDelay = pair.second.Get< float >();
        }
        else if( key == "duration" )
        {
          element->timePeriodDuration = pair.second.Get< float >();
        }
      }
    }
  }

  outputAnimationData.Add( element );
}

} // namespace scripting

} // namespace Dali
