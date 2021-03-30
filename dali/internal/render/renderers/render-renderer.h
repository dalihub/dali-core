#ifndef DALI_INTERNAL_RENDER_RENDERER_H
#define DALI_INTERNAL_RENDER_RENDERER_H

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

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/rendering/texture-set.h>

#include <dali/graphics-api/graphics-controller.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/const-string.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/uniform-buffer-manager.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/update/manager/render-instruction-processor.h>

namespace Dali
{
namespace Internal
{
class Context;
class Texture;
class ProgramCache;

namespace SceneGraph
{
class SceneController;
class Shader;
class NodeDataProvider;

class RenderInstruction; //for relfection effect
} // namespace SceneGraph

namespace Render
{
struct ShaderCache;
class UniformBufferManager;

/**
 * Renderers are used to render meshes
 * These objects are used during RenderManager::Render(), so properties modified during
 * the Update must either be double-buffered, or set via a message added to the RenderQueue.
 */
class Renderer : public GlResourceOwner
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
   * @copydoc Dali::Internal::GlResourceOwner::GlContextDestroyed()
   */
  void GlContextDestroyed() override;

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlCleanup()
   */
  void GlCleanup() override;

  /**
   * Create a new renderer instance
   * @param[in] dataProviders The data providers for the renderer
   * @param[in] geometry The geometry for the renderer
   * @param[in] blendingBitmask A bitmask of blending options.
   * @param[in] blendColor The blend color to pass to GL
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
   * @param[in] blendColor The blend color to pass to GL
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
  const std::vector<Dali::DevelRenderer::DrawCommand>& GetDrawCommands() const
  {
    return mDrawCommands;
  }

  /**
   * Second-phase construction.
   * This is called when the renderer is inside render thread
   * @param[in] context Context used by the renderer (To be removed)
   * @param[in] graphicsController The graphics controller to use
   * @param[in] programCache Cache of program objects
   * @param[in] shaderCache Cache of shaders
   * @param[in] uniformBufferManager Uniform buffer manager
   */
  void Initialize(Context&                      context,
                  Graphics::Controller&         graphicsController,
                  ProgramCache&                 programCache,
                  Render::ShaderCache&          shaderCache,
                  Render::UniformBufferManager& uniformBufferManager);

  /**
   * Destructor
   */
  ~Renderer() override;

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
   * @param[in] blendColor The blend color to pass to GL
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
  DepthWriteMode::Type GetDepthWriteMode() const;

  /**
   * Sets the depth test mode
   * @param[in] depthTestMode The depth test mode
   */
  void SetDepthTestMode(DepthTestMode::Type depthTestMode);

  /**
   * Query the Renderer's depth test mode
   * @return The renderer depth test mode
   */
  DepthTestMode::Type GetDepthTestMode() const;

  /**
   * Sets the depth function
   * @param[in] depthFunction The depth function
   */
  void SetDepthFunction(DepthFunction::Type depthFunction);

  /**
   * Query the Renderer's depth function
   * @return The renderer depth function
   */
  DepthFunction::Type GetDepthFunction() const;

  /**
   * Sets the render mode
   * @param[in] renderMode The render mode
   */
  void SetRenderMode(RenderMode::Type mode);

  /**
   * Gets the render mode
   * @return The render mode
   */
  RenderMode::Type GetRenderMode() const;

  /**
   * Sets the stencil function
   * @param[in] stencilFunction The stencil function
   */
  void SetStencilFunction(StencilFunction::Type stencilFunction);

  /**
   * Gets the stencil function
   * @return The stencil function
   */
  StencilFunction::Type GetStencilFunction() const;

  /**
   * Sets the stencil function mask
   * @param[in] stencilFunctionMask The stencil function mask
   */
  void SetStencilFunctionMask(int stencilFunctionMask);

  /**
   * Gets the stencil function mask
   * @return The stencil function mask
   */
  int GetStencilFunctionMask() const;

  /**
   * Sets the stencil function reference
   * @param[in] stencilFunctionReference The stencil function reference
   */
  void SetStencilFunctionReference(int stencilFunctionReference);

