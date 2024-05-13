/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/render-tasks/render-task-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include "render-task-impl.h"

// INTERNAL INCLUDES
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/update/common/animatable-property-messages.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-messages.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>

#if defined(DEBUG_ENABLED)
namespace
{
Debug::Filter* gLogRender = Debug::Filter::New(Debug::Concise, false, "LOG_RENDER_TASK");
}
#endif

namespace Dali
{
namespace Internal
{
namespace // For internal properties
{
// Properties

//              Name                 Type     writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY("viewportPosition", VECTOR2, true, true, true, Dali::RenderTask::Property::VIEWPORT_POSITION)
DALI_PROPERTY("viewportSize", VECTOR2, true, true, true, Dali::RenderTask::Property::VIEWPORT_SIZE)
DALI_PROPERTY("clearColor", VECTOR4, true, true, true, Dali::RenderTask::Property::CLEAR_COLOR)
DALI_PROPERTY("requiresSync", BOOLEAN, true, false, false, Dali::RenderTask::Property::REQUIRES_SYNC)
DALI_PROPERTY_TABLE_END(DEFAULT_OBJECT_PROPERTY_START_INDEX, RenderTaskDefaultProperties)

// Signals

const char* const SIGNAL_FINISHED = "finished";

TypeRegistration mType(typeid(Dali::RenderTask), typeid(Dali::BaseHandle), nullptr, RenderTaskDefaultProperties);

SignalConnectorType signalConnector1(mType, SIGNAL_FINISHED, &RenderTask::DoConnectSignal);

} // Unnamed namespace

RenderTaskPtr RenderTask::New(Actor* sourceActor, CameraActor* cameraActor, RenderTaskList& renderTaskList, bool isOverlayTask)
{
  // create scene object first so it's guaranteed to exist for the event side
  auto sceneObject = SceneGraph::RenderTask::New();

  // pass the pointer to base for message passing
  RenderTaskPtr task(new RenderTask(sceneObject, renderTaskList));

  // transfer scene object ownership to update manager
  const SceneGraph::RenderTaskList&    parentSceneObject = renderTaskList.GetSceneObject();
  OwnerPointer<SceneGraph::RenderTask> transferOwnership(sceneObject);
  AddTaskMessage(task->GetEventThreadServices(), parentSceneObject, transferOwnership);

  // Set the default source & camera actors
  task->SetSourceActor(sourceActor);
  task->SetCameraActor(cameraActor);

  // no need for additional messages as scene objects defaults match ours
  return task;
}

void RenderTask::SetSourceActor(Actor* actor)
{
  mSourceActor.SetActor(actor);

  if(GetRenderTaskSceneObject())
  {
    if(actor)
    {
      SetSourceNodeMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), &actor->GetNode());
    }
    else
    {
      SetSourceNodeMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), nullptr);
    }
  }

  // set the actor on exclusive container for hit testing
  mRenderTaskList.SetExclusive(this, mExclusive);
}

Actor* RenderTask::GetSourceActor() const
{
  return mSourceActor.GetActor();
}

Actor* RenderTask::GetStopperActor() const
{
  return mStopperActor.GetActor();
}

void RenderTask::SetExclusive(bool exclusive)
{
  if(mExclusive != exclusive)
  {
    mExclusive = exclusive;

    mRenderTaskList.SetExclusive(this, exclusive);

    if(GetRenderTaskSceneObject())
    {
      // scene object is being used in a separate thread; queue a message to set the value
      SetExclusiveMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), mExclusive);
    }
  }
}

bool RenderTask::IsExclusive() const
{
  return mExclusive;
}

void RenderTask::SetInputEnabled(bool enabled)
{
  mInputEnabled = enabled;
}

bool RenderTask::GetInputEnabled() const
{
  return mInputEnabled;
}

void RenderTask::SetCameraActor(CameraActor* cameraActor)
{
  mCameraActor.SetActor(cameraActor);

  if(GetRenderTaskSceneObject())
  {
    if(cameraActor)
    {
      SetCameraMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), &cameraActor->GetCameraSceneObject());
    }
    else
    {
      SetCameraMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), nullptr);
    }
  }

  // set the actor on exclusive container for hit testing
  mRenderTaskList.SetExclusive(this, mExclusive);
}

