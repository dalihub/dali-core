/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace // unnamed namespace
{

const uint32_t UNIFORM_MAP_READY      = 0;
const uint32_t COPY_UNIFORM_MAP       = 1;
const uint32_t REGENERATE_UNIFORM_MAP = 2;

//Memory pool used to allocate new renderers. Memory used by this pool will be released when shutting down DALi
MemoryPoolObjectAllocator<Renderer> gRendererMemoryPool;

void AddMappings( CollectedUniformMap& localMap, const UniformMap& uniformMap )
{
  // Iterate thru uniformMap.
  // Any maps that aren't in localMap should be added in a single step

  // keep a static vector to avoid temporary heap allocation.
  // As this function gets called only from update thread we don't have to
  // make it thread safe (so no need to keep a thread_local variable).
  static CollectedUniformMap newUniformMappings;

  newUniformMappings.Clear();

  for( UniformMap::SizeType i = 0, count=uniformMap.Count(); i<count; ++i )
  {
    UniformPropertyMapping::Hash nameHash = uniformMap[i].uniformNameHash;
    bool found = false;

    for( CollectedUniformMap::Iterator iter = localMap.Begin() ; iter != localMap.End() ; ++iter )
    {
      const UniformPropertyMapping* map = (*iter);
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

    for( CollectedUniformMap::Iterator iter = newUniformMappings.Begin(),
           end = newUniformMappings.End() ;
         iter != end ;
         ++iter )
    {
      const UniformPropertyMapping* map = (*iter);
      localMap.PushBack( map );
    }
  }
}

// Flags for re-sending data to renderer.
enum Flags
{
  RESEND_GEOMETRY                    = 1 << 0,
  RESEND_FACE_CULLING_MODE           = 1 << 1,
  RESEND_BLEND_COLOR                 = 1 << 2,
  RESEND_BLEND_BIT_MASK              = 1 << 3,
  RESEND_PREMULTIPLIED_ALPHA         = 1 << 4,
  RESEND_INDEXED_DRAW_FIRST_ELEMENT  = 1 << 5,
  RESEND_INDEXED_DRAW_ELEMENTS_COUNT = 1 << 6,
  RESEND_DEPTH_WRITE_MODE            = 1 << 7,
  RESEND_DEPTH_TEST_MODE             = 1 << 8,
  RESEND_DEPTH_FUNCTION              = 1 << 9,
  RESEND_RENDER_MODE                 = 1 << 10,
  RESEND_STENCIL_FUNCTION            = 1 << 11,
  RESEND_STENCIL_FUNCTION_MASK       = 1 << 12,
  RESEND_STENCIL_FUNCTION_REFERENCE  = 1 << 13,
  RESEND_STENCIL_MASK                = 1 << 14,
  RESEND_STENCIL_OPERATION_ON_FAIL   = 1 << 15,
  RESEND_STENCIL_OPERATION_ON_Z_FAIL = 1 << 16,
  RESEND_STENCIL_OPERATION_ON_Z_PASS = 1 << 17,
  RESEND_WRITE_TO_COLOR_BUFFER       = 1 << 18,
  RESEND_SHADER                      = 1 << 19,
  RESEND_DRAW_COMMANDS               = 1 << 20
};

} // Anonymous namespace

Renderer* Renderer::New()
{
  return new ( gRendererMemoryPool.AllocateRawThreadSafe() ) Renderer();
}

Renderer::Renderer()
: mSceneController( nullptr ),
  mRenderer( nullptr ),
  mTextureSet( nullptr ),
  mGeometry( nullptr ),
  mShader( nullptr ),
  mRenderDataProvider( nullptr ),
  mBlendColor( nullptr ),
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
  mRenderingBehavior( DevelRenderer::Rendering::IF_REQUIRED ),
  mPremultipledAlphaEnabled( false ),
  mOpacity( 1.0f ),
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
    mTextureSet = nullptr;
  }
  if( mShader )
  {
    mShader->RemoveConnectionObserver( *this );
    mShader = nullptr;
  }
}

void Renderer::operator delete( void* ptr )
{
  gRendererMemoryPool.FreeThreadSafe( static_cast<Renderer*>( ptr ) );
}


