#ifndef DALI_INTERNAL_RENDER_RENDERER_H
#define DALI_INTERNAL_RENDER_RENDERER_H

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

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/update/manager/render-instruction-processor.h>
#include <dali/integration-api/debug.h>

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

/**
 * Renderers are used to render meshes
 * These objects are used during RenderManager::Render(), so properties modified during
 * the Update must either be double-buffered, or set via a message added to the RenderQueue.
 */
class Renderer : public GlResourceOwner
{
public:

  /**
   * @brief Struct to encapsulate stencil parameters required for control of the stencil buffer.
   */
  struct StencilParameters
  {
    StencilParameters( RenderMode::Type renderMode, StencilFunction::Type stencilFunction, int stencilFunctionMask,
                       int stencilFunctionReference, int stencilMask, StencilOperation::Type stencilOperationOnFail,
                       StencilOperation::Type stencilOperationOnZFail, StencilOperation::Type stencilOperationOnZPass )
    : stencilFunctionMask      ( stencilFunctionMask      ),
      stencilFunctionReference ( stencilFunctionReference ),
      stencilMask              ( stencilMask              ),
      renderMode               ( renderMode               ),
      stencilFunction          ( stencilFunction          ),
      stencilOperationOnFail   ( stencilOperationOnFail   ),
      stencilOperationOnZFail  ( stencilOperationOnZFail  ),
      stencilOperationOnZPass  ( stencilOperationOnZPass  )
    {
    }

    int stencilFunctionMask;                          ///< The stencil function mask
    int stencilFunctionReference;                     ///< The stencil function reference
    int stencilMask;                                  ///< The stencil mask
    RenderMode::Type       renderMode:3;              ///< The render mode
    StencilFunction::Type  stencilFunction:3;         ///< The stencil function
    StencilOperation::Type stencilOperationOnFail:3;  ///< The stencil operation for stencil test fail
    StencilOperation::Type stencilOperationOnZFail:3; ///< The stencil operation for depth test fail
    StencilOperation::Type stencilOperationOnZPass:3; ///< The stencil operation for depth test pass
  };

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
   * @param[in] depthTestMode Depth buffer test mode
   * @param[in] depthFunction Depth function
   * @param[in] stencilParameters Struct containing all stencil related options
   */
  static Renderer* New( SceneGraph::RenderDataProvider* dataProviders,
                        Render::Geometry* geometry,
                        unsigned int blendingBitmask,
                        const Vector4* blendColor,
                        FaceCullingMode::Type faceCullingMode,
                        bool preMultipliedAlphaEnabled,
                        DepthWriteMode::Type depthWriteMode,
                        DepthTestMode::Type depthTestMode,
                        DepthFunction::Type depthFunction,
                        StencilParameters& stencilParameters );

  /**
   * Constructor.
   * @param[in] dataProviders The data providers for the renderer
   * @param[in] geometry The geometry for the renderer
   * @param[in] blendingBitmask A bitmask of blending options.
   * @param[in] blendColor The blend color to pass to GL
   * @param[in] faceCullingMode The face-culling mode.
   * @param[in] preMultipliedAlphaEnabled whether alpha is pre-multiplied.
   * @param[in] depthWriteMode Depth buffer write mode
   * @param[in] depthTestMode Depth buffer test mode
   * @param[in] depthFunction Depth function
   * @param[in] stencilParameters Struct containing all stencil related options
   */
  Renderer( SceneGraph::RenderDataProvider* dataProviders,
            Render::Geometry* geometry,
            unsigned int blendingBitmask,
            const Vector4* blendColor,
            FaceCullingMode::Type faceCullingMode,
            bool preMultipliedAlphaEnabled,
            DepthWriteMode::Type depthWriteMode,
            DepthTestMode::Type depthTestMode,
            DepthFunction::Type depthFunction,
            StencilParameters& stencilParameters );

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
   * Retrieves the geometry used by the renderer
   * @return The geometry used by the renderer
   */
  Render::Geometry* GetGeometry() const
  {
    return mGeometry;
  }

  /**
   * Second-phase construction.
   * This is called when the renderer is inside render thread
   * @param[in] context Context used by the renderer
   * @param[in] textureCache The texture cache to use
   */
  void Initialize( Context& context, SceneGraph::TextureCache& textureCache );

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
   * Sets the depth test mode
   * @param[in] depthTestMode The depth test mode
   */
  void SetDepthTestMode( DepthTestMode::Type depthTestMode );

  /**
   * Query the Renderer's depth test mode
   * @return The renderer depth test mode
   */
  DepthTestMode::Type GetDepthTestMode() const;

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
   * Sets the render mode
   * @param[in] renderMode The render mode
   */
  void SetRenderMode( RenderMode::Type mode );

  /**
   * Gets the render mode
   * @return The render mode
   */
  RenderMode::Type GetRenderMode() const;

