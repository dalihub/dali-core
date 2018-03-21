#ifndef DALI_GRAPHICS_VULAKN_GRAPHICS_CONTROLLER_H
#define DALI_GRAPHICS_VULAKN_GRAPHICS_CONTROLLER_H

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
#include <dali/graphics/graphics-controller.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;

/**
 * @brief Interface class for Manager types in the graphics API.
 */
class Controller : public Dali::Graphics::Controller
{
public:

  explicit Controller( Vulkan::Graphics& vulkanGraphics );

  /**
   * Must be created from VulkanGraphics
   * @param vulkanGraphics
   * @return
   */
  static std::unique_ptr<Controller> New( Vulkan::Graphics& vulkanGraphics );

  /**
   * @brief Create a new object
   */
  API::Accessor<API::Shader> CreateShader( const API::BaseFactory<API::Shader>& factory ) override;

  /**
   * @brief Create a new object
   */
  API::Accessor<API::Texture> CreateTexture( const API::BaseFactory<API::Texture>& factory ) override;

  /**
   * @brief Create a new object
   */
  API::Accessor<API::TextureSet> CreateTextureSet( const API::BaseFactory<API::TextureSet>& factory ) override;

  /**
   * @brief Create a new object
   */
  API::Accessor<API::DynamicBuffer> CreateDynamicBuffer( const API::BaseFactory<API::DynamicBuffer>& factory ) override;

  /**
   * @brief Create a new object
   */
  API::Accessor<API::StaticBuffer> CreateStaticBuffer( const API::BaseFactory<API::StaticBuffer>& factory ) override;

  /**
   * @brief Create a new object
   */
  API::Accessor<API::Sampler> CreateSampler( const API::BaseFactory<API::Sampler>& factory ) override;

  /**
   * @brief Create a new object
   */
  API::Accessor<API::Framebuffer> CreateFramebuffer( const API::BaseFactory<API::Framebuffer>& factory ) override;

  std::unique_ptr<char> CreateBuffer( size_t numberOfElements, size_t elementSize ) override;

  void* CreateTextureRGBA32( void* data, size_t sizeInBytes, uint32_t width, uint32_t height ) override;

  /**
   * @brief Get a render list
   */
  void GetRenderItemList() override;

  void SubmitCommand( API::RenderCommand&& command ) override;

  void BeginFrame() override;

  void EndFrame() override;

public:
  // not copyable
  Controller( const Controller& ) = delete;
  Controller& operator=( const Controller& ) = delete;

  ~Controller() override;

protected:
  // derived types should not be moved direcly to prevent slicing
  Controller( Controller&& );
  Controller& operator=( Controller&& );

  /**
   * Objects of this type should not directly.
   */
  Controller();

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULAKN_GRAPHICS_CONTROLLER_H
