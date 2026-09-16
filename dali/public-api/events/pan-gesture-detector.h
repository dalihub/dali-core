#ifndef DALI_PAN_GESTURE_DETECTOR_H
#define DALI_PAN_GESTURE_DETECTOR_H

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
#include <dali/public-api/common/dali-pair.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/events/gesture-detector.h>
#include <dali/public-api/events/gesture-device-selector.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/signals/dali-signal.h>

namespace DALI_NAMESPACE
{
struct Radian;

namespace Internal DALI_INTERNAL
{
class PanGestureDetector;
struct PanGestureProfile;
} //namespace Internal DALI_INTERNAL

/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief This class looks for panning (or dragging) gestures.
 *
 * The user will be pressing one or more fingers on an actor while they pan it.
 *
 * The application programmer can use this gesture detector as follows:
 * @code
 * PanGestureDetector detector = PanGestureDetector::New();
 * detector.Attach(myActor);
 * detector.DetectedSignal().Connect(this, &MyApplication::OnPan);
 *
 * // Detect pan gesture for single and double touch.
 * detector.SetMaximumTouchesRequired(2);
 * @endcode
 *
 * @SINCE_1_0.0
 * @see PanGesture
 *
 * Signals
 * | %Signal Name | Method                |
 * |--------------|-----------------------|
 * | panDetected  | @ref DetectedSignal() |
 */
class DALI_CORE_API PanGestureDetector : public GestureDetector
{
public:
  /**
   * @brief Enumeration for the instance of properties belonging to the PanGestureDetector class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the PanGestureDetector class.
     * @SINCE_1_0.0
     */
    enum
    {
      SCREEN_POSITION = DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX, ///< name "screenPosition",      type Vector2 @SINCE_1_0.0
      SCREEN_DISPLACEMENT,                                             ///< name "screenDisplacement",  type Vector2 @SINCE_1_0.0
      SCREEN_VELOCITY,                                                 ///< name "screenVelocity",      type Vector2 @SINCE_1_0.0
      LOCAL_POSITION,                                                  ///< name "localPosition",       type Vector2 @SINCE_1_0.0
      LOCAL_DISPLACEMENT,                                              ///< name "localDisplacement",   type Vector2 @SINCE_1_0.0
      LOCAL_VELOCITY,                                                  ///< name "localVelocity",       type Vector2 @SINCE_1_0.0
      PANNING,                                                         ///< name "panning",             type bool @SINCE_1_0.0
    };
  };

  // Typedefs
  using DetectedSignalType = Signal<void(Actor, PanGesture)>; ///< Pan gesture detected signal type @SINCE_1_0.0

  // Directional Pan
  using AngleThresholdPair = Dali::Pair<Radian, Radian>; ///< Range of angles for a direction @SINCE_1_0.0

  static const Radian DIRECTION_LEFT;       ///< For a left pan (-PI Radians).
  static const Radian DIRECTION_RIGHT;      ///< For a right pan (0 Radians).
  static const Radian DIRECTION_UP;         ///< For an up pan (-0.5 * PI Radians).
  static const Radian DIRECTION_DOWN;       ///< For a down pan (0.5 * PI Radians).
  static const Radian DIRECTION_HORIZONTAL; ///< For a left and right pan (PI Radians). Useful for AddDirection().
  static const Radian DIRECTION_VERTICAL;   ///< For an up and down pan (-0.5 * PI Radians). Useful for AddDirection().

  static const Radian DEFAULT_THRESHOLD; ///< The default threshold is PI * 0.25 radians (or 45 degrees).

  /**
   * @brief A complete set of pan recognition options for one input device profile.
   *
   * Holds the same options as the detector itself: the touch count range, the maximum motion event
   * age and the allowed pan angles. Angles are normalised exactly as by the detector's AddAngle().
   *
   * Register a profile for a device with SetDeviceOptions(). The usual way to build one is to copy
   * the detector's default options and change what differs:
   * @code
   * PanGestureDetector::Options remote = detector.GetDefaultOptions();
   * remote.SetMaximumMotionEventAge(120u);
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
     * @brief Creates options with the detector defaults: one touch, unlimited motion event age, any direction.
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
     * @brief Sets the minimum number of touches required for the pan gesture to be detected with these options.
     *
     * @SINCE_2_5.40
     * @param[in] minimum Minimum touches required
     * @note The default minimum is '1'.
     * @see Dali::PanGestureDetector::SetMinimumTouchesRequired()
     */
    void SetMinimumTouchesRequired(uint32_t minimum);

