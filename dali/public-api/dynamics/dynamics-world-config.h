#ifndef __DALI_DYNAMICS_WORLD_CONFIG_H__
#define __DALI_DYNAMICS_WORLD_CONFIG_H__

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

// EXTERNAL INCLUDES
#include <string>

// BASE CLASS INCLUDES
#include <dali/public-api/object/base-handle.h>

namespace Dali DALI_IMPORT_API
{

struct Vector3;

namespace Internal DALI_INTERNAL
{
class DynamicsWorldConfig;
} // namespace Internal

/**
 * @brief Describes the requirements/capabilities of a physics siumlation encapsulated by a DynamicsWorld.
 */
class DynamicsWorldConfig : public BaseHandle
{
public:
  /**
   * @brief Enumeration specifying the capabilities required by the dynamics simulation.
   */
  enum WorldType
  {
    RIGID,                ///< Adds rigid body dynamics (gravity, mass, velocity, friction)
    SOFT,                 ///< Adds soft body dynamics - implies RIGID
  }; // enum WorldType


public:

  /**
   * @brief Create a new DynamicsWorld configuration object.
   *
   * All values are set to default values.
   * @return A handle to the new DynamicsWorld configuration object.
   */
  static DynamicsWorldConfig New();

  /**
   * @brief Default constructor, creates an uninitalized handle.
   */
  DynamicsWorldConfig();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DynamicsWorldConfig();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  DynamicsWorldConfig(const DynamicsWorldConfig& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  DynamicsWorldConfig& operator=(const DynamicsWorldConfig& rhs);

  /**
   * @brief This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   *
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  DynamicsWorldConfig& operator=(BaseHandle::NullType* rhs);

public:
  /**
   * @brief Set the type of Dynamics world required for the application.
   *
   * @param[in] type A member of WorldType enumeration.
   */
  void SetType( const WorldType type );

  /**
   * @brief Get the type of Dynamics World specified by the Configuration.
   *
   * @return A member of WorldType enumeration.
   */
  WorldType GetType() const;

  /**
   * @brief Set the world unit.
   *
   * The simulation units are in meters
   * The entire simulation can be scaled by changing the world unit
   * EG calling SetUnit(0.01f) will scale to centimeters
   * position (Actor::SetPosition...), forces (DynamicsBody::SetLinearVelocity...) and sizes (DynamicsShape...) passed to the simulation
   * @param[in] unit The world unit [default: 0.01f - 1 DALi unit == 1 simulation centimeter]
   */
  void SetUnit(float unit);

  /**
   * @brief Get the current world unit.
   *
   * @return The current world unit
   * @ref SetUnit
   */
  float GetUnit() const;

  /**
   * @brief Set the number of simulation steps / DALi update tick.
   *
   * Use this to advance the simulation in smaller time steps, thus gaining a more accurate
   * simulation for collision detection.
   * Example: assume DAli is updating at 60Hz (16.667ms / update)
   * Setting subSteps to 1 will update the simulation once per DALi update.
   * Setting subSteps to 4 will perform 4 simulation updates per DALi update each with a time step of Approx 4.2ms.
   * @param[in] subSteps The number of simulation steps per update tick
   */
  void SetSimulationSubSteps(int subSteps);

  /**
   * @brief Get the number of simulation steps / DALi update tick.
   *
   * @return The number of simulation steps per update tick
   */
  int GetSimulationSubSteps() const;

  // Not intended for application developers
private:
  /**
   * @brief This constructor is used internally by Dali.
   *
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL DynamicsWorldConfig( Internal::DynamicsWorldConfig* internal );
}; // class DynamicsWorldConfig

} // namespace Dali

#endif /* __DALI_DYNAMICS_WORLD_CONFIG_H__ */
