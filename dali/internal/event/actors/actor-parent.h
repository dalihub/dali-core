#ifndef DALI_INTERNAL_ACTOR_PARENT_H
#define DALI_INTERNAL_ACTOR_PARENT_H
/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/internal/common/const-string.h>
#include <dali/internal/event/actors/actor-declarations.h>

namespace Dali
{
namespace Internal
{
/**
 * Interface that enables parenting of actors and managing child sibling order.
 */
class ActorParent
{
protected:
  /**
   * Constructor, not to be directly instantiated
   */
  ActorParent() = default;

  /**
   * Virtual destructor. No deletion through this interface.
   */
  virtual ~ActorParent() = default;

public:
  /**
   * Adds a child Actor to this Actor.
   * @pre The child actor is not the same as the parent actor.
   * @pre The child actor does not already have a parent.
   * @param [in] child The child.
   * @param [in] notify Emits notification if set to true. Default is true.
   * @post The child will be referenced by its parent.
   */
  virtual void Add(Actor& child, bool notify = true) = 0;

  /**
   * Removes a child Actor from this Actor.
   * @param [in] child The child.
   * @param [in] notify Emits notification if set to true. Default is true.
   * @post The child will be unreferenced.
   * @note If notify is false, Add() method must be called after this method.
   */
  virtual void Remove(Actor& child, bool notify = true) = 0;

  /**
   * Retrieve the number of children held by the actor.
   * @return The number of children
   */
  virtual uint32_t GetChildCount() const = 0;

  /**
   * @copydoc Dali::Actor::GetChildAt
   */
  virtual ActorPtr GetChildAt(uint32_t index) const = 0;

  /**
   * @copydoc Dali::Actor::FindChildByName
   */
  virtual ActorPtr FindChildByName(const std::string_view& actorName) = 0;

  /**
   * @copydoc Dali::Actor::FindChildById
   */
  virtual ActorPtr FindChildById(const uint32_t id) = 0;

  /**
   * @brief Unparent all the children
   */
  virtual void UnparentChildren() = 0;

  /**
   * @brief Change the sibling order of the given child.
   *
   * @param[in] child The actor to change
   * @param[in] order The new order for the actor
   * @return true if order has been modified
   */
  virtual void SetSiblingOrderOfChild(Actor& child, uint32_t order) = 0;

  /**
   * @brief Get the sibling order of the given actor.
   *
   * @param[in] child The actor to query
   * @return the order in the sibling array of the actor
   */
  virtual uint32_t GetSiblingOrderOfChild(const Actor& child) const = 0;

  /**
   * @brief Raise the actor within the siblings list by one

   * @param[in] child The actor to move
   * @return true if order has been modified
   */
  virtual void RaiseChild(Actor& child) = 0;

  /**
   * @brief Lower the actor within the siblings list by one

   * @param[in] child The actor to move
   * @return true if order has been modified
   */
  virtual void LowerChild(Actor& child) = 0;

  /**
   * @brief Raise the actor to the top of the siblings list.
   *
   * @param[in] child The actor to move
   * @return true if order has been modified
   */
  virtual void RaiseChildToTop(Actor& child) = 0;

  /**
   * @brief Lower the actor to the bottom of the siblings list.
   *
   * @param[in] child The actor to move
   * @return true if order has been modified
   */
  virtual void LowerChildToBottom(Actor& child) = 0;

  /**
   * @brief Raise the actor above the target actor within the siblings list.
   *
   * @param[in] child The actor to move
   * @param[in] target The target actor
   * @return true if order has been modified
   */
  virtual void RaiseChildAbove(Actor& child, Actor& target) = 0;

  /**
   * @brief Lower the actor below the target actor within the siblings list.
   *
   * @param[in] child The actor to move
   * @param[in] target The target actor
   * @return true if order has been modified
   */
  virtual void LowerChildBelow(Actor& child, Actor& target) = 0;
};

} // namespace Internal

} // namespace Dali

#endif
