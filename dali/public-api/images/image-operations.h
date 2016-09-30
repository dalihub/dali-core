#ifndef __DALI_IMAGE_OPERATIONS_H__
#define __DALI_IMAGE_OPERATIONS_H__

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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/math/uint-16-pair.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

/**
 * @brief The integer dimensions of an image or a region of an image packed into
 *        16 bits per component.
 *
 * This can only be used for images of up to 65535 x 65535 pixels.
 * @SINCE_1_0.0
 */
typedef Dali::Uint16Pair ImageDimensions;

/**
 * @brief Fitting options, used when resizing images to fit desired dimensions.
 *
 * A fitting mode controls the region of a loaded image to be mapped to the
 * desired image rectangle.
 * All fitting modes preserve the aspect ratio of the image contents.
 * @SINCE_1_0.0
 */
namespace FittingMode
{
  enum Type
  {
    SHRINK_TO_FIT, ///< Fit full image inside desired width & height, potentially not
                   ///  filling one of either the desired image width or height with
                   ///  pixels.
                   ///  @SINCE_1_0.0
    SCALE_TO_FILL, ///< Image fills whole desired width & height with image data. The
                   ///  image is centred in the desired dimensions, exactly touching
                   ///  in one dimension, with image regions outside the other desired
                   ///  dimension cropped away.
                   ///  @SINCE_1_0.0
    FIT_WIDTH,     ///< Image fills whole width. Height is scaled proportionately to
                   ///  maintain aspect ratio.
                   ///  @SINCE_1_0.0
    FIT_HEIGHT     ///< Image fills whole height. Width is scaled proportionately to
                   ///  maintain aspect ratio.
                   ///  @SINCE_1_0.0
  };
  const Type DEFAULT = SHRINK_TO_FIT;
}

/**
 * @brief Filtering options, used when resizing images to sample original pixels.
 *
 * A SamplingMode controls how pixels in an input image are sampled and
 * combined to generate each pixel of a destination image during a scaling.
 *
 * NoFilter and Box modes do not guarantee that the output pixel array
 * exactly matches the rectangle specified by the desired dimensions and
 * FittingMode, but all other filter modes do if the desired dimensions are
 * `<=` the raw dimensions of the input image file.
 * @SINCE_1_0.0
 */
namespace SamplingMode
{
  enum Type
  {
    BOX,              ///< Iteratively box filter to generate an image of 1/2, 1/4,
                      ///  1/8, etc width and height and approximately the desired
                      ///  size. This is the default.
                      ///  @SINCE_1_0.0
    NEAREST,          ///< For each output pixel, read one input pixel.
                      ///  @SINCE_1_0.0
    LINEAR,           ///< For each output pixel, read a quad of four input pixels
                      ///  and write a weighted average of them.
                      ///  @SINCE_1_0.0
    BOX_THEN_NEAREST, ///< Iteratively box filter to generate an image of 1/2, 1/4,
                      ///  1/8 etc width and height and approximately the desired
                      ///  size, then for each output pixel, read one pixel from the
                      ///  last level of box filtering.
                      ///  @SINCE_1_0.0
    BOX_THEN_LINEAR,  ///< Iteratively box filter to almost the right size, then for
                      ///  each output pixel, read four pixels from the last level of
                      ///  box filtering and write their weighted average.
                      ///  @SINCE_1_0.0
    NO_FILTER,        ///< No filtering is performed. If the SCALE_TO_FILL scaling mode
                      ///  is enabled, the borders of the image may be trimmed to
                      ///  match the aspect ratio of the desired dimensions.
                      ///  @SINCE_1_0.0
    DONT_CARE         ///< For caching algorithms where a client strongly prefers a
                      ///  cache-hit to reuse a cached image.
                      ///  @SINCE_1_0.0
  };
  const Type DEFAULT = BOX;
}

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_IMAGE_OPERATIONS_H__
