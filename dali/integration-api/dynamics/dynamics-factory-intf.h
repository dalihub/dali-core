#ifndef __DALI_INTEGRATION_DYNAMICS_FACTORY_INTF_H__
#define __DALI_INTEGRATION_DYNAMICS_FACTORY_INTF_H__

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

namespace Dali
{

namespace Integration
{

class  DynamicsBody;
class  DynamicsJoint;
class  DynamicsShape;
class  DynamicsWorld;
struct DynamicsWorldSettings;

/**
 *
 */
class DALI_IMPORT_API DynamicsFactory
{
public:

  /**
   * Destructor
   */
  virtual ~DynamicsFactory() {}

  /**
   * Initializes the dynamics simulation. Loads/links a dynamics library which can support
   * all the features requested in worldSettings.
   * @param[in] worldSettings Configuration data for the simulation.
   * @return true if successful, otherwise false.
   */
  virtual bool InitializeDynamics( const DynamicsWorldSettings& worldSettings ) = 0;

  /**
   * Terminates the dynamics simulation. Unloads the dynamics library.
   */
  virtual void TerminateDynamics() = 0;

  /**
   * Create a DynamicsWorld object.
   * @return A pointer to a DynamicsWorld object if successful, otherwise NULL.
   */
  virtual DynamicsWorld* CreateDynamicsWorld() = 0;

  /**
   * Create a DynamicsBody object.
   * @return A pointer to a DynamicsBody object if successful, otherwise NULL.
   */
  virtual DynamicsBody* CreateDynamicsBody( ) = 0;

  /**
   * Create a DynamicsJoint object.
   * @return A pointer to a DynamicsJoint object if successful, otherwise NULL.
   */
  virtual DynamicsJoint* CreateDynamicsJoint() = 0;

  /**
   * Create a DynamicsShape object.
   * @return A pointer to a DynamicsShape object if successful, otherwise NULL.
   */
  virtual DynamicsShape* CreateDynamicsShape() = 0;

}; // class DynamicsFactory

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_DYNAMICS_FACTORY_INTF_H__
