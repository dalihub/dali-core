#ifndef DALI_INTERNAL_ACTOR_SIZER_H
#define DALI_INTERNAL_ACTOR_SIZER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/size-negotiation/relayout-container.h>

namespace Dali::Internal
{
class Actor;
class Animation;

/**
 * Class to handle sizing of actor. Uses size negotiation and animation.
 */
class ActorSizer
{
public:
  struct AnimatedSizeFlag
  {
    enum Type
    {
      CLEAR  = 0,
      WIDTH  = 1,
      HEIGHT = 2,
      DEPTH  = 4
    };
  };

  /**
   * @brief Struct to hold an actor and a dimension
   */
  struct ActorDimensionPair
  {
    /**
     * @brief Constructor
     *
     * @param[in] newActor The actor to assign
     * @param[in] newDimension The dimension to assign
     */
    ActorDimensionPair(Actor* newActor, Dimension::Type newDimension)
    : actor(newActor),
      dimension(newDimension)
    {
    }

    /**
     * @brief Equality operator
     *
     * @param[in] lhs The left hand side argument
     * @param[in] rhs The right hand side argument
     */
    bool operator==(const ActorDimensionPair& rhs)
    {
      return (actor == rhs.actor) && (dimension == rhs.dimension);
    }

    Actor*          actor;     ///< The actor to hold
    Dimension::Type dimension; ///< The dimension to hold
  };

  using ActorDimensionStack = std::vector<ActorDimensionPair>;

public:
  /// Constructor
  ActorSizer(Internal::Actor& owner);

  /// Destructor
  ~ActorSizer();

  // Remove default constructor, copy constructor and assignment operator
  ActorSizer()                  = delete;
  ActorSizer(const ActorSizer&) = delete;
  ActorSizer& operator=(const ActorSizer) = delete;

  ///@copydoc Actor::SetSizerModeFactor
  void SetSizeModeFactor(const Vector3& factor);

  ///@copydoc Actor::
  const Vector3& GetSizeModeFactor() const;

  ///@copydoc Actor::SetSize
  void SetSize(const Vector3& size);

  ///@ Set the target size / preferred size
  void SetSizeInternal(const Vector3& size);

  ///@copydoc Actor::SetWidth
  void SetWidth(float width);

  ///@copydoc Actor::SetHeight
  void SetHeight(float height);

  ///@copydoc Actor::SetDepth
  void SetDepth(float depth);

  ///@copydoc Actor::GetTargetSize
  Vector3 GetTargetSize() const;

  ///@copydoc Actor::SetResizePolicy
  void SetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  ///@copydoc Actor:::Type
  ResizePolicy::Type GetResizePolicy(Dimension::Type dimension) const;

  ///@copydoc Actor::SetSizeScalePolicy
  void SetSizeScalePolicy(SizeScalePolicy::Type policy);

  ///@copydoc Actor:::GetSizeScalePolicy
  SizeScalePolicy::Type GetSizeScalePolicy() const;

  ///@copydoc Actor::SetDimensionDependency
  void SetDimensionDependency(Dimension::Type dimension, Dimension::Type dependency);

  ///@copydoc Actor::GetDimensionDependency
  Dimension::Type GetDimensionDependency(Dimension::Type dimension) const;

  ///@copydoc Actor::SetRelayoutEnabled
  void SetRelayoutEnabled(bool relayoutEnabled);

  ///@copydoc Actor::IsRelayoutEnabled
  bool IsRelayoutEnabled() const;

