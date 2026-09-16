/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#ifndef DALI_INTERNAL_SCENE_GRAPH_NODE_RARE_DATA_H
#define DALI_INTERNAL_SCENE_GRAPH_NODE_RARE_DATA_H

// EXTERNAL INCLUDES
#include <vector>

// INTERNAL INCLUDES
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

class RenderTask; // Forward declaration

// Type aliases for containers
using RenderTaskContainer = std::vector<RenderTask*>;

/**
 * @brief RareData holds properties that are rarely used by most Nodes.
 *
 * This structure is heap-allocated only when one of these properties is actually used,
 * saving memory for the common case where these properties remain unused.
 *
 * Properties stored here:
 *
 * - mCacheRenderers: Off-screen rendering cache (usually empty, <1% usage)
 * - mExclusiveRenderTasks: Custom render tasks (usually empty, <0.1% usage)
 * - mClippingDepth: Stencil clipping depth (usually 0, but changes dynamically)
 * - mScissorDepth: Scissor clipping depth (usually 0, but changes dynamically)
 */
struct NodeRareData
{
  /**
   * @brief Default constructor
   */
  NodeRareData()
  : mClippingDepth(0),
    mScissorDepth(0)
  {
  }

  /**
   * @brief Destructor
   */
  ~NodeRareData() = default;

  // Rare containers (usually empty)
  RendererContainer   mCacheRenderers;       // Container of renderers drawing offscreen rendering results
  RenderTaskContainer mExclusiveRenderTasks; // Nodes can be marked as exclusive to multiple RenderTasks

  // Clipping depths (usually 0, but dynamic)
  uint32_t mClippingDepth; ///< The number of stencil clipping nodes deep this node is
  uint32_t mScissorDepth;  ///< The number of scissor clipping nodes deep this node is
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_NODE_RARE_DATA_H
