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
 *
 */

// CLASS HEADER
#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>

// EXTERNAL INCLUDES
#if defined(DEBUG_ENABLED)
#include <sstream>
#endif // defined(DEBUG_ENABLED)

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/render-controller.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);

#if defined(DEBUG_ENABLED)

Integration::Log::Filter* gLogFilter(Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_RELAYOUT_CONTROLLER"));

/**
 * Prints out all the children of the given actor when debug is enabled.
 *
 * @param[in]  actor  The actor whose children to print.
 * @param[in]  level  The number of " | " to put in front of the children.
 */
void PrintChildren(Dali::Actor actor, int level)
{
  std::ostringstream output;

  for(int t = 0; t < level; ++t)
  {
    output << " | ";
  }

  output << actor.GetTypeName();

  output << ", " << actor.GetProperty<std::string>(Dali::Actor::Property::NAME);

  output << " - Pos: " << actor.GetCurrentProperty<Vector3>(Dali::Actor::Property::POSITION) << " Size: " << actor.GetTargetSize();

  output << ", Dirty: (" << (GetImplementation(actor).IsLayoutDirty(Dimension::WIDTH) ? "TRUE" : "FALSE") << "," << (GetImplementation(actor).IsLayoutDirty(Dimension::HEIGHT) ? "TRUE" : "FALSE") << ")";
  output << ", Negotiated: (" << (GetImplementation(actor).IsLayoutNegotiated(Dimension::WIDTH) ? "TRUE" : "FALSE") << "," << (GetImplementation(actor).IsLayoutNegotiated(Dimension::HEIGHT) ? "TRUE" : "FALSE") << ")";
  output << ", Enabled: " << (GetImplementation(actor).IsRelayoutEnabled() ? "TRUE" : "FALSE");

  output << ", (" << actor.GetObjectPtr() << ")" << std::endl;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, output.str().c_str());

  ++level;
  uint32_t numChildren = actor.GetChildCount();
  for(uint32_t i = 0; i < numChildren; ++i)
  {
    PrintChildren(actor.GetChildAt(i), level);
  }
  --level;
}

/**
 * Prints the entire hierarchy of the scene.
 */
void PrintHierarchy()
{
  if(gLogFilter->IsEnabledFor(Debug::Verbose))
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "---------- ROOT LAYER ----------\n");

    PrintChildren(Stage::GetCurrent()->GetRootLayer(), 0);
  }
}

#define PRINT_HIERARCHY PrintHierarchy()

#else // defined(DEBUG_ENABLED)

#define PRINT_HIERARCHY

#endif // defined(DEBUG_ENABLED)

} // unnamed namespace

RelayoutController::RelayoutController(Integration::RenderController& controller)
: mRenderController(controller),
  mRelayoutInfoAllocator(true /* Forcibly use memory pool */),
  mSlotDelegate(this),
  mRelayoutStack(new MemoryPoolRelayoutContainer(mRelayoutInfoAllocator)),
  mRelayoutConnection(false),
  mRelayoutFlag(false),
  mEnabled(false),
  mPerformingRelayout(false),
  mProcessingCoreEvents(false)
{
  // Make space for 32 controls to avoid having to copy construct a lot in the beginning
  mRelayoutStack->Reserve(32);
  mPotentialRedundantSubRoots.reserve(32);
  mTopOfSubTreeStack.reserve(32);
}

RelayoutController::~RelayoutController() = default;

RelayoutController* RelayoutController::Get()
{
  // There was crash when destroying actors and the ResizePolicy is USE_NATURAL_SIZE
  // The ThreadLocalStorage::Get() only retrieve STL without checking if it exists.
  // The caller of RelayoutController::Get() should check if RelayoutController is not null.
  if(ThreadLocalStorage::Created())
  {
    return &ThreadLocalStorage::Get().GetRelayoutController();
  }

  return nullptr;
}

void RelayoutController::QueueActor(Internal::Actor& actorImpl, RelayoutContainer& actors, Vector2 size)
{
  if(actorImpl.RelayoutRequired())
  {
    actors.Add(Dali::Actor(&actorImpl), size);
  }
}

