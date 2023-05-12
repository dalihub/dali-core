#ifndef DALI_INTERNAL_RENDER_PIPELINE_CACHE_H
#define DALI_INTERNAL_RENDER_PIPELINE_CACHE_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-pipeline.h>
#include <dali/graphics-api/graphics-types.h>
#include <dali/internal/common/blending-options.h>
#include <dali/public-api/common/list-wrapper.h>

namespace Dali::Internal
{
class Program;
namespace Render
{
class Renderer;
class Geometry;

struct PipelineCacheL2;
struct PipelineCacheL1;
struct PipelineCacheL0;
using PipelineCacheL2Container = std::list<PipelineCacheL2>;
using PipelineCacheL1Container = std::list<PipelineCacheL1>;
using PipelineCacheL0Container = std::list<PipelineCacheL0>;
using PipelineCacheL2Ptr       = PipelineCacheL2Container::iterator;
using PipelineCacheL1Ptr       = PipelineCacheL1Container::iterator;
using PipelineCacheL0Ptr       = PipelineCacheL0Container::iterator;

using PipelineCachePtr = PipelineCacheL2Ptr;

/**
 * Cache Level 2 : Last level of cache, stores actual pipeline
 */
struct PipelineCacheL2
{
  uint32_t                                hash{};
  uint32_t                                referenceCount{0u};
  Graphics::ColorBlendState               colorBlendState{};
  Graphics::UniquePtr<Graphics::Pipeline> pipeline{};
};

/**
 * Cache Level 1 : Stores rasterization and input assembly states
 */
struct PipelineCacheL1
{
  PipelineCacheL2Ptr GetPipelineCacheL2(bool blend, bool premul, BlendingOptions& blendingOptions);

  /**
   * @brief Clear unused caches.
   */
  bool ClearUnusedCache();

  uint32_t                     hashCode{}; // 1byte cull, 1byte poly, 1byte frontface
  Graphics::RasterizationState rs{};
  Graphics::InputAssemblyState ia{};

  PipelineCacheL2Container noBlends; // special case
  PipelineCacheL2Container level2nodes;
};

/**
 * Cache Level 0 : Stores hash, geometry, program amd vertex input state
 */
struct PipelineCacheL0 // L0 cache
{
  PipelineCacheL1Ptr GetPipelineCacheL1(Render::Renderer* renderer, bool usingReflection);

  /**
   * @brief Clear unused caches.
   */
  void ClearUnusedCache();

  std::size_t                hash{};
  Geometry*                  geometry{};
  Program*                   program{};
  Graphics::VertexInputState inputState;

  PipelineCacheL1Container level1nodes;
};

struct PipelineCacheQueryInfo
{
  // Program/Geometry
  Renderer* renderer;
  Program*  program;
  Geometry* geometry;

  bool cameraUsingReflection;

  // Blending
  bool             blendingEnabled;
  bool             alphaPremultiplied;
  BlendingOptions* blendingOptions;

  // Lightweight hash value before compare each query.
  std::size_t hash{0u};

  // Generate hash value for this query.
  void GenerateHash();

  // Value comparision between two query info.
  static bool Equal(const PipelineCacheQueryInfo& lhs, const PipelineCacheQueryInfo& rhs) noexcept;
};

/**
 * Result of PipelineCache::GetPipeline() call
 */
struct PipelineResult
{
  Graphics::Pipeline* pipeline;
  PipelineCachePtr    level2;
};

/**
 * Pipeline cache
 */
class PipelineCache
{
public:
  /**
   * Constructor
   * @param[in] controller Graphics controller
   */
  explicit PipelineCache(Graphics::Controller& controller);

  /**
   * Retrieves next cache level
   */
  PipelineCacheL0Ptr GetPipelineCacheL0(std::size_t hash, Program* program, Render::Geometry* geometry);

  /**
   * Retrieves pipeline matching queryInfo struct
   *
   * May retrieve existing pipeline or create one or return nullptr.
   */
  PipelineResult GetPipeline(const PipelineCacheQueryInfo& queryInfo, bool createNewIfNotFound);

  /**
   * @brief Check whether we can reuse latest found PipelineResult.
   * We can reuse latest pipeline only if query info is equal with latest query
   * and we don't call CleanLatestUsedCache() before.
   *
   * @param[in] latestUsedCacheIndex Index of cache we want to compare.
   * @param[in] queryInfo Query for current pipeline.
   * @return True if we can reuse latest pipeline result. False otherwise
   */
  bool ReuseLatestBoundPipeline(const int latestUsedCacheIndex, const PipelineCacheQueryInfo& queryInfo) const;

  /**
   * @brief This is called before rendering every frame.
   */
  void PreRender();

  /**
   * @brief Decrease the reference count of the pipeline cache.
   * @param pipelineCache The pipeline cache to decrease the reference count
   */
  void ResetPipeline(PipelineCachePtr pipelineCache);

private:
  /**
   * @brief Clear latest bound result.
   */
  void CleanLatestUsedCache()
  {
    // Set pipeline as nullptr is enough.
    mLatestResult[0].pipeline = nullptr;
    mLatestResult[1].pipeline = nullptr;
  }

  /**
   * @brief Clear unused caches.
   */
  void ClearUnusedCache();

private:
  Graphics::Controller*    graphicsController{nullptr};
  PipelineCacheL0Container level0nodes;

  // Cache latest queries whether blend enabled or not.
  // (Since most UI case (like Text and Image) enable blend, and most 3D case disable blend.)
  PipelineCacheQueryInfo mLatestQuery[2];  ///< Latest requested query info. It will be invalidate after query's renderer / geometry / blendingOptions value changed.
  PipelineResult         mLatestResult[2]; ///< Latest used result. It will be invalidate when we call CleanLatestUsedCache() or some cache changed.

  uint32_t mFrameCount{0u};
};

} // namespace Render
} // namespace Dali::Internal

#endif // DALI_INTERNAL_RENDER_PIPELINE_CACHE_H
