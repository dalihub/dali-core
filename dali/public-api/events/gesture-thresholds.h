#ifndef DALI_GESTURE_THRESHOLDS_H
#define DALI_GESTURE_THRESHOLDS_H

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
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/events/gesture-device-selector.h>

namespace DALI_NAMESPACE
{
namespace Internal DALI_INTERNAL
{
struct PanThresholdValues;
struct TapThresholdValues;
struct LongPressThresholdValues;
struct PinchThresholdValues;
struct RotationThresholdValues;
} //namespace Internal DALI_INTERNAL

/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief Application-wide gesture recognition thresholds, optionally per input device.
 *
 * The thresholds decide when the recognizer turns raw touch input into a gesture: how far a pointer
 * must move before a pan starts, how long a touch may be held to still be a tap, and so on. They
 * apply to every gesture detector of the application, including detectors created inside UI
 * frameworks that the application cannot reach.
 *
 * Without any registration, the values set through the platform (environment variables) or through
 * the framework configuration apply to every device. Register a set of thresholds for a
 * GestureDeviceSelector to use different values for the devices it matches. When a gesture starts,
 * the recognizer picks the thresholds in this order: a matching device-name selector, then a
 * matching class-and-subclass selector, then a matching class selector, then the application-wide
 * values. The choice is made once per gesture sequence.
 *
 * @code
 * // A pointing remote controller is jittery: require a longer movement before a pan starts.
 * GestureThresholds::PanThresholds remote = GestureThresholds::GetDefaultPanThresholds();
 * remote.SetMinimumDistance(20);
 * remote.SetMinimumPanEvents(7);
 * GestureThresholds::SetPanThresholds(GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::POINTER, Device::Subclass::REMOCON), remote);
 * @endcode
 *
 * @SINCE_2_5.40
 */
