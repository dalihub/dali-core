#ifndef DALI_INTERNAL_ACTOR_SIBLINGS_H
#define DALI_INTERNAL_ACTOR_SIBLINGS_H
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use actor file except in compliance with the License.
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

namespace Dali
{
namespace Internal
{

struct Actor::SiblingHandler
{
  /**
   * @brief Change the sibling order of the given actor.
   *
   * @param[in,out] siblings The container of siblings
   * @param[in] actor The actor to change
   * @param[in] order The new order for the actor
   * @return true if order has been modified
   */
  static bool SetSiblingOrder( ActorContainer& siblings, Actor& actor, uint32_t order );

  /**
   * @brief Get the sibling order of the given actor.
   *
   * @param[in] siblings The container of siblings
   * @param[in] actor The actor to query
   * @return the order in the sibling array of the actor
   */
  static uint32_t GetSiblingOrder(const ActorContainer& siblings, const Actor& actor);

  /**
   * @brief Raise the actor within the siblings list by one
   * @param[in,out] siblings The container of siblings
   * @param[in] actor The actor to move
   * @return true if order has been modified
   */
  static bool Raise(ActorContainer& siblings, Actor& actor);

  /**
   * @brief Lower the actor within the siblings list by one
   * @param[in,out] siblings The container of siblings
   * @param[in] actor The actor to move
   * @return true if order has been modified
   */
  static bool Lower(ActorContainer& siblings, Actor& actor);

  /**
   * @brief Raise the actor to the top of the siblings list.
   *
   * @param[in,out] siblings The container of siblings
   * @param[in] actor The actor to move
   * @return true if order has been modified
   */
  static bool RaiseToTop(ActorContainer& siblings, Actor& actor);

  /**
   * @brief Lower the actor to the bottom of the siblings list.
   *
   * @param[in,out] siblings The container of siblings
   * @param[in] actor The actor to move
   * @return true if order has been modified
   */
  static bool LowerToBottom(ActorContainer& siblings, Actor& actor);

  /**
   * @brief Raise the actor above the target actor within the siblings list.
   *
   * @param[in,out] siblings The container of siblings
   * @param[in] actor The actor to move
   * @param[in] target The target actor
   * @return true if order has been modified
   */
  static bool RaiseAbove(ActorContainer& siblings, Actor& actor, Actor& target);

  /**
   * @brief Lower the actor below the target actor within the siblings list.
   *
   * @param[in,out] siblings The container of siblings
   * @param[in] actor The actor to move
   * @param[in] target The target actor
   * @return true if order has been modified
   */
  static bool LowerBelow(ActorContainer& siblings, Actor& actor, Actor& target);
};

} // namespace Internal

} // namespace Dali


#endif
