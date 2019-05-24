#ifndef DALI_INTERNAL_ANIMATOR_CONNECTOR_H
#define DALI_INTERNAL_ANIMATOR_CONNECTOR_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/animation/animator-connector-base.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/property-component-accessor.h>

namespace Dali
{

namespace Internal
{

/**
 * AnimatorConnector is used to connect SceneGraph::Animators.
 *
 * SceneGraph::Animators weakly reference scene objects, and are automatically deleted when orphaned.
 * Therefore the AnimatorConnector is NOT responsible for disconnecting animators.
 * This is the common template for non float properties, there's a specialization for float properties as they can be component properties
 */
template < typename PropertyType >
class AnimatorConnector : public AnimatorConnectorBase
{
public:

  using AnimatorType = SceneGraph::Animator< PropertyType, PropertyAccessor<PropertyType> >;
  using PropertyInterfaceType = SceneGraph::AnimatableProperty< PropertyType >;

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
                                     int32_t componentIndex,
                                     AnimatorFunctionBase* animatorFunction,
                                     AlphaFunction alpha,
                                     const TimePeriod& period )
  {
    return new AnimatorConnector( object,
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
  }

private:

  /**
   * Private constructor; see also AnimatorConnector::New().
   */
  AnimatorConnector( Object& object,
                     Property::Index propertyIndex,
                     int32_t componentIndex,
                     Internal::AnimatorFunctionBase* animatorFunction,
                     AlphaFunction alpha,
                     const TimePeriod& period )
  : AnimatorConnectorBase( object, propertyIndex, componentIndex, animatorFunction, alpha, period )
  {
  }

  // Undefined
  AnimatorConnector() = delete;
  AnimatorConnector( const AnimatorConnector& ) = delete;
  AnimatorConnector& operator=( const AnimatorConnector& rhs ) = delete;

  /**
   * @copydoc AnimatorConnectorBase::DoCreateAnimator()
   */
  bool DoCreateAnimator( const SceneGraph::PropertyOwner& propertyOwner, const SceneGraph::PropertyBase& baseProperty ) override final
  {
    bool resetterRequired = false;
    // components only supported for float property type
    DALI_ASSERT_DEBUG( mComponentIndex == Property::INVALID_COMPONENT_INDEX );
    // Animating the whole property

    // Cast to AnimatableProperty
    const PropertyInterfaceType* animatableProperty = dynamic_cast< const PropertyInterfaceType* >( &baseProperty );

    if( animatableProperty == nullptr )
    {
      if( baseProperty.IsTransformManagerProperty() )
      {
        mAnimator = SceneGraph::AnimatorTransformProperty< PropertyType,TransformManagerPropertyAccessor<PropertyType> >::New( propertyOwner, baseProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
        // Don't reset transform manager properties - TransformManager will do it more efficiently
      }
      else
      {
        DALI_ASSERT_DEBUG( animatableProperty && "Animating non-animatable property" );
      }
    }
    else
    {
      // Create the animator and resetter
      mAnimator = AnimatorType::New( propertyOwner, *animatableProperty, mAnimatorFunction,
                                     mAlphaFunction, mTimePeriod );
      resetterRequired = true;
    }
    return resetterRequired;
  }
};

/**
 * Specialization for float as that type supports component properties
 */
template <>
class AnimatorConnector< float > : public AnimatorConnectorBase
{
public:

  using AnimatorType = SceneGraph::Animator< float, PropertyAccessor<float> >;
  using PropertyInterfaceType = SceneGraph::AnimatableProperty< float >;

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
                                     int32_t componentIndex,
                                     AnimatorFunctionBase* animatorFunction,
                                     AlphaFunction alpha,
                                     const TimePeriod& period )
  {
    return new AnimatorConnector( object,
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
  }

private:

  /**
   * Private constructor; see also AnimatorConnector::New().
   */
  AnimatorConnector( Object& object,
                     Property::Index propertyIndex,
                     int32_t componentIndex,
                     Internal::AnimatorFunctionBase* animatorFunction,
                     AlphaFunction alpha,
                     const TimePeriod& period )
  : AnimatorConnectorBase( object, propertyIndex, componentIndex, animatorFunction, alpha, period )
  {
  }

  // Undefined
  AnimatorConnector() = delete;
  AnimatorConnector( const AnimatorConnector& ) = delete;
  AnimatorConnector& operator=( const AnimatorConnector& rhs ) = delete;

  /**
   * @copydoc AnimatorConnectorBase::DoCreateAnimator()
   */
  bool DoCreateAnimator( const SceneGraph::PropertyOwner& propertyOwner, const SceneGraph::PropertyBase& baseProperty ) override final
  {
    bool resetterRequired = false;
    if( mComponentIndex == Property::INVALID_COMPONENT_INDEX )
    {
      // Animating the whole property

      // Cast to AnimatableProperty
      const PropertyInterfaceType* animatableProperty = dynamic_cast< const PropertyInterfaceType* >( &baseProperty );

      if( animatableProperty == nullptr )
      {
        if( baseProperty.IsTransformManagerProperty() )
        {
          mAnimator = SceneGraph::AnimatorTransformProperty< float,TransformManagerPropertyAccessor<float> >::New( propertyOwner, baseProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
          // Don't reset transform manager properties - TransformManager will do it more efficiently
        }
        else
        {
          DALI_ASSERT_DEBUG( animatableProperty && "Animating non-animatable property" );
        }
      }
      else
      {
        // Create the animator and resetter
        mAnimator = AnimatorType::New( propertyOwner, *animatableProperty, mAnimatorFunction,
                                       mAlphaFunction, mTimePeriod );
        resetterRequired = true;
      }
    }
    else
    {
      {
        // Animating a component of the property
        if ( PropertyTypes::Get< Vector2 >() == baseProperty.GetType() )
        {
          // Animate float component of Vector2 property

          // Cast to AnimatableProperty of type Vector2
          const SceneGraph::AnimatableProperty<Vector2>* animatableProperty = dynamic_cast< const SceneGraph::AnimatableProperty<Vector2>* >( &baseProperty );
          DALI_ASSERT_DEBUG( animatableProperty && "Animating non-animatable property" );

          switch( mComponentIndex )
          {
            case 0:
            {
              mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorX<Vector2> >::New( propertyOwner,
                                                                                                   *animatableProperty,
                                                                                                   mAnimatorFunction,
                                                                                                   mAlphaFunction,
                                                                                                   mTimePeriod );
              break;
            }
            case 1:
            {
              mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorY<Vector2> >::New( propertyOwner,
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

          resetterRequired = ( mAnimator != nullptr );
        }

        else if ( PropertyTypes::Get< Vector3 >() == baseProperty.GetType() )
        {
          // Animate float component of Vector3 property
          // Cast to AnimatableProperty of type Vector3
          const SceneGraph::AnimatableProperty<Vector3>* animatableProperty = dynamic_cast< const SceneGraph::AnimatableProperty<Vector3>* >( &baseProperty );

          if( animatableProperty == nullptr )
          {
            if( baseProperty.IsTransformManagerProperty() )
            {
              if( mComponentIndex == 0 )
              {
                mAnimator = SceneGraph::AnimatorTransformProperty< float,TransformManagerPropertyComponentAccessor<Vector3,0> >::New( propertyOwner, baseProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
              }
              else if( mComponentIndex == 1 )
              {
                mAnimator = SceneGraph::AnimatorTransformProperty< float,TransformManagerPropertyComponentAccessor<Vector3,1> >::New( propertyOwner, baseProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
              }
              else if( mComponentIndex == 2 )
              {
                mAnimator = SceneGraph::AnimatorTransformProperty< float,TransformManagerPropertyComponentAccessor<Vector3,2> >::New( propertyOwner, baseProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
              }
            }
            else
            {
              DALI_ASSERT_DEBUG( animatableProperty && "Animating non-animatable property" );
            }
            // Don't manually reset transform property - TransformManager will do it more efficiently
          }
          else
          {
            // Dynamic cast will fail if BaseProperty is not a Vector3 AnimatableProperty
            DALI_ASSERT_DEBUG( animatableProperty && "Animating non-animatable property" );

            switch( mComponentIndex )
            {
              case 0:
              {
                mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorX<Vector3> >::New( propertyOwner,
                                                                                                     *animatableProperty,
                                                                                                     mAnimatorFunction,
                                                                                                     mAlphaFunction,
                                                                                                     mTimePeriod );
                break;
              }
              case 1:
              {
                mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorY<Vector3> >::New( propertyOwner,
                                                                                                     *animatableProperty,
                                                                                                     mAnimatorFunction,
                                                                                                     mAlphaFunction,
                                                                                                     mTimePeriod );
                break;
              }
              case 2:
              {
                mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorZ<Vector3> >::New( propertyOwner,
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

            resetterRequired = ( mAnimator != nullptr );
          }
        }
        else if ( PropertyTypes::Get< Vector4 >() == baseProperty.GetType() )
        {
          // Animate float component of Vector4 property

          // Cast to AnimatableProperty of type Vector4
          const SceneGraph::AnimatableProperty<Vector4>* animatableProperty = dynamic_cast< const SceneGraph::AnimatableProperty<Vector4>* >( &baseProperty );

          //Dynamic cast will fail if BaseProperty is not a Vector4 AnimatableProperty
          DALI_ASSERT_DEBUG( animatableProperty && "Animating non-animatable property" );

          switch( mComponentIndex )
          {
            case 0:
            {
              mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorX<Vector4> >::New( propertyOwner,
                                                                                                   *animatableProperty,
                                                                                                   mAnimatorFunction,
                                                                                                   mAlphaFunction,
                                                                                                   mTimePeriod );
              break;
            }
            case 1:
            {
              mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorY<Vector4> >::New( propertyOwner,
                                                                                                   *animatableProperty,
                                                                                                   mAnimatorFunction,
                                                                                                   mAlphaFunction,
                                                                                                   mTimePeriod );
              break;
            }
            case 2:
            {
              mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorZ<Vector4> >::New( propertyOwner,
                                                                                                   *animatableProperty,
                                                                                                   mAnimatorFunction,
                                                                                                   mAlphaFunction,
                                                                                                   mTimePeriod );
              break;
            }
            case 3:
            {
              mAnimator = SceneGraph::Animator< float, PropertyComponentAccessorW<Vector4> >::New( propertyOwner,
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
          resetterRequired = ( mAnimator != nullptr );
        }
      }
    }
    return resetterRequired;
  }
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_ANIMATOR_CONNECTOR_H
