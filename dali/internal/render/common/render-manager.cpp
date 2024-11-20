/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/render/common/render-manager.h>

// EXTERNAL INCLUDES
#include <memory.h>

// INTERNAL INCLUDES
#include <dali/integration-api/core.h>
#include <dali/integration-api/ordered-set.h>
#include <dali/integration-api/trace.h>

#include <dali/internal/event/common/scene-impl.h>

#include <dali/internal/update/common/scene-graph-scene.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>

#include <dali/internal/common/owner-key-container.h>

#include <dali/internal/render/common/render-algorithms.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/pipeline-cache.h>
#include <dali/internal/render/renderers/render-frame-buffer.h>
#include <dali/internal/render/renderers/render-texture.h>
#include <dali/internal/render/renderers/uniform-buffer-manager.h>
#include <dali/internal/render/renderers/uniform-buffer.h>
#include <dali/internal/render/shaders/program-controller.h>

#include <memory>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
#if defined(DEBUG_ENABLED)
namespace
{
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_MANAGER");
} // unnamed namespace
#endif

namespace
{
constexpr uint32_t PROGRAM_CACHE_CLEAN_FRAME_COUNT = 300u; // 60fps * 5sec

constexpr uint32_t INITIAL_PROGRAM_CACHE_CLEAN_THRESHOLD = 64u; // Let we trigger program cache clean if the number of shader is bigger than this value
constexpr uint32_t MAXIMUM_PROGRAM_CACHE_CLEAN_THRESHOLD = 1024u;

constexpr uint32_t PROGRAM_CACHE_FORCE_CLEAN_FRAME_COUNT = PROGRAM_CACHE_CLEAN_FRAME_COUNT + MAXIMUM_PROGRAM_CACHE_CLEAN_THRESHOLD;

static_assert(PROGRAM_CACHE_CLEAN_FRAME_COUNT <= PROGRAM_CACHE_FORCE_CLEAN_FRAME_COUNT);
static_assert(MAXIMUM_PROGRAM_CACHE_CLEAN_THRESHOLD <= PROGRAM_CACHE_FORCE_CLEAN_FRAME_COUNT);

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
constexpr uint32_t SHRINK_TO_FIT_FRAME_COUNT = (1u << 8); ///< 256 frames. Make this value as power of 2.

/**
 * Flag whether property has changed, during the Render phase.
 */
enum ContainerRemovedFlagBits : uint8_t
{
  NOTHING  = 0x00,
  RENDERER = 0x01,
  TEXTURE  = 0x02,
};

/**
 * @brief ContainerRemovedFlags alters behaviour of implementation
 */
using ContainerRemovedFlags = uint8_t;
#endif

inline Graphics::Rect2D RecalculateScissorArea(const Graphics::Rect2D& scissorArea, int orientation, const Rect<int32_t>& viewportRect)
{
  Graphics::Rect2D newScissorArea;

  if(orientation == 90)
  {
    newScissorArea.x      = viewportRect.height - (scissorArea.y + scissorArea.height);
    newScissorArea.y      = scissorArea.x;
    newScissorArea.width  = scissorArea.height;
    newScissorArea.height = scissorArea.width;
  }
  else if(orientation == 180)
  {
    newScissorArea.x      = viewportRect.width - (scissorArea.x + scissorArea.width);
    newScissorArea.y      = viewportRect.height - (scissorArea.y + scissorArea.height);
    newScissorArea.width  = scissorArea.width;
    newScissorArea.height = scissorArea.height;
  }
  else if(orientation == 270)
  {
    newScissorArea.x      = scissorArea.y;
    newScissorArea.y      = viewportRect.width - (scissorArea.x + scissorArea.width);
    newScissorArea.width  = scissorArea.height;
    newScissorArea.height = scissorArea.width;
  }
  else
  {
    newScissorArea.x      = scissorArea.x;
    newScissorArea.y      = scissorArea.y;
    newScissorArea.width  = scissorArea.width;
    newScissorArea.height = scissorArea.height;
  }
  return newScissorArea;
}

inline Rect<int32_t> CalculateUpdateArea(RenderItem& item, BufferIndex renderBufferIndex, const Rect<int32_t>& viewportRect)
{
  Vector4 updateArea;
  if(item.mNode && item.mNode->IsTextureUpdateAreaUsed() && item.mRenderer)
  {
    updateArea = item.mRenderer->GetTextureUpdateArea();
  }
  else
  {
    updateArea = item.mRenderer ? item.mRenderer->GetVisualTransformedUpdateArea(renderBufferIndex, item.mUpdateArea) : item.mUpdateArea;
  }

  return RenderItem::CalculateViewportSpaceAABB(item.mModelViewMatrix, Vector3(updateArea.x, updateArea.y, 0.0f), Vector3(updateArea.z, updateArea.w, 0.0f), viewportRect.width, viewportRect.height);
}

inline void AlignDamagedRect(Rect<int32_t>& rect)
{
  const int left   = rect.x;
  const int top    = rect.y;
  const int right  = rect.x + rect.width;
  const int bottom = rect.y + rect.height;
  rect.x           = (left / 16) * 16;
  rect.y           = (top / 16) * 16;
  rect.width       = ((right + 16) / 16) * 16 - rect.x;
  rect.height      = ((bottom + 16) / 16) * 16 - rect.y;
}

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_RENDER_PROCESS, false);
} // namespace

/**
 * Structure to contain internal data
 */
struct RenderManager::Impl
{
  Impl(Graphics::Controller&               graphicsController,
       Integration::DepthBufferAvailable   depthBufferAvailableParam,
       Integration::StencilBufferAvailable stencilBufferAvailableParam,
       Integration::PartialUpdateAvailable partialUpdateAvailableParam)
  : graphicsController(graphicsController),
    renderAlgorithms(graphicsController),
    programController(graphicsController),
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
    containerRemovedFlags(ContainerRemovedFlagBits::NOTHING),
#endif
    depthBufferAvailable(depthBufferAvailableParam),
    stencilBufferAvailable(stencilBufferAvailableParam),
    partialUpdateAvailable(partialUpdateAvailableParam)
  {
    uniformBufferManager = std::make_unique<Render::UniformBufferManager>(&graphicsController);
    pipelineCache        = std::make_unique<Render::PipelineCache>(graphicsController);
  }

  ~Impl()
  {
    rendererContainer.Clear(); // clear now before the program contoller and the pipeline cache are deleted
    pipelineCache.reset();     // clear now before the program contoller is deleted
  }

  void AddRenderTracker(Render::RenderTracker* renderTracker)
  {
    DALI_ASSERT_DEBUG(renderTracker != nullptr);
    mRenderTrackers.PushBack(renderTracker);
  }

