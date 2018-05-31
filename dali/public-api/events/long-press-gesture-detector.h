#ifndef __DALI_LONG_PRESS_GESTURE_DETECTOR_H__
#define __DALI_LONG_PRESS_GESTURE_DETECTOR_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/gesture-detector.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

namespace Internal DALI_INTERNAL
{
class LongPressGestureDetector;
}

struct LongPressGesture;

/**
 * @brief This class emits a signals when a long press gesture occurs that meets the requirements set by the application.
 * @SINCE_1_0.0
 *
 * For any valid long press, two signals will be emitted:
 * - First identifying the beginning (state = Started) i.e. when fingers held down for the required time.
 * - Second identifying the ending (state = Finished) i.e. when fingers are released.
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
public: // Typedefs

  typedef Signal< void ( Actor, const LongPressGesture& ) > DetectedSignalType; ///< Gesture detected signal type @SINCE_1_0.0

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
  static LongPressGestureDetector New(unsigned int touchesRequired);

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
  static LongPressGestureDetector New(unsigned int minTouches, unsigned int maxTouches);

  /**
   * @brief Downcasts a handle to LongPressGestureDetector handle.
   *
   * If handle points to a LongPressGestureDetector object, the
   * downcast produces valid handle. If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a LongPressGestureDetector object or an uninitialized handle
   */
  static LongPressGestureDetector DownCast( BaseHandle handle );

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
  void SetTouchesRequired(unsigned int touches);

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
  void SetTouchesRequired(unsigned int minTouches, unsigned int maxTouches);

public: // Getters

  /**
   * @brief Retrieves the minimum number of touches required.
   *
   * @SINCE_1_0.0
   * @return The minimum number of touches required
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetMinimumTouchesRequired() const;

  /**
   * @brief Retrieves the maximum number of touches required.
   *
   * @SINCE_1_0.0
   * @return The maximum number of touches required
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetMaximumTouchesRequired() const;

public: // Signals

  /**
   * @brief This signal is emitted when the specified long press is detected on the attached actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, const LongPressGesture& gesture );
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
} // namespace Dali

#endif // __DALI_LONG_PRESS_GESTURE_DETECTOR_H__
