#ifndef DALI_INTERNAL_SCENEGRAPH_PROPERTY_RESETTER_H
#define DALI_INTERNAL_SCENEGRAPH_PROPERTY_RESETTER_H

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
 */

#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/property-component-accessor.h>
#include <dali/internal/update/common/property-owner.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

/**
 * Class to reset the watched property to it's base value. Used by UpdateManager to
 * reset animating and constrained properties. The derived classes AnimatorResetter and
 * ConstraintResetter are designed to match the lifecycle of the SceneGraph::Animator
 * and SceneGraph::Constraint respectively.
 */
class PropertyResetterBase : public PropertyOwner::Observer
{
public:
  /**
   * Virtual Destructor
   */
  virtual ~PropertyResetterBase()
  {
    if( mPropertyOwner != nullptr )
    {
      mPropertyOwner->RemoveObserver(*this);
    }
  }

  /**
   * Initialize.
   *
   * Watches the property owner to track if it's disconnected or not.
   */
  void Initialize()
  {
    mPropertyOwner->AddObserver(*this);
  }

  /**
   * Reset the property to it's base value if the property owner is still alive and on stage
   * @param[in] updateBufferIndex the current buffer index
   */
  void ResetToBaseValue( BufferIndex updateBufferIndex )
  {
    if( mPropertyOwner != nullptr && !mDisconnected )
    {
      mBaseProperty->ResetToBaseValue( updateBufferIndex );
    }
  };

  /**
   * Called when mPropertyOwner is connected to the scene graph.
   *
   * Note, this resetter object may be created after the property owner has been connected
   * to the stage, so we track disconnection and re-connection instead of connection
   *
   * @param[in] owner The property owner
   */
  virtual void PropertyOwnerConnected( PropertyOwner& owner ) override
  {
    mDisconnected = false;
  }

  /**
   * Called when mPropertyOwner is disconnected from the scene graph.
   * @param[in] bufferIndex the current buffer index
   * @param[in] owner The property owner
   */
  virtual void PropertyOwnerDisconnected( BufferIndex bufferIndex, PropertyOwner& owner ) override
  {
    mDisconnected = true;
  }

  /**
   * Called shortly before the propertyOwner is destroyed
   * @param[in] owner The property owner
   */
  virtual void PropertyOwnerDestroyed( PropertyOwner& owner ) override
  {
    mDisconnected = true;
    mPropertyOwner = nullptr;

    // Don't need to wait another frame as the property is being destroyed
    mRunning = 0;
  }

  /**
   * Determine if the resetter has finished.
   *
   * If an animation or constraint stops, then we need to reset the
   * property in the next frame as well to ensure both property values
   * are set appropriately.
   *
   * @return true if the resetter has finished.
   */
  virtual bool IsFinished()
  {
    bool finished = mRunning <= 0;
    if( mRunning == 1 )
    {
      mRunning = 0;
    }
    return finished;
  }

protected:

  /**
   * Constructor
   *
   * @param[in] propertyOwner The property owner storing the base property
   * @param[in] baseProperty The base property
   */
  PropertyResetterBase( PropertyOwner* propertyOwner,
                        PropertyBase* baseProperty )
  : mPropertyOwner( propertyOwner ),
    mBaseProperty( baseProperty ),
    mRunning( 2 ),
    mDisconnected( false )
  {
  }

  PropertyOwner* mPropertyOwner; ///< The property owner
  PropertyBase* mBaseProperty;   ///< The base property being animated or constrained
  int mRunning;                  ///< 2 if running, 1 if aging, 0 if stopped
  bool mDisconnected;            ///< True if the property owner has been disconnected
};


/**
 * Specialization of Resetter based on templated modifier type (either Constraint or Animator)
 */
template< typename ModifierType>
class Resetter : public PropertyResetterBase, public ModifierType::LifecycleObserver
{
public:

  /**
   * New method.
   * @param[in] propertyOwner  The owner of the property
   * @param[in] baseProperty   The property being animated
   * @param[in] modifier       The scene-graph animator/constraint of the property
   * @return the new property resetter
   */
  static PropertyResetterBase* New( const PropertyOwner& propertyOwner,
                                    const PropertyBase& baseProperty,
                                    const ModifierType& modifier )
  {
    return new Resetter<ModifierType>( const_cast<PropertyOwner*>( &propertyOwner ),
                                       const_cast<PropertyBase*>( &baseProperty ),
                                       const_cast<ModifierType*>( &modifier ) );
  }

  /**
   * Virtual destructor.
   */
  virtual ~Resetter()
  {
    // Disconnect from modifier object. Although this resetter should match the lifecycle of the modifier object,
    // there are situations where it is deleted first (e.g. if the property owner is destroyed)
    if( mModifier )
    {
      mModifier->RemoveLifecycleObserver(*this);
    }
  }

private:
  /**
   * Constructor
   * @param[in] propertyOwner The owner of the property
   * @param[in] baseProperty  The property being animated
   * @param[in] modifier      The scene-graph animator/constraint of the property
   */
  Resetter( PropertyOwner* propertyOwner,
            PropertyBase* baseProperty,
            ModifierType* modifier )
  : PropertyResetterBase( propertyOwner, baseProperty ),
    mModifier( modifier )
  {
    // Track the lifecycle of the modifying object
    mModifier->AddLifecycleObserver(*this);
  }

  /**
   * The Animator or Constraint is destroyed
   */
  virtual void ObjectDestroyed() override
  {
    // When the modifier is destroyed, reduce the running value to ensure we stay alive for
    // another frame to reset the other buffer.
    --mRunning;
    mModifier = nullptr;
  }

  ModifierType* mModifier;
};


typedef Resetter<SceneGraph::AnimatorBase> AnimatorResetter;
typedef Resetter<SceneGraph::ConstraintBase> ConstraintResetter;



} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif //DALI_INTERNAL_SCENEGRAPH_PROPERTY_RESETTER_H
