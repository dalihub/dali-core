#ifndef DALI_INTERNAL_UPDATE_RENDERING_SHADER_CACHE_H
#define DALI_INTERNAL_UPDATE_RENDERING_SHADER_CACHE_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali/integration-api/graphics/graphics.h>
#include <dali/graphics-api/graphics-api-accessor.h>
#include <dali/graphics-api/graphics-api-shader.h>
#include <dali/graphics-api/graphics-api-shader-details.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

/**
 * Caches graphics shaders as they are created by SceneGraph::Shader.
 */
struct ShaderCache
{
  struct Item
  {
    Item() = default;
    ~Item() = default;
    Dali::Graphics::API::Accessor<Dali::Graphics::API::Shader> shader{ nullptr };
    Dali::Graphics::API::ShaderDetails::ShaderSource           vertexSource{""};
    Dali::Graphics::API::ShaderDetails::ShaderSource           fragmentSource{""};
  };

  /**
   * Constructor
   *
   * @param[in] controller The graphics controller
   */
  explicit ShaderCache( Dali::Graphics::API::Controller& controller );

  /**
   * Get a shader from it's source code
   */
  Dali::Graphics::API::Accessor<Dali::Graphics::API::Shader> GetShader(
    const Dali::Graphics::API::ShaderDetails::ShaderSource& vsh,
    const Dali::Graphics::API::ShaderDetails::ShaderSource& fsh );

private:
  std::vector<Item> mItems;
  Dali::Graphics::API::Controller& mController;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //DALI_INTERNAL_UPDATE_RENDERING_SHADER_CACHE_H
