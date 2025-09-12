#ifndef DALI_GRAPHICS_PROGRAM_H
#define DALI_GRAPHICS_PROGRAM_H

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

namespace Dali
{
namespace Graphics
{
/**
 * @brief The Program class represents a set of shaders that can
 * be compiled into a complete program and attached to a Pipeline.
 * It can be used to fetch a Reflection object to determine the
 * structure of attributes, uniforms and samplers within the shaders.
 */
class Program
{
public:
  Program()          = default;
  virtual ~Program() = default;

  // not copyable
  Program(const Program&)            = delete;
  Program& operator=(const Program&) = delete;

protected:
  Program(Program&&)            = default;
  Program& operator=(Program&&) = default;
};

} // namespace Graphics

} // namespace Dali

#endif //DALI_GRAPHICS_PROGRAM_H
