#ifndef DALI_INTERNAL_PAN_GESTURE_H
#define DALI_INTERNAL_PAN_GESTURE_H

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
#include <dali/public-api/events/pan-gesture.h>
#include <dali/internal/event/events/gesture-impl.h>

namespace Dali
{

namespace Internal
{

class PanGesture;
using PanGesturePtr = IntrusivePtr<PanGesture>;

/**
 * @copydoc Dali::PanGesture
 */
class PanGesture final : public Gesture
{
public:

  /**
   * @brief Default constructor
   */
  PanGesture(GestureState state)
  : Gesture(GestureType::PAN, state)
  {
  }

  PanGesture(const PanGesture&) = delete; ///< Deleted copy constructor
  PanGesture(PanGesture&&) = delete; ///< Deleted move constructor
  PanGesture& operator=(const PanGesture&) = delete; ///< Deleted copy assignment operator
  PanGesture& operator=(PanGesture&&) = delete; ///< Deleted move assignment operator

  /**
   * @brief Set The velocity at which the user is moving their fingers.
   * @param[in] velocity The Vector2 in local coordinates to set.
   */
  inline void SetVelocity(const Vector2& velocity)
  {
    mVelocity = velocity;
  }

  /**
   * @copydoc Dali::PanGesture::GetVelocity()
   */
  inline const Vector2& GetVelocity() const
  {
    return mVelocity;
  }

  /**
   * @brief Set This is a Vector2 showing how much the user has panned (dragged) since the last pan gesture or,
   * if the gesture has just started, then the amount panned since the user touched the screen.
   * @param[in] displacement The Vector2 in local coordinates to set.
   */
  inline void SetDisplacement(const Vector2& displacement)
  {
    mDisplacement = displacement;
  }

  /**
   * @copydoc Dali::PanGesture::GetDisplacement()
   */
  inline const Vector2& GetDisplacement() const
  {
    return mDisplacement;
  }

  /**
   * @brief Set This current touch position of the primary touch point in local actor coordinates.
   * @param[in] velocity The current touch position to set.
   */
  inline void SetPosition(const Vector2& position)
  {
    mPosition = position;
  }

  /**
   * @copydoc Dali::PanGesture::GetPosition()
   */
  inline const Vector2& GetPosition() const
  {
    return mPosition;
  }

  /**
   * @brief Set The velocity at which the user is moving their fingers.
   * @param[in] screenVelocity The Vector2 in screen coordinates to set.
   */
  inline void SetScreenVelocity(const Vector2& screenVelocity)
  {
    mScreenVelocity = screenVelocity;
  }

  /**
   * @copydoc Dali::PanGesture::GetScreenVelocity()
   */
  inline const Vector2& GetScreenVelocity() const
  {
    return mScreenVelocity;
  }

  /**
   * @brief Set This is a Vector2 showing how much the user has panned (dragged) since the last pan gesture or,
   * if the gesture has just started, then the amount panned since the user touched the screen.
   * @param[in] screenDisplacement The Vector2 in screen coordinates to set.
   */
  inline void SetScreenDisplacement(const Vector2& screenDisplacement)
  {
    mScreenDisplacement = screenDisplacement;
  }

  /**
   * @copydoc Dali::PanGesture::GetScreenDisplacement()
   */
  inline const Vector2& GetScreenDisplacement() const
  {
    return mScreenDisplacement;
  }

  /**
   * @brief Set This current touch position of the primary touch point in screen coordinates.
   * @param[in] screenPosition The Vector2 in screen coordinates to set.
   */
  inline void SetScreenPosition(const Vector2& screenPosition)
  {
    mScreenPosition = screenPosition;
  }

  /**
   * @copydoc Dali::PanGesture::GetScreenPosition()
   */
  inline const Vector2& GetScreenPosition() const
  {
    return mScreenPosition;
  }

  /**
   * @brief The total number of fingers touching the screen in a pan gesture.
   * @param[in] numberOfTouches The total number of fingers touching the screen to set.
   */
  inline void SetNumberOfTouches(uint32_t numberOfTouches)
  {
    mNumberOfTouches = numberOfTouches;
  }

  /**
   * @brief The total number of fingers touching the screen in a pan gesture.
   */
  inline uint32_t GetNumberOfTouches() const
  {
    return mNumberOfTouches;
  }

  /**
   * @copydoc Dali::PanGesture::GetSpeed()
   */
  inline float GetSpeed() const
  {
    return mVelocity.Length();
  }

  /**
   * @copydoc Dali::PanGesture::GetDistance()
   */
  inline float GetDistance() const
  {
    return mDisplacement.Length();
  }

  /**
   * @copydoc Dali::PanGesture::GetScreenSpeed()
   */
  inline float GetScreenSpeed() const
  {
    return mScreenVelocity.Length();
  }

  /**
   * @copydoc Dali::PanGesture::GetScreenDistance()
   */
  inline float GetScreenDistance() const
  {
    return mScreenDisplacement.Length();
  }

private:

  /**
   * @brief Virtual destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~PanGesture() override = default;

private:
  Vector2 mVelocity;
  Vector2 mDisplacement;
  Vector2 mPosition;
  Vector2 mScreenVelocity;
  Vector2 mScreenDisplacement;
  Vector2 mScreenPosition;
  uint32_t mNumberOfTouches{1u};
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::PanGesture& GetImplementation( Dali::PanGesture& panGesture )
{
  DALI_ASSERT_ALWAYS( panGesture && "panGesture handle is empty" );

  BaseObject& object = panGesture.GetBaseObject();

  return static_cast< Internal::PanGesture& >( object );
}

inline const Internal::PanGesture& GetImplementation( const Dali::PanGesture& panGesture )
{
  DALI_ASSERT_ALWAYS( panGesture && "panGesture handle is empty" );

  const BaseObject& object = panGesture.GetBaseObject();

  return static_cast< const Internal::PanGesture& >( object );
}

} // namespace Dali

#endif // DALI_INTERNAL_PAN_GESTURE_H
