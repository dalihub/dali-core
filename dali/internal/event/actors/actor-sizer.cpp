/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/event/actors/actor-sizer.h>

// EXTERNAL INCLUDES
#include <cfloat>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-relayouter.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/internal/update/nodes/node-messages.h>
#include <dali/internal/update/nodes/node.h>

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gLogRelayoutFilter;
#endif

namespace
{
/**
 * @brief Extract a given dimension from a Vector2
 *
 * @param[in] values The values to extract from
 * @param[in] dimension The dimension to extract
 * @return Return the value for the dimension
 */
constexpr float GetDimensionValue(const Dali::Vector2& values, const Dali::Dimension::Type dimension)
{
  switch(dimension)
  {
    case Dali::Dimension::WIDTH:
    {
      return values.width;
    }
    case Dali::Dimension::HEIGHT:
    {
      return values.height;
    }
    default:
    {
      break;
    }
  }
  return 0.0f;
}

/**
 * @brief Keep a static recursionstack vector to avoid creating temporary vectors every Relayout().
 */
Dali::Internal::ActorSizer::ActorDimensionStack& GetRecursionStack()
{
  static Dali::Internal::ActorSizer::ActorDimensionStack gRecursionStack{};
  return gRecursionStack;
}
} // namespace

namespace Dali::Internal
{
ActorSizer::ActorSizer(Internal::Actor& owner)
: mOwner(owner),
  mRelayoutData(nullptr),
  mTargetSize(Vector3::ZERO),
  mAnimatedSize(Vector3::ZERO),
  mUseAnimatedSize(AnimatedSizeFlag::CLEAR),
  mTargetSizeDirtyFlag(false),
  mInsideOnSizeSet(false)
{
}

ActorSizer::~ActorSizer()
{
  // Delete optional relayout data
  delete mRelayoutData;
}

void ActorSizer::SetSizeModeFactor(const Vector3& factor)
{
  EnsureRelayouter();

  mRelayoutData->sizeModeFactor = factor;
}
const Vector3& ActorSizer::GetSizeModeFactor() const
{
  return mRelayoutData ? mRelayoutData->sizeModeFactor : Relayouter::DEFAULT_SIZE_MODE_FACTOR;
}

void ActorSizer::SetSize(const Vector3& size)
{
  if(IsRelayoutEnabled() && !mRelayoutData->insideRelayout)
  {
    // TODO we cannot just ignore the given Z but that means rewrite the size negotiation!!
    SetPreferredSize(size.GetVectorXY());
  }
  else
  {
    SetSizeInternal(size);
  }
}

void ActorSizer::SetSizeInternal(const Vector3& size)
{
  // dont allow recursive loop
  DALI_ASSERT_ALWAYS(!mInsideOnSizeSet && "Cannot call SetSize from OnSizeSet");
  // check that we have a node AND the new size width, height or depth is at least a little bit different from the old one
  if(mTargetSize != size || mTargetSizeDirtyFlag)
  {
    mTargetSizeDirtyFlag = false;
    mTargetSize          = size;

    // Update the preferred size after relayoutting
    // It should be used in the next relayoutting
    if(mUseAnimatedSize & AnimatedSizeFlag::WIDTH && mRelayoutData)
    {
      mRelayoutData->preferredSize.width = mAnimatedSize.width;
    }

    if(mUseAnimatedSize & AnimatedSizeFlag::HEIGHT && mRelayoutData)
    {
      mRelayoutData->preferredSize.height = mAnimatedSize.height;
    }

    // node is being used in a separate thread; queue a message to set the value & base value
    auto& node = mOwner.GetNode();
    SceneGraph::NodeTransformPropertyMessage<Vector3>::Send(mOwner.GetEventThreadServices(), &node, &node.mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::Bake, mTargetSize);

    // Notification for derived classes
    mInsideOnSizeSet = true;
    mOwner.OnSizeSet(mTargetSize);
    mInsideOnSizeSet = false;

    // Raise a relayout request if the flag is not locked
    if(mRelayoutData && !mRelayoutData->insideRelayout)
    {
      RelayoutRequest();
    }
  }
}

void ActorSizer::SetWidth(float width)
{
  if(IsRelayoutEnabled() && !mRelayoutData->insideRelayout)
  {
    SetResizePolicy(ResizePolicy::FIXED, Dimension::WIDTH);
    mRelayoutData->preferredSize.width = width;
  }
  else
  {
    mTargetSize.width = width;

    // node is being used in a separate thread; queue a message to set the value & base value
    auto& node = mOwner.GetNode();
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send(mOwner.GetEventThreadServices(), &node, &node.mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeX, width);
  }

  mUseAnimatedSize &= ~AnimatedSizeFlag::WIDTH;
  RelayoutRequest();
}

void ActorSizer::SetHeight(float height)
{
  if(IsRelayoutEnabled() && !mRelayoutData->insideRelayout)
  {
    SetResizePolicy(ResizePolicy::FIXED, Dimension::HEIGHT);
    mRelayoutData->preferredSize.height = height;
  }
  else
  {
    mTargetSize.height = height;

    // node is being used in a separate thread; queue a message to set the value & base value
    auto& node = mOwner.GetNode();
    SceneGraph::NodeTransformComponentMessage<Vector3>::Send(mOwner.GetEventThreadServices(), &node, &node.mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeY, height);
  }

  mUseAnimatedSize &= ~AnimatedSizeFlag::HEIGHT;
  RelayoutRequest();
}

void ActorSizer::SetDepth(float depth)
{
  mTargetSize.depth = depth;

  mUseAnimatedSize &= ~AnimatedSizeFlag::DEPTH;

  // node is being used in a separate thread; queue a message to set the value & base value
  auto& node = mOwner.GetNode();
  SceneGraph::NodeTransformComponentMessage<Vector3>::Send(mOwner.GetEventThreadServices(), &node, &node.mSize, &SceneGraph::TransformManagerPropertyHandler<Vector3>::BakeZ, depth);
}

Vector3 ActorSizer::GetTargetSize() const
{
  Vector3 size = mTargetSize;

  if(mUseAnimatedSize & AnimatedSizeFlag::WIDTH)
  {
    // Should return animated size if size is animated
    size.width = mAnimatedSize.width;
  }
  else
  {
    // Should return preferred size if size is fixed as set by SetSize
    if(GetResizePolicy(Dimension::WIDTH) == ResizePolicy::FIXED)
    {
      size.width = GetPreferredSize().width;
    }
  }

  if(mUseAnimatedSize & AnimatedSizeFlag::HEIGHT)
  {
    size.height = mAnimatedSize.height;
  }
  else
  {
    if(GetResizePolicy(Dimension::HEIGHT) == ResizePolicy::FIXED)
    {
      size.height = GetPreferredSize().height;
    }
  }

  if(mUseAnimatedSize & AnimatedSizeFlag::DEPTH)
  {
    size.depth = mAnimatedSize.depth;
  }

  return size;
}

void ActorSizer::SetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
  EnsureRelayouter().SetResizePolicy(policy, dimension, mTargetSize, mTargetSizeDirtyFlag);
  mOwner.OnSetResizePolicy(policy, dimension);
  RelayoutRequest();
}

