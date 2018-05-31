#ifndef __DALI_INTERNAL_ACTIVE_CONSTRAINT_H__
#define __DALI_INTERNAL_ACTIVE_CONSTRAINT_H__

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
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/animation/constraint-base.h>
#include <dali/internal/event/animation/constraint-source-impl.h>
#include <dali/internal/event/animation/property-constraint-ptr.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/animation/scene-graph-constraint.h>
#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/property-component-accessor.h>
#include <memory>

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
class Constraint : public ConstraintBase
{
public:

  typedef SceneGraph::Constraint< PropertyType, PropertyAccessor<PropertyType> > SceneGraphConstraint;
  typedef const SceneGraph::AnimatableProperty<PropertyType>* ScenePropertyPtr;
  typedef typename PropertyConstraintPtr<PropertyType>::Type ConstraintFunctionPtr;
  typedef const SceneGraph::TransformManagerPropertyHandler<PropertyType> TransformManagerProperty;

  /**
   * Construct a new constraint.
   * @param[in] object The property-owning object.
   * @param[in] targetIndex The index of the property to constrain.
   * @param[in] sources The sources of the input properties passed to func.
   * @param[in] func The constraint function.
   * @return A newly allocated active-constraint.
   */
  static ConstraintBase* New( Object& object,
                              Property::Index targetIndex,
                              SourceContainer& sources,
                              ConstraintFunctionPtr func )
  {
    return new Constraint< PropertyType >( object, targetIndex, sources, func );
  }

  /**
   * @copydoc ConstraintBase::Clone()
   */
  virtual ConstraintBase* Clone( Object& object )
  {
    DALI_ASSERT_ALWAYS( !mSourceDestroyed && "An input source object has been destroyed" );

    ConstraintBase* clone( NULL );

    ConstraintFunctionPtr funcPtr( mUserFunction->Clone() );

    clone = new Constraint< PropertyType >( object,
                                            mTargetIndex,
                                            mSources,
                                            funcPtr );

    clone->SetRemoveAction(mRemoveAction);
    clone->SetTag( mTag );

    return clone;
  }


  /**
   * Virtual destructor.
   */
  virtual ~Constraint()
  {
    // This is not responsible for removing constraints.
  }

private:

  /**
   * Private constructor; see also Constraint::New().
   */
  Constraint( Object& object,
              Property::Index targetIndex,
              SourceContainer& sources,
              ConstraintFunctionPtr& func )
  : ConstraintBase( object, targetIndex, sources ),
    mTargetIndex( targetIndex ),
    mUserFunction( func )
  {
  }

  // Undefined
  Constraint( const Constraint& );

  // Undefined
  Constraint& operator=( const Constraint& rhs );

  /**
   * Create and connect a constraint and property resetter for a scene-graph property
   */
  void ConnectConstraint()
  {
    // Should not come here if target object has been destroyed
    DALI_ASSERT_DEBUG( NULL != mTargetObject );

    // Guard against double connections
    DALI_ASSERT_DEBUG( NULL == mSceneGraphConstraint );

    // Short-circuit until the target scene-object exists
    SceneGraph::PropertyOwner* targetObject = const_cast< SceneGraph::PropertyOwner* >( mTargetObject->GetSceneObject() );
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
      // Create the SceneGraphConstraint and PropertyResetter, and connect them to the scene-graph

      const SceneGraph::PropertyBase* targetProperty = mTargetObject->GetSceneObjectAnimatableProperty( mTargetIndex );
      OwnerPointer<SceneGraph::PropertyResetterBase> resetter;

      // The targetProperty should exist, when targetObject exists
      DALI_ASSERT_ALWAYS( NULL != targetProperty && "Constraint target property does not exist" );

      if( targetProperty->IsTransformManagerProperty() )  //It is a property managed by the transform manager
      {
        // Connect the constraint
        mSceneGraphConstraint =
          SceneGraph::Constraint<PropertyType, TransformManagerPropertyAccessor<PropertyType> >::New( *targetProperty,
                                                                                                      propertyOwners,
                                                                                                      func,
                                                                                                      mRemoveAction );
        // Don't create a resetter for transform manager property, it's less efficient
      }
      else  //SceneGraph property
      {
        // Connect the constraint
        mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty,
                                                           propertyOwners,
                                                           func,
                                                           mRemoveAction );
        // Connect the resetter
        resetter = SceneGraph::ConstraintResetter::New( *targetObject, *targetProperty, *mSceneGraphConstraint );

      }
      OwnerPointer< SceneGraph::ConstraintBase > transferOwnership( const_cast< SceneGraph::ConstraintBase* >( mSceneGraphConstraint ) );
      ApplyConstraintMessage( GetEventThreadServices(), *targetObject, transferOwnership );

