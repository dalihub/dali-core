#ifndef __DALI_INTERNAL_ANIMATOR_CONNECTOR_H__
#define __DALI_INTERNAL_ANIMATOR_CONNECTOR_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/animation/animator-connector-base.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/property-component-accessor.h>
#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{

namespace Internal
{

/**
 * AnimatorConnector is used to connect SceneGraph::Animators for newly created scene-graph objects.
 *
 * The scene-graph objects are created by a Object e.g. Actor is a proxy for SceneGraph::Node.
 * AnimatorConnector observes the proxy object, in order to detect when a scene-graph object is created.
 *
 * SceneGraph::Animators weakly reference scene objects, and are automatically deleted when orphaned.
 * Therefore the AnimatorConnector is NOT responsible for disconnecting animators.
 */
template < typename PropertyType >
class AnimatorConnector : public AnimatorConnectorBase, public Object::Observer
{
public:

  typedef SceneGraph::Animator< PropertyType, PropertyAccessor<PropertyType> > AnimatorType;
  typedef SceneGraph::AnimatableProperty< PropertyType > PropertyInterfaceType;

  /**
   * Construct a new animator connector.
   * @param[in] object The object for a scene-graph object to animate.
   * @param[in] propertyIndex The index of a property provided by the object.
   * @param[in] componentIndex Index to a sub component of a property, for use with Vector2, Vector3 and Vector4 (INVALID_PROPERTY_COMPONENTINDEX to use the whole property)
   * @param[in] animatorFunction A function used to animate the property.
   * @param[in] alpha The alpha function to apply.
   * @param[in] period The time period of the animator.
   * @return A pointer to a newly allocated animator connector.
   */
  static AnimatorConnectorBase* New( Object& object,
                                     Property::Index propertyIndex,
                                     int componentIndex,
                                     AnimatorFunctionBase* animatorFunction,
                                     AlphaFunction alpha,
                                     const TimePeriod& period )
  {
    return new AnimatorConnector< PropertyType >( object,
                                                  propertyIndex,
                                                  componentIndex,
                                                  animatorFunction,
                                                  alpha,
                                                  period );
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatorConnector()
  {
    if( mObject )
    {
      mObject->RemoveObserver( *this );
    }

    //If there is not a SceneGraph::Animator, the AnimatorConnector is responsible for deleting the mAnimatorFunction
    //otherwise, the animator function ownership is transferred to the SceneGraph::Animator
    if( !mAnimator )
    {
      delete mAnimatorFunction;
      mAnimatorFunction = 0;
    }
  }

  /**
   * From AnimatorConnectorBase.
   * This is only expected to be called once, when added to an Animation.
   */
  void SetParent( Animation& parent )
  {
    DALI_ASSERT_ALWAYS( mParent == NULL && "AnimationConnector already has a parent" );
    mParent = &parent;

    if( mObject && mObject->GetSceneObject() )
    {
      CreateAnimator();
    }
  }

private:

  /**
   * Private constructor; see also AnimatorConnector::New().
   */
  AnimatorConnector( Object& object,
                     Property::Index propertyIndex,
                     int componentIndex,
                     Internal::AnimatorFunctionBase* animatorFunction,
                     AlphaFunction alpha,
                     const TimePeriod& period )
  : AnimatorConnectorBase( alpha, period ),
    mObject( &object ),
    mAnimator(0),
    mPropertyIndex( propertyIndex ),
    mComponentIndex( componentIndex ),
    mAnimatorFunction( animatorFunction )
  {
    object.AddObserver( *this );
  }

  // Undefined
  AnimatorConnector( const AnimatorConnector& );

  // Undefined
  AnimatorConnector& operator=( const AnimatorConnector& rhs );

  /**
   * From Object::Observer
   */
  virtual void SceneObjectAdded( Object& object )
  {
    //If the animator has not been created yet, create it now.
    if( !mAnimator )
    {
      CreateAnimator();
    }
  }

  /**
   * From Object::Observer
   */
  virtual void SceneObjectRemoved( Object& object )
  {
  }

  /**
   * From Object::Observer
   */
  virtual void ObjectDestroyed( Object& object )
  {
    mObject = NULL;
  }

   /**
   * Helper function to create a Scenegraph::Animator and add it to its correspondent SceneGraph::Animation.
   * @note This function will only be called the first time the object is added to the scene or at creation time if
   * the object was already in the scene
   */
  void CreateAnimator()
  {
    DALI_ASSERT_DEBUG( mAnimator == NULL );
    DALI_ASSERT_DEBUG( mAnimatorFunction != NULL );
    DALI_ASSERT_DEBUG( mParent != NULL );

    //Get the PropertyOwner the animator is going to animate
    const SceneGraph::PropertyOwner* propertyOwner = mObject->GetSceneObject();

    //Get SceneGraph::BaseProperty
    const SceneGraph::PropertyBase* baseProperty = mObject->GetSceneObjectAnimatableProperty( mPropertyIndex );

    //Check if property is a component of another property
    const int componentIndex = mObject->GetPropertyComponentIndex( mPropertyIndex );
    if( componentIndex != Property::INVALID_COMPONENT_INDEX )
    {
      mComponentIndex = componentIndex;
    }

    if( mComponentIndex == Property::INVALID_COMPONENT_INDEX )
    {
      ///Animating the whole property

      //Cast to AnimatableProperty
      const PropertyInterfaceType* animatableProperty = dynamic_cast< const PropertyInterfaceType* >( baseProperty );

      //Dynamic cast will fail if BaseProperty is not an AnimatableProperty
      DALI_ASSERT_DEBUG( animatableProperty != NULL && "Animating non-animatable property" );

      //Create the animator
      mAnimator = AnimatorType::New( *propertyOwner, *animatableProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );

    }
    else
    {
      ///Animating a component of the property
      if ( PropertyTypes::Get< Vector2 >() == baseProperty->GetType() )
      {
        // Animate float component of Vector2 property

        // Cast to AnimatableProperty of type Vector2
        const SceneGraph::AnimatableProperty<Vector2>* animatableProperty = dynamic_cast< const SceneGraph::AnimatableProperty<Vector2>* >( baseProperty );

        //Dynamic cast will fail if BaseProperty is not a Vector2 AnimatableProperty
        DALI_ASSERT_DEBUG( animatableProperty != NULL && "Animating non-animatable property" );

        switch( mComponentIndex )
        {
          case 0:
          {
            mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorX<Vector2> >::New( *propertyOwner,
                                                                                                 *animatableProperty,
                                                                                                 mAnimatorFunction,
                                                                                                 mAlphaFunction,
                                                                                                 mTimePeriod );
            break;
          }
          case 1:
          {
            mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorY<Vector2> >::New( *propertyOwner,
                                                                                                 *animatableProperty,
                                                                                                 mAnimatorFunction,
                                                                                                 mAlphaFunction,
                                                                                                 mTimePeriod );
            break;
          }
          default:
          {
            break;
          }
        }
      }
      else if ( PropertyTypes::Get< Vector3 >() == baseProperty->GetType() )
      {
        // Animate float component of Vector3 property

        // Cast to AnimatableProperty of type Vector3
        const SceneGraph::AnimatableProperty<Vector3>* animatableProperty = dynamic_cast< const SceneGraph::AnimatableProperty<Vector3>* >( baseProperty );

        //Dynamic cast will fail if BaseProperty is not a Vector3 AnimatableProperty
        DALI_ASSERT_DEBUG( animatableProperty != NULL && "Animating non-animatable property" );

        switch( mComponentIndex )
        {
          case 0:
          {
            mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorX<Vector3> >::New( *propertyOwner,
                                                                                                 *animatableProperty,
                                                                                                 mAnimatorFunction,
                                                                                                 mAlphaFunction,
                                                                                                 mTimePeriod );
            break;
          }
          case 1:
          {
            mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorY<Vector3> >::New( *propertyOwner,
                                                                                                 *animatableProperty,
                                                                                                 mAnimatorFunction,
                                                                                                 mAlphaFunction,
                                                                                                 mTimePeriod );
            break;
          }
          case 2:
          {
            mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorZ<Vector3> >::New( *propertyOwner,
                                                                                                 *animatableProperty,
                                                                                                 mAnimatorFunction,
                                                                                                 mAlphaFunction,
                                                                                                 mTimePeriod );
            break;
          }
          default:
          {
            break;
          }
        }
      }
      else if ( PropertyTypes::Get< Vector4 >() == baseProperty->GetType() )
      {
        // Animate float component of Vector4 property

        // Cast to AnimatableProperty of type Vector4
        const SceneGraph::AnimatableProperty<Vector4>* animatableProperty = dynamic_cast< const SceneGraph::AnimatableProperty<Vector4>* >( baseProperty );

        //Dynamic cast will fail if BaseProperty is not a Vector4 AnimatableProperty
        DALI_ASSERT_DEBUG( animatableProperty != NULL && "Animating non-animatable property" );

        switch( mComponentIndex )
        {
          case 0:
          {
            mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorX<Vector4> >::New( *propertyOwner,
                                                                                                 *animatableProperty,
                                                                                                 mAnimatorFunction,
                                                                                                 mAlphaFunction,
                                                                                                 mTimePeriod );
            break;
          }
          case 1:
          {
            mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorY<Vector4> >::New( *propertyOwner,
                                                                                                 *animatableProperty,
                                                                                                 mAnimatorFunction,
                                                                                                 mAlphaFunction,
                                                                                                 mTimePeriod );
            break;
          }
          case 2:
          {
            mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorZ<Vector4> >::New( *propertyOwner,
                                                                                                 *animatableProperty,
                                                                                                 mAnimatorFunction,
                                                                                                 mAlphaFunction,
                                                                                                 mTimePeriod );
            break;
          }
          case 3:
          {
            mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorW<Vector4> >::New( *propertyOwner,
                                                                                                 *animatableProperty,
                                                                                                 mAnimatorFunction,
                                                                                                 mAlphaFunction,
                                                                                                 mTimePeriod );
            break;
          }

          default:
          {
            break;
          }
        }
      }
    }

    DALI_ASSERT_DEBUG( mAnimator != NULL );

    // Add the new SceneGraph::Animator to its correspondent SceneGraph::Animation via message
    const SceneGraph::Animation* animation = mParent->GetSceneObject();
    DALI_ASSERT_DEBUG( NULL != animation );
    AddAnimatorMessage( mParent->GetEventThreadServices(), *animation, *mAnimator );
  }

protected:

  Object* mObject; ///< Not owned by the animator connector. Valid until ObjectDestroyed() is called.
  SceneGraph::AnimatorBase* mAnimator;

  Property::Index mPropertyIndex;
  int mComponentIndex;

  Internal::AnimatorFunctionBase* mAnimatorFunction;  ///< Owned by the animator connector until an Scenegraph::Animator is created
};


} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ANIMATOR_CONNECTOR_H__
