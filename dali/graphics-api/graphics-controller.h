#ifndef DALI_GRAPHICS_CONTROLLER_H
#define DALI_GRAPHICS_CONTROLLER_H

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
 *
 */

// EXTERNAL INCLUDES
#include <memory>
#include <vector>

// INTERNAL INCLUDES
#include "graphics-buffer-create-info.h"
#include "graphics-command-buffer-create-info.h"
#include "graphics-framebuffer-create-info.h"
#include "graphics-memory.h"
#include "graphics-pipeline-create-info.h"
#include "graphics-render-pass-create-info.h"
#include "graphics-render-target-create-info.h"
#include "graphics-sampler-create-info.h"
#include "graphics-shader-create-info.h"
#include "graphics-texture-create-info.h"

namespace Dali
{
namespace Integration
{
class GlAbstraction;
class GlSyncAbstraction;
class GlContextHelperAbstraction;
} // namespace Integration

namespace Graphics
{
class CommandBuffer;
class Command;
class RenderTarget;
class RenderPass;
class Buffer;
class Texture;
class Shader;
class Framebuffer;
class Pipeline;
class Sampler;
class Memory;

/**
 * @brief Controller class controls render loop
 *
 * Controller class is responsible for executing render calls
 * and controlling pipeline state.
 */
class Controller
{
public:
  // Temporary until graphics api is complete
  virtual Integration::GlAbstraction&              GetGlAbstraction()              = 0;
  virtual Integration::GlSyncAbstraction&          GetGlSyncAbstraction()          = 0;
  virtual Integration::GlContextHelperAbstraction& GetGlContextHelperAbstraction() = 0;

  /**
   * @brief Destroys controller
   */
  virtual ~Controller() = default;

  /**
   * @brief Submits array of command buffers
   *
   * Submits command buffers to the graphics pipeline. Submitted commands
   * may be executed instantly or postponed.
   *
   * @param[in] submitInfo Valid SubmitInfo structure
   */
  virtual void SubmitCommandBuffers(const SubmitInfo& submitInfo) = 0;

  /**
   * @brief Presents render target
   * @param renderTarget render target to present
   */
  virtual void PresentRenderTarget(RenderTarget* renderTarget) = 0;

  /**
   * @brief Waits until the GPU is idle
   */
  virtual void WaitIdle() = 0;

  /**
   * @brief Lifecycle pause event
   */
  virtual void Pause() = 0;

  /**
   * @brief Lifecycle resume event
   */
  virtual void Resume() = 0;

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
  virtual void UpdateTextures(const std::vector<TextureUpdateInfo>&       updateInfoList,
                              const std::vector<TextureUpdateSourceInfo>& sourceList) = 0;

  /**
   * @brief Enables depth/stencil buffer
   *
   * @param[in] enableDepth True to enable depth
   * @param[in] enableStencil True to enable stencil
   * @return True on success
   */
  virtual bool EnableDepthStencilBuffer(bool enableDepth, bool enableStencil) = 0;

  /**
   * @brief Runs garbage collector (if supported)
   *
   * @param[in] numberOfDiscardedRenderers number of discarded renderers
   */
  virtual void RunGarbageCollector(size_t numberOfDiscardedRenderers) = 0;

  /**
   * @brief Discards unused resources
   */
  virtual void DiscardUnusedResources() = 0;

  /**
   * @brief Tests whether discard queue is empty
   *
   * @return True if empty
   */
  virtual bool IsDiscardQueueEmpty() = 0;

  /**
   * @brief Test if the graphics subsystem has resumed & should force a draw
   *
   * @return true if the graphics subsystem requires a re-draw
   */
  virtual bool IsDrawOnResumeRequired() = 0;

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
   * @param[in] oldBuffer The valid pointer to the old object or nullptr. The object will be reused or destroyed.
   * @return pointer to the Buffer object
   */
  virtual std::unique_ptr<Buffer> CreateBuffer(const BufferCreateInfo& bufferCreateInfo, std::unique_ptr<Buffer>&& oldBuffer) = 0;

  /**
   * @brief Creates new CommandBuffer object
   *
   * @param[in] bufferCreateInfo The valid BufferCreateInfo structure
   * @param[in] oldCommandBuffer The valid pointer to the old object or nullptr. The object will be reused or destroyed.
   * @return pointer to the CommandBuffer object
   */
  virtual std::unique_ptr<CommandBuffer> CreateCommandBuffer(const CommandBufferCreateInfo& commandBufferCreateInfo, std::unique_ptr<CommandBuffer>&& oldCommandBuffer) = 0;

  /**
   * @brief Creates new RenderPass object
   *
   * @param[in] renderPassCreateInfo The valid RenderPassCreateInfo structure
   * @param[in] oldRenderPass The valid pointer to the old object or nullptr. The object will be reused or destroyed.
   * @return pointer to the RenderPass object
   */
  virtual std::unique_ptr<RenderPass> CreateRenderPass(const RenderPassCreateInfo& renderPassCreateInfo, std::unique_ptr<RenderPass>&& oldRenderPass) = 0;

