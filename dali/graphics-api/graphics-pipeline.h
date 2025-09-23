#ifndef DALI_GRAPHICS_PIPELINE_H
#define DALI_GRAPHICS_PIPELINE_H

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

namespace Dali
{
namespace Graphics
{
/**
 * @brief Pipeline object represents a single full graphics pipeline state.
 *
 * The state involves compiled and linked shaders as well as state parameters
 * like blending, stencil, scissors, viewport etc.
 *
 * Some of the parameters can be modified by issuing commands but
 * the Pipeline must mark those states
 * as dynamic.
 *
 */
class Pipeline
{
public:
  Pipeline()          = default;
  virtual ~Pipeline() = default;

  // not copyable
  Pipeline(const Pipeline&)            = delete;
  Pipeline& operator=(const Pipeline&) = delete;

protected:
  Pipeline(Pipeline&&)            = default;
  Pipeline& operator=(Pipeline&&) = default;
};

} // Namespace Graphics
} // Namespace Dali

#endif