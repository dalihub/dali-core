#ifndef __DALI_INTERNAL_ACTIVE_CONSTRAINT_H__
#define __DALI_INTERNAL_ACTIVE_CONSTRAINT_H__

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

/**
 * Helper to add only unique entries to the propertyOwner container
 * @param propertyOwners to add the entries to
 * @param object to add
 */
inline void AddUnique( SceneGraph::PropertyOwnerContainer& propertyOwners, SceneGraph::PropertyOwner* object )
{
  const SceneGraph::PropertyOwnerIter iter = std::find( propertyOwners.Begin(), propertyOwners.End(), object );
  if( iter == propertyOwners.End() )
  {
    // each owner should only be added once
    propertyOwners.PushBack( object );
  }
}

/**
 * Connects a constraint which takes another property as an input.
 */
template < typename PropertyType >
class ActiveConstraint : public ActiveConstraintBase
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

    clone->SetAlphaFunction(mAlphaFunction);
    clone->SetRemoveAction(mRemoveAction);
    clone->SetTag( mTag );

    return clone;
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
  : ActiveConstraintBase( eventToUpdate, targetIndex, sources, sourceCount ),
    mTargetIndex( targetIndex ),
    mUserFunction( func ),
    mInterpolatorFunction( interpolator )
  {
  }

  // Undefined
  ActiveConstraint( const ActiveConstraint& );

  // Undefined
  ActiveConstraint& operator=( const ActiveConstraint& rhs );

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

    // Build a container of property-owners, providing the scene-graph properties
    SceneGraph::PropertyOwnerContainer propertyOwners;
    propertyOwners.PushBack( targetObject );

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
                                                                                     mInterpolatorFunction,
                                                                                     mCustomWeight );
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
   * Also populates the property-owner container, for each input connected to the constraint-function.
   * @param[out] propertyOwners The container of property-owners providing the scene-graph properties.
   * @return A connected constraint-function, or NULL if the scene-graph properties are not available.
   */
  PropertyConstraintBase<PropertyType>* ConnectConstraintFunction( SceneGraph::PropertyOwnerContainer& propertyOwners )
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
          AddUnique( propertyOwners, owner );
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
            AddUnique( propertyOwners, owner );
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

  ConstraintFunctionPtr mUserFunction;
  InterpolatorFunction mInterpolatorFunction;
};

/**
 * Variant which allows float components to be animated individually.
 */
template <>
class ActiveConstraint<float> : public ActiveConstraintBase
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

    clone->SetAlphaFunction(mAlphaFunction);
    clone->SetRemoveAction(mRemoveAction);
    clone->SetTag( mTag );

    return clone;
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
  : ActiveConstraintBase( eventToUpdate, targetIndex, sources, sourceCount ),
    mTargetIndex( targetIndex ),
    mUserFunction( func ),
    mInterpolatorFunction( interpolator )
  {
  }

  // Undefined
  ActiveConstraint( const ActiveConstraint& );

  // Undefined
  ActiveConstraint& operator=( const ActiveConstraint& rhs );

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

    // Build a container of property-owners, providing the scene-graph properties
    SceneGraph::PropertyOwnerContainer propertyOwners;
    propertyOwners.PushBack( targetObject );

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
                                                           mInterpolatorFunction,
                                                           mCustomWeight );
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
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction, mCustomWeight );
          }
          else if ( 1 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorY<Vector3> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction, mCustomWeight );
          }
          else if ( 2 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorZ<Vector3> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction, mCustomWeight );
          }
        }
        else if ( PropertyTypes::Get< Vector4 >() == targetProperty->GetType() )
        {
          // Constrain float component of Vector4 property

          if ( 0 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorX<Vector4> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction, mCustomWeight );
          }
          else if ( 1 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorY<Vector4> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction, mCustomWeight );
          }
          else if ( 2 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorZ<Vector4> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction, mCustomWeight );
          }
          else if ( 3 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorW<Vector4> > SceneGraphConstraint;
            sceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mInterpolatorFunction, mCustomWeight );
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
   * Also populates the property-owner container, for each input connected to the constraint-function.
   * @param[out] propertyOwners The container of property-owners providing the scene-graph properties.
   * @return A connected constraint-function, or NULL if the scene-graph properties are not available.
   */
  PropertyConstraintBase<float>* ConnectConstraintFunction( SceneGraph::PropertyOwnerContainer& propertyOwners )
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
          AddUnique( propertyOwners, owner );
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
            AddUnique( propertyOwners, owner );
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

  ConstraintFunctionPtr mUserFunction;
  InterpolatorFunction mInterpolatorFunction;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACTIVE_CONSTRAINT_H__
