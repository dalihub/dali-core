#ifndef __DALI_PROPERTY_NOTIFICATION_H__
#define __DALI_PROPERTY_NOTIFICATION_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-notification-declarations.h>
#include <dali/public-api/object/property-conditions.h>
#include <dali/public-api/object/property.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

namespace Internal DALI_INTERNAL
{
class PropertyNotification;
}

/**
 * @brief This is used to issue a notification upon a condition of the property being met.
 *
 * For example checking if Actor::POSITION_X > 100.0.
 * @SINCE_1_0.0
 * @see Dali::PropertyCondition
 */
class DALI_IMPORT_API PropertyNotification : public BaseHandle
{
public:
  /**
   * @brief Enumeration for description of how to check condition.
   * @SINCE_1_0.0
   */
  enum NotifyMode
  {
    Disabled,                             ///< Don't notify, regardless of result of Condition @SINCE_1_0.0
    NotifyOnTrue,                         ///< Notify whenever condition changes from false to true. @SINCE_1_0.0
    NotifyOnFalse,                        ///< Notify whenever condition changes from true to false. @SINCE_1_0.0
    NotifyOnChanged                       ///< Notify whenever condition changes (false to true, and true to false) @SINCE_1_0.0
  };

public:

  /**
   * @brief Creates an uninitialized PropertyNotification; this can be initialized with PropertyNotification::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  PropertyNotification();

  /**
   * @brief Downcasts a handle to PropertyNotification handle.
   *
   * If handle points to a PropertyNotification object, the downcast
   * produces valid handle. If not, the returned handle is left
   * uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle to An object
   * @return handle to a PropertyNotification object or an uninitialized handle
   */
  static PropertyNotification DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~PropertyNotification();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  PropertyNotification(const PropertyNotification& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  PropertyNotification& operator=(const PropertyNotification& rhs);

  /**
   * @brief Gets the condition of this notification.
   *
   * @SINCE_1_0.0
   * @return The condition is returned
   */
  PropertyCondition GetCondition();

  /**
   * @brief Gets the condition of this notification.
   *
   * @SINCE_1_0.0
   * @return The condition is returned
   */
  const PropertyCondition& GetCondition() const;

  /**
   * @brief Gets the target handle that this notification is observing.
   * @SINCE_1_0.0
   * @return The target handle
   */
  Dali::Handle GetTarget() const;

  /**
   * @brief Gets the target handle's property index that this notification
   * is observing.
   *
   * @SINCE_1_0.0
   * @return The target property
   */
  Property::Index GetTargetProperty() const;

  /**
   * @brief Sets the Notification mode.
   *
   * This determines how the property
   * notification should respond to the result of a condition.
   *
   * @SINCE_1_0.0
   * @param[in] mode Notification mode (Default is PropertyNotification::NotifyOnTrue)
   */
  void SetNotifyMode( NotifyMode mode );

  /**
   * @brief Retrieves the current Notification mode.
   *
   * @SINCE_1_0.0
   * @return Notification mode
   */
  NotifyMode GetNotifyMode();

  /**
   * @brief Gets the result of the last condition check that caused a signal emit,
   * useful when using NotifyOnChanged mode and need to know what it changed to.
   *
   * @SINCE_1_0.0
   * @return whether condition result that triggered last emit was true or false
   */
  bool GetNotifyResult() const;

  /**
   * @brief Connects to this signal to be notified when the notification has occurred.
   *
   * @SINCE_1_0.0
   * @return A signal object to Connect() with
   */
  PropertyNotifySignalType& NotifySignal();

public: // Not intended for use by Application developers

  /**
   * @internal
   * @brief This constructor is used by Dali New() methods.
   *
   * @SINCE_1_0.0
   * @param[in] propertyNotification A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL PropertyNotification(Internal::PropertyNotification* propertyNotification);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PROPERTY_NOTIFICATION_H__
