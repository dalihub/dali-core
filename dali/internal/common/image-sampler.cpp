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

// CLASS HEADER
#include <dali/internal/common/image-sampler.h>

#include <iostream>

namespace Dali
{

namespace Internal
{

namespace ImageSampler
{

namespace
{

// Adjust these shift sizes if the FilterMode enum grows
const int MINIFY_BIT_SHIFT  = 0;    // Room for 16
const int MAGNIFY_BIT_SHIFT = 4;

const int MASK_MINIFY_FILTER    = 0x0000000F;
const int MASK_MAGNIFY_FILTER   = 0x000000F0;

const unsigned int FILTER_MODE_COUNT = 3;

FilterMode::Type FILTER_MODE_OPTIONS[ FILTER_MODE_COUNT ] =
  { FilterMode::DEFAULT,
    FilterMode::NEAREST,
    FilterMode::LINEAR };

// These are the default sampling options - must match what is in GL
const FilterMode::Type DEFAULT_MINIFY = FilterMode::LINEAR;
const FilterMode::Type DEFAULT_MAGNIFY = FilterMode::LINEAR;

} // namespace

/**
 * Utility to store one of the FilterMode values.
 * @param[out] options A bitmask used to store the FilterMode values.
 * @param[in] factor The FilterMode value.
 * @param[in] bitshift Used to shift to the correct part of options.
 */
void StoreFilterMode( unsigned int& options, FilterMode::Type mode, int bitShift )
{
  // Start shifting from 1 as 0 is the unassigned state
  switch ( mode )
  {
    case FilterMode::DEFAULT:
    {
      options |= ( 1u << bitShift );
      break;
    }
    case FilterMode::NEAREST:
    {
      options |= ( 2u << bitShift );
      break;
    }
    case FilterMode::LINEAR:
    {
      options |= ( 3u << bitShift );
      break;
    }
  }
}

/**
 * Utility to retrieve one of the FilterMode values.
 * @param[in] options A bitmask of filter values.
 * @param[in] mask The used to mask unwanted values.
 * @param[in] bitshift Used to shift to the correct part of options.
 * @return The filter mode.
 */
bool RetrieveFilterMode( unsigned int options, int mask, int bitShift, FilterMode::Type& filterModeOut )
{
  unsigned int index = options & mask;

  if( index != 0 )
  {
    index = ( index >> bitShift ) - 1;    // Zero based index for array

    DALI_ASSERT_DEBUG( index < FILTER_MODE_COUNT );

    filterModeOut = FILTER_MODE_OPTIONS[ index ];
    return true;
  }

  return false;
}

unsigned int DefaultOptions()
{
  // Only initialise min filter as mag filter will use the system default
  unsigned int bitfield = 0;
  StoreFilterMode( bitfield, DEFAULT_MINIFY, MINIFY_BIT_SHIFT );
  return bitfield;
}

bool IsMinifyAssigned( unsigned int bitfield )
{
  FilterMode::Type filterModeDummy = FilterMode::NEAREST;
  return RetrieveFilterMode( bitfield, MASK_MINIFY_FILTER, MINIFY_BIT_SHIFT, filterModeDummy );
}

bool IsMagnifyAssigned( unsigned int bitfield )
{
  FilterMode::Type filterModeDummy = FilterMode::NEAREST;
  return RetrieveFilterMode( bitfield, MASK_MAGNIFY_FILTER, MAGNIFY_BIT_SHIFT, filterModeDummy );
}

unsigned int PackBitfield( FilterMode::Type minify, FilterMode::Type magnify )
{
  unsigned int bitfield = 0;
  StoreFilterMode( bitfield, minify, MINIFY_BIT_SHIFT );
  StoreFilterMode( bitfield, magnify, MAGNIFY_BIT_SHIFT );
  return bitfield;
}

FilterMode::Type GetMinifyFilterMode( unsigned int bitfield )
{
  FilterMode::Type filterMode = FilterMode::NEAREST;
  RetrieveFilterMode( bitfield, MASK_MINIFY_FILTER, MINIFY_BIT_SHIFT, filterMode );
  return filterMode;
}

FilterMode::Type GetMagnifyFilterMode( unsigned int bitfield )
{
  FilterMode::Type filterMode = FilterMode::NEAREST;
  RetrieveFilterMode( bitfield, MASK_MAGNIFY_FILTER, MAGNIFY_BIT_SHIFT, filterMode );
  return filterMode;
}

} // namespace ImageSampler

} // namespace Internal

} // namespace Dali
