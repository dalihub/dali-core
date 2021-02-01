#ifndef DALI_INTERNAL_PROPERTY_NOTIFICATION_MANAGER_H
#define DALI_INTERNAL_PROPERTY_NOTIFICATION_MANAGER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/property-notifier.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
class PropertyNotification;

/**
 * PropertyNotificationManager issues notifications to applications
 * a condition of a property being met.
 * It also monitors the lifetime of PropertyNotification objects and will
 * only emit signals for PropertyNotification objects which are still valid.
 */
class PropertyNotificationManager : public PropertyNotifier
{
public:
  /**
   * Create an PropertyNotificationManager.
   * @return A newly allocated object.
   */
  static PropertyNotificationManager* New();

  /**
   * Virtual destructor.
   */
  ~PropertyNotificationManager() override;

  /**
   * Called when a PropertyNotification is constructed.
   */
  void PropertyNotificationCreated(PropertyNotification& propertyNotification);

  /**
   * Called when a PropertyNotification is destroyed.
   */
  void PropertyNotificationDestroyed(PropertyNotification& propertyNotification);

private: // private virtual overrides
  /**
   * @copydoc PropertyNotifier::NotifyProperty
   */
  void NotifyProperty(SceneGraph::PropertyNotification* propertyNotification, bool validity) override;

private:
  /**
   * Default constructor.
   */
  PropertyNotificationManager();

  // Undefined
  PropertyNotificationManager(const PropertyNotificationManager&);

  // Undefined
  PropertyNotificationManager& operator=(const PropertyNotificationManager& rhs);

private:
  Dali::Vector<PropertyNotification*> mPropertyNotifications; ///< All existing PropertyNotifications (not owned)
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PROPERTY_NOTIFICATION_MANAGER_H
