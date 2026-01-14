#ifndef DALI_INTERNAL_EVENT_ACTORS_ACTOR_COORDS_H
#define DALI_INTERNAL_EVENT_ACTORS_ACTOR_COORDS_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 * Calculate the screen position of the actor from its transform and anchor point
 *
 * This function calculates the screen coordinates of an actor in 2D space using
 * the actor's target (goal) state properties. It calculates the world transform
 * from scratch using only event-side properties and does not rely on the update
 * thread to have already calculated the transform.
 *
 * @note This function operates under the following assumptions:
 *       - 2D space only (Z coordinates are always treated as 0)
 *       - No camera rotation (camera always faces +Z direction)
 *       - No perspective projection (orthographic projection or simple parallel translation)
 *       - Uses actor's target size and target transform properties
 *
 * @param[in] actor The actor to calculate the screen position for
 * @return Vector2 The calculated screen coordinates. Returns (0,0) if actor is not on scene
 */
const Vector2 CalculateActorScreenPosition(const Actor& actor);

/**
 * Calculate the current screen position of the actor from its node transform and anchor point
 *
 * This function calculates the screen coordinates of an actor in 2D space using
 * the actor's current state from the node. It uses the current world matrix
 * and size from the update thread for the specified buffer index.
 *
 * @note This function operates under the following assumptions:
 *       - 2D space only (Z coordinates are always treated as 0)
 *       - No camera rotation (camera always faces +Z direction)
 *       - No perspective projection (orthographic projection or simple parallel translation)
 *       - Uses actor's current node state for the specified buffer index
 *
 * @param[in] actor The actor to calculate the screen position for
 * @param[in] bufferIndex The current event buffer index for accessing current node state
 * @return Vector2 The calculated screen coordinates. Returns (0,0) if actor is not on scene
 */
const Vector2 CalculateCurrentActorScreenPosition(const Actor& actor, BufferIndex bufferIndex);

/**
 * Calculate the screen extents of the actor from its transform, anchor point and size
 *
 * This function calculates the bounding box screen coordinates and size of an actor
 * in 2D space using the actor's target (goal) state properties. It transforms the
 * 4 corner points of the actor and determines the area from their minimum/maximum
 * coordinates. The world transform is calculated from scratch using only event-side
 * properties and does not rely on the update thread.
 *
 * @note This function operates under the following assumptions:
 *       - 2D space only (Z coordinates are always treated as 0)
 *       - No camera rotation (camera always faces +Z direction)
 *       - No perspective projection (orthographic projection or simple parallel translation)
 *       - Uses actor's target size and target transform properties
 *       - Accurate bounding box calculation considering actor rotation
 *
 * @param[in] actor The actor to calculate the screen extents for
 * @return Rect<> The calculated screen extents (x, y, width, height). Returns (0,0,0,0) if actor is not on scene
 */
Rect<> CalculateActorScreenExtents(const Actor& actor);

/**
 * Calculate the current screen extents of the actor from its node transform, anchor point and size
 *
 * This function calculates the bounding box screen coordinates and size of an actor
 * in 2D space using the actor's current state from the node. It transforms the
 * 4 corner points of the actor and determines the area from their minimum/maximum
 * coordinates using the current world matrix and size from the update thread
 * for the specified buffer index.
 *
 * @note This function operates under the following assumptions:
 *       - 2D space only (Z coordinates are always treated as 0)
 *       - No camera rotation (camera always faces +Z direction)
 *       - No perspective projection (orthographic projection or simple parallel translation)
 *       - Uses actor's current node state for the specified buffer index
 *       - Accurate bounding box calculation considering actor rotation
 *
 * @param[in] actor The actor to calculate the screen extents for
 * @param[in] bufferIndex The current event buffer index for accessing current node state
 * @return Rect<> The calculated screen extents (x, y, width, height). Returns (0,0,0,0) if actor is not on scene
 */
Rect<> CalculateCurrentActorScreenExtents(const Actor& actor, BufferIndex bufferIndex);

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
 * Calculate the screen position of the actor from it's transform and anchor point.
 * It will consider rendertasklist.
 *
 * @param[in] actor The actor
 * @return the screen position of the actor consider camera.
 */
const Vector2 CalculateActorScreenPositionRenderTaskList(const Actor& actor);

/**
 * Calculate the screen position of the actor from it's node transform and anchor point.
 * It will consider rendertasklist.
 *
 * @param[in] actor The actor
 * @param[in] bufferIndex The current event buffer index
 * @return the screen position of the actor consider camera.
 */
const Vector2 CalculateCurrentActorScreenPositionRenderTaskList(const Actor& actor, BufferIndex bufferIndex);

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
 * Calculate the screen extents of the actor from it's transform, anchor point and size.
 * It will consider rendertasklist.
 *
 * @param[in] actor The actor
 * @return the screen extents of the actor consider camera.
 */
Rect<> CalculateActorScreenExtentsRenderTaskList(const Actor& actor);

/**
 * Calculate the screen extents of the actor from it's node transform, anchor point and size.
 * It will consider rendertasklist.
 *
 * @param[in] actor The actor
 * @param[in] bufferIndex The current event buffer index
 * @return the screen extents of the actor consider camera.
 */
Rect<> CalculateCurrentActorScreenExtentsRenderTaskList(const Actor& actor, BufferIndex bufferIndex);

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
 * Get the inherited visibility of the actor.
 *
 * This calcualtes the visible of the actor from scratch using
 * only event side properties. It does not rely on the update thread
 * to have already calculated the visible.
 *
 * @param[in] actor The actor to calculate the inherited visible for
 * @return the inherited visible
 */
bool CalculateActorInheritedVisible(const Actor& actor);

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
