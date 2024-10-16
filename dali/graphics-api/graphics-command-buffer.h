#ifndef DALI_GRAPHICS_COMMAND_BUFFER_H
#define DALI_GRAPHICS_COMMAND_BUFFER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include "graphics-types.h"

namespace Dali::Graphics
{
class Buffer;
class Pipeline;
class RenderTarget;
class RenderPass;
class Sampler;
class SyncObject;
class Texture;

/**
 * @brief Uniform buffer bindings.
 */
struct UniformBufferBinding
{
  Buffer* buffer; // Buffer
  union
  {
    void*    offsetPtr; // pointer to the client-side memory
    uint32_t offset;    // Offset within buffer
  };
  uint32_t dataSize; // Size of data to bind
  uint32_t binding;  // Binding index
};

/**
 * @brief Texture bindings
 *
 * Additionally, sampler may be used in case of having combined
 * image and sampler.
 *
 */
struct TextureBinding
{
  const Texture* texture; // texture to be bound
  const Sampler* sampler; // sampler to be bound
  uint32_t       binding; // binding index
  auto&          SetTexture(const Texture* _texture)
  {
    texture = _texture;
    return *this;
  }
  auto& SetBinding(uint32_t _binding)
  {
    binding = _binding;
    return *this;
  }
  auto& SetSampler(const Sampler* _sampler)
  {
    sampler = _sampler;
    return *this;
  }
};

/**
 * @brief Sampler binding
 */
struct SamplerBinding
{
  Sampler* sampler; // sampler to be bound
  uint32_t binding; // binding index
};

/**
 * @brief ClearValue contains an union of RGBA and depthStencil values.
 */
struct ClearValue
{
  union
  {
    struct
    {
      float r, g, b, a;
    } color;
    struct
    {
      float    depth;   // glClearDepthf
      uint32_t stencil; // glClearStencil
    } depthStencil;
  };
};

// Native rendering (using native APIs)

enum class DrawNativeAPI
{
  GLES,
  UNDEFINED
};

/**
 * Specifies native draw commands execution mode
 */
enum class DrawNativeExecutionMode
{
  ISOLATED, ///< Commands execute isolated from the main pipeline (not altering state)
  DIRECT    ///< Commands inherit and alter current state of the main pipeline (unsafe!)
};

struct DrawNativeInfo
{
  DrawNativeAPI       api;      ///< API used by the callback
  Dali::CallbackBase* callback; ///< Callback pointer

  /**
   * The call allows binding the resource so they can be passed into the callback
   * Each resource will pass API specific data (for example GL texture and buffer ids)
   */
  Graphics::Texture** textureList;  ///< Textures to be used by the call
  uint32_t            textureCount; ///< Number of texture used by the callback
  Graphics::Buffer**  bufferList;   ///< Buffers to be used by the call
  uint32_t            bufferCount;  ///< Number of buffers used by the callback

  DrawNativeExecutionMode executionMode; ///< Specifies whether to isolate rendering from main pipeline

  /**
   * The GLES api specific structure that stores pointers to objects to be filled when requested
   * by caller. The structure cointains void* to avoid creating any complex constructors and keep
   * the class trivial.
   */
  struct GLESNativeInfo
  {
    void* eglSharedContextStoragePointer; ///< Indicates the storage object to pass the shared context, must be null if not in use

    /**
     * If false, it will inject GL calls into current context (window)
     * and won't create own context. This will alter GLES state and
     * it's application responsibility to maintain it so DALi can render
     * correctly after.
     */
    bool useOwnEglContext;
  } glesNativeInfo;

  void* userData; ///< Data passed into the callback (unspecified type, callback should decode it)
  void* reserved; ///< Reserved for internal use
};

/**
 * @brief CommandBuffer contains a stream of commands to be executed
 * by the controller.
 */
class CommandBuffer
{
public:
  CommandBuffer()          = default;
  virtual ~CommandBuffer() = default;

  // not copyable
  CommandBuffer(const CommandBuffer&) = delete;
  CommandBuffer& operator=(const CommandBuffer&) = delete;

  /**
   * @brief Begin recording the command buffer.
   *
   * @param[in] info The command buffer usage flags and any other data.
   */
  virtual void Begin(const CommandBufferBeginInfo& info) = 0;

  /**
   * @brief Finish recording the command buffer and put it into execution mode.
   *
   * All render passes should be ended prior to calling this.
   */
  virtual void End() = 0;