ResizePolicy::Type ActorSizer::GetResizePolicy(Dimension::Type dimension) const
{
  return mRelayoutData ? mRelayoutData->GetResizePolicy(dimension) : ResizePolicy::DEFAULT;
}

void ActorSizer::SetSizeScalePolicy(SizeScalePolicy::Type policy)
{
  EnsureRelayouter();
  mRelayoutData->sizeSetPolicy = policy;
  RelayoutRequest();
}

SizeScalePolicy::Type ActorSizer::GetSizeScalePolicy() const
{
  return mRelayoutData ? mRelayoutData->sizeSetPolicy : Relayouter::DEFAULT_SIZE_SCALE_POLICY;
}

Dimension::Type ActorSizer::GetDimensionDependency(Dimension::Type dimension) const
{
  return mRelayoutData ? mRelayoutData->GetDimensionDependency(dimension) : Dimension::ALL_DIMENSIONS;
}

void ActorSizer::SetRelayoutEnabled(bool relayoutEnabled)
{
  // If relayout data has not been allocated yet and the client is requesting
  // to disable it, do nothing
  if(mRelayoutData || relayoutEnabled)
  {
    EnsureRelayouter();

    DALI_ASSERT_DEBUG(mRelayoutData && "mRelayoutData not created");

    mRelayoutData->relayoutEnabled = relayoutEnabled;
  }
}

