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

// CLASS HEADER
#include <dali/internal/event/events/tap-gesture/tap-gesture-event.h>

namespace Dali
{

namespace Internal
{

TapGestureEvent::TapGestureEvent( GestureState state )
: GestureEvent(GestureType::TAP, state),
  numberOfTaps(1),
  numberOfTouches(1)
{
}

TapGestureEvent::~TapGestureEvent() = default;

} // namespace Internal

} // namespace Dali
