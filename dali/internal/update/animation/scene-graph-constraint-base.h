#ifndef __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_BASE_H__
#define __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_BASE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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
  ConstraintBase( PropertyOwnerSet& ownerSet );

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

private:

  /**
   * Helper to start observing property owners
   */
  void StartObservation()
  {
    for( PropertyOwnerIter iter = mObservedOwners.begin(); mObservedOwners.end() != iter; ++iter )
    {
      (*iter)->AddObserver( *this );
    }
  }

  /**
   * Helper to stop observing property owners
   */
  void StopObservation()
  {
    for( PropertyOwnerIter iter = mObservedOwners.begin(); mObservedOwners.end() != iter; ++iter )
    {
      (*iter)->RemoveObserver( *this );
    }

    mObservedOwners.clear();
  }

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerDestroyed()
   */
  virtual void PropertyOwnerDestroyed( PropertyOwner& owner )
  {
    // Discard pointer to destroyed property owner
    PropertyOwnerIter iter = mObservedOwners.find( &owner );
    DALI_ASSERT_DEBUG( mObservedOwners.end() != iter );
    mObservedOwners.erase( iter );

    // Stop observing the remaining property owners
    StopObservation();

    // Notification for derived class
    OnDisconnect();

    mDisconnected = true;
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

  PropertyOwnerSet mObservedOwners; ///< A set of pointers to each observed object. Not owned.
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
