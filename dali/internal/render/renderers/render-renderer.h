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
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/update/manager/prepare-render-instructions.h>

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
class NewRenderer;

/**
 * Renderers are used to render meshes
 * These objects are used during RenderManager::Render(), so properties modified during
 * the Update must either be double-buffered, or set via a message added to the RenderQueue.
 */
class Renderer : public GlResourceOwner
{
public:

  /**
   * Second-phase construction.
   * This is called when the renderer is inside render thread
   * @param[in] context to use
   * @param[in] textureCache to use
   * @param[in] uniformNameCache to use
   */
  void Initialize( Context& context, SceneGraph::TextureCache& textureCache, Render::UniformNameCache& uniformNameCache );

  /**
   * Virtual destructor
   */
  virtual ~Renderer();

  /**
   * Set the Shader used to render.
   * @param[in] shader The shader used to render.
   */
  void SetShader( SceneGraph::Shader* shader );

  /**
   * Set the face-culling mode.
   * @param[in] mode The face-culling mode.
   */
  void SetCullFace( CullFaceMode mode );

  /**
   * Set the sampler used to render the set texture.
   * @param[in] samplerBitfield The packed sampler options used to render.
   */
  void SetSampler( unsigned int samplerBitfield );

  /**
   * Query whether the derived type of Renderer requires depth testing.
   * @return True if the renderer requires depth testing.
   */
  virtual bool RequiresDepthTest() const = 0;

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
   * @param[in] cull Whether to frustum cull this renderer
   */
  void Render( Context& context,
               SceneGraph::TextureCache& textureCache,
               BufferIndex bufferIndex,
               const SceneGraph::NodeDataProvider& node,
               SceneGraph::Shader& defaultShader,
               const Matrix& modelViewMatrix,
               const Matrix& viewMatrix,
               const Matrix& projectionMatrix,
               bool cull,
               bool blend);

  /**
   * Write the renderer's sort attributes to the passed in reference
   *
   * @param[in] bufferIndex The current update buffer index.
   * @param[out] sortAttributes
   */
  virtual void SetSortAttributes( BufferIndex bufferIndex, SceneGraph::RendererWithSortAttributes& sortAttributes ) const;

protected:
  /**
   * Protected constructor; only derived classes can be instantiated.
   * @param dataprovider for rendering
   */
  Renderer();

private:

  // Undefined
  Renderer( const Renderer& );

  // Undefined
  Renderer& operator=( const Renderer& rhs );

  /**
   * @return NewRenderer or NULL if this is an old renderer
   */
  virtual NewRenderer* GetNewRenderer()
  {
    return NULL;
  }

  /**
   * Checks if renderer's resources are ready to be used.
   *
   * @return \e true if they are. Otherwise \e false.
   */
  virtual bool CheckResources() = 0;

  /**
   * Called from Render prior to DoRender().
   * @todo MESH_REWORK Remove after merge
   */
  virtual void DoSetUniforms( Context& context, BufferIndex bufferIndex, SceneGraph::Shader* shader, Program* program );

  /**
   * Called from Render prior to DoRender(). Default method to set CullFaceMode
   * @param context to use
   * @param bufferIndex to use
   */
  virtual void DoSetCullFaceMode( Context& context, BufferIndex bufferIndex );

  /**
   * Called from Render prior to DoRender(). Default method to set blending options
   * @param context to use
   */
  virtual void DoSetBlending( Context& context ) = 0;

  /**
   * Called from Render; implemented in derived classes.
   * @param[in] context The context used for rendering
   * @param[in] textureCache The texture cache used to get textures
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] program to use.
   * @param[in] modelViewMatrix The model-view matrix.
   * @param[in] viewMatrix The view matrix.
   */
  virtual void DoRender( Context& context, SceneGraph::TextureCache& textureCache, const SceneGraph::NodeDataProvider& node, BufferIndex bufferIndex, Program& program, const Matrix& modelViewMatrix, const Matrix& viewMatrix ) = 0;

protected:

  Context* mContext;
  SceneGraph::TextureCache* mTextureCache;
  Render::UniformNameCache* mUniformNameCache;
  SceneGraph::Shader* mShader;
  unsigned int mSamplerBitfield;          ///< Sampler options used for texture filtering

private:

  CullFaceMode mCullFaceMode:3;     ///< cullface enum, 3 bits is enough
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RENDER_RENDERER_H__
