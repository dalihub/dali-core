#ifndef __DALI_INTERNAL_ANIMATOR_CONNECTOR_BASE_H__
#define __DALI_INTERNAL_ANIMATOR_CONNECTOR_BASE_H__

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
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/manager/update-manager.h>


namespace Dali
{

namespace Internal
{

class Animation;

/**
 * An abstract base class to create animator connectors and property re-setters for them when needed.
 *
 * The scene-graph objects are created by a Object e.g. Actor is a proxy for SceneGraph::Node.
 * AnimatorConnectorBase observes the proxy object, in order to detect when a scene-graph object is created
 * to avoid having unnecessary animations on the scene-graph and allow apps to create animations in initialisation
 */
class AnimatorConnectorBase: public Object::Observer
{
public:

  /**
   * Constructor.
   */
  AnimatorConnectorBase(
      Object& object,
      Property::Index propertyIndex,
      int32_t componentIndex,
      Internal::AnimatorFunctionBase* animatorFunction,
      AlphaFunction alpha,
      const TimePeriod& period)
  : mParent( nullptr ),
    mObject( &object ),
    mAnimator( nullptr ),
    mAnimatorFunction( animatorFunction ),
    mAlphaFunction(alpha),
    mTimePeriod(period),
    mPropertyIndex( propertyIndex ),
    mComponentIndex( componentIndex )
  {
    object.AddObserver( *this );
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatorConnectorBase()
  {
    if( mObject )
    {
      mObject->RemoveObserver( *this );
    }

    // If there is not a SceneGraph::Animator, the AnimatorConnector is responsible for deleting the mAnimatorFunction
    // otherwise, the animator function ownership is transferred to the SceneGraph::Animator
    if( !mAnimator )
    {
      delete mAnimatorFunction;
    }
  }

  /**
   * Helper function to create a Scenegraph::Animator and PropertyResetter and add it to its correspondent
   * SceneGraph::Animation.
   *
   * @note This function will only be called the first time the object is added to the scene or at creation time if
   * the object was already in the scene
   */
  void CreateAnimator()
  {
    DALI_ASSERT_DEBUG( mAnimator == nullptr );
    DALI_ASSERT_DEBUG( mAnimatorFunction != nullptr );
    DALI_ASSERT_DEBUG( mParent != nullptr );

    //Get the PropertyOwner the animator is going to animate
    const SceneGraph::PropertyOwner& propertyOwner = mObject->GetSceneObject();

    // Get SceneGraph::BaseProperty
    const SceneGraph::PropertyBase* baseProperty = mObject->GetSceneObjectAnimatableProperty( mPropertyIndex );
    DALI_ASSERT_ALWAYS( baseProperty && "Property is not animatable" );

    // Check if property is a component of another property
    const int32_t componentIndex = mObject->GetPropertyComponentIndex( mPropertyIndex );
    if( componentIndex != Property::INVALID_COMPONENT_INDEX )
    {
      mComponentIndex = componentIndex;
    }

    // call the type specific method to create the concrete animator
    bool resetterRequired = DoCreateAnimator( propertyOwner, *baseProperty );

    DALI_ASSERT_DEBUG( mAnimator != nullptr );

    // Add the new SceneGraph::Animator to its correspondent SceneGraph::Animation via message
    const SceneGraph::Animation* animation = mParent->GetSceneObject();
    DALI_ASSERT_DEBUG( nullptr != animation );
    AddAnimatorMessage( mParent->GetEventThreadServices(), *animation, *mAnimator );

    // Add the new SceneGraph::PropertyResetter to the update manager via message
    if( resetterRequired )
    {
      OwnerPointer<SceneGraph::PropertyResetterBase> resetter = SceneGraph::AnimatorResetter::New( propertyOwner, *baseProperty, *mAnimator );
      AddResetterMessage( mParent->GetEventThreadServices().GetUpdateManager(), resetter );
    }
  }

  /**
   * Type specific extension of animator creation
   */
  virtual bool DoCreateAnimator( const SceneGraph::PropertyOwner& propertyOwner, const SceneGraph::PropertyBase& baseProperty ) = 0;

  /**
   * Set the parent of the AnimatorConnector.
   * @pre The connector does not already have a parent.
   * @param [in] parent The parent object.
   */
  void SetParent( Animation& parent )
  {
    DALI_ASSERT_ALWAYS( mParent == nullptr && "AnimationConnector already has a parent" );
    mParent = &parent;

    if( mObject )
    {
      CreateAnimator();
    }
  }

  /**
   * Retrieve the parent of the AnimatorConnector.
   * @return The parent object, or nullptr.
   */
  Animation* GetParent() const
  {
    return mParent;
  }

  Object* GetObject() const
  {
    return mObject;
  }

  Property::Index GetPropertyIndex() const
  {
    return mPropertyIndex;
  }

  int32_t GetComponentIndex() const
  {
    return mComponentIndex;
  }

private:

  /**
   * From Object::Observer
   */
  virtual void SceneObjectAdded( Object& object ) override final
  {
    // If the animator has not been created yet, create it now.
    if( !mAnimator && mObject )
    {
      CreateAnimator();
    }
  }

  /**
   * From Object::Observer
   */
  virtual void SceneObjectRemoved( Object& object ) override final
  {
  }

  /**
   * From Object::Observer
   */
  virtual void ObjectDestroyed( Object& object )
  {
    mObject = nullptr;
  }

protected:

  Animation* mParent; ///< The parent owns the connector.
  Object* mObject; ///< Not owned by the animator connector. Valid until ObjectDestroyed() is called.
  SceneGraph::AnimatorBase* mAnimator;
  Internal::AnimatorFunctionBase* mAnimatorFunction;  ///< Owned by the animator connector until an Scenegraph::Animator is created

  AlphaFunction mAlphaFunction;
  TimePeriod mTimePeriod;

  Property::Index mPropertyIndex;
  int32_t mComponentIndex;

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ANIMATOR_CONNECTOR_BASE_H__
