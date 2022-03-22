#ifndef DALI_INTERNAL_RENDER_RENDERER_H
#define DALI_INTERNAL_RENDER_RENDERER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/signals/render-callback.h>

#include <dali/graphics-api/graphics-controller.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/const-string.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/uniform-buffer-manager.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/update/manager/render-instruction-processor.h>

namespace Dali
{
namespace Internal
{
class Texture;
class ProgramCache;

namespace SceneGraph
{
class SceneController;
class Shader;
class NodeDataProvider;
class RenderInstruction; //for reflection effect
} // namespace SceneGraph

namespace Render
{
struct ShaderCache;
class PipelineCache;
class UniformBufferManager;
class PipelineCache;

/**
 * Renderers are used to render meshes
 * These objects are used during RenderManager::Render(), so properties modified during
 * the Update must either be double-buffered, or set via a message added to the RenderQueue.
 */
class Renderer
{
public:
  /**
   * @brief Struct to encapsulate stencil parameters required for control of the stencil buffer.
   */
  struct StencilParameters
  {
    StencilParameters(RenderMode::Type renderMode, StencilFunction::Type stencilFunction, int stencilFunctionMask, int stencilFunctionReference, int stencilMask, StencilOperation::Type stencilOperationOnFail, StencilOperation::Type stencilOperationOnZFail, StencilOperation::Type stencilOperationOnZPass)
    : stencilFunctionMask(stencilFunctionMask),
      stencilFunctionReference(stencilFunctionReference),
      stencilMask(stencilMask),
      renderMode(renderMode),
      stencilFunction(stencilFunction),
      stencilOperationOnFail(stencilOperationOnFail),
      stencilOperationOnZFail(stencilOperationOnZFail),
      stencilOperationOnZPass(stencilOperationOnZPass)
    {
    }

    int                    stencilFunctionMask;         ///< The stencil function mask
    int                    stencilFunctionReference;    ///< The stencil function reference
    int                    stencilMask;                 ///< The stencil mask
    RenderMode::Type       renderMode : 4;              ///< The render mode
    StencilFunction::Type  stencilFunction : 4;         ///< The stencil function
    StencilOperation::Type stencilOperationOnFail : 4;  ///< The stencil operation for stencil test fail
    StencilOperation::Type stencilOperationOnZFail : 4; ///< The stencil operation for depth test fail
    StencilOperation::Type stencilOperationOnZPass : 4; ///< The stencil operation for depth test pass
  };

  /**
   * Create a new renderer instance
   * @param[in] dataProviders The data providers for the renderer
   * @param[in] geometry The geometry for the renderer
   * @param[in] blendingBitmask A bitmask of blending options.
   * @param[in] blendColor The blend color
   * @param[in] faceCullingMode The face-culling mode.
   * @param[in] preMultipliedAlphaEnabled whether alpha is pre-multiplied.
   * @param[in] depthWriteMode Depth buffer write mode
   * @param[in] depthTestMode Depth buffer test mode
   * @param[in] depthFunction Depth function
   * @param[in] stencilParameters Struct containing all stencil related options
   */
  static Renderer* New(SceneGraph::RenderDataProvider* dataProviders,
                       Render::Geometry*               geometry,
                       uint32_t                        blendingBitmask,
                       const Vector4&                  blendColor,
                       FaceCullingMode::Type           faceCullingMode,
                       bool                            preMultipliedAlphaEnabled,
                       DepthWriteMode::Type            depthWriteMode,
                       DepthTestMode::Type             depthTestMode,
                       DepthFunction::Type             depthFunction,
                       StencilParameters&              stencilParameters);

