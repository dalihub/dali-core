#ifndef DALI_INTERNAL_SCENE_GRAPH_SAMPLER_H
#define DALI_INTERNAL_SCENE_GRAPH_SAMPLER_H

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
 */

#include <dali/public-api/shader-effects/sampler.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/double-buffered-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-connection-observers.h>
#include <dali/internal/update/resources/bitmap-metadata.h>
#include <dali/internal/render/data-providers/sampler-data-provider.h>

#include <string>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class SceneController;

class Sampler : public PropertyOwner, public SamplerDataProvider
{
public:
  typedef Dali::Sampler::FilterMode FilterMode;
  typedef Dali::Sampler::WrapMode   WrapMode;

  /**
   * Constructor
   */
  Sampler( const std::string& samplerName );

  /**
   * Destructor
   */
  virtual ~Sampler();

  /**
   * Set the uniform name of this sampler. This allows the shader to find the
   * GL index of this sampler.
   */
  void SetUnitName( const std::string& unitName );

  /**
   * Set the texture identity of this sampler (needs to double buffer this value because
   * it can be read through the data provider interface in the render thread )
   * @param[in] bufferIndex The buffer index to use
   * @param[in] textureId The identity of the texture
   */
  void SetTexture( BufferIndex bufferIndex, Integration::ResourceId textureId );

  /**
   * Set the filter modes for minify and magnify filters
   * @param[in] bufferIndex The buffer index to use
   * @param[in] minFilter The minify filter
   * @param[in] magFilter The magnify filter
   */
  void SetFilterMode( BufferIndex bufferIndex, FilterMode minFilter, FilterMode magFilter );

  /**
   * @param[in] bufferIndex The buffer index to use
   */
  void SetWrapMode( BufferIndex bufferIndex, WrapMode uWrap, WrapMode vWrap );

  /**
   * @param[in] bufferIndex The buffer index to use
   * @return true if this sampler affects transparency of the material
   * @note this should only be called from Update thread
   */
  bool AffectsTransparency( BufferIndex bufferIndex ) const;

  /**
   * Sets whether the associated texture is fully opaque or not.
   * @param[in] fullyOpaque true if it's fully opaque
   */
  void SetFullyOpaque( bool fullyOpaque );

  /**
   * @param[in] bufferIndex The buffer index to use
   * @return true if the texture is fully opaque
   * @note this should only be called from Update thread
   */
  bool IsFullyOpaque( BufferIndex bufferIndex ) const;


public: // SamplerDataProvider interface - called from RenderThread
  /**
   * Get the texture unit uniform name
   * @return the name of the texture unit uniform
   */
  virtual const std::string& GetUnitName() const;

  /**
   * Get the texture ID
   * @param[in] bufferIndex The buffer index to use
   * @return the identity of the associated texture
   */
  virtual Integration::ResourceId GetTextureId(BufferIndex buffer) const;

  /**
   * Get the filter mode
   * @param[in] bufferIndex The buffer index to use
   * @return The minify filter mode
   */
  virtual FilterMode GetMinifyFilterMode( BufferIndex bufferIndex ) const;

  /**
   * Get the filter mode
   * @param[in] bufferIndex The buffer index to use
   * @return The magnify filter mode
   */
  virtual FilterMode GetMagnifyFilterMode( BufferIndex bufferIndex ) const;

  /**
   * Get the horizontal wrap mode
   * @param[in] bufferIndex The buffer index to use
   * @return The horizontal wrap mode
   */
  virtual WrapMode GetUWrapMode( BufferIndex bufferIndex ) const;

  /**
   * Get the vertical wrap mode
   * @param[in] bufferIndex The buffer index to use
   * @return The vertical wrap mode
   */
  virtual WrapMode GetVWrapMode( BufferIndex bufferIndex ) const;

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
  /**
   * @copydoc ConnectionObservers::AddObserver
   */
  void AddConnectionObserver(ConnectionObservers::Observer& observer);

  /**
   * @copydoc ConnectionObservers::RemoveObserver
   */
  void RemoveConnectionObserver(ConnectionObservers::Observer& observer);

public: // PropertyOwner implementation
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex );

private:
  std::string mUnitName; ///< The name of the uniform of the texture unit

  DoubleBufferedProperty<unsigned int> mTextureId;
  DoubleBufferedProperty<int> mMinFilter;    ///< The minify filter
  DoubleBufferedProperty<int> mMagFilter;    ///< The magnify filter
  DoubleBufferedProperty<int> mUWrapMode;    ///< The horizontal wrap mode
  DoubleBufferedProperty<int> mVWrapMode;    ///< The vertical wrap mode

  // Note, this is only called from UpdateThread
  DoubleBufferedProperty<bool>     mAffectsTransparency; ///< If this sampler affects renderer transparency

  ConnectionObservers mConnectionObservers; ///< Connection observers that will be informed when textures change.
  bool mFullyOpaque; // Update only flag - no need for double buffering
};

} // namespace SceneGraph

inline void SetUnitNameMessage( EventThreadServices& eventThreadServices, const SceneGraph::Sampler& sampler, const std::string& name )
{
  typedef MessageValue1< SceneGraph::Sampler, std::string > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &sampler, &SceneGraph::Sampler::SetUnitName, name );
}


inline void SetTextureMessage( EventThreadServices& eventThreadServices, const SceneGraph::Sampler& sampler, unsigned int resourceId )
{
  typedef MessageDoubleBuffered1< SceneGraph::Sampler, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &sampler, &SceneGraph::Sampler::SetTexture, resourceId );
}

// Declare enum as a message parameter type outside the SceneGraph namespace
template <> struct ParameterType< SceneGraph::Sampler::FilterMode > : public BasicType< SceneGraph::Sampler::FilterMode > {};


inline void SetFilterModeMessage( EventThreadServices& eventThreadServices, const SceneGraph::Sampler& sampler, SceneGraph::Sampler::FilterMode minFilter, SceneGraph::Sampler::FilterMode magFilter )
{
  typedef MessageDoubleBuffered2< SceneGraph::Sampler, SceneGraph::Sampler::FilterMode, SceneGraph::Sampler::FilterMode > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &sampler, &SceneGraph::Sampler::SetFilterMode, minFilter, magFilter );
}

// Declare enum as a message parameter type
template <> struct ParameterType< SceneGraph::Sampler::WrapMode > : public BasicType< SceneGraph::Sampler::WrapMode > {};


inline void SetWrapModeMessage( EventThreadServices& eventThreadServices, const SceneGraph::Sampler& sampler, SceneGraph::Sampler::WrapMode horizontalWrap, SceneGraph::Sampler::WrapMode verticalWrap )
{
  typedef MessageDoubleBuffered2< SceneGraph::Sampler, SceneGraph::Sampler::WrapMode, SceneGraph::Sampler::WrapMode > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &sampler, &SceneGraph::Sampler::SetWrapMode, horizontalWrap, verticalWrap );
}

} // namespace Internal
} // namespace Dali


#endif //  DALI_INTERNAL_SCENE_GRAPH_SAMPLER_H
