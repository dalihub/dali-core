#ifndef DALI_INTERNAL_RENDER_TEXTURE_H
#define DALI_INTERNAL_RENDER_TEXTURE_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/integration-api/gl-defines.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
struct Sampler;

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
   * Creates the texture in the GPU.
   * Creates the texture and reserves memory for the first mipmap level
   * @param[in] context The GL context
   */
  void Initialize( Context& context );

  /**
   * Deletes the texture from the GPU
   * @param[in] context The GL context
   */
  void Destroy( Context& context );

  /**
   * Called by RenderManager to inform the texture that the context has been destroyed
   */
  void GlContextDestroyed();

  /**
   * Uploads data to the texture.
   * @param[in] context The GL context
   * @param[in] pixelData A pixel data object
   * @param[in] params Upload parameters. See UploadParams
   */
  void Upload( Context& context, PixelDataPtr pixelData, const Internal::Texture::UploadParams& params );

  /**
   * Bind the texture to the given texture unit and applies the given sampler
   * @param[in] context The GL context
   * @param[in] textureUnit the texture unit
   * @param[in] sampler The sampler to be used with the texture
   * @return true if the bind succeeded, false otherwise
   */
  bool Bind( Context& context, unsigned int textureUnit, Render::Sampler* sampler );

  /**
   * Auto generates mipmaps for the texture
   * @param[in] context The GL context
   */
  void GenerateMipmaps( Context& context );

  /**
   * Retrieve wheter the texture has an alpha channel
   * @return True if the texture has alpha channel, false otherwise
   */
  bool HasAlphaChannel();

  /**
   * Get the id of the texture
   * @return Id of the texture
   */
  GLuint GetId()
  {
    return mId;
  }

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

  void SetId( void* textureId )
  {
    mId = *reinterpret_cast<GLuint*>(&textureId);
  }

private:

  /**
   * Helper method to apply a sampler to the texture
   * @param[in] context The GL context
   * @param[in] sampler The sampler
   */
  void ApplySampler( Context& context, Render::Sampler* sampler );

  NativeImageInterfacePtr mNativeImage; ///< Pointer to native image
  Render::Sampler mSampler;             ///< The current sampler state
  GLuint mId;                           ///< Id of the texture
  GLuint mTarget;                       ///< Specifies the target to which the texture is bound.
  GLint mGlInternalFormat;              ///< The gl internal format of the pixel data
  GLenum mGlFormat;                     ///< The gl format of the pixel data
  GLenum mPixelDataType;                ///< The data type of the pixel data
  uint16_t mWidth;                      ///< Width of the texture
  uint16_t mHeight;                     ///< Height of the texture
  uint16_t mMaxMipMapLevel;             ///< Maximum mipmap level
  Type mType:2;                         ///< Type of the texture
  bool mHasAlpha : 1;                   ///< Whether the format has an alpha channel
  bool mIsCompressed : 1;               ///< Whether the format is compressed

};


} // namespace Render

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_TEXTURE_H
