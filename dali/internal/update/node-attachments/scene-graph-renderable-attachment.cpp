/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>

// INTERNAL INCLUDES
#include <dali/integration-api/resource-declarations.h>
#include <dali/public-api/actors/renderable-actor.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/resources/complete-status-manager.h>
#include <dali/internal/update/resources/resource-tracker.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>
#include <dali/internal/render/shaders/shader.h>
#include <dali/internal/common/image-sampler.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

void RenderableAttachment::SetSortModifier(float modifier)
{
  // Setting sort modifier makes the node dirty, i.e. we cannot reuse previous frames render items
  if( mParent )
  {
    // only do this if we are on-stage
    mParent->SetDirtyFlag( SortModifierFlag );
  }
  mSortModifier = modifier;
}

void RenderableAttachment::SetBlendingMode( BlendingMode::Type mode )
{
  mBlendingMode = mode;
}

BlendingMode::Type RenderableAttachment::GetBlendingMode() const
{
  return mBlendingMode;
}

void RenderableAttachment::ChangeBlending( BufferIndex updateBufferIndex, bool useBlend )
{
  if ( mUseBlend != useBlend )
  {
    mUseBlend = useBlend;

    // Enable/disable blending in the next render
    typedef MessageValue1< Renderer, bool > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &GetRenderer(), &Renderer::SetUseBlend, useBlend );
  }
}

void RenderableAttachment::SetBlendingOptions( BufferIndex updateBufferIndex, unsigned int options )
{
  // Blending options are forwarded to renderer in render-thread
  typedef MessageValue1< Renderer, unsigned int > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &GetRenderer(), &Renderer::SetBlendingOptions, options );
}

void RenderableAttachment::SetBlendColor( BufferIndex updateBufferIndex, const Vector4& color )
{
  // Blend color is forwarded to renderer in render-thread
  typedef MessageValue1< Renderer, Vector4 > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &GetRenderer(), &Renderer::SetBlendColor, color );
}

void RenderableAttachment::PrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager )
{
  mHasUntrackedResources = false; // Only need to know this if the resources are not yet complete
  mTrackedResources.Clear(); // Resource trackers are only needed if not yet completea

  if( mShader )
  {
    Integration::ResourceId id = mShader->GetEffectTextureResourceId();

    if( id != 0 )
    {
      CompleteStatusManager& completeStatusManager = mSceneController->GetCompleteStatusManager();

      if(CompleteStatusManager::COMPLETE != completeStatusManager.GetStatus( id ))
      {
        FollowTracker(id);
        mFinishedResourceAcquisition = false;
        mResourcesReady = false;

        // If shader has effect texture and it's not complete and tracked, ensure
        // we call DoPrepareResources, as the effect texture may become ready in
        // the ProcessRenderTasks step.  Otherwise, may early out.
        if( mHasUntrackedResources )
        {
          return;
        }
      }
    }
  }

  mResourcesReady = DoPrepareResources( updateBufferIndex, resourceManager );
}

void RenderableAttachment::FollowTracker( Integration::ResourceId id )
{
  CompleteStatusManager& completeStatusManager = mSceneController->GetCompleteStatusManager();

  if( completeStatusManager.FindResourceTracker(id) != NULL )
  {
    bool found = false;
    std::size_t numTrackedResources = mTrackedResources.Count();
    for( size_t i=0; i < numTrackedResources; ++i )
    {
      if(mTrackedResources[i] == id)
      {
        found = true;
        break;
      }
    }
    if( ! found )
    {
      mTrackedResources.PushBack( id );
    }
  }
  else
  {
    mHasUntrackedResources = true;
  }
}

void RenderableAttachment::SetCullFace( BufferIndex updateBufferIndex, CullFaceMode mode )
{
  DALI_ASSERT_DEBUG(mSceneController);
  DALI_ASSERT_DEBUG(mode >= CullNone && mode <= CullFrontAndBack);

  mCullFaceMode = mode;

  typedef MessageValue1< Renderer, CullFaceMode > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &GetRenderer(), &Renderer::SetCullFace, mode );
}

void RenderableAttachment::SetSampler( BufferIndex updateBufferIndex, unsigned int samplerBitfield )
{
  DALI_ASSERT_DEBUG(mSceneController);

  typedef MessageValue1< Renderer, unsigned int > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &GetRenderer(), &Renderer::SetSampler, samplerBitfield );
}

void RenderableAttachment::SetRecalculateScaleForSize()
{
  mScaleForSizeDirty = true;
}

void RenderableAttachment::GetScaleForSize( const Vector3& nodeSize, Vector3& scaling )
{
  DoGetScaleForSize( nodeSize, scaling );
  mScaleForSizeDirty = false;
}

void RenderableAttachment::ApplyShader( BufferIndex updateBufferIndex, Shader* shader )
{
  mShader = shader;

  // send the message to renderer
  SendShaderChangeMessage( updateBufferIndex );

  // tell derived class to do something
  ShaderChanged( updateBufferIndex );
}

void RenderableAttachment::RemoveShader( BufferIndex updateBufferIndex )
{
  // return to default shader
  mShader = NULL;

  // send the message to renderer
  SendShaderChangeMessage( updateBufferIndex );

  // tell derived class to do something
  ShaderChanged( updateBufferIndex );
}

