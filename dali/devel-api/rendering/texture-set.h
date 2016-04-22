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
#include <dali/public-api/images/image.h> // Dali::Image
#include <dali/public-api/object/handle.h> // Dali::Handle
#include <dali/devel-api/rendering/sampler.h> // Dali::Sampler
#include <dali/devel-api/rendering/shader.h> // Dali::Shader

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class TextureSet;
}

/**
 * @brief TextureSet is a handle to an object that specifies the set of images used as textures by a renderer
 * The images have to be ordered in the same order they are declared in the shader
 */
class DALI_IMPORT_API TextureSet : public Handle
{
public:

  /**
   * @brief Creates a new TextureSet object
   *
   * @return A handle to a newly allocated TextureSet
   */
  static TextureSet New();

  /**
   * @brief Default constructor, creates an empty handle
   */
  TextureSet();

  /**
   * @brief Destructor
   */
  ~TextureSet();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @param[in] handle Handle to an object
   */
  TextureSet( const TextureSet& handle );

  /**
   * @brief Downcast to a TextureSet handle.
   *
   * If handle is not a TextureSet, the returned handle is left uninitialized.
   * @param[in] handle to an object
   * @return TextureSet handle or an uninitialized handle
   */
  static TextureSet DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   */
  TextureSet& operator=( const TextureSet& handle );

  /**
   * @brief Set the image at position "index"
   * @param[in] index The position in the texture set of the image
   * @param[in] image The image
   */
  void SetImage( size_t index, Image image );

  /**
   * @brief Get the image at position "index"
   * @param[in] index The position in the texture set of the image
   * @return A handle to the image at the the specified position
   */
  Image GetImage( size_t index ) const;

  /**
   * @brief Set the sampler to be used by the image at position "index"
   * @param[in] index The position in the texture set of the sampler
   * @param[in] sampler The sampler to use
   */
  void SetSampler( size_t index, Sampler sampler );

  /**
   * @brief Set the sampler to be used by the image at position "index"
   * @param[in] index The position in the texture set of the image
   * @return A handle to the sampler at the specified position
   */
  Sampler GetSampler( size_t index ) const;

  /**
   * @brief Get the number of textures present in the TextureSet
   *
   * @return The number of textures in the TextureSet
   */
  size_t GetTextureCount() const;

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated TextureSet
   */
  explicit DALI_INTERNAL TextureSet( Internal::TextureSet* pointer );
};

} //namespace Dali



#endif // DALI_TEXTURE_SET_H
