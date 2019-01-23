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
 */

#include "test-graphics-framebuffer-factory.h"

namespace Test
{
GraphicsFramebufferFactory::GraphicsFramebufferFactory() = default;
GraphicsFramebufferFactory::~GraphicsFramebufferFactory() = default;

Dali::Graphics::FramebufferFactory& GraphicsFramebufferFactory::SetSize( const Dali::Graphics::Extent2D& size )
{
  return *this;
}

Dali::Graphics::FramebufferFactory& GraphicsFramebufferFactory::SetColorAttachment(
  Dali::Graphics::TextureDetails::AttachmentId attachmentIndex,
  const Dali::Graphics::Texture&               texture,
  Dali::Graphics::TextureDetails::LayerId      layer,
  Dali::Graphics::TextureDetails::LevelId      level )
{
  return *this;
}

Dali::Graphics::FramebufferFactory& GraphicsFramebufferFactory::SetDepthStencilAttachment(
  const Dali::Graphics::Texture&                   texture,
  Dali::Graphics::TextureDetails::LayerId          layer,
  Dali::Graphics::TextureDetails::LevelId          level,
  Dali::Graphics::TextureDetails::DepthStencilFlag depthStencilFlag )
{
  return *this;
}

Dali::Graphics::FramebufferFactory::PointerType GraphicsFramebufferFactory::Create() const
{
  return nullptr;
}

} // Test
