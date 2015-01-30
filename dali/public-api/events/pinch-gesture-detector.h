#ifndef __DALI_PINCH_GESTURE_DETECTOR_H__
#define __DALI_PINCH_GESTURE_DETECTOR_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

namespace Internal DALI_INTERNAL
{
class PinchGestureDetector;
}

struct PinchGesture;

/**
 * @brief This class looks for pinching gestures involving two touches.
 *
 * It tries to detect when the user moves two touch points towards or
 * away from each other.  Please see PinchGesture for more
 * information.
 *
 * The application programmer can use this gesture detector as follows:
 * @code
 * PinchGestureDetector detector = PinchGestureDetector::New();
 * detector.Attach(myActor);
 * detector.DetectedSignal().Connect(this, &MyApplication::OnPinch);
 * @endcode
 *
 * @see PinchGesture
 */
class DALI_IMPORT_API PinchGestureDetector : public GestureDetector
{
public: // Typedefs

  /**
   * @brief Signal type.
   */
  typedef Signal< void ( Actor, const PinchGesture& ) > DetectedSignalType;

  //Signal Names
  static const char* const SIGNAL_PINCH_DETECTED; ///< name "pinch-detected"

public: // Creation & Destruction

  /**
   * @brief Create an uninitialized PinchGestureDetector; this can be initialized with PinchGestureDetector::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  PinchGestureDetector();

  /**
   * @brief Create an initialized PinchGestureDetector.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static PinchGestureDetector New();

  /**
   * @brief Downcast an Object handle to PinchGestureDetector handle.
   *
   * If handle points to a PinchGestureDetector object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a PinchGestureDetector object or an uninitialized handle
   */
  static PinchGestureDetector DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~PinchGestureDetector();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  PinchGestureDetector(const PinchGestureDetector& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  PinchGestureDetector& operator=(const PinchGestureDetector& rhs);

public: // Signals

  /**
   * @brief This signal is emitted when the pinch gesture is detected on the attached actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, const PinchGesture& gesture );
   * @endcode
   * @pre The gesture detector has been initialized.
   * @return The signal to connect to.
   */
  DetectedSignalType& DetectedSignal();

public: // Not intended for Application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in]  internal  A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL PinchGestureDetector(Internal::PinchGestureDetector* internal);

};

} // namespace Dali

#endif // __DALI_PINCH_GESTURE_DETECTOR_H__
