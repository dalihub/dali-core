#ifndef __DALI_INTERNAL_SCENE_GRAPH_NEW_RENDERER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_NEW_RENDERER_H__

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

#include <dali/integration-api/resource-declarations.h> // For resource id
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/gl-resources/texture-units.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>
#include <dali/internal/render/renderers/render-geometry.h>

namespace Dali
{
namespace Internal
{
class PropertyInputImpl;

namespace SceneGraph
{

/**
 * The new geometry renderer.
 *
 * @todo MESH_REWORK It will be merged into the base class eventually
 */
class NewRenderer : public Renderer
{
public:
  typedef Integration::ResourceId ResourceId;

public:
  /**
   * Create a new renderer instance
   * @param[in] nodeDataProvider The node data provider
   * @param[in] dataProviders The data providers for the renderer
   */
  static NewRenderer* New( NodeDataProvider& nodeDataProvider, RenderDataProvider* dataProviders );
  /**
   * Constructor.
   * @param[in] nodeDataProvider The node data provider
   * @param[in] dataProviders The data providers for the renderer
   */
  NewRenderer( NodeDataProvider& nodeDataProvider, RenderDataProvider* dataProviders );

  virtual ~NewRenderer();

  /**
   * Change the data providers of the renderer
   * @param[in] dataProviders The data providers
   */
  void SetRenderDataProvider( RenderDataProvider* dataProviders );

  /**
   * Set flag that says that Geometry has been updated.
   */
  void SetGeometryUpdated();

public: // Implementation of Renderer
  /**
   * @copydoc SceneGraph::Renderer::RequiresDepthTest()
   */
  virtual bool RequiresDepthTest() const;

  /**
   * @copydoc SceneGraph::Renderer::CheckResources()
   */
  virtual bool CheckResources();

  /**
   * @copydoc SceneGraph::Renderer::IsOutsideClipSpace()
   */
  virtual bool IsOutsideClipSpace( Context& context,
                                   const Matrix& modelMatrix,
                                   const Matrix& modelViewProjectionMatrix );

  /**
   * @copydoc SceneGraph::Renderer::DoSetUniforms()
   */
  virtual void DoSetUniforms( Context& context, BufferIndex bufferIndex, Shader* shader, Program* program, unsigned int programIndex );

  /**
   * @copydoc SceneGraph::Renderer::DoSetCullFaceMode
   */
  virtual void DoSetCullFaceMode(Context& context, BufferIndex bufferIndex );

  /**
   * @copydoc SceneGraph::Renderer::DoSetBlending
   */
  virtual void DoSetBlending(Context& context, BufferIndex bufferIndex );

  /**
   * @copydoc SceneGraph::Renderer::DoRender()
   */
  virtual void DoRender( Context& context,
                         TextureCache& textureCache,
                         BufferIndex bufferIndex,
                         Program& program,
                         const Matrix& modelViewMatrix,
                         const Matrix& viewMatrix );

public: // Implementation of GlResourceOwner

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlContextDestroyed()
   */
  virtual void GlContextDestroyed();

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlCleanup()
   */
  virtual void GlCleanup();

private:
  struct UniformIndexMap;

  /**
   * Set the uniforms from properties according to the uniform map
   * @param[in] program The shader program on which to set the uniforms.
   */
  void SetUniforms( BufferIndex bufferIndex, Program& program );

  /**
   * Set the program uniform in the map from the mapped property
   */
  void SetUniformFromProperty( BufferIndex bufferIndex, Program& program, UniformIndexMap& map );

  /**
   * Bind the material textures in the samplers and setup the samplers
   * @param[in] textureCache The texture cache
   * @param[in] bufferIndex The buffer index
   * @param[in] program The shader program
   * @param[in] samplers The samplers to bind
   */
  void BindTextures( TextureCache& textureCache,
                     BufferIndex bufferIndex,
                     Program& program,
                     const RenderDataProvider::Samplers& samplers );

  /**
   * Bind a material texture to a texture unit, and set the sampler's texture uniform
   * to that texture unit.
   * @param[in] textureCache The texture cache
   * @param[in] program The shader program
   * @param[in] id The resource id of the texture to bind
   * @param[in] texture The texture to bind
   * @param[in] textureUnit The texture unit index to use
   * @param[in] nameIndex The index of the texture uniform in the program
   */
  void BindTexture( TextureCache& textureCache,
                    Program& program,
                    ResourceId id,
                    Texture* texture,
                    TextureUnit textureUnit,
                    unsigned int nameIndex );

  /**
   * Apply the sampler modes to the texture.
   * @param[in] bufferIndex The current buffer index
   * @param[in] texture The texture to which to apply the sampler modes
   * @param[in] textureUnit The texture unit of the texture
   * @param[in] sampler The sampler from which to get the modes.
   */
  void ApplySampler( BufferIndex bufferIndex,
                     Texture* texture,
                     TextureUnit textureUnit,
                     const SamplerDataProvider& sampler );

  /**
   * Get the texture uniform index of the name sampler in the program.
   * If not already registered in the program, then this performs the registration
   * @param[in] program The shader program
   * @param[in] sampler The sampler holding a texture unit uniform name to search for
   * @return The texture uniform index in the program
   */
  unsigned int GetTextureUnitUniformIndex( Program& program,
                                           const SamplerDataProvider& sampler );

public: //@todo MESH_REWORK make private after merge with SceneGraph::Renderer
  OwnerPointer< RenderDataProvider > mRenderDataProvider;

private:
  RenderGeometry mRenderGeometry;

  struct TextureUnitUniformIndex
  {
    const SamplerDataProvider* sampler;
    unsigned int index;
  };

  typedef Dali::Vector< TextureUnitUniformIndex > TextureUnitUniforms;
  TextureUnitUniforms mTextureUnitUniforms;

  struct UniformIndexMap
  {
    unsigned int uniformIndex; // The index of the cached location in the Program
    const PropertyInputImpl* propertyValue;
  };

  typedef Dali::Vector< UniformIndexMap > UniformIndexMappings;
  UniformIndexMappings mUniformIndexMap;
};


} // SceneGraph
} // Internal
} // Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_NEW_RENDERER_H__
