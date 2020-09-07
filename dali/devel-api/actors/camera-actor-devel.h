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
} // namespace DevelCameraActor
} // Namespace Dali

#endif // DALI_CAMERA_ACTOR_DEVEL_H
