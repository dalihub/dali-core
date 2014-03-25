#ifndef __DALI_INTERNAL_ACTIVE_CONSTRAINT_H__
#define __DALI_INTERNAL_ACTIVE_CONSTRAINT_H__

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
#include <set>
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/proxy-object.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/animation/active-constraint-base.h>
#include <dali/internal/event/animation/constraint-source-impl.h>
#include <dali/internal/event/animation/property-constraint-ptr.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/internal/update/animation/scene-graph-constraint.h>
#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/property-component-accessor.h>

namespace Dali
{

namespace Internal
{

typedef std::set<ProxyObject*>          ProxyObjectContainer;
typedef ProxyObjectContainer::iterator  ProxyObjectIter;

/**
 * Connects a constraint which takes another property as an input.
 */
template < typename PropertyType >
class ActiveConstraint : public ActiveConstraintBase, public ProxyObject::Observer
{
public:

  typedef SceneGraph::Constraint< PropertyType, PropertyAccessor<PropertyType> > SceneGraphConstraint;
  typedef const SceneGraph::AnimatableProperty<PropertyType>* ScenePropertyPtr;
  typedef typename PropertyConstraintPtr<PropertyType>::Type ConstraintFunctionPtr;
  typedef boost::function< PropertyType (const PropertyType&, const PropertyType&, float) > InterpolatorFunction;

  /**
   * Construct a new active-constraint.
   * @param[in] targetIndex The index of the property to constrain.
   * @param[in] sources The sources of the input properties passed to func.
   * @param[in] func The constraint function.
   * @param[in] interpolator The interpolator function.
   * @return A newly allocated active-constraint.
   */
  static ActiveConstraintBase* New( Property::Index targetIndex,
                                    SourceContainer& sources,
                                    ConstraintFunctionPtr func,
                                    InterpolatorFunction interpolator )
  {
    ThreadLocalStorage& tls = ThreadLocalStorage::Get();

    return new ActiveConstraint< PropertyType >( tls.GetEventToUpdate(), targetIndex, sources, sources.size(), func, interpolator );
  }

  /**
   * Virtual destructor.
   */
  virtual ~ActiveConstraint()
  {
    StopObservation();

    // This is not responsible for removing constraints.
  }

  /**
   * @copydoc ActiveConstraintBase::Clone()
   */
  virtual ActiveConstraintBase* Clone()
  {
    ActiveConstraintBase* clone( NULL );

    ConstraintFunctionPtr funcPtr( mUserFunction->Clone() );

    clone = new ActiveConstraint< PropertyType >( mEventToUpdate,
                                                  mTargetIndex,
                                                  mSources,
                                                  mSourceCount,
                                                  funcPtr,
                                                  mInterpolatorFunction );

    clone->SetRemoveTime(mRemoveTime);
    clone->SetAlphaFunction(mAlphaFunction);
    clone->SetRemoveAction(mRemoveAction);

    return clone;
  }

  /**
   * @copydoc ActiveConstraintBase::OnFirstApply()
   */
  virtual void OnFirstApply( ProxyObject& parent )
  {
    DALI_ASSERT_ALWAYS( NULL == mTargetProxy && "Parent of ActiveConstraint already set" );

    // No need to do anything, if the source objects are gone
    if( mSources.size() == mSourceCount )
    {
      mTargetProxy = &parent;

      ObserveProxy( parent );

      ConnectConstraint();
    }
  }

  /**
   * @copydoc ActiveConstraintBase::OnBeginRemove()
   */
  virtual void OnBeginRemove()
  {
    // Stop observing the remaining proxies
    StopObservation();

    // Discard all proxy pointers
    mSources.clear();
  }

