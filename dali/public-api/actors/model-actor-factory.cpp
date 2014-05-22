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

// CLASS HEADER
#include <dali/public-api/actors/model-actor-factory.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/modeling/model.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/modeling/model-impl.h>
#include <dali/internal/event/actors/model-actor-factory-impl.h>


namespace Dali
{

Actor ModelActorFactory::BuildActorTree(Model& model, const std::string& entityName)
{
  Internal::ActorPtr actor = Internal::ModelActorFactory::BuildActorTree(model, entityName);
  return Actor(actor.Get());
}

Animation ModelActorFactory::BuildAnimation(Model& model, Actor rootActor, size_t index)
{
  Internal::Model& modelImpl = GetImplementation(model);
  Internal::Actor& actorImpl = GetImplementation(rootActor);
  Internal::AnimationPtr internal = Internal::ModelActorFactory::BuildAnimation(modelImpl, actorImpl, index);
  return Animation(internal.Get());
}

Animation ModelActorFactory::BuildAnimation(Model& model, Actor rootActor, size_t index, float durationSeconds)
{
  Internal::Model& modelImpl = GetImplementation(model);
  Internal::Actor& actorImpl = GetImplementation(rootActor);
  Internal::AnimationPtr internal = Internal::ModelActorFactory::BuildAnimation(modelImpl, actorImpl, index, durationSeconds);
  return Animation(internal.Get());
}

Animation ModelActorFactory::BuildAnimation(Model& model, Actor rootActor, size_t index, AlphaFunction alpha)
{
  Internal::Model& modelImpl = GetImplementation(model);
  Internal::Actor& actorImpl = GetImplementation(rootActor);
  Internal::AnimationPtr internal = Internal::ModelActorFactory::BuildAnimation(modelImpl, actorImpl, index, alpha);
  return Animation(internal.Get());
}

Animation ModelActorFactory::BuildAnimation(Model& model, Actor rootActor, size_t index, AlphaFunction alpha, float durationSeconds)
{
  Internal::Model& modelImpl = GetImplementation(model);
  Internal::Actor& actorImpl = GetImplementation(rootActor);
  Internal::AnimationPtr internal = Internal::ModelActorFactory::BuildAnimation(modelImpl, actorImpl, index, alpha, durationSeconds);
  return Animation(internal.Get());
}


} // namespace Dali