bool ActorSizer::IsRelayoutEnabled() const
{
  // Assume that if relayout data has not been allocated yet then relayout is disabled
  return mRelayoutData && mRelayoutData->relayoutEnabled;
}

void ActorSizer::SetLayoutDirty(bool dirty, Dimension::Type dimension)
{
  EnsureRelayouter().SetLayoutDirty(dirty, dimension);
}

bool ActorSizer::IsLayoutDirty(Dimension::Type dimension) const
{
  return mRelayoutData && mRelayoutData->IsLayoutDirty(dimension);
}

bool ActorSizer::RelayoutPossible(Dimension::Type dimension) const
{
  return mRelayoutData && mRelayoutData->relayoutEnabled && !IsLayoutDirty(dimension);
}

bool ActorSizer::RelayoutRequired(Dimension::Type dimension) const
{
  return mRelayoutData && mRelayoutData->relayoutEnabled && IsLayoutDirty(dimension);
}

ActorSizer::Relayouter& ActorSizer::EnsureRelayouter()
{
  // Assign relayouter
  if(!mRelayoutData)
  {
    mRelayoutData = new Relayouter();
  }

  return *mRelayoutData;
}

bool ActorSizer::RelayoutDependentOnParent(Dimension::Type dimension)
{
  // If there is no relayouting, GetResizePolicy returns Default, which is USE_NATURAL_SIZE. This
  // will keep the existing behaviour, and return false.

  // Check if actor is dependent on parent
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if((dimension & (1 << i)))
    {
      const ResizePolicy::Type resizePolicy = GetResizePolicy(static_cast<Dimension::Type>(1 << i));
      if(resizePolicy == ResizePolicy::FILL_TO_PARENT || resizePolicy == ResizePolicy::SIZE_RELATIVE_TO_PARENT || resizePolicy == ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT)
      {
        return true;
      }
    }
  }

  return false;
}

bool ActorSizer::RelayoutDependentOnChildrenBase(Dimension::Type dimension)
{
  // If there is no relayouting, GetResizePolicy returns Default, which is USE_NATURAL_SIZE.
  // This means this will return true when there is no relayouting, but that seems
  // counter-intuitive. Will keep current behaviour for now, as it is consistent.

  // Check if actor is dependent on children
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if((dimension & (1 << i)))
    {
      const ResizePolicy::Type resizePolicy = GetResizePolicy(static_cast<Dimension::Type>(1 << i));
      switch(resizePolicy)
      {
        case ResizePolicy::FIT_TO_CHILDREN:
        case ResizePolicy::USE_NATURAL_SIZE: // i.e. For things that calculate their size based on children
        {
          return true;
        }

        default:
        {
          break;
        }
      }
    }
  }

  return false;
}

bool ActorSizer::RelayoutDependentOnDimension(Dimension::Type dimension, Dimension::Type dependentDimension)
{
  return mRelayoutData && mRelayoutData->GetRelayoutDependentOnDimension(dimension, dependentDimension);
}

void ActorSizer::SetNegotiatedDimension(float negotiatedDimension, Dimension::Type dimension)
{
  if(mRelayoutData)
  {
    mRelayoutData->SetNegotiatedDimension(negotiatedDimension, dimension);
  }
}

float ActorSizer::GetNegotiatedDimension(Dimension::Type dimension) const
{
  return mRelayoutData ? mRelayoutData->GetNegotiatedDimension(dimension) : 0.0f;
}

void ActorSizer::SetPadding(const Vector2& padding, Dimension::Type dimension)
{
  EnsureRelayouter().SetPadding(padding, dimension);
}

Vector2 ActorSizer::GetPadding(Dimension::Type dimension) const
{
  return mRelayoutData ? mRelayoutData->GetPadding(dimension) : Relayouter::DEFAULT_DIMENSION_PADDING;
}

void ActorSizer::SetLayoutNegotiated(bool negotiated, Dimension::Type dimension)
{
  EnsureRelayouter().SetLayoutNegotiated(negotiated, dimension);
}

bool ActorSizer::IsLayoutNegotiated(Dimension::Type dimension) const
{
  return mRelayoutData && mRelayoutData->IsLayoutNegotiated(dimension);
}