bool Renderer::PrepareRender( BufferIndex updateBufferIndex )
{
  if( mRegenerateUniformMap == UNIFORM_MAP_READY )
  {
    mUniformMapChanged[updateBufferIndex] = false;
  }
  else
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

      uint32_t index=0;
      for( CollectedUniformMap::Iterator iter = oldMap.Begin(), end = oldMap.End() ; iter != end ; ++iter, ++index )
      {
        localMap[index] = *iter;
      }
    }

    mUniformMapChanged[updateBufferIndex] = true;
    mRegenerateUniformMap--;
  }

  bool rendererUpdated = mUniformMapChanged[updateBufferIndex] || mResendFlag;

  if( mResendFlag != 0 )
  {
    if( mResendFlag & RESEND_GEOMETRY )
    {
      typedef MessageValue1< Render::Renderer, Render::Geometry* > DerivedType;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetGeometry, mGeometry );
    }

    if( mResendFlag & RESEND_DRAW_COMMANDS )
    {
      using DerivedType = MessageValue2<Render::Renderer, Dali::DevelRenderer::DrawCommand*, uint32_t>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetDrawCommands, mDrawCommands.data(), mDrawCommands.size() );
    }

    if( mResendFlag & RESEND_FACE_CULLING_MODE )
    {
      using DerivedType = MessageValue1<Render::Renderer, FaceCullingMode::Type>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetFaceCullingMode, mFaceCullingMode );
    }

    if( mResendFlag & RESEND_BLEND_BIT_MASK )
    {
      using DerivedType = MessageValue1<Render::Renderer, uint32_t>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetBlendingBitMask, mBlendBitmask );
    }

    if( mResendFlag & RESEND_BLEND_COLOR )
    {
      using DerivedType = MessageValue1<Render::Renderer, Vector4>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetBlendColor, GetBlendColor() );
    }

    if( mResendFlag & RESEND_PREMULTIPLIED_ALPHA  )
    {
      using DerivedType = MessageValue1<Render::Renderer, bool>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::EnablePreMultipliedAlpha, mPremultipledAlphaEnabled );
    }

    if( mResendFlag & RESEND_INDEXED_DRAW_FIRST_ELEMENT )
    {
      using DerivedType = MessageValue1<Render::Renderer, uint32_t>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetIndexedDrawFirstElement, mIndexedDrawFirstElement );
    }

    if( mResendFlag & RESEND_INDEXED_DRAW_ELEMENTS_COUNT )
    {
      using DerivedType = MessageValue1<Render::Renderer, uint32_t>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetIndexedDrawElementsCount, mIndexedDrawElementsCount );
    }

    if( mResendFlag & RESEND_DEPTH_WRITE_MODE )
    {
      using DerivedType = MessageValue1<Render::Renderer, DepthWriteMode::Type>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetDepthWriteMode, mDepthWriteMode );
    }

    if( mResendFlag & RESEND_DEPTH_TEST_MODE )
    {
      using DerivedType = MessageValue1<Render::Renderer, DepthTestMode::Type>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetDepthTestMode, mDepthTestMode );
    }

    if( mResendFlag & RESEND_DEPTH_FUNCTION )
    {
      using DerivedType = MessageValue1<Render::Renderer, DepthFunction::Type>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetDepthFunction, mDepthFunction );
    }

    if( mResendFlag & RESEND_RENDER_MODE )
    {
      using DerivedType = MessageValue1<Render::Renderer, RenderMode::Type>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetRenderMode, mStencilParameters.renderMode );
    }

    if( mResendFlag & RESEND_STENCIL_FUNCTION )
    {
      using DerivedType = MessageValue1<Render::Renderer, StencilFunction::Type>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetStencilFunction, mStencilParameters.stencilFunction );
    }

    if( mResendFlag & RESEND_STENCIL_FUNCTION_MASK )
    {
      using DerivedType = MessageValue1<Render::Renderer, int>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetStencilFunctionMask, mStencilParameters.stencilFunctionMask );
    }

    if( mResendFlag & RESEND_STENCIL_FUNCTION_REFERENCE )
    {
      using DerivedType = MessageValue1<Render::Renderer, int>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetStencilFunctionReference, mStencilParameters.stencilFunctionReference );
    }

    if( mResendFlag & RESEND_STENCIL_MASK )
    {
      using DerivedType = MessageValue1<Render::Renderer, int>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetStencilMask, mStencilParameters.stencilMask );
    }

    if( mResendFlag & RESEND_STENCIL_OPERATION_ON_FAIL )
    {
      using DerivedType = MessageValue1<Render::Renderer, StencilOperation::Type>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetStencilOperationOnFail, mStencilParameters.stencilOperationOnFail );
    }

    if( mResendFlag & RESEND_STENCIL_OPERATION_ON_Z_FAIL )
    {
      using DerivedType = MessageValue1<Render::Renderer, StencilOperation::Type>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetStencilOperationOnZFail, mStencilParameters.stencilOperationOnZFail );
    }

    if( mResendFlag & RESEND_STENCIL_OPERATION_ON_Z_PASS )
    {
      using DerivedType = MessageValue1<Render::Renderer, StencilOperation::Type>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetStencilOperationOnZPass, mStencilParameters.stencilOperationOnZPass );
    }

    if( mResendFlag & RESEND_SHADER )
    {
      using DerivedType = MessageValue1<Render::Renderer, bool>;
      uint32_t* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
      new (slot) DerivedType( mRenderer, &Render::Renderer::SetShaderChanged, true );
    }

    mResendFlag = 0;
  }

  return rendererUpdated;
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

  UpdateTextureSet();
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
  mResendFlag |= RESEND_GEOMETRY | RESEND_SHADER;

  if( mRenderDataProvider )
  {
    mRenderDataProvider->mShader = mShader;
  }
}

