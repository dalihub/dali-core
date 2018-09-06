#ifndef DALI_INTERNAL_GRAPHICS_ALGORITHMS_H
#define DALI_INTERNAL_GRAPHICS_ALGORITHMS_H

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

// EXTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-controller.h>

// INTERNAL INCLUDES
#include <dali/internal/update/rendering/render-instruction-container.h>
#include <dali/internal/update/rendering/render-instruction.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/graphics/graphics-buffer-manager.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class UniformBuffer;
class GraphicsAlgorithms
{
public:

  GraphicsAlgorithms() = default;
  ~GraphicsAlgorithms() = default;

  GraphicsAlgorithms(const GraphicsAlgorithms&) = delete;
  GraphicsAlgorithms& operator=(const GraphicsAlgorithms&) = delete;

  GraphicsAlgorithms(GraphicsAlgorithms&&) = default;
  GraphicsAlgorithms& operator=(GraphicsAlgorithms&&) = default;


  /**
   * Submits render instructions
   * @param graphics Instance of the Graphics object
   * @param renderInstructions container of render instructions
   * @param bufferIndex current buffer index
   */
  void SubmitRenderInstructions(Graphics::API::Controller &graphics,
                                SceneGraph::RenderInstructionContainer &renderInstructions,
                                BufferIndex bufferIndex);

private:

  bool SetupScissorClipping( const RenderItem& item );

  bool SetupPipelineViewportState( Graphics::API::ViewportState& outViewportState );

  void SubmitRenderItemList( Graphics::API::Controller&           graphics,
                             BufferIndex                          bufferIndex,
                             Graphics::API::RenderCommand::RenderTargetBinding& renderTargetBinding,
                             Matrix                               viewProjection,
                             RenderInstruction&                   instruction,
                             const RenderList&                    renderItemList );

  void SubmitInstruction( Graphics::API::Controller& graphics,
                          BufferIndex                bufferIndex,
                          RenderInstruction&         instruction );

  bool PrepareGraphicsPipeline( Graphics::API::Controller& controller,
                                RenderInstruction& instruction,
                                const RenderList* renderList,
                                RenderItem& item,
                                BufferIndex bufferIndex );

  void PrepareRendererPipelines( Graphics::API::Controller& controller,
                                 RenderInstructionContainer& renderInstructions,
                                 BufferIndex bufferIndex );

  using ScissorStackType = std::vector<Dali::ClippingBox>;      ///< The container type used to maintain the applied scissor hierarchy

  ScissorStackType                        mScissorStack{};        ///< Contains the currently applied scissor hierarchy (so we can undo clips)

  std::unique_ptr<GraphicsBufferManager> mGraphicsBufferManager;
  std::unique_ptr<GraphicsBuffer>        mUniformBuffer[2u];

  uint32_t mUniformBlockAllocationCount;
  uint32_t mUniformBlockAllocationBytes;
  uint32_t mUniformBlockMaxSize;
  uint32_t mUboOffset { 0u };
  uint32_t mCurrentFrameIndex { 0u };
};
} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_GRAPHICS_ALGORITHMS_H
