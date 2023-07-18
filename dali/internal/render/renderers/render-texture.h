#ifndef DALI_INTERNAL_RENDER_TEXTURE_H
#define DALI_INTERNAL_RENDER_TEXTURE_H

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
 */

// EXTERNAL INCLUDES
#include <cstdint> // uint16_t, uint32_t
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/images/image-operations.h> // Dali::ImageDimensions
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/texture.h>

#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-texture-create-info.h>
#include <dali/graphics-api/graphics-texture.h>
#include <dali/graphics-api/graphics-types.h>
#include <dali/internal/event/rendering/texture-impl.h>
#include <dali/internal/render/renderers/render-sampler.h>

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
  using Type = Dali::TextureType::Type;

  /**
   * Factory method to return a new texture accessed by key.
   */
  static TextureKey NewKey(Type type, Pixel::Format format, ImageDimensions size);

  /**
   * Factory method to return a new texture accessed by key.
   */
  static TextureKey NewKey(NativeImageInterfacePtr nativeImageInterface);

  /**
   * Constructor
   * @param[in] type The type of the texture
   * @param[in] format The format of the pixel data
   * @param[in] size The size of the texture
   */
  Texture(Type type, Pixel::Format format, ImageDimensions size);

  /**
   * Constructor from native image
   * @param[in] nativeImageInterface The native image
   */
  explicit Texture(NativeImageInterfacePtr nativeImageInterface);

  /**
   * Destructor
   */
  ~Texture();

  /**
   * Deletes the texture from it's global memory pool
   */
  void operator delete(void* ptr);

  static Texture* Get(TextureKey::KeyType);

  /**
   * Get the key of the given renderer in the associated memory pool.
   * @param[in] renderer the given renderer
   * @return The key in the associated memory pool.
   */
  static TextureKey GetKey(const Render::Texture& renderer);

  /**
   * Get the key of the given renderer in the associated memory pool.
   * @param[in] renderer the given renderer
   * @return The key in the associated memory pool, or -1 if not
   * found.
   */
  static TextureKey GetKey(Render::Texture* renderer);

  /**
   * Stores the graphics controller for use when required.
   *
   * @param[in] graphicsController The graphics controller to use
   */
  void Initialize(Graphics::Controller& graphicsController);

  /**
   * Create the texture without a buffer
   * @param[in] usage How texture will be used
   */
  void Create(Graphics::TextureUsageFlags usage);

  /**
   * Deletes the texture from the GPU
   */
  void Destroy();

  /**
   * Uploads data to the texture.
   * @param[in] pixelData A pixel data object
   * @param[in] params Upload parameters. See UploadParams
   */
  void Upload(PixelDataPtr pixelData, const Internal::Texture::UploadParams& params);

  /**
   * Auto generates mipmaps for the texture
   */
  void GenerateMipmaps();

  /**
   * Retrieve whether the texture has an alpha channel
   * @return True if the texture has alpha channel, false otherwise
   */
  [[nodiscard]] bool HasAlphaChannel() const;

  /**
   * Get the graphics object associated with this texture
   */
  [[nodiscard]] Graphics::Texture* GetGraphicsObject() const;

  /**
   * Get the type of the texture
   * @return Type of the texture
   */
  [[nodiscard]] Type GetType() const
  {
    return mType;
  }

  /**
   * Check if the texture is a native image
   * @return if the texture is a native image
   */
  [[nodiscard]] bool IsNativeImage() const
  {
    return static_cast<bool>(mNativeImage);
  }

  /**
   * Return the pixel format of the texture
   * @return The pixel format of the texture data.
   */
  Pixel::Format GetPixelFormat() const
  {
    return mPixelFormat;
  }

  /**
   * Return the width of the texture
   * @return The width of the texture
   */
  uint16_t GetWidth() const
  {
    return mWidth;
  }

  /**
   * Return the height of the texture
   * @return The height of the texture
   */
  uint32_t GetHeight() const
  {
    return mHeight;
  }

  /**
   * Called from RenderManager to notify the texture that current rendering pass has finished.
   */
  void OnRenderFinished();

  /**
   * Set the updated flag.
   * @param[in] updated The updated flag
   */
  void SetUpdated(bool updated)
  {
    mUpdated = updated;
  }

  /**
   * Check if the texture is updated
   * @return True if the texture is updated
   */
  [[nodiscard]] bool Updated()
  {
    if(mUpdated || (mNativeImage && mNativeImage->SourceChanged()))
    {
      return true;
    }
    return false;
  }

  /**
   * Get the updated area
   * @return The updated area if the texture is updated, otherwise an empty area
   * @note The origin of the area is the top-left corner of the texture.
   */
  Rect<uint16_t> GetUpdatedArea();

private:
  /**
   * Helper method to apply a sampler to the texture
   * @param[in] sampler The sampler
   */
  void ApplySampler(Render::Sampler* sampler);

  /**
   * Create a texture with a buffer if non-null
   * @param[in] usage How texture will be used
   * @param[in] buffer Buffer to copy
   * @param[in] bufferSize Size of buffer to copy
   */
  void CreateWithData(Graphics::TextureUsageFlags usage, uint8_t* buffer, uint32_t bufferSize);

private:
  Graphics::Controller*                  mGraphicsController;
  Graphics::UniquePtr<Graphics::Texture> mGraphicsTexture;

  NativeImageInterfacePtr mNativeImage; ///< Pointer to native image
  Render::Sampler         mSampler;     ///< The current sampler state

  Rect<uint16_t> mUpdatedArea{}; ///< Updated area of the texture

  Pixel::Format mPixelFormat; ///< Pixel format of the texture
  uint16_t      mWidth;       ///< Width of the texture
  uint16_t      mHeight;      ///< Height of the texture

  Type mType : 3;     ///< Type of the texture
  bool mHasAlpha : 1; ///< Whether the format has an alpha channel
  bool mUpdated : 1;  ///< Whether the texture is updated

  bool mUseUploadedParameter : 1; ///< Whether ths texture size and format depend on uploaded image or not.
};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif //  DALI_INTERNAL_RENDER_TEXTURE_H
