#ifndef __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_BASE_H__
#define __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_BASE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/animation/scene-graph-constraint-declarations.h>

namespace Dali
{

namespace Internal
{

// value types used by messages
template <> struct ParameterType< Dali::Constraint::RemoveAction >
: public BasicType< Dali::Constraint::RemoveAction > {};

namespace SceneGraph
{

typedef Dali::Vector<PropertyOwner*>     PropertyOwnerContainer;
typedef PropertyOwnerContainer::Iterator PropertyOwnerIter;

/**
 * An abstract base class for Constraints.
 * This can be used to constrain a property of a scene-object, after animations have been applied.
 */
class ConstraintBase : public PropertyOwner, public PropertyOwner::Observer
{
public:

  typedef Dali::Constraint::RemoveAction RemoveAction;

  /**
   * Constructor
   */
  ConstraintBase( PropertyOwnerContainer& ownerContainer );

  /**
   * Virtual destructor.
   */
  virtual ~ConstraintBase();

  /**
   * Initialize the constraint.
   * This should by called by a scene-object, when the constraint is connected.
   */
  void OnConnect()
  {
    StartObservation();

    mDisconnected = false;
  }

  /**
   * @copydoc Dali::Constraint::SetRemoveAction()
   */
  void SetRemoveAction( RemoveAction action )
  {
    mRemoveAction = action;
  }

  /**
   * @copydoc Dali::Constraint::GetRemoveAction()
   */
  RemoveAction GetRemoveAction() const
  {
    return mRemoveAction;
  }

  /**
   * Bake the weight property.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] weight The new weight.
   */
  void BakeWeight( BufferIndex updateBufferIndex, float weight )
  {
    mWeight.Bake( updateBufferIndex, weight );
  }

  /**
   * Set the initial weight.
   * @pre The constraint has not been connected to the scene-graph.
   * @param[in] weight The new weight.
   */
  void SetInitialWeight( float weight )
  {
    mWeight.SetInitial( weight );
  }

  /**
   * Retrieve the weight property.
   * @param[in] bufferIndex The buffer index to read from.
   * @return The current weight.
   */
  float GetWeight( BufferIndex bufferIndex ) const
  {
    return mWeight[ bufferIndex ];
  }

  /**
   * Constrain the associated scene object.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  virtual void Apply( BufferIndex updateBufferIndex ) = 0;

  /**
   * Helper for internal test cases; only available for debug builds.
   * @return The current number of Constraint instances in existence.
   */
  static unsigned int GetCurrentInstanceCount();

  /**
   * Helper for internal test cases; only available for debug builds.
   * @return The total number of Constraint instances created during the Dali core lifetime.
   */
  static unsigned int GetTotalInstanceCount();

private:

  /**
   * Helper to start observing property owners
   */
  void StartObservation()
  {
    const PropertyOwnerIter end =  mObservedOwners.End();
    for( PropertyOwnerIter iter = mObservedOwners.Begin(); end != iter; ++iter )
    {
      (*iter)->AddObserver( *this );
    }
  }

  /**
   * Helper to stop observing property owners
   */
  void StopObservation()
  {
    const PropertyOwnerIter end =  mObservedOwners.End();
    for( PropertyOwnerIter iter = mObservedOwners.Begin(); end != iter; ++iter )
    {
      (*iter)->RemoveObserver( *this );
    }

    mObservedOwners.Clear();
  }

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerDisconnected()
   */
  virtual void PropertyOwnerDisconnected( BufferIndex bufferIndex, PropertyOwner& owner )
  {
    PropertyOwnerDestroyed( owner );
  }

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerDestroyed()
   */
  virtual void PropertyOwnerDestroyed( PropertyOwner& owner )
  {
    if ( !mDisconnected )
    {
      // Discard pointer to disconnected property owner
      PropertyOwnerIter iter = std::find( mObservedOwners.Begin(), mObservedOwners.End(), &owner );
      if( mObservedOwners.End() != iter )
      {
        mObservedOwners.Erase( iter );

        // Stop observing the remaining property owners
        StopObservation();

        // Notification for derived class
        OnDisconnect();

        mDisconnected = true;
      }
    }
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex );

  /**
   * Notify the derived class to disconnect from property owners
   */
  virtual void OnDisconnect() = 0;

public:

  AnimatableProperty<float> mWeight; ///< The constraint is "fully-applied" when weight = 1

protected:

  RemoveAction mRemoveAction;

  bool mFirstApply   : 1;
  bool mDisconnected : 1;

private:

  PropertyOwnerContainer mObservedOwners; ///< A set of pointers to each observed object. Not owned.

#ifdef DEBUG_ENABLED
  static unsigned int mCurrentInstanceCount;  ///< The current number of Constraint instances in existence.
  static unsigned int mTotalInstanceCount;    ///< The total number of Constraint instances created during the Dali core lifetime.
#endif
};

// Messages for ConstraintBase

inline void  BakeWeightMessage( EventToUpdate& eventToUpdate, const ConstraintBase& constraint, float weight )
{
  typedef MessageDoubleBuffered1< ConstraintBase, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &constraint, &ConstraintBase::BakeWeight, weight );
}

inline void  SetRemoveActionMessage( EventToUpdate& eventToUpdate, const ConstraintBase& constraint, Dali::Constraint::RemoveAction removeAction )
{
  typedef MessageValue1< ConstraintBase, Dali::Constraint::RemoveAction > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &constraint, &ConstraintBase::SetRemoveAction, removeAction );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_BASE_H__
