#ifndef __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__

/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/shader-effects/shader-effect.h>

#include <dali/integration-api/shader-data.h>

#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/type-abstraction-enums.h>

#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/effects/shader-declarations.h>

#include <dali/internal/update/common/property-owner.h>

#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/internal/render/gl-resources/texture-declarations.h>
#include <dali/internal/render/common/render-manager.h>


namespace Dali
{

namespace Integration
{
typedef unsigned int ResourceId;
} // namespace Integration

namespace Internal
{

class ProgramController;
class Program;

namespace SceneGraph
{

class RenderQueue;
class UniformMeta;
class TextureCache;

/**
 * A base class for a collection of shader programs, to apply an effect to different geometry types.
 * This class is also the default shader so its easier to override default behaviour
 */
class Shader : public PropertyOwner
{
public:

  /**
   * Constructor
   * @param hints Geometry hints
   */
  Shader( Dali::ShaderEffect::GeometryHints& hints );

  /**
   * Virtual destructor
   */
  virtual ~Shader();

  /**
   * Second stage initialization, called when added to the UpdateManager
   * @param renderQueue Used to queue messages from update to render thread.
   * @param textureCache Used to retrieve effect textures when rendering.
   */
  void Initialize( RenderQueue& renderQueue, TextureCache& textureCache );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // The following methods are called during UpdateManager::Update()
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Query whether a shader geometry hint is set.
   * @pre The shader has been initialized.
   * @param[in] hint The geometry hint to check.
   * @return True if the given geometry hint is set.
   */
  bool GeometryHintEnabled( Dali::ShaderEffect::GeometryHints hint ) const
  {
    return mGeometryHints & hint;
  }

  /**
   * Retrieve the set of geometry hints.
   * @return The hints.
   */
  Dali::ShaderEffect::GeometryHints GetGeometryHints() const
  {
    return mGeometryHints;
  }

  /**
   * Set the geometry hints.
   * @param[in] hints The hints.
   */
  void SetGeometryHints( Dali::ShaderEffect::GeometryHints hints )
  {
    mGeometryHints = hints;
  }

  /**
   * @return True if the fragment shader outputs only 1.0 on the alpha channel
   *
   * @note Shaders that can output any value on the alpha channel
   * including 1.0 should return false for this.
   */
  bool IsOutputOpaque();

  /**
   * @return True if the fragment shader can output any value but 1.0 on the alpha channel
   *
   * @note Shaders that can output any value on the alpha channel
   * including 1.0 should return false for this
   */
  bool IsOutputTransparent();

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex )
  {
    // no default properties
  }

  /**
   * Set the ID used to access textures
   * @pre This method is not thread-safe, and should only be called from the update-thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] textureId The texture ID.
   */
  void ForwardTextureId( BufferIndex updateBufferIndex, Integration::ResourceId textureId );

  /**
   * Gets the effect texture resource ID
   * This is zero if there is effect texture
   * @return the resource Id
   */
  Integration::ResourceId GetEffectTextureResourceId();

  /**
   * Forwards the meta data from the update thread to the render thread for actual
   * installation. (Installation is to a std::vector, which is not thread safe)
   * @sa InstallUniformMetaInRender
   * @pre This method should only be called from the update thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] meta A pointer to a UniformMeta to be owned by the Shader.
   */
  void ForwardUniformMeta( BufferIndex updateBufferIndex, UniformMeta* meta );

  /**
   * Forwards coordinate type to render
   * @sa InstallUniformMetaInRender
   * @pre This method should only be called from the update thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] index of the metadata.
   * @param[in] type the coordinate type.
   */
  void ForwardCoordinateType( BufferIndex updateBufferIndex, unsigned int index, Dali::ShaderEffect::UniformCoordinateType type );

  /**
   * Forwards the grid density.
   * @pre This method is not thread-safe, and should only be called from the update thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] density The grid density.
   */
  void ForwardGridDensity( BufferIndex updateBufferIndex, float density );

  /**
   * Forwards hints.
   * @pre This method is not thread-safe, and should only be called from the update thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] hint The geometry hints.
   */
  void ForwardHints( BufferIndex updateBufferIndex, Dali::ShaderEffect::GeometryHints hint );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // The following methods are called in Render thread
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Set the ID used to access textures
   * @pre This method is not thread-safe, and should only be called from the render thread.
   * @param[in] textureId The texture ID.
   */
  void SetTextureId( Integration::ResourceId textureId );

  /**
   * Get the texture id, that will be used in the next call to Shader::Apply()
   * @return textureId The texture ID
   */
  Integration::ResourceId GetTextureIdToRender();

  /**
   * Sets grid density
   * @pre This method is not thread-safe, and should only be called from the update thread.
   * @param[in] value The grid density
   */
  void SetGridDensity(float value);

