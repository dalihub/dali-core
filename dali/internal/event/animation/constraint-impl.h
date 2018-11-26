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
 * Connects a constraint which takes another property as an input.
 */
template < typename PropertyType >
class Constraint : public ConstraintBase
{
public:

  using ConstraintFunctionPtr = typename PropertyConstraintPtr<PropertyType>::Type;

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
    return new Constraint( object, targetIndex, sources, func );
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
   * @copydoc ConstraintBase::DoClone()
   */
  ConstraintBase* DoClone( Object& object ) override final
  {
    ConstraintFunctionPtr funcPtr( mUserFunction->Clone() );
    return new Constraint( object,
                            mTargetPropertyIndex,
                            mSources,
                            funcPtr );
  }

  /**
   * Private constructor; see also Constraint::New().
   */
  Constraint( Object& object,
              Property::Index targetIndex,
              SourceContainer& sources,
              ConstraintFunctionPtr& func )
  : ConstraintBase( object, targetIndex, sources ),
    mUserFunction( func )
  {
  }

  // Undefined
  Constraint() = delete;
  Constraint( const Constraint& ) = delete;
  Constraint& operator=( const Constraint& rhs ) = delete;


  /**
   * @copydoc ConstraintBase::ConnectConstraint()
   */
  void ConnectConstraint() override final
  {
    // Should not come here if target object has been destroyed
    DALI_ASSERT_DEBUG( nullptr != mTargetObject );

    // Guard against double connections
    DALI_ASSERT_DEBUG( nullptr == mSceneGraphConstraint );

    SceneGraph::PropertyOwner& targetObject = const_cast< SceneGraph::PropertyOwner& >( mTargetObject->GetSceneObject() );

    // Build a container of property-owners, providing the scene-graph properties
    SceneGraph::PropertyOwnerContainer propertyOwners;
    propertyOwners.PushBack( &targetObject );

    // Build the constraint function; this requires a scene-graph property from each source
    ConstraintFunctionPtr func( ConnectConstraintFunction( propertyOwners ) );

    if ( func )
    {
      OwnerPointer<SceneGraph::PropertyResetterBase> resetter;
      // Create the SceneGraphConstraint and PropertyResetter, and connect them to the scene-graph
      const SceneGraph::PropertyBase* targetProperty = mTargetObject->GetSceneObjectAnimatableProperty( mTargetPropertyIndex );
      DALI_ASSERT_ALWAYS( targetProperty && "Constraint target property does not exist" );
      if( targetProperty->IsTransformManagerProperty() )  //It is a property managed by the transform manager
      {
        // Connect the constraint
        mSceneGraphConstraint = SceneGraph::Constraint< PropertyType, TransformManagerPropertyAccessor<PropertyType> >::New( *targetProperty,
                                                                                                                             propertyOwners,
                                                                                                                             func,
                                                                                                                             mRemoveAction );
        // Don't create a resetter for transform manager property, it's less efficient
      }
      else  //SceneGraph property
      {
        // Connect the constraint
        mSceneGraphConstraint = SceneGraph::Constraint< PropertyType, PropertyAccessor<PropertyType> >::New( *targetProperty,
                                                                                                             propertyOwners,
                                                                                                             func,
                                                                                                             mRemoveAction );
        resetter = SceneGraph::ConstraintResetter::New( targetObject, *targetProperty, *mSceneGraphConstraint );
      }
      OwnerPointer< SceneGraph::ConstraintBase > transferOwnership( const_cast< SceneGraph::ConstraintBase* >( mSceneGraphConstraint ) );
      ApplyConstraintMessage( GetEventThreadServices(), targetObject, transferOwnership );
      if( resetter )
      {
        AddResetterMessage( GetEventThreadServices().GetUpdateManager(), resetter );
      }
    }
  }

  /**
   * Helper for ConnectConstraint. Creates a connected constraint-function.
   * Also populates the property-owner container, for each input connected to the constraint-function.
   * @param[out] propertyOwners The container of property-owners providing the scene-graph properties.
   * @return A connected constraint-function, or nullptr if the scene-graph properties are not available.
   */
  PropertyConstraint<PropertyType>* ConnectConstraintFunction( SceneGraph::PropertyOwnerContainer& propertyOwners )
  {
    PropertyConstraint<PropertyType>* func = mUserFunction->Clone();

    for ( auto&& source : mSources )
    {
      int32_t componentIndex = Property::INVALID_COMPONENT_INDEX;
      PropertyInputImpl* inputProperty = AddInputProperty( source, propertyOwners, componentIndex );

      if ( nullptr == inputProperty )
      {
        delete func;
        func = nullptr;

        // Exit if a scene-graph object is not available from one of the sources
        break;
      }

      func->AddInput( inputProperty, componentIndex );
    }

    return func;
  }

protected:

