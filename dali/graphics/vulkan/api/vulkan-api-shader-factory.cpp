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
 *
 */

#include <dali/graphics/vulkan/api/vulkan-api-shader-factory.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
ShaderFactory::ShaderFactory( Vulkan::Graphics& graphics )
{
}

void ShaderFactory::SetSource( Graphics::API::ShaderDetails::PipelineStage       pipelineStage,
                               Graphics::API::ShaderDetails::Language            language,
                               const Graphics::API::ShaderDetails::ShaderSource& source )
{
}

std::unique_ptr<Graphics::API::Shader> ShaderFactory::Create() const
{
  return nullptr;
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
