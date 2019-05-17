#ifndef DALI_INTERNAL_KEY_EVENT_PROCESSOR_H
#define DALI_INTERNAL_KEY_EVENT_PROCESSOR_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <vector>

namespace Dali
{

namespace Integration
{
struct KeyEvent;
}

namespace Internal
{

class Scene;
class Stage;

/**
 *  KeyEventProcessor receives the filtered key events from the Dali Event processor.
 *
 *  When a key event is received the KeyEventProcessor queues the event in the scene events queue.
 *  Any actor can be set by the actor api to be the focus of key events.  The actor is then sent the key event.
 *  If no actor is set for focus then the key event is discarded.
 */
class KeyEventProcessor
{
public:

  /**
   * Create a Key event processor.
   * @param[in] scene The scene the event processor belongs to.
   */
  KeyEventProcessor( Scene& scene );

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

  Scene& mScene; ///< Used to deliver key events
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_KEY_EVENT_PROCESSOR_H
