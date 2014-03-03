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
#include <dali/integration-api/events/pan-gesture-event.h>

namespace Dali
{

namespace Integration
{


PanGestureEvent::PanGestureEvent(Gesture::State state)
: GestureEvent(Gesture::Pan, state),
  timeDelta(0),
  numberOfTouches(1)
{
}

PanGestureEvent::~PanGestureEvent()
{
}

} // namespace Integration

} // namespace Dali