CameraActor* RenderTask::GetCameraActor() const
{
  if(mCameraActor.GetActor())
  {
    return static_cast<CameraActor*>(mCameraActor.GetActor());
  }
  return nullptr;
}

void RenderTask::SetFrameBuffer(FrameBufferPtr frameBuffer)
{
  mFrameBuffer = frameBuffer;
  Render::FrameBuffer* renderFrameBufferPtr(nullptr);
  if(frameBuffer)
  {
    renderFrameBufferPtr = mFrameBuffer->GetRenderObject();
  }

  if(GetRenderTaskSceneObject())
  {
    SetFrameBufferMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), renderFrameBufferPtr);
  }
}

FrameBuffer* RenderTask::GetFrameBuffer() const
{
  return mFrameBuffer.Get();
}

void RenderTask::SetScreenToFrameBufferFunction(ScreenToFrameBufferFunction conversionFunction)
{
  mScreenToFrameBufferFunction = conversionFunction;
}

RenderTask::ScreenToFrameBufferFunction RenderTask::GetScreenToFrameBufferFunction() const
{
  return mScreenToFrameBufferFunction;
}

void RenderTask::SetScreenToFrameBufferMappingActor(Dali::Actor& mappingActor)
{
  mInputMappingActor = WeakHandle<Dali::Actor>(mappingActor);
}

Dali::Actor RenderTask::GetScreenToFrameBufferMappingActor() const
{
  return mInputMappingActor.GetHandle();
}

void RenderTask::SetViewportGuideActor(Actor* actor)
{
  mViewportGuideActor.SetActor(actor);

  auto sceneObject = GetRenderTaskSceneObject();
  if(sceneObject)
  {
    auto& eventThreadServices = GetEventThreadServices();
    auto& updateManager       = eventThreadServices.GetUpdateManager();

    if(actor)
    {
      SetViewportGuideNodeMessage(eventThreadServices, *sceneObject, &actor->GetNode());
    }
    else
    {
      // Ensure that if the node is removed through this API, that the
      // viewport values are set back to their base value and that their dirty
      // flags are cleared after 1 frame.
      SetViewportGuideNodeMessage(eventThreadServices, *sceneObject, nullptr);

      auto renderTask               = const_cast<SceneGraph::RenderTask*>(sceneObject);
      auto viewportPositionProperty = const_cast<SceneGraph::AnimatableProperty<Vector2>*>(&renderTask->mViewportPosition);
      auto viewportSizeProperty     = const_cast<SceneGraph::AnimatableProperty<Vector2>*>(&renderTask->mViewportSize);

      OwnerPointer<SceneGraph::PropertyResetterBase> resetter1(
        new SceneGraph::BakerResetter(renderTask, viewportPositionProperty, SceneGraph::BakerResetter::Lifetime::SET));
      OwnerPointer<SceneGraph::PropertyResetterBase> resetter2(
        new SceneGraph::BakerResetter(renderTask, viewportSizeProperty, SceneGraph::BakerResetter::Lifetime::SET));
      AddResetterMessage(updateManager, resetter1);
      AddResetterMessage(updateManager, resetter2);
    }
  }
}

Actor* RenderTask::GetViewportGuideActor() const
{
  return mViewportGuideActor.GetActor();
}

void RenderTask::ResetViewportGuideActor()
{
  // Don't re-use the SetViewportGuideActor method for this task - the bake messages below will create their own resetters.
  mViewportGuideActor.SetActor(nullptr);

  if(GetRenderTaskSceneObject())
  {
    SetViewportGuideNodeMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), nullptr);

    EventThreadServices& eventThreadServices      = GetEventThreadServices();
    auto                 renderTask               = const_cast<SceneGraph::RenderTask*>(GetRenderTaskSceneObject());
    auto                 viewportPositionProperty = const_cast<SceneGraph::AnimatableProperty<Vector2>*>(&renderTask->mViewportPosition);
    auto                 viewportSizeProperty     = const_cast<SceneGraph::AnimatableProperty<Vector2>*>(&renderTask->mViewportSize);
    BakeMessage<Vector2>(eventThreadServices, *renderTask, *viewportPositionProperty, mViewportPosition);
    BakeMessage<Vector2>(eventThreadServices, *renderTask, *viewportSizeProperty, mViewportSize);
  }
}

