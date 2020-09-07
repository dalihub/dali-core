#ifndef DALI_INTERNAL_LONG_PRESS_GESTURE_H
#define DALI_INTERNAL_LONG_PRESS_GESTURE_H

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
#include <dali/internal/event/events/gesture-impl.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

namespace Internal
{

class LongPressGesture;
using LongPressGesturePtr = IntrusivePtr<LongPressGesture>;

/**
 * @copydoc Dali::LongPressGesture
 */
class LongPressGesture final : public Gesture
{
public:

  /**
   * Default Constructor
   * @param[in]  state  STARTED, when we detect a long press.
   *                    FINISHED, when all touches are finished.
   */
  LongPressGesture( GestureState state )
  : Gesture( GestureType::LONG_PRESS, state )
  {
  }

  LongPressGesture(const LongPressGesture&) = delete; ///< Deleted copy constructor
  LongPressGesture(LongPressGesture&&) = delete; ///< Deleted move constructor
  LongPressGesture& operator=(const LongPressGesture&) = delete; ///< Deleted copy assignment operator
  LongPressGesture& operator=(LongPressGesture&&) = delete; ///< Deleted move assignment operator

  /**
   * @brief Set the number of touch points.
   * @param[in] numberOfTouches The number of touch points to set.
   */
  inline void SetNumberOfTouches( uint32_t numberOfTouches )
  {
    mNumberOfTouches = numberOfTouches;
  }

  /**
   * @copydoc Dali::LongPressGesture::GetNumberOfTouches()
   */
  inline uint32_t GetNumberOfTouches() const
  {
    return mNumberOfTouches;
  }

  /**
   * @brief Set This is the point, in screen coordinates.
   * @param[in] screenPoint The point in screen coordinates to set.
   */
  inline void SetScreenPoint( const Vector2& screenPoint )
  {
    mScreenPoint = screenPoint;
  }

  /**
   * @copydoc Dali::LongPressGesture::GetScreenPoint()
   */
  inline const Vector2& GetScreenPoint() const
  {
    return mScreenPoint;
  }

  /**
   * @brief Set This is the point, in local actor coordinates.
   * @param[in] localPoint The point in local actor coordinates to set.
   */
  inline void SetLocalPoint( const Vector2& localPoint )
  {
    mLocalPoint = localPoint;
  }

  /**
   * @copydoc Dali::LongPressGesture::GetLocalPoint()
   */
  inline const Vector2& GetLocalPoint() const
  {
    return mLocalPoint;
  }

private:

  /**
   * @brief Virtual destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~LongPressGesture() override = default;

private:
  Vector2 mScreenPoint;
  Vector2 mLocalPoint;
  uint32_t mNumberOfTouches{1u};
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::LongPressGesture& GetImplementation( Dali::LongPressGesture& longPressGesture )
{
  DALI_ASSERT_ALWAYS( longPressGesture && "longPressGesture handle is empty" );

  BaseObject& object = longPressGesture.GetBaseObject();

  return static_cast< Internal::LongPressGesture& >( object );
}

inline const Internal::LongPressGesture& GetImplementation( const Dali::LongPressGesture& longPressGesture )
{
  DALI_ASSERT_ALWAYS( longPressGesture && "longPressGesture handle is empty" );

  const BaseObject& object = longPressGesture.GetBaseObject();

  return static_cast< const Internal::LongPressGesture& >( object );
}

} // namespace Dali

#endif // DALI_INTERNAL_LONG_PRESS_GESTURE_H
