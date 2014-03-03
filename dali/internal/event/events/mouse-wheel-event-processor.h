#ifndef __DALI_INTERNAL_MOUSE_WHEEL_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_MOUSE_WHEEL_EVENT_PROCESSOR_H__

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

namespace Dali
{

namespace Integration
{
struct MouseWheelEvent;
}

namespace Internal
{

class Stage;

/**
 *  MouseWheelEventProcessor receives the mouse wheel events from the Dali Event processor.
 *
 *  When a mouse wheel event is received the MouseWheelEvent Processor emits the MouseWheeled signal
 *  on the hit actor (and its parents).
 *
 * - Hit Testing is described in Dali::Actor.
 */
class MouseWheelEventProcessor
{
public:

  /**
   * Create a Mouse wheel event processor.
   */
  MouseWheelEventProcessor(Stage& stage);

  /**
   * Non-virtual destructor; MouseWheelEventProcessor is not a base class
   */
  ~MouseWheelEventProcessor();

  /**
   * This function is called by the event processor whenever a nouse wheel event occurs.
   * @param[in] event The mouse wheel event that has occurred.
   */
  void ProcessMouseWheelEvent(const Integration::MouseWheelEvent& event);

private:

  // Undefined
  MouseWheelEventProcessor(const MouseWheelEventProcessor&);

  // Undefined
  MouseWheelEventProcessor& operator=(const MouseWheelEventProcessor& rhs);

private:

  Stage& mStage;               ///< Used to deliver the mouse wheel events
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_MOUSE_WHEEL_EVENT_PROCESSOR_H__
