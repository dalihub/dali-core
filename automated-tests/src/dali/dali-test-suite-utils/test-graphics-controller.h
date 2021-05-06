#ifndef TEST_GRAPHICS_CONTROLLER_H
#define TEST_GRAPHICS_CONTROLLER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-controller.h>
#include "test-gl-abstraction.h"
#include "test-gl-context-helper-abstraction.h"
#include "test-gl-sync-abstraction.h"
#include "test-graphics-command-buffer.h"
#include "test-graphics-program.h"
#include "test-graphics-reflection.h"

namespace Dali
{
std::ostream& operator<<(std::ostream& o, const Graphics::BufferCreateInfo& bufferCreateInfo);
std::ostream& operator<<(std::ostream& o, const Graphics::CommandBufferCreateInfo& commandBufferCreateInfo);
std::ostream& operator<<(std::ostream& o, const Graphics::TextureType& textureType);
std::ostream& operator<<(std::ostream& o, const Graphics::Extent2D extent);
std::ostream& operator<<(std::ostream& o, const Graphics::TextureCreateInfo& createInfo);
std::ostream& operator<<(std::ostream& o, Graphics::SamplerAddressMode addressMode);
std::ostream& operator<<(std::ostream& o, Graphics::SamplerFilter filterMode);
std::ostream& operator<<(std::ostream& o, Graphics::SamplerMipmapMode mipmapMode);
std::ostream& operator<<(std::ostream& o, const Graphics::SamplerCreateInfo& createInfo);

template<typename T>
T* Uncast(const Graphics::CommandBuffer* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::Texture* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::Sampler* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::Buffer* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::Shader* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::Framebuffer* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::Pipeline* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::RenderTarget* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

class TestGraphicsController : public Dali::Graphics::Controller
{
public:
  TestGraphicsController();

  virtual ~TestGraphicsController() = default;

  void Initialize()
  {
    mGl.Initialize();
  }

  Integration::GlAbstraction& GetGlAbstraction() override
  {
    return mGl;
  }

  Integration::GlSyncAbstraction& GetGlSyncAbstraction() override
  {
    return mGlSyncAbstraction;
  }

  Integration::GlContextHelperAbstraction& GetGlContextHelperAbstraction() override
  {
    return mGlContextHelperAbstraction;
  }

  void SubmitCommandBuffers(const Graphics::SubmitInfo& submitInfo) override;

  /**
   * @brief Presents render target
   * @param renderTarget render target to present
   */
  void PresentRenderTarget(Graphics::RenderTarget* renderTarget) override;

  /**
   * @brief Waits until the GPU is idle
   */
  void WaitIdle() override;

  /**
   * @brief Lifecycle pause event
   */
  void Pause() override;

  /**
   * @brief Lifecycle resume event
   */
  void Resume() override;

  /**
   * @brief Lifecycle shutdown event
   */
  void Shutdown() override;

  /**
   * @brief Lifecycle destroy event
   */
  void Destroy() override;

  /**
   * @brief Executes batch update of textures
   *
   * This function may perform full or partial update of many textures.
   * The data source may come from:
   * - CPU memory (client side)
   * - GPU memory (another Texture or Buffer)
   *
   * UpdateTextures() is the only way to update unmappable Texture objects.
   * It is recommended to batch updates as it may help with optimizing
   * memory transfers based on dependencies.
   *
   */
  void UpdateTextures(const std::vector<Graphics::TextureUpdateInfo>&       updateInfoList,
                      const std::vector<Graphics::TextureUpdateSourceInfo>& sourceList) override;

  /**
   * Auto generates mipmaps for the texture
   * @param[in] texture The texture
   */
  void GenerateTextureMipmaps(const Graphics::Texture& texture) override;

  /**
   * TBD: do we need those functions in the new implementation?
   */
  bool EnableDepthStencilBuffer(bool enableDepth, bool enableStencil) override;

  void RunGarbageCollector(size_t numberOfDiscardedRenderers) override;

  void DiscardUnusedResources() override;

  bool IsDiscardQueueEmpty() override;

  /**
   * @brief Test if the graphics subsystem has resumed & should force a draw
   *
   * @return true if the graphics subsystem requires a re-draw
   */
  bool IsDrawOnResumeRequired() override;

  /**
   * @brief Creates new Buffer object
   *
   * The Buffer object is created with underlying memory. The Buffer
   * specification is immutable. Based on the BufferCreateInfo::usage,
   * the memory may be client-side mappable or not.
   *
   * The old buffer may be passed as BufferCreateInfo::oldbuffer, however,
   * it's up to the implementation whether the object will be reused or
   * discarded and replaced by the new one.
   *
   * @param[in] bufferCreateInfo The valid BufferCreateInfo structure
   * @return pointer to the Buffer object
   */
  Graphics::UniquePtr<Graphics::Buffer> CreateBuffer(const Graphics::BufferCreateInfo& bufferCreateInfo, Graphics::UniquePtr<Graphics::Buffer>&& oldBuffer) override;

  /**
   * @brief Creates new CommandBuffer object
   *
   * @param[in] bufferCreateInfo The valid BufferCreateInfo structure
   * @return pointer to the CommandBuffer object
   */
  Graphics::UniquePtr<Graphics::CommandBuffer> CreateCommandBuffer(const Graphics::CommandBufferCreateInfo& commandBufferCreateInfo, Graphics::UniquePtr<Graphics::CommandBuffer>&& oldCommandBuffer) override;

  /**
   * @brief Creates new RenderPass object
   *
   * @param[in] renderPassCreateInfo The valid RenderPassCreateInfo structure
   * @return pointer to the RenderPass object
   */
  Graphics::UniquePtr<Graphics::RenderPass> CreateRenderPass(const Graphics::RenderPassCreateInfo& renderPassCreateInfo, Graphics::UniquePtr<Graphics::RenderPass>&& oldRenderPass) override;

  /**
   * @brief Creates new Texture object
   *
   * @param[in] textureCreateInfo The valid TextureCreateInfo structure
   * @return pointer to the TextureCreateInfo object
   */
  Graphics::UniquePtr<Graphics::Texture> CreateTexture(const Graphics::TextureCreateInfo& textureCreateInfo, Graphics::UniquePtr<Graphics::Texture>&& oldTexture) override;

  /**
   * @brief Creates new Framebuffer object
   *
   * @param[in] framebufferCreateInfo The valid FramebufferCreateInfo structure
   * @return pointer to the Framebuffer object
   */
  Graphics::UniquePtr<Graphics::Framebuffer> CreateFramebuffer(const Graphics::FramebufferCreateInfo& framebufferCreateInfo, Graphics::UniquePtr<Graphics::Framebuffer>&& oldFramebuffer) override;

  /**
   * @brief Creates new Pipeline object
   *
   * @param[in] pipelineCreateInfo The valid PipelineCreateInfo structure
   * @return pointer to the Pipeline object
   */
  Graphics::UniquePtr<Graphics::Pipeline> CreatePipeline(const Graphics::PipelineCreateInfo& pipelineCreateInfo, Graphics::UniquePtr<Graphics::Pipeline>&& oldPipeline) override;

  /**
   * @brief Creates new Program object
   *
   * @param[in] programCreateInfo The valid ProgramCreateInfo structure
   * @return pointer to the Program object
   */
  Graphics::UniquePtr<Graphics::Program> CreateProgram(const Graphics::ProgramCreateInfo& programCreateInfo, Graphics::UniquePtr<Graphics::Program>&& oldProgram) override;

  /**
   * @brief Creates new Shader object
   *
   * @param[in] shaderCreateInfo The valid ShaderCreateInfo structure
   * @return pointer to the Shader object
   */
  Graphics::UniquePtr<Graphics::Shader> CreateShader(const Graphics::ShaderCreateInfo& shaderCreateInfo, Graphics::UniquePtr<Graphics::Shader>&& oldShader) override;

  /**
   * @brief Creates new Sampler object
   *
   * @param[in] samplerCreateInfo The valid SamplerCreateInfo structure
   * @return pointer to the Sampler object
   */
  Graphics::UniquePtr<Graphics::Sampler> CreateSampler(const Graphics::SamplerCreateInfo& samplerCreateInfo, Graphics::UniquePtr<Graphics::Sampler>&& oldSampler) override;

  /**
   * @brief Creates new RenderTarget object
   *
   * @param[in] renderTargetCreateInfo The valid RenderTargetCreateInfo structure
   * @return pointer to the RenderTarget object
   */
  Graphics::UniquePtr<Graphics::RenderTarget> CreateRenderTarget(const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo, Graphics::UniquePtr<Graphics::RenderTarget>&& oldRenderTarget) override;

  /**
   * @brief Maps memory associated with Buffer object
   *
   * @param[in] mapInfo Filled details of mapped resource
   *
   * @return Returns pointer to Memory object or Graphicsnullptr on error
   */
  Graphics::UniquePtr<Graphics::Memory> MapBufferRange(const Graphics::MapBufferInfo& mapInfo) override;

  /**
   * @brief Maps memory associated with the texture.
   *
   * Only Texture objects that are backed with linear memory (staging memory) can be mapped.
   * Example:
   * 1) GLES implementation may create PBO object as staging memory and couple it
   * with the texture. Texture can be mapped and the memory can be read/write on demand.
   *
   * 2) Vulkan implementation may allocate DeviceMemory and use linear layout.
   *
   * @param[in] mapInfo Filled details of mapped resource
   *
   * @return Valid Memory object or nullptr on error
   */
  Graphics::UniquePtr<Graphics::Memory> MapTextureRange(const Graphics::MapTextureInfo& mapInfo) override;

  /**
   * @brief Unmaps memory and discards Memory object
   *
   * This function automatically removes lock if Memory has been
   * previously locked.
   *
   * @param[in] memory Valid and previously mapped Memory object
   */
  void UnmapMemory(Graphics::UniquePtr<Graphics::Memory> memory) override;

  /**
   * @brief Returns memory requirements of the Texture object.
   *
   * Call this function whenever it's necessary to know how much memory
   * is needed to store all the texture data and what memory alignment
   * the data should follow.
   *
   * @return Returns memory requirements of Texture
   */
  Graphics::MemoryRequirements GetTextureMemoryRequirements(Graphics::Texture& texture) const override;

  /**
   * @brief Returns memory requirements of the Buffer object.
   *
   * Call this function whenever it's necessary to know how much memory
   * is needed to store all the buffer data and what memory alignment
   * the data should follow.
   *
   * @return Returns memory requirements of Buffer
   */
  Graphics::MemoryRequirements GetBufferMemoryRequirements(Graphics::Buffer& buffer) const override;

  /**
   * @brief Returns specification of the Texture object
   *
   * Function obtains specification of the Texture object. It may retrieve
   * implementation dependent details like ie. whether the texture is
   * emulated (for example, RGB emulated on RGBA), compressed etc.
   *
   * @return Returns the TextureProperties object
   */
  const Graphics::TextureProperties& GetTextureProperties(const Graphics::Texture& texture) override;

  /**
   * @brief Returns the reflection of the given program
   *
   * @param[in] program The program
   * @return The reflection of the program
   */
  const Graphics::Reflection& GetProgramReflection(const Graphics::Program& program) override;

  /**
   * @brief Tests whether two Pipelines are the same.
   *
   * On the higher level, this function may help wit creating pipeline cache.
   *
   * @return true if pipeline objects match
   */
  bool PipelineEquals(const Graphics::Pipeline& pipeline0, const Graphics::Pipeline& pipeline1) const override;

public: // Test Functions
  void SetVertexFormats(Property::Array& vfs)
  {
    mVertexFormats = vfs;
  }

  void AddCustomUniforms(std::vector<UniformData>& customUniforms)
  {
    mCustomUniforms = customUniforms;
  }

  void ClearSubmitStack()
  {
    mSubmitStack.clear();
  }

  /**
   * @brief Retrieves program parameters
   *
   * This function can be used to retrieve data from internal implementation
   *
   * @param[in] program Valid program object
   * @param[in] parameterId Integer parameter id
   * @param[out] outData Pointer to output memory
   * @return True on success
   */
  bool GetProgramParameter(Graphics::Program& program, uint32_t parameterId, void* outData) override;

  void ProcessCommandBuffer(TestGraphicsCommandBuffer& commandBuffer);

  void BindPipeline(TestGraphicsPipeline* pipeline);

public:
  mutable TraceCallStack                    mCallStack;
  mutable TraceCallStack                    mCommandBufferCallStack;
  mutable TraceCallStack                    mFrameBufferCallStack;
  mutable std::vector<Graphics::SubmitInfo> mSubmitStack;

  TestGlAbstraction              mGl;
  TestGlSyncAbstraction          mGlSyncAbstraction;
  TestGlContextHelperAbstraction mGlContextHelperAbstraction;

  bool isDiscardQueueEmptyResult{true};
  bool isDrawOnResumeRequiredResult{true};

  Property::Array mVertexFormats;

  struct ProgramCache
  {
    std::map<Graphics::PipelineStage, std::vector<uint8_t>> shaders;
    TestGraphicsProgramImpl*                                programImpl;
  };
  std::vector<ProgramCache> mProgramCache;

  struct PipelineCache
  {
  };

  std::vector<UniformData> mCustomUniforms;
};

} // namespace Dali

#endif //TEST_GRAPHICS_CONTROLLER_H
