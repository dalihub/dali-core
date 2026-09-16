#ifndef DALI_LONG_PRESS_GESTURE_DETECTOR_H
#define DALI_LONG_PRESS_GESTURE_DETECTOR_H

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
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/events/gesture-detector.h>
#include <dali/public-api/events/gesture-device-selector.h>
#include <dali/public-api/signals/dali-signal.h>

namespace DALI_NAMESPACE
{
/**
 * @addtogroup dali_core_events
 * @{
 */

namespace Internal DALI_INTERNAL
{
class LongPressGestureDetector;
struct LongPressGestureProfile;
} //namespace Internal DALI_INTERNAL

class LongPressGesture;

/**
 * @brief This class emits a signals when a long press gesture occurs that meets the requirements set by the application.
 * @SINCE_1_0.0
 *
 * For any valid long press, two signals will be emitted:
 * - First identifying the beginning (state = GestureState::STARTED) i.e. when fingers held down for the required time.
 * - Second identifying the ending (state = GestureState::FINISHED) i.e. when fingers are released.
 *
 * The application programmer can use this gesture detector as follows:
 * @code
 * LongPressGestureDetector detector = LongPressGestureDetector::New();
 * detector.Attach(myActor);
 * detector.DetectedSignal().Connect(this, &MyApplication::OnLongPress);
 * @endcode
 *
 * @see LongPressGesture
 *
 * Signals
 * | %Signal Name      | Method                |
 * |-------------------|-----------------------|
 * | longPressDetected | @ref DetectedSignal() |
 */
class DALI_CORE_API LongPressGestureDetector : public GestureDetector
{
public:                                                             // Typedefs
  using DetectedSignalType = Signal<void(Actor, LongPressGesture)>; ///< Gesture detected signal type @SINCE_1_0.0

  /**
   * @brief A complete set of long press recognition options for one input device profile.
   *
   * Holds the same options as the detector itself: the touch count range.
   *
   * Register a profile for a device with SetDeviceOptions(). The usual way to build one is to copy
   * the detector's default options and change what differs:
   * @code
   * LongPressGestureDetector::Options touch = detector.GetDefaultOptions();
   * touch.SetTouchesRequired(2u); // two fingers on a touch screen
   * detector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH), touch);
   * @endcode
   *
   * This is a value type: copies are independent.
   * @SINCE_2_5.40
   */
  class DALI_CORE_API Options
  {
  public:
    /**
     * @brief Creates options with the detector default: exactly one touch.
     * @SINCE_2_5.40
     */
    Options();

    /**
     * @brief Copy constructor.
     * @SINCE_2_5.40
     * @param[in] rhs The options to copy
     */
    Options(const Options& rhs);

    /**
     * @brief Move constructor.
     * @SINCE_2_5.40
     * @param[in] rhs The options to move
     */
    Options(Options&& rhs) noexcept;

    /**
     * @brief Copy assignment operator.
     * @SINCE_2_5.40
     * @param[in] rhs The options to copy
     * @return A reference to this
     */
    Options& operator=(const Options& rhs);

    /**
     * @brief Move assignment operator.
     * @SINCE_2_5.40
     * @param[in] rhs The options to move
     * @return A reference to this
     */
    Options& operator=(Options&& rhs) noexcept;

    /**
     * @brief Destructor.
     * @SINCE_2_5.40
     */
    ~Options();

    /**
     * @brief Sets the number of touches required by these options.
     *
     * The number of touches corresponds to the number of fingers a user has on the screen.
     * This sets the minimum and maximum touches to the input parameter.
     *
     * @SINCE_2_5.40
     * @param[in] touches Touches required
     * @note The default is '1'.
     * @see Dali::LongPressGestureDetector::SetTouchesRequired(uint32_t)
     */
    void SetTouchesRequired(uint32_t touches);

    /**
     * @brief Sets the minimum and maximum touches required by these options.
     *
     * The number of touches corresponds to the number of fingers a user has on the screen.
     *
     * @SINCE_2_5.40
     * @param[in] minTouches Minimum Touches required
     * @param[in] maxTouches Maximum Touches required
     * @note The default is '1'.
     * @see Dali::LongPressGestureDetector::SetTouchesRequired(uint32_t,uint32_t)
     */
    void SetTouchesRequired(uint32_t minTouches, uint32_t maxTouches);

    /**
     * @brief Retrieves the minimum number of touches required by these options.
     *
     * @SINCE_2_5.40
     * @return The minimum number of touches required
     */
    uint32_t GetMinimumTouchesRequired() const;

    /**
     * @brief Retrieves the maximum number of touches required by these options.
     *
     * @SINCE_2_5.40
     * @return The maximum number of touches required
     */
    uint32_t GetMaximumTouchesRequired() const;

  public: // Not intended for Application developers
    /// @cond internal
    /**
     * @brief Creates options from an internal profile.
     * @param[in] profile The profile to copy
     */
    explicit DALI_INTERNAL Options(const Internal::LongPressGestureProfile& profile);

    /**
     * @brief Retrieves the internal profile.
     * @return The profile
     */
    DALI_INTERNAL const Internal::LongPressGestureProfile& GetProfile() const;
    /// @endcond

  private:
    struct Impl;
    UniquePtr<Impl> mImpl;
  };

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized LongPressGestureDetector; this can be initialized with LongPressGestureDetector::New().
   *
   * Calling member functions with an uninitialized LongPressGestureDetector handle is not allowed.
   * @SINCE_1_0.0
   */
  LongPressGestureDetector();

  /**
   * @brief Creates an initialized LongPressGestureDetector.
   *
   * By default, this would create a gesture detector that requires only one touch.
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource
   */
  static LongPressGestureDetector New();