  /**
   * @copydoc ProxyObject::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded( ProxyObject& proxy )
  {
    // Should not be getting callbacks when mSources has been cleared
    DALI_ASSERT_DEBUG( mSources.size() == mSourceCount );

    if ( mTargetProxy )
    {
      ConnectConstraint();
    }
  }

  /**
   * @copydoc ProxyObject::Observer::SceneObjectRemoved()
   */
  virtual void SceneObjectRemoved( ProxyObject& proxy )
  {
    // Notify base class that the scene-graph constraint is being removed
    OnSceneObjectRemove();

    if ( mSceneGraphConstraint )
    {
      // Preserve the previous weight
      mOffstageWeight = mSceneGraphConstraint->GetWeight( mEventToUpdate.GetEventBufferIndex() );

      // This is not responsible for removing constraints.
      mSceneGraphConstraint = NULL;
    }
  }

  /**
   * @copydoc ProxyObject::Observer::ProxyDestroyed()
   */
  virtual void ProxyDestroyed( ProxyObject& proxy )
  {
    // Remove proxy pointer from observation set
    ProxyObjectIter iter = mObservedProxies.find( &proxy );
    DALI_ASSERT_DEBUG( mObservedProxies.end() != iter );
    mObservedProxies.erase( iter );

    // Stop observing the remaining proxies
    StopObservation();

    // Discard all proxy pointers
    mTargetProxy = NULL;
    mSources.clear();
  }

private:

  /**
   * Private constructor; see also ActiveConstraint::New().
   */
  ActiveConstraint( EventToUpdate& eventToUpdate,
                    Property::Index targetIndex,
                    SourceContainer& sources,
                    unsigned int sourceCount,
                    ConstraintFunctionPtr& func,
                    InterpolatorFunction& interpolator )
  : ActiveConstraintBase( eventToUpdate, targetIndex ),
    mTargetIndex( targetIndex ),
    mSources( sources ),
    mSourceCount( sourceCount ),
    mUserFunction( func ),
    mInterpolatorFunction( interpolator )
  {
    // Skip init when any of the proxy objects have been destroyed
    if ( mSources.size() != mSourceCount )
    {
      // Discard all proxy pointers
      mTargetProxy = NULL;
      mSources.clear();
    }

    // Observe the objects providing properties
    for ( SourceIter iter = mSources.begin(); mSources.end() != iter; ++iter )
    {
      if ( OBJECT_PROPERTY == iter->sourceType )
      {
        DALI_ASSERT_ALWAYS( NULL != iter->object && "ActiveConstraint source object not found" );

        ObserveProxy( *(iter->object) );
      }
    }
  }

  // Undefined
  ActiveConstraint( const ActiveConstraint& );

  // Undefined
  ActiveConstraint& operator=( const ActiveConstraint& rhs );

  /**
   * Helper to observe a proxy, if not already observing it
   */
  void ObserveProxy( ProxyObject& proxy )
  {
    if ( mObservedProxies.end() == mObservedProxies.find(&proxy) )
    {
      proxy.AddObserver( *this );
      mObservedProxies.insert( &proxy );
    }
  }

  /**
   * Helper to stop observing proxies
   */
  void StopObservation()
  {
    for( ProxyObjectIter iter = mObservedProxies.begin(); mObservedProxies.end() != iter; ++iter )
    {
      (*iter)->RemoveObserver( *this );
    }

    mObservedProxies.clear();
  }

