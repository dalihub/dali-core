/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "scene-graph-renderer.h"

// INTERNAL INCLUDES
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/matrix-utils.h>
#include <dali/internal/common/memory-pool-object-allocator.h>

#include <dali/internal/update/common/collected-uniform-map.h>

#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/rendering/render-instruction.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-shader.h>
#include <dali/internal/update/rendering/scene-graph-sampler.h>
#include <dali/internal/update/rendering/scene-graph-texture.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/update/graphics/uniform-buffer-manager.h>
#include <dali/internal/update/graphics/uniform-buffer-view.h>

#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-command-buffer.h>
#include <dali/graphics-api/graphics-shader.h>
#include <dali/graphics-api/graphics-types.h>
#include <dali/graphics-api/graphics-sampler.h>

#include <cstring>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace // unnamed namespace
{
#if defined( DEBUG_ENABLED )
Debug::Filter* gVulkanFilter  = Debug::Filter::New( Debug::NoLogging, false, "LOG_VULKAN_UNIFORMS" );
Debug::Filter* gTextureFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_TEXTURE" );
#endif

const uint32_t UNIFORM_MAP_READY      = 0;
const uint32_t COPY_UNIFORM_MAP       = 1;
const uint32_t REGENERATE_UNIFORM_MAP = 2;

//Memory pool used to allocate new renderers. Memory used by this pool will be released when shutting down DALi
MemoryPoolObjectAllocator<Renderer> gRendererMemoryPool;

/**
 * Helper function computing correct alignment of data for uniform buffers
 * @param dataSize size of uniform buffer
 * @return aligned offset of data
 */
inline uint32_t GetUniformBufferDataAlignment( uint32_t dataSize )
{
  return ( ( dataSize / 256u ) + ( ( dataSize % 256u ) ? 1u : 0u ) ) * 256u;
}

void WriteUniform( UniformBufferView& ubo,
                   const Graphics::UniformInfo& uniformInfo,
                   const void* data, uint32_t size)
{
  ubo.Write(data, size, ubo.GetOffset() + uniformInfo.offset);
}
template<class T>
void WriteUniform(UniformBufferView&   ubo,
                  const Graphics::UniformInfo& uniformInfo,
                  const T&                     data)
{
  WriteUniform(ubo, uniformInfo, &data, sizeof(T));
}

template<>
void WriteUniform( UniformBufferView& ubo,
                   const Graphics::UniformInfo& uniformInfo,
                   const Matrix3&               data )
{
  // Matrix3 has to take stride in account ( 16 )
#if defined(VULKAN_ENABLED)
  float values[12];

  std::memcpy( &values[0], data.AsFloat(), sizeof( float ) * 3 );
  std::memcpy( &values[4], &data.AsFloat()[3], sizeof( float ) * 3 );
  std::memcpy( &values[8], &data.AsFloat()[6], sizeof( float ) * 3 );

  WriteUniform( ubo, uniformInfo, &values, sizeof( float ) * 12 );
#else
  WriteUniform( ubo, uniformInfo, data.AsFloat(), sizeof(Matrix3));
#endif
}

} // Anonymous namespace

Renderer* Renderer::New()
{
  return new( gRendererMemoryPool.AllocateRawThreadSafe() ) Renderer();
}

Renderer::Renderer()
: mGraphicsController( nullptr ),
  mTextureSet( NULL ),
  mGeometry( NULL ),
  mShader( NULL ),
  mBlendColor( NULL ),
  mStencilParameters( RenderMode::AUTO,
                      StencilFunction::ALWAYS,
                      0xFF,
                      0x00,
                      0xFF,
                      StencilOperation::KEEP,
                      StencilOperation::KEEP,
                      StencilOperation::KEEP ),
  mIndexedDrawFirstElement( 0u ),
  mIndexedDrawElementsCount( 0u ),
  mRegenerateUniformMap( 0u ),
  mBlendOptions(),
  mDepthFunction( DepthFunction::LESS ),
  mFaceCullingMode( FaceCullingMode::NONE ),
  mBlendMode( BlendMode::AUTO ),
  mDepthWriteMode( DepthWriteMode::AUTO ),
  mDepthTestMode( DepthTestMode::AUTO ),
  mRenderingBehavior( DevelRenderer::Rendering::IF_REQUIRED ),
  mPremultipliedAlphaEnabled( false ),
  mTextureBindings{},
  mOpacity( 1.0f ),
  mDepthIndex( 0 )
{
}

