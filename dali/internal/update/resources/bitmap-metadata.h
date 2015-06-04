#ifndef __DALI_BITMAP_METADATA_H__
#define  __DALI_BITMAP_METADATA_H__

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
#include <dali/integration-api/bitmap.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/images/native-image-interface.h>

namespace Dali
{
namespace Internal
{

/**
 * Texture class.
 */
class BitmapMetadata
{
public:
  /**
   * Creates a BitmapMetadata object from a native image (eg.: EGLImage).
   * @param[in] nativeImage The native image to load
   * @return A newly allocated BitmapMetadata
   */
  static BitmapMetadata New(NativeImageInterfacePtr nativeImage);

  /**
   * Creates a new BitmapMetadata object from a Bitmap
   * @param[in] bitmap The bitmap
   * @return A newly allocated BitmapMetadata
   */
  static BitmapMetadata New(Integration::Bitmap* const bitmap);

  /**
   * Creates a new BitmapMetadata object from framebuffer metadata
   * @return A newly allocated BitmapMetadata
   */
  static BitmapMetadata New(unsigned int width, unsigned int height, bool hasAlphaChannel);

  /**
   * Constructor
   */
  BitmapMetadata( unsigned int width, unsigned int height, bool hasAlphaChanne, bool opaqueness );

  /**
   * Copy constructor
   */
  BitmapMetadata( const BitmapMetadata& rhs );

  /**
   * Assignment operator
   */
  BitmapMetadata& operator=( const BitmapMetadata& rhs );

  /**
   * Default Constructor
   */
  BitmapMetadata();

  /**
   * Updates the metadata with information from the native image
   * @param[in] nativeImage The native image that was updated
   */
  void Update(NativeImageInterfacePtr nativeImage);

  /**
   * Updates the metadata with information from the bitmap
   * @param[in] bitmap The bitmap that was updated
   */
  void Update(Integration::Bitmap* const bitmap);

  /**
   * Return the width of image in pixels.
   * @return width
   */
  unsigned int GetWidth() const;

  /**
   * Return the height of image in pixels.
   * @return height
   */
  unsigned int GetHeight() const;

  /**
   * Query whether the texture data has an alpha channel.
   * @return True if the texture data has an alpha channel.
   */
  bool HasAlphaChannel() const;

  /**
   * Query whether the texture is completely opaque
   * @return True if all pixels of the texture data are opaque
   */
  bool IsFullyOpaque() const;

  /**
   * Set the width of image
   * @param[in] width The width of the image
   */
  void SetWidth(unsigned int width);

  /**
   * Set the height of image
   * @param[in] height The height of the image in pixels
   */
  void SetHeight(unsigned int height);

  /**
   * Set whether the texture has alpha channel
   * @param[in] hasAlphaChannel whether the texture has alpha channel
   */
  void SetHasAlphaChannel( bool hasAlphaChannel );

  /**
   * Set whether the texture is completely opaque, i.e.
   * true if all pixels of the texture data are opaque.
   * @param[in] opaqueness If the alpha channel is set to fully opaque.
   */
  void SetOpaqueness(bool opaqueness);

  void SetIsNativeImage( bool isNativeImage );
  bool GetIsNativeImage( );
  void SetIsFramebuffer( bool isFramebuffer );
  bool GetIsFramebuffer( );

private:
  unsigned int  mImageWidth;      ///< width of the original image
  unsigned int  mImageHeight;     ///< height of the original image
  bool          mHasAlphaChannel:1; ///< Pixel format of the contained image data.
  bool          mOpaqueness:1;    ///< Whether the bitmap was fully opaque when loaded / updated
  bool          mIsNativeImage:1; ///< Whether the image is native or not
  bool          mIsFramebuffer:1; ///< Whether the image is an FBO
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_BITMAP_METADATA_H__
