#ifndef DALI_GRAPHICS_VULKAN_API_PIPELINE_H
#define DALI_GRAPHICS_VULKAN_API_PIPELINE_H

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

#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics-api/graphics-api-pipeline.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
class PipelineCache;
}
namespace VulkanAPI
{
using Graphics = Vulkan::Graphics;
using PipelineCache = Vulkan::PipelineCache;
class Controller;
class Pipeline
{
public:

  Pipeline();

  ~Pipeline();

  bool Create( PipelineState )

private:

  Controller&                mController;
  Graphics&                  mGraphics;
  PipelineCache&             mPipelineCache;
  Vulkan::RefCountedPipeline mPipeline;
};
}
}
}

#endif // DALI_GRAPHICS_VULKAN_API_PIPELINE_H
