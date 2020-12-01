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

// INTERNAL INCLUDES
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/actor-siblings.h>

// EXTERNAL INCLUDES
#include <algorithm>

namespace Dali
{
namespace Internal
{

bool Actor::SiblingHandler::SetSiblingOrder(
  ActorContainer& siblings,
  Actor& actor,
  uint32_t order )
{
  bool changed = false;

  uint32_t currentOrder = GetSiblingOrder(siblings, actor);
  if( order != currentOrder )
  {
    if( order == 0 )
    {
      changed = LowerToBottom(siblings, actor);
    }
    else if( order < siblings.size() -1 )
    {
      if( order > currentOrder )
      {
        changed = RaiseAbove( siblings, actor, *siblings[order] );
      }
      else
      {
        changed = LowerBelow( siblings, actor, *siblings[order] );
      }
    }
    else
    {
      changed = RaiseToTop(siblings, actor);
    }
  }
  return changed;
}

uint32_t Actor::SiblingHandler::GetSiblingOrder(const ActorContainer& siblings, const Actor& actor)
{
  uint32_t order=0;
  for( std::size_t i = 0; i < siblings.size(); ++i )
  {
    if( siblings[i] == &actor )
    {
      order = static_cast<uint32_t>( i );
      break;
    }
  }
  return order;
}

bool Actor::SiblingHandler::Raise(ActorContainer& siblings, Actor& actor)
{
  bool changed = false;
  if( siblings.back() != &actor ) // If not already at end
  {
    for( std::size_t i=0; i<siblings.size(); ++i )
    {
      if( siblings[i] == &actor )
      {
        // Swap with next
        ActorPtr next = siblings[i+1];
        siblings[i+1] = &actor;
        siblings[i] = next;
        changed = true;
        break;
      }
    }
  }
  return changed;
}

bool Actor::SiblingHandler::Lower(ActorContainer& siblings, Actor& actor)
{
  bool changed = false;
  if( siblings.front() != &actor ) // If not already at beginning
  {
    for( std::size_t i=1; i<siblings.size(); ++i )
    {
      if( siblings[i] == &actor )
      {
        // Swap with previous
        ActorPtr previous = siblings[i-1];
        siblings[i-1] = &actor;
        siblings[i] = previous;
        changed = true;
        break;
      }
    }
  }
  return changed;
}

bool Actor::SiblingHandler::RaiseToTop(ActorContainer& siblings, Actor& actor)
{
  bool changed = false;
  if( siblings.back() != &actor ) // If not already at end
  {
    auto iter = std::find( siblings.begin(), siblings.end(), &actor );
    if( iter != siblings.end() )
    {
      siblings.erase(iter);
      siblings.push_back(ActorPtr(&actor));
      changed = true;
    }
  }
  return changed;
}

bool Actor::SiblingHandler::LowerToBottom(ActorContainer& siblings, Actor& actor)
{
  bool changed = false;
  if( siblings.front() != &actor ) // If not already at bottom,
  {
    ActorPtr actorPtr(&actor); // ensure actor actor remains referenced.

    auto iter = std::find( siblings.begin(), siblings.end(), &actor );
    if( iter != siblings.end() )
    {
      siblings.erase(iter);
      siblings.insert(siblings.begin(), actorPtr);
      changed = true;
    }
  }
  return changed;
}

bool Actor::SiblingHandler::RaiseAbove(ActorContainer& siblings, Actor& actor, Actor& target)
{
  bool raised = false;
  if( siblings.back() != &actor && target.GetParent() == actor.GetParent() ) // If not already at top
  {
    ActorPtr actorPtr(&actor); // ensure actor actor remains referenced.

    auto targetIter = std::find( siblings.begin(), siblings.end(), &target );
    auto actorIter   = std::find( siblings.begin(), siblings.end(), &actor );
    if( actorIter < targetIter )
    {
      siblings.erase(actorIter);
      // Erasing early invalidates the targetIter. (Conversely, inserting first may also
      // invalidate actorIter)
      targetIter = std::find( siblings.begin(), siblings.end(), &target );
      ++targetIter;
      siblings.insert(targetIter, actorPtr);
    }
    raised = true;
  }
  return raised;
}

bool Actor::SiblingHandler::LowerBelow(ActorContainer& siblings, Actor& actor, Actor& target)
{
  bool lowered = false;

  // If not already at bottom
  if( siblings.front() != &actor && target.GetParent() == actor.GetParent() )
  {
    ActorPtr actorPtr(&actor); // ensure actor actor remains referenced.

    auto targetIter = std::find( siblings.begin(), siblings.end(), &target );
    auto actorIter   = std::find( siblings.begin(), siblings.end(), &actor );

    if( actorIter > targetIter )
    {
      siblings.erase(actorIter); // actor only invalidates iterators at or after actor point.
      siblings.insert(targetIter, actorPtr);
    }
    lowered = true;
  }
  return lowered;
}


} // namespace Internal

} // namespace Dali
