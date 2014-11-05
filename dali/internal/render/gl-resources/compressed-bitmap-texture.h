#ifndef __DALI_INTERNAL_COMPRESSED_BITMAP_TEXTURE_H__
#define __DALI_INTERNAL_COMPRESSED_BITMAP_TEXTURE_H__

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
#include <string>
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/images/bitmap-image.h>
#include <dali/internal/common/message.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/common/bitmap-upload.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/event/images/bitmap-compressed.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

class CompressedBitmapTexture;
typedef IntrusivePtr<CompressedBitmapTexture> CompressedBitmapTexturePointer;

/**
 * Compressed Texture class.
 * If you want to load a file to a CompressedBitmapTexture use
 * TextureManager::GetTexture()
 * Compressed textures differ from ordinary ones in that their in-memory
 * representation cannot be inspected at a pixel level by simple pointer
 * arithmetic and in that they cannot be partially updated.
 */
class CompressedBitmapTexture : public Texture
{
public:
  /**
   * Constructor
   * Creates a new texture object from a Bitmap
   * @param[in] bitmap The Bitmap
   * @param[in] context The GL context
   * @param[in] discardPolicy The discard policy
   */
  CompressedBitmapTexture( Internal::BitmapCompressed* const bitmap, Context& context, ResourcePolicy::Discardable discardPolicy );

  /**
   * Destructor.
   */
  virtual ~CompressedBitmapTexture();

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
   * Always returns false as we can't know what is going on inside the encoded pixel data. ///!Todo: Revise this decision: Paul, didn't you want the opposite assumption?
   */
  virtual bool IsFullyOpaque() const;

  /**
   * Bitmap has been reloaded - update the texture appropriately.
   * @param[in] bitmap The new bitmap
   */
  virtual void Update( Integration::Bitmap* bitmap );

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
   * Assigns the bitmap data to an OpenGL texture
   * Creates a new texture object and copies
   * the image data held in the pixels parameter
   * @param[in] generateTexture True if we should generate a GL texture id
   * @param[in] pixels The pixel data
   */
  void AssignBitmap( bool generateTexture, const unsigned char* pixels, const size_t bufferSize );

private:
  Internal::BitmapCompressedPtr mBitmap;      ///< The Bitmap the Texture was created from (may be NULL)
  ResourcePolicy::Discardable mDiscardPolicy;

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};
} //namespace Internal
} //namespace Dali

#endif //__DALI_INTERNAL_COMPRESSED_BITMAP_TEXTURE_H__
