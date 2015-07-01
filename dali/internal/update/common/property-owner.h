#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/update/animation/scene-graph-constraint-declarations.h>


namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class PropertyOwner;

typedef OwnerContainer< PropertyBase* > OwnedPropertyContainer;
typedef OwnedPropertyContainer::Iterator  OwnedPropertyIter;

/**
 * An update-thread object which own properties.
 * This allows observers to track the lifetime of the object & its properties.
 */
class PropertyOwner
{
public:

  class Observer
  {
  public:

    /**
     * Called when the observable object is connected to the scene graph.
     * @param[in] owner A reference to the connected PropertyOwner
     */
    virtual void PropertyOwnerConnected( PropertyOwner& owner ) = 0;

    /**
     * Called when the observable object is disconnected from the scene graph.
     * @param[in] currentBufferIndex The buffer to reset.
     * @param[in] owner A reference to the disconnected PropertyOwner
     */
    virtual void PropertyOwnerDisconnected( BufferIndex updateBufferIndex, PropertyOwner& owner ) = 0;

    /**
     * Called shortly before the observable object is destroyed.
     *
     * @note Cleanup should be done in both this and PropertyOwnerDisconnected as PropertyOwnerDisconnected
     * may not be called (i.e. when shutting down).
     */
    virtual void PropertyOwnerDestroyed( PropertyOwner& owner ) = 0;
  };

  /**
   * Create a property owner.
   * @return A newly allocated object.
   */
  static PropertyOwner* New();

  /**
   * Virtual destructor; this is intended as a base class.
   */
  virtual ~PropertyOwner();

  /**
   * Add an observer.
   * The observer is responsible for calling RemoveObserver(*this) during its own destruction.
   * Connecting an actor-side object as an observer is not allowed, due to thread-safety issues.
   * @param[in] observer The observer.
   */
  void AddObserver(Observer& observer);

  /**
   * Remove an observer.
   * @param[in] observer The observer.
   */
  void RemoveObserver(Observer& observer);

  /**
   * This method can be used to determine if there is an animation or
   * constraint that is using this property owner.
   * @return true if there are observers.
   */
  bool IsObserved();

  /**
   * Called just before destruction to disconnect all observers and remove constraints.
   * This occurs when the object is in the process of being destroyed.
   */
  void Destroy();

  /**
   * Notify all observers that the object has been connected
   * This occurs when the object is connected to the scene-graph during UpdateManager::Update().
   */
  void ConnectToSceneGraph();

  /**
   * Notify all observers that the object has been disconnected and remove constraints.
   * This occurs when the object is disconnected from the scene-graph during UpdateManager::Update().
   * @param[in] currentBufferIndex The current update buffer.
   */
  void DisconnectFromSceneGraph( BufferIndex updateBufferIndex );

  /**
   * Install a custom property.
   * @post The PropertyOwner takes ownership of the property.
   * @param[in] property A pointer to a newly allocated property.
   */
  void InstallCustomProperty(PropertyBase* property);

  /**
   * Retrieve the custom properties owned by the object.
   * @return A container of properties.
   */
  OwnedPropertyContainer& GetCustomProperties()
  {
    return mCustomProperties;
  }

  /**
   * Retrieve the custom properties owned by the object.
   * @return A container of properties.
   */
  const OwnedPropertyContainer& GetCustomProperties() const
  {
    return mCustomProperties;
  }

  /**
   * Reset animatable properties to the corresponding base values.
   * @param[in] currentBufferIndex The buffer to reset.
   * @post The ResetDefaultProperties method is called, during which derived classes can reset default properties.
   */
  void ResetToBaseValues( BufferIndex updateBufferIndex );

  // Constraints

  /**
   * Apply a constraint.
   * @param[in] constraint The constraint to apply.
   */
  void ApplyConstraint( ConstraintBase* constraint );

  /**
   * Begin removal of constraints.
   * @param[in] constraint The constraint to remove.
   */
  void RemoveConstraint( ConstraintBase* constraint );

  /**
   * Retrieve the constraints that are currently applied.
   * @return A container of constraints.
   */
  ConstraintOwnerContainer& GetConstraints();

  /**
   * @copydoc UniformMap::Add
   */
  void AddUniformMapping( UniformPropertyMapping* map );

  /**
   * @copydoc UniformMap::Remove
   */
  void RemoveUniformMapping( const std::string& uniformName );

  /**
   * Get the mappings table
   */
  const UniformMap& GetUniformMap() const;

  /**
   * @copydoc UniformMap::AddUniformMapObserver
   */
  void AddUniformMapObserver( UniformMap::Observer& observer );

  /**
   * @copydoc UniformMap::RemoveUniformMapObserver
   */
  void RemoveUniformMapObserver( UniformMap::Observer& observer );


protected:

  /**
   * Protected constructor.
   */
  PropertyOwner();

private:

  // Undefined
  PropertyOwner(const PropertyOwner&);

  // Undefined
  PropertyOwner& operator=(const PropertyOwner& rhs);

  /**
   * Called after ResetToBaseValues; derived classes should reset any default properties.
   * @param[in] currentBufferIndex The buffer to reset.
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex ) {}

protected:

  OwnedPropertyContainer mCustomProperties; ///< Properties provided with InstallCustomProperty()
  UniformMap mUniformMaps; ///< Container of owned uniform maps

private:

  typedef Dali::Vector<PropertyOwner::Observer*> ObserverContainer;
  typedef ObserverContainer::Iterator ObserverIter;
  typedef ObserverContainer::ConstIterator ConstObserverIter;

  ObserverContainer mObservers; ///< Container of observer raw-pointers (not owned)

  ConstraintOwnerContainer mConstraints; ///< Container of owned constraints
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_H__
