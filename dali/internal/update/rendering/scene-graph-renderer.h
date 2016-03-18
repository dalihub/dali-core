#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDERER_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDERER_H

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
 */

#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h> // Dali::Renderer
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/renderers/render-renderer.h>

namespace Dali
{

namespace Internal
{

namespace Render
{
class Renderer;
class Geometry;
}

namespace SceneGraph
{
class SceneController;

class Renderer;
typedef Dali::Vector< Renderer* > RendererContainer;
typedef RendererContainer::Iterator RendererIter;
typedef RendererContainer::ConstIterator RendererConstIter;

class TextureSet;
class Geometry;

class Renderer :  public PropertyOwner,
                  public UniformMapDataProvider,
                  public UniformMap::Observer,
                  public ConnectionChangePropagator::Observer
{
public:

  enum Opacity
  {
    OPAQUE,
    TRANSPARENT,
    TRANSLUCENT
  };

  /**
   * Construct a new Renderer
   */
  static Renderer* New();

  /**
   * Destructor
   */
  virtual ~Renderer();

  /**
   * Overriden delete operator
   * Deletes the renderer from its global memory pool
   */
  void operator delete( void* ptr );

  /**
   * Set the texture set for the renderer
   * @param[in] textureSet The texture set this renderer will use
   */
  void SetTextures( TextureSet* textureSet );

  /**
   * Returns current texture set object
   * @return Pointer to the texture set
   */
  const TextureSet* GetTextures() const
  {
    return mTextureSet;
  }

  /**
   * Set the shader for the renderer
   * @param[in] shader The shader this renderer will use
   */
  void SetShader( Shader* shader );

  /**
   * Get the shader used by this renderer
   * @return the shader this renderer uses
   */
  const Shader& GetShader() const
  {
    return *mShader;
  }

  /**
   * Set the geometry for the renderer
   * @param[in] geometry The geometry this renderer will use
   */
  void SetGeometry( Render::Geometry* geometry );

  /**
   * Get the geometry of this renderer
   * @return the geometry this renderer uses
   */
  const Render::Geometry& GetGeometry() const
  {
    return *mGeometry;
  }

  /**
   * Set the depth index
   * @param[in] depthIndex the new depth index to use
   */
  void SetDepthIndex( int depthIndex );

  /**
   * @brief Get the depth index
   * @return The depth index
   */
  int GetDepthIndex() const
  {
    return mDepthIndex;
  }

  /**
   * Set the face culling mode
   * @param[in] faceCullingMode to use
   */
  void SetFaceCullingMode( FaceCullingMode::Type faceCullingMode );

  /**
   * Set the blending mode
   * @param[in] blendingMode to use
   */
  void SetBlendMode( BlendMode::Type blendingMode );

  /**
   * Set the blending options. This should only be called from the update thread.
   * @param[in] options A bitmask of blending options.
   */
  void SetBlendingOptions( unsigned int options );

  /**
   * Set the blend color for blending operation
   * @param blendColor to pass to GL
   */
  void SetBlendColor( const Vector4& blendColor );

  /**
   * Set the index of first element for indexed draw
   * @param[in] firstElement index of first element to draw
   */
  void SetIndexedDrawFirstElement( size_t firstElement );

  /**
   * Set the number of elements to draw by indexed draw
   * @param[in] elementsCount number of elements to draw
   */
  void SetIndexedDrawElementsCount( size_t elementsCount );

  /**
   * @brief Set whether the Pre-multiplied Alpha Blending is required
   * @param[in] preMultipled whether alpha is pre-multiplied.
   */
  void EnablePreMultipliedAlpha( bool preMultipled );

  /**
   * Sets the depth buffer write mode
   * @param[in] depthWriteMode The depth buffer write mode
   */
  void SetDepthWriteMode( DepthWriteMode::Type depthWriteMode );

  /**
   * Sets the depth buffer test mode
   * @param[in] depthTestMode The depth buffer test mode
   */
  void SetDepthTestMode( DepthTestMode::Type depthTestMode );

  /**
   * Sets the depth function
   * @param[in] depthFunction The depth function
   */
  void SetDepthFunction( DepthFunction::Type depthFunction );

  /**
   * Sets the render mode
   * @param[in] mode The render mode
   */
  void SetRenderMode( RenderMode::Type mode );

  /**
   * Sets the stencil function
   * @param[in] stencilFunction The stencil function
   */
  void SetStencilFunction( StencilFunction::Type stencilFunction );

  /**
   * Sets the stencil function mask
   * @param[in] stencilFunctionMask The stencil function mask
   */
  void SetStencilFunctionMask( int stencilFunctionMask );

  /**
   * Sets the stencil function reference
   * @param[in] stencilFunctionReference The stencil function reference
   */
  void SetStencilFunctionReference( int stencilFunctionReference );

  /**
   * Sets the stencil mask
   * @param[in] stencilMask The stencil mask
   */
  void SetStencilMask( int stencilMask );

  /**
   * Sets the stencil operation for when the stencil test fails
   * @param[in] stencilOperationOnFail The stencil operation
   */
  void SetStencilOperationOnFail( StencilOperation::Type stencilOperationOnFail );

  /**
   * Sets the stencil operation for when the depth test fails
   * @param[in] stencilOperationOnZFail The stencil operation
   */
  void SetStencilOperationOnZFail( StencilOperation::Type stencilOperationOnZFail );

  /**
   * Sets the stencil operation for when the depth test passes
   * @param[in] stencilOperationOnZPass The stencil operation
   */
  void SetStencilOperationOnZPass( StencilOperation::Type stencilOperationOnZPass );

  /**
   * Turns on batching feature for the renderer
   * @param[in] batchingEnabled if true, enables the batching mode for the renderer
   */
  void SetBatchingEnabled( bool batchingEnabled );

  /**
   * Tests whether batching feature is enabled for this renderer
   * @return batching state
   */
  bool IsBatchingEnabled() const
  {
    return mBatchingEnabled;
  }

  /**
   * Prepare the object for rendering.
   * This is called by the UpdateManager when an object is due to be rendered in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void PrepareRender( BufferIndex updateBufferIndex );

  /*
   * Retrieve the Render thread renderer
   * @return The associated render thread renderer
   */
  Render::Renderer& GetRenderer();

  /**
   * Check whether the renderer has been marked as ready to render
   * ready means that renderer has all resources and should produce correct result
   * complete means all resources have finished loading
   * It's possible that renderer is complete but not ready,
   * for example in case of resource load failed
   * @param[out] ready TRUE if the renderer has resources to render
   * @param[out] complete TRUE if the renderer resources are complete
   */
  void GetReadyAndComplete( bool& ready, bool& complete ) const;

  /**
   * Query whether the renderer is fully opaque, fully transparent or transparent.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return OPAQUE if fully opaque, TRANSPARENT if fully transparent and TRANSLUCENT if in between
   */
  Opacity GetOpacity( BufferIndex updateBufferIndex, const Node& node ) const;

  /**
   * Called by the TextureSet to notify to the renderer that it has changed
   */
  void TextureSetChanged();

public: // Implementation of ObjectOwnerContainer template methods
  /**
   * Connect the object to the scene graph
   *
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

  /**
   * Disconnect the object from the scene graph
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

public: // Implementation of ConnectionChangePropagator
  /**
   * @copydoc ConnectionChangePropagator::AddObserver
   */
  void AddConnectionObserver(ConnectionChangePropagator::Observer& observer){};

  /**
   * @copydoc ConnectionChangePropagator::RemoveObserver
   */
  void RemoveConnectionObserver(ConnectionChangePropagator::Observer& observer){};

public:


public: // UniformMap::Observer
  /**
   * @copydoc UniformMap::Observer::UniformMappingsChanged
   */
  virtual void UniformMappingsChanged( const UniformMap& mappings );

public: // ConnectionChangePropagator::Observer

  /**
   * @copydoc ConnectionChangePropagator::ConnectionsChanged
   */
  virtual void ConnectionsChanged( PropertyOwner& owner );

  /**
   * @copydoc ConnectionChangePropagator::ConnectedUniformMapChanged
   */
  virtual void ConnectedUniformMapChanged( );

  /**
   * @copydoc ConnectionChangePropagator::ConnectedUniformMapChanged
   */
  virtual void ObservedObjectDestroyed(PropertyOwner& owner);

public: // PropertyOwner implementation
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex ){};

public: // From UniformMapDataProvider