  ConstraintFunctionPtr mUserFunction;

};

/**
 * Variant which allows float components to be constrained individually.
 */
template <>
class Constraint<float> : public ConstraintBase
{
public:

  using ConstraintFunctionPtr = typename PropertyConstraintPtr<float>::Type;

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
    return new Constraint( object, targetIndex, sources, func );
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
   * @copydoc ConstraintBase::DoClone()
   */
  virtual ConstraintBase* DoClone( Object& object ) override final
  {
    ConstraintFunctionPtr funcPtr( mUserFunction->Clone() );
    return new Constraint( object,
                           mTargetPropertyIndex,
                           mSources,
                           funcPtr );
  }

  /**
   * Private constructor; see also Constraint::New().
   */
  Constraint( Object& object,
              Property::Index targetIndex,
              SourceContainer& sources,
              ConstraintFunctionPtr& func )
  : ConstraintBase( object, targetIndex, sources ),
    mUserFunction( func )
  {
  }

  // Undefined
  Constraint() = delete;
  Constraint( const Constraint& ) = delete;
  Constraint& operator=( const Constraint& rhs ) = delete;

  /**
   * @copydoc ConstraintBase::ConnectConstraint()
   */
  void ConnectConstraint() override final
  {
    // Should not come here if target object has been destroyed
    DALI_ASSERT_DEBUG( nullptr != mTargetObject );
    // Guard against double connections
    DALI_ASSERT_DEBUG( nullptr == mSceneGraphConstraint );

    SceneGraph::PropertyOwner& targetObject = const_cast< SceneGraph::PropertyOwner& >( mTargetObject->GetSceneObject() );

    // Build a container of property-owners, providing the scene-graph properties
    SceneGraph::PropertyOwnerContainer propertyOwners;
    propertyOwners.PushBack( &targetObject );

    // Build the constraint function; this requires a scene-graph property from each source
    ConstraintFunctionPtr func( ConnectConstraintFunction( propertyOwners ) );

    if ( func )
    {
      // Create the SceneGraphConstraint, and connect to the scene-graph
      bool resetterRequired = false;
      const SceneGraph::PropertyBase* targetProperty = mTargetObject->GetSceneObjectAnimatableProperty( mTargetPropertyIndex );
      // The targetProperty should exist, when targetObject exists
      DALI_ASSERT_ALWAYS( nullptr != targetProperty && "Constraint target property does not exist" );
      const int32_t componentIndex = mTargetObject->GetPropertyComponentIndex( mTargetPropertyIndex );
      if ( Property::INVALID_COMPONENT_INDEX == componentIndex )
      {
        // Not a Vector2, Vector3 or Vector4 component, expecting float type
        DALI_ASSERT_DEBUG( PropertyTypes::Get< float >() == targetProperty->GetType() );

        mSceneGraphConstraint = SceneGraph::Constraint< float, PropertyAccessor<float> >::New( *targetProperty, propertyOwners, func, mRemoveAction );
        resetterRequired = true;
      }
      else
      {
        // Expecting Vector2, Vector3 or Vector4 type
        if ( PropertyTypes::Get< Vector2 >() == targetProperty->GetType() )
        {
          // Constrain float component of Vector2 property
          if ( 0 == componentIndex )
          {
            mSceneGraphConstraint = SceneGraph::Constraint< float, PropertyComponentAccessorX<Vector2> >::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }
          else if ( 1 == componentIndex )
          {
            mSceneGraphConstraint = SceneGraph::Constraint< float, PropertyComponentAccessorY<Vector2> >::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }
          resetterRequired = (mSceneGraphConstraint != nullptr);
        }
        else if ( PropertyTypes::Get< Vector3 >() == targetProperty->GetType() )
        {
          // Constrain float component of Vector3 property
          if( targetProperty->IsTransformManagerProperty() )
          {
            if ( 0 == componentIndex )
            {
              mSceneGraphConstraint = SceneGraph::Constraint< float, TransformManagerPropertyComponentAccessor<Vector3,0> >::New( *targetProperty,
                                                                                                                                  propertyOwners,
                                                                                                                                  func,
                                                                                                                                  mRemoveAction );
            }
            else if ( 1 == componentIndex )
            {
              mSceneGraphConstraint = SceneGraph::Constraint< float, TransformManagerPropertyComponentAccessor<Vector3,1> >::New( *targetProperty,
                                                                                                                                  propertyOwners,
                                                                                                                                  func,
                                                                                                                                  mRemoveAction );
            }
            else if ( 2 == componentIndex )
            {
              mSceneGraphConstraint = SceneGraph::Constraint< float, TransformManagerPropertyComponentAccessor<Vector3,2> >::New( *targetProperty,
                                                                                                                                  propertyOwners,
                                                                                                                                  func,
                                                                                                                                  mRemoveAction );
            }
            // Do not create a resetter for transform manager property
          }
          else
          {
            if ( 0 == componentIndex )
            {
              mSceneGraphConstraint = SceneGraph::Constraint< float, PropertyComponentAccessorX<Vector3> >::New( *targetProperty, propertyOwners, func, mRemoveAction );
            }
            else if ( 1 == componentIndex )
            {
              mSceneGraphConstraint = SceneGraph::Constraint< float, PropertyComponentAccessorY<Vector3> >::New( *targetProperty, propertyOwners, func, mRemoveAction );
            }
            else if ( 2 == componentIndex )
            {
              mSceneGraphConstraint = SceneGraph::Constraint< float, PropertyComponentAccessorZ<Vector3> >::New( *targetProperty, propertyOwners, func, mRemoveAction );
            }
            resetterRequired = (mSceneGraphConstraint != nullptr);
          }
        }
        else if ( PropertyTypes::Get< Vector4 >() == targetProperty->GetType() )
        {
          // Constrain float component of Vector4 property
          if ( 0 == componentIndex )
          {
            mSceneGraphConstraint = SceneGraph::Constraint< float, PropertyComponentAccessorX<Vector4> >::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }
          else if ( 1 == componentIndex )
          {
            mSceneGraphConstraint = SceneGraph::Constraint< float, PropertyComponentAccessorY<Vector4> >::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }
          else if ( 2 == componentIndex )
          {
            mSceneGraphConstraint = SceneGraph::Constraint< float, PropertyComponentAccessorZ<Vector4> >::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }
          else if ( 3 == componentIndex )
          {
            mSceneGraphConstraint = SceneGraph::Constraint< float, PropertyComponentAccessorW<Vector4> >::New( *targetProperty, propertyOwners, func, mRemoveAction );
          }

          resetterRequired = (mSceneGraphConstraint != nullptr);
        }
      }
      if( mSceneGraphConstraint )
      {
        OwnerPointer< SceneGraph::ConstraintBase > transferOwnership( const_cast< SceneGraph::ConstraintBase* >( mSceneGraphConstraint ) );
        ApplyConstraintMessage( GetEventThreadServices(), targetObject, transferOwnership );
        if( resetterRequired )
        {
          OwnerPointer<SceneGraph::PropertyResetterBase> resetter = SceneGraph::ConstraintResetter::New( targetObject, *targetProperty, *mSceneGraphConstraint );
          AddResetterMessage( GetEventThreadServices().GetUpdateManager(), resetter );
        }
      }
    }
  }

  /**
   * Helper for ConnectConstraint. Creates a connected constraint-function.
   * Also populates the property-owner container, for each input connected to the constraint-function.
   * @param[out] propertyOwners The container of property-owners providing the scene-graph properties.
   * @return A connected constraint-function, or nullptr if the scene-graph properties are not available.
   */
  PropertyConstraint<float>* ConnectConstraintFunction( SceneGraph::PropertyOwnerContainer& propertyOwners )
  {
    PropertyConstraint<float>* func = mUserFunction->Clone();

    for ( auto&& source : mSources )
    {
      int32_t componentIndex = Property::INVALID_COMPONENT_INDEX;
      PropertyInputImpl* inputProperty = AddInputProperty( source, propertyOwners, componentIndex );

      if ( nullptr == inputProperty )
      {
        delete func;
        func = nullptr;

        // Exit if a scene-graph object is not available from one of the sources
        break;
      }

      func->AddInput( inputProperty, componentIndex );
    }

    return func;
  }

protected:

  ConstraintFunctionPtr mUserFunction;

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACTIVE_CONSTRAINT_H__
