#ifndef _TEST_TOUCH_UTILS_H_
#define _TEST_TOUCH_UTILS_H_

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/actors/actor.h>

namespace Dali
{

// Data for touch events
struct TouchEventData
{
  TouchEventData()
  : functorCalled(false),
    receivedTouch(),
    touchActor()
  {
  }

  void Reset()
  {
    functorCalled = false;

    receivedTouch.points.clear();
    receivedTouch.time = 0;

    touchActor = NULL;
  }

  bool functorCalled;
  TouchEvent receivedTouch;
  Actor touchActor;
};

// Functor that sets the data when called
struct TouchEventDataFunctor
{
  TouchEventDataFunctor(TouchEventData& data) : touchEventData(data) { }

  bool operator()(Actor actor, const TouchEvent& touch)
  {
    touchEventData.functorCalled = true;
    touchEventData.touchActor = actor;
    touchEventData.receivedTouch = touch;
    return false;
  }

  // Generate a touch-event
  Integration::TouchEvent GenerateSingleTouch( TouchPoint::State state, Vector2 screenPosition ) const
  {
    Integration::TouchEvent touchEvent;
    touchEvent.points.push_back( TouchPoint ( 0, state, screenPosition.x, screenPosition.y ) );
    return touchEvent;
  }

  TouchEventData& touchEventData;
};


} // namespace Dali

#endif //  _TEST_TOUCH_UTILS_H_
