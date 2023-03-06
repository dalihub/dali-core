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
#include <dali/devel-api/events/hit-test-algorithm.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{
namespace HitTestAlgorithm
{
bool HitTest(Stage stage, const Vector2& screenCoordinates, Results& results, HitTestFunction func)
{
  Internal::Stage& stageImpl = GetImplementation(stage);
  return Internal::HitTestAlgorithm::HitTest(stageImpl.GetSize(), stageImpl.GetRenderTaskList(), stageImpl.GetLayerList(), screenCoordinates, results, func);
}

bool BuildPickingRay(RenderTask renderTask, const Vector2& screenCoordinates, Vector3& origin, Vector3& direction)
{
  Viewport viewport = renderTask.GetViewport();
  if(screenCoordinates.x < static_cast<float>(viewport.x) ||
     screenCoordinates.x > static_cast<float>(viewport.x + viewport.width) ||
     screenCoordinates.y < static_cast<float>(viewport.y) ||
     screenCoordinates.y > static_cast<float>(viewport.y + viewport.height))
  {
    // The screen coordinate is outside the viewport of render task. The viewport clips all layers.
    return false;
  }
  CameraActor            cameraActor     = renderTask.GetCameraActor();
  Internal::CameraActor& cameraActorImpl = GetImplementation(cameraActor);
  Vector4                rayOrigin;
  Vector4                rayDirection;
  bool                   success = cameraActorImpl.BuildPickingRay(screenCoordinates, viewport, rayOrigin, rayDirection);
  if(success)
  {
    origin    = Vector3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
    direction = Vector3(rayDirection.x, rayDirection.y, rayDirection.z);
  }
  return success;
}

} // namespace HitTestAlgorithm

} // namespace Dali
