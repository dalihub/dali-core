#ifndef DALI_INTERNAL_SCENE_GRAPH_CAMERA_MESSAGES_H
#define DALI_INTERNAL_SCENE_GRAPH_CAMERA_MESSAGES_H

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
 */

#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>

namespace Dali::Internal
{
// value types used by messages
template<>
struct ParameterType<Dali::Camera::Type>
: public BasicType<Dali::Camera::Type>
{
};

template<>
struct ParameterType<Dali::Camera::ProjectionMode>
: public BasicType<Dali::Camera::ProjectionMode>
{
};

namespace SceneGraph
{
inline void SetTypeMessage(EventThreadServices& eventThreadServices, const Camera& camera, Dali::Camera::Type parameter)
{
  using LocalType = MessageValue1<Camera, Dali::Camera::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetType, parameter);
}

inline void SetProjectionModeMessage(EventThreadServices& eventThreadServices, const Camera& camera, Dali::Camera::ProjectionMode parameter)
{
  using LocalProjectionMode = MessageValue1<Camera, Dali::Camera::ProjectionMode>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalProjectionMode));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalProjectionMode(&camera, &Camera::SetProjectionMode, parameter);
}

inline void SetProjectionDirectionMessage(EventThreadServices& eventThreadServices, const Camera& camera, Dali::DevelCameraActor::ProjectionDirection parameter)
{
  using LocalProjectionDirection = MessageValue1<Camera, Dali::DevelCameraActor::ProjectionDirection>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalProjectionDirection));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalProjectionDirection(&camera, &Camera::SetProjectionDirection, parameter);
}

inline void BakeFieldOfViewMessage(EventThreadServices& eventThreadServices, const Camera& camera, float parameter)
{
  using LocalType = MessageDoubleBuffered1<Camera, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::BakeFieldOfView, parameter);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
    new SceneGraph::BakerResetter(const_cast<SceneGraph::Camera*>(&camera),
                                  const_cast<SceneGraph::AnimatableProperty<float>*>(&camera.mFieldOfView),
                                  BakerResetter::Lifetime::BAKE));
  AddResetterMessage(eventThreadServices.GetUpdateManager(), resetter);
}

inline void BakeOrthographicSizeMessage(EventThreadServices& eventThreadServices, const Camera& camera, float parameter)
{
  using LocalType = MessageDoubleBuffered1<Camera, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::BakeOrthographicSize, parameter);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
    new SceneGraph::BakerResetter(const_cast<SceneGraph::Camera*>(&camera),
                                  const_cast<SceneGraph::AnimatableProperty<float>*>(&camera.mOrthographicSize),
                                  BakerResetter::Lifetime::BAKE));
  AddResetterMessage(eventThreadServices.GetUpdateManager(), resetter);
}

inline void BakeAspectRatioMessage(EventThreadServices& eventThreadServices, const Camera& camera, float parameter)
{
  using LocalType = MessageDoubleBuffered1<Camera, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::BakeAspectRatio, parameter);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
    new SceneGraph::BakerResetter(const_cast<SceneGraph::Camera*>(&camera),
                                  const_cast<SceneGraph::AnimatableProperty<float>*>(&camera.mAspectRatio),
                                  BakerResetter::Lifetime::BAKE));
  AddResetterMessage(eventThreadServices.GetUpdateManager(), resetter);
}

inline void SetNearClippingPlaneMessage(EventThreadServices& eventThreadServices, const Camera& camera, float parameter)
{
  using LocalType = MessageValue1<Camera, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetNearClippingPlane, parameter);
}

inline void SetFarClippingPlaneMessage(EventThreadServices& eventThreadServices, const Camera& camera, float parameter)
{
  using LocalType = MessageValue1<Camera, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetFarClippingPlane, parameter);
}

inline void SetReflectByPlaneMessage(EventThreadServices& eventThreadServices, const Camera& camera, const Vector4& plane)
{
  using LocalType = MessageValue1<Camera, Vector4>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetReflectByPlane, plane);
}

inline void SetTargetPositionMessage(EventThreadServices& eventThreadServices, const Camera& camera, const Vector3& parameter)
{
  using LocalType = MessageValue1<Camera, Vector3>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetTargetPosition, parameter);
}

inline void SetInvertYAxisMessage(EventThreadServices& eventThreadServices, const Camera& camera, bool parameter)
{
  using LocalType = MessageValue1<Camera, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetInvertYAxis, parameter);
}

inline void RotateProjectionMessage(EventThreadServices& eventThreadServices, const Camera& camera, int parameter)
{
  typedef MessageValue1<Camera, int> LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::RotateProjection, parameter);
}

} // namespace SceneGraph
} // namespace Dali::Internal

#endif //DALI_INTERNAL_SCENE_GRAPH_CAMERA_MESSAGES_H
