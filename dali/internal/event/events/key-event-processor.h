#ifndef __DALI_INTERNAL_KEY_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_KEY_EVENT_PROCESSOR_H__

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
struct KeyEvent;
}

namespace Internal
{

class Stage;

/**
 *  KeyEventProcessor receives the filtered key events from the Dali Event processor.
 *
 *  When a key event is received the KeyEvent Processor queries the Stage to get the actor in focus.
 *  Any actor can be set by the actor api to be the focus of key events.  The actor is then sent the key event.
 *  If no actor is set for focus then the key event is discarded.
 */
class KeyEventProcessor
{
public:

  /**
   * Create a Key event processor.
   */
  KeyEventProcessor(Stage& stage);

  /**
   * Non-virtual destructor; KeyEventProcessor is not a base class
   */
  ~KeyEventProcessor();

  /**
   * This function is called by the event processor whenever a key event occurs.
   * @param[in] event The key event that has occurred.
   */
  void ProcessKeyEvent(const Integration::KeyEvent& event);

private:

  // Undefined
  KeyEventProcessor(const KeyEventProcessor&);

  // Undefined
  KeyEventProcessor& operator=(const KeyEventProcessor& rhs);

private:

  Stage& mStage; ///< Used to deliver touch events

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_KEY_EVENT_PROCESSOR_H__
