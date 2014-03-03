#ifndef __DALI_DYNAMICS_WORLD_CONFIG_H__
#define __DALI_DYNAMICS_WORLD_CONFIG_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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
 * Describes the requirements/capabilities of a physics siumlation encapsulated by a DynamicsWorld
 */
class DynamicsWorldConfig : public BaseHandle
{
public:
  /**
   * Enumeration specifying the capabilities required by the dynamics simulation
   */
  enum WorldType
  {
    RIGID,                ///< Adds rigid body dynamics (gravity, mass, velocity, friction)
    SOFT,                 ///< Adds soft body dynamics - implies RIGID
  }; // enum WorldType


public:

  /**
   * Create a new DynamicsWorld configuration object
   * All values are set to default values...
   */
  static DynamicsWorldConfig New();

  /**
   * Default constructor, creates an uninitalized handle
   */
  DynamicsWorldConfig();

  /**
   * Virtual destructor.
   */
  virtual ~DynamicsWorldConfig();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public:
  /**
   * Set the type of Dynamics world required for the application
   * @param[in] type A member of WorldType enumeration.
   */
  void SetType( const WorldType type );

  /**
   * Get the type of Dynamics World specified by the Configuration
   * @return A member of WorldType enumeration.
   */
  WorldType GetType() const;

  /**
   * Set the world unit.
   * The simulation units are in meters
   * The entire simulation can be scaled by changing the world unit
   * EG calling SetUnit(0.01f) will scale to centimeters
   * position (Actor::SetPosition...), forces (DynamicsBody::SetLinearVelocity...) and sizes (DynamicsShape...) passed to the simulation
   * @param[in] unit The world unit [default: 0.01f - 1 DALi unit == 1 simulation centimeter]
   */
  void SetUnit(const float unit);

  /**
   * Get the current world unit.
   * @return The current world unit
   * @ref SetUnit
   */
  const float GetUnit() const;

  /**
   * Set the number of simulation steps / DALi update tick.
   * Use this to advance the simulation in smaller time steps, thus gaining a more accurate
   * simulation for collision detection.
   * Example: assume DAli is updating at 60Hz (16.667ms / update)
   * Setting subSteps to 1 will update the simulation once per DALi update.
   * Setting subSteps to 4 will perform 4 simulation updates per DALi update each with a time step of Approx 4.2ms.
   * @param[in] subSteps The number of simulation steps per update tick
   */
  void SetSimulationSubSteps( const int subSteps);

  /**
   * Get the number of simulation steps / DALi update tick
   * @return The number of simulation steps per update tick
   */
  const int GetSimulationSubSteps() const;

  // Not intended for application developers
private:
  /**
   * This constructor is used internally by Dali
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL DynamicsWorldConfig( Internal::DynamicsWorldConfig* internal );
}; // class DynamicsWorldConfig

} // namespace Dali

#endif /* __DALI_DYNAMICS_WORLD_CONFIG_H__ */