void RelayoutController::RequestRelayout(Dali::Actor& actor, Dimension::Type dimension)
{
  if(!mEnabled)
  {
    return;
  }

  RawActorList& potentialRedundantSubRoots = mPotentialRedundantSubRoots;
  RawActorList& topOfSubTreeStack          = mTopOfSubTreeStack;

  DALI_ASSERT_ALWAYS(potentialRedundantSubRoots.empty() && "potentialRedundantSubRoots must be empty before RequestRelayout!");
  DALI_ASSERT_ALWAYS(topOfSubTreeStack.empty() && "topOfSubTreeStack must be empty before RequestRelayout!");

  {
    Dali::Internal::Actor& actorImpl = GetImplementation(actor);

    topOfSubTreeStack.push_back(&actorImpl);

    // Propagate on all dimensions
    for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
    {
      if(dimension & (1 << i))
      {
        // Do the propagation
        PropagateAll(actorImpl, static_cast<Dimension::Type>(1 << i), topOfSubTreeStack, potentialRedundantSubRoots);
      }
    }
  }

  while(!topOfSubTreeStack.empty())
  {
    // Request this actor as head of sub-tree if it is not dependent on a parent that is dirty
    Dali::Internal::Actor& subTreeActorImpl = *topOfSubTreeStack.back();
    topOfSubTreeStack.pop_back();

    Dali::Internal::Actor* parentImplPtr = subTreeActorImpl.GetParent();
    if(!parentImplPtr || !(subTreeActorImpl.RelayoutDependentOnParent() && (*parentImplPtr).RelayoutRequired()))
    {
      // Add sub tree root to relayout list
      AddRequest(subTreeActorImpl);

      // Flag request for end of frame
      Request();
    }
    else
    {
      potentialRedundantSubRoots.push_back(&subTreeActorImpl);
    }
  }

  // Remove any redundant sub-tree heads
  for(const auto& subRoot : potentialRedundantSubRoots)
  {
    RemoveRequest(*subRoot);
  }

  potentialRedundantSubRoots.clear();

  if(!mProcessingCoreEvents)
  {
    mRenderController.RequestProcessEventsOnIdle();
  }
}

void RelayoutController::OnApplicationSceneCreated()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[Internal::RelayoutController::OnApplicationSceneCreated]\n");

  // Open relayout controller to receive relayout requests
  mEnabled = true;

  // Flag request for end of frame
  Request();
}

void RelayoutController::RequestRelayoutRecursively(Dali::Internal::Actor& actorImpl)
{
  // Only set dirty flag if doing relayout and not already marked as dirty
  if(actorImpl.RelayoutPossible())
  {
    // If parent is not in relayout we are at the top of a new sub-tree
    Dali::Internal::Actor* parentImplPtr = actorImpl.GetParent();
    if(!parentImplPtr || !(*parentImplPtr).IsRelayoutEnabled())
    {
      AddRequest(actorImpl);
    }

    // Set dirty flag on actors that are enabled
    actorImpl.SetLayoutDirty(true);
    actorImpl.SetLayoutNegotiated(false); // Reset this flag ready for next relayout
  }

  // Propagate down to children
  if(actorImpl.GetChildCount())
  {
    // Get reference of children container, to avoid useless reference count changing
    auto& children = actorImpl.GetChildrenInternal();
    for(auto& childImplPtr : children)
    {
      RequestRelayoutRecursively(*childImplPtr);
    }
  }
}

