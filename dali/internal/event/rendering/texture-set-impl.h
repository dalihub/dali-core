#ifndef DALI_INTERNAL_TEXTURE_SET_H
#define DALI_INTERNAL_TEXTURE_SET_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h> // std::vector

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-impl.h>     // Dali::Internal::Object
#include <dali/internal/event/rendering/sampler-impl.h> // Dali::Internal::Sampler
#include <dali/internal/event/rendering/shader-impl.h>  // Dali::Internal::Shader
#include <dali/internal/event/rendering/texture-impl.h> // Dali::Internal::Texture
#include <dali/public-api/common/dali-common.h>         // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h>       // Dali::IntrusivePtr
#include <dali/public-api/rendering/texture-set.h>      // Dali::TextureSet

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class TextureSet;
}

class TextureSet;
using TextureSetPtr = IntrusivePtr<TextureSet>;

/**
 * TextureSet is an object that holds all the textures used by a renderer
 */
class TextureSet : public BaseObject
{
public:
  /**
   * @copydoc Dali::TextureSet::New()
   */
  static TextureSetPtr New();

  /**
   * @copydoc Dali::TextureSet::SetTexture()
   */
  void SetTexture(uint32_t index, TexturePtr texture);

  /**
   * @copydoc Dali::TextureSet::GetTexture()
   */
  Texture* GetTexture(uint32_t index) const;

  /**
   * @copydoc Dali::TextureSet::SetSampler()
   */
  void SetSampler(uint32_t index, SamplerPtr sampler);

  /**
   * @copydoc Dali::TextureSet::GetSampler()
   */
  Sampler* GetSampler(uint32_t index) const;

  /**
   * @copydoc Dali::TextureSet::GetTextureCount()
   */
  uint32_t GetTextureCount() const;

  /**
   * @brief Get the TextureSet scene object
   *
   * @return the texture set scene object
   */
  const SceneGraph::TextureSet* GetTextureSetSceneObject() const;

private: // implementation
  TextureSet();

  /**
   * Second stage initialization
   */
  void Initialize();

  /**
   * @brief Change the count of texture and sampler. It will increase count automatically if we need more textures.
   *
   * @param[in] count The number of textures what this texture set want to hold
   */
  void SetTextureCount(uint32_t count);

  /**
   * @brief Change the count of sampler. It will increase count automatically if we need more samplers.
   *
   * @param[in] count The number of samplers what this texture set want to hold
   */
  void SetSamplerCount(uint32_t count);

  /**
   * @brief Remove empty textures and samplers at the back of container, and resize.
   */
  void TrimContainers();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~TextureSet() override;

private: // unimplemented methods
  TextureSet(const TextureSet&);
  TextureSet& operator=(const TextureSet&);

private:                                        // Data
  EventThreadServices&    mEventThreadServices; ///<Used to send messages to the update thread
  SceneGraph::TextureSet* mSceneObject;
  std::vector<SamplerPtr> mSamplers;
  std::vector<TexturePtr> mTextures;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::TextureSet& GetImplementation(Dali::TextureSet& handle)
{
  DALI_ASSERT_ALWAYS(handle && "TextureSet handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::TextureSet&>(object);
}

inline const Internal::TextureSet& GetImplementation(const Dali::TextureSet& handle)
{
  DALI_ASSERT_ALWAYS(handle && "TextureSet handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::TextureSet&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_TEXTURE_SET_H
