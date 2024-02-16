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
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/rendering/render-instruction.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-shader.h>
#include <dali/internal/update/rendering/scene-graph-sampler.h>
#include <dali/internal/update/rendering/scene-graph-texture.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/update/graphics/graphics-buffer-manager.h>

#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-render-command.h>
#include <dali/graphics-api/graphics-api-shader.h>
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics-api/graphics-api-pipeline-factory.h>
#include <dali/graphics-api/graphics-api-sampler.h>
#include <dali/graphics-api/graphics-api-sampler-factory.h>

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
#if defined(DEBUG_ENABLED)
Debug::Filter* gVulkanFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VULKAN_UNIFORMS");
Debug::Filter* gTextureFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TEXTURE");
#endif


const uint32_t UNIFORM_MAP_READY      = 0;
const uint32_t COPY_UNIFORM_MAP       = 1;
const uint32_t REGENERATE_UNIFORM_MAP = 2;

//Memory pool used to allocate new renderers. Memory used by this pool will be released when shutting down DALi
MemoryPoolObjectAllocator<Renderer> gRendererMemoryPool;

void AddMappings( CollectedUniformMap& localMap, const UniformMap& uniformMap )
{
  // Iterate thru uniformMap.
  //   Any maps that aren't in localMap should be added in a single step
  CollectedUniformMap newUniformMappings;

  for( UniformMap::SizeType i = 0, count=uniformMap.Count(); i<count; ++i )
  {
    UniformPropertyMapping::Hash nameHash = uniformMap[i].uniformNameHash;
    bool found = false;

    for( auto map : localMap )
    {
      if( map->uniformNameHash == nameHash )
      {
        if( map->uniformName == uniformMap[i].uniformName )
        {
          found = true;
          break;
        }
      }
    }
    if( !found )
    {
      // it's a new mapping. Add raw ptr to temporary list
      newUniformMappings.PushBack( &uniformMap[i] );
    }
  }

  if( newUniformMappings.Count() > 0 )
  {
    localMap.Reserve( localMap.Count() + newUniformMappings.Count() );

    for( auto map : newUniformMappings )
    {
      localMap.PushBack( map );
    }
  }
}

/**
 * Helper function computing correct alignment of data for uniform buffers
 * @param dataSize size of uniform buffer
 * @return aligned offset of data
 */
inline uint32_t GetUniformBufferDataAlignment( uint32_t dataSize )
{
  return ( (dataSize / 256u) + ( (dataSize % 256u) ? 1u : 0u ) ) * 256u;
}

} // Anonymous namespace

Renderer* Renderer::New()
{
  return new ( gRendererMemoryPool.AllocateRawThreadSafe() ) Renderer();
}

