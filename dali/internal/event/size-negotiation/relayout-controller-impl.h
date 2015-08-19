#ifndef __DALI_INTERNAL_RELAYOUT_CONTROLLER_IMPL_H__
#define __DALI_INTERNAL_RELAYOUT_CONTROLLER_IMPL_H__

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/event/size-negotiation/memory-pool-relayout-container.h>

namespace Dali
{

namespace Integration
{
class RenderController;
}

namespace Internal
{

/**
 * @brief The relayout controller is responsible for taking request from actors to relayout their sizes.
 * The requests are actioned on at the end of the frame where all actors that have made a request are
 * resized.
 */
class RelayoutController : public Dali::BaseObject
{
public:

  /**
   * @brief Constructor.
   * We should only create a unique instance.
   * @param[in] controller to request a render from the RenderController if core is not processing events.
   */
  RelayoutController( Integration::RenderController& controller );

  /**
   * Destructor
   */
  virtual ~RelayoutController();

  /**
   * @brief Get the singleton of RelayoutController object.
   *
   * @return A handle to the RelayoutController control.
   */
  static RelayoutController* Get();

  /**
   * Set the stage size
   * @param width of the stage
   * @param height of the stage
   */
  void SetStageSize( unsigned int width, unsigned int height );

  /**
   * @brief Request to relayout the given actor and all sub-actors of it.
   *
   * This flags the actor and all actors dependent on it for relayout. The actual
   * relayout is performed at the end of the frame. This means that multiple calls to relayout
   * will not cause multiple relayouts to occur.
   *
   * @param[in] actor The actor to request relayout on
   * @param[in] dimension The dimension(s) to request the relayout on. Defaults to all dimensions
   */
  void RequestRelayout( Dali::Actor& actor, Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @brief Request to relayout of all actors in the sub-tree below the given actor.
   *
   * This flags the actor and all actors below it for relayout. The actual
   * relayout is performed at the end of the frame. This means that multiple calls to relayout
   * will not cause multiple relayouts to occur.
   *
   * @param[in] actor The actor to request relayout on
   */
  void RequestRelayoutTree( Dali::Actor& actor );

  /**
   * @brief Force propagate relayout flags through the tree. This is similiar to Request Relayout
   * except all dependencies have their flags reset in spite of whether they are all ready set.
   *
   * This is useful for resetting layout flags during the layout process.
   *
   * @param[in] actor The actor to propagate from
   * @param[in] dimension The dimension to propagate on
   */
  void PropagateFlags( Dali::Actor& actor, Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @brief Relayouts all actors that have been marked as dirty
   */
  void Relayout();

  /**
   * @brief Enable/disable the controller
   *
   * @param[in] enabled Flag to indicate whether the controller should be enabled or disabled
   */
  void SetEnabled( bool enabled );

  /**
   * @brief Return true if the relayout controller is currently performing a relayout
   *
   * @return Return true if the relayout controller is currently performing a relayout
   */
  bool IsPerformingRelayout() const;

  /**
   * @brief Sets whether core is processing events.
   *
   * @param[in] processingEvents whether core is processing events.
   */
  void SetProcessingCoreEvents( bool processingEvents );

public: // CALLBACKS

  /**
   * @brief Callback raised after the application creates the scene
   */
  void OnApplicationSceneCreated();

  /**
   * @brief Callback for when an object is destroyed
   *
   * @param[in] object The object being destroyed
   */
  void OnObjectDestroyed( const Dali::RefObject* object );

private:

  typedef Dali::Vector< BaseObject* > RawActorList;

  /**
   * @brief Request for relayout. Relays out whole scene.
   */
  void Request();

  /**
   * @brief Add actor to request list
   *
   * @param[in] actor The root of the sub tree to add
   */
  void AddRequest( Dali::Actor& actor );

  /**
   * @brief Remove actor from request list
   *
   * @param[in] actor The root of the sub tree to remove
   */
  void RemoveRequest( Dali::Actor& actor );

  /**
   * @brief Disconnect the Relayout() method from the Stage::EventProcessingFinishedSignal().
   */
  void Disconnect();

  /**
   * @brief Propagate dirty layout flags to actor and all sub-actors. This will stop propagating when a dirty actor
   * is found.
   *
   * @param[in] actor The actor to propagate on
   * @param[in] dimension The dimension to propagate on
   * @param[in] topOfSubTreeStack The top of the sub tree that this actor is in
   * @param[in] potentialRedundantSubRoots Actors collected as potentially already being included in relayout
   */
  void PropagateAll( Dali::Actor& actor, Dimension::Type dimension, std::vector< Dali::Actor >& topOfSubTreeStack, std::vector< Dali::Actor >& potentialRedundantSubRoots );

  /**
   * Queue an actor on the relayout container
   *
   * @param[in] actor The actor to be queued
   * @param[in] actors The container to add the actor to
   * @param[in] size The size that this actor should be
   */
  void QueueActor( Dali::Actor& actor, RelayoutContainer& actors, Vector2 size );

  /**
   * @brief Find the given object in the list and null it out
   *
   * @param[in] list The list to search
   * @param[in] object The object to search for
   */
  void FindAndZero( const RawActorList& list, const Dali::RefObject* object );

  // Undefined
  RelayoutController(const RelayoutController&);
  RelayoutController& operator=(const RelayoutController&);

private:

  Integration::RenderController& mRenderController;
  MemoryPoolObjectAllocator< MemoryPoolRelayoutContainer::RelayoutInfo > mRelayoutInfoAllocator;

  SlotDelegate< RelayoutController > mSlotDelegate;

  RawActorList mDirtyLayoutSubTrees;    ///< List of roots of sub trees that are dirty
  MemoryPoolRelayoutContainer* mRelayoutStack;  ///< Stack for relayouting

  Vector2 mStageSize;              ///< size of the stage
  bool mRelayoutConnection : 1;    ///< Whether EventProcessingFinishedSignal signal is connected.
  bool mRelayoutFlag : 1;          ///< Relayout flag to avoid unnecessary calls
  bool mEnabled : 1;               ///< Initially disabled. Must be enabled at some point.
  bool mPerformingRelayout : 1;    ///< The relayout controller is currently performing a relayout
  bool mProcessingCoreEvents : 1;  ///< Whether core is processing events.

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RELAYOUT_CONTROLLER_IMPL_H__