void RenderTask::SetViewportPosition(const Vector2& value)
{
  mViewportPosition = value;

  if(GetRenderTaskSceneObject())
  {
    EventThreadServices& eventThreadServices      = GetEventThreadServices();
    auto                 renderTask               = const_cast<SceneGraph::RenderTask*>(GetRenderTaskSceneObject());
    auto                 viewportPositionProperty = const_cast<SceneGraph::AnimatableProperty<Vector2>*>(&renderTask->mViewportPosition);
    BakeMessage<Vector2>(eventThreadServices, *renderTask, *viewportPositionProperty, mViewportPosition);
  }
}

Vector2 RenderTask::GetCurrentViewportPosition() const
{
  return GetRenderTaskSceneObject() ? GetRenderTaskSceneObject()->GetViewportPosition(GetEventThreadServices().GetEventBufferIndex()) : Vector2::ZERO;
}

void RenderTask::SetViewportSize(const Vector2& value)
{
  mViewportSize = value;

  if(GetRenderTaskSceneObject())
  {
    EventThreadServices& eventThreadServices  = GetEventThreadServices();
    auto                 renderTask           = const_cast<SceneGraph::RenderTask*>(GetRenderTaskSceneObject());
    auto                 viewportSizeProperty = const_cast<SceneGraph::AnimatableProperty<Vector2>*>(&renderTask->mViewportSize);
    BakeMessage<Vector2>(eventThreadServices, *renderTask, *viewportSizeProperty, mViewportSize);
  }
}

Vector2 RenderTask::GetCurrentViewportSize() const
{
  return GetRenderTaskSceneObject() ? GetRenderTaskSceneObject()->GetViewportSize(GetEventThreadServices().GetEventBufferIndex()) : Vector2::ZERO;
}

void RenderTask::SetViewport(const Viewport& viewport)
{
  SetViewportPosition(Vector2(static_cast<float>(viewport.x), static_cast<float>(viewport.y)));
  SetViewportSize(Vector2(static_cast<float>(viewport.width), static_cast<float>(viewport.height)));
}

void RenderTask::GetViewport(Viewport& viewPort) const
{
  if(GetRenderTaskSceneObject())
  {
    BufferIndex bufferIndex = GetEventThreadServices().GetEventBufferIndex();

    if(!GetRenderTaskSceneObject()->GetViewportEnabled(bufferIndex))
    {
      Internal::Stage* stage = Internal::Stage::GetCurrent();
      if(stage)
      {
        Vector2 size(stage->GetSize());
        Actor*  sourceActor = mSourceActor.GetActor();
        if(sourceActor && sourceActor->OnScene())
        {
          Scene& scene = sourceActor->GetScene();
          size         = scene.GetSize();
        }

        viewPort.x = viewPort.y = 0;
        viewPort.width          = static_cast<int32_t>(size.width);  // truncated
        viewPort.height         = static_cast<int32_t>(size.height); // truncated
      }
    }
    else
    {
      const Vector2& position = GetRenderTaskSceneObject()->GetViewportPosition(bufferIndex);
      const Vector2& size     = GetRenderTaskSceneObject()->GetViewportSize(bufferIndex);
      viewPort.x              = static_cast<int32_t>(position.x);  // truncated
      viewPort.y              = static_cast<int32_t>(position.y);  // truncated
      viewPort.width          = static_cast<int32_t>(size.width);  // truncated
      viewPort.height         = static_cast<int32_t>(size.height); // truncated
    }
  }
}

void RenderTask::SetClearColor(const Vector4& color)
{
  if(mClearColor != color)
  {
    mClearColor = color;

    if(GetRenderTaskSceneObject())
    {
      // scene object is being used in a separate thread; queue a message to set the value
      EventThreadServices& eventThreadServices = GetEventThreadServices();
      auto                 renderTask          = const_cast<SceneGraph::RenderTask*>(GetRenderTaskSceneObject());
      auto                 clearColorProperty  = const_cast<SceneGraph::AnimatableProperty<Vector4>*>(&renderTask->mClearColor);
      BakeMessage<Vector4>(eventThreadServices, *renderTask, *clearColorProperty, mClearColor);
    }
  }
}

const Vector4& RenderTask::GetClearColor() const
{
  return GetRenderTaskSceneObject() ? GetRenderTaskSceneObject()->GetClearColor(GetEventThreadServices().GetEventBufferIndex()) : Vector4::ZERO;
}

