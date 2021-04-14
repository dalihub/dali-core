/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/gl-context-helper-abstraction.h>

#include <dali/internal/event/common/scene-impl.h>

#include <dali/internal/update/common/scene-graph-scene.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>

#include <dali/internal/render/common/render-algorithms.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-frame-buffer.h>
#include <dali/internal/render/renderers/render-texture.h>
#include <dali/internal/render/renderers/shader-cache.h>
#include <dali/internal/render/renderers/uniform-buffer-manager.h>
#include <dali/internal/render/shaders/program-controller.h>

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

/**
 * Structure to contain internal data
 */
struct RenderManager::Impl
{
  Impl(Graphics::Controller&               graphicsController,
       Integration::DepthBufferAvailable   depthBufferAvailableParam,
       Integration::StencilBufferAvailable stencilBufferAvailableParam,
       Integration::PartialUpdateAvailable partialUpdateAvailableParam)
  : context(graphicsController.GetGlAbstraction(), &sceneContextContainer),
    currentContext(&context),
    graphicsController(graphicsController),
    renderQueue(),
    renderAlgorithms(graphicsController),
    frameCount(0u),
    renderBufferIndex(SceneGraphBuffers::INITIAL_UPDATE_BUFFER_INDEX),
    rendererContainer(),
    samplerContainer(),
    textureContainer(),
    frameBufferContainer(),
    lastFrameWasRendered(false),
    programController(graphicsController),
    shaderCache(graphicsController),
    depthBufferAvailable(depthBufferAvailableParam),
    stencilBufferAvailable(stencilBufferAvailableParam),
    partialUpdateAvailable(partialUpdateAvailableParam)
  {
    // Create thread pool with just one thread ( there may be a need to create more threads in the future ).
    threadPool = std::unique_ptr<Dali::ThreadPool>(new Dali::ThreadPool());
    threadPool->Initialize(1u);

    uniformBufferManager.reset(new Render::UniformBufferManager(&graphicsController));
  }

  ~Impl()
  {
    threadPool.reset(nullptr); // reset now to maintain correct destruction order
  }

  void AddRenderTracker(Render::RenderTracker* renderTracker)
  {
    DALI_ASSERT_DEBUG(renderTracker != NULL);
    mRenderTrackers.PushBack(renderTracker);
  }

  void RemoveRenderTracker(Render::RenderTracker* renderTracker)
  {
    mRenderTrackers.EraseObject(renderTracker);
  }

  Context* CreateSceneContext()
  {
    Context* context = new Context(graphicsController.GetGlAbstraction());
    sceneContextContainer.PushBack(context);
    return context;
  }

  void DestroySceneContext(Context* sceneContext)
  {
    auto iter = std::find(sceneContextContainer.Begin(), sceneContextContainer.End(), sceneContext);
    if(iter != sceneContextContainer.End())
    {
      (*iter)->GlContextDestroyed();
      sceneContextContainer.Erase(iter);
    }
  }

  Context* ReplaceSceneContext(Context* oldSceneContext)
  {
    Context* newContext = new Context(graphicsController.GetGlAbstraction());

    oldSceneContext->GlContextDestroyed();

    std::replace(sceneContextContainer.begin(), sceneContextContainer.end(), oldSceneContext, newContext);
    return newContext;
  }

  void UpdateTrackers()
  {
    for(auto&& iter : mRenderTrackers)
    {
      iter->PollSyncObject();
    }
  }

  // the order is important for destruction,
  // programs are owned by context at the moment.
  Context                  context;               ///< Holds the GL state of the share resource context
  Context*                 currentContext;        ///< Holds the GL state of the current context for rendering
  OwnerContainer<Context*> sceneContextContainer; ///< List of owned contexts holding the GL state per scene
  Graphics::Controller&    graphicsController;
  RenderQueue              renderQueue; ///< A message queue for receiving messages from the update-thread.

  std::vector<SceneGraph::Scene*> sceneContainer; ///< List of pointers to the scene graph objects of the scenes

