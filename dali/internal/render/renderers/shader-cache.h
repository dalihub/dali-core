#ifndef DALI_INTERNAL_SHADER_CACHE_H
#define DALI_INTERNAL_SHADER_CACHE_H

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

#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-shader-create-info.h>
#include <dali/graphics-api/graphics-shader.h>
#include <memory>

namespace Dali
{
namespace Internal
{
namespace Render
{
/**
 * Caches graphics shaders as they are created by SceneGraph::Shader.
 */
struct ShaderCache
{
  struct Item
  {
    Item()            = default;
    Item(const Item&) = delete;
    Item(Item&&)      = default;

    Item(Graphics::UniquePtr<Dali::Graphics::Shader> shader,
         const std::vector<char>&                    shaderCode,
         Graphics::PipelineStage                     stage,
         Graphics::ShaderSourceMode                  type)
    : shader(std::move(shader)),
      shaderCode(shaderCode),
      stage(stage),
      type(type)
    {
    }

    ~Item() = default;

    Graphics::UniquePtr<Dali::Graphics::Shader> shader{nullptr};
    std::vector<char>                           shaderCode;
    Graphics::PipelineStage                     stage;
    Graphics::ShaderSourceMode                  type;
  };

  /**
   * Constructor
   *
   * @param[in] controller The graphics controller
   */
  explicit ShaderCache(Dali::Graphics::Controller& controller);

  /**
   * Get a shader from it's source code
   *
   * @param[in] shaderCode The shader code
   * @param[in] stage The pipeline stage (e.g. VERTEX_SHADER or FRAGMENT_SHADER etc.)
   * @param[in] type The type of the shader (i.e. text or binary)
   * @return the graphics shader
   */
  Dali::Graphics::Shader& GetShader(const std::vector<char>& shaderCode, Graphics::PipelineStage stage, Graphics::ShaderSourceMode type);

private:
  std::vector<Item>           mItems;
  Dali::Graphics::Controller& mController;
};

} // namespace Render
} // namespace Internal
} // namespace Dali

#endif //DALI_INTERNAL_SHADER_CACHE_H
