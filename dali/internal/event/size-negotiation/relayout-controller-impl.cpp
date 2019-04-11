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

// CLASS HEADER
#include "relayout-controller-impl.h"

// EXTERNAL INCLUDES
#if defined(DEBUG_ENABLED)
#include <sstream>
#endif // defined(DEBUG_ENABLED)

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/render-controller.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{

namespace Internal
{

namespace
{
#if defined(DEBUG_ENABLED)

Integration::Log::Filter* gLogFilter( Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_RELAYOUT_CONTROLLER") );

/**
 * Prints out all the children of the given actor when debug is enabled.
 *
 * @param[in]  actor  The actor whose children to print.
 * @param[in]  level  The number of " | " to put in front of the children.
 */
void PrintChildren( Dali::Actor actor, int level )
{
  std::ostringstream output;

  for ( int t = 0; t < level; ++t )
  {
    output << " | ";
  }

  output << actor.GetTypeName();

  output << ", " << actor.GetName();

  output << " - Pos: " << actor.GetCurrentPosition() << " Size: " << actor.GetTargetSize();

  output << ", Dirty: (" << ( GetImplementation( actor ).IsLayoutDirty( Dimension::WIDTH ) ? "TRUE" : "FALSE" ) << "," << ( GetImplementation( actor ).IsLayoutDirty( Dimension::HEIGHT ) ? "TRUE" : "FALSE" ) << ")";
  output << ", Negotiated: (" << ( GetImplementation( actor ).IsLayoutNegotiated( Dimension::WIDTH ) ? "TRUE" : "FALSE" ) << "," << ( GetImplementation( actor ).IsLayoutNegotiated( Dimension::HEIGHT ) ? "TRUE" : "FALSE" ) << ")";
  output << ", Enabled: " << ( GetImplementation( actor ).IsRelayoutEnabled() ? "TRUE" : "FALSE" );

  output << ", (" << actor.GetObjectPtr() << ")" << std::endl;

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, output.str().c_str() );

  ++level;
  uint32_t numChildren = actor.GetChildCount();
  for( uint32_t i=0; i<numChildren; ++i )
  {
    PrintChildren( actor.GetChildAt(i), level );
  }
  --level;
}

/**
 * Prints the entire hierarchy of the scene.
 */
void PrintHierarchy()
{
  if ( gLogFilter->IsEnabledFor( Debug::Verbose ) )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "---------- ROOT LAYER ----------\n" );

    PrintChildren( Stage::GetCurrent()->GetRootLayer(), 0 );
  }
}

#define PRINT_HIERARCHY PrintHierarchy()

#else // defined(DEBUG_ENABLED)

#define PRINT_HIERARCHY

#endif // defined(DEBUG_ENABLED)

} // unnamed namespace

RelayoutController::RelayoutController( Integration::RenderController& controller )
: mRenderController( controller ),
  mRelayoutInfoAllocator(),
  mSlotDelegate( this ),
  mRelayoutStack( new MemoryPoolRelayoutContainer( mRelayoutInfoAllocator ) ),
  mRelayoutConnection( false ),
  mRelayoutFlag( false ),
  mEnabled( false ),
  mPerformingRelayout( false ),
  mProcessingCoreEvents( false )
{
  // Make space for 32 controls to avoid having to copy construct a lot in the beginning
  mRelayoutStack->Reserve( 32 );
}

RelayoutController::~RelayoutController()
{
  delete mRelayoutStack;
}

RelayoutController* RelayoutController::Get()
{
  return &ThreadLocalStorage::Get().GetRelayoutController();
}

void RelayoutController::QueueActor( Internal::Actor* actor, RelayoutContainer& actors, Vector2 size )
{
  if( actor && actor->RelayoutRequired() )
  {
    Dali::Actor actorHandle = Dali::Actor( actor );
    actors.Add( actorHandle, size );
  }
}

