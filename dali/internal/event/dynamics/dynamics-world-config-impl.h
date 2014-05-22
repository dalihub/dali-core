#ifndef __DYNAMICS_WORLD_CONFIG_IMPL_H__
#define __DYNAMICS_WORLD_CONFIG_IMPL_H__

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

// BASE CLASS HEADER
#include <dali/public-api/object/base-object.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/public-api/dynamics/dynamics-world-config.h>

namespace Dali
{

namespace Integration
{

struct DynamicsWorldSettings;

} // namespace Integration

namespace Internal
{
/// @copydoc Dali::DynamicsWorldConfig
class DynamicsWorldConfig : public BaseObject
{
public:
  /**
   * Constructor.
   */
  DynamicsWorldConfig();

protected:
  /**
   * Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsWorldConfig();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsWorldConfig(const DynamicsWorldConfig&);
  DynamicsWorldConfig& operator=(const DynamicsWorldConfig&);

  // Methods
public:
  /// @copydoc Dali::DynamicsWorldConfig::SetType
  void SetType( const Dali::DynamicsWorldConfig::WorldType type );

  /// @copydoc Dali::DynamicsWorldConfig::GetType
  Dali::DynamicsWorldConfig::WorldType GetType() const;

  /// @copydoc Dali::DynamicsWorldConfig::SetGravity
  void SetGravity( const Vector3& gravity );

  /// @copydoc Dali::DynamicsWorldConfig::GetGravity
  const Vector3& GetGravity() const;

  /// @copydoc Dali::DynamicsWorldConfig::SetUnit
  void SetUnit(const float unit);

  /// @copydoc Dali::DynamicsWorldConfig::GetUnit
  const float GetUnit() const;

  /// @copydoc Dali::DynamicsWorldConfig::SetSimulationSubSteps
  void SetSimulationSubSteps( const int subSteps);

  /// @copydoc Dali::DynamicsWorldConfig::GetSimulationSubSteps
  const int GetSimulationSubSteps() const;

  Integration::DynamicsWorldSettings* GetSettings() const;

private:
  Integration::DynamicsWorldSettings* mSettings;
}; // class DynamicsWorldConfig

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::DynamicsWorldConfig& GetImplementation(DynamicsWorldConfig& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsWorldConfig object is uninitialized!");

  Dali::RefObject& handle = object.GetBaseObject();

  return static_cast<Internal::DynamicsWorldConfig&>(handle);
}

inline const Internal::DynamicsWorldConfig& GetImplementation(const DynamicsWorldConfig& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsWorldConfig object is uninitialized!");

  const Dali::RefObject& handle = object.GetBaseObject();

  return static_cast<const Internal::DynamicsWorldConfig&>(handle);
}

} // namespace Dali

#endif // __DYNAMICS_WORLD_CONFIG_IMPL_H__
