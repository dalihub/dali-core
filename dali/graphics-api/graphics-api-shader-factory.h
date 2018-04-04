#ifndef DALI_GRAPHICS_API_SHADER_FACTORY_H
#define DALI_GRAPHICS_API_SHADER_FACTORY_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-base-factory.h>
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics-api/graphics-api-shader.h>

namespace Dali
{
namespace Graphics
{
namespace API
{

/**
 * @brief Interface class for ShaderFactory types in the graphics API.
 */
class ShaderFactory : public BaseFactory<Shader>
{
public:
  /**
   * @brief Set the source for a pipeline stage
   */
  virtual ShaderFactory& SetShaderModule(ShaderDetails::PipelineStage       pipelineStage,
                                         ShaderDetails::Language            language,
                                         const ShaderDetails::ShaderSource& source) = 0;

  // not copyable
  ShaderFactory(const ShaderFactory&) = delete;
  ShaderFactory& operator=(const ShaderFactory&) = delete;

  virtual ~ShaderFactory() = default;

protected:
  /// @brief default constructor
  ShaderFactory() = default;

  // derived types should not be moved direcly to prevent slicing
  ShaderFactory(ShaderFactory&&) = default;
  ShaderFactory& operator=(ShaderFactory&&) = default;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_SHADER_FACTORY_H
