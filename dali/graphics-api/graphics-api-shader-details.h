#ifndef DALI_GRAPHICS_API_TEXTURE_DETAILS_H
#define DALI_GRAPHICS_API_TEXTURE_DETAILS_H

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

#include <string>

namespace Dali
{
namespace Graphics
{
namespace API
{
namespace ShaderDetails
{

using ShaderSource = std::string;

enum class Language {
  GLSL_1,
  GLSL_3_1,
  GLSL_3_2,
  SPIRV_1_0,
  SPIRV_1_1,
};

enum class PipelineStage {
  VERTEX,
  GEOMETRY,
  FRAGMENT,
  COMPUTE,
  TESSELATION_CONTROL,
  TESSELATION_EVALUATION,
};

} // namespace TextureDetails
} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_TEXTURE_DETAILS_H
