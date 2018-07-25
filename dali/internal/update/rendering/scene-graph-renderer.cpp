/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/rendering/data-providers/node-data-provider.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-shader.h>
#include <dali/internal/update/rendering/scene-graph-sampler.h>
#include <dali/internal/update/rendering/scene-graph-texture.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>

#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-render-command.h>
#include <dali/graphics-api/graphics-api-shader.h>
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics-api/graphics-api-pipeline-factory.h>
#include <dali/graphics-api/graphics-api-sampler.h>
#include <dali/graphics-api/graphics-api-sampler-factory.h>

#include <cstring>
#include <dali/integration-api/debug.h>

namespace // unnamed namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gVulkanFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VULKAN_UNIFORMS");
Debug::Filter* gTextureFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TEXTURE");
#endif


const unsigned int UNIFORM_MAP_READY      = 0;
const unsigned int COPY_UNIFORM_MAP       = 1;
const unsigned int REGENERATE_UNIFORM_MAP = 2;

//Memory pool used to allocate new renderers. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Renderer> gRendererMemoryPool;

void AddMappings( Dali::Internal::SceneGraph::CollectedUniformMap& localMap, const Dali::Internal::SceneGraph::UniformMap& uniformMap )
{
  // Iterate thru uniformMap.
  //   Any maps that aren't in localMap should be added in a single step
  Dali::Internal::SceneGraph::CollectedUniformMap newUniformMappings;

  for( unsigned int i=0, count = uniformMap.Count(); i<count; ++i )
  {
    Dali::Internal::SceneGraph::UniformPropertyMapping::Hash nameHash = uniformMap[i].uniformNameHash;
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

} // Anonymous namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Renderer* Renderer::New()
{
  return new ( gRendererMemoryPool.AllocateRawThreadSafe() ) Renderer();
}

Renderer::Renderer()
: mGraphics( nullptr ),
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
  mPremultipledAlphaEnabled( false ),
  mGraphicsRenderCommand(),
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

void Renderer::Initialize( Integration::Graphics::Graphics& graphics )
{
  mGraphics = &graphics;

  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::UpdateUniformMap( BufferIndex updateBufferIndex, Node& node )
{
  // Called every frame.
  CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];

  // See if we need to update the collected map:
  if( mRegenerateUniformMap == REGENERATE_UNIFORM_MAP || // renderer or shader's uniform map has changed
      node.GetUniformMapChanged( updateBufferIndex ) ) // node's uniform map has change
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

    unsigned int index=0;
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

void Renderer::PrepareRender( BufferIndex updateBufferIndex )
{
  auto &controller = mGraphics->GetController();

  if( !mGraphicsRenderCommand )
  {
    mGraphicsRenderCommand = controller.AllocateRenderCommand();
  }

  if( !mShader->GetGfxObject() )
  {
    return;
  }

  /**
   * Prepare vertex attribute buffer bindings
   */
  std::vector<const Graphics::API::Buffer*> vertexBuffers{};

  for(auto&& vertexBuffer : mGeometry->GetVertexBuffers())
  {
    vertexBuffers.push_back( vertexBuffer->GetGfxObject() );
  }

  /**
   * Prepare textures
   */
  auto textureBindings = Graphics::API::RenderCommand::NewTextureBindings();
  auto samplers        = mShader->GetGfxObject()->GetSamplers();
  if(mTextureSet)
  {
    if (!samplers.empty())
    {
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
              sampler->Initialize( *mGraphics );
            }
            sampler->mIsDirty = false;
          }

          texture->PrepareTexture(); // Ensure that a native texture is ready to be drawn

          auto binding    = Graphics::API::RenderCommand::TextureBinding{}
            .SetBinding(samplers[i].binding)
            .SetTexture(texture->GetGfxObject())
            .SetSampler(sampler ? sampler->GetGfxObject() : nullptr);

          textureBindings.emplace_back(binding);
        }
      }
    }
  }
  mGraphicsRenderCommand->BindTextures( std::move(textureBindings) );

  // Build render command
  // todo: this may be deferred until all render items are sorted, otherwise
  // certain optimisations cannot be done

  const auto &vb = mGeometry->GetVertexBuffers()[0];

  // set optional index buffer
  bool usesIndexBuffer{false};
  if ((usesIndexBuffer = mGeometry->HasIndexBuffer()))
  {
    mGraphicsRenderCommand->BindIndexBuffer(Graphics::API::RenderCommand::IndexBufferBinding()
                                         .SetBuffer(mGeometry->GetIndexBuffer())
                                         .SetOffset(0)
    );
  }

  mGraphicsRenderCommand->BindVertexBuffers(std::move(vertexBuffers) );


  if(usesIndexBuffer)
  {
    mGraphicsRenderCommand->Draw(std::move(Graphics::API::RenderCommand::DrawCommand{}
                                        .SetFirstIndex( uint32_t(mIndexedDrawFirstElement) )
                                        .SetDrawType( Graphics::API::RenderCommand::DrawType::INDEXED_DRAW )
                                        .SetFirstInstance( 0u )
                                        .SetIndicesCount(
                                          mIndexedDrawElementsCount ?
                                            uint32_t( mIndexedDrawElementsCount ) :
                                            mGeometry->GetIndexBufferElementCount() )
                                        .SetInstanceCount( 1u )));
  }
  else
  {
    mGraphicsRenderCommand->Draw(std::move(Graphics::API::RenderCommand::DrawCommand{}
                                        .SetFirstVertex(0u)
                                        .SetDrawType(Graphics::API::RenderCommand::DrawType::VERTEX_DRAW)
                                        .SetFirstInstance(0u)
                                        .SetVertexCount(vb->GetElementCount())
                                        .SetInstanceCount(1u)));
  }
  DALI_LOG_STREAM( gVulkanFilter, Debug::Verbose,  "done\n" );
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

