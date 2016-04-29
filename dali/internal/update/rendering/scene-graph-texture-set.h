#ifndef DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H
#define DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H

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

// INTERNAL INCLUDES
#include <dali/devel-api/rendering/texture-set.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>

namespace Dali
{
namespace Internal
{
class ResourceManager;

namespace Render
{
class Sampler;
}
namespace SceneGraph
{
class Renderer;
class Sampler;

class TextureSet
{
public:

  /**
   * Construct a new TextureSet.
   */
  static TextureSet* New();

  /**
   * Destructor
   */
  virtual ~TextureSet();

  /**
   * Overriden delete operator
   * Deletes the texture set from its global memory pool
   */
  void operator delete( void* ptr );

  /**
   * Prepare the texture set, check texture loading status, opacity etc
   * @param resourceManager for checking texture details and loading status
   */
  void Prepare( const ResourceManager& resourceManager );

  /**
   * Set the resource id for the texture at position "index"
   * @param[in] index The index of the texture
   * @param[in] imageId the resource id of the imag
   */
  void SetImage( size_t index,  ResourceId imageId );

  /**
   * Set the sampler to be used by the texture at position "index"
   * @param[in] index The index of the texture
   * @param[in] sampler The sampler to be used by the texture
   */
  void SetSampler( size_t index, Render::Sampler* sampler );

  /**
   * Return whether any texture in the texture set has an alpha channel
   * @return true if at least one texture in the texture set has an alpha channel, false otherwise
   */
  bool HasAlpha() const;

  /**
   * Get the resource status
   * Note, we need two values as it's possible that some resource failed to load
   * in which case resourcesReady is false (the texture set is not good to be used for rendering)
   * but finishedResourceAcquisition if true as there is no more loading going on
   * @param[out] resourcesReady if the texture set is ready to be used for rendering
   * @param[out] finishedResourceAcquisition if
   */
  void GetResourcesStatus( bool& resourcesReady, bool& finishedResourceAcquisition );

  /**
   * Adds a renderer to the Renderers list of the texture set.
   * Renderers using the TextureSet get a notification when the texture set changes
   *
   * @param[in] renderer The renderer using the TextureSet
   */
  void AddObserver( Renderer* renderer );

  /**
   * Removes a renderer from the TextureSet renderers list
   *
   * @param[in] renderer The renderer no longer using the TextureSet
   */
  void RemoveObserver( Renderer* renderer );

  /**
   * Get the ResourceId of a texture in the TextureSet
   * @param[in] index The index of the texture in the textures array
   * @return the ResourceId
   */
  ResourceId GetTextureId( size_t index )
  {
    return mTextureId[index];
  }

  /**
   * Get the sampler of a texture in the TextureSet
   * @param[in] index The index of the texture in the textures array
   * @return the sampler used by the texture
   */
  Render::Sampler* GetTextureSampler( size_t index )
  {
    return mSamplers[index];
  }

  /**
   * Get the number of textures in the texture set
   * @return The number of textures
   */
  size_t GetTextureCount()
  {
    return mTextureId.Size();
  }


private:

  /**
   * Protected constructor; See also TextureSet::New()
   */
  TextureSet();

  /**
   * Helper method to notify the renderers observing the TextureSet
   * that the TextureSet has changed
   */
  void NotifyChangeToRenderers();

private: // Data

  Vector< Render::Sampler* >      mSamplers;                    ///< List of samplers used by each texture. Not owned
  Vector< ResourceId >            mTextureId;                   ///< List of texture ids
  Vector<Renderer*>               mRenderers;                   ///< List of renderers using the TextureSet
  bool                            mResourcesReady;              ///< if the textures are ready to be used for rendering
  bool                            mFinishedResourceAcquisition; ///< if resource loading is completed
  bool                            mChanged;                     ///< if the texture set has changed since the last frame
  bool                            mHasAlpha;                    ///< if any of the textures has an alpha channel
};

inline void SetImageMessage( EventThreadServices& eventThreadServices, const TextureSet& textureSet, size_t index, ResourceId resourceId )
{
  typedef MessageValue2< TextureSet, size_t, ResourceId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &textureSet, &TextureSet::SetImage, index, resourceId );
}

inline void SetSamplerMessage( EventThreadServices& eventThreadServices, const TextureSet& textureSet, size_t index, Render::Sampler* sampler )
{
  typedef MessageValue2< TextureSet, size_t, Render::Sampler* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &textureSet, &TextureSet::SetSampler, index, sampler );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H
