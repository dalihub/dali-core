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
#include <dali/public-api/rendering/texture-set.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>

namespace Dali
{
namespace Internal
{
class ResourceManager;

namespace Render
{
struct Sampler;
class NewTexture;
}
namespace SceneGraph
{
class Renderer;

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
   * Set the sampler to be used by the texture at position "index"
   * @param[in] index The index of the texture
   * @param[in] sampler The sampler to be used by the texture
   */
  void SetSampler( size_t index, Render::Sampler* sampler );

  /**
   * Set the texture at position "index"
   * @param[in] index The index of the texture
   * @param[in] texture The texture
   */
  void SetTexture( size_t index, Render::NewTexture* texture );

  /**
   * Return whether any texture in the texture set has an alpha channel
   * @return true if at least one texture in the texture set has an alpha channel, false otherwise
   */
  bool HasAlpha() const;

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
   * Get the sampler of a texture in the TextureSet
   * @param[in] index The index of the texture in the textures array
   * @return the sampler used by the texture
   */
  Render::Sampler* GetTextureSampler( size_t index )
  {
    return mSamplers[index];
  }

  /**
   * Get the number of NewTextures in the texture set
   * @return The number of NewTextures
   */
  size_t GetNewTextureCount()
  {
    return mTextures.Size();
  }

  /**
   * Get the pointer to  a NewTexture in the TextureSet
   * @param[in] index The index of the texture in the textures array
   * @return the pointer to the NewTexture in that position
   */
  Render::NewTexture* GetNewTexture( size_t index )
  {
    return mTextures[index];
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
  Vector< Render::NewTexture* >   mTextures;                    ///< List of NewTextures. Not owned
  Vector<Renderer*>               mRenderers;                   ///< List of renderers using the TextureSet
  bool                            mHasAlpha;                    ///< if any of the textures has an alpha channel
};

inline void SetTextureMessage( EventThreadServices& eventThreadServices, const TextureSet& textureSet, size_t index, Render::NewTexture* texture )
{
  typedef MessageValue2< TextureSet, size_t, Render::NewTexture* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &textureSet, &TextureSet::SetTexture, index, texture );
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
