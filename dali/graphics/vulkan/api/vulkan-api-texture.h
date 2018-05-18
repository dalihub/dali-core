#ifndef DALI_GRAPHICS_VULKAN_API_TEXTURE_H
#define DALI_GRAPHICS_VULKAN_API_TEXTURE_H

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

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-texture-factory.h>
#include <dali/graphics-api/graphics-api-texture.h>
#include <dali/graphics/vulkan/vulkan-types.h>
#include <dali/graphics/vulkan/vulkan-graphics-texture.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
/**
 * This is temporary implementation. It should be using graphics-texture as base
 * interface.
 */
class Texture : public Dali::Graphics::API::Texture
{
public:

  explicit Texture( Dali::Graphics::API::TextureFactory& factory );
  ~Texture() override;

  bool Initialise();

  Vulkan::TextureRef GetTextureRef() const;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
#endif // DALI_GRAPHICS_VULKAN_API_TEXTURE_H