  void RemoveRenderTracker(Render::RenderTracker* renderTracker)
  {
    mRenderTrackers.EraseObject(renderTracker);
  }

  void UpdateTrackers()
  {
    for(auto&& iter : mRenderTrackers)
    {
      iter->PollSyncObject();
    }
  }

  /**
   * @brief Prepare to check used count of shader and program cache.
   * It will be used when the size of shader and program cache is too big, so we need to collect garbages.
   * Currently, we collect and remove programs only if the number of program/shader is bigger than threshold.
   *
   * @note Should be called at PreRender
   */
  void RequestProgramCacheCleanIfNeed()
  {
    if(programCacheCleanRequestedFrame == 0u)
    {
      if(DALI_UNLIKELY(programController.GetCachedProgramCount() > programCacheCleanRequiredThreshold))
      {
        // Mark current frame count
        programCacheCleanRequestedFrame = frameCount;

        DALI_LOG_RELEASE_INFO("Trigger ProgramCache GC. program : [%u]\n", programController.GetCachedProgramCount());

        // Prepare to collect program used flag.
        programController.ResetUsedFlag();
      }
    }
  }

  /**
   * @brief Cleanup unused program and shader cache if need.
   *
   * @note Should be called at PostRender
   */
  void ClearUnusedProgramCacheIfNeed()
  {
    // Remove unused shader and programs during we render PROGRAM_CACHE_CLEAN_FRAME_COUNT frames.
    if(programCacheCleanRequestedFrame != 0u && programCacheCleanRequestedFrame + PROGRAM_CACHE_CLEAN_FRAME_COUNT - 1u <= frameCount)
    {
      // Clean cache incrementally, or force clean if we spend too much frames to collect them.
      if(!programController.ClearUnusedCacheIncrementally(programCacheCleanRequestedFrame + PROGRAM_CACHE_FORCE_CLEAN_FRAME_COUNT - 1u <= frameCount))
      {
        // Reset current frame count.
        programCacheCleanRequestedFrame = 0u;

        DALI_LOG_RELEASE_INFO("ProgramCache GC finished. program : [%u]\n", programController.GetCachedProgramCount());

        // Double up threshold
        programCacheCleanRequiredThreshold <<= 1;
        programCacheCleanRequiredThreshold = std::max(programCacheCleanRequiredThreshold, programController.GetCachedProgramCount());

        if(programCacheCleanRequiredThreshold > MAXIMUM_PROGRAM_CACHE_CLEAN_THRESHOLD)
        {
          programCacheCleanRequiredThreshold = MAXIMUM_PROGRAM_CACHE_CLEAN_THRESHOLD;
        }
      }
    }
  }

  /**
   * @brief Remove all owned render context.
   *
   * @note Should be called at ContextDestroyed case.
   */
  void ContextDestroyed()
  {
    sceneContainer.clear();
    renderAlgorithms.DestroyCommandBuffer();

    samplerContainer.Clear();
    frameBufferContainer.Clear();
    vertexBufferContainer.Clear();
    geometryContainer.Clear();
    rendererContainer.Clear();
    textureContainer.Clear();

    mRenderTrackers.Clear();

    updatedTextures.Clear();
    textureDiscardQueue.Clear();

    pipelineCache.reset(); // clear now before the program contoller is deleted

    if(DALI_LIKELY(uniformBufferManager))
    {
      uniformBufferManager->ContextDestroyed();
    }
    uniformBufferManager.reset();
  }

  // the order is important for destruction,
  Graphics::Controller&           graphicsController;
  RenderQueue                     renderQueue;      ///< A message queue for receiving messages from the update-thread.
  std::vector<SceneGraph::Scene*> sceneContainer;   ///< List of pointers to the scene graph objects of the scenes
  Render::RenderAlgorithms        renderAlgorithms; ///< The RenderAlgorithms object is used to action the renders required by a RenderInstruction

  Integration::OrderedSet<Render::Sampler>      samplerContainer;      ///< List of owned samplers
  Integration::OrderedSet<Render::FrameBuffer>  frameBufferContainer;  ///< List of owned framebuffers
  Integration::OrderedSet<Render::VertexBuffer> vertexBufferContainer; ///< List of owned vertex buffers
  Integration::OrderedSet<Render::Geometry>     geometryContainer;     ///< List of owned Geometries
  OwnerKeyContainer<Render::Renderer>           rendererContainer;     ///< List of owned renderers
  OwnerKeyContainer<Render::Texture>            textureContainer;      ///< List of owned textures

  Integration::OrderedSet<Render::RenderTracker> mRenderTrackers; ///< List of owned render trackers

  OwnerKeyContainer<Render::Texture> textureDiscardQueue; ///< Discarded textures

  ProgramController programController; ///< Owner of the programs

  std::unique_ptr<Render::UniformBufferManager> uniformBufferManager; ///< The uniform buffer manager
  std::unique_ptr<Render::PipelineCache>        pipelineCache;

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  ContainerRemovedFlags containerRemovedFlags; ///< cumulative container removed flags during current frame
#endif

  Integration::DepthBufferAvailable   depthBufferAvailable;   ///< Whether the depth buffer is available
  Integration::StencilBufferAvailable stencilBufferAvailable; ///< Whether the stencil buffer is available
  Integration::PartialUpdateAvailable partialUpdateAvailable; ///< Whether the partial update is available

  Vector<Render::TextureKey> updatedTextures{}; ///< The updated texture list

  uint32_t    frameCount{0u};                                                    ///< The current frame count
  BufferIndex renderBufferIndex{SceneGraphBuffers::INITIAL_UPDATE_BUFFER_INDEX}; ///< The index of the buffer to read from;

  uint32_t programCacheCleanRequiredThreshold{INITIAL_PROGRAM_CACHE_CLEAN_THRESHOLD}; ///< The threshold to request program cache clean up operation.
                                                                                      ///< It will be automatically increased after we request cache clean.

  uint32_t programCacheCleanRequestedFrame{0u}; ///< 0 mean, we didn't request program cache clean. otherwise, we request cache clean.
                                                ///< otherwise, we request program cache clean at that frame, and now we are checking reference.

  bool lastFrameWasRendered{false}; ///< Keeps track of the last frame being rendered due to having render instructions
  bool commandBufferSubmitted{false};
};

RenderManager* RenderManager::New(Graphics::Controller&               graphicsController,
                                  Integration::DepthBufferAvailable   depthBufferAvailable,
                                  Integration::StencilBufferAvailable stencilBufferAvailable,
                                  Integration::PartialUpdateAvailable partialUpdateAvailable)
{
  auto* manager  = new RenderManager;
  manager->mImpl = new Impl(graphicsController,
                            depthBufferAvailable,
                            stencilBufferAvailable,
                            partialUpdateAvailable);
  return manager;
}

