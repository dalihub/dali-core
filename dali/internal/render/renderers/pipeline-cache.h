#ifndef DALI_INTERNAL_RENDER_PIPELINE_CACHE_H
#define DALI_INTERNAL_RENDER_PIPELINE_CACHE_H

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

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-pipeline.h>
#include <dali/graphics-api/graphics-types.h>
#include <dali/public-api/common/list-wrapper.h>

#include <dali/internal/common/blending-options.h>
#include <dali/internal/render/common/render-target-graphics-objects.h> ///< For RenderTargetGraphicsObjects::LifecycleObserver
#include <dali/internal/render/renderers/render-geometry.h>             ///< For Geometry::LifecycleObserver
#include <dali/internal/render/shaders/program.h>                       ///< For Program::LifecycleObserver

namespace Dali::Internal
{
namespace Render
{
class Renderer;

struct PipelineCacheL2;
struct PipelineCacheL1;
struct PipelineCacheL0;
using PipelineCacheL2Container = std::list<PipelineCacheL2>;
using PipelineCacheL1Container = std::list<PipelineCacheL1>;
using PipelineCacheL0Container = std::list<PipelineCacheL0>;
using PipelineCacheL2Ptr       = PipelineCacheL2Container::iterator;
using PipelineCacheL1Ptr       = PipelineCacheL1Container::iterator;
using PipelineCacheL0Ptr       = PipelineCacheL0Container::iterator;

using PipelineLifecycleNotifierPtr = PipelineCacheL0Ptr;
using PipelineCachePtr             = PipelineCacheL2Ptr;

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

  Graphics::UniquePtr<Graphics::Pipeline> dynamicBlendPipeline{};
};

/**
 * Cache Level 0 : Stores geometry, program, render target and vertex input state
 */
struct PipelineCacheL0 // L0 cache
{
  /**
   * Observer to determine when PipelineCacheL0 is no longer present
   */
  class LifecycleObserver
  {
  public:
    enum class NotificationType
    {
      NONE,
      PROGRAM_DESTROYED,
      GEOMETRY_BUFFER_CHANGED,
      GEOMETRY_DESTROYED,
      RENDER_TARGET_GRAPHICS_OBJECTS_DESTROYED,
    };

  public:
    /**
     * Called shortly before the geometry is destroyed.
     */
    virtual void PipelineCacheInvalidated(NotificationType notificationType) = 0;

  protected:
    /**
     * Virtual destructor, no deletion through this interface
     */
    virtual ~LifecycleObserver() = default;
  };

  PipelineCacheL1Ptr GetPipelineCacheL1(Render::Renderer* renderer, bool usingReflection);

  /**
   * @brief Clear unused caches.
   */
  void ClearUnusedCache();

  /**
   * Allows Geometry to track the life-cycle of this object.
   * Note that we allow to observe lifecycle multiple times.
   * But GeometryDestroyed callback will be called only one time.
   * @param[in] observer The observer to add.
   */
  void AddLifecycleObserver(LifecycleObserver& observer)
  {
    DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot add observer while notifying PipelineCacheL0::LifecycleObservers");

    auto iter = mLifecycleObservers.find(&observer);
    if(iter != mLifecycleObservers.end())
    {
      // Increase the number of observer count
      ++(iter->second);
    }
    else
    {
      mLifecycleObservers.insert({&observer, 1u});
    }
  }

  /**
   * The Geometry no longer needs to track the life-cycle of this object.
   * @param[in] observer The observer that to remove.
   */
  void RemoveLifecycleObserver(LifecycleObserver& observer)
  {
    DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot remove observer while notifying PipelineCacheL0::LifecycleObservers");

    auto iter = mLifecycleObservers.find(&observer);
    DALI_ASSERT_ALWAYS(iter != mLifecycleObservers.end());

    if(--(iter->second) == 0u)
    {
      mLifecycleObservers.erase(iter);
    }
  }

  /**
   * Notify to observers and clear.
   */
  void NotifyPipelineCacheDestroyed(PipelineCacheL0::LifecycleObserver::NotificationType notificationType)
  {
    mObserverNotifying = true;
    for(auto&& iter : mLifecycleObservers)
    {
      auto* observer = iter.first;
      observer->PipelineCacheInvalidated(notificationType);
    }
    mLifecycleObservers.clear();

    // DevNote : We don't need to restore mObserverNotifying to false as we are in delete this object.
  }

  Geometry*                                geometry{};
  Program*                                 program{};
  SceneGraph::RenderTargetGraphicsObjects* renderTargetGraphicsObjects{};
  Graphics::VertexInputState               inputState;

