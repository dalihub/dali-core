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
#include <dali/internal/render/renderers/shader-cache.h>
#include <dali/internal/render/renderers/uniform-buffer-view-pool.h>
#include <dali/internal/render/renderers/uniform-buffer-view.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/render-shader.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/public-api/signals/render-callback.h>

namespace Dali::Internal
{
namespace
{
// Helper to get the property value getter by type
typedef const float& (PropertyInputImpl::*FuncGetter)(BufferIndex) const;
constexpr FuncGetter GetPropertyValueGetter(Property::Type type)
{
  switch(type)
  {
    case Property::BOOLEAN:
    {
      return FuncGetter(&PropertyInputImpl::GetBoolean);
    }
    case Property::INTEGER:
    {
      return FuncGetter(&PropertyInputImpl::GetInteger);
    }
    case Property::FLOAT:
    {
      return FuncGetter(&PropertyInputImpl::GetFloat);
    }
    case Property::VECTOR2:
    {
      return FuncGetter(&PropertyInputImpl::GetVector2);
    }
    case Property::VECTOR3:
    {
      return FuncGetter(&PropertyInputImpl::GetVector3);
    }
    case Property::VECTOR4:
    {
      return FuncGetter(&PropertyInputImpl::GetVector4);
    }
    case Property::MATRIX3:
    {
      return FuncGetter(&PropertyInputImpl::GetMatrix3);
    }
    case Property::MATRIX:
    {
      return FuncGetter(&PropertyInputImpl::GetMatrix);
    }
    default:
    {
      return nullptr;
    }
  }
}

/**
 * Helper function that returns size of uniform datatypes based
 * on property type.
 */
constexpr int GetPropertyValueSizeForUniform(Property::Type type)
{
  switch(type)
  {
    case Property::Type::BOOLEAN:
    {
      return sizeof(bool);
    }
    case Property::Type::FLOAT:
    {
      return sizeof(float);
    }
    case Property::Type::INTEGER:
    {
      return sizeof(int);
    }
    case Property::Type::VECTOR2:
    {
      return sizeof(Vector2);
    }
    case Property::Type::VECTOR3:
    {
      return sizeof(Vector3);
    }
    case Property::Type::VECTOR4:
    {
      return sizeof(Vector4);
    }
    case Property::Type::MATRIX3:
    {
      return sizeof(Matrix3);
    }
    case Property::Type::MATRIX:
    {
      return sizeof(Matrix);
    }
    default:
    {
      return 0;
    }
  };
}

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
MemoryPoolObjectAllocator<Renderer>& GetRenderRendererMemoryPool()
{
  static MemoryPoolObjectAllocator<Renderer> gRenderRendererMemoryPool;
  return gRenderRendererMemoryPool;
}
}

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
  mShaderChanged(false)
{
  if(blendingBitmask != 0u)
  {
    mBlendingOptions.SetBitmask(blendingBitmask);
  }

  mBlendingOptions.SetBlendColor(blendColor);
}

void Renderer::Initialize(Graphics::Controller& graphicsController, ProgramCache& programCache, Render::ShaderCache& shaderCache, Render::UniformBufferManager& uniformBufferManager, Render::PipelineCache& pipelineCache)
{
  mGraphicsController   = &graphicsController;
  mProgramCache         = &programCache;
  mShaderCache          = &shaderCache;
  mUniformBufferManager = &uniformBufferManager;
  mPipelineCache        = &pipelineCache;
}

Renderer::~Renderer() = default;

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
  mGeometry = geometry;
}

void Renderer::SetDrawCommands(Dali::DevelRenderer::DrawCommand* pDrawCommands, uint32_t size)
{
  mDrawCommands.clear();
  mDrawCommands.insert(mDrawCommands.end(), pDrawCommands, pDrawCommands + size);
}

void Renderer::BindTextures(Graphics::CommandBuffer& commandBuffer, Vector<Graphics::Texture*>& boundTextures)
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

        const Graphics::Sampler* graphicsSampler = samplers ? ((*samplers)[i] ? (*samplers)[i]->GetGraphicsObject()
                                                                              : nullptr)
                                                            : nullptr;

        boundTextures.PushBack(graphicsTexture);
        const Graphics::TextureBinding textureBinding{graphicsTexture, graphicsSampler, textureUnit};
        textureBindings.push_back(textureBinding);

        ++textureUnit;
      }
    }
  }

  if(!textureBindings.empty())
  {
    commandBuffer.BindTextures(textureBindings);
  }
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

