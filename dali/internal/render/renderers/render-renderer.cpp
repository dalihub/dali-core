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
#include <dali/internal/render/renderers/render-renderer.h>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-program.h>
#include <dali/graphics-api/graphics-types.h>
#include <dali/internal/common/image-sampler.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/render/data-providers/uniform-map-data-provider.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/internal/render/renderers/render-texture.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/internal/render/renderers/shader-cache.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/update/common/uniform-map.h>

namespace Dali
{
namespace Internal
{
namespace
{
// Size of uniform buffer page used when resizing
constexpr uint32_t UBO_PAGE_SIZE = 8192u;

// UBO allocation threshold below which the UBO will shrink
constexpr auto UBO_SHRINK_THRESHOLD = 0.75f;

// Helper to get the vertex input format
Dali::Graphics::VertexInputFormat GetPropertyVertexFormat(Property::Type propertyType)
{
  Dali::Graphics::VertexInputFormat type{};

  switch(propertyType)
  {
    case Property::NONE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::EXTENTS:   // i4?
    case Property::RECTANGLE: // i4/f4?
    case Property::ROTATION:
    {
      type = Dali::Graphics::VertexInputFormat::UNDEFINED;
      break;
    }
    case Property::BOOLEAN:
    {
      type = Dali::Graphics::VertexInputFormat::UNDEFINED; // type = GL_BYTE; @todo new type for this?
      break;
    }
    case Property::INTEGER:
    {
      type = Dali::Graphics::VertexInputFormat::INTEGER; // (short)
      break;
    }
    case Property::FLOAT:
    {
      type = Dali::Graphics::VertexInputFormat::FLOAT;
      break;
    }
    case Property::VECTOR2:
    {
      type = Dali::Graphics::VertexInputFormat::FVECTOR2;
      break;
    }
    case Property::VECTOR3:
    {
      type = Dali::Graphics::VertexInputFormat::FVECTOR3;
      break;
    }
    case Property::VECTOR4:
    {
      type = Dali::Graphics::VertexInputFormat::FVECTOR4;
      break;
    }
    case Property::MATRIX3:
    {
      type = Dali::Graphics::VertexInputFormat::FLOAT;
      break;
    }
    case Property::MATRIX:
    {
      type = Dali::Graphics::VertexInputFormat::FLOAT;
      break;
    }
  }

  return type;
}

constexpr Graphics::CullMode ConvertCullFace(Dali::FaceCullingMode::Type mode)
{
  switch(mode)
  {
    case Dali::FaceCullingMode::NONE:
    {
      return Graphics::CullMode::NONE;
    }
    case Dali::FaceCullingMode::FRONT:
    {
      return Graphics::CullMode::FRONT;
    }
    case Dali::FaceCullingMode::BACK:
    {
      return Graphics::CullMode::BACK;
    }
    case Dali::FaceCullingMode::FRONT_AND_BACK:
    {
      return Graphics::CullMode::FRONT_AND_BACK;
    }
  }
  return Graphics::CullMode::NONE;
}

constexpr Graphics::BlendFactor ConvertBlendFactor(BlendFactor::Type blendFactor)
{
  switch(blendFactor)
  {
    case BlendFactor::ZERO:
      return Graphics::BlendFactor::ZERO;
    case BlendFactor::ONE:
      return Graphics::BlendFactor::ONE;
    case BlendFactor::SRC_COLOR:
      return Graphics::BlendFactor::SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_SRC_COLOR;
    case BlendFactor::SRC_ALPHA:
      return Graphics::BlendFactor::SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DST_ALPHA:
      return Graphics::BlendFactor::DST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_DST_ALPHA;
    case BlendFactor::DST_COLOR:
      return Graphics::BlendFactor::DST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_DST_COLOR;
    case BlendFactor::SRC_ALPHA_SATURATE:
      return Graphics::BlendFactor::SRC_ALPHA_SATURATE;
    case BlendFactor::CONSTANT_COLOR:
      return Graphics::BlendFactor::CONSTANT_COLOR;
    case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::CONSTANT_ALPHA:
      return Graphics::BlendFactor::CONSTANT_ALPHA;
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_CONSTANT_ALPHA;
  }
  return Graphics::BlendFactor{};
}

constexpr Graphics::BlendOp ConvertBlendEquation(DevelBlendEquation::Type blendEquation)
{
  switch(blendEquation)
  {
    case DevelBlendEquation::ADD:
      return Graphics::BlendOp::ADD;
    case DevelBlendEquation::SUBTRACT:
      return Graphics::BlendOp::SUBTRACT;
    case DevelBlendEquation::REVERSE_SUBTRACT:
      return Graphics::BlendOp::REVERSE_SUBTRACT;
    case DevelBlendEquation::COLOR:
    case DevelBlendEquation::COLOR_BURN:
    case DevelBlendEquation::COLOR_DODGE:
    case DevelBlendEquation::DARKEN:
    case DevelBlendEquation::DIFFERENCE:
    case DevelBlendEquation::EXCLUSION:
    case DevelBlendEquation::HARD_LIGHT:
    case DevelBlendEquation::HUE:
    case DevelBlendEquation::LIGHTEN:
    case DevelBlendEquation::LUMINOSITY:
    case DevelBlendEquation::MAX:
    case DevelBlendEquation::MIN:
    case DevelBlendEquation::MULTIPLY:
    case DevelBlendEquation::OVERLAY:
    case DevelBlendEquation::SATURATION:
    case DevelBlendEquation::SCREEN:
    case DevelBlendEquation::SOFT_LIGHT:
      return Graphics::BlendOp{};
  }
  return Graphics::BlendOp{};
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
  mContext(nullptr),
  mGeometry(geometry),
  mProgramCache(nullptr),
  mUniformIndexMap(),
  mAttributeLocations(),
  mUniformsHash(),
  mStencilParameters(stencilParameters),
  mBlendingOptions(),
  mIndexedDrawFirstElement(0),
  mIndexedDrawElementsCount(0),
  mDepthFunction(depthFunction),
  mFaceCullingMode(faceCullingMode),
  mDepthWriteMode(depthWriteMode),
  mDepthTestMode(depthTestMode),
  mUpdateAttributeLocations(true),
  mPremultipledAlphaEnabled(preMultipliedAlphaEnabled),
  mShaderChanged(false),
  mUpdated(true)
{
  if(blendingBitmask != 0u)
  {
    mBlendingOptions.SetBitmask(blendingBitmask);
  }

  mBlendingOptions.SetBlendColor(blendColor);
}

void Renderer::Initialize(Context& context, Graphics::Controller& graphicsController, ProgramCache& programCache, Render::ShaderCache& shaderCache, Render::UniformBufferManager& uniformBufferManager)
{
  mContext              = &context;
  mGraphicsController   = &graphicsController;
  mProgramCache         = &programCache;
  mShaderCache          = &shaderCache;
  mUniformBufferManager = &uniformBufferManager;
}

Renderer::~Renderer() = default;

void Renderer::SetGeometry(Render::Geometry* geometry)
{
  mGeometry                 = geometry;
  mUpdateAttributeLocations = true;
}
void Renderer::SetDrawCommands(Dali::DevelRenderer::DrawCommand* pDrawCommands, uint32_t size)
{
  mDrawCommands.clear();
  mDrawCommands.insert(mDrawCommands.end(), pDrawCommands, pDrawCommands + size);
}

void Renderer::GlContextDestroyed()
{
  mGeometry->GlContextDestroyed();
}

void Renderer::GlCleanup()
{
}

void Renderer::SetUniformFromProperty(BufferIndex bufferIndex, Program& program, UniformIndexMap& map)
{
  GLint location = program.GetUniformLocation(map.uniformIndex);
  if(Program::UNIFORM_UNKNOWN != location)
  {
    // switch based on property type to use correct GL uniform setter
    switch(map.propertyValue->GetType())
    {
      case Property::INTEGER:
      {
        program.SetUniform1i(location, map.propertyValue->GetInteger(bufferIndex));
        break;
      }
      case Property::FLOAT:
      {
        program.SetUniform1f(location, map.propertyValue->GetFloat(bufferIndex));
        break;
      }
      case Property::VECTOR2:
      {
        Vector2 value(map.propertyValue->GetVector2(bufferIndex));
        program.SetUniform2f(location, value.x, value.y);
        break;
      }

      case Property::VECTOR3:
      {
        Vector3 value(map.propertyValue->GetVector3(bufferIndex));
        program.SetUniform3f(location, value.x, value.y, value.z);
        break;
      }

      case Property::VECTOR4:
      {
        Vector4 value(map.propertyValue->GetVector4(bufferIndex));
        program.SetUniform4f(location, value.x, value.y, value.z, value.w);
        break;
      }

      case Property::ROTATION:
      {
        Quaternion value(map.propertyValue->GetQuaternion(bufferIndex));
        program.SetUniform4f(location, value.mVector.x, value.mVector.y, value.mVector.z, value.mVector.w);
        break;
      }

      case Property::MATRIX:
      {
        const Matrix& value = map.propertyValue->GetMatrix(bufferIndex);
        program.SetUniformMatrix4fv(location, 1, value.AsFloat());
        break;
      }

      case Property::MATRIX3:
      {
        const Matrix3& value = map.propertyValue->GetMatrix3(bufferIndex);
        program.SetUniformMatrix3fv(location, 1, value.AsFloat());
        break;
      }

      default:
      {
        // Other property types are ignored
        break;
      }
    }
  }
}

void Renderer::BindTextures(Program& program, Graphics::CommandBuffer& commandBuffer, Vector<Graphics::Texture*>& boundTextures)
{
  uint32_t textureUnit = 0;

  GLint                          uniformLocation(-1);
  std::vector<Render::Sampler*>& samplers(mRenderDataProvider->GetSamplers());
  std::vector<Render::Texture*>& textures(mRenderDataProvider->GetTextures());

  std::vector<Graphics::TextureBinding> textureBindings;
  for(uint32_t i = 0; i < static_cast<uint32_t>(textures.size()); ++i) // not expecting more than uint32_t of textures
  {
    if(textures[i] && textures[i]->GetGraphicsObject())
    {
      if(program.GetSamplerUniformLocation(i, uniformLocation))
      {
        // if the sampler exists,
        //   if it's default, delete the graphics object
        //   otherwise re-initialize it if dirty

        const Graphics::Sampler* graphicsSampler = (samplers[i] ? samplers[i]->GetGraphicsObject()
                                                                : nullptr);

        boundTextures.PushBack(textures[i]->GetGraphicsObject());
        const Graphics::TextureBinding textureBinding{textures[i]->GetGraphicsObject(), graphicsSampler, textureUnit};
        textureBindings.push_back(textureBinding);

        program.SetUniform1i(uniformLocation, textureUnit); // Get through shader reflection
        ++textureUnit;
      }
    }
  }

  if(textureBindings.size() > 0)
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
  mPremultipledAlphaEnabled = enable;
  mUpdated                  = true;
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

void Renderer::Render(Context&                                             context,
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
    return;
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
    return;
  }

  // Create command buffer if not present
  if(!mGraphicsCommandBuffer)
  {
    mGraphicsCommandBuffer = mGraphicsController->CreateCommandBuffer(
      Graphics::CommandBufferCreateInfo()
        .SetLevel(Graphics::CommandBufferLevel::SECONDARY),
      nullptr);
  }
  else
  {
    mGraphicsCommandBuffer->Reset();
  }

  auto& commandBuffer = mGraphicsCommandBuffer;

  // Set blending mode
  if(!mDrawCommands.empty())
  {
    blend = (commands[0]->queue == DevelRenderer::RENDER_QUEUE_OPAQUE ? false : blend);
  }

  // Create Program
  ShaderDataPtr            shaderData   = mRenderDataProvider->GetShader().GetShaderData();
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

  auto     graphicsProgram = mGraphicsController->CreateProgram(createInfo, nullptr);
  Program* program         = Program::New(*mProgramCache,
                                  shaderData,
                                  *mGraphicsController,
                                  std::move(graphicsProgram),
                                  (shaderData->GetHints() & Dali::Shader::Hint::MODIFIES_GEOMETRY) != 0x0);

  if(!program)
  {
    DALI_LOG_ERROR("Failed to get program for shader at address %p.\n", reinterpret_cast<void*>(&mRenderDataProvider->GetShader()));
    return;
  }

  // Temporarily create a pipeline here - this will be used for transporting
  // topology, vertex format, attrs, rasterization state
  mGraphicsPipeline = PrepareGraphicsPipeline(*program, instruction, blend, std::move(mGraphicsPipeline));

  commandBuffer->BindPipeline(*mGraphicsPipeline.get());

  BindTextures(*program, *commandBuffer.get(), boundTextures);

  BuildUniformIndexMap(bufferIndex, node, size, *program);

  WriteUniformBuffer(bufferIndex, *commandBuffer.get(), program, instruction, node, modelMatrix, modelViewMatrix, viewMatrix, projectionMatrix, size);

  bool drawn = false; // Draw can fail if there are no vertex buffers or they haven't been uploaded yet
                      // @todo We should detect this case much earlier to prevent unnecessary work

  //@todo manage mDrawCommands in the same way as above command buffer?!
  if(mDrawCommands.empty())
  {
    drawn = mGeometry->Draw(*mGraphicsController, *commandBuffer.get(), mIndexedDrawFirstElement, mIndexedDrawElementsCount);
  }
  else
  {
    for(auto& cmd : commands)
    {
      // @todo This should generate a command buffer per cmd
      // Tests WILL fail. (Temporarily commented out)
      mGeometry->Draw(*mGraphicsController, *commandBuffer.get(), cmd->firstIndex, cmd->elementCount);
    }
  }

  // Command buffer contains Texture bindings, vertex bindings, index buffer binding, pipeline(vertex format)
  // @todo We should return the command buffer(s) and let the calling method submit
  // If not drawn, then don't add command buffer to submit info, and if empty, don't
  // submit.
  if(drawn)
  {
    Graphics::SubmitInfo submitInfo{{}, 0 | Graphics::SubmitFlagBits::FLUSH};
    submitInfo.cmdBuffer.push_back(commandBuffer.get());
    mGraphicsController->SubmitCommandBuffers(submitInfo);
  }

  mUpdated = false;
}

void Renderer::BuildUniformIndexMap(BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, const Vector3& size, Program& program)
{
  // Check if the map has changed
  DALI_ASSERT_DEBUG(mRenderDataProvider && "No Uniform map data provider available");

  const SceneGraph::UniformMapDataProvider& uniformMapDataProvider = mRenderDataProvider->GetUniformMap();

  if(uniformMapDataProvider.GetUniformMapChanged(bufferIndex) ||
     node.GetUniformMapChanged(bufferIndex) ||
     mUniformIndexMap.Count() == 0 ||
     mShaderChanged)
  {
    // Reset shader pointer
    mShaderChanged = false;

    const SceneGraph::CollectedUniformMap& uniformMap     = uniformMapDataProvider.GetUniformMap(bufferIndex);
    const SceneGraph::CollectedUniformMap& uniformMapNode = node.GetUniformMap(bufferIndex);

    uint32_t maxMaps = static_cast<uint32_t>(uniformMap.Count() + uniformMapNode.Count()); // 4,294,967,295 maps should be enough
    mUniformIndexMap.Clear();                                                              // Clear contents, but keep memory if we don't change size
    mUniformIndexMap.Resize(maxMaps);

    uint32_t mapIndex = 0;
    for(; mapIndex < uniformMap.Count(); ++mapIndex)
    {
      mUniformIndexMap[mapIndex].propertyValue          = uniformMap[mapIndex].propertyPtr;
      mUniformIndexMap[mapIndex].uniformIndex           = program.RegisterUniform(uniformMap[mapIndex].uniformName);
      mUniformIndexMap[mapIndex].uniformName            = uniformMap[mapIndex].uniformName;
      mUniformIndexMap[mapIndex].uniformNameHash        = uniformMap[mapIndex].uniformNameHash;
      mUniformIndexMap[mapIndex].uniformNameHashNoArray = uniformMap[mapIndex].uniformNameHashNoArray;
      mUniformIndexMap[mapIndex].arrayIndex             = uniformMap[mapIndex].arrayIndex;
    }

    for(uint32_t nodeMapIndex = 0; nodeMapIndex < uniformMapNode.Count(); ++nodeMapIndex)
    {
      uint32_t uniformIndex = program.RegisterUniform(uniformMapNode[nodeMapIndex].uniformName);
      bool     found(false);
      for(uint32_t i = 0; i < uniformMap.Count(); ++i)
      {
        if(mUniformIndexMap[i].uniformIndex == uniformIndex)
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
        mUniformIndexMap[mapIndex].uniformIndex           = uniformIndex;
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
  uint32_t uniformBlockAllocationBytes{0u};
  uint32_t uniformBlockMaxSize{0u};
  uint32_t uboOffset{0u};

  auto& reflection = mGraphicsController->GetProgramReflection(program->GetGraphicsProgram());
  for(auto i = 0u; i < reflection.GetUniformBlockCount(); ++i)
  {
    auto blockSize = GetUniformBufferDataAlignment(reflection.GetUniformBlockSize(i));
    if(uniformBlockMaxSize < blockSize)
    {
      uniformBlockMaxSize = blockSize;
    }
    uniformBlockAllocationBytes += blockSize;
  }

  auto pagedAllocation = ((uniformBlockAllocationBytes / UBO_PAGE_SIZE + 1u)) * UBO_PAGE_SIZE;

  // Allocate twice memory as required by the uniform buffers
  // todo: memory usage backlog to use optimal allocation
  if(uniformBlockAllocationBytes && !mUniformBuffer[bufferIndex])
  {
    mUniformBuffer[bufferIndex] = mUniformBufferManager->AllocateUniformBuffer(pagedAllocation);
  }
  else if(uniformBlockAllocationBytes && (mUniformBuffer[bufferIndex]->GetSize() < pagedAllocation ||
                                          (pagedAllocation < uint32_t(float(mUniformBuffer[bufferIndex]->GetSize()) * UBO_SHRINK_THRESHOLD))))
  {
    mUniformBuffer[bufferIndex]->Reserve(pagedAllocation);
  }

  // Clear UBO
  if(mUniformBuffer[bufferIndex])
  {
    mUniformBuffer[bufferIndex]->Fill(0, 0u, 0u);
  }

  // update the uniform buffer
  // pass shared UBO and offset, return new offset for next item to be used
  // don't process bindings if there are no uniform buffers allocated
  auto ubo = mUniformBuffer[bufferIndex].get();
  if(ubo)
  {
    auto uboCount = reflection.GetUniformBlockCount();
    mUniformBufferBindings.resize(uboCount);

    std::vector<Graphics::UniformBufferBinding>* bindings{&mUniformBufferBindings};

    // Write default uniforms
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::MODEL_MATRIX), *ubo, *bindings, modelMatrix);
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::VIEW_MATRIX), *ubo, *bindings, viewMatrix);
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::PROJECTION_MATRIX), *ubo, *bindings, projectionMatrix);
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::MODEL_VIEW_MATRIX), *ubo, *bindings, modelViewMatrix);