RenderManager::RenderManager()
: mImpl(nullptr)
{
}

RenderManager::~RenderManager()
{
  delete mImpl;

  // Ensure to release memory pool
  Render::Renderer::ResetMemoryPool();
  Render::Texture::ResetMemoryPool();
}

void RenderManager::ContextDestroyed()
{
  // Call Destroy for some items.
  for(auto&& item : mImpl->frameBufferContainer)
  {
    if(DALI_LIKELY(item))
    {
      item->Destroy();
    }
  }
  for(auto&& item : mImpl->textureContainer)
  {
    if(DALI_LIKELY(item))
    {
      item->Destroy();
    }
  }

  // Remove owned render context
  mImpl->ContextDestroyed();
}

RenderQueue& RenderManager::GetRenderQueue()
{
  return mImpl->renderQueue;
}

void RenderManager::SetShaderSaver(ShaderSaver& upstream)
{
}

void RenderManager::AddRenderer(const Render::RendererKey& renderer)
{
  // Initialize the renderer as we are now in render thread
  renderer->Initialize(mImpl->graphicsController, mImpl->programController, *(mImpl->uniformBufferManager.get()), *(mImpl->pipelineCache.get()));

  mImpl->rendererContainer.PushBack(renderer);
}

void RenderManager::RemoveRenderer(const Render::RendererKey& renderer)
{
  mImpl->rendererContainer.EraseKey(renderer);
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  mImpl->containerRemovedFlags |= ContainerRemovedFlagBits::RENDERER;
#endif
}

void RenderManager::AddSampler(OwnerPointer<Render::Sampler>& sampler)
{
  sampler->Initialize(mImpl->graphicsController);
  mImpl->samplerContainer.PushBack(sampler.Release());
}

void RenderManager::RemoveSampler(Render::Sampler* sampler)
{
  mImpl->samplerContainer.EraseObject(sampler);
}

void RenderManager::AddTexture(const Render::TextureKey& textureKey)
{
  DALI_ASSERT_DEBUG(textureKey && "Trying to add empty texture key");

  textureKey->Initialize(mImpl->graphicsController, *this);
  mImpl->textureContainer.PushBack(textureKey);
  mImpl->updatedTextures.PushBack(textureKey);
}

void RenderManager::RemoveTexture(const Render::TextureKey& textureKey)
{
  DALI_ASSERT_DEBUG(textureKey && "Trying to remove empty texture key");

  // Find the texture, use std::find so we can do the erase by iterator safely
  auto iter = std::find(mImpl->textureContainer.Begin(), mImpl->textureContainer.End(), textureKey);

  if(iter != mImpl->textureContainer.End())
  {
    // Destroy texture.
    textureKey->Destroy();

    // Transfer ownership to the discard queue, this keeps the object alive, until the render-thread has finished with it
    mImpl->textureDiscardQueue.PushBack(mImpl->textureContainer.Release(iter));

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
    mImpl->containerRemovedFlags |= ContainerRemovedFlagBits::TEXTURE;
#endif
  }
}

void RenderManager::UploadTexture(const Render::TextureKey& textureKey, PixelDataPtr pixelData, const Graphics::UploadParams& params)
{
  DALI_ASSERT_DEBUG(textureKey && "Trying to upload to empty texture key");
  textureKey->Upload(pixelData, params);

  mImpl->updatedTextures.PushBack(textureKey);
}

void RenderManager::GenerateMipmaps(const Render::TextureKey& textureKey)
{
  DALI_ASSERT_DEBUG(textureKey && "Trying to generate mipmaps on empty texture key");
  textureKey->GenerateMipmaps();

  mImpl->updatedTextures.PushBack(textureKey);
}

void RenderManager::SetTextureSize(const Render::TextureKey& textureKey, const Dali::ImageDimensions& size)
{
  DALI_ASSERT_DEBUG(textureKey && "Trying to set size on empty texture key");
  textureKey->SetWidth(size.GetWidth());
  textureKey->SetHeight(size.GetHeight());
}

void RenderManager::SetTextureFormat(const Render::TextureKey& textureKey, Dali::Pixel::Format pixelFormat)
{
  DALI_ASSERT_DEBUG(textureKey && "Trying to set pixel format on empty texture key");
  textureKey->SetPixelFormat(pixelFormat);
}

void RenderManager::SetTextureUpdated(const Render::TextureKey& textureKey)
{
  DALI_ASSERT_DEBUG(textureKey && "Trying to set updated on empty texture key");
  textureKey->SetUpdated(true);

  mImpl->updatedTextures.PushBack(textureKey);
}

void RenderManager::SetFilterMode(Render::Sampler* sampler, uint32_t minFilterMode, uint32_t magFilterMode)
{
  sampler->SetFilterMode(static_cast<Dali::FilterMode::Type>(minFilterMode),
                         static_cast<Dali::FilterMode::Type>(magFilterMode));
}

void RenderManager::SetWrapMode(Render::Sampler* sampler, uint32_t rWrapMode, uint32_t sWrapMode, uint32_t tWrapMode)
{
  sampler->SetWrapMode(static_cast<Dali::WrapMode::Type>(rWrapMode),
                       static_cast<Dali::WrapMode::Type>(sWrapMode),
                       static_cast<Dali::WrapMode::Type>(tWrapMode));
}

void RenderManager::AddFrameBuffer(OwnerPointer<Render::FrameBuffer>& frameBuffer)
{
  Render::FrameBuffer* frameBufferPtr = frameBuffer.Release();
  mImpl->frameBufferContainer.PushBack(frameBufferPtr);
  frameBufferPtr->Initialize(mImpl->graphicsController);
}

void RenderManager::RemoveFrameBuffer(Render::FrameBuffer* frameBuffer)
{
  DALI_ASSERT_DEBUG(nullptr != frameBuffer);

  // Find the framebuffer, use OrderedSet.Find so we can safely do the erase
  auto iter = mImpl->frameBufferContainer.Find(frameBuffer);

  if(iter != mImpl->frameBufferContainer.End())
  {
    frameBuffer->Destroy();
    mImpl->frameBufferContainer.Erase(iter); // frameBuffer found; now destroy it
  }
}

void RenderManager::InitializeScene(SceneGraph::Scene* scene)
{
  scene->Initialize(mImpl->graphicsController, mImpl->depthBufferAvailable, mImpl->stencilBufferAvailable);
  mImpl->sceneContainer.push_back(scene);
  mImpl->uniformBufferManager->RegisterScene(scene);
}

