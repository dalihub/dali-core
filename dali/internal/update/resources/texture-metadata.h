#ifndef __DALI_TEXTURE_METADATA_H__
#define __DALI_TEXTURE_METADATA_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/type-traits.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/images/native-image-interface.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>

namespace Dali
{

namespace Internal
{

/**
 * Texture metadata.
 */
class TextureMetadata
{
public:
  /**
   * Creates a BitmapMetadata object from a native image (eg.: EGLImage).
   * @param[in] nativeImage The native image to load
   * @return A newly allocated BitmapMetadata
   */
  static TextureMetadata New( ResourceId id, NativeImageInterfacePtr nativeImage )
  {
    return TextureMetadata( id, nativeImage->GetWidth(), nativeImage->GetHeight(), nativeImage->RequiresBlending(), ! nativeImage->RequiresBlending() );
  }

  /**
   * Creates a new BitmapMetadata object from a Bitmap
   * @param[in] bitmap The bitmap
   * @return A newly allocated BitmapMetadata
   */
  static TextureMetadata New( ResourceId id, Integration::Bitmap* const bitmap )
  {
    return TextureMetadata( id, bitmap->GetImageWidth(), bitmap->GetImageHeight(), Pixel::HasAlpha(bitmap->GetPixelFormat()), bitmap->IsFullyOpaque() );
  }

  /**
   * Creates a new BitmapMetadata object
   * @return A newly allocated BitmapMetadata
   */
  static TextureMetadata New( ResourceId id, unsigned int width, unsigned int height, bool hasAlphaChannel )
  {
    return TextureMetadata( id, width, height, hasAlphaChannel, !hasAlphaChannel );
  }

  /**
   * Default Constructor
   */
  TextureMetadata()
  : mSize( 0, 0 ),
    mId( 0 ),
    mHasAlphaChannel( true ),
    mOpaqueness( false ),
    mIsNativeImage( false ),
    mIsFramebuffer( false ),
    mHasFrameBufferBeenRenderedTo( false )
  {
  }

  /**
   * Constructor
   */
  TextureMetadata( ResourceId id, unsigned int width, unsigned int height, bool hasAlphaChannel, bool opaqueness )
  : mSize( width, height ),
    mId( id ),
    mHasAlphaChannel( hasAlphaChannel ),
    mOpaqueness( opaqueness ),
    mIsNativeImage( false ),
    mIsFramebuffer( false ),
    mHasFrameBufferBeenRenderedTo( false )
  {
  }

  /**
   * As this class is POD, compiler generated copy constructor and assignment operator are ok (bitwise copy)
   */

  /**
   * @return the id
   */
  ResourceId GetId() const
  {
    return mId;
  }

  /**
   * Updates the metadata with information from the native image
   * @param[in] nativeImage The native image that was updated
   */
  void Update( NativeImageInterfacePtr nativeImage )
  {
    mSize = ImageDimensions( nativeImage->GetWidth(), nativeImage->GetHeight() );
    mHasAlphaChannel = nativeImage->RequiresBlending();
    mOpaqueness  = ! mHasAlphaChannel;
    mIsNativeImage = true;
  }

  /**
   * Updates the metadata with information from the bitmap
   * @param[in] bitmap The bitmap that was updated
   */
  void Update( Integration::Bitmap* const bitmap )
  {
    mSize = ImageDimensions( bitmap->GetImageWidth(), bitmap->GetImageHeight() );
    mHasAlphaChannel = Pixel::HasAlpha(bitmap->GetPixelFormat());
    mOpaqueness  = bitmap->IsFullyOpaque();
    mIsNativeImage = false;
  }

  /**
   * Set the width of image
   * @param[in] width The width of the image
   */
  void SetWidth( unsigned int width )
  {
    mSize.SetWidth( width );
  }

  /**
   * Return the width of image in pixels.
   * @return width
   */
  unsigned int GetWidth() const
  {
    return mSize.GetWidth();
  }

  /**
   * Set the height of image
   * @param[in] height The height of the image in pixels
   */
  void SetHeight(unsigned int height)
  {
    mSize.SetHeight( height );
  }

  /**
   * Return the height of image in pixels.
   * @return height
   */
  unsigned int GetHeight() const
  {
    return mSize.GetHeight();
  }

  /**
   * Query whether the texture data has an alpha channel.
   * @return True if the texture data has an alpha channel.
   */
  bool HasAlphaChannel() const
  {
    return mHasAlphaChannel;
  }

  /**
   * Set whether the texture is completely opaque, i.e.
   * true if all pixels of the texture data are opaque.
   * @param[in] opaqueness If the alpha channel is set to fully opaque.
   */
  void SetOpaqueness( bool opaqueness )
  {
    mOpaqueness = opaqueness;
  }

  /**
   * Query whether the texture is completely opaque
   * @return True if all pixels of the texture data are opaque
   */
  bool IsFullyOpaque() const
  {
    return mOpaqueness;
  }

  /**
   * @param isNativeImage flag
   */
  void SetIsNativeImage( bool isNativeImage )
  {
    mIsNativeImage = isNativeImage;
  }

  /**
   * @return isNativeImage flag
   */
  bool IsNativeImage() const
  {
    return mIsNativeImage;
  }

  /**
   * @param isFramebuffer flag
   */
  void SetIsFramebuffer( bool isFramebuffer )
  {
    mIsFramebuffer = isFramebuffer;
  }

  /**
   * @return isFramebuffer flag
   */
  bool IsFramebuffer() const
  {
    return mIsFramebuffer;
  }

  /**
   * @param value if the framebuffer has been rendered to
   */
  void SetFrameBufferBeenRenderedTo( bool value )
  {
    mHasFrameBufferBeenRenderedTo = value;
  }

  /**
   * @return true if the framebuffer has been rendered to
   */
  bool HasFrameBufferBeenRenderedTo() const
  {
    return mHasFrameBufferBeenRenderedTo;
  }

private: // Data

  ImageDimensions mSize;              ///< size compressed into 32 bits
  ResourceId      mId;                ///< Unique resource/texture ID
  bool            mHasAlphaChannel:1; ///< Pixel format of the contained image data.
  bool            mOpaqueness:1;      ///< Whether the bitmap was fully opaque when loaded / updated
  bool            mIsNativeImage:1;   ///< Whether the image is native or not
  bool            mIsFramebuffer:1;   ///< Whether the image is an FBO
  bool            mHasFrameBufferBeenRenderedTo:1; /// If the FBO has been rendered to

};

} // namespace Internal

// Allow BitmapMetadataEntry to be treated as a POD type (need to do this in Dali namespace)
template <> struct TypeTraits< Internal::TextureMetadata > : public BasicTypes< Internal::TextureMetadata > { enum { IS_TRIVIAL_TYPE = true }; };

} // namespace Dali

#endif // __DALI_TEXTURE_METADATA_H__
