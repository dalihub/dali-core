#ifndef DALI_UPDATE_PROXY_H
#define DALI_UPDATE_PROXY_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class UpdateProxy;
}

/**
 * @brief This class is used to access data of the actors from the update-thread.
 *
 * The Actor API _CANNOT_ be called directly from the update-thread.
 * This class can be used as a proxy to that required Actor data.
 * An actor's data can be accessed using the Actor's Unique ID.
 * The unique ID should be passed to the callback using this class in a thread-safe manner
 * (as you cannot call Actor::GetId from the update-thread).
 */
class DALI_CORE_API UpdateProxy
{
public:

  /**
   * @brief Given the Actor ID, this retrieves that Actor's local position.
   * @param[in]   id        The Actor ID
   * @param[out]  position  Set to the Actor's current position, if Actor ID is valid
   * @return Whether the method call was successful or not.
   */
  bool GetPosition( uint32_t id, Vector3& position ) const;

  /**
   * @brief Allows setting an Actor's local position from the Frame callback function for the current frame only.
   * @param[in]  id        The Actor ID
   * @param[in]  position  The position to set
   * @return Whether the method call was successful or not.
   * @note This will get reset to the internally calculated or previously baked value in the next frame, so will have to be set again.
   */
  bool SetPosition( uint32_t id, const Vector3& position );

  /**
   * @brief Allows baking an Actor's local position from the Frame callback function.
   * @param[in]  id        The Actor ID
   * @param[in]  position  The position to bake
   * @return Whether the method call was successful or not.
   * @note The value is saved so will cause undesired effects if this property is being animated.
   */
  bool BakePosition( uint32_t id, const Vector3& position );

  /**
   * @brief Given the Actor ID, this retrieves that Actor's size.
   * @param[in]   id    The Actor ID
   * @param[out]  size  Set to the Actor's current size, if Actor ID is valid
   * @return Whether the method call was successful or not.
   */
  bool GetSize( uint32_t id, Vector3& size ) const;

  /**
   * @brief Allows setting an Actor's size from the Frame callback function for the current frame only.
   * @param[in]  id    The Actor ID
   * @param[in]  size  The size to set
   * @return Whether the method call was successful or not.
   * @note This will get reset to the internally calculated or previously baked value in the next frame, so will have to be set again.
   */
  bool SetSize( uint32_t id, const Vector3& size );

  /**
   * @brief Allows baking an Actor's size from the Frame callback function.
   * @param[in]  id    The Actor ID
   * @param[in]  size  The size to bake
   * @return Whether the method call was successful or not.
   * @note The value is saved so will cause undesired effects if this property is being animated.
   */
  bool BakeSize( uint32_t id, const Vector3& size );

  /**
   * @brief Given the Actor ID, this retrieves that Actor's local position and size.
   * @param[in]   id        The Actor ID
   * @param[out]  position  Set to the Actor's current position, if Actor ID is valid
   * @param[out]  size      Set to the Actor's current size, if Actor ID is valid
   * @return Whether the method call was successful or not.
   */
  bool GetPositionAndSize( uint32_t id, Vector3& position, Vector3& size ) const;

  /**
   * @brief Given the Actor ID, this retrieves that Actor's local scale.
   * @param[in]   id     The Actor ID
   * @param[out]  scale  Set to the Actor's current scale, if Actor ID is valid
   * @return Whether the method call was successful or not.
   */
  bool GetScale( uint32_t id, Vector3& scale ) const;

  /**
   * @brief Allows setting an Actor's local scale from the Frame callback function for the current frame only.
   * @param[in]  id     The Actor ID
   * @param[in]  scale  The scale to set
   * @return Whether the method call was successful or not.
   * @note This will get reset to the internally calculated or previously baked value in the next frame, so will have to be set again.
   */
  bool SetScale( uint32_t id, const Vector3& scale );

  /**
   * @brief Allows baking an Actor's local scale from the Frame callback function.
   * @param[in]  id     The Actor ID
   * @param[in]  scale  The scale to bake
   * @return Whether the method call was successful or not.
   * @note The value is saved so will cause undesired effects if this property is being animated.
   */
  bool BakeScale( uint32_t id, const Vector3& scale );

  /**
   * @brief Given the Actor ID, this retrieves that Actor's local color.
   * @param[in]   id     The Actor ID
   * @param[out]  color  Set to the Actor's current color, if Actor ID is valid
   * @return Whether the method call was successful or not.
   */
  bool GetColor( uint32_t id, Vector4& color ) const;

  /**
   * @brief Allows setting an Actor's local color from the Frame callback function for the current frame only.
   * @param[in]  id     The Actor ID
   * @param[in]  color  The color to set
   * @return Whether the method call was successful or not.
   * @note This will get reset to the internally calculated or previously baked value in the next frame, so will have to be set again.
   */
  bool SetColor( uint32_t id, const Vector4& color );

  /**
   * @brief Allows baking an Actor's local color from the Frame callback function.
   * @param[in]  id     The Actor ID
   * @param[in]  color  The color to bake
   * @return Whether the method call was successful or not.
   * @note The value is saved so will cause undesired effects if this property is being animated.
   */
  bool BakeColor( uint32_t id, const Vector4& color );

public: // Not intended for application developers

  /// @cond internal

  /**
   * @brief Constructor.
   * @param[in]  impl  A reference to the internal object.
   */
  DALI_INTERNAL UpdateProxy( Internal::UpdateProxy& impl );

  /**
   * @brief Destructor.
   */
  DALI_INTERNAL ~UpdateProxy();

  // Not copyable or movable

  UpdateProxy( const UpdateProxy& )            = delete; ///< Deleted copy constructor
  UpdateProxy( UpdateProxy&& )                 = delete; ///< Deleted move constructor
  UpdateProxy& operator=( const UpdateProxy& ) = delete; ///< Deleted copy assignment operator
  UpdateProxy& operator=( UpdateProxy&& )      = delete; ///< Deleted move assignment operator

  /// @endcond

private:

  /// @cond internal
  Internal::UpdateProxy& mImpl;
  /// @endcond
};

} // namespace Dali

#endif // DALI_UPDATE_PROXY_H
