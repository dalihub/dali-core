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

// Flags for re-sending data to renderer.
enum Flags
{
  RESEND_DATA_PROVIDER               = 1 << 0,
  RESEND_GEOMETRY                    = 1 << 1,
  RESEND_FACE_CULLING_MODE           = 1 << 2,
  RESEND_BLEND_COLOR                 = 1 << 3,
  RESEND_BLEND_BIT_MASK              = 1 << 4,
  RESEND_PREMULTIPLIED_ALPHA         = 1 << 5,
  RESEND_INDEXED_DRAW_FIRST_ELEMENT  = 1 << 6,
  RESEND_INDEXED_DRAW_ELEMENTS_COUNT = 1 << 7,
  RESEND_DEPTH_WRITE_MODE            = 1 << 8,
  RESEND_DEPTH_TEST_MODE             = 1 << 9,
  RESEND_DEPTH_FUNCTION              = 1 << 10,
  RESEND_RENDER_MODE                 = 1 << 11,
  RESEND_STENCIL_FUNCTION            = 1 << 12,
  RESEND_STENCIL_FUNCTION_MASK       = 1 << 13,
  RESEND_STENCIL_FUNCTION_REFERENCE  = 1 << 14,
  RESEND_STENCIL_MASK                = 1 << 15,
  RESEND_STENCIL_OPERATION_ON_FAIL   = 1 << 16,
  RESEND_STENCIL_OPERATION_ON_Z_FAIL = 1 << 17,
  RESEND_STENCIL_OPERATION_ON_Z_PASS = 1 << 18,
  RESEND_WRITE_TO_COLOR_BUFFER       = 1 << 19,
};

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
: mRenderDataProvider(),
  mTextureSet( NULL ),
  mGeometry( NULL ),
  mShader( NULL ),
  mBlendColor( NULL ),
  mStencilParameters( RenderMode::AUTO, StencilFunction::ALWAYS, 0xFF, 0x00, 0xFF, StencilOperation::KEEP, StencilOperation::KEEP, StencilOperation::KEEP ),
  mIndexedDrawFirstElement( 0u ),
  mIndexedDrawElementsCount( 0u ),
  mBlendBitmask( 0u ),
  mRegenerateUniformMap( 0u ),
  mResendFlag( 0u ),
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


void* AllocateUniformBufferMemory( size_t size )
{
  return nullptr;
}



