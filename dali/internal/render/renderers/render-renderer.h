#ifndef __DALI_INTERNAL_RENDER_RENDERER_H__
#define __DALI_INTERNAL_RENDER_RENDERER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector4.h>
#include <dali/devel-api/rendering/material.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/update/manager/prepare-render-instructions.h>
#include <dali/internal/render/renderers/render-geometry.h>

namespace Dali
{

namespace Internal
{
class Context;
class Texture;
class Program;

namespace SceneGraph
{
class SceneController;
class Shader;
class TextureCache;
class NodeDataProvider;
}


namespace Render
{
class UniformNameCache;

/**
 * Renderers are used to render meshes
 * These objects are used during RenderManager::Render(), so properties modified during
 * the Update must either be double-buffered, or set via a message added to the RenderQueue.
 */
class Renderer : public GlResourceOwner
{
public:

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlContextDestroyed()
   */
  void GlContextDestroyed();

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlCleanup()
   */
  void GlCleanup();

  /**
   * Create a new renderer instance
   * @param[in] dataProviders The data providers for the renderer
   * @param[in] renderGeometry The geometry for the renderer
   */
  static Renderer* New( SceneGraph::RenderDataProvider* dataProviders, SceneGraph::RenderGeometry* renderGeometry );

  /**
   * Constructor.
   * @param[in] dataProviders The data providers for the renderer
   * @param[in] renderGeometry The geometry for the renderer
   */
  Renderer( SceneGraph::RenderDataProvider* dataProviders, SceneGraph::RenderGeometry* renderGeometry );

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
   * Second-phase construction.
   * This is called when the renderer is inside render thread
   * @param[in] context to use
   * @param[in] textureCache to use
   * @param[in] uniformNameCache to use
   */
  void Initialize( Context& context, SceneGraph::TextureCache& textureCache, Render::UniformNameCache& uniformNameCache );

  /**
   * Destructor
   */
  ~Renderer();

  /**
   * Set the face-culling mode.
   * @param[in] mode The face-culling mode.
   */
  void SetCullFace( Dali::Material::FaceCullingMode mode );

  /**
   * Set the sampler used to render the set texture.
   * @param[in] samplerBitfield The packed sampler options used to render.
   */
  void SetSampler( unsigned int samplerBitfield );

  /**
   * Query whether the derived type of Renderer requires depth testing.
   * @return True if the renderer requires depth testing.
   */
  bool RequiresDepthTest() const;

  /**
   * Called to render during RenderManager::Render().
   * @param[in] context The context used for rendering
   * @param[in] textureCache The texture cache used to get textures
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] node The node using this renderer
   * @param[in] defaultShader in case there is no custom shader
   * @param[in] modelViewMatrix The model-view matrix.
   * @param[in] viewMatrix The view matrix.
   * @param[in] projectionMatrix The projection matrix.
   */
  void Render( Context& context,
               SceneGraph::TextureCache& textureCache,
               BufferIndex bufferIndex,
               const SceneGraph::NodeDataProvider& node,
               SceneGraph::Shader& defaultShader,
               const Matrix& modelViewMatrix,
               const Matrix& viewMatrix,
               const Matrix& projectionMatrix,
               bool blend);

  /**
   * Write the renderer's sort attributes to the passed in reference
   *
   * @param[in] bufferIndex The current update buffer index.
   * @param[out] sortAttributes
   */
  void SetSortAttributes( BufferIndex bufferIndex, SceneGraph::RendererWithSortAttributes& sortAttributes ) const;

private:

  struct UniformIndexMap;

  // Undefined
  Renderer( const Renderer& );

  // Undefined
  Renderer& operator=( const Renderer& rhs );

  /**
   * Sets blending options
   * @param context to use
   * @param blend Wheter blending should be enabled or not
   */
  void SetBlending( Context& context, bool blend );

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
   * @param[in] program The shader program
   */
  void BindTextures( SceneGraph::TextureCache& textureCache, Program& program );

public:

  OwnerPointer< SceneGraph::RenderDataProvider > mRenderDataProvider;

private:

  Context* mContext;
  SceneGraph::TextureCache* mTextureCache;
  Render::UniformNameCache* mUniformNameCache;
  SceneGraph::RenderGeometry* mRenderGeometry;

  struct UniformIndexMap
  {
    unsigned int uniformIndex; // The index of the cached location in the Program
    const PropertyInputImpl* propertyValue;
  };

  typedef Dali::Vector< UniformIndexMap > UniformIndexMappings;
  UniformIndexMappings mUniformIndexMap;

  Vector<GLint> mAttributesLocation;

  unsigned int mSamplerBitfield;                    ///< Sampler options used for texture filtering
  bool mUpdateAttributesLocation:1;                 ///< Indicates attribute locations have changed
  Dali::Material::FaceCullingMode mCullFaceMode:2;  ///< cullface enum, 3 bits is enough
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RENDER_RENDERER_H__
