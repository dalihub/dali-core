#ifndef DALI_INTERNAL_GRAPHICS_ALGORITHMS_H
#define DALI_INTERNAL_GRAPHICS_ALGORITHMS_H

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

// EXTERNAL INCLUDES
#include <dali/graphics-api/graphics-controller.h>

// INTERNAL INCLUDES
#include <dali/integration-api/core-enumerations.h>

#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/graphics/uniform-buffer-manager.h>
#include <dali/internal/update/rendering/render-instruction-container.h>
#include <dali/internal/update/rendering/render-instruction.h>

namespace Dali::Internal::SceneGraph
{
class UniformBuffer;

class GraphicsAlgorithms
{
public:
  GraphicsAlgorithms() = delete;
  explicit GraphicsAlgorithms( Graphics::Controller& controller );

  ~GraphicsAlgorithms() = default;

  GraphicsAlgorithms(const GraphicsAlgorithms&) = delete;
  GraphicsAlgorithms& operator=(const GraphicsAlgorithms&) = delete;

  GraphicsAlgorithms(GraphicsAlgorithms&&) = delete;
  GraphicsAlgorithms& operator=(GraphicsAlgorithms&&) = delete;

  /**
   * Clear down and re-create primary command buffer.
   */
  void ResetCommandBuffer();

  /**
   * Submits render instructions
   * @param renderInstructions container of render instructions
   * @param bufferIndex current buffer index
   */
  void RenderScene( const SceneGraph::Scene* scene,
                    BufferIndex bufferIndex,
                    bool renderToFbo);

    UniformBufferManager& GetUniformBufferManager();

private:

  void SetupScissorClipping( const RenderItem& item,
                             Graphics::CommandBuffer& commandBuffer,
                             const RenderInstruction& instruction,
                             int orientation);

  void SetupStencilClipping( const RenderItem& item, Graphics::CommandBuffer& commandBuffer,
                             uint32_t& lastClippingDepth, uint32_t& lastClippingId,
                             const RenderInstruction& instruction,
                             int orientation);

  void SetupClipping( const RenderItem& item,
                      Graphics::CommandBuffer& commandBuffer,
                      bool& usedStencilBuffer,
                      uint32_t& lastClippingDepth,
                      uint32_t& lastClippingId,
                      bool stencilBufferAvailable,
                      const RenderInstruction& instruction,
                      int orientation);


  bool SetupPipelineViewportState( Graphics::ViewportState& outViewportState );

  void RecordRenderItemList( const RenderList& renderList,
                             BufferIndex bufferIndex,
                             const Matrix& viewMatrix,
                             const Matrix& projectionMatrix,
                             const RenderInstruction& instruction,
                             const Rect<int32_t>& viewport,
                             const Rect<int>& rootClippingRect,
                             Uint16Pair sceneSize,
                             bool depthBufferAvailable,
                             bool stencilBufferAvailable);

  void RecordInstruction( const RenderInstruction& instruction,
                          bool renderToFbo,
                          bool depthBufferAvailable,
                          bool stencilBufferAvailable,
                          const Rect<int32_t>& viewport,
                          const Rect<int>& rootClippingRect,
                          Uint16Pair sceneSize,
                          BufferIndex bufferIndex);

  bool PrepareGraphicsPipeline(const RenderInstruction& instruction,
                               const RenderList* renderList,
                               RenderItem& item,
                               BufferIndex bufferIndex );

  void PrepareRendererPipelines(const RenderInstructionContainer& renderInstructions,
                                BufferIndex bufferIndex );

private:
  using ScissorStackType = std::vector<Dali::ClippingBox>;      ///< The container type used to maintain the applied scissor hierarchy

  Graphics::Controller& mGraphicsController;

  Graphics::UniquePtr<Graphics::CommandBuffer> mGraphicsCommandBuffer{};
  std::unique_ptr<UniformBufferManager> mUniformBufferManager;
  ScissorStackType mScissorStack{};        ///< Contains the currently applied scissor hierarchy (so we can undo clips)
  Dali::ClippingBox mViewportRectangle;   ///< The viewport dimensions, used to translate AABBs to scissor coordinates
  bool              mHasLayerScissor : 1; ///< Marks if the currently process render instruction has a layer-based clipping region

  uint32_t mUniformBlockAllocationCount{};
  uint32_t mUniformBlockAllocationBytes{};
  uint32_t mUniformBlockMaxSize{};
  uint32_t mUboOffset { 0u };

  uint32_t mCurrentFrameIndex { 0u };

  Dali::Graphics::DepthStencilState mCurrentStencilState{};
};

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_GRAPHICS_ALGORITHMS_H
