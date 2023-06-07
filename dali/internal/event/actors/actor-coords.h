#ifndef DALI_INTERNAL_EVENT_ACTORS_ACTOR_COORDS_H
#define DALI_INTERNAL_EVENT_ACTORS_ACTOR_COORDS_H

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

#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/viewport.h>

namespace Dali::Internal
{
/**
 * Convert screen coordinates to local coordinates
 *
 * @param[in] viewMatrix The view matrix used to display this entity
 * @param[in] projectionMatrix The projection matrix used to display this entity
 * @param[in] worldMatrix The world matrix of this entity
 * @param[in] currentSize The 2d bounding box for this entity
 * @param[in] viewport The viewport used for drawing
 * @param[out] localX The local X coordinate
 * @param[out] localY The local Y coordinate
 * @param[in] screenX The screen X coordinate
 * @param[in] screenY The screen Y coordinate
 * @return true if the conversion was successful
 */
bool ConvertScreenToLocal(const Matrix&   viewMatrix,
                          const Matrix&   projectionMatrix,
                          const Matrix&   worldMatrix,
                          const Vector3&  currentSize,
                          const Viewport& viewport,
                          float&          localX,
                          float&          localY,
                          float           screenX,
                          float           screenY);

/**
 * Convert screen coordinates to local coordinates
 *
 * @param[in] renderTask The render task used to display this entity
 * @param[in] worldMatrix The world matrix of this entity
 * @param[in] currentSize The 2d bounding box for this entity
 * @param[out] localX The local X coordinate
 * @param[out] localY The local Y coordinate
 * @param[in] screenX The screen X coordinate
 * @param[in] screenY The screen Y coordinate
 * @return true if the conversion was successful
 */
bool ConvertScreenToLocalRenderTask(
  const RenderTask& renderTask,
  const Matrix&     worldMatrix,
  const Vector3&    currentSize,
  float&            localX,
  float&            localY,
  float             screenX,
  float             screenY);

/**
 * Convert screen coordinates to local coordinates
 * Search through the given renderTaskList to check if this entity can be converted
 *
 * @param[in] renderTaskList The render task list to search
 * @param[in] worldMatrix The world matrix of this entity
 * @param[in] currentSize The 2d bounding box for this entity
 * @param[out] localX The local X coordinate
 * @param[out] localY The local Y coordinate
 * @param[in] screenX The screen X coordinate
 * @param[in] screenY The screen Y coordinate
 * @return true if the conversion was successful
 */
bool ConvertScreenToLocalRenderTaskList(
  const RenderTaskList& renderTaskList,
  const Matrix&         worldMatrix,
  const Vector3&        currentSize,
  float&                localX,
  float&                localY,
  float                 screenX,
  float                 screenY);

/**
 * Calculate the screen position of the actor from it's node transform and anchor point
 *
 * @param[in] actor The actor to calculate the screen position for
 * @param[in] bufferIndex The current event buffer index
 * @return the screen position
 */
const Vector2 CalculateActorScreenPosition(const Actor& actor, BufferIndex bufferIndex);

/**
 * Calculate the screen extents of the actor from its node transform, anchor point and size
 *
 * @param[in] actor The actor
 * @param[in] screenPosition The actor's screen position
 * @param[in] bufferIndex The current event buffer index
 * @return the screen extents of the actor
 */
Rect<> CalculateActorScreenExtents(const Actor& actor, const Vector2& screenPosition, BufferIndex bufferIndex);

/**
 * Convert local coordinates to screen coordinates
 *
 * @param[in] viewMatrix The view matrix used to display this entity
 * @param[in] projectionMatrix The projection matrix used to display this entity
 * @param[in] worldMatrix The world matrix of this entity
 * @param[in] viewport The viewport used for drawing
 * @param[in] localPosition The local position
 * @param[out] screenX The screen X coordinate
 * @param[out] screenY The screen Y coordinate
 * @return true if the conversion was successful
 */
bool ConvertLocalToScreen(const Matrix&   viewMatrix,
                          const Matrix&   projectionMatrix,
                          const Matrix&   worldMatrix,
                          const Viewport& viewport,
                          const Vector3&  localPosition,
                          float&          screenX,
                          float&          screenY);

/**
 * Convert local coordinates to screen coordinates
 *
 * @param[in] renderTask The render task used to display this entity
 * @param[in] actor The actor of this entity
 * @param[in] worldMatrix The world matrix of this entity
 * @param[in] localPosition The local position
 * @param[out] screenX The screen X coordinate
 * @param[out] screenY The screen Y coordinate
 * @return true if the conversion was successful
 */
bool ConvertLocalToScreenRenderTask(
  const RenderTask& renderTask,
  const Actor&      actor,
  const Matrix&     worldMatrix,
  const Vector3&    localPosition,
  float&            screenX,
  float&            screenY);

/**
 * Convert local coordinates to screen coordinates
 * Search through the given renderTaskList to check if this entity can be converted
 *
 * @param[in] renderTaskList The render task list to search
 * @param[in] actor The actor of this entity
 * @param[in] worldMatrix The world matrix of this entity
 * @param[in] localPosition The local position
 * @param[out] screenX The screen X coordinate
 * @param[out] screenY The screen Y coordinate
 * @return true if the conversion was successful
 */
bool ConvertLocalToScreenRenderTaskList(
  const RenderTaskList& renderTaskList,
  const Actor&          actor,
  const Matrix&         worldMatrix,
  const Vector3&        localPosition,
  float&                screenX,
  float&                screenY);

/**
 * Calculate the screen position of the actor from it's node transform and anchor point.
 * It will consider rendertasklist.
 *
 * @param[in] actor The actor
 * @param[in] bufferIndex The current event buffer index
 * @return the screen extents of the actor consider camera.
 */
const Vector2 CalculateActorScreenPositionRenderTaskList(const Actor& actor, BufferIndex bufferIndex);

/**
 * Convert local coordinates to screen coordinates extent
 *
 * @param[in] renderTask The render task used to display this entity
 * @param[in] actor The actor of this entity
 * @param[in] worldMatrix The world matrix of this entity
 * @param[in] localPosition The local position
 * @param[out] screenExtent The screen Extent
 * @return true if the conversion was successful
 */
bool ConvertLocalToScreenExtentRenderTask(
  const RenderTask& renderTask,
  const Actor&      actor,
  const Matrix&     worldMatrix,
  const Vector3&    currentSize,
  Rect<>&           screenExtent);

/**
 * Convert local coordinates to screen coordinates extent
 * Search through the given renderTaskList to check if this entity can be converted
 *
 * @param[in] renderTaskList The render task list to search
 * @param[in] actor The actor of this entity
 * @param[in] worldMatrix The world matrix of this entity
 * @param[in] localPosition The local position
 * @param[out] screenExtent The screen Extent
 * @return true if the conversion was successful
 */
bool ConvertLocalToScreenExtentRenderTaskList(
  const RenderTaskList& renderTaskList,
  const Actor&          actor,
  const Matrix&         worldMatrix,
  const Vector3&        currentSize,
  Rect<>&               screenExtent);
/**
 * Calculate the screen extents of the actor from its node transform, anchor point and size.
 * It will consider rendertasklist.
 *
 * @param[in] actor The actor
 * @param[in] bufferIndex The current event buffer index
 * @return the screen extents of the actor consider camera.
 */
Rect<> CalculateActorScreenExtentsRenderTaskList(const Actor& actor, BufferIndex bufferIndex);

/**
 * Get the world transform of the actor.
 *
 * This calculates the world transform from scratch using only event
 * side properties - it does not rely on the update thread to have
 * already calculated the transform.
 *
 * @param[in] actor The actor to calculate the world matrix for
 * @return The world transform matrix
 */
Matrix CalculateActorWorldTransform(const Actor& actor);

/**
 * Get the world color of the actor.
 *
 * This calcualtes the world color of the actor from scratch using
 * only event side properties. It does not rely on the update thread
 * to have already calculated the color.
 *
 * @param[in] actor The actor to calculate the world color for
 * @return the world color
 */
Vector4 CalculateActorWorldColor(const Actor& actor);

/**
 * @brief Get the rotate of the actor look at specific position.
 *
 * @param[in] actor The actor for which to calculate the look at orientation.
 * @param[in] target The target world position to look at.
 * @param[in] up The up vector after target look at.
 * @param[in] localForward The forward vector of actor when it's orientation is not applied.
 * @param[in] localUp The up vector of actor when it's orientation is not applied.
 * @return The orientation result of this lookAt result.
 */
Quaternion CalculateActorLookAtOrientation(const Actor& actor, Vector3 target, Vector3 up, Vector3 localForward, Vector3 localUp);

} // namespace Dali::Internal

#endif // DALI_INTERNAL_EVENT_ACTORS_ACTOR_COORDS_H