    /**
     * @brief Sets the maximum number of touches required for the pan gesture to be detected with these options.
     *
     * @SINCE_2_5.40
     * @param[in] maximum Maximum touches required
     * @note The default maximum is '1'.
     * @see Dali::PanGestureDetector::SetMaximumTouchesRequired()
     */
    void SetMaximumTouchesRequired(uint32_t maximum);

    /**
     * @brief Sets the maximum duration a motion event may live on the pan gesture event queue with these options.
     * If the duration is exceeded, the motion event is discarded.
     *
     * @SINCE_2_5.40
     * @param[in] maximumAge Maximum age of motion events as milliseconds
     * @note The default maximumAge is 'std::numeric_limits<uint32_t>::max()'.
     * @see Dali::PanGestureDetector::SetMaximumMotionEventAge()
     */
    void SetMaximumMotionEventAge(uint32_t maximumAge);

    /**
     * @brief Retrieves the minimum number of touches required by these options.
     *
     * @SINCE_2_5.40
     * @return The minimum touches required
     */
    uint32_t GetMinimumTouchesRequired() const;

    /**
     * @brief Retrieves the maximum number of touches required by these options.
     *
     * @SINCE_2_5.40
     * @return The maximum touches required
     */
    uint32_t GetMaximumTouchesRequired() const;

    /**
     * @brief Retrieves the maximum age for pan gesture motion events in these options, as milliseconds.
     *
     * @SINCE_2_5.40
     * @return The maximum age of motion events as milliseconds
     */
    uint32_t GetMaximumMotionEventAge() const;

    /**
     * @brief Adds an angle, with a +/- threshold allowance, in which the pan gesture is emitted with these options.
     *
     * The angle range, wrapping and threshold rules are those of Dali::PanGestureDetector::AddAngle().
     *
     * @SINCE_2_5.40
     * @param[in] angle     The angle that pan should be allowed
     * @param[in] threshold The threshold around that angle
     * @note If no threshold is provided, then the default threshold (PI * 0.25) is used.
     * @see Dali::PanGestureDetector::AddAngle()
     */
    void AddAngle(Radian angle, Radian threshold = DEFAULT_THRESHOLD);

    /**
     * @brief Adds a bi-directional angle in which the pan gesture is emitted with these options.
     *
     * In other words, if 0 is requested, then PI will also be added so that both left and right panning are allowed.
     *
     * @SINCE_2_5.40
     * @param[in] direction The direction of panning required
     * @param[in] threshold The threshold around that direction
     * @note If no threshold is provided, then the default threshold (PI * 0.25) is used.
     * @see Dali::PanGestureDetector::AddDirection()
     */
    void AddDirection(Radian direction, Radian threshold = DEFAULT_THRESHOLD);

    /**
     * @brief Returns the count of angles in these options.
     *
     * @SINCE_2_5.40
     * @return The count
     */
    uint32_t GetAngleCount() const;

    /**
     * @brief Returns the angle by index in these options.
     *
     * @SINCE_2_5.40
     * @param[in] index The angle's index
     * @return An angle threshold pair, or a zero valued angle pair when index is invalid
     * @pre The index is less than GetAngleCount()
     */
    AngleThresholdPair GetAngle(uint32_t index) const;

    /**
     * @brief Clears any directional angles from these options.
     *
     * After this, the pan gesture will be emitted for a pan in ANY direction.
     * @SINCE_2_5.40
     */
    void ClearAngles();

    /**
     * @brief Removes the angle specified from these options.
     *
     * @SINCE_2_5.40
     * @param[in] angle The angle to remove
     * @note This will only remove the first instance of the angle found.
     * @see Dali::PanGestureDetector::RemoveAngle()
     */
    void RemoveAngle(Radian angle);

    /**
     * @brief Removes the two angles that make up the direction from these options.
     *
     * @SINCE_2_5.40
     * @param[in] direction The direction to remove
     * @see Dali::PanGestureDetector::RemoveDirection()
     */
    void RemoveDirection(Radian direction);

  public: // Not intended for Application developers
    /// @cond internal
    /**
     * @brief Creates options from an internal profile.
     * @param[in] profile The profile to copy
     */
    explicit DALI_INTERNAL Options(const Internal::PanGestureProfile& profile);

    /**
     * @brief Retrieves the internal profile.
     * @return The profile
     */
    DALI_INTERNAL const Internal::PanGestureProfile& GetProfile() const;
    /// @endcond