void RenderTask::SetSyncRequired(bool requiresSync)
{
  if(mRequiresSync != requiresSync)
  {
    mRequiresSync = requiresSync;

    if(GetRenderTaskSceneObject())
    {
      // scene object is being used in a separate thread; queue a message to set the value
      SetSyncRequiredMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), requiresSync);
    }
  }
}

bool RenderTask::IsSyncRequired() const
{
  return mRequiresSync;
}

void RenderTask::SetClearEnabled(bool enabled)
{
  if(mClearEnabled != enabled)
  {
    mClearEnabled = enabled;

    if(GetRenderTaskSceneObject())
    {
      // scene object is being used in a separate thread; queue a message to set the value
      SetClearEnabledMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), mClearEnabled);
    }
  }
}

bool RenderTask::GetClearEnabled() const
{
  return mClearEnabled;
}

void RenderTask::SetCullMode(bool mode)
{
  if(mCullMode != mode)
  {
    mCullMode = mode;

    if(GetRenderTaskSceneObject())
    {
      // scene object is being used in a separate thread; queue a message to set the value
      SetCullModeMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), mCullMode);
    }
  }
}

bool RenderTask::GetCullMode() const
{
  return mCullMode;
}

void RenderTask::SetRefreshRate(uint32_t refreshRate)
{
  DALI_LOG_TRACE_METHOD_FMT(gLogRender, "this:%p  rate:%d\n", this, refreshRate);
  DALI_LOG_INFO(gLogRender, Debug::General, "RenderTask::SetRefreshRate(this:%p, %d)\n", this, refreshRate);

  mRefreshRate = refreshRate; // cached for GetRefreshRate()

  // Note - even when refreshRate is the same as mRefreshRate, a message should be sent

  if(GetRenderTaskSceneObject())
  {
    // sceneObject is being used in a separate thread; queue a message to set the value
    SetRefreshRateMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), refreshRate);
  }
}

uint32_t RenderTask::GetRefreshRate() const
{
  return mRefreshRate;
}

bool RenderTask::IsHittable(Vector2& screenCoords) const
{
  // True when input is enabled, source & camera actor are valid
  bool inputEnabled(false);

  Actor*       sourceActor = GetSourceActor();
  CameraActor* cameraActor = GetCameraActor();

  if(mInputEnabled &&
     nullptr != sourceActor &&
     sourceActor->OnScene() &&
     nullptr != cameraActor &&
     cameraActor->OnScene())
  {
    // If the actors are rendered off-screen, then the screen coordinates must be converted
    // and the conversion function will tell us if they are inside or outside
    if(TranslateCoordinates(screenCoords))
    {
      // This is a suitable render-task for input handling
      inputEnabled = true;
    }
  }

  return inputEnabled;
}

bool RenderTask::TranslateCoordinates(Vector2& screenCoords) const
{
  // return true for on-screen tasks
  bool inside(true);
  // If the actors are rendered off-screen, then the screen coordinates must be converted
  // the function should only be called for offscreen tasks
  Dali::Actor mappingActor = GetScreenToFrameBufferMappingActor();

  if(mFrameBuffer && mappingActor)
  {
    Internal::Actor* inputMappingActor = &GetImplementation(mappingActor);
    if(!inputMappingActor->OnScene())
    {
      return false;
    }

    CameraActor*     localCamera       = GetCameraActor();
    StagePtr         stage             = Stage::GetCurrent();
    if(stage)
    {
      Vector2      size(stage->GetSize());
      CameraActor* defaultCamera(&stage->GetDefaultCameraActor());
      Actor*       sourceActor = mSourceActor.GetActor();
      if(sourceActor && sourceActor->OnScene())
      {
        Scene& scene  = sourceActor->GetScene();
        size          = scene.GetSize();
        defaultCamera = &scene.GetDefaultCameraActor();
      }

      if(localCamera)
      {
        Viewport viewport;
        viewport.x = viewport.y = 0;
        viewport.width          = static_cast<int32_t>(size.width);  // truncated
        viewport.height         = static_cast<int32_t>(size.height); // truncated

        float localX, localY;
        inside            = inputMappingActor->ScreenToLocal(defaultCamera->GetViewMatrix(), defaultCamera->GetProjectionMatrix(), viewport, localX, localY, screenCoords.x, screenCoords.y);
        Vector3 actorSize = inputMappingActor->GetCurrentSize() * inputMappingActor->GetCurrentWorldScale();
        if(inside && localX >= 0.f && localX <= actorSize.x && localY >= 0.f && localY <= actorSize.y)
        {
          screenCoords.x = localX;
          screenCoords.y = localY;
        }
        else
        {
          inside = false;
        }
      }
      else
      {
        inside = false;
      }
    }
  }
  else if(mFrameBuffer && mScreenToFrameBufferFunction)
  {
    inside = mScreenToFrameBufferFunction(screenCoords);
  }
  return inside;
}

