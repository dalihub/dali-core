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
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/resources/bitmap-metadata.h>
#include <dali/internal/render/data-providers/sampler-data-provider.h>

#include <string>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

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
   * @param[in] bitmapMetadata The metadata for the texture
   */
  void SetTexture( BufferIndex bufferIndex, Integration::ResourceId textureId, const BitmapMetadata& bitmapMetadata );

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

private:
  std::string mUnitName; ///< The name of the uniform of the texture unit

  // @todo MESH_REWORK Need these to automatically copy
  // new value into old value on frame change

  ResourceId mTextureId[2]; ///< The identity of the associated texture for this frame (Can be read from RenderThread)

  BitmapMetadata mBitmapMetadata[2]; /// The meta data of the associated texture for this frame (Not needed in RenderThread)

  // @todo MESH_REWORK These need to be non-animatable properties (that also copy
  // new values into old on frame change ) that can be read in RenderThread
  DoubleBuffered<FilterMode> mMinFilter;    ///< The minify filter
  DoubleBuffered<FilterMode> mMagFilter;    ///< The magnify filter
  DoubleBuffered<WrapMode> mUWrapMode;      ///< The horizontal wrap mode
  DoubleBuffered<WrapMode> mVWrapMode;      ///< The vertical wrap mode

  // Note, this is only called from UpdateThread
  DoubleBuffered<bool>     mAffectsTransparency; ///< If this sampler affects renderer transparency
};

inline void SetUniformNameMessage( EventToUpdate& eventToUpdate, const Sampler& sampler, const std::string& name )
{
  typedef MessageValue1< Sampler, std::string > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &sampler, &Sampler::SetUnitName, name );
}

} // namespace SceneGraph

// Declare enum as a message parameter type outside the SceneGraph namespace
template <> struct ParameterType< SceneGraph::Sampler::FilterMode > : public BasicType< SceneGraph::Sampler::FilterMode > {};

namespace SceneGraph
{

inline void SetFilterModeMessage( EventToUpdate& eventToUpdate, const Sampler& sampler, Sampler::FilterMode minFilter, Sampler::FilterMode magFilter )
{
  typedef MessageDoubleBuffered2< Sampler, Sampler::FilterMode, Sampler::FilterMode > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &sampler, &Sampler::SetFilterMode, minFilter, magFilter );
}

} // namespace SceneGraph

// Declare enum as a message parameter type
template <> struct ParameterType< SceneGraph::Sampler::WrapMode > : public BasicType< SceneGraph::Sampler::WrapMode > {};

namespace SceneGraph
{

inline void SetWrapModeMessage( EventToUpdate& eventToUpdate, const SceneGraph::Sampler& sampler, SceneGraph::Sampler::WrapMode horizontalWrap, SceneGraph::Sampler::WrapMode verticalWrap )
{
  typedef MessageDoubleBuffered2< Sampler, Sampler::WrapMode, Sampler::WrapMode > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &sampler, &Sampler::SetWrapMode, horizontalWrap, verticalWrap );
}


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali


#endif //  DALI_INTERNAL_SCENE_GRAPH_SAMPLER_H
