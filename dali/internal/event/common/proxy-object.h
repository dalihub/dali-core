#ifndef __DALI_INTERNAL_PROXY_OBJECT_H__
#define __DALI_INTERNAL_PROXY_OBJECT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/constrainable.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/common/custom-property.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/property-base.h>

namespace Dali
{

namespace Internal
{

class Stage;
class PropertyInputImpl;
class ProxyObject;
class Constraint;
class TypeInfo;

namespace SceneGraph
{
class PropertyBase;
class PropertyOwner;
}

typedef std::vector< Dali::ActiveConstraint >     ActiveConstraintContainer;
typedef ActiveConstraintContainer::iterator       ActiveConstraintIter;
typedef ActiveConstraintContainer::const_iterator ActiveConstraintConstIter;

/**
 * A proxy for a property-owning object in the scene-graph.
 * This provides an interface for observing the addition/removal of scene-objects.
 *
 * The concrete derived class is responsible for:
 *   1) Adding & removing an object from the scene-graph. The OnSceneObjectAdd() and OnSceneObjectRemove()
 *      methods should be called by the derived class, to trigger observer callbacks.
 *   3) Implementing the GetSceneObject() methods, used to access the scene-object.
 *   4) Providing access to properties stored by the scene-graph object. These should match the properties
 *      reported by the base Dali::Internal::Object methods.
 */
class ProxyObject : public Object
{
public:

  class Observer
  {
  public:

    /**
     * Called immediately after the proxy has created & passed ownership of a scene-graph object.
     * @param[in] proxy The proxy object.
     */
    virtual void SceneObjectAdded(ProxyObject& proxy) = 0;

    /**
     * Called shortly before the proxy sends a message to remove its scene object.
     * @param[in] proxy The proxy object.
     */
    virtual void SceneObjectRemoved(ProxyObject& proxy) = 0;

    /**
     * Called shortly before the proxy itself is destroyed; no further callbacks will be received.
     * @param[in] proxy The proxy object.
     */
    virtual void ProxyDestroyed(ProxyObject& proxy) = 0;

  protected:
    /**
     * Virtual destructor
     */
    virtual ~Observer(){}
  };

  /**
   * Constructor.
   */
  ProxyObject();

  /**
   * Query whether the proxy object removes (& re-adds) its associated scene-object.
   * Otherwise the scene-object lifetime is expected to match that of the proxy.
   * @return True if scene-objects are removed.
   */
  virtual bool IsSceneObjectRemovable() const = 0;

  /**
   * Add an observer to the proxy.
   * @param[in] observer The observer to add.
   */
  virtual void AddObserver(Observer& observer);

  /**
   * Remove an observer from the proxy
   * @pre The observer has already been added.
   * @param[in] observer The observer to remove.
   */
  virtual void RemoveObserver(Observer& observer);

