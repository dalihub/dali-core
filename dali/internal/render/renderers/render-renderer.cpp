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
#include <dali/internal/common/image-sampler.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/render/data-providers/uniform-map-data-provider.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/internal/render/renderers/render-texture.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>

namespace Dali
{
namespace Internal
{
namespace
{
/**
 * Helper to set view and projection matrices once per program
 * @param program to set the matrices to
 * @param modelMatrix to set
 * @param viewMatrix to set
 * @param projectionMatrix to set
 * @param modelViewMatrix to set
 * @param modelViewProjectionMatrix to set
 */
inline void SetMatrices(Program&      program,
                        const Matrix& modelMatrix,
                        const Matrix& viewMatrix,
                        const Matrix& projectionMatrix,
                        const Matrix& modelViewMatrix)
{
  GLint loc = program.GetUniformLocation(Program::UNIFORM_MODEL_MATRIX);
  if(Program::UNIFORM_UNKNOWN != loc)
  {
    program.SetUniformMatrix4fv(loc, 1, modelMatrix.AsFloat());
  }
  loc = program.GetUniformLocation(Program::UNIFORM_VIEW_MATRIX);
  if(Program::UNIFORM_UNKNOWN != loc)
  {
    if(program.GetViewMatrix() != &viewMatrix)
    {
      program.SetViewMatrix(&viewMatrix);
      program.SetUniformMatrix4fv(loc, 1, viewMatrix.AsFloat());
    }
  }
  // set projection matrix if program has not yet received it this frame or if it is dirty
  loc = program.GetUniformLocation(Program::UNIFORM_PROJECTION_MATRIX);
  if(Program::UNIFORM_UNKNOWN != loc)
  {
    if(program.GetProjectionMatrix() != &projectionMatrix)
    {
      program.SetProjectionMatrix(&projectionMatrix);
      program.SetUniformMatrix4fv(loc, 1, projectionMatrix.AsFloat());
    }
  }
  loc = program.GetUniformLocation(Program::UNIFORM_MODELVIEW_MATRIX);
  if(Program::UNIFORM_UNKNOWN != loc)
  {
    program.SetUniformMatrix4fv(loc, 1, modelViewMatrix.AsFloat());
  }

  loc = program.GetUniformLocation(Program::UNIFORM_MVP_MATRIX);
  if(Program::UNIFORM_UNKNOWN != loc)
  {
    Matrix modelViewProjectionMatrix(false);
    Matrix::Multiply(modelViewProjectionMatrix, modelViewMatrix, projectionMatrix);
    program.SetUniformMatrix4fv(loc, 1, modelViewProjectionMatrix.AsFloat());
  }

  loc = program.GetUniformLocation(Program::UNIFORM_NORMAL_MATRIX);
  if(Program::UNIFORM_UNKNOWN != loc)
  {
    Matrix3 normalMatrix;
    normalMatrix = modelViewMatrix;
    normalMatrix.Invert();
    normalMatrix.Transpose();
    program.SetUniformMatrix3fv(loc, 1, normalMatrix.AsFloat());
  }
}

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
: mRenderDataProvider(dataProvider),
  mContext(nullptr),
  mGeometry(geometry),
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

void Renderer::Initialize(Context& context, Graphics::Controller& graphicsController)
{
  mContext            = &context;
  mGraphicsController = &graphicsController;
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

void Renderer::SetBlending(Context& context, bool blend)
{
  context.SetBlend(blend);
  if(blend)
  {
    // Blend color is optional and rarely used
    const Vector4* blendColor = mBlendingOptions.GetBlendColor();
    if(blendColor)
    {
      context.SetCustomBlendColor(*blendColor);
    }
    else
    {
      context.SetDefaultBlendColor();
    }

    // Set blend source & destination factors
    context.BlendFuncSeparate(mBlendingOptions.GetBlendSrcFactorRgb(),
                              mBlendingOptions.GetBlendDestFactorRgb(),
                              mBlendingOptions.GetBlendSrcFactorAlpha(),
                              mBlendingOptions.GetBlendDestFactorAlpha());

    // Set blend equations
    Dali::DevelBlendEquation::Type rgbEquation   = mBlendingOptions.GetBlendEquationRgb();
    Dali::DevelBlendEquation::Type alphaEquation = mBlendingOptions.GetBlendEquationAlpha();

    if(mBlendingOptions.IsAdvancedBlendEquationApplied() && mPremultipledAlphaEnabled)
    {
      if(rgbEquation != alphaEquation)
      {
        DALI_LOG_ERROR("Advanced Blend Equation have to be appried by using BlendEquation.\n");
      }
      context.BlendEquation(rgbEquation);
    }
    else
    {
      context.BlendEquationSeparate(rgbEquation, alphaEquation);
    }
  }

  mUpdated = true;
}

void Renderer::GlContextDestroyed()
{
  mGeometry->GlContextDestroyed();
}

void Renderer::GlCleanup()
{
}

void Renderer::SetUniforms(BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, const Vector3& size, Program& program)
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
      mUniformIndexMap[mapIndex].propertyValue = uniformMap[mapIndex].propertyPtr;
      mUniformIndexMap[mapIndex].uniformIndex  = program.RegisterUniform(uniformMap[mapIndex].uniformName);
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
        mUniformIndexMap[mapIndex].propertyValue = uniformMapNode[nodeMapIndex].propertyPtr;
        mUniformIndexMap[mapIndex].uniformIndex  = uniformIndex;
        ++mapIndex;
      }
    }

    mUniformIndexMap.Resize(mapIndex);
  }