void RelayoutController::PropagateAll(Dali::Internal::Actor& actorImpl, Dimension::Type dimension, RawActorList& topOfSubTreeStack, RawActorList& potentialRedundantSubRoots)
{
  // Only set dirty flag if doing relayout and not already marked as dirty
  if(actorImpl.RelayoutPossible(dimension))
  {
    // Set dirty and negotiated flags
    actorImpl.SetLayoutDirty(true, dimension);
    actorImpl.SetLayoutNegotiated(false, dimension); // Reset this flag ready for next relayout

    // Check for dimension dependecy: width for height/height for width etc
    // Check each possible dimension and see if it is dependent on the input one
    for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
    {
      Dimension::Type dimensionToCheck = static_cast<Dimension::Type>(1 << i);

      if(actorImpl.RelayoutDependentOnDimension(dimension, dimensionToCheck) &&
         !actorImpl.IsLayoutDirty(dimensionToCheck))
      {
        PropagateAll(actorImpl, dimensionToCheck, topOfSubTreeStack, potentialRedundantSubRoots);
      }
    }

    // Propagate up to parent
    {
      Dali::Internal::Actor* parentImplPtr = actorImpl.GetParent();
      if(parentImplPtr)
      {
        Actor& parentImpl = *parentImplPtr;
        if(parentImpl.RelayoutDependentOnChildren(dimension) && !parentImpl.IsLayoutDirty(dimension))
        {
          // Store the highest parent reached
          bool found = false;
          for(auto&& element : topOfSubTreeStack)
          {
            if(element == &parentImpl)
            {
              found = true;
              break;
            }
          }

          if(!found)
          {
            topOfSubTreeStack.push_back(&parentImpl);
          }

          // Propagate up
          PropagateAll(parentImpl, dimension, topOfSubTreeStack, potentialRedundantSubRoots);
        }
      }
    }

    // Propagate down to children
    if(actorImpl.GetChildCount())
    {
      // Get reference of children container, to avoid useless reference count changing
      auto& children = actorImpl.GetChildrenInternal();
      for(auto& childImplPtr : children)
      {
        Internal::Actor& childImpl = *childImplPtr;

        if(childImpl.IsRelayoutEnabled() && childImpl.RelayoutDependentOnParent(dimension))
        {
          if(childImpl.IsLayoutDirty(dimension))
          {
            // We have found a child that could potentially have already been collected for relayout
            potentialRedundantSubRoots.push_back(&childImpl);
          }
          else
          {
            PropagateAll(childImpl, dimension, topOfSubTreeStack, potentialRedundantSubRoots);
          }
        }
      }
    }
  }
}

void RelayoutController::RequestRelayoutTree(Dali::Actor& actor)
{
  if(!mEnabled)
  {
    return;
  }

  RequestRelayoutRecursively(GetImplementation(actor));
}

void RelayoutController::PropagateFlags(Dali::Actor& actor, Dimension::Type dimension)
{
  // Only set dirty flag if doing relayout and not already marked as dirty
  Actor& actorImpl = GetImplementation(actor);
  if(actorImpl.IsRelayoutEnabled())
  {
    // Set dirty and negotiated flags
    actorImpl.SetLayoutDirty(true, dimension);
    actorImpl.SetLayoutNegotiated(false, dimension); // Reset this flag ready for next relayout

    // Check for dimension dependecy: width for height/height for width etc
    // Check each possible dimension and see if it is dependent on the input one
    for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
    {
      Dimension::Type dimensionToCheck = static_cast<Dimension::Type>(1 << i);

      if(actorImpl.RelayoutDependentOnDimension(dimension, dimensionToCheck))
      {
        PropagateFlags(actor, dimensionToCheck);
      }
    }

    // Propagate up to parent
    Dali::Actor parent = actor.GetParent();
    if(parent)
    {
      Actor& parentImpl = GetImplementation(parent);
      if(parentImpl.RelayoutDependentOnChildren(dimension))
      {
        // Propagate up
        PropagateFlags(parent, dimension);
      }
    }

    // Propagate down to children
    for(uint32_t i = 0, childCount = actor.GetChildCount(); i < childCount; ++i)
    {
      Dali::Actor child     = actor.GetChildAt(i);
      Actor&      childImpl = GetImplementation(child);

      if(childImpl.RelayoutDependentOnParent(dimension))
      {
        PropagateFlags(child, dimension);
      }
    }
  }
}

void RelayoutController::AddRequest(Internal::Actor& actorImpl)
{
  // Only add the rootActor if it is not already recorded
  auto iter = mDirtyLayoutSubTrees.Find(&actorImpl);

  if(iter == mDirtyLayoutSubTrees.End())
  {
    mDirtyLayoutSubTrees.PushBack(&actorImpl);
  }
}

void RelayoutController::RemoveRequest(const Internal::Actor& actorImpl)
{
  mDirtyLayoutSubTrees.EraseObject(&actorImpl);
}

void RelayoutController::Request()
{
  mRelayoutFlag = true;

  if(!mRelayoutConnection)
  {
    ThreadLocalStorage::Get().GetObjectRegistry().ObjectDestroyedSignal().Connect(mSlotDelegate, &RelayoutController::OnObjectDestroyed);

    mRelayoutConnection = true;
  }
}