void RenderManager::UninitializeScene(SceneGraph::Scene* scene)
{
  mImpl->uniformBufferManager->UnregisterScene(scene);
  auto iter = std::find(mImpl->sceneContainer.begin(), mImpl->sceneContainer.end(), scene);
  if(iter != mImpl->sceneContainer.end())
  {
    mImpl->sceneContainer.erase(iter);
  }
}

void RenderManager::SurfaceReplaced(SceneGraph::Scene* scene)
{
  scene->Initialize(mImpl->graphicsController, mImpl->depthBufferAvailable, mImpl->stencilBufferAvailable);
}

void RenderManager::AttachColorTextureToFrameBuffer(Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer)
{
  frameBuffer->AttachColorTexture(texture, mipmapLevel, layer);
}

void RenderManager::AttachDepthTextureToFrameBuffer(Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel)
{
  frameBuffer->AttachDepthTexture(texture, mipmapLevel);
}

void RenderManager::AttachDepthStencilTextureToFrameBuffer(Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel)
{
  frameBuffer->AttachDepthStencilTexture(texture, mipmapLevel);
}

void RenderManager::SetMultiSamplingLevelToFrameBuffer(Render::FrameBuffer* frameBuffer, uint8_t multiSamplingLevel)
{
  frameBuffer->SetMultiSamplingLevel(multiSamplingLevel);
}

void RenderManager::AddVertexBuffer(OwnerPointer<Render::VertexBuffer>& vertexBuffer)
{
  mImpl->vertexBufferContainer.PushBack(vertexBuffer.Release());
}

void RenderManager::RemoveVertexBuffer(Render::VertexBuffer* vertexBuffer)
{
  mImpl->vertexBufferContainer.EraseObject(vertexBuffer);
}

void RenderManager::SetVertexBufferFormat(Render::VertexBuffer* vertexBuffer, OwnerPointer<Render::VertexBuffer::Format>& format)
{
  vertexBuffer->SetFormat(format.Release());
}

void RenderManager::SetVertexBufferData(Render::VertexBuffer* vertexBuffer, OwnerPointer<Vector<uint8_t>>& data, uint32_t size)
{
  vertexBuffer->SetData(data.Release(), size);
}

void RenderManager::SetVertexBufferUpdateCallback(Render::VertexBuffer* vertexBuffer, Dali::VertexBufferUpdateCallback* callback)
{
  vertexBuffer->SetVertexBufferUpdateCallback(callback);
}

void RenderManager::SetIndexBuffer(Render::Geometry* geometry, Render::Geometry::Uint16ContainerType& indices)
{
  geometry->SetIndexBuffer(indices);
}

void RenderManager::SetIndexBuffer(Render::Geometry* geometry, Render::Geometry::Uint32ContainerType& indices)
{
  geometry->SetIndexBuffer(indices);
}

void RenderManager::AddGeometry(OwnerPointer<Render::Geometry>& geometry)
{
  mImpl->geometryContainer.PushBack(geometry.Release());
}

void RenderManager::RemoveGeometry(Render::Geometry* geometry)
{
  mImpl->geometryContainer.EraseObject(geometry);
}

void RenderManager::AttachVertexBuffer(Render::Geometry* geometry, Render::VertexBuffer* vertexBuffer)
{
  geometry->AddVertexBuffer(vertexBuffer);
}

void RenderManager::RemoveVertexBuffer(Render::Geometry* geometry, Render::VertexBuffer* vertexBuffer)
{
  geometry->RemoveVertexBuffer(vertexBuffer);
}

void RenderManager::SetGeometryType(Render::Geometry* geometry, uint32_t geometryType)
{
  geometry->SetType(Render::Geometry::Type(geometryType));
}

void RenderManager::AddRenderTracker(Render::RenderTracker* renderTracker)
{
  mImpl->AddRenderTracker(renderTracker);
}

void RenderManager::RemoveRenderTracker(Render::RenderTracker* renderTracker)
{
  mImpl->RemoveRenderTracker(renderTracker);
}

void RenderManager::PreRender(Integration::RenderStatus& status, bool forceClear)
{
  DALI_PRINT_RENDER_START(mImpl->renderBufferIndex);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "\n\nNewFrame %d\n", mImpl->frameCount);

  // Increment the frame count at the beginning of each frame
  ++mImpl->frameCount;

  // Process messages queued during previous update
  mImpl->renderQueue.ProcessMessages(mImpl->renderBufferIndex);

  uint32_t totalInstructionCount = 0u;
  for(auto& i : mImpl->sceneContainer)
  {
    totalInstructionCount += i->GetRenderInstructions().Count(mImpl->renderBufferIndex);
  }

  const bool haveInstructions = totalInstructionCount > 0u;

  DALI_LOG_INFO(gLogFilter, Debug::General, "Render: haveInstructions(%s) || mImpl->lastFrameWasRendered(%s) || forceClear(%s)\n", haveInstructions ? "true" : "false", mImpl->lastFrameWasRendered ? "true" : "false", forceClear ? "true" : "false");

  // Only render if we have instructions to render, or the last frame was rendered (and therefore a clear is required).
  if(haveInstructions || mImpl->lastFrameWasRendered || forceClear)
  {
    DALI_LOG_INFO(gLogFilter, Debug::General, "Render: Processing\n");

    // Upload the geometries
    for(auto&& geom : mImpl->geometryContainer)
    {
      geom->Upload(mImpl->graphicsController);
    }
  }

  // Reset pipeline cache before rendering
  mImpl->pipelineCache->PreRender();

  // Check we need to clean up program cache
  mImpl->RequestProgramCacheCleanIfNeed();

  mImpl->commandBufferSubmitted = false;
}

