/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/event/actors/actor-parent.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/public-api/common/vector-wrapper.h>

// EXTERNAL INCLUDES
#include <algorithm>

namespace Dali
{
namespace Internal
{
namespace
{
/// Helper for emitting signals with multiple parameters
template<typename Signal, typename... Param>
void EmitSignal(Actor& actor, Signal& signal, Param... params)
{
  if(!signal.Empty())
  {
    Dali::Actor handle(&actor);
    signal.Emit(handle, params...);
  }
}

} //anonymous namespace

ActorParentImpl::ActorParentImpl(Actor& owner)
: mOwner(owner),
  mChildAddedSignal(),
  mChildRemovedSignal(),
  mChildOrderChangedSignal()
{
}

ActorParentImpl::~ActorParentImpl()
{
  delete mChildren;
}

void ActorParentImpl::Add(Actor& child)
{
  DALI_ASSERT_ALWAYS(&mOwner != &child && "Cannot add actor to itself");
  DALI_ASSERT_ALWAYS(!child.IsRoot() && "Cannot add root actor");

  if(!mChildren)
  {
    mChildren = new ActorContainer;
  }

  Actor* oldParent = child.GetParent();

  // child might already be ours
  if(&mOwner != oldParent)
  {
    // if we already have parent, unparent us first
    if(oldParent)
    {
      oldParent->Remove(child); // This causes OnChildRemove callback & ChildRemoved signal

      // Old parent may need to readjust to missing child
      if(oldParent->RelayoutDependentOnChildren())
      {
        oldParent->RelayoutRequest();
      }
    }

    // Guard against Add() during previous OnChildRemove callback
    if(!child.GetParent())
    {
      // Do this first, since user callbacks from within SetParent() may need to remove child
      mChildren->push_back(ActorPtr(&child));

      // SetParent asserts that child can be added
      child.SetParent(&mOwner);

      // Notification for derived classes
      mOwner.OnChildAdd(child);
      EmitChildAddedSignal(child);

      child.InheritLayoutDirectionRecursively(mOwner.GetLayoutDirection());

      // Only put in a relayout request if there is a suitable dependency
      if(mOwner.RelayoutDependentOnChildren())
      {
        mOwner.RelayoutRequest();
      }
    }
  }
}

void ActorParentImpl::Remove(Actor& child)
{
  if((&mOwner == &child) || (!mChildren))
  {
    // no children or removing itself
    return;
  }

  ActorPtr removed;

  // Find the child in mChildren, and unparent it
  ActorIter end = mChildren->end();
  for(ActorIter iter = mChildren->begin(); iter != end; ++iter)
  {
    ActorPtr actor = (*iter);

    if(actor.Get() == &child)
    {
      // Keep handle for OnChildRemove notification
      removed = actor;

      // Do this first, since user callbacks from within SetParent() may need to add the child
      mChildren->erase(iter);

      DALI_ASSERT_DEBUG(actor->GetParent() == &mOwner);
      actor->SetParent(nullptr);

      break;
    }
  }

  if(removed)
  {
    // Only put in a relayout request if there is a suitable dependency
    if(mOwner.RelayoutDependentOnChildren())
    {
      mOwner.RelayoutRequest();
    }
  }

  // Notification for derived classes
  mOwner.OnChildRemove(child);
  EmitChildRemovedSignal(child);
}

uint32_t ActorParentImpl::GetChildCount() const
{
  return (nullptr != mChildren) ? static_cast<uint32_t>(mChildren->size()) : 0; // only 4,294,967,295 children per actor
}

ActorPtr ActorParentImpl::GetChildAt(uint32_t index) const
{
  DALI_ASSERT_ALWAYS(index < GetChildCount());

  return ((mChildren) ? (*mChildren)[index] : ActorPtr());
}

ActorPtr ActorParentImpl::FindChildByName(const std::string& actorName)
{
  ActorPtr child = nullptr;
  if(actorName == mOwner.GetName())
  {
    child = &mOwner;
  }
  else if(mChildren)
  {
    for(const auto& actor : *mChildren)
    {
      child = actor->FindChildByName(actorName);

      if(child)
      {
        break;
      }
    }
  }
  return child;
}

ActorPtr ActorParentImpl::FindChildById(const uint32_t id)
{
  ActorPtr child = nullptr;
  if(id == mOwner.GetId())
  {
    child = &mOwner;
  }
  else if(mChildren)
  {
    for(const auto& actor : *mChildren)
    {
      child = actor->FindChildById(id);

      if(child)
      {
        break;
      }
    }
  }
  return child;
}

void ActorParentImpl::UnparentChildren()
{
  if(mChildren)
  {
    for(const auto& child : *mChildren)
    {
      child->SetParent(nullptr);
    }
  }
}

void ActorParentImpl::SetSiblingOrderOfChild(
  Actor&   child,
  uint32_t order)
{
  if(mChildren)
  {
    uint32_t currentOrder = GetSiblingOrderOfChild(child);
    if(order != currentOrder)
    {
      if(order == 0)
      {
        LowerChildToBottom(child);
      }
      else if(order < mChildren->size() - 1)
      {
        if(order > currentOrder)
        {
          RaiseChildAbove(child, *((*mChildren)[order]));
        }
        else
        {
          LowerChildBelow(child, *((*mChildren)[order]));
        }
      }
      else
      {
        RaiseChildToTop(child);
      }
    }
  }
}

uint32_t ActorParentImpl::GetSiblingOrderOfChild(const Actor& child) const
{
  uint32_t order = 0;
  if(mChildren)
  {
    for(std::size_t i = 0; i < mChildren->size(); ++i)
    {
      if((*mChildren)[i] == &child)
      {
        order = static_cast<uint32_t>(i);
        break;
      }
    }
  }
  return order;
}

void ActorParentImpl::RaiseChild(Actor& child)
{
  bool changed = false;
  if(mChildren && mChildren->back() != &child) // If not already at end
  {
    for(std::size_t i = 0; i < mChildren->size(); ++i)
    {
      if((*mChildren)[i] == &child)
      {
        // Swap with next
        ActorPtr next       = (*mChildren)[i + 1];
        (*mChildren)[i + 1] = &child;
        (*mChildren)[i]     = next;
        changed             = true;
        break;
      }
    }
  }
  if(changed)
  {
    EmitOrderChangedAndRebuild(child);
  }
}

void ActorParentImpl::LowerChild(Actor& child)
{
  bool changed = false;
  if(mChildren && mChildren->front() != &child) // If not already at beginning
  {
    for(std::size_t i = 1; i < mChildren->size(); ++i)
    {
      if((*mChildren)[i] == &child)
      {
        // Swap with previous
        ActorPtr previous   = (*mChildren)[i - 1];
        (*mChildren)[i - 1] = &child;
        (*mChildren)[i]     = previous;
        changed             = true;
        break;
      }
    }
  }
  if(changed)
  {
    EmitOrderChangedAndRebuild(child);
  }
}

void ActorParentImpl::RaiseChildToTop(Actor& child)
{
  bool changed = false;
  if(mChildren && mChildren->back() != &child) // If not already at end
  {
    auto iter = std::find(mChildren->begin(), mChildren->end(), &child);
    if(iter != mChildren->end())
    {
      mChildren->erase(iter);
      mChildren->push_back(ActorPtr(&child));
      changed = true;
    }
  }
  if(changed)
  {
    EmitOrderChangedAndRebuild(child);
  }
}

void ActorParentImpl::LowerChildToBottom(Actor& child)
{
  bool changed = false;
  if(mChildren && mChildren->front() != &child) // If not already at bottom,
  {
    ActorPtr childPtr(&child); // ensure actor remains referenced.

    auto iter = std::find(mChildren->begin(), mChildren->end(), &child);
    if(iter != mChildren->end())
    {
      mChildren->erase(iter);
      mChildren->insert(mChildren->begin(), childPtr);
      changed = true;
    }
  }
  if(changed)
  {
    EmitOrderChangedAndRebuild(child);
  }
}

void ActorParentImpl::RaiseChildAbove(Actor& child, Actor& target)
{
  bool raised = false;
  if(mChildren && mChildren->back() != &child && target.GetParent() == child.GetParent()) // If not already at top
  {
    ActorPtr childPtr(&child); // ensure actor actor remains referenced.

    auto targetIter = std::find(mChildren->begin(), mChildren->end(), &target);
    auto childIter  = std::find(mChildren->begin(), mChildren->end(), &child);
    if(childIter < targetIter)
    {
      mChildren->erase(childIter);
      // Erasing early invalidates the targetIter. (Conversely, inserting first may also
      // invalidate actorIter)
      targetIter = std::find(mChildren->begin(), mChildren->end(), &target);
      ++targetIter;
      mChildren->insert(targetIter, childPtr);
    }
    raised = true;
  }
  if(raised)
  {
    EmitOrderChangedAndRebuild(child);
  }
}

void ActorParentImpl::LowerChildBelow(Actor& child, Actor& target)
{
  bool lowered = false;

  // If not already at bottom
  if(mChildren && mChildren->front() != &child && target.GetParent() == child.GetParent())
  {
    ActorPtr childPtr(&child); // ensure actor actor remains referenced.

    auto targetIter = std::find(mChildren->begin(), mChildren->end(), &target);
    auto childIter  = std::find(mChildren->begin(), mChildren->end(), &child);

    if(childIter > targetIter)
    {
      mChildren->erase(childIter); // actor only invalidates iterators at or after actor point.
      mChildren->insert(targetIter, childPtr);
    }
    lowered = true;
  }
  if(lowered)
  {
    EmitOrderChangedAndRebuild(child);
  }
}

void ActorParentImpl::EmitChildAddedSignal(Actor& child)
{
  EmitSignal(child, mChildAddedSignal);
}

void ActorParentImpl::EmitChildRemovedSignal(Actor& child)
{
  EmitSignal(child, mChildRemovedSignal);
}

void ActorParentImpl::EmitOrderChangedAndRebuild(Actor& child)
{
  EmitSignal(child, mChildOrderChangedSignal);

  if(mOwner.OnScene())
  {
    mOwner.GetScene().RequestRebuildDepthTree();
  }
}

} // namespace Internal

} // namespace Dali