  /**
   * @copydoc UniformMapDataProvider::GetUniformMapChanged
   */
  virtual bool GetUniformMapChanged( BufferIndex bufferIndex ) const{ return mUniformMapChanged[bufferIndex];}

  /**
   * @copydoc UniformMapDataProvider::GetUniformMap
   */
  virtual const CollectedUniformMap& GetUniformMap( BufferIndex bufferIndex ) const;

private:

  /**
   * Protected constructor; See also Renderer::New()
   */
  Renderer();

  /**
   * Helper function to create a new render data provider
   * @return the new (initialized) data provider
   */
  RenderDataProvider* NewRenderDataProvider();

private:

  CollectedUniformMap          mCollectedUniformMap[2];           ///< Uniform maps collected by the renderer
  SceneController*             mSceneController;                  ///< Used for initializing renderers
  Render::Renderer*            mRenderer;                         ///< Raw pointer to the renderer (that's owned by RenderManager)
  TextureSet*                  mTextureSet;                       ///< The texture set this renderer uses. (Not owned)
  Render::Geometry*            mGeometry;                         ///< The geometry this renderer uses. (Not owned)
  Shader*                      mShader;                           ///< The shader this renderer uses. (Not owned)
  Vector4*                     mBlendColor;                       ///< The blend color for blending operation

