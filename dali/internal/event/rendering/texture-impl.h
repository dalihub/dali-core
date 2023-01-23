#ifndef DALI_INTERNAL_NEW_TEXTURE_H
#define DALI_INTERNAL_NEW_TEXTURE_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/images/pixel-data-impl.h>
#include <dali/internal/render/renderers/render-texture-key.h>
#include <dali/public-api/common/dali-common.h>      // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h>    // Dali::IntrusivePtr
#include <dali/public-api/images/image-operations.h> // Dali::ImageDimensions
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/texture.h> // Dali::Internal::Render::Texture

namespace Dali
{
namespace Internal
{
class Texture;
using TexturePtr = IntrusivePtr<Texture>;

class Texture : public BaseObject
{
public:
  /**
   * @brief Structure used to pass parameters to the Upload method
   */
  struct UploadParams
  {
    uint32_t dataXOffset; ///< Specifies a pixeldata offset in the x direction within the pixeldata buffer.
    uint32_t dataYOffset; ///< Specifies a pixeldata offset in the y direction within the pixeldata buffer.
    uint16_t dataWidth;   ///< Specifies the width of the pixeldata subimage.
    uint16_t dataHeight;  ///< Specifies the height of the pixeldata subimage.
    uint16_t layer;       ///< Specifies the layer of a cube map or array texture
    uint16_t mipmap;      ///< Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
    uint16_t xOffset;     ///< Specifies a texel offset in the x direction within the texture array.
    uint16_t yOffset;     ///< Specifies a texel offset in the y direction within the texture array.
    uint16_t width;       ///< Specifies the width of the texture subimage
    uint16_t height;      ///< Specifies the height of the texture subimage.
  };

  /**
   * @brief Create a new Texture.
   *
   * @param[in] type The type of the texture
   * @param[in] format The format of the pixel data
   * @param[in] width The width of the texture
   * @param[in] height The height of the texture
   * @return A smart-pointer to the newly allocated Texture.
   */
  static TexturePtr New(TextureType::Type type, Pixel::Format format, uint32_t width, uint32_t height);

  /**
   * @brief Creates a new Texture from a native image
   * @param[in] nativeImageInterface The native image
   * @return A smart-pointer to the newly allocated Texture.
   */
  static TexturePtr New(NativeImageInterface& nativeImageInterface);

  /**
   * @brief Get the texture render object
   *
   * @return the texture render object
   */
  Render::TextureKey GetRenderTextureKey() const;

  /**
   * @copydoc Dali::Texture::Upload()
   */
  bool Upload(PixelDataPtr pixelData);

  /**
   * @copydoc Dali::Texture::Upload()
   */
  bool Upload(PixelDataPtr pixelData,
              uint32_t     layer,
              uint32_t     mipmap,
              uint32_t     xOffset,
              uint32_t     yOffset,
              uint32_t     width,
              uint32_t     height);

  /**
   * @copydoc Dali::Texture::UploadSubPixelData()
   */
  bool UploadSubPixelData(PixelDataPtr pixelData,
                          uint32_t     dataXOffset,
                          uint32_t     dataYOffset,
                          uint32_t     dataWidth,
                          uint32_t     dataHeight);

  /**
   * @copydoc Dali::Texture::UploadSubPixelData()
   */
  bool UploadSubPixelData(PixelDataPtr pixelData,
                          uint32_t     dataXOffset,
                          uint32_t     dataYOffset,
                          uint32_t     dataWidth,
                          uint32_t     dataHeight,
                          uint32_t     layer,
                          uint32_t     mipmap,
                          uint32_t     xOffset,
                          uint32_t     yOffset,
                          uint32_t     width,
                          uint32_t     height);

  /**
   * @copydoc Dali::Texture::GenerateMipmaps()
   */
  void GenerateMipmaps();

  /**
   * @copydoc Dali::Texture::GetWidth()
   */
  uint32_t GetWidth() const;

  /**
   * @copydoc Dali::Texture::GetHeight()
   */
  uint32_t GetHeight() const;

  /**
   * @copydoc Dali::Texture::GetPixelFormat()
   */
  Pixel::Format GetPixelFormat() const;

  /**
   * @brief Determine if the texture is a native image
   *
   * @return true if the texture has been initialized with a native image
   */
  bool IsNative() const;

  /**
   * @brief Apply any native texture code to the given fragment shader
   *
   * @param[in,out] shader The fragment shader
   * @return true if the shader has been modified.
   */
  bool ApplyNativeFragmentShader(std::string& shader);

private: // implementation
  /**
   * Constructor
   * @param[in] type The type of the texture
   * @param[in] format The format of the pixel data
   * @param[in] size The size of the texture
   */
  Texture(TextureType::Type type, Pixel::Format format, ImageDimensions size);

  /**
   * Constructor from native image
   * @param[in] nativeImageInterface The native image
   */
  Texture(NativeImageInterfacePtr nativeImageInterface);

  /**
   * Second stage initialization of the Texture
   */
  void Initialize();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~Texture() override;

private: // unimplemented methods
  Texture(const Texture&);
  Texture& operator=(const Texture&);

private:                                               // data
  Internal::EventThreadServices& mEventThreadServices; ///<Used to send messages to the render thread via update thread
  Internal::Render::TextureKey   mTextureKey;          ///<The Render::Texture associated to this texture

  NativeImageInterfacePtr mNativeImage; ///< Pointer to native image
  ImageDimensions         mSize;        ///< Size of the texture
  Dali::TextureType::Type mType;        ///< Texture type (cached)
  Pixel::Format           mFormat;      ///< Pixel format
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::Texture& GetImplementation(Dali::Texture& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Texture handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Texture&>(object);
}

inline const Internal::Texture& GetImplementation(const Dali::Texture& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Texture handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Texture&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_TEXTURE_H
