#ifndef DALI_INTERNAL_UPDATE_RENDERING_SHADER_CACHE_H
#define DALI_INTERNAL_UPDATE_RENDERING_SHADER_CACHE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-shader.h>
#include <dali/graphics-api/graphics-types.h>
#include <memory>

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
    Item( const Item& ) = delete;
    Item( Item&& ) = default;

    Item( std::unique_ptr<Dali::Graphics::Shader> _shader,
          Dali::Graphics::ShaderDetails::ShaderSource _vertexSource,
          Dali::Graphics::ShaderDetails::ShaderSource _fragmentSource )
    : shader(std::move(_shader)),
      vertexSource( _vertexSource ),
      fragmentSource( _fragmentSource )
    {}

    ~Item() = default;

    std::unique_ptr<Dali::Graphics::Shader>               shader{ nullptr };
    Dali::Graphics::ShaderDetails::ShaderSource           vertexSource{""};
    Dali::Graphics::ShaderDetails::ShaderSource           fragmentSource{""};
  };

  /**
   * Constructor
   *
   * @param[in] controller The graphics controller
   */
  explicit ShaderCache( Dali::Graphics::Controller& controller );

  /**
   * Get a shader from it's source code
   */
  Dali::Graphics::Shader& GetShader(
    const Dali::Graphics::ShaderDetails::ShaderSource& vsh,
    const Dali::Graphics::ShaderDetails::ShaderSource& fsh );

  /**
   * Destroy any graphics objects owned by this scene graph object
   */
  void DestroyGraphicsObjects();

private:
  std::vector<Item> mItems;
  Dali::Graphics::Controller& mController;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //DALI_INTERNAL_UPDATE_RENDERING_SHADER_CACHE_H
