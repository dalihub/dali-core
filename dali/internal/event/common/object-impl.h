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

typedef std::vector< Dali::Constraint >     ConstraintContainer;
typedef ConstraintContainer::iterator       ConstraintIter;
typedef ConstraintContainer::const_iterator ConstraintConstIter;


/**
 * A base class for objects which optionally provide properties.
 * The concrete derived class is responsible for implementing the property system methods.
 * Classes may derive from Dali::BaseObject, until any properties are required.
 *
 * An object for a property-owning object in the scene-graph.
 * This provides an interface for observing the addition/removal of scene-objects.
 *
 * The concrete derived class is responsible for:
 *   1) Adding & removing an object from the scene-graph. The OnSceneObjectAdd() and OnSceneObjectRemove()
 *      methods should be called by the derived class, to trigger observer callbacks.
 *   3) Implementing the GetSceneObject() methods, used to access the scene-object.
 *   4) Providing access to properties stored by the scene-graph object. These should match the properties
 *      reported by the base Dali::Internal::Object methods.
 *
 */
class Object : public Dali::BaseObject
{
public:

  typedef Dali::Handle::Capability Capability;

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
   * Constructor.
   */
  Object();

  /**
   * Add an observer to the object.
   * @param[in] observer The observer to add.
   */
  virtual void AddObserver( Observer& observer );

  /**
   * Remove an observer from the object
   * @pre The observer has already been added.
   * @param[in] observer The observer to remove.
   */
  virtual void RemoveObserver( Observer& observer );

