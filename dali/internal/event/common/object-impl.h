#ifndef __DALI_INTERNAL_OBJECT_H__
#define __DALI_INTERNAL_OBJECT_H__

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
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/event/common/property-metadata.h>
#include <dali/internal/update/common/property-base.h>

namespace Dali
{

class PropertyNotification;

namespace Internal
{
class ConstraintBase;
class EventThreadServices;
class PropertyCondition;
class PropertyInputImpl;
class Stage;
class TypeInfo;

namespace SceneGraph
{
class PropertyBase;
class PropertyOwner;
}

using ConstraintContainer = std::vector< Dali::Constraint >;
using ConstraintIter = ConstraintContainer::iterator;
using ConstraintConstIter = ConstraintContainer::const_iterator;

/**
 * A base class for objects which optionally provide properties.
 * The concrete derived class is responsible for implementing the property system methods.
 * Classes may derive from Dali::BaseObject, until any properties are required.
 *
 * An object for a property-owning object in the scene-graph.
 * This provides an interface for observing the addition/removal of scene-objects.
 *
 * The derived class should either:
 *   a) Create their own scene-graph object and pass it to Object constructor.
 *   b) pass nullptr to Object constructor, in this case Object will create default scene object for property handling
 */
class Object : public Dali::BaseObject
{
public:

  using Capability = Dali::Handle::Capability;

  class Observer
  {
  public:

    /**
     * Called immediately after the object has created & passed ownership of a scene-graph object.
     * @param[in] object The object object.
     */
    virtual void SceneObjectAdded(Object& object) = 0;

    /**
     * Called shortly before the object sends a message to remove its scene object.
     * @param[in] object The object object.
     */
    virtual void SceneObjectRemoved(Object& object) = 0;

    /**
     * Called shortly before the object itself is destroyed; no further callbacks will be received.
     * @param[in] object The object object.
     */
    virtual void ObjectDestroyed(Object& object) = 0;

  protected:

    /**
     * Virtual destructor
     */
    virtual ~Observer(){}
  };

  /**
   * Creates a new object
   *
   * @return an smart pointer to the object
   */
  static IntrusivePtr<Object> New();

  /**
   * Add an observer to the object.
   * @param[in] observer The observer to add.
   */
  void AddObserver( Observer& observer );

  /**
   * Remove an observer from the object
   * @pre The observer has already been added.
   * @param[in] observer The observer to remove.
   */
  void RemoveObserver( Observer& observer );

