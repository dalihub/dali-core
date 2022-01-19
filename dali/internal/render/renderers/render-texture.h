#ifndef DALI_INTERNAL_RENDER_TEXTURE_H
#define DALI_INTERNAL_RENDER_TEXTURE_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
   * Create a texture with a buffer if non-null
   * @param[in] usage How texture will be used
   * @param[in] buffer Buffer to copy
   */
  void CreateWithData(Graphics::TextureUsageFlags usage, uint8_t* buffer, uint32_t bufferSize);

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

private:
  /**
   * Helper method to apply a sampler to the texture
   * @param[in] sampler The sampler
   */
  void ApplySampler(Render::Sampler* sampler);

private:
  Graphics::Controller*                  mGraphicsController;
  Graphics::UniquePtr<Graphics::Texture> mGraphicsTexture;

  NativeImageInterfacePtr mNativeImage; ///< Pointer to native image
  Render::Sampler         mSampler;     ///< The current sampler state

  Pixel::Format mPixelFormat;  ///< Pixel format of the texture
  uint16_t      mWidth;        ///< Width of the texture
  uint16_t      mHeight;       ///< Height of the texture
  Type          mType : 3;     ///< Type of the texture
  bool          mHasAlpha : 1; ///< Whether the format has an alpha channel
};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif //  DALI_INTERNAL_RENDER_TEXTURE_H