bool Renderer::Render(Graphics::CommandBuffer&                             commandBuffer,
                      BufferIndex                                          bufferIndex,
                      const SceneGraph::NodeDataProvider&                  node,
                      const Matrix&                                        modelMatrix,
                      const Matrix&                                        modelViewMatrix,
                      const Matrix&                                        viewMatrix,
                      const Matrix&                                        projectionMatrix,
                      const Vector3&                                       size,
                      bool                                                 blend,
                      Vector<Graphics::Texture*>&                          boundTextures,
                      const Dali::Internal::SceneGraph::RenderInstruction& instruction,
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

    Graphics::DrawNativeInfo info{};
    info.api      = Graphics::DrawNativeAPI::GLES;
    info.callback = &static_cast<Dali::CallbackBase&>(*mRenderCallback);
    info.userData = mRenderCallbackInput.get();

    // Set storage for the context to be used
    info.glesNativeInfo.eglSharedContextStoragePointer = &mRenderCallbackInput->eglContext;
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

  // Create Program
  ShaderDataPtr shaderData = mRenderDataProvider->GetShader().GetShaderData();

  Program* program = Program::New(*mProgramCache,
                                  shaderData,
                                  *mGraphicsController);
  if(!program)
  {
    DALI_LOG_ERROR("Failed to get program for shader at address %p.\n", reinterpret_cast<const void*>(&mRenderDataProvider->GetShader()));
    return false;
  }

  // If program doesn't have Gfx program object assigned yet, prepare it.
  if(!program->GetGraphicsProgramPtr())
  {
    const std::vector<char>& vertShader   = shaderData->GetShaderForPipelineStage(Graphics::PipelineStage::VERTEX_SHADER);
    const std::vector<char>& fragShader   = shaderData->GetShaderForPipelineStage(Graphics::PipelineStage::FRAGMENT_SHADER);
    Dali::Graphics::Shader&  vertexShader = mShaderCache->GetShader(
      vertShader,
      Graphics::PipelineStage::VERTEX_SHADER,
      shaderData->GetSourceMode());

    Dali::Graphics::Shader& fragmentShader = mShaderCache->GetShader(
      fragShader,
      Graphics::PipelineStage::FRAGMENT_SHADER,
      shaderData->GetSourceMode());

    std::vector<Graphics::ShaderState> shaderStates{
      Graphics::ShaderState()
        .SetShader(vertexShader)
        .SetPipelineStage(Graphics::PipelineStage::VERTEX_SHADER),
      Graphics::ShaderState()
        .SetShader(fragmentShader)
        .SetPipelineStage(Graphics::PipelineStage::FRAGMENT_SHADER)};

    auto createInfo = Graphics::ProgramCreateInfo();
    createInfo.SetShaderState(shaderStates);
    auto graphicsProgram = mGraphicsController->CreateProgram(createInfo, nullptr);
    program->SetGraphicsProgram(std::move(graphicsProgram));
  }

  // Prepare the graphics pipeline. This may either re-use an existing pipeline or create a new one.
  auto& pipeline = PrepareGraphicsPipeline(*program, instruction, node, blend);

  commandBuffer.BindPipeline(pipeline);

  BindTextures(commandBuffer, boundTextures);

  std::size_t nodeIndex = BuildUniformIndexMap(bufferIndex, node, size, *program);

  WriteUniformBuffer(bufferIndex, commandBuffer, program, instruction, node, modelMatrix, modelViewMatrix, viewMatrix, projectionMatrix, size, nodeIndex);

  bool drawn = false; // Draw can fail if there are no vertex buffers or they haven't been uploaded yet
                      // @todo We should detect this case much earlier to prevent unnecessary work

  // Reuse latest bound vertex attributes location, or Bind buffers to attribute locations.
  if(ReuseLatestBoundVertexAttributes(mGeometry) || mGeometry->BindVertexAttributes(commandBuffer))
  {
    if(mDrawCommands.empty())
    {
      drawn = mGeometry->Draw(*mGraphicsController, commandBuffer, mIndexedDrawFirstElement, mIndexedDrawElementsCount);
    }
    else
    {
      for(auto& cmd : commands)
      {
        drawn |= mGeometry->Draw(*mGraphicsController, commandBuffer, cmd->firstIndex, cmd->elementCount);
      }
    }
  }
  else
  {
    // BindVertexAttributes failed. Reset cached geometry.
    ReuseLatestBoundVertexAttributes(nullptr);
  }

  return drawn;
}

