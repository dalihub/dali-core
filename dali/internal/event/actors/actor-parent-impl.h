#ifndef DALI_INTERNAL_ACTOR_PARENT_IMPL_H
#define DALI_INTERNAL_ACTOR_PARENT_IMPL_H
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/actor-parent.h>

// EXTERNAL INCLUDES
#include <string>

namespace Dali
{
namespace Internal
{
class Actor;

class ActorParentImpl : public ActorParent
{
public:
  /**
   * constructor
   * @param[in] owner The owning actor
   */
  ActorParentImpl(Actor& owner);

  /**
   * destructor
   */
  virtual ~ActorParentImpl();

  /**
   * Adds a child Actor to this ActorParent.
   * @pre The child actor is not the same as the parent actor.
   * @pre The child actor does not already have a parent.
   * @param [in] child The child.
   * @post The child will be referenced by its parent.
   */
  void Add(Actor& child);

  /**
   * Removes a child Actor from this ActorParent.
   * @param [in] child The child.
   * @post The child will be unreferenced.
   */
  void Remove(Actor& child);

  /**
   * Retrieve the number of children held by the actor.
   * @return The number of children
   */
  uint32_t GetChildCount() const;

  /**
   * @copydoc Dali::Actor::GetChildAt
   */
  ActorPtr GetChildAt(uint32_t index) const;

  /**
   * @copydoc Dali::Actor::FindChildByName
   */
  ActorPtr FindChildByName(ConstString actorName) override;

  /**
   * @copydoc Dali::Actor::FindChildById
   */
  ActorPtr FindChildById(const uint32_t id);

  /**
   * Retrieve a reference to children.
   * @note Not for public use.
   * @return A reference to the container of children.
   * @note The internal container is lazily initialized so ensure you check the child count before using this method.
   */
  ActorContainer& GetChildrenInternal()
  {
    return *mChildren;
  }

  /**
   * @brief Unparent all the children
   */
  void UnparentChildren();

  /**
   * @brief Change the sibling order of the given child.
   *
   * @param[in] child The actor to change
   * @param[in] order The new order for the actor
   * @return true if order has been modified
   */
  void SetSiblingOrderOfChild(Actor& child, uint32_t order) override;

  /**
   * @brief Get the sibling order of the given actor.
   *
   * @param[in] child The actor to query
   * @return the order in the sibling array of the actor
   */
  uint32_t GetSiblingOrderOfChild(const Actor& child) const override;

  /**
   * @brief Raise the actor within the siblings list by one

   * @param[in] child The actor to move
   * @return true if order has been modified
   */
  void RaiseChild(Actor& child) override;

  /**
   * @brief Lower the actor within the siblings list by one

   * @param[in] child The actor to move
   * @return true if order has been modified
   */
  void LowerChild(Actor& child) override;

  /**
   * @brief Raise the actor to the top of the siblings list.
   *
   * @param[in] child The actor to move
   * @return true if order has been modified
   */
  void RaiseChildToTop(Actor& child) override;

  /**
   * @brief Lower the actor to the bottom of the siblings list.
   *
   * @param[in] child The actor to move
   * @return true if order has been modified
   */
  void LowerChildToBottom(Actor& child) override;

  /**
   * @brief Raise the actor above the target actor within the siblings list.
   *
   * @param[in] child The actor to move
   * @param[in] target The target actor
   * @return true if order has been modified
   */
  void RaiseChildAbove(Actor& child, Actor& target) override;

  /**
   * @brief Lower the actor below the target actor within the siblings list.
   *
   * @param[in] child The actor to move
   * @param[in] target The target actor
   * @return true if order has been modified
   */
  void LowerChildBelow(Actor& child, Actor& target) override;

  /**
   * @copydoc DevelActor::ChildAddedSignal
   */
  DevelActor::ChildChangedSignalType& ChildAddedSignal()
  {
    return mChildAddedSignal;
  }

  /**
   * @copydoc DevelActor::ChildRemovedSignal
   */
  DevelActor::ChildChangedSignalType& ChildRemovedSignal()
  {
    return mChildRemovedSignal;
  }

  /**
   * @copydoc DevelActor::ChildOrderChangedSignal
   */
  DevelActor::ChildOrderChangedSignalType& ChildOrderChangedSignal()
  {
    return mChildOrderChangedSignal;
  }

private:
  /**
   * @brief Emits the ChildAdded signal for this actor
   * @param[in] child The child actor that has been added
   */
  void EmitChildAddedSignal(Actor& child);

  /**
   * @brief Emits the ChildRemoved signal for this actor
   * @param[in] child The child actor that has been removed
   */
  void EmitChildRemovedSignal(Actor& child);

  /**
   * @brief Emit the child order changed signal, and rebuild the depth tree
   *
   * @param[in] child The child actor that changed order
   */
  void EmitOrderChangedAndRebuild(Actor& child);

private:
  Dali::Internal::Actor&                  mOwner; ///* Owning actor
  DevelActor::ChildChangedSignalType      mChildAddedSignal;
  DevelActor::ChildChangedSignalType      mChildRemovedSignal;
  DevelActor::ChildOrderChangedSignalType mChildOrderChangedSignal;
  ActorContainer*                         mChildren{nullptr}; ///< Container of referenced actors, lazily initialized
};

} // namespace Internal

} // namespace Dali

#endif
