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
#include <dali/internal/event/common/system-overlay-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/actor-attachments/camera-attachment-impl.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>

namespace Dali
{

namespace Internal
{

SystemOverlay* SystemOverlay::New( EventThreadServices& eventThreadServices )
{
  SystemOverlay* overlay = new SystemOverlay( eventThreadServices );

  overlay->Initialize();

  return overlay;
}

SystemOverlay::~SystemOverlay()
{
  if ( mRootLayer )
  {
    // we are closing down so just delete the root, no point emit disconnect
    // signals or send messages to update
    mRootLayer.Reset();
  }
}

void SystemOverlay::Add( Actor& actor )
{
  CreateRootLayer();

  mRootLayer->Add( actor );
}

void SystemOverlay::Remove( Actor& actor )
{
  if ( mRootLayer )
  {
    mRootLayer->Remove( actor );
  }
}

RenderTaskList& SystemOverlay::GetOverlayRenderTasks()
{
  if ( !mOverlayRenderTaskList )
  {
    mOverlayRenderTaskList = RenderTaskList::New( mEventThreadServices, *this, true/*system-overlay*/ );
  }

  return *mOverlayRenderTaskList;
}

void SystemOverlay::SetSize( float width, float height )
{
  mSize = Vector2( width, height );

  if ( mRootLayer )
  {
    mRootLayer->SetSize( mSize.width, mSize.height );
  }

  if ( mDefaultCameraActor )
  {
    // Sets the default perspective projection for the given size.
    mDefaultCameraActor->SetPerspectiveProjection( mSize );
  }
}

LayerList& SystemOverlay::GetLayerList()
{
  return *mLayerList;
}

Actor& SystemOverlay::GetDefaultRootActor()
{
  CreateRootLayer();

  return *mRootLayer;
}

CameraActor& SystemOverlay::GetDefaultCameraActor()
{
  CreateDefaultCameraActor();

  return *mDefaultCameraActor;
}

SystemOverlay::SystemOverlay( EventThreadServices& eventThreadServices )
: mEventThreadServices( eventThreadServices )
{
}

void SystemOverlay::Initialize()
{
  // Create the ordered list of layers
  mLayerList = LayerList::New( mEventThreadServices.GetUpdateManager(), true/*system layers*/ );
}

void SystemOverlay::CreateRootLayer()
{
  // Lazy initialization; SystemOverlay may never be used
  if ( !mRootLayer )
  {
    mRootLayer = Layer::NewRoot( *mLayerList, mEventThreadServices.GetUpdateManager(), true/*system layer*/ );
    mRootLayer->SetName("SystemOverlayRoot");
    mRootLayer->SetSize( mSize.width, mSize.height );
  }
}

void SystemOverlay::CreateDefaultCameraActor()
{
  // Lazy initialization; SystemOverlay may never be used
  if ( !mDefaultCameraActor )
  {
    // Creates a default camera with a default perspective projection.
    mDefaultCameraActor = CameraActor::New( mSize );
    mDefaultCameraActor->SetParentOrigin( ParentOrigin::CENTER );

    Add( *mDefaultCameraActor );
  }
}

} // namespace Internal

} // namespace Dali
