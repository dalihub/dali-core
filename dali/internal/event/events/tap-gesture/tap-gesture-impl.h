#ifndef DALI_INTERNAL_TAP_GESTURE_H
#define DALI_INTERNAL_TAP_GESTURE_H

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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/internal/event/events/gesture-impl.h>

namespace Dali
{

namespace Internal
{

class TapGesture;
using TapGesturePtr = IntrusivePtr<TapGesture>;

/**
 * @copydoc Dali::TapGesture
 */
class TapGesture final : public Gesture
{
public:

  /**
   * @brief Default constructor
   */
  TapGesture( GestureState state )
  : Gesture(GestureType::TAP, state)
  {
  }

  TapGesture(const TapGesture&) = delete; ///< Deleted copy constructor
  TapGesture(TapGesture&&) = delete; ///< Deleted move constructor
  TapGesture& operator=(const TapGesture&) = delete; ///< Deleted copy assignment operator
  TapGesture& operator=(TapGesture&&) = delete; ///< Deleted move assignment operator

  /**
   * @brief The number of taps in this tap gesture.
   * @param[in] numberOfTaps The number of taps to set.
   */
  inline void SetNumberOfTaps( uint32_t numberOfTaps )
  {
    mNumberOfTaps = numberOfTaps;
  }

  /**
   * @copydoc Dali::TapGesture::GetNumberOfTaps()
   */
  inline uint32_t GetNumberOfTaps() const
  {
    return mNumberOfTaps;
  }

  /**
   * @brief The number of touch points in this tap gesture, i.e. the number of fingers the user had on the screen to generate the tap gesture.
   * @param[in] numberOfTouches The number of touch points in this tap gesture to set.
   */
  inline void SetNumberOfTouches( uint32_t numberOfTouches )
  {
    mNumberOfTouches = numberOfTouches;
  }

  /**
   * @copydoc Dali::TapGesture::GetPointCount()
   */
  inline uint32_t GetNumberOfTouches() const
  {
    return mNumberOfTouches;
  }

  /**
   * @brief This is the point, in screen coordinates, where the tap occurred.
   * @param[in] screenPoint The point in screen coordinates to set.
   */
  inline void SetScreenPoint( const Vector2& screenPoint )
  {
    mScreenPoint = screenPoint;
  }

  /**
   * @copydoc Dali::TapGesture::GetDeviceId()
   */
  inline const Vector2& GetScreenPoint() const
  {
    return mScreenPoint;
  }

  /**
   * @brief This is the point, in local actor coordinates, where the tap occurred.
   * @param[in] localPoint The point in local actor coordinates to set.
   */
  inline void SetLocalPoint( const Vector2& localPoint )
  {
    mLocalPoint = localPoint;
  }

  /**
   * @copydoc Dali::TapGesture::GetGetState()
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
  ~TapGesture() override = default;

private:
  Vector2 mScreenPoint;
  Vector2 mLocalPoint;
  uint32_t mNumberOfTaps{1u};
  uint32_t mNumberOfTouches{1u};
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TapGesture& GetImplementation( Dali::TapGesture& tapGesture )
{
  DALI_ASSERT_ALWAYS( tapGesture && "TapGesture handle is empty" );

  BaseObject& object = tapGesture.GetBaseObject();

  return static_cast< Internal::TapGesture& >( object );
}

inline const Internal::TapGesture& GetImplementation( const Dali::TapGesture& tapGesture )
{
  DALI_ASSERT_ALWAYS( tapGesture && "TapGesture handle is empty" );

  const BaseObject& object = tapGesture.GetBaseObject();

  return static_cast< const Internal::TapGesture& >( object );
}

} // namespace Dali

#endif // DALI_INTERNAL_TAP_GESTURE_H
