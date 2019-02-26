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
class GraphicsController;

class GraphicsFramebufferFactory : public Dali::Graphics::FramebufferFactory
{
public:
  GraphicsFramebufferFactory(GraphicsController* controller);
  virtual ~GraphicsFramebufferFactory();

  Dali::Graphics::FramebufferFactory& SetSize( const Dali::Graphics::Extent2D& size ) override;

  Dali::Graphics::FramebufferFactory& SetColorAttachment( Dali::Graphics::TextureDetails::AttachmentId attachmentIndex,
                                                               const Dali::Graphics::Texture&               texture,
                                                               Dali::Graphics::TextureDetails::LayerId      layer,
                                                               Dali::Graphics::TextureDetails::LevelId      level ) override;

  Dali::Graphics::FramebufferFactory& SetDepthStencilAttachment( const Dali::Graphics::Texture&                   texture,
                                                                 Dali::Graphics::TextureDetails::LayerId          layer,
                                                                 Dali::Graphics::TextureDetails::LevelId          level,
                                                                 Dali::Graphics::TextureDetails::DepthStencilFlag depthStencilFlag ) override;

  Dali::Graphics::FramebufferFactory::PointerType Create() const override;

public: // test methods
  void TestReset();

public:
  struct ColorAttachment
  {
    const Dali::Graphics::Texture*    texture;
    Dali::Graphics::TextureDetails::LayerId layer;
    Dali::Graphics::TextureDetails::LevelId mipmapLevel;
  };

  struct DepthAttachment
  {
    const Dali::Graphics::Texture*                   texture;
    Dali::Graphics::TextureDetails::LayerId          layer;
    Dali::Graphics::TextureDetails::LevelId          mipmapLevel;
    Dali::Graphics::TextureDetails::DepthStencilFlag depthStencilFlag;
  };

public:
  GraphicsController& mController;
  Dali::Graphics::Extent2D mSize;
  std::vector<ColorAttachment> mColorAttachments;
  DepthAttachment mDepthAttachment;
};

} // Test

#endif //TEST_GRAPHICS_FRAMEBUFFER_FACTORY_H
