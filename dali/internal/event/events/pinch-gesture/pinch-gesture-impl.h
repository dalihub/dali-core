#ifndef DALI_INTERNAL_PINCH_GESTURE_H
#define DALI_INTERNAL_PINCH_GESTURE_H

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
#include <dali/public-api/events/pinch-gesture.h>
#include <dali/internal/event/events/gesture-impl.h>

namespace Dali
{

namespace Internal
{

class PinchGesture;
using PinchGesturePtr = IntrusivePtr<PinchGesture>;

/**
 * @copydoc Dali::PinchGesture
 */
class PinchGesture final : public Gesture
{
public:

  /**
   * @brief Default constructor
   */
  PinchGesture( GestureState state )
  : Gesture(GestureType::PINCH, state)
  {
  }

  PinchGesture(const PinchGesture&) = delete; ///< Deleted copy constructor
  PinchGesture(PinchGesture&&) = delete; ///< Deleted move constructor
  PinchGesture& operator=(const PinchGesture&) = delete; ///< Deleted copy assignment operator
  PinchGesture& operator=(PinchGesture&&) = delete; ///< Deleted move assignment operator

  /**
   * @brief The scale factor from the start of the pinch gesture till the latest pinch gesture.
   * @param[in] scale The scale to set.
   */
  inline void SetScale( float scale )
  {
    mScale = scale;
  }

  /**
   * @copydoc Dali::PinchGesture::GetScale()
   */
  inline float GetScale() const
  {
    return mScale;
  }

  /**
   * @brief The speed at which the user is moving their fingers.
   * @param[in] speed The speed to set.
   */
  inline void SetSpeed( float speed )
  {
    mSpeed = speed;
  }

  /**
   * @copydoc Dali::PinchGesture::GetSpeed()
   */
  inline float GetSpeed() const
  {
    return mSpeed;
  }

  /**
   * @brief The center point of the two points that caused the pinch gesture in screen coordinates.
   * @param[in] screenCenterPoint The point in screen coordinates to set.
   */
  inline void SetScreenCenterPoint( const Vector2& screenCenterPoint )
  {
    mScreenCenterPoint = screenCenterPoint;
  }

  /**
   * @copydoc Dali::PinchGesture::GetScreenCenterPoint()
   */
  inline const Vector2& GetScreenCenterPoint() const
  {
    return mScreenCenterPoint;
  }

  /**
   * @brief The center point of the two points that caused the pinch gesture in local actor coordinates.
   * @param[in] localCenterPoint The point in local actor coordinates to set.
   */
  inline void SetLocalCenterPoint( const Vector2& localCenterPoint )
  {
    mLocalCenterPoint = localCenterPoint;
  }

  /**
   * @copydoc Dali::PinchGesture::GetLocalCenterPoint()
   */
  inline const Vector2& GetLocalCenterPoint() const
  {
    return mLocalCenterPoint;
  }

private:

  /**
   * @brief Virtual destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~PinchGesture() override = default;

private:
  Vector2 mScreenCenterPoint;
  Vector2 mLocalCenterPoint;
  float mScale{0.0f};
  float mSpeed{0.0f};
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::PinchGesture& GetImplementation( Dali::PinchGesture& pinchGesture )
{
  DALI_ASSERT_ALWAYS( pinchGesture && "pinchGesture handle is empty" );

  BaseObject& object = pinchGesture.GetBaseObject();

  return static_cast< Internal::PinchGesture& >( object );
}

inline const Internal::PinchGesture& GetImplementation( const Dali::PinchGesture& pinchGesture )
{
  DALI_ASSERT_ALWAYS( pinchGesture && "pinchGesture handle is empty" );

  const BaseObject& object = pinchGesture.GetBaseObject();

  return static_cast< const Internal::PinchGesture& >( object );
}

} // namespace Dali

#endif // DALI_INTERNAL_PINCH_GESTURE_H