  /**
   * @brief Binds vertex buffers
   *
   * The buffers and offsets arrays must be same length
   *
   * @param[in] firstBinding First binding index
   * @param[in] buffers List of buffers to bind
   * @param[in] offsets List of offsets for each buffer
   */
  virtual void BindVertexBuffers(uint32_t                          firstBinding,
                                 const std::vector<const Buffer*>& buffers,
                                 const std::vector<uint32_t>&      offsets) = 0;

  /**
   * @brief Binds uniform buffers
   *
   * @param[in] bindings List of uniform buffer bindings
   */
  virtual void BindUniformBuffers(const std::vector<UniformBufferBinding>& bindings) = 0;

  /**
   * @brief Binds pipeline
   *
   * @param[in] pipeline valid pipeline
   */
  virtual void BindPipeline(const Pipeline& pipeline) = 0;

  /**
   * @brief Binds textures
   *
   * @param[in] textureBindings List of texture bindings
   */
  virtual void BindTextures(const std::vector<TextureBinding>& textureBindings) = 0;

  /**
   * @brief Binds samplers
   *
   * @param[in] samplerBindings List of sampler bindings
   */
  virtual void BindSamplers(const std::vector<SamplerBinding>& samplerBindings) = 0;

  /**
   * @brief Binds buffer containing push constants
   *
   * @param[in] data pointer to the buffer
   * @param[in] size size of data in bytes
   * @param[in] binding push constants binding index
   */
  virtual void BindPushConstants(void*    data,
                                 uint32_t size,
                                 uint32_t binding) = 0;

  /**
   * @brief Binds index buffer
   *
   * Most commonly used formats:
   * R32_UINT,
   * R16_UINT
   *
   * @param[in] buffer Valid buffer
   * @param[in] offset offset within buffer
   * @param[in] format Format of index buffer
   */
  virtual void BindIndexBuffer(const Buffer& buffer,
                               uint32_t      offset,
                               Format        format) = 0;
  /**
   * @brief Begins render pass
   *
   * The function initialises rendering for specified RenderPass object
   * onto renderTarget. renderArea defines the scissor rect. Depends on the
   * renderPass spec, the clearValues may be used.
   *
   * Calling EndRenderPass() is necessary to finalize the render pass.
   *
   * @param[in] renderPass valid render pass object
   * @param[in] renderTarget valid render target, must not be used when framebuffer set
   * @param[in] renderArea area to draw (clear operation is affected)
   * @param[in] clearValues clear values (compatible with renderpass spec)
   */
  virtual void BeginRenderPass(
    RenderPass*                    renderPass,
    RenderTarget*                  renderTarget,
    Rect2D                         renderArea,
    const std::vector<ClearValue>& clearValues) = 0;

  /**
   * @brief Ends current render pass
   *
   * This command must be issued in order to finalize the render pass.
   * It's up to the implementation whether anything has to be done but
   * the Controller may use end RP marker in order to resolve resource
   * dependencies (for example, to know when target texture is ready
   * before passing it to another render pass).
   *
   * The caller may query the sync object to determine when this render
   * pass has actually finished on the GPU.
   *
   * @param[in] syncObject If non-null, this object will ensure an
   * appropriate fence sync object is created after the render pass is
   * executed.
   */
  virtual void EndRenderPass(Graphics::SyncObject* syncObject) = 0;

  /**
   * @brief Executes a list of secondary command buffers
   *
   * The secondary command buffers will be executed as a part of a primary
   * command buffer that calls this function.
   *
   * @param[in] commandBuffers List of buffers to execute
   */
  virtual void ExecuteCommandBuffers(std::vector<const CommandBuffer*>&& commandBuffers) = 0;

  /**
   * @brief Draw primitives
   *
   * @param[in] vertexCount number of vertices
   * @param[in] instanceCount number of instances
   * @param[in] firstVertex index of first vertex
   * @param[in] firstInstance index of first instance
   */
  virtual void Draw(
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance) = 0;

  /**
   * @brief Draws indexed primitives
   *
   * @param[in] indexCount Number of indices
   * @param[in] instanceCount Number of instances
   * @param[in] firstIndex first index
   * @param[in] vertexOffset offset of first vertex
   * @param[in] firstInstance first instance
   */
  virtual void DrawIndexed(
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t  vertexOffset,
    uint32_t firstInstance) = 0;

  /**
   * @brief Draws indexed primitives indirectly
   *
   * Indirect draw uses additional buffer that holds render data.
   *
   * Indirect draw support depends on the hardware (most of modern hardware
   * supports this drawing technique).
   *
   * @param[in] buffer Buffer containing draw parameters
   * @param[in] offset Offset in bytes where parameters begin
   * @param[in] drawCount number of draws to execute
   * @param[in] stride stride between draw parameters
   */
  virtual void DrawIndexedIndirect(
    Buffer&  buffer,
    uint32_t offset,
    uint32_t drawCount,
    uint32_t stride) = 0;