  /**
   * Create and connect a constraint for a scene-object.
   */
  void ConnectConstraint()
  {
    // Should not come here any proxies have been destroyed
    DALI_ASSERT_DEBUG( NULL != mTargetProxy );
    DALI_ASSERT_DEBUG( mSources.size() == mSourceCount );

    // Guard against double connections
    DALI_ASSERT_DEBUG( NULL == mSceneGraphConstraint );

    // Short-circuit until the target scene-object exists
    SceneGraph::PropertyOwner* targetObject = const_cast< SceneGraph::PropertyOwner* >( mTargetProxy->GetSceneObject() );
    if ( NULL == targetObject )
    {
      return;
    }

    // Build a set of property-owners, providing the scene-graph properties
    SceneGraph::PropertyOwnerSet propertyOwners;
    propertyOwners.insert( targetObject );

    // Build the constraint function; this requires a scene-graph property from each source
    ConstraintFunctionPtr func( ConnectConstraintFunction( propertyOwners ) );

    if ( func )
    {
      // Create the SceneGraphConstraint, and connect to the scene-graph

      const SceneGraph::PropertyBase* targetProperty = mTargetProxy->GetSceneObjectAnimatableProperty( mTargetIndex );

      // The targetProperty should exist, when targetObject exists
      DALI_ASSERT_ALWAYS( NULL != targetProperty && "Constraint target property does not exist" );

      // Connect the constraint
      SceneGraph::ConstraintBase* sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty,
                                                                                     propertyOwners,
                                                                                     func,
                                                                                     mInterpolatorFunction );
      DALI_ASSERT_DEBUG( NULL != sceneGraphConstraint );
      sceneGraphConstraint->SetInitialWeight( mOffstageWeight );
      sceneGraphConstraint->SetRemoveAction( mRemoveAction );

      // object is being used in a separate thread; queue a message to apply the constraint
      ApplyConstraintMessage( Stage::GetCurrent()->GetUpdateInterface(), *targetObject, *sceneGraphConstraint );

      // Keep a raw-pointer to the scene-graph constraint
      mSceneGraphConstraint = sceneGraphConstraint;

      // Notify ProxyObject base-class that the scene-graph constraint has been added
      OnSceneObjectAdd();
    }
  }

  /**
   * Helper for ConnectConstraint. Creates a connected constraint-function.
   * Also populates the property-owner set, for each input connected to the constraint-function.
   * @param[out] propertyOwners The set of property-owners providing the scene-graph properties.
   * @return A connected constraint-function, or NULL if the scene-graph properties are not available.
   */
  PropertyConstraintBase<PropertyType>* ConnectConstraintFunction( SceneGraph::PropertyOwnerSet& propertyOwners )
  {
    PropertyConstraintBase<PropertyType>* func = mUserFunction->Clone();
    bool usingComponentFunc( false );

    for ( SourceIter iter = mSources.begin(); mSources.end() != iter; ++iter )
    {
      Source& source = *iter;

      PropertyInputImpl* inputProperty( NULL );
      int componentIndex( Property::INVALID_COMPONENT_INDEX );

      if ( OBJECT_PROPERTY == source.sourceType )
      {
        DALI_ASSERT_ALWAYS( source.object->IsPropertyAConstraintInput( source.propertyIndex ) );

        SceneGraph::PropertyOwner* owner = const_cast< SceneGraph::PropertyOwner* >( source.object->GetSceneObject() );

        // The property owner will not exist, if the target proxy-object is off-stage
        if( NULL != owner )
        {
          propertyOwners.insert( owner );
          inputProperty = const_cast< PropertyInputImpl* >( source.object->GetSceneObjectInputProperty( source.propertyIndex ) );
          componentIndex = source.object->GetPropertyComponentIndex( source.propertyIndex );

          // The scene-object property should exist, when the property owner exists
          DALI_ASSERT_ALWAYS( NULL != inputProperty && "Constraint source property does not exist" );
        }
      }
      else if ( LOCAL_PROPERTY == source.sourceType )
      {
        DALI_ASSERT_ALWAYS( mTargetProxy->IsPropertyAConstraintInput( source.propertyIndex ) );

        inputProperty = const_cast< PropertyInputImpl* >( mTargetProxy->GetSceneObjectInputProperty( source.propertyIndex ) );
        componentIndex = mTargetProxy->GetPropertyComponentIndex( source.propertyIndex );

        // The target scene-object should provide this property
        DALI_ASSERT_ALWAYS( NULL != inputProperty && "Constraint source property does not exist" );
      }
      else
      {
        DALI_ASSERT_ALWAYS( PARENT_PROPERTY == source.sourceType && "Constraint source property type is invalid" );

        ProxyObject* proxyParent = dynamic_cast< Actor& >( *mTargetProxy ).GetParent();

        // This will not exist, if the target proxy-object is off-stage
        if ( NULL != proxyParent )
        {
          DALI_ASSERT_ALWAYS( proxyParent->IsPropertyAConstraintInput( source.propertyIndex ) );

          SceneGraph::PropertyOwner* owner = const_cast< SceneGraph::PropertyOwner* >( proxyParent->GetSceneObject() );

          // The property owner will not exist, if the parent proxy-object is off-stage
          if ( NULL != owner )
          {
            propertyOwners.insert( owner );
            inputProperty = const_cast< PropertyInputImpl* >( proxyParent->GetSceneObjectInputProperty( source.propertyIndex ) );
            componentIndex = proxyParent->GetPropertyComponentIndex( source.propertyIndex );

            // The scene-object property should exist, when the property owner exists
            DALI_ASSERT_ALWAYS( NULL != inputProperty && "Constraint source property does not exist" );
          }
        }
      }

      if ( NULL == inputProperty )
      {
        delete func;
        func = NULL;

        // Exit if a scene-graph object is not available from one of the sources
        break;
      }
      else if ( Property::INVALID_COMPONENT_INDEX != componentIndex )
      {
        // Special case where component indices are required
        if ( !usingComponentFunc )
        {
          PropertyConstraintBase<PropertyType>* componentFunc = func->CloneComponentFunc();
          usingComponentFunc = true;

          // Switch to function supporting component indices
          delete func;
          func = componentFunc;
        }
      }

      func->SetInput( ( iter - mSources.begin() ), componentIndex, *inputProperty );
    }

    return func;
  }

protected:

