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

// EXTERNAL INCLUDES
#include <iosfwd>

namespace Dali
{

namespace Internal
{

namespace ImageSampler
{

namespace
{

// @todo MESH_REWORK Remove file after image removal

// Adjust these shift sizes if the FilterMode enum grows
const int MINIFY_BIT_SHIFT  = 0;    // Room for 16
const int MAGNIFY_BIT_SHIFT = 4;

const int MASK_MINIFY_FILTER    = 0x0000000F;
const int MASK_MAGNIFY_FILTER   = 0x000000F0;

const unsigned int FILTER_MODE_COUNT = 4;

FilterMode::Type FILTER_MODE_OPTIONS[ FILTER_MODE_COUNT ] =
  { FilterMode::NONE,
    FilterMode::DEFAULT,
    FilterMode::NEAREST,
    FilterMode::LINEAR };

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
    case FilterMode::NONE:
    {
      // Nothing to do
      break;
    }
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
 * @return Return the filter mode.
 */
FilterMode::Type RetrieveFilterMode( unsigned int options, int mask, int bitShift )
{
  unsigned int index = options & mask;

  index = ( index >> bitShift );    // Zero based index for array

  DALI_ASSERT_DEBUG( index < FILTER_MODE_COUNT );

  return FILTER_MODE_OPTIONS[ index ];
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
  return RetrieveFilterMode( bitfield, MASK_MINIFY_FILTER, MINIFY_BIT_SHIFT );
}

FilterMode::Type GetMagnifyFilterMode( unsigned int bitfield )
{
  return RetrieveFilterMode( bitfield, MASK_MAGNIFY_FILTER, MAGNIFY_BIT_SHIFT );
}

} // namespace ImageSampler

} // namespace Internal

} // namespace Dali
