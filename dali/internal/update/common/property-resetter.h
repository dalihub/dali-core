#ifndef DALI_INTERNAL_SCENEGRAPH_PROPERTY_RESETTER_H
#define DALI_INTERNAL_SCENEGRAPH_PROPERTY_RESETTER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLDUES
#include <cstdint> // int8_t

#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/property-component-accessor.h>
#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
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
  ~PropertyResetterBase() override
  {
    if(DALI_LIKELY(mInitialized))
    {
      if(mPropertyOwner != nullptr)
      {
        mPropertyOwner->RemoveObserver(*this);
      }
    }
  }

  /**
   * Initialize.
   *
   * Watches the property owner to track if it's disconnected or not.
   */
  virtual void Initialize()
  {
    DALI_ASSERT_ALWAYS(!mInitialized && "Dont call PropertyResetterBase::Initialize() twice");

    mInitialized = true;
    mPropertyOwner->AddObserver(*this);
    mPropertyOwner->SetUpdated(true);
  }

  /**
   * Request to reset the property to it's base value if the property owner is still alive and on stage
   */
  virtual void RequestResetToBaseValues()
  {
    if(mPropertyOwner != nullptr && mActive)
    {
      // If property-owner has disconnected, start aging.
      // We need to reset the property for two frames after disconnection to ensure both
      // property values are set appropriately.
      if(mDisconnected)
      {
        --mActive;
      }

      if(DALI_LIKELY(mBaseProperty))
      {
        mBaseProperty->RequestResetToBaseValue();
      }
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
  void PropertyOwnerConnected(PropertyOwner& owner) override
  {
    mDisconnected = false;
    mActive       = ACTIVE;
  }

  /**
   * Called when mPropertyOwner is disconnected from the scene graph.
   * @param[in] bufferIndex the current buffer index
   * @param[in] owner The property owner
   */
  NotifyReturnType PropertyOwnerDisconnected(BufferIndex bufferIndex, PropertyOwner& owner) override
  {
    mDisconnected = true;
    return NotifyReturnType::KEEP_OBSERVING;
  }

  /**
   * Called shortly before the propertyOwner is destroyed
   * @param[in] owner The property owner
   */
  void PropertyOwnerDestroyed(PropertyOwner& owner) override
  {
    mDisconnected  = true;
    mPropertyOwner = nullptr;

    // Don't need to wait another frame as the property is being destroyed
    mActive  = STOPPED;
    mRunning = STOPPED;
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
    const bool finished = mRunning <= STOPPED;
    if(mRunning == AGING)
    {
      // If this resetter is aging now, make it as stopped.
      // Now we can assume that this PropertyResetter is finished next frame.
      mRunning = STOPPED;
    }
    return finished;
  }

  enum
  {
    STOPPED = 0,
    AGING   = 1,
    ACTIVE  = 2,
  };

protected:
  /**
   * Constructor
   *
   * @param[in] propertyOwner The property owner storing the base property
   * @param[in] baseProperty The base property
   */
  PropertyResetterBase(PropertyOwner* propertyOwner,
                       PropertyBase*  baseProperty)
  : mPropertyOwner(propertyOwner),
    mBaseProperty(baseProperty),
    mRunning(ACTIVE),
    mActive(ACTIVE),
    mInitialized(false),
    mDisconnected(false)
  {
  }

  PropertyOwner* mPropertyOwner; ///< The property owner
  PropertyBase*  mBaseProperty;  ///< The base property being animated or constrained

  int8_t mRunning; ///< Used to determine if we should finish or not, 2 if running, 1 if aging, 0 if stopped
  int8_t mActive;  ///< 2 if active, 1 if aging, 0 if stopped
  bool   mInitialized : 1;
  bool   mDisconnected : 1; ///< True if the property owner has been disconnected
};

class BakerResetter : public PropertyResetterBase
{
public:
  enum class Lifetime
  {
    BAKE,
    SET
  };

  /**
   * New method.
   * @param[in] propertyOwner  The owner of the property
   * @param[in] baseProperty   The property being animated
   * @param[in] lifetime       How long this resetter stays alive (1 or 2 frames)
   * @return the new property resetter
   */
  static PropertyResetterBase* New(const PropertyOwner& propertyOwner,
                                   const PropertyBase&  baseProperty,
                                   Lifetime             lifetime)
  {
    return new BakerResetter(const_cast<PropertyOwner*>(&propertyOwner),
                             const_cast<PropertyBase*>(&baseProperty),
                             lifetime);
  }

  /**
   * Constructor
   * @param[in] propertyOwner The owner of the property
   * @param[in] baseProperty  The property being animated
   * @param[in] lifetime      How many frames this stays alive for
   */
  BakerResetter(PropertyOwner* propertyOwner,
                PropertyBase*  baseProperty,
                Lifetime       lifetime)
  : PropertyResetterBase(propertyOwner, baseProperty)
  {
    mRunning = lifetime == Lifetime::BAKE ? AGING : ACTIVE;
  }

  /**
   * Virtual destructor.
   */
  ~BakerResetter() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyResetterBase::RequestResetToBaseValues
   */
  void RequestResetToBaseValues() override
  {
    if(mPropertyOwner && mRunning > 0)
    {
      --mRunning;
      if(mRunning > 0)
      {
        mPropertyOwner->SetUpdated(true);
      }
      if(DALI_LIKELY(mBaseProperty))
      {
        mBaseProperty->RequestResetToBaseValue();
      }
    }
  }
};

/**
 * Specialization of Resetter based on templated modifier type (either Constraint or Animator)
 */
template<typename ModifierType>
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
  static PropertyResetterBase* New(const PropertyOwner& propertyOwner,
                                   const PropertyBase&  baseProperty,
                                   const ModifierType&  modifier)
  {
    return new Resetter<ModifierType>(const_cast<PropertyOwner*>(&propertyOwner),
                                      const_cast<PropertyBase*>(&baseProperty),
                                      const_cast<ModifierType*>(&modifier));
  }

  /**
   * Virtual destructor.
   */
  ~Resetter() override
  {
    // Disconnect from modifier object. Although this resetter should match the lifecycle of the modifier object,
    // there are situations where it is deleted first (e.g. if the property owner is destroyed)
    if(DALI_LIKELY(mInitialized) && mModifier)
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
  Resetter(PropertyOwner* propertyOwner,
           PropertyBase*  baseProperty,
           ModifierType*  modifier)
  : PropertyResetterBase(propertyOwner, baseProperty),
    mModifier(modifier)
  {
  }

  /**
   * Second phase initialization at render thread.
   */
  void Initialize() override
  {
    // Track the lifecycle of the modifying object
    mModifier->AddLifecycleObserver(*this);

    PropertyResetterBase::Initialize();
  }

  /**
   * The Animator or Constraint is destroyed
   */
  void ObjectDestroyed() override
  {
    // When the modifier is destroyed, reduce the running value to ensure we stay alive for
    // another frame to reset the other buffer.
    --mRunning;
    mModifier = nullptr;
  }

  ModifierType* mModifier;
};

using AnimatorResetter   = Resetter<SceneGraph::AnimatorBase>;
using ConstraintResetter = Resetter<SceneGraph::ConstraintBase>;

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif //DALI_INTERNAL_SCENEGRAPH_PROPERTY_RESETTER_H
