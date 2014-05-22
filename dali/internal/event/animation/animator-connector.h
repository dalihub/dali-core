#ifndef __DALI_INTERNAL_ANIMATOR_CONNECTOR_H__
#define __DALI_INTERNAL_ANIMATOR_CONNECTOR_H__

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
#include <dali/internal/event/common/proxy-object.h>
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
 * The scene-graph objects are created by a ProxyObject e.g. Actor is a proxy for SceneGraph::Node.
 * AnimatorConnector observes the proxy object, in order to detect when a scene-graph object is created.
 *
 * SceneGraph::Animators weakly reference scene objects, and are automatically deleted when orphaned.
 * Therefore the AnimatorConnector is NOT responsible for disconnecting animators.
 */
template < typename PropertyType >
class AnimatorConnector : public AnimatorConnectorBase, public ProxyObject::Observer
{
public:

  typedef boost::function< PropertyType (float, const PropertyType&) > AnimatorFunction;
  typedef SceneGraph::Animator< PropertyType, PropertyAccessor<PropertyType> > AnimatorType;
  typedef SceneGraph::AnimatableProperty< PropertyType > PropertyInterfaceType;

  /**
   * Construct a new animator connector.
   * @param[in] proxy The proxy for a scene-graph object to animate.
   * @param[in] propertyIndex The index of a property provided by the object.
   * @param[in] componentIndex Index to a sub component of a property, for use with Vector2, Vector3 and Vector4 (INVALID_PROPERTY_COMPONENTINDEX to use the whole property)
   * @param[in] animatorFunction A function used to animate the property.
   * @param[in] alpha The alpha function to apply.
   * @param[in] period The time period of the animator.
   * @return A pointer to a newly allocated animator connector.
   */
  static AnimatorConnectorBase* New( ProxyObject& proxy,
                                     Property::Index propertyIndex,
                                     int componentIndex,
                                     const AnimatorFunction& animatorFunction,
                                     AlphaFunction alpha,
                                     const TimePeriod& period )
  {
    return new AnimatorConnector< PropertyType >( proxy,
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
    if( mProxy )
    {
      mProxy->RemoveObserver( *this );
    }

    // Removing animators via Animation is not necessary.
    // The corresponding scene graph animation will destroy any animators that become disconnected.
  }

  /**
   * From AnimatorConnectorBase.
   * This is only expected to be called once, when added to an Animation.
   */
  void SetParent( Animation& parent )
  {
    DALI_ASSERT_ALWAYS( mParent == NULL && "AnimationConnector already has a parent" );
    mParent = &parent;

    if( mProxy )
    {
      // Connect an animator, if the proxy has added an object to the scene-graph
      const SceneGraph::PropertyOwner* object = mProxy->GetSceneObject();
      if( object )
      {
        ConnectAnimator( *object );
      }
    }
  }

private:

  /**
   * Private constructor; see also AnimatorConnector::New().
   */
  AnimatorConnector( ProxyObject& proxy,
                     Property::Index propertyIndex,
                     int componentIndex,
                     const AnimatorFunction& animatorFunction,
                     AlphaFunction alpha,
                     const TimePeriod& period )
  : AnimatorConnectorBase( alpha, period ),
    mProxy( &proxy ),
    mPropertyIndex( propertyIndex ),
    mComponentIndex( componentIndex ),
    mAnimatorFunction( animatorFunction ),
    mConnected( false )
  {
    proxy.AddObserver( *this );
  }

  // Undefined
  AnimatorConnector( const AnimatorConnector& );

  // Undefined
  AnimatorConnector& operator=( const AnimatorConnector& rhs );

  /**
   * From ProxyObject::Observer
   */
  virtual void SceneObjectAdded( ProxyObject& proxy )
  {
    const SceneGraph::PropertyOwner* object = mProxy->GetSceneObject();
    DALI_ASSERT_DEBUG( NULL != object );

    ConnectAnimator( *object );
  }

  /**
   * From ProxyObject::Observer
   */
  virtual void SceneObjectRemoved( ProxyObject& proxy )
  {
    // Removing animators via Animation is not necessary.
    // The corresponding scene graph animation will destroy any animators that become disconnected.
    mConnected = false;
  }

  /**
   * From ProxyObject::Observer
   */
  virtual void ProxyDestroyed( ProxyObject& proxy )
  {
    mConnected = false;
    mProxy = NULL;
  }

  /**
   * Create and connect an animator via update manager
   */
  void ConnectAnimator( const SceneGraph::PropertyOwner& object )
  {
    DALI_ASSERT_DEBUG( false == mConnected ); // Guard against double connections
    DALI_ASSERT_DEBUG( NULL != mParent );

    const SceneGraph::PropertyBase* base = mProxy->GetSceneObjectAnimatableProperty( mPropertyIndex );

    const PropertyInterfaceType* sceneProperty = dynamic_cast< const PropertyInterfaceType* >( base );
    DALI_ASSERT_DEBUG( NULL != sceneProperty && "Animating property with invalid type" );

    SceneGraph::AnimatorBase* animator = AnimatorType::New( *sceneProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
    DALI_ASSERT_DEBUG( NULL != animator );

    const SceneGraph::Animation* animation = mParent->GetSceneObject();
    DALI_ASSERT_DEBUG( NULL != animation );

    AddAnimatorMessage( mParent->GetUpdateManager().GetEventToUpdate(), *animation, *animator, object );

    mConnected = true; // not really necessary, but useful for asserts
  }

protected:

  ProxyObject* mProxy; ///< Not owned by the animator connector. Valid until ProxyDestroyed() is called.

  Property::Index mPropertyIndex;

  int mComponentIndex;

  AnimatorFunction mAnimatorFunction;

  bool mConnected; ///< Used to guard against double connections
};

/**
 * Variant which allows float components to be animated individually.
 */
template <>
class AnimatorConnector<float> : public AnimatorConnectorBase, public ProxyObject::Observer
{
public:

  typedef boost::function< float (float, const float&) > AnimatorFunction;
  typedef SceneGraph::Animator< float, PropertyAccessor<float> > AnimatorType;

  /**
   * Construct a new animator connector.
   * @param[in] proxy The proxy for a scene-graph object to animate.
   * @param[in] propertyIndex The index of a property provided by the object.
   * @param[in] componentIndex Index to a sub component of a property, for use with Vector2, Vector3 and Vector4
   * @param[in] animatorFunction A function used to animate the property.
   * @param[in] alpha The alpha function to apply.
   * @param[in] period The time period of the animator.
   * @return A pointer to a newly allocated animator connector.
   */
  static AnimatorConnectorBase* New( ProxyObject& proxy,
                                     Property::Index propertyIndex,
                                     int componentIndex,
                                     const AnimatorFunction& animatorFunction,
                                     AlphaFunction alpha,
                                     const TimePeriod& period )
  {
    return new AnimatorConnector<float>( proxy,
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
    if( mProxy )
    {
      mProxy->RemoveObserver( *this );
    }

    // Removing animators via Animation is not necessary.
    // The corresponding scene graph animation will destroy any animators that become disconnected.
  }

  /**
   * From AnimatorConnectorBase.
   * This is only expected to be called once, when added to an Animation.
   */
  void SetParent( Animation& parent )
  {
    DALI_ASSERT_ALWAYS( mParent == NULL && "AnimationConnector already has a parent");
    mParent = &parent;

    if( mProxy )
    {
      // Connect an animator, if the proxy has added an object to the scene-graph
      const SceneGraph::PropertyOwner* object = mProxy->GetSceneObject();
      if( object )
      {
        ConnectAnimator( *object );
      }
    }
  }

private:

  /**
   * Private constructor; see also AnimatorConnector::New().
   */
  AnimatorConnector( ProxyObject& proxy,
                     Property::Index propertyIndex,
                     int componentIndex,
                     const AnimatorFunction& animatorFunction,
                     AlphaFunction alpha,
                     const TimePeriod& period )
  : AnimatorConnectorBase( alpha, period ),
    mProxy( &proxy ),
    mPropertyIndex( propertyIndex ),
    mComponentIndex( componentIndex ),
    mAnimatorFunction( animatorFunction ),
    mConnected( false )
  {
    proxy.AddObserver( *this );
  }

  // Undefined
  AnimatorConnector( const AnimatorConnector& );

  // Undefined
  AnimatorConnector& operator=( const AnimatorConnector& rhs );

  /**
   * From ProxyObject::Observer
   */
  virtual void SceneObjectAdded( ProxyObject& proxy )
  {
    const SceneGraph::PropertyOwner* object = mProxy->GetSceneObject();
    DALI_ASSERT_DEBUG( NULL != object );

    ConnectAnimator( *object );
  }

  /**
   * From ProxyObject::Observer
   */
  virtual void SceneObjectRemoved( ProxyObject& proxy )
  {
    // Removing animators via Animation is not necessary.
    // The corresponding scene graph animation will destroy any animators that become disconnected.
    mConnected = false;
  }

  /**
   * From ProxyObject::Observer
   */
  virtual void ProxyDestroyed( ProxyObject& proxy )
  {
    mConnected = false;
    mProxy = NULL;
  }

  /**
   * Create and connect an animator via update manager
   */
  void ConnectAnimator( const SceneGraph::PropertyOwner& object )
  {
    DALI_ASSERT_DEBUG( false == mConnected ); // Guard against double connections
    DALI_ASSERT_DEBUG( NULL != mParent );

    const SceneGraph::PropertyBase* base = mProxy->GetSceneObjectAnimatableProperty( mPropertyIndex );
    DALI_ASSERT_DEBUG( NULL != base );

    SceneGraph::AnimatorBase* animator( NULL );

    const int componentIndex = mProxy->GetPropertyComponentIndex( mPropertyIndex );
    if( componentIndex != Property::INVALID_COMPONENT_INDEX )
    {
      mComponentIndex = componentIndex;
    }

    if ( Property::INVALID_COMPONENT_INDEX == mComponentIndex )
    {
      // Not a Vector3 or Vector4 component, expecting float type
      DALI_ASSERT_DEBUG( PropertyTypes::Get< float >() == base->GetType() );

      typedef SceneGraph::AnimatableProperty< float > PropertyInterfaceType;

      const PropertyInterfaceType* sceneProperty = dynamic_cast< const PropertyInterfaceType* >( base );
      DALI_ASSERT_DEBUG( NULL != sceneProperty );

      animator = AnimatorType::New( *sceneProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
    }
    else
    {
      // Expecting Vector3 or Vector4 type

      if ( PropertyTypes::Get< Vector3 >() == base->GetType() )
      {
        // Animate float component of Vector3 property
        typedef SceneGraph::AnimatableProperty< Vector3 > PropertyInterfaceType;

        const PropertyInterfaceType* sceneProperty = dynamic_cast< const PropertyInterfaceType* >( base );
        DALI_ASSERT_DEBUG( NULL != sceneProperty );

        if ( 0 == mComponentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorX<Vector3> > Vector3AnimatorType;
          animator = Vector3AnimatorType::New( *sceneProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
        }
        else if ( 1 == mComponentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorY<Vector3> > Vector3AnimatorType;
          animator = Vector3AnimatorType::New( *sceneProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
        }
        else if ( 2 == mComponentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorZ<Vector3> > Vector3AnimatorType;
          animator = Vector3AnimatorType::New( *sceneProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
        }
      }
      else if ( PropertyTypes::Get< Vector4 >() == base->GetType() )
      {
        // Animate float component of Vector4 property
        typedef SceneGraph::AnimatableProperty< Vector4 > PropertyInterfaceType;

        const PropertyInterfaceType* sceneProperty = dynamic_cast< const PropertyInterfaceType* >( base );
        DALI_ASSERT_DEBUG( NULL != sceneProperty );

        if ( 0 == mComponentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorX<Vector4> > Vector4AnimatorType;
          animator = Vector4AnimatorType::New( *sceneProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
        }
        else if ( 1 == mComponentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorY<Vector4> > Vector4AnimatorType;
          animator = Vector4AnimatorType::New( *sceneProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
        }
        else if ( 2 == mComponentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorZ<Vector4> > Vector4AnimatorType;
          animator = Vector4AnimatorType::New( *sceneProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
        }
        else if ( 3 == mComponentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorW<Vector4> > Vector4AnimatorType;
          animator = Vector4AnimatorType::New( *sceneProperty, mAnimatorFunction, mAlphaFunction, mTimePeriod );
        }
      }
    }

    DALI_ASSERT_DEBUG( NULL != animator && "Animating property with invalid type" );

    const SceneGraph::Animation* animation = mParent->GetSceneObject();
    DALI_ASSERT_DEBUG( NULL != animation );

    AddAnimatorMessage( mParent->GetUpdateManager().GetEventToUpdate(), *animation, *animator, object );

    mConnected = true; // not really necessary, but useful for asserts
  }

protected:

  ProxyObject* mProxy; ///< Not owned by the animator connector. Valid until ProxyDestroyed() is called.

  Property::Index mPropertyIndex;

  int mComponentIndex;

  AnimatorFunction mAnimatorFunction;

  bool mConnected; ///< Used to guard against double connections
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ANIMATOR_CONNECTOR_H__
