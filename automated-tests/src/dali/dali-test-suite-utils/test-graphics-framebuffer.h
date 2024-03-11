#ifndef TEST_GRAPHICS_FRAMEBUFFER_H
#define TEST_GRAPHICS_FRAMEBUFFER_H

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

#include <dali/graphics-api/graphics-framebuffer-create-info.h>
#include <dali/graphics-api/graphics-framebuffer.h>
#include <dali/graphics-api/graphics-types.h>
#include "test-trace-call-stack.h"

namespace Dali
{
class TestGraphicsFramebuffer : public Graphics::Framebuffer
{
public:
  TestGraphicsFramebuffer(TraceCallStack& callStack, const Graphics::FramebufferCreateInfo& createInfo);
  ~TestGraphicsFramebuffer();

  void Initialize();
  void AttachTexture(Graphics::Texture* texture, uint32_t attachmentId, uint32_t layerId, uint32_t levelId);
  void Bind();

  Graphics::FramebufferCreateInfo mCreateInfo;
  TraceCallStack&                 mCallStack;

  uint32_t mId{0};
  uint32_t mDepthBuffer{0};
  uint32_t mStencilBuffer{0};
};

} // namespace Dali

#endif //TEST_GRAPHICS_FRAMEBUFFER_H