  /**
   * @copydoc Dali::Handle::Supports()
   */
  bool Supports( Capability capability ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyCount()
   */
  uint32_t GetPropertyCount() const;

  /**
   * @copydoc Dali::Handle::GetPropertyName()
   */
  std::string GetPropertyName( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndex()
   */
  Property::Index GetPropertyIndex( const std::string& name ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndex()
   */
  Property::Index GetPropertyIndex( Property::Index key ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndex()
   */
  Property::Index GetPropertyIndex( Property::Key key ) const;

  /**
   * @copydoc Dali::Handle::IsPropertyWritable()
   */
  bool IsPropertyWritable( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::IsPropertyAnimatable()
   */
  bool IsPropertyAnimatable( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::IsPropertyAConstraintInput()
   */
  bool IsPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyType()
   */
  Property::Type GetPropertyType( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::SetProperty()
   */
  void SetProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Handle::GetProperty()
   */
  Property::Value GetProperty( Property::Index index ) const;

  /**
   * @brief Retrieves the latest value of the property on the scene-graph.
   * @param[in]  index  The index of the property required.
   * @return The latest value of the property on the scene-graph.
   */
  Property::Value GetCurrentProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndices()
   */
  void GetPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Handle::RegisterProperty()
   */
  Property::Index RegisterProperty( const std::string& name, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Handle::RegisterProperty()
   */
  Property::Index RegisterProperty( const std::string& name, Property::Index key, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Handle::RegisterProperty(std::string name, Property::Value propertyValue, Property::AccessMode accessMode)
   */
  Property::Index RegisterProperty( const std::string& name, const Property::Value& propertyValue, Property::AccessMode accessMode );

  /**
   * @brief Implementing method for this override
   */
  Property::Index RegisterProperty( const std::string& name, Property::Index key, const Property::Value& propertyValue, Property::AccessMode accessMode );

  /**
   * @brief returns true if the custom property exists on this object.
   *
   * @note The property may be defined for a type within the type registry, but it isn't explicity
   * defined in every consequent instantiation. It can be automatically added, e.g. parenting an actor
   * automatically registers it's parent container's child properties.
   *
   * @param[in] handle The handle of the object to test
   * @param[in] index The property index to look for.
   * @return true if the property exists on the object, false otherwise.
   */
  bool DoesCustomPropertyExist( Property::Index index );

  /**
   * @copydoc Dali::Handle::AddPropertyNotification()
   */
  Dali::PropertyNotification AddPropertyNotification( Property::Index index,
                                                      int32_t componentIndex,
                                                      const Dali::PropertyCondition& condition );

  /**
   * @copydoc Dali::Handle::RemovePropertyNotification()
   */
  void RemovePropertyNotification( Dali::PropertyNotification propertyNotification );

  /**
   * @copydoc Dali::Handle::RemovePropertyNotifications()
   */
  void RemovePropertyNotifications();

  /**
   * Notifies that a property is being animated.
   * @param[in] animation The animation animating the property.
   * @param[in] index The index of the property.
   * @param[in] value The value of the property after the animation.
   * @param[in] animationType Whether the property value given is the target or a relative value.
   */
  void NotifyPropertyAnimation( Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType );

  /******************************** Uniform Mappings ********************************/

  /**
   * Adds uniform mapping for given property
   * @param propertyIndex index of the property
   * @param uniformName name of the uniform (same as property name)
   */
  void AddUniformMapping( Property::Index propertyIndex, const std::string& uniformName ) const;

  /**
   * Removes uniform mapping for given property
   * @param uniformName name of the uniform (same as property name)
   */
  void RemoveUniformMapping( const std::string& uniformName ) const;

  /******************************** Constraints ********************************/

  /**
   * Apply a constraint to an Object.
   * @param[in] constraint The constraint to apply.
   */
  void ApplyConstraint( ConstraintBase& constraint );

  /**
   * Remove one constraint from an Object.
   * @param[in] constraint The constraint to remove.
   */
  void RemoveConstraint( ConstraintBase& constraint );

  /**
   * Remove all constraints from a Object.
   */
  void RemoveConstraints();

  /**
   * @copydoc Dali::Handle::RemoveConstraints( uint32_t )
   */
  void RemoveConstraints( uint32_t tag );

  /**
   * Called by TypeInfo to set the type-info that this object-impl is created by.
   * @param[in] typeInfo The TypeInfo that creates this object-impl.
   */
  void SetTypeInfo( const TypeInfo* typeInfo );

  /**
   * @return the index from which custom properties start
   */
  uint32_t CustomPropertyStartIndex()
  {
    return PROPERTY_CUSTOM_START_INDEX;
  }

  /**
   * Retrieve the scene-graph object added by this object.
   * @return reference to the scene-graph object, it will always exist
   */
  const SceneGraph::PropertyOwner& GetSceneObject() const;

  /********************  Can be overridden by deriving classes ********************/

  /**
   * Retrieve an animatable property owned by the scene-graph object.
   * @pre -1 < index < GetPropertyCount().
   * @param[in] index The index of the property.
   * @return A dereferenceable pointer to a property, or NULL if a scene-object does not exist with this property.
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * Retrieve a constraint input-property owned by the scene-graph object.
   * @pre -1 < index < GetPropertyCount().
   * @param[in] index The index of the property.
   * @return A dereferenceable pointer to an input property, or NULL if a scene-object does not exist with this property.
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

  /**
   * Query whether the property is a component of a scene-graph property.
   * @pre -1 < index < GetPropertyCount().
   * @param[in] index The index of the property.
   * @return The index or Property::INVALID_COMPONENT_INDEX.
   */
  virtual int32_t GetPropertyComponentIndex( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::PropertySetSignal()
   */
  DevelHandle::PropertySetSignalType& PropertySetSignal();

protected:

  /**
   * Constructor. Protected so use New to construct an instance of this class
   *
   * @param sceneObject the scene graph property owner
   */
  Object( const SceneGraph::PropertyOwner* sceneObject );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Object();

  /**
   * Called immediately by derived classes, after the scene-object has been created & passed to the scene-graph.
   */
  void OnSceneObjectAdd();

  /**
   * Called by derived classes, shortly before send a message to remove the scene-object.
   */
  void OnSceneObjectRemove();

  /**
   * For overriding by derived classes to return the parent of this object.
   */
  virtual Object* GetParentObject() const
  {
    // By default the Object does not have a parent
    return nullptr;
  };

  /**
   * For use in derived classes.
   * This is called after a property is set.
   * @param [in] index The index of the property.
   * @param [in] propertyValue The value of the property.
   */
  virtual void OnPropertySet( Property::Index index, Property::Value propertyValue ) {}

  /**
   * Retrieves the TypeInfo for this object. Only retrieves it from the type-registry once and then stores a pointer
   * to it locally there-after. The type info will not change during the life-time of the application.
   * @return The type-info for this object (Can be NULL)
   */
  const TypeInfo* GetTypeInfo() const;

  /**
   * Helper to find custom property
   * @param index
   * @return pointer to the property
   */
  CustomPropertyMetadata* FindCustomProperty( Property::Index index ) const;

  /**
   * Helper to find animatable property
   * @param index
   * @return pointer to the property
   */
  AnimatablePropertyMetadata* FindAnimatableProperty( Property::Index index ) const;

  /**
   * Helper to register a scene-graph property
   * @param [in] name The name of the property.
   * @param [in] key The key of the property
   * @param [in] index The index of the property
   * @param [in] value The value of the property.
   * @return The index of the registered property or Property::INVALID_INDEX if registration failed.
   */
  Property::Index RegisterSceneGraphProperty( const std::string& name, Property::Index key, Property::Index index, const Property::Value& propertyValue ) const;

  /**
   * Registers animatable scene property
   * @param typeInfo to check the default value
   * @param index of the property to register
   * @param value initial value or nullptr
   */
  void RegisterAnimatableProperty( const TypeInfo& typeInfo, Property::Index index, const Property::Value* value ) const;

  /**
   * Check whether the animatable property is registered already, if not then register on.
   * @param [in] index The index of the property
   * @param [in] value optional value for the property
   * @return pointer to the property metadata
   */
  AnimatablePropertyMetadata* GetSceneAnimatableProperty( Property::Index index, const Property::Value* value ) const;

  /**
   * Resolve the index and name of child properties if any.
   */
  void ResolveChildProperties();

private: // Default property extensions for derived classes

  /**
   * Set the value of a default property.
   * @pre The property types match i.e. propertyValue.GetType() is equal to GetPropertyType(index).
   * @param [in] index The index of the property.
   * @param [in] propertyValue The new value of the property.
   */
  virtual void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * Retrieve a default property value.
   * @param [in] index The index of the property.
   * @return The property value.
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * Retrieve the latest scene-graph value of a default property.
   * @param[in] index The index of the property.
   * @return The latest scene-graph value of a default property.
   */
  virtual Property::Value GetDefaultPropertyCurrentValue( Property::Index index ) const;

  /**
   * Notifies that a default property is being animated so the deriving class should update the cached value.
   * @param[in] animation The animation animating the property.
   * @param[in] index The index of the property.
   * @param[in] value The value of the property after the animation.
   * @param[in] animationType Whether the property value given is the target or a relative value.
   */
  virtual void OnNotifyDefaultPropertyAnimation( Animation& animation, Property::Index index, const Property::Value& value, Animation::Type propertyChangeType )
  { }

private:

  // no default, copy or assignment
  Object() = delete;
  Object(const Object& rhs) = delete;
  Object& operator=(const Object& rhs) = delete;

  /**
   * Enable property notifications in scene graph
   */
  void EnablePropertyNotifications();

  /**
   * Enable property notifications in scene graph
   */
  void DisablePropertyNotifications();

  /**
   * Get the latest value of the property on the scene-graph.
   * @param[in] entry An entry from the property lookup container.
   * @return The latest value of the property.
   */
  Property::Value GetCurrentPropertyValue( const PropertyMetadata& entry ) const;

  /**
   * Set the value of scene graph property.
   * @param [in] index The index of the property.
   * @param [in] entry An entry from the property lookup container.
   * @param [in] value The new value of the property.
   */
  virtual void SetSceneGraphProperty( Property::Index index, const PropertyMetadata& entry, const Property::Value& value );

protected:
  /**
   * Get the event thread services object - used for sending messages to the scene graph
   * Assert if called from the wrong thread.
   * This is intentionally inline for performance reasons.
   *
   * @return The event thread services object
   */
  inline EventThreadServices& GetEventThreadServices()
  {
    DALI_ASSERT_DEBUG( EventThreadServices::IsCoreRunning() );
    return mEventThreadServices;
  }

  /**
   * Get the event thread services object - used for sending messages to the scene graph
   * Assert if called from the wrong thread
   * This is intentionally inline for performance reasons.
   *
   * @return The event thread services object
   */
  inline const EventThreadServices& GetEventThreadServices() const
  {
    DALI_ASSERT_DEBUG( EventThreadServices::IsCoreRunning() );
    return mEventThreadServices;
  }

private:

  EventThreadServices& mEventThreadServices;

protected:

  // mutable because it's lazy initialised and GetSceneObject has to be const so it can be called from const methods
  // const to prevent accidentally calling setters directly from event thread
  // protected to allow fast access from derived classes that have their own scene object (no function call overhead)
  mutable const SceneGraph::PropertyOwner* mUpdateObject; ///< Reference to object to hold the scene graph properties

private:

  Dali::Vector<Observer*> mObservers;
  mutable OwnerContainer<PropertyMetadata*> mCustomProperties; ///< Used for accessing custom Node properties
  mutable OwnerContainer<PropertyMetadata*> mAnimatableProperties; ///< Used for accessing animatable Node properties
  mutable const TypeInfo* mTypeInfo; ///< The type-info for this object, mutable so it can be lazy initialized from const method if it is required

  ConstraintContainer* mConstraints;               ///< Container of owned -constraints.

  using PropertyNotificationContainer = std::vector< Dali::PropertyNotification >;
  PropertyNotificationContainer* mPropertyNotifications; ///< Container of owned property notifications.
  DevelHandle::PropertySetSignalType mPropertySetSignal;

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Object& GetImplementation(Dali::Handle& object)
{
  DALI_ASSERT_ALWAYS( object && "Object handle is empty" );

  BaseObject& handle = object.GetBaseObject();

  return static_cast<Internal::Object&>(handle);
}

inline const Internal::Object& GetImplementation(const Dali::Handle& object)
{
  DALI_ASSERT_ALWAYS( object && "Object handle is empty" );

  const BaseObject& handle = object.GetBaseObject();

  return static_cast<const Internal::Object&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_OBJECT_H__