  // Set uniforms in local map
  for(UniformIndexMappings::Iterator iter = mUniformIndexMap.Begin(),
                                     end  = mUniformIndexMap.End();
      iter != end;
      ++iter)
  {
    SetUniformFromProperty(bufferIndex, program, *iter);
  }

  GLint sizeLoc = program.GetUniformLocation(Program::UNIFORM_SIZE);
  if(-1 != sizeLoc)
  {
    program.SetSizeUniform3f(sizeLoc, size.x, size.y, size.z);
  }
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

bool Renderer::BindTextures(Context& context, Program& program, Graphics::CommandBuffer& commandBuffer, Vector<Graphics::Texture*>& boundTextures)
{
  uint32_t textureUnit = 0;
  bool     result      = true;

  GLint                          uniformLocation(-1);
  std::vector<Render::Sampler*>& samplers(mRenderDataProvider->GetSamplers());
  std::vector<Render::Texture*>& textures(mRenderDataProvider->GetTextures());

  std::vector<Graphics::TextureBinding> textureBindings;
  for(uint32_t i = 0; i < static_cast<uint32_t>(textures.size()) && result; ++i) // not expecting more than uint32_t of textures
  {
    if(textures[i])
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

  return result;
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

  // Get the program to use:
  Program* program = mRenderDataProvider->GetShader().GetProgram();
  if(!program)
  {
    DALI_LOG_ERROR("Failed to get program for shader at address %p.\n", reinterpret_cast<void*>(&mRenderDataProvider->GetShader()));
    return;
  }

  Graphics::UniquePtr<Graphics::CommandBuffer> commandBuffer = mGraphicsController->CreateCommandBuffer(
    Graphics::CommandBufferCreateInfo()
      .SetLevel(Graphics::CommandBufferLevel::SECONDARY),
    nullptr);

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
    context.CullFace(adjFaceCullingMode);
  }
  else
  {
    context.CullFace(mFaceCullingMode);
  }

  // Temporarily create a pipeline here - this will be used for transporting
  // topology and vertex format for now.
  Graphics::UniquePtr<Graphics::Pipeline> pipeline = PrepareGraphicsPipeline(*program);
  commandBuffer->BindPipeline(*pipeline.get());

  // Take the program into use so we can send uniforms to it
  program->Use();

  if(DALI_LIKELY(BindTextures(context, *program, *commandBuffer.get(), boundTextures)))
  {
    // Only set up and draw if we have textures and they are all valid

    // set projection and view matrix if program has not yet received them yet this frame
    SetMatrices(*program, modelMatrix, viewMatrix, projectionMatrix, modelViewMatrix);

    // set color uniform
    GLint loc = program->GetUniformLocation(Program::UNIFORM_COLOR);
    if(Program::UNIFORM_UNKNOWN != loc)
    {
      const Vector4& color = node.GetRenderColor(bufferIndex);
      if(mPremultipledAlphaEnabled)
      {
        float alpha = color.a * mRenderDataProvider->GetOpacity(bufferIndex);
        program->SetUniform4f(loc, color.r * alpha, color.g * alpha, color.b * alpha, alpha);
      }
      else
      {
        program->SetUniform4f(loc, color.r, color.g, color.b, color.a * mRenderDataProvider->GetOpacity(bufferIndex));
      }
    }

    SetUniforms(bufferIndex, node, size, *program);

    if(mBlendingOptions.IsAdvancedBlendEquationApplied() && mPremultipledAlphaEnabled)
    {
      context.BlendBarrier();
    }

    //@todo manage mDrawCommands in the same way as above command buffer?!
    if(mDrawCommands.empty())
    {
      SetBlending(context, blend);

      mGeometry->Draw(*mGraphicsController, *commandBuffer.get(), mIndexedDrawFirstElement, mIndexedDrawElementsCount);
    }
    else
    {
      for(auto& cmd : commands)
      {
        if(cmd->queue == queueIndex)
        {
          //Set blending mode
          SetBlending(context, cmd->queue == DevelRenderer::RENDER_QUEUE_OPAQUE ? false : blend);

          // @todo This should generate a command buffer per cmd
          // Tests WILL fail.
          mGeometry->Draw(*mGraphicsController, *commandBuffer.get(), cmd->firstIndex, cmd->elementCount);
        }
      }
    }

    // Command buffer contains Texture bindings, vertex bindings, index buffer binding, pipeline(vertex format)
    Graphics::SubmitInfo submitInfo{{}, 0 | Graphics::SubmitFlagBits::FLUSH};
    submitInfo.cmdBuffer.push_back(commandBuffer.get());
    mGraphicsController->SubmitCommandBuffers(submitInfo);

    mUpdated = false;
  }
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

Graphics::UniquePtr<Graphics::Pipeline> Renderer::PrepareGraphicsPipeline(Program& program)
{
  Graphics::InputAssemblyState inputAssemblyState{};
  Graphics::VertexInputState   vertexInputState{};
  uint32_t                     bindingIndex{0u};

  if(mUpdateAttributeLocations || mGeometry->AttributesChanged())
  {
    mAttributeLocations.Clear();
    mUpdateAttributeLocations = true;
  }

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
        auto     attributeName = vertexBuffer->GetAttributeName(i);
        uint32_t index         = program.RegisterCustomAttribute(attributeName);
        int32_t  pLocation     = program.GetCustomAttributeLocation(index);
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

  // Create a new pipeline
  return mGraphicsController->CreatePipeline(
    Graphics::PipelineCreateInfo()
      .SetInputAssemblyState(&inputAssemblyState) // Passed as pointers - shallow copy will break. TOO C LIKE
      .SetVertexInputState(&vertexInputState),
    nullptr);
}

} // namespace Render

} // namespace Internal

} // namespace Dali
