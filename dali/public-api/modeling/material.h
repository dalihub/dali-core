#ifndef __DALI_MATERIAL_H__
#define __DALI_MATERIAL_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/image.h>
#include <dali/public-api/common/constants.h>

namespace Dali
{

struct Vector4;

namespace Internal DALI_INTERNAL
{
class Material;
}


class Material;
typedef std::vector<Material>             MaterialContainer; ///< Container for Dali::Material
typedef MaterialContainer::iterator       MaterialIter;      ///< Iterator for Dali::MaterialContainer
typedef MaterialContainer::const_iterator MaterialConstIter; ///< Const iterator for Dali::MaterialContainer

/**
 * @brief Encapsulates the data describing a material.
 *
 * Color, opacity, shininess, shading mode, texture/normal/height mapping...
 */
class DALI_IMPORT_API Material : public BaseHandle
{
public:
  /**
   * @brief How the material's texture is sampled outside the range 0..1
   */
  enum MappingMode
  {
    MAPPING_MODE_WRAP   = 0x0,            ///< A texture coordinate u|v is translated to u%1|v%1
    MAPPING_MODE_CLAMP  = 0x1,            ///< Texture coordinates outside [0...1] are clamped to the nearest valid value
    MAPPING_MODE_MIRROR = 0x2,            ///< A texture coordinate u|v becomes u%1|v%1 if (u-(u%1))%2 is zero and 1-(u%1)|1-(v%1) otherwise
    MAPPING_MODE_DECAL  = 0x3,            ///< If the texture coordinates for a pixel are outside [0...1] the texture is not applied to that pixel
  };

  static const float        DEFAULT_OPACITY;          ///< 1.0f
  static const float        DEFAULT_SHININESS;        ///< 0.5f
  static const Vector4      DEFAULT_AMBIENT_COLOR;    ///< (0.2f, 0.2f, 0.2f, 1.0f)
  static const Vector4      DEFAULT_DIFFUSE_COLOR;    ///< (0.8f, 0.8f, 0.8f, 1.0f)
  static const Vector4      DEFAULT_SPECULAR_COLOR;   ///< (0.0f, 0.0f, 0.0f, 1.0f)
  static const Vector4      DEFAULT_EMISSIVE_COLOR;   ///< (0.0f, 0.0f, 0.0f, 1.0f)
  static const MappingMode  DEFAULT_MAPPING_MODE;     ///< Material::MAPPING_MODE_WRAP
  static const size_t       DEFAULT_DIFFUSE_UV_INDEX; ///< 0
  static const size_t       DEFAULT_OPACITY_UV_INDEX; ///< 0
  static const size_t       DEFAULT_NORMAL_UV_INDEX;  ///< 0
  static const bool         DEFAULT_HAS_HEIGHT_MAP;   ///< false

  /**
   * @brief Create an initialized Material.
   *
   * @param[in] name The material's name
   * @return A handle to a newly allocated Dali resource.
   */
  static Material New(const std::string& name);

  /**
   * @brief Downcast an Object handle to Material handle.
   *
   * If handle points to a Material object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Material object or an uninitialized handle
   */
  static Material DownCast( BaseHandle handle );

  /**
   * @brief Create an uninitialized material; this can be initialized with Material::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Material();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Material();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Material(const Material& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Material& operator=(const Material& rhs);

  /**
   * @brief Set the material's name.
   *
   * @param[in] name The material's name
   */
  void SetName(const std::string& name);

  /**
   * @brief Get the material's name.
   */
  const std::string& GetName() const;

  /**
   * @brief Set the material's opacity.
   *
   * @param[in] opacity The new opacity value
   */
  void SetOpacity(const float opacity);

  /**
   * @brief Get the material's opacity.
   *
   * @return The opacity.
   */
  float GetOpacity() const;

  /**
   * @brief Set the material's shininess (used for specular highlighting).
   *
   * @param[in] shininess The new shininess value
   */
  void SetShininess(const float shininess);

  /**
   * @brief Get the material's shininess.
   *
   * @return The shininess.
   */
  float GetShininess() const;

  /**
   * @brief Set the material's ambient color.
   *
   * @param[in] color The new color value
   */
  void SetAmbientColor(const Vector4& color);

  /**
   * @brief Get the material's ambient color.
   *
   * @return The color value
   */
  const Vector4& GetAmbientColor() const;

  /**
   * @brief Set the material's diffuse color.
   *
   * @param[in] color The new color value
   */
  void SetDiffuseColor(const Vector4& color);

  /**
   * @brief Get the material's diffuse color.
   *
   * @return The color value
   */
  const Vector4& GetDiffuseColor() const;

