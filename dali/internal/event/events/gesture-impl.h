#ifndef DALI_INTERNAL_GESTURE_H
#define DALI_INTERNAL_GESTURE_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/gesture.h>
#include <dali/integration-api/events/event.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{

namespace Internal
{

class Gesture;
using GesturePtr = IntrusivePtr<Gesture>;

/**
 * This is the abstract base structure for any gestures that the adaptor detects and wishes to send
 * to the Core.
 */
class Gesture : public BaseObject
{
public:

  /**
   * @brief Get the gesture type.
   *
   * @return The gesture type.
   */
  inline GestureType::Value GetType() const
  {
    return mGestureType;
  }

  /**
   * @brief Set the state of the gesture.
   * @param[in] state The state of the gesture to set
   */
  inline void SetState( GestureState state )
  {
    mState = state;
  }

   /**
   * @brief Get the state of the gesture.
   *
   * @return The state of the gesture.
   */
  inline GestureState GetState() const
  {
    return mState;
  }

  /**
   * @brief Set The time the gesture took place.
   * @param[in] time The time the gesture took place. to set
   */
  inline void SetTime( uint32_t time )
  {
    mTime = time;
  }

  /**
   * @brief Get the time the gesture took place.
   *
   * @return The time the gesture took place.
   */
  inline uint32_t GetTime() const
  {
    return mTime;
  }

  Gesture(const Gesture&) = delete; ///< Deleted copy constructor
  Gesture(Gesture&&) = delete; ///< Deleted move constructor
  Gesture& operator=(const Gesture&) = delete; ///< Deleted copy assignment operator
  Gesture& operator=(Gesture&&) = delete; ///< Deleted move assignment operator

protected:

  /**
   * This constructor is only used by derived classes.
   * @param[in] gestureType   The type of gesture event.
   * @param[in] gestureState  The state of the gesture event.
   */
  Gesture(GestureType::Value gestureType, GestureState gestureState)
  : mGestureType( gestureType ),
    mState( gestureState )
  {
  }

  /**
   * @brief Virtual destructor.
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~Gesture() override = default;

private:
  GestureType::Value mGestureType;
  GestureState mState;
  uint32_t mTime{0u};
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::Gesture& GetImplementation(Dali::Gesture& gesture)
{
  DALI_ASSERT_ALWAYS( gesture && "gesture handle is empty" );

  BaseObject& handle = gesture.GetBaseObject();

  return static_cast<Internal::Gesture&>(handle);
}

inline const Internal::Gesture& GetImplementation(const Dali::Gesture& gesture)
{
  DALI_ASSERT_ALWAYS( gesture && "gesture handle is empty" );

  const BaseObject& handle = gesture.GetBaseObject();

  return static_cast<const Internal::Gesture&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_GESTURE_H
