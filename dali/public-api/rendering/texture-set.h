#ifndef DALI_TEXTURE_SET_H
#define DALI_TEXTURE_SET_H

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

// EXTERNAL INCLUDES
#include <cstddef> // std::size_t

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>     // Dali::Handle
#include <dali/public-api/rendering/sampler.h> // Dali::Sampler
#include <dali/public-api/rendering/shader.h>  // Dali::Shader
#include <dali/public-api/rendering/texture.h> // Dali::Texture

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class TextureSet;
}

/**
 * @brief TextureSet is a handle to an object that specifies the set of images used as textures by a renderer.
 * The images have to be ordered in the same order they are declared in the shader.
 * @SINCE_1_1.43
 */
class DALI_IMPORT_API TextureSet : public BaseHandle
{
public:

  /**
   * @brief Creates a new TextureSet object.
   *
   * @SINCE_1_1.43
   * @return A handle to a newly allocated TextureSet
   */
  static TextureSet New();

  /**
   * @brief Default constructor, creates an empty handle.
   *
   * @SINCE_1_1.43
   */
  TextureSet();

  /**
   * @brief Destructor.
   *
   * @SINCE_1_1.43
   */
  ~TextureSet();

  /**
   * @brief Copy constructor, creates a new handle to the same object.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   */
  TextureSet( const TextureSet& handle );

  /**
   * @brief Downcasts to a TextureSet handle.
   * If handle is not a TextureSet, the returned handle is left uninitialized.
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return TextureSet handle or an uninitialized handle
   */
  static TextureSet DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return A reference to this
   */
  TextureSet& operator=( const TextureSet& handle );

  /**
   * @brief Sets the texture at position "index".
   *
   * @SINCE_1_1.43
   * @param[in] index The position in the texture set of the texture
   * @param[in] texture The texture
   */
  void SetTexture( size_t index, Texture texture );

  /**
   * @brief Gets the image at position "index".
   *
   * @SINCE_1_1.43
   * @param[in] index The position in the texture set of the image
   * @return A handle to the image at the the specified position
   */
  Texture GetTexture( size_t index ) const;

  /**
   * @brief Sets the sampler to be used by the image at position "index".
   *
   * @SINCE_1_1.43
   * @param[in] index The position in the texture set of the sampler
   * @param[in] sampler The sampler to use
   */
  void SetSampler( size_t index, Sampler sampler );

  /**
   * @brief Sets the sampler to be used by the image at position "index".
   *
   * @SINCE_1_1.43
   * @param[in] index The position in the texture set of the image
   * @return A handle to the sampler at the specified position
   */
  Sampler GetSampler( size_t index ) const;

  /**
   * @brief Gets the number of textures present in the TextureSet.
   *
   * @SINCE_1_1.43
   * @return The number of textures in the TextureSet
   */
  size_t GetTextureCount() const;

public:

  /**
   * @brief The constructor.
   * @note  Not intended for application developers.
   * @SINCE_1_1.43
   * @param[in] pointer A pointer to a newly allocated TextureSet
   */
  explicit DALI_INTERNAL TextureSet( Internal::TextureSet* pointer );
};

} //namespace Dali


#endif // DALI_TEXTURE_SET_H
