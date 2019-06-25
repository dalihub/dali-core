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

ScenePtr Scene::New( const Size& size )
{
  ScenePtr scene = new Scene( size );

  // Second-phase construction
  scene->Initialize();

  return scene;
}

Scene::Scene( const Size& size )
: mSurface( nullptr ),
  mSize( size ),
  mSurfaceSize( Vector2::ZERO ),
  mDpi( Vector2::ZERO ),
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

  if( ThreadLocalStorage::Created() )
  {
    ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
    tls->RemoveScene( this );
  }
}

void Scene::Initialize()
{
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();

  DALI_ASSERT_ALWAYS( tls && "Attempt to create scene before core exists!" );

  tls->AddScene( this );

  SceneGraph::UpdateManager& updateManager = tls->GetUpdateManager();

  // Create the ordered list of layers
  mLayerList = LayerList::New( updateManager );

  // The scene owns the default layer
  mRootLayer = Layer::NewRoot( *mLayerList, updateManager );
  mRootLayer->SetName("RootLayer");
  mRootLayer->SetScene( *this );

  // The root layer needs to have a fixed resize policy (as opposed to the default USE_NATURAL_SIZE).
  // This stops actors parented to the stage having their relayout requests propagating
  // up to the root layer, and down through other children unnecessarily.
  mRootLayer->SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

  // Create the default camera actor first; this is needed by the RenderTaskList
  // The default camera attributes and position is such that children of the default layer,
  // can be positioned at (0,0) and be at the top-left of the viewport.
  mDefaultCamera = CameraActor::New( mSize );
  mDefaultCamera->SetParentOrigin(ParentOrigin::CENTER);
  Add(*(mDefaultCamera.Get()));

  // Create the list of render-tasks
  mRenderTaskList = RenderTaskList::New();

  // Create the default render-task
  mRenderTaskList->CreateTask( mRootLayer.Get(), mDefaultCamera.Get() );
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
  mSurface = &surface;
  if ( mSurface )
  {
    mSurfaceSize.width = static_cast<float>( mSurface->GetPositionSize().width );
    mSurfaceSize.height = static_cast<float>( mSurface->GetPositionSize().height );

    mSize.width = mSurfaceSize.width;
    mSize.height = mSurfaceSize.height;

    // Calculates the aspect ratio, near and far clipping planes, field of view and camera Z position.
    mDefaultCamera->SetPerspectiveProjection( mSurfaceSize );

    mRootLayer->SetSize( mSize.width, mSize.height );

    ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
    SceneGraph::UpdateManager& updateManager = tls->GetUpdateManager();
    SetDefaultSurfaceRectMessage( updateManager, Rect<int32_t>( 0, 0, static_cast<int32_t>( mSurfaceSize.width ), static_cast<int32_t>( mSurfaceSize.height ) ) ); // truncated

    RenderTaskPtr defaultRenderTask = mRenderTaskList->GetTask( 0u );

    // if single render task to screen then set its viewport parameters
    if( 1 == mRenderTaskList->GetTaskCount() )
    {
      if( !defaultRenderTask->GetTargetFrameBuffer() )
      {
        defaultRenderTask->SetViewport( Viewport( 0, 0, static_cast<int32_t>( mSurfaceSize.width ), static_cast<int32_t>( mSurfaceSize.height ) ) ); // truncated
      }
    }

    mFrameBuffer = Dali::Internal::FrameBuffer::New( surface, Dali::FrameBuffer::Attachment::NONE );
    defaultRenderTask->SetFrameBuffer( mFrameBuffer );
  }
}

Integration::RenderSurface* Scene::GetSurface() const
{
  return mSurface;
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

void Scene::SetBackgroundColor(Vector4 color)
{
  if( mSurface )
  {
    mSurface->SetBackgroundColor( color );
  }
}

Vector4 Scene::GetBackgroundColor() const
{
  return mSurface ? mSurface->GetBackgroundColor() : DEFAULT_BACKGROUND_COLOR;
}

void Scene::EmitKeyEventSignal(const KeyEvent& event)
{
  mKeyEventSignal.Emit( event );
}

void Scene::EmitEventProcessingFinishedSignal()
{
  mEventProcessingFinishedSignal.Emit();
}

void Scene::EmitTouchedSignal( const TouchEvent& touchEvent, const Dali::TouchData& touch )
{
  mTouchedSignal.Emit( touchEvent );
  mTouchSignal.Emit( touch );
}

void Scene::EmitWheelEventSignal(const WheelEvent& event)
{
  mWheelEventSignal.Emit( event );
}

Integration::Scene::KeyEventSignalType& Scene::KeyEventSignal()
{
  return mKeyEventSignal;
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