  /**
   * @brief Creates an initialized LongPressGestureDetector with the number of touches required.
   *
   * A long press gesture will be emitted from this detector if the number of fingers touching the
   * screen is equal to the touches required.
   * @SINCE_1_0.0
   * @param[in] touchesRequired The number of touches required
   * @return A handle to a newly allocated Dali resource
   */
  static LongPressGestureDetector New(uint32_t touchesRequired);

  /**
   * @brief Creates an initialized LongPressGestureDetector with the minimum and maximum number of touches required.
   *
   * A long press gesture will be emitted from this detector if the number of fingers touching the screen
   * falls between the minimum and maximum touches set.
   * @SINCE_1_0.0
   * @param[in] minTouches The minimum number of touches required
   * @param[in] maxTouches The maximum number of touches required
   * @return A handle to a newly allocated Dali resource
   */
  static LongPressGestureDetector New(uint32_t minTouches, uint32_t maxTouches);

  /**
   * @brief Downcasts a handle to LongPressGestureDetector handle.
   *
   * If handle points to a LongPressGestureDetector object, the
   * downcast produces valid handle. If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a LongPressGestureDetector object or an uninitialized handle
   */
  static LongPressGestureDetector DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~LongPressGestureDetector();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  LongPressGestureDetector(const LongPressGestureDetector& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  LongPressGestureDetector& operator=(const LongPressGestureDetector& rhs);

  /**
   * @brief This move constructor is required for (smart) pointer semantics.
   *
   * @SINCE_2_2.4
   * @param[in] handle A reference to the moved handle
   */
  LongPressGestureDetector(LongPressGestureDetector&& handle) noexcept;

  /**
   * @brief This move assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_2_2.4
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  LongPressGestureDetector& operator=(LongPressGestureDetector&& rhs) noexcept;

public: // Setters
  /**
   * @brief Sets the number of touches required.
   *
   * The number of touches corresponds to the number of fingers a user
   * has on the screen.  This sets the minimum and maximum touches to
   * the input parameter.
   *
   * @SINCE_1_0.0
   * @param[in] touches Touches required
   * @pre The gesture detector has been initialized.
   * @note The default is '1'.
   */
  void SetTouchesRequired(uint32_t touches);

  /**
   * @brief Sets the minimum and maximum touches required.
   *
   * The number of touches corresponds to the number of fingers a user
   * has on the screen.
   *
   * @SINCE_1_0.0
   * @param[in] minTouches Minimum Touches required
   * @param[in] maxTouches Maximum Touches required
   * @pre The gesture detector has been initialized.
   * @note The default is '1'.
   */
  void SetTouchesRequired(uint32_t minTouches, uint32_t maxTouches);

public: // Getters
  /**
   * @brief Retrieves the minimum number of touches required.
   *
   * @SINCE_1_0.0
   * @return The minimum number of touches required
   * @pre The gesture detector has been initialized.
   */
  uint32_t GetMinimumTouchesRequired() const;

  /**
   * @brief Retrieves the maximum number of touches required.
   *
   * @SINCE_1_0.0
   * @return The maximum number of touches required
   * @pre The gesture detector has been initialized.
   */
  uint32_t GetMaximumTouchesRequired() const;

public: // Per-device options
  /**
   * @brief Retrieves a copy of the options that apply to devices without a registered profile.
   *
   * These are the values set through SetTouchesRequired(). The copy is independent of the detector.
   *
   * @SINCE_2_5.40
   * @return The default options
   * @pre The gesture detector has been initialized.
   */
  Options GetDefaultOptions() const;

  /**
   * @brief Registers the options to use for long presses made by the devices matching the selector.
   *
   * The options are copied and replace any options previously registered for the same selector.
   * When a long press starts, the detector picks the options in this order: a matching device-name
   * selector, then a matching class-and-subclass selector, then a matching class selector, then the
   * default options.
   *
   * @SINCE_2_5.40
   * @param[in] selector The devices the options apply to
   * @param[in] options  The options. Minimum and maximum touches must be greater than zero and
   *                     minimum must not exceed maximum.
   * @pre The gesture detector has been initialized.
   */
  void SetDeviceOptions(const GestureDeviceSelector& selector, const Options& options);

  /**
   * @brief Retrieves the options registered for exactly this selector.
   *
   * Only options registered with SetDeviceOptions() for the same selector are returned; the
   * fallback order used during recognition is not applied.
   *
   * @SINCE_2_5.40
   * @param[in]  selector The selector the options were registered with
   * @param[out] options  Receives a copy of the options. Left unchanged when none are registered.
   * @return true if options are registered for the selector
   * @pre The gesture detector has been initialized.
   */
  bool GetDeviceOptions(const GestureDeviceSelector& selector, Options& options) const;

  /**
   * @brief Removes the options registered for exactly this selector.
   *
   * Devices that matched the selector fall back to the next matching selector or the default
   * options from the next long press on. Does nothing if no options are registered for the selector.
   *
   * @SINCE_2_5.40
   * @param[in] selector The selector the options were registered with
   * @pre The gesture detector has been initialized.
   */
  void ClearDeviceOptions(const GestureDeviceSelector& selector);

public: // Signals
  /**
   * @brief This signal is emitted when the specified long press is detected on the attached actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, LongPressGesture gesture );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The gesture detector has been initialized.
   */
  DetectedSignalType& DetectedSignal();

public: // Not intended for Application developers
  /// @cond internal
  /**
   * @brief This constructor is used by LongPressGestureDetector::New() methods.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL LongPressGestureDetector(Internal::LongPressGestureDetector* internal);
  /// @endcond
};

/**
 * @}
 */
} //namespace DALI_NAMESPACE

#endif // DALI_LONG_PRESS_GESTURE_DETECTOR_H
