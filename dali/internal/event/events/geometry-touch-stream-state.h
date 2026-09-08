#ifndef DALI_INTERNAL_GEOMETRY_TOUCH_STREAM_STATE_H
#define DALI_INTERNAL_GEOMETRY_TOUCH_STREAM_STATE_H

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

#include <dali/internal/event/actors/actor-declarations.h>

#include <cstdint>
#include <list>
#include <vector>

namespace DALI_NAMESPACE::Internal
{
enum class GeometryTouchStreamPhase : uint8_t
{
  UNOWNED,
  OWNED,
  FINISHING,
  FINISHED,
};

enum class GeometryTouchRecipientState : uint8_t
{
  ACTIVE,
  TERMINATED,
};

struct GeometryTouchRecipient
{
  ActorPtr                    actor;
  GeometryTouchRecipientState state{GeometryTouchRecipientState::ACTIVE};
};

/**
 * Holds the ordered geometry state whose lifetime belongs to one routed stream.
 */
struct GeometryTouchStreamState
{
  GeometryTouchStreamPhase            phase{GeometryTouchStreamPhase::UNOWNED};
  ActorPtr                            initialHitActor;
  std::list<ActorPtr>                 initialHitPathRootToTarget;
  std::list<ActorPtr>                 ownerPathRootToOwner;
  std::list<ActorPtr>                 interceptedRootToTarget;
  std::list<ActorPtr>                 candidatesRootToFront;
  std::vector<GeometryTouchRecipient> recipientsInDispatchOrder;
};

} //namespace DALI_NAMESPACE::Internal

#endif // DALI_INTERNAL_GEOMETRY_TOUCH_STREAM_STATE_H