float ActorSizer::GetHeightForWidthBase(float width)
{
  const Vector3 naturalSize = mOwner.GetNaturalSize();
  return naturalSize.width > 0.0f ? naturalSize.height * width / naturalSize.width : width;
}

float ActorSizer::GetWidthForHeightBase(float height)
{
  const Vector3 naturalSize = mOwner.GetNaturalSize();
  return naturalSize.height > 0.0f ? naturalSize.width * height / naturalSize.height : height;
}

float ActorSizer::CalculateChildSizeBase(const Dali::Actor& child, Dimension::Type dimension)
{
  // Fill to parent, taking size mode factor into account
  switch(child.GetResizePolicy(dimension))
  {
    case ResizePolicy::FILL_TO_PARENT:
    {
      return GetLatestSize(dimension);
    }

    case ResizePolicy::SIZE_RELATIVE_TO_PARENT:
    {
      Property::Value value               = child.GetProperty(Dali::Actor::Property::SIZE_MODE_FACTOR);
      Vector3         childSizeModeFactor = value.Get<Vector3>();
      return GetLatestSize(dimension) * GetDimensionValue(childSizeModeFactor, dimension);
    }

    case ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT:
    {
      Property::Value value               = child.GetProperty(Dali::Actor::Property::SIZE_MODE_FACTOR);
      Vector3         childSizeModeFactor = value.Get<Vector3>();
      return GetLatestSize(dimension) + GetDimensionValue(childSizeModeFactor, dimension);
    }

    default:
    {
      return GetLatestSize(dimension);
    }
  }
}

float ActorSizer::GetLatestSize(Dimension::Type dimension) const
{
  return IsLayoutNegotiated(dimension) ? GetNegotiatedDimension(dimension) : GetSize(dimension);
}

float ActorSizer::GetRelayoutSize(Dimension::Type dimension) const
{
  Vector2 padding = GetPadding(dimension);

  return GetLatestSize(dimension) + padding.x + padding.y;
}

float ActorSizer::NegotiateFromParent(Dimension::Type dimension)
{
  Actor* parent = mOwner.GetParent();
  if(parent)
  {
    Vector2 padding(GetPadding(dimension));
    Vector2 parentPadding(parent->mSizer.GetPadding(dimension));

    // Need to use actor API here to allow deriving actors to layout their children
    return parent->CalculateChildSize(Dali::Actor(&mOwner), dimension) - parentPadding.x - parentPadding.y - padding.x - padding.y;
  }

  return 0.0f;
}

float ActorSizer::NegotiateFromChildren(Dimension::Type dimension)
{
  float maxDimensionPoint = 0.0f;

  for(uint32_t i = 0, count = mOwner.GetChildCount(); i < count; ++i)
  {
    ActorPtr child = mOwner.GetChildAt(i);

    if(!child->RelayoutDependentOnParent(dimension))
    {
      // Calculate the min and max points that the children range across
      float childPosition = GetDimensionValue(child->GetTargetPosition(), dimension);
      float dimensionSize = child->mSizer.GetRelayoutSize(dimension);
      maxDimensionPoint   = std::max(maxDimensionPoint, childPosition + dimensionSize);
    }
  }

  return maxDimensionPoint;
}

float ActorSizer::GetSize(Dimension::Type dimension) const
{
  return GetDimensionValue(mTargetSize, dimension);
}

float ActorSizer::GetNaturalSize(Dimension::Type dimension) const
{
  return GetDimensionValue(mOwner.GetNaturalSize(), dimension);
}

Vector2 ActorSizer::ApplySizeSetPolicy(const Vector2& size)
{
  return mRelayoutData->ApplySizeSetPolicy(mOwner, size);
}

void ActorSizer::SetNegotiatedSize(RelayoutContainer& container)
{
  // Do the set actor size
  Vector2 negotiatedSize(GetLatestSize(Dimension::WIDTH), GetLatestSize(Dimension::HEIGHT));

  // Adjust for size set policy
  negotiatedSize = ApplySizeSetPolicy(negotiatedSize);

  // Lock the flag to stop recursive relayouts on set size
  mRelayoutData->insideRelayout = true;
  SetSize(Vector3(negotiatedSize.width, negotiatedSize.height, 0.0f));
  mRelayoutData->insideRelayout = false;

  // Clear flags for all dimensions
  SetLayoutDirty(false);

  // Give deriving classes a chance to respond
  mOwner.OnRelayout(negotiatedSize, container);

  if(!mOwner.mOnRelayoutSignal.Empty())
  {
    Dali::Actor handle(&mOwner);
    mOwner.mOnRelayoutSignal.Emit(handle);
  }

  mRelayoutData->relayoutRequested = false;
}

