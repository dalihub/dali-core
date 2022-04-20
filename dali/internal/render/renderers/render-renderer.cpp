/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

} // namespace

namespace Render
{
Renderer* Renderer::New(SceneGraph::RenderDataProvider* dataProvider,
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
  return new Renderer(dataProvider, geometry, blendingBitmask, blendColor, faceCullingMode, preMultipliedAlphaEnabled, depthWriteMode, depthTestMode, depthFunction, stencilParameters);
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
  mUniformIndexMap(),
  mUniformsHash(),
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
  mUpdated(true)
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

void Renderer::SetGeometry(Render::Geometry* geometry)
{
  mGeometry = geometry;
  mUpdated  = true;
}
void Renderer::SetDrawCommands(Dali::DevelRenderer::DrawCommand* pDrawCommands, uint32_t size)
{
  mDrawCommands.clear();
  mDrawCommands.insert(mDrawCommands.end(), pDrawCommands, pDrawCommands + size);
}

void Renderer::BindTextures(Graphics::CommandBuffer& commandBuffer, Vector<Graphics::Texture*>& boundTextures)
{
  uint32_t textureUnit = 0;

  const Dali::Vector<Render::Texture*>* textures(mRenderDataProvider->GetTextures());
  const Dali::Vector<Render::Sampler*>* samplers(mRenderDataProvider->GetSamplers());

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
  mUpdated         = true;
}

void Renderer::SetBlendingBitMask(uint32_t bitmask)
{
  mBlendingOptions.SetBitmask(bitmask);
  mUpdated = true;
}

void Renderer::SetBlendColor(const Vector4& color)
{
  mBlendingOptions.SetBlendColor(color);
  mUpdated = true;
}

void Renderer::SetIndexedDrawFirstElement(uint32_t firstElement)
{
  mIndexedDrawFirstElement = firstElement;
  mUpdated                 = true;
}

void Renderer::SetIndexedDrawElementsCount(uint32_t elementsCount)
{
  mIndexedDrawElementsCount = elementsCount;
  mUpdated                  = true;
}

void Renderer::EnablePreMultipliedAlpha(bool enable)
{
  mPremultipliedAlphaEnabled = enable;
  mUpdated                   = true;
}

void Renderer::SetDepthWriteMode(DepthWriteMode::Type depthWriteMode)
{
  mDepthWriteMode = depthWriteMode;
  mUpdated        = true;
}

void Renderer::SetDepthTestMode(DepthTestMode::Type depthTestMode)
{
  mDepthTestMode = depthTestMode;
  mUpdated       = true;
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
  mUpdated       = true;
}

DepthFunction::Type Renderer::GetDepthFunction() const
{
  return mDepthFunction;
}

void Renderer::SetRenderMode(RenderMode::Type renderMode)
{
  mStencilParameters.renderMode = renderMode;
  mUpdated                      = true;
}

RenderMode::Type Renderer::GetRenderMode() const
{
  return mStencilParameters.renderMode;
}

void Renderer::SetStencilFunction(StencilFunction::Type stencilFunction)
{
  mStencilParameters.stencilFunction = stencilFunction;
  mUpdated                           = true;
}

StencilFunction::Type Renderer::GetStencilFunction() const
{
  return mStencilParameters.stencilFunction;
}

void Renderer::SetStencilFunctionMask(int stencilFunctionMask)
{
  mStencilParameters.stencilFunctionMask = stencilFunctionMask;
  mUpdated                               = true;
}

int Renderer::GetStencilFunctionMask() const
{
  return mStencilParameters.stencilFunctionMask;
}

void Renderer::SetStencilFunctionReference(int stencilFunctionReference)
{
  mStencilParameters.stencilFunctionReference = stencilFunctionReference;
  mUpdated                                    = true;
}

int Renderer::GetStencilFunctionReference() const
{
  return mStencilParameters.stencilFunctionReference;
}

void Renderer::SetStencilMask(int stencilMask)
{
  mStencilParameters.stencilMask = stencilMask;
  mUpdated                       = true;
}

int Renderer::GetStencilMask() const
{
  return mStencilParameters.stencilMask;
}

void Renderer::SetStencilOperationOnFail(StencilOperation::Type stencilOperationOnFail)
{
  mStencilParameters.stencilOperationOnFail = stencilOperationOnFail;
  mUpdated                                  = true;
}

StencilOperation::Type Renderer::GetStencilOperationOnFail() const
{
  return mStencilParameters.stencilOperationOnFail;
}

void Renderer::SetStencilOperationOnZFail(StencilOperation::Type stencilOperationOnZFail)
{
  mStencilParameters.stencilOperationOnZFail = stencilOperationOnZFail;
  mUpdated                                   = true;
}

StencilOperation::Type Renderer::GetStencilOperationOnZFail() const
{
  return mStencilParameters.stencilOperationOnZFail;
}

void Renderer::SetStencilOperationOnZPass(StencilOperation::Type stencilOperationOnZPass)
{
  mStencilParameters.stencilOperationOnZPass = stencilOperationOnZPass;
  mUpdated                                   = true;
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
    Graphics::DrawNativeInfo info{};
    info.api      = Graphics::DrawNativeAPI::GLES;
    info.callback = &static_cast<Dali::CallbackBase&>(*mRenderCallback);
    info.userData = &mRenderCallbackInput;
    info.reserved = nullptr;

    // pass render callback input
    mRenderCallbackInput.size       = size;
    mRenderCallbackInput.projection = projectionMatrix;
    Matrix::Multiply(mRenderCallbackInput.mvp, modelViewMatrix, projectionMatrix);

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

  BuildUniformIndexMap(bufferIndex, node, size, *program);

  WriteUniformBuffer(bufferIndex, commandBuffer, program, instruction, node, modelMatrix, modelViewMatrix, viewMatrix, projectionMatrix, size);

  bool drawn = false; // Draw can fail if there are no vertex buffers or they haven't been uploaded yet
                      // @todo We should detect this case much earlier to prevent unnecessary work

  if(mDrawCommands.empty())
  {
    drawn = mGeometry->Draw(*mGraphicsController, commandBuffer, mIndexedDrawFirstElement, mIndexedDrawElementsCount);
  }
  else
  {
    for(auto& cmd : commands)
    {
      mGeometry->Draw(*mGraphicsController, commandBuffer, cmd->firstIndex, cmd->elementCount);
    }
  }

  mUpdated = false;
  return drawn;
}

void Renderer::BuildUniformIndexMap(BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, const Vector3& size, Program& program)
{
  // Check if the map has changed
  DALI_ASSERT_DEBUG(mRenderDataProvider && "No Uniform map data provider available");

  const SceneGraph::UniformMapDataProvider& uniformMapDataProvider = mRenderDataProvider->GetUniformMapDataProvider();

  if(uniformMapDataProvider.GetUniformMapChanged(bufferIndex) ||
     node.GetUniformMapChanged(bufferIndex) ||
     mUniformIndexMap.Count() == 0 ||
     mShaderChanged)
  {
    // Reset shader pointer
    mShaderChanged = false;

    const SceneGraph::CollectedUniformMap& uniformMap     = uniformMapDataProvider.GetUniformMap(bufferIndex);
    const SceneGraph::CollectedUniformMap& uniformMapNode = node.GetUniformMap(bufferIndex);

    auto maxMaps = static_cast<uint32_t>(uniformMap.Count() + uniformMapNode.Count()); // 4,294,967,295 maps should be enough
    mUniformIndexMap.Clear();                                                          // Clear contents, but keep memory if we don't change size
    mUniformIndexMap.Resize(maxMaps);

    // Copy uniform map into mUniformIndexMap
    uint32_t mapIndex = 0;
    for(; mapIndex < uniformMap.Count(); ++mapIndex)
    {
      mUniformIndexMap[mapIndex].propertyValue          = uniformMap[mapIndex].propertyPtr;
      mUniformIndexMap[mapIndex].uniformName            = uniformMap[mapIndex].uniformName;
      mUniformIndexMap[mapIndex].uniformNameHash        = uniformMap[mapIndex].uniformNameHash;
      mUniformIndexMap[mapIndex].uniformNameHashNoArray = uniformMap[mapIndex].uniformNameHashNoArray;
      mUniformIndexMap[mapIndex].arrayIndex             = uniformMap[mapIndex].arrayIndex;
    }

    for(uint32_t nodeMapIndex = 0; nodeMapIndex < uniformMapNode.Count(); ++nodeMapIndex)
    {
      auto  hash = uniformMapNode[nodeMapIndex].uniformNameHash;
      auto& name = uniformMapNode[nodeMapIndex].uniformName;
      bool  found(false);
      for(uint32_t i = 0; i < uniformMap.Count(); ++i)
      {
        if(mUniformIndexMap[i].uniformNameHash == hash &&
           mUniformIndexMap[i].uniformName == name)
        {
          mUniformIndexMap[i].propertyValue = uniformMapNode[nodeMapIndex].propertyPtr;
          found                             = true;
          break;
        }
      }

      if(!found)
      {
        mUniformIndexMap[mapIndex].propertyValue          = uniformMapNode[nodeMapIndex].propertyPtr;
        mUniformIndexMap[mapIndex].uniformName            = uniformMapNode[nodeMapIndex].uniformName;
        mUniformIndexMap[mapIndex].uniformNameHash        = uniformMapNode[nodeMapIndex].uniformNameHash;
        mUniformIndexMap[mapIndex].uniformNameHashNoArray = uniformMapNode[nodeMapIndex].uniformNameHashNoArray;
        mUniformIndexMap[mapIndex].arrayIndex             = uniformMapNode[nodeMapIndex].arrayIndex;
        ++mapIndex;
      }
    }

    mUniformIndexMap.Resize(mapIndex);
  }
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
  const Vector3&                       size)
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

    uboView = uboPoolView->CreateUniformBufferView(uniformBlockAllocationBytes);
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
      Matrix::Multiply(modelViewProjectionMatrix, modelViewMatrix, projectionMatrix);
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
    FillUniformBuffer(*program, instruction, *uboView, bindings, uboOffset, bufferIndex);

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
                                 BufferIndex                                   updateBufferIndex)
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

