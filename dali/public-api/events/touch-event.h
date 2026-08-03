#ifndef DALI_TOUCH_EVENT_H
#define DALI_TOUCH_EVENT_H

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
#include <cstdint> // for uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/events/device.h>
#include <dali/public-api/events/mouse-button.h>
#include <dali/public-api/events/point-state.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/render-tasks/render-task.h>

namespace Dali
{
namespace Internal DALI_INTERNAL
{
class TouchEvent;
}

/**
 * @addtogroup dali_core_events
 * @{
 */

class Actor;
struct Vector2;

/**
 * @brief Touch events are a collection of points at a specific moment in time.
 *
 * When a multi-touch event occurs, each point represents the points that are currently being
 * touched or the points where a touch has stopped.
 *
 * The first point is the primary point that's used for hit-testing.
 * @SINCE_1_9.26
 * @note As this is a handle to an internal object, it should not be copied (or used in a container) as all that will do is copy the handle to the same object.
 * The internal object can change which may not be what an application writer expects.
 * If data does need to be stored in the application, then only the required data should be saved (retrieved using the methods of this class).
 *
 * Should not use this in a TouchEvent container as it is just a handle and the internal object can change.
 *
 * @note The setters are intended for touch events that the application creates itself with New().
 * As this class is a handle to a reference-counted object, modifying a touch event received from a
 * signal also changes what the other observers of that event see, even if the handle was received
 * as a const reference and then copied.
 */
class DALI_CORE_API TouchEvent : public BaseHandle
{
public:
  // Construction & Destruction

  /**
   * @brief Creates an initialized TouchEvent with no points.
   *
   * The points of the touch event are added with AddPoint().
   * This is intended for applications that need to synthesize a touch event,
   * e.g. to feed it to a window with Window::FeedTouchEvent().
   *
   * @SINCE_2_5.34
   * @param[in] time The time (in ms) that the touch event occurred
   * @return A handle to a newly allocated Dali resource
   */
  static TouchEvent New(uint32_t time);

  /**
   * @brief An uninitialized TouchEvent instance.
   *
   * Calling member functions with an uninitialized TouchEvent handle is not allowed.
   * @SINCE_1_9.26
   */
  TouchEvent();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_9.26
   * @param[in] other The TouchEvent to copy from
   */
  TouchEvent(const TouchEvent& other);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.28
   * @param[in] other The TouchEvent to move
   */
  TouchEvent(TouchEvent&& other) noexcept;

  /**
   * @brief Destructor.
   *
   * @SINCE_1_9.26
   */
  ~TouchEvent();

  // Operators

  /**
   * @brief Assignment Operator.
   *
   * @SINCE_1_9.26
   * @param[in] other The TouchEvent to copy from
   * @return A reference to this
   */
  TouchEvent& operator=(const TouchEvent& other);

  /**
   * @brief Move assignment Operator.
   *
   * @SINCE_1_9.28
   * @param[in] other The TouchEvent to move
   * @return A reference to this
   */
  TouchEvent& operator=(TouchEvent&& other) noexcept;

  // Getters

  /**
   * @brief Returns the time (in ms) that the touch event occurred.
   *
   * @SINCE_1_9.26
   * @return The time (in ms) that the touch event occurred
   */
  uint32_t GetTime() const;

  /**
   * @brief Returns the total number of points in this TouchEvent.
   *
   * @SINCE_1_9.26
   * @return Total number of Points
   */
  uint32_t GetPointCount() const;

  /**
   * @brief Returns the ID of the device used for the Point specified.
   *
   * Each point has a unique device ID which specifies the device used for that
   * point. This is returned by this method.
   *
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The Device ID of this point
   * @note If point is greater than GetPointCount() then this method will return -1.
   */
  int32_t GetDeviceId(uint32_t point) const;

  /**
   * @brief Retrieves the State of the point specified.
   *
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The state of the point specified
   * @note If point is greater than GetPointCount() then this method will return PointState::FINISHED.
   * @see State
   */
  PointState::Type GetState(uint32_t point) const;

  /**
   * @brief Retrieves the actor that was underneath the point specified.
   *
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The actor that was underneath the point specified
   * @note If point is greater than GetPointCount() then this method will return an empty handle.
   */
  Actor GetHitActor(uint32_t point) const;

  /**
   * @brief Retrieves the co-ordinates relative to the top-left of the hit-actor at the point specified.
   *
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The co-ordinates relative to the top-left of the hit-actor of the point specified
   *
   * @note The top-left of an actor is (0.0, 0.0, 0.5).
   * @note If you require the local coordinates of another actor (e.g the parent of the hit actor),
   * then you should use Actor::ScreenToLocal().
   * @note If point is greater than GetPointCount() then this method will return Vector2::ZERO.
   */
  const Vector2& GetLocalPosition(uint32_t point) const;