  Render::RenderAlgorithms renderAlgorithms; ///< The RenderAlgorithms object is used to action the renders required by a RenderInstruction

  uint32_t    frameCount;        ///< The current frame count
  BufferIndex renderBufferIndex; ///< The index of the buffer to read from; this is opposite of the "update" buffer

  OwnerContainer<Render::Renderer*>     rendererContainer;     ///< List of owned renderers
  OwnerContainer<Render::Sampler*>      samplerContainer;      ///< List of owned samplers
  OwnerContainer<Render::Texture*>      textureContainer;      ///< List of owned textures
  OwnerContainer<Render::FrameBuffer*>  frameBufferContainer;  ///< List of owned framebuffers
  OwnerContainer<Render::VertexBuffer*> vertexBufferContainer; ///< List of owned vertex buffers
  OwnerContainer<Render::Geometry*>     geometryContainer;     ///< List of owned Geometries

  bool lastFrameWasRendered; ///< Keeps track of the last frame being rendered due to having render instructions

  OwnerContainer<Render::RenderTracker*> mRenderTrackers; ///< List of render trackers

  ProgramController   programController; ///< Owner of the GL programs
  Render::ShaderCache shaderCache;       ///< The cache for the graphics shaders

  std::unique_ptr<Render::UniformBufferManager> uniformBufferManager; ///< The uniform buffer manager

  Integration::DepthBufferAvailable   depthBufferAvailable;   ///< Whether the depth buffer is available
  Integration::StencilBufferAvailable stencilBufferAvailable; ///< Whether the stencil buffer is available
  Integration::PartialUpdateAvailable partialUpdateAvailable; ///< Whether the partial update is available

  std::unique_ptr<Dali::ThreadPool> threadPool;            ///< The thread pool
  Vector<Graphics::Texture*>        boundTextures;         ///< The textures bound for rendering
  Vector<Graphics::Texture*>        textureDependencyList; ///< The dependency list of bound textures
};

