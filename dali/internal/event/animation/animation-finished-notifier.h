#ifndef __DALI_INTERNAL_ANIMATION_FINISHED_NOTIFIER_H__
#define __DALI_INTERNAL_ANIMATION_FINISHED_NOTIFIER_H__

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

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>

namespace Dali
{

namespace Internal
{

/**
 * Interface used by the update-thread to trigger animation "Finished" signals.
 */
class AnimationFinishedNotifier
{
public:

  /**
   * Virtual destructor.
   */
  virtual ~AnimationFinishedNotifier()
  {
  }

  /**
   * Provide notification signals for any "Finished" animations.
   * This method should be called in the event-thread; the update-thread must use AnimationFinishedMessage.
   */
  virtual void NotifyFinishedAnimations() = 0;
};

/**
 * Notification message for when 1+ animations have finished
 * @param[in] notifier This will provide the notification signals.
 */
inline MessageBase* AnimationFinishedMessage( AnimationFinishedNotifier& notifier )
{
  return new Message< AnimationFinishedNotifier >( &notifier, &AnimationFinishedNotifier::NotifyFinishedAnimations );
}

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ANIMATION_FINISHED_NOTIFIER_H__