  /**
   * @brief Set the material's specular color.
   *
   * @param[in] color The new color value
   */
  void SetSpecularColor(const Vector4& color);

  /**
   * @brief Get the material's specular color.
   *
   * @return The color value
   */
  const Vector4& GetSpecularColor() const;

  /**
   * @brief Set the material's emissive color.
   *
   * @param[in] color The new color value
   */
  void SetEmissiveColor(const Vector4& color);

  /**
   * @brief Get the material's emissive color.
   *
   * @return The color value
   */
  const Vector4& GetEmissiveColor() const;

  /**
   * @brief Set the diffuse texture image.
   *
   * @param[in] image The new texture image
   */
  void SetDiffuseTexture(Image image);

  /**
   * @brief Set the diffuse texture image.
   *
   * @param[in] filename The name of the image file
   */
  void SetDiffuseTextureFileName(const std::string filename);

  /**
   * @brief Get the diffuse texture image.
   *
   * @return The texture image
   */
  Image GetDiffuseTexture() const;

  /**
   * @brief Get the diffuse texture image filename.
   *
   * @return the filename
   */
  const std::string& GetDiffuseFileName() const;

  /**
   * @brief Set the opacity texture image.
   *
   * @param[in] image The new texture image
   */
  void SetOpacityTexture(Image image);

  /**
   * @brief Set the opacity texture image.
   *
   * @param[in] filename The opacity texture image filename
   */
  void SetOpacityTextureFileName(const std::string filename);

  /**
   * @brief Get the opacity texture image.
   *
   * @return The texture image
   */
  Image GetOpacityTexture() const;

  /**
   * @brief Get the opacity texture image filename.
   *
   * @return The texture image's filename
   */
  const std::string& GetOpacityTextureFileName() const;


  /**
   * @brief Set the normal/height map texture image.
   *
   * @param[in] image The new texture image
   */
  void SetNormalMap(Image image);

  /**
   * @brief Set the normal/height map texture image filename.
   *
   * @param[in] filename The new texture image filename
   */
  void SetNormalMapFileName(const std::string filename);

  /**
   * @brief Get the normal/height map texture image.
   *
   * @return The texture image
   */
  Image GetNormalMap() const;

  /**
   * @brief Get the normal/height map texture image filename.
   *
   * @return The texture image filename
   */
  const std::string& GetNormalMapFileName() const;

  /**
   * @brief Set the U mapping mode.
   *
   * @param[in] map The mapping mode
   */
  void SetMapU(const unsigned int map);

  /**
   * @brief Get the U mapping mode.
   *
   * @return The mapping mode
   */
  const unsigned int GetMapU() const;

  /**
   * @brief Set the V mapping mode.
   *
   * @param[in] map The mapping mode
   */
  void SetMapV(const unsigned int map);

  /**
   * @brief Get the U mapping mode.
   *
   * @return The mapping mode
   */
  const unsigned int GetMapV() const;

  /**
   * @brief Set the index into the bound mesh's array of UV's for the diffuse texture coordinates.
   *
   * @param[in] index The diffuse uv index
   */
  void SetDiffuseUVIndex(const int index);

  /**
   * @brief Get the index into the bound mesh's array of UV's for the diffuse texture coordinates.
   *
   * @return The uv index
   */
  const unsigned int GetDiffuseUVIndex() const;

  /**
   * @brief Set the index into the bound mesh's array of UV's for the opacity texture coordinates.
   *
   * @param[in] index The opacity uv index
   */
  void SetOpacityUVIndex(const int index);

  /**
   * @brief Get the index into the bound mesh's array of UV's for the opacity texture coordinates.
   *
   * @return The uv index
   */
  const unsigned int GetOpacityUVIndex() const;

  /**
   * @brief Set the index into the bound mesh's array of UV's for the normal/height map texture coordinates.
   *
   * @param[in] index The normal/height map uv index
   */
  void SetNormalUVIndex(const int index);

  /**
   * @brief Get the index into the bound mesh's array of UV's for the normal/height map texture coordinates.
   *
   * @return The uv index
   */
  const unsigned int GetNormalUVIndex() const;

  /**
   * @brief Set whether the normal texture contains a normal or height map.
   *
   * @param[in] flag true if the normal map contains a height map
   */
  void SetHasHeightMap(const bool flag);

  /**
   * @brief Get whether the normal texture contains a normal or height map.
   *
   * @return true if the normal map contains a height map, false otherwise
   */
  const bool GetHasHeightMap() const;

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] material A pointer to an internal material resource
   */
  explicit DALI_INTERNAL Material(Internal::Material* material);
}; // class Material

} // namespace Dali

#endif // __DALI_MATERIAL_H__