void Renderer::SetIndexedDrawFirstElement( size_t firstElement )
{
  mIndexedDrawFirstElement = firstElement;
}

size_t Renderer::GetIndexedDrawFirstElement() const
{
  return mIndexedDrawFirstElement;
}

void Renderer::SetIndexedDrawElementsCount( size_t elementsCount )
{
  mIndexedDrawElementsCount = elementsCount;
}

size_t Renderer::GetIndexedDrawElementsCount() const
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

void Renderer::BindPipeline( std::unique_ptr<Graphics::API::Pipeline> pipeline )
{
  mGraphicsPipeline = std::move(pipeline);
  mGraphicsRenderCommand->BindPipeline( *mGraphicsPipeline.get() );
}

void Renderer::UpdateUniformBuffers( BufferIndex updateBufferIndex,
                                     std::vector<std::vector<char>>& uniformBufferOutput,
                                     std::vector<Graphics::API::RenderCommand::PushConstantsBinding>& outBindings )
{
  if( !GetShader().GetGfxObject() )
  {
    return;
  }

  auto uboCount = GetShader().GetGfxObject()->GetUniformBlockCount();

  outBindings.clear();
  outBindings = Graphics::API::RenderCommand::NewPushConstantsBindings(uboCount);

  if( uniformBufferOutput.size() != uboCount)
  {
    uniformBufferOutput.resize(uboCount);
  }

  outBindings.clear();
  outBindings = Graphics::API::RenderCommand::NewPushConstantsBindings(uboCount);

  for (auto i = 0u; i < uboCount; ++i)
  {
    Graphics::API::ShaderDetails::UniformBlockInfo ubInfo;

    GetShader().GetGfxObject()->GetUniformBlock(i, ubInfo);

    if( uniformBufferOutput[i].size() != ubInfo.size)
    {
      uniformBufferOutput[i].resize(ubInfo.size);
    }

    // Set push constant bindings
    auto &pushBinding = outBindings[i];
    pushBinding.data    = uniformBufferOutput[i].data();
    pushBinding.size    = uint32_t(uniformBufferOutput[i].size());
    pushBinding.binding = ubInfo.binding;
  }

  // write to memory
  auto count = mCollectedUniformMap->Count();
  for(auto index = 0u; index < count; ++index )
  {
    const auto& uniformMap = (*mCollectedUniformMap)[index];

    // test for array ( special case )
    std::string uniformName(uniformMap->uniformName);

    auto hashValue = uniformMap->uniformNameHash;
    int         arrayIndex       = 0;
    auto        arrayLeftBracket = uniformMap->uniformName .find('[');
    if (arrayLeftBracket != std::string::npos)
    {
      arrayIndex = std::atoi(&uniformName.c_str()[arrayLeftBracket + 1]);
      uniformName = uniformName.substr(0, arrayLeftBracket);
      hashValue = CalculateHash( uniformName );
    }

    auto uniformInfo = Graphics::API::ShaderDetails::UniformInfo{};
    if( GetShader().GetUniform( uniformName, hashValue, uniformInfo ) )
    {
      // write into correct uniform buffer
      auto dst = (uniformBufferOutput[uniformInfo.bufferIndex].data() + uniformInfo.offset);

      switch (uniformMap->propertyPtr->GetType())
      {
        case Property::Type::FLOAT:
        case Property::Type::INTEGER:
        case Property::Type::BOOLEAN:
        {
          dst += sizeof(float) * arrayIndex;
          memcpy(dst, &uniformMap->propertyPtr->GetFloat(updateBufferIndex), sizeof(float));
          break;
        }
        case Property::Type::VECTOR2:
        {
          dst += /* sizeof(Vector2) * */arrayIndex * 16; // todo: use array stride from spirv
          memcpy(dst, &uniformMap->propertyPtr->GetVector2(updateBufferIndex), sizeof(Vector2));
          break;
        }
        case Property::Type::VECTOR3:
        {
          dst += sizeof(Vector3) * arrayIndex;
          memcpy(dst, &uniformMap->propertyPtr->GetVector3(updateBufferIndex), sizeof(Vector3));
          break;
        }
        case Property::Type::VECTOR4:
        {
          dst += sizeof(float) * arrayIndex;
          memcpy(dst, &uniformMap->propertyPtr->GetVector4(updateBufferIndex), sizeof(Vector4));
          break;
        }
        case Property::Type::MATRIX:
        {
          dst += sizeof(Matrix) * arrayIndex;
          memcpy(dst, &uniformMap->propertyPtr->GetMatrix(updateBufferIndex), sizeof(Matrix));
          break;
        }
        case Property::Type::MATRIX3:
        {
          dst += sizeof(Matrix3) * arrayIndex;

          auto& matrix = uniformMap->propertyPtr->GetMatrix3(updateBufferIndex);

          float* values = reinterpret_cast<float*>(dst);
          std::fill( values, values+12, 10.0f );
          std::memcpy( &values[0], matrix.AsFloat(), sizeof(float)*3 );
          std::memcpy( &values[4], &matrix.AsFloat()[3], sizeof(float)*3 );
          std::memcpy( &values[8], &matrix.AsFloat()[6], sizeof(float)*3 );

          memcpy(dst, values, sizeof(float)*12);
          break;
        }
        default:
        {
        }
      }
    }
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
