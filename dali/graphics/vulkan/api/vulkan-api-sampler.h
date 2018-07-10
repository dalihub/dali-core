#ifndef DALI_VULKAN_API_SAMPLER_H
#define DALI_VULKAN_API_SAMPLER_H

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

#include <dali/graphics-api/graphics-api-sampler.h>
#include <dali/graphics/vulkan/internal/vulkan-sampler.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
class SamplerFactory;
class Controller;
/**
 * @brief Interface class for Sampler types in the graphics API.
 */
class Sampler : public API::Sampler
{
public:

  Sampler( Controller& controller, const SamplerFactory& factory );

  const Vulkan::RefCountedSampler& GetSampler() const
  {
    return mSampler;
  }

protected:

  Controller& mController;
  Vulkan::RefCountedSampler mSampler;
};

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali

#endif // DALI_VULKAN_API_SAMPLER_H
