#ifndef __DALI_NINE_PATCH_IMAGE_H__
#define __DALI_NINE_PATCH_IMAGE_H__

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/bitmap-image.h>
#include <dali/public-api/math/rect.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class NinePatchImage;
}


/**
 * @brief NinePatchImage represents an image resource that can be added to ImageActors.
 * It contains a bitmap that is synchronously loaded from the file system that contains
 * a 9 patch border - a 1 pixel border that describes the stretch borders and the child
 * area.
 *
 * The class offers an API to read the stretch area and child area, but it does not
 * remove the border from it's bitmap. An API can be used to obtain a BitmapImage with
 * the border removed.
 *
 * Adding this image to an ImageActor using an Image handle will automatically convert
 * to use the cropped BitmapImage - if you don't retain a handle to this object, it will
 * be automatically destroyed.
 */
class NinePatchImage : public Image
{
public:
  /**
   * @brief Constructor which creates an uninitialized NinePatchImage object.
   *
   * Use Image::New(...) to create an initialised object.
   */
  NinePatchImage();

  /**
   * @brief Create a new NinePatchImage.
   *
   * A pixel buffer for image data is allocated and loaded from the filesystem.
   * Dali has ownership of the buffer.
   * @note: default resource management policies are Immediate and Never
   *
   * @param [in] filename    File to load synchronously into buffer
   * @return a handle to a new instance of NinePatchImage
   */
  static NinePatchImage New( const std::string& filename );

  /**
   * @brief Downcast an Object handle to NinePatchImage.
   *
   * If handle points to a NinePatchImage the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a NinePatchImage or an uninitialized handle
   */
  static NinePatchImage DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   */
  virtual ~NinePatchImage();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * Get the stretch borders
   * @return The border in pixels from the left, top, right, and bottom of the image respectively.
   */
  Vector4 GetStretchBorders();

  /**
   * Get the child rectangle
   * @return the position and size of the child rectangle
   */
  Rect<int> GetChildRectangle();

  /**
   * Creates a bitmap image from the bitmap with the 1 pixel border cropped off.
   * This does not change the internal bitmap.
   *
   * @return The cropped BitmapImage
   */
  BitmapImage CreateCroppedBitmapImage();

public: // Not intended for application developers

  explicit DALI_INTERNAL NinePatchImage(Internal::NinePatchImage*);
};

} // namespace Dali

#endif // __DALI_NINE_PATCH_IMAGE_H__
