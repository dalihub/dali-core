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
const int UWRAP_BIT_SHIFT   = 8;
const int VWRAP_BIT_SHIFT   = 12;

const int MASK_MINIFY_FILTER    = 0x0000000F;
const int MASK_MAGNIFY_FILTER   = 0x000000F0;
const int MASK_UWRAP_MODE       = 0x00000F00;
const int MASK_VWRAP_MODE       = 0x0000F000;

} // namespace

/**
 * Utility to store one of the sampling parameters values.
 * @param[out] options A bitmask used to store the FilterMode values.
 * @param[in] factor The FilterMode value.
 * @param[in] bitshift Used to shift to the correct part of options.
 */
void StoreSamplingParameter( unsigned int& options, unsigned int mode, int bitShift )
{
  if( mode != 0 )
  {
    options |= ( mode << bitShift );
  }
}

/**
 * Utility to retrieve one of the FilterMode values.
 * @param[in] options A bitmask of filter values.
 * @param[in] mask The used to mask unwanted values.
 * @param[in] bitshift Used to shift to the correct part of options.
 * @return Return the filter mode.
 */
unsigned int RetrieveSamplingParameter( unsigned int options, int mask, int bitShift )
{
  unsigned int index = options & mask;

  index = ( index >> bitShift );    // Zero based index for array
  return index;
}

unsigned int PackBitfield( FilterMode::Type minify, FilterMode::Type magnify, WrapMode::Type uWrap, WrapMode::Type vWrap )
{
  unsigned int bitfield = 0;
  StoreSamplingParameter( bitfield, minify, MINIFY_BIT_SHIFT );
  StoreSamplingParameter( bitfield, magnify, MAGNIFY_BIT_SHIFT );
  StoreSamplingParameter( bitfield, uWrap, UWRAP_BIT_SHIFT );
  StoreSamplingParameter( bitfield, vWrap, VWRAP_BIT_SHIFT );
  return bitfield;
}

FilterMode::Type GetMinifyFilterMode( unsigned int bitfield )
{
  return static_cast<FilterMode::Type>( RetrieveSamplingParameter( bitfield, MASK_MINIFY_FILTER, MINIFY_BIT_SHIFT ) );
}

FilterMode::Type GetMagnifyFilterMode( unsigned int bitfield )
{
  return static_cast<FilterMode::Type>( RetrieveSamplingParameter( bitfield, MASK_MAGNIFY_FILTER, MAGNIFY_BIT_SHIFT ) );
}

WrapMode::Type GetUWrapMode( unsigned int bitfield )
{
  return static_cast<WrapMode::Type>( RetrieveSamplingParameter( bitfield, MASK_UWRAP_MODE, UWRAP_BIT_SHIFT ) );
}

WrapMode::Type GetVWrapMode( unsigned int bitfield )
{
  return static_cast<WrapMode::Type>( RetrieveSamplingParameter( bitfield, MASK_VWRAP_MODE, VWRAP_BIT_SHIFT ) );
}

} // namespace ImageSampler

} // namespace Internal

} // namespace Dali
