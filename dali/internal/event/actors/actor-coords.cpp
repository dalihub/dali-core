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
 */

#include <dali/internal/common/matrix-utils.h>
#include <dali/internal/event/actors/actor-coords.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali::Internal
{
namespace
{
/**
 * @brief Get the Viewport Extents from RenderTask
 *
 * @param[in] renderTask RenderTask what we want to get viewport.
 * @param[out] viewportExtent Calculated extent by renderTask.
 * @return True if we success to get viewports. False otherwise
 */
bool GetViewportExtentsFromRenderTask(const RenderTask& renderTask, Rect<float>& viewportExtent)
{
  if(renderTask.GetFrameBuffer())
  {
    Dali::Actor mappingActor = renderTask.GetScreenToFrameBufferMappingActor();
    if(mappingActor)
    {
      // NOTE : We will assume that mapping actor always use default camera.
      Vector2 screenPosition    = mappingActor.GetProperty<Vector2>(Dali::Actor::Property::SCREEN_POSITION);
      Vector3 size              = mappingActor.GetCurrentProperty<Vector3>(Dali::Actor::Property::SIZE) * mappingActor.GetCurrentProperty<Vector3>(Dali::Actor::Property::WORLD_SCALE);
      Vector3 anchorPointOffSet = size * GetImplementation(mappingActor).GetAnchorPointForPosition();
      Vector2 position          = Vector2(screenPosition.x - anchorPointOffSet.x, screenPosition.y - anchorPointOffSet.y);
      viewportExtent.x          = position.x;
      viewportExtent.y          = position.y;
      viewportExtent.width      = size.x;
      viewportExtent.height     = size.y;
    }
    else
    {
      return false;
    }
  }
  else
  {
    Viewport viewport;
    renderTask.GetViewport(viewport);
    viewportExtent.x      = viewport.x;
    viewportExtent.y      = viewport.y;
    viewportExtent.width  = viewport.width;
    viewportExtent.height = viewport.height;
  }
  return true;
}

/**
 * @brief Get the Orientation from Forward vector and Up vector
 * If vectors are valid, return Quaternion to make forward direction as +Z, and up direction near as -Y axis.
 * If some invalid vector inputed (like Zero length, or parallel vector), return identity quaternion
 *
 * @param[in] forward The vector that want to be +Z axis.
 * @param[in] up The vector that want to be -Y axis.
 * @return Quaternion to make forward direction as +Z, and up direction near as -Y axis.
 */
Quaternion GetOrientationFromForwardAndUpVector(Vector3 forward, Vector3 up)
{
  Vector3 vZ = forward;
  vZ.Normalize();

  Vector3 vX = up.Cross(vZ);
  vX.Normalize();

  // If something invalid input comes, vX length become zero.
  if(DALI_UNLIKELY(Dali::EqualsZero(vX.Length())))
  {
    DALI_LOG_ERROR("Invalid value inputed, forward : %f %f %f ,  up : %f %f %f\n", forward.x, forward.y, forward.z, up.x, up.y, up.z);
    return Quaternion();
  }

  Vector3 vY = vZ.Cross(vX);
  vY.Normalize();

  return Quaternion(vX, vY, vZ);
}

// Common API for calculating screen position

/**
 * @brief Retrieve actor's world position by Event related properties after calculating the world transform matrix.
 *
 * @param[in] actor The actor that wants to get WorldPosition
 * @param[out] worldTransformMatrix Calculated world matrix as output. We can reuse this value in other logics.
 * @return Calculated world position
 */
Vector3 RetrieveCalculatedWorldPosition(const Actor& actor, Matrix& worldTransformMatrix)
{
  worldTransformMatrix = Dali::Internal::CalculateActorWorldTransform(actor);

  Scene& scene = actor.GetScene();

  Vector3 worldPosition  = worldTransformMatrix.GetTranslation3();
  Vector3 cameraPosition = Dali::Internal::CalculateActorWorldTransform(scene.GetDefaultCameraActor()).GetTranslation3();
  worldPosition -= cameraPosition;

  return worldPosition;
}

/**
 * @brief Calculate actor's current world position by Update related properties.
 *
 * @param[in] actor The actor that wants to get WorldPosition
 * @param[in] bufferIndex Buffer index of update
 * @return Calculated world position
 */
Vector3 CalculateCurrentWorldPosition(const Actor& actor, BufferIndex bufferIndex)
{
  Scene& scene = actor.GetScene();

  Vector3 worldPosition  = actor.GetNode().GetWorldPosition(bufferIndex);
  Vector3 cameraPosition = scene.GetDefaultCameraActor().GetNode().GetWorldPosition(bufferIndex);
  worldPosition -= cameraPosition;

  return worldPosition;
}

/**
 * @brief Calculate actor's scaled size by world scale.
 *
 * @param[in] actor The actor that wants to get scaled size
 * @param[in] worldTransformMatrix The actor's world matrix
 * @return The size scaled by world scale
 */
Vector3 CalculateScaledActorSize(const Actor& actor, const Matrix& worldTransformMatrix)
{
  return actor.GetTargetSize() * worldTransformMatrix.GetScale();
}

/**
 * @brief Calculate actor's current scaled size by world scale.
 *
 * @param[in] actor The actor that wants to get scaled size
 * @param[in] bufferIndex Buffer index of update
 * @return The size scaled by world scale
 */
Vector3 CalculateCurrentScaledActorSize(const Actor& actor, BufferIndex bufferIndex)
{
  auto& node = actor.GetNode();
  return node.GetSize(bufferIndex) * node.GetWorldScale(bufferIndex);
}

/**
 * @brief Calculate screen extents top-left point position
 *
 * @param[in] sceneSize The scene size
 * @param[in] actorSize The actor size
 * @param[in] worldPosition The actor's world position
 * @return The size scaled by world scale
 */
template<typename SceneSizeType, typename ActorSizeType, typename WorldPositionType>
Vector2 CalculateActorTopLeftScreenPosition(const SceneSizeType& sceneSize, const ActorSizeType& actorSize, const WorldPositionType& worldPosition)
{
  const Vector2 halfSceneSize(sceneSize.width * 0.5f, sceneSize.height * 0.5f);
  const Vector2 halfActorSize(actorSize.width * 0.5f, actorSize.height * 0.5f);

  return Vector2(halfSceneSize.width - halfActorSize.width + worldPosition.x, halfSceneSize.height - halfActorSize.height + worldPosition.y);
}
} // namespace

bool ConvertScreenToLocal(
  const Matrix&   viewMatrix,
  const Matrix&   projectionMatrix,
  const Matrix&   worldMatrix,
  const Vector3&  currentSize,
  const Viewport& viewport,
  float&          localX,
  float&          localY,
  float           screenX,
  float           screenY)
{
  // Get the ModelView matrix
  Matrix modelView;
  MatrixUtils::MultiplyTransformMatrix(modelView, worldMatrix, viewMatrix);

  // Calculate the inverted ModelViewProjection matrix; this will be used for 2 unprojects
  Matrix invertedMvp(false /*don't init*/);
  MatrixUtils::MultiplyProjectionMatrix(invertedMvp, modelView, projectionMatrix);
  bool success = invertedMvp.Invert();

  // Convert to GL coordinates
  Vector4 screenPos(screenX - static_cast<float>(viewport.x), static_cast<float>(viewport.height) - screenY - static_cast<float>(viewport.y), 0.f, 1.f);

  Vector4 nearPos;
  if(success)
  {
    success = Unproject(screenPos, invertedMvp, static_cast<float>(viewport.width), static_cast<float>(viewport.height), nearPos);
  }

  Vector4 farPos;
  if(success)
  {
    screenPos.z = 1.0f;
    success     = Unproject(screenPos, invertedMvp, static_cast<float>(viewport.width), static_cast<float>(viewport.height), farPos);
  }

  if(success)
  {
    Vector4 local;
    if(XyPlaneIntersect(nearPos, farPos, local))
    {
      Vector3 size = currentSize;
      localX       = local.x + size.x * 0.5f;
      localY       = local.y + size.y * 0.5f;
    }
    else
    {
      success = false;
    }
  }

  return success;
}

bool ConvertScreenToLocalRenderTask(
  const RenderTask& renderTask,
  const Matrix&     worldMatrix,
  const Vector3&    currentSize,
  float&            localX,
  float&            localY,
  float             screenX,
  float             screenY)
{
  bool         success = false;
  CameraActor* camera  = renderTask.GetCameraActor();
  if(camera)
  {
    Viewport viewport;
    renderTask.GetViewport(viewport);

    // need to translate coordinates to render tasks coordinate space
    Vector2 converted(screenX, screenY);
    if(renderTask.TranslateCoordinates(converted))
    {
      success = ConvertScreenToLocal(camera->GetViewMatrix(), camera->GetProjectionMatrix(), worldMatrix, currentSize, viewport, localX, localY, converted.x, converted.y);
    }
  }
  return success;
}

bool ConvertScreenToLocalRenderTaskList(
  const RenderTaskList& renderTaskList,
  const Matrix&         worldMatrix,
  const Vector3&        currentSize,
  float&                localX,
  float&                localY,
  float                 screenX,
  float                 screenY)
{
  // do a reverse traversal of all lists (as the default onscreen one is typically the last one)
  uint32_t taskCount = renderTaskList.GetTaskCount();
  for(uint32_t i = taskCount; i > 0; --i)
  {
    RenderTaskPtr task = renderTaskList.GetTask(i - 1);
    if(ConvertScreenToLocalRenderTask(*task, worldMatrix, currentSize, localX, localY, screenX, screenY))
    {
      // found a task where this conversion was ok so return
      return true;
    }
  }
  return false;
}

const Vector2 CalculateActorScreenPosition(const Actor& actor)
{
  Vector2 result;
  if(actor.OnScene())
  {
    Matrix worldTransformMatrix(false); // Do not initialize. It will be calculated in RetrieveCalculatedWorldPosition API.

    Vector3 worldPosition = RetrieveCalculatedWorldPosition(actor, worldTransformMatrix);
    Vector3 actorSize     = CalculateScaledActorSize(actor, worldTransformMatrix);

    const auto& sceneSize = actor.GetScene().GetSize();

    Vector2 screenPositionTopLeft = CalculateActorTopLeftScreenPosition(sceneSize, actorSize, worldPosition);
    Vector2 anchorPointOffSet     = (actorSize * actor.GetAnchorPointForPosition()).GetVectorXY();

    result = screenPositionTopLeft + anchorPointOffSet;
  }
  return result;
}

const Vector2 CalculateCurrentActorScreenPosition(const Actor& actor, BufferIndex bufferIndex)
{
  Vector2 result;
  if(actor.OnScene())
  {
    Vector3 worldPosition = CalculateCurrentWorldPosition(actor, bufferIndex);
    Vector3 actorSize     = CalculateCurrentScaledActorSize(actor, bufferIndex);

    const auto& sceneSize = actor.GetScene().GetCurrentSurfaceRect(); // Use the update object's size

    Vector2 screenPositionTopLeft = CalculateActorTopLeftScreenPosition(sceneSize, actorSize, worldPosition);
    Vector2 anchorPointOffSet     = (actorSize * actor.GetAnchorPointForPosition()).GetVectorXY();

    result = screenPositionTopLeft + anchorPointOffSet;
  }
  return result;
}

Rect<> CalculateActorScreenExtents(const Actor& actor)
{
  Vector2 position2;
  Vector2 size2;
  if(actor.OnScene())
  {
    Matrix worldTransformMatrix(false); // Do not initialize. It will be calculated in RetrieveCalculatedWorldPosition API.

    Vector3 worldPosition = RetrieveCalculatedWorldPosition(actor, worldTransformMatrix);
    Vector3 actorSize     = CalculateScaledActorSize(actor, worldTransformMatrix);

    const auto& sceneSize = actor.GetScene().GetSize();

    position2 = CalculateActorTopLeftScreenPosition(sceneSize, actorSize, worldPosition);
    size2     = Vector2(actorSize.width, actorSize.height);
  }
  return {position2.x, position2.y, size2.x, size2.y};
}

Rect<> CalculateCurrentActorScreenExtents(const Actor& actor, BufferIndex bufferIndex)
{
  Vector2 position2;
  Vector2 size2;
  if(actor.OnScene())
  {
    Vector3 worldPosition = CalculateCurrentWorldPosition(actor, bufferIndex);
    Vector3 actorSize     = CalculateCurrentScaledActorSize(actor, bufferIndex);

    const auto& sceneSize = actor.GetScene().GetCurrentSurfaceRect(); // Use the update object's size

    position2 = CalculateActorTopLeftScreenPosition(sceneSize, actorSize, worldPosition);
    size2     = Vector2(actorSize.width, actorSize.height);
  }
  return {position2.x, position2.y, size2.x, size2.y};
}

bool ConvertLocalToScreen(
  const Matrix&  viewMatrix,
  const Matrix&  projectionMatrix,
  const Matrix&  worldMatrix,
  const Rect<>&  viewportExtent,
  const Vector3& localPosition,
  float&         screenX,
  float&         screenY)
{
  bool success = false;

  // Convert local to projection coordinates
  // note, P*(V*(M*pos))) is faster than (P*V*M)*pos
  Vector4 mvpPos(localPosition.x, localPosition.y, localPosition.z, 1.0f);

  mvpPos = worldMatrix * mvpPos;
  mvpPos = viewMatrix * mvpPos;
  mvpPos = projectionMatrix * mvpPos;

  if(DALI_LIKELY(!EqualsZero(mvpPos.w)))
  {
    success = true;
    screenX = viewportExtent.x + (mvpPos.x + mvpPos.w) * viewportExtent.width * 0.5f / mvpPos.w;
    screenY = viewportExtent.y + (-mvpPos.y + mvpPos.w) * viewportExtent.height * 0.5f / mvpPos.w;
  }
  return success;
}

bool ConvertLocalToScreenRenderTask(
  const RenderTask& renderTask,
  const Actor&      actor,
  const Matrix&     worldMatrix,
  const Vector3&    localPosition,
  float&            screenX,
  float&            screenY)
{
  bool         success     = false;
  const Actor* sourceActor = renderTask.GetSourceActor();
  if(sourceActor == nullptr)
  {
    return success;
  }

  // Check whether current actor is in this rendertask.
  bool         actorInRendertask = false;
  const Actor* targetActor       = &actor;
  while(targetActor)
  {
    if(sourceActor == targetActor)
    {
      actorInRendertask = true;
      break;
    }
    targetActor = targetActor->GetParent();
  }
  if(!actorInRendertask)
  {
    return success;
  }

  CameraActor* camera = renderTask.GetCameraActor();
  if(camera)
  {
    Rect<float> viewportExtent = {0.f, 0.f, 0.f, 0.f};
    if(!GetViewportExtentsFromRenderTask(renderTask, viewportExtent))
    {
      return success;
    }

    if(ConvertLocalToScreen(camera->GetViewMatrix(), camera->GetProjectionMatrix(), worldMatrix, viewportExtent, localPosition, screenX, screenY))
    {
      success = true;
    }
  }
  return success;
}

bool ConvertLocalToScreenRenderTaskList(
  const RenderTaskList& renderTaskList,
  const Actor&          actor,
  const Matrix&         worldMatrix,
  const Vector3&        localPosition,
  float&                screenX,
  float&                screenY)
{
  // do a reverse traversal of all lists (as the default onscreen one is typically the last one)
  uint32_t taskCount = renderTaskList.GetTaskCount();
  for(uint32_t i = taskCount; i > 0; --i)
  {
    RenderTaskPtr task = renderTaskList.GetTask(i - 1);
    if(ConvertLocalToScreenRenderTask(*task, actor, worldMatrix, localPosition, screenX, screenY))
    {
      // found a task where this conversion was ok so return
      return true;
    }
  }
  return false;
}

const Vector2 CalculateActorScreenPositionRenderTaskList(const Actor& actor)
{
  Vector2 result;
  if(actor.OnScene())
  {
    auto        worldMatrix    = Dali::Internal::CalculateActorWorldTransform(actor);
    const auto& renderTaskList = actor.GetScene().GetRenderTaskList();

    ConvertLocalToScreenRenderTaskList(renderTaskList, actor, worldMatrix, actor.GetTargetSize() * (actor.GetAnchorPointForPosition() - Vector3(0.5f, 0.5f, 0.5f)), result.x, result.y);
  }
  return result;
}

const Vector2 CalculateCurrentActorScreenPositionRenderTaskList(const Actor& actor, BufferIndex bufferIndex)
{
  Vector2 result;
  if(actor.OnScene())
  {
    const auto& node           = actor.GetNode();
    const auto& worldMatrix    = node.GetWorldMatrix(bufferIndex);
    const auto& renderTaskList = actor.GetScene().GetRenderTaskList();

    ConvertLocalToScreenRenderTaskList(renderTaskList, actor, worldMatrix, node.GetSize(bufferIndex) * (actor.GetAnchorPointForPosition() - Vector3(0.5f, 0.5f, 0.5f)), result.x, result.y);
  }
  return result;
}

bool ConvertLocalToScreenExtentRenderTask(
  const RenderTask& renderTask,
  const Actor&      actor,
  const Matrix&     worldMatrix,
  const Vector3&    currentSize,
  Rect<>&           screenExtent)
{
  bool         success     = false;
  const Actor* sourceActor = renderTask.GetSourceActor();
  if(sourceActor == nullptr)
  {
    return success;
  }

  // Check whether current actor is in this rendertask.
  bool         actorInRendertask = false;
  const Actor* targetActor       = &actor;
  while(targetActor)
  {
    if(sourceActor == targetActor)
    {
      actorInRendertask = true;
      break;
    }
    targetActor = targetActor->GetParent();
  }
  if(!actorInRendertask)
  {
    return success;
  }

  CameraActor* camera = renderTask.GetCameraActor();
  if(camera)
  {
    Rect<float> viewportExtent = {0.f, 0.f, 0.f, 0.f};
    if(!GetViewportExtentsFromRenderTask(renderTask, viewportExtent))
    {
      return success;
    }

    constexpr uint32_t BOX_POINT_COUNT           = 8;
    const Vector3      BBOffset[BOX_POINT_COUNT] = {
      Vector3(-currentSize.width * 0.5f, -currentSize.height * 0.5f, -currentSize.depth * 0.5f),
      Vector3(-currentSize.width * 0.5f, currentSize.height * 0.5f, -currentSize.depth * 0.5f),
      Vector3(currentSize.width * 0.5f, -currentSize.height * 0.5f, -currentSize.depth * 0.5f),
      Vector3(currentSize.width * 0.5f, currentSize.height * 0.5f, -currentSize.depth * 0.5f),
      Vector3(-currentSize.width * 0.5f, -currentSize.height * 0.5f, currentSize.depth * 0.5f),
      Vector3(-currentSize.width * 0.5f, currentSize.height * 0.5f, currentSize.depth * 0.5f),
      Vector3(currentSize.width * 0.5f, -currentSize.height * 0.5f, currentSize.depth * 0.5f),
      Vector3(currentSize.width * 0.5f, currentSize.height * 0.5f, currentSize.depth * 0.5f),
    };

    float minScreenX = 0.0f;
    float minScreenY = 0.0f;
    float maxScreenX = 0.0f;
    float maxScreenY = 0.0f;

    const auto& viewMatrix       = camera->GetViewMatrix();
    const auto& projectionMatrix = camera->GetProjectionMatrix();

    success = ConvertLocalToScreen(viewMatrix, projectionMatrix, worldMatrix, viewportExtent, BBOffset[0], minScreenX, minScreenY);
    if(success)
    {
      maxScreenX = minScreenX;
      maxScreenY = minScreenY;
      for(uint32_t i = 1; i < BOX_POINT_COUNT; ++i)
      {
        float   screenX       = 0.0f;
        float   screenY       = 0.0f;
        Vector3 localPosition = BBOffset[i];
        if(DALI_UNLIKELY(!ConvertLocalToScreen(viewMatrix, projectionMatrix, worldMatrix, viewportExtent, localPosition, screenX, screenY)))
        {
          success = false;
          break;
        }
        minScreenX = std::min(minScreenX, screenX);
        maxScreenX = std::max(maxScreenX, screenX);
        minScreenY = std::min(minScreenY, screenY);
        maxScreenY = std::max(maxScreenY, screenY);
      }
      if(success)
      {
        screenExtent.x      = minScreenX;
        screenExtent.y      = minScreenY;
        screenExtent.width  = maxScreenX - minScreenX;
        screenExtent.height = maxScreenY - minScreenY;
      }
    }
  }
  return success;
}

bool ConvertLocalToScreenExtentRenderTaskList(
  const RenderTaskList& renderTaskList,
  const Actor&          actor,
  const Matrix&         worldMatrix,
  const Vector3&        currentSize,
  Rect<>&               screenExtent)
{
  // do a reverse traversal of all lists (as the default onscreen one is typically the last one)
  uint32_t taskCount = renderTaskList.GetTaskCount();
  for(uint32_t i = taskCount; i > 0; --i)
  {
    RenderTaskPtr task = renderTaskList.GetTask(i - 1);
    if(ConvertLocalToScreenExtentRenderTask(*task, actor, worldMatrix, currentSize, screenExtent))
    {
      // found a task where this conversion was ok so return
      return true;
    }
  }
  return false;
}

Rect<> CalculateActorScreenExtentsRenderTaskList(const Actor& actor)
{
  Rect<> result = {0.0f, 0.0f, 0.0f, 0.0f};

  if(actor.OnScene())
  {
    Scene& scene = actor.GetScene();

    auto        worldMatrix    = Dali::Internal::CalculateActorWorldTransform(actor);
    const auto& renderTaskList = scene.GetRenderTaskList();
    ConvertLocalToScreenExtentRenderTaskList(renderTaskList, actor, worldMatrix, actor.GetTargetSize(), result);
  }
  return result;
}

Rect<> CalculateCurrentActorScreenExtentsRenderTaskList(const Actor& actor, BufferIndex bufferIndex)
{
  Rect<> result = {0.0f, 0.0f, 0.0f, 0.0f};

  if(actor.OnScene())
  {
    const auto& node           = actor.GetNode();
    const auto& worldMatrix    = node.GetWorldMatrix(bufferIndex);
    const auto& renderTaskList = actor.GetScene().GetRenderTaskList();

    ConvertLocalToScreenExtentRenderTaskList(renderTaskList, actor, worldMatrix, node.GetSize(bufferIndex), result);
  }
  return result;
}

/**
 * @brief Computes and center position by using transform properties.
 * @param[in] anchorPoint anchorPoint of an actor.
 * @param[in] positionUsesAnchorPoint positionUsesAnchorPoint of an actor.
 * @param[in] size size of an actor.
 * @param[in] scale scale of an actor.
 * @param[in] orientation orientation of an actor.
 */
Vector3 CalculateCenterPosition(
  const Vector3&    anchorPoint,
  const bool        positionUsesAnchorPoint,
  const Vector3&    size,
  const Vector3&    scale,
  const Quaternion& orientation)
{
  Vector3       centerPosition;
  const Vector3 half(0.5f, 0.5f, 0.5f);
  const Vector3 topLeft(0.0f, 0.0f, 0.5f);
  // Calculate the center-point by applying the scale and rotation on the anchor point.
  centerPosition = (half - anchorPoint) * size * scale;
  centerPosition *= orientation;

  // If the position is ignoring the anchor-point, then remove the anchor-point shift from the position.
  if(!positionUsesAnchorPoint)
  {
    centerPosition -= (topLeft - anchorPoint) * size;
  }
  return centerPosition;
}

Matrix CalculateActorWorldTransform(const Actor& actor)
{
  enum InheritanceMode
  {
    DONT_INHERIT_TRANSFORM = 0,
    INHERIT_POSITION       = 1,
    INHERIT_SCALE          = 2,
    INHERIT_ORIENTATION    = 4,
    INHERIT_ALL            = INHERIT_POSITION | INHERIT_SCALE | INHERIT_ORIENTATION,
  };

  std::vector<Dali::Actor>     descentList;
  std::vector<InheritanceMode> inheritanceModeList;
  Dali::Actor                  currentActor(const_cast<Actor*>(&actor));
  int                          inheritance = 0;
  do
  {
    inheritance = (static_cast<int>(currentActor.GetProperty<bool>(Dali::Actor::Property::INHERIT_ORIENTATION)) << 2) +
                  (static_cast<int>(currentActor.GetProperty<bool>(Dali::Actor::Property::INHERIT_SCALE)) << 1) +
                  static_cast<int>(currentActor.GetProperty<bool>(Dali::Actor::Property::INHERIT_POSITION));
    inheritanceModeList.push_back(static_cast<InheritanceMode>(inheritance));
    descentList.push_back(currentActor);
    currentActor = currentActor.GetParent();
  } while(inheritance != DONT_INHERIT_TRANSFORM && currentActor);

  Matrix  worldMatrix;
  Vector3 localPosition;

  // descentList is leaf first, so traverse from root (end) to leaf (beginning)
  const size_t descentCount = descentList.size();
  for(size_t iter = 0u; iter < descentCount; ++iter)
  {
    auto       i                       = descentCount - iter - 1u;
    Vector3    anchorPoint             = descentList[i].GetProperty<Vector3>(Dali::Actor::Property::ANCHOR_POINT);
    Vector3    parentOrigin            = descentList[i].GetProperty<Vector3>(Dali::Actor::Property::PARENT_ORIGIN);
    bool       positionUsesAnchorPoint = descentList[i].GetProperty<bool>(Dali::Actor::Property::POSITION_USES_ANCHOR_POINT);
    Vector3    size                    = descentList[i].GetProperty<Vector3>(Dali::Actor::Property::SIZE);
    Vector3    actorPosition           = descentList[i].GetProperty<Vector3>(Dali::Actor::Property::POSITION);
    Quaternion localOrientation        = descentList[i].GetProperty<Quaternion>(Dali::Actor::Property::ORIENTATION);
    Vector3    localScale              = descentList[i].GetProperty<Vector3>(Dali::Actor::Property::SCALE);

    Vector3 centerPosition = CalculateCenterPosition(anchorPoint, positionUsesAnchorPoint, size, localScale, localOrientation);
    if(inheritanceModeList[i] != DONT_INHERIT_TRANSFORM && descentList[i].GetParent())
    {
      Matrix  localMatrix;
      Vector3 parentSize = descentList[i + 1].GetProperty<Vector3>(Dali::Actor::Property::SIZE);
      if(inheritanceModeList[i] == INHERIT_ALL)
      {
        localPosition = actorPosition + centerPosition + (parentOrigin - Vector3(0.5f, 0.5f, 0.5f)) * parentSize;
        localMatrix.SetTransformComponents(localScale, localOrientation, localPosition);

        //Update the world matrix
        Matrix tempMatrix;
        MatrixUtils::MultiplyTransformMatrix(tempMatrix, localMatrix, worldMatrix);
        worldMatrix = tempMatrix;
      }
      else
      {
        // Get Parent information.
        Vector3       parentPosition, parentScale;
        Quaternion    parentOrientation;
        const Matrix& parentMatrix = worldMatrix;
        parentMatrix.GetTransformComponents(parentPosition, parentOrientation, parentScale);

        // Compute intermediate Local information
        centerPosition                    = CalculateCenterPosition(anchorPoint, positionUsesAnchorPoint, size, localScale, localOrientation);
        Vector3 intermediateLocalPosition = actorPosition + centerPosition + (parentOrigin - Vector3(0.5f, 0.5f, 0.5f)) * parentSize;
        Matrix  intermediateLocalMatrix;
        intermediateLocalMatrix.SetTransformComponents(localScale, localOrientation, intermediateLocalPosition);

        // Compute intermediate world information
        Matrix intermediateWorldMatrix;
        MatrixUtils::MultiplyTransformMatrix(intermediateWorldMatrix, intermediateLocalMatrix, parentMatrix);

        Vector3    intermediateWorldPosition, intermediateWorldScale;
        Quaternion intermediateWorldOrientation;
        intermediateWorldMatrix.GetTransformComponents(intermediateWorldPosition, intermediateWorldOrientation, intermediateWorldScale);

        // Compute final world information
        Vector3    finalWorldPosition    = intermediateWorldPosition;
        Vector3    finalWorldScale       = intermediateWorldScale;
        Quaternion finalWorldOrientation = intermediateWorldOrientation;
        // worldScale includes the influence of local scale, local rotation, and parent scale.
        // So, for the final world matrix, if this node inherits its parent scale, use worldScale.
        // If not, use local scale for the final world matrix.
        if((inheritanceModeList[i] & INHERIT_SCALE) == 0)
        {
          finalWorldScale = localScale;
        }

        // For the final world matrix, if this node inherits its parent orientation, use worldOrientation.
        // If not, use local orientation for the final world matrix.
        if((inheritanceModeList[i] & INHERIT_ORIENTATION) == 0)
        {
          finalWorldOrientation = localOrientation;
        }

        // The final world position of this node is computed as a sum of
        // parent origin position in world space and relative position of center from parent origin.
        // If this node doesn't inherit its parent position, simply use the relative position as a final world position.
        Vector3 localCenterPosition = CalculateCenterPosition(anchorPoint, positionUsesAnchorPoint, size, finalWorldScale, finalWorldOrientation);
        finalWorldPosition          = actorPosition * finalWorldScale;
        finalWorldPosition *= finalWorldOrientation;
        finalWorldPosition += localCenterPosition;
        if((inheritanceModeList[i] & INHERIT_POSITION) != 0)
        {
          Vector4 parentOriginPosition((parentOrigin - Vector3(0.5f, 0.5f, 0.5f)) * parentSize);
          parentOriginPosition.w = 1.0f;
          finalWorldPosition += Vector3(parentMatrix * parentOriginPosition);
        }

        worldMatrix.SetTransformComponents(finalWorldScale, finalWorldOrientation, finalWorldPosition);
      }
    }
    else
    {
      localPosition = actorPosition + centerPosition;
      worldMatrix.SetTransformComponents(localScale, localOrientation, localPosition);
    }
  }

  return worldMatrix;
}

Vector4 CalculateActorWorldColor(const Actor& actor)
{
  std::vector<Dali::Actor>     descentList;
  std::vector<Dali::ColorMode> inheritanceModeList;
  Dali::Actor                  currentActor(const_cast<Actor*>(&actor));
  Dali::ColorMode              inheritance = Dali::ColorMode::USE_OWN_MULTIPLY_PARENT_ALPHA;
  do
  {
    inheritance = currentActor.GetProperty<Dali::ColorMode>(Dali::Actor::Property::COLOR_MODE);
    inheritanceModeList.push_back(inheritance);
    descentList.push_back(currentActor);
    currentActor = currentActor.GetParent();
  } while(inheritance != Dali::ColorMode::USE_OWN_COLOR && currentActor);

  Vector4      worldColor;
  const size_t descentCount = descentList.size();
  for(size_t iter = 0u; iter < descentCount; ++iter)
  {
    auto i = descentCount - iter - 1u;
    if(inheritanceModeList[i] == USE_OWN_COLOR || i == descentList.size() - 1)
    {
      worldColor = descentList[i].GetProperty<Vector4>(Dali::Actor::Property::COLOR);
    }
    else if(inheritanceModeList[i] == USE_OWN_MULTIPLY_PARENT_ALPHA)
    {
      Vector4 ownColor = descentList[i].GetProperty<Vector4>(Dali::Actor::Property::COLOR);
      worldColor       = Vector4(ownColor.r, ownColor.g, ownColor.b, ownColor.a * worldColor.a);
    }
    else if(inheritanceModeList[i] == USE_OWN_MULTIPLY_PARENT_COLOR)
    {
      Vector4 ownColor = descentList[i].GetProperty<Vector4>(Dali::Actor::Property::COLOR);
      worldColor *= ownColor;
    }
  }

  return worldColor;
}

Quaternion CalculateActorLookAtOrientation(const Actor& actor, Vector3 target, Vector3 up, Vector3 localForward, Vector3 localUp)
{
  Vector3 currentWorldPosition = Dali::Internal::CalculateActorWorldTransform(actor).GetTranslation3();

  Quaternion worldToTarget = GetOrientationFromForwardAndUpVector(target - currentWorldPosition, up);
  Quaternion worldToLocal  = GetOrientationFromForwardAndUpVector(localForward, localUp);

  // Rotate by this order : Local --> World --> Target
  Quaternion ret = worldToTarget / worldToLocal;

  // If we inherit orientation, get parent's world orientation, and revert it.
  if(actor.IsOrientationInherited() && actor.GetParent())
  {
    // Get Parent information.
    Vector3       parentPosition, parentScale;
    Quaternion    parentOrientation;
    const Matrix& parentMatrix = Dali::Internal::CalculateActorWorldTransform(*actor.GetParent());
    parentMatrix.GetTransformComponents(parentPosition, parentOrientation, parentScale);

    ret = ret / parentOrientation;
  }

  return ret;
}

} // namespace Dali::Internal