    for(UniformIndexMappings::Iterator iter = mUniformIndexMap.Begin(),
                                       end  = mUniformIndexMap.End();
        iter != end;
        ++iter)
    {
      auto& uniform    = *iter;
      int   arrayIndex = uniform.arrayIndex;

      if(!uniform.uniformFunc)
      {
        auto uniformInfo  = Graphics::UniformInfo{};
        auto uniformFound = program.GetUniform(uniform.uniformName.GetCString(),
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

bool Renderer::Updated(BufferIndex bufferIndex, const SceneGraph::NodeDataProvider* node)
{
  if(mUpdated)
  {
    mUpdated = false;
    return true;
  }

  if(mRenderCallback || mShaderChanged || mGeometry->AttributesChanged())
  {
    return true;
  }

  auto* textures = mRenderDataProvider->GetTextures();
  if(textures)
  {
    for(auto iter = textures->Begin(), end = textures->End(); iter < end; ++iter)
    {
      auto texture = *iter;
      if(texture && texture->IsNativeImage())
      {
        return true;
      }
    }
  }

  uint64_t                               hash           = 0xc70f6907UL;
  const SceneGraph::CollectedUniformMap& uniformMapNode = node->GetUniformMap(bufferIndex);
  for(const auto& uniformProperty : uniformMapNode)
  {
    hash = uniformProperty.propertyPtr->Hash(bufferIndex, hash);
  }

  const SceneGraph::UniformMapDataProvider& uniformMapDataProvider = mRenderDataProvider->GetUniformMapDataProvider();
  const SceneGraph::CollectedUniformMap&    uniformMap             = uniformMapDataProvider.GetUniformMap(bufferIndex);
  for(const auto& uniformProperty : uniformMap)
  {
    hash = uniformProperty.propertyPtr->Hash(bufferIndex, hash);
  }

  if(mUniformsHash != hash)
  {
    mUniformsHash = hash;
    return true;
  }

  return false;
}

Graphics::Pipeline& Renderer::PrepareGraphicsPipeline(
  Program&                                             program,
  const Dali::Internal::SceneGraph::RenderInstruction& instruction,
  const SceneGraph::NodeDataProvider&                  node,
  bool                                                 blend)
{
  if(mGeometry->AttributesChanged())
  {
    mUpdated = true;
  }

  // Prepare query info
  PipelineCacheQueryInfo queryInfo{};
  queryInfo.program               = &program;
  queryInfo.renderer              = this;
  queryInfo.geometry              = mGeometry;
  queryInfo.blendingEnabled       = blend;
  queryInfo.blendingOptions       = &mBlendingOptions;
  queryInfo.alphaPremultiplied    = mPremultipliedAlphaEnabled;
  queryInfo.cameraUsingReflection = instruction.GetCamera()->GetReflectionUsed();

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