  /**
   * Constructor.
   * @param[in] dataProviders The data providers for the renderer
   * @param[in] geometry The geometry for the renderer
   * @param[in] blendingBitmask A bitmask of blending options.
   * @param[in] blendColor The blend color
   * @param[in] faceCullingMode The face-culling mode.
   * @param[in] preMultipliedAlphaEnabled whether alpha is pre-multiplied.
   * @param[in] depthWriteMode Depth buffer write mode
   * @param[in] depthTestMode Depth buffer test mode
   * @param[in] depthFunction Depth function
   * @param[in] stencilParameters Struct containing all stencil related options
   */
  Renderer(SceneGraph::RenderDataProvider* dataProviders,
           Render::Geometry*               geometry,
           uint32_t                        blendingBitmask,
           const Vector4&                  blendColor,
           FaceCullingMode::Type           faceCullingMode,
           bool                            preMultipliedAlphaEnabled,
           DepthWriteMode::Type            depthWriteMode,
           DepthTestMode::Type             depthTestMode,
           DepthFunction::Type             depthFunction,
           StencilParameters&              stencilParameters);

  /**
   * Change the geometry used by the renderer
   * @param[in] geometry The new geometry
   */
  void SetGeometry(Render::Geometry* geometry);

  void SetDrawCommands(Dali::DevelRenderer::DrawCommand* pDrawCommands, uint32_t size);

  /**
   * @brief Returns a reference to an array of draw commands
   * @return Valid array of draw commands (may be empty)
   */
  [[nodiscard]] const std::vector<Dali::DevelRenderer::DrawCommand>& GetDrawCommands() const
  {
    return mDrawCommands;
  }

  /**
   * Second-phase construction.
   * This is called when the renderer is inside render thread
   * @param[in] graphicsController The graphics controller to use
   * @param[in] programCache Cache of program objects
   * @param[in] shaderCache Cache of shaders
   * @param[in] uniformBufferManager Uniform buffer manager
   * @param[in] pipelineCache Cache of pipelines
   */
  void Initialize(Graphics::Controller&         graphicsController,
                  ProgramCache&                 programCache,
                  Render::ShaderCache&          shaderCache,
                  Render::UniformBufferManager& uniformBufferManager,
                  Render::PipelineCache&        pipelineCache);

  /**
   * Destructor
   */
  ~Renderer();

  /**
   * Set the face-culling mode.
   * @param[in] mode The face-culling mode.
   */
  void SetFaceCullingMode(FaceCullingMode::Type mode);

  /**
   * Set the bitmask for blending options
   * @param[in] bitmask A bitmask of blending options.
   */
  void SetBlendingBitMask(uint32_t bitmask);

  /**
   * Set the blend color for blending options
   * @param[in] blendColor The blend color
   */
  void SetBlendColor(const Vector4& color);

  /**
   * Set the first element index to draw by the indexed draw
   * @param[in] firstElement index of first element to draw
   */
  void SetIndexedDrawFirstElement(uint32_t firstElement);

  /**
   * Set the number of elements to draw by the indexed draw
   * @param[in] elementsCount number of elements to draw
   */
  void SetIndexedDrawElementsCount(uint32_t elementsCount);

  /**
   * @brief Set whether the Pre-multiplied Alpha Blending is required
   *
   * @param[in] preMultipled whether alpha is pre-multiplied.
   */
  void EnablePreMultipliedAlpha(bool preMultipled);

  /**
   * Sets the depth write mode
   * @param[in] depthWriteMode The depth write mode
   */
  void SetDepthWriteMode(DepthWriteMode::Type depthWriteMode);

  /**
   * Query the Renderer's depth write mode
   * @return The renderer depth write mode
   */
  [[nodiscard]] DepthWriteMode::Type GetDepthWriteMode() const;

  /**
   * Sets the depth test mode
   * @param[in] depthTestMode The depth test mode
   */
  void SetDepthTestMode(DepthTestMode::Type depthTestMode);

  /**
   * Query the Renderer's depth test mode
   * @return The renderer depth test mode
   */
  [[nodiscard]] DepthTestMode::Type GetDepthTestMode() const;

  /**
   * Sets the depth function
   * @param[in] depthFunction The depth function
   */
  void SetDepthFunction(DepthFunction::Type depthFunction);