  PipelineCacheL1Container level1nodes;

private:
  using LifecycleObserverContainer = std::unordered_map<LifecycleObserver*, uint32_t>; ///< Lifecycle observers container. We allow to add same observer multiple times.
                                                                                       ///< Key is a pointer to observer, value is the number of observer added.
  LifecycleObserverContainer mLifecycleObservers{};

  bool mObserverNotifying{false}; ///< Safety guard flag to ensure that the LifecycleObserver not be added or deleted while observing.
};

struct PipelineCacheQueryInfo
{
  // Program/Geometry
  Renderer* renderer;
  Program*  program;
  Geometry* geometry;

  SceneGraph::RenderTargetGraphicsObjects* renderTargetGraphicsObjects;

  bool cameraUsingReflection;

  // Blending
  bool             blendingEnabled;
  bool             alphaPremultiplied;
  bool             isDynamicBlendEnabled;
  BlendingOptions* blendingOptions;

  // Lightweight hash value before compare each query.
  std::size_t hash{0u};

  // Generate hash value for this query.
  void GenerateHash();

  // Value comparision between two query info.
  static bool Equal(const PipelineCacheQueryInfo& lhs, const PipelineCacheQueryInfo& rhs, const bool compareRenderTarget) noexcept;
};

/**
 * Result of PipelineCache::GetPipeline() call
 */
struct PipelineResult
{
  Graphics::Pipeline*          pipeline;
  PipelineLifecycleNotifierPtr level0;
  PipelineCachePtr             level2;
};

/**
 * Pipeline cache
 */
class PipelineCache : public Program::LifecycleObserver, public Geometry::LifecycleObserver, public SceneGraph::RenderTargetGraphicsObjects::LifecycleObserver
{
public:
  /**
   * Constructor
   * @param[in] controller Graphics controller
   */
  explicit PipelineCache(Graphics::Controller& controller);

  /**
   * Destructor
   */
  ~PipelineCache();

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
  void ResetPipeline(PipelineCachePtr pipelineCache);

  /**
   * @brief Check if dynamic blending is enabled
   * @return True if dynamic blending is enabled, false otherwise
   */
  bool IsDynamicBlendEnabled() const;

public: // From Program::LifecycleObserver
  /**
   * @copydoc Dali::Internal::Program::LifecycleObserver::ProgramDestroyed()
   */
  void ProgramDestroyed(const Program* program);

public: // From Geometry::LifecycleObserver
  /**
   * @copydoc Dali::Internal::Geometry::LifecycleObserver::GeometryBufferChanged()
   */
  void GeometryBufferChanged(const Geometry* geometry);

  /**
   * @copydoc Dali::Internal::Geometry::LifecycleObserver::GeometryDestroyed()
   */
  void GeometryDestroyed(const Geometry* geometry);

public: // From SceneGraph::RenderTargetGraphicsObjects::LifecycleObserver
  /**
   * @copydoc Dali::Internal::SceneGraph::RenderTargetGraphicsObjects::LifecycleObserver::RenderTargetGraphicsObjectsDestroyed()
   */
  void RenderTargetGraphicsObjectsDestroyed(const SceneGraph::RenderTargetGraphicsObjects* renderTargetGraphicsObjects);

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

  /**
   * Retrieves next cache level
   */
  PipelineCacheL0Ptr GetPipelineCacheL0(Program* program, Render::Geometry* geometry, SceneGraph::RenderTargetGraphicsObjects* renderTargetGraphicsObjects);

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

private:
  Graphics::Controller*    graphicsController{nullptr};
  PipelineCacheL0Container level0nodes;

  // Cache latest queries whether blend enabled or not.
  // (Since most UI case (like Text and Image) enable blend, and most 3D case disable blend.)
  PipelineCacheQueryInfo mLatestQuery[2];  ///< Latest requested query info. It will be invalidate after query's renderer / geometry / blendingOptions value changed.
  PipelineResult         mLatestResult[2]; ///< Latest used result. It will be invalidate when we call CleanLatestUsedCache() or some cache changed.

  uint32_t mFrameCount{0u};
  const bool mPipelineUseRenderTarget; ///< Ask from Graphics::Controller
  mutable uint32_t mSupportedDynamicStates{0u};
  mutable bool mDynamicBlendEnabled{false};
  mutable bool mDeviceCapabilitiesCached{false};
};

} // namespace Render
} // namespace Dali::Internal

#endif // DALI_INTERNAL_RENDER_PIPELINE_CACHE_H
