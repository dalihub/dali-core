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

// CLASS HEADER
#include <dali/internal/render/renderers/render-renderer.h>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-types.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/image-sampler.h>
#include <dali/internal/common/matrix-utils.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/event/rendering/texture-impl.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/render/data-providers/uniform-map-data-provider.h>
#include <dali/internal/render/renderers/pipeline-cache.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/internal/render/renderers/render-texture.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/internal/render/renderers/uniform-buffer-view.h>
#include <dali/internal/render/renderers/uniform-buffer.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/render-shader.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/public-api/signals/render-callback.h>

namespace Dali::Internal
{
Dali::Matrix* testMVP;
uint32_t      mvpBufferIndex;

namespace
{
/**
 * Helper function to calculate the correct alignment of data for uniform buffers
 * @param dataSize size of uniform buffer
 * @return aligned offset of data
 */
inline uint32_t GetUniformBufferDataAlignment(uint32_t dataSize)
{
  return ((dataSize / 256u) + ((dataSize % 256u) ? 1u : 0u)) * 256u;
}

/**
 * @brief Store latest bound RenderGeometry, and help that we can skip duplicated vertex attributes bind.
 *
 * @param[in] geometry Current geometry to be used, or nullptr if render finished
 * @return True if we can reuse latest bound vertex attributes. False otherwise.
 */
inline bool ReuseLatestBoundVertexAttributes(const Render::Geometry* geometry)
{
  static const Render::Geometry* gLatestVertexBoundGeometry = nullptr;
  if(gLatestVertexBoundGeometry == geometry)
  {
    return true;
  }
  gLatestVertexBoundGeometry = geometry;
  return false;
}

} // namespace

namespace Render
{
namespace
{
Dali::Internal::MemoryPoolObjectAllocator<Renderer>& GetRenderRendererMemoryPool()
{
  static Dali::Internal::MemoryPoolObjectAllocator<Renderer> gRenderRendererMemoryPool;
  return gRenderRendererMemoryPool;
}
} // namespace

void Renderer::PrepareCommandBuffer()
{
  // Reset latest geometry informations, So we can bind the first of geometry.
  ReuseLatestBoundVertexAttributes(nullptr);

  // todo : Fill here as many caches as we can store for reduce the number of command buffers
}

RendererKey Renderer::NewKey(SceneGraph::RenderDataProvider* dataProvider,
                             Render::Geometry*               geometry,
                             uint32_t                        blendingBitmask,
                             const Vector4&                  blendColor,
                             FaceCullingMode::Type           faceCullingMode,
                             bool                            preMultipliedAlphaEnabled,
                             DepthWriteMode::Type            depthWriteMode,
                             DepthTestMode::Type             depthTestMode,
                             DepthFunction::Type             depthFunction,
                             StencilParameters&              stencilParameters)
{
  void* ptr = GetRenderRendererMemoryPool().AllocateRawThreadSafe();
  auto  key = GetRenderRendererMemoryPool().GetKeyFromPtr(static_cast<Renderer*>(ptr));

  // Use placement new to construct renderer.
  new(ptr) Renderer(dataProvider, geometry, blendingBitmask, blendColor, faceCullingMode, preMultipliedAlphaEnabled, depthWriteMode, depthTestMode, depthFunction, stencilParameters);
  return RendererKey(key);
}

void Renderer::ResetMemoryPool()
{
  GetRenderRendererMemoryPool().ResetMemoryPool();
}

Renderer::Renderer(SceneGraph::RenderDataProvider* dataProvider,
                   Render::Geometry*               geometry,
                   uint32_t                        blendingBitmask,
                   const Vector4&                  blendColor,
                   FaceCullingMode::Type           faceCullingMode,
                   bool                            preMultipliedAlphaEnabled,
                   DepthWriteMode::Type            depthWriteMode,
                   DepthTestMode::Type             depthTestMode,
                   DepthFunction::Type             depthFunction,
                   StencilParameters&              stencilParameters)
: mGraphicsController(nullptr),
  mRenderDataProvider(dataProvider),
  mGeometry(geometry),
  mProgramCache(nullptr),
  mStencilParameters(stencilParameters),
  mBlendingOptions(),
  mIndexedDrawFirstElement(0),
  mIndexedDrawElementsCount(0),
  mDepthFunction(depthFunction),
  mFaceCullingMode(faceCullingMode),
  mDepthWriteMode(depthWriteMode),
  mDepthTestMode(depthTestMode),
  mPremultipliedAlphaEnabled(preMultipliedAlphaEnabled),
  mShaderChanged(false),
  mPipelineCached(false)
{
  if(blendingBitmask != 0u)
  {
    mBlendingOptions.SetBitmask(blendingBitmask);
  }

  mBlendingOptions.SetBlendColor(blendColor);
}

void Renderer::Initialize(Graphics::Controller& graphicsController, ProgramCache& programCache, Render::UniformBufferManager& uniformBufferManager, Render::PipelineCache& pipelineCache)
{
  mGraphicsController   = &graphicsController;
  mProgramCache         = &programCache;
  mUniformBufferManager = &uniformBufferManager;
  mPipelineCache        = &pipelineCache;

  // Add Observer now
  if(mGeometry)
  {
    mGeometry->AddLifecycleObserver(*this);
  }
}

Renderer::~Renderer()
{
  // Reset old pipeline
  if(DALI_LIKELY(mPipelineCached))
  {
    mPipelineCache->ResetPipeline(mPipeline);
    mPipelineCached = false;
  }

  // Stop observing
  if(mCurrentProgram)
  {
    mCurrentProgram->RemoveLifecycleObserver(*this);
    mCurrentProgram = nullptr;
  }
  if(mGeometry)
  {
    mGeometry->RemoveLifecycleObserver(*this);
    mGeometry = nullptr;
  }
}

void Renderer::operator delete(void* ptr)
{
  GetRenderRendererMemoryPool().FreeThreadSafe(static_cast<Renderer*>(ptr));
}

Renderer* Renderer::Get(RendererKey::KeyType rendererKey)
{
  return GetRenderRendererMemoryPool().GetPtrFromKey(rendererKey);
}

void Renderer::SetGeometry(Render::Geometry* geometry)
{
  if(mGeometry != geometry)
  {
    if(mGeometry)
    {
      mGeometry->RemoveLifecycleObserver(*this);
    }

    mGeometry = geometry;

    if(mGeometry)
    {
      mGeometry->AddLifecycleObserver(*this);
    }

    // Reset old pipeline
    if(DALI_LIKELY(mPipelineCached))
    {
      mPipelineCache->ResetPipeline(mPipeline);
      mPipelineCached = false;
    }
  }
}

void Renderer::SetDrawCommands(Dali::DevelRenderer::DrawCommand* pDrawCommands, uint32_t size)
{
  mDrawCommands.clear();
  mDrawCommands.insert(mDrawCommands.end(), pDrawCommands, pDrawCommands + size);
}

bool Renderer::BindTextures(Graphics::CommandBuffer& commandBuffer)
{
  uint32_t textureUnit = 0;

  auto textures(mRenderDataProvider->GetTextures());
  auto samplers(mRenderDataProvider->GetSamplers());

  std::vector<Graphics::TextureBinding> textureBindings;

  if(textures != nullptr)
  {
    const std::uint32_t texturesCount(static_cast<std::uint32_t>(textures->Count()));
    textureBindings.reserve(texturesCount);

    for(uint32_t i = 0; i < texturesCount; ++i) // not expecting more than uint32_t of textures
    {
      if((*textures)[i] && (*textures)[i]->GetGraphicsObject())
      {
        Graphics::Texture* graphicsTexture = (*textures)[i]->GetGraphicsObject();
        // if the sampler exists,
        //   if it's default, delete the graphics object
        //   otherwise re-initialize it if dirty

        const Graphics::Sampler* graphicsSampler = samplers ? ((i < (*samplers).Size() && (*samplers)[i]) ? (*samplers)[i]->GetGraphicsObject() : nullptr) : nullptr;

        const Graphics::TextureBinding textureBinding{graphicsTexture, graphicsSampler, textureUnit};
        textureBindings.push_back(textureBinding);

        ++textureUnit;
      }
      else
      {
        // Texture is not prepared yet. We should not render now.
        return false;
      }
    }
  }

  if(!textureBindings.empty())
  {
    commandBuffer.BindTextures(textureBindings);
  }

  return true;
}

void Renderer::SetFaceCullingMode(FaceCullingMode::Type mode)
{
  mFaceCullingMode = mode;
}

void Renderer::SetBlendingBitMask(uint32_t bitmask)
{
  mBlendingOptions.SetBitmask(bitmask);
}

void Renderer::SetBlendColor(const Vector4& color)
{
  mBlendingOptions.SetBlendColor(color);
}

void Renderer::SetIndexedDrawFirstElement(uint32_t firstElement)
{
  mIndexedDrawFirstElement = firstElement;
}

void Renderer::SetIndexedDrawElementsCount(uint32_t elementsCount)
{
  mIndexedDrawElementsCount = elementsCount;
}

void Renderer::EnablePreMultipliedAlpha(bool enable)
{
  mPremultipliedAlphaEnabled = enable;
}

void Renderer::SetDepthWriteMode(DepthWriteMode::Type depthWriteMode)
{
  mDepthWriteMode = depthWriteMode;
}

void Renderer::SetDepthTestMode(DepthTestMode::Type depthTestMode)
{
  mDepthTestMode = depthTestMode;
}

DepthWriteMode::Type Renderer::GetDepthWriteMode() const
{
  return mDepthWriteMode;
}

DepthTestMode::Type Renderer::GetDepthTestMode() const
{
  return mDepthTestMode;
}

void Renderer::SetDepthFunction(DepthFunction::Type depthFunction)
{
  mDepthFunction = depthFunction;
}

DepthFunction::Type Renderer::GetDepthFunction() const
{
  return mDepthFunction;
}

void Renderer::SetRenderMode(RenderMode::Type renderMode)
{
  mStencilParameters.renderMode = renderMode;
}

RenderMode::Type Renderer::GetRenderMode() const
{
  return mStencilParameters.renderMode;
}

void Renderer::SetStencilFunction(StencilFunction::Type stencilFunction)
{
  mStencilParameters.stencilFunction = stencilFunction;
}

StencilFunction::Type Renderer::GetStencilFunction() const
{
  return mStencilParameters.stencilFunction;
}

void Renderer::SetStencilFunctionMask(int stencilFunctionMask)
{
  mStencilParameters.stencilFunctionMask = stencilFunctionMask;
}

int Renderer::GetStencilFunctionMask() const
{
  return mStencilParameters.stencilFunctionMask;
}

void Renderer::SetStencilFunctionReference(int stencilFunctionReference)
{
  mStencilParameters.stencilFunctionReference = stencilFunctionReference;
}

int Renderer::GetStencilFunctionReference() const
{
  return mStencilParameters.stencilFunctionReference;
}

void Renderer::SetStencilMask(int stencilMask)
{
  mStencilParameters.stencilMask = stencilMask;
}

int Renderer::GetStencilMask() const
{
  return mStencilParameters.stencilMask;
}

void Renderer::SetStencilOperationOnFail(StencilOperation::Type stencilOperationOnFail)
{
  mStencilParameters.stencilOperationOnFail = stencilOperationOnFail;
}

StencilOperation::Type Renderer::GetStencilOperationOnFail() const
{
  return mStencilParameters.stencilOperationOnFail;
}

void Renderer::SetStencilOperationOnZFail(StencilOperation::Type stencilOperationOnZFail)
{
  mStencilParameters.stencilOperationOnZFail = stencilOperationOnZFail;
}

StencilOperation::Type Renderer::GetStencilOperationOnZFail() const
{
  return mStencilParameters.stencilOperationOnZFail;
}

void Renderer::SetStencilOperationOnZPass(StencilOperation::Type stencilOperationOnZPass)
{
  mStencilParameters.stencilOperationOnZPass = stencilOperationOnZPass;
}

StencilOperation::Type Renderer::GetStencilOperationOnZPass() const
{
  return mStencilParameters.stencilOperationOnZPass;
}

void Renderer::Upload()
{
  mGeometry->Upload(*mGraphicsController);
}

bool Renderer::NeedsProgram() const
{
  // Our access to shader is currently through the RenderDataProvider, which
  // returns a reference to the shader (as at the time, we couldn't have empty
  // renderers). We prefer to keep this as a reference. So, we don't use it
  // here. Instead, we use the mGeometry pointer to decide if this is an empty
  // renderer.
  return (!mRenderCallback && mGeometry != nullptr);
}

Program* Renderer::PrepareProgram(const SceneGraph::RenderInstruction& instruction)
{
  // Create Program
  const SceneGraph::Shader& shader = mRenderDataProvider->GetShader();

  const ShaderDataPtr& shaderData = shader.GetShaderData(instruction.mRenderPassTag);
  if(!shaderData)
  {
    DALI_LOG_ERROR("Failed to get shader data.\n");
    if(mCurrentProgram)
    {
      mCurrentProgram->RemoveLifecycleObserver(*this);
    }
    mCurrentProgram = nullptr;
    return nullptr;
  }

  Program* program = Program::New(*mProgramCache,
                                  shaderData,
                                  *mGraphicsController);
  if(!program)
  {
    DALI_LOG_ERROR("Failed to create program for shader at address %p.\n", reinterpret_cast<const void*>(&shader));
    if(mCurrentProgram)
    {
      mCurrentProgram->RemoveLifecycleObserver(*this);
    }
    mCurrentProgram = nullptr;
    return nullptr;
  }

  // If program doesn't have Gfx program object assigned yet, prepare it.
  if(!program->GetGraphicsProgramPtr())
  {
    Graphics::ShaderCreateInfo vertexShaderCreateInfo;
    vertexShaderCreateInfo.SetPipelineStage(Graphics::PipelineStage::VERTEX_SHADER);
    vertexShaderCreateInfo.SetSourceMode(shaderData->GetSourceMode());
    const std::vector<char>& vertexShaderSrc = shaderData->GetShaderForPipelineStage(Graphics::PipelineStage::VERTEX_SHADER);
    vertexShaderCreateInfo.SetSourceSize(vertexShaderSrc.size());
    vertexShaderCreateInfo.SetSourceData(static_cast<const void*>(vertexShaderSrc.data()));
    vertexShaderCreateInfo.SetShaderVersion(shaderData->GetVertexShaderVersion());
    auto vertexShader = mGraphicsController->CreateShader(vertexShaderCreateInfo, nullptr);

    Graphics::ShaderCreateInfo fragmentShaderCreateInfo;
    fragmentShaderCreateInfo.SetPipelineStage(Graphics::PipelineStage::FRAGMENT_SHADER);
    fragmentShaderCreateInfo.SetSourceMode(shaderData->GetSourceMode());
    const std::vector<char>& fragmentShaderSrc = shaderData->GetShaderForPipelineStage(Graphics::PipelineStage::FRAGMENT_SHADER);
    fragmentShaderCreateInfo.SetSourceSize(fragmentShaderSrc.size());
    fragmentShaderCreateInfo.SetSourceData(static_cast<const void*>(fragmentShaderSrc.data()));
    fragmentShaderCreateInfo.SetShaderVersion(shaderData->GetFragmentShaderVersion());
    auto fragmentShader = mGraphicsController->CreateShader(fragmentShaderCreateInfo, nullptr);

    std::vector<Graphics::ShaderState> shaderStates{
      Graphics::ShaderState()
        .SetShader(*vertexShader.get())
        .SetPipelineStage(Graphics::PipelineStage::VERTEX_SHADER),
      Graphics::ShaderState()
        .SetShader(*fragmentShader.get())
        .SetPipelineStage(Graphics::PipelineStage::FRAGMENT_SHADER)};

    auto createInfo = Graphics::ProgramCreateInfo();
    createInfo.SetShaderState(shaderStates);
    createInfo.SetName(shaderData->GetName());
    auto graphicsProgram = mGraphicsController->CreateProgram(createInfo, nullptr);
    program->SetGraphicsProgram(std::move(graphicsProgram), *mUniformBufferManager); // generates reflection
  }

  // Set prefetched program to be used during rendering
  if(mCurrentProgram != program)
  {
    if(mCurrentProgram)
    {
      mCurrentProgram->RemoveLifecycleObserver(*this);
    }
    mCurrentProgram = program;
    mCurrentProgram->AddLifecycleObserver(*this);
  }
  return mCurrentProgram;
}

bool Renderer::Render(Graphics::CommandBuffer&                             commandBuffer,
                      BufferIndex                                          bufferIndex,
                      const SceneGraph::NodeDataProvider&                  node,
                      const Matrix&                                        modelMatrix,
                      const Matrix&                                        modelViewMatrix,
                      const Matrix&                                        viewMatrix,
                      const Matrix&                                        projectionMatrix,
                      const Vector4&                                       worldColor,
                      const Vector3&                                       scale,
                      const Vector3&                                       size,
                      bool                                                 blend,
                      const Dali::Internal::SceneGraph::RenderInstruction& instruction,
                      Graphics::RenderTarget*                              renderTarget,
                      uint32_t                                             queueIndex)
{
  // Before doing anything test if the call happens in the right queue
  if(mDrawCommands.empty() && queueIndex > 0)
  {
    return false;
  }

  // Check if there is render callback
  if(mRenderCallback)
  {
    if(!mRenderCallbackInput)
    {
      mRenderCallbackInput = std::unique_ptr<RenderCallbackInput>(new RenderCallbackInput);
    }

    bool isolatedNotDirect = (mRenderCallback->GetExecutionMode() == RenderCallback::ExecutionMode::ISOLATED);

    Graphics::DrawNativeInfo info{};
    info.api      = Graphics::DrawNativeAPI::GLES;
    info.callback = &static_cast<Dali::CallbackBase&>(*mRenderCallback);
    info.userData = mRenderCallbackInput.get();

    // Tell custom renderer whether code executes in isolation or is injected directly and may alter
    // state or DALi rendering pipeline
    mRenderCallbackInput->usingOwnEglContext = isolatedNotDirect;
    // Set storage for the context to be used
    info.glesNativeInfo.eglSharedContextStoragePointer = &mRenderCallbackInput->eglContext;
    info.executionMode                                 = isolatedNotDirect ? Graphics::DrawNativeExecutionMode::ISOLATED : Graphics::DrawNativeExecutionMode::DIRECT;
    info.reserved                                      = nullptr;

    auto& textureResources = mRenderCallback->GetTextureResources();

    if(!textureResources.empty())
    {
      mRenderCallbackTextureBindings.clear();
      mRenderCallbackInput->textureBindings.resize(textureResources.size());
      auto i = 0u;
      for(auto& texture : textureResources)
      {
        auto& textureImpl     = GetImplementation(texture);
        auto  graphicsTexture = textureImpl.GetRenderTextureKey()->GetGraphicsObject();

        auto properties = mGraphicsController->GetTextureProperties(*graphicsTexture);

        mRenderCallbackTextureBindings.emplace_back(graphicsTexture);
        mRenderCallbackInput->textureBindings[i++] = properties.nativeHandle;
      }
      info.textureCount = mRenderCallbackTextureBindings.size();
      info.textureList  = mRenderCallbackTextureBindings.data();
    }

    // pass render callback input
    mRenderCallbackInput->size       = size;
    mRenderCallbackInput->view       = viewMatrix;
    mRenderCallbackInput->projection = projectionMatrix;

    MatrixUtils::MultiplyProjectionMatrix(mRenderCallbackInput->mvp, modelViewMatrix, projectionMatrix);

    // submit draw
    commandBuffer.DrawNative(&info);
    return true;
  }

  // Prepare commands
  std::vector<DevelRenderer::DrawCommand*> commands;
  for(auto& cmd : mDrawCommands)
  {
    if(cmd.queue == queueIndex)
    {
      commands.emplace_back(&cmd);
    }
  }

  // Have commands but nothing to be drawn - abort
  if(!mDrawCommands.empty() && commands.empty())
  {
    return false;
  }

  // Set blending mode
  if(!mDrawCommands.empty())
  {
    blend = (commands[0]->queue != DevelRenderer::RENDER_QUEUE_OPAQUE) && blend;
  }

  bool drawn = false;

  // Check all textures are prepared first.
  if(!BindTextures(commandBuffer))
  {
    return drawn;
  }

  // We should have a shader here (as only RenderCallback has no shader, and that's been early out)
  Program* program = PrepareProgram(instruction);
  if(program)
  {
    // Prepare the graphics pipeline. This may either re-use an existing pipeline or create a new one.
    auto& pipeline = PrepareGraphicsPipeline(*program, instruction, renderTarget, node, blend);

    commandBuffer.BindPipeline(pipeline);

    if(queueIndex == 0)
    {
      std::size_t nodeIndex = BuildUniformIndexMap(bufferIndex, node, *program);
      WriteUniformBuffer(bufferIndex, commandBuffer, program, instruction, modelMatrix, modelViewMatrix, viewMatrix, projectionMatrix, worldColor, scale, size, nodeIndex);
    }
    // @todo We should detect this case much earlier to prevent unnecessary work
    // Reuse latest bound vertex attributes location, or Bind buffers to attribute locations.
    if(ReuseLatestBoundVertexAttributes(mGeometry) || mGeometry->BindVertexAttributes(commandBuffer))
    {
      uint32_t instanceCount = mRenderDataProvider->GetInstanceCount();

      if(mDrawCommands.empty())
      {
        drawn = mGeometry->Draw(*mGraphicsController, commandBuffer, mIndexedDrawFirstElement, mIndexedDrawElementsCount, instanceCount);
      }
      else
      {
        for(auto& cmd : commands)
        {
          drawn |= mGeometry->Draw(*mGraphicsController, commandBuffer, cmd->firstIndex, cmd->elementCount, instanceCount);
        }
      }
    }
    else
    {
      // BindVertexAttributes failed. Reset cached geometry.
      ReuseLatestBoundVertexAttributes(nullptr);
    }
  }

  return drawn;
}

std::size_t Renderer::BuildUniformIndexMap(BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, Program& program)
{
  // Check if the map has changed
  DALI_ASSERT_DEBUG(mRenderDataProvider && "No Uniform map data provider available");

  const SceneGraph::UniformMapDataProvider& uniformMapDataProvider = mRenderDataProvider->GetUniformMapDataProvider();
  const SceneGraph::CollectedUniformMap&    uniformMap             = uniformMapDataProvider.GetCollectedUniformMap();
  const SceneGraph::UniformMap&             uniformMapNode         = node.GetNodeUniformMap();

  bool updateMaps;

  // Usual case is to only have 1 node, however we do allow multiple nodes to reuse the same
  // renderer, so we have to cache uniform map per render item (node / renderer pair).

  // Specially, if node don't have uniformMap, we mark nodePtr as nullptr.
  // So, all nodes without uniformMap will share same UniformIndexMap, contains only render data providers.
  const auto nodePtr    = uniformMapNode.Count() ? &node : nullptr;
  const auto programPtr = &program;

  const auto nodeChangeCounter          = nodePtr ? uniformMapNode.GetChangeCounter() : 0;
  const auto renderItemMapChangeCounter = uniformMap.GetChangeCounter();

  auto iter = std::find_if(mNodeIndexMap.begin(), mNodeIndexMap.end(), [nodePtr, programPtr](RenderItemLookup& element) { return (element.node == nodePtr && element.program == programPtr); });

  std::size_t renderItemMapIndex;
  if(iter == mNodeIndexMap.end())
  {
    renderItemMapIndex = mUniformIndexMaps.size();
    RenderItemLookup renderItemLookup;
    renderItemLookup.node                       = nodePtr;
    renderItemLookup.program                    = programPtr;
    renderItemLookup.index                      = renderItemMapIndex;
    renderItemLookup.nodeChangeCounter          = nodeChangeCounter;
    renderItemLookup.renderItemMapChangeCounter = renderItemMapChangeCounter;
    mNodeIndexMap.emplace_back(renderItemLookup);

    updateMaps = true;
    mUniformIndexMaps.resize(mUniformIndexMaps.size() + 1);
  }
  else
  {
    renderItemMapIndex = iter->index;

    updateMaps = (nodeChangeCounter != iter->nodeChangeCounter) ||
                 (renderItemMapChangeCounter != iter->renderItemMapChangeCounter) ||
                 (mUniformIndexMaps[renderItemMapIndex].size() == 0);

    iter->nodeChangeCounter          = nodeChangeCounter;
    iter->renderItemMapChangeCounter = renderItemMapChangeCounter;
  }

  if(updateMaps || mShaderChanged)
  {
    // Reset shader pointer
    mShaderChanged = false;

    const uint32_t mapCount     = uniformMap.Count();
    const uint32_t mapNodeCount = uniformMapNode.Count();

    mUniformIndexMaps[renderItemMapIndex].clear(); // Clear contents, but keep memory if we don't change size
    mUniformIndexMaps[renderItemMapIndex].resize(mapCount + mapNodeCount);

    // Copy uniform map into mUniformIndexMap
    uint32_t mapIndex = 0;
    for(; mapIndex < mapCount; ++mapIndex)
    {
      mUniformIndexMaps[renderItemMapIndex][mapIndex].propertyValue          = uniformMap.mUniformMap[mapIndex].propertyPtr;
      mUniformIndexMaps[renderItemMapIndex][mapIndex].uniformName            = uniformMap.mUniformMap[mapIndex].uniformName;
      mUniformIndexMaps[renderItemMapIndex][mapIndex].uniformNameHash        = uniformMap.mUniformMap[mapIndex].uniformNameHash;
      mUniformIndexMaps[renderItemMapIndex][mapIndex].uniformNameHashNoArray = uniformMap.mUniformMap[mapIndex].uniformNameHashNoArray;
      mUniformIndexMaps[renderItemMapIndex][mapIndex].arrayIndex             = uniformMap.mUniformMap[mapIndex].arrayIndex;
    }

    for(uint32_t nodeMapIndex = 0; nodeMapIndex < mapNodeCount; ++nodeMapIndex)
    {
      auto  hash = uniformMapNode[nodeMapIndex].uniformNameHash;
      auto& name = uniformMapNode[nodeMapIndex].uniformName;
      bool  found(false);
      for(uint32_t i = 0; i < mapCount; ++i)
      {
        if(mUniformIndexMaps[renderItemMapIndex][i].uniformNameHash == hash &&
           mUniformIndexMaps[renderItemMapIndex][i].uniformName == name)
        {
          mUniformIndexMaps[renderItemMapIndex][i].propertyValue = uniformMapNode[nodeMapIndex].propertyPtr;
          found                                                  = true;
          break;
        }
      }

      if(!found)
      {
        mUniformIndexMaps[renderItemMapIndex][mapIndex].propertyValue          = uniformMapNode[nodeMapIndex].propertyPtr;
        mUniformIndexMaps[renderItemMapIndex][mapIndex].uniformName            = uniformMapNode[nodeMapIndex].uniformName;
        mUniformIndexMaps[renderItemMapIndex][mapIndex].uniformNameHash        = uniformMapNode[nodeMapIndex].uniformNameHash;
        mUniformIndexMaps[renderItemMapIndex][mapIndex].uniformNameHashNoArray = uniformMapNode[nodeMapIndex].uniformNameHashNoArray;
        mUniformIndexMaps[renderItemMapIndex][mapIndex].arrayIndex             = uniformMapNode[nodeMapIndex].arrayIndex;
        ++mapIndex;
      }
    }

    mUniformIndexMaps[renderItemMapIndex].resize(mapIndex);
  }
  return renderItemMapIndex;
}

void Renderer::WriteUniformBuffer(
  BufferIndex                          bufferIndex,
  Graphics::CommandBuffer&             commandBuffer,
  Program*                             program,
  const SceneGraph::RenderInstruction& instruction,
  const Matrix&                        modelMatrix,
  const Matrix&                        modelViewMatrix,
  const Matrix&                        viewMatrix,
  const Matrix&                        projectionMatrix,
  const Vector4&                       worldColor,
  const Vector3&                       scale,
  const Vector3&                       size,
  std::size_t                          nodeIndex)
{
  auto& reflection = mGraphicsController->GetProgramReflection(program->GetGraphicsProgram());

  const auto& programRequirements = program->GetUniformBlocksMemoryRequirements();

  // Allocate UBO view per each block (include standalone block)
  auto blockCount = programRequirements.blockCount;

  std::vector<std::unique_ptr<UniformBufferView>> uboViews;
  uboViews.resize(blockCount);

  // Prepare bindings
  auto uboCount = reflection.GetUniformBlockCount();
  mUniformBufferBindings.resize(uboCount);

  for(auto i = 0u; i < blockCount; ++i)
  {
    bool standaloneUniforms = (i == 0);
    if(programRequirements.blockSize[i])
    {
      auto uniformBufferView             = mUniformBufferManager->CreateUniformBufferView(programRequirements.blockSize[i], standaloneUniforms);
      mUniformBufferBindings[i].buffer   = uniformBufferView->GetBuffer();
      mUniformBufferBindings[i].offset   = uniformBufferView->GetOffset();
      mUniformBufferBindings[i].binding  = standaloneUniforms ? 0 : reflection.GetUniformBlockBinding(i);
      mUniformBufferBindings[i].dataSize = reflection.GetUniformBlockSize(i);
      uboViews[i].reset(uniformBufferView.release());
    }
  }

  // update the uniform buffer
  // pass shared UBO and offset, return new offset for next item to be used
  // don't process bindings if there are no uniform buffers allocated
  if(!uboViews.empty())
  {
    // Write default uniforms
    WriteDefaultUniformV2(program->GetDefaultUniform(Program::DefaultUniformIndex::MODEL_MATRIX), uboViews, modelMatrix);
    WriteDefaultUniformV2(program->GetDefaultUniform(Program::DefaultUniformIndex::VIEW_MATRIX), uboViews, viewMatrix);
    WriteDefaultUniformV2(program->GetDefaultUniform(Program::DefaultUniformIndex::PROJECTION_MATRIX), uboViews, projectionMatrix);
    WriteDefaultUniformV2(program->GetDefaultUniform(Program::DefaultUniformIndex::MODEL_VIEW_MATRIX), uboViews, modelViewMatrix);

    auto mvpUniformInfo = program->GetDefaultUniform(Program::DefaultUniformIndex::MVP_MATRIX);
    if(mvpUniformInfo && !mvpUniformInfo->name.empty())
    {
      Matrix modelViewProjectionMatrix(false);
      MatrixUtils::MultiplyProjectionMatrix(modelViewProjectionMatrix, modelViewMatrix, projectionMatrix);
      WriteDefaultUniformV2(mvpUniformInfo, uboViews, modelViewProjectionMatrix);
    }

    auto normalUniformInfo = program->GetDefaultUniform(Program::DefaultUniformIndex::NORMAL_MATRIX);
    if(normalUniformInfo && !normalUniformInfo->name.empty())
    {
      Matrix3 normalMatrix(modelViewMatrix);
      normalMatrix.Invert();
      normalMatrix.Transpose();
      WriteDefaultUniformV2(normalUniformInfo, uboViews, normalMatrix);
    }

    WriteDefaultUniformV2(program->GetDefaultUniform(Program::DefaultUniformIndex::SCALE), uboViews, scale);

    const Vector4& mixColor   = mRenderDataProvider->GetMixColor(bufferIndex); ///< Renderer's mix color
    Vector4        finalColor = worldColor * mixColor;                         ///< Applied Actor's original color to renderer's mix color
    if(mPremultipliedAlphaEnabled)
    {
      const float alpha = finalColor.a;
      finalColor.r *= alpha;
      finalColor.g *= alpha;
      finalColor.b *= alpha;
    }
    WriteDefaultUniformV2(program->GetDefaultUniform(Program::DefaultUniformIndex::COLOR), uboViews, finalColor);
    WriteDefaultUniformV2(program->GetDefaultUniform(Program::DefaultUniformIndex::ACTOR_COLOR), uboViews, worldColor);

    // Write uniforms from the uniform map
    FillUniformBuffer(*program, instruction, uboViews, bufferIndex, nodeIndex);

    // Write uSize in the end, as it shouldn't be overridable by dynamic properties.
    WriteDefaultUniformV2(program->GetDefaultUniform(Program::DefaultUniformIndex::SIZE), uboViews, size);

    commandBuffer.BindUniformBuffers(mUniformBufferBindings);
  }
}

template<class T>
bool Renderer::WriteDefaultUniform(const Graphics::UniformInfo* uniformInfo, Render::UniformBufferView& ubo, const T& data)
{
  if(uniformInfo && !uniformInfo->name.empty())
  {
    WriteUniform(ubo, *uniformInfo, data);
    return true;
  }
  return false;
}

template<class T>
bool Renderer::WriteDefaultUniformV2(const Graphics::UniformInfo* uniformInfo, const std::vector<std::unique_ptr<Render::UniformBufferView>>& uboViews, const T& data)
{
  if(uniformInfo && !uniformInfo->name.empty())
  {
    WriteUniform(*uboViews[uniformInfo->bufferIndex], *uniformInfo, data);
    return true;
  }
  return false;
}

template<class T>
void Renderer::WriteUniform(Render::UniformBufferView& ubo, const Graphics::UniformInfo& uniformInfo, const T& data)
{
  WriteUniform(ubo, uniformInfo, &data, sizeof(T));
}

template<>
void Renderer::WriteUniform<Matrix3>(Render::UniformBufferView& ubo, const Graphics::UniformInfo& uniformInfo, const Matrix3& matrix)
{
  auto     dst       = ubo.GetOffset() + uniformInfo.offset;
  uint32_t rowStride = 3 * sizeof(float); // Gles2 standalone buffer is tightly packed
  if(uniformInfo.bufferIndex > 0)
  {
    rowStride = uniformInfo.matrixStride; // Gles3/Vulkan uniform block, mat3 row is padded to vec4
  }
  for(int i = 0; i < 3; ++i)
  {
    ubo.Write(&matrix.AsFloat()[i * 3],
              sizeof(float) * 3,
              dst + (i * rowStride));
  }
}

void Renderer::WriteUniform(Render::UniformBufferView& ubo, const Graphics::UniformInfo& uniformInfo, const void* data, uint32_t size)
{
  ubo.Write(data, size, ubo.GetOffset() + uniformInfo.offset);
}

void Renderer::FillUniformBuffer(Program&                                                       program,
                                 const SceneGraph::RenderInstruction&                           instruction,
                                 const std::vector<std::unique_ptr<Render::UniformBufferView>>& uboViews,
                                 BufferIndex                                                    updateBufferIndex,
                                 std::size_t                                                    nodeIndex)
{
  for(auto& iter : mUniformIndexMaps[nodeIndex])
  {
    auto& uniform = iter;

    if(!uniform.initialized)
    {
      auto uniformInfo  = Graphics::UniformInfo{};
      auto uniformFound = program.GetUniform(uniform.uniformName.GetStringView(),
                                             uniform.uniformNameHash,
                                             uniform.uniformNameHashNoArray,
                                             uniformInfo);
      if(!uniformFound)
      {
        continue;
      }

      uniform.uniformOffset     = uniformInfo.offset;
      uniform.uniformLocation   = int16_t(uniformInfo.location);
      uniform.uniformBlockIndex = uniformInfo.bufferIndex;
      uniform.initialized       = true;

      const auto typeSize        = iter.propertyValue->GetValueSize();
      uniform.arrayElementStride = uniformInfo.elementCount > 0 ? (uniformInfo.elementStride ? uniformInfo.elementStride : typeSize) : typeSize;
      uniform.matrixStride       = uniformInfo.matrixStride;

      WriteDynUniform(iter.propertyValue, uniform, uboViews, updateBufferIndex);
    }
    else
    {
      WriteDynUniform(iter.propertyValue, uniform, uboViews, updateBufferIndex);
    }
  }
}

void Renderer::WriteDynUniform(
  const PropertyInputImpl*                                       propertyValue,
  UniformIndexMap&                                               uniform,
  const std::vector<std::unique_ptr<Render::UniformBufferView>>& uboViews,
  BufferIndex                                                    updateBufferIndex)
{
  const auto         typeSize   = propertyValue->GetValueSize();
  UniformBufferView* ubo        = uboViews[uniform.uniformBlockIndex].get();
  int                arrayIndex = uniform.arrayIndex;
  auto               dst        = ubo->GetOffset() + uniform.uniformOffset;
  const auto         dest       = dst + uniform.arrayElementStride * arrayIndex;

  const auto valueAddress = propertyValue->GetValueAddress(updateBufferIndex);

  if((propertyValue->GetType() == Property::MATRIX3 || propertyValue->GetType() == Property::VECTOR4) &&
     uniform.matrixStride != uint32_t(-1) &&
     uniform.matrixStride > 0)
  {
    // If the property is Vector4 type and matrixStride is valid integer, then we should treat it as mat2 type uniforms.
    const uint32_t matrixRow = (propertyValue->GetType() == Property::MATRIX3) ? 3 : 2;
    for(uint32_t i = 0; i < matrixRow; ++i)
    {
      ubo->Write(reinterpret_cast<const float*>(valueAddress) + i * matrixRow,
                 sizeof(float) * matrixRow,
                 dest + (i * uniform.matrixStride));
    }
  }
  else
  {
    ubo->Write(valueAddress, typeSize, dest);
  }
}

void Renderer::SetSortAttributes(SceneGraph::RenderInstructionProcessor::SortAttributes& sortAttributes) const
{
  sortAttributes.shader   = &mRenderDataProvider->GetShader();
  sortAttributes.geometry = mGeometry;
}

void Renderer::SetShaderChanged(bool value)
{
  mShaderChanged = value;
}

bool Renderer::Updated()
{
  if(mRenderCallback || mShaderChanged || mGeometry->Updated() || mRenderDataProvider->IsUpdated())
  {
    return true;
  }

  auto* textures = mRenderDataProvider->GetTextures();
  if(textures)
  {
    for(auto iter = textures->Begin(), end = textures->End(); iter < end; ++iter)
    {
      auto texture = *iter;
      if(texture && texture->Updated())
      {
        return true;
      }
    }
  }
  return false;
}

Vector4 Renderer::GetVisualTransformedUpdateArea(BufferIndex bufferIndex, const Vector4& originalUpdateArea) const noexcept
{
  return mRenderDataProvider->GetVisualTransformedUpdateArea(bufferIndex, originalUpdateArea);
}

void Renderer::DetachFromNodeDataProvider(const SceneGraph::NodeDataProvider& node)
{
  // All nodes without uniformMap will share same UniformIndexMap, contains only render data providers.
  // It is special case so we don't need to remove index map in this case. Fast out.
  if(node.GetNodeUniformMap().Count() == 0u)
  {
    return;
  }

  // Remove mNodeIndexMap and mUniformIndexMaps.
  auto iter = std::find_if(mNodeIndexMap.begin(), mNodeIndexMap.end(), [&node](RenderItemLookup& element) { return element.node == &node; });
  while(iter != mNodeIndexMap.end())
  {
    // Swap between end of mUniformIndexMaps and removed.
    auto nodeIndex           = iter->index;
    auto uniformIndexMapSize = mUniformIndexMaps.size();

    // Remove node index map.
    mNodeIndexMap.erase(iter);

    if(nodeIndex + 1 != uniformIndexMapSize)
    {
      std::swap(mUniformIndexMaps[nodeIndex], mUniformIndexMaps[uniformIndexMapSize - 1u]);
      // Change node index map.
      for(auto&& renderItemLookup : mNodeIndexMap)
      {
        if(renderItemLookup.index == uniformIndexMapSize - 1u)
        {
          renderItemLookup.index = nodeIndex;
          break;
        }
      }
    }

    // Remove uniform index maps.
    mUniformIndexMaps.pop_back();

    iter = std::find_if(mNodeIndexMap.begin(), mNodeIndexMap.end(), [&node](RenderItemLookup& element) { return element.node == &node; });
  }
}

void Renderer::ProgramDestroyed(const Program* program)
{
  DALI_ASSERT_ALWAYS(mCurrentProgram == program && "Something wrong happend when Render::Renderer observed by program!");
  mCurrentProgram = nullptr;

  // The cached pipeline might be invalided after program destroyed.
  // Remove current cached pipeline information.
  // Note that reset flag is enought since mPipeline pointer will be invalidate now.
  mPipelineCached = false;

  // Destroy whole mNodeIndexMap and mUniformIndexMaps container.
  // It will be re-created at next render time.
  // Note : Destroy the program will be happened at RenderManager::PostRender().
  //        We don't worry about the mNodeIndexMap and mUniformIndexMaps become invalidated after this call.
  mNodeIndexMap.clear();
  mUniformIndexMaps.clear();
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  mNodeIndexMap.shrink_to_fit();
  mUniformIndexMaps.shrink_to_fit();
#endif
}

Geometry::LifecycleObserver::NotifyReturnType Renderer::GeometryBufferChanged(const Geometry* geometry)
{
  DALI_ASSERT_ALWAYS(mGeometry == geometry && "Something wrong happend when Render::Renderer observed by geometry!");

  // Reset old pipeline
  if(DALI_LIKELY(mPipelineCached))
  {
    mPipelineCache->ResetPipeline(mPipeline);
    mPipelineCached = false;
  }

  return Geometry::LifecycleObserver::NotifyReturnType::KEEP_OBSERVING;
}

void Renderer::GeometryDestroyed(const Geometry* geometry)
{
  // Let just run the same logic with geometry buffer changed cases.
  [[maybe_unused]] auto ret = GeometryBufferChanged(geometry);
  mGeometry                 = nullptr;
}

Vector4 Renderer::GetTextureUpdateArea() const noexcept
{
  Vector4 result = Vector4::ZERO;

  auto* textures = mRenderDataProvider->GetTextures();
  if(textures)
  {
    Rect<uint16_t> updatedArea{};
    uint16_t       width = 0, height = 0;
    for(auto iter = textures->Begin(), end = textures->End(); iter < end; ++iter)
    {
      if(*iter)
      {
        Rect<uint16_t> area = (*iter)->GetUpdatedArea();
        if(updatedArea.IsEmpty())
        {
          updatedArea = area;

          // Assume all texture sizes are same
          width  = (*iter)->GetWidth();
          height = (*iter)->GetHeight();
        }
        else
        {
          // Merge
          updatedArea.Merge(area);
        }
        if(updatedArea.width == width && updatedArea.height == height)
        {
          break; // full area
        }
      }
    }
    result = Vector4(updatedArea.x - width / 2.0f + updatedArea.width / 2.0f, updatedArea.y - height / 2.0f + updatedArea.height / 2.0f, updatedArea.width, updatedArea.height);
  }
  return result;
}

Graphics::Pipeline& Renderer::PrepareGraphicsPipeline(
  Program&                                             program,
  const Dali::Internal::SceneGraph::RenderInstruction& instruction,
  Graphics::RenderTarget*                              renderTarget,
  const SceneGraph::NodeDataProvider&                  node,
  bool                                                 blend)
{
  // Prepare query info
  PipelineCacheQueryInfo queryInfo{};
  queryInfo.program               = &program;
  queryInfo.renderer              = this;
  queryInfo.geometry              = mGeometry;
  queryInfo.blendingEnabled       = blend;
  queryInfo.blendingOptions       = &mBlendingOptions;
  queryInfo.alphaPremultiplied    = mPremultipliedAlphaEnabled;
  queryInfo.cameraUsingReflection = instruction.GetCamera()->GetReflectionUsed();
  queryInfo.renderTarget          = renderTarget;

  queryInfo.GenerateHash();

  // Reset old pipeline
  if(DALI_LIKELY(mPipelineCached))
  {
    mPipelineCache->ResetPipeline(mPipeline);
    mPipelineCached = false;
  }

  // Find or generate new pipeline.
  auto pipelineResult = mPipelineCache->GetPipeline(queryInfo, true);

  mPipeline       = pipelineResult.level2;
  mPipelineCached = true;

  // should be never null?
  return *pipelineResult.pipeline;
}

void Renderer::SetRenderCallback(RenderCallback* callback)
{
  mRenderCallback = callback;
}

} // namespace Render

} // namespace Dali::Internal