  /**
   * Gets the stencil function reference
   * @return The stencil function reference
   */
  int GetStencilFunctionReference() const;

  /**
   * Sets the stencil mask
   * @param[in] stencilMask The stencil mask
   */
  void SetStencilMask(int stencilMask);

  /**
   * Gets the stencil mask
   * @return The stencil mask
   */
  int GetStencilMask() const;

  /**
   * Sets the stencil operation for when the stencil test fails
   * @param[in] stencilOperationOnFail The stencil operation
   */
  void SetStencilOperationOnFail(StencilOperation::Type stencilOperationOnFail);

  /**
   * Gets the stencil operation for when the stencil test fails
   * @return The stencil operation
   */
  StencilOperation::Type GetStencilOperationOnFail() const;

  /**
   * Sets the stencil operation for when the depth test fails
   * @param[in] stencilOperationOnZFail The stencil operation
   */
  void SetStencilOperationOnZFail(StencilOperation::Type stencilOperationOnZFail);

  /**
   * Gets the stencil operation for when the depth test fails
   * @return The stencil operation
   */
  StencilOperation::Type GetStencilOperationOnZFail() const;

  /**
   * Sets the stencil operation for when the depth test passes
   * @param[in] stencilOperationOnZPass The stencil operation
   */
  void SetStencilOperationOnZPass(StencilOperation::Type stencilOperationOnZPass);

  /**
   * Gets the stencil operation for when the depth test passes
   * @return The stencil operation
   */
  StencilOperation::Type GetStencilOperationOnZPass() const;

  /**
   * Called to upload during RenderManager::Render().
   */
  void Upload();

  /**
   * Called to render during RenderManager::Render().
   * @param[in] context The context used for rendering
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] node The node using this renderer
   * @param[in] modelViewMatrix The model-view matrix.
   * @param[in] viewMatrix The view matrix.
   * @param[in] projectionMatrix The projection matrix.
   * @param[in] size Size of the render item
   * @param[in] blend If true, blending is enabled
   * @param[in] boundTextures The textures bound for rendering
   * @param[in] instruction. for use case like reflection where CullFace needs to be adjusted

   */
  void Render(Context&                                             context,
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
   * Write the renderer's sort attributes to the passed in reference
   *
   * @param[in] bufferIndex The current update buffer index.
   * @param[out] sortAttributes
   */
  void SetSortAttributes(BufferIndex bufferIndex, SceneGraph::RenderInstructionProcessor::SortAttributes& sortAttributes) const;

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
  bool WriteDefaultUniform(const Graphics::UniformInfo*                       uniformInfo,
                           Render::UniformBuffer&                             ubo,
                           const std::vector<Graphics::UniformBufferBinding>& bindings,
                           const T&                                           data);

  template<class T>
  void WriteUniform(Render::UniformBuffer&                             ubo,
                    const std::vector<Graphics::UniformBufferBinding>& bindings,
                    const Graphics::UniformInfo&                       uniformInfo,
                    const T&                                           data);

  void WriteUniform(Render::UniformBuffer&                             ubo,
                    const std::vector<Graphics::UniformBufferBinding>& bindings,
                    const Graphics::UniformInfo&                       uniformInfo,
                    const void*                                        data,
                    uint32_t                                           size);

private:
  struct UniformIndexMap;

  // Undefined
  Renderer(const Renderer&);

  // Undefined
  Renderer& operator=(const Renderer& rhs);

  /**
   * Sets blending options
   * @param context to use
   * @param blend Wheter blending should be enabled or not
   */
  void SetBlending(Context& context, bool blend);

  /**
   * Builds a uniform map based on the index of the cached location in the Program.
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] node The node using the renderer
   * @param[in] size The size of the renderer
   * @param[in] program The shader program on which to set the uniforms.
   */
  void BuildUniformIndexMap(BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, const Vector3& size, Program& program);

  /**
   * Set the program uniform in the map from the mapped property
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] program The shader program
   * @param[in] map The uniform
   */
  void SetUniformFromProperty(BufferIndex bufferIndex, Program& program, UniformIndexMap& map);

  /**
   * Bind the textures and setup the samplers
   * @param[in] context The GL context
   * @param[in] program The shader program
   * @param[in] boundTextures The textures bound for rendering
   */
  void BindTextures(Program& program, Graphics::CommandBuffer& commandBuffer, Vector<Graphics::Texture*>& boundTextures);

  /**
   * Prepare a pipeline for this renderer
   */
  Graphics::UniquePtr<Graphics::Pipeline> PrepareGraphicsPipeline(
    Program&                                             program,
    const Dali::Internal::SceneGraph::RenderInstruction& instruction,
    bool                                                 blend,
    Graphics::UniquePtr<Graphics::Pipeline>&&            oldPipeline);

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
                         Render::UniformBuffer&                        ubo,
                         std::vector<Graphics::UniformBufferBinding>*& outBindings,
                         uint32_t&                                     offset,
                         BufferIndex                                   updateBufferIndex);

private:
  Graphics::Controller*                        mGraphicsController;
  OwnerPointer<SceneGraph::RenderDataProvider> mRenderDataProvider;