Renderer::Renderer()
: mGraphicsController( nullptr ),
  mTextureSet( NULL ),
  mGeometry( NULL ),
  mShader( NULL ),
  mBlendColor( NULL ),
  mStencilParameters( RenderMode::AUTO, StencilFunction::ALWAYS, 0xFF, 0x00, 0xFF, StencilOperation::KEEP, StencilOperation::KEEP, StencilOperation::KEEP ),
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
  mPremultipledAlphaEnabled( false ),
  mRenderCommands{},
  mTextureBindings{},
  mOpacity( 1.0f ),
  mDepthIndex( 0 )
{
  // Observe our own PropertyOwner's uniform map
  AddUniformMapObserver( *this );
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

void Renderer::Initialize( Graphics::Controller& graphicsController )
{
  mGraphicsController = &graphicsController;

  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::UpdateUniformMap( BufferIndex updateBufferIndex, Node& node )
{
  // Called every frame. Well, it is now...
  CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];

  // See if we need to update the collected map:
  if( mRegenerateUniformMap == REGENERATE_UNIFORM_MAP || // renderer or shader's uniform map has changed
      node.GetUniformMapChanged( updateBufferIndex ) )   // node's uniform map has change
  {
    localMap.Resize(0);

    // Priority is: High: Node, Renderer, Shader, to Low
    AddMappings( localMap, node.GetUniformMap() );

    const UniformMap& rendererUniformMap = PropertyOwner::GetUniformMap();
    AddMappings( localMap, rendererUniformMap );

    if( mShader )
    {
      AddMappings( localMap, mShader->GetUniformMap() );
    }
  }
  else if( mRegenerateUniformMap == COPY_UNIFORM_MAP )
  {
    // Copy old map into current map
    CollectedUniformMap& oldMap = mCollectedUniformMap[ 1-updateBufferIndex ];

    localMap.Resize( oldMap.Count() );

      uint32_t index=0;
    for( CollectedUniformMap::Iterator iter = oldMap.Begin(), end = oldMap.End() ; iter != end ; ++iter, ++index )
    {
      localMap[index] = *iter;
    }
  }

  if( mRegenerateUniformMap > 0 )
  {
    mRegenerateUniformMap--;
  }
}

void Renderer::FreeRenderCommand( RenderInstruction* renderInstruction )
{

  mRenderCommands.DestroyRenderCommand( renderInstruction );
}

RenderCommand& Renderer::GetRenderCommand( RenderInstruction* renderInstruction,
                                           BufferIndex updateBufferIndex )
{
  return mRenderCommands.GetRenderCommand( renderInstruction, updateBufferIndex );
}

void Renderer::PrepareRender( BufferIndex updateBufferIndex, RenderInstruction* renderInstruction )
{
  auto& renderCmd = mRenderCommands.AllocRenderCommand( renderInstruction, *mGraphicsController, updateBufferIndex );
  auto& cmd = renderCmd.GetGfxRenderCommand( updateBufferIndex );

  if (!mShader->GetGfxObject())
  {
    return;
  }

  /**
   * Prepare vertex attribute buffer bindings
   */
  std::vector<const Graphics::Buffer*> vertexBuffers{};

  for(auto&& vertexBuffer : mGeometry->GetVertexBuffers())
  {
    vertexBuffers.push_back( vertexBuffer->GetGfxObject() );
  }

  auto& shader = *mShader->GetGfxObject();

  /**
   * Prepare textures
   */
  auto samplers = shader.GetSamplers();
  if(mTextureSet)
  {
    if (!samplers.empty())
    {
      mTextureBindings.clear();
      for (auto i = 0u; i < mTextureSet->GetTextureCount(); ++i)
      {
        auto texture = mTextureSet->GetTexture(i);
        if( texture )
        {
          auto sampler = mTextureSet->GetTextureSampler( i );

          if( sampler && sampler->mIsDirty )
          {
            // if default sampler
            if( sampler->mMagnificationFilter == FilterMode::DEFAULT && sampler->mMinificationFilter == FilterMode::DEFAULT &&
                sampler->mSWrapMode == WrapMode::DEFAULT &&
                sampler->mTWrapMode == WrapMode::DEFAULT &&
                sampler->mRWrapMode == WrapMode::DEFAULT )
            {
              sampler->mGfxSampler.reset( nullptr );
            }
            else
            {
              // reinitialize sampler
              sampler->Initialize( *mGraphicsController );
            }
            sampler->mIsDirty = false;
          }

          texture->PrepareTexture(); // Ensure that a native texture is ready to be drawn

          auto binding    = Graphics::RenderCommand::TextureBinding{}
            .SetBinding(samplers[i].binding)
            .SetTexture(texture->GetGfxObject())
            .SetSampler(sampler ? sampler->GetGfxObject() : nullptr);

          mTextureBindings.emplace_back(binding);
        }
      }
      renderCmd.BindTextures( mTextureBindings, updateBufferIndex );
    }
  }


  // Build render command
  // todo: this may be deferred until all render items are sorted, otherwise
  // certain optimisations cannot be done

  const auto &vb = mGeometry->GetVertexBuffers()[0];

  // set optional index buffer
  bool usesIndexBuffer{false};
  if ((usesIndexBuffer = mGeometry->HasIndexBuffer()))
  {
    cmd.BindIndexBuffer(Graphics::RenderCommand::IndexBufferBinding()
                        .SetBuffer(mGeometry->GetIndexBuffer())
                        .SetOffset(0)
    );
  }

  cmd.BindVertexBuffers(std::move(vertexBuffers) );

  if(usesIndexBuffer)
  {
    cmd.Draw(std::move(Graphics::RenderCommand::DrawCommand{}
                                        .SetFirstIndex( uint32_t(mIndexedDrawFirstElement) )
                                        .SetDrawType( Graphics::RenderCommand::DrawType::INDEXED_DRAW )
                                        .SetFirstInstance( 0u )
                                        .SetIndicesCount(
                                          mIndexedDrawElementsCount ?
                                            uint32_t( mIndexedDrawElementsCount ) :
                                            mGeometry->GetIndexBufferElementCount() )
                                        .SetInstanceCount( 1u )));
  }
  else
  {
    cmd.Draw(std::move(Graphics::RenderCommand::DrawCommand{}
                                        .SetFirstVertex(0u)
                                        .SetDrawType(Graphics::RenderCommand::DrawType::VERTEX_DRAW)
                                        .SetFirstInstance(0u)
                                        .SetVertexCount(vb->GetElementCount())
                                        .SetInstanceCount(1u)));
  }

  DALI_LOG_STREAM( gVulkanFilter, Debug::Verbose,  "done\n" );
}

void Renderer::WriteUniform( GraphicsBuffer& ubo, const std::vector<Graphics::RenderCommand::UniformBufferBinding>& bindings, const std::string& name, size_t hash, const void* data, uint32_t size )
{
  if( !mShader->GetGfxObject() )
  {
    // Invalid pipeline
    return;
  }
  auto uniformInfo = Graphics::ShaderDetails::UniformInfo{};
  if( mShader->GetUniform( name, hash, uniformInfo ) )
  {
    ubo.Write( data, size, bindings[uniformInfo.bufferIndex].offset + uniformInfo.offset, false );
  }
}

void Renderer::WriteUniform( GraphicsBuffer& ubo, const std::vector<Graphics::RenderCommand::UniformBufferBinding>& bindings, const Graphics::ShaderDetails::UniformInfo& uniformInfo, const void* data, uint32_t size )
{
  if( !mShader->GetGfxObject() )
  {
    // Invalid pipeline
    return;
  }

  ubo.Write( data, size, bindings[uniformInfo.bufferIndex].offset + uniformInfo.offset, false );
}

bool Renderer::UpdateUniformBuffers( RenderInstruction& instruction,
                                     GraphicsBuffer& ubo,
                                     std::vector<Graphics::RenderCommand::UniformBufferBinding>*& outBindings,
                                     uint32_t& offset,
                                     BufferIndex updateBufferIndex )
{
  int updates( 0u );

  const UniformMap& rendererUniformMap = PropertyOwner::GetUniformMap();
  const auto& collectedMap = mCollectedUniformMap[updateBufferIndex];
  if( collectedMap.Count() < rendererUniformMap.Count() )
  {
    DALI_LOG_ERROR("%p, i:%u - FAILED TO REGENERATE UNIFORM MAP", this, updateBufferIndex );
  }

  auto uboCount = mShader->GetGfxObject()->GetUniformBlockCount();
  auto gfxShader = mShader->GetGfxObject();

  RenderCommand* renderCommand;
  if( !mRenderCommands.Find( &instruction, renderCommand, updateBufferIndex ) )
  {
    DALI_ASSERT_DEBUG( 0 && "Can't find render command for this instruction" );
    return false;
  }

  auto& currentUboBindings = renderCommand->mUboBindings;


  if(currentUboBindings.size() != uboCount )
  {
    currentUboBindings.resize( uboCount );
    ++updates;
  }

  // setup bindings
  uint32_t dataOffset = offset;
  for (auto i = 0u; i < uboCount; ++i)
  {
    currentUboBindings[i].dataSize = gfxShader->GetUniformBlockSize(i);
    currentUboBindings[i].binding = gfxShader->GetUniformBlockBinding(i);

    if( currentUboBindings[i].offset != dataOffset )
    {
      currentUboBindings[i].offset = dataOffset;
      ++updates;
    }

    dataOffset += GetUniformBufferDataAlignment( currentUboBindings[i].dataSize );

    if( currentUboBindings[i].buffer != ubo.GetBuffer() )
    {
      currentUboBindings[i].buffer = ubo.GetBuffer();
      ++updates;
    }
  }

  // write to memory
  for (auto&& uniformMap : mCollectedUniformMap[updateBufferIndex])
  {
    // Convert uniform name into hash value
    auto uniformInfo = Graphics::ShaderDetails::UniformInfo{};

    // test for array ( special case )
    // @todo This means parsing the uniform string every frame. Instead, store the array index if present.
    int arrayIndex = uniformMap->arrayIndex;

    auto uniformFound = mShader->GetUniform( uniformMap->uniformName,
                                             uniformMap->uniformNameHashNoArray ?
                                             uniformMap->uniformNameHashNoArray :
                                             uniformMap->uniformNameHash, uniformInfo );

    if( uniformFound )
    {
      auto dst = currentUboBindings[uniformInfo.bufferIndex].offset + uniformInfo.offset;

      switch (uniformMap->propertyPtr->GetType())
      {
        case Property::Type::FLOAT:
        case Property::Type::INTEGER:
        case Property::Type::BOOLEAN:
        {
          ubo.Write( &uniformMap->propertyPtr->GetFloat(updateBufferIndex),
                     sizeof(float),
                     dst + static_cast<uint32_t>(sizeof(Vector4)) * arrayIndex,
                     false );
          break;
        }
        case Property::Type::VECTOR2:
        {
          ubo.Write( &uniformMap->propertyPtr->GetVector2(updateBufferIndex),
                     sizeof(Vector2),
                     dst + static_cast<uint32_t>(sizeof(Vector4)) * arrayIndex,
                     false );
          break;
        }
        case Property::Type::VECTOR3:
        {
          ubo.Write( &uniformMap->propertyPtr->GetVector3(updateBufferIndex),
                     sizeof(Vector3),
                     dst + static_cast<uint32_t>(sizeof(Vector4)) * arrayIndex,
                     false );
          break;
        }
        case Property::Type::VECTOR4:
        {
          ubo.Write( &uniformMap->propertyPtr->GetVector4(updateBufferIndex),
                     sizeof(Vector4),
                     dst + static_cast<uint32_t>(sizeof(Vector4)) * arrayIndex,
                     false );
          break;
        }
        case Property::Type::MATRIX:
        {
          ubo.Write( &uniformMap->propertyPtr->GetMatrix(updateBufferIndex),
                     sizeof(Matrix),
                     dst + static_cast<uint32_t>(sizeof(Matrix)) * arrayIndex,
                     false );
          break;
        }
        case Property::Type::MATRIX3:
        {
          const auto& matrix = uniformMap->propertyPtr->GetMatrix3(updateBufferIndex);
          for( int i = 0; i < 3; ++i )
          {
            ubo.Write( &matrix.AsFloat()[i*3],
                       sizeof(float)*3,
                       dst + (i * static_cast<uint32_t>(sizeof(Vector4))),
                       false );
          }

          break;
        }
        default:
        {
        }
      }
    }
  }

  // write output bindings
  outBindings = &currentUboBindings;

  // update offset
  offset = dataOffset;

  // return update status
  return (0 != updates);
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

  // Rebind textures to make sure old data won't be used. Ensure this
  // occurs for both buffers.
  mRenderCommands.BindTextures( mTextureBindings );

  DALI_LOG_INFO( gTextureFilter, Debug::General, "SG::Renderer(%p)::SetTextures( SG::TS:%p )\n"
                 "  SG:Texture:%p  GfxTexture:%p\n", this, textureSet,
                 textureSet?textureSet->GetTexture(0):nullptr,
                 textureSet?(textureSet->GetTexture(0)?textureSet->GetTexture(0)->GetGfxObject():nullptr):nullptr );
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
  mRenderCommands.ClearUniformBindings();
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
    mBlendColor = NULL;
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
  mPremultipledAlphaEnabled = preMultipled;
}

bool Renderer::IsPreMultipliedAlphaEnabled() const
{
  return mPremultipledAlphaEnabled;
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
  mTextureSet = NULL;
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

void Renderer::UniformMappingsChanged( const UniformMap& mappings )
{
  // The mappings are from PropertyOwner base class
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::ObservedObjectDestroyed(PropertyOwner& owner)
{
  if( reinterpret_cast<PropertyOwner*>(mShader) == &owner )
  {
    mShader = NULL;
  }
}

void Renderer::DestroyGraphicsObjects()
{
  mRenderCommands.DestroyAll();
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
