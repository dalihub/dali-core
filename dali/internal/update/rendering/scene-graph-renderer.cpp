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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

// CLASS HEADER
#include "scene-graph-renderer.h"

// INTERNAL INCLUDES
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/rendering/data-providers/node-data-provider.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/update/rendering/scene-graph-shader.h>

#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-render-command.h>
#include <dali/graphics-api/graphics-api-shader.h>
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics-api/graphics-api-pipeline-factory.h>
#include <dali/graphics-api/graphics-api-pipeline-cache.h>

#include <cstring>


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
inline void SetMatrices(
                         const Dali::Matrix& modelMatrix,
                         const Dali::Matrix& viewMatrix,
                         const Dali::Matrix& projectionMatrix,
                         const Dali::Matrix& modelViewMatrix )
{

}

}

namespace // unnamed namespace
{

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

  for( unsigned int i=0, count=uniformMap.Count(); i<count; ++i )
  {
    Dali::Internal::SceneGraph::UniformPropertyMapping::Hash nameHash = uniformMap[i].uniformNameHash;
    bool found = false;

    for( Dali::Internal::SceneGraph::CollectedUniformMap::Iterator iter = localMap.Begin() ; iter != localMap.End() ; ++iter )
    {
      const Dali::Internal::SceneGraph::UniformPropertyMapping* map = (*iter);
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

    for( Dali::Internal::SceneGraph::CollectedUniformMap::Iterator iter = newUniformMappings.Begin(),
           end = newUniformMappings.End() ;
         iter != end ;
         ++iter )
    {
      const Dali::Internal::SceneGraph::UniformPropertyMapping* map = (*iter);
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
  mRenderDataProvider(),
  mTextureSet( NULL ),
  mGeometry( NULL ),
  mShader( NULL ),
  mBlendColor( NULL ),
  mStencilParameters( RenderMode::AUTO, StencilFunction::ALWAYS, 0xFF, 0x00, 0xFF, StencilOperation::KEEP, StencilOperation::KEEP, StencilOperation::KEEP ),
  mIndexedDrawFirstElement( 0u ),
  mIndexedDrawElementsCount( 0u ),
  mBlendBitmask( 0u ),
  mRegenerateUniformMap( 0u ),
  mDepthFunction( DepthFunction::LESS ),
  mFaceCullingMode( FaceCullingMode::NONE ),
  mBlendMode( BlendMode::AUTO ),
  mDepthWriteMode( DepthWriteMode::AUTO ),
  mDepthTestMode( DepthTestMode::AUTO ),
  mPremultipledAlphaEnabled( false ),
  mGfxRenderCommand(),
  mDepthIndex( 0 )
{
  mUniformMapChanged[0] = false;
  mUniformMapChanged[1] = false;

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

  mRenderDataProvider = std::make_unique< RenderDataProvider >();

  mRenderDataProvider->mUniformMapDataProvider = this;
  mRenderDataProvider->mShader = mShader;

  if( mTextureSet )
  {
    size_t textureCount = mTextureSet->GetTextureCount();
    mRenderDataProvider->mTextures.resize( textureCount );
    mRenderDataProvider->mSamplers.resize( textureCount );
    for( unsigned int i(0); i<textureCount; ++i )
    {
      mRenderDataProvider->mTextures[i] = mTextureSet->GetTexture(i);
      mRenderDataProvider->mSamplers[i] = mTextureSet->GetTextureSampler(i);
    }
  }
}


void* AllocateUniformBufferMemory( size_t size )
{
  return nullptr;
}


void Renderer::UpdateUniformMap( BufferIndex updateBufferIndex )
{

  if( mRegenerateUniformMap > UNIFORM_MAP_READY )
  {
    if( mRegenerateUniformMap == REGENERATE_UNIFORM_MAP)
    {
      CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];
      localMap.Resize(0);

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
      CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];
      CollectedUniformMap& oldMap = mCollectedUniformMap[ 1-updateBufferIndex ];

      localMap.Resize( oldMap.Count() );

      unsigned int index=0;
      for( CollectedUniformMap::Iterator iter = oldMap.Begin(), end = oldMap.End() ; iter != end ; ++iter, ++index )
      {
        localMap[index] = *iter;
      }
    }

    mUniformMapChanged[updateBufferIndex] = true;
    mRegenerateUniformMap--;
  }
}

void Renderer::PrepareRender( BufferIndex updateBufferIndex )
{
  auto &controller = mGraphics->GetController();

  // prepare all stuff
  auto gfxShader                   = mShader->GetGfxObject();

  if (!mGfxRenderCommand)
  {
    mGfxRenderCommand = controller.AllocateRenderCommand();
  }

  if (!mShader->GetGfxObject()
              .Exists())
  {
    return;
  }

  using Graphics::API::Pipeline;

  // vertex input state
  Pipeline::VertexInputState vi{};
  /**
   * Prepare vertex attribute buffer bindings
   */
  uint32_t bindingIndex{0u};
  uint32_t locationIndex{0u};

  std::vector<Graphics::API::Accessor<Graphics::API::Buffer>> vertexBuffers{};

  for(auto&& vertexBuffer : mGeometry->GetVertexBuffers())
  {
    vertexBuffers.push_back( vertexBuffer->GetGfxObject() );
    auto attributeCountInForBuffer = vertexBuffer->GetAttributeCount();

    // update vertex buffer if necessary
    vertexBuffer->Update(controller);

    // store buffer binding
    vi.bufferBindings
      .emplace_back(&vertexBuffer->GetGfxObject()
                                 .Get(), vertexBuffer->GetFormat()
                                                     ->size, Pipeline::VertexInputRate::PER_VERTEX);

    for (auto i = 0u; i < attributeCountInForBuffer; ++i)
    {
      // create attribute description
      vi.attributes
        .emplace_back(locationIndex + i, bindingIndex, (vertexBuffer->GetFormat()
                                                                    ->components[i]).offset,
                      Pipeline::VertexInputFormat::UNDEFINED);

    }
    locationIndex += attributeCountInForBuffer;
    bindingIndex++;
  }


  // Invalid input attributes!
  if (mShader->GetGfxObject()
             .Get()
             .GetVertexAttributeLocations()
             .size() != vi.attributes.size())
    return;

  UpdateUniformMap(updateBufferIndex);

  auto &shader = mShader->GetGfxObject()
                        .Get();
  auto uboCount = shader.GetUniformBlockCount();

  auto pushConstantsBindings = Graphics::API::RenderCommand::NewPushConstantsBindings(uboCount);

  // allocate new command ( may be not necessary at all )
  // mGfxRenderCommand = Graphics::API::RenderCommandBuilder().Build();

  // see if we need to reallocate memory for each UBO
  // todo: do it only when shader has changed
  if (mUboMemory.size() != uboCount)
  {
    mUboMemory.resize(uboCount);
  }

  for (auto i = 0u; i < uboCount; ++i)
  {
    Graphics::API::ShaderDetails::UniformBlockInfo ubInfo;

    std::cout << sizeof(ubInfo) << std::endl;

    shader.GetUniformBlock(i, ubInfo);

    if (mUboMemory[i].size() != ubInfo.size)
    {
      mUboMemory[i].resize(ubInfo.size);
    }

    // Set push constant bindings
    auto &pushBinding = pushConstantsBindings[i];
    pushBinding.data    = mUboMemory[i].data();
    pushBinding.size    = uint32_t(mUboMemory[i].size());
    pushBinding.binding = ubInfo.binding;
  }

  // add built-in uniforms

  // write to memory
  for (auto &&i : mCollectedUniformMap)
  {
    for (auto &&j : i)
    {
      std::string uniformName(j->uniformName);
      int         arrayIndex       = 0;
      auto        arrayLeftBracket = j->uniformName
                                      .find('[');
      if (arrayLeftBracket != std::string::npos)
      {
        auto arrayRightBracket = j->uniformName
                                  .find(']');
        arrayIndex = std::atoi(&uniformName.c_str()[arrayLeftBracket + 1]);
        std::cout << "UNIFORM NAME: " << j->uniformName << ", index: " << arrayIndex << std::endl;
        uniformName = uniformName.substr(0, arrayLeftBracket);
      }

      auto uniformInfo = Graphics::API::ShaderDetails::UniformInfo{};
      if (shader.GetNamedUniform(uniformName, uniformInfo))
      {
        // write into correct uniform buffer
        auto dst = (mUboMemory[uniformInfo.bufferIndex].data() + uniformInfo.offset);
        switch (j->propertyPtr
                 ->GetType())
        {
          case Property::Type::FLOAT:
          case Property::Type::INTEGER:
          case Property::Type::BOOLEAN:
          {
            std::cout << uniformInfo.name << ":[" << uniformInfo.bufferIndex << "]: " << "Writing 32bit offset: "
                      << uniformInfo.offset << ", size: " << sizeof(float) << std::endl;
            dst += sizeof(float) * arrayIndex;
            memcpy(dst, &j->propertyPtr
                          ->GetFloat(updateBufferIndex), sizeof(float));
            break;
          }
          case Property::Type::VECTOR2:
          {
            std::cout << uniformInfo.name << ":[" << uniformInfo.bufferIndex << "]: " << "Writing vec2 offset: "
                      << uniformInfo.offset << ", size: " << sizeof(Vector2) << std::endl;
            dst += /* sizeof(Vector2) * */arrayIndex * 16; // todo: use array stride from spirv
            memcpy(dst, &j->propertyPtr
                          ->GetVector2(updateBufferIndex), sizeof(Vector2));
            break;
          }
          case Property::Type::VECTOR3:
          {
            std::cout << uniformInfo.name << ":[" << uniformInfo.bufferIndex << "]: " << "Writing vec3 offset: "
                      << uniformInfo.offset << ", size: " << sizeof(Vector3) << std::endl;
            dst += sizeof(Vector3) * arrayIndex;
            memcpy(dst, &j->propertyPtr
                          ->GetVector3(updateBufferIndex), sizeof(Vector3));
            break;
          }
          case Property::Type::VECTOR4:
          {
            std::cout << uniformInfo.name << ":[" << uniformInfo.bufferIndex << "]: " << "Writing vec4 offset: "
                      << uniformInfo.offset << ", size: " << sizeof(Vector4) << std::endl;
            dst += sizeof(float) * arrayIndex;
            memcpy(dst, &j->propertyPtr
                          ->GetVector4(updateBufferIndex), sizeof(Vector4));
            break;
          }
          case Property::Type::MATRIX:
          {
            std::cout << uniformInfo.name << ":[" << uniformInfo.bufferIndex << "]: " << "Writing mat4 offset: "
                      << uniformInfo.offset << ", size: " << sizeof(Matrix) << std::endl;
            dst += sizeof(Matrix) * arrayIndex;
            memcpy(dst, &j->propertyPtr
                          ->GetMatrix(updateBufferIndex), sizeof(Matrix));
            break;
          }
          case Property::Type::MATRIX3:
          {
            std::cout << uniformInfo.name << ":[" << uniformInfo.bufferIndex << "]: " << "Writing mat3 offset: "
                      << uniformInfo.offset << ", size: " << sizeof(Matrix3) << std::endl;
            dst += sizeof(Matrix3) * arrayIndex;
            memcpy(dst, &j->propertyPtr
                          ->GetMatrix3(updateBufferIndex), sizeof(Matrix3));
            break;
          }
          default:
          {
          }
        }
      }
    }
  }

  /**
   * Prepare textures
   */
  auto textureBindings = Graphics::API::RenderCommand::NewTextureBindings();
  auto samplers        = shader.GetSamplers();

  if (mTextureSet)
  {
    if (!samplers.empty())
    {
      for (auto i = 0u; i < mTextureSet->GetTextureCount(); ++i)
      {

        auto texture    = mTextureSet->GetTexture(i);
        auto gfxTexture = texture->GetGfxObject();
        auto binding    = Graphics::API::RenderCommand::TextureBinding{}
          .SetBinding(samplers[i].binding)
          .SetTexture(texture->GetGfxObject())
          .SetSampler(nullptr);

        textureBindings.emplace_back(binding);
      }
    }
  }

  // Build render command
  // todo: this may be deferred until all render items are sorted, otherwise
  // certain optimisations cannot be done

  const auto &vb = mGeometry->GetVertexBuffers()[0];

  // set optional index buffer
  bool usesIndexBuffer{false};
  auto topology  = Graphics::API::Pipeline::PrimitiveTopology::TRIANGLE_STRIP;
  auto geometryTopology = mGeometry->GetType();
  switch (geometryTopology)
  {
    case Dali::Geometry::Type::TRIANGLE_STRIP:
    {
      topology = Graphics::API::Pipeline::PrimitiveTopology::TRIANGLE_STRIP;
      break;
    }
    default:
    {
      topology = Graphics::API::Pipeline::PrimitiveTopology::TRIANGLE_LIST;
    }
  }

  if ((usesIndexBuffer = mGeometry->HasIndexBuffer()))
  {
    mGfxRenderCommand->BindIndexBuffer(Graphics::API::RenderCommand::IndexBufferBinding()
                                         .SetBuffer(mGeometry->GetIndexBuffer())
                                         .SetOffset(0)
    );
  }


  mGfxRenderCommand->PushConstants( std::move(pushConstantsBindings) );
  mGfxRenderCommand->BindVertexBuffers( std::move( vertexBuffers ) );
  mGfxRenderCommand->BindTextures( std::move(textureBindings) );



  // create pipeline
  if(!mGfxPipeline)
  {
    mGfxPipeline = controller.CreatePipeline(controller.GetPipelineFactory()

                                                       // vertex input
                                                       .SetVertexInputState( vi )

                                                       // shaders
                                                       .SetShaderState( Pipeline::ShaderState()
                                                                                  .SetShaderProgram( shader ))

                                                       // input assembly
                                                       .SetInputAssemblyState( Pipeline::InputAssemblyState()
                                                                                  .SetTopology( topology )
                                                                                  .SetPrimitiveRestartEnable( true ))

                                                         // viewport ( if zeroes then framebuffer size used )
                                                       .SetViewportState( Pipeline::ViewportState()
                                                                                  .SetViewport( { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 } ))

                                                       // depth stencil
                                                       .SetDepthStencilState( Pipeline::DepthStencilState()
                                                                                  .SetDepthTestEnable( false ))

                                                       // color blend
                                                       .SetColorBlendState( Pipeline::ColorBlendState()
                                                                                  .SetBlendEnable( mBlendMode != BlendMode::OFF ? true : false  )
                                                                                  .SetColorComponentsWriteBits( 0xff )
                                                                                  .SetSrcColorBlendFactor( Pipeline::BlendFactor::SRC_ALPHA )
                                                                                  .SetDstColorBlendFactor( Pipeline::BlendFactor::ONE_MINUS_SRC_ALPHA )
                                                                                  .SetSrcAlphaBlendFactor( Pipeline::BlendFactor::ONE )
                                                                                  .SetDstAlphaBlendFactor( Pipeline::BlendFactor::ONE_MINUS_SRC_ALPHA )
                                                                                  .SetColorBlendOp( Pipeline::BlendOp::ADD )
                                                                                  .SetAlphaBlendOp( Pipeline::BlendOp::ADD )
                                                                                  .SetLogicOpEnable( false ))

                                                       // rasterization
                                                       .SetRasterizationState( Pipeline::RasterizationState()
                                                                                  .SetCullMode( Pipeline::CullMode::NONE )
                                                                                  .SetPolygonMode( Pipeline::PolygonMode::FILL )
                                                                                  .SetFrontFace( Pipeline::FrontFace::CLOCKWISE )));
  }

  // bind pipeline
  mGfxRenderCommand->BindPipeline( *mGfxPipeline.get() );
  if(usesIndexBuffer)
  {
    mGfxRenderCommand->Draw(std::move(Graphics::API::RenderCommand::DrawCommand{}
                                        .SetFirstIndex( uint32_t(mIndexedDrawFirstElement) )
                                        .SetDrawType(Graphics::API::RenderCommand::DrawType::INDEXED_DRAW)
                                        .SetFirstInstance(0u)
                                        .SetIndicesCount( mGeometry->GetIndexBufferElementCount() )
                                        .SetInstanceCount(1u)));
  }
  else
  {
    mGfxRenderCommand->Draw(std::move(Graphics::API::RenderCommand::DrawCommand{}
                                        .SetFirstVertex(0u)
                                        .SetDrawType(Graphics::API::RenderCommand::DrawType::VERTEX_DRAW)
                                        .SetFirstInstance(0u)
                                        .SetVertexCount(vb->GetElementCount())
                                        .SetInstanceCount(1u)));
  }
  std::cout << "done\n";
}

void Renderer::WriteUniform( const std::string& name, const void* data, uint32_t size )
{
  auto& gfxShader = mShader->GetGfxObject().Get();
  auto uniformInfo = Graphics::API::ShaderDetails::UniformInfo{};
  if( gfxShader.GetNamedUniform( name, uniformInfo ) )
  {
    auto dst = (mUboMemory[uniformInfo.bufferIndex].data()+uniformInfo.offset);
    memcpy( dst, data, size );
  }
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
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
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

void Renderer::SetBlendMode( BlendMode::Type blendingMode )
{
  mBlendMode = blendingMode;
}

void Renderer::SetBlendingOptions( unsigned int options )
{
  if( mBlendBitmask != options)
  {
    mBlendBitmask = options;
  }
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

void Renderer::SetIndexedDrawFirstElement( size_t firstElement )
{
  mIndexedDrawFirstElement = firstElement;
}

void Renderer::SetIndexedDrawElementsCount( size_t elementsCount )
{
  mIndexedDrawElementsCount = elementsCount;
}

void Renderer::EnablePreMultipliedAlpha( bool preMultipled )
{
  mPremultipledAlphaEnabled = preMultipled;
}

void Renderer::SetDepthWriteMode( DepthWriteMode::Type depthWriteMode )
{
  mDepthWriteMode = depthWriteMode;
}

void Renderer::SetDepthTestMode( DepthTestMode::Type depthTestMode )
{
  mDepthTestMode = depthTestMode;
}

void Renderer::SetDepthFunction( DepthFunction::Type depthFunction )
{
  mDepthFunction = depthFunction;
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

const Vector4& Renderer::GetBlendColor() const
{
  if( mBlendColor )
  {
    return *mBlendColor;
  }
  return Color::TRANSPARENT;
}

const CollectedUniformMap& Renderer::GetUniformMap( BufferIndex bufferIndex ) const
{
  return mCollectedUniformMap[bufferIndex];
}

Renderer::Opacity Renderer::GetOpacity( BufferIndex updateBufferIndex, const Node& node ) const
{
  Renderer::Opacity opacity = Renderer::OPAQUE;

  switch( mBlendMode )
  {
    case BlendMode::ON: // If the renderer should always be use blending
    {
      opacity = Renderer::TRANSLUCENT;
      break;
    }
    case BlendMode::AUTO:
    {
      bool shaderRequiresBlending( mShader->HintEnabled( Dali::Shader::Hint::OUTPUT_IS_TRANSPARENT ) );
      if( shaderRequiresBlending || ( mTextureSet && mTextureSet->HasAlpha() ) )
      {
        opacity = Renderer::TRANSLUCENT;
      }
      else // renderer should determine opacity using the actor color
      {
        float alpha = node.GetWorldColor( updateBufferIndex ).a;
        if( alpha <= FULLY_TRANSPARENT )
        {
          opacity = TRANSPARENT;
        }
        else if( alpha <= FULLY_OPAQUE )
        {
          opacity = TRANSLUCENT;
        }
      }
      break;
    }
    case BlendMode::OFF: // the renderer should never use blending
    default:
    {
      opacity = Renderer::OPAQUE;
      break;
    }
  }

  return opacity;
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
  // One of our child objects has changed it's connections. Ensure the uniform
  // map gets regenerated during PrepareRender
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::ConnectedUniformMapChanged()
{
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::UniformMappingsChanged( const UniformMap& mappings )
{
  // The mappings are either from PropertyOwner base class, or the Actor
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::ObservedObjectDestroyed(PropertyOwner& owner)
{
  if( reinterpret_cast<PropertyOwner*>(mShader) == &owner )
  {
    mShader = NULL;
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#pragma GCC diagnostic pop
