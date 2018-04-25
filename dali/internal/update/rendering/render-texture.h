#ifndef DALI_INTERNAL_RENDER_TEXTURE_H
#define DALI_INTERNAL_RENDER_TEXTURE_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/images/image-operations.h> // Dali::ImageDimensions
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/internal/event/rendering/texture-impl.h>
#include <dali/internal/update/rendering/render-sampler.h>
#include <dali/graphics-api/graphics-api-accessor.h>
#include <dali/graphics-api/graphics-api-texture.h>

namespace Dali
{
namespace Internal
{
namespace Render
{


class Texture
{
public:

  typedef Dali::TextureType::Type Type;

  /**
   * Constructor
   * @param[in] type The type of the texture
   * @param[in] format The format of the pixel data
   * @param[in] size The size of the texture
   */
  Texture( Type type, Pixel::Format format, ImageDimensions size );

  /**
   * Constructor from native image
   * @param[in] nativeImageInterface The native image
   */
  Texture( NativeImageInterfacePtr nativeImageInterface );

  /**
   * Destructor
   */
  ~Texture();


  /**
   * Retrieve wheter the texture has an alpha channel
   * @return True if the texture has alpha channel, false otherwise
   */
  bool HasAlphaChannel();


  /**
   * Get the type of the texture
   * @return Type of the texture
   */
  Type GetType() const
  {
    return mType;
  }

  /**
   * Check if the texture is a native image
   * @return if the texture is a native image
   */
  bool IsNativeImage() const
  {
    return mNativeImage;
  }


  void SetGfxObject( Graphics::API::Accessor<Graphics::API::Texture> texture )
  {
    mGfxTexture = texture;
  }

  const Graphics::API::Accessor<Graphics::API::Texture>& GetGfxObject() const
  {
    return mGfxTexture;
  }

  void SetId( uint32_t id )
  {
    mId = id;
  }

  uint32_t GetId()
  {
    return mId;
  }

private:

  /**
   * Helper method to apply a sampler to the texture
   * @param[in] context The GL context
   * @param[in] sampler The sampler
   */

  NativeImageInterfacePtr mNativeImage; ///< Pointer to native image
  Render::Sampler mSampler;             ///< The current sampler state
  uint32_t mId;                         ///< The Graphics texture handle
  uint16_t mWidth;                      ///< Width of the texture
  uint16_t mHeight;                     ///< Height of the texture
  uint16_t mMaxMipMapLevel;             ///< Maximum mipmap level
  Type mType:2;                         ///< Type of the texture
  bool mHasAlpha : 1;                   ///< Whether the format has an alpha channel
  bool mIsCompressed : 1;               ///< Whether the format is compressed

  Graphics::API::Accessor<Graphics::API::Texture> mGfxTexture; ///< TODO: find right place to store texture
};


} // namespace Render

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_TEXTURE_H