Renderer::~Renderer()
{
  if( mTextureSet )
  {
    mTextureSet->RemoveObserver( this );
    mTextureSet = NULL;
  }
  if( mShader )
  {
    mShader->RemoveConnectionObserver( *this );
    mShader = NULL;
  }
}

void Renderer::operator delete( void* ptr )
{
  gRendererMemoryPool.FreeThreadSafe( static_cast<Renderer*>( ptr ) );
}

void Renderer::Initialize( Graphics::Controller& graphicsController, UniformBufferManager& uniformBufferManager)
{
  mGraphicsController = &graphicsController;
  mUniformBufferManager = &uniformBufferManager;
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::UpdateUniformMap( BufferIndex updateBufferIndex, Node& node )
{
  auto shaderMapChangeCounter = mShader ? mShader->GetUniformMap().GetChangeCounter() : 0u;
  bool shaderMapChanged       = mShader && (mShaderMapChangeCounter != shaderMapChangeCounter);
  if(shaderMapChanged)
  {
    mShaderMapChangeCounter = shaderMapChangeCounter;
  }

  if(mUniformMapChangeCounter != mUniformMaps.GetChangeCounter() || shaderMapChanged)
  {
    mRegenerateUniformMap = true;

    // Update local counters to identify any future changes to maps
    // (unlikely, but allowed by API).
    mUniformMapChangeCounter = mUniformMaps.GetChangeCounter();
  }

  if(mRegenerateUniformMap)
  {
    CollectedUniformMap& localMap = mCollectedUniformMap;
    localMap.Clear();

    const UniformMap& rendererUniformMap = PropertyOwner::GetUniformMap();

    auto size = rendererUniformMap.Count();
    if( mShader )
    {
      size += mShader->GetUniformMap().Count();
    }

    localMap.Reserve( size );
    localMap.AddMappings( rendererUniformMap );
    if( mShader )
    {
      localMap.AddMappings( mShader->GetUniformMap() );
    }
    localMap.UpdateChangeCounter();

    mRegenerateUniformMap = false;
  }
}

void Renderer::PrepareRender( Graphics::CommandBuffer& commandBuffer,
                              const Matrix& viewMatrix,
                              const Matrix& projectionMatrix,
                              BufferIndex              bufferIndex,
                              const RenderInstruction& renderInstruction,
                              RenderItem&              item )
{
  if( !mShader->GetGraphicsObject() || !mGraphicsController )
  {
    return;
  }

  auto& program    = *mShader->GetGraphicsObject();
  auto& reflection = mGraphicsController->GetProgramReflection( program );

  BindTextures( commandBuffer, reflection );

  uint32_t instanceCount = 0u;
  auto& pipeline = PrepareGraphicsPipeline(program, renderInstruction, item.mNode, !item.mIsOpaque);

  commandBuffer.BindPipeline(pipeline);

  auto nodeIndex = BuildUniformIndexMap(bufferIndex, *item.mNode, &program);
  WriteUniformBuffer(bufferIndex, commandBuffer, *mShader, item, viewMatrix, projectionMatrix, nodeIndex);

  mGeometry->BindVertexAttributes( commandBuffer );

  mGeometry->Draw(*mGraphicsController, commandBuffer,
                   mIndexedDrawFirstElement, mIndexedDrawElementsCount,
                   instanceCount );

  DALI_LOG_STREAM( gVulkanFilter, Debug::Verbose, "done\n" );
}

void Renderer::BindTextures( Graphics::CommandBuffer& commandBuffer, const Graphics::Reflection& reflection )
{
   /**
   * Prepare textures
   */
  auto samplers = reflection.GetSamplers();
  if( mTextureSet )
  {
    if( !samplers.empty() )
    {
      mTextureBindings.clear();
      uint32_t textureUnit=0;
      for( auto i = 0u; i < mTextureSet->GetTextureCount(); ++i )
      {
        auto texture = mTextureSet->GetTexture( i );
        if( texture )
        {
          auto sampler = mTextureSet->GetTextureSampler( i );

          if( sampler && sampler->mIsDirty )
          {
            // if default sampler
            if( sampler->mMagnificationFilter == FilterMode::DEFAULT &&
                sampler->mMinificationFilter == FilterMode::DEFAULT && sampler->mSWrapMode == WrapMode::DEFAULT &&
                sampler->mTWrapMode == WrapMode::DEFAULT && sampler->mRWrapMode == WrapMode::DEFAULT )
            {
              sampler->DestroyGraphicsObjects();
            }
            else
            {
              // reinitialize sampler
              sampler->Initialize( *mGraphicsController );
            }
            sampler->mIsDirty = false;
          }

          texture->PrepareTexture(); // Ensure that a native texture is ready to be drawn

          auto binding = Graphics::TextureBinding{}
                           .SetBinding( textureUnit /*samplers[i].binding*/ )
                           .SetTexture( texture->GetGraphicsObject() )
                           .SetSampler( sampler ? sampler->GetGraphicsObject() : nullptr );

          mTextureBindings.emplace_back( binding );
        }
        ++textureUnit;
      }
      commandBuffer.BindTextures( mTextureBindings );
    }
  }
}

std::size_t Renderer::BuildUniformIndexMap(BufferIndex bufferIndex, const Node& node, Graphics::Program* program)
{
  const SceneGraph::CollectedUniformMap&    uniformMap             = mCollectedUniformMap;
  const SceneGraph::UniformMap&             uniformMapNode         = node.GetUniformMap();

  bool updateMaps;

  auto shaderMapChangeCounter = mShader ? mShader->GetUniformMap().GetChangeCounter() : 0u;
  bool shaderMapChanged       = mShader && (mShaderMapChangeCounter != shaderMapChangeCounter);
  if(shaderMapChanged)
  {
    mShaderMapChangeCounter = shaderMapChangeCounter;
  }

  // Usual case is to only have 1 node, however we do allow multiple nodes to reuse the same
  // renderer, so we have to cache uniform map per render item (node / renderer pair).

  // Specially, if node don't have uniformMap, we mark nodePtr as nullptr.
  // So, all nodes without uniformMap will share same UniformIndexMap, contains only render data providers.
  const auto nodePtr    = uniformMapNode.Count() ? &node : nullptr;

  const auto nodeChangeCounter          = nodePtr ? uniformMapNode.GetChangeCounter() : 0;
  const auto renderItemMapChangeCounter = uniformMap.GetChangeCounter();

  auto iter = std::find_if(mNodeIndexMap.begin(), mNodeIndexMap.end(), [nodePtr, program](RenderItemLookup& element) { return (element.node == nodePtr && element.program == program); });

  std::size_t renderItemMapIndex;
  if(iter == mNodeIndexMap.end())
  {
    renderItemMapIndex = mUniformIndexMaps.size();
    RenderItemLookup renderItemLookup;
    renderItemLookup.node                       = nodePtr;
    renderItemLookup.program                    = program;
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
                 (mUniformIndexMaps[renderItemMapIndex].empty());

    iter->nodeChangeCounter          = nodeChangeCounter;
    iter->renderItemMapChangeCounter = renderItemMapChangeCounter;
  }

  if(updateMaps || shaderMapChanged)
  {
    const uint32_t mapCount     = uniformMap.Count();
    const uint32_t mapNodeCount = uniformMapNode.Count();

    mUniformIndexMaps[renderItemMapIndex].clear(); // Clear contents, but keep memory if we don't change size
    mUniformIndexMaps[renderItemMapIndex].resize(mapCount + mapNodeCount);

    // Copy uniform map into mUniformIndexMap
    uint32_t mapIndex = 0;
    for(; mapIndex < mapCount; ++mapIndex)
    {
      mUniformIndexMaps[renderItemMapIndex][mapIndex].propertyValue          = uniformMap.mUniformMap[mapIndex]->propertyPtr;
      mUniformIndexMaps[renderItemMapIndex][mapIndex].uniformName            = uniformMap.mUniformMap[mapIndex]->uniformName;
      mUniformIndexMaps[renderItemMapIndex][mapIndex].uniformNameHash        = uniformMap.mUniformMap[mapIndex]->uniformNameHash;
      mUniformIndexMaps[renderItemMapIndex][mapIndex].uniformNameHashNoArray = uniformMap.mUniformMap[mapIndex]->uniformNameHashNoArray;
      mUniformIndexMaps[renderItemMapIndex][mapIndex].arrayIndex             = uniformMap.mUniformMap[mapIndex]->arrayIndex;
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
  SceneGraph::Shader&                  shader,
  const RenderItem&                    item,
  const Matrix&                        viewMatrix,
  const Matrix&                        projectionMatrix,
  std::size_t nodeIndex)
{
  auto program = shader.GetGraphicsObject();
  auto& reflection = mGraphicsController->GetProgramReflection(*program);

  const auto& programRequirements = shader.GetUniformBlockMemoryRequirements();

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
      mUniformBufferBindings[i].binding  = standaloneUniforms ? 0 : i - 1;
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
    WriteDefaultUniform(shader.GetDefaultUniform(Shader::DefaultUniformIndex::MODEL_MATRIX), uboViews, item.mModelMatrix);
    WriteDefaultUniform(shader.GetDefaultUniform(Shader::DefaultUniformIndex::VIEW_MATRIX), uboViews, viewMatrix);
    WriteDefaultUniform(shader.GetDefaultUniform(Shader::DefaultUniformIndex::PROJECTION_MATRIX), uboViews, projectionMatrix);
    WriteDefaultUniform(shader.GetDefaultUniform(Shader::DefaultUniformIndex::MODEL_VIEW_MATRIX), uboViews, item.mModelViewMatrix);

    auto mvpUniformInfo = shader.GetDefaultUniform(Shader::DefaultUniformIndex::MVP_MATRIX);
    if(mvpUniformInfo && !mvpUniformInfo->name.empty())
    {
      Matrix modelViewProjectionMatrix(false);
      MatrixUtils::MultiplyProjectionMatrix(modelViewProjectionMatrix, item.mModelViewMatrix, projectionMatrix);
      WriteDefaultUniform(mvpUniformInfo, uboViews, modelViewProjectionMatrix);
    }

    auto normalUniformInfo = shader.GetDefaultUniform(Shader::DefaultUniformIndex::NORMAL_MATRIX);
    if(normalUniformInfo && !normalUniformInfo->name.empty())
    {
      Matrix3 normalMatrix(item.mModelViewMatrix);
      normalMatrix.Invert();
      normalMatrix.Transpose();
      WriteDefaultUniform(normalUniformInfo, uboViews, normalMatrix);
    }

    Vector4        finalColor;                               ///< Applied renderer's opacity color
    const Vector4& color = item.mNode->GetWorldColor(bufferIndex); ///< Actor's original color
    if(mPremultipliedAlphaEnabled)
    {
      const float& alpha = color.a * GetOpacity(bufferIndex);
      finalColor         = Vector4(color.r * alpha, color.g * alpha, color.b * alpha, alpha);
    }
    else
    {
      finalColor = Vector4(color.r, color.g, color.b, color.a * GetOpacity(bufferIndex));
    }
    WriteDefaultUniform(shader.GetDefaultUniform(Shader::DefaultUniformIndex::COLOR), uboViews, finalColor);

    // Write uniforms from the uniform map
    FillUniformBuffer(shader, uboViews, bufferIndex, nodeIndex);

    // Write uSize in the end, as it shouldn't be overridable by dynamic properties.
    WriteDefaultUniform(shader.GetDefaultUniform(Shader::DefaultUniformIndex::SIZE), uboViews, item.mSize);

    commandBuffer.BindUniformBuffers(mUniformBufferBindings);
  }
}

void Renderer::FillUniformBuffer(SceneGraph::Shader& shader,
                        const std::vector<std::unique_ptr<UniformBufferView>>& uboViews,
                        BufferIndex updateBufferIndex,
                        std::size_t nodeIndex)
{
  for(auto& iter : mUniformIndexMaps[nodeIndex])
  {
    auto& uniform = iter;
    int arrayIndex = uniform.arrayIndex;
    if(!uniform.initialized)
    {
      auto uniformInfo = Graphics::UniformInfo{};
      auto uniformFound = shader.GetUniform( uniform.uniformName, uniform.uniformNameHash,
                                             uniform.uniformNameHashNoArray, uniformInfo);
      UniformBufferView* ubo = nullptr;
      if(uniformFound)
      {
        ubo = uboViews[uniformInfo.bufferIndex].get();
      }
      else
      {
        continue;
      }
      uniform.uniformOffset = uniformInfo.offset;
      uniform.uniformLocation = uniformInfo.location;
      uniform.uniformBlockIndex = uniformInfo.bufferIndex;
      uniform.initialized = true;

      auto offset = ubo->GetOffset() + uniformInfo.offset;
      const auto typeSize = iter.propertyValue->GetValueSize();
      const auto dest = offset + static_cast<uint32_t>(typeSize)*arrayIndex;
      ubo->Write(iter.propertyValue->GetValueAddress(updateBufferIndex),
                  typeSize, dest);
    }
    else
    {
      UniformBufferView* ubo = uboViews[uniform.uniformBlockIndex].get();
      auto offset = ubo->GetOffset()+uniform.uniformOffset;
      const auto typeSize = iter.propertyValue->GetValueSize();
      const auto dest = offset + static_cast<uint32_t>(typeSize) * arrayIndex;
      ubo->Write(iter.propertyValue->GetValueAddress(updateBufferIndex),
                 typeSize, dest);
    }
  }
}

template<class T>
bool Renderer::WriteDefaultUniform(const Graphics::UniformInfo* uniformInfo, const std::vector<std::unique_ptr<UniformBufferView>>& uboViews, const T& data)
{
  if(uniformInfo && !uniformInfo->name.empty())
  {
    WriteUniform(*uboViews[uniformInfo->bufferIndex], *uniformInfo, data);
    return true;
  }
  return false;
}

void Renderer::SetTextures( TextureSet* textureSet )
{
  DALI_ASSERT_DEBUG( textureSet != NULL && "Texture set pointer is NULL" );

  if( mTextureSet )
  {
    mTextureSet->RemoveObserver(this);
  }

  mTextureSet = textureSet;
  mTextureSet->AddObserver( this );

  DALI_LOG_INFO( gTextureFilter, Debug::General, "SG::Renderer(%p)::SetTextures( SG::TS:%p )\n"
                 "  SG:Texture:%p  GfxTexture:%p\n", this, textureSet,
                 textureSet?textureSet->GetTexture(0):nullptr,
                 textureSet?(textureSet->GetTexture(0)?textureSet->GetTexture(0)->GetGraphicsObject():nullptr):nullptr );
}

void Renderer::SetShader( Shader* shader )
{
  DALI_ASSERT_DEBUG( shader != NULL && "Shader pointer is NULL" );

  if( mShader )
  {
    mShader->RemoveConnectionObserver(*this);
  }

  mShader = shader;
  mShader->AddConnectionObserver( *this );

  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::SetGeometry( SceneGraph::Geometry* geometry )
{
  DALI_ASSERT_DEBUG( geometry != NULL && "Geometry pointer is NULL");
  mGeometry = geometry;
}

void Renderer::SetDepthIndex( int depthIndex )
{
  mDepthIndex = depthIndex;
}

void Renderer::SetFaceCullingMode( FaceCullingMode::Type faceCullingMode )
{
  mFaceCullingMode = faceCullingMode;
}

FaceCullingMode::Type Renderer::GetFaceCullingMode() const
{
  return mFaceCullingMode;
}

void Renderer::SetBlendMode( BlendMode::Type blendingMode )
{
  mBlendMode = blendingMode;
}

BlendMode::Type Renderer::GetBlendMode() const
{
  return mBlendMode;
}

void Renderer::SetBlendingOptions( const BlendingOptions& options )
{
  if( mBlendOptions.GetBitmask() != options.GetBitmask())
  {
    mBlendOptions.SetBitmask( options.GetBitmask() );
  }
}

void Renderer::SetBlendingOptions( unsigned int options )
{
  if( options != mBlendOptions.GetBitmask() )
  {
    mBlendOptions.SetBitmask( options );
  }
}

const BlendingOptions& Renderer::GetBlendingOptions() const
{
  return mBlendOptions;
}

void Renderer::SetBlendColor( const Vector4& blendColor )
{
  if( blendColor == Color::TRANSPARENT )
  {
    mBlendColor = nullptr;
  }
  else
  {
    if( !mBlendColor )
    {
      mBlendColor = new Vector4( blendColor );
    }
    else
    {
      *mBlendColor = blendColor;
    }
  }
}

const Vector4& Renderer::GetBlendColor() const
{
  if( mBlendColor )
  {
    return *mBlendColor;
  }
  return Color::TRANSPARENT;
}

void Renderer::SetIndexedDrawFirstElement( uint32_t firstElement )
{
  mIndexedDrawFirstElement = firstElement;
}

uint32_t Renderer::GetIndexedDrawFirstElement() const
{
  return mIndexedDrawFirstElement;
}

void Renderer::SetIndexedDrawElementsCount( uint32_t elementsCount )
{
  mIndexedDrawElementsCount = elementsCount;
}

uint32_t Renderer::GetIndexedDrawElementsCount() const
{
  return mIndexedDrawElementsCount;
}

void Renderer::EnablePreMultipliedAlpha( bool preMultipled )
{
  mPremultipliedAlphaEnabled = preMultipled;
}

bool Renderer::IsPreMultipliedAlphaEnabled() const
{
  return mPremultipliedAlphaEnabled;
}

void Renderer::SetDepthWriteMode( DepthWriteMode::Type depthWriteMode )
{
  mDepthWriteMode = depthWriteMode;
}

DepthWriteMode::Type Renderer::GetDepthWriteMode() const
{
  return mDepthWriteMode;
}

void Renderer::SetDepthTestMode( DepthTestMode::Type depthTestMode )
{
  mDepthTestMode = depthTestMode;
}

DepthTestMode::Type Renderer::GetDepthTestMode() const
{
  return mDepthTestMode;
}

void Renderer::SetDepthFunction( DepthFunction::Type depthFunction )
{
  mDepthFunction = depthFunction;
}

DepthFunction::Type Renderer::GetDepthFunction() const
{
  return mDepthFunction;
}

void Renderer::SetRenderMode( RenderMode::Type mode )
{
  mStencilParameters.renderMode = mode;
}

void Renderer::SetStencilFunction( StencilFunction::Type stencilFunction )
{
  mStencilParameters.stencilFunction = stencilFunction;
}

void Renderer::SetStencilFunctionMask( int stencilFunctionMask )
{
  mStencilParameters.stencilFunctionMask = stencilFunctionMask;
}

void Renderer::SetStencilFunctionReference( int stencilFunctionReference )
{
  mStencilParameters.stencilFunctionReference = stencilFunctionReference;
}

void Renderer::SetStencilMask( int stencilMask )
{
  mStencilParameters.stencilMask = stencilMask;
}

void Renderer::SetStencilOperationOnFail( StencilOperation::Type stencilOperationOnFail )
{
  mStencilParameters.stencilOperationOnFail = stencilOperationOnFail;
}

void Renderer::SetStencilOperationOnZFail( StencilOperation::Type stencilOperationOnZFail )
{
  mStencilParameters.stencilOperationOnZFail = stencilOperationOnZFail;
}

void Renderer::SetStencilOperationOnZPass( StencilOperation::Type stencilOperationOnZPass )
{
  mStencilParameters.stencilOperationOnZPass = stencilOperationOnZPass;
}

const StencilParameters& Renderer::GetStencilParameters() const
{
  return mStencilParameters;
}

void Renderer::BakeOpacity( BufferIndex updateBufferIndex, float opacity )
{
  mOpacity.Bake( updateBufferIndex, opacity );
}

float Renderer::GetOpacity( BufferIndex updateBufferIndex ) const
{
  return mOpacity[updateBufferIndex];
}
void Renderer::SetRenderingBehavior( DevelRenderer::Rendering::Type renderingBehavior )
{
  mRenderingBehavior = renderingBehavior;
}

DevelRenderer::Rendering::Type Renderer::GetRenderingBehavior() const
{
  return mRenderingBehavior;
}

Renderer::OpacityType Renderer::GetOpacityType( BufferIndex updateBufferIndex, const Node& node ) const
{
  Renderer::OpacityType opacityType = Renderer::OPAQUE;

  switch( mBlendMode )
  {
    case BlendMode::ON: // If the renderer should always be use blending
    {
      float alpha = node.GetWorldColor( updateBufferIndex ).a * mOpacity[updateBufferIndex];
      if( alpha <= FULLY_TRANSPARENT )
      {
        opacityType = Renderer::TRANSPARENT;
      }
      else
      {
        opacityType = Renderer::TRANSLUCENT;
      }
      break;
    }
    case BlendMode::AUTO:
    {
      bool shaderRequiresBlending( mShader->HintEnabled( Dali::Shader::Hint::OUTPUT_IS_TRANSPARENT ) );
      if( shaderRequiresBlending || ( mTextureSet && mTextureSet->HasAlpha() ) )
      {
        opacityType = Renderer::TRANSLUCENT;
      }

      // renderer should determine opacity using the actor color
      float alpha = node.GetWorldColor( updateBufferIndex ).a * mOpacity[updateBufferIndex];
      if( alpha <= FULLY_TRANSPARENT )
      {
        opacityType = Renderer::TRANSPARENT;
      }
      else if( alpha <= FULLY_OPAQUE )
      {
        opacityType = Renderer::TRANSLUCENT;
      }
      break;
    }
    case BlendMode::OFF: // the renderer should never use blending
    default:
    {
      opacityType = Renderer::OPAQUE;
      break;
    }
  }

  return opacityType;
}

void Renderer::TextureSetChanged()
{
}

void Renderer::TextureSetDeleted()
{
  mTextureSet = nullptr;
}

void Renderer::ConnectionsChanged( PropertyOwner& object )
{
  // One of our child objects has changed it's connections. Ensure the collected uniform
  // map gets regenerated during UpdateUniformMap
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::ConnectedUniformMapChanged()
{
  // Called if the shader's uniform map has changed
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::ObservedObjectDestroyed(PropertyOwner& owner)
{
  if( reinterpret_cast<PropertyOwner*>(mShader) == &owner )
  {
    mShader = nullptr;
  }
}

Shader* Renderer::PrepareShader()
{
  DALI_ASSERT_ALWAYS(mShader->GetGraphicsObject());
  return mShader;
}

Graphics::Pipeline& Renderer::PrepareGraphicsPipeline( Graphics::Program&       program,
                                                       const RenderInstruction& instruction,
                                                       Node*                    node,
                                                       bool                     blend )
{
  //@todo CACHE ME!
  DALI_ASSERT_ALWAYS(mPipeline != nullptr);
  return *mPipeline.get();
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
