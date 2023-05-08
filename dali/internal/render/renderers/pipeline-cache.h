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
  uint32_t                                referenceCount{0u};
  Graphics::ColorBlendState               colorBlendState;
  Graphics::UniquePtr<Graphics::Pipeline> pipeline;
};

/**
 * Cache Level 1 : Stores rasterization and input assembly states
 */
struct PipelineCacheL1
{
  PipelineCacheL2* GetPipelineCacheL2(bool blend, bool premul, BlendingOptions& blendingOptions);

  /**
   * @brief Clear unused caches.
   */
  bool ClearUnusedCache();

  uint32_t                     hashCode{}; // 1byte cull, 1byte poly, 1byte frontface
  Graphics::RasterizationState rs{};
  Graphics::InputAssemblyState ia{};

  PipelineCacheL2              noBlend; // special case
  std::vector<PipelineCacheL2> level2nodes;
};

/**
 * Cache Level 0 : Stores geometry, program amd vertex input state
 */
struct PipelineCacheL0 // L0 cache
{
  PipelineCacheL1* GetPipelineCacheL1(Render::Renderer* renderer, bool usingReflection);

  /**
   * @brief Clear unused caches.
   */
  void ClearUnusedCache();

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
};

/**
 * Result of PipelineCache::GetPipeline() call
 */
struct PipelineResult
{
  Graphics::Pipeline* pipeline;
  PipelineCacheL2*    level2;
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
  PipelineCacheL0* GetPipelineCacheL0(Program* program, Render::Geometry* geometry);

  /**
   * Retrieves pipeline matching queryInfo struct
   *
   * May retrieve existing pipeline or create one or return nullptr.
   */
  PipelineResult GetPipeline(const PipelineCacheQueryInfo& queryInfo, bool createNewIfNotFound);

  /**
   * @brief This is called before rendering every frame.
   */
  void PreRender();

  /**
   * @brief Decrease the reference count of the pipeline cache.
   * @param pipelineCache The pipeline cache to decrease the reference count
   */
  void ResetPipeline(PipelineCacheL2* pipelineCache);

private:
  /**
   * @brief Clear unused caches.
   */
  void ClearUnusedCache();

private:
  Graphics::Controller*        graphicsController{nullptr};
  std::vector<PipelineCacheL0> level0nodes;

  uint32_t mFrameCount{0u};
};

} // namespace Render
} // namespace Dali::Internal

#endif // DALI_INTERNAL_RENDER_PIPELINE_CACHE_H