void RenderTask::GetHittableViewport(Viewport& viewPort) const
{
  if(GetRenderTaskSceneObject())
  {
    if(GetInputEnabled())
    {
      if(mFrameBuffer)
      {
        auto mappingActor = GetScreenToFrameBufferMappingActor();
        if(mappingActor)
        {
          Actor& inputMappingActor = GetImplementation(mappingActor);

          Vector3 actorSize = inputMappingActor.GetCurrentSize() * inputMappingActor.GetCurrentWorldScale();

          viewPort.x = viewPort.y = 0;
          viewPort.width          = static_cast<int32_t>(actorSize.x + 0.5f); // rounded
          viewPort.height         = static_cast<int32_t>(actorSize.y + 0.5f); // rounded
        }
        else
        {
          // For the case to use ScreenToFrameBufferFunction
          GetViewport(viewPort);
        }
      }
      else
      {
        GetViewport(viewPort);
      }
    }
  }
}

bool RenderTask::WorldToViewport(const Vector3& position, float& viewportX, float& viewportY) const
{
  CameraActor* cam = GetCameraActor();

  Vector4 pos(position);
  pos.w = 1.0;

  Vector4 viewportPosition;

  Viewport viewport;
  GetViewport(viewport);

  bool ok = ProjectFull(pos,
                        cam->GetViewMatrix(),
                        cam->GetProjectionMatrix(),
                        static_cast<float>(viewport.x),      // truncated
                        static_cast<float>(viewport.y),      // truncated
                        static_cast<float>(viewport.width),  // truncated
                        static_cast<float>(viewport.height), // truncated
                        viewportPosition);
  if(ok)
  {
    viewportX = viewportPosition.x;
    viewportY = viewportPosition.y;
  }

  return ok;
}

bool RenderTask::ViewportToLocal(Actor* actor, float viewportX, float viewportY, float& localX, float& localY) const
{
  return actor->ScreenToLocal(*this, localX, localY, viewportX, viewportY);
}

void RenderTask::SetRenderPassTag(uint32_t renderPassTag)
{
  if(mRenderPassTag != renderPassTag)
  {
    mRenderPassTag = renderPassTag;
    if(GetRenderTaskSceneObject())
    {
      SetRenderPassTagMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), renderPassTag);
    }
  }
}

uint32_t RenderTask::GetRenderPassTag() const
{
  return mRenderPassTag;
}

void RenderTask::SetOrderIndex(int32_t orderIndex)
{
  if(mOrderIndex != orderIndex)
  {
    mOrderIndex = orderIndex;

    // We only need to sort render task list if it is valid.
    if(GetRenderTaskSceneObject())
    {
      mRenderTaskList.RequestToSort();
    }
  }
}

int32_t RenderTask::GetOrderIndex() const
{
  return mOrderIndex;
}

uint32_t RenderTask::GetRenderTaskId() const
{
  return mRenderTaskId;
}

void RenderTask::RenderUntil(Actor* actor)
{
  Actor* target = mSourceActor.GetActor();
  DALI_ASSERT_ALWAYS((target && actor)
      && "RenderTask::RenderUntil() has empty actors.");
  DALI_ASSERT_ALWAYS((target->GetHierarchyDepth() < actor->GetHierarchyDepth())
      && "RenderTask::RenderUntil() has reversed hierarchy.");

  Actor* parent = actor;
  while(parent != target && !(parent->IsLayer()))
  {
    parent = parent->GetParent();
  }

  if(parent == target && GetRenderTaskSceneObject())
  {
    mStopperActor.SetActor(actor);
    SetStopperNodeMessage(GetEventThreadServices(), *GetRenderTaskSceneObject(), &actor->GetNode());
  }
}

