//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/integration-api/events/mouse-wheel-event-integ.h>

namespace Dali
{

namespace Integration
{

MouseWheelEvent::MouseWheelEvent()
: Event(MouseWheel),
  direction(0),
  modifiers(0),
  point(Vector2::ZERO),
  z(0),
  timeStamp(0)
{
}

MouseWheelEvent::MouseWheelEvent(int direction, unsigned int modifiers, Vector2 point, int z, unsigned int timeStamp)
: Event(MouseWheel),
  direction(direction),
  modifiers(modifiers),
  point(point),
  z(z),
  timeStamp(timeStamp)
{
}

MouseWheelEvent::~MouseWheelEvent()
{
}

} // namespace Integration

} // namespace Dali
