#ifndef DALI_INTERNAL_SHARED_UNIFORM_BUFFER_VIEW_CONTAINER_H
#define DALI_INTERNAL_SHARED_UNIFORM_BUFFER_VIEW_CONTAINER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-types.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/public-api/math/compile-time-math.h>

namespace Dali::Internal
{
class Program;

namespace Render
{
class UniformBlock;
class UniformBufferManager;
class UniformBufferView;
} // namespace Render

/**
 * Container of shared uniform blocks and pairwised programs.
 * Before initialize, should register whole programs and uniform blocks pair.
 * At the end of scene rendering, We should call Finalize to ensure UBO memory flushed.
 * @code
 * // Frame begin
 * for each program :
 *   RegisterSharedUniformBlockAndPrograms(program, uniformBlock, blockSize);
 *
 * Initialize(uboManager); ///< Write uniformBlock's uniforms here.
 *
 * for each renderer :
 *   GetSharedUniformBlockBufferView(program, uniformBlock);
 *
 * Finalize();
 * // Frame end
 * @endcode
 */
class SharedUniformBufferViewContainer
{
public:
  /**
   * Constructor
   */
  SharedUniformBufferViewContainer();

public:
  SharedUniformBufferViewContainer(const SharedUniformBufferViewContainer&)            = delete; ///< copy constructor, not defined
  SharedUniformBufferViewContainer& operator=(const SharedUniformBufferViewContainer&) = delete; ///< assignment operator, not defined

  /**
   * Destructor, non virtual as no virtual methods or inheritance
   */
  ~SharedUniformBufferViewContainer();

public:
  /**
   * @brief Register the pair of program and shared uniform blocks.
   * It will create and write uniform informations at Initialize timing.
   * @pre Finialize() should be called before beginning of register.
   * @post Initialize() should be called at the end of register.
   *
   * @param[in] program Program who using shared uniform block
   * @param[in] sharedUniformBlock UniformBlock who has the uniform informations.
   * @param[in] blockSize Size of uniform block.
   * @param[in] blockSizeAligned Aligned size of uniform block.
   * @return True if new program registered into shared uniform blocks. False if we can reuse previous registered program.
   */
  bool RegisterSharedUniformBlockAndPrograms(const Program& program, Render::UniformBlock& sharedUniformBlock, uint32_t blockSize, uint32_t blockSizeAligned);

  /**
   * @brief Total aligned block sizes for registered uniform block and programs.
   * @return Total aligned block size which registered this frame.
   */
  uint32_t GetTotalAlignedBlockSize() const;

  /**
   * @brief Create UniformBufferView for each pair of program and shared uniform blocks.
   * It will also write uniforms now.
   *
   * @param[in] renderBufferIndex Buffer index of current frame
   * @param[in] uniformBufferManager UBO manager to handle the GPU memories.
   */
  void Initialize(BufferIndex renderBufferIndex, Render::UniformBufferManager& uniformBufferManager);

  /**
   * @brief Get UniformBufferView for pair of program and shared uniform blocks.
   *
   * @param[in] program Program who using shared uniform block
   * @param[in] sharedUniformBlock UniformBlock who has the uniform informations.
   * @return Pointer of shared uniform block's UniformBufferView (not owned)
   */
  Render::UniformBufferView* GetSharedUniformBlockBufferView(const Program& program, Render::UniformBlock& sharedUniformBlock) const;

  /**
   * @brief Release all UniformBufferView and registered shared uniform blocks infomations.
   */
  void Finalize();

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};
};
} // namespace Dali::Internal

#endif // DALI_INTERNAL_SHARED_UNIFORM_BUFFER_VIEW_CONTAINER_H
