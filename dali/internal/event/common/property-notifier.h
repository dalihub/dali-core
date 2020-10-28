#ifndef DALI_INTERNAL_PROPERTY_NOTIFIER_H
#define DALI_INTERNAL_PROPERTY_NOTIFIER_H

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

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class PropertyNotification;

} //namespace SceneGraph

/**
 * Interface used by the update-thread to trigger property changed notification signals.
 */
class PropertyNotifier
{
public:

  /**
   * Virtual destructor.
   */
  virtual ~PropertyNotifier() = default;

  /**
   * Provide notification signals for a changed property.
   * This method should be called in the event-thread; the update-thread must use PropertyChangedMessage.
   * @param[in] propertyNotification A pointer to the SceneGraph::PropertyNotification that has been mnodified.
   * @param[in] validity Passes in whether the notification was triggered by a true or false condition result
   */
  virtual void NotifyProperty( SceneGraph::PropertyNotification* propertyNotification, bool validity ) = 0;
};

/**
 * Notification message for when a property has been modified
 * @param[in] notifier This will provide the notification signal.
 */
inline MessageBase* PropertyChangedMessage( PropertyNotifier& notifier, SceneGraph::PropertyNotification* propertyNotification, bool validity )
{
  return new MessageValue2< PropertyNotifier, SceneGraph::PropertyNotification*, bool >( &notifier,
                                                                                   &PropertyNotifier::NotifyProperty,
                                                                                   propertyNotification, validity);
}

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PROPERTY_NOTIFIER_H
