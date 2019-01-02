#ifndef TEST_GRAPHICS_CONTROLLER_H
#define TEST_GRAPHICS_CONTROLLER_H

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

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-controller.h>

#include "test-graphics-texture-factory.h"
#include "test-graphics-shader-factory.h"
#include "test-graphics-buffer-factory.h"
#include "test-graphics-framebuffer-factory.h"
#include "test-graphics-sampler-factory.h"
#include "test-graphics-pipeline-factory.h"

namespace Test
{

class GraphicsController : public Dali::Graphics::API::Controller
{
public:
  explicit GraphicsController();

  virtual ~GraphicsController();

  /**
   * @brief Create a new object
   */
  std::unique_ptr<Dali::Graphics::API::Shader> CreateShader( const Dali::Graphics::API::BaseFactory<Dali::Graphics::API::Shader>& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr<Dali::Graphics::API::Texture> CreateTexture( const Dali::Graphics::API::BaseFactory<Dali::Graphics::API::Texture>& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr<Dali::Graphics::API::Buffer> CreateBuffer( const Dali::Graphics::API::BaseFactory<Dali::Graphics::API::Buffer>& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr<Dali::Graphics::API::Sampler> CreateSampler( const Dali::Graphics::API::BaseFactory<Dali::Graphics::API::Sampler>& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr<Dali::Graphics::API::Framebuffer> CreateFramebuffer( const Dali::Graphics::API::BaseFactory<Dali::Graphics::API::Framebuffer>& factory ) override;

  /**
   * Creates new pipeline
   */
  std::unique_ptr<Dali::Graphics::API::Pipeline> CreatePipeline( const Dali::Graphics::API::PipelineFactory& factory ) override;

  /**
   * @brief Returns texture factory
   * @return
   */
  Dali::Graphics::API::TextureFactory& GetTextureFactory() const override;

  /**
   * @brief Returns shader factory
   * @return
   */
  Dali::Graphics::API::ShaderFactory& GetShaderFactory() const override;

  /**
   * @brief Returns Framebuffer factory
   * @return
   */
  Dali::Graphics::API::FramebufferFactory& GetFramebufferFactory() const override;

  /**
   * @brief Returns shader factory
   * @return
   */
  Dali::Graphics::API::BufferFactory& GetBufferFactory() const override;

  /**
   * @brief Returns pipeline factory
   * @return
   */
  Dali::Graphics::API::PipelineFactory& GetPipelineFactory() override;

  /**
   * @brief Returns sampler factory
   * @return
   */
  Dali::Graphics::API::SamplerFactory& GetSamplerFactory() override;

  /**
   * @brief alAllocates render command ( depends on implementation );
   * @return
   */
  std::unique_ptr<Dali::Graphics::API::RenderCommand> AllocateRenderCommand() override;

  /**
   * @brief Submits a list of commands
   * @param commands
   */
  void SubmitCommands( std::vector<Dali::Graphics::API::RenderCommand*> commands ) override;

  /**
   * @brief Mark the beginning of a frame
   */
  void BeginFrame() override;

  /**
   * @brief Mark the end of a frame
   */
  void EndFrame() override;

  /**
   * @brief Respond to pause lifecycle event
   */
  void Pause() override;

  /**
   * @brief Respond to resume lifecycle event
   */
  void Resume() override;

  /**
   * @brief Enables depth/stencil buffer if supported
   *
   * @return True if state changed, False otherwise
   */
  bool EnableDepthStencilBuffer( bool enableDepth, bool enableStencil ) override;

  /**
   * @brief Inform graphics backend if renderers have been discarded.
   *
   * @note The graphics backend does not have to run the GC if it is not overly fragmented
   */
  void RunGarbageCollector( size_t numberOfDiscardedRenderers ) override;

  /**
   * @brief Discards all the graphics resources by forcing full
   * garbage collection.
   */
  void DiscardUnusedResources() override;

  /**
   * @brief Tests whether the graphics discard queue doesn't contain any resources
   * to be released.
   *
   * @return True if queue is empty
   */
  bool IsDiscardQueueEmpty() override;

  /**
   * @brief Test if the graphics subsystem has resumed & should force a draw
   *
   * @return true if the graphics subsystem requires a re-draw
   */
  bool IsDrawOnResumeRequired() override;

  /**
   * @brief Waits until all previously submitted commands have been fully
   * executed.
   */
  void WaitIdle() override;

  /**
   * @brief Swaps graphics buffers
   */
  void SwapBuffers() override;

  /**
   * Returns number of buffers allocated by the swapchain
   * @return
   */
  uint32_t GetSwapchainBufferCount() override;

public:
  // not copyable
  GraphicsController( const GraphicsController& ) = delete;
  GraphicsController& operator=( const GraphicsController& ) = delete;

protected:
  // derived types should not be moved direcly to prevent slicing
  GraphicsController( GraphicsController&& ) = default;
  GraphicsController& operator=( GraphicsController&& ) = default;

private:
  GraphicsTextureFactory mTextureFactory;
  GraphicsShaderFactory mShaderFactory;
  GraphicsFramebufferFactory mFramebufferFactory;
  GraphicsBufferFactory mBufferFactory;
  GraphicsPipelineFactory mPipelineFactory;
  GraphicsSamplerFactory mSamplerFactory;
};

} // namespace Test

#endif // TEST_GRAPHICS_CONTROLLER_H
