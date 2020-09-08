#ifndef DALI_INTERNAL_ROTATION_GESTURE_H
#define DALI_INTERNAL_ROTATION_GESTURE_H

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
#include <dali/public-api/events/rotation-gesture.h>
#include <dali/internal/event/events/gesture-impl.h>

namespace Dali
{

namespace Internal
{

class RotationGesture;
using RotationGesturePtr = IntrusivePtr<RotationGesture>;

/**
 * @copydoc Dali::RotationGesture
 */
class RotationGesture final : public Gesture
{
public:

  /**
   * @brief Default constructor
   */
  RotationGesture( GestureState state )
  : Gesture( GestureType::ROTATION, state )
  {
  }

  RotationGesture(const RotationGesture&) = delete; ///< Deleted copy constructor
  RotationGesture(RotationGesture&&) = delete; ///< Deleted move constructor
  RotationGesture& operator=(const RotationGesture&) = delete; ///< Deleted copy assignment operator
  RotationGesture& operator=(RotationGesture&&) = delete; ///< Deleted move assignment operator

  /**
   * @brief The overall rotation from the start of the rotation gesture till the latest rotation gesture.
   * @param[in] rotation The rotation to set.
   */
  inline void SetRotation( const Radian& rotation )
  {
    mRotation = rotation;
  }

  /**
   * @copydoc Dali::RotationGesture::GetRotation()
   */
  inline const Radian& GetRotation() const
  {
    return mRotation;
  }

  /**
   * @brief The center point of the two points that caused the rotation gesture in screen coordinates.
   * @param[in] screenCenterPoint The point in screen coordinates to set.
   */
  inline void SetScreenCenterPoint( const Vector2& screenCenterPoint )
  {
    mScreenCenterPoint = screenCenterPoint;
  }

  /**
   * @copydoc Dali::RotationGesture::GetScreenCenterPoint()
   */
  inline const Vector2& GetScreenCenterPoint() const
  {
    return mScreenCenterPoint;
  }

  /**
   * @brief The center point of the two points that caused the rotation gesture in local actor coordinates.
   * @param[in] localCenterPoint The point in local actor coordinates to set.
   */
  inline void SetLocalCenterPoint( const Vector2& localCenterPoint )
  {
    mLocalCenterPoint = localCenterPoint;
  }

  /**
   * @copydoc Dali::RotationGesture::GetLocalCenterPoint()
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
  ~RotationGesture() override = default;

private:
  Radian mRotation;
  Vector2 mScreenCenterPoint;
  Vector2 mLocalCenterPoint;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::RotationGesture& GetImplementation( Dali::RotationGesture& rotationGesture )
{
  DALI_ASSERT_ALWAYS( rotationGesture && "rotationGesture handle is empty" );

  BaseObject& object = rotationGesture.GetBaseObject();

  return static_cast< Internal::RotationGesture& >( object );
}

inline const Internal::RotationGesture& GetImplementation( const Dali::RotationGesture& rotationGesture )
{
  DALI_ASSERT_ALWAYS( rotationGesture && "rotationGesture handle is empty" );

  const BaseObject& object = rotationGesture.GetBaseObject();

  return static_cast< const Internal::RotationGesture& >( object );
}

} // namespace Dali

#endif // DALI_INTERNAL_ROTATION_GESTURE_H