  /**
   * @brief Retrieves the co-ordinates relative to the top-left of the screen of the point specified.
   *
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The co-ordinates relative to the top-left of the screen of the point specified
   * @note If point is greater than GetPointCount() then this method will return Vector2::ZERO.
   */
  const Vector2& GetScreenPosition(uint32_t point) const;

  /**
   * @brief Retrieves the radius of the press point.
   *
   * This is the average of both the horizontal and vertical radii of the press point.
   *
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The radius of the press point
   * @note If point is greater than GetPointCount() then this method will return 0.0f.
   */
  float GetRadius(uint32_t point) const;

  /**
   * @brief Retrieves BOTH the horizontal and the vertical radii of the press point.
   *
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The horizontal and vertical radii of the press point
   * @note If point is greater than GetPointCount() then this method will return Vector2::ZERO.
   */
  const Vector2& GetEllipseRadius(uint32_t point) const;

  /**
   * @brief Retrieves the touch pressure.
   *
   * The pressure range starts at 0.0f.
   * Normal pressure is defined as 1.0f.
   * A value between 0.0f and 1.0f means light pressure has been applied.
   * A value greater than 1.0f means more pressure than normal has been applied.
   *
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The touch pressure
   * @note If point is greater than GetPointCount() then this method will return 1.0f.
   */
  float GetPressure(uint32_t point) const;

  /**
   * @brief Retrieves the angle of the press point relative to the Y-Axis.
   *
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The angle of the press point
   * @note If point is greater than GetPointCount() then this method will return Degree().
   */
  Degree GetAngle(uint32_t point) const;

  /**
   * @brief Gets the device class type from which the mouse/touch event is originated.
   *
   * The device class type is classification type of the input device of event received.
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The type of the device class
   */
  Device::Class::Type GetDeviceClass(uint32_t point) const;

  /**
   * @brief Gets the subclass type of the device from which the mouse/touch event is originated.
   *
   * The device subclass type is subclassification type of the input device of event received.
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The type of the device subclass
   */
  Device::Subclass::Type GetDeviceSubclass(uint32_t point) const;

  /**
   * @brief Gets the value, which indicates a mouse button. (ex: right/left button)
   *
   * @SINCE_1_9.26
   * @param[in] point The point required
   * @return The mouse button value
   */
  MouseButton::Type GetMouseButton(uint32_t point) const;

  /**
   * @brief Retrieves the render-task that was used to generate this touch-event.
   *
   * @SINCE_2_3.0
   * @return The render-task used to generate this touch-event.
   */
  RenderTask GetRenderTask() const;

  /**
   * @brief Gets the device name
   *
   * @SINCE_2_4.34
   * @return The device name string
   */
  const Dali::String& GetDeviceName(uint32_t point) const;

  // Setters

  /**
   * @brief Sets the time (in ms) that the touch event occurred.
   *
   * @SINCE_2_5.34
   * @param[in] time The time (in ms)
   */
  void SetTime(uint32_t time);

  /**
   * @brief Adds a point to this touch event.
   *
   * The first point added is the primary point.
   *
   * @SINCE_2_5.34
   * @param[in] deviceId The unique device ID of the device used for the point
   * @param[in] state The state of the point
   * @param[in] screenPosition The co-ordinates relative to the top-left of the screen
   *
   * @note The hit-actor and the local position of the point are not set here,
   * they are filled in by DALi when the touch event is processed.
   */
  void AddPoint(int32_t deviceId, PointState::Type state, const Vector2& screenPosition);

  /**
   * @brief Adds a point to this touch event with the information of the device it originated from.
   *
   * The first point added is the primary point.
   *
   * @SINCE_2_5.34
   * @param[in] deviceId The unique device ID of the device used for the point
   * @param[in] state The state of the point
   * @param[in] screenPosition The co-ordinates relative to the top-left of the screen
   * @param[in] deviceClass The class of the device the point originated from
   * @param[in] deviceSubclass The subclass of the device the point originated from
   * @param[in] deviceName The name of the device the point originated from
   * @param[in] mouseButton The mouse button pressed, if the point originated from a mouse
   *
   * @note The hit-actor and the local position of the point are not set here,
   * they are filled in by DALi when the touch event is processed.
   */
  void AddPoint(int32_t deviceId, PointState::Type state, const Vector2& screenPosition, Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass, const Dali::String& deviceName, MouseButton::Type mouseButton);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized TouchEvent handle.
   *
   * @SINCE_1_9.26
   * @param[in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL TouchEvent(Internal::TouchEvent* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_TOUCH_EVENT_H