    auto mvpUniformInfo = program->GetDefaultUniform(Program::DefaultUniformIndex::MVP_MATRIX);
    if(mvpUniformInfo && !mvpUniformInfo->name.empty())
    {
      Matrix modelViewProjectionMatrix(false);
      Matrix::Multiply(modelViewProjectionMatrix, modelViewMatrix, projectionMatrix);
      WriteDefaultUniform(mvpUniformInfo, *ubo, *bindings, modelViewProjectionMatrix);
    }

    auto normalUniformInfo = program->GetDefaultUniform(Program::DefaultUniformIndex::NORMAL_MATRIX);
    if(normalUniformInfo && !normalUniformInfo->name.empty())
    {
      Matrix3 normalMatrix(modelViewMatrix);
      normalMatrix.Invert();
      normalMatrix.Transpose();
      WriteDefaultUniform(normalUniformInfo, *ubo, *bindings, normalMatrix);
    }

    Vector4        finalColor;
    const Vector4& color = node.GetRenderColor(bufferIndex);
    if(mPremultipledAlphaEnabled)
    {
      float alpha = color.a * mRenderDataProvider->GetOpacity(bufferIndex);
      finalColor  = Vector4(color.r * alpha, color.g * alpha, color.b * alpha, alpha);
    }
    else
    {
      finalColor = Vector4(color.r, color.g, color.b, color.a * mRenderDataProvider->GetOpacity(bufferIndex));
    }
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::COLOR), *ubo, *bindings, finalColor);

    // Write uniforms from the uniform map
    FillUniformBuffer(*program, instruction, *ubo, bindings, uboOffset, bufferIndex);

    // Write uSize in the end, as it shouldn't be overridable by dynamic properties.
    WriteDefaultUniform(program->GetDefaultUniform(Program::DefaultUniformIndex::SIZE), *ubo, *bindings, size);

    commandBuffer.BindUniformBuffers(*bindings);
  }
}

