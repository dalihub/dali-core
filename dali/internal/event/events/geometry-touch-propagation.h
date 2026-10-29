#ifndef DALI_INTERNAL_GEOMETRY_TOUCH_PROPAGATION_H
#define DALI_INTERNAL_GEOMETRY_TOUCH_PROPAGATION_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <algorithm>
#include <iterator>
#include <list>
#include <vector>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/actor-impl.h>

namespace DALI_NAMESPACE::Internal
{
/**
 * @brief Captures boundary actors before the first event can change their properties or scene membership.
 * @param[in] candidates The complete initial geometry hit candidates
 * @return Boundaries retained for the lifetime of this stream, including possible intercepted routes
 */
inline std::vector<ActorPtr> CaptureTouchPropagationBoundaries(const std::list<ActorPtr>& candidates)
{
  std::vector<ActorPtr> boundaries;
  for(const auto& actor : candidates)
  {
    if(actor->IsTouchPropagationBlocked() && std::find(boundaries.begin(), boundaries.end(), actor) == boundaries.end())
    {
      boundaries.push_back(actor);
    }
  }
  return boundaries;
}

/**
 * @brief Keeps ordinary touch candidates up to and including the first captured boundary in dispatch order.
 *
 * This changes neither consumption nor the recipients of cancellation and intercept notifications.
 * @param[in,out] candidates Candidates ordered from back to front
 * @param[in] boundaries The stream's initial boundary snapshot
 * @param[in] initialCandidates Optional initial scope, used when an in-scope interceptor has no boundary ancestor
 * @return The captured boundary limiting this route, or an empty pointer if no boundary applies
 */
inline ActorPtr LimitTouchPropagation(std::list<ActorPtr>&         candidates,
                                      const std::vector<ActorPtr>& boundaries,
                                      const std::list<ActorPtr>*   initialCandidates = nullptr)
{
  if(boundaries.empty())
  {
    return ActorPtr();
  }

  const bool inInitialScope = initialCandidates && !candidates.empty() &&
                              std::find(initialCandidates->begin(), initialCandidates->end(), candidates.back()) != initialCandidates->end();
  for(auto candidate = candidates.end(); candidate != candidates.begin();)
  {
    --candidate;
    if(std::find(boundaries.begin(), boundaries.end(), *candidate) != boundaries.end() &&
       (!inInitialScope || std::find(initialCandidates->begin(), initialCandidates->end(), *candidate) != initialCandidates->end()))
    {
      candidates.erase(candidates.begin(), candidate);
      return *candidate;
    }
  }

  // A front sibling can intercept without having the geometry boundary on its ancestor path.
  // Its ordinary route must not reopen the excluded background, even if that background
  // has another boundary. An outside ancestor that explicitly intercepts is allowed to
  // establish its own route, as before.
  if(inInitialScope)
  {
    for(auto candidate = candidates.end(); candidate != candidates.begin();)
    {
      --candidate;
      if(std::find(initialCandidates->begin(), initialCandidates->end(), *candidate) == initialCandidates->end())
      {
        candidates.erase(candidates.begin(), std::next(candidate));
        return initialCandidates->front();
      }
    }
  }
  return ActorPtr();
}
} // namespace DALI_NAMESPACE::Internal

#endif // DALI_INTERNAL_GEOMETRY_TOUCH_PROPAGATION_H
