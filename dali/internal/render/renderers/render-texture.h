#ifndef DALI_INTERNAL_RENDER_TEXTURE_H
#define DALI_INTERNAL_RENDER_TEXTURE_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/graphics-api/graphics-texture-upload-helper.h>
#include <dali/graphics-api/graphics-texture.h>
#include <dali/graphics-api/graphics-types.h>
#include <dali/internal/event/rendering/texture-impl.h>
#include <dali/internal/render/renderers/render-sampler.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class RenderManager;
} // namespace SceneGraph

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
   * Factory method to return a new texture accessed by key.
   */
  static TextureKey NewKey(Type type, uint32_t resourceId);

  /**
   * Clear memory pool of texture.
   * This should be called at the begin of Core.
   * (Since Core could be recreated, we need to reset the memory pool.)
   * After this API call, all Render::Texture classes are invalid.
   */
  static void ResetMemoryPool();

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
   * Constructor from resource id
   * @param[in] type The type of the texture
   * @param[in] resourceId The resouce id for texture upload manager using
   */
  explicit Texture(Type type, uint32_t resourceId);

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
   * @param[in] renderManager The render manager to be used.
   */
  void Initialize(Graphics::Controller& graphicsController, SceneGraph::RenderManager& renderManager);

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
  void Upload(PixelDataPtr pixelData, const Graphics::UploadParams& params);

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
  [[nodiscard]] Graphics::Texture* GetGraphicsObject();

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
  uint16_t GetHeight() const
  {
    return mHeight;
  }

  /**
   * Set the pixel format of the texture
   * @param[in] pixelFormat The pixel format of the texture data.
   */
  void SetPixelFormat(Pixel::Format pixelFormat)
  {
    mPixelFormat = pixelFormat;
  }

  /**
   * Set the width of the texture
   * @param[in] width The width of the texture
   */
  void SetWidth(uint16_t width)
  {
    mWidth = width;
  }

  /**
   * Set the height of the texture
   * @param[in] height The height of the texture
   */
  void SetHeight(uint16_t height)
  {
    mHeight = height;
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
    if(mUpdated || (mNativeImage && mNativeImage->SourceChanged()) || (mResourceId != 0u && IsGraphicsObjectChanged()))
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
   * Create the texture without a buffer
   * @param[in] usage How texture will be used
   * @param[in] allocationPolicy Policy of texture allocation
   */
  void Create(Graphics::TextureUsageFlags usage, Graphics::TextureAllocationPolicy allocationPolicy);

  /**
   * Create a texture with a buffer if non-null
   * @param[in] usage How texture will be used
   * @param[in] allocationPolicy Policy of texture allocation
   * @param[in] buffer Buffer to copy
   * @param[in] bufferSize Size of buffer to copy
   */
  void CreateWithData(Graphics::TextureUsageFlags usage, Graphics::TextureAllocationPolicy allocationPolicy, uint8_t* buffer, uint32_t bufferSize);

  /**
   * @brief Check whether graphics object pointer were changed what we know before.
   *
   * @return True if graphics object created newly. False otherwised.
   */
  bool IsGraphicsObjectChanged();

  /**
   * @brief Notify to RenderManager that this texture has been updated.
   */
  void NotifyTextureUpdated();

private:
  Graphics::Controller*                  mGraphicsController;
  SceneGraph::RenderManager*             mRenderManager;
  Graphics::UniquePtr<Graphics::Texture> mGraphicsTexture;

  NativeImageInterfacePtr mNativeImage; ///< Pointer to native image
  Render::Sampler         mSampler;     ///< The current sampler state

  Rect<uint16_t> mUpdatedArea{}; ///< Updated area of the texture

  Pixel::Format mPixelFormat; ///< Pixel format of the texture
  uint16_t      mWidth;       ///< Width of the texture
  uint16_t      mHeight;      ///< Height of the texture

  uint32_t           mResourceId;
  Graphics::Texture* mLatestUsedGraphicsTexture{nullptr};

  Type mType : 3;     ///< Type of the texture
  bool mHasAlpha : 1; ///< Whether the format has an alpha channel
  bool mUpdated : 1;  ///< Whether the texture is updated

  bool mUseUploadedParameter : 1; ///< Whether ths texture size and format depend on uploaded image or not.
};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif //  DALI_INTERNAL_RENDER_TEXTURE_H