void RenderManager::PreRender(Integration::Scene& scene, std::vector<Rect<int>>& damagedRects)
{
  if(mImpl->partialUpdateAvailable != Integration::PartialUpdateAvailable::TRUE)
  {
    return;
  }

  Internal::Scene&   sceneInternal = GetImplementation(scene);
  SceneGraph::Scene* sceneObject   = sceneInternal.GetSceneObject();

  if(!sceneObject || sceneObject->IsRenderingSkipped())
  {
    // We don't need to calculate dirty rects
    if(!sceneObject)
    {
      DALI_LOG_ERROR("Scene was empty handle. Skip pre-rendering\n");
    }
    else
    {
      DALI_LOG_RELEASE_INFO("RenderingSkipped was set true. Skip pre-rendering\n");
    }
    return;
  }

  class DamagedRectsCleaner
  {
  public:
    explicit DamagedRectsCleaner(std::vector<Rect<int>>& damagedRects, Rect<int>& surfaceRect)
    : mDamagedRects(damagedRects),
      mSurfaceRect(surfaceRect),
      mCleanOnReturn(true)
    {
    }

    void SetCleanOnReturn(bool cleanOnReturn)
    {
      mCleanOnReturn = cleanOnReturn;
    }

    ~DamagedRectsCleaner()
    {
      if(mCleanOnReturn)
      {
        mDamagedRects.clear();
        mDamagedRects.push_back(mSurfaceRect);
      }
    }

  private:
    std::vector<Rect<int>>& mDamagedRects;
    Rect<int>               mSurfaceRect;
    bool                    mCleanOnReturn;
  };

  Rect<int32_t> surfaceRect = sceneObject->GetSurfaceRect();

  // Clean collected dirty/damaged rects on exit if 3d layer or 3d node or other conditions.
  DamagedRectsCleaner damagedRectCleaner(damagedRects, surfaceRect);
  bool                cleanDamagedRect = false;

  Scene::ItemsDirtyRectsContainer& itemsDirtyRects = sceneObject->GetItemsDirtyRects();

  if(!sceneObject->IsPartialUpdateEnabled())
  {
    // Clear all dirty rects
    // The rects will be added when partial updated is enabled again
    itemsDirtyRects.clear();
    return;
  }

  // Mark previous dirty rects in the std::unordered_map.
  for(auto& dirtyRectPair : itemsDirtyRects)
  {
    dirtyRectPair.second.visited = false;
  }

  uint32_t instructionCount = sceneObject->GetRenderInstructions().Count(mImpl->renderBufferIndex);
  for(uint32_t i = 0; i < instructionCount; ++i)
  {
    RenderInstruction& instruction = sceneObject->GetRenderInstructions().At(mImpl->renderBufferIndex, i);

    if(instruction.mFrameBuffer)
    {
      cleanDamagedRect = true;
      continue; // TODO: reset, we don't deal with render tasks with framebuffers (for now)
    }

    const Camera* camera = instruction.GetCamera();
    if(camera && camera->mType == Camera::DEFAULT_TYPE && camera->mTargetPosition == Camera::DEFAULT_TARGET_POSITION)
    {
      Vector3    position;
      Vector3    scale;
      Quaternion orientation;
      camera->GetWorldMatrix(mImpl->renderBufferIndex).GetTransformComponents(position, orientation, scale);

      Vector3 orientationAxis;
      Radian  orientationAngle;
      orientation.ToAxisAngle(orientationAxis, orientationAngle);

      if(position.x > Math::MACHINE_EPSILON_10000 ||
         position.y > Math::MACHINE_EPSILON_10000 ||
         orientationAxis != Vector3(0.0f, 1.0f, 0.0f) ||
         orientationAngle != ANGLE_180 ||
         scale != Vector3(1.0f, 1.0f, 1.0f))
      {
        cleanDamagedRect = true;
        continue;
      }
    }
    else
    {
      cleanDamagedRect = true;
      continue;
    }

    Rect<int32_t> viewportRect;
    if(instruction.mIsViewportSet)
    {
      const int32_t y = (surfaceRect.height - instruction.mViewport.height) - instruction.mViewport.y;
      viewportRect.Set(instruction.mViewport.x, y, instruction.mViewport.width, instruction.mViewport.height);
      if(viewportRect.IsEmpty() || !viewportRect.IsValid())
      {
        cleanDamagedRect = true;
        continue; // just skip funny use cases for now, empty viewport means it is set somewhere else
      }
    }
    else
    {
      viewportRect = surfaceRect;
    }

    const Matrix* viewMatrix       = instruction.GetViewMatrix(mImpl->renderBufferIndex);
    const Matrix* projectionMatrix = instruction.GetProjectionMatrix(mImpl->renderBufferIndex);
    if(viewMatrix && projectionMatrix)
    {
      const RenderListContainer::SizeType count = instruction.RenderListCount();
      for(RenderListContainer::SizeType index = 0u; index < count; ++index)
      {
        const RenderList* renderList = instruction.GetRenderList(index);
        if(renderList)
        {
          if(!renderList->IsEmpty())
          {
            const std::size_t listCount = renderList->Count();
            for(uint32_t listIndex = 0u; listIndex < listCount; ++listIndex)
            {
              RenderItem& item = renderList->GetItem(listIndex);
              // If the item does 3D transformation, make full update
              if(item.mUpdateArea == Vector4::ZERO)
              {
                cleanDamagedRect = true;

                // Save the full rect in the damaged list. We need it when this item is removed
                DirtyRectKey dirtyRectKey(item.mNode, item.mRenderer);
                auto         dirtyRectPos = itemsDirtyRects.find(dirtyRectKey);
                if(dirtyRectPos != itemsDirtyRects.end())
                {
                  // Replace the rect
                  dirtyRectPos->second.visited = true;
                  dirtyRectPos->second.rect    = surfaceRect;
                }
                else
                {
                  // Else, just insert the new dirtyrect
                  itemsDirtyRects.insert({dirtyRectKey, surfaceRect});
                }
                continue;
              }

              Rect<int>    rect;
              DirtyRectKey dirtyRectKey(item.mNode, item.mRenderer);
              // If the item refers to updated node or renderer.
              if(item.mIsUpdated ||
                 (item.mNode &&
                  (item.mNode->Updated() || (item.mRenderer && item.mRenderer->Updated()))))
              {
                item.mIsUpdated = false;

                rect = CalculateUpdateArea(item, mImpl->renderBufferIndex, viewportRect);
                if(rect.IsValid() && rect.Intersect(viewportRect) && !rect.IsEmpty())
                {
                  AlignDamagedRect(rect);

                  // Found valid dirty rect.
                  auto dirtyRectPos = itemsDirtyRects.find(dirtyRectKey);
                  if(dirtyRectPos != itemsDirtyRects.end())
                  {
                    Rect<int> currentRect = rect;

                    // Same item, merge it with the previous rect
                    rect.Merge(dirtyRectPos->second.rect);

                    // Replace the rect as current
                    dirtyRectPos->second.visited = true;
                    dirtyRectPos->second.rect    = currentRect;
                  }
                  else
                  {
                    // Else, just insert the new dirtyrect
                    itemsDirtyRects.insert({dirtyRectKey, rect});
                  }

                  damagedRects.push_back(rect);
                }
              }
              else
              {
                // 1. The item is not dirty, the node and renderer referenced by the item are still exist.
                // 2. Mark the related dirty rects as visited so they will not be removed below.
                auto dirtyRectPos = itemsDirtyRects.find(dirtyRectKey);
                if(dirtyRectPos != itemsDirtyRects.end())
                {
                  dirtyRectPos->second.visited = true;
                }
                else
                {
                  // The item is not in the list for some reason. Add the current rect!
                  rect = CalculateUpdateArea(item, mImpl->renderBufferIndex, viewportRect);
                  if(rect.IsValid() && rect.Intersect(viewportRect) && !rect.IsEmpty())
                  {
                    AlignDamagedRect(rect);

                    itemsDirtyRects.insert({dirtyRectKey, rect});
                  }
                  cleanDamagedRect = true; // And make full update at this frame
                }
              }
            }
          }
        }
      }
    }
  }

  // Check removed nodes or removed renderers dirty rects
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  for(auto iter = itemsDirtyRects.begin(); iter != itemsDirtyRects.end();)
#else
  // Note, std::unordered_map end iterator is validate if we call erase.
  for(auto iter = itemsDirtyRects.cbegin(), iterEnd = itemsDirtyRects.cend(); iter != iterEnd;)
#endif
  {
    if(!iter->second.visited)
    {
      damagedRects.push_back(iter->second.rect);
      iter = itemsDirtyRects.erase(iter);
    }
    else
    {
      ++iter;
    }
  }

  if(sceneObject->IsNeededFullUpdate())
  {
    cleanDamagedRect = true; // And make full update at this frame
  }

  if(!cleanDamagedRect)
  {
    damagedRectCleaner.SetCleanOnReturn(false);
  }
}

