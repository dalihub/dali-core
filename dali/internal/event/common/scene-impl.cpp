/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/rendering/frame-buffer-impl.h>
#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>
#include <dali/internal/update/common/scene-graph-scene.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/render-tasks/render-task-list.h>

using Dali::Internal::SceneGraph::Node;

namespace Dali
{
namespace Internal
{
ScenePtr Scene::New(Size size, int32_t windowOrientation, int32_t screenOrientation)
{
  ScenePtr scene = new Scene;

  // Second-phase construction
  scene->Initialize(size, windowOrientation, screenOrientation);

  return scene;
}

Scene::Scene()
: mSceneObject(nullptr),
  mSize(), // Don't set the proper value here, this will be set when the surface is set later
  mDpi(),
  mBackgroundColor(DEFAULT_BACKGROUND_COLOR),
  mDepthTreeDirty(false),
  mPartialUpdateEnabled(true),
  mGeometryHittest(false),
  mIsVisible(true),
  mEventProcessor(*this, ThreadLocalStorage::GetInternal()->GetGestureEventProcessor()),
  mSurfaceOrientation(0),
  mScreenOrientation(0),
  mNativeId(0),
  mPanGestureState(GestureState::CLEAR)
{
}

Scene::~Scene()
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("~Scene[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }

  if(mDefaultCamera)
  {
    // its enough to release the handle so the object is released
    // don't need to remove it from root actor as root actor will delete the object
    mDefaultCamera.Reset();
  }

  if(mRootLayer)
  {
    // we are closing down so just delete the root, no point emit disconnect
    // signals or send messages to update
    mRootLayer.Reset();
  }

  if(mOverlayLayer)
  {
    mOverlayLayer.Reset();
  }

  if(mRenderTaskList)
  {
    mRenderTaskList.Reset();
  }

  // No need to discard this Scene from Core, as Core stores an intrusive_ptr to this scene
  // When this destructor is called, the scene has either already been removed from Core or Core has already been destroyed
}

void Scene::Initialize(Size size, int32_t windowOrientation, int32_t screenOrientation)
{
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();

  DALI_ASSERT_ALWAYS(tls && "Attempt to create scene before core exists!");

  tls->AddScene(this);

  SceneGraph::UpdateManager& updateManager = tls->GetUpdateManager();

  // Create the ordered list of layers
  mLayerList = LayerList::New(updateManager);

  // The scene owns the default layer
  mRootLayer = Layer::NewRoot(*mLayerList);
  mRootLayer->SetName("RootLayer");
  mRootLayer->SetScene(*this);

  // The root layer needs to have a fixed resize policy (as opposed to the default USE_NATURAL_SIZE).
  // This stops actors parented to the stage having their relayout requests propagating
  // up to the root layer, and down through other children unnecessarily.
  mRootLayer->SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);

  // Create the default camera actor first; this is needed by the RenderTaskList
  // The default camera attributes and position is such that children of the default layer,
  // can be positioned at (0,0) and be at the top-left of the viewport.
  mDefaultCamera = CameraActor::New(size);
  mDefaultCamera->SetParentOrigin(ParentOrigin::CENTER);
  Add(*(mDefaultCamera.Get()));

  // Create the list of render-tasks
  mRenderTaskList = RenderTaskList::New();

  // Create the default render-task and ensure clear is enabled on it to show the background color
  RenderTaskPtr renderTask = mRenderTaskList->CreateTask(mRootLayer.Get(), mDefaultCamera.Get());
  renderTask->SetOrderIndex(INT32_MIN);
  renderTask->SetClearEnabled(true);

  // Create scene graph object
  mSceneObject = new SceneGraph::Scene();
  OwnerPointer<SceneGraph::Scene> transferOwnership(const_cast<SceneGraph::Scene*>(mSceneObject));
  AddSceneMessage(updateManager, transferOwnership);