void ActorSizer::NegotiateSize(const Vector2& allocatedSize, RelayoutContainer& container)
{
  // Force a size negotiation for actors that has assigned size during relayout
  // This is required as otherwise the flags that force a relayout will not
  // necessarilly be set. This will occur if the actor has already been laid out.
  // The dirty flags are then cleared. Then if the actor is added back into the
  // relayout container afterwards, the dirty flags would still be clear...
  // causing a relayout to be skipped. Here we force any actors added to the
  // container to be relayed out.
  DALI_LOG_TIMER_START(NegSizeTimer1);

  if(GetUseAssignedSize(Dimension::WIDTH))
  {
    SetLayoutNegotiated(false, Dimension::WIDTH);
  }
  if(GetUseAssignedSize(Dimension::HEIGHT))
  {
    SetLayoutNegotiated(false, Dimension::HEIGHT);
  }

  // Do the negotiation
  NegotiateDimensions(allocatedSize);

  // Set the actor size
  SetNegotiatedSize(container);

  // Negotiate down to children
  for(uint32_t i = 0, count = mOwner.GetChildCount(); i < count; ++i)
  {
    ActorPtr    child      = mOwner.GetChildAt(i);
    ActorSizer& childSizer = child->mSizer;

    // Forces children that have already been laid out to be relayed out
    // if they have assigned size during relayout.
    if(childSizer.GetUseAssignedSize(Dimension::WIDTH))
    {
      childSizer.SetLayoutNegotiated(false, Dimension::WIDTH);
      childSizer.SetLayoutDirty(true, Dimension::WIDTH);
    }

    if(childSizer.GetUseAssignedSize(Dimension::HEIGHT))
    {
      childSizer.SetLayoutNegotiated(false, Dimension::HEIGHT);
      childSizer.SetLayoutDirty(true, Dimension::HEIGHT);
    }

    // Only relayout if required
    if(childSizer.RelayoutRequired())
    {
      container.Add(Dali::Actor(child.Get()), mTargetSize.GetVectorXY());
    }
  }

  // Reset the flag so that size negotiation will respect the actor's original resize policy
  SetUseAssignedSize(false);
  DALI_LOG_TIMER_END(NegSizeTimer1, gLogRelayoutFilter, Debug::Concise, "NegotiateSize() took: ");
}

void ActorSizer::SetUseAssignedSize(bool use, Dimension::Type dimension)
{
  if(mRelayoutData)
  {
    mRelayoutData->SetUseAssignedSize(use, dimension);
  }
}

bool ActorSizer::GetUseAssignedSize(Dimension::Type dimension) const
{
  return mRelayoutData && mRelayoutData->GetUseAssignedSize(dimension);
}

void ActorSizer::RelayoutRequest(Dimension::Type dimension)
{
  Internal::RelayoutController* relayoutController = Internal::RelayoutController::Get();
  if(relayoutController)
  {
    Dali::Actor owner(&mOwner);
    relayoutController->RequestRelayout(owner, dimension);

    if(mRelayoutData)
    {
      mRelayoutData->relayoutRequested = true;
    }
  }
}

void ActorSizer::SetPreferredSize(const Vector2& size)
{
  EnsureRelayouter();

  // If valid width or height, then set the resize policy to FIXED
  // A 0 width or height may also be required so if the resize policy has not been changed, i.e. is still set to DEFAULT,
  // then change to FIXED as well

  if(size.width > 0.0f || GetResizePolicy(Dimension::WIDTH) == ResizePolicy::DEFAULT)
  {
    SetResizePolicy(ResizePolicy::FIXED, Dimension::WIDTH);
  }

  if(size.height > 0.0f || GetResizePolicy(Dimension::HEIGHT) == ResizePolicy::DEFAULT)
  {
    SetResizePolicy(ResizePolicy::FIXED, Dimension::HEIGHT);
  }

  mRelayoutData->preferredSize = size;
  mUseAnimatedSize             = AnimatedSizeFlag::CLEAR;
  RelayoutRequest();
}