  /**
   * @brief Draws using native API (via callback)
   *
   * DrawNative should be use in order to acquire direct access to the
   * graphics API like GL. Upon command execution, the backend will
   * invoke given callback and pass API-specific arguments (for example,
   * the GL callback will receive EGL context used for rendering).
   *
   * The client side must make sure the callback is valid for the
   * time of execution.
   *
   * @param[in] drawInfo NativeDrawInfo structure
   */
  virtual void DrawNative(const DrawNativeInfo* drawInfo) = 0;

  /**
   * @brief Resets CommandBuffer
   *
   * This function resets the command buffer and discards all previously
   * recorded commands.
   *
   * Since the allocation may use internal memory pool of the CommandBuffer,
   * resetting doesn't have to discard all the resources (for example, it doesn't
   * need to destroy command but only move the pointer to the beginning of
   * the command buffer).
   *
   * It is useful if the command buffer has to be re-recorded frequently, for example,
   * every frame.
   */
  virtual void Reset() = 0;

  /**
   * @brief Changes scissor rect
   *
   * @param[in] value 2D scissor rectangle
   */
  virtual void SetScissor(Rect2D value) = 0;

  /**
   * @brief Enables/disables scissor test
   *
   * @param[in] value State of scissor test
   */
  virtual void SetScissorTestEnable(bool value) = 0;

  /**
   * @brief Sets viewport
   *
   * @param[in] value 2D viewport area
   */
  virtual void SetViewport(Viewport value) = 0;

  /**
   * @brief Sets whether the viewport should be changed
   * @param[in] value state of viewport
   */
  virtual void SetViewportEnable(bool value) = 0;

  /**
   * @brief Sets the color mask for all channels.
   */
  virtual void SetColorMask(bool enabled) = 0;

  /**
   * @brief Clears the stencil buffer (outside of BeginRenderPass) to the current stencil mask
   */
  virtual void ClearStencilBuffer() = 0;

  /**
   * @brief Clears the depth buffer (outside of BeginRenderPass) to the current depth mask
   */
  virtual void ClearDepthBuffer() = 0;

  /**
   * @brief Enable or disable the stencil test
   *
   * @param[in] stencilEnable whether stencil test should be enabled
   */
  virtual void SetStencilTestEnable(bool stencilEnable) = 0;

  /**
   * @brief The mask used for writing to the stencil buffer.
   *
   * It should be as wide as necessary for the stencil texture format.
   * @param[in] writeMask The mask for wriing to / clearing the stencil buffer
   */
  virtual void SetStencilWriteMask(uint32_t writeMask) = 0;

  /**
   * @brief Setup the dynamic stencil state
   *
   * @param[in] compareOp How the stencil buffer, reference and compareMask are combined to determine whether to draw a pixel or not.
   * @param[in] reference A reference value that is ANDed with the mask in the compare op.
   * @param[in] compareMask The bitplanes from the stencil buffer that are active.
   * @param[in] failOp What happens to stencil buffer if drawing a pixel fails the stencil test
   * @param[in] passOp What happens to stencil buffer if drawing a pixel passes stencil & depth test
   * @param[in] depthFailOp What happens to stencil buffer if drawing a pixel passes stencil but fails depth test.
   */
  virtual void SetStencilState(Graphics::CompareOp compareOp,
                               uint32_t            reference,
                               uint32_t            compareMask,
                               Graphics::StencilOp failOp,
                               Graphics::StencilOp passOp,
                               Graphics::StencilOp depthFailOp) = 0;

  /**
   * @brief Defines the comparison operator for passing the depth test.
   *
   * @param[in] compareOp The comparison operator
   */
  virtual void SetDepthCompareOp(Graphics::CompareOp compareOp) = 0;

  /**
   * @brief Enables depth testing
   *
   * @param[in] depthTestEnable True if depth testing will be enabled.
   */
  virtual void SetDepthTestEnable(bool depthTestEnable) = 0;

  /**
   * @brief Enables depth writing / clearing
   *
   * @param[in] depthWriteEnabled True if the depth buffer can be updated or cleared.
   */
  virtual void SetDepthWriteEnable(bool depthWriteEnable) = 0;

protected:
  CommandBuffer(CommandBuffer&&) = default;
  CommandBuffer& operator=(CommandBuffer&&) = default;
};
} // namespace Dali::Graphics

#endif