  Dali::Internal::Render::Renderer::StencilParameters mStencilParameters;         ///< Struct containing all stencil related options

  size_t                       mIndexedDrawFirstElement;          ///< first element index to be drawn using indexed draw
  size_t                       mIndexedDrawElementsCount;         ///< number of elements to be drawn using indexed draw
  unsigned int                 mBlendBitmask;                     ///< The bitmask of blending options
  unsigned int                 mRegenerateUniformMap;             ///< 2 if the map should be regenerated, 1 if it should be copied.
  unsigned int                 mResendFlag;                       ///< Indicate whether data should be resent to the renderer

  DepthFunction::Type          mDepthFunction:3;                  ///< Local copy of the depth function
  FaceCullingMode::Type        mFaceCullingMode:2;                ///< Local copy of the mode of face culling
  BlendMode::Type              mBlendMode:2;                      ///< Local copy of the mode of blending
  DepthWriteMode::Type         mDepthWriteMode:2;                 ///< Local copy of the depth write mode
  DepthTestMode::Type          mDepthTestMode:2;                  ///< Local copy of the depth test mode

  bool                         mUniformMapChanged[2];             ///< Records if the uniform map has been altered this frame
  bool                         mResourcesReady;                   ///< Set during the Update algorithm; true if the renderer has resources ready for the current frame.
  bool                         mFinishedResourceAcquisition;      ///< Set during DoPrepareResources; true if ready & all resource acquisition has finished (successfully or otherwise)
  bool                         mPremultipledAlphaEnabled:1;       ///< Flag indicating whether the Pre-multiplied Alpha Blending is required

public:

  bool                         mBatchingEnabled:1;                ///< Flag indicating whether the render supports batching

  int                          mDepthIndex;                       ///< Used only in PrepareRenderInstructions
};


/// Messages
inline void SetTexturesMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, const TextureSet& textureSet )
{
  typedef MessageValue1< Renderer, TextureSet* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::SetTextures, const_cast<TextureSet*>(&textureSet) );
}

