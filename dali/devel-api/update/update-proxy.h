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
   * @brief Given the Actor ID, this retrieves that Actor's position.
   * @param[in]  id  The Actor ID
   * @return If valid Actor ID, then the Actor's position is returned.
   */
  Vector3 GetPosition( unsigned int id ) const;

  /**
   * @brief Allows setting an Actor's position from the Frame callback function.
   * @param[in]  id        The Actor ID
   * @param[in]  position  The position to set.
   * @note This will get reset to the internally calculated value in the next frame, so will have to be set again.
   */
  void SetPosition( unsigned int id, const Vector3& position );

  /**
   * @brief Given the Actor ID, this retrieves that Actor's size.
   * @param[in]  id  The Actor ID
   * @return If valid Actor ID, then Actor's size is returned, otherwise Vector3::ZERO.
   */
  const Vector3& GetSize( unsigned int id ) const;

  /**
   * @brief Allows setting an Actor's size from the Frame callback function.
   * @param[in]  id    The Actor ID
   * @param[in]  size  The size to set.
   * @note This will get reset to the internally calculated value in the next frame, so will have to be set again.
   */
  void SetSize( unsigned int id, const Vector3& size );

  /**
   * @brief Given the Actor ID, this retrieves that Actor's position and size.
   * @param[in]   id        The Actor ID
   * @param[out]  position  If valid Actor ID, then Actor's position is set
   * @param[out]  size      If valid Actor ID, then Actor's size is set
   */
  void GetPositionAndSize( unsigned int id, Vector3& position, Vector3& size ) const;

  /**
   * @brief Given the Actor ID, this retrieves that Actor's color.
   * @param[in]   id        The Actor ID
   * @return If valid Actor ID, then Actor's color is returned, otherwise Vector4::ZERO.
   */
  Vector4 GetWorldColor( unsigned int id ) const;

  /**
   * @brief Allows setting an Actor's color from the Frame callback function.
   * @param[in]  id     The Actor ID
   * @param[in]  color  The color to set
   * @note This will get reset to the internally calculated value in the next frame, so will have to be set again.
   */
  void SetWorldColor( unsigned int id, const Vector4& color ) const;

  /**
   * @brief Given the Actor ID, this retrieves that Actor's world-matrix and size.
   * @param[in]   id           The Actor ID
   * @param[out]  worldMatrix  If valid Actor ID, then Actor's world matrix is set
   * @param[out]  size         If valid Actor ID, then Actor's size is set
   */
  void GetWorldMatrixAndSize( unsigned int id, Matrix& worldMatrix, Vector3& size ) const;

  /**
   * @brief Given the Actor ID, this retrieves that Actor's world-matrix.
   * @param[in]  id  The Actor ID
   * @return If valid Actor ID, then Actor's world matrix is returned, otherwise Matrix::IDENTITY.
   */
  const Matrix& GetWorldMatrix( unsigned int id ) const;

  /**
   * @brief Allows the setting an Actor's world-matrix from the Frame callback function.
   * @param[in]  id           The Actor ID
   * @param[in]  worldMatrix  The world matrix to set.
   * @note This will get reset to the internally calculated value in the next frame, so will have to be set again.
   * @note This will only set the world matrix for that particular actor.
   *       The world matrices of the children will not change and will have to be manually changed in the callback
   *       as well (if required).
   */
  void SetWorldMatrix( unsigned int id, const Matrix& worldMatrix );

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
