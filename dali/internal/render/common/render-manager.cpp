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
 *
 */

// CLASS HEADER
#include <dali/internal/render/common/render-manager.h>

// EXTERNAL INCLUDES
#include <memory.h>

// INTERNAL INCLUDES
#include <dali/devel-api/threading/thread-pool.h>
#include <dali/integration-api/core.h>
#include <dali/internal/common/ordered-set.h>

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
#include <dali/internal/render/renderers/shader-cache.h>
#include <dali/internal/render/renderers/uniform-buffer-manager.h>
#include <dali/internal/render/renderers/uniform-buffer-view-pool.h>
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
inline Graphics::Rect2D RecalculateScissorArea(Graphics::Rect2D scissorArea, int orientation, Rect<int32_t> viewportRect)
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
    shaderCache(graphicsController),
    depthBufferAvailable(depthBufferAvailableParam),
    stencilBufferAvailable(stencilBufferAvailableParam),
    partialUpdateAvailable(partialUpdateAvailableParam)
  {
    // Create thread pool with just one thread ( there may be a need to create more threads in the future ).
    threadPool = std::make_unique<Dali::ThreadPool>();
    threadPool->Initialize(1u);

    uniformBufferManager = std::make_unique<Render::UniformBufferManager>(&graphicsController);
    pipelineCache        = std::make_unique<Render::PipelineCache>(graphicsController);
  }

  ~Impl()
  {
    threadPool.reset(nullptr); // reset now to maintain correct destruction order
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

  // the order is important for destruction,
  Graphics::Controller&           graphicsController;
  RenderQueue                     renderQueue;      ///< A message queue for receiving messages from the update-thread.
  std::vector<SceneGraph::Scene*> sceneContainer;   ///< List of pointers to the scene graph objects of the scenes
  Render::RenderAlgorithms        renderAlgorithms; ///< The RenderAlgorithms object is used to action the renders required by a RenderInstruction

  OrderedSet<Render::Sampler>         samplerContainer;      ///< List of owned samplers
  OrderedSet<Render::FrameBuffer>     frameBufferContainer;  ///< List of owned framebuffers
  OrderedSet<Render::VertexBuffer>    vertexBufferContainer; ///< List of owned vertex buffers
  OrderedSet<Render::Geometry>        geometryContainer;     ///< List of owned Geometries
  OwnerKeyContainer<Render::Renderer> rendererContainer;     ///< List of owned renderers
  OwnerKeyContainer<Render::Texture>  textureContainer;      ///< List of owned textures

  OrderedSet<Render::RenderTracker> mRenderTrackers; ///< List of owned render trackers

  ProgramController   programController; ///< Owner of the programs
  Render::ShaderCache shaderCache;       ///< The cache for the graphics shaders

  std::unique_ptr<Render::UniformBufferManager> uniformBufferManager; ///< The uniform buffer manager
  std::unique_ptr<Render::PipelineCache>        pipelineCache;

  Integration::DepthBufferAvailable   depthBufferAvailable;   ///< Whether the depth buffer is available
  Integration::StencilBufferAvailable stencilBufferAvailable; ///< Whether the stencil buffer is available
  Integration::PartialUpdateAvailable partialUpdateAvailable; ///< Whether the partial update is available

  std::unique_ptr<Dali::ThreadPool> threadPool;            ///< The thread pool
  Vector<Graphics::Texture*>        boundTextures;         ///< The textures bound for rendering
  Vector<Graphics::Texture*>        textureDependencyList; ///< The dependency list of bound textures
  Vector<Render::TextureKey>        updatedTextures{};     ///< The updated texture list

  uint32_t    frameCount{0u};                                                    ///< The current frame count
  BufferIndex renderBufferIndex{SceneGraphBuffers::INITIAL_UPDATE_BUFFER_INDEX}; ///< The index of the buffer to read from; this is opposite of the "update" buffer

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
  renderer->Initialize(mImpl->graphicsController, mImpl->programController, mImpl->shaderCache, *(mImpl->uniformBufferManager.get()), *(mImpl->pipelineCache.get()));

  mImpl->rendererContainer.PushBack(renderer);
}