  Property::Index mTargetIndex;

  SourceContainer mSources;
  unsigned int mSourceCount;

  ProxyObjectContainer mObservedProxies; // We don't observe the same object twice

  ConstraintFunctionPtr mUserFunction;
  InterpolatorFunction mInterpolatorFunction;
};

/**
 * Variant which allows float components to be animated individually.
 */
template <>
class ActiveConstraint<float> : public ActiveConstraintBase, public ProxyObject::Observer
{
public:

  typedef typename PropertyConstraintPtr<float>::Type ConstraintFunctionPtr;
  typedef boost::function< float (const float&, const float&, float) > InterpolatorFunction;

  /**
   * Construct a new active-constraint.
   * @param[in] targetIndex The index of the property to constrain.
   * @param[in] sources The sources of the input properties passed to func.
   * @param[in] func The constraint function.
   * @param[in] interpolator The interpolator function.
   * @return A newly allocated active-constraint.
   */
  static ActiveConstraintBase* New( Property::Index targetIndex,
                                    SourceContainer& sources,
                                    ConstraintFunctionPtr func,
                                    InterpolatorFunction interpolator )
  {
    ThreadLocalStorage& tls = ThreadLocalStorage::Get();

    return new ActiveConstraint< float >( tls.GetEventToUpdate(), targetIndex, sources, sources.size(), func, interpolator );
  }

  /**
   * Virtual destructor.
   */
  virtual ~ActiveConstraint()
  {
    StopObservation();

    // This is not responsible for removing constraints.
  }

  /**
   * @copydoc ActiveConstraintBase::Clone()
   */
  virtual ActiveConstraintBase* Clone()
  {
    ActiveConstraintBase* clone( NULL );

    ConstraintFunctionPtr funcPtr( mUserFunction->Clone() );

    clone = new ActiveConstraint< float >( mEventToUpdate,
                                           mTargetIndex,
                                           mSources,
                                           mSourceCount,
                                           funcPtr,
                                           mInterpolatorFunction );

    clone->SetRemoveTime(mRemoveTime);
    clone->SetAlphaFunction(mAlphaFunction);
    clone->SetRemoveAction(mRemoveAction);

    return clone;
  }

