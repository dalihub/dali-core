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
 *
 */

// CLASS HEADER
#include <dali/internal/event/common/scene-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/internal/event/rendering/frame-buffer-impl.h>
#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>

using Dali::Internal::SceneGraph::Node;

namespace Dali
{

namespace Internal
{

namespace
{

const Vector4 DEFAULT_BACKGROUND_COLOR(0.0f, 0.0f, 0.0f, 1.0f); // Default background color

} //Unnamed namespace

ScenePtr Scene::New( Integration::RenderSurface& surface )
{
  ScenePtr scene = new Scene;

  // Second-phase construction
  scene->Initialize( surface );

  return scene;
}

Scene::Scene()
: mSurface( nullptr ),
  mSize(), // Don't set the proper value here, this will be set when the surface is set later
  mDpi(),
  mBackgroundColor( DEFAULT_BACKGROUND_COLOR ),
  mDepthTreeDirty( false ),
  mEventProcessor( *this, ThreadLocalStorage::GetInternal()->GetGestureEventProcessor() )
{
}

Scene::~Scene()
{
  if( mDefaultCamera )
  {
    // its enough to release the handle so the object is released
    // don't need to remove it from root actor as root actor will delete the object
    mDefaultCamera.Reset();
  }

  if( mRootLayer )
  {
    // we are closing down so just delete the root, no point emit disconnect
    // signals or send messages to update
    mRootLayer.Reset();
  }

  if( mRenderTaskList )
  {
    mRenderTaskList.Reset();
  }

  if ( mFrameBuffer )
  {
    mFrameBuffer.Reset();
  }

  // No need to discard this Scene from Core, as Core stores an intrusive_ptr to this scene
  // When this destructor is called, the scene has either already been removed from Core or Core has already been destroyed
}

void Scene::Initialize( Integration::RenderSurface& surface )
{
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();

  DALI_ASSERT_ALWAYS( tls && "Attempt to create scene before core exists!" );

  tls->AddScene( this );

  SceneGraph::UpdateManager& updateManager = tls->GetUpdateManager();

  // Create the ordered list of layers
  mLayerList = LayerList::New( updateManager );

  // The scene owns the default layer
  mRootLayer = Layer::NewRoot( *mLayerList );
  mRootLayer->SetName("RootLayer");
  mRootLayer->SetScene( *this );

  // The root layer needs to have a fixed resize policy (as opposed to the default USE_NATURAL_SIZE).
  // This stops actors parented to the stage having their relayout requests propagating
  // up to the root layer, and down through other children unnecessarily.
  mRootLayer->SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

  // Create the default camera actor first; this is needed by the RenderTaskList
  // The default camera attributes and position is such that children of the default layer,
  // can be positioned at (0,0) and be at the top-left of the viewport.
  const PositionSize positionSize = surface.GetPositionSize();
  const Vector2 surfaceSize( static_cast< float >( positionSize.width ), static_cast< float >( positionSize.height ) );
  mDefaultCamera = CameraActor::New( surfaceSize );
  mDefaultCamera->SetParentOrigin(ParentOrigin::CENTER);
  Add(*(mDefaultCamera.Get()));

  // Create the list of render-tasks
  mRenderTaskList = RenderTaskList::New();

  // Create the default render-task
  mRenderTaskList->CreateTask( mRootLayer.Get(), mDefaultCamera.Get() );

  // Set the surface
  SetSurface( surface );
}

void Scene::Add(Actor& actor)
{
  mRootLayer->Add( actor );
}

void Scene::Remove(Actor& actor)
{
  mRootLayer->Remove( actor );
}

Size Scene::GetSize() const
{
  return mSize;
}

void Scene::SetDpi(Vector2 dpi)
{
  mDpi = dpi;
}

Vector2 Scene::GetDpi() const
{
  return mDpi;
}

RenderTaskList& Scene::GetRenderTaskList() const
{
  return *mRenderTaskList;
}

Dali::Layer Scene::GetRootLayer() const
{
  return Dali::Layer( mRootLayer.Get() );
}

LayerList& Scene::GetLayerList() const
{
  return *mLayerList;
}

uint32_t Scene::GetLayerCount() const
{
  return mLayerList->GetLayerCount();
}

Dali::Layer Scene::GetLayer( uint32_t depth ) const
{
  return Dali::Layer(mLayerList->GetLayer( depth ));
}

CameraActor& Scene::GetDefaultCameraActor()
{
  return *mDefaultCamera;
}

Actor& Scene::GetDefaultRootActor()
{
  return *mRootLayer;
}

void Scene::SetSurface( Integration::RenderSurface& surface )
{
  if( mSurface != &surface )
  {
    mSurface = &surface;

    RenderTaskPtr defaultRenderTask = mRenderTaskList->GetTask( 0u );

    mFrameBuffer = Dali::Internal::FrameBuffer::New( surface, Dali::FrameBuffer::Attachment::NONE );
    defaultRenderTask->SetFrameBuffer( mFrameBuffer );

    SurfaceResized();
  }
}

void Scene::SurfaceResized()
{
  if( mSurface )
  {
    const PositionSize surfacePositionSize = mSurface->GetPositionSize();
    const float fWidth = static_cast< float >( surfacePositionSize.width );
    const float fHeight = static_cast< float >( surfacePositionSize.height );

    if( ( fabsf( mSize.width - fWidth ) > Math::MACHINE_EPSILON_1 ) || ( fabsf( mSize.height - fHeight ) > Math::MACHINE_EPSILON_1 ) )
    {
      Rect< int32_t > newSize( 0, 0, static_cast< int32_t >( surfacePositionSize.width ), static_cast< int32_t >( surfacePositionSize.height ) ); // truncated

      mSize.width = fWidth;
      mSize.height = fHeight;

      // Calculates the aspect ratio, near and far clipping planes, field of view and camera Z position.
      mDefaultCamera->SetPerspectiveProjection( mSize );

      mRootLayer->SetSize( mSize.width, mSize.height );

      ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
      SceneGraph::UpdateManager& updateManager = tls->GetUpdateManager();
      SetDefaultSurfaceRectMessage( updateManager, newSize );

      // set default render-task viewport parameters
      RenderTaskPtr defaultRenderTask = mRenderTaskList->GetTask( 0u );
      defaultRenderTask->SetViewport( newSize );
      defaultRenderTask->GetFrameBuffer()->SetSize( static_cast<uint32_t>( newSize.width ), static_cast<uint32_t>( newSize.height ) );
    }
  }
}

void Scene::SurfaceDeleted()
{
  mSurface = nullptr;
  if ( mFrameBuffer )
  {
    // The frame buffer doesn't have a valid render surface any more.
    mFrameBuffer->MarkSurfaceAsInvalid();
  }
}

Integration::RenderSurface* Scene::GetSurface() const
{
  return mSurface;
}

void Scene::Discard()
{
  if( ThreadLocalStorage::Created() )
  {
    ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
    tls->RemoveScene( this );
  }
}

void Scene::RequestRebuildDepthTree()
{
  mDepthTreeDirty = true;
}

void Scene::QueueEvent( const Integration::Event& event )
{
  mEventProcessor.QueueEvent( event );
}

void Scene::ProcessEvents()
{
  mEventProcessor.ProcessEvents();
}

void Scene::RebuildDepthTree()
{
  // If the depth tree needs rebuilding, do it in this frame only.
  if( mDepthTreeDirty )
  {
    ActorPtr actor( mRootLayer.Get() );
    actor->RebuildDepthTree();
    mDepthTreeDirty = false;
  }
}

void Scene::SetBackgroundColor( const Vector4& color )
{
  mBackgroundColor = color;

  if( mSurface )
  {
    mRenderTaskList->GetTask( 0u )->GetFrameBuffer()->SetBackgroundColor( color );
  }
}

Vector4 Scene::GetBackgroundColor() const
{
  return mBackgroundColor;
}

void Scene::EmitKeyEventSignal(const KeyEvent& event)
{
  if ( !mKeyEventSignal.Empty() )
  {
    Dali::Integration::Scene handle( this );
    mKeyEventSignal.Emit( event );
  }
}

bool Scene::EmitKeyEventGeneratedSignal(const KeyEvent& event)
{
  // Emit the KeyEventGenerated signal when KeyEvent is generated
  return mKeyEventGeneratedSignal.Emit( event );
}

void Scene::EmitEventProcessingFinishedSignal()
{
  if ( !mEventProcessingFinishedSignal.Empty() )
  {
    Dali::Integration::Scene handle( this );
    mEventProcessingFinishedSignal.Emit();
  }
}

void Scene::EmitTouchedSignal( const TouchEvent& touchEvent, const Dali::TouchData& touch )
{
  Dali::Integration::Scene handle( this );
  if ( !mTouchedSignal.Empty() )
  {
    mTouchedSignal.Emit( touchEvent );
  }
  if ( !mTouchSignal.Empty() )
  {
    mTouchSignal.Emit( touch );
  }
}

void Scene::EmitWheelEventSignal(const WheelEvent& event)
{
  if ( !mWheelEventSignal.Empty() )
  {
    Dali::Integration::Scene handle( this );
    mWheelEventSignal.Emit( event );
  }
}

Integration::Scene::KeyEventSignalType& Scene::KeyEventSignal()
{
  return mKeyEventSignal;
}

Integration::Scene::KeyEventGeneratedSignalType& Scene::KeyEventGeneratedSignal()
{
  return mKeyEventGeneratedSignal;
}

Integration::Scene::EventProcessingFinishedSignalType& Scene::EventProcessingFinishedSignal()
{
  return mEventProcessingFinishedSignal;
}

Scene::TouchedSignalType& Scene::TouchedSignal()
{
  return mTouchedSignal;
}

Integration::Scene::TouchSignalType& Scene::TouchSignal()
{
  return mTouchSignal;
}

Integration::Scene::WheelEventSignalType& Scene::WheelEventSignal()
{
  return mWheelEventSignal;
}

} // Internal

} // Dali
