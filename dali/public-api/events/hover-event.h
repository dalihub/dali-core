#ifndef DALI_HOVER_EVENT_H
#define DALI_HOVER_EVENT_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/device.h>
#include <dali/public-api/events/point-state.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Internal DALI_INTERNAL
{
class HoverEvent;
}

/**
 * @addtogroup dali_core_events
 * @{
 */

class Actor;
struct Vector2;

/**
 * @brief Hover events are a collection of touch points at a specific moment in time.
 *
 * When a multi-touch event occurs, each touch point represents the points that are currently being
 * hovered or the points where a hover has stopped.
 * @SINCE_1_0.0
 */
class DALI_CORE_API HoverEvent : public BaseHandle
{
public:
  // Construction & Destruction

  /**
   * @brief An uninitialized HoverEvent instance.
   *
   * Calling member functions with an uninitialized HoverEvent handle is not allowed.
   * @SINCE_1_0.0
   */
  HoverEvent();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_9.25
   * @param[in] rhs The HoverEvent to copy from
   */
  HoverEvent(const HoverEvent& rhs);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.25
   * @param[in] rhs A reference to the moved HoverEvent
   */
  HoverEvent(HoverEvent&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_1_0.0
   */
  ~HoverEvent();

  // Operators

  /**
   * @brief Copy assignment operator.
   *
   * @SINCE_1_1.37
   * @param[in] rhs The HoverEvent to copy from
   * @return A reference to this
   */
  HoverEvent& operator=(const HoverEvent& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.25
   * @param[in] rhs A reference to the moved HoverEvent
   * @return A reference to this
   */
  HoverEvent& operator=(HoverEvent&& rhs) noexcept;

  // Getters

  /**
   * @brief Returns the time (in ms) that the hover event occurred.
   *
   * @SINCE_1_9.25
   * @return The time (in ms) that the hover event occurred
   */
  unsigned long GetTime() const;

  /**
   * @brief Returns the total number of points in this hover event.
   *
   * @SINCE_1_9.25
   * @return Total number of Points
   */
  std::size_t GetPointCount() const;

  /**
   * @brief Returns the ID of the device used for the Point specified.
   *
   * Each point has a unique device ID which specifies the device used for that
   * point. This is returned by this method.
   *
   * @SINCE_1_9.25
   * @param[in] point The point required
   * @return The Device ID of this point
   * @note If point is greater than GetPointCount() then this method will return -1.
   */
  int32_t GetDeviceId(std::size_t point) const;

  /**
   * @brief Retrieves the State of the point specified.
   *
   * @SINCE_1_9.25
   * @param[in] point The point required
   * @return The state of the point specified
   * @note If point is greater than GetPointCount() then this method will return PointState::FINISHED.
   * If you set Actor::Property::LEAVE_REQUIRED to true, when the hover event enters the actor, it will receive STARTED state.
   * an actor that received a hover PointState::MOTION event will receive PointState::INTERRUPTED event when Actor::Property::SENSITIVE is changed to false.
   * However, an actor that received a hover PointState::STARTED event will not receive any event when Actor::Property::SENSITIVE is changed to false.
   * @see State
   */
  PointState::Type GetState(std::size_t point) const;

  /**
   * @brief Retrieves the actor that was underneath the point specified.
   *
   * @SINCE_1_9.25
   * @param[in] point The point required
   * @return The actor that was underneath the point specified
   * @note If point is greater than GetPointCount() then this method will return an empty handle.
   */
  Actor GetHitActor(std::size_t point) const;

  /**
   * @brief Retrieves the co-ordinates relative to the top-left of the hit-actor at the point specified.
   *
   * @SINCE_1_9.25
   * @param[in] point The point required
   * @return The co-ordinates relative to the top-left of the hit-actor of the point specified
   *
   * @note The top-left of an actor is (0.0, 0.0, 0.5).
   * @note If you require the local coordinates of another actor (e.g the parent of the hit actor),
   * then you should use Actor::ScreenToLocal().
   * @note If point is greater than GetPointCount() then this method will return Vector2::ZERO.
   */
  const Vector2& GetLocalPosition(std::size_t point) const;

  /**
   * @brief Retrieves the co-ordinates relative to the top-left of the screen of the point specified.
   *
   * @SINCE_1_9.25
   * @param[in] point The point required
   * @return The co-ordinates relative to the top-left of the screen of the point specified
   * @note If point is greater than GetPointCount() then this method will return Vector2::ZERO.
   */
  const Vector2& GetScreenPosition(std::size_t point) const;

  /**
   * @brief Gets the device class type from which the hover event is originated.
   *
   * The device class type is classification type of the input device of event received.
   * @SINCE_2_4.31
   * @param[in] point The point required
   * @return The type of the device class
   */
  Device::Class::Type GetDeviceClass(std::size_t point) const;

  /**
   * @brief Gets the subclass type of the device from which the hover event is originated.
   *
   * The device subclass type is subclassification type of the input device of event received.
   * @SINCE_2_4.31
   * @param[in] point The point required
   * @return The type of the device subclass
   */
  Device::Subclass::Type GetDeviceSubclass(std::size_t point) const;

  /**
   * @brief Gets the device name
   *
   * @SINCE_2_4.34
   * @return The device name string
   */
  const std::string& GetDeviceName(std::size_t point) const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized HoverEvent handle.
   *
   * @SINCE_1_9.25
   * @param[in] hoverEvent A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL HoverEvent(Internal::HoverEvent* hoverEvent);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_HOVER_EVENT_H