void RenderManager::RenderScene(Integration::RenderStatus& status, Integration::Scene& scene, bool renderToFbo)
{
  SceneGraph::Scene* sceneObject = GetImplementation(scene).GetSceneObject();
  if(!sceneObject)
  {
    DALI_LOG_ERROR("Scene was empty handle. Skip rendering\n");
    return;
  }

  Rect<int> clippingRect = sceneObject->GetSurfaceRect();
  RenderScene(status, scene, renderToFbo, clippingRect);
}

void RenderManager::RenderScene(Integration::RenderStatus& status, Integration::Scene& scene, bool renderToFbo, Rect<int>& clippingRect)
{
  if(mImpl->partialUpdateAvailable == Integration::PartialUpdateAvailable::TRUE && !renderToFbo && clippingRect.IsEmpty())
  {
    DALI_LOG_DEBUG_INFO("ClippingRect was empty. Skip rendering\n");
    return;
  }

  Internal::Scene&   sceneInternal = GetImplementation(scene);
  SceneGraph::Scene* sceneObject   = sceneInternal.GetSceneObject();
  if(!sceneObject)
  {
    DALI_LOG_ERROR("Scene was empty handle. Skip rendering\n");
    return;
  }

  // @todo These should be members of scene
  const Integration::DepthBufferAvailable   depthBufferAvailable   = mImpl->depthBufferAvailable;
  const Integration::StencilBufferAvailable stencilBufferAvailable = mImpl->stencilBufferAvailable;

  uint32_t instructionCount = sceneObject->GetRenderInstructions().Count(mImpl->renderBufferIndex);

  std::vector<Graphics::RenderTarget*> targetsToPresent;

  Rect<int32_t> surfaceRect = sceneObject->GetSurfaceRect();
  if(clippingRect == surfaceRect)
  {
    // Full rendering case
    // Make clippingRect empty because we're doing full rendering now if the clippingRect is empty.
    // To reduce side effects, keep this logic now.
    clippingRect = Rect<int>();
  }

  // Prefetch programs before we start rendering so reflection is
  // ready, and we can pull exact size of UBO needed (no need to resize during drawing)
  auto totalSizeCPU = 0u;
  auto totalSizeGPU = 0u;

  std::unordered_map<Graphics::Program*, Graphics::ProgramResourceBindingInfo> programUsageCount;

  bool sceneNeedsDepthBuffer   = false;
  bool sceneNeedsStencilBuffer = false;
  for(uint32_t i = 0; i < instructionCount; ++i)
  {
    RenderInstruction& instruction = sceneObject->GetRenderInstructions().At(mImpl->renderBufferIndex, i);

    if((instruction.mFrameBuffer != nullptr && renderToFbo) ||
       (instruction.mFrameBuffer == nullptr && !renderToFbo))
    {
      for(auto j = 0u; j < instruction.RenderListCount(); ++j)
      {
        const auto& renderList = instruction.GetRenderList(j);
        bool        autoDepthTestMode((depthBufferAvailable == Integration::DepthBufferAvailable::TRUE) &&
                               !(renderList->GetSourceLayer()->IsDepthTestDisabled()) &&
                               renderList->HasColorRenderItems());
        bool        usesDepthBuffer   = false;
        bool        usesStencilBuffer = false;
        for(auto k = 0u; k < renderList->Count(); ++k)
        {
          auto& item = renderList->GetItem(k);
          usesStencilBuffer |= item.UsesStencilBuffer();
          if(item.mRenderer && item.mRenderer->NeedsProgram())
          {
            usesDepthBuffer |= item.UsesDepthBuffer(autoDepthTestMode);

            // Prepare and store used programs for further processing
            auto program = item.mRenderer->PrepareProgram(instruction);
            if(program)
            {
              const auto& memoryRequirements = program->GetUniformBlocksMemoryRequirements();

              // collect how many programs we use in this frame
              auto key = &program->GetGraphicsProgram();
              auto it  = programUsageCount.find(key);
              if(it == programUsageCount.end())
              {
                programUsageCount[key] = Graphics::ProgramResourceBindingInfo{.program = key, .count = 1};
              }
              else
              {
                (*it).second.count++;
              }

              totalSizeCPU += memoryRequirements.totalCpuSizeRequired;
              totalSizeGPU += memoryRequirements.totalGpuSizeRequired;
            }
          }
        }
        if(!instruction.mFrameBuffer)
        {
          sceneNeedsDepthBuffer |= usesDepthBuffer;
          sceneNeedsStencilBuffer |= usesStencilBuffer;
        }
      }
    }
  }
  if(!renderToFbo)
  {
    mImpl->graphicsController.EnableDepthStencilBuffer(*sceneObject->GetSurfaceRenderTarget(), sceneNeedsDepthBuffer, sceneNeedsStencilBuffer);
  }
  // Fill resource binding for the command buffer
  std::vector<Graphics::CommandBufferResourceBinding> commandBufferResourceBindings;
  if(!programUsageCount.empty())
  {
    commandBufferResourceBindings.resize(programUsageCount.size());
    auto iter = commandBufferResourceBindings.begin();
    for(auto& item : programUsageCount)
    {
      iter->type           = Graphics::ResourceType::PROGRAM;
      iter->programBinding = &item.second;
      ++iter;
    }
  }

  // Reset main algorithms command buffer
  mImpl->renderAlgorithms.ResetCommandBuffer(commandBufferResourceBindings.empty() ? nullptr : &commandBufferResourceBindings);

  auto mainCommandBuffer = mImpl->renderAlgorithms.GetMainCommandBuffer();

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Render scene (%s), CPU:%d GPU:%d\n", renderToFbo ? "Offscreen" : "Onscreen", totalSizeCPU, totalSizeGPU);

  auto& uboManager = mImpl->uniformBufferManager;

  uboManager->SetCurrentSceneRenderInfo(sceneObject, renderToFbo);
  uboManager->Rollback(sceneObject, renderToFbo);

  // Respec UBOs for this frame (orphan buffers or double buffer in the GPU)
  if(instructionCount)
  {
    uboManager->GetUniformBufferForScene(sceneObject, renderToFbo, true)->ReSpecify(totalSizeCPU);
    uboManager->GetUniformBufferForScene(sceneObject, renderToFbo, false)->ReSpecify(totalSizeGPU);
  }

#if defined(DEBUG_ENABLED)
  auto uniformBuffer1 = uboManager->GetUniformBufferForScene(sceneObject, renderToFbo, true);
  auto uniformBuffer2 = uboManager->GetUniformBufferForScene(sceneObject, renderToFbo, false);
  if(uniformBuffer1)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "CPU buffer: Offset(%d), Cap(%d)\n", uniformBuffer1->GetCurrentOffset(), uniformBuffer1->GetCurrentCapacity());
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "CPU buffer: nil\n");
  }
  if(uniformBuffer2)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "GPU buffer: Offset(%d), Cap(%d)\n", uniformBuffer2->GetCurrentOffset(), uniformBuffer2->GetCurrentCapacity());
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "GPU buffer: nil\n");
  }