template<class T>
bool Renderer::WriteDefaultUniform(const Graphics::UniformInfo* uniformInfo, Render::UniformBuffer& ubo, const std::vector<Graphics::UniformBufferBinding>& bindings, const T& data)
{
  if(uniformInfo && !uniformInfo->name.empty())
  {
    WriteUniform(ubo, bindings, *uniformInfo, data);
    return true;
  }
  return false;
}

template<class T>
void Renderer::WriteUniform(Render::UniformBuffer& ubo, const std::vector<Graphics::UniformBufferBinding>& bindings, const Graphics::UniformInfo& uniformInfo, const T& data)
{
  WriteUniform(ubo, bindings, uniformInfo, &data, sizeof(T));
}

void Renderer::WriteUniform(Render::UniformBuffer& ubo, const std::vector<Graphics::UniformBufferBinding>& bindings, const Graphics::UniformInfo& uniformInfo, const void* data, uint32_t size)
{
  ubo.Write(data, size, bindings[uniformInfo.bufferIndex].offset + uniformInfo.offset);
}

void Renderer::FillUniformBuffer(Program&                                      program,
                                 const SceneGraph::RenderInstruction&          instruction,
                                 Render::UniformBuffer&                        ubo,
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
    mUniformBufferBindings[i].offset   = dataOffset;

    dataOffset += GetUniformBufferDataAlignment(mUniformBufferBindings[i].dataSize);
    mUniformBufferBindings[i].buffer = ubo.GetBuffer();

    for(UniformIndexMappings::Iterator iter = mUniformIndexMap.Begin(),
                                       end  = mUniformIndexMap.End();
        iter != end;
        ++iter)
    {
      // @todo This means parsing the uniform string every frame. Instead, store the array index if present.
      int arrayIndex = (*iter).arrayIndex;

      auto uniformInfo  = Graphics::UniformInfo{};
      auto uniformFound = program.GetUniform((*iter).uniformName.GetCString(),
                                             (*iter).uniformNameHashNoArray ? (*iter).uniformNameHashNoArray
                                                                            : (*iter).uniformNameHash,
                                             uniformInfo);

      if(uniformFound)
      {
        auto dst = mUniformBufferBindings[uniformInfo.bufferIndex].offset + uniformInfo.offset;

        switch((*iter).propertyValue->GetType())
        {
          case Property::Type::BOOLEAN:
          {
            ubo.Write(&(*iter).propertyValue->GetBoolean(updateBufferIndex),
                      sizeof(bool),
                      dst + static_cast<uint32_t>(sizeof(bool)) * arrayIndex);
            break;
          }
          case Property::Type::INTEGER:
          {
            ubo.Write(&(*iter).propertyValue->GetInteger(updateBufferIndex),
                      sizeof(int32_t),
                      dst + static_cast<int32_t>(sizeof(int32_t)) * arrayIndex);
            break;
          }
          case Property::Type::FLOAT:
          {
            ubo.Write(&(*iter).propertyValue->GetFloat(updateBufferIndex),
                      sizeof(float),
                      dst + static_cast<uint32_t>(sizeof(float)) * arrayIndex);
            break;
          }
          case Property::Type::VECTOR2:
          {
            ubo.Write(&(*iter).propertyValue->GetVector2(updateBufferIndex),
                      sizeof(Vector2),
                      dst + static_cast<uint32_t>(sizeof(Vector2)) * arrayIndex);
            break;
          }
          case Property::Type::VECTOR3:
          {
            ubo.Write(&(*iter).propertyValue->GetVector3(updateBufferIndex),
                      sizeof(Vector3),
                      dst + static_cast<uint32_t>(sizeof(Vector3)) * arrayIndex);
            break;
          }
          case Property::Type::VECTOR4:
          {
            ubo.Write(&(*iter).propertyValue->GetVector4(updateBufferIndex),
                      sizeof(Vector4),
                      dst + static_cast<uint32_t>(sizeof(Vector4)) * arrayIndex);
            break;
          }
          case Property::Type::MATRIX:
          {
            ubo.Write(&(*iter).propertyValue->GetMatrix(updateBufferIndex),
                      sizeof(Matrix),
                      dst + static_cast<uint32_t>(sizeof(Matrix)) * arrayIndex);
            break;
          }
          case Property::Type::MATRIX3:
          {
            // todo: handle data padding properly
            // Vulkan:
            //
            //const auto& matrix = &(*iter).propertyValue->GetMatrix3(updateBufferIndex);
            //for(int i = 0; i < 3; ++i)
            //{
            //ubo.Write(&matrix->AsFloat()[i * 3],
            //          sizeof(float) * 3,
            //          dst + (i * static_cast<uint32_t>(sizeof(Vector4))));
            //}
            // GL:
            ubo.Write(&(*iter).propertyValue->GetMatrix3(updateBufferIndex),
                      sizeof(Matrix3),
                      dst + static_cast<uint32_t>(sizeof(Matrix3)) * arrayIndex);
            break;
          }
          default:
          {
          }
        }
      }
    }
  }
  // write output bindings
  outBindings = &mUniformBufferBindings;

  // Update offset
  offset = dataOffset;
}