  Context*          mContext;
  Render::Geometry* mGeometry;

  Graphics::UniquePtr<Graphics::CommandBuffer> mGraphicsCommandBuffer{};

  ProgramCache*        mProgramCache{nullptr};
  Render::ShaderCache* mShaderCache{nullptr};

  Render::UniformBufferManager*               mUniformBufferManager{};
  std::vector<Graphics::UniformBufferBinding> mUniformBufferBindings{};

  Graphics::UniquePtr<Graphics::Pipeline> mGraphicsPipeline{}; ///< The graphics pipeline. (Cached implementation)
  std::vector<Graphics::ShaderState>      mShaderStates{};

  using Hash = unsigned long;
  struct UniformIndexMap
  {
    uint32_t                 uniformIndex;  ///< The index of the cached location in the Program
    ConstString              uniformName;   ///< The uniform name
    const PropertyInputImpl* propertyValue; ///< The property value
    Hash                     uniformNameHash{0u};
    Hash                     uniformNameHashNoArray{0u};
    int32_t                  arrayIndex; ///< The array index
  };

  using UniformIndexMappings = Dali::Vector<UniformIndexMap>;

  UniformIndexMappings mUniformIndexMap;
  Vector<int32_t>      mAttributeLocations;

  uint64_t mUniformsHash;

  StencilParameters mStencilParameters; ///< Struct containing all stencil related options
  BlendingOptions   mBlendingOptions;   ///< Blending options including blend color, blend func and blend equation

  uint32_t mIndexedDrawFirstElement;  ///< Offset of first element to draw
  uint32_t mIndexedDrawElementsCount; ///< Number of elements to draw

  DepthFunction::Type   mDepthFunction : 4;            ///< The depth function
  FaceCullingMode::Type mFaceCullingMode : 3;          ///< The mode of face culling
  DepthWriteMode::Type  mDepthWriteMode : 3;           ///< The depth write mode
  DepthTestMode::Type   mDepthTestMode : 3;            ///< The depth test mode
  bool                  mUpdateAttributeLocations : 1; ///< Indicates attribute locations have changed
  bool                  mPremultipledAlphaEnabled : 1; ///< Flag indicating whether the Pre-multiplied Alpha Blending is required
  bool                  mShaderChanged : 1;            ///< Flag indicating the shader changed and uniform maps have to be updated
  bool                  mUpdated : 1;

  std::vector<Dali::DevelRenderer::DrawCommand> mDrawCommands; // Devel stuff

  struct LegacyProgram : Graphics::ExtensionCreateInfo
  {
    uint32_t programId{0};
  };

  LegacyProgram mLegacyProgram; ///< The structure to pass the program ID into Graphics::PipelineCreateInfo

  Graphics::UniquePtr<Render::UniformBuffer> mUniformBuffer[2]{nullptr, nullptr}; ///< The double-buffered uniform buffer
};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_RENDER_RENDERER_H