bool RenderableAttachment::ResolveVisibility( BufferIndex updateBufferIndex )
{
  mHasSizeAndColorFlag = false;
  const Vector4& color = mParent->GetWorldColor( updateBufferIndex );
  if( color.a > FULLY_TRANSPARENT )               // not fully transparent
  {
    const float MAX_NODE_SIZE = float(1u<<30);
    const Vector3& size = mParent->GetSize( updateBufferIndex );
    if( ( size.width > Math::MACHINE_EPSILON_1000 ) &&    // width is greater than a very small number
        ( size.height > Math::MACHINE_EPSILON_1000 ) &&   // height is greater than a very small number
        ( size.width < MAX_NODE_SIZE ) &&                 // width is smaller than the maximum allowed size
        ( size.height < MAX_NODE_SIZE ) )                 // height is smaller than the maximum allowed size
    {
      mHasSizeAndColorFlag = true;
    }
    else
    {
      DALI_LOG_ERROR("Actor size should be bigger than 0 but not bigger than %f.\n", MAX_NODE_SIZE );
      DALI_LOG_ACTOR_TREE( mParent );
    }
  }
  return mHasSizeAndColorFlag;
}

void RenderableAttachment::DoGetScaleForSize( const Vector3& nodeSize, Vector3& scaling )
{
  scaling = Vector3::ONE;
}

void RenderableAttachment::GetReadyAndComplete(bool& ready, bool& complete) const
{
  ready = mResourcesReady;
  complete = false;

  CompleteStatusManager& completeStatusManager = mSceneController->GetCompleteStatusManager();

  std::size_t numTrackedResources = mTrackedResources.Count();
  if( mHasUntrackedResources || numTrackedResources == 0 )
  {
    complete = mFinishedResourceAcquisition;
  }
  else
  {
    // If there are tracked resources and no untracked resources, test the trackers
    bool trackersComplete = true;
    for( size_t i=0; i < numTrackedResources; ++i )
    {
      ResourceId id = mTrackedResources[i];
      ResourceTracker* tracker = completeStatusManager.FindResourceTracker(id);
      if( tracker  && ! tracker->IsComplete() )
      {
        trackersComplete = false;
        break;
      }
    }

    complete = mFinishedResourceAcquisition || trackersComplete;
  }
}

bool RenderableAttachment::IsBlendingOn( BufferIndex updateBufferIndex )
{
  // Check whether blending needs to be disabled / enabled
  bool blend = false;
  switch( mBlendingMode )
  {
    case BlendingMode::OFF:
    {
      // No blending.
      blend = false;
      break;
    }
    case BlendingMode::AUTO:
    {
      // Blending if the node is not fully opaque only.
      blend = !IsFullyOpaque( updateBufferIndex );
      break;
    }
    case BlendingMode::ON:
    {
      // Blending always.
      blend = true;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"RenderableAttachment::PrepareRender. Wrong blending mode" );
    }
  }
  return blend;
}

void RenderableAttachment::PrepareRender( BufferIndex updateBufferIndex )
{
  // call the derived class first as it might change its state regarding blending
  DoPrepareRender( updateBufferIndex );

  bool blend = IsBlendingOn( updateBufferIndex );
  ChangeBlending( updateBufferIndex, blend );
}

RenderableAttachment::RenderableAttachment( bool usesGeometryScaling )
: mSceneController(NULL),
  mShader( NULL ),
  mTrackedResources(),
  mSortModifier( 0.0f ),
  mBlendingMode( Dali::RenderableActor::DEFAULT_BLENDING_MODE ),
  mUsesGeometryScaling( usesGeometryScaling ),
  mScaleForSizeDirty( true ),
  mUseBlend( false ),
  mHasSizeAndColorFlag( false ),
  mResourcesReady( false ),
  mFinishedResourceAcquisition( false ),
  mHasUntrackedResources( false ),
  mCullFaceMode( CullNone )
{
}

RenderableAttachment::~RenderableAttachment()
{
}

void RenderableAttachment::ConnectToSceneGraph( SceneController& sceneController, BufferIndex updateBufferIndex )
{
  mSceneController = &sceneController;

  // Chain to derived attachments
  ConnectToSceneGraph2( updateBufferIndex );

  // After derived classes have (potentially) created their renderer
  Renderer& renderer = GetRenderer();
  renderer.SetCullFace( mCullFaceMode );

  // set the default shader here as well
  renderer.SetShader( mShader );
}

void RenderableAttachment::OnDestroy()
{
  // Chain to derived attachments
  OnDestroy2();

  // SceneController is no longer valid
  mSceneController = NULL;
}

RenderableAttachment* RenderableAttachment::GetRenderable()
{
  return this;
}

void RenderableAttachment::SendShaderChangeMessage( BufferIndex updateBufferIndex )
{
  typedef MessageValue1< Renderer, Shader* > DerivedType;
  // Reserve memory inside the render queue
  unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
  // Construct message in the mRenderer queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &GetRenderer(), &Renderer::SetShader, mShader );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
