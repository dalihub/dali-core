#ifndef TEST_GRAPHICS_FRAMEBUFFER_FACTORY_H
#define TEST_GRAPHICS_FRAMEBUFFER_FACTORY_H

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

#include <dali/graphics-api/graphics-api-framebuffer-factory.h>

namespace Test
{

class GraphicsFramebufferFactory : public Dali::Graphics::API::FramebufferFactory
{
public:
  GraphicsFramebufferFactory();
  virtual ~GraphicsFramebufferFactory();

  Dali::Graphics::API::FramebufferFactory& SetSize( const Dali::Graphics::API::RectSize& size ) override;

  Dali::Graphics::API::FramebufferFactory& SetColorAttachment( Dali::Graphics::API::TextureDetails::AttachmentId attachmentIndex,
                                                               const Dali::Graphics::API::Texture&               texture,
                                                               Dali::Graphics::API::TextureDetails::LayerId      layer,
                                                               Dali::Graphics::API::TextureDetails::LevelId      level ) override;

  Dali::Graphics::API::FramebufferFactory& SetDepthStencilAttachment( const Dali::Graphics::API::Texture&                   texture,
                                                                      Dali::Graphics::API::TextureDetails::LayerId          layer,
                                                                      Dali::Graphics::API::TextureDetails::LevelId          level,
                                                                      Dali::Graphics::API::TextureDetails::DepthStencilFlag depthStencilFlag ) override;
  Dali::Graphics::API::FramebufferFactory::PointerType Create() const override;

};

} // Test

#endif //TEST_GRAPHICS_FRAMEBUFFER_FACTORY_H
