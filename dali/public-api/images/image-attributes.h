#ifndef __DALI_IMAGE_ATTRIBUTES_H__
#define __DALI_IMAGE_ATTRIBUTES_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

class ImageAttributes;

/**
 * @brief Describes Image properties like dimensions and pixel format and
 * operations to be applied to images during the load process.
 *
 * ImageAttributes is used to define a set of properties of an image and a
 * sequence of operations to be applied when loading it.
 *
 * The overall order of operations which can be applied is:
 *   1. Determine the desired dimensions for the final bitmap.
 *   2. Scale the image to fit the desired dimensions.
 *
 * The default for each stage is to do nothing.
 * To enable a calculation of desired final image dimensions and fitting to it, SetSize() must be called.
 *
 * The loader does not guarantee to rescale a loaded image to the exact desired dimensions, but it will make a best effort to downscale images.
 * The fitting to destination dimensions controlled by the ScalingMode may choose to fit to a larger area with an equivalent aspect ratio.
 * If the requested dimensions are larger than the loaded ones, it will never upscale on load to fill them but will instead fit to smaller dimensions of identical aspect ratio.
 * This is transparent to an application as the upscaling can happen during rendering.
 *
 * To enable scaling of images on load, desired dimensions must be set using SetSize().
 * Only one of the dimensions need be supplied, in which case, the other is calculated based on the aspect ratio of the raw loaded image.
 * The desired dimensions 2-tuple 'd' is determined as follows for loaded image dimensions 'l' and 's', the dimensions tuple set with SetSize():
 *   *  `d = s, if s.x != 0 & s.y != 0, else:`
 *   *  `d = [s.x, s.x * (l.y / l.x)], if s.x != 0 & s.y = 0, else:`
 *   *  `d = [s.y * (l.x / l.y), s.y], if s.x = 0 & s.y != 0, else:`
 *   *  `d = l, otherwise.`
 *
 * Use cases for scaling images on load include:
 *   1. Full-screen image display: Limit loaded image resolution to device resolution using ShrinkToFit mode.
 *   2. Thumbnail gallery grid: Limit loaded image resolution to screen tile using ScaleToFill mode.
 *   3. Image columns: Limit loaded image resolution to column width using FitWidth mode.
 *   4. Image rows: Limit loaded image resolution to row height using FitHeight mode.
 *
 * @note The aspect ratio of image contents is preserved by all scaling modes, so for example squares in input images stay square after loading.
 */
class DALI_IMPORT_API ImageAttributes
{
public:

  /**
   * @brief Scaling options, used when resizing images on load to fit desired dimensions.
   *
   * A scaling mode controls the region of a loaded image to be mapped to the
   * desired image rectangle specified using ImageAttributes.SetSize().
   * All scaling modes preserve the aspect ratio of the image contents.
   */
  enum ScalingMode
  {
    ShrinkToFit, ///< Fit full image inside desired width & height, potentially not filling one of either the desired image width or height with pixels.
    ScaleToFill, ///< Image fills whole desired width & height with image data. The image is centred in the desired dimensions, exactly touching in one dimension, with image regions outside the other desired dimension cropped away.
    FitWidth,    ///< Image fills whole width. Height is scaled proportionately to maintain aspect ratio.
    FitHeight    ///< Image fills whole height. Width is scaled proportionately to maintain aspect ratio.
  };

  /**
   * @brief Filtering options, used when resizing images on load to sample original pixels.
   *
   * A FilterMode controls how pixels in the raw image on-disk are sampled and
   * combined to generate each pixel of the destination loaded image.
   *
   * @note NoFilter and Box modes do not guarantee that the loaded pixel array
   * exactly matches the rectangle specified by the desired dimensions and
   * ScalingMode, but all other filter modes do if the desired dimensions are
   * `<=` the raw dimensions of the image file.
   */
  enum FilterMode
  {
    Box,            ///< Iteratively box filter to generate an image of 1/2, 1/4, 1/8, ... width and height and
                    ///  approximately the desired size, then if the ScaleToFill scaling mode is enabled, cut away the
                    ///  top/bottom or left/right borders of the image to match the aspect ratio of desired dimensions.
                    ///  This is the default.
    Nearest,        ///< For each output pixel, read one input pixel.
    Linear,         ///< For each output pixel, read a quad of four input pixels and write a weighted average of them.
    BoxThenNearest, ///< Iteratively box filter to generate an image of 1/2, 1/4, 1/8, ... width and height and
                    ///  approximately the desired size, then for each output pixel, read one pixel from the last level
                    ///  of box filtering.
    BoxThenLinear,  ///< Iteratively box filter to almost the right size, then for each output pixel, read four pixels
                    ///  from the last level of box filtering and write their weighted average.
    NoFilter,       ///< No filtering is performed. If the ScaleToFill scaling mode is enabled, the borders of the
                    ///  image may be trimmed to match the aspect ratio of the desired dimensions.
    DontCare        ///< For when the client strongly prefers a cache-hit. Defaults to Box.
  };

  static const ImageAttributes DEFAULT_ATTRIBUTES; ///< Default attributes have no size

  /**
   * @brief Default constructor, initializes to default values.
   */
  ImageAttributes();

  /**
   * @brief This copy constructor is required for correctly copying internal implementation.
   *
   * @param [in] rhs A reference to the copied handle
   */
  ImageAttributes(const ImageAttributes& rhs);

