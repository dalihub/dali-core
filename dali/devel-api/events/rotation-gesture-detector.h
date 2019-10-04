#ifndef DALI_ROTATION_GESTURE_DETECTOR_H
#define DALI_ROTATION_GESTURE_DETECTOR_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
class RotationGestureDetector;
}

struct RotationGesture;

/**
 * @brief This class looks for a rotation gesture involving two touches.
 *
 * It measures the relative rotation of two touch points & emits a signal
 * when this changes. Please see RotationGesture for more information.
 *
 * The application programmer can use this gesture detector as follows:
 * @code
 * RotationGestureDetector detector = RotationGestureDetector::New();
 * detector.Attach( myActor );
 * detector.DetectedSignal().Connect( this, &MyApplication::OnRotation );
 * @endcode
 *
 * @see RotationGesture
 *
 * Signals
 * | %Signal Name      | Method                |
 * |-------------------|-----------------------|
 * | rotationDetected  | @ref DetectedSignal() |
 */
class DALI_CORE_API RotationGestureDetector : public GestureDetector
{
public: // Typedefs

  /**
   * @brief Signal type.
   */
  typedef Signal< void ( Actor, const RotationGesture& ) > DetectedSignalType;

public: // Creation & Destruction

  /**
   * @brief Creates an uninitialized RotationGestureDetector; this can be initialized with RotationGestureDetector::New().
   *
   * Calling member functions with an uninitialized RotationGestureDetector handle is not allowed.
   */
  RotationGestureDetector();

  /**
   * @brief Creates an initialized RotationGestureDetector.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static RotationGestureDetector New();

  /**
   * @brief Downcasts a handle to RotationGestureDetector handle.
   *
   * If handle points to a RotationGestureDetector object, the
   * downcast produces valid handle. If not, the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return Handle to a RotationGestureDetector object or an uninitialized handle
   */
  static RotationGestureDetector DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~RotationGestureDetector();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle
   */
  RotationGestureDetector(const RotationGestureDetector& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  RotationGestureDetector& operator=(const RotationGestureDetector& rhs);

public: // Signals

  /**
   * @brief This signal is emitted when the rotation gesture is detected on the attached actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, const RotationGesture& gesture );
   * @endcode
   * @return The signal to connect to
   * @pre The gesture detector has been initialized.
   */
  DetectedSignalType& DetectedSignal();

public: // Not intended for Application developers

  /// @cond internal
  /**
   * @brief This constructor is used by RotationGestureDetector::New() methods.
   *
   * @param[in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL RotationGestureDetector(Internal::RotationGestureDetector* internal);
  /// @endcond

};

} // namespace Dali

#endif // DALI_ROTATION_GESTURE_DETECTOR_H