  /**
   * @copydoc ActiveConstraintBase::OnFirstApply()
   */
  virtual void OnFirstApply( ProxyObject& parent )
  {
    DALI_ASSERT_ALWAYS( NULL == mTargetProxy && "Parent of ActiveConstraint already set" );

    // No need to do anything, if the source objects are gone
    if( mSources.size() == mSourceCount )
    {
      mTargetProxy = &parent;

      ObserveProxy( parent );

      ConnectConstraint();
    }
  }

  /**
   * @copydoc ActiveConstraintBase::OnBeginRemove()
   */
  virtual void OnBeginRemove()
  {
    // Stop observing the remaining proxies
    StopObservation();

    // Discard all proxy pointers
    mSources.clear();
  }

  /**
   * @copydoc ProxyObject::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded( ProxyObject& proxy )
  {
    // Should not be getting callbacks when mSources has been cleared
    DALI_ASSERT_DEBUG( mSources.size() == mSourceCount );

    if ( mTargetProxy )
    {
      ConnectConstraint();
    }
  }

  /**
   * @copydoc ProxyObject::Observer::SceneObjectRemoved()
   */
  virtual void SceneObjectRemoved( ProxyObject& proxy )
  {
    // Notify base class that the scene-graph constraint is being removed
    OnSceneObjectRemove();

    if ( mSceneGraphConstraint )
    {
      // Preserve the previous weight
      mOffstageWeight = mSceneGraphConstraint->GetWeight( mEventToUpdate.GetEventBufferIndex() );

      // This is not responsible for removing constraints.
      mSceneGraphConstraint = NULL;
    }
  }

  /**
   * @copydoc ProxyObject::Observer::ProxyDestroyed()
   */
  virtual void ProxyDestroyed( ProxyObject& proxy )
  {
    // Remove proxy pointer from observation set
    ProxyObjectIter iter = mObservedProxies.find( &proxy );
    DALI_ASSERT_DEBUG( mObservedProxies.end() != iter );
    mObservedProxies.erase( iter );

    // Stop observing the remaining proxies
    StopObservation();

    // Discard all proxy pointers
    mTargetProxy = NULL;
    mSources.clear();
  }

private:

  /**
   * Private constructor; see also ActiveConstraint::New().
   */
  ActiveConstraint( EventToUpdate& eventToUpdate,
                    Property::Index targetIndex,
                    SourceContainer& sources,
                    unsigned int sourceCount,
                    ConstraintFunctionPtr& func,
                    InterpolatorFunction& interpolator )
  : ActiveConstraintBase( eventToUpdate, targetIndex ),
    mTargetIndex( targetIndex ),
    mSources( sources ),
    mSourceCount( sourceCount ),
    mUserFunction( func ),
    mInterpolatorFunction( interpolator )
  {
    // Skip init when any of the proxy objects have been destroyed
    if ( mSources.size() != mSourceCount )
    {
      // Discard all proxy pointers
      mTargetProxy = NULL;
      mSources.clear();
    }

    // Observe the objects providing properties
    for ( SourceIter iter = mSources.begin(); mSources.end() != iter; ++iter )
    {
      if ( OBJECT_PROPERTY == iter->sourceType )
      {
        DALI_ASSERT_ALWAYS( NULL != iter->object && "ActiveConstraint source object not found" );

        ObserveProxy( *(iter->object) );
      }
    }
  }

  // Undefined
  ActiveConstraint( const ActiveConstraint& );

  // Undefined
  ActiveConstraint& operator=( const ActiveConstraint& rhs );

  /**
   * Helper to observe a proxy, if not already observing it
   */
  void ObserveProxy( ProxyObject& proxy )
  {
    if ( mObservedProxies.end() == mObservedProxies.find(&proxy) )
    {
      proxy.AddObserver( *this );
      mObservedProxies.insert( &proxy );
    }
  }

  /**
   * Helper to stop observing proxies
   */
  void StopObservation()
  {
    for( ProxyObjectIter iter = mObservedProxies.begin(); mObservedProxies.end() != iter; ++iter )
    {
      (*iter)->RemoveObserver( *this );
    }

    mObservedProxies.clear();
  }