  /**
   * Get the grid density ID.
   * @pre This method is not thread-safe, and should only be called from the render thread.
   * @return The grid density.
   */
  float GetGridDensity();

  /**
   * Installs metadata related to a newly installed uniform property.
   * @pre This method is not thread-safe, and should only be called from the render-thread.
   * @param[in] meta A pointer to a UniformMeta to be owned by the Shader.
   */
  void InstallUniformMetaInRender( UniformMeta* meta );

  /**
   * Sets the uniform coordinate type
   * @param index of the uniform
   * @param type to set
   */
  void SetCoordinateTypeInRender( unsigned int index, Dali::ShaderEffect::UniformCoordinateType type );

  /**
   * @brief Set the program for a geometry type.
   * @param[in] geometryType      The type of the object (geometry) that is to be rendered.
   * @param[in] resourceId        The resource ID for the program.
   * @param[in] shaderData        The program's vertex/fragment source and optionally compiled bytecode
   * @param[in] programCache      Owner of the Programs
   * @param[in] modifiesGeometry  True if the vertex shader changes geometry
   */
  void SetProgram( /** @deprecated This is currently ignored and will soon disappear. */
                   GeometryType geometryType,
                   Integration::ResourceId resourceId,
                   Integration::ShaderDataPtr shaderData,
                   ProgramCache* programCache,
                   bool modifiesGeometry );

  /**
   * Determine if subtypes are required for the given geometry type
   * @deprecated
   * @param[in] geometryType The type of the object (geometry) that is to be rendered.
   * @return TRUE if subtypes are required, FALSE if there is only one subtype available
   */
  bool AreSubtypesRequired(GeometryType geometryType);

  /**
   * Get the program associated with the given type and subtype
   * @deprecated
   * @param[in]  context      the context used to render.
   * @param[in]  type         the type of the object (geometry) that is being rendered.
   * @param[in]  subType      Identifier for geometry types with specialised default shaders
   * @param[out] programIndex returns the program index to be passed onto SetUniforms.
   * @return the program to use.
   */
  Program* GetProgram( Context& context,
                       GeometryType type,
                       ShaderSubTypes subType,
                       unsigned int& programIndex );

  /**
   * Get the program built for this shader
   * @deprecated
   * @return the program to use.
   */
  Program* GetProgram();

  /**
   * Sets the shader specific uniforms including custom uniforms
   * @pre The shader has been initialized.
   * @pre This method is not thread-safe, and should only be called from the render-thread.
   * @param[in] context The context used to render.
   * @param[in] program to use.
   * @param[in] bufferIndex The buffer to read shader properties from.
   * @param[in] type        the type of the object (geometry) that is being rendered.
   * @param[in] subType     Identifier for geometry types with specialised default shaders
   */
  void SetUniforms( Context& context,
                    Program& program,
                    BufferIndex bufferIndex,
                    unsigned int programIndex,
                    ShaderSubTypes subType = SHADER_DEFAULT );

private: // Data

  Dali::ShaderEffect::GeometryHints mGeometryHints;    ///< shader geometry hints for building the geometry
  float                          mGridDensity;      ///< grid density
  Texture*                       mTexture;          ///< Raw Pointer to Texture
  Integration::ResourceId        mRenderTextureId;  ///< Copy of the texture ID for the render thread
  Integration::ResourceId        mUpdateTextureId;  ///< Copy of the texture ID for update thread

  Program*                       mProgram;

  typedef OwnerContainer< UniformMeta* > UniformMetaContainer;
  UniformMetaContainer           mUniformMetadata;     ///< A container of owned UniformMeta values; one for each property in PropertyOwner::mDynamicProperties

  // These members are only safe to access during UpdateManager::Update()
  RenderQueue*                   mRenderQueue;                   ///< Used for queuing a message for the next Render

  // These members are only safe to access in render thread
  TextureCache*                  mTextureCache; // Used for retrieving textures in the render thread
};

// Messages for Shader, to be processed in Update thread.
void SetTextureIdMessage( EventThreadServices& eventThreadServices, const Shader& shader, Integration::ResourceId textureId );
void SetGridDensityMessage( EventThreadServices& eventThreadServices, const Shader& shader, float density );
void SetHintsMessage( EventThreadServices& eventThreadServices, const Shader& shader, Dali::ShaderEffect::GeometryHints hint );
void InstallUniformMetaMessage( EventThreadServices& eventThreadServices, const Shader& shader, UniformMeta& meta );
void SetCoordinateTypeMessage( EventThreadServices& eventThreadServices, const Shader& shader, unsigned int index, Dali::ShaderEffect::UniformCoordinateType type );

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__
