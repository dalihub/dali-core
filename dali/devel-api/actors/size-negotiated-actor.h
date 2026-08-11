#ifndef DALI_SIZE_NEGOTIATED_ACTOR_H
#define DALI_SIZE_NEGOTIATED_ACTOR_H

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

// INTERNAL INCLUDES
#include <dali/devel-api/actors/actor-enumerations-devel.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
class Actor;
struct Vector2;
struct Vector3;
class RelayoutContainer;

/**
 * @brief SizeNegotiatedActor is a mixin interface for actors that need size negotiation.
 *
 * Classes should inherit from BOTH CustomActorImpl and SizeNegotiatedActor to support size negotiation:
 * `class MyActor : public CustomActorImpl, public SizeNegotiatedActor { ... }`
 *
 * This provides the full set of size negotiation virtual methods.
 * Use this if your custom actor needs to participate in the size negotiation algorithm.
 * For simple actors that don't need size negotiation, just inherit from CustomActorImpl.
 */
class DALI_CORE_API SizeNegotiatedActor
{
public:
  /**
   * @brief Virtual destructor
   */
  virtual ~SizeNegotiatedActor() = default;

  /**
   * @brief Notification for deriving classes when relayout of the actor is required.
   *
   * Should be overridden by derived classes if they need to layout
   * actors differently after certain operations like add or remove
   * actors, resize or after changing specific properties.
   *
   * @param[in]      size       The allocated size
   * @param[in,out]  container  The control should add actors to this container that it is not able
   *                            to allocate a size for
   * @note  As this function is called from inside the size negotiation algorithm, you cannot
   * call RequestRelayout (the call would just be ignored).
   */
  virtual void OnRelayout(const Vector2& size, RelayoutContainer& container) = 0;

  /**
   * @brief Notification for deriving classes when resize policy is set.
   *
   * @param[in] policy The policy being set
   * @param[in] dimension The dimension the policy is being set for
   */
  virtual void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) = 0;

  /**
   * @brief Returns the natural size of the actor.
   *
   * @return The actor's natural size
   */
  virtual Vector3 GetNaturalSize() = 0;

  /**
   * @brief Calculates the size for a child.
   *
   * @param[in] child The child actor to calculate the size for
   * @param[in] dimension The dimension to calculate the size for. E.g. width or height
   * @return Return the calculated size for the given dimension
   */
  virtual float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension) = 0;

  /**
   * @brief This method is called during size negotiation when a height is required for a given width.
   *
   * Derived classes should override this if they wish to customize the height returned.
   *
   * @param[in] width Width to use
   * @return The height based on the width
   */
  virtual float GetHeightForWidth(float width) = 0;

  /**
   * @brief This method is called during size negotiation when a width is required for a given height.
   *
   * Derived classes should override this if they wish to customize the width returned.
   *
   * @param[in] height Height to use
   * @return The width based on the height
   */
  virtual float GetWidthForHeight(float height) = 0;

  /**
   * @brief Determines if this actor is dependent on its children for relayout.
   *
   * @param[in] dimension The dimension(s) to check for
   * @return Return if the actor is dependent on its children
   */
  virtual bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) = 0;

  /**
   * @brief Virtual method to notify deriving classes that relayout dependencies have been
   * met and the size for this object is about to be calculated for the given dimension.
   *
   * @param[in] dimension The dimension that is about to be calculated
   */
  virtual void OnCalculateRelayoutSize(Dimension::Type dimension) = 0;

  /**
   * @brief Virtual method to notify deriving classes that the size for a dimension
   * has just been negotiated.
   *
   * @param[in] size The new size for the given dimension
   * @param[in] dimension The dimension that was just negotiated
   */
  virtual void OnLayoutNegotiated(float size, Dimension::Type dimension) = 0;

protected: // Size negotiation helpers
  /**
   * @brief Requests a relayout, which means performing a size negotiation on this actor, its parent and children (and potentially whole scene).
   *
   * This method can also be called from a derived class every time it needs a different size.
   * At the end of event processing, the relayout process starts and
   * all controls which requested Relayout will have their sizes (re)negotiated.
   *
   * @note RelayoutRequest() can be called multiple times; the size negotiation is still
   * only performed once, i.e. there is no need to keep track of this in the calling side.
   */
  void RelayoutRequest();

  /**
   * @brief Provides the Actor implementation of GetHeightForWidth.
   * @param[in] width Width to use
   * @return The height based on the width
   */
  float GetHeightForWidthBase(float width);

  /**
   * @brief Provides the Actor implementation of GetWidthForHeight.
   * @param[in] height Height to use
   * @return The width based on the height
   */
  float GetWidthForHeightBase(float height);

  /**
   * @brief Calculates the size for a child using the base actor object.
   *
   * @param[in] child The child actor to calculate the size for
   * @param[in] dimension The dimension to calculate the size for. E.g. width or height
   * @return Return the calculated size for the given dimension. If more than one dimension is requested, just return the first one found
   */
  float CalculateChildSizeBase(const Dali::Actor& child, Dimension::Type dimension);

  /**
   * @brief Determines if this actor is dependent on its children for relayout from the base class.
   *
   * @param[in] dimension The dimension(s) to check for
   * @return Return if the actor is dependent on its children
   */
  bool RelayoutDependentOnChildrenBase(Dimension::Type dimension = Dimension::ALL_DIMENSIONS);
};

} // namespace Dali

#endif // DALI_SIZE_NEGOTIATED_ACTOR_H
