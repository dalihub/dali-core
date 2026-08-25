#ifndef DALI_INTERNAL_TOUCH_RECIPIENT_DISPATCHER_H
#define DALI_INTERNAL_TOUCH_RECIPIENT_DISPATCHER_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/events/point-state.h>

namespace Dali
{
class TouchEvent;

namespace Internal
{
class Actor;
class RenderTask;

/**
 * Performs the mechanical signal dispatch for one touch recipient.
 */
class TouchRecipientDispatcher
{
public:
  static bool DispatchParentTouch(Actor& actor, const Dali::TouchEvent& touchEvent);
  static bool DispatchParentIntercept(Actor& actor, const Dali::TouchEvent& touchEvent);

  static bool IsGeometryTouchDispatchable(const Actor& actor, const Dali::TouchEvent& touchEvent);

  /**
   * @param[in] primaryState The state the primary point will carry once it reaches the recipient,
   *                         which is not always the state it has in touchEvent.
   */
  static bool IsGeometryTouchDispatchable(const Actor& actor, const Dali::TouchEvent& touchEvent, PointState::Type primaryState);

  static bool IsGeometryInterceptDispatchable(const Actor& actor, const Dali::TouchEvent& touchEvent);

  static bool DispatchGeometryTouch(Actor&                  actor,
                                    RenderTask*             renderTask,
                                    const Dali::TouchEvent& sourceEvent,
                                    Actor*                  initialHitActor,
                                    PointState::Type        primaryState);
  static bool DispatchGeometryIntercept(Actor&                  actor,
                                        RenderTask*             renderTask,
                                        const Dali::TouchEvent& sourceEvent,
                                        Actor*                  initialHitActor);
};

} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_TOUCH_RECIPIENT_DISPATCHER_H
