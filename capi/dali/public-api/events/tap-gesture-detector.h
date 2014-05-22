#ifndef __DALI_TAP_GESTURE_DETECTOR_H__
#define __DALI_TAP_GESTURE_DETECTOR_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_EVENTS_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/events/gesture-detector.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class TapGestureDetector;
}

struct TapGesture;

/**
 * @brief This class emits a signal when a tap gesture occurs that meets the requirements set by the
 * application.
 *
 * See TapGestureDetector::SetTapsRequired and TapGestureDetector::SetTouchesRequired.
 *
 * A Tap Gesture is a discrete gesture, which means it does not have any state information attached
 * to it.  Please see TapGesture for more information.
 *
 * The application programmer can use this gesture detector as follows:
 * @code
 * TapGestureDetector detector = TapGestureDetector::New();
 * detector.Attach(myActor);
 * detector.SignalDetected().Connect(this, &MyApplication::OnTap);
 * @endcode
 *
 * @see TapGesture
 */
class TapGestureDetector : public GestureDetector
{
public: // Typedefs

  /**
   * @brief Signal type for detected signal.
   */
  typedef SignalV2< void (Actor, TapGesture) > DetectedSignalV2;

  //Signal Names
  static const char* const SIGNAL_TAP_DETECTED; ///< name "tap-detected"

public: // Creation & Destruction

  /**
   * @brief Create an uninitialized TapGestureDetector; this can be initialized with TapGestureDetector::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  TapGestureDetector();

  /**
   * @brief Create an initialized TapGestureDetector.
   *
   * By default, this would create a gesture detector which requires one tap with one touch.
   * @return A handle to a newly allocated Dali resource.
   */
  static TapGestureDetector New();

  /**
   * @brief Create an initialized TapGestureDetector with the specified parameters.
   *
   * @param[in]  tapsRequired     The number of taps required.
   * @param[in]  touchesRequired  The number of touches required.
   * @return A handle to a newly allocated Dali resource.
   */
  static TapGestureDetector New(unsigned int tapsRequired, unsigned int touchesRequired);

  /**
   * @brief Downcast an Object handle to TapGestureDetector handle.
   *
   * If handle points to a TapGestureDetector object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a TapGestureDetector object or an uninitialized handle
   */
  static TapGestureDetector DownCast( BaseHandle handle );

  /**
   * @brief Virtual destructor.
   *
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~TapGestureDetector();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public: // Setters

  /**
   * @brief Set the number of taps required.
   *
   * The tap count is the number of times a user should "tap" the screen.
   * @param[in]  taps  Taps required.
   * @pre The gesture detector has been initialized.
   * @note The default is '1'.
   */
  void SetTapsRequired(unsigned int taps);

  /**
   * @brief Set the number of touches required.
   *
   * The number of touches corresponds to the number of fingers a user has on the screen.
   * @param[in]  touches  Touches required.
   * @pre The gesture detector has been initialized.
   * @note The default is '1'.
   */
  void SetTouchesRequired(unsigned int touches);

public: // Getters

  /**
   * @brief Retrieves the number of taps required.
   *
   * @return The taps required.
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetTapsRequired() const;

  /**
   * @brief Retrieves the number of touches required.
   *
   * @return The number of touches required.
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetTouchesRequired() const;

public: // Signals

  /**
   * @brief This signal is emitted when the specified tap is detected on the attached actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor actor, TapGesture gesture);
   * @endcode
   * @pre The gesture detector has been initialized.
   * @return The signal to connect to.
   */
  DetectedSignalV2& DetectedSignal();

public: // Not intended for Application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in]  internal  A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL TapGestureDetector(Internal::TapGestureDetector* internal);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TAP_GESTURE_DETECTOR_H__
