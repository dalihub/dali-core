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
#include "test-graphics-framebuffer.h"

namespace Test
{
GraphicsFramebufferFactory::GraphicsFramebufferFactory(GraphicsController* controller)
: mController(*controller)
{
}

GraphicsFramebufferFactory::~GraphicsFramebufferFactory() = default;

Dali::Graphics::FramebufferFactory& GraphicsFramebufferFactory::SetSize( const Dali::Graphics::Extent2D& size )
{
  mSize = size;
  return *this;
}

Dali::Graphics::FramebufferFactory& GraphicsFramebufferFactory::SetColorAttachment(
  Dali::Graphics::TextureDetails::AttachmentId attachmentIndex,
  const Dali::Graphics::Texture&               texture,
  Dali::Graphics::TextureDetails::LayerId      layer,
  Dali::Graphics::TextureDetails::LevelId      mipmapLevel )
{
  if( mColorAttachments.size() <= attachmentIndex )
  {
    mColorAttachments.resize( attachmentIndex+1 );
  }
  mColorAttachments[attachmentIndex].texture = &texture;
  mColorAttachments[attachmentIndex].layer = layer;
  mColorAttachments[attachmentIndex].mipmapLevel = mipmapLevel;
  return *this;
}

Dali::Graphics::FramebufferFactory& GraphicsFramebufferFactory::SetDepthStencilAttachment(
  const Dali::Graphics::Texture&                   texture,
  Dali::Graphics::TextureDetails::LayerId          layer,
  Dali::Graphics::TextureDetails::LevelId          level,
  Dali::Graphics::TextureDetails::DepthStencilFlag depthStencilFlag )
{
  mDepthAttachment = DepthAttachment{ &texture, layer, level, depthStencilFlag };
  return *this;
}

Dali::Graphics::FramebufferFactory::PointerType GraphicsFramebufferFactory::Create() const
{
  auto fb = std::make_unique<Test::GraphicsFramebuffer>( mController, mSize, mColorAttachments, mDepthAttachment );
  return fb;
}

void GraphicsFramebufferFactory::TestReset()
{
  mColorAttachments.clear();
  mDepthAttachment = GraphicsFramebufferFactory::DepthAttachment { };
  mSize = { 0,0 };
}


} // Test
