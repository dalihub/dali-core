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

// INTERNAL INCLUDES
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <dali/internal/event/actors/camera-actor-impl.h>

namespace Dali
{
namespace DevelCameraActor
{

void RotateProjection(Dali::CameraActor camera, int32_t rotationAngle)
{
  Dali::GetImplementation(camera).RotateProjection(rotationAngle);
}

} // namespace DevelCustomActor

} // namespace Dali
