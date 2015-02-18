#ifndef __DALI_INTERNAL_BITMAP_TEXTURE_H__
#define __DALI_INTERNAL_BITMAP_TEXTURE_H__

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

// EXTERNAL INCLUDES
#include <stdint.h> // for uint32_t

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/common/bitmap-upload.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/render/gl-resources/texture-cache.h>

namespace Dali
{

namespace Internal
{

class BitmapTexture;
typedef IntrusivePtr<BitmapTexture> BitmapTexturePointer;

/**
 *
 * Texture class.
 * If you want to load a file to a BitmapTexture use
 * TextureManager::GetTexture()
 *
 */
class BitmapTexture : public Texture
{
public:
  /**
   * Constructor
   * Creates a new texture object from a Bitmap
   * @param[in] bitmap The Bitmap
   * @param[in] bitmapPackedPixelsProfile The Bitmap features related to an addressable array of raw pixel data
   * @param     context The GL context
   */
  BitmapTexture(Integration::Bitmap* const bitmap, const Integration::Bitmap::PackedPixelsProfile * const bitmapPackedPixelsProfile, Context& context, ResourcePolicy::Discardable discardPolicy);

  /**
   * Constructor
   * Creates a new texture object
   * @param[in] width width in pixels
   * @param[in] height height in pixels
   * @param[in] pixelFormat pixel format
   * @param[in] clearPixels True if the pixel data should be cleared before gl texture creation
   * @param[in] context Dali context
   */
  BitmapTexture( unsigned int width, unsigned int height, Pixel::Format pixelFormat,
                 bool clearPixels, Context& context, ResourcePolicy::Discardable discardPolicy );

  /**
   * Destructor.
   */
  virtual ~BitmapTexture();

public: // Message interface

  /**
   * Upload an array of bitmaps
   * @param bitmapArray array of bitmap items
   */
  void UploadBitmapArray( const BitmapUploadArray& bitmapArray);

  /**
   * Clear an array of areas from bitmap to the given color
   * @param[in] areaArray Array of rects to clear
   * @param[in] blockSize Size of block to clear
   * @param[in] color Clear color
   */
  void ClearAreas( const BitmapClearArray& areaArray, std::size_t blockSize, uint32_t color );

  /**
   * Retrieve the bitmap
   * @return The bitmap or NULL if already discarded
   */
  Integration::Bitmap* GetBitmap() { return mBitmap.Get(); }

public:

  /**
   * @copydoc Texture::Init
   */
  virtual bool Init();

  /**
   * @copydoc Texture::GetWidth
   */
  virtual unsigned int GetWidth() const;

  /**
   * @copydoc Texture::GetHeight
   */
  virtual unsigned int GetHeight() const;

  /**
   * @copydoc Texture::HasAlphaChannel
   */
  virtual bool HasAlphaChannel() const;

  /**
   * @copydoc Texture::IsFullyOpaque
   */
  virtual bool IsFullyOpaque() const;

  /**
   * Replace current bitmap with a fresh one, for instance after a Bitmap has
   * been reloaded.
   * @param[in] bitmap The new bitmap
   */
  virtual void Update( Integration::Bitmap* bitmap );

  /**
   * Update part of the texture with a different bitmap
   * @param[in] srcBitmap The bitmap to copy from
   * @param [in] xOffset Specifies an offset in the x direction within the texture
   * @param [in] yOffset Specifies an offset in the y direction within the texture
   */
  virtual void Update( Integration::Bitmap* srcBitmap, std::size_t xOffset, std::size_t yOffset );

  /**
   * Bitmap area has been modified - update the texture appropriately.
   * @pre The bitmap hasn't been discarded (should be external type)
   * @param[in] area The updated area
   */
  virtual void UpdateArea( const RectArea& area );

  /**
   * @return Return true if the texture should be updated on GL texture creation.
   */
  virtual bool UpdateOnCreate();

protected:
  /**
   * @copydoc Texture::CreateGlTexture
   */
  virtual bool CreateGlTexture();

private:

  /**
   * Uploads changes to GPU after Bitmap buffer has changed.
   * @param [in] updateArea area which changed
   * @param[in] pixels The pixel data
   */
  void AreaUpdated( const RectArea& updateArea, const unsigned char* pixels );

  /**
   * Assigns the bitmap data to an OpenGL texture
   * Creates a new texture object and copies
   * the image data held in the pixels parameter
   * @pre The texture has to have a width/height that is a power of 2.
   * @param[in] generateTexture True if we should generate a GL texture id
   * @param[in] pixels The pixel data
   */
  void AssignBitmap( bool generateTexture, const unsigned char* pixels );

  /**
   * If the discard policy is not RETAIN, then discards the bitmap's pixel buffer
   */
  void DiscardBitmapBuffer();

private:
  Integration::BitmapPtr mBitmap;      ///< The Bitmap the Texture was created from (may be NULL)
  bool                   mClearPixels; ///< true if initial texture should be cleared on creation
  ResourcePolicy::Discardable mDiscardPolicy; ///< The bitmap discard policy
  Pixel::Format            mPixelFormat;

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};

//
// Upload bitmap array message
//
inline MessageBase* UploadBitmapArrayMessage( BitmapTexture& texture, const BitmapUploadArray& bitmapArray )
{
  return new MessageValue1< BitmapTexture, BitmapUploadArray >( &texture, &BitmapTexture::UploadBitmapArray, bitmapArray );
}

}  //namespace Internal

} //namespace Dali

#endif //__DALI_INTERNAL_BITMAP_TEXTURE_H__
