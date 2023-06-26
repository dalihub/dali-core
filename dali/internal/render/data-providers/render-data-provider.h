#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_DATA_PROVIDER_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali/internal/common/hash-utils.h>
#include <dali/internal/render/data-providers/uniform-map-data-provider.h>
#include <dali/internal/render/renderers/render-texture-key.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
struct BlendingOptions;

namespace Render
{
class Texture;
class Sampler;
class UniformBlock;
} // namespace Render

namespace SceneGraph
{
class Shader;
class Renderer;

/**
 * Wraps all the data providers for the renderer. This allows the
 * individual providers to change connections in the update thread without affecting
 * the current render. It essentially provides double buffering of the
 * provider accessor through the message system. It does not take ownership
 * of any objects.
 */
class RenderDataProvider
{
protected:
  /**
   * Destructor
   */
  virtual ~RenderDataProvider() = default;

public:
  /**
   * Get the uniform map data provider
   */
  virtual const UniformMapDataProvider& GetUniformMapDataProvider() const = 0;

  /**
   * Returns the shader
   * @return The shader
   */
  virtual const Shader& GetShader() const = 0;

  /**
   * Returns the list of samplers
   * @return The list of samplers
   */
  virtual const Dali::Vector<Render::Sampler*>* GetSamplers() const = 0;

  /**
   * Returns the list of Textures
   * @return The list of Textures
   */
  virtual const Dali::Vector<Render::TextureKey>* GetTextures() const = 0;

  /**
   * Get the mix color
   * @param[in] bufferIndex The current buffer index.
   * @return The mix color
   */
  virtual Vector4 GetMixColor(BufferIndex bufferIndex) const = 0;

  /**
   * @brief Retrieve if the render data is updated
   * @return An updated flag
   */
  virtual bool IsUpdated() const = 0;

  /**
   * @brief Get the update area after visual properties applied.
   * @param[in] bufferIndex The current buffer index.
   * @param[in] originalUpdateArea The original update area before apply the visual properties.
   * @return The recalculated size after visual properties applied.
   */
  virtual Vector4 GetVisualTransformedUpdateArea(BufferIndex bufferIndex, const Vector4& originalUpdateArea) noexcept = 0;

  virtual uint32_t GetInstanceCount() const = 0;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_DATA_PROVIDER_H