void Renderer::PrepareRender( Graphics::API::Controller& controller, BufferIndex updateBufferIndex )
{
  // prepare all stuff
  auto gfxShader = mShader->GetGfxObject();

  if( !mGfxRenderCommand )
  {
    mGfxRenderCommand = controller.AllocateRenderCommand();
  }

  /**
   * Prepare vertex attribute buffer bindings
   */
  uint32_t bindingIndex { 0u };
  uint32_t locationIndex { 0u };
  auto vertexAttributeBindings = Graphics::API::RenderCommand::NewVertexAttributeBufferBindings();
  for( auto&& vertexBuffer : mGeometry->GetVertexBuffers() )
  {
    auto attributeCountInForBuffer = vertexBuffer->GetAttributeCount();

    // update vertex buffer if necessary
    vertexBuffer->Update( controller );

    for( auto i = 0u; i < attributeCountInForBuffer; ++i )
    {
      // create binding per attribute
      auto binding = Graphics::API::RenderCommand::VertexAttributeBufferBinding{}
        .SetOffset( (vertexBuffer->GetFormat()->components[i]).offset )
        .SetBinding( bindingIndex )
        .SetBuffer( vertexBuffer->GetGfxObject() )
        .SetInputAttributeRate( Graphics::API::RenderCommand::InputAttributeRate::PER_VERTEX )
        .SetLocation( locationIndex + i )
        .SetStride( vertexBuffer->GetFormat()->size );
      vertexAttributeBindings.emplace_back( binding );
    }
  }

  /**
   * REGENERATE UNIFORM MAP
   */
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

      unsigned int index = 0;
      for( CollectedUniformMap::Iterator iter = oldMap.Begin(), end = oldMap.End() ; iter != end ; ++iter, ++index )
      {
        localMap[index] = *iter;
      }
    }

    mUniformMapChanged[updateBufferIndex] = true;
    mRegenerateUniformMap--;
  }

  auto& shader = mShader->GetGfxObject().Get();
  auto uboCount = shader.GetUniformBlockCount();

  auto pushConstantsBindings = Graphics::API::RenderCommand::NewPushConstantsBindings( uboCount );

  // allocate new command ( may be not necessary at all )
  // mGfxRenderCommand = Graphics::API::RenderCommandBuilder().Build();

  // see if we need to reallocate memory for each UBO
  // todo: do it only when shader has changed
  if( mUboMemory.size() != uboCount )
  {
    mUboMemory.resize(uboCount);
  }

  for( auto i = 0u; i < uboCount; ++i )
  {
    Graphics::API::ShaderDetails::UniformBlockInfo ubInfo;

    std::cout<<sizeof(ubInfo) << std::endl;

    shader.GetUniformBlock( i, ubInfo );

    if( mUboMemory[i].size() != ubInfo.size )
    {
      mUboMemory[i].resize( ubInfo.size );
    }

    // Set push constant bindings
    auto &pushBinding = pushConstantsBindings[i];
    pushBinding.data = mUboMemory[i].data();
    pushBinding.size = uint32_t(mUboMemory[i].size());
    pushBinding.binding = ubInfo.binding;
  }

  // add built-in uniforms

  // write to memory
  for( auto&& i : mCollectedUniformMap )
  {
    for( auto&& j : i )
    {
      auto uniformInfo = Graphics::API::ShaderDetails::UniformInfo{};
      if( shader.GetNamedUniform( j->uniformName, uniformInfo ) )
      {
        // write into correct uniform buffer
        auto dst = (mUboMemory[uniformInfo.bufferIndex].data()+uniformInfo.offset);
        switch( j->propertyPtr->GetType() )
        {
          case Property::Type::FLOAT:
          case Property::Type::INTEGER:
          case Property::Type::BOOLEAN:
          {
            std::cout << uniformInfo.name << ":["<<uniformInfo.bufferIndex<<"]: " << "Writing 32bit offset: " << uniformInfo.offset << ", size: " << sizeof(float) << std::endl;
            memcpy( dst, &j->propertyPtr->GetFloat( updateBufferIndex ), sizeof(float) );
            break;
          }
          case Property::Type::VECTOR2:
          {
            std::cout << uniformInfo.name << ":["<<uniformInfo.bufferIndex<<"]: " << "Writing vec2 offset: " << uniformInfo.offset << ", size: " << sizeof(Vector2) << std::endl;
            memcpy( dst, &j->propertyPtr->GetVector2( updateBufferIndex ), sizeof(Vector2) );
            break;
          }
          case Property::Type::VECTOR3:
          {
            std::cout << uniformInfo.name << ":["<<uniformInfo.bufferIndex<<"]: " <<  "Writing vec3 offset: " << uniformInfo.offset << ", size: " << sizeof(Vector3) << std::endl;
            memcpy( dst, &j->propertyPtr->GetVector3( updateBufferIndex ), sizeof(Vector3) );
            break;
          }
          case Property::Type::VECTOR4:
          {
            std::cout << uniformInfo.name << ":["<<uniformInfo.bufferIndex<<"]: " << "Writing vec4 offset: " << uniformInfo.offset << ", size: " << sizeof(Vector4) << std::endl;
            memcpy( dst, &j->propertyPtr->GetVector4( updateBufferIndex ), sizeof(Vector4) );
            break;
          }
          case Property::Type::MATRIX:
          {
            std::cout << uniformInfo.name << ":["<<uniformInfo.bufferIndex<<"]: " << "Writing mat4 offset: " << uniformInfo.offset << ", size: " << sizeof(Matrix) << std::endl;
            memcpy( dst, &j->propertyPtr->GetMatrix( updateBufferIndex ), sizeof(Matrix) );
            break;
          }
          case Property::Type::MATRIX3:
          {
            std::cout << uniformInfo.name << ":["<<uniformInfo.bufferIndex<<"]: " << "Writing mat3 offset: " << uniformInfo.offset << ", size: " << sizeof(Matrix3) << std::endl;
            memcpy( dst, &j->propertyPtr->GetMatrix3( updateBufferIndex ), sizeof(Matrix3) );
            break;
          }
          default:
          {}
        }
      }
    }
  }

  /**
   * Prepare textures
   */
  auto textureBindings = Graphics::API::RenderCommand::NewTextureBindings();
  auto samplers = shader.GetSamplers();

  for( auto i = 0u; i < mTextureSet->GetTextureCount(); ++i )
  {
    auto texture = mTextureSet->GetTexture( i );
    auto gfxTexture = texture->GetGfxObject();
    auto binding = Graphics::API::RenderCommand::TextureBinding{}
        .SetBinding( samplers[i].binding )
        .SetTexture( texture->GetGfxObject() )
        .SetSampler( nullptr );

    textureBindings.emplace_back( binding );
  }

  // Build render command
  // todo: this may be deferred until all render items are sorted, otherwise
  // certain optimisations cannot be done

  const auto& vb = mGeometry->GetVertexBuffers()[0];
  //vb->Update()
  mGfxRenderCommand->PushConstants( std::move(pushConstantsBindings) );
  mGfxRenderCommand->BindVertexBuffers( std::move(vertexAttributeBindings) );
  mGfxRenderCommand->BindTextures( std::move(textureBindings) );
  mGfxRenderCommand->BindRenderState( std::move( Graphics::API::RenderCommand::RenderState{}
                                       .SetShader( mShader->GetGfxObject() ) ) );
  mGfxRenderCommand->Draw( std::move(Graphics::API::RenderCommand::DrawCommand{}
                   .SetFirstVertex(0u)
                   .SetDrawType( Graphics::API::RenderCommand::DrawType::VERTEX_DRAW )
                   .SetFirstInstance(0u)
                   .SetVertexCount( vb->GetElementCount() )
                   .SetInstanceCount( 1u )));

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