  /**
   * @copydoc Dali::Handle::Supports()
   */
  virtual bool Supports( Capability capability ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyCount()
   */
  virtual unsigned int GetPropertyCount() const;

  /**
   * @copydoc Dali::Handle::GetPropertyName()
   */
  virtual std::string GetPropertyName( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndex()
   */
  virtual Property::Index GetPropertyIndex( const std::string& name ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndex()
   */
  virtual Property::Index GetPropertyIndex( Property::Index key ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndex()
   */
  virtual Property::Index GetPropertyIndex( Property::Key key ) const;

  /**
   * @copydoc Dali::Handle::IsPropertyWritable()
   */
  virtual bool IsPropertyWritable( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::IsPropertyAnimatable()
   */
  virtual bool IsPropertyAnimatable( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::IsPropertyAConstraintInput()
   */
  virtual bool IsPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyType()
   */
  virtual Property::Type GetPropertyType( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::SetProperty()
   */
  virtual void SetProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Handle::GetProperty()
   */
  virtual Property::Value GetProperty( Property::Index index ) const;

  /**
   * @brief Retrieves the latest value of the property on the scene-graph.
   * @param[in]  index  The index of the property required.
   * @return The latest value of the property on the scene-graph.
   */
  virtual Property::Value GetCurrentProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndices()
   */
  virtual void GetPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Handle::RegisterProperty()
   */
  virtual Property::Index RegisterProperty( const std::string& name, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Handle::RegisterProperty()
   */
  virtual Property::Index RegisterProperty( const std::string& name, Property::Index key, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Handle::RegisterProperty(std::string name, Property::Value propertyValue, Property::AccessMode accessMode)
   */
  virtual Property::Index RegisterProperty( const std::string& name, const Property::Value& propertyValue, Property::AccessMode accessMode );

  /**
   * @brief Implementing method for this override
   */
  virtual Property::Index RegisterProperty( const std::string& name, Property::Index key, const Property::Value& propertyValue, Property::AccessMode accessMode );

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
  virtual Dali::PropertyNotification AddPropertyNotification( Property::Index index,
                                                              int componentIndex,
                                                              const Dali::PropertyCondition& condition );

  /**
   * @copydoc Dali::Handle::RemovePropertyNotification()
   */
  virtual void RemovePropertyNotification( Dali::PropertyNotification propertyNotification );

  /**
   * @copydoc Dali::Handle::RemovePropertyNotifications()
   */
  virtual void RemovePropertyNotifications();

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
   * @copydoc Dali::Handle::AddUniformMapping()
   */
  void AddUniformMapping( Property::Index propertyIndex, const std::string& uniformName ) const;

  /**
   * @copydoc Dali::Handle::RemoveUniformMapping( )
   */
  void RemoveUniformMapping( const std::string& uniformName );

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
   * Remove all constraints from a Object with a matching tag
   */
  void RemoveConstraints( unsigned int tag );

  /**
   * Called by TypeInfo to set the type-info that this object-impl is created by.
   * @param[in] typeInfo The TypeInfo that creates this object-impl.
   */
  void SetTypeInfo( const TypeInfo* typeInfo );

  /**
   * @return the index from which custom properties start
   */
  unsigned int CustomPropertyStartIndex()
  {
    return PROPERTY_CUSTOM_START_INDEX;
  }

  /********************  To be overridden by deriving classes ********************/

  /**
   * Retrieve the scene-graph object added by this object.
   * @return A pointer to the object, or NULL if no object has been added to the scene-graph.
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const = 0;

  /**
   * Retrieve an animatable property owned by the scene-graph object.
   * @pre -1 < index < GetPropertyCount().
   * @param[in] index The index of the property.
   * @return A dereferenceable pointer to a property, or NULL if a scene-object does not exist with this property.
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const = 0;

  /**
   * Retrieve a constraint input-property owned by the scene-graph object.
   * @pre -1 < index < GetPropertyCount().
   * @param[in] index The index of the property.
   * @return A dereferenceable pointer to an input property, or NULL if a scene-object does not exist with this property.
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const = 0;

  /**
   * Query whether the property is a component of a scene-graph property.
   * @pre -1 < index < GetPropertyCount().
   * @param[in] index The index of the property.
   * @return The index or Property::INVALID_COMPONENT_INDEX.
   */
  virtual int GetPropertyComponentIndex( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::PropertySetSignal()
   */
  DevelHandle::PropertySetSignalType& PropertySetSignal();

protected:

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
    return NULL;
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
  Property::Index RegisterSceneGraphProperty(const std::string& name, Property::Index key, Property::Index index, const Property::Value& propertyValue) const;

  /**
   * Check whether the animatable property is registered already, if not then register one.
   * @param [in] index The index of the property
   * @return pointer to the property.
   */
  AnimatablePropertyMetadata* RegisterAnimatableProperty(Property::Index index) const;

  /**
   * Resolve the index and name of child properties if any.
   */
  void ResolveChildProperties();

private: // Default property extensions for derived classes

  /**
   * Query how many default properties the derived class supports.
   * @return The number of default properties.
   */
  virtual unsigned int GetDefaultPropertyCount() const = 0;

  /**
   * Retrieve all the indices that are associated with the default properties supported by the derived class.
   * @return A container of default property indices.
   * @note The deriving class must not modify the existing elements in the container.
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const = 0;

  /**
   * Query how many default properties the derived class supports.
   * @return The number of default properties.
   */
  virtual const char* GetDefaultPropertyName( Property::Index index ) const = 0;

  /**
   * Query the index of a default property.
   * @param [in] name The name of the property.
   * @return The index of the property, or Property::INVALID_INDEX if no default property exists with the given name.
   */
  virtual Property::Index GetDefaultPropertyIndex( const std::string& name ) const = 0;

  /**
   * Query whether a default property is writable.
   * @param [in] index The index of the property.
   * @return True if the property is animatable.
   */
  virtual bool IsDefaultPropertyWritable( Property::Index index ) const = 0;

  /**
   * Query whether a default property is animatable.
   * This determines whether the property can be the target of an animation or constraint.
   * @param [in] index The index of the property.
   * @return True if the property is animatable.
   */
  virtual bool IsDefaultPropertyAnimatable( Property::Index index ) const = 0;

  /**
   * @brief Query whether a default property can be used as an input to a constraint.
   *
   * @param [in] index The index of the property.
   * @return True if the property can be used as an input to a constraint.
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const = 0;

  /**
   * Query the type of a default property.
   * @param [in] index The index of the property.
   * @return The type of the property.
   */
  virtual Property::Type GetDefaultPropertyType( Property::Index index ) const = 0;

  /**
   * Set the value of a default property.
   * @pre The property types match i.e. propertyValue.GetType() is equal to GetPropertyType(index).
   * @param [in] index The index of the property.
   * @param [in] propertyValue The new value of the property.
   */
  virtual void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue ) = 0;

  /**
   * Retrieve a default property value.
   * @param [in] index The index of the property.
   * @return The property value.
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const = 0;

  /**
   * Retrieve the latest scene-graph value of a default property.
   * @param[in] index The index of the property.
   * @return The latest scene-graph value of a default property.
   */
  virtual Property::Value GetDefaultPropertyCurrentValue( Property::Index index ) const = 0;

  /**
   * Notifies that a default property is being animated so the deriving class should update the cached value.
   * @param[in] animation The animation animating the property.
   * @param[in] index The index of the property.
   * @param[in] value The value of the property after the animation.
   * @param[in] animationType Whether the property value given is the target or a relative value.
   */
  virtual void OnNotifyDefaultPropertyAnimation( Animation& animation, Property::Index index, const Property::Value& value, Animation::Type propertyChangeType )
  { }

  /**
   * @todo this is virtual so that for now actor can override it,
   * it needs to be removed and only have GetSceneObject but that requires changing actor and constraint logic
   * Retrieve the scene-graph object added by this object.
   * @return A pointer to the object, or NULL if no object has been added to the scene-graph.
   */
  virtual const SceneGraph::PropertyOwner* GetPropertyOwner() const
  {
    return GetSceneObject();
  }

  /**
   * Notify derived class of installation of a new scene-object property.
   * This method is called after the message is to sent to install the property
   * @param [in] newProperty A newly allocated scene-object property. Ownership is obviously not passed.
   * @param [in] name The name allocated to this custom property.
   * @param [in] index The index allocated to this custom property.
   */
  virtual void NotifyScenePropertyInstalled( const SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index ) const
  { }

private:

  // Not implemented
  Object(const Object& rhs);

  // Not implemented
  Object& operator=(const Object& rhs);

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
  Property::Value GetCurrentPropertyValue( const PropertyMetadata* entry ) const;

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

private:

  typedef OwnerContainer<PropertyMetadata*> PropertyMetadataLookup;
  mutable PropertyMetadataLookup mCustomProperties; ///< Used for accessing custom Node properties
  mutable PropertyMetadataLookup mAnimatableProperties; ///< Used for accessing animatable Node properties
  mutable TypeInfo const *  mTypeInfo; ///< The type-info for this object, mutable so it can be lazy initialized from const method if it is required

  Dali::Vector<Observer*> mObservers;

  ConstraintContainer* mConstraints;               ///< Container of owned -constraints.

  typedef std::vector< Dali::PropertyNotification >     PropertyNotificationContainer;
  typedef PropertyNotificationContainer::iterator       PropertyNotificationContainerIter;
  typedef PropertyNotificationContainer::const_iterator PropertyNotificationContainerConstIter;
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
