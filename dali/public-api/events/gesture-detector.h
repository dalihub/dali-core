#ifndef __DALI_GESTURE_DETECTOR_H__
#define __DALI_GESTURE_DETECTOR_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/handle.h>

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

namespace Internal DALI_INTERNAL
{
class GestureDetector;
}

class Actor;

/**
 * @brief GestureDetectors analyse a stream of touch events and attempt to determine the intention of the user.
 *
 * An actor is attached to a gesture detector and if the detector recognises a pattern in its analysis, it will
 * emit a detected signal to the application.
 *
 * This is the base class for different gesture detectors available and provides functionality that is common
 * to all the gesture detectors.
 *
 * @SINCE_1_0.0
 * @see Gesture
 */
class DALI_IMPORT_API GestureDetector : public Handle
{
public: // Creation & Destruction

  /**
   * @brief Create an uninitialized GestureDetector.
   *
   * This can be initialized with one of the derived gesture detectors' New() methods. For example, PanGestureDetector::New().
   *
   * Calling member functions with an uninitialized Dali::GestureDetector handle is not allowed.
   * @SINCE_1_0.0
   */
  GestureDetector();

  /**
   * @brief Downcast a handle to GestureDetector handle.
   *
   * If handle points to a GestureDetector object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a GestureDetector object or an uninitialized handle
   */
  static GestureDetector DownCast( BaseHandle handle );

  /**
   * @brief Dali::GestureDetector is intended as a base class
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~GestureDetector();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] handle A reference to the copied handle
   */
  GestureDetector(const GestureDetector& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  GestureDetector& operator=(const GestureDetector& rhs);

public: // Actor related

  /**
   * @brief Attaches an actor to the gesture.
   *
   * The detected signal will be dispatched when the gesture occurs on
   * the attached actor.
   * @SINCE_1_0.0
   * @param[in]  actor  The actor to attach to the gesture detector
   * @pre The gesture detector has been initialized.
   * @note You can attach several actors to a gesture detector.
   */
  void Attach(Actor actor);

  /**
   * @brief Detaches the attached actor from the gesture detector.
   *
   * @SINCE_1_0.0
   * @param[in]  actor  The actor to detach from the gesture detector.
   * @pre The gesture detector has been initialized.
   * @pre The specified actor has been attached to the gesture detector.
   */
  void Detach(Actor actor);

  /**
   * @brief Detaches all the actors that have been attached to the gesture detector.
   *
   * @SINCE_1_0.0
   * @pre The gesture detector has been initialized.
   * @pre At least one actor has been attached to the gesture detector.
   */
  void DetachAll();

  /**
   * @brief Returns the number of actors attached to the gesture detector.
   *
   * @SINCE_1_0.0
   * @return The count
   * @pre The gesture detector has been initialized.
   */
  size_t GetAttachedActorCount() const;

  /**
   * @brief Returns an actor by index. An empty handle if the index is not valid.
   *
   * @SINCE_1_0.0
   * @return The attached actor or an empty handle.
   * @pre The gesture detector has been initialized.
   */
  Actor GetAttachedActor(size_t index) const;

protected:

  /// @cond internal
  /**
   * @brief This constructor is used by New() methods of derived classes (For example, PanGestureDetector::New()).
   *
   * @SINCE_1_0.0
   * @param [in]  internal  A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL GestureDetector(Internal::GestureDetector* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_GESTURE_DETECTOR_H__
