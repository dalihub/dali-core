#ifndef DALI_TAP_GESTURE_DETECTOR_H
#define DALI_TAP_GESTURE_DETECTOR_H

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
class TapGestureDetector;
struct TapGestureProfile;
} //namespace Internal DALI_INTERNAL

class TapGesture;

/**
 * @brief This class emits a signal when a tap gesture occurs that meets the requirements set by the
 * application.
 *
 * See @ref TapGestureDetector::SetMinimumTapsRequired
 * See @ref TapGestureDetector::SetMaximumTapsRequired
 *
 * A TapGesture is a discrete gesture, which means it does not have any state information attached
 * to it.  Please see TapGesture for more information.
 *
 * The application programmer can use this gesture detector as follows:
 * @code
 * TapGestureDetector detector = TapGestureDetector::New();
 * detector.Attach(myActor);
 * detector.DetectedSignal().Connect(this, &MyApplication::OnTap);
 * @endcode
 *
 * @SINCE_1_0.0
 * @note Multi-touch taps (two or more points of contact with the surface) are not currently
 * supported. However, multiple taps (double & triple tap etc.) are supported.
 *
 * Signals
 * | %Signal Name | Method                |
 * |--------------|-----------------------|
 * | tapDetected  | @ref DetectedSignal() |
 * @see TapGesture
 *
 */
class DALI_CORE_API TapGestureDetector : public GestureDetector
{
public: // Typedefs
  /**
   * @brief Signal type for detected signal.
   * @SINCE_1_0.0
   */
  using DetectedSignalType = Signal<void(Actor, TapGesture)>;

  /**
   * @brief A complete set of tap recognition options for one input device profile.
   *
   * Holds the same options as the detector itself: the tap count range and whether every tap in
   * the range is delivered immediately.
   *
   * Register a profile for a device with SetDeviceOptions(). The usual way to build one is to copy
   * the detector's default options and change what differs:
   * @code
   * TapGestureDetector::Options remote = detector.GetDefaultOptions();
   * remote.SetMaximumTapsRequired(1u); // no double tap from the remote controller
   * detector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::POINTER, Device::Subclass::REMOCON), remote);
   * @endcode
   *
   * This is a value type: copies are independent.
   * @SINCE_2_5.40
   */
  class DALI_CORE_API Options
  {
  public:
    /**
     * @brief Creates options with the detector defaults: exactly one tap, not receiving every tap event.
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
     * @brief Sets the minimum number of taps required by these options.
     *
     * The tap count is the number of times a user should "tap" the screen.
     * @SINCE_2_5.40
     * @param[in] minimumTaps The minimum taps required
     * @note The default is '1', the maximum is 2.
     * @see Dali::TapGestureDetector::SetMinimumTapsRequired()
     */
    void SetMinimumTapsRequired(uint32_t minimumTaps);

    /**
     * @brief Sets the maximum number of taps required by these options.
     *
     * The tap count is the number of times a user should "tap" the screen.
     * @SINCE_2_5.40
     * @param[in] maximumTaps The maximum taps required
     * @note The default is '1', the maximum is 2.
     * @see Dali::TapGestureDetector::SetMaximumTapsRequired()
     */
    void SetMaximumTapsRequired(uint32_t maximumTaps);

    /**
     * @brief Sets whether every tap between the minimum and maximum is delivered immediately.
     * @SINCE_2_5.40
     * @param[in] enabled true to deliver every tap, false to wait for the maximum or the multi-tap timeout
     * @see Dali::TapGestureDetector::ReceiveAllTapEvents()
     */
    void SetReceiveAllTapEventsEnabled(bool enabled);

    /**
     * @brief Retrieves the minimum number of taps required by these options.
     *
     * @SINCE_2_5.40
     * @return The minimum taps required
     */
    uint32_t GetMinimumTapsRequired() const;

    /**
     * @brief Retrieves the maximum number of taps required by these options.
     *
     * @SINCE_2_5.40
     * @return The maximum taps required
     */
    uint32_t GetMaximumTapsRequired() const;

    /**
     * @brief Retrieves whether every tap between the minimum and maximum is delivered immediately.
     * @SINCE_2_5.40
     * @return true if every tap is delivered
     */
    bool IsReceiveAllTapEventsEnabled() const;

  public: // Not intended for Application developers
    /// @cond internal
    /**
     * @brief Creates options from an internal profile.
     * @param[in] profile The profile to copy
     */
    explicit DALI_INTERNAL Options(const Internal::TapGestureProfile& profile);

    /**
     * @brief Retrieves the internal profile.
     * @return The profile
     */
    DALI_INTERNAL const Internal::TapGestureProfile& GetProfile() const;
    /// @endcond

  private:
    struct Impl;
    UniquePtr<Impl> mImpl;
  };

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized TapGestureDetector; this can be initialized with TapGestureDetector::New().
   *
   * Calling member functions with an uninitialized TapGestureDetector handle is not allowed.
   * @SINCE_1_0.0
   */
  TapGestureDetector();

  /**
   * @brief Creates an initialized TapGestureDetector.
   *
   * By default, this would create a gesture detector which requires one tap with one touch.
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource
   */
  static TapGestureDetector New();