  private:
    struct Impl;
    UniquePtr<Impl> mImpl;
  };

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized PanGestureDetector; this can be initialized with PanGestureDetector::New().
   *
   * Calling member functions with an uninitialized PanGestureDetector handle is not allowed.
   * @SINCE_1_0.0
   */
  PanGestureDetector();

  /**
   * @brief Creates an initialized PanGestureDetector.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource
   */
  static PanGestureDetector New();

  /**
   * @brief Downcasts a handle to PanGestureDetector handle.
   *
   * If handle points to a PanGestureDetector object, the
   * downcast produces valid handle. If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a PanGestureDetector object or an uninitialized handle
   */
  static PanGestureDetector DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~PanGestureDetector();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  PanGestureDetector(const PanGestureDetector& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  PanGestureDetector& operator=(const PanGestureDetector& rhs);

  /**
   * @brief This move constructor is required for (smart) pointer semantics.
   *
   * @SINCE_2_2.4
   * @param[in] handle A reference to the moved handle
   */
  PanGestureDetector(PanGestureDetector&& handle) noexcept;

  /**
   * @brief This move assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_2_2.4
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  PanGestureDetector& operator=(PanGestureDetector&& rhs) noexcept;

public: // Setters
  /**
   * @brief This is the minimum number of touches required for the pan gesture to be detected.
   *
   * @SINCE_1_0.0
   * @param[in] minimum Minimum touches required
   * @pre The gesture detector has been initialized.
   * @note The default minimum is '1'.
   */
  void SetMinimumTouchesRequired(uint32_t minimum);

  /**
   * @brief This is the maximum number of touches required for the pan gesture to be detected.
   *
   * @SINCE_1_0.0
   * @param[in] maximum Maximum touches required
   * @pre The gesture detector has been initialized.
   * @note The default maximum is '1'.
   */
  void SetMaximumTouchesRequired(uint32_t maximum);

  /**
   * @brief This value is a maximum duration of motion can live on the pan gesture event queue.
   * If duration exceed it, the motion event is discarded.
   *
   * @SINCE_2_1.28
   * @param[in] maximumAge Maximum age of motion events as milliseconds
   * @pre The gesture detector has been initialized.
   * @note The default maximumAge is 'std::numeric_limits<uint32_t>::max()'.
   */
  void SetMaximumMotionEventAge(uint32_t maximumAge);

public: // Getters
  /**
   * @brief Retrieves the minimum number of touches required for the pan gesture to be detected.
   *
   * @SINCE_1_0.0
   * @return The minimum touches required
   * @pre The gesture detector has been initialized.
   */
  uint32_t GetMinimumTouchesRequired() const;

  /**
   * @brief Retrieves the maximum number of touches required for the pan gesture to be detected.
   *
   * @SINCE_1_0.0
   * @return The maximum touches required
   * @pre The gesture detector has been initialized.
   */
  uint32_t GetMaximumTouchesRequired() const;

  /**
   * @brief Retrieves the maximum age for the pan gesture motion as milliseconds.
   *
   * @SINCE_2_1.28
   * @return The maximum age of motion events as milliseconds
   * @pre The gesture detector has been initialized.
   */
  uint32_t GetMaximumMotionEventAge() const;

public: // Directional Panning
  /**
   * @brief The pan gesture is only emitted if the pan occurs in the direction specified by this method with a +/- threshold allowance.
   *
   * The angle is from -180 -> 0 -> 180 degrees (or -M_PI -> 0 -> M_PI in radians) i.e:
   *
   * @code
   *           -90.0f ( -0.5f * PI )
   *                     |
   *                     |
   * 180.0f ( PI ) ------------- 0.0f ( 0.0f )
   *                     |
   *                     |
   *            90.0f ( 0.5f * PI )
   * @endcode
   *
   * If an angle of 0.0 degrees is specified and the threshold is 45 degrees then the acceptable
   * direction range is from -45 to 45 degrees.
   *
   * @SINCE_1_0.0
   * @param[in] angle     The angle that pan should be allowed
   * @param[in] threshold The threshold around that angle
   *
   * @pre The gesture detector has been initialized.
   * @note The angle added using this API is only checked when the gesture first starts, after that,
   *       this detector will emit the gesture regardless of what angle the pan is moving.
   * @note The user can add as many angles as they require.
   * @note If an angle outside the range above is given, then it is wrapped within the range, i.e.
   *       190 degrees becomes -170 degrees and 370 degrees becomes 10 degrees.
   * @note As long as you specify the type, you can also pass in a Dali::Degree to this method.
   * @note If no threshold is provided, then the default threshold (PI * 0.25) is used.
   * @note If the threshold is greater than PI, then PI will be used as the threshold.
   *
   */
  void AddAngle(Radian angle, Radian threshold = DEFAULT_THRESHOLD);

  /**
   * @brief A helper method for adding bi-directional angles where the pan should take place.
   *
   * In other words, if 0 is requested, then PI will also be added so that we have both left and
   * right scrolling.
   *
   * @SINCE_1_0.0
   * @param[in] direction The direction of panning required
   * @param[in] threshold The threshold
   *
   * @pre The gesture detector has been initialized.
   *
   * @note If a direction outside the range above is given, then it is wrapped within the range, i.e.
   *       190 degrees becomes -170 degrees and 370 degrees becomes 10 degrees.
   * @note If no threshold is provided, then the default threshold (PI * 0.25) is used.
   * @note If the threshold is greater than PI, then PI will be used as the threshold.
   * @note As long as you specify the type, you can also pass in a Dali::Degree to this method.
   *
   * @see AddAngle
   */
  void AddDirection(Radian direction, Radian threshold = DEFAULT_THRESHOLD);

  /**
   * @brief Returns the count of angles that this pan gesture detector emits a signal.
   *
   * @SINCE_1_0.0
   * @return The count
   * @pre The gesture detector has been initialized.
   */
  uint32_t GetAngleCount() const;

  /**
   * @brief Returns the angle by index that this pan gesture detector emits a signal.
   *
   * @SINCE_1_0.0
   * @param[in] index The angle's index
   * @return An angle threshold pair, or a zero valued angle pair when index is invalid
   * @pre The gesture detector has been initialized.
   * @pre The index is less than GetAngleCount()
   */
  AngleThresholdPair GetAngle(uint32_t index) const;

  /**
   * @brief Clears any directional angles that are used by the gesture detector.
   *
   * After this, the pan gesture
   * will be emitted for a pan in ANY direction.
   * @SINCE_1_0.0
   * @pre The gesture detector has been initialized.
   */
  void ClearAngles();

  /**
   * @brief Removes the angle specified from the container.
   *
   * @SINCE_1_0.0
   * @param[in] angle The angle to remove
   * @pre The gesture detector has been initialized.
   * @note This will only remove the first instance of the angle found from the container.
   * @note If an angle outside the range in AddAngle() is given, then the value is wrapped within
   *       the range and that is removed.
   */
  void RemoveAngle(Radian angle);

  /**
   * @brief Removes the two angles that make up the direction from the container.
   *
   * @SINCE_1_0.0
   * @param[in] direction The direction to remove
   * @pre The gesture detector has been initialized.
   * @note If a direction outside the range in AddAngle() is given, then the value is wrapped within
   *       the range and that is removed.
   */
  void RemoveDirection(Radian direction);

public: // Per-device options
  /**
   * @brief Retrieves a copy of the options that apply to devices without a registered profile.
   *
   * These are the values set through the detector's own setters (SetMinimumTouchesRequired(),
   * AddAngle(), ...). The copy is independent of the detector.
   *
   * @SINCE_2_5.40
   * @return The default options
   * @pre The gesture detector has been initialized.
   */
  Options GetDefaultOptions() const;

  /**
   * @brief Registers the options to use for gestures started by the devices matching the selector.
   *
   * The options are copied and replace any options previously registered for the same selector.
   * When a pan starts, the detector picks the options in this order: a matching device-name
   * selector, then a matching class-and-subclass selector, then a matching class selector, then
   * the default options. The choice is made once per gesture and does not change while that gesture
   * is in progress.
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
   * options from the next gesture on. Does nothing if no options are registered for the selector.
   *
   * @SINCE_2_5.40
   * @param[in] selector The selector the options were registered with
   * @pre The gesture detector has been initialized.
   */
  void ClearDeviceOptions(const GestureDeviceSelector& selector);

public: // Signals
  /**
   * @brief This signal is emitted when the pan gesture is detected on the attached actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, PanGesture gesture );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The gesture detector has been initialized.
   */
  DetectedSignalType& DetectedSignal();

public: // Pan Properties Setters
  /**
   * @brief Allows setting of the pan properties that are returned in constraints.
   *
   * @SINCE_1_0.0
   * @param[in] pan The pan gesture to set
   * @note If a normal pan is taking place, then any value set is ignored.
   */
  static void SetPanGestureProperties(const PanGesture& pan);

public: // Not intended for Application developers
  /// @cond internal
  /**
   * @brief This constructor is used by PanGestureDetector::New() methods.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL PanGestureDetector(Internal::PanGestureDetector* internal);
  /// @endcond
};

/**
 * @}
 */

} //namespace DALI_NAMESPACE

#endif // DALI_PAN_GESTURE_DETECTOR_H
