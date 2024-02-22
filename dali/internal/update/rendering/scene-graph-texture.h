#ifndef DALI_INTERNAL_SCENE_GRAPH_TEXTURE_H
#define DALI_INTERNAL_SCENE_GRAPH_TEXTURE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-texture.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/rendering/texture-impl.h>
#include <dali/internal/update/rendering/scene-graph-sampler.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

/**
 * The SceneGraph::Texture object wraps the Graphics texture object, and is owned
 *  by the UpdateManager. It is used by SceneGraphRenderer to set up RenderCommands.
 */
class Texture
{
public:
  typedef Dali::TextureType::Type Type;

  enum class Usage
  {
    SAMPLE,
    COLOR_ATTACHMENT,
    DEPTH_ATTACHMENT
  };

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
   * Initialize the texture object with the Graphics API when added to UpdateManager
   *
   * @param[in] graphics The Graphics API
   */
  void Initialize( Graphics::Controller& graphics );

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

  /**
   * Get the Graphics object associated with this texture
   * @return The graphics object.
   */
  Graphics::Texture* GetGfxObject() const;

  /**
   * Destroy any graphics objects owned by this scene graph object
   */
  void DestroyGraphicsObjects();

public: // From messages
  /**
   * Uploads data to Graphics
   * @param[in] pixelData The pixel data object
   * @param[in] params The parameters for the upload
   */
  void UploadTexture( PixelDataPtr pixelData, const Internal::Texture::UploadParams& params );

  /**
   * Generates mipmaps
   */
  void GenerateMipmaps();

  /**
   * Create a graphics texture using the parameters defined in this object
   */
  void CreateTexture(Usage usage);

  /**
   * Called when the texture is about to be used for drawing.
   */
  void PrepareTexture();

public:
  /**
   * Creates a texture with the specified buffer and buffer size.
   * Buffer may be null.
   */
  void CreateTextureInternal( Usage usage, unsigned char* buffer, unsigned int bufferSize );

private:
  Graphics::Controller*   mGraphicsController; ///< Graphics interface object
  std::unique_ptr<Graphics::Texture> mGraphicsTexture; ///< Graphics texture

  NativeImageInterfacePtr mNativeImage;      ///< Pointer to native image
  SceneGraph::Sampler     mSampler;          ///< The current sampler state
  Pixel::Format           mFormat;           ///< The pixel format of the texture
  uint16_t                mWidth;            ///< Width of the texture
  uint16_t                mHeight;           ///< Height of the texture
  uint16_t                mMaxMipMapLevel;   ///< Maximum mipmap level
  Type                    mType:2;           ///< Type of the texture
  bool                    mHasAlpha : 1;     ///< Whether the format has an alpha channel
  bool                    mIsCompressed : 1; ///< Whether the format is compressed
};

inline void GenerateMipmapsMessage( EventThreadServices& eventThreadServices, SceneGraph::Texture& texture )
{
  typedef Message< SceneGraph::Texture > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &texture, &SceneGraph::Texture::GenerateMipmaps );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_SCENE_GRAPH_TEXTURE_H
