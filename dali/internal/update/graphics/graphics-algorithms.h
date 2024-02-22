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

  GraphicsAlgorithms() = delete;
  explicit GraphicsAlgorithms( Graphics::Controller& controller );

  ~GraphicsAlgorithms() = default;

  GraphicsAlgorithms(const GraphicsAlgorithms&) = delete;
  GraphicsAlgorithms& operator=(const GraphicsAlgorithms&) = delete;

  GraphicsAlgorithms(GraphicsAlgorithms&&) = default;
  GraphicsAlgorithms& operator=(GraphicsAlgorithms&&) = default;


  /*
   *@todo: Convert this to work the same way that RenderAlgorithms does in latest DALi.
   *Plan of attack:
   * 1. Change object creation (remove factories)
   * 2. Make command buffers work same as latest DALi
   *    a. Fix up ownership model of command buffers.
   *    b. Change APIs to pass command buffer
   * 3. Update UBOs to work same as new API
   */

  /**
   * Submits render instructions
   * @param graphics Instance of the Graphics object
   * @param renderInstructions container of render instructions
   * @param bufferIndex current buffer index
   */
  void SubmitRenderInstructions( Graphics::Controller &graphics,
                                 SceneGraph::RenderInstructionContainer &renderInstructions,
                                 BufferIndex bufferIndex );

  void DiscardUnusedResources( Graphics::Controller& controller );

private:

  bool SetupScissorClipping( const RenderItem& item );

  bool SetupStencilClipping( const RenderItem& item, uint32_t& lastClippingDepth, uint32_t& lastClippingId );

  void SetupClipping( const RenderItem& item,
                            bool& usedStencilBuffer,
                            uint32_t& lastClippingDepth,
                            uint32_t& lastClippingId );


  bool SetupPipelineViewportState( Graphics::ViewportState& outViewportState );

  void RecordRenderItemList( Graphics::Controller& graphics,
                             BufferIndex bufferIndex,
                             Graphics::RenderTarget& renderTarget, //@todo ???
                             Matrix viewProjection,
                             RenderInstruction& instruction,
                             const RenderList& renderItemList,
                             std::vector<Graphics::CommandBuffer*>& commandList);

  void RecordInstruction( Graphics::Controller& graphics,
                          BufferIndex bufferIndex,
                          RenderInstruction& instruction,
                          std::vector<Graphics::CommandBuffer*>& commandList);

  bool PrepareGraphicsPipeline( Graphics::Controller& controller,
                                RenderInstruction& instruction,
                                const RenderList* renderList,
                                RenderItem& item,
                                bool& usesDepth,
                                bool& usesStencil,
                                BufferIndex bufferIndex );

  void PrepareRendererPipelines( Graphics::Controller& controller,
                                 RenderInstructionContainer& renderInstructions,
                                 bool& usesDepth,
                                 bool& usesStencil,
                                 BufferIndex bufferIndex );

  using ScissorStackType = std::vector<Dali::ClippingBox>;      ///< The container type used to maintain the applied scissor hierarchy

  ScissorStackType                        mScissorStack{};        ///< Contains the currently applied scissor hierarchy (so we can undo clips)

  std::unique_ptr<GraphicsBufferManager> mGraphicsBufferManager;

  using UniformBufferList = std::array<std::unique_ptr<GraphicsBuffer>, 2u>;
  UniformBufferList           mUniformBuffer;

  uint32_t mUniformBlockAllocationCount;
  uint32_t mUniformBlockAllocationBytes;
  uint32_t mUniformBlockMaxSize;
  uint32_t mUboOffset { 0u };
  uint32_t mCurrentFrameIndex { 0u };

  Dali::Graphics::DepthStencilState mCurrentStencilState{};
};
} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_GRAPHICS_ALGORITHMS_H
