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

// HEADER
#include <dali/internal/event/text/atlas/atlas-size.h>

// EXTERNAL INCLUDES
#include <cstddef>

namespace Dali
{

namespace Internal
{

namespace
{
const unsigned int DISTANCE_FIELD_BLOCK_SIZE( 64 ); ///< @todo add to platform abstraction

/**
 * Lookup table used to determine how an atlas should grow
 * in size, as more characters are added
 */
const unsigned int ATLAS_SIZES[] =
{
  //
  // Each character has a  pixel size of 64x64 @ 1 byte per pixel = 4096 bytes ( 4KB)
  // An atlas with 16 characters has an OpenGL Texture size of 16 * 4096 bytes = 64 KB.
  // Atlas are square.
  //
  //  Atlas width ( pixels)         // Number of characters  | Texture Memory | Texture Size | Power of 2 |
  //                                //                       |                |              |
  4 * DISTANCE_FIELD_BLOCK_SIZE,    // 16 (4*4)              | 64 KB          |  256 x 256   | Yes
  8 * DISTANCE_FIELD_BLOCK_SIZE,    // 64 (8*8)              | 256 KB         |  512 x 512   | Yes
  12 * DISTANCE_FIELD_BLOCK_SIZE,   // 144                   | 576 KB         |  768 x 768   | ---
  16 * DISTANCE_FIELD_BLOCK_SIZE,   // 256                   | 1 MB           | 1024 x 1024  | Yes
  20 * DISTANCE_FIELD_BLOCK_SIZE,   // 400                   | 1.56 MB        | 1280 x 1280  | ---
  24 * DISTANCE_FIELD_BLOCK_SIZE,   // 576                   | 2.25 MB        | 1536 x 1536  | ---
  28 * DISTANCE_FIELD_BLOCK_SIZE,   // 784                   | 3.06 MB        | 1792 x 1792  | ---
  32 * DISTANCE_FIELD_BLOCK_SIZE,   // 1024                  | 4 MB           | 2048 x 2048  | Yes
  48 * DISTANCE_FIELD_BLOCK_SIZE,   // 2304                  | 9 MB           | 3072 x 3072  | ---
  64 * DISTANCE_FIELD_BLOCK_SIZE,   // 4096                  | 16 MB          | 4096 x 4096  | Yes
};


const std::size_t ATLAS_SIZE_COUNT = (sizeof( ATLAS_SIZES))/ (sizeof(unsigned int)) ;

}  // un-named name space

unsigned int GlyphAtlasSize::GetAtlasCharacterCount( unsigned int size )
{
  // if atlas size = 256.
  // Then character count is ( 256 / block )= 4. Squared = 4 * 4 = 16 characters
  unsigned int oneSide= (size / DISTANCE_FIELD_BLOCK_SIZE );
  return oneSide * oneSide;
}

unsigned int GlyphAtlasSize::GetInitialSize( unsigned int initialiCharacterCount)
{
  for( std::size_t i = 0; i <  ATLAS_SIZE_COUNT; ++i)
  {
    unsigned int atlasCharCount = GetAtlasCharacterCount( ATLAS_SIZES[i] );

    if( initialiCharacterCount <= atlasCharCount )
     {
       return ATLAS_SIZES[i];
     }
  }
  return GetMaxSize();
}

unsigned int GlyphAtlasSize::GetNextSize( unsigned int currentSize )
{
  for( std::size_t i = 0; i <  ATLAS_SIZE_COUNT; ++i)
  {
    if( currentSize < ATLAS_SIZES[i] )
    {
      return ATLAS_SIZES[i];
    }
  }
  return GetMaxSize();
}

unsigned int GlyphAtlasSize::GetMaxSize()
{
  return ATLAS_SIZES[ ATLAS_SIZE_COUNT -1];
}

unsigned int GlyphAtlasSize::GetBlockSize()
{
  return DISTANCE_FIELD_BLOCK_SIZE;
}

} // namespace Internal

} // namespace Dali