  /**
   * @brief Creates an initialized TapGestureDetector with the specified parameters.
   *
   * @SINCE_1_0.0
   * @param[in] tapsRequired The minimum & maximum number of taps required
   * @return A handle to a newly allocated Dali resource
   */
  static TapGestureDetector New(uint32_t tapsRequired);

  /**
   * @brief Downcasts a handle to TapGestureDetector handle.
   *
   * If handle points to a TapGestureDetector object, the
   * downcast produces valid handle. If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a TapGestureDetector object or an uninitialized handle
   */
  static TapGestureDetector DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~TapGestureDetector();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  TapGestureDetector(const TapGestureDetector& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  TapGestureDetector& operator=(const TapGestureDetector& rhs);

  /**
   * @brief This move constructor is required for (smart) pointer semantics.
   *
   * @SINCE_2_2.4
   * @param[in] handle A reference to the moved handle
   */
  TapGestureDetector(TapGestureDetector&& handle) noexcept;

  /**
   * @brief This move assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_2_2.4
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  TapGestureDetector& operator=(TapGestureDetector&& rhs) noexcept;

public: // Setters
  /**
   * @brief Sets the minimum number of taps required.
   *
   * The tap count is the number of times a user should "tap" the screen.
   * @SINCE_1_0.0
   * @param[in] minimumTaps The minimum taps required
   * @pre The gesture detector has been initialized.
   * @note The default is '1', the maximum is 2.
   * @see ReceiveAllTapEvents
   */
  void SetMinimumTapsRequired(uint32_t minimumTaps);

  /**
   * @brief Sets the maximum number of taps required.
   *
   * The tap count is the number of times a user should "tap" the screen.
   * @SINCE_1_0.0
   * @param[in] maximumTaps The maximum taps required
   * @pre The gesture detector has been initialized.
   * @note The default is '1', the maximum is 2.
   * @see ReceiveAllTapEvents
   */
  void SetMaximumTapsRequired(uint32_t maximumTaps);

  /**
   * @brief When set to true, all tap gestures will be received when multiple taps are supported by the gesture detector.
   *
   * @param[in] receive The receiving all tap events flag
   * @pre The gesture detector has been initialized.
   * @note The default is false.
   */
  void ReceiveAllTapEvents(bool receive);

public: // Getters
  /**
   * @brief Retrieves the minimum number of taps required.
   *
   * @SINCE_1_0.0
   * @return The minimum taps required
   * @pre The gesture detector has been initialized.
   */
  uint32_t GetMinimumTapsRequired() const;

  /**
   * @brief Retrieves the maximum number of taps required.
   *
   * @SINCE_1_0.0
   * @return The maximum taps required
   * @pre The gesture detector has been initialized.
   */
  uint32_t GetMaximumTapsRequired() const;

  /**
   * @brief Retrieves whether every tap between the minimum and maximum is delivered immediately.
   *
   * @SINCE_2_5.40
   * @return true if every tap is delivered
   * @pre The gesture detector has been initialized.
   * @see ReceiveAllTapEvents()
   */
  bool IsReceiveAllTapEventsEnabled() const;

public: // Per-device options
  /**
   * @brief Retrieves a copy of the options that apply to devices without a registered profile.
   *
   * These are the values set through the detector's own setters (SetMinimumTapsRequired(),
   * ReceiveAllTapEvents(), ...). The copy is independent of the detector.
   *
   * @SINCE_2_5.40
   * @return The default options
   * @pre The gesture detector has been initialized.
   */
  Options GetDefaultOptions() const;

  /**
   * @brief Registers the options to use for taps made by the devices matching the selector.
   *
   * The options are copied and replace any options previously registered for the same selector.
   * When a tap sequence starts, the detector picks the options in this order: a matching device-name
   * selector, then a matching class-and-subclass selector, then a matching class selector, then the
   * default options. The choice is kept for every tap of that sequence. Taps made by different
   * devices never combine into one multi-tap.
   *
   * @SINCE_2_5.40
   * @param[in] selector The devices the options apply to
   * @param[in] options  The options. Minimum and maximum taps must be greater than zero and minimum
   *                     must not exceed maximum.
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
   * options from the next tap sequence on. Does nothing if no options are registered for the selector.
   *
   * @SINCE_2_5.40
   * @param[in] selector The selector the options were registered with
   * @pre The gesture detector has been initialized.
   */
  void ClearDeviceOptions(const GestureDeviceSelector& selector);

public: // Signals
  /**
   * @brief This signal is emitted when the specified tap is detected on the attached actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, TapGesture gesture );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The gesture detector has been initialized.
   */
  DetectedSignalType& DetectedSignal();

public: // Not intended for Application developers
  /// @cond internal
  /**
   * @brief This constructor is used by TapGestureDetector::New() methods.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL TapGestureDetector(Internal::TapGestureDetector* internal);
  /// @endcond
};

/**
 * @}
 */

} //namespace DALI_NAMESPACE

#endif // DALI_TAP_GESTURE_DETECTOR_H
