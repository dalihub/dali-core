#ifndef DALI_CAMERA_ACTOR_DEVEL_H
#define DALI_CAMERA_ACTOR_DEVEL_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/actors/camera-actor.h>

namespace Dali
{
namespace DevelCameraActor
{
/**
 * @brief Enumeration for projection direction.
 */
enum ProjectionDirection
{
  VERTICAL,   ///< Field of view direction based on vertial.
  HORIZONTAL, ///< Field of view direction based on horizontal.
};

namespace Property
{
enum
{
  /**
   * @brief Orthographic Size of this camera.
   * If ProjectionDirection is Vertical, OrthographicSize is height/2 of viewing cube of Orthographic projection.
   * If ProjectionDirection is Horizontal, OrthographicSize is width/2 of viewing cube of Orthographic projection.
   * Remained Width or Height of viewing cube is internally computed by using aspect ratio.
   *
   * @details Name "orthographicSize", type Property::FLOAT, animatable, constraint-input
   * @note Default is 400.0f
   */
  ORTHOGRAPHIC_SIZE = CameraActor::Property::INVERT_Y_AXIS + 1,

  /**
   * @brief Determine basic direction of projection relative properties.
   * It will be used when we need to calculate some values relative with aspect ratio automatically.
   *
   * For example, if aspect ratio is 4:3 and set fieldOfView as 60 degree.
   *  - ProjectionDirection::VERTICAL : basic direction is vertical. so, FoV of horizontal direction become ~75.2 degree
   *  - ProjectionDirection::HORIZONTAL : basic direction is horizontal. so, FoV of vertical direction become ~46.8 degree
   *
   * Another example, if aspect ratio is 4:3 and set orthographicSize as 400.0f.
   *  - ProjectionDirection::VERTICAL : basic direction is vertical. so, topPlaneDistance become 400.0f, rightPlaneDistance value become 533.3f.
   *  - ProjectionDirection::HORIZONTAL : basic direction is horizontal. so, topPlaneDistance become 300.0f, rightPlaneDistance value become 400.0f.
   *
   * @details Type Property::INT
   * @note This property doesn't change FieldOfView and OrthographicSize value automatically. So result scene might be changed.
   * @note Default is ProjectionDirection::VERTICAL.
   */
  PROJECTION_DIRECTION,

  /**
   * @brief Defines reflection plane for the camera
   * @details Type Property::VECTOR4
   * @note Optional
   */
  REFLECTION_PLANE,
};

} // Namespace Property

/**
 * @brief Request to rotate window after MVP matrix is multiplied.
 * It is used in case that the target buffer orientation is different from the window orientation.
 * @param [in] camera Dali::CameraActor that will be rotated.
 * @param [in] rotationAngle The rotation angle (0, 90, 180, and 270. See Dali::WindowOrientation.)
 */
DALI_CORE_API void RotateProjection(Dali::CameraActor camera, int32_t rotationAngle);

} // namespace DevelCameraActor
} // Namespace Dali

#endif // DALI_CAMERA_ACTOR_DEVEL_H