void Renderer::SetGeometry( Render::Geometry* geometry )
{
  DALI_ASSERT_DEBUG( geometry != NULL && "Geometry pointer is NULL");
  mGeometry = geometry;

  if( mRenderer )
  {
    mResendFlag |= RESEND_GEOMETRY;
  }
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

void Renderer::SetBlendingOptions( uint32_t options )
{
  if( mBlendBitmask != options)
  {
    mBlendBitmask = options;
    mResendFlag |= RESEND_BLEND_BIT_MASK;
  }
}

uint32_t Renderer::GetBlendingOptions() const
{
  return mBlendBitmask;
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

  mResendFlag |= RESEND_BLEND_COLOR;
}

Vector4 Renderer::GetBlendColor() const
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
  mResendFlag |= RESEND_INDEXED_DRAW_FIRST_ELEMENT;
}

uint32_t Renderer::GetIndexedDrawFirstElement() const
{
  return mIndexedDrawFirstElement;
}

void Renderer::SetIndexedDrawElementsCount( uint32_t elementsCount )
{
  mIndexedDrawElementsCount = elementsCount;
  mResendFlag |= RESEND_INDEXED_DRAW_ELEMENTS_COUNT;
}

uint32_t Renderer::GetIndexedDrawElementsCount() const
{
  return mIndexedDrawElementsCount;
}

void Renderer::EnablePreMultipliedAlpha( bool preMultipled )
{
  mPremultipledAlphaEnabled = preMultipled;
  mResendFlag |= RESEND_PREMULTIPLIED_ALPHA;
}

bool Renderer::IsPreMultipliedAlphaEnabled() const
{
  return mPremultipledAlphaEnabled;
}

void Renderer::SetDepthWriteMode( DepthWriteMode::Type depthWriteMode )
{
  mDepthWriteMode = depthWriteMode;
  mResendFlag |= RESEND_DEPTH_WRITE_MODE;
}

DepthWriteMode::Type Renderer::GetDepthWriteMode() const
{
  return mDepthWriteMode;
}

void Renderer::SetDepthTestMode( DepthTestMode::Type depthTestMode )
{
  mDepthTestMode = depthTestMode;
  mResendFlag |= RESEND_DEPTH_TEST_MODE;
}

DepthTestMode::Type Renderer::GetDepthTestMode() const
{
  return mDepthTestMode;
}

void Renderer::SetDepthFunction( DepthFunction::Type depthFunction )
{
  mDepthFunction = depthFunction;
  mResendFlag |= RESEND_DEPTH_FUNCTION;
}

DepthFunction::Type Renderer::GetDepthFunction() const
{
  return mDepthFunction;
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

const Render::Renderer::StencilParameters& Renderer::GetStencilParameters() const
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

//Called when SceneGraph::Renderer is added to update manager ( that happens when an "event-thread renderer" is created )
void Renderer::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
  mSceneController = &sceneController;

  mRenderDataProvider = new RenderDataProvider( mOpacity );
  mRenderDataProvider->mUniformMapDataProvider = this;

  mRenderer = Render::Renderer::New( mRenderDataProvider, mGeometry, mBlendBitmask, GetBlendColor(), static_cast< FaceCullingMode::Type >( mFaceCullingMode ),
                                         mPremultipledAlphaEnabled, mDepthWriteMode, mDepthTestMode, mDepthFunction, mStencilParameters );

  OwnerPointer< Render::Renderer > transferOwnership( mRenderer );
  mSceneController->GetRenderMessageDispatcher().AddRenderer( transferOwnership );
}

//Called just before destroying the scene-graph renderer ( when the "event-thread renderer" is no longer referenced )
void Renderer::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
  //Remove renderer from RenderManager
  if( mRenderer )
  {
    mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
    mRenderer = nullptr;
  }
  mSceneController = nullptr;
  mRenderDataProvider = nullptr;
}

void Renderer::UpdateTextureSet()
{
  if( mRenderDataProvider )
  {
    if( mTextureSet )
    {
      uint32_t textureCount = mTextureSet->GetTextureCount();
      mRenderDataProvider->mTextures.resize( textureCount );
      mRenderDataProvider->mSamplers.resize( textureCount );
      for( uint32_t i = 0; i<textureCount; ++i )
      {
        mRenderDataProvider->mTextures[i] = mTextureSet->GetTexture(i);
        mRenderDataProvider->mSamplers[i] = mTextureSet->GetTextureSampler(i);
      }
    }
    else
    {
      mRenderDataProvider->mTextures.clear();
      mRenderDataProvider->mSamplers.clear();
    }
  }
}

Render::Renderer& Renderer::GetRenderer()
{
  return *mRenderer;
}

const CollectedUniformMap& Renderer::GetUniformMap( BufferIndex bufferIndex ) const
{
  return mCollectedUniformMap[bufferIndex];
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
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  UpdateTextureSet();
}

void Renderer::TextureSetDeleted()
{
  mTextureSet = nullptr;

  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  UpdateTextureSet();
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
    mShader = nullptr;
  }
}

void Renderer::SetDrawCommands( Dali::DevelRenderer::DrawCommand* pDrawCommands, uint32_t size )
{
  mDrawCommands.clear();
  mDrawCommands.insert( mDrawCommands.end(), pDrawCommands, pDrawCommands+size );
  mResendFlag |= RESEND_DRAW_COMMANDS;
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
