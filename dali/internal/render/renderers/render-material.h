#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_MATERIAL_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_MATERIAL_H__
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
#include <dali/public-api/math/vector4.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/render/shaders/custom-uniform.h>
#include <dali/internal/render/gl-resources/texture-observer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{

namespace Internal
{

class Texture;

namespace SceneGraph
{

class TextureCache;
class RenderMaterial;

/**
 * Helper class to manage uniforms for RenderMaterial. There is a 1-1
 * correspondance between a helper object and a Renderer instance,
 * whereas there is a 1-many relationship between RenderMaterial and
 * Renderer instances.
 */
class RenderMaterialUniforms
{
public:
  /**
   * If the owner's program changes, then this should be called to reset the uniform locations
   */
  void ResetCustomUniforms();

  /**
   * Set the uniform values from the renderMaterial onto the program
   * @param[in] renderMaterial The material from which to get the values
   * @param[in] program The program for which to set the values
   * @param[in] shaderType The sub type of shader program
   */
  void SetUniforms( const RenderMaterial& renderMaterial, Program& program, ShaderSubTypes shaderType );

private:
  static const unsigned int mNumberOfCustomUniforms = 6; ///< Number of material uniforms
  mutable CustomUniform mCustomUniform[SHADER_SUBTYPE_LAST][ mNumberOfCustomUniforms ]; ///< The uniform locations
};


/**
 * Holds values to be stored into uniforms by the renderer
 * Holds texture pointers for binding by the renderer
 */
class RenderMaterial : public TextureObserver
{
public:
  /**
   * Constructor
   */
  RenderMaterial();

  /**
   * Destructor
   */
  virtual ~RenderMaterial();

  /**
   * Secondary stage initialization
   * @param[in] textureCache The GL Texture cache
   */
  void Initialize(TextureCache& textureCache);

  // Message setters

  /**
   * Set the diffuse texture resource id
   * @param[in] textureId The resource id of the texture
   */
  void SetDiffuseTextureId( unsigned int textureId );

  /**
   * Set the opacity texture id
   * @param[in] textureId The resource id of the texture
   */
  void SetOpacityTextureId( unsigned int textureId );

  /**
   * Set the normal / height map texture id
   * @param[in] textureId The resource id of the texture
   */
  void SetNormalMapTextureId( unsigned int textureId );

  /**
   * Set the opacity
   * @param[in] opacity The opacity
   */
  void SetOpacity(float opacity);

  /**
   * Set the shininess
   * @param[in] shininess The shininess
   */
  void SetShininess(float shininess);

  /**
   * Set the ambient color
   * @param[in] color The color
   */
  void SetAmbientColor( const Vector4& color );

  /**
   * Set the diffuse color
   * @param[in] color The color
   */
  void SetDiffuseColor( const Vector4& color );

  /**
   * Set the specular color
   * @param[in] color The color
   */
  void SetSpecularColor( const Vector4& color );

  /**
   * Set the emissive color
   * @param[in] color The color
   */
  void SetEmissiveColor( const Vector4& color );

  /**
   * @return true if a texture has been set on this material, false otherwise
   */
  bool HasTexture() const;

  // Rendering

  /**
   * Set the uniform values for the locations defined in the uniforms structure.
   * @param[in] uniforms The material uniforms
   * @param[in] program The shader program for which to set the uniform values
   * @param[in] shaderType The shader type
   */
  void SetUniforms( RenderMaterialUniforms& uniforms, Program& program, ShaderSubTypes shaderType ) const;

  /**
   * Bind all the valid textures. Will also store valid texture pointer
   * @param[in] program The shader program with which to bind the textures
   * @param[in] textureSampler The texture sampling parameters for the diffuse, opacity and normal textures
   */
  void BindTextures( Program& program, unsigned int textureSampler );

protected: // TextureObserver implementation
  /**
   * @copydoc Dali::Internal::TextureObserver::TextureDiscarded()
   */
  virtual void TextureDiscarded( unsigned int textureId );

private:
  /**
   * Bind a texture.
   * @param[in] program The shader program with which to bind the texture
   * @param[in] textureId the texture id to bind
   * @param[in] texture The texture to bind
   * @param[in] textureUnit Texture unit ( should be a value between 0 and the max number of samplers - 1 )
   * @param[in] samplerIndex The index of the uniform to bind the texture sampler
   */
  void BindTexture( Program& program, unsigned int textureId, Texture* texture, unsigned int textureUnit, Program::UniformType samplerIndex ) const;

private:
  TextureCache*  mTextureCache;       ///< Texture cache of GL resources

  unsigned int   mDiffuseTextureId;   ///< Diffuse texture id
  unsigned int   mOpacityTextureId;   ///< opacity texture id
  unsigned int   mNormalMapTextureId; ///< Normal/Height map texture id

  Texture*       mDiffuseTexture;     ///< Pointer to ready texture
  Texture*       mOpacityTexture;     ///< Pointer to ready texture
  Texture*       mNormalMapTexture;   ///< Pointer to ready texture

  float          mOpacity;            ///< opacity (0 = transparent, 1 = opaque)
  float          mShininess;          ///< value between 0 and 128

  Vector4        mAmbientColor;       ///< Ambient color
  Vector4        mDiffuseColor;       ///< Diffuse color
  Vector4        mSpecularColor;      ///< Specular color
  Vector4        mEmissiveColor;      ///< Emissive color

  friend class RenderMaterialUniforms;
};

} // SceneGraph

} // Internal

} // Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_MATERIAL_H__
