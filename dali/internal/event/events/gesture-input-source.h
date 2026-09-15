#ifndef DALI_INTERNAL_GESTURE_INPUT_SOURCE_H
#define DALI_INTERNAL_GESTURE_INPUT_SOURCE_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/events/device.h>
#include <dali/public-api/events/gesture-device-selector.h>

namespace DALI_NAMESPACE
{
namespace Integration
{
struct Point;
}

namespace Internal
{
/**
 * @brief The input device that started a gesture sequence.
 *
 * Captured from the first DOWN of a sequence, independently of the mouse button, so it is
 * available for devices that never report one. Used to resolve per-device gesture profiles.
 */
struct GestureInputSource
{
  /**
   * @brief Builds the source from a touch point.
   * @param[in] point The point of the touch event that starts the sequence
   * @return The source
   */
  static GestureInputSource FromPoint(const Integration::Point& point);

  /**
   * @brief Checks whether the selector describes this source.
   *
   * A name selector never matches a source without a name.
   * @param[in] selector The selector to test
   * @return true if the selector matches
   */
  bool Matches(const GestureDeviceSelector& selector) const;

  /**
   * @brief Checks whether another source describes the same device (class, subclass and name).
   * @param[in] other The source to compare with
   * @return true if both describe the same device
   */
  bool IsSameDevice(const GestureInputSource& other) const;

  Device::Class::Type    deviceClass{Device::Class::NONE};       ///< The device class.
  Device::Subclass::Type deviceSubclass{Device::Subclass::NONE}; ///< The device subclass.
  Dali::String           deviceName;                             ///< The platform device name, empty when not reported.
  bool                   valid{false};                           ///< false until a sequence has been captured.
};

} // namespace Internal

} // namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_GESTURE_INPUT_SOURCE_H