inline void SetGeometryMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, const Render::Geometry& geometry )
{
  typedef MessageValue1< Renderer, Render::Geometry* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::SetGeometry, const_cast<Render::Geometry*>(&geometry) );
}

inline void SetShaderMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, Shader& shader )
{
  typedef MessageValue1< Renderer, Shader* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::SetShader, &shader );
}

inline void SetDepthIndexMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, int depthIndex )
{
  typedef MessageValue1< Renderer, int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::SetDepthIndex, depthIndex );
}

inline void SetFaceCullingModeMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, FaceCullingMode::Type faceCullingMode )
{
  typedef MessageValue1< Renderer, FaceCullingMode::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetFaceCullingMode, faceCullingMode );
}

inline void SetBlendModeMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, BlendMode::Type blendingMode )
{
  typedef MessageValue1< Renderer, BlendMode::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetBlendMode, blendingMode );
}

inline void SetBlendingOptionsMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, unsigned int options )
{
  typedef MessageValue1< Renderer, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetBlendingOptions, options );
}

inline void SetBlendColorMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, const Vector4& blendColor )
{
  typedef MessageValue1< Renderer, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetBlendColor, blendColor );
}

inline void SetIndexedDrawFirstElementMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, size_t firstElement )
{
  typedef MessageValue1< Renderer, size_t > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetIndexedDrawFirstElement, firstElement );
}

inline void SetIndexedDrawElementsCountMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, size_t elementsCount )
{
  typedef MessageValue1< Renderer, size_t > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetIndexedDrawElementsCount, elementsCount );
}

inline void SetEnablePreMultipliedAlphaMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, bool preMultiplied )
{
  typedef MessageValue1< Renderer, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::EnablePreMultipliedAlpha, preMultiplied );
}

inline void SetDepthWriteModeMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, DepthWriteMode::Type depthWriteMode )
{
  typedef MessageValue1< Renderer, DepthWriteMode::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetDepthWriteMode, depthWriteMode );
}

inline void SetDepthTestModeMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, DepthTestMode::Type depthTestMode )
{
  typedef MessageValue1< Renderer, DepthTestMode::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetDepthTestMode, depthTestMode );
}

inline void SetDepthFunctionMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, DepthFunction::Type depthFunction )
{
  typedef MessageValue1< Renderer, DepthFunction::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetDepthFunction, depthFunction );
}

inline void SetRenderModeMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, RenderMode::Type mode )
{
  typedef MessageValue1< Renderer, RenderMode::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetRenderMode, mode );
}

inline void SetStencilFunctionMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, StencilFunction::Type stencilFunction )
{
  typedef MessageValue1< Renderer, StencilFunction::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetStencilFunction, stencilFunction );
}

inline void SetStencilFunctionMaskMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, int mask )
{
  typedef MessageValue1< Renderer, int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetStencilFunctionMask, mask );
}

inline void SetStencilFunctionReferenceMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, int stencilFunctionReference )
{
  typedef MessageValue1< Renderer, int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetStencilFunctionReference, stencilFunctionReference );
}

inline void SetStencilMaskMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, int stencilMask )
{
  typedef MessageValue1< Renderer, int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetStencilMask, stencilMask );
}

inline void SetStencilOperationOnFailMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, StencilOperation::Type stencilOperation )
{
  typedef MessageValue1< Renderer, StencilOperation::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetStencilOperationOnFail, stencilOperation );
}

inline void SetStencilOperationOnZFailMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, StencilOperation::Type stencilOperation )
{
  typedef MessageValue1< Renderer, StencilOperation::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetStencilOperationOnZFail, stencilOperation );
}

inline void SetStencilOperationOnZPassMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, StencilOperation::Type stencilOperation )
{
  typedef MessageValue1< Renderer, StencilOperation::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetStencilOperationOnZPass, stencilOperation );
}

inline void SetBatchingEnabledMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, bool batchable )
{
  typedef MessageValue1< Renderer, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetBatchingEnabled, batchable );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_RENDERER_H