  SurfaceRotated(size.width, size.height, windowOrientation, screenOrientation);
}

void Scene::Add(Actor& actor)
{
  mRootLayer->Add(actor);
}

void Scene::Remove(Actor& actor)
{
  mRootLayer->Remove(actor);
}

void Scene::Show()
{
  if(!mIsVisible)
  {
    mIsVisible = true;
    mRootLayer->EmitInheritedVisibilityChangedSignalRecursively(true);
  }
}

void Scene::Hide()
{
  if(mIsVisible)
  {
    mIsVisible = false;
    mRootLayer->EmitInheritedVisibilityChangedSignalRecursively(false);
  }
}

bool Scene::IsVisible() const
{
  return mIsVisible;
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
  return Dali::Layer(mRootLayer.Get());
}

Dali::Layer Scene::GetOverlayLayer()
{
  if(!mOverlayLayer)
  {
    // Creates overlay layer.
    mOverlayLayer = Layer::New();
    mOverlayLayer->SetName("OverlayLayer");
    mOverlayLayer->SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    mOverlayLayer->SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
    mOverlayLayer->SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
    mRootLayer->Add(*mOverlayLayer);

    // Create the overlay render-task and set exclusive to true.
    RenderTaskPtr renderTask = mRenderTaskList->CreateOverlayTask(mOverlayLayer.Get(), mDefaultCamera.Get());
    renderTask->SetExclusive(true);
    renderTask->SetInputEnabled(true);
  }
  return Dali::Layer(mOverlayLayer.Get());
}

LayerList& Scene::GetLayerList() const
{
  return *mLayerList;
}

uint32_t Scene::GetLayerCount() const
{
  return mLayerList->GetLayerCount();
}

Dali::Layer Scene::GetLayer(uint32_t depth) const
{
  return Dali::Layer(mLayerList->GetLayer(depth));
}

CameraActor& Scene::GetDefaultCameraActor() const
{
  return *mDefaultCamera;
}

Actor& Scene::GetDefaultRootActor()
{
  return *mRootLayer;
}

void Scene::SurfaceResized(float width, float height)
{
  if((fabsf(mSize.width - width) > Math::MACHINE_EPSILON_1) || (fabsf(mSize.height - height) > Math::MACHINE_EPSILON_1))
  {
    ChangedSurface(width, height, mSurfaceOrientation, mScreenOrientation);
  }
}

void Scene::SurfaceReplaced()
{
  if(mSceneObject)
  {
    ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
    SurfaceReplacedMessage(tls->GetUpdateManager(), *mSceneObject);
  }
}

void Scene::RemoveSceneObject()
{
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mSceneObject))
  {
    ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
    RemoveSceneMessage(tls->GetUpdateManager(), *mSceneObject);
    mSceneObject = nullptr;
  }
  else if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("Scene[%p] called RemoveSceneObject API from non-UI thread!\n", this);
  }
}

void Scene::Discard()
{
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning()))
  {
    ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
    tls->RemoveScene(this);
  }
  else if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("Scene[%p] called Discard API from non-UI thread!\n", this);
  }
}

void Scene::RequestRebuildDepthTree()
{
  mDepthTreeDirty = true;
}

void Scene::QueueEvent(const Integration::Event& event)
{
  mEventProcessor.QueueEvent(event);
}

void Scene::ProcessEvents()
{
  mEventProcessor.ProcessEvents();
}

void Scene::SendInterruptedEvents(Dali::Internal::Actor* actor)
{
  mEventProcessor.SendInterruptedEvents(actor);
}

void Scene::RebuildDepthTree()
{
  // If the depth tree needs rebuilding, do it in this frame only.
  if(mDepthTreeDirty)
  {
    ActorPtr actor(mRootLayer.Get());
    actor->RebuildDepthTree();
    mDepthTreeDirty = false;
  }
  mRenderTaskList->SortTasks();
}

void Scene::SetBackgroundColor(const Vector4& color)
{
  mBackgroundColor = color;

  mRenderTaskList->GetTask(0u)->SetClearColor(color);
  mRenderTaskList->GetTask(0u)->SetClearEnabled(true);
}

Vector4 Scene::GetBackgroundColor() const
{
  return mBackgroundColor;
}

SceneGraph::Scene* Scene::GetSceneObject() const
{
  return mSceneObject;
}

void Scene::EmitKeyEventSignal(const Dali::KeyEvent& event)
{
  if(!mKeyEventSignal.Empty())
  {
    Dali::Integration::Scene handle(this);
    mKeyEventSignal.Emit(event);
  }
}

void Scene::SurfaceRotated(float width, float height, int32_t windowOrientation, int32_t screenOrientation)
{
  ChangedSurface(width, height, windowOrientation, screenOrientation);
}

int32_t Scene::GetCurrentSurfaceOrientation() const
{
  return mSceneObject ? mSceneObject->GetSurfaceOrientation() : 0;
}

