#ifndef DALI_INTERNAL_ACTOR_DECLARATIONS_H
#define DALI_INTERNAL_ACTOR_DECLARATIONS_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
namespace Internal
{
class Actor;
class CameraActor;
class CustomActor;
class Layer;

using ActorPtr       = IntrusivePtr<Actor>;
using CameraActorPtr = IntrusivePtr<CameraActor>;
using CustomActorPtr = IntrusivePtr<CustomActor>;
using LayerPtr       = IntrusivePtr<Layer>;

using ActorContainer = std::vector<ActorPtr>;
using ActorIter      = ActorContainer::iterator;
using ActorConstIter = ActorContainer::const_iterator;

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_ACTOR_DECLARATIONS_H