  /**
   * Create and connect a constraint for a scene-object.
   */
  void ConnectConstraint()
  {
    // Should not come here any proxies have been destroyed
    DALI_ASSERT_DEBUG( NULL != mTargetProxy );
    DALI_ASSERT_DEBUG( mSources.size() == mSourceCount );

    // Guard against double connections
    DALI_ASSERT_DEBUG( NULL == mSceneGraphConstraint );

    // Short-circuit until the target scene-object exists
    SceneGraph::PropertyOwner* targetObject = const_cast< SceneGraph::PropertyOwner* >( mTargetProxy->GetSceneObject() );
    if ( NULL == targetObject )
    {
      return;
    }

    // Build a set of property-owners, providing the scene-graph properties
    SceneGraph::PropertyOwnerSet propertyOwners;
    propertyOwners.insert( targetObject );

    // Build the constraint function; this requires a scene-graph property from each source
    ConstraintFunctionPtr func( ConnectConstraintFunction( propertyOwners ) );

    if ( func )
    {
      // Create the SceneGraphConstraint, and connect to the scene-graph

      const SceneGraph::PropertyBase* targetProperty = mTargetProxy->GetSceneObjectAnimatableProperty( mTargetIndex );

      // The targetProperty should exist, when targetObject exists
      DALI_ASSERT_ALWAYS( NULL != targetProperty && "Constraint target property does not exist" );

      const int componentIndex = mTargetProxy->GetPropertyComponentIndex( mTargetIndex );

      SceneGraph::ConstraintBase* sceneGraphConstraint( NULL );

      if ( Property::INVALID_COMPONENT_INDEX == componentIndex )
      {
        // Not a Vector3 or Vector4 component, expecting float type
        DALI_ASSERT_DEBUG( PropertyTypes::Get< float >() == targetProperty->GetType() );

        typedef SceneGraph::Constraint< float, PropertyAccessor<float> > SceneGraphConstraint;

        sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty,
                                                           propertyOwners,
                                                           func,
                                                           mInterpolatorFunction );
      }
      else
      {
        // Expecting Vector3 or Vector4 type

        if ( PropertyTypes::Get< Vector3 >() == targetProperty->GetType() )
        {
          // Constrain float component of Vector3 property

          if ( 0 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorX<Vector3> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction );
          }
          else if ( 1 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorY<Vector3> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction );
          }
          else if ( 2 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorZ<Vector3> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction );
          }
        }
        else if ( PropertyTypes::Get< Vector4 >() == targetProperty->GetType() )
        {
          // Constrain float component of Vector4 property

          if ( 0 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorX<Vector4> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction );
          }
          else if ( 1 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorY<Vector4> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction );
          }
          else if ( 2 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorZ<Vector4> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction );
          }
          else if ( 3 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorW<Vector4> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction );
          }
        }
      }

      DALI_ASSERT_DEBUG( NULL != sceneGraphConstraint );
      sceneGraphConstraint->SetInitialWeight( mOffstageWeight );
      sceneGraphConstraint->SetRemoveAction( mRemoveAction );

        // object is being used in a separate thread; queue a message to apply the constraint
      ApplyConstraintMessage( Stage::GetCurrent()->GetUpdateInterface(), *targetObject, *sceneGraphConstraint );

      // Keep a raw-pointer to the scene-graph constraint
      mSceneGraphConstraint = sceneGraphConstraint;

      // Notify ProxyObject base-class that the scene-graph constraint has been added
      OnSceneObjectAdd();
    }
  }

  /**
   * Helper for ConnectConstraint. Creates a connected constraint-function.
   * Also populates the property-owner set, for each input connected to the constraint-function.
   * @param[out] propertyOwners The set of property-owners providing the scene-graph properties.
   * @return A connected constraint-function, or NULL if the scene-graph properties are not available.
   */
  PropertyConstraintBase<float>* ConnectConstraintFunction( SceneGraph::PropertyOwnerSet& propertyOwners )
  {
    PropertyConstraintBase<float>* func = mUserFunction->Clone();
    bool usingComponentFunc( false );

    for ( SourceIter iter = mSources.begin(); mSources.end() != iter; ++iter )
    {
      Source& source = *iter;

      PropertyInputImpl* inputProperty( NULL );
      int componentIndex( Property::INVALID_COMPONENT_INDEX );

      if ( OBJECT_PROPERTY == source.sourceType )
      {
        DALI_ASSERT_ALWAYS( source.object->IsPropertyAConstraintInput( source.propertyIndex ) );

        SceneGraph::PropertyOwner* owner = const_cast< SceneGraph::PropertyOwner* >( source.object->GetSceneObject() );

        // The property owner will not exist, if the target proxy-object is off-stage
        if( NULL != owner )
        {
          propertyOwners.insert( owner );
          inputProperty = const_cast< PropertyInputImpl* >( source.object->GetSceneObjectInputProperty( source.propertyIndex ) );
          componentIndex = source.object->GetPropertyComponentIndex( source.propertyIndex );

          // The scene-object property should exist, when the property owner exists
          DALI_ASSERT_ALWAYS( NULL != inputProperty && "Constraint source property does not exist" );
        }
      }
      else if ( LOCAL_PROPERTY == source.sourceType )
      {
        DALI_ASSERT_ALWAYS( mTargetProxy->IsPropertyAConstraintInput( source.propertyIndex ) );

        inputProperty = const_cast< PropertyInputImpl* >( mTargetProxy->GetSceneObjectInputProperty( source.propertyIndex ) );
        componentIndex = mTargetProxy->GetPropertyComponentIndex( source.propertyIndex );

        // The target scene-object should provide this property
        DALI_ASSERT_ALWAYS( NULL != inputProperty && "Constraint source property does not exist" );
      }
      else
      {
        DALI_ASSERT_ALWAYS( PARENT_PROPERTY == source.sourceType && "Constraint source property type is invalid" );

        ProxyObject* proxyParent = dynamic_cast< Actor& >( *mTargetProxy ).GetParent();

        // This will not exist, if the target proxy-object is off-stage
        if ( NULL != proxyParent )
        {
          DALI_ASSERT_ALWAYS( proxyParent->IsPropertyAConstraintInput( source.propertyIndex ) );

          SceneGraph::PropertyOwner* owner = const_cast< SceneGraph::PropertyOwner* >( proxyParent->GetSceneObject() );

          // The property owner will not exist, if the parent proxy-object is off-stage
          if ( NULL != owner )
          {
            propertyOwners.insert( owner );
            inputProperty = const_cast< PropertyInputImpl* >( proxyParent->GetSceneObjectInputProperty( source.propertyIndex ) );
            componentIndex = proxyParent->GetPropertyComponentIndex( source.propertyIndex );

            // The scene-object property should exist, when the property owner exists
            DALI_ASSERT_ALWAYS( NULL != inputProperty && "Constraint source property does not exist" );
          }
        }
      }

      if ( NULL == inputProperty )
      {
        delete func;
        func = NULL;

        // Exit if a scene-graph object is not available from one of the sources
        break;
      }
      else if ( Property::INVALID_COMPONENT_INDEX != componentIndex )
      {
        // Special case where component indices are required
        if ( !usingComponentFunc )
        {
          PropertyConstraintBase<float>* componentFunc = func->CloneComponentFunc();
          usingComponentFunc = true;

          // Switch to function supporting component indices
          delete func;
          func = componentFunc;
        }
      }

      func->SetInput( ( iter - mSources.begin() ), componentIndex, *inputProperty );
    }

    return func;
  }

protected:

  Property::Index mTargetIndex;

  SourceContainer mSources;
  unsigned int mSourceCount;

  ProxyObjectContainer mObservedProxies; // We don't observe the same object twice

  ConstraintFunctionPtr mUserFunction;
  InterpolatorFunction mInterpolatorFunction;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACTIVE_CONSTRAINT_H__