#endif

  for(uint32_t i = 0; i < instructionCount; ++i)
  {
    RenderInstruction& instruction = sceneObject->GetRenderInstructions().At(mImpl->renderBufferIndex, i);

    if((renderToFbo && !instruction.mFrameBuffer) || (!renderToFbo && instruction.mFrameBuffer))
    {
      continue; // skip
    }

    // Mark that we will require a post-render step to be performed (includes swap-buffers).
    status.SetNeedsPostRender(true);

    Rect<int32_t> viewportRect;

    int32_t surfaceOrientation = sceneObject->GetSurfaceOrientation() + sceneObject->GetScreenOrientation();
    if(surfaceOrientation >= 360)
    {
      surfaceOrientation -= 360;
    }

    Graphics::RenderTarget*           currentRenderTarget = nullptr;
    Graphics::RenderPass*             currentRenderPass   = nullptr;
    std::vector<Graphics::ClearValue> currentClearValues{};

    if(instruction.mFrameBuffer)
    {
      // Ensure graphics framebuffer is created, bind attachments and create render passes
      // Only happens once per framebuffer. If the create fails, e.g. no attachments yet,
      // then don't render to this framebuffer.
      if(!instruction.mFrameBuffer->GetGraphicsObject())
      {
        const bool created = instruction.mFrameBuffer->CreateGraphicsObjects();
        if(!created)
        {
          continue;
        }
      }

      auto& clearValues = instruction.mFrameBuffer->GetGraphicsRenderPassClearValues();

      // Set the clear color for first color attachment
      if(instruction.mIsClearColorSet && !clearValues.empty())
      {
        clearValues[0].color = {
          instruction.mClearColor.r,
          instruction.mClearColor.g,
          instruction.mClearColor.b,
          instruction.mClearColor.a};
      }

      currentClearValues = clearValues;

      auto loadOp = instruction.mIsClearColorSet ? Graphics::AttachmentLoadOp::CLEAR : Graphics::AttachmentLoadOp::LOAD;

      // offscreen buffer
      currentRenderTarget = instruction.mFrameBuffer->GetGraphicsRenderTarget();
      currentRenderPass   = instruction.mFrameBuffer->GetGraphicsRenderPass(loadOp, Graphics::AttachmentStoreOp::STORE);
    }
    else // no framebuffer
    {
      // surface
      auto& clearValues = sceneObject->GetGraphicsRenderPassClearValues();

      if(instruction.mIsClearColorSet)
      {
        clearValues[0].color = {
          instruction.mClearColor.r,
          instruction.mClearColor.g,
          instruction.mClearColor.b,
          instruction.mClearColor.a};
      }

      currentClearValues = clearValues;

      // @todo SceneObject should already have the depth clear / stencil clear in the clearValues array.
      // if the window has a depth/stencil buffer.
      if((depthBufferAvailable == Integration::DepthBufferAvailable::TRUE ||
          stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE) &&
         (currentClearValues.size() <= 1))
      {
        currentClearValues.emplace_back();
        currentClearValues.back().depthStencil.depth   = 1.0f;
        currentClearValues.back().depthStencil.stencil = 0;
      }

      auto loadOp = instruction.mIsClearColorSet ? Graphics::AttachmentLoadOp::CLEAR : Graphics::AttachmentLoadOp::LOAD;

      currentRenderTarget = sceneObject->GetSurfaceRenderTarget();
      currentRenderPass   = sceneObject->GetGraphicsRenderPass(loadOp, Graphics::AttachmentStoreOp::STORE);
    }

    targetsToPresent.emplace_back(currentRenderTarget);

    if(!instruction.mIgnoreRenderToFbo && (instruction.mFrameBuffer != nullptr))
    {
      // Offscreen buffer rendering
      if(instruction.mIsViewportSet)
      {
        // For Viewport the lower-left corner is (0,0)
        const int32_t y = (instruction.mFrameBuffer->GetHeight() - instruction.mViewport.height) - instruction.mViewport.y;
        viewportRect.Set(instruction.mViewport.x, y, instruction.mViewport.width, instruction.mViewport.height);
      }
      else
      {
        viewportRect.Set(0, 0, instruction.mFrameBuffer->GetWidth(), instruction.mFrameBuffer->GetHeight());
      }
      surfaceOrientation = 0;
    }
    else // No Offscreen frame buffer rendering
    {
      // Check whether a viewport is specified, otherwise the full surface size is used
      if(instruction.mIsViewportSet)
      {
        // For Viewport the lower-left corner is (0,0)
        const int32_t y = (surfaceRect.height - instruction.mViewport.height) - instruction.mViewport.y;
        viewportRect.Set(instruction.mViewport.x, y, instruction.mViewport.width, instruction.mViewport.height);
      }
      else
      {
        viewportRect = surfaceRect;
      }
    }

    // Set surface orientation
    // @todo Inform graphics impl by another route.
    // was: mImpl->currentContext->SetSurfaceOrientation(surfaceOrientation);

    /*** Clear region of framebuffer or surface before drawing ***/
    bool clearFullFrameRect = (surfaceRect == viewportRect);
    if(instruction.mFrameBuffer != nullptr)
    {
      Viewport frameRect(0, 0, instruction.mFrameBuffer->GetWidth(), instruction.mFrameBuffer->GetHeight());
      clearFullFrameRect = (frameRect == viewportRect);
    }

    if(!clippingRect.IsEmpty())
    {
      if(!clippingRect.Intersect(viewportRect))
      {
        DALI_LOG_ERROR("Invalid clipping rect %d %d %d %d\n", clippingRect.x, clippingRect.y, clippingRect.width, clippingRect.height);
        clippingRect = Rect<int>();
      }
      clearFullFrameRect = false;
    }

    Graphics::Rect2D scissorArea{viewportRect.x, viewportRect.y, uint32_t(viewportRect.width), uint32_t(viewportRect.height)};
    if(instruction.mIsClearColorSet)
    {
      if(!clearFullFrameRect)
      {
        if(!clippingRect.IsEmpty())
        {
          scissorArea = {clippingRect.x, clippingRect.y, uint32_t(clippingRect.width), uint32_t(clippingRect.height)};
        }
      }
    }

    // Scissor's value should be set based on the default system coordinates.
    // When the surface is rotated, the input values already were set with the rotated angle.
    // So, re-calculation is needed.
    scissorArea = RecalculateScissorArea(scissorArea, surfaceOrientation, surfaceRect);

    // Begin render pass
    mainCommandBuffer->BeginRenderPass(
      currentRenderPass,
      currentRenderTarget,
      scissorArea,
      currentClearValues);

    // Note, don't set the viewport/scissor on the primary command buffer.

    mImpl->renderAlgorithms.ProcessRenderInstruction(
      instruction,
      mImpl->renderBufferIndex,
      depthBufferAvailable,
      stencilBufferAvailable,
      viewportRect,
      clippingRect,
      surfaceOrientation,
      Uint16Pair(surfaceRect.width, surfaceRect.height),
      currentRenderPass,
      currentRenderTarget);

    Graphics::SyncObject* syncObject{nullptr};
    // If the render instruction has an associated render tracker (owned separately)
    // and framebuffer, create a one shot sync object, and use it to determine when
    // the render pass has finished executing on GPU.
    if(instruction.mRenderTracker && instruction.mFrameBuffer)
    {
      syncObject                 = instruction.mRenderTracker->CreateSyncObject(mImpl->graphicsController);
      instruction.mRenderTracker = nullptr;
    }
    mainCommandBuffer->EndRenderPass(syncObject);
  }

  if(targetsToPresent.size() > 0u)
  {
    DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_RENDER_FINISHED", [&](std::ostringstream& oss) { oss << "[" << targetsToPresent.size() << "]"; });
  }

  // Flush UBOs
  mImpl->uniformBufferManager->Flush(sceneObject, renderToFbo);
  mImpl->renderAlgorithms.SubmitCommandBuffer();
  mImpl->commandBufferSubmitted = true;

  if(targetsToPresent.size() > 0u)
  {
    std::sort(targetsToPresent.begin(), targetsToPresent.end());

    Graphics::RenderTarget* rt = nullptr;
    for(auto& target : targetsToPresent)
    {
      if(target != rt)
      {
        mImpl->graphicsController.PresentRenderTarget(target);
        rt = target;
      }
    }

    DALI_TRACE_END(gTraceFilter, "DALI_RENDER_FINISHED");
  }
}