void RelayoutController::RequestRelayout( Dali::Actor& actor, Dimension::Type dimension )
{
  if( !mEnabled )
  {
    return;
  }

  std::vector< Dali::Actor > potentialRedundantSubRoots;
  std::vector< Dali::Actor > topOfSubTreeStack;

  topOfSubTreeStack.push_back( actor );

  // Propagate on all dimensions
  for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      // Do the propagation
      PropagateAll( actor, static_cast< Dimension::Type >( 1 << i ), topOfSubTreeStack, potentialRedundantSubRoots );
    }
  }

  while( !topOfSubTreeStack.empty() )
  {
    // Request this actor as head of sub-tree if it is not dependent on a parent that is dirty
    Dali::Actor subTreeActor = topOfSubTreeStack.back();
    topOfSubTreeStack.pop_back();

    Dali::Actor parent = subTreeActor.GetParent();
    if( !parent || !( GetImplementation( subTreeActor ).RelayoutDependentOnParent() && GetImplementation( parent ).RelayoutRequired() ) )
    {
      // Add sub tree root to relayout list
      AddRequest( subTreeActor );

      // Flag request for end of frame
      Request();
    }
    else
    {
      potentialRedundantSubRoots.push_back( subTreeActor );
    }
  }

  // Remove any redundant sub-tree heads
  for( std::vector< Dali::Actor >::iterator it = potentialRedundantSubRoots.begin(), itEnd = potentialRedundantSubRoots.end(); it != itEnd; ++it )
  {
    Dali::Actor subRoot = *it;

    RemoveRequest( subRoot );
  }

  if ( !mProcessingCoreEvents )
  {
    mRenderController.RequestProcessEventsOnIdle( false );
  }
}

void RelayoutController::OnApplicationSceneCreated()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "[Internal::RelayoutController::OnApplicationSceneCreated]\n" );

  // Open relayout controller to receive relayout requests
  mEnabled = true;

  // Flag request for end of frame
  Request();
}

void RelayoutController::RequestRelayoutTree( Dali::Actor& actor )
{
  if( !mEnabled )
  {
    return;
  }

  // Only set dirty flag if doing relayout and not already marked as dirty
  Actor& actorImpl = GetImplementation( actor );
  if( actorImpl.RelayoutPossible() )
  {
    // If parent is not in relayout we are at the top of a new sub-tree
    Dali::Actor parent = actor.GetParent();
    if( !parent || !GetImplementation( parent ).IsRelayoutEnabled() )
    {
      AddRequest( actor );
    }

    // Set dirty flag on actors that are enabled
    actorImpl.SetLayoutDirty( true );
    actorImpl.SetLayoutNegotiated( false );    // Reset this flag ready for next relayout
  }

  // Propagate down to children
  for( uint32_t i = 0; i < actor.GetChildCount(); ++i )
  {
    Dali::Actor child = actor.GetChildAt( i );

    RequestRelayoutTree( child );
  }
}