  /**
   * Sets the stencil function
   * @param[in] stencilFunction The stencil function
   */
  void SetStencilFunction( StencilFunction::Type stencilFunction );

  /**
   * Gets the stencil function
   * @return The stencil function
   */
  StencilFunction::Type GetStencilFunction() const;

  /**
   * Sets the stencil function mask
   * @param[in] stencilFunctionMask The stencil function mask
   */
  void SetStencilFunctionMask( int stencilFunctionMask );

  /**
   * Gets the stencil function mask
   * @return The stencil function mask
   */
  int GetStencilFunctionMask() const;

  /**
   * Sets the stencil function reference
   * @param[in] stencilFunctionReference The stencil function reference
   */
  void SetStencilFunctionReference( int stencilFunctionReference );

  /**
   * Gets the stencil function reference
   * @return The stencil function reference
   */
  int GetStencilFunctionReference() const;

  /**
   * Sets the stencil mask
   * @param[in] stencilMask The stencil mask
   */
  void SetStencilMask( int stencilMask );

  /**
   * Gets the stencil mask
   * @return The stencil mask
   */
  int GetStencilMask() const;

  /**
   * Sets the stencil operation for when the stencil test fails
   * @param[in] stencilOperationOnFail The stencil operation
   */
  void SetStencilOperationOnFail( StencilOperation::Type stencilOperationOnFail );

  /**
   * Gets the stencil operation for when the stencil test fails
   * @return The stencil operation
   */
  StencilOperation::Type GetStencilOperationOnFail() const;

  /**
   * Sets the stencil operation for when the depth test fails
   * @param[in] stencilOperationOnZFail The stencil operation
   */
  void SetStencilOperationOnZFail( StencilOperation::Type stencilOperationOnZFail );

  /**
   * Gets the stencil operation for when the depth test fails
   * @return The stencil operation
   */
  StencilOperation::Type GetStencilOperationOnZFail() const;

  /**
   * Sets the stencil operation for when the depth test passes
   * @param[in] stencilOperationOnZPass The stencil operation
   */
  void SetStencilOperationOnZPass( StencilOperation::Type stencilOperationOnZPass );

  /**
   * Gets the stencil operation for when the depth test passes
   * @return The stencil operation
   */
  StencilOperation::Type GetStencilOperationOnZPass() const;

  /**
   * Sets batching mode on the renderer
   * @param[in] batchingEnabled batching state
   */
  void SetBatchingEnabled( bool batchingEnabled );

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
   * @param[in] size Size of the render item
   * @param[in] externalGeometry Optional external geometry, if set the original geometry is ignored. If NULL, original geometry will be drawn as normal.
   * @param[in] blend If true, blending is enabled
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
               Render::Geometry* externalGeometry,
               bool blend);

  /**
   * Write the renderer's sort attributes to the passed in reference
   *
   * @param[in] bufferIndex The current update buffer index.
   * @param[out] sortAttributes
   */
  void SetSortAttributes( BufferIndex bufferIndex, SceneGraph::RenderInstructionProcessor::SortAttributes& sortAttributes ) const;

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
   * @param[in] context The GL context
   * @param[in] textureCache The texture cache
   * @param[in] program The shader program
   * @return False if create or bind failed, true if success.
   */
  bool BindTextures( Context& context, SceneGraph::TextureCache& textureCache, Program& program );

private:

  OwnerPointer< SceneGraph::RenderDataProvider > mRenderDataProvider;

  Context*                     mContext;
  SceneGraph::TextureCache*    mTextureCache;
  Render::Geometry*            mGeometry;

  struct UniformIndexMap
  {
    unsigned int               uniformIndex;                ///< The index of the cached location in the Program
    const PropertyInputImpl*   propertyValue;
  };

  typedef Dali::Vector< UniformIndexMap > UniformIndexMappings;

  UniformIndexMappings         mUniformIndexMap;
  Vector<GLint>                mAttributesLocation;

  StencilParameters            mStencilParameters;          ///< Struct containing all stencil related options
  BlendingOptions              mBlendingOptions;            ///< Blending options including blend color, blend func and blend equation

  size_t                       mIndexedDrawFirstElement;    ///< Offset of first element to draw
  size_t                       mIndexedDrawElementsCount;   ///< Number of elements to draw

  DepthFunction::Type          mDepthFunction:3;            ///< The depth function
  FaceCullingMode::Type        mFaceCullingMode:2;          ///< The mode of face culling
  DepthWriteMode::Type         mDepthWriteMode:2;           ///< The depth write mode
  DepthTestMode::Type          mDepthTestMode:2;            ///< The depth test mode
  bool                         mUpdateAttributesLocation:1; ///< Indicates attribute locations have changed
  bool                         mPremultipledAlphaEnabled:1; ///< Flag indicating whether the Pre-multiplied Alpha Blending is required
  bool                         mBatchingEnabled:1;          ///< Flag indicating if the renderer is batchable

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_RENDER_RENDERER_H
