#ifndef DALI_GRAPHICS_VULKAN_API_FRAMEBUFFER_FACTORY_H
#define DALI_GRAPHICS_VULKAN_API_FRAMEBUFFER_FACTORY_H

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

#include <dali/graphics-api/graphics-api-framebuffer-factory.h>
#include <dali/graphics-api/graphics-api-texture-details.h>
#include <vector>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
}
namespace VulkanAPI
{
class Controller;

/**
 * Implementation of the frame buffer factory
 */
class FramebufferFactory : public Dali::Graphics::API::FramebufferFactory
{
public:

  /**
   * Constructor
   */
  explicit FramebufferFactory( Controller& controller );

  /**
   * Reset the factory to remove attachments and zero size
   */
  void Reset();

  /**
   * Set the size of the framebuffer
   */
  Graphics::API::FramebufferFactory& SetSize( const API::RectSize& size ) override;

  /**
   * Set a color attachment for the framebuffer
   */
  Graphics::API::FramebufferFactory& SetColorAttachment(
    Graphics::API::TextureDetails::AttachmentId attachmentIndex,
    const Graphics::API::Texture&               texture,
    Graphics::API::TextureDetails::LayerId      layer,
    Graphics::API::TextureDetails::LevelId      level ) override;

  /**
   * Set a depth attachment for the framebuffer
   */
  Graphics::API::FramebufferFactory& SetDepthStencilAttachment(
    const Graphics::API::Texture&                   texture,
    Graphics::API::TextureDetails::LayerId          layer,
    Graphics::API::TextureDetails::LevelId          level,
    Graphics::API::TextureDetails::DepthStencilFlag depthStencilFlag ) override;


  // not copyable
  FramebufferFactory(const FramebufferFactory&) = delete;
  FramebufferFactory& operator=(const FramebufferFactory&) = delete;

  ~FramebufferFactory() override;

  std::unique_ptr<Graphics::API::Framebuffer> Create() const override;


protected:
  /// @brief default constructor
  FramebufferFactory() = default;

  // derived types should not be moved direcly to prevent slicing
  FramebufferFactory(FramebufferFactory&&) = default;
  FramebufferFactory& operator=(FramebufferFactory&&) = default;

public:

  struct ColorAttachment
  {/*
    ColorAttachment()
      : texture( nullptr ),
        layer( 0u ),
        mipmapLevel( 0u )
    {
    }*/
    Graphics::API::Texture*                         texture;
    Graphics::API::TextureDetails::LayerId          layer;
    Graphics::API::TextureDetails::LevelId          mipmapLevel;
  };

  struct DepthAttachment
  {
    /*
    DepthAttachment()
      : texture( nullptr ),
        layer( 0u ),
        mipmapLevel( 0u ),
        depthStencilFlag(Graphics::API::TextureDetails::DepthStencilFlag::NONE)
    {
    }*/

    Graphics::API::Texture*                         texture;
    Graphics::API::TextureDetails::LayerId          layer;
    Graphics::API::TextureDetails::LevelId          mipmapLevel;
    Graphics::API::TextureDetails::DepthStencilFlag depthStencilFlag;
  };

private:
  Controller& mController;

  uint32_t mWidth;
  uint32_t mHeight;

  std::vector< ColorAttachment > mColorAttachments;
  DepthAttachment mDepthStencilAttachment;
};

}
}
}

#endif //DALI_GRAPHICS_VULKAN_API_FRAMEBUFFER_FACTORY_H