void RelayoutController::PropagateAll( Dali::Actor& actor, Dimension::Type dimension, std::vector< Dali::Actor >& topOfSubTreeStack, std::vector< Dali::Actor >& potentialRedundantSubRoots )
{
  // Only set dirty flag if doing relayout and not already marked as dirty
  Actor& actorImpl = GetImplementation( actor );
  if( actorImpl.RelayoutPossible( dimension ) )
  {
    // Set dirty and negotiated flags
    actorImpl.SetLayoutDirty( true, dimension );
    actorImpl.SetLayoutNegotiated( false, dimension );    // Reset this flag ready for next relayout

    // Check for dimension dependecy: width for height/height for width etc
    // Check each possible dimension and see if it is dependent on the input one
    for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      Dimension::Type dimensionToCheck = static_cast< Dimension::Type >( 1 << i );

      if( actorImpl.RelayoutDependentOnDimension( dimension, dimensionToCheck ) &&
          !actorImpl.IsLayoutDirty( dimensionToCheck ) )
      {
        PropagateAll( actor, dimensionToCheck, topOfSubTreeStack, potentialRedundantSubRoots );
      }
    }

    // Propagate up to parent
    Dali::Actor parent = actor.GetParent();
    if( parent )
    {
      Actor& parentImpl = GetImplementation( parent );
      if( parentImpl.RelayoutDependentOnChildren( dimension ) && !parentImpl.IsLayoutDirty( dimension ) )
      {
        // Store the highest parent reached
        bool found = false;
        for( auto&& element : topOfSubTreeStack )
        {
          if( element == parent )
          {
            found = true;
            break;
          }
        }

        if( !found )
        {
          topOfSubTreeStack.push_back( parent );
        }

        // Propagate up
        PropagateAll( parent, dimension, topOfSubTreeStack, potentialRedundantSubRoots );
      }
    }

    // Propagate down to children
    for( unsigned int i = 0, childCount = actor.GetChildCount(); i < childCount; ++i )
    {
      Dali::Actor child = actor.GetChildAt( i );
      Actor& childImpl = GetImplementation( child );

      if( childImpl.IsRelayoutEnabled() && childImpl.RelayoutDependentOnParent( dimension ) )
      {
        if( childImpl.IsLayoutDirty( dimension ) )
        {
          // We have found a child that could potentially have already been collected for relayout
          potentialRedundantSubRoots.push_back( child );
        }
        else
        {
          PropagateAll( child, dimension, topOfSubTreeStack, potentialRedundantSubRoots );
        }
      }
    }
  }
}


void RelayoutController::PropagateFlags( Dali::Actor& actor, Dimension::Type dimension )
{
  // Only set dirty flag if doing relayout and not already marked as dirty
  Actor& actorImpl = GetImplementation( actor );
  if( actorImpl.IsRelayoutEnabled() )
  {
    // Set dirty and negotiated flags
    actorImpl.SetLayoutDirty( true, dimension );
    actorImpl.SetLayoutNegotiated( false, dimension );    // Reset this flag ready for next relayout

    // Check for dimension dependecy: width for height/height for width etc
    // Check each possible dimension and see if it is dependent on the input one
    for( uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i )
    {
      Dimension::Type dimensionToCheck = static_cast< Dimension::Type >( 1 << i );

      if( actorImpl.RelayoutDependentOnDimension( dimension, dimensionToCheck ) )
      {
        PropagateFlags( actor, dimensionToCheck );
      }
    }

    // Propagate up to parent
    Dali::Actor parent = actor.GetParent();
    if( parent )
    {
      Actor& parentImpl = GetImplementation( parent );
      if( parentImpl.RelayoutDependentOnChildren( dimension ) )
      {
        // Propagate up
        PropagateFlags( parent, dimension );
      }
    }

    // Propagate down to children
    for( uint32_t i = 0, childCount = actor.GetChildCount(); i < childCount; ++i )
    {
      Dali::Actor child = actor.GetChildAt( i );
      Actor& childImpl = GetImplementation( child );

      if( childImpl.RelayoutDependentOnParent( dimension ) )
      {
        PropagateFlags( child, dimension );
      }
    }
  }
}

void RelayoutController::AddRequest( Dali::Actor& actor )
{
  Internal::Actor* actorPtr = &GetImplementation( actor );

  // Only add the rootActor if it is not already recorded
  bool found = false;
  for( auto&& item : mDirtyLayoutSubTrees )
  {
    if( item == actorPtr )
    {
      found = true;
      break;
    }
  }

  if( !found )
  {
    mDirtyLayoutSubTrees.PushBack( actorPtr );
  }
}

void RelayoutController::RemoveRequest( Dali::Actor& actor )
{
  Internal::Actor* actorPtr = &GetImplementation( actor );

  // Remove actor from dirty sub trees
  for( RawActorList::Iterator it = mDirtyLayoutSubTrees.Begin(), itEnd = mDirtyLayoutSubTrees.End(); it != itEnd; ++it )
  {
    if( *it == actorPtr )
    {
      mDirtyLayoutSubTrees.Erase( it );
      break;
    }
  }
}