  /**
   * Query the Renderer's depth function
   * @return The renderer depth function
   */
  [[nodiscard]] DepthFunction::Type GetDepthFunction() const;

  /**
   * Sets the render mode
   * @param[in] renderMode The render mode
   */
  void SetRenderMode(RenderMode::Type mode);

  /**
   * Gets the render mode
   * @return The render mode
   */
  [[nodiscard]] RenderMode::Type GetRenderMode() const;

  /**
   * Sets the stencil function
   * @param[in] stencilFunction The stencil function
   */
  void SetStencilFunction(StencilFunction::Type stencilFunction);

  /**
   * Gets the stencil function
   * @return The stencil function
   */
  [[nodiscard]] StencilFunction::Type GetStencilFunction() const;

  /**
   * Sets the stencil function mask
   * @param[in] stencilFunctionMask The stencil function mask
   */
  void SetStencilFunctionMask(int stencilFunctionMask);

  /**
   * Gets the stencil function mask
   * @return The stencil function mask
   */
  [[nodiscard]] int GetStencilFunctionMask() const;

  /**
   * Sets the stencil function reference
   * @param[in] stencilFunctionReference The stencil function reference
   */
  void SetStencilFunctionReference(int stencilFunctionReference);

  /**
   * Gets the stencil function reference
   * @return The stencil function reference
   */
  [[nodiscard]] int GetStencilFunctionReference() const;

  /**
   * Sets the stencil mask
   * @param[in] stencilMask The stencil mask
   */
  void SetStencilMask(int stencilMask);

  /**
   * Gets the stencil mask
   * @return The stencil mask
   */
  [[nodiscard]] int GetStencilMask() const;

  /**
   * Sets the stencil operation for when the stencil test fails
   * @param[in] stencilOperationOnFail The stencil operation
   */
  void SetStencilOperationOnFail(StencilOperation::Type stencilOperationOnFail);

  /**
   * Gets the stencil operation for when the stencil test fails
   * @return The stencil operation
   */
  [[nodiscard]] StencilOperation::Type GetStencilOperationOnFail() const;

  /**
   * Sets the stencil operation for when the depth test fails
   * @param[in] stencilOperationOnZFail The stencil operation
   */
  void SetStencilOperationOnZFail(StencilOperation::Type stencilOperationOnZFail);

  /**
   * Gets the stencil operation for when the depth test fails
   * @return The stencil operation
   */
  [[nodiscard]] StencilOperation::Type GetStencilOperationOnZFail() const;

  /**
   * Sets the stencil operation for when the depth test passes
   * @param[in] stencilOperationOnZPass The stencil operation
   */
  void SetStencilOperationOnZPass(StencilOperation::Type stencilOperationOnZPass);

  /**
   * Gets the stencil operation for when the depth test passes
   * @return The stencil operation
   */
  [[nodiscard]] StencilOperation::Type GetStencilOperationOnZPass() const;

  /**
   * Called to upload during RenderManager::Render().
   */
  void Upload();

  /**
   * Called to render during RenderManager::Render().
   * @param[in,out] commandBuffer The command buffer to write into
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] node The node using this renderer
   * @param[in] modelViewMatrix The model-view matrix.
   * @param[in] viewMatrix The view matrix.
   * @param[in] projectionMatrix The projection matrix.
   * @param[in] size Size of the render item
   * @param[in] blend If true, blending is enabled
   * @param[in] boundTextures The textures bound for rendering
   * @param[in] instruction. for use case like reflection where CullFace needs to be adjusted
   *
   * @return True if commands have been added to the command buffer
   */
  bool Render(Graphics::CommandBuffer&                             commandBuffer,
              BufferIndex                                          bufferIndex,
              const SceneGraph::NodeDataProvider&                  node,
              const Matrix&                                        modelMatrix,
              const Matrix&                                        modelViewMatrix,
              const Matrix&                                        viewMatrix,
              const Matrix&                                        projectionMatrix,
              const Vector3&                                       size,
              bool                                                 blend,
              Vector<Graphics::Texture*>&                          boundTextures,
              const Dali::Internal::SceneGraph::RenderInstruction& instruction,
              uint32_t                                             queueIndex);