std::size_t Renderer::BuildUniformIndexMap(BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, const Vector3& size, Program& program)
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
  const auto nodePtr = uniformMapNode.Count() ? &node : nullptr;

  const auto nodeChangeCounter          = nodePtr ? uniformMapNode.GetChangeCounter() : 0;
  const auto renderItemMapChangeCounter = uniformMap.GetChangeCounter();

  auto iter = std::find_if(mNodeIndexMap.begin(), mNodeIndexMap.end(), [nodePtr](RenderItemLookup& element) { return element.node == nodePtr; });

  std::size_t renderItemMapIndex;
  if(iter == mNodeIndexMap.end())
  {
    renderItemMapIndex = mUniformIndexMaps.size();
    RenderItemLookup renderItemLookup;
    renderItemLookup.node                       = nodePtr;
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
  const SceneGraph::NodeDataProvider&  node,
  const Matrix&                        modelMatrix,
  const Matrix&                        modelViewMatrix,
  const Matrix&                        viewMatrix,
  const Matrix&                        projectionMatrix,
  const Vector3&                       size,
  std::size_t                          nodeIndex)
{
  // Create the UBO
  uint32_t uboOffset{0u};

  auto& reflection = mGraphicsController->GetProgramReflection(program->GetGraphicsProgram());

  uint32_t uniformBlockAllocationBytes = program->GetUniformBlocksMemoryRequirements().totalSizeRequired;

  // Create uniform buffer view from uniform buffer
  Graphics::UniquePtr<Render::UniformBufferView> uboView{nullptr};
  if(uniformBlockAllocationBytes)
  {
    auto uboPoolView = mUniformBufferManager->GetUniformBufferViewPool(bufferIndex);
    uboView          = uboPoolView->CreateUniformBufferView(uniformBlockAllocationBytes);
  }

  // update the uniform buffer
  // pass shared UBO and offset, return new offset for next item to be used
  // don't process bindings if there are no uniform buffers allocated
  if(uboView)
  {
    auto uboCount = reflection.GetUniformBlockCount();
    mUniformBufferBindings.resize(uboCount);

    std::vector<Graphics::UniformBufferBinding>* bindings{&mUniformBufferBindings};

    mUniformBufferBindings[0].buffer = uboView->GetBuffer(&mUniformBufferBindings[0].offset);

    // Write default uniforms
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::MODEL_MATRIX), *uboView, modelMatrix);
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::VIEW_MATRIX), *uboView, viewMatrix);
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::PROJECTION_MATRIX), *uboView, projectionMatrix);
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::MODEL_VIEW_MATRIX), *uboView, modelViewMatrix);

    auto mvpUniformInfo = program->GetDefaultUniform(Program::DefaultUniformIndex::MVP_MATRIX);
    if(mvpUniformInfo && !mvpUniformInfo->name.empty())
    {
      Matrix modelViewProjectionMatrix(false);
      MatrixUtils::MultiplyProjectionMatrix(modelViewProjectionMatrix, modelViewMatrix, projectionMatrix);
      WriteDefaultUniform(mvpUniformInfo, *uboView, modelViewProjectionMatrix);
    }

    auto normalUniformInfo = program->GetDefaultUniform(Program::DefaultUniformIndex::NORMAL_MATRIX);
    if(normalUniformInfo && !normalUniformInfo->name.empty())
    {
      Matrix3 normalMatrix(modelViewMatrix);
      normalMatrix.Invert();
      normalMatrix.Transpose();
      WriteDefaultUniform(normalUniformInfo, *uboView, normalMatrix);
    }

    Vector4        finalColor;                               ///< Applied renderer's opacity color
    const Vector4& color = node.GetRenderColor(bufferIndex); ///< Actor's original color
    if(mPremultipliedAlphaEnabled)
    {
      const float& alpha = color.a * mRenderDataProvider->GetOpacity(bufferIndex);
      finalColor         = Vector4(color.r * alpha, color.g * alpha, color.b * alpha, alpha);
    }
    else
    {
      finalColor = Vector4(color.r, color.g, color.b, color.a * mRenderDataProvider->GetOpacity(bufferIndex));
    }
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::COLOR), *uboView, finalColor);
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::ACTOR_COLOR), *uboView, color);

    // Write uniforms from the uniform map
    FillUniformBuffer(*program, instruction, *uboView, bindings, uboOffset, bufferIndex, nodeIndex);

    // Write uSize in the end, as it shouldn't be overridable by dynamic properties.
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::SIZE), *uboView, size);

    commandBuffer.BindUniformBuffers(*bindings);
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
void Renderer::WriteUniform(Render::UniformBufferView& ubo, const Graphics::UniformInfo& uniformInfo, const T& data)
{
  WriteUniform(ubo, uniformInfo, &data, sizeof(T));
}