Vector2 ActorSizer::GetPreferredSize() const
{
  return mRelayoutData ? Vector2(mRelayoutData->preferredSize) : Relayouter::DEFAULT_PREFERRED_SIZE;
}

void ActorSizer::SetMinimumSize(float size, Dimension::Type dimension)
{
  EnsureRelayouter().SetMinimumSize(size, dimension);
  RelayoutRequest();
}

float ActorSizer::GetMinimumSize(Dimension::Type dimension) const
{
  return mRelayoutData ? mRelayoutData->GetMinimumSize(dimension) : 0.0f;
}

void ActorSizer::SetMaximumSize(float size, Dimension::Type dimension)
{
  EnsureRelayouter().SetMaximumSize(size, dimension);
  RelayoutRequest();
}

float ActorSizer::GetMaximumSize(Dimension::Type dimension) const
{
  return mRelayoutData ? mRelayoutData->GetMaximumSize(dimension) : FLT_MAX;
}

void ActorSizer::OnAnimateSize(Animation& animation, Vector3 targetSize, bool relative)
{
  mTargetSize      = targetSize + mTargetSize * float(relative);
  mAnimatedSize    = mTargetSize;
  mUseAnimatedSize = AnimatedSizeFlag::WIDTH | AnimatedSizeFlag::HEIGHT | AnimatedSizeFlag::DEPTH;

  if(mRelayoutData && !mRelayoutData->relayoutRequested)
  {
    mRelayoutData->preferredSize.width  = mAnimatedSize.width;
    mRelayoutData->preferredSize.height = mAnimatedSize.height;
  }

  // Notify deriving classes
  mOwner.OnSizeAnimation(animation, mTargetSize);
}

void ActorSizer::OnAnimateWidth(Animation& animation, float width, bool relative)
{
  mTargetSize.width   = width + float(relative) * mTargetSize.width;
  mAnimatedSize.width = mTargetSize.width;
  mUseAnimatedSize |= AnimatedSizeFlag::WIDTH;

  if(mRelayoutData && !mRelayoutData->relayoutRequested)
  {
    mRelayoutData->preferredSize.width = mAnimatedSize.width;
  }

  // Notify deriving classes
  mOwner.OnSizeAnimation(animation, mTargetSize);
}

void ActorSizer::OnAnimateHeight(Animation& animation, float height, bool relative)
{
  mTargetSize.height   = height + float(relative) * mTargetSize.height;
  mAnimatedSize.height = mTargetSize.height;
  mUseAnimatedSize |= AnimatedSizeFlag::HEIGHT;

  if(mRelayoutData && !mRelayoutData->relayoutRequested)
  {
    mRelayoutData->preferredSize.height = mAnimatedSize.height;
  }

  // Notify deriving classes
  mOwner.OnSizeAnimation(animation, mTargetSize);
}

void ActorSizer::OnAnimateDepth(Animation& animation, float depth, bool relative)
{
  mTargetSize.depth   = depth + float(relative) * mTargetSize.depth;
  mAnimatedSize.depth = mTargetSize.depth;
  mUseAnimatedSize |= AnimatedSizeFlag::DEPTH;

  // Notify deriving classes
  mOwner.OnSizeAnimation(animation, mTargetSize);
}

/**
 * @brief Extract a given dimension from a Vector3
 *
 * @param[in] values The values to extract from
 * @param[in] dimension The dimension to extract
 * @return Return the value for the dimension
 */
float ActorSizer::GetDimensionValue(const Vector3& values, const Dimension::Type dimension) const
{
  return ::GetDimensionValue(values.GetVectorXY(), dimension);
}

float ActorSizer::ClampDimension(float size, Dimension::Type dimension) const
{
  const float minSize = GetMinimumSize(dimension);
  const float maxSize = GetMaximumSize(dimension);

  return std::max(minSize, std::min(size, maxSize));
}

