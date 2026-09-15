#ifndef DALI_GESTURE_DEVICE_SELECTOR_H
#define DALI_GESTURE_DEVICE_SELECTOR_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstdint> // uint8_t

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/events/device.h>

namespace DALI_NAMESPACE
{
/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief Selects the input devices a gesture recognition profile applies to.
 *
 * A selector matches an input device by its name, by its class and subclass, or by its class
 * alone. Gesture detectors and the application-wide gesture thresholds keep one profile per
 * selector and pick the profile for the device that starts a gesture in this order:
 *
 * @code
 * 1. A selector created with ByDeviceName() whose name equals the device name
 * 2. A selector created with ByDeviceClassAndSubclass() whose class and subclass equal the device's
 * 3. A selector created with ByDeviceClass() whose class equals the device's
 * 4. The default profile
 * @endcode
 *
 * Device names are compared exactly and are case sensitive. They are the names the platform
 * reports for the device (see Gesture::GetDeviceName()); a device that reports no name never
 * matches a name selector and falls through to the class selectors.
 *
 * This is an immutable value type: copies are independent.
 *
 * @SINCE_2_5.40
 * @see PanGestureDetector, TapGestureDetector, Gesture::GetDeviceName()
 */
class DALI_CORE_API GestureDeviceSelector
{
public:
  /**
   * @brief How a selector is matched against an input device.
   * @SINCE_2_5.40
   */
  enum class MatchType : uint8_t
  {
    DEVICE_NAME               = 0, ///< Matches the device name exactly. @SINCE_2_5.40
    DEVICE_CLASS              = 1, ///< Matches the device class. @SINCE_2_5.40
    DEVICE_CLASS_AND_SUBCLASS = 2  ///< Matches the device class and subclass. @SINCE_2_5.40
  };

  /**
   * @brief Creates a selector matching every device of the given class.
   *
   * @SINCE_2_5.40
   * @param[in] deviceClass The device class to match. Device::Class::NONE is a valid value for
   *                        platforms that report no class.
   * @return The selector
   */
  static GestureDeviceSelector ByDeviceClass(Device::Class::Type deviceClass);

  /**
   * @brief Creates a selector matching every device of the given class and subclass.
   *
   * @SINCE_2_5.40
   * @param[in] deviceClass    The device class to match
   * @param[in] deviceSubclass The device subclass to match
   * @return The selector
   */
  static GestureDeviceSelector ByDeviceClassAndSubclass(Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass);

  /**
   * @brief Creates a selector matching the device with exactly the given name.
   *
   * @SINCE_2_5.40
   * @param[in] deviceName The platform device name. Must not be empty.
   * @return The selector
   * @pre deviceName is not empty.
   */
  static GestureDeviceSelector ByDeviceName(const Dali::String& deviceName);

  /**
   * @brief Creates a selector equivalent to ByDeviceClass(Device::Class::NONE).
   * @SINCE_2_5.40
   */
  GestureDeviceSelector();

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The selector to copy
   */
  GestureDeviceSelector(const GestureDeviceSelector& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The selector to move
   */
  GestureDeviceSelector(GestureDeviceSelector&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The selector to copy
   * @return A reference to this
   */
  GestureDeviceSelector& operator=(const GestureDeviceSelector& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The selector to move
   * @return A reference to this
   */
  GestureDeviceSelector& operator=(GestureDeviceSelector&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_2_5.40
   */
  ~GestureDeviceSelector();

  /**
   * @brief Retrieves how this selector is matched.
   * @SINCE_2_5.40
   * @return The match type
   */
  MatchType GetMatchType() const;

  /**
   * @brief Retrieves the device class this selector matches.
   * @SINCE_2_5.40
   * @return The device class, or Device::Class::NONE for a name selector
   */
  Device::Class::Type GetDeviceClass() const;

  /**
   * @brief Retrieves the device subclass this selector matches.
   * @SINCE_2_5.40
   * @return The device subclass, or Device::Subclass::NONE unless created with ByDeviceClassAndSubclass()
   */
  Device::Subclass::Type GetDeviceSubclass() const;

  /**
   * @brief Retrieves the device name this selector matches.
   * @SINCE_2_5.40
   * @return The device name, or an empty string unless created with ByDeviceName()
   */
  Dali::String GetDeviceName() const;

  /**
   * @brief Compares two selectors.
   *
   * Two selectors are equal when they have the same match type and the fields relevant to that
   * match type are equal.
   * @SINCE_2_5.40
   * @param[in] rhs The selector to compare with
   * @return true if equal
   */
  bool operator==(const GestureDeviceSelector& rhs) const;

  /**
   * @brief Compares two selectors.
   * @SINCE_2_5.40
   * @param[in] rhs The selector to compare with
   * @return true if not equal
   */
  bool operator!=(const GestureDeviceSelector& rhs) const;

private:
  struct Impl;
  UniquePtr<Impl> mImpl;
};

/**
 * @}
 */
} // namespace DALI_NAMESPACE

#endif // DALI_GESTURE_DEVICE_SELECTOR_H