void Renderer::WriteUniform(Render::UniformBufferView& ubo, const Graphics::UniformInfo& uniformInfo, const void* data, uint32_t size)
{
  ubo.Write(data, size, ubo.GetOffset() + uniformInfo.offset);
}

void Renderer::FillUniformBuffer(Program&                                      program,
                                 const SceneGraph::RenderInstruction&          instruction,
                                 Render::UniformBufferView&                    ubo,
                                 std::vector<Graphics::UniformBufferBinding>*& outBindings,
                                 uint32_t&                                     offset,
                                 BufferIndex                                   updateBufferIndex,
                                 std::size_t                                   nodeIndex)
{
  auto& reflection = mGraphicsController->GetProgramReflection(program.GetGraphicsProgram());
  auto  uboCount   = reflection.GetUniformBlockCount();

  // Setup bindings
  uint32_t dataOffset = offset;
  for(auto i = 0u; i < uboCount; ++i)
  {
    mUniformBufferBindings[i].dataSize = reflection.GetUniformBlockSize(i);
    mUniformBufferBindings[i].binding  = reflection.GetUniformBlockBinding(i);

    dataOffset += GetUniformBufferDataAlignment(mUniformBufferBindings[i].dataSize);
    mUniformBufferBindings[i].buffer = ubo.GetBuffer(&mUniformBufferBindings[i].offset);

    for(auto iter = mUniformIndexMaps[nodeIndex].begin(),
             end  = mUniformIndexMaps[nodeIndex].end();
        iter != end;
        ++iter)
    {
      auto& uniform    = *iter;
      int   arrayIndex = uniform.arrayIndex;

      if(!uniform.uniformFunc)
      {
        auto uniformInfo  = Graphics::UniformInfo{};
        auto uniformFound = program.GetUniform(uniform.uniformName.GetStringView(),
                                               uniform.uniformNameHash,
                                               uniform.uniformNameHashNoArray,
                                               uniformInfo);

        uniform.uniformOffset   = uniformInfo.offset;
        uniform.uniformLocation = uniformInfo.location;

        if(uniformFound)
        {
          auto       dst      = ubo.GetOffset() + uniformInfo.offset;
          const auto typeSize = GetPropertyValueSizeForUniform((*iter).propertyValue->GetType());
          const auto dest     = dst + static_cast<uint32_t>(typeSize) * arrayIndex;
          const auto func     = GetPropertyValueGetter((*iter).propertyValue->GetType());

          ubo.Write(&((*iter).propertyValue->*func)(updateBufferIndex),
                    typeSize,
                    dest);

          uniform.uniformSize = typeSize;
          uniform.uniformFunc = func;
        }
      }
      else
      {
        auto       dst      = ubo.GetOffset() + uniform.uniformOffset;
        const auto typeSize = uniform.uniformSize;
        const auto dest     = dst + static_cast<uint32_t>(typeSize) * arrayIndex;
        const auto func     = uniform.uniformFunc;

        ubo.Write(&((*iter).propertyValue->*func)(updateBufferIndex),
                  typeSize,
                  dest);
      }
    }
  }
  // write output bindings
  outBindings = &mUniformBufferBindings;

  // Update offset
  offset = dataOffset;
}

void Renderer::SetSortAttributes(SceneGraph::RenderInstructionProcessor::SortAttributes& sortAttributes) const
{
  sortAttributes.shader   = &(mRenderDataProvider->GetShader());
  sortAttributes.geometry = mGeometry;
}

void Renderer::SetShaderChanged(bool value)
{
  mShaderChanged = value;
}

bool Renderer::Updated(BufferIndex bufferIndex)
{
  if(mRenderCallback || mShaderChanged || mGeometry->AttributesChanged() || mRenderDataProvider->IsUpdated())
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

Graphics::Pipeline& Renderer::PrepareGraphicsPipeline(
  Program&                                             program,
  const Dali::Internal::SceneGraph::RenderInstruction& instruction,
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

  queryInfo.GenerateHash();

  // Find or generate new pipeline.
  auto pipelineResult = mPipelineCache->GetPipeline(queryInfo, true);

  // should be never null?
  return *pipelineResult.pipeline;
}

void Renderer::SetRenderCallback(RenderCallback* callback)
{
  mRenderCallback = callback;
}

} // namespace Render

} // namespace Dali::Internal
