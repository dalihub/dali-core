#ifndef DALI_GRAPHICS_API_FRAMEBUFFER_FACTORY_H
#define DALI_GRAPHICS_API_FRAMEBUFFER_FACTORY_H

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

#include <dali/graphics-api/graphics-api-base-factory.h>
#include <dali/graphics-api/graphics-api-framebuffer.h>
#include <dali/graphics-api/graphics-api-texture-details.h>
#include <dali/graphics-api/graphics-api-texture.h>

namespace Dali
{
namespace Graphics
{

/**
 * @brief Interface class for FramebufferFactory types in the graphics API.
 */
class FramebufferFactory : public BaseFactory< Framebuffer >
{
public:
  /**
   * @brief Set the size of framebuffer
   */
  virtual FramebufferFactory& SetSize( const Extent2D& size ) = 0;

  /**
   * @brief Set a color attachment texture on the framebuffer
   */
  virtual FramebufferFactory& SetColorAttachment( TextureDetails::AttachmentId attachmentIndex,
                                                  const Texture&               texture,
                                                  TextureDetails::LayerId      layer,
                                                  TextureDetails::LevelId      level ) = 0;

  /**
   * @brief Set a depth/stencil attachment texture on the framebuffer. Queries driver to provide
   * optimal format.
   */
  virtual FramebufferFactory& SetDepthStencilAttachment( const Texture&                   texture,
                                                         TextureDetails::LayerId          layer,
                                                         TextureDetails::LevelId          level,
                                                         TextureDetails::DepthStencilFlag depthStencilFlag ) = 0;

  // not copyable
  FramebufferFactory(const FramebufferFactory&) = delete;
  FramebufferFactory& operator=(const FramebufferFactory&) = delete;

  virtual ~FramebufferFactory() = default;

protected:
  /// @brief default constructor
  FramebufferFactory() = default;

  // derived types should not be moved direcly to prevent slicing
  FramebufferFactory(FramebufferFactory&&) = default;
  FramebufferFactory& operator=(FramebufferFactory&&) = default;
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_FRAMEBUFFER_FACTORY_H
