#ifndef DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H
#define DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/public-api/rendering/texture-set.h>

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
class Renderer;

class TextureSet
{
public:
  /**
   * Construct a new TextureSet.
   */
  static TextureSet* New();

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
  void SetTexture(uint32_t index, Render::Texture* texture);

  /**
   * Return whether any texture in the texture set has an alpha channel
   * @return true if at least one texture in the texture set has an alpha channel, false otherwise
   */
  bool HasAlpha() const;

  /**
   * Accessor for textures (used by RenderDataProvider impl)
   */
  const Vector<Render::Texture*>& GetTextures()
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

private:
  /**
   * Protected constructor; See also TextureSet::New()
   */
  TextureSet();

private:                              // Data
  Vector<Render::Sampler*> mSamplers; ///< List of samplers used by each texture. Not owned
  Vector<Render::Texture*> mTextures; ///< List of Textures. Not owned
  bool                     mHasAlpha; ///< if any of the textures has an alpha channel
};

inline void SetTextureMessage(EventThreadServices& eventThreadServices, const TextureSet& textureSet, uint32_t index, Render::Texture* texture)
{
  using LocalType = MessageValue2<TextureSet, uint32_t, Render::Texture*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&textureSet, &TextureSet::SetTexture, index, texture);
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
