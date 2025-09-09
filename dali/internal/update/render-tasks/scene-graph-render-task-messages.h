#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_MESSAGES_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_MESSAGES_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/renderers/render-frame-buffer.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/public-api/math/viewport.h>
#include <dali/public-api/render-tasks/render-task.h>

namespace Dali::Internal::SceneGraph
{
// Messages for RenderTask
inline void SetFrameBufferMessage(EventThreadServices& eventThreadServices, const RenderTask& task, Render::FrameBuffer* frameBuffer)
{
  using LocalType = MessageValue1<RenderTask, Render::FrameBuffer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetFrameBuffer, frameBuffer);
}

inline void SetClearEnabledMessage(EventThreadServices& eventThreadServices, const RenderTask& task, bool enabled)
{
  using LocalType = MessageValue1<RenderTask, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetClearEnabled, enabled);
}

inline void SetCullModeMessage(EventThreadServices& eventThreadServices, const RenderTask& task, bool mode)
{
  using LocalType = MessageValue1<RenderTask, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetCullMode, mode);
}

inline void SetRefreshRateMessage(EventThreadServices& eventThreadServices, const RenderTask& task, uint32_t refreshRate)
{
  using LocalType = MessageValue1<RenderTask, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetRefreshRate, refreshRate);
}

inline void SetSourceNodeMessage(EventThreadServices& eventThreadServices, const RenderTask& task, const Node* constNode)
{
  // Scene graph thread can destroy this object.
  Node* node = const_cast<Node*>(constNode);

  using LocalType = MessageValue1<RenderTask, Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetSourceNode, node);
}

inline void SetStopperNodeMessage(EventThreadServices& eventThreadServices, const RenderTask& task, const Node* constNode)
{
  // Scene graph thread can destroy this object.
  Node* node = const_cast<Node*>(constNode);

  using LocalType = MessageValue1<RenderTask, Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetStopperNode, node);
}

inline void SetCameraMessage(EventThreadServices& eventThreadServices, const RenderTask& task, const Camera* constCamera)
{
  using LocalType = MessageValue1<RenderTask, Camera*>;

  Camera* cameraNode = const_cast<Camera*>(constCamera);
  // Reserve memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetCamera, cameraNode);
}

inline void SetViewportGuideNodeMessage(EventThreadServices& eventThreadServices, const RenderTask& task, const Node* constNode)
{
  // Scene graph thread can destroy this object.
  Node* node = const_cast<Node*>(constNode);

  using LocalType = MessageValue1<RenderTask, Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetViewportGuideNode, node);
}

inline void SetExclusiveMessage(EventThreadServices& eventThreadServices, const RenderTask& task, bool exclusive)
{
  using LocalType = MessageValue1<RenderTask, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetExclusive, exclusive);
}

inline void SetSyncRequiredMessage(EventThreadServices& eventThreadServices, const RenderTask& task, bool requiresSync)
{
  using LocalType = MessageValue1<RenderTask, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetSyncRequired, requiresSync);
}

inline void SetRenderPassTagMessage(EventThreadServices& eventThreadServices, const RenderTask& task, uint32_t renderPass)
{
  using LocalType = MessageValue1<RenderTask, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetRenderPassTag, renderPass);
}

inline void SetRenderedScaleFactorMessage(EventThreadServices& eventThreadServices, const RenderTask& task, const Vector2& scaleFactor)
{
  using LocalType = MessageValue1<RenderTask, Vector2>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&task, &RenderTask::SetRenderedScaleFactor, scaleFactor);
}

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_MESSAGES_H
