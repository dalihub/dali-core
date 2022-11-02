#ifndef DALI_GESTURE_H
#define DALI_GESTURE_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/gesture-enumerations.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

namespace Internal DALI_INTERNAL
{
class Gesture;
}

/**
 * @brief Base structure for different gestures that an application can receive.
 *
 * A gesture is an event that is produced from a combination of
 * several touch events in a particular order or within a certain time
 * frame (e.g pinch).
 *
 * To receive a particular gesture, the application has to create and connect to the appropriate
 * GestureDetector.
 *
 * @SINCE_1_9.28
 * @note An instance of this class cannot be created.
 * @see GestureDetector
 *
 */
class DALI_CORE_API Gesture : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized Gesture handle.
   *
   * Calling member functions with an uninitialized Actor handle is not allowed.
   * @SINCE_1_9.28
   */
  Gesture();

  /**
   * @brief Copy constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   */
  Gesture(const Gesture& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the handle to move
   */
  Gesture(Gesture&& rhs);

  /**
   * @brief Assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  Gesture& operator=(const Gesture& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the handle to move
   * @return A reference to this
   */
  Gesture& operator=(Gesture&& rhs);

  /**
   * @brief Non virtual destructor.
   * @SINCE_1_9.28
   */
  ~Gesture();

  /**
   * @brief The gesture type.
   * @SINCE_1_9.28
   * @return The gesture type
   */
  GestureType::Value GetType() const;

  /**
   * @brief The gesture state.
   *
   * Please see the description in the sub-classes to see the states
   * used by the actual gesture.
   * @SINCE_1_9.28
   * @return The state of the gesture
   */
  GestureState GetState() const;

  /**
   * @brief The time the gesture took place.
   * @SINCE_1_9.28
   * @return The time the gesture took place
   */
  uint32_t GetTime() const;

  /**
   * @brief Gets the value of which source the gesture was started with. (ex : mouse)
   * @SINCE_2_2.0
   * @return The gesture input source.
   */
  GestureSourceType GetSourceType() const;

  /**
   * @brief Gets the data of the source type.
   * @SINCE_2_2.0
   * @return The gesture source data.
   */
  GestureSourceData GetSourceData() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Internal constructor.
   * @SINCE_1_9.28
   * @param[in] gesture Pointer to internal gesture
   */
  explicit DALI_INTERNAL Gesture(Internal::Gesture* gesture);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_GESTURE_H
