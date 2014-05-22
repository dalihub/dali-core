#ifndef __DALI_PROPERTY_NOTIFICATION_H__
#define __DALI_PROPERTY_NOTIFICATION_H__

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

/**
 * @addtogroup CAPI_DALI_OBJECT_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-notification-declarations.h>
#include <dali/public-api/object/property-conditions.h>
#include <dali/public-api/object/property.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class PropertyNotification;
}

/**
 * @brief This is used to issue a notification upon a condition of the property being met.
 *
 * For example checking if Actor::POSITION_X > 100.0.
 * @see Dali::PropertyCondition
 */
class DALI_IMPORT_API PropertyNotification : public BaseHandle
{
public:
  /**
   * @brief Enumeration to describe how to check condition
   */
  enum NotifyMode
  {
    Disabled,                             ///< Don't notify, regardless of result of Condition
    NotifyOnTrue,                         ///< Notify whenever condition changes from false to true.
    NotifyOnFalse,                        ///< Notify whenever condition changes from true to false.
    NotifyOnChanged                       ///< Notify whenever condition changes (false to true, and true to false)
  };

public:

  /**
   * @brief Create an uninitialized PropertyNotification; this can be initialized with PropertyNotification::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  PropertyNotification();

  /**
   * @brief Downcast an Object handle to PropertyNotification.
   *
   * If handle points to an PropertyNotification object the downcast
   * produces valid handle. If not the returned handle is left
   * uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a PropertyNotification object or an uninitialized handle
   */
  static PropertyNotification DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   */
  virtual ~PropertyNotification();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * @brief Get the condition of this notification
   *
   * @return The condition is returned
   */
  PropertyCondition GetCondition();

  /**
   * @brief Get the condition of this notification
   *
   * @return The condition is returned
   */
  const PropertyCondition& GetCondition() const;

  /**
   * @brief Get the target handle that this notification is observing.
   */
  Dali::Handle GetTarget() const;

  /**
   * @brief Get the target handle's property index that this notification
   * is observing.
   *
   * @return The target property.
   */
  Property::Index GetTargetProperty() const;

  /**
   * @brief Sets the Notification mode. This determines how the property
   * notification should respond to the result of a condition.
   *
   * @param[in] mode Notification mode (Default is PropertyNotification::NotifyOnTrue)
   */
  void SetNotifyMode( NotifyMode mode );

  /**
   * @brief Retrieves the current Notification mode.
   *
   * @return Notification mode.
   */
  NotifyMode GetNotifyMode();

  /**
   * @brief Gets the result of the last condition check that caused a signal emit,
   * useful when using NotifyOnChanged mode and need to know what it changed to.
   *
   * @return whether condition result that triggered last emit was true or false
   */
  bool GetNotifyResult() const;

  /**
   * @brief Connect to this signal to be notified when the notification has occurred.
   *
   * @return A signal object to Connect() with.
   */
  PropertyNotifySignalV2& NotifySignal();

public: // Not intended for use by Application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] propertyNotification A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL PropertyNotification(Internal::PropertyNotification* propertyNotification);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_PROPERTY_NOTIFICATION_H__