int32_t Scene::GetCurrentScreenOrientation() const
{
  return mSceneObject ? mSceneObject->GetScreenOrientation() : 0;
}

const Rect<int32_t>& Scene::GetCurrentSurfaceRect() const
{
  static Rect<int32_t> emptyRect{};
  return mSceneObject ? mSceneObject->GetSurfaceRect() : emptyRect;
}

void Scene::ChangedSurface(float width, float height, int32_t windowOrientation, int32_t screenOrientation)
{
  bool          changedOrientation = false;
  Rect<int32_t> newSize(0, 0, static_cast<int32_t>(width), static_cast<int32_t>(height)); // truncated
  mSize.width  = width;
  mSize.height = height;

  if(mSurfaceOrientation != windowOrientation || mScreenOrientation != screenOrientation)
  {
    changedOrientation = true;
  }

  mSurfaceOrientation = windowOrientation;
  mScreenOrientation  = screenOrientation;

  // Calculates the aspect ratio, near and far clipping planes, field of view and camera Z position.
  mDefaultCamera->SetPerspectiveProjection(mSize);
  // Set the surface orientation to Default camera for window/screen rotation
  if(changedOrientation)
  {
    int32_t orientation = (windowOrientation + screenOrientation) % 360;
    mDefaultCamera->RotateProjection(orientation);
  }

  mRootLayer->SetSize(width, height);

  // Send the surface rectangle/orientation to SceneGraph::Scene for calculating glViewport/Scissor
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
  DALI_LOG_RELEASE_INFO("Send Surface Rect Message, width[%d], height[%d]\n", newSize.width, newSize.height);
  SetSurfaceRectMessage(tls->GetEventThreadServices(), *mSceneObject, newSize);
  if(changedOrientation)
  {
    DALI_LOG_RELEASE_INFO("Send Surface Orientation Message, surface orientation[%d], screen orientation[%d]\n", mSurfaceOrientation, mScreenOrientation);
    SetSurfaceOrientationsMessage(tls->GetEventThreadServices(), *mSceneObject, mSurfaceOrientation, mScreenOrientation);
  }

  // set default render-task viewport parameters
  RenderTaskPtr defaultRenderTask = mRenderTaskList->GetTask(0u);
  defaultRenderTask->SetViewport(newSize);
  // set overlay render-task viewport parameters
  RenderTaskPtr overlayRenderTask = mRenderTaskList->GetOverlayTask();
  if(overlayRenderTask)
  {
    overlayRenderTask->SetViewport(newSize);
  }
}

uint32_t Scene::GetSurfaceRectChangedCount() const
{
  return mSceneObject ? mSceneObject->GetSurfaceRectChangedCount() : 0u;
}

bool Scene::IsRotationCompletedAcknowledgementSet() const
{
  return mSceneObject ? mSceneObject->IsRotationCompletedAcknowledgementSet() : false;
}

void Scene::SetRotationCompletedAcknowledgement()
{
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
  SetRotationCompletedAcknowledgementMessage(tls->GetEventThreadServices(), *mSceneObject);
}

void Scene::SetSurfaceRenderTarget(const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo)
{
  // Send the surface render target to SceneGraph::Scene
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
  SetSurfaceRenderTargetCreateInfoMessage(tls->GetEventThreadServices(), *mSceneObject, renderTargetCreateInfo);
}

bool Scene::EmitKeyEventGeneratedSignal(const Dali::KeyEvent& event)
{
  // Emit the KeyEventGenerated signal when KeyEvent is generated
  Dali::Integration::Scene handle(this);
  return mKeyEventGeneratedSignal.Emit(event);
}

bool Scene::EmitInterceptKeyEventSignal(const Dali::KeyEvent& event)
{
  // Emit the InterceptKeyEvent signal
  Dali::Integration::Scene handle(this);
  return mInterceptKeyEventSignal.Emit(event);
}

void Scene::EmitEventProcessingFinishedSignal()
{
  if(!mEventProcessingFinishedSignal.Empty())
  {
    Dali::Integration::Scene handle(this);
    mEventProcessingFinishedSignal.Emit();
  }
}

void Scene::EmitTouchedSignal(const Dali::TouchEvent& touch)
{
  Dali::Integration::Scene handle(this);
  if(!mTouchedSignal.Empty())
  {
    mTouchedSignal.Emit(touch);
  }
}

