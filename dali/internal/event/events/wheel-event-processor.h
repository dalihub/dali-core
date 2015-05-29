#ifndef __DALI_INTERNAL_WHEEL_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_WHEEL_EVENT_PROCESSOR_H__

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

namespace Dali
{

namespace Integration
{
struct WheelEvent;
}

namespace Internal
{

class Stage;

/**
 *  WheelEventProcessor receives the wheel events from the Dali Event processor.
 *
 *  When a wheel event is received the WheelEvent Processor emits the Wheeled signal
 *  on the hit actor (and its parents).
 *
 * - Hit Testing is described in Dali::Actor.
 */
class WheelEventProcessor
{
public:

  /**
   * Create a wheel event processor.
   */
  WheelEventProcessor(Stage& stage);

  /**
   * Non-virtual destructor; WheelEventProcessor is not a base class
   */
  ~WheelEventProcessor();

  /**
   * This function is called by the event processor whenever a nouse wheel event occurs.
   * @param[in] event The wheel event that has occurred.
   */
  void ProcessWheelEvent(const Integration::WheelEvent& event);

private:

  // Undefined
  WheelEventProcessor(const WheelEventProcessor&);

  // Undefined
  WheelEventProcessor& operator=(const WheelEventProcessor& rhs);

private:

  Stage& mStage;               ///< Used to deliver the wheel events
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_WHEEL_EVENT_PROCESSOR_H__