RenderManager* RenderManager::New(Graphics::Controller&               graphicsController,
                                  Integration::DepthBufferAvailable   depthBufferAvailable,
                                  Integration::StencilBufferAvailable stencilBufferAvailable,
                                  Integration::PartialUpdateAvailable partialUpdateAvailable)
{
  RenderManager* manager = new RenderManager;
  manager->mImpl         = new Impl(graphicsController,
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

void RenderManager::ContextCreated()
{
  mImpl->context.GlContextCreated();
  mImpl->programController.GlContextCreated();

  // renderers, textures and gpu buffers cannot reinitialize themselves
  // so they rely on someone reloading the data for them
}

void RenderManager::ContextDestroyed()
{
  mImpl->context.GlContextDestroyed();
  mImpl->programController.GlContextDestroyed();

  //Inform textures
  for(auto&& texture : mImpl->textureContainer)
  {
    texture->Destroy();
  }

  //Inform framebuffers
  for(auto&& framebuffer : mImpl->frameBufferContainer)
  {
    framebuffer->Destroy();
  }

  // inform context
  for(auto&& context : mImpl->sceneContextContainer)
  {
    context->GlContextDestroyed();
  }
}

void RenderManager::SetShaderSaver(ShaderSaver& upstream)
{
  mImpl->programController.SetShaderSaver(upstream);
}

void RenderManager::AddRenderer(OwnerPointer<Render::Renderer>& renderer)
{
  // Initialize the renderer as we are now in render thread
  renderer->Initialize(mImpl->context, mImpl->graphicsController, mImpl->programController, mImpl->shaderCache, *(mImpl->uniformBufferManager.get()));

  mImpl->rendererContainer.PushBack(renderer.Release());
}

void RenderManager::RemoveRenderer(Render::Renderer* renderer)
{
  mImpl->rendererContainer.EraseObject(renderer);
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

void RenderManager::AddTexture(OwnerPointer<Render::Texture>& texture)
{
  texture->Initialize(mImpl->graphicsController);
  mImpl->textureContainer.PushBack(texture.Release());
}

void RenderManager::RemoveTexture(Render::Texture* texture)
{
  DALI_ASSERT_DEBUG(NULL != texture);

  // Find the texture, use reference to pointer so we can do the erase safely
  for(auto&& iter : mImpl->textureContainer)
  {
    if(iter == texture)
    {
      texture->Destroy();
      mImpl->textureContainer.Erase(&iter); // Texture found; now destroy it
      return;
    }
  }
}

void RenderManager::UploadTexture(Render::Texture* texture, PixelDataPtr pixelData, const Texture::UploadParams& params)
{
  texture->Upload(pixelData, params);
}

void RenderManager::GenerateMipmaps(Render::Texture* texture)
{
  texture->GenerateMipmaps();
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
  DALI_ASSERT_DEBUG(NULL != frameBuffer);

  // Find the sampler, use reference so we can safely do the erase
  for(auto&& iter : mImpl->frameBufferContainer)
  {
    if(iter == frameBuffer)
    {
      frameBuffer->Destroy();
      mImpl->frameBufferContainer.Erase(&iter); // frameBuffer found; now destroy it

      break;
    }
  }
}

void RenderManager::InitializeScene(SceneGraph::Scene* scene)
{
  scene->Initialize(*mImpl->CreateSceneContext(), mImpl->graphicsController);
  mImpl->sceneContainer.push_back(scene);
}

void RenderManager::UninitializeScene(SceneGraph::Scene* scene)
{
  mImpl->DestroySceneContext(scene->GetContext());

  auto iter = std::find(mImpl->sceneContainer.begin(), mImpl->sceneContainer.end(), scene);
  if(iter != mImpl->sceneContainer.end())
  {
    mImpl->sceneContainer.erase(iter);
  }
}

void RenderManager::SurfaceReplaced(SceneGraph::Scene* scene)
{
  Context* newContext = mImpl->ReplaceSceneContext(scene->GetContext());
  scene->Initialize(*newContext, mImpl->graphicsController);
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

void RenderManager::SetIndexBuffer(Render::Geometry* geometry, Dali::Vector<uint16_t>& indices)
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
  DALI_ASSERT_DEBUG(NULL != geometry);

  // Find the geometry
  for(auto&& iter : mImpl->geometryContainer)
  {
    if(iter == geometry)
    {
      iter->AddVertexBuffer(vertexBuffer);
      break;
    }
  }
}

void RenderManager::RemoveVertexBuffer(Render::Geometry* geometry, Render::VertexBuffer* vertexBuffer)
{
  DALI_ASSERT_DEBUG(NULL != geometry);

  // Find the geometry
  for(auto&& iter : mImpl->geometryContainer)
  {
    if(iter == geometry)
    {
      iter->RemoveVertexBuffer(vertexBuffer);
      break;
    }
  }
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

ProgramCache* RenderManager::GetProgramCache()
{
  return &(mImpl->programController);
}

void RenderManager::PreRender(Integration::RenderStatus& status, bool forceClear, bool uploadOnly)
{
  DALI_PRINT_RENDER_START(mImpl->renderBufferIndex);

  // Core::Render documents that GL context must be current before calling Render
  DALI_ASSERT_DEBUG(mImpl->context.IsGlContextCreated());

  // Increment the frame count at the beginning of each frame
  ++mImpl->frameCount;

  // Process messages queued during previous update
  mImpl->renderQueue.ProcessMessages(mImpl->renderBufferIndex);

  uint32_t count = 0u;
  for(uint32_t i = 0; i < mImpl->sceneContainer.size(); ++i)
  {
    count += mImpl->sceneContainer[i]->GetRenderInstructions().Count(mImpl->renderBufferIndex);
  }

  const bool haveInstructions = count > 0u;

  DALI_LOG_INFO(gLogFilter, Debug::General, "Render: haveInstructions(%s) || mImpl->lastFrameWasRendered(%s) || forceClear(%s)\n", haveInstructions ? "true" : "false", mImpl->lastFrameWasRendered ? "true" : "false", forceClear ? "true" : "false");

  // Only render if we have instructions to render, or the last frame was rendered (and therefore a clear is required).
  if(haveInstructions || mImpl->lastFrameWasRendered || forceClear)
  {
    DALI_LOG_INFO(gLogFilter, Debug::General, "Render: Processing\n");

    // Switch to the shared context
    if(mImpl->currentContext != &mImpl->context)
    {
      mImpl->currentContext = &mImpl->context;

      // Clear the current cached program when the context is switched
      mImpl->programController.ClearCurrentProgram();
    }

    // Upload the geometries
    for(uint32_t i = 0; i < mImpl->sceneContainer.size(); ++i)
    {
      RenderInstructionContainer& instructions = mImpl->sceneContainer[i]->GetRenderInstructions();
      for(uint32_t j = 0; j < instructions.Count(mImpl->renderBufferIndex); ++j)
      {
        RenderInstruction& instruction = instructions.At(mImpl->renderBufferIndex, j);

        const Matrix* viewMatrix       = instruction.GetViewMatrix(mImpl->renderBufferIndex);
        const Matrix* projectionMatrix = instruction.GetProjectionMatrix(mImpl->renderBufferIndex);

        DALI_ASSERT_DEBUG(viewMatrix);
        DALI_ASSERT_DEBUG(projectionMatrix);

        if(viewMatrix && projectionMatrix)
        {
          const RenderListContainer::SizeType renderListCount = instruction.RenderListCount();

          // Iterate through each render list.
          for(RenderListContainer::SizeType index = 0; index < renderListCount; ++index)
          {
            const RenderList* renderList = instruction.GetRenderList(index);

            if(renderList && !renderList->IsEmpty())
            {
              const std::size_t itemCount = renderList->Count();
              for(uint32_t itemIndex = 0u; itemIndex < itemCount; ++itemIndex)
              {
                const RenderItem& item = renderList->GetItem(itemIndex);
                if(DALI_LIKELY(item.mRenderer))
                {
                  item.mRenderer->Upload();
                }
              }
            }
          }
        }
      }
    }
  }
}

void RenderManager::PreRender(Integration::Scene& scene, std::vector<Rect<int>>& damagedRects)
{
  if(mImpl->partialUpdateAvailable != Integration::PartialUpdateAvailable::TRUE)
  {
    return;
  }

  Internal::Scene&   sceneInternal = GetImplementation(scene);
  SceneGraph::Scene* sceneObject   = sceneInternal.GetSceneObject();

  if(sceneObject->IsRenderingSkipped())
  {
    // We don't need to calculate dirty rects
    return;
  }

  class DamagedRectsCleaner
  {
  public:
    DamagedRectsCleaner(std::vector<Rect<int>>& damagedRects)
    : mDamagedRects(damagedRects),
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
      }
    }

  private:
    std::vector<Rect<int>>& mDamagedRects;
    bool                    mCleanOnReturn;
  };

  Rect<int32_t> surfaceRect = sceneObject->GetSurfaceRect();

  // Clean collected dirty/damaged rects on exit if 3d layer or 3d node or other conditions.
  DamagedRectsCleaner damagedRectCleaner(damagedRects);

  // Mark previous dirty rects in the sorted array. The array is already sorted by node and renderer, frame number.
  // so you don't need to sort: std::stable_sort(itemsDirtyRects.begin(), itemsDirtyRects.end());
  std::vector<DirtyRect>& itemsDirtyRects = sceneInternal.GetItemsDirtyRects();
  for(DirtyRect& dirtyRect : itemsDirtyRects)
  {
    dirtyRect.visited = false;
  }

  uint32_t count = sceneObject->GetRenderInstructions().Count(mImpl->renderBufferIndex);
  for(uint32_t i = 0; i < count; ++i)
  {
    RenderInstruction& instruction = sceneObject->GetRenderInstructions().At(mImpl->renderBufferIndex, i);

    if(instruction.mFrameBuffer)
    {
      return; // TODO: reset, we don't deal with render tasks with framebuffers (for now)
    }

    const Camera* camera = instruction.GetCamera();
    if(camera->mType == Camera::DEFAULT_TYPE && camera->mTargetPosition == Camera::DEFAULT_TARGET_POSITION)
    {
      const Node* node = instruction.GetCamera()->GetNode();
      if(node)
      {
        Vector3    position;
        Vector3    scale;
        Quaternion orientation;
        node->GetWorldMatrix(mImpl->renderBufferIndex).GetTransformComponents(position, orientation, scale);

        Vector3 orientationAxis;
        Radian  orientationAngle;
        orientation.ToAxisAngle(orientationAxis, orientationAngle);

        if(position.x > Math::MACHINE_EPSILON_10000 ||
           position.y > Math::MACHINE_EPSILON_10000 ||
           orientationAxis != Vector3(0.0f, 1.0f, 0.0f) ||
           orientationAngle != ANGLE_180 ||
           scale != Vector3(1.0f, 1.0f, 1.0f))
        {
          return;
        }
      }
    }
    else
    {
      return;
    }

    Rect<int32_t> viewportRect;
    if(instruction.mIsViewportSet)
    {
      const int32_t y = (surfaceRect.height - instruction.mViewport.height) - instruction.mViewport.y;
      viewportRect.Set(instruction.mViewport.x, y, instruction.mViewport.width, instruction.mViewport.height);
      if(viewportRect.IsEmpty() || !viewportRect.IsValid())
      {
        return; // just skip funny use cases for now, empty viewport means it is set somewhere else
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
        if(renderList && !renderList->IsEmpty())
        {
          const std::size_t count = renderList->Count();
          for(uint32_t index = 0u; index < count; ++index)
          {
            RenderItem& item = renderList->GetItem(index);
            // If the item does 3D transformation, do early exit and clean the damaged rect array
            if(item.mUpdateSize == Vector3::ZERO)
            {
              return;
            }

            Rect<int> rect;
            DirtyRect dirtyRect(item.mNode, item.mRenderer, mImpl->frameCount, rect);
            // If the item refers to updated node or renderer.
            if(item.mIsUpdated ||
               (item.mNode &&
                (item.mNode->Updated() || (item.mRenderer && item.mRenderer->Updated(mImpl->renderBufferIndex, item.mNode)))))
            {
              item.mIsUpdated = false;
              item.mNode->SetUpdated(false);

              rect = item.CalculateViewportSpaceAABB(item.mUpdateSize, viewportRect.width, viewportRect.height);
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
                // 1. Insert it in the sorted array of the dirty rects.
                // 2. Mark the related dirty rects as visited so they will not be removed below.
                // 3. Keep only last 3 dirty rects for the same node and renderer (Tizen uses 3 back buffers, Ubuntu 1).
                dirtyRect.rect    = rect;
                auto dirtyRectPos = std::lower_bound(itemsDirtyRects.begin(), itemsDirtyRects.end(), dirtyRect);
                dirtyRectPos      = itemsDirtyRects.insert(dirtyRectPos, dirtyRect);

                int c = 1;
                while(++dirtyRectPos != itemsDirtyRects.end())
                {
                  if(dirtyRectPos->node != item.mNode || dirtyRectPos->renderer != item.mRenderer)
                  {
                    break;
                  }

                  dirtyRectPos->visited = true;
                  Rect<int>& dirtRect   = dirtyRectPos->rect;
                  rect.Merge(dirtRect);

                  c++;
                  if(c > 3) // no more then 3 previous rects
                  {
                    itemsDirtyRects.erase(dirtyRectPos);
                    break;
                  }
                }

                damagedRects.push_back(rect);
              }
            }
            else
            {
              // 1. The item is not dirty, the node and renderer referenced by the item are still exist.
              // 2. Mark the related dirty rects as visited so they will not be removed below.
              auto dirtyRectPos = std::lower_bound(itemsDirtyRects.begin(), itemsDirtyRects.end(), dirtyRect);
              while(dirtyRectPos != itemsDirtyRects.end())
              {
                if(dirtyRectPos->node != item.mNode || dirtyRectPos->renderer != item.mRenderer)
                {
                  break;
                }

                dirtyRectPos->visited = true;
                dirtyRectPos++;
              }
            }
          }
        }
      }
    }
  }

  // Check removed nodes or removed renderers dirty rects
  auto i = itemsDirtyRects.begin();
  auto j = itemsDirtyRects.begin();
  while(i != itemsDirtyRects.end())
  {
    if(i->visited)
    {
      *j++ = *i;
    }
    else
    {
      Rect<int>& dirtRect = i->rect;
      damagedRects.push_back(dirtRect);
    }
    i++;
  }

  itemsDirtyRects.resize(j - itemsDirtyRects.begin());
  damagedRectCleaner.SetCleanOnReturn(false);
}

void RenderManager::RenderScene(Integration::RenderStatus& status, Integration::Scene& scene, bool renderToFbo)
{
  Rect<int> clippingRect;
  RenderScene(status, scene, renderToFbo, clippingRect);
}

void RenderManager::RenderScene(Integration::RenderStatus& status, Integration::Scene& scene, bool renderToFbo, Rect<int>& clippingRect)
{
  // Reset main algorithms command buffer
  mImpl->renderAlgorithms.ResetCommandBuffer();

  auto mainCommandBuffer = mImpl->renderAlgorithms.GetMainCommandBuffer();

  Internal::Scene&   sceneInternal = GetImplementation(scene);
  SceneGraph::Scene* sceneObject   = sceneInternal.GetSceneObject();

  uint32_t count = sceneObject->GetRenderInstructions().Count(mImpl->renderBufferIndex);

  std::vector<Graphics::RenderTarget*> targetstoPresent;

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
    Vector4       clearColor;

    if(instruction.mIsClearColorSet)
    {
      clearColor = instruction.mClearColor;
    }
    else
    {
      clearColor = Dali::RenderTask::DEFAULT_CLEAR_COLOR;
    }

    Rect<int32_t> surfaceRect        = sceneObject->GetSurfaceRect();
    int32_t       surfaceOrientation = sceneObject->GetSurfaceOrientation();

    Integration::DepthBufferAvailable   depthBufferAvailable   = mImpl->depthBufferAvailable;
    Integration::StencilBufferAvailable stencilBufferAvailable = mImpl->stencilBufferAvailable;

    Graphics::RenderTarget*           currentRenderTarget = nullptr;
    Graphics::RenderPass*             currentRenderPass   = nullptr;
    std::vector<Graphics::ClearValue> currentClearValues{};

    if(instruction.mFrameBuffer)
    {
      // Ensure graphics framebuffer is created, bind atachments and create render passes
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
      if(instruction.mIsClearColorSet && clearValues.size() > 0)
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

      if(mImpl->currentContext != &mImpl->context)
      {
        // Switch to shared context for off-screen buffer
        mImpl->currentContext = &mImpl->context;

        // Clear the current cached program when the context is switched
        mImpl->programController.ClearCurrentProgram();
      }
    }
    else
    {
      if(mImpl->currentContext->IsSurfacelessContextSupported())
      {
        if(mImpl->currentContext != sceneObject->GetContext())
        {
          // Switch the correct context if rendering to a surface
          mImpl->currentContext = sceneObject->GetContext();

          // Clear the current cached program when the context is switched
          mImpl->programController.ClearCurrentProgram();
        }
      }

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

      auto loadOp = instruction.mIsClearColorSet ? Graphics::AttachmentLoadOp::CLEAR : Graphics::AttachmentLoadOp::LOAD;

      currentRenderTarget = sceneObject->GetSurfaceRenderTarget();
      currentRenderPass   = sceneObject->GetGraphicsRenderPass(loadOp, Graphics::AttachmentStoreOp::STORE);
    }

    targetstoPresent.emplace_back(currentRenderTarget);

    // Make sure that GL context must be created
    mImpl->currentContext->GlContextCreated();

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
        // For glViewport the lower-left corner is (0,0)
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
        // For glViewport the lower-left corner is (0,0)
        const int32_t y = (surfaceRect.height - instruction.mViewport.height) - instruction.mViewport.y;
        viewportRect.Set(instruction.mViewport.x, y, instruction.mViewport.width, instruction.mViewport.height);
      }
      else
      {
        viewportRect = surfaceRect;
      }
    }

    // Set surface orientation
    mImpl->currentContext->SetSurfaceOrientation(surfaceOrientation);

    /*** Clear region of framebuffer or surface before drawing ***/

    bool clearFullFrameRect = true;
    if(instruction.mFrameBuffer != nullptr)
    {
      Viewport frameRect(0, 0, instruction.mFrameBuffer->GetWidth(), instruction.mFrameBuffer->GetHeight());
      clearFullFrameRect = (frameRect == viewportRect);
    }
    else
    {
      clearFullFrameRect = (surfaceRect == viewportRect);
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
      *mImpl->currentContext,
      mImpl->renderBufferIndex,
      depthBufferAvailable,
      stencilBufferAvailable,
      mImpl->boundTextures,
      viewportRect,
      clippingRect,
      surfaceOrientation);

    // Synchronise the FBO/Texture access when there are multiple contexts
    if(mImpl->currentContext->IsSurfacelessContextSupported())
    {
      // Check whether any bound texture is in the dependency list
      bool textureFound = false;

      if(mImpl->boundTextures.Count() > 0u && mImpl->textureDependencyList.Count() > 0u)
      {
        for(auto texture : mImpl->textureDependencyList)
        {
          textureFound = std::find_if(mImpl->boundTextures.Begin(), mImpl->boundTextures.End(), [texture](Graphics::Texture* graphicsTexture) {
                           return texture == graphicsTexture;
                         }) != mImpl->boundTextures.End();
        }
      }

      if(textureFound)
      {
        if(instruction.mFrameBuffer)
        {
          // For off-screen buffer

          // Clear the dependency list
          mImpl->textureDependencyList.Clear();
        }
        else
        {
          // Worker thread lambda function
          auto& glContextHelperAbstraction = mImpl->graphicsController.GetGlContextHelperAbstraction();
          auto  workerFunction             = [&glContextHelperAbstraction](int workerThread) {
            // Switch to the shared context in the worker thread
            glContextHelperAbstraction.MakeSurfacelessContextCurrent();

            // Wait until all rendering calls for the shared context are executed
            glContextHelperAbstraction.WaitClient();

            // Must clear the context in the worker thread
            // Otherwise the shared context cannot be switched to from the render thread
            glContextHelperAbstraction.MakeContextNull();
          };

          auto future = mImpl->threadPool->SubmitTask(0u, workerFunction);
          if(future)
          {
            mImpl->threadPool->Wait();

            // Clear the dependency list
            mImpl->textureDependencyList.Clear();
          }
        }
      }
    }

    if(instruction.mRenderTracker && instruction.mFrameBuffer)
    {
      // This will create a sync object every frame this render tracker
      // is alive (though it should be now be created only for
      // render-once render tasks)
      instruction.mRenderTracker->CreateSyncObject(mImpl->graphicsController.GetGlSyncAbstraction());
      instruction.mRenderTracker = nullptr; // Only create once.
    }

    // End render pass
    mainCommandBuffer->EndRenderPass();
  }
  mImpl->renderAlgorithms.SubmitCommandBuffer();

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

void RenderManager::PostRender(bool uploadOnly)
{
  // Notify RenderGeometries that rendering has finished
  for(auto&& iter : mImpl->geometryContainer)
  {
    iter->OnRenderFinished();
  }

  mImpl->UpdateTrackers();

  uint32_t count = 0u;
  for(uint32_t i = 0; i < mImpl->sceneContainer.size(); ++i)
  {
    count += mImpl->sceneContainer[i]->GetRenderInstructions().Count(mImpl->renderBufferIndex);
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
