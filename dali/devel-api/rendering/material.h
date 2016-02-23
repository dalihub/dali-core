#ifndef DALI_MATERIAL_H
#define DALI_MATERIAL_H

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <string> // std::string

// INTERNAL INCLUDES
#include <dali/public-api/images/image.h> // Dali::Image
#include <dali/public-api/object/handle.h> // Dali::Handle
#include <dali/devel-api/rendering/sampler.h> // Dali::Sampler
#include <dali/devel-api/rendering/shader.h> // Dali::Shader

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Material;
}

/**
 * @brief Material is a handle to an object that specifies the visual properties of the renderer.
 */
class DALI_IMPORT_API Material : public Handle
{
public:

  /**
   * @brief Creates a new Material object
   *
   * @return A handle to a newly allocated Material
   */
  static Material New( Shader shader );

  /**
   * @brief Default constructor, creates an empty handle
   */
  Material();

  /**
   * @brief Destructor
   */
  ~Material();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @param[in] handle Handle to an object
   */
  Material( const Material& handle );

  /**
   * @brief Downcast to a material handle.
   *
   * If handle is not a material, the returned handle is left uninitialized.
   * @param[in] handle to an object
   * @return material handle or an uninitialized handle
   */
  static Material DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   */
  Material& operator=( const Material& handle );

  /**
   * @brief Sets the Shader used by this material
   *
   * @param[in] shader Handle to a shader
   */
  void SetShader( Shader& shader );

  /**
   * @brief Gets the shader used by this material
   *
   * @return The shader used by the material
   */
  Shader GetShader() const;

  /**
   * @brief Add a new texture to be used by the material
   *
   * @param[in] image The image used by the texture
   * @param[in] uniformName The uniform name of the texture
   * @param[in] sampler Sampling parameters. If not provided the default sampling parameters will be used
   * @return The index of the texture in the array of textures or -1 if texture can not be added
   */
  int AddTexture( Image image, const std::string& uniformName, Sampler sampler = Sampler() );

  /**
   * @brief Removes a texture from the material
   *
   * @param[in] index The index of the texture in the array of textures
   */
  void RemoveTexture( size_t  index );

  /**
   * @brief Sets the image to be used by a given texture
   *
   * @param[in] index The index of the texture in the array of textures
   * @param[in] image The new image
   */
  void SetTextureImage( size_t index, Image image );

  /**
   * @brief Set the sampler used by a given texture
   *
   * @param[in] index The index of the texture in the array of textures
   * @param[in] sampler The new sampler
   */
  void SetTextureSampler( size_t index, Sampler sampler );

  /**
   * @brief Retrieve the sampler of a texture given its texture index
   *
   * @param[in] index The index of the texture in the array of textures
   * @return Returns the sampler of a texture given its texture index
   */
  Sampler GetTextureSampler( size_t index ) const;

  /**
   * @brief Set the uniform name of a given texture
   *
   * @param[in] index The index of the texture in the array of textures
   * @param[in] uniformName The new uniform name
   */
  void SetTextureUniformName( size_t index, const std::string& uniformName );

  /**
   * @brief Retrive the index of a texture given its uniform name
   *
   * @param[in] uniformName the uniform name
   * @returns The index in the array of textures or -1 if the texture is not found
   */
  int GetTextureIndex( const std::string& uniformName ) const;

  /**
   * @brief Retrive the texture given its uniform name
   *
   * @param[in] uniformName the uniform name
   * @returns The image in the array of textures corresponding to the uniformName or an empty handle if the texture is not found
   */
  Image GetTexture( const std::string& uniformName ) const;

  /**
   * @brief Retrive the texture given its index
   *
   * @param[in] index The index in the array of textures
   * @returns The image in the array of textures corresponding to the index or an empty handle if the texture is not found
   */
  Image GetTexture( size_t index ) const;

  /**
   * @brief Retrieve the number of textures used by the material
   */
  std::size_t GetNumberOfTextures() const;

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated Material
   */
  explicit DALI_INTERNAL Material( Internal::Material* pointer );
};

} //namespace Dali



#endif // DALI_MATERIAL_H