void Renderer::PrepareRender( BufferIndex updateBufferIndex )
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

  if( mResendFlag != 0 )
  {
    // This used to send messages to obsolete Render::Renderer at this point
    mResendFlag = 0;
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
  mResendFlag |= RESEND_DATA_PROVIDER;
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
  mResendFlag |= RESEND_DATA_PROVIDER;
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
  mResendFlag |= RESEND_FACE_CULLING_MODE;
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
    mResendFlag |= RESEND_BLEND_BIT_MASK;
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

  mResendFlag |= RESEND_BLEND_COLOR;
}

void Renderer::SetIndexedDrawFirstElement( size_t firstElement )
{
  mIndexedDrawFirstElement = firstElement;
  mResendFlag |= RESEND_INDEXED_DRAW_FIRST_ELEMENT;
}

void Renderer::SetIndexedDrawElementsCount( size_t elementsCount )
{
  mIndexedDrawElementsCount = elementsCount;
  mResendFlag |= RESEND_INDEXED_DRAW_ELEMENTS_COUNT;
}

void Renderer::EnablePreMultipliedAlpha( bool preMultipled )
{
  mPremultipledAlphaEnabled = preMultipled;
  mResendFlag |= RESEND_PREMULTIPLIED_ALPHA;
}

void Renderer::SetDepthWriteMode( DepthWriteMode::Type depthWriteMode )
{
  mDepthWriteMode = depthWriteMode;
  mResendFlag |= RESEND_DEPTH_WRITE_MODE;
}

void Renderer::SetDepthTestMode( DepthTestMode::Type depthTestMode )
{
  mDepthTestMode = depthTestMode;
  mResendFlag |= RESEND_DEPTH_TEST_MODE;
}

void Renderer::SetDepthFunction( DepthFunction::Type depthFunction )
{
  mDepthFunction = depthFunction;
  mResendFlag |= RESEND_DEPTH_FUNCTION;
}

void Renderer::SetRenderMode( RenderMode::Type mode )
{
  mStencilParameters.renderMode = mode;
  mResendFlag |= RESEND_RENDER_MODE;
}

void Renderer::SetStencilFunction( StencilFunction::Type stencilFunction )
{
  mStencilParameters.stencilFunction = stencilFunction;
  mResendFlag |= RESEND_STENCIL_FUNCTION;
}

void Renderer::SetStencilFunctionMask( int stencilFunctionMask )
{
  mStencilParameters.stencilFunctionMask = stencilFunctionMask;
  mResendFlag |= RESEND_STENCIL_FUNCTION_MASK;
}

void Renderer::SetStencilFunctionReference( int stencilFunctionReference )
{
  mStencilParameters.stencilFunctionReference = stencilFunctionReference;
  mResendFlag |= RESEND_STENCIL_FUNCTION_REFERENCE;
}

void Renderer::SetStencilMask( int stencilMask )
{
  mStencilParameters.stencilMask = stencilMask;
  mResendFlag |= RESEND_STENCIL_MASK;
}

void Renderer::SetStencilOperationOnFail( StencilOperation::Type stencilOperationOnFail )
{
  mStencilParameters.stencilOperationOnFail = stencilOperationOnFail;
  mResendFlag |= RESEND_STENCIL_OPERATION_ON_FAIL;
}

void Renderer::SetStencilOperationOnZFail( StencilOperation::Type stencilOperationOnZFail )
{
  mStencilParameters.stencilOperationOnZFail = stencilOperationOnZFail;
  mResendFlag |= RESEND_STENCIL_OPERATION_ON_Z_FAIL;
}

void Renderer::SetStencilOperationOnZPass( StencilOperation::Type stencilOperationOnZPass )
{
  mStencilParameters.stencilOperationOnZPass = stencilOperationOnZPass;
  mResendFlag |= RESEND_STENCIL_OPERATION_ON_Z_PASS;
}

//Called when SceneGraph::Renderer is added to update manager ( that happens when an "event-thread renderer" is created )
void Renderer::ConnectToSceneGraph( BufferIndex bufferIndex )
{
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

//Called just before destroying the scene-graph renderer ( when the "event-thread renderer" is no longer referenced )
void Renderer::DisconnectFromSceneGraph(  BufferIndex bufferIndex )
{
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
  mResendFlag |= RESEND_DATA_PROVIDER;
}

void Renderer::TextureSetDeleted()
{
  mTextureSet = NULL;

  mResendFlag |= RESEND_DATA_PROVIDER;
}
void Renderer::ConnectionsChanged( PropertyOwner& object )
{
  // One of our child objects has changed it's connections. Ensure the uniform
  // map gets regenerated during PrepareRender
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  // Ensure the child object pointers get re-sent to the renderer
  mResendFlag |= RESEND_DATA_PROVIDER;
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