void RelayoutController::Request()
{
  mRelayoutFlag = true;

  if( !mRelayoutConnection )
  {
    ThreadLocalStorage::Get().GetObjectRegistry().ObjectDestroyedSignal().Connect( mSlotDelegate, &RelayoutController::OnObjectDestroyed );

    mRelayoutConnection = true;
  }
}

void RelayoutController::OnObjectDestroyed( const Dali::RefObject* object )
{
  // Search for and null the object if found in the following lists
  FindAndZero( mDirtyLayoutSubTrees, object );
}

void RelayoutController::Relayout()
{
  // Only do something when requested
  if( mRelayoutFlag )
  {
    mPerformingRelayout = true;

    // Clear the flag as we're now doing the relayout
    mRelayoutFlag = false;

    // 1. Finds all top-level controls from the dirty list and allocate them the size of the stage
    //    These controls are paired with the parent/stage size and added to the stack.
    for( RawActorList::Iterator it = mDirtyLayoutSubTrees.Begin(), itEnd = mDirtyLayoutSubTrees.End(); it != itEnd; ++it )
    {
      Internal::Actor* dirtyActor = *it;

      // Need to test if actor is valid (could have been deleted and had the pointer cleared)
      if( dirtyActor )
      {
        // Only negotiate actors that are on stage
        if( dirtyActor->OnStage() )
        {
          Internal::Actor* parent = dirtyActor->GetParent();
          QueueActor( dirtyActor, *mRelayoutStack, ( parent ) ? Vector2( parent->GetTargetSize() ) : dirtyActor->GetScene().GetSize() );
        }
      }
    }

    mDirtyLayoutSubTrees.Clear();

    // 2. Iterate through the stack until it's empty.
    if( mRelayoutStack->Size() > 0 )
    {
      PRINT_HIERARCHY;

      while( mRelayoutStack->Size() > 0 )
      {
        Dali::Actor actor;
        Vector2 size;
        mRelayoutStack->Get( mRelayoutStack->Size() - 1, actor, size );
        Actor& actorImpl = GetImplementation( actor );
        mRelayoutStack->PopBack();

        if( actorImpl.RelayoutRequired() )
        {
          DALI_LOG_INFO( gLogFilter, Debug::General, "[Internal::RelayoutController::Relayout] Negotiating %p %s %s (%.2f, %.2f)\n", &actorImpl, actor.GetTypeName().c_str(), actor.GetName().c_str(), size.width, size.height );

          // 3. Negotiate the size with the current actor. Pass it an empty container which the actor
          //    has to fill with all the actors it has not done any size negotiation for.

          actorImpl.NegotiateSize( size, *mRelayoutStack );

          // Reset the flag so that size negotiation will respect the actor's original resize policy
          actorImpl.SetUseAssignedSize( false );
        }
      }

      // We are done with the RelayoutInfos now so delete the pool
      mRelayoutInfoAllocator.ResetMemoryPool();

      PRINT_HIERARCHY;
    }

    mPerformingRelayout = false;
  }
  // should not disconnect the signal as that causes some control size negotiations to not work correctly
  // this algorithm needs more optimization as well
}

void RelayoutController::SetEnabled( bool enabled )
{
  mEnabled = enabled;
}

bool RelayoutController::IsPerformingRelayout() const
{
  return mPerformingRelayout;
}

void RelayoutController::SetProcessingCoreEvents( bool processingEvents )
{
  mProcessingCoreEvents = processingEvents;
}

void RelayoutController::FindAndZero( const RawActorList& list, const Dali::RefObject* object )
{
  // Object has been destroyed so clear it from this list
  for( RawActorList::Iterator it = list.Begin(), itEnd = list.End(); it != itEnd; ++it )
  {
    BaseObject* actor = *it;

    if( actor && ( actor == object ) )
    {
      *it = NULL;    // Reset the pointer in the list. We don't want to remove it in case something is iterating over the list.
    }
  }
}

} // namespace Internal

} // namespace Dali