void RelayoutController::OnObjectDestroyed(const Dali::RefObject* object)
{
  mDirtyLayoutSubTrees.EraseObject(static_cast<const Dali::Internal::Actor*>(object));
}

void RelayoutController::Relayout()
{
  if(!mEnabled)
  {
    return;
  }

  // Only do something when requested
  if(mRelayoutFlag)
  {
    mPerformingRelayout = true;

    // Clear the flag as we're now doing the relayout
    mRelayoutFlag = false;

    // 1. Finds all top-level controls from the dirty list and allocate them the size of the scene
    //    These controls are paired with the parent/scene size and added to the stack.
    for(auto iter = mDirtyLayoutSubTrees.Begin(); iter != mDirtyLayoutSubTrees.End();)
    {
      Internal::Actor* dirtyActorImplPtr = *iter;
      // Need to test if actor is valid (could have been deleted and had the pointer cleared)
      if(dirtyActorImplPtr)
      {
        auto& dirtyActorImpl = *dirtyActorImplPtr;
        // Only negotiate actors that are on the scene
        if(dirtyActorImpl.OnScene())
        {
          Internal::Actor* parent = dirtyActorImpl.GetParent();
          QueueActor(dirtyActorImpl, *mRelayoutStack, (parent) ? Vector2(parent->GetTargetSize()) : dirtyActorImpl.GetScene().GetSize());
        }
      }
      iter = mDirtyLayoutSubTrees.Erase(iter);
    }

    // 2. Iterate through the stack until it's empty.
    if(mRelayoutStack->Size() > 0)
    {
      DALI_TRACE_BEGIN(gTraceFilter, "DALI_RELAYOUT");
      PRINT_HIERARCHY;
#ifdef TRACE_ENABLED
      uint32_t relayoutActorCount   = 0u;
      uint32_t negotiatedActorCount = 0u;
#endif

      while(mRelayoutStack->Size() > 0)
      {
#ifdef TRACE_ENABLED
        ++relayoutActorCount;
#endif
        Dali::Actor actor;
        Vector2     size;

        mRelayoutStack->PopBack(actor, size);

        Internal::Actor& actorImpl = GetImplementation(actor);

        if(actorImpl.RelayoutRequired() && actorImpl.OnScene())
        {
#ifdef TRACE_ENABLED
          ++negotiatedActorCount;
#endif
          DALI_LOG_INFO(gLogFilter, Debug::General, "[Internal::RelayoutController::Relayout] Negotiating %p %s %s (%.2f, %.2f)\n", &actorImpl, actorImpl.GetTypeName().c_str(), std::string(actorImpl.GetName()).c_str(), size.width, size.height);

          // 3. Negotiate the size with the current actor. Pass it an empty container which the actor
          //    has to fill with all the actors it has not done any size negotiation for.
          actorImpl.NegotiateSize(size, *mRelayoutStack);
        }
      }

      // We are done with the RelayoutInfos now so delete the pool
      mRelayoutInfoAllocator.ResetMemoryPool();

      PRINT_HIERARCHY;

#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        std::ostringstream stream;
        stream << "[relayoutActor:" << relayoutActorCount << " negotiatedActor:" << negotiatedActorCount << "]";
        DALI_TRACE_END_WITH_MESSAGE(gTraceFilter, "DALI_RELAYOUT", stream.str().c_str());
      }
#endif
    }

    mPerformingRelayout = false;
  }
  // should not disconnect the signal as that causes some control size negotiations to not work correctly
  // this algorithm needs more optimization as well
}

void RelayoutController::SetEnabled(bool enabled)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[Internal::RelayoutController::SetEnabled(%d)]\n", enabled);

  mEnabled = enabled;
}

bool RelayoutController::IsPerformingRelayout() const
{
  return mPerformingRelayout;
}

void RelayoutController::SetProcessingCoreEvents(bool processingEvents)
{
  mProcessingCoreEvents = processingEvents;
}

uint32_t RelayoutController::GetMemoryPoolCapacity()
{
  return mRelayoutInfoAllocator.GetCapacity();
}

} // namespace Internal

} // namespace Dali