const SceneGraph::RenderTask* RenderTask::GetRenderTaskSceneObject() const
{
  return static_cast<const SceneGraph::RenderTask*>(mUpdateObject);
}

void RenderTask::RemoveRenderTaskSceneObject(RenderTaskList& renderTaskList)
{
  // send a message to remove the scene-graph RenderTask
  const SceneGraph::RenderTaskList& parentSceneObject = renderTaskList.GetSceneObject();
  RemoveTaskMessage(GetEventThreadServices(), parentSceneObject, *GetRenderTaskSceneObject());

  mUpdateObject = nullptr;
}

RenderTaskList& RenderTask::GetRenderTaskList() const
{
  return mRenderTaskList;
}

/********************************************************************************
 ********************************   PROPERTY METHODS   **************************
 ********************************************************************************/

void RenderTask::SetDefaultProperty(Property::Index index, const Property::Value& property)
{
  switch(index)
  {
    case Dali::RenderTask::Property::VIEWPORT_POSITION:
    {
      SetViewportPosition(property.Get<Vector2>());
      break;
    }
    case Dali::RenderTask::Property::VIEWPORT_SIZE:
    {
      SetViewportSize(property.Get<Vector2>());
      break;
    }
    case Dali::RenderTask::Property::CLEAR_COLOR:
    {
      SetClearColor(property.Get<Vector4>());
      break;
    }
    case Dali::RenderTask::Property::REQUIRES_SYNC:
    {
      SetSyncRequired(property.Get<bool>());
      break;
    }
    default:
    {
      // nothing to do
      break;
    }
  }
}

Property::Value RenderTask::GetDefaultProperty(Property::Index index) const
{
  Property::Value value;

  switch(index)
  {
    case Dali::RenderTask::Property::VIEWPORT_POSITION:
    {
      value = mViewportPosition;
      break;
    }
    case Dali::RenderTask::Property::VIEWPORT_SIZE:
    {
      value = mViewportSize;
      break;
    }
    case Dali::RenderTask::Property::CLEAR_COLOR:
    {
      value = mClearColor;
      break;
    }
    case Dali::RenderTask::Property::REQUIRES_SYNC:
    {
      value = IsSyncRequired();
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS(false && "RenderTask property index out of range"); // should not come here
      break;
    }
  }

  return value;
}

Property::Value RenderTask::GetDefaultPropertyCurrentValue(Property::Index index) const
{
  Property::Value value;

  switch(index)
  {
    case Dali::RenderTask::Property::VIEWPORT_POSITION:
    {
      value = GetCurrentViewportPosition();
      break;
    }
    case Dali::RenderTask::Property::VIEWPORT_SIZE:
    {
      value = GetCurrentViewportSize();
      break;
    }
    case Dali::RenderTask::Property::CLEAR_COLOR:
    {
      value = GetClearColor();
      break;
    }
    case Dali::RenderTask::Property::REQUIRES_SYNC:
    {
      value = IsSyncRequired();
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS(false && "RenderTask property index out of range"); // should not come here
      break;
    }
  }

  return value;
}

void RenderTask::OnNotifyDefaultPropertyAnimation(Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType)
{
  switch(animationType)
  {
    case Animation::TO:
    case Animation::BETWEEN:
    {
      switch(index)
      {
        case Dali::RenderTask::Property::VIEWPORT_POSITION:
        {
          value.Get(mViewportPosition);
          break;
        }
        case Dali::RenderTask::Property::VIEWPORT_SIZE:
        {
          value.Get(mViewportSize);
          break;
        }
        case Dali::RenderTask::Property::CLEAR_COLOR:
        {
          value.Get(mClearColor);
          break;
        }
        case Dali::RenderTask::Property::REQUIRES_SYNC:
        default:
        {
          // Nothing to do as not animatable
          break;
        }
      }
      break;
    }

    case Animation::BY:
    {
      switch(index)
      {
        case Dali::RenderTask::Property::VIEWPORT_POSITION:
        {
          AdjustValue<Vector2>(mViewportPosition, value);
          break;
        }
        case Dali::RenderTask::Property::VIEWPORT_SIZE:
        {
          AdjustValue<Vector2>(mViewportSize, value);
          break;
        }
        case Dali::RenderTask::Property::CLEAR_COLOR:
        {
          AdjustValue<Vector4>(mClearColor, value);
          break;
        }
        case Dali::RenderTask::Property::REQUIRES_SYNC:
        default:
        {
          // Nothing to do as not animatable
          break;
        }
      }
      break;
    }
  }
}

