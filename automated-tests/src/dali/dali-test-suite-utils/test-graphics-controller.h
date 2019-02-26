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
#include "test-trace-call-stack.h"

#include "test-graphics-texture-factory.h"
#include "test-graphics-shader-factory.h"
#include "test-graphics-buffer-factory.h"
#include "test-graphics-framebuffer-factory.h"
#include "test-graphics-sampler-factory.h"
#include "test-graphics-pipeline-factory.h"
#include "test-graphics-render-command.h"

namespace Test
{

class GraphicsController : public Dali::Graphics::Controller
{
public:
  explicit GraphicsController();

  virtual ~GraphicsController();

  /**
   * @brief Create a new object
   */
  std::unique_ptr<Dali::Graphics::Shader> CreateShader( const Dali::Graphics::BaseFactory<Dali::Graphics::Shader>& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr<Dali::Graphics::Texture> CreateTexture( const Dali::Graphics::BaseFactory<Dali::Graphics::Texture>& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr<Dali::Graphics::Buffer> CreateBuffer( const Dali::Graphics::BaseFactory<Dali::Graphics::Buffer>& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr<Dali::Graphics::Sampler> CreateSampler( const Dali::Graphics::BaseFactory<Dali::Graphics::Sampler>& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr<Dali::Graphics::Framebuffer> CreateFramebuffer( const Dali::Graphics::BaseFactory<Dali::Graphics::Framebuffer>& factory ) override;

  /**
   * Creates new pipeline
   */
  std::unique_ptr<Dali::Graphics::Pipeline> CreatePipeline( const Dali::Graphics::PipelineFactory& factory ) override;

  /**
   * @brief Returns texture factory
   * @return
   */
  Dali::Graphics::TextureFactory& GetTextureFactory() const override;

  /**
   * @brief Returns shader factory
   * @return
   */
  Dali::Graphics::ShaderFactory& GetShaderFactory() const override;

  /**
   * @brief Returns Framebuffer factory
   * @return
   */
  Dali::Graphics::FramebufferFactory& GetFramebufferFactory() const override;

  /**
   * @brief Returns shader factory
   * @return
   */
  Dali::Graphics::BufferFactory& GetBufferFactory() const override;

  /**
   * @brief Returns pipeline factory
   * @return
   */
  Dali::Graphics::PipelineFactory& GetPipelineFactory() override;

  /**
   * @brief Returns sampler factory
   * @return
   */
  Dali::Graphics::SamplerFactory& GetSamplerFactory() override;

  /**
   * @brief alAllocates render command ( depends on implementation );
   * @return
   */
  std::unique_ptr<Dali::Graphics::RenderCommand> AllocateRenderCommand() override;

  /**
   * @brief Submits a list of commands
   * @param commands
   */
  void SubmitCommands( std::vector<Dali::Graphics::RenderCommand*> commands ) override;

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
   * Updates multiple textures
   * @param updateInfoList list of update info structures
   * @param sourceList  list of source info structures
   */
  void UpdateTextures( const std::vector<Dali::Graphics::TextureUpdateInfo>& updateInfoList, const std::vector<Dali::Graphics::TextureUpdateSourceInfo>& sourceList ) override;

  /**
   * Returns number of buffers allocated by the swapchain
   * @return
   */
  uint32_t GetSwapchainBufferCount() override;

public: // For test cases
  std::vector<Test::GraphicsRenderCommand>& GetRenderCommands()
  {
    return mRenderCommands;
  }
  std::vector<Dali::Graphics::Texture*>& GetTextures()
  {
    return mTextures;
  }

public:
  // not copyable
  GraphicsController( const GraphicsController& ) = delete;
  GraphicsController& operator=( const GraphicsController& ) = delete;

protected:
  // derived types should not be moved direcly to prevent slicing
  GraphicsController( GraphicsController&& ) = default;
  GraphicsController& operator=( GraphicsController&& ) = default;


public: // For test framework
  void DestroyShader( Dali::Graphics::Shader* shader );
  void DestroyTexture( Dali::Graphics::Texture* texture );
  void DestroyFramebuffer( Dali::Graphics::Framebuffer* framebuffer );
  void DestroyBuffer( Dali::Graphics::Buffer* buffer );
  void DestroySampler( Dali::Graphics::Sampler* sampler );
  void DestroyPipeline( Dali::Graphics::Pipeline* pipeline );


public:
  GraphicsTextureFactory mTextureFactory;
  GraphicsShaderFactory mShaderFactory;
  GraphicsFramebufferFactory mFramebufferFactory;
  GraphicsBufferFactory mBufferFactory;
  GraphicsPipelineFactory mPipelineFactory;
  GraphicsSamplerFactory mSamplerFactory;

  TraceCallStack mTextureTrace;
  TraceCallStack mShaderTrace;
  TraceCallStack mBufferTrace;
  TraceCallStack mControllerTrace;

  std::vector<Dali::Graphics::Texture*> mTextures;

  // Add these as needed
  //std::vector<Dali::Graphics::Shader*> mShaders;
  //std::vector<Dali::Graphics::Framebuffer*> mFramebuffers;
  //std::vector<Dali::Graphics::Buffer*> mBuffers;
  //std::vector<Dali::Graphics::Sampler*> mSampler;
  //std::vector<Dali::Graphics::Pipeline*> mPipeline;

  std::vector<Test::GraphicsRenderCommand> mRenderCommands;
};

} // namespace Test

#endif // TEST_GRAPHICS_CONTROLLER_H
