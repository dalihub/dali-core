#ifndef __DALI_LIGHT_ACTOR_H__
#define __DALI_LIGHT_ACTOR_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/light.h>


namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class LightActor;
}

/**
 * @brief Controls a light source loaded from a file containing a 3D scene.
 *
 * Allows the developer to use actor semantics to control a light source.
 * see @ref Dali::Model
 */
class DALI_IMPORT_API LightActor : public Actor
{
public:

  // Default Properties additional to Actor
  static const Property::Index LIGHT_TYPE;                           ///< name "light-type",       type STRING
  static const Property::Index ENABLE;                               ///< name "enable",           type VECTOR2
  static const Property::Index FALL_OFF;                             ///< name "fall-off",         type VECTOR2
  static const Property::Index SPOT_ANGLE;                           ///< name "spot-angle",       type VECTOR2
  static const Property::Index AMBIENT_COLOR;                        ///< name "ambient-color",    type VECTOR3
  static const Property::Index DIFFUSE_COLOR;                        ///< name "diffuse-color",    type VECTOR3
  static const Property::Index SPECULAR_COLOR;                       ///< name "specular-color",   type VECTOR3
  static const Property::Index DIRECTION;                            ///< name "direction",        type VECTOR3

  /**
   * @brief Create an uninitialized LightActor handle.
   *
   * This can be initialised with LightActor::New().
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  LightActor();

  /**
   * @brief Create a LightActor object.
   *
   * @return A handle to a newly allocated LightActor.
   */
  static LightActor New();

  /**
   * @brief Downcast an Object handle to LightActor.
   *
   * If handle points to a LightActor the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a LightActor or an uninitialized handle
   */
  static LightActor DownCast( BaseHandle handle );

  /**
   * @brief Virtual destructor.
   *
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~LightActor();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * @brief Set the light properties for the actor.
   *
   * @param[in] light The light properties
   */
  void SetLight(Light light);

  /**
   * @brief Get the current light properties for the actor.
   *
   * @return The light properties
   */
  Light GetLight();

  /**
   * @brief Set this light's active state.
   *
   * @param[in] active Set to true to activate this light
   */
  void SetActive( bool active);

  /**
   * @brief Get the active status of this light.
   *
   * @return true if this light is active
   */
  bool GetActive();


public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL LightActor(Internal::LightActor* actor);
};

} // namespace Dali


#endif // __DALI_LIGHT_ACTOR_H__