  /**
   * @brief Creates new Texture object
   *
   * @param[in] textureCreateInfo The valid TextureCreateInfo structure
   * @param[in] oldTexture The valid pointer to the old object or nullptr. The object will be reused or destroyed.
   * @return pointer to the TextureCreateInfo object
   */
  virtual std::unique_ptr<Texture> CreateTexture(const TextureCreateInfo& textureCreateInfo, std::unique_ptr<Texture>&& oldTexture) = 0;

  /**
   * @brief Creates new Framebuffer object
   *
   * @param[in] framebufferCreateInfo The valid FramebufferCreateInfo structure
   * @param[in] oldFramebuffer The valid pointer to the old object or nullptr. The object will be reused or destroyed.
   * @return pointer to the Framebuffer object
   */
  virtual std::unique_ptr<Framebuffer> CreateFramebuffer(const FramebufferCreateInfo& framebufferCreateInfo, std::unique_ptr<Framebuffer>&& oldFramebuffer) = 0;

  /**
   * @brief Creates new Pipeline object
   *
   * @param[in] pipelineCreateInfo The valid PipelineCreateInfo structure
   * @param[in] oldPipeline The valid pointer to the old object or nullptr. The object will be reused or destroyed.
   * @return pointer to the Pipeline object
   */
  virtual std::unique_ptr<Pipeline> CreatePipeline(const PipelineCreateInfo& pipelineCreateInfo, std::unique_ptr<Pipeline>&& oldPipeline) = 0;

  /**
   * @brief Creates new Shader object
   *
   * @param[in] shaderCreateInfo The valid ShaderCreateInfo structure
   * @param[in] oldShader The valid pointer to the old object or nullptr. The object will be reused or destroyed.
   * @return pointer to the Shader object
   */
  virtual std::unique_ptr<Shader> CreateShader(const ShaderCreateInfo& shaderCreateInfo, std::unique_ptr<Shader>&& oldShader) = 0;

  /**
   * @brief Creates new Sampler object
   *
   * @param[in] samplerCreateInfo The valid SamplerCreateInfo structure
   * @param[in] oldSampler The valid pointer to the old object or nullptr. The object will be reused or destroyed.
   * @return pointer to the Sampler object
   */
  virtual std::unique_ptr<Sampler> CreateSampler(const SamplerCreateInfo& samplerCreateInfo, std::unique_ptr<Sampler>&& oldSampler) = 0;

  /**
   * @brief Creates new RenderTarget object
   *
   * @param[in] renderTargetCreateInfo The valid RenderTargetCreateInfo structure
   * @param[in] oldRenderTarget The valid pointer to the old object or nullptr. The object will be reused or destroyed.
   * @return pointer to the RenderTarget object
   */
  virtual std::unique_ptr<RenderTarget> CreateRenderTarget(const RenderTargetCreateInfo& renderTargetCreateInfo, std::unique_ptr<RenderTarget>&& oldRenderTarget) = 0;

  /**
   * @brief Maps memory associated with Buffer object
   *
   * @param[in] mapInfo Filled details of mapped resource
   * @return Returns pointer to Memory object or nullptr on error
   */
  virtual std::unique_ptr<Memory> MapBufferRange(const MapBufferInfo& mapInfo) = 0;

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
  virtual std::unique_ptr<Memory> MapTextureRange(const MapTextureInfo& mapInfo) = 0;

  /**
   * @brief Unmaps memory and discards Memory object
   *
   * This function automatically removes lock if Memory has been
   * previously locked.
   *
   * @param[in] memory Valid and previously mapped Memory object
   */
  virtual void UnmapMemory(std::unique_ptr<Memory> memory) = 0;

  /**
   * @brief Returns memory requirements of the Texture object.
   *
   * Call this function whenever it's necessary to know how much memory
   * is needed to store all the texture data and what memory alignment
   * the data should follow.
   *
   * @return Returns memory requirements of Texture
   */
  virtual MemoryRequirements GetTextureMemoryRequirements(Texture& texture) const = 0;

  /**
   * @brief Returns memory requirements of the Buffer object.
   *
   * Call this function whenever it's necessary to know how much memory
   * is needed to store all the buffer data and what memory alignment
   * the data should follow.
   *
   * @return Returns memory requirements of Buffer
   */
  virtual MemoryRequirements GetBufferMemoryRequirements(Buffer& buffer) const = 0;

  /**
   * @brief Returns specification of the Texture object
   *
   * Function obtains specification of the Texture object. It may retrieve
   * implementation dependent details like ie. whether the texture is
   * emulated (for example, RGB emulated on RGBA), compressed etc.
   *
   * @return Returns the TextureProperties object
   */
  virtual const TextureProperties& GetTextureProperties(const Texture& texture) = 0;

  /**
   * @brief Tests whether two Pipelines are the same.
   *
   * On the higher level, this function may help wit creating pipeline cache.
   *
   * @return true if pipeline objects match
   */
  virtual bool PipelineEquals(const Pipeline& pipeline0, const Pipeline& pipeline1) const = 0;

protected:
  /**
   * Creates controller
   */
  Controller() = default;
};
} // namespace Graphics
} // namespace Dali

#endif
