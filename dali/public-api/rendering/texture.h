#ifndef DALI_TEXTURE_H
#define DALI_TEXTURE_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/images/native-image-interface.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/images/pixel-data.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Texture;
}

namespace TextureType
{

/**
 * @brief Enumeration for texture types.
 * @SINCE_1_1.43
 */
enum Type
{
  TEXTURE_2D,   ///< One 2D image                            @SINCE_1_1.43
  TEXTURE_CUBE  ///< Six 2D images arranged in a cube-shape  @SINCE_1_1.43
};

} // namespace TextureType

namespace CubeMapLayer
{

  /**
   * @brief Faces of a cube map.
   * These constants should be used as the "layer" parameter when uploading a cube-map with Texture::Upload.
   * @SINCE_1_1.43
   */
  const unsigned int POSITIVE_X = 0u; ///< CubeMap image for +x @SINCE_1_1.43
  const unsigned int NEGATIVE_X = 1u; ///< CubeMap image for -x @SINCE_1_1.43
  const unsigned int POSITIVE_Y = 2u; ///< CubeMap image for +y @SINCE_1_1.43
  const unsigned int NEGATIVE_Y = 3u; ///< CubeMap image for -y @SINCE_1_1.43
  const unsigned int POSITIVE_Z = 4u; ///< CubeMap image for +z @SINCE_1_1.43
  const unsigned int NEGATIVE_Z = 5u; ///< CubeMap image for -z @SINCE_1_1.43

} // namespace CubeMapLayer


/**
 * @brief Texture represents a texture object used as input or output by shaders.
 * @SINCE_1_1.43
 */
class DALI_IMPORT_API Texture : public BaseHandle
{
public:

  /**
   * @brief Creates a new Texture object.
   *
   * @SINCE_1_1.43
   * @param[in] type The type of the texture
   * @param[in] format The format of the pixel data
   * @param[in] width The width of the texture
   * @param[in] height The height of the texture
   * @return A handle to a newly allocated Texture
   */
  static Texture New( TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height );

  /**
   * @brief Creates a new Texture object from a native image.
   *
   * @SINCE_1_1.43
   * @param[in] nativeImageInterface A native image
   * @return A handle to a newly allocated Texture
   * @note It is not possible to upload data to textures created from a native image using Upload methods
   * although there might be platform specific APIs to upload data to a native image.
   */
  static Texture New( NativeImageInterface& nativeImageInterface );

  /**
   * @brief Default constructor, creates an empty handle.
   *
   * @SINCE_1_1.43
   */
  Texture();

  /**
   * @brief Destructor.
   *
   * @SINCE_1_1.43
   */
  ~Texture();

  /**
   * @brief Copy constructor, creates a new handle to the same object.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   */
  Texture( const Texture& handle );

  /**
   * @brief Downcasts to a texture.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Texture handle or an uninitialized handle
   */
  static Texture DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Texture& operator=( const Texture& handle );

  /**
   * @brief Uploads data to the texture from a PixelData object.
   *
   * @SINCE_1_1.43
   * @param[in] pixelData The pixelData object
   * @return True if the PixelData object has compatible pixel format and fits within the texture, false otherwise
   */
  bool Upload( PixelData pixelData );

  /**
   * @brief Uploads data to the texture from a PixelData object.
   * @note Upload does not upsample or downsample pixel data to fit the specified rectangular area in the texture.
   *
   * @SINCE_1_1.43
   * @param[in] pixelData The pixelData object
   * @param[in] layer Specifies the layer of a cube map or array texture (Unused for 2D textures). @see CubeMapLayer
   * @param[in] mipmap Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image
   * @param[in] xOffset Specifies an horizontal offset of the rectangular area in the texture that will be updated
   * @param[in] yOffset Specifies a vertical offset of the rectangular area in the texture that will be updated
   * @param[in] width Specifies the width of the rectangular area in the texture that will be updated
   * @param[in] height Specifies the height of the rectangular area in the texture that will be updated
   * @return True if the PixelData object has compatible pixel format and fits in the rectangle specified, false otherwise
   */
  bool Upload( PixelData pixelData,
               unsigned int layer, unsigned int mipmap,
               unsigned int xOffset, unsigned int yOffset,
               unsigned int width, unsigned int height );

  /**
   * @brief Generates mipmaps for the texture.
   * This will auto generate all the mipmaps for the texture based on the data in the base level.
   *
   * @SINCE_1_1.43
   */
  void GenerateMipmaps();

  /**
   * @brief Returns the width of the texture.
   *
   * @SINCE_1_1.43
   * @return The width, in pixels, of the texture
   */
  unsigned int GetWidth() const;

  /**
   * @brief Returns the height of the texture.
   *
   * @SINCE_1_1.43
   * @return The height, in pixels, of the texture
   */
  unsigned int GetHeight() const;

public:

  /**
   * @brief The constructor.
   * @note  Not intended for application developers.
   * @SINCE_1_1.43
   * @param[in] pointer A pointer to a newly allocated Texture
   */
  explicit DALI_INTERNAL Texture( Internal::Texture* pointer );
};

} //namespace Dali

#endif // DALI_TEXTURE_H
