#ifndef __DALI_INTERNAL_NINE_PATCH_IMAGE_H__
#define __DALI_INTERNAL_NINE_PATCH_IMAGE_H__

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
#include <dali/public-api/images/nine-patch-image.h>
#include <dali/internal/event/images/resource-image-impl.h>
#include <dali/internal/event/images/bitmap-image-impl.h>

namespace Dali
{

namespace Internal
{

class NinePatchImage;
typedef IntrusivePtr<NinePatchImage> NinePatchImagePtr;

class ResourceClient;
class ResourceManager;

namespace SceneGraph
{
class UpdateManager;
}

/**
 * NinePatchImage represents an image resource that can be added to actors etc.
 * It's image data has a border which determines stretch and fill areas
 * Its pixel buffer data is loaded synchronously from file.
 */
class NinePatchImage : public ResourceImage
{
public:

  /**
   * Create a new NinePatchImage.
   * Also a pixel buffer for image data is allocated.
   * Dali has ownership of the buffer.
   * @param [in] filename    File to load synchronously into buffer
   * @param [in] attributes  Image attributes of the file
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  static NinePatchImagePtr New( const std::string& filename,
                                const ImageAttributes& attributes,
                                ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT );

  /**
   * Create a new NinePatchImage
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] filename    File to load synchronously into buffer
   * @param [in] attributes  Image attributes of the file
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  NinePatchImage( const std::string& filename,
                  const ImageAttributes& attributes,
                  ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT );

  /**
   * Convert Image object to a 9 patch image object if possible.
   * @param[in] image The image to convert
   * @return A pointer to the 9 patch image object, or NULL
   * if the conversion is not possible.
   */
  static NinePatchImage* DownCast( Image* image);


protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~NinePatchImage();

public:
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
   * @brief Create a cropped image from the bitmap with the 1 pixel border cropped off.
   * This does not change the internal bitmap.
   *
   * @return the cropped bitmap.
   */
  BitmapImagePtr CreateCroppedBitmapImage();


protected: // From Resource
  /**
   * @copydoc Dali::Internal::Image::Connect
   */
  virtual void Connect();

  /**
   * @copydoc Dali::Internal::Image::Disconnect
   */
  virtual void Disconnect();

private:
  /**
   * Read the borders of the bitmap and determine the child area
   * and stretch borders
   */
  void ParseBorders();

private:
  ResourceClient*               mResourceClient;
  Integration::BitmapPtr        mBitmap;
  Vector4                       mStretchBorders;
  Rect<int>                     mChildRectangle;
  bool                          mParsedBorder;
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::NinePatchImage& GetImplementation(Dali::NinePatchImage& handle)
{
  DALI_ASSERT_ALWAYS( handle && "NinePatchImage handle is empty" );

  BaseObject& image = handle.GetBaseObject();

  return static_cast<Internal::NinePatchImage&>(image);
}

inline const Internal::NinePatchImage& GetImplementation(const Dali::NinePatchImage& handle)
{
  DALI_ASSERT_ALWAYS( handle && "NinePatchImage handle is empty" );

  const BaseObject& image = handle.GetBaseObject();

  return static_cast<const Internal::NinePatchImage&>(image);
}

} // namespace Dali

#endif // __DALI_INTERNAL_NINE_PATCH_IMAGE_H__
