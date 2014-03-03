#ifndef __DALI_IMAGE_ATTRIBUTES_H__
#define __DALI_IMAGE_ATTRIBUTES_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>

namespace Dali DALI_IMPORT_API
{

class ImageAttributes;

/**
 * Describes Image properties like width or pixel format.
 * ImageAttributes are used to request specific details when loading an image.
 * Loading a scaled-down version of a JPEG image can be done by requesting width and height values that are
 * smaller than the image's natural size. The requested values are taken into consideration but
 * original aspect ratio is still preserved.
 * After an image has successfuly loaded, ImageAttributes contain the actual size of the image.
 */
class ImageAttributes
{
public:

  /**
   * Scaling options, meant for thumbnail loading.
   */
  enum ScalingMode
  {
    ShrinkToFit, ///< fit full thumbnail inside width & height, maintain aspect ratio
    ScaleToFill, ///< thumbnail fills whole width & height, maintain aspect ratio
    FitWidth,    ///< thumbnail fills whole width, height is scaled to maintain aspect ratio
    FitHeight     ///< thumbnail fills whole height, width is scaled to maintain aspect ratio
  };

  static const ImageAttributes DEFAULT_ATTRIBUTES;

  /**
   * Default constructor, initializes to default values.
   */
  ImageAttributes();

  /**
   * This copy constructor is required for correctly copying internal implementation
   * @param [in] rhs A reference to the copied handle
   */
  ImageAttributes(const ImageAttributes& rhs);

  /**
   * This assignment operator is required for correctly handling the internal implementation
   * @param [in] rhs  A reference to the copied handle
   */
  ImageAttributes& operator=(const ImageAttributes& rhs);

  /**
   * Default destructor,
   */
  ~ImageAttributes();

  /**
   * Create an initialised image attributes object
   *
   * @return A handle to a newly allocated object
   */
  static ImageAttributes New();

  /**
   * Create an initialised image attributes object
   * @param [in] width         desired width.
   * @param [in] height        desired height
   * @param [in] format        desired pixelformat
   * @return A handle to a newly allocated object
   */
  static ImageAttributes New(unsigned int width, unsigned int height, Pixel::Format format);

  /**
   * Create an initialised image attributes object for distance field generation
   * Using default parameters.
   *
   * @return A handle to a newly allocated object
   */
  static ImageAttributes NewDistanceField();

  /**
   * Create an initialised image attributes object for distance field generation
   *
   * @param [in] fieldRadius The minimum search radius to check for differing pixels
   * @param [in] fieldBorder The amount of distancefield cells to add around the data (for glow/shadow effects)
   * @return A handle to a newly allocated object
   */
  static ImageAttributes NewDistanceField(float fieldRadius, int fieldBorder);

  /**
   * Set the size properties
   * By default width and height are set to zero which means the image loaded has the original size.
   * @param [in] width  desired width.
   * @param [in] height desired height
   */
  void SetSize(unsigned int width, unsigned int height);

  /**
   * Set the size properties
   * By default width and height are set to zero which means the image loaded has the original size.
   * @param [in] size  desired size.
   */
  void SetSize( const Size& size );

  /**
   * Set the pixelformat field of the image attributes
   * By default is set to Pixel::RGBA8888.
   * @param [in] format desired pixelformat
   */
  void SetPixelFormat(Pixel::Format format);

  /**
   * Set the scale field of the image attributes.
   * By default, ShrinkToFit is set.
   * @param [in] scalingMode The desired scaling mode
   */
  void SetScalingMode(ScalingMode scalingMode);

  /**
   * Set the desired cropping for the attribute
   *
   * Cropping options, relative to image actual size.
   * (0.0, 0.0) is top left corner, (1.0, 1.0) is the full width and height
   * defaults are (0,0,1,1) so that whole image is loaded
   * (0.25, 0.25, 0.5, 0.5) would mean that 50% of the image is loaded from the middle
   *
   * @param [in] cropRect - The crop rectangle
   */
  void SetCrop(const Rect<float>& cropRect);

  /**
   * Set whether the image will be rotated/flipped back into portrait
   * orientation. This will only be necessary if metadata indicates that the
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
   * Return the width currently represented by the attribute
   *
   * @return width
   */
  unsigned int GetWidth() const;

  /**
   * Return the height currently represented by the attribute
   *
   * @return height
   */
  unsigned int GetHeight() const;

  /**
   * Return the size currently represented by the attribute
   *
   * @return size
   */
  Size GetSize() const;

  /**
   * Return the pixel format currently represented by the attribute
   *
   * @return pixel format
   */
  Pixel::Format GetPixelFormat() const;

  /**
   * Return the scale currently represented by the attribute
   *
   * @return scale
   */
  ScalingMode GetScalingMode() const;

  /**
   * Return if the attribute set up as a distance field
   *
   * @return true, if the attribute is a distance field.
   */
  bool IsDistanceField() const;

  /**
   * Return the field border currently represented by the attribute
   *
   * @return field border
   */
  int GetFieldBorder() const;

  /**
   * Return the field search radius currently represented by the attribute
   *
   * @return field radius
   */
  float GetFieldRadius() const;

  /**
   * Return the crop currently represented by the attribute
   *
   * @return crop
   */
  const Rect<float>& GetCrop() const;

  /**
   * @return Whether image pixels should be transformed according to the
   *         orientation metadata, if any.
   */
  bool GetOrientationCorrection() const;

  /**
   * Less then comparison operator.
   * @param [in] a parameter tested
   * @param [in] b parameter tested
   */
  friend bool operator<(const ImageAttributes& a, const ImageAttributes& b);

  /**
   * Equal to comparison operator.
   * @param [in] a parameter tested for equality
   * @param [in] b parameter tested for equality
   */
  friend bool operator==(const ImageAttributes& a, const ImageAttributes& b);

  /**
   * Not equal to comparison operator.
   * @param [in] a parameter tested for equality
   * @param [in] b parameter tested for equality
   */
  friend bool operator!=(const ImageAttributes& a, const ImageAttributes& b);

private:
  struct ImageAttributesImpl;
  ImageAttributesImpl* impl;
};

/**
 * Less then comparison operator.
 * @param [in] a parameter tested
 * @param [in] b parameter tested
 */
bool operator<(const ImageAttributes& a, const ImageAttributes& b);

/**
 * Equal to comparison operator.
 * @param [in] a parameter tested for equality
 * @param [in] b parameter tested for equality
 */
bool operator==(const ImageAttributes& a, const ImageAttributes& b);

/**
 * Not equal to comparison operator.
 * @param [in] a parameter tested for equality
 * @param [in] b parameter tested for equality
 */
bool operator!=(const ImageAttributes& a, const ImageAttributes& b);

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_IMAGE_ATTRIBUTES_H__
