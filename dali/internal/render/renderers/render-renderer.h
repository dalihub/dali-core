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
#include <dali/devel-api/rendering/texture-set.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/update/manager/prepare-render-instructions.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
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
   * @param[in] geometry The geometry for the renderer
   * @param[in] blendingBitmask A bitmask of blending options.
   * @param[in] blendColor The blend color to pass to GL
   * @param[in] faceCullingMode The face-culling mode.
   * @param[in] preMultipliedAlphaEnabled whether alpha is pre-multiplied.
   * @param[in] depthWriteMode Depth buffer write mode
   * @param[in] depthFunction Depth function
   */
  static Renderer* New( SceneGraph::RenderDataProvider* dataProviders,
                        Render::Geometry* geometry,
                        unsigned int blendingBitmask,
                        const Vector4* blendColor,
                        FaceCullingMode::Type faceCullingMode,
                        bool preMultipliedAlphaEnabled,
                        DepthWriteMode::Type depthWriteMode,
                        DepthFunction::Type depthFunction );

  /**
   * Constructor.
   * @param[in] dataProviders The data providers for the renderer
   * @param[in] geometry The geometry for the renderer
   * @param[in] blendingBitmask A bitmask of blending options.
   * @param[in] blendColor The blend color to pass to GL
   * @param[in] faceCullingMode The face-culling mode.
   * @param[in] preMultipliedAlphaEnabled whether alpha is pre-multiplied.
   * @param[in] depthWriteMode Depth buffer write mode
   * @param[in] depthFunction Depth function
   */
  Renderer( SceneGraph::RenderDataProvider* dataProviders,
            Render::Geometry* geometry,
            unsigned int blendingBitmask,
            const Vector4* blendColor,
            FaceCullingMode::Type faceCullingMode,
            bool preMultipliedAlphaEnabled,
            DepthWriteMode::Type depthWriteMode,
            DepthFunction::Type depthFunction );

  /**
   * Change the data providers of the renderer
   * @param[in] dataProviders The data providers
   */
  void SetRenderDataProvider( SceneGraph::RenderDataProvider* dataProviders );

  /**
   * Change the geometry used by the renderer
   * @param[in] geometry The new geometry
   */
  void SetGeometry( Render::Geometry* geometry );
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
  void SetFaceCullingMode( FaceCullingMode::Type mode );

  /**
   * Set the bitmask for blending options
   * @param[in] bitmask A bitmask of blending options.
   */
  void SetBlendingBitMask( unsigned int bitmask );

  /**
   * Set the blend color for blending options
   * @param[in] blendColor The blend color to pass to GL
   */
  void SetBlendColor( const Vector4* color );

  /**
   * Set the first element index to draw by the indexed draw
   * @param[in] firstElement index of first element to draw
   */
  void SetIndexedDrawFirstElement( size_t firstElement );

  /**
   * Set the number of elements to draw by the indexed draw
   * @param[in] elementsCount number of elements to draw
   */
  void SetIndexedDrawElementsCount( size_t elementsCount );

  /**
   * @brief Set whether the Pre-multiplied Alpha Blending is required
   *
   * @param[in] preMultipled whether alpha is pre-multiplied.
   */
  void EnablePreMultipliedAlpha( bool preMultipled );

  /**
   * Sets the depth write mode
   * @param[in] depthWriteMode The depth write mode
   */
  void SetDepthWriteMode( DepthWriteMode::Type depthWriteMode );

  /**
   * Query the Renderer's depth write mode
   * @return The renderer depth write mode
   */
  DepthWriteMode::Type GetDepthWriteMode() const;

  /**
   * Sets the depth function
   * @param[in] depthFunction The depth function
   */
  void SetDepthFunction( DepthFunction::Type depthFunction );

  /**
   * Query the Renderer's depth function
   * @return The renderer depth function
   */
  DepthFunction::Type GetDepthFunction() const;

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
               const Matrix& modelMatrix,
               const Matrix& modelViewMatrix,
               const Matrix& viewMatrix,
               const Matrix& projectionMatrix,
               const Vector3& size,
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
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] node The node using the renderer
   * @param[in] size The size of the renderer
   * @param[in] program The shader program on which to set the uniforms.
   */
  void SetUniforms( BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, const Vector3& size, Program& program );

  /**
   * Set the program uniform in the map from the mapped property
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] program The shader program
   * @param[in] map The uniform
   */
  void SetUniformFromProperty( BufferIndex bufferIndex, Program& program, UniformIndexMap& map );

  /**
   * Bind the textures and setup the samplers
   * @param[in] textureCache The texture cache
   * @param[in] program The shader program
   * @return False if create or bind failed, true if success.
   */
  bool BindTextures( SceneGraph::TextureCache& textureCache, Program& program );

private:

  OwnerPointer< SceneGraph::RenderDataProvider > mRenderDataProvider;

  Context* mContext;
  SceneGraph::TextureCache* mTextureCache;
  Render::UniformNameCache* mUniformNameCache;
  Render::Geometry* mGeometry;

  struct UniformIndexMap
  {
    unsigned int uniformIndex; // The index of the cached location in the Program
    const PropertyInputImpl* propertyValue;
  };

  typedef Dali::Vector< UniformIndexMap > UniformIndexMappings;
  UniformIndexMappings mUniformIndexMap;

  Vector<GLint> mAttributesLocation;

  BlendingOptions       mBlendingOptions; /// Blending options including blend color, blend func and blend equation
  FaceCullingMode::Type mFaceCullingMode; /// Mode of face culling
  DepthWriteMode::Type  mDepthWriteMode;  /// Depth write mode
  DepthFunction::Type   mDepthFunction;   /// Depth function

  size_t mIndexedDrawFirstElement;                  /// Offset of first element to draw
  size_t mIndexedDrawElementsCount;                 /// Number of elements to draw

  bool mUpdateAttributesLocation:1;                 ///< Indicates attribute locations have changed
  bool mPremultipledAlphaEnabled:1;                 ///< Flag indicating whether the Pre-multiplied Alpha Blending is required
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RENDER_RENDERER_H__
