/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
const uint32_t PIXEL_FORMAT_TABLE_COUNT = static_cast<uint32_t>( sizeof( PIXEL_FORMAT_TABLE ) / sizeof( PIXEL_FORMAT_TABLE[0] ) );

const StringEnum IMAGE_FITTING_MODE_TABLE[] =
{
  { "SHRINK_TO_FIT", FittingMode::SHRINK_TO_FIT },
  { "SCALE_TO_FILL", FittingMode::SCALE_TO_FILL },
  { "FIT_WIDTH",     FittingMode::FIT_WIDTH    },
  { "FIT_HEIGHT",    FittingMode::FIT_HEIGHT   },
};
const uint32_t IMAGE_FITTING_MODE_TABLE_COUNT = static_cast<uint32_t>( sizeof( IMAGE_FITTING_MODE_TABLE ) / sizeof( IMAGE_FITTING_MODE_TABLE[0] ) );

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
const uint32_t IMAGE_SAMPLING_MODE_TABLE_COUNT = static_cast<uint32_t>( sizeof( IMAGE_SAMPLING_MODE_TABLE ) / sizeof( IMAGE_SAMPLING_MODE_TABLE[0] ) );

const char* ImageTypeName[] = { "ResourceImage", "FrameBufferImage", "BufferImage" };
enum ImageType                { RESOURCE_IMAGE,  FRAME_BUFFER_IMAGE, BUFFER_IMAGE };
const uint32_t imageTypeCount = static_cast<uint32_t>( sizeof( ImageTypeName ) / sizeof( const char* ) );

} // unnamed namespace

bool EnumStringToInteger( const char * const value, const StringEnum* const enumTable, uint32_t tableCount, int& integerEnum )
{
  int ret = 0;

  bool found = false;
  bool done = false;

  if( value && enumTable && tableCount )
  {
    const char* pValue = value;

    while(!done)
    {
      uint32_t size = 0;

      const StringEnum* table = enumTable;

      for ( uint32_t i = 0; i < tableCount; ++i )
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

uint32_t FindEnumIndex( const char* value, const StringEnum* table, uint32_t tableCount )
{
  uint32_t index = 0;
  bool found = false;
  for ( uint32_t i = 0; i < tableCount; ++i, ++index )
  {
    uint32_t sizeIgnored = 0;
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
    for ( Property::Map::SizeType i = 0, mapCount = map.Count(); i < mapCount; ++i )
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
    std::size_t childCount( actor.GetChildCount() );
    if ( childCount )
    {
      Property::Array childArray;
      for ( uint32_t child = 0; child < childCount; ++child )
      {
        Property::Map childMap;
        CreatePropertyMap( actor.GetChildAt( child ), childMap );
        childArray.PushBack( childMap );
      }
      map[ "actors" ] = childArray;
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
  for( Property::Map::SizeType i = 0, animationMapCount = map.Count(); i < animationMapCount; ++i )
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
      for( Property::Map::SizeType i = 0; i < timeMap.Count(); ++i )
      {
        const KeyValuePair timePair( timeMap.GetKeyValue( i ) );
        if( timePair.first.type == Property::Key::INDEX )
        {
          continue;
        }
        const std::string& key( timePair.first.stringKey );

        if( key == "delay" )
        {
          element->timePeriodDelay = timePair.second.Get< float >();
        }
        else if( key == "duration" )
        {
          element->timePeriodDuration = timePair.second.Get< float >();
        }
      }
    }
  }

  outputAnimationData.Add( element );
}

} // namespace scripting

} // namespace Dali