  /**
   * Sets RenderCallback object
   *
   * @param[in] callback Valid pointer to RenderCallback object
   */
  void SetRenderCallback(RenderCallback* callback);

  /**
   * Returns currently set RenderCallback object
   *
   * @return Valid pointer to RenderCallback object or nullptr
   */
  RenderCallback* GetRenderCallback()
  {
    return mRenderCallback;
  }

  /**
   * Write the renderer's sort attributes to the passed in reference
   *
   * @param[out] sortAttributes
   */
  void SetSortAttributes(SceneGraph::RenderInstructionProcessor::SortAttributes& sortAttributes) const;

  /**
   * Sets the flag indicating whether shader changed.
   *
   * @param[in] value True if shader changed
   */
  void SetShaderChanged(bool value);

  /**
   * Check if the renderer attributes/uniforms are updated and returns the flag
   *
   * @param[in] bufferIndex The current update buffer index.
   * @param[in] node The node using this renderer
   */
  bool Updated(BufferIndex bufferIndex, const SceneGraph::NodeDataProvider* node);

  template<class T>
  bool WriteDefaultUniform(const Graphics::UniformInfo* uniformInfo,
                           Render::UniformBufferView&   ubo,
                           const T&                     data);

  template<class T>
  void WriteUniform(Render::UniformBufferView&   ubo,
                    const Graphics::UniformInfo& uniformInfo,
                    const T&                     data);

  void WriteUniform(Render::UniformBufferView&   ubo,
                    const Graphics::UniformInfo& uniformInfo,
                    const void*                  data,
                    uint32_t                     size);

  [[nodiscard]] FaceCullingMode::Type GetFaceCullMode() const
  {
    return mFaceCullingMode;
  }

private:
  struct UniformIndexMap;

  // Undefined
  Renderer(const Renderer&);

  // Undefined
  Renderer& operator=(const Renderer& rhs);

  /**
   * Builds a uniform map based on the index of the cached location in the Program.
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] node The node using the renderer
   * @param[in] size The size of the renderer
   * @param[in] program The shader program on which to set the uniforms.
   */
  void BuildUniformIndexMap(BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, const Vector3& size, Program& program);

  /**
   * Bind the textures and setup the samplers
   * @param[in] commandBuffer The command buffer to record binding into
   * @param[in] boundTextures The textures bound for rendering
   */
  void BindTextures(Graphics::CommandBuffer& commandBuffer, Vector<Graphics::Texture*>& boundTextures);

  /**
   * Prepare a pipeline for this renderer.
   *
   * As a renderer can be re-used in a single frame (e.g. being used by multiple nodes, or
   * by non-exclusive render tasks), we store a pipeline per node/instruction.
   * In practice, the implementation will cached pipelines, so we normally only have
   * multiple handles.
   */
  Graphics::Pipeline& PrepareGraphicsPipeline(
    Program&                                             program,
    const Dali::Internal::SceneGraph::RenderInstruction& instruction,
    const SceneGraph::NodeDataProvider&                  node,
    bool                                                 blend);

  /**
   * Setup and write data to the uniform buffer
   *
   * @param[in] bufferIndex The current buffer index
   * @param[in] commandBuffer The command buffer to bind the uniform buffer to
   * @param[in] node The node using this renderer
   * @param[in] modelViewMatrix The model-view matrix.
   * @param[in] viewMatrix The view matrix.
   * @param[in] projectionMatrix The projection matrix.
   * @param[in] size Size of the render item
   * @param[in] blend If true, blending is enabled
   * @param[in] instruction The render instruction
   */
  void WriteUniformBuffer(BufferIndex                          bufferIndex,
                          Graphics::CommandBuffer&             commandBuffer,
                          Program*                             program,
                          const SceneGraph::RenderInstruction& instruction,
                          const SceneGraph::NodeDataProvider&  node,
                          const Matrix&                        modelMatrix,
                          const Matrix&                        modelViewMatrix,
                          const Matrix&                        viewMatrix,
                          const Matrix&                        projectionMatrix,
                          const Vector3&                       size);

