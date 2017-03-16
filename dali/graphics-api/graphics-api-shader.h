#ifndef DALI_GRAPHICS_API_SHADER_H
#define DALI_GRAPHICS_API_SHADER_H

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

namespace Dali
{
namespace Graphics
{
namespace API
{

/**
 * @brief Interface class for Shader types in the graphics API.
 */
class Shader
{
public:
  // not copyable
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;

  virtual ~Shader() = default;

protected:
  // derived types should not be moved direcly to prevent slicing
  Shader(Shader&&) = default;
  Shader& operator=(Shader&&) = default;

  /**
   * Objects of this type should not directly.
   */
  Shader() = default;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_SHADER_H
