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

// CLASS HEADER
#include <dali/internal/update/rendering/scene-graph-frame-buffer.h>

// INTERNAL INCLUDES
#include <dali/internal/update/rendering/scene-graph-texture.h>
#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-texture-details.h>
#include <dali/graphics-api/graphics-api-framebuffer.h>
#include <dali/graphics-api/graphics-api-framebuffer-factory.h>
#include <dali/graphics-api/graphics-api-utility.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

FrameBuffer::FrameBuffer( unsigned int width, unsigned int height, unsigned int attachments )
: mGraphics( nullptr ),
  mGraphicsFramebuffer( nullptr ),
  mWidth( width ),
  mHeight( height )
{
}

FrameBuffer::~FrameBuffer() = default;

void FrameBuffer::Initialize( Integration::Graphics::GraphicsInterface& graphics )
{
  mGraphics = &graphics;
  // Defer creation of framebuffer until a render instruction uses it
}

void FrameBuffer::AttachColorTexture( SceneGraph::Texture* texture, unsigned int mipmapLevel, unsigned int layer )
{
  mColorAttachment.texture = texture;
  mColorAttachment.mipmapLevel = mipmapLevel;
  mColorAttachment.layer = layer;
}

void FrameBuffer::AttachDepthStencilTexture( SceneGraph::Texture* texture,
                                             Dali::FrameBuffer::Attachment::Mask format,
                                             unsigned int mipmapLevel,
                                             unsigned int layer )
{
  mDepthAttachment.texture = texture;
  mDepthAttachment.format = format;
  mDepthAttachment.mipmapLevel = mipmapLevel;
  mDepthAttachment.layer = layer;
}

unsigned int FrameBuffer::GetWidth() const
{
  return mWidth;
}

unsigned int FrameBuffer::GetHeight() const
{
  return mHeight;
}

void FrameBuffer::PrepareFramebuffer()
{
  auto& controller = mGraphics->GetController();

  // @todo Check if the color attachments or depth attachments have changed since the graphics object was created
  if( !mGraphicsFramebuffer )
  {
    auto& factory = controller.GetFramebufferFactory();
    factory.SetSize( Graphics::API::RectSize{ mWidth, mHeight } );

    /**
     * If the framebuffer has a color attachment without a graphics object, create one
     */
    if( mColorAttachment.texture != nullptr )
    {
      auto graphicsTexture = mColorAttachment.texture->GetGfxObject();
      //@todo check if the texture has the right creation flags - if it's already been uploaded, it
      // needs the layout & usage changing appropriately.
      if( !graphicsTexture )
      {
        mColorAttachment.texture->CreateTexture( Texture::Usage::COLOR_ATTACHMENT );
        graphicsTexture = mColorAttachment.texture->GetGfxObject();
      }

      factory.SetColorAttachment( 0, // index of attachment (could have multiple attachments)
                                  *graphicsTexture, mColorAttachment.mipmapLevel, mColorAttachment.layer );
    }

    if( mDepthAttachment.texture != nullptr )
    {
      auto graphicsTexture = mDepthAttachment.texture->GetGfxObject();
      if( graphicsTexture )
      {
        auto depthStencilFormat = Graphics::API::TextureDetails::DepthStencilFlag::NONE;
        switch( mDepthAttachment.format )
        {
          case Dali::FrameBuffer::Attachment::NONE:
          {
            depthStencilFormat = Graphics::API::TextureDetails::DepthStencilFlag::NONE;
            break;
          }
          case Dali::FrameBuffer::Attachment::DEPTH:
          {
            depthStencilFormat = Graphics::API::TextureDetails::DepthStencilFlag::DEPTH;
            break;
          }
          case Dali::FrameBuffer::Attachment::STENCIL:
          {
            depthStencilFormat = Graphics::API::TextureDetails::DepthStencilFlag::STENCIL;
            break;
          }
          case Dali::FrameBuffer::Attachment::DEPTH_STENCIL:
          {
            depthStencilFormat = Graphics::API::TextureDetails::DepthStencilFlag::DEPTH_STENCIL;
            break;
          }
        }

        factory.SetDepthStencilAttachment( *graphicsTexture, mDepthAttachment.mipmapLevel, mDepthAttachment.layer,
                                           depthStencilFormat );
      }
    }

    mGraphicsFramebuffer = controller.CreateFramebuffer( factory );
  }
}

const Graphics::API::Framebuffer* FrameBuffer::GetGfxObject() const
{
  return mGraphicsFramebuffer.get();
}

void FrameBuffer::DestroyGraphicsObjects()
{
  mGraphicsFramebuffer.reset();
}

} //Render

} //Internal

} //Dali