void RenderManager::PostRender()
{
  if(!mImpl->commandBufferSubmitted)
  {
    // Rendering is skipped but there may be pending tasks. Flush them.
    Graphics::SubmitInfo submitInfo;
    submitInfo.cmdBuffer.clear(); // Only flush
    submitInfo.flags = 0 | Graphics::SubmitFlagBits::FLUSH;
    mImpl->graphicsController.SubmitCommandBuffers(submitInfo);

    mImpl->commandBufferSubmitted = true;
  }

  // Notify RenderGeometries that rendering has finished
  for(auto&& iter : mImpl->geometryContainer)
  {
    iter->OnRenderFinished();
  }

  // Notify updated RenderTexture that rendering has finished
  // Note : updatedTextures could be added during OnRenderFinished
  while(!mImpl->updatedTextures.Empty())
  {
    auto updatedTextures = std::move(mImpl->updatedTextures);
    mImpl->updatedTextures.Clear();
    for(auto&& iter : updatedTextures)
    {
      iter->OnRenderFinished();
    }
  }

  // Remove discarded textures after OnRenderFinished called
  mImpl->textureDiscardQueue.Clear();

  mImpl->UpdateTrackers();

  uint32_t count = 0u;
  for(auto& scene : mImpl->sceneContainer)
  {
    count += scene->GetRenderInstructions().Count(mImpl->renderBufferIndex);
  }

  mImpl->ClearUnusedProgramCacheIfNeed();

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  // Shrink relevant containers if required.
  if(mImpl->containerRemovedFlags & ContainerRemovedFlagBits::RENDERER)
  {
    mImpl->rendererContainer.ShrinkToFitIfNeeded();
  }
  if(mImpl->containerRemovedFlags & ContainerRemovedFlagBits::TEXTURE)
  {
    mImpl->textureContainer.ShrinkToFitIfNeeded();
  }

  // Both containers always have empty slots, so we can shrink them.
  // Note that we don't need to release it every frames. So just check every specific frames
  if((mImpl->frameCount & (SHRINK_TO_FIT_FRAME_COUNT - 1)) == 0)
  {
    mImpl->updatedTextures.ShrinkToFit();
    mImpl->textureDiscardQueue.ShrinkToFit();
  }

  // Reset flag
  mImpl->containerRemovedFlags = ContainerRemovedFlagBits::NOTHING;
#endif

  const bool haveInstructions = count > 0u;

  // If this frame was rendered due to instructions existing, we mark this so we know to clear the next frame.
  mImpl->lastFrameWasRendered = haveInstructions;

  /**
   * The rendering has finished; swap to the next buffer.
   * Ideally the update has just finished using this buffer; otherwise the render thread
   * should block until the update has finished.
   */
  mImpl->renderBufferIndex = (0 != mImpl->renderBufferIndex) ? 0 : 1;

  DALI_PRINT_RENDER_END();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
