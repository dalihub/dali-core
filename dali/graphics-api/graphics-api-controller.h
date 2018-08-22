#ifndef DALI_GRAPHICS_API_CONTROLLER_H
#define DALI_GRAPHICS_API_CONTROLLER_H

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
#include <dali/graphics-api/graphics-api-base-factory.h>
#include <dali/graphics-api/graphics-api-framebuffer.h>
#include <dali/graphics-api/graphics-api-framebuffer-factory.h>
#include <dali/graphics-api/graphics-api-render-command.h>
#include <dali/graphics-api/graphics-api-sampler.h>
#include <dali/graphics-api/graphics-api-shader.h>
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics-api/graphics-api-shader-factory.h>
#include <dali/graphics-api/graphics-api-texture-factory.h>
#include <dali/graphics-api/graphics-api-texture.h>
#include <dali/graphics-api/graphics-api-buffer.h>
#include <dali/graphics-api/graphics-api-buffer-factory.h>
#include <dali/graphics-api/graphics-api-pipeline-factory.h>
#include <dali/graphics-api/graphics-api-pipeline.h>
#include <dali/graphics-api/graphics-api-sampler-factory.h>
#include <dali/graphics-api/utility/utility-builder.h>

namespace Dali
{
namespace Graphics
{
namespace API
{
class ShaderFactory;
/**
 * @brief Interface class for Manager types in the graphics API.
 */
class Controller
{
public:

  virtual ~Controller() = default;

  /**
   * @brief Create a new object
   */
  virtual std::unique_ptr<Shader> CreateShader( const BaseFactory<Shader>& factory ) = 0;

  /**
   * @brief Create a new object
   */
  virtual std::unique_ptr<Texture> CreateTexture( const BaseFactory<Texture>& factory ) = 0;

  /**
   * @brief Create a new object
   */
  virtual std::unique_ptr<Buffer> CreateBuffer( const BaseFactory<Buffer>& factory ) = 0;

  /**
   * @brief Create a new object
   */
  virtual std::unique_ptr<Sampler> CreateSampler( const BaseFactory<Sampler>& factory ) = 0;

  /**
   * @brief Create a new object
   */
  virtual std::unique_ptr<Framebuffer> CreateFramebuffer( const BaseFactory<Framebuffer>& factory ) = 0;

  /**
   * Creates new pipeline
   */
  virtual std::unique_ptr<Pipeline> CreatePipeline( const PipelineFactory& factory ) = 0;

  /**
   * @brief Returns texture factory
   * @return
   */
  virtual TextureFactory& GetTextureFactory() const = 0;

  /**
   * @brief Returns shader factory
   * @return
   */
  virtual ShaderFactory& GetShaderFactory() const = 0;

  /**
   * @brief Returns Framebuffer factory
   * @return
   */
  virtual FramebufferFactory& GetFramebufferFactory() const = 0;

  /**
   * @brief Returns shader factory
   * @return
   */
  virtual BufferFactory& GetBufferFactory() const = 0;

  /**
   * @brief Returns pipeline factory
   * @return
   */
  virtual PipelineFactory& GetPipelineFactory() = 0;

  /**
   * @brief Returns sampler factory
   * @return
   */
  virtual SamplerFactory& GetSamplerFactory() = 0;

  /**
   * @brief alAllocates render command ( depends on implementation );
   * @return
   */
  virtual std::unique_ptr<API::RenderCommand> AllocateRenderCommand() = 0;

  /**
   * @brief Submits a list of commands
   * @param commands
   */
  virtual void SubmitCommands( std::vector<API::RenderCommand*> commands ) = 0;

  /**
   * @brief Mark the beginning of a frame
   */
  virtual void BeginFrame() = 0;

  /**
   * @brief Mark the end of a frame
   */
  virtual void EndFrame() = 0;

  /**
   * @brief Respond to pause lifecycle event
   */
  virtual void Pause() = 0;

  /**
   * @brief Respond to resume lifecycle event
   */
  virtual void Resume() = 0;

public:
  // not copyable
  Controller( const Controller& ) = delete;
  Controller& operator=( const Controller& ) = delete;

protected:
  // derived types should not be moved direcly to prevent slicing
  Controller( Controller&& ) = default;
  Controller& operator=( Controller&& ) = default;

  /**
   * Objects of this type should not directly.
   */
  Controller() = default;

private:
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_CONTROLLER_H
