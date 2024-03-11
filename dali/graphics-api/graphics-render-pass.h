#ifndef DALI_GRAPHICS_RENDER_PASS_H
#define DALI_GRAPHICS_RENDER_PASS_H

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
 *
 */

namespace Dali
{
namespace Graphics
{
/**
 * @brief RenderPass is very similar to the the Vulkan equivalent VkRenderPass.
 *
 * In general, the RenderPass object carries all the information on:
 * - the render target (framebuffer, textures)
 * - clear colors setup
 * - inputs/outputs to build dependencies
 *
 */
class RenderPass
{
public:
  RenderPass()          = default;
  virtual ~RenderPass() = default;

  // not copyable
  RenderPass(const RenderPass&) = delete;
  RenderPass& operator=(const RenderPass&) = delete;

protected:
  RenderPass(RenderPass&&) = default;
  RenderPass& operator=(RenderPass&&) = default;
};

} // Namespace Graphics
} // Namespace Dali

#endif