void Scene::EmitWheelEventSignal(const Dali::WheelEvent& event)
{
  if(!mWheelEventSignal.Empty())
  {
    Dali::Integration::Scene handle(this);
    mWheelEventSignal.Emit(event);
  }
}

bool Scene::EmitWheelEventGeneratedSignal(const Dali::WheelEvent& event)
{
  // Emit the WheelEventGenerated signal when WheelEvent is generated
  Dali::Integration::Scene handle(this);
  return mWheelEventGeneratedSignal.Emit(event);
}

void Scene::EmitKeyEventMonitorSignal(const Dali::KeyEvent& event)
{
  if(!mKeyEventMonitorSignal.Empty())
  {
    Dali::Integration::Scene handle(this);
    mKeyEventMonitorSignal.Emit(event);
  }
}

void Scene::AddFrameRenderedCallback(std::unique_ptr<CallbackBase> callback, int32_t frameId)
{
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
  AddFrameRenderedCallbackMessage(tls->GetEventThreadServices(), *mSceneObject, callback.release(), frameId);
}

void Scene::AddFramePresentedCallback(std::unique_ptr<CallbackBase> callback, int32_t frameId)
{
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
  AddFramePresentedCallbackMessage(tls->GetEventThreadServices(), *mSceneObject, callback.release(), frameId);
}

void Scene::GetFrameRenderedCallback(Dali::Integration::Scene::FrameCallbackContainer& callbacks)
{
  if(mSceneObject)
  {
    mSceneObject->GetFrameRenderedCallback(callbacks);
  }
}

void Scene::GetFramePresentedCallback(Dali::Integration::Scene::FrameCallbackContainer& callbacks)
{
  if(mSceneObject)
  {
    mSceneObject->GetFramePresentedCallback(callbacks);
  }
}

void Scene::KeepRendering(float durationSeconds)
{
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
  KeepRenderingMessage(tls->GetEventThreadServices(), *mSceneObject, durationSeconds);
}

void Scene::SetPartialUpdateEnabled(bool enabled)
{
  mPartialUpdateEnabled = enabled;

  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
  SetPartialUpdateEnabledMessage(tls->GetEventThreadServices(), *mSceneObject, enabled);
}

bool Scene::IsPartialUpdateEnabled() const
{
  return mPartialUpdateEnabled;
}

void Scene::SetGeometryHittestEnabled(bool enabled)
{
  mGeometryHittest = enabled;
}

bool Scene::IsGeometryHittestEnabled() const
{
  return mGeometryHittest;
}

Dali::Integration::Scene::TouchPropagationType Scene::GetTouchPropagationType() const
{
  return mGeometryHittest ? Dali::Integration::Scene::TouchPropagationType::GEOMETRY : Dali::Integration::Scene::TouchPropagationType::PARENT;
}

void Scene::SetNativeId(int32_t nativeId)
{
  mNativeId = nativeId;
}

int32_t Scene::GetNativeId() const
{
  return mNativeId;
}

void Scene::SetLastPanGestureState(Dali::GestureState state)
{
  mPanGestureState = state;
}

Dali::GestureState Scene::GetLastPanGestureState()
{
  return mPanGestureState;
}

Integration::Scene::KeyEventSignalType& Scene::KeyEventSignal()
{
  return mKeyEventSignal;
}

Integration::Scene::KeyEventGeneratedSignalType& Scene::KeyEventGeneratedSignal()
{
  return mKeyEventGeneratedSignal;
}

Integration::Scene::KeyEventGeneratedSignalType& Scene::InterceptKeyEventSignal()
{
  return mInterceptKeyEventSignal;
}

Integration::Scene::KeyEventSignalType& Scene::KeyEventMonitorSignal()
{
  return mKeyEventMonitorSignal;
}

Integration::Scene::EventProcessingFinishedSignalType& Scene::EventProcessingFinishedSignal()
{
  return mEventProcessingFinishedSignal;
}

Integration::Scene::TouchEventSignalType& Scene::TouchedSignal()
{
  return mTouchedSignal;
}

Integration::Scene::WheelEventSignalType& Scene::WheelEventSignal()
{
  return mWheelEventSignal;
}

Integration::Scene::WheelEventGeneratedSignalType& Scene::WheelEventGeneratedSignal()
{
  return mWheelEventGeneratedSignal;
}

} // namespace Internal

} // namespace Dali
