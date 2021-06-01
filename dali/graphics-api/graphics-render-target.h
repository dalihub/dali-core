#ifndef DALI_GRAPHICS_RENDER_TARGET_H
#define DALI_GRAPHICS_RENDER_TARGET_H

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

#include "graphics-types.h"

namespace Dali::Graphics
{
class Framebuffer;
/**
 * @brief RenderTarget object binds to any presentable graphics output.
 */
class RenderTarget
{
public:
  RenderTarget()          = default;
  virtual ~RenderTarget() = default;

  // not copyable
  RenderTarget(const RenderTarget&) = delete;
  RenderTarget& operator=(const RenderTarget&) = delete;

protected:
  RenderTarget(RenderTarget&&) = default;
  RenderTarget& operator=(RenderTarget&&) = default;
};

} // Namespace Dali::Graphics

#endif