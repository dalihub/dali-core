#ifndef DALI_TEST_GRAPHICS_RENDER_TARGET_H
#define DALI_TEST_GRAPHICS_RENDER_TARGET_H

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
 */

#include <dali/graphics-api/graphics-render-target-create-info.h>
#include <dali/graphics-api/graphics-render-target.h>

namespace Dali
{
class TestGraphicsRenderTarget : public Graphics::RenderTarget
{
public:
  TestGraphicsRenderTarget(TestGlAbstraction& gl, Graphics::RenderTargetCreateInfo createInfo)
  : mGl(gl)
  {
    mCreateInfo.surface      = createInfo.surface;
    mCreateInfo.framebuffer  = createInfo.framebuffer;
    mCreateInfo.extent       = createInfo.extent;
    mCreateInfo.preTransform = createInfo.preTransform;
  }
  ~TestGraphicsRenderTarget() = default;

  TestGlAbstraction&               mGl;
  Graphics::RenderTargetCreateInfo mCreateInfo;
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_RENDER_TARGET_H