  ///@copydoc Actor::SetLayoutDirty
  void SetLayoutDirty(bool dirty, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  ///@copydoc Actor::IsLayoutDirty
  bool IsLayoutDirty(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) const;

  ///@copydoc Actor::RelayoutPossible
  bool RelayoutPossible(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) const;

  ///@copydoc Actor::RelayoutRequired
  bool RelayoutRequired(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) const;

  ///@copydoc Actor::RelayoutDependentOnParent
  bool RelayoutDependentOnParent(Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  ///@copydoc Actor::RelayoutDependentOnChildren
  bool RelayoutDependentOnChildrenBase(Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  ///@copydoc Actor::RelayoutDependentOnDimension
  bool RelayoutDependentOnDimension(Dimension::Type dimension, Dimension::Type dependentDimension);

  ///@copydoc Actor::SetNegotiatedDimension
  void SetNegotiatedDimension(float negotiatedDimension, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  ///@copydoc Actor::GetNegotiatedDimension
  float GetNegotiatedDimension(Dimension::Type dimension) const;

  ///@copydoc Actor::SetPadding
  void SetPadding(const Vector2& padding, Dimension::Type dimension);

  ///@copydoc Actor::GetPadding
  Vector2 GetPadding(Dimension::Type dimension) const;

  ///@copydoc Actor::SetLayoutNegotiated
  void SetLayoutNegotiated(bool negotiated, Dimension::Type dimension);

  ///@copydoc Actor::IsLayoutNegotiated
  bool IsLayoutNegotiated(Dimension::Type dimension) const;

  ///@copydoc Actor::GetHeightForWidth
  float GetHeightForWidthBase(float width);

  ///@copydoc Actor::GetWidthForHeight
  float GetWidthForHeightBase(float height);

  ///@copydoc Actor::CalculateChildSize
  float CalculateChildSizeBase(const Dali::Actor& child, Dimension::Type dimension);

  ///@copydoc Actor::GetLatestSize
  float GetLatestSize(Dimension::Type dimension) const;

  ///@copydoc Actor::GetRelayoutSize
  float GetRelayoutSize(Dimension::Type dimension) const;

  ///@copydoc Actor::GetSize
  float GetSize(Dimension::Type dimension) const;

  ///@copydoc Actor::GetNaturalSize
  float GetNaturalSize(Dimension::Type dimension) const;

  ///@copydoc Actor::ApplySizeSetPolicy
  Vector2 ApplySizeSetPolicy(const Vector2& size);

  ///@copydoc Actor::SetNegotiatedSize
  void SetNegotiatedSize(RelayoutContainer& container);

  ///@copydoc Actor::NegotiateSize
  void NegotiateSize(const Vector2& allocatedSize, RelayoutContainer& container);

  ///@copydoc Actor::RelayoutRequest
  void RelayoutRequest(Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  ///@copydoc Actor::SetMinimumSize
  void SetMinimumSize(float size, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  ///@copydoc Actor::GetMinimumSize
  float GetMinimumSize(Dimension::Type dimension) const;

  ///@copydoc Actor::SetMaximumSize
  void SetMaximumSize(float size, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  ///@copydoc Actor::GetMaximumSize
  float GetMaximumSize(Dimension::Type dimension) const;

  /**
   * Update target / preferred / animated size when size animation starts
   * @param[in] animation The animation modifying the size
   * @param[in] targetSize The new target size
   */
  void OnAnimateSize(Animation& animation, Vector3 targetSize, bool relative);

  /**
   * Update target / preferred / animated width when size animation starts
   * @param[in] animation The animation modifying the width
   * @param[in] targetWidth The new target width
   */
  void OnAnimateWidth(Animation& animation, float targetWidth, bool relative);

  /**
   * Update target / preferred / animated height when size animation starts
   * @param[in] animation The animation modifying the height
   * @param[in] targetHeight The new target height
   */
  void OnAnimateHeight(Animation& animation, float targetHeight, bool relative);

  /**
   * Update target / preferred / animated depth when size animation starts
   * @param[in] animation The animation modifying the depth
   * @param[in] targetDepth The new target depth
   */
  void OnAnimateDepth(Animation& animation, float targetDepth, bool relative);

private:
  struct Relayouter;

  /**
   * @brief Ensure the relayouter is allocated
   */
  Relayouter& EnsureRelayouter();

  /**
  * @brief Extract a given dimension from a Vector3
  *
  * @param[in] values The values to extract from
  * @param[in] dimension The dimension to extract
  * @return Return the value for the dimension
  */
  float GetDimensionValue(const Vector3& values, const Dimension::Type dimension) const;

  /**
   * @brief Clamp a dimension given the relayout constraints on given actor
   *
   * @param[in] size The size to constrain
   * @param[in] dimension The dimension the size exists in
   * @return Return the clamped size
   */
  float ClampDimension(float size, Dimension::Type dimension) const;

  /**
   * Negotiate a dimension based on the size of the parent
   *
   * @param[in] dimension The dimension to negotiate on
   * @return Return the negotiated size
   */
  float NegotiateFromParent(Dimension::Type dimension);

  /**
   * @brief Negotiate a dimension based on the size of the children
   *
   * @param[in] dimension The dimension to negotiate on
   * @return Return the negotiated size
   */
  float NegotiateFromChildren(Dimension::Type dimension);

  /**
   * Negotiate size for a specific dimension
   *
   * The algorithm adopts a recursive dependency checking approach. Meaning, that wherever dependencies
   * are found, e.g. an actor dependent on its parent, the dependency will be calculated first with NegotiatedDimension and
   * LayoutDimensionNegotiated flags being filled in on the actor.
   *
   * @post All actors that exist in the dependency chain connected to the given actor will have had their NegotiatedDimensions
   * calculated and set as well as the LayoutDimensionNegotiated flags.
   *
   * @param[in] dimension The dimension to negotiate on
   * @param[in] allocatedSize The size constraint that the actor must respect
   */
  void NegotiateDimension(Dimension::Type dimension, const Vector2& allocatedSize, ActorDimensionStack& recursionStack);

  /**
   * Negotiate sizes for a control in all dimensions
   *
   * @param[in] allocatedSize The size constraint that the control must respect
   */
  void NegotiateDimensions(const Vector2& allocatedSize);

  /**
   * @brief Set whether size negotiation should use the assigned size of the actor
   * during relayout for the given dimension(s)
   *
   * @param[in] use Whether the assigned size of the actor should be used
   * @param[in] dimension The dimension(s) to set. Can be a bitfield of multiple dimensions
   */
  void SetUseAssignedSize(bool use, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @brief Returns whether size negotiation should use the assigned size of the actor
   * during relayout for a single dimension
   *
   * @param[in] dimension The dimension to get
   * @return Return whether the assigned size of the actor should be used. If more than one dimension is requested, just return the first one found
   */
  bool GetUseAssignedSize(Dimension::Type dimension) const;

  /**
   * @brief Calculate the size of a dimension
   *
   * @param[in] dimension The dimension to calculate the size for
   * @param[in] maximumSize The upper bounds on the size
   * @return Return the calculated size for the dimension
   */
  float CalculateSize(Dimension::Type dimension, const Vector2& maximumSize);

  /**
   * @brief Set the preferred size for size negotiation
   *
   * @param[in] size The preferred size to set
   */
  void SetPreferredSize(const Vector2& size);

  /**
   * @brief Get the preferred size for size negotiation
   *
   * @return size The preferred size to set
   */
  Vector2 GetPreferredSize() const;

private:
  Internal::Actor& mOwner;               // Owner of this actor sizer
  Relayouter*      mRelayoutData;        ///< Struct to hold optional collection of relayout variables
  Dali::Vector3    mTargetSize;          ///< Event-side storage for size (not a pointer as most actors will have a size)
  Dali::Vector3    mAnimatedSize;        ///< Event-side storage for size animation
  uint16_t         mUseAnimatedSize;     ///< Whether the size is animated.
  bool             mTargetSizeDirtyFlag; ///< Whether the target size is dirty or not.
  bool             mInsideOnSizeSet : 1;
};

} // namespace Dali::Internal

#endif //DALI_INTERNAL_ACTOR_SIZER_H