const SceneGraph::PropertyBase* RenderTask::GetSceneObjectAnimatableProperty(Property::Index index) const
{
  if(!GetRenderTaskSceneObject())
  {
    return nullptr;
  }

  const SceneGraph::PropertyBase* property(nullptr);

  switch(index)
  {
    case Dali::RenderTask::Property::VIEWPORT_POSITION:
    {
      property = &GetRenderTaskSceneObject()->mViewportPosition;
      break;
    }
    case Dali::RenderTask::Property::VIEWPORT_SIZE:
    {
      property = &GetRenderTaskSceneObject()->mViewportSize;
      break;
    }
    case Dali::RenderTask::Property::CLEAR_COLOR:
    {
      property = &GetRenderTaskSceneObject()->mClearColor;
      break;
    }
    default:
    {
      break;
    }
  }
  if(!property)
  {
    // not our property, ask base
    property = Object::GetSceneObjectAnimatableProperty(index);
  }

  return property;
}

const PropertyInputImpl* RenderTask::GetSceneObjectInputProperty(Property::Index index) const
{
  // animatable properties are input as well, Object::GetSceneObjectInputProperty does the same so no need to call it
  return GetSceneObjectAnimatableProperty(index);
}

bool RenderTask::HasFinished()
{
  bool finished = false;

  if(GetRenderTaskSceneObject())
  {
    const uint32_t counter = GetRenderTaskSceneObject()->GetRenderedOnceCounter();

    if(mRefreshOnceCounter < counter)
    {
      finished            = true;
      mRefreshOnceCounter = counter;
    }
  }

  DALI_LOG_INFO(gLogRender, Debug::General, "RenderTask::HasFinished()=%s SCRT:%p  SC\n", finished ? "T" : "F", GetRenderTaskSceneObject());

  return finished;
}

void RenderTask::EmitSignalFinish()
{
  DALI_LOG_INFO(gLogRender, Debug::General, "RenderTask::EmitSignalFinish(this:%p)\n", this);

  if(!mSignalFinished.Empty())
  {
    Dali::RenderTask handle(this);
    mSignalFinished.Emit(handle);
  }
}

Dali::RenderTask::RenderTaskSignalType& RenderTask::FinishedSignal()
{
  return mSignalFinished;
}

bool RenderTask::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  bool        connected(true);
  RenderTask* renderTask = static_cast<RenderTask*>(object); // TypeRegistry guarantees that this is the correct type.

  if(0 == strcmp(signalName.c_str(), SIGNAL_FINISHED))
  {
    renderTask->FinishedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

RenderTask::RenderTask(const SceneGraph::RenderTask* sceneObject, RenderTaskList& renderTaskList)
: Object(sceneObject),
  mSourceActor(),
  mCameraActor(),
  mViewportGuideActor(),
  mInputMappingActor(),
  mRenderTaskList(renderTaskList),
  mClearColor(Dali::RenderTask::DEFAULT_CLEAR_COLOR),
  mViewportPosition(Vector2::ZERO),
  mViewportSize(Vector2::ZERO),
  mRefreshRate(Dali::RenderTask::DEFAULT_REFRESH_RATE),
  mRefreshOnceCounter(0u),
  mScreenToFrameBufferFunction(Dali::RenderTask::DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION),
  mExclusive(Dali::RenderTask::DEFAULT_EXCLUSIVE),
  mInputEnabled(Dali::RenderTask::DEFAULT_INPUT_ENABLED),
  mClearEnabled(Dali::RenderTask::DEFAULT_CLEAR_ENABLED),
  mCullMode(Dali::RenderTask::DEFAULT_CULL_MODE),
  mRequiresSync(false)
{
  // Set id of render task
  mRenderTaskId = sceneObject->GetNotifyId();

  DALI_LOG_INFO(gLogRender, Debug::General, "RenderTask::RenderTask(this:%p)\n", this);
  // scene object handles observation of source and camera
}

RenderTask::~RenderTask()
{
  DALI_LOG_INFO(gLogRender, Debug::General, "RenderTask::~RenderTask(this:%p)\n", this);
  // scene object deletion is handled by our parent
  // scene object handles observation of source and camera
}

} // namespace Internal

} // namespace Dali