namespace GestureThresholds
{
/**
 * @brief The application-wide pan recognition thresholds.
 *
 * A value type: copies are independent. A default-constructed object holds the DALi built-in
 * defaults; use GetDefaultPanThresholds() for the values currently in effect.
 * @SINCE_2_5.40
 */
class DALI_CORE_API PanThresholds
{
public:
  /**
   * @brief Creates thresholds holding the DALi built-in defaults.
   * @SINCE_2_5.40
   */
  PanThresholds();

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to copy
   */
  PanThresholds(const PanThresholds& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to move
   */
  PanThresholds(PanThresholds&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to copy
   * @return A reference to this
   */
  PanThresholds& operator=(const PanThresholds& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to move
   * @return A reference to this
   */
  PanThresholds& operator=(PanThresholds&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_2_5.40
   */
  ~PanThresholds();

  /**
   * @brief Sets the motion distance in pixels a pointer must travel before a pan starts. Must be >= 0.
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMinimumDistance(int32_t value);

  /**
   * @brief Retrieves the motion distance in pixels a pointer must travel before a pan starts.
   * @SINCE_2_5.40
   * @return The value
   */
  int32_t GetMinimumDistance() const;

  /**
   * @brief Sets the number of touch events (the DOWN included) before a pan starts. Must be >= 1.
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMinimumPanEvents(int32_t value);

  /**
   * @brief Retrieves the number of touch events (the DOWN included) before a pan starts.
   * @SINCE_2_5.40
   * @return The value
   */
  int32_t GetMinimumPanEvents() const;

public: // Not intended for Application developers
  /// @cond internal
  /**
   * @brief Creates thresholds from internal values.
   * @param[in] values The values to copy
   */
  explicit DALI_INTERNAL PanThresholds(const Internal::PanThresholdValues& values);

  /**
   * @brief Retrieves the internal values.
   * @return The values
   */
  DALI_INTERNAL const Internal::PanThresholdValues& GetValues() const;
  /// @endcond

private:
  struct Impl;
  UniquePtr<Impl> mImpl;
};

/**
 * @brief Retrieves the pan thresholds currently in effect for devices without a registered entry.
 * @SINCE_2_5.40
 * @return A copy of the thresholds
 */
DALI_CORE_API PanThresholds GetDefaultPanThresholds();

/**
 * @brief Registers the pan thresholds to use for gestures started by the devices matching the selector.
 *
 * The thresholds are copied and replace any registered for the same selector. They apply to every
 * pan gesture detector of the application from the next gesture sequence on.
 * @SINCE_2_5.40
 * @param[in] selector   The devices the thresholds apply to
 * @param[in] thresholds The thresholds. Invalid values (see the setters) are rejected.
 */
DALI_CORE_API void SetPanThresholds(const GestureDeviceSelector& selector, const PanThresholds& thresholds);

/**
 * @brief Retrieves the pan thresholds registered for exactly this selector.
 *
 * Only thresholds registered with SetPanThresholds() for the same selector are returned; the fallback order
 * used during recognition is not applied.
 * @SINCE_2_5.40
 * @param[in]  selector   The selector the thresholds were registered with
 * @param[out] thresholds Receives a copy of the thresholds. Left unchanged when none are registered.
 * @return true if thresholds are registered for the selector
 */
DALI_CORE_API bool GetPanThresholds(const GestureDeviceSelector& selector, PanThresholds& thresholds);

/**
 * @brief Removes the pan thresholds registered for exactly this selector.
 *
 * Devices that matched the selector fall back to the next matching selector or the application-wide
 * values from the next gesture sequence on. Does nothing if none are registered for the selector.
 * @SINCE_2_5.40
 * @param[in] selector The selector the thresholds were registered with
 */
DALI_CORE_API void ClearPanThresholds(const GestureDeviceSelector& selector);

/**
 * @brief The application-wide tap recognition thresholds.
 *
 * A value type: copies are independent. A default-constructed object holds the DALi built-in
 * defaults; use GetDefaultTapThresholds() for the values currently in effect.
 * @SINCE_2_5.40
 */
class DALI_CORE_API TapThresholds
{
public:
  /**
   * @brief Creates thresholds holding the DALi built-in defaults.
   * @SINCE_2_5.40
   */
  TapThresholds();

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to copy
   */
  TapThresholds(const TapThresholds& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to move
   */
  TapThresholds(TapThresholds&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to copy
   * @return A reference to this
   */
  TapThresholds& operator=(const TapThresholds& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to move
   * @return A reference to this
   */
  TapThresholds& operator=(TapThresholds&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_2_5.40
   */
  ~TapThresholds();

  /**
   * @brief Sets the interval in milliseconds within which a further tap continues a multi tap. Must be > 0.
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMaximumMultiTapInterval(uint32_t value);

  /**
   * @brief Retrieves the interval in milliseconds within which a further tap continues a multi tap.
   * @SINCE_2_5.40
   * @return The value
   */
  uint32_t GetMaximumMultiTapInterval() const;

  /**
   * @brief Sets the time in milliseconds a touch may be held and still count as a tap. Must be > 0.
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMaximumHoldingTime(uint32_t value);

  /**
   * @brief Retrieves the time in milliseconds a touch may be held and still count as a tap.
   * @SINCE_2_5.40
   * @return The value
   */
  uint32_t GetMaximumHoldingTime() const;

  /**
   * @brief Sets the distance in pixels a touch may move and still count as a tap. Must be >= 0.
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMaximumMotionDistance(float value);

  /**
   * @brief Retrieves the distance in pixels a touch may move and still count as a tap.
   * @SINCE_2_5.40
   * @return The value
   */
  float GetMaximumMotionDistance() const;

public: // Not intended for Application developers
  /// @cond internal
  /**
   * @brief Creates thresholds from internal values.
   * @param[in] values The values to copy
   */
  explicit DALI_INTERNAL TapThresholds(const Internal::TapThresholdValues& values);

  /**
   * @brief Retrieves the internal values.
   * @return The values
   */
  DALI_INTERNAL const Internal::TapThresholdValues& GetValues() const;
  /// @endcond

private:
  struct Impl;
  UniquePtr<Impl> mImpl;
};

/**
 * @brief Retrieves the tap thresholds currently in effect for devices without a registered entry.
 * @SINCE_2_5.40
 * @return A copy of the thresholds
 */
DALI_CORE_API TapThresholds GetDefaultTapThresholds();

/**
 * @brief Registers the tap thresholds to use for gestures started by the devices matching the selector.
 *
 * The thresholds are copied and replace any registered for the same selector. They apply to every
 * tap gesture detector of the application from the next gesture sequence on.
 * @SINCE_2_5.40
 * @param[in] selector   The devices the thresholds apply to
 * @param[in] thresholds The thresholds. Invalid values (see the setters) are rejected.
 */
DALI_CORE_API void SetTapThresholds(const GestureDeviceSelector& selector, const TapThresholds& thresholds);

/**
 * @brief Retrieves the tap thresholds registered for exactly this selector.
 *
 * Only thresholds registered with SetTapThresholds() for the same selector are returned; the fallback order
 * used during recognition is not applied.
 * @SINCE_2_5.40
 * @param[in]  selector   The selector the thresholds were registered with
 * @param[out] thresholds Receives a copy of the thresholds. Left unchanged when none are registered.
 * @return true if thresholds are registered for the selector
 */
DALI_CORE_API bool GetTapThresholds(const GestureDeviceSelector& selector, TapThresholds& thresholds);

/**
 * @brief Removes the tap thresholds registered for exactly this selector.
 *
 * Devices that matched the selector fall back to the next matching selector or the application-wide
 * values from the next gesture sequence on. Does nothing if none are registered for the selector.
 * @SINCE_2_5.40
 * @param[in] selector The selector the thresholds were registered with
 */
DALI_CORE_API void ClearTapThresholds(const GestureDeviceSelector& selector);

/**
 * @brief The application-wide long press recognition thresholds.
 *
 * A value type: copies are independent. A default-constructed object holds the DALi built-in
 * defaults; use GetDefaultLongPressThresholds() for the values currently in effect.
 * @SINCE_2_5.40
 */
class DALI_CORE_API LongPressThresholds
{
public:
  /**
   * @brief Creates thresholds holding the DALi built-in defaults.
   * @SINCE_2_5.40
   */
  LongPressThresholds();

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to copy
   */
  LongPressThresholds(const LongPressThresholds& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to move
   */
  LongPressThresholds(LongPressThresholds&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to copy
   * @return A reference to this
   */
  LongPressThresholds& operator=(const LongPressThresholds& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to move
   * @return A reference to this
   */
  LongPressThresholds& operator=(LongPressThresholds&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_2_5.40
   */
  ~LongPressThresholds();

  /**
   * @brief Sets the time in milliseconds a touch must be held before a long press is recognised. Must be > 0.
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMinimumHoldingTime(uint32_t value);

  /**
   * @brief Retrieves the time in milliseconds a touch must be held before a long press is recognised.
   * @SINCE_2_5.40
   * @return The value
   */
  uint32_t GetMinimumHoldingTime() const;

public: // Not intended for Application developers
  /// @cond internal
  /**
   * @brief Creates thresholds from internal values.
   * @param[in] values The values to copy
   */
  explicit DALI_INTERNAL LongPressThresholds(const Internal::LongPressThresholdValues& values);

  /**
   * @brief Retrieves the internal values.
   * @return The values
   */
  DALI_INTERNAL const Internal::LongPressThresholdValues& GetValues() const;
  /// @endcond

private:
  struct Impl;
  UniquePtr<Impl> mImpl;
};

/**
 * @brief Retrieves the long press thresholds currently in effect for devices without a registered entry.
 * @SINCE_2_5.40
 * @return A copy of the thresholds
 */
DALI_CORE_API LongPressThresholds GetDefaultLongPressThresholds();

/**
 * @brief Registers the long press thresholds to use for gestures started by the devices matching the selector.
 *
 * The thresholds are copied and replace any registered for the same selector. They apply to every
 * long press gesture detector of the application from the next gesture sequence on.
 * @SINCE_2_5.40
 * @param[in] selector   The devices the thresholds apply to
 * @param[in] thresholds The thresholds. Invalid values (see the setters) are rejected.
 */
DALI_CORE_API void SetLongPressThresholds(const GestureDeviceSelector& selector, const LongPressThresholds& thresholds);

/**
 * @brief Retrieves the long press thresholds registered for exactly this selector.
 *
 * Only thresholds registered with SetLongPressThresholds() for the same selector are returned; the fallback order
 * used during recognition is not applied.
 * @SINCE_2_5.40
 * @param[in]  selector   The selector the thresholds were registered with
 * @param[out] thresholds Receives a copy of the thresholds. Left unchanged when none are registered.
 * @return true if thresholds are registered for the selector
 */
DALI_CORE_API bool GetLongPressThresholds(const GestureDeviceSelector& selector, LongPressThresholds& thresholds);

/**
 * @brief Removes the long press thresholds registered for exactly this selector.
 *
 * Devices that matched the selector fall back to the next matching selector or the application-wide
 * values from the next gesture sequence on. Does nothing if none are registered for the selector.
 * @SINCE_2_5.40
 * @param[in] selector The selector the thresholds were registered with
 */
DALI_CORE_API void ClearLongPressThresholds(const GestureDeviceSelector& selector);

/**
 * @brief The application-wide pinch recognition thresholds.
 *
 * A value type: copies are independent. A default-constructed object holds the DALi built-in
 * defaults; use GetDefaultPinchThresholds() for the values currently in effect.
 * @SINCE_2_5.40
 */
class DALI_CORE_API PinchThresholds
{
public:
  /**
   * @brief Creates thresholds holding the DALi built-in defaults.
   * @SINCE_2_5.40
   */
  PinchThresholds();

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to copy
   */
  PinchThresholds(const PinchThresholds& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to move
   */
  PinchThresholds(PinchThresholds&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to copy
   * @return A reference to this
   */
  PinchThresholds& operator=(const PinchThresholds& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to move
   * @return A reference to this
   */
  PinchThresholds& operator=(PinchThresholds&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_2_5.40
   */
  ~PinchThresholds();

  /**
   * @brief Sets the change in distance between the two touch points in pixels before a pinch starts. A negative value means "derive from the scene DPI".
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMinimumDistance(float value);

  /**
   * @brief Retrieves the change in distance between the two touch points in pixels before a pinch starts.
   * @SINCE_2_5.40
   * @return The value
   */
  float GetMinimumDistance() const;

  /**
   * @brief Sets the number of touch events collected before a pinch can start. Must be > 1.
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMinimumTouchEvents(uint32_t value);

  /**
   * @brief Retrieves the number of touch events collected before a pinch can start.
   * @SINCE_2_5.40
   * @return The value
   */
  uint32_t GetMinimumTouchEvents() const;

  /**
   * @brief Sets the number of touch events collected between updates once a pinch has started. Must be > 1.
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMinimumTouchEventsAfterStart(uint32_t value);

  /**
   * @brief Retrieves the number of touch events collected between updates once a pinch has started.
   * @SINCE_2_5.40
   * @return The value
   */
  uint32_t GetMinimumTouchEventsAfterStart() const;

public: // Not intended for Application developers
  /// @cond internal
  /**
   * @brief Creates thresholds from internal values.
   * @param[in] values The values to copy
   */
  explicit DALI_INTERNAL PinchThresholds(const Internal::PinchThresholdValues& values);

  /**
   * @brief Retrieves the internal values.
   * @return The values
   */
  DALI_INTERNAL const Internal::PinchThresholdValues& GetValues() const;
  /// @endcond

private:
  struct Impl;
  UniquePtr<Impl> mImpl;
};

/**
 * @brief Retrieves the pinch thresholds currently in effect for devices without a registered entry.
 * @SINCE_2_5.40
 * @return A copy of the thresholds
 */
DALI_CORE_API PinchThresholds GetDefaultPinchThresholds();

/**
 * @brief Registers the pinch thresholds to use for gestures started by the devices matching the selector.
 *
 * The thresholds are copied and replace any registered for the same selector. They apply to every
 * pinch gesture detector of the application from the next gesture sequence on.
 * @SINCE_2_5.40
 * @param[in] selector   The devices the thresholds apply to
 * @param[in] thresholds The thresholds. Invalid values (see the setters) are rejected.
 */
DALI_CORE_API void SetPinchThresholds(const GestureDeviceSelector& selector, const PinchThresholds& thresholds);

/**
 * @brief Retrieves the pinch thresholds registered for exactly this selector.
 *
 * Only thresholds registered with SetPinchThresholds() for the same selector are returned; the fallback order
 * used during recognition is not applied.
 * @SINCE_2_5.40
 * @param[in]  selector   The selector the thresholds were registered with
 * @param[out] thresholds Receives a copy of the thresholds. Left unchanged when none are registered.
 * @return true if thresholds are registered for the selector
 */
DALI_CORE_API bool GetPinchThresholds(const GestureDeviceSelector& selector, PinchThresholds& thresholds);

/**
 * @brief Removes the pinch thresholds registered for exactly this selector.
 *
 * Devices that matched the selector fall back to the next matching selector or the application-wide
 * values from the next gesture sequence on. Does nothing if none are registered for the selector.
 * @SINCE_2_5.40
 * @param[in] selector The selector the thresholds were registered with
 */
DALI_CORE_API void ClearPinchThresholds(const GestureDeviceSelector& selector);

/**
 * @brief The application-wide rotation recognition thresholds.
 *
 * A value type: copies are independent. A default-constructed object holds the DALi built-in
 * defaults; use GetDefaultRotationThresholds() for the values currently in effect.
 * @SINCE_2_5.40
 */
class DALI_CORE_API RotationThresholds
{
public:
  /**
   * @brief Creates thresholds holding the DALi built-in defaults.
   * @SINCE_2_5.40
   */
  RotationThresholds();

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to copy
   */
  RotationThresholds(const RotationThresholds& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to move
   */
  RotationThresholds(RotationThresholds&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to copy
   * @return A reference to this
   */
  RotationThresholds& operator=(const RotationThresholds& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.40
   * @param[in] rhs The thresholds to move
   * @return A reference to this
   */
  RotationThresholds& operator=(RotationThresholds&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_2_5.40
   */
  ~RotationThresholds();

  /**
   * @brief Sets the number of touch events collected before a rotation can start. Must be > 1.
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMinimumTouchEvents(uint32_t value);

  /**
   * @brief Retrieves the number of touch events collected before a rotation can start.
   * @SINCE_2_5.40
   * @return The value
   */
  uint32_t GetMinimumTouchEvents() const;

  /**
   * @brief Sets the number of touch events collected between updates once a rotation has started. Must be > 1.
   * @SINCE_2_5.40
   * @param[in] value The value
   */
  void SetMinimumTouchEventsAfterStart(uint32_t value);

  /**
   * @brief Retrieves the number of touch events collected between updates once a rotation has started.
   * @SINCE_2_5.40
   * @return The value
   */
  uint32_t GetMinimumTouchEventsAfterStart() const;

public: // Not intended for Application developers
  /// @cond internal
  /**
   * @brief Creates thresholds from internal values.
   * @param[in] values The values to copy
   */
  explicit DALI_INTERNAL RotationThresholds(const Internal::RotationThresholdValues& values);

  /**
   * @brief Retrieves the internal values.
   * @return The values
   */
  DALI_INTERNAL const Internal::RotationThresholdValues& GetValues() const;
  /// @endcond

private:
  struct Impl;
  UniquePtr<Impl> mImpl;
};

/**
 * @brief Retrieves the rotation thresholds currently in effect for devices without a registered entry.
 * @SINCE_2_5.40
 * @return A copy of the thresholds
 */
DALI_CORE_API RotationThresholds GetDefaultRotationThresholds();

/**
 * @brief Registers the rotation thresholds to use for gestures started by the devices matching the selector.
 *
 * The thresholds are copied and replace any registered for the same selector. They apply to every
 * rotation gesture detector of the application from the next gesture sequence on.
 * @SINCE_2_5.40
 * @param[in] selector   The devices the thresholds apply to
 * @param[in] thresholds The thresholds. Invalid values (see the setters) are rejected.
 */
DALI_CORE_API void SetRotationThresholds(const GestureDeviceSelector& selector, const RotationThresholds& thresholds);

/**
 * @brief Retrieves the rotation thresholds registered for exactly this selector.
 *
 * Only thresholds registered with SetRotationThresholds() for the same selector are returned; the fallback order
 * used during recognition is not applied.
 * @SINCE_2_5.40
 * @param[in]  selector   The selector the thresholds were registered with
 * @param[out] thresholds Receives a copy of the thresholds. Left unchanged when none are registered.
 * @return true if thresholds are registered for the selector
 */
DALI_CORE_API bool GetRotationThresholds(const GestureDeviceSelector& selector, RotationThresholds& thresholds);

/**
 * @brief Removes the rotation thresholds registered for exactly this selector.
 *
 * Devices that matched the selector fall back to the next matching selector or the application-wide
 * values from the next gesture sequence on. Does nothing if none are registered for the selector.
 * @SINCE_2_5.40
 * @param[in] selector The selector the thresholds were registered with
 */
DALI_CORE_API void ClearRotationThresholds(const GestureDeviceSelector& selector);

} // namespace GestureThresholds

/**
 * @}
 */
} // namespace DALI_NAMESPACE

#endif // DALI_GESTURE_THRESHOLDS_H
