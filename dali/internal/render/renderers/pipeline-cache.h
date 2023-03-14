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

// EXTERNAL INCLUDES
#include <vector>

namespace Dali::Internal
{
class Program;
namespace Render
{
class Renderer;
class Geometry;

/**
 * Cache Level 2 : Last level of cache, stores actual pipeline
 */
struct PipelineCacheL2
{
  uint32_t                                hash{};
  Graphics::ColorBlendState               colorBlendState;
  Graphics::UniquePtr<Graphics::Pipeline> pipeline;
};

/**
 * Cache Level 1 : Stores rasterization and input assembly states
 */
struct PipelineCacheL1
{
  PipelineCacheL2* GetPipelineCacheL2(bool blend, bool premul, BlendingOptions& blendingOptions);

  uint32_t                     hashCode{}; // 1byte cull, 1byte poly, 1byte frontface
  Graphics::RasterizationState rs{};
  Graphics::InputAssemblyState ia{};

  PipelineCacheL2              noBlend; // special case
  std::vector<PipelineCacheL2> level2nodes;
};

/**
 * Cache Level 0 : Stores hash, geometry, program amd vertex input state
 */
struct PipelineCacheL0 // L0 cache
{
  PipelineCacheL1* GetPipelineCacheL1(Render::Renderer* renderer, bool usingReflection);

  std::size_t                hash{};
  Geometry*                  geometry{};
  Program*                   program{};
  Graphics::VertexInputState inputState;

  std::vector<PipelineCacheL1> level1nodes;
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

  PipelineCacheL0* level0;
  PipelineCacheL1* level1;
  PipelineCacheL2* level2;
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
  PipelineCacheL0* GetPipelineCacheL0(std::size_t hash, Program* program, Render::Geometry* geometry);

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
   * @brief Clear latest bound result.
   */
  void CleanLatestUsedCache()
  {
    // Set pipeline as nullptr is enough.
    mLatestResult[0].pipeline = nullptr;
    mLatestResult[1].pipeline = nullptr;
  }

private:
  Graphics::Controller*        graphicsController{nullptr};
  std::vector<PipelineCacheL0> level0nodes;

  // Cache latest queries whether blend enabled or not.
  // (Since most UI case (like Text and Image) enable blend, and most 3D case disable blend.)
  PipelineCacheQueryInfo mLatestQuery[2];  ///< Latest requested query info. It will be invalidate after query's renderer / geometry / blendingOptions value changed.
  PipelineResult         mLatestResult[2]; ///< Latest used result. It will be invalidate when we call CleanLatestUsedCache() or some cache changed.
};

} // namespace Render
} // namespace Dali::Internal

#endif // DALI_INTERNAL_RENDER_PIPELINE_CACHE_H
