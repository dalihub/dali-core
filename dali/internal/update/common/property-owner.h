#ifndef DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_H
#define DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
#include <dali/devel-api/common/map-wrapper.h>
#else
#include <unordered_map>
#endif

// INTERNAL INCLUDES
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/common/const-string.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/animation/scene-graph-constraint-container.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class PropertyOwner;
class ResetterManager;
class PropertyOwnerFlagManager;

using OwnedPropertyContainer = OwnerContainer<PropertyBase*>;
using OwnedPropertyIter      = OwnedPropertyContainer::Iterator;

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
    enum NotifyReturnType
    {
      STOP_OBSERVING,
      KEEP_OBSERVING,
    };

  public:
    /**
     * Called when the observable object is connected to the scene graph.
     * @param[in] owner A reference to the connected PropertyOwner
     */
    virtual void PropertyOwnerConnected(PropertyOwner& owner) = 0;

    /**
     * Called when the observable object is disconnected from the scene graph.
     * @param[in] owner A reference to the disconnected PropertyOwner
     * @return NotifyReturnType::STOP_OBSERVING if we will not observe this object after this called
     *         NotifyReturnType::KEEP_OBSERVING if we will observe this object after this called.
     */
    virtual NotifyReturnType PropertyOwnerDisconnected(PropertyOwner& owner) = 0;

    /**
     * Called shortly before the observable object is destroyed.
     *
     * @note Cleanup should be done in both this and PropertyOwnerDisconnected as PropertyOwnerDisconnected
     * may not be called (i.e. when shutting down).
     */
    virtual void PropertyOwnerDestroyed(PropertyOwner& owner) = 0;

  protected:
    /**
     * Virtual destructor, no deletion through this interface
     */
    virtual ~Observer() = default;
  };

  /**
   * Create a property owner.
   * @return A newly allocated object.
   */
  static PropertyOwner* New();

  /**
   * @brief Registers a property owner flag manager to control flag resetted.
   *
   * @param [in] manager The manager to register.
   */
  static void RegisterPropertyOwnerFlagManager(PropertyOwnerFlagManager& manager);

  /**
   * @brief Unregisters a property owner flag manager.
   */
  static void UnregisterPropertyOwnerFlagManager();

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
   */
  void DisconnectFromSceneGraph();

  /**
   * Reserve the given number of properties
   */
  void ReserveProperties(int propertyCount);

  /**
   * Install a custom property.
   * @post The PropertyOwner takes ownership of the property.
   * @param[in] property A pointer to a newly allocated property.
   */
  void InstallCustomProperty(OwnerPointer<PropertyBase>& property);

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
   * Request to call ResetUpdated() for this frame at the end of update/render loop.
   */
  void RequestResetUpdated() const;

  /**
   * Reset flag implements at the end of update/render loop.
   * @note Need to base call at the end of function call.
   */
  virtual void ResetUpdated()
  {
    mUpdated = false;
  }

  /**
   * Mark an property owner with the updated flag.
   * @param[in] updated The updated flag
   */
  void SetUpdated(bool updated)
  {
    if(!mUpdated && updated)
    {
      RequestResetUpdated();
    }
    mUpdated = updated;
  }

  /**
   * Retrieve if the property owner is updated due to the property is being animating.
   * @return An updated flag
   */
  bool Updated() const
  {
    return mUpdated;
  }

  // Constraints

  /**
   * Apply a constraint.
   * @param[in] constraint The constraint to apply.
   */
  void ApplyConstraint(OwnerPointer<ConstraintBase>& constraint);

  /**
   * Begin removal of constraints.
   * @param[in] constraint The constraint to remove.
   */
  void RemoveConstraint(ConstraintBase* constraint);

  /**
   * Notify that constraint apply rate chagned.
   * @param[in] constraint The apply rate changed constraint.
   */
  void ConstraintApplyRateChanged(ConstraintBase* constraint);

  /**
   * Apply a post constraint.
   * @param[in] constraint The constraint to apply.
   */
  void ApplyPostConstraint(OwnerPointer<ConstraintBase>& constraint);

  /**
   * Begin removal of post constraints.
   * @param[in] constraint The constraint to remove.
   */
  void RemovePostConstraint(ConstraintBase* constraint);

  /**
   * Notify that post constraint apply rate chagned.
   * @param[in] constraint The apply rate changed post constraint.
   */
  void PostConstraintApplyRateChanged(ConstraintBase* constraint);

  /**
   * Retrieve the constraints that are currently applied.
   * @return A container of constraints.
   */
  ConstraintContainer& GetConstraints();

  /**
   * Retrieve the post constraints that are currently applied.
   * @return A container of post constraints.
   */
  ConstraintContainer& GetPostConstraints();

  /**
   * Retrieve the number of activated post constraints that are currently applied.
   * @return The number of activated post constraints.
   */
  uint32_t GetPostConstraintsActivatedCount() const;

  /**
   * @copydoc UniformMap::Add
   */
  virtual void AddUniformMapping(const UniformPropertyMapping& map);

  /**
   * @copydoc UniformMap::Remove
   */
  virtual void RemoveUniformMapping(const ConstString& uniformName);

  /**
   * Get the mappings table
   */
  const UniformMap& GetUniformMap() const;

  /**
   * Query whether playing an animation is possible or not.
   * @return true if playing an animation is possible.
   */
  virtual bool IsAnimationPossible() const
  {
    return true;
  }

  /**
   * @brief Install custom resetter messages to resetter manager.
   * @pre ConnectToSceneGraph() Should be called before this API.
   *
   * @param[in] manager ResetterManager to add resetter.
   */
  virtual void AddInitializeResetter(ResetterManager& manager) const
  {
    // Do nothing
  }

protected:
  /**
   * Protected constructor.
   */
  PropertyOwner();

  /**
   * Method to inform derived classes when property maps have been modified.
   */
  virtual void OnMappingChanged()
  {
    // Default behaviour is to do nothing
  }

private:
  // Undefined
  PropertyOwner(const PropertyOwner&);

  // Undefined
  PropertyOwner& operator=(const PropertyOwner& rhs);

protected:
  OwnedPropertyContainer mCustomProperties; ///< Properties provided with InstallCustomProperty()
  UniformMap             mUniformMaps;      ///< Container of owned uniform maps
  bool                   mUpdated;
  bool                   mIsConnectedToSceneGraph;

private:
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  using ObserverContainer = std::map<PropertyOwner::Observer*, uint32_t>; ///< Observers container. We allow to add same observer multiple times.
                                                                          ///< Key is a pointer to observer, value is the number of observer added.
#else
  using ObserverContainer = std::unordered_map<PropertyOwner::Observer*, uint32_t>; ///< Observers container. We allow to add same observer multiple times.
                                                                                    ///< Key is a pointer to observer, value is the number of observer added.
#endif
  using ObserverIter      = ObserverContainer::iterator;
  using ConstObserverIter = ObserverContainer::const_iterator;

  ObserverContainer mObservers; ///< Container of observer raw-pointers (not owned)

  ConstraintContainer mConstraints;     ///< Container of owned constraints
  ConstraintContainer mPostConstraints; ///< Container of owned constraints

  bool mObserverNotifying : 1; ///< Whether we are currently notifying observers.
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_H
