/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/touch-point.h>

namespace Dali
{

TouchPoint::TouchPoint(int32_t id, State state, float screenX, float screenY)
: deviceId(id),
  state(state),
  local(screenX, screenY),
  screen(screenX, screenY)
{
}

TouchPoint::TouchPoint(int32_t id, State state, float screenX, float screenY, float localX, float localY)
: deviceId(id),
  state(state),
  local(localX, localY),
  screen(screenX, screenY)
{
}

TouchPoint::~TouchPoint()
{
}

} // namespace Dali
