#ifndef TEST_GRAPHICS_FRAMEBUFFER_H
#define TEST_GRAPHICS_FRAMEBUFFER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-framebuffer.h>
#include <test-graphics-framebuffer-factory.h>

namespace Test
{
class GraphicsController;

class GraphicsFramebuffer : public Dali::Graphics::Framebuffer
{
public:
  explicit GraphicsFramebuffer( GraphicsController& mController,
                                Dali::Graphics::Extent2D size,
                                const std::vector<GraphicsFramebufferFactory::ColorAttachment>& colorAttachments,
                                const GraphicsFramebufferFactory::DepthAttachment& depthAttachment );

  ~GraphicsFramebuffer() override;

public:
  GraphicsController& mController;
  Dali::Graphics::Extent2D mSize;
  const std::vector<GraphicsFramebufferFactory::ColorAttachment>& mColorAttachments;
  const GraphicsFramebufferFactory::DepthAttachment& mDepthAttachment;
};

} // namespace Test

#endif // TEST_GRAPHICS_FRAMEBUFFER_H