  /**
   * @brief This assignment operator is required for correctly handling the internal implementation.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return a reference to this object
   */
  ImageAttributes& operator=(const ImageAttributes& rhs);

  /**
   * @brief Default destructor.
   */
  ~ImageAttributes();

  /**
   * @brief Create an initialised image attributes object.
   *
   * @return A handle to a newly allocated object
   */
  static ImageAttributes New();

  /**
   * @brief Create an initialised image attributes object.
   *
   * @param [in] width         desired width.
   * @param [in] height        desired height
   * @return A handle to a newly allocated object
   */
   static ImageAttributes New(unsigned int width, unsigned int height);

  /**
   * @brief Set the size properties.
   *
   * By default width and height are set to zero which means the image loaded has the original size.
   * If one dimension is set to non-zero, but the other zeroed, the unspecified one is derived from
   * the one that is set and the aspect ratio of the image.
   *
   * @param [in] width  desired width.
   * @param [in] height desired height
   */
  void SetSize(unsigned int width, unsigned int height);

  /**
   * @brief Set the image dimension properties.
   *
   * By default, width and height are set to zero which means the image loaded has the original size.
   * If one dimension is set to non-zero, but the other zeroed, the unspecified one is derived from
   * the one that is set and the aspect ratio of the image.
   *
   * @param [in] size desired size.
   */
  void SetSize( const Size& size );

  /**
   * @brief Set the scale field of the image attributes.
   *
   * By default, ShrinkToFit is set.
   * @param [in] scalingMode The desired scaling mode
   */
  void SetScalingMode(ScalingMode scalingMode);

  /**
   * @brief Setter for the FilterMode.
   * By default, Box is set.
   * @param [in] filterMode The desired filter mode.
   */
  void SetFilterMode( FilterMode filterMode );

  /**
   * @brief Set whether the image will be rotated/flipped back into portrait orientation.
   *
   * This will only be necessary if metadata indicates that the
   * image has a different viewing orientation.
   *
   * This metadata, optionally present in formats that use exif for example,
   * can encode the physical orientation of the camera which took the picture,
   * establishing which directions in the image correspond to real-world "up"
   * and the horizon.
   * By default the metadata is ignored, but if this function is called with
   * the value "true", the pixels of an image are reordered at load time to reflect
   * the orientation in the metadata.
   *
   * @param [in] enabled If true, the image orientation metadata will be used to
   *                     transform the pixels of the image as laid-out in memory.
   */
  void SetOrientationCorrection(bool enabled);


  /**
   * @brief Return the width currently represented by the attribute.
   *
   * @return width
   */
  unsigned int GetWidth() const;

  /**
   * @brief Return the height currently represented by the attribute.
   *
   * @return height
   */
  unsigned int GetHeight() const;

  /**
   * @brief Return the size currently represented by the attribute.
   *
   * @return size
   */
  Size GetSize() const;

  /**
   * @brief Return the scale currently represented by the attribute.
   *
   * @return scale
   */
  ScalingMode GetScalingMode() const;

  /**
   * @brief Getter for the FilterMode
   *
   * @return The FilterMode previously set, or the default value if none has
   *         been.
   */
  FilterMode GetFilterMode() const;

  /**
   * @brief Whether to correct for physical orientation of an image.
   *
   * @return Whether image pixels should be transformed according to the
   *         orientation metadata, if any.
   */
  bool GetOrientationCorrection() const;

  /**
   * @brief Less then comparison operator.
   *
   * @param [in] a parameter tested
   * @param [in] b parameter tested
   * @return true if a is less than b
   */
  friend bool operator<(const ImageAttributes& a, const ImageAttributes& b);

  /**
   * @brief Equal to comparison operator.
   *
   * @param [in] a parameter tested for equality
   * @param [in] b parameter tested for equality
   * @return true if a is equal to b
   */
  friend bool operator==(const ImageAttributes& a, const ImageAttributes& b);

  /**
   * @brief Not equal to comparison operator.
   *
   * @param [in] a parameter tested for equality
   * @param [in] b parameter tested for equality
   * @return true if a is not equal to b
   */
  friend bool operator!=(const ImageAttributes& a, const ImageAttributes& b);

private:
  struct ImageAttributesImpl;
  ImageAttributesImpl* impl; ///< Implementation pointer
};

/**
 * @brief Less then comparison operator.
 *
 * @param [in] a parameter tested
 * @param [in] b parameter tested
 * @return true if a is less than b
 */
DALI_IMPORT_API bool operator<(const ImageAttributes& a, const ImageAttributes& b);

/**
 * @brief Equal to comparison operator.
 *
 * @param [in] a parameter tested for equality
 * @param [in] b parameter tested for equality
 * @return true if a is equal to b
 */
DALI_IMPORT_API bool operator==(const ImageAttributes& a, const ImageAttributes& b);

/**
 * @brief Not equal to comparison operator.
 *
 * @param [in] a parameter tested for equality
 * @param [in] b parameter tested for equality
 * @return true if a is not equal to b
 */
DALI_IMPORT_API bool operator!=(const ImageAttributes& a, const ImageAttributes& b);

} // namespace Dali

#endif // __DALI_IMAGE_ATTRIBUTES_H__