void RenderManager::RemoveRenderer(const Render::RendererKey& renderer)
{
  mImpl->rendererContainer.EraseKey(renderer);
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

  textureKey->Initialize(mImpl->graphicsController);
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
    textureKey->Destroy();
    mImpl->textureContainer.Erase(iter); // Texture found; now destroy it
  }
}

void RenderManager::UploadTexture(const Render::TextureKey& textureKey, PixelDataPtr pixelData, const Texture::UploadParams& params)
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
}

void RenderManager::UninitializeScene(SceneGraph::Scene* scene)
{
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

  // Rollback
  mImpl->uniformBufferManager->GetUniformBufferViewPool(mImpl->renderBufferIndex)->Rollback();

  // Increment the frame count at the beginning of each frame
  ++mImpl->frameCount;

  // Process messages queued during previous update
  mImpl->renderQueue.ProcessMessages(mImpl->renderBufferIndex);

  uint32_t count = 0u;
  for(auto& i : mImpl->sceneContainer)
  {
    count += i->GetRenderInstructions().Count(mImpl->renderBufferIndex);
  }

  const bool haveInstructions = count > 0u;

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

  // Clean latest used pipeline
  mImpl->pipelineCache->CleanLatestUsedCache();

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

  // Mark previous dirty rects in the std::unordered_map.
  Scene::ItemsDirtyRectsContainer& itemsDirtyRects = sceneObject->GetItemsDirtyRects();
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
                  (item.mNode->Updated() || (item.mRenderer && item.mRenderer->Updated(mImpl->renderBufferIndex)))))
              {
                item.mIsUpdated = false;

                Vector4 updateArea = item.mRenderer ? item.mRenderer->GetVisualTransformedUpdateArea(mImpl->renderBufferIndex, item.mUpdateArea) : item.mUpdateArea;

                rect = RenderItem::CalculateViewportSpaceAABB(item.mModelViewMatrix, Vector3(updateArea.x, updateArea.y, 0.0f), Vector3(updateArea.z, updateArea.w, 0.0f), viewportRect.width, viewportRect.height);
                if(rect.IsValid() && rect.Intersect(viewportRect) && !rect.IsEmpty())
                {
                  const int left   = rect.x;
                  const int top    = rect.y;
                  const int right  = rect.x + rect.width;
                  const int bottom = rect.y + rect.height;
                  rect.x           = (left / 16) * 16;
                  rect.y           = (top / 16) * 16;
                  rect.width       = ((right + 16) / 16) * 16 - rect.x;
                  rect.height      = ((bottom + 16) / 16) * 16 - rect.y;

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
                  // The item is not in the list for some reason. Add it!
                  itemsDirtyRects.insert({dirtyRectKey, surfaceRect});
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
  // Note, std::unordered_map end iterator is validate if we call erase.
  for(auto iter = itemsDirtyRects.cbegin(), iterEnd = itemsDirtyRects.cend(); iter != iterEnd;)
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
    return;
  }

  Rect<int> clippingRect = sceneObject->GetSurfaceRect();
  RenderScene(status, scene, renderToFbo, clippingRect);
}

