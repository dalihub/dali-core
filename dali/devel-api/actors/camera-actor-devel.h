#ifndef DALI_CAMERA_ACTOR_DEVEL_H
#define DALI_CAMERA_ACTOR_DEVEL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
namespace Property
{
enum
{
  /**
   * @brief Defines reflection plane for the camera
   * @details Type Property::VECTOR4
   * @note Optional
   */
  REFLECTION_PLANE = CameraActor::Property::INVERT_Y_AXIS + 1
};

} // Namespace Property

/**
 * @brief Request to rotate window after MVP matrix is multiplied.
 * It is used in case that the target buffer direction is different from the window direction.
 * @param [in] camera Dali::CameraActor that will be rotated.
 * @param [in] rotationAngle The rotation angle (0, 90, 180, and 270. Note Dali::WindowOrientation.)
 */
DALI_CORE_API void RotateProjection(Dali::CameraActor camera, int32_t rotationAngle);
} // namespace DevelCameraActor
} // Namespace Dali

#endif // DALI_CAMERA_ACTOR_DEVEL_H
