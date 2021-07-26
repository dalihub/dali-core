#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_DATA_PROVIDER_H

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
 *
 */

#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/render/data-providers/uniform-map-data-provider.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/rendering/renderer.h>

namespace Dali
{
namespace Internal
{
struct BlendingOptions;

namespace Render
{
class Texture;
class Sampler;
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
  virtual const Dali::Vector<Render::Texture*>* GetTextures() const = 0;

  /**
   * Get the opacity
   * @return The opacity
   */
  virtual float GetOpacity(BufferIndex bufferIndex) const = 0;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_DATA_PROVIDER_H
