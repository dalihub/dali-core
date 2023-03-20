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

// CLASS HEADER
#include <dali/internal/event/common/property-notification-manager.h>

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/property-notification-impl.h>

namespace Dali
{
namespace Internal
{
PropertyNotificationManager* PropertyNotificationManager::New()
{
  return new PropertyNotificationManager;
}

PropertyNotificationManager::~PropertyNotificationManager() = default;

void PropertyNotificationManager::PropertyNotificationCreated(PropertyNotification& propertyNotification)
{
  mPropertyNotifications.PushBack(&propertyNotification);
}

void PropertyNotificationManager::PropertyNotificationDestroyed(PropertyNotification& propertyNotification)
{
  auto iter = mPropertyNotifications.Find(&propertyNotification);
  DALI_ASSERT_ALWAYS(iter != mPropertyNotifications.End() && "PropertyNotification not found");

  mPropertyNotifications.Erase(iter);
}

void PropertyNotificationManager::NotifyProperty(NotifierInterface::NotifyId notifyId, bool validity)
{
  Dali::PropertyNotification handle; // Will own handle until all emits have been done.

  auto* propertyNotification = GetEventObject(notifyId);
  if(DALI_LIKELY(propertyNotification))
  {
    // Check if this notification hold inputed scenegraph property notification.
    DALI_ASSERT_DEBUG(propertyNotification->CompareSceneObjectNotifyId(notifyId));

    handle = Dali::PropertyNotification(propertyNotification);

    // allow application to access the value that triggered this emit incase of NOTIFY_ON_CHANGED mode
    propertyNotification->SetNotifyResult(validity);
    // yes..emit signal
    propertyNotification->EmitSignalNotify();
  }
}

PropertyNotificationManager::PropertyNotificationManager() = default;

} // namespace Internal

} // namespace Dali