  /**
   * @brief Fill uniform buffer at index. Writes uniforms into given memory address
   *
   * @param[in] instruction The render instruction
   * @param[in,out] ubo Target uniform buffer object
   * @param[out] outBindings output bindings vector
   * @param[out] offset output offset of the next uniform buffer memory address
   * @param[in] updateBufferIndex update buffer index
   */
  void FillUniformBuffer(Program&                                      program,
                         const SceneGraph::RenderInstruction&          instruction,
                         Render::UniformBufferView&                    ubo,
                         std::vector<Graphics::UniformBufferBinding>*& outBindings,
                         uint32_t&                                     offset,
                         BufferIndex                                   updateBufferIndex);

private:
  Graphics::Controller*           mGraphicsController;
  SceneGraph::RenderDataProvider* mRenderDataProvider;

  Render::Geometry* mGeometry;

  ProgramCache*        mProgramCache{nullptr};
  Render::ShaderCache* mShaderCache{nullptr};

  Render::UniformBufferManager*               mUniformBufferManager{};
  std::vector<Graphics::UniformBufferBinding> mUniformBufferBindings{};

  Render::PipelineCache* mPipelineCache{nullptr};

  using Hash = unsigned long;

  typedef const float& (PropertyInputImpl::*FuncGetter)(BufferIndex) const;

  struct UniformIndexMap
  {
    ConstString              uniformName;            ///< The uniform name
    const PropertyInputImpl* propertyValue{nullptr}; ///< The property value
    Hash                     uniformNameHash{0u};
    Hash                     uniformNameHashNoArray{0u};
    int32_t                  arrayIndex{-1}; ///< The array index

    int16_t    uniformLocation{0u};
    uint16_t   uniformOffset{0u};
    uint16_t   uniformSize{0u};
    FuncGetter uniformFunc{0};
  };

  using UniformIndexMappings = Dali::Vector<UniformIndexMap>;

  UniformIndexMappings mUniformIndexMap;
  uint64_t             mUniformsHash;

  struct HashedPipeline
  {
    uint64_t                                mHash{0u};
    Graphics::UniquePtr<Graphics::Pipeline> mGraphicsPipeline{nullptr};
    inline static uint64_t                  GetHash(const void* node, const void* instruction, bool blend)
    {
      return (reinterpret_cast<uint64_t>(node) << 32) | ((reinterpret_cast<uint64_t>(instruction) & 0xFFFFFFF) << 1) | blend;
    }
  };

  StencilParameters mStencilParameters; ///< Struct containing all stencil related options
  BlendingOptions   mBlendingOptions;   ///< Blending options including blend color, blend func and blend equation

  uint32_t mIndexedDrawFirstElement;  ///< Offset of first element to draw
  uint32_t mIndexedDrawElementsCount; ///< Number of elements to draw

  DepthFunction::Type   mDepthFunction : 4;             ///< The depth function
  FaceCullingMode::Type mFaceCullingMode : 3;           ///< The mode of face culling
  DepthWriteMode::Type  mDepthWriteMode : 3;            ///< The depth write mode
  DepthTestMode::Type   mDepthTestMode : 3;             ///< The depth test mode
  bool                  mPremultipliedAlphaEnabled : 1; ///< Flag indicating whether the Pre-multiplied Alpha Blending is required
  bool                  mShaderChanged : 1;             ///< Flag indicating the shader changed and uniform maps have to be updated
  bool                  mUpdated : 1;

  std::vector<Dali::DevelRenderer::DrawCommand> mDrawCommands; // Devel stuff
  RenderCallback*                               mRenderCallback{nullptr};
  RenderCallbackInput                           mRenderCallbackInput{};
};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_RENDER_RENDERER_H
