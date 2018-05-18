#ifndef __DALI_TAP_GESTURE_DETECTOR_H__
#define __DALI_TAP_GESTURE_DETECTOR_H__

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
class TapGestureDetector;
}

struct TapGesture;

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
  typedef Signal< void ( Actor, const TapGesture& ) > DetectedSignalType;

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
  static TapGestureDetector New( unsigned int tapsRequired );

  /**
   * @brief Downcasts a handle to TapGestureDetector handle.
   *
   * If handle points to a TapGestureDetector object, the
   * downcast produces valid handle. If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a TapGestureDetector object or an uninitialized handle
   */
  static TapGestureDetector DownCast( BaseHandle handle );

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

public: // Setters

  /**
   * @brief Sets the minimum number of taps required.
   *
   * The tap count is the number of times a user should "tap" the screen.
   * @SINCE_1_0.0
   * @param[in] minimumTaps The minimum taps required
   * @pre The gesture detector has been initialized.
   * @note The default is '1'.
   */
  void SetMinimumTapsRequired( unsigned int minimumTaps );

  /**
   * @brief Sets the maximum number of taps required.
   *
   * The tap count is the number of times a user should "tap" the screen.
   * @SINCE_1_0.0
   * @param[in] maximumTaps The maximum taps required
   * @pre The gesture detector has been initialized.
   * @note The default is '1'.
   */
  void SetMaximumTapsRequired( unsigned int maximumTaps );

public: // Getters

  /**
   * @brief Retrieves the minimum number of taps required.
   *
   * @SINCE_1_0.0
   * @return The minimum taps required
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetMinimumTapsRequired() const;

  /**
   * @brief Retrieves the maximum number of taps required.
   *
   * @SINCE_1_0.0
   * @return The maximum taps required
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetMaximumTapsRequired() const;

public: // Signals

  /**
   * @brief This signal is emitted when the specified tap is detected on the attached actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, const TapGesture& gesture );
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

} // namespace Dali

#endif // __DALI_TAP_GESTURE_DETECTOR_H__