void ActorSizer::NegotiateDimension(Dimension::Type dimension, const Vector2& allocatedSize, ActorDimensionStack& recursionStack)
{
  // Check if it needs to be negotiated
  if(IsLayoutDirty(dimension) && !IsLayoutNegotiated(dimension))
  {
    // Check that we havn't gotten into an infinite loop
    ActorDimensionPair searchActor    = ActorDimensionPair(&mOwner, dimension);
    bool               recursionFound = false;
    for(auto& element : recursionStack)
    {
      if(element == searchActor)
      {
        recursionFound = true;
        break;
      }
    }

    if(!recursionFound)
    {
      // Record the path that we have taken
      recursionStack.emplace_back(&mOwner, dimension);

      // Dimension dependency check
      for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
      {
        Dimension::Type dimensionToCheck = static_cast<Dimension::Type>(1 << i);

        if(RelayoutDependentOnDimension(dimension, dimensionToCheck))
        {
          NegotiateDimension(dimensionToCheck, allocatedSize, recursionStack);
        }
      }

      // Parent dependency check
      Actor* parent = mOwner.GetParent();
      if(parent && RelayoutDependentOnParent(dimension))
      {
        parent->mSizer.NegotiateDimension(dimension, allocatedSize, recursionStack);
      }

      // Children dependency check
      if(mOwner.RelayoutDependentOnChildren(dimension))
      {
        for(uint32_t i = 0, count = mOwner.GetChildCount(); i < count; ++i)
        {
          ActorPtr child = mOwner.GetChildAt(i);

          // Only relayout child first if it is not dependent on this actor
          if(!child->RelayoutDependentOnParent(dimension))
          {
            child->mSizer.NegotiateDimension(dimension, allocatedSize, recursionStack);
          }
        }
      }

      // For deriving classes
      mOwner.OnCalculateRelayoutSize(dimension);

      // All dependencies checked, calculate the size and set negotiated flag
      const float newSize = ClampDimension(CalculateSize(dimension, allocatedSize), dimension);

      SetNegotiatedDimension(newSize, dimension);
      SetLayoutNegotiated(true, dimension);

      // For deriving classes
      mOwner.OnLayoutNegotiated(newSize, dimension);

      // This actor has been successfully processed, pop it off the recursion stack
      recursionStack.pop_back();
    }
    else
    {
      // TODO: Break infinite loop
      SetLayoutNegotiated(true, dimension);
    }
  }
}

void ActorSizer::NegotiateDimensions(const Vector2& allocatedSize)
{
  // Negotiate all dimensions that require it
  GetRecursionStack().clear();

  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    const Dimension::Type dimension = static_cast<Dimension::Type>(1 << i);

    // Negotiate
    NegotiateDimension(dimension, allocatedSize, GetRecursionStack());
  }
}

float ActorSizer::CalculateSize(Dimension::Type dimension, const Vector2& maximumSize)
{
  switch(GetResizePolicy(dimension))
  {
    case ResizePolicy::USE_NATURAL_SIZE:
    {
      return GetNaturalSize(dimension);
    }

    case ResizePolicy::FIXED:
    {
      return ::GetDimensionValue(GetPreferredSize(), dimension);
    }

    case ResizePolicy::USE_ASSIGNED_SIZE:
    {
      return ::GetDimensionValue(maximumSize, dimension);
    }

    case ResizePolicy::FILL_TO_PARENT:
    case ResizePolicy::SIZE_RELATIVE_TO_PARENT:
    case ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT:
    {
      return NegotiateFromParent(dimension);
    }

    case ResizePolicy::FIT_TO_CHILDREN:
    {
      return NegotiateFromChildren(dimension);
    }

    case ResizePolicy::DIMENSION_DEPENDENCY:
    {
      const Dimension::Type dimensionDependency = GetDimensionDependency(dimension);

      // Custom rules
      if(dimension == Dimension::WIDTH && dimensionDependency == Dimension::HEIGHT)
      {
        // Use actor API to allow deriving actors to layout their content
        return mOwner.GetWidthForHeight(GetNegotiatedDimension(Dimension::HEIGHT));
      }

      if(dimension == Dimension::HEIGHT && dimensionDependency == Dimension::WIDTH)
      {
        // Use actor API to allow deriving actors to layout their content
        return mOwner.GetHeightForWidth(GetNegotiatedDimension(Dimension::WIDTH));
      }

      break;
    }

    default:
    {
      break;
    }
  }

  return 0.0f; // Default
}

} // namespace Dali::Internal
