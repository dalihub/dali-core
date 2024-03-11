#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_CONTAINER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_CONTAINER_H__

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

// INTERNAL INCLUDES
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali::Internal::SceneGraph
{
class RenderInstruction;

/**
 * Class to hold ordered list of current frame's render instructions. Does
 * not own the instructions.
 */
class RenderInstructionContainer
{
public:

  /**
   * Constructor
   */
  RenderInstructionContainer();

  /**
   * Destructor
   */
  ~RenderInstructionContainer();

  /**
   * Reset the container index and reserve space in the container if needed
   * @param bufferIndex to reset
   * @param capacityRequired in the container
   */
  void ResetAndReserve( BufferIndex bufferIndex, uint32_t capacityRequired );

  /**
   * Return the count of instructions in the container
   * @param bufferIndex to use
   * @return the count of elements
   */
  [[nodiscard]] uint32_t Count( BufferIndex bufferIndex ) const;

  /**
   * Get a reference to the instruction at index
   * @param bufferIndex to use
   * @param index to use
   */
  RenderInstruction& At( BufferIndex bufferIndex, uint32_t index );

  /**
   * Get a reference to the instruction at index
   * @param bufferIndex to use
   * @param index to use
   */
  [[nodiscard]] const RenderInstruction& At( BufferIndex bufferIndex, uint32_t index ) const;

  /**
   * Add an instruction to the end of the container
   * @param bufferIndex to use
   */
  void PushBack( BufferIndex index, RenderInstruction* renderInstruction );

  /**
   * Discard an instruction from the end of the container
   * @param bufferIndex to use
   */
  void DiscardCurrentInstruction( BufferIndex updateBufferIndex );

  /**
   * System is shutting down. Don't need to force destroy unowned resources
   */
  void Shutdown();

private:
  std::vector<RenderInstruction*> mInstructions[2u];
};

} // namespace Dali::Internal::SceneGraph





#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_CONTAINER_H__
