#ifndef __DALI_INTERNAL_RENDER_NEW_RENDERER_H__
#define __DALI_INTERNAL_RENDER_NEW_RENDERER_H__

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
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/update/manager/prepare-render-instructions.h>

namespace Dali
{
namespace Internal
{
class PropertyInputImpl;

namespace Render
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
   * @param[in] dataProviders The data providers for the renderer
   * @param[in] renderGeometry The geometry for the renderer
   */
  static NewRenderer* New( SceneGraph::RenderDataProvider* dataProviders, SceneGraph::RenderGeometry* renderGeometry );

  /**
   * Constructor.
   * @param[in] dataProviders The data providers for the renderer
   * @param[in] renderGeometry The geometry for the renderer
   */
  NewRenderer( SceneGraph::RenderDataProvider* dataProviders, SceneGraph::RenderGeometry* renderGeometry );

  virtual ~NewRenderer();

  /**
   * Change the data providers of the renderer
   * @param[in] dataProviders The data providers
   */
  void SetRenderDataProvider( SceneGraph::RenderDataProvider* dataProviders );

  /**
   * Change the geometry used by the renderer
   * @param[in] renderGeometry The new geometry
   */
  void SetGeometry( SceneGraph::RenderGeometry* renderGeometry );

  /**
   * Write the renderer's sort attributes to the passed in reference.
   * @param[in] bufferIndex The buffer index
   * @param[out] sortAttributes
   */
  void SetSortAttributes( BufferIndex bufferIndex, SceneGraph::RendererWithSortAttributes& sortAttributes ) const
  {
    sortAttributes.shader = &( mRenderDataProvider->GetShader() );
    const SceneGraph::RenderDataProvider::Samplers& samplers = mRenderDataProvider->GetSamplers();

    sortAttributes.textureResourceId = samplers.Empty() ? Integration::InvalidResourceId : samplers[ 0 ]->GetTextureId( bufferIndex );
    sortAttributes.geometry = mRenderGeometry;
  }

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
                                   const Matrix& modelViewProjectionMatrix );

  /**
   * @copydoc SceneGraph::Renderer::DoSetUniforms()
   */
  virtual void DoSetUniforms( Context& context, BufferIndex bufferIndex, SceneGraph::Shader* shader, Program* program, unsigned int programIndex );

  /**
   * @copydoc SceneGraph::Renderer::DoSetCullFaceMode
   */
  virtual void DoSetCullFaceMode(Context& context, BufferIndex bufferIndex );

  /**
   * @copydoc SceneGraph::Renderer::DoSetBlending
   */
  virtual void DoSetBlending(Context& context, BufferIndex bufferIndex, bool blend );

  /**
   * @copydoc SceneGraph::Renderer::DoRender()
   */
  virtual void DoRender( Context& context,
                         SceneGraph::TextureCache& textureCache,
                         const SceneGraph::NodeDataProvider& node,
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
   * @param[in] node The node using the renderer
   * @param[in] program The shader program on which to set the uniforms.
   */
  void SetUniforms( BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, Program& program );

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
  void BindTextures( SceneGraph::TextureCache& textureCache,
                     BufferIndex bufferIndex,
                     Program& program,
                     const SceneGraph::RenderDataProvider::Samplers& samplers );

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
  void BindTexture( SceneGraph::TextureCache& textureCache,
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
                     const SceneGraph::SamplerDataProvider& sampler );

  /**
   * Get the texture uniform index of the name sampler in the program.
   * If not already registered in the program, then this performs the registration
   * @param[in] program The shader program
   * @param[in] sampler The sampler holding a texture unit uniform name to search for
   * @return The texture uniform index in the program
   */
  unsigned int GetTextureUnitUniformIndex( Program& program,
                                           const SceneGraph::SamplerDataProvider& sampler );



public: //@todo MESH_REWORK make private after merge with SceneGraph::Renderer
  OwnerPointer< SceneGraph::RenderDataProvider > mRenderDataProvider;

private:
  SceneGraph::RenderGeometry* mRenderGeometry;

  struct TextureUnitUniformIndex
  {
    const SceneGraph::SamplerDataProvider* sampler;
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

  Vector<GLint> mAttributesLocation;
  bool mUpdateAttributesLocation;

};


} // SceneGraph
} // Internal
} // Dali

#endif // __DALI_INTERNAL_RENDER_NEW_RENDERER_H__