  /**
   * Retrieve the scene-graph object added by this proxy.
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
   * Retrieve an constraint input-property owned by the scene-graph object.
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

public: // Property system interface from Internal::Object

  /**
   * @copydoc Dali::Internal::Object::Supports()
   */
  virtual bool Supports( Object::Capability capability ) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyCount()
   */
  virtual unsigned int GetPropertyCount() const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyName()
   */
  virtual const std::string& GetPropertyName( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyIndex()
   */
  virtual Property::Index GetPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::Object::IsPropertyWritable()
   */
  virtual bool IsPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsPropertyAnimatable()
   */
  virtual bool IsPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsPropertyAConstraintInput()
   */
  virtual bool IsPropertyAConstraintInput(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyType()
   */
  virtual Property::Type GetPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::SetProperty()
   */
  virtual void SetProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::Object::GetProperty()
   */
  virtual Property::Value GetProperty(Property::Index index) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndices()
   */
  virtual void GetPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::Object::RegisterProperty()
   */
  virtual Property::Index RegisterProperty(std::string name, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Handle::RegisterProperty(std::string name, Property::Value propertyValue, Property::AccessMode accessMode)
   */
  virtual Property::Index RegisterProperty(std::string name, const Property::Value& propertyValue, Property::AccessMode accessMode);

  /**
   * @copydoc Dali::Handle::AddPropertyNotification()
   */
  virtual Dali::PropertyNotification AddPropertyNotification(Property::Index index,
                                                             int componentIndex,
                                                             const Dali::PropertyCondition& condition);

  /**
   * @copydoc Dali::Handle::RemovePropertyNotification()
   */
  virtual void RemovePropertyNotification(Dali::PropertyNotification propertyNotification);

  /**
   * @copydoc Dali::Handle::RemovePropertyNotifications()
   */
  void RemovePropertyNotifications();

private:
  /**
   * Enable property notifications in scene graph
   */
  void EnablePropertyNotifications();

  /**
   * Enable property notifications in scene graph
   */
  void DisablePropertyNotifications();

public: // Constraints

  /**
   * Apply a constraint to a ProxyObject.
   * @param[in] constraint The constraint to apply.
   */
  Dali::ActiveConstraint ApplyConstraint( Constraint& constraint );

  /**
   * Apply a constraint to a ProxyObject.
   * @param[in] constraint The constraint to apply.
   * @param[in] weightObject An object with a "weight" float property.
   */
  Dali::ActiveConstraint ApplyConstraint( Constraint& constraint, Dali::Constrainable weightObject );

  /**
   * Remove one constraint from a ProxyObject.
   * @param[in] activeConstraint The active constraint to remove.
   */
  void RemoveConstraint( Dali::ActiveConstraint activeConstraint );

  /**
   * Remove all constraints from a ProxyObject.
   */
  void RemoveConstraints();

  /**
   * Remove all constraints from a ProxyObject with a matching tag
   */
  void RemoveConstraints( unsigned int tag );

public: // Called by TypeInfo

  /**
   * Called by TypeInfo to set the type-info that this proxy-object is created by.
   * @param[in] typeInfo The TypeInfo that creates this proxy-object.
   */
  void SetTypeInfo( const TypeInfo* typeInfo );

protected:

  /**
   * Called immediately by derived classes, after the scene-object has been created & passed to the scene-graph.
   */
  void OnSceneObjectAdd();

  /**
   * Called by derived classes, shortly before send a message to remove the scene-object.
   */
  void OnSceneObjectRemove();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ProxyObject();

private:

  // Undefined
  ProxyObject(const ProxyObject&);

  // Undefined
  ProxyObject& operator=(const ProxyObject& rhs);

  /**
   * Helper for ApplyConstraint overloads.
   * @param[in] constraint The constraint to apply.
   * @param[in] weightObject An object with a "weight" float property, or an empty handle.
   * @return The new active-constraint which is owned by ProxyObject.
   */
  ActiveConstraintBase* DoApplyConstraint( Constraint& constraint, Dali::Constrainable weightObject );

  /**
   * Helper to delete removed constraints
   */
  void DeleteRemovedConstraints();

  /**
   * Helper to remove active constraints
   */
  void RemoveConstraint( ActiveConstraint& constraint, bool isInScenegraph );


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
  virtual const std::string& GetDefaultPropertyName( Property::Index index ) const = 0;

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
   * Set the value of custom property.
   * @param [in] index The index of the property.
   * @param [in] entry An entry from the CustomPropertyLookup.
   * @param [in] value The new value of the property.
   */
  virtual void SetCustomProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value );

  /**
   * Retrieve a default property value.
   * @param [in] index The index of the property.
   * @return The property value.
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const = 0;

  /**
   * Install a newly allocated scene-object property.
   * The derived class is responsible for this, since the type of scene-object is not known to this base class.
   * @param [in] newProperty A newly allocated scene-object property.
   * @param [in] name The name allocated to this custom property.
   * @param [in] index The index allocated to this custom property.
   */
  virtual void InstallSceneObjectProperty( SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index ) = 0;

protected:

  /**
   * Not all proxy objects will have custom properties so we want to only create the lookup when its actually needed
   * @return the custom property lookup
   */
  CustomPropertyLookup& GetCustomPropertyLookup() const;

  /**
   * Retrieves the TypeInfo for this object. Only retrieves it from the type-registry once and then stores a pointer
   * to it locally there-after. The type info will not change during the life-time of the application.
   * @return The type-info for this object (Can be NULL)
   */
  const TypeInfo* GetTypeInfo() const;

private:

  Property::Index mNextCustomPropertyIndex; ///< The ID of the next custom property to be registered

  mutable CustomPropertyLookup* mCustomProperties; ///< Used for accessing custom Node properties, mutable so it can be lazy initialized from const function
  mutable TypeInfo const *  mTypeInfo; ///< The type-info for this object, mutable so it can be lazy initialized from const method if it is required

  Dali::Vector<Observer*> mObservers;

  ActiveConstraintContainer* mConstraints;               ///< Container of owned active-constraints.
  ActiveConstraintContainer* mRemovedConstraints;        ///< Container of owned active-constraints, which are being removed.

  typedef std::vector< Dali::PropertyNotification >     PropertyNotificationContainer;
  typedef PropertyNotificationContainer::iterator       PropertyNotificationContainerIter;
  typedef PropertyNotificationContainer::const_iterator PropertyNotificationContainerConstIter;
  PropertyNotificationContainer* mPropertyNotifications; ///< Container of owned property notifications.
};

} // namespace Internal

inline Internal::ProxyObject& GetImplementation(Dali::Constrainable& object)
{
  DALI_ASSERT_ALWAYS( object && "ProxyObject handle is empty" );
  BaseObject& handle = object.GetBaseObject();
  return static_cast<Internal::ProxyObject&>(handle);
}

inline const Internal::ProxyObject& GetImplementation(const Dali::Constrainable& object)
{
  DALI_ASSERT_ALWAYS( object && "ProxyObject handle is empty" );
  const BaseObject& handle = object.GetBaseObject();
  return static_cast<const Internal::ProxyObject&>(handle);
}


} // namespace Dali

#endif // __DALI_INTERNAL_PROXY_OBJECT_H__
