#ifndef DALI_INTERNAL_NEW_TEXTURE_H
#define DALI_INTERNAL_NEW_TEXTURE_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/event-thread-services-holder.h>
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

class Texture : public BaseObject, public EventThreadServicesHolder
{
public:
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
   * @brief Create a new Texture with resourceId.
   *
   * @param[in] type The type of the texture
   * @param[in] resourceId The unique id of this texture combind with TextureUploadManager
   * @return A smart-pointer to the newly allocated Texture.
   */
  static TexturePtr New(TextureType::Type type, uint32_t resourceId);

  /**
   * @brief Get the texture render object
   *
   * @return the texture render object
   */
  Render::TextureKey GetRenderTextureKey() const;

#if defined(ENABLE_GPU_MEMORY_PROFILE)
  /**
   * @copydoc Dali::Texture::Upload()
   */
  bool Upload(PixelDataPtr pixelData, std::string url, int32_t textureId);
#endif

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
   * @copydoc Dali::Integration::GetTextureResourceId()
   */
  uint32_t GetResourceId() const;

  /**
   * @copydoc Dali::Integration::GetTextureType()
   */
  Dali::TextureType::Type GetTextureType() const;

  /**
   * @copydoc Dali::Integration::SetTextureSize()
   */
  void SetSize(const ImageDimensions& size);

  /**
   * @copydoc Dali::Integration::SetTexturePixelFormat()
   */
  void SetPixelFormat(Pixel::Format format);

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
   * @param[in] mask the mask of samplers those uses native image.
   * @return true if the shader has been modified.
   */
  bool ApplyNativeFragmentShader(std::string& shader, int mask);

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
   * Constructor from resource id
   * @param[in] type The type of the texture
   * @param[in] resourceId The resouce id for texture upload manager using
   */
  Texture(TextureType::Type type, uint32_t resourceId);

  /**
   * Second stage initialization of the Texture
   */
  void Initialize();

#if defined(ENABLE_GPU_MEMORY_PROFILE)
  /**
   * @brief Print TotalMemory
   */
  static void PrintTotalMemory();
#endif

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~Texture() override;

private: // unimplemented methods
  Texture(const Texture&);
  Texture& operator=(const Texture&);

private:                                    // data
  Internal::Render::TextureKey mTextureKey; ///<The Render::Texture associated to this texture

  NativeImageInterfacePtr mNativeImage; ///< Pointer to native image
  ImageDimensions         mSize;        ///< Size of the texture
  Dali::TextureType::Type mType;        ///< Texture type (cached)
  Pixel::Format           mFormat;      ///< Pixel format
  uint32_t                mResourceId;
  bool                    mUseUploadedParameter : 1; ///< Whether ths texture size and format depend on uploaded image or not.

#if defined(ENABLE_GPU_MEMORY_PROFILE)
  // For memory profiling
  int32_t     mTextureId;
  uint32_t    mDataSize;
  std::string mUrl;
#endif
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