void Renderer::SetSortAttributes(BufferIndex                                             bufferIndex,
                                 SceneGraph::RenderInstructionProcessor::SortAttributes& sortAttributes) const
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

  if(mShaderChanged || mUpdateAttributeLocations || mGeometry->AttributesChanged())
  {
    return true;
  }

  for(const auto& texture : mRenderDataProvider->GetTextures())
  {
    if(texture && texture->IsNativeImage())
    {
      return true;
    }
  }

  uint64_t                               hash           = 0xc70f6907UL;
  const SceneGraph::CollectedUniformMap& uniformMapNode = node->GetUniformMap(bufferIndex);
  for(const auto& uniformProperty : uniformMapNode)
  {
    hash = uniformProperty.propertyPtr->Hash(bufferIndex, hash);
  }

  const SceneGraph::UniformMapDataProvider& uniformMapDataProvider = mRenderDataProvider->GetUniformMap();
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

Graphics::UniquePtr<Graphics::Pipeline> Renderer::PrepareGraphicsPipeline(
  Program&                                             program,
  const Dali::Internal::SceneGraph::RenderInstruction& instruction,
  bool                                                 blend,
  Graphics::UniquePtr<Graphics::Pipeline>&&            oldPipeline)
{
  Graphics::InputAssemblyState inputAssemblyState{};
  Graphics::VertexInputState   vertexInputState{};
  Graphics::ProgramState       programState{};
  uint32_t                     bindingIndex{0u};

  if(mUpdateAttributeLocations || mGeometry->AttributesChanged())
  {
    mAttributeLocations.Clear();
    mUpdateAttributeLocations = true;
  }

  auto& reflection = mGraphicsController->GetProgramReflection(program.GetGraphicsProgram());

  /**
   * Bind Attributes
   */
  uint32_t base = 0;
  for(auto&& vertexBuffer : mGeometry->GetVertexBuffers())
  {
    const VertexBuffer::Format& vertexFormat = *vertexBuffer->GetFormat();

    vertexInputState.bufferBindings.emplace_back(vertexFormat.size, // stride
                                                 Graphics::VertexInputRate::PER_VERTEX);

    const uint32_t attributeCount = vertexBuffer->GetAttributeCount();
    for(uint32_t i = 0; i < attributeCount; ++i)
    {
      if(mUpdateAttributeLocations)
      {
        auto    attributeName = vertexBuffer->GetAttributeName(i);
        int32_t pLocation     = reflection.GetVertexAttributeLocation(std::string(attributeName.GetStringView()));
        if(-1 == pLocation)
        {
          DALI_LOG_WARNING("Attribute not found in the shader: %s\n", attributeName.GetCString());
        }
        mAttributeLocations.PushBack(pLocation);
      }

      uint32_t location = static_cast<uint32_t>(mAttributeLocations[base + i]);

      vertexInputState.attributes.emplace_back(location,
                                               bindingIndex,
                                               vertexFormat.components[i].offset,
                                               GetPropertyVertexFormat(vertexFormat.components[i].type));
    }
    base += attributeCount;
    ++bindingIndex;
  }
  mUpdateAttributeLocations = false;

  // Get the topology
  inputAssemblyState.SetTopology(mGeometry->GetTopology());

  // Get the program
  programState.SetProgram(program.GetGraphicsProgram());

  Graphics::RasterizationState rasterizationState{};

  //Set cull face  mode
  const Dali::Internal::SceneGraph::Camera* cam = instruction.GetCamera();
  if(cam->GetReflectionUsed())
  {
    auto adjFaceCullingMode = mFaceCullingMode;
    switch(mFaceCullingMode)
    {
      case FaceCullingMode::Type::FRONT:
      {
        adjFaceCullingMode = FaceCullingMode::Type::BACK;
        break;
      }
      case FaceCullingMode::Type::BACK:
      {
        adjFaceCullingMode = FaceCullingMode::Type::FRONT;
        break;
      }
      default:
      {
        // nothing to do, leave culling as it is
      }
    }
    rasterizationState.SetCullMode(ConvertCullFace(adjFaceCullingMode));
  }
  else
  {
    rasterizationState.SetCullMode(ConvertCullFace(mFaceCullingMode));
  }

  rasterizationState.SetFrontFace(Graphics::FrontFace::COUNTER_CLOCKWISE);

  /**
   * Set Polygon mode
   */
  switch(mGeometry->GetTopology())
  {
    case Graphics::PrimitiveTopology::TRIANGLE_LIST:
    case Graphics::PrimitiveTopology::TRIANGLE_STRIP:
    case Graphics::PrimitiveTopology::TRIANGLE_FAN:
      rasterizationState.SetPolygonMode(Graphics::PolygonMode::FILL);
      break;
    case Graphics::PrimitiveTopology::LINE_LIST:
    case Graphics::PrimitiveTopology::LINE_LOOP:
    case Graphics::PrimitiveTopology::LINE_STRIP:
      rasterizationState.SetPolygonMode(Graphics::PolygonMode::LINE);
      break;
    case Graphics::PrimitiveTopology::POINT_LIST:
      rasterizationState.SetPolygonMode(Graphics::PolygonMode::POINT);
      break;
  }

  // @todo How to signal a blend barrier is needed?
  //if(mBlendingOptions.IsAdvancedBlendEquationApplied() && mPremultipledAlphaEnabled)
  //{
  //  context.BlendBarrier();
  //}

  Graphics::ColorBlendState colorBlendState{};
  colorBlendState.SetBlendEnable(false);

  if(blend)
  {
    colorBlendState.SetBlendEnable(true);

    Graphics::BlendOp rgbOp   = ConvertBlendEquation(mBlendingOptions.GetBlendEquationRgb());
    Graphics::BlendOp alphaOp = ConvertBlendEquation(mBlendingOptions.GetBlendEquationRgb());
    if(mBlendingOptions.IsAdvancedBlendEquationApplied() && mPremultipledAlphaEnabled)
    {
      if(rgbOp != alphaOp)
      {
        DALI_LOG_ERROR("Advanced Blend Equation MUST be applied by using BlendEquation.\n");
        alphaOp = rgbOp;
      }
    }

    colorBlendState
      .SetSrcColorBlendFactor(ConvertBlendFactor(mBlendingOptions.GetBlendSrcFactorRgb()))
      .SetSrcAlphaBlendFactor(ConvertBlendFactor(mBlendingOptions.GetBlendSrcFactorAlpha()))
      .SetDstColorBlendFactor(ConvertBlendFactor(mBlendingOptions.GetBlendDestFactorRgb()))
      .SetDstAlphaBlendFactor(ConvertBlendFactor(mBlendingOptions.GetBlendDestFactorAlpha()))
      .SetColorBlendOp(rgbOp)
      .SetAlphaBlendOp(alphaOp);

    // Blend color is optional and rarely used
    Vector4* blendColor = const_cast<Vector4*>(mBlendingOptions.GetBlendColor());
    if(blendColor)
    {
      colorBlendState.SetBlendConstants(blendColor->AsFloat());
    }
  }

  // Take the program into use so we can send uniforms to it
  // @todo Remove this call entirely!
  program.Use();

  mUpdated = true;

  // @todo Should instead set framebuffer once through Renderpass, rather than modifying
  // pipeline repeatedly.
  Graphics::FramebufferState framebufferState{};
  if(instruction.mFrameBuffer)
  {
    instruction.mFrameBuffer->Bind(); // Ensure graphics object is created.
    framebufferState.SetFramebuffer(*instruction.mFrameBuffer->GetGraphicsObject());
  }

  // Create a new pipeline
  // @todo Passed as pointers - shallow copy will break. Implementation MUST deep copy.
  return mGraphicsController->CreatePipeline(
    Graphics::PipelineCreateInfo()
      .SetInputAssemblyState(&inputAssemblyState)
      .SetVertexInputState(&vertexInputState)
      .SetRasterizationState(&rasterizationState)
      .SetColorBlendState(&colorBlendState)
      .SetFramebufferState(&framebufferState)
      .SetProgramState(&programState)
      .SetNextExtension(&mLegacyProgram),
    std::move(oldPipeline));
}

} // namespace Render

} // namespace Internal

} // namespace Dali