      if( resetter != nullptr )
      {
        AddResetterMessage( GetEventThreadServices().GetUpdateManager(), resetter );
      }
    }
  }

  /**
   * Helper for ConnectConstraint. Creates a connected constraint-function.
   * Also populates the property-owner container, for each input connected to the constraint-function.
   * @param[out] propertyOwners The container of property-owners providing the scene-graph properties.
   * @return A connected constraint-function, or NULL if the scene-graph properties are not available.
   */
  PropertyConstraint<PropertyType>* ConnectConstraintFunction( SceneGraph::PropertyOwnerContainer& propertyOwners )
  {
    PropertyConstraint<PropertyType>* func = mUserFunction->Clone();

    for ( SourceIter iter = mSources.begin(); mSources.end() != iter; ++iter )
    {
      Source& source = *iter;

      PropertyInputImpl* inputProperty( NULL );
      int componentIndex( Property::INVALID_COMPONENT_INDEX );

      if ( OBJECT_PROPERTY == source.sourceType )
      {
        DALI_ASSERT_ALWAYS( source.object->IsPropertyAConstraintInput( source.propertyIndex ) );

        SceneGraph::PropertyOwner* owner = const_cast< SceneGraph::PropertyOwner* >( source.object->GetSceneObject() );

        // The property owner will not exist, if the target object is off-stage
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
        DALI_ASSERT_ALWAYS( mTargetObject->IsPropertyAConstraintInput( source.propertyIndex ) );

        inputProperty = const_cast< PropertyInputImpl* >( mTargetObject->GetSceneObjectInputProperty( source.propertyIndex ) );
        componentIndex = mTargetObject->GetPropertyComponentIndex( source.propertyIndex );

        // The target scene-object should provide this property
        DALI_ASSERT_ALWAYS( NULL != inputProperty && "Constraint source property does not exist" );
      }
      else
      {
        DALI_ASSERT_ALWAYS( PARENT_PROPERTY == source.sourceType && "Constraint source property type is invalid" );

        Object* objectParent = dynamic_cast< Actor& >( *mTargetObject ).GetParent();

        // This will not exist, if the target object is off-stage
        if ( NULL != objectParent )
        {
          DALI_ASSERT_ALWAYS( objectParent->IsPropertyAConstraintInput( source.propertyIndex ) );

          SceneGraph::PropertyOwner* owner = const_cast< SceneGraph::PropertyOwner* >( objectParent->GetSceneObject() );

          // The property owner will not exist, if the parent object is off-stage
          if ( NULL != owner )
          {
            AddUnique( propertyOwners, owner );
            inputProperty = const_cast< PropertyInputImpl* >( objectParent->GetSceneObjectInputProperty( source.propertyIndex ) );
            componentIndex = objectParent->GetPropertyComponentIndex( source.propertyIndex );

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

      func->SetInput( ( iter - mSources.begin() ), componentIndex, *inputProperty );
    }

    return func;
  }

protected:

  Property::Index mTargetIndex;

  ConstraintFunctionPtr mUserFunction;
};

/**
 * Variant which allows float components to be animated individually.
 */
template <>
class Constraint<float> : public ConstraintBase
{
public:

  typedef typename PropertyConstraintPtr<float>::Type ConstraintFunctionPtr;

  /**
   * Construct a new constraint.
   * @param[in] object The property-owning object.
   * @param[in] targetIndex The index of the property to constrain.
   * @param[in] sources The sources of the input properties passed to func.
   * @param[in] func The constraint function.
   * @return A newly allocated constraint.
   */
  static ConstraintBase* New( Object& object,
                              Property::Index targetIndex,
                              SourceContainer& sources,
                              ConstraintFunctionPtr func )
  {
    return new Constraint< float >( object, targetIndex, sources, func );
  }

  /**
   * @copydoc ConstraintBase::Clone()
   */
  virtual ConstraintBase* Clone( Object& object )
  {
    DALI_ASSERT_ALWAYS( !mSourceDestroyed && "An input source object has been destroyed" );

    ConstraintBase* clone( NULL );

    ConstraintFunctionPtr funcPtr( mUserFunction->Clone() );

    clone = new Constraint< float >( object,
                                     mTargetIndex,
                                     mSources,
                                     funcPtr );

    clone->SetRemoveAction(mRemoveAction);
    clone->SetTag( mTag );

    return clone;
  }

  /**
   * Virtual destructor.
   */
  virtual ~Constraint()
  {
    // This is not responsible for removing constraints.
  }

private:

  /**
   * Private constructor; see also Constraint::New().
   */
  Constraint( Object& object,
              Property::Index targetIndex,
              SourceContainer& sources,
              ConstraintFunctionPtr& func )
  : ConstraintBase( object, targetIndex, sources ),
    mTargetIndex( targetIndex ),
    mUserFunction( func )
  {
  }

  // Undefined
  Constraint( const Constraint& );

  // Undefined
  Constraint& operator=( const Constraint& rhs );

  /**
   * Create and connect a constraint for a scene-object.
   */
  void ConnectConstraint()
  {
    // Should not come here if target-object has been destroyed
    DALI_ASSERT_DEBUG( NULL != mTargetObject );

    // Guard against double connections
    DALI_ASSERT_DEBUG( NULL == mSceneGraphConstraint );

    // Short-circuit until the target scene-object exists
    SceneGraph::PropertyOwner* targetObject = const_cast< SceneGraph::PropertyOwner* >( mTargetObject->GetSceneObject() );
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

      const SceneGraph::PropertyBase* targetProperty = mTargetObject->GetSceneObjectAnimatableProperty( mTargetIndex );
      OwnerPointer<SceneGraph::PropertyResetterBase> resetter;

      // The targetProperty should exist, when targetObject exists
      DALI_ASSERT_ALWAYS( NULL != targetProperty && "Constraint target property does not exist" );

      const int componentIndex = mTargetObject->GetPropertyComponentIndex( mTargetIndex );

      if ( Property::INVALID_COMPONENT_INDEX == componentIndex )
      {
        // Not a Vector2, Vector3 or Vector4 component, expecting float type
        DALI_ASSERT_DEBUG( PropertyTypes::Get< float >() == targetProperty->GetType() );

        typedef SceneGraph::Constraint< float, PropertyAccessor<float> > SceneGraphConstraint;

        mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
        resetter = SceneGraph::ConstraintResetter::New( *targetObject, *targetProperty, *mSceneGraphConstraint );
      }
      else
      {
        // Expecting Vector2, Vector3 or Vector4 type

        if ( PropertyTypes::Get< Vector2 >() == targetProperty->GetType() )
        {
          // Constrain float component of Vector2 property

          if ( 0 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorX<Vector2> > SceneGraphConstraint;
            mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }
          else if ( 1 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorY<Vector2> > SceneGraphConstraint;
            mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }
          if( mSceneGraphConstraint )
          {
            resetter = SceneGraph::ConstraintResetter::New( *targetObject, *targetProperty, *mSceneGraphConstraint );
          }
        }
        else if ( PropertyTypes::Get< Vector3 >() == targetProperty->GetType() )
        {
          // Constrain float component of Vector3 property
          if( targetProperty->IsTransformManagerProperty() )
          {
            if ( 0 == componentIndex )
            {
              typedef SceneGraph::Constraint< float, TransformManagerPropertyComponentAccessor<Vector3,0> > SceneGraphConstraint;
              mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
            }
            else if ( 1 == componentIndex )
            {
              typedef SceneGraph::Constraint< float, TransformManagerPropertyComponentAccessor<Vector3,1> > SceneGraphConstraint;
              mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
            }
            else if ( 2 == componentIndex )
            {
              typedef SceneGraph::Constraint< float, TransformManagerPropertyComponentAccessor<Vector3,2> > SceneGraphConstraint;
              mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
            }
            // Do not create a resetter for transform manager property
          }
          else
          {
            if ( 0 == componentIndex )
            {
              typedef SceneGraph::Constraint< float, PropertyComponentAccessorX<Vector3> > SceneGraphConstraint;
              mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
            }
            else if ( 1 == componentIndex )
            {
              typedef SceneGraph::Constraint< float, PropertyComponentAccessorY<Vector3> > SceneGraphConstraint;
              mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
            }
            else if ( 2 == componentIndex )
            {
              typedef SceneGraph::Constraint< float, PropertyComponentAccessorZ<Vector3> > SceneGraphConstraint;
              mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
            }
            if( mSceneGraphConstraint )
            {
              resetter = SceneGraph::ConstraintResetter::New( *targetObject, *targetProperty, *mSceneGraphConstraint );
            }
          }
        }
        else if ( PropertyTypes::Get< Vector4 >() == targetProperty->GetType() )
        {
          // Constrain float component of Vector4 property

          if ( 0 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorX<Vector4> > SceneGraphConstraint;
            mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }
          else if ( 1 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorY<Vector4> > SceneGraphConstraint;
            mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }
          else if ( 2 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorZ<Vector4> > SceneGraphConstraint;
            mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }
          else if ( 3 == componentIndex )
          {
            typedef SceneGraph::Constraint< float, PropertyComponentAccessorW<Vector4> > SceneGraphConstraint;
            mSceneGraphConstraint = SceneGraphConstraint::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }

          if( mSceneGraphConstraint )
          {
            resetter = SceneGraph::ConstraintResetter::New( *targetObject, *targetProperty, *mSceneGraphConstraint );
          }
        }
      }

      if( mSceneGraphConstraint )
      {
        OwnerPointer< SceneGraph::ConstraintBase > transferOwnership( const_cast< SceneGraph::ConstraintBase* >( mSceneGraphConstraint ) );
        ApplyConstraintMessage( GetEventThreadServices(), *targetObject, transferOwnership );
      }
      if( resetter != nullptr )
      {
        AddResetterMessage( GetEventThreadServices().GetUpdateManager(), resetter );
      }
    }
  }

  /**
   * Helper for ConnectConstraint. Creates a connected constraint-function.
   * Also populates the property-owner container, for each input connected to the constraint-function.
   * @param[out] propertyOwners The container of property-owners providing the scene-graph properties.
   * @return A connected constraint-function, or NULL if the scene-graph properties are not available.
   */
  PropertyConstraint<float>* ConnectConstraintFunction( SceneGraph::PropertyOwnerContainer& propertyOwners )
  {
    PropertyConstraint<float>* func = mUserFunction->Clone();

    for ( SourceIter iter = mSources.begin(); mSources.end() != iter; ++iter )
    {
      Source& source = *iter;

      PropertyInputImpl* inputProperty( NULL );
      int componentIndex( Property::INVALID_COMPONENT_INDEX );

      if ( OBJECT_PROPERTY == source.sourceType )
      {
        DALI_ASSERT_ALWAYS( source.object->IsPropertyAConstraintInput( source.propertyIndex ) );

        SceneGraph::PropertyOwner* owner = const_cast< SceneGraph::PropertyOwner* >( source.object->GetSceneObject() );

        // The property owner will not exist, if the target object is off-stage
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
        DALI_ASSERT_ALWAYS( mTargetObject->IsPropertyAConstraintInput( source.propertyIndex ) );

        inputProperty = const_cast< PropertyInputImpl* >( mTargetObject->GetSceneObjectInputProperty( source.propertyIndex ) );
        componentIndex = mTargetObject->GetPropertyComponentIndex( source.propertyIndex );

        // The target scene-object should provide this property
        DALI_ASSERT_ALWAYS( NULL != inputProperty && "Constraint source property does not exist" );
      }
      else
      {
        DALI_ASSERT_ALWAYS( PARENT_PROPERTY == source.sourceType && "Constraint source property type is invalid" );

        Object* objectParent = dynamic_cast< Actor& >( *mTargetObject ).GetParent();

        // This will not exist, if the target object is off-stage
        if ( NULL != objectParent )
        {
          DALI_ASSERT_ALWAYS( objectParent->IsPropertyAConstraintInput( source.propertyIndex ) );

          SceneGraph::PropertyOwner* owner = const_cast< SceneGraph::PropertyOwner* >( objectParent->GetSceneObject() );

          // The property owner will not exist, if the parent object is off-stage
          if ( NULL != owner )
          {
            AddUnique( propertyOwners, owner );
            inputProperty = const_cast< PropertyInputImpl* >( objectParent->GetSceneObjectInputProperty( source.propertyIndex ) );
            componentIndex = objectParent->GetPropertyComponentIndex( source.propertyIndex );

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

      func->SetInput( ( iter - mSources.begin() ), componentIndex, *inputProperty );
    }

    return func;
  }

protected:

  Property::Index mTargetIndex;

  ConstraintFunctionPtr mUserFunction;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACTIVE_CONSTRAINT_H__
