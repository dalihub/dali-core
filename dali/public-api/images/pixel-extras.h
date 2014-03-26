#ifndef __DALI_PIXEL_EXTRAS_H__
#define __DALI_PIXEL_EXTRAS_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/public-api/images/pixel.h>

namespace Dali DALI_IMPORT_API
{

/**
 * Pixel format extra types, their properties, and utilities for working with
 * pixel types.
 */
namespace Pixel
{

/**
 * For asserting that a pixel format is suitable for an addressable bitmap array
 * of pixels and not a signifier of an opaque one such as the compressed formats.
 **/
const Format LAST_ADDRESSABLE_PIXEL_FORMAT = BGRA8888;

/**
 * Whether specified pixel format is of an opaque encoded type such as a
 * GL compressed texture.
 * @param [in] pixelformat pixel format
 * @return true if format is encoded, false otherwise.
 */
bool IsEncoded( const Format pixelformat );

} //namespace Pixel

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_PIXEL_EXTRAS_H__
