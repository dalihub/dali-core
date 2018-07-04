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

// CLASS INCLUDE
#include <dali/graphics/vulkan/api/vulkan-api-framebuffer.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-framebuffer-factory.h>
#include <dali/graphics-api/graphics-api-framebuffer.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture.h>
#include <dali/graphics/vulkan/internal/vulkan-types.h>
#include <dali/graphics/vulkan/internal/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>


namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

Framebuffer::Framebuffer( Controller& controller, uint32_t width, uint32_t height )

: mController( controller ),
  mWidth( width ),
  mHeight( height )
{
}

Framebuffer::~Framebuffer()
{
}

bool Framebuffer::Initialise( const std::vector<FramebufferFactory::ColorAttachment>& colorAttachments,
                              const FramebufferFactory::DepthAttachment& depthAttachment )
{
  std::vector< Vulkan::RefCountedFramebufferAttachment > colorAttachmentObjects;
  Vulkan::RefCountedFramebufferAttachment depthAttachmentObject;

  for( auto&& element : colorAttachments )
  {
    assert( element.texture != nullptr );
    auto texture = static_cast<VulkanAPI::Texture*>( element.texture );

    // @todo handle mipmap level and cubemap
    auto imageView = texture->GetImageViewRef();

    // @todo Get a clear color from API.
    auto clearColor = vk::ClearColorValue{}.setFloat32( { 1.0f, 0.0f, 1.0f, 1.0f } );
    colorAttachmentObjects.push_back( Vulkan::FramebufferAttachment::NewColorAttachment( imageView, clearColor, false ) );
  }

  if( depthAttachment.texture != nullptr )
  {
    auto texture = dynamic_cast<VulkanAPI::Texture *>( depthAttachment.texture );
    if( texture )
    {
      // @todo handle mipmap level and cubemap
      auto imageView = texture->GetImageViewRef();

      // @todo Get a clear color from API.
      auto depthClearValue = vk::ClearDepthStencilValue{}.setDepth( 0.0 ).setStencil( 1 );
      depthAttachmentObject = Vulkan::FramebufferAttachment::NewDepthAttachment( imageView, depthClearValue );
    }
  }

  mFramebufferRef = mController.GetGraphics().CreateFramebuffer( colorAttachmentObjects,
                                                                 depthAttachmentObject,
                                                                 mWidth,
                                                                 mHeight );

  return true;
}

Vulkan::RefCountedFramebuffer Framebuffer::GetFramebufferRef() const
{
  return mFramebufferRef;
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