void RenderManager::RenderScene(Integration::RenderStatus& status, Integration::Scene& scene, bool renderToFbo, Rect<int>& clippingRect)
{
  if(mImpl->partialUpdateAvailable == Integration::PartialUpdateAvailable::TRUE && !renderToFbo && clippingRect.IsEmpty())
  {
    // ClippingRect is empty. Skip rendering
    return;
  }

  // Reset main algorithms command buffer
  mImpl->renderAlgorithms.ResetCommandBuffer();

  auto mainCommandBuffer = mImpl->renderAlgorithms.GetMainCommandBuffer();

  Internal::Scene&   sceneInternal = GetImplementation(scene);
  SceneGraph::Scene* sceneObject   = sceneInternal.GetSceneObject();
  if(!sceneObject)
  {
    return;
  }

  uint32_t count = sceneObject->GetRenderInstructions().Count(mImpl->renderBufferIndex);

  std::vector<Graphics::RenderTarget*> targetstoPresent;

  Rect<int32_t> surfaceRect = sceneObject->GetSurfaceRect();
  if(clippingRect == surfaceRect)
  {
    // Full rendering case
    // Make clippingRect empty because we're doing full rendering now if the clippingRect is empty.
    // To reduce side effects, keep this logic now.
    clippingRect = Rect<int>();
  }

  // Prepare to lock and map standalone uniform buffer.
  mImpl->uniformBufferManager->ReadyToLockUniformBuffer(mImpl->renderBufferIndex);

  for(uint32_t i = 0; i < count; ++i)
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

    // @todo Should these be part of scene?
    Integration::DepthBufferAvailable   depthBufferAvailable   = mImpl->depthBufferAvailable;
    Integration::StencilBufferAvailable stencilBufferAvailable = mImpl->stencilBufferAvailable;

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
        currentClearValues.back().depthStencil.depth   = 0;
        currentClearValues.back().depthStencil.stencil = 0;
      }

      auto loadOp = instruction.mIsClearColorSet ? Graphics::AttachmentLoadOp::CLEAR : Graphics::AttachmentLoadOp::LOAD;

      currentRenderTarget = sceneObject->GetSurfaceRenderTarget();
      currentRenderPass   = sceneObject->GetGraphicsRenderPass(loadOp, Graphics::AttachmentStoreOp::STORE);
    }

    targetstoPresent.emplace_back(currentRenderTarget);

    // reset the program matrices for all programs once per frame
    // this ensures we will set view and projection matrix once per program per camera
    mImpl->programController.ResetProgramMatrices();

    if(instruction.mFrameBuffer)
    {
      // For each offscreen buffer, update the dependency list with the new texture id used by this frame buffer.
      for(unsigned int i0 = 0, i1 = instruction.mFrameBuffer->GetColorAttachmentCount(); i0 < i1; ++i0)
      {
        mImpl->textureDependencyList.PushBack(instruction.mFrameBuffer->GetTexture(i0));
      }
    }

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

    mainCommandBuffer->SetViewport({float(viewportRect.x),
                                    float(viewportRect.y),
                                    float(viewportRect.width),
                                    float(viewportRect.height)});

    // Clear the list of bound textures
    mImpl->boundTextures.Clear();

    mImpl->renderAlgorithms.ProcessRenderInstruction(
      instruction,
      mImpl->renderBufferIndex,
      depthBufferAvailable,
      stencilBufferAvailable,
      mImpl->boundTextures,
      viewportRect,
      clippingRect,
      surfaceOrientation,
      Uint16Pair(surfaceRect.width, surfaceRect.height));

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

  // Unlock standalone uniform buffer.
  mImpl->uniformBufferManager->UnlockUniformBuffer(mImpl->renderBufferIndex);

  mImpl->renderAlgorithms.SubmitCommandBuffer();
  mImpl->commandBufferSubmitted = true;

  std::sort(targetstoPresent.begin(), targetstoPresent.end());

  Graphics::RenderTarget* rt = nullptr;
  for(auto& target : targetstoPresent)
  {
    if(target != rt)
    {
      mImpl->graphicsController.PresentRenderTarget(target);
      rt = target;
    }
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
  for(auto&& iter : mImpl->updatedTextures)
  {
    iter->OnRenderFinished();
  }
  mImpl->updatedTextures.Clear();

  mImpl->UpdateTrackers();

  uint32_t count = 0u;
  for(auto& scene : mImpl->sceneContainer)
  {
    count += scene->GetRenderInstructions().Count(mImpl->renderBufferIndex);
  }

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
