#ifndef DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H
#define DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-texture-key.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
struct Sampler;
class Texture;
} // namespace Render
namespace SceneGraph
{
class MemoryPoolCollection;
class Renderer;
class RenderManagerDispatcher;

class TextureSet
{
public:
  /**
   * Construct a new TextureSet.
   */
  static TextureSet* New();

  /**
   * Register memory pool of texture set.
   * This should be called at the begin of Core.
   */
  static void RegisterMemoryPoolCollection(MemoryPoolCollection& memoryPoolCollection);

  /**
   * Unregister memory pool of texture set.
   * This should be called at the end of Core.
   */
  static void UnregisterMemoryPoolCollection();

  /**
   * Destructor. Not virtual as not a base class and not inheriting anything
   */
  ~TextureSet();

  /**
   * Overriden delete operator
   * Deletes the texture set from its global memory pool
   */
  void operator delete(void* ptr);

  /**
   * Set the sampler to be used by the texture at position "index"
   * @param[in] index The index of the texture
   * @param[in] sampler The sampler to be used by the texture
   */
  void SetSampler(uint32_t index, Render::Sampler* sampler);

  /**
   * Set the texture at position "index"
   * @param[in] index The index of the texture
   * @param[in] texture The texture
   */
  void SetTexture(uint32_t index, const Render::TextureKey& texture);

  /**
   * Return whether any texture in the texture set has an alpha channel
   * @return true if at least one texture in the texture set has an alpha channel, false otherwise
   */
  bool HasAlpha() const;

  /**
   * Accessor for textures (used by RenderDataProvider impl)
   */
  const Vector<Render::TextureKey>& GetTextures()
  {
    return mTextures;
  }

  /**
   * Accessor for samplers (used by RenderDataProvider impl)
   */
  const Vector<Render::Sampler*>& GetSamplers()
  {
    return mSamplers;
  }

  /**
   * Set the renderManagerDispatcher to call function to render-manager.
   * @param[in] renderManagerDispatcher The renderManagerDispatcher to notify texture updated to render-manager.
   */
  void SetRenderManagerDispatcher(RenderManagerDispatcher* renderManagerDispatcher);

private:
  /**
   * Protected constructor; See also TextureSet::New()
   */
  TextureSet();

private:
  /**
   * @brief Change the count of texture set. It will increase count automatically if we need more textures.
   *
   * @param[in] count The number of textures what this texture set want to hold
   */
  void SetTextureCount(uint32_t count);

  /**
   * @brief Change the count of sampler set. It will increase count automatically if we need more samplers.
   *
   * @param[in] count The number of samplers what this texture set want to hold
   */
  void SetSamplerCount(uint32_t count);

  /**
   * @brief Remove empty textures and samplers at the back of container, and resize.
   */
  void TrimContainers();

private:
  Vector<Render::Sampler*>   mSamplers;                         ///< List of samplers used by each texture. Not owned
  Vector<Render::TextureKey> mTextures;                         ///< List of Textures. Not owned
  RenderManagerDispatcher*   mRenderManagerDispatcher{nullptr}; ///< for sending messages to render thread. Not owned
  bool                       mHasAlpha;                         ///< if any of the textures has an alpha channel
};

inline void SetTextureMessage(EventThreadServices& eventThreadServices, const TextureSet& textureSet, uint32_t index, const Render::TextureKey& textureKey)
{
  using LocalType = MessageValue2<TextureSet, uint32_t, Render::TextureKey>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&textureSet, &TextureSet::SetTexture, index, textureKey);
}

inline void SetSamplerMessage(EventThreadServices& eventThreadServices, const TextureSet& textureSet, uint32_t index, Render::Sampler* sampler)
{
  using LocalType = MessageValue2<TextureSet, uint32_t, Render::Sampler*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&textureSet, &TextureSet::SetSampler, index, sampler);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H
