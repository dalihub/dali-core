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
 *
 */

// CLASS HEADER
#include <dali/internal/event/actors/actor-relayouter.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>

#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogRelayoutFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RELAYOUT_TIMER");
#endif

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

} // unnamed namespace

namespace Dali
{
namespace Internal
{
Actor::Relayouter::Relayouter()
: sizeModeFactor(DEFAULT_SIZE_MODE_FACTOR),
  preferredSize(DEFAULT_PREFERRED_SIZE),
  sizeSetPolicy(DEFAULT_SIZE_SCALE_POLICY),
  relayoutEnabled(false),
  insideRelayout(false),
  relayoutRequested(false)
{
  // Set size negotiation defaults
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    resizePolicies[i]        = ResizePolicy::DEFAULT;
    useAssignedSize[i]       = false;
    negotiatedDimensions[i]  = 0.0f;
    dimensionNegotiated[i]   = false;
    dimensionDirty[i]        = false;
    dimensionDependencies[i] = Dimension::ALL_DIMENSIONS;
    dimensionPadding[i]      = DEFAULT_DIMENSION_PADDING;
    minimumSize[i]           = 0.0f;
    maximumSize[i]           = FLT_MAX;
  }
}

ResizePolicy::Type Actor::Relayouter::GetResizePolicy(Dimension::Type dimension) const
{
  // If more than one dimension is requested, just return the first one found
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if((dimension & (1 << i)))
    {
      if(useAssignedSize[i])
      {
        return ResizePolicy::USE_ASSIGNED_SIZE;
      }
      else
      {
        return resizePolicies[i];
      }
    }
  }

  return ResizePolicy::DEFAULT;
}

void Actor::Relayouter::SetPadding(const Vector2& padding, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      dimensionPadding[i] = padding;
    }
  }
}

Vector2 Actor::Relayouter::GetPadding(Dimension::Type dimension)
{
  // If more than one dimension is requested, just return the first one found
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if((dimension & (1 << i)))
    {
      return dimensionPadding[i];
    }
  }

  return DEFAULT_DIMENSION_PADDING;
}

void Actor::Relayouter::SetLayoutNegotiated(bool negotiated, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      dimensionNegotiated[i] = negotiated;
    }
  }
}

bool Actor::Relayouter::IsLayoutNegotiated(Dimension::Type dimension) const
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if((dimension & (1 << i)) && dimensionNegotiated[i])
    {
      return true;
    }
  }
  return false;
}

Vector2 Actor::Relayouter::ApplySizeSetPolicy(Internal::Actor& actor, const Vector2& size)
{
  switch(sizeSetPolicy)
  {
    case SizeScalePolicy::USE_SIZE_SET:
    {
      return size;
    }

    case SizeScalePolicy::FIT_WITH_ASPECT_RATIO:
    {
      // Scale size to fit within the original size bounds, keeping the natural size aspect ratio
      const Vector3 naturalSize = actor.GetNaturalSize();
      if(naturalSize.width > 0.0f && naturalSize.height > 0.0f && size.width > 0.0f && size.height > 0.0f)
      {
        const float sizeRatio        = size.width / size.height;
        const float naturalSizeRatio = naturalSize.width / naturalSize.height;

        if(naturalSizeRatio < sizeRatio)
        {
          return Vector2(naturalSizeRatio * size.height, size.height);
        }
        else if(naturalSizeRatio > sizeRatio)
        {
          return Vector2(size.width, size.width / naturalSizeRatio);
        }
        else
        {
          return size;
        }
      }

      break;
    }

    case SizeScalePolicy::FILL_WITH_ASPECT_RATIO:
    {
      // Scale size to fill the original size bounds, keeping the natural size aspect ratio. Potentially exceeding the original bounds.
      const Vector3 naturalSize = actor.GetNaturalSize();
      if(naturalSize.width > 0.0f && naturalSize.height > 0.0f && size.width > 0.0f && size.height > 0.0f)
      {
        const float sizeRatio        = size.width / size.height;
        const float naturalSizeRatio = naturalSize.width / naturalSize.height;

        if(naturalSizeRatio < sizeRatio)
        {
          return Vector2(size.width, size.width / naturalSizeRatio);
        }
        else if(naturalSizeRatio > sizeRatio)
        {
          return Vector2(naturalSizeRatio * size.height, size.height);
        }
        else
        {
          return size;
        }
      }
      break;
    }

    default:
    {
      break;
    }
  }

  return size;
}

void Actor::Relayouter::SetUseAssignedSize(bool use, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      useAssignedSize[i] = use;
    }
  }
}

bool Actor::Relayouter::GetUseAssignedSize(Dimension::Type dimension) const
{
  // If more than one dimension is requested, just return the first one found
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      return useAssignedSize[i];
    }
  }

  return false;
}

void Actor::Relayouter::SetMinimumSize(float size, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      minimumSize[i] = size;
    }
  }
}

float Actor::Relayouter::GetMinimumSize(Dimension::Type dimension) const
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      return minimumSize[i];
    }
  }

  return 0.0f; // Default
}

void Actor::Relayouter::SetMaximumSize(float size, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      maximumSize[i] = size;
    }
  }
}

float Actor::Relayouter::GetMaximumSize(Dimension::Type dimension) const
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      return maximumSize[i];
    }
  }

  return FLT_MAX; // Default
}

void Actor::Relayouter::SetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension, Vector3& targetSize)
{
  ResizePolicy::Type originalWidthPolicy  = GetResizePolicy(Dimension::WIDTH);
  ResizePolicy::Type originalHeightPolicy = GetResizePolicy(Dimension::HEIGHT);

  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      if(policy == ResizePolicy::USE_ASSIGNED_SIZE)
      {
        useAssignedSize[i] = true;
      }
      else
      {
        resizePolicies[i]  = policy;
        useAssignedSize[i] = false;
      }
    }
  }

  if(policy == ResizePolicy::DIMENSION_DEPENDENCY)
  {
    if(dimension & Dimension::WIDTH)
    {
      SetDimensionDependency(Dimension::WIDTH, Dimension::HEIGHT);
    }

    if(dimension & Dimension::HEIGHT)
    {
      SetDimensionDependency(Dimension::HEIGHT, Dimension::WIDTH);
    }
  }

  // If calling SetResizePolicy, assume we want relayout enabled
  relayoutEnabled = true;

  // If the resize policy is set to be FIXED, the preferred size
  // should be overrided by the target size. Otherwise the target
  // size should be overrided by the preferred size.

  if(dimension & Dimension::WIDTH)
  {
    if(originalWidthPolicy != ResizePolicy::FIXED && policy == ResizePolicy::FIXED)
    {
      preferredSize.width = targetSize.width;
    }
    else if(originalWidthPolicy == ResizePolicy::FIXED && policy != ResizePolicy::FIXED)
    {
      targetSize.width = preferredSize.width;
    }
  }

  if(dimension & Dimension::HEIGHT)
  {
    if(originalHeightPolicy != ResizePolicy::FIXED && policy == ResizePolicy::FIXED)
    {
      preferredSize.height = targetSize.height;
    }
    else if(originalHeightPolicy == ResizePolicy::FIXED && policy != ResizePolicy::FIXED)
    {
      targetSize.height = preferredSize.height;
    }
  }
}

bool Actor::Relayouter::GetRelayoutDependentOnParent(Dimension::Type dimension)
{
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

bool Actor::Relayouter::GetRelayoutDependentOnChildren(Dimension::Type dimension)
{
  // Check if actor is dependent on children
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if((dimension & (1 << i)))
    {
      const ResizePolicy::Type resizePolicy = GetResizePolicy(static_cast<Dimension::Type>(1 << i));
      if(resizePolicy == ResizePolicy::FIT_TO_CHILDREN || resizePolicy == ResizePolicy::USE_NATURAL_SIZE)
      {
        return true;
      }
      break;
    }
  }

  return false;
}

bool Actor::Relayouter::GetRelayoutDependentOnDimension(Dimension::Type dimension, Dimension::Type dependency)
{
  // Check each possible dimension and see if it is dependent on the input one
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      return resizePolicies[i] == ResizePolicy::DIMENSION_DEPENDENCY && dimensionDependencies[i] == dependency;
    }
  }

  return false;
}

void Actor::Relayouter::SetDimensionDependency(Dimension::Type dimension, Dimension::Type dependency)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      dimensionDependencies[i] = dependency;
    }
  }
}

Dimension::Type Actor::Relayouter::GetDimensionDependency(Dimension::Type dimension) const
{
  // If more than one dimension is requested, just return the first one found
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if((dimension & (1 << i)))
    {
      return dimensionDependencies[i];
    }
  }

  return Dimension::ALL_DIMENSIONS; // Default
}

void Actor::Relayouter::SetLayoutDirty(bool dirty, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      dimensionDirty[i] = dirty;
    }
  }
}

bool Actor::Relayouter::IsLayoutDirty(Dimension::Type dimension) const
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if((dimension & (1 << i)) && dimensionDirty[i])
    {
      return true;
    }
  }

  return false;
}

void Actor::Relayouter::SetPreferredSize(Actor& actor, const Vector2& size)
{
  // If valid width or height, then set the resize policy to FIXED
  // A 0 width or height may also be required so if the resize policy has not been changed, i.e. is still set to DEFAULT,
  // then change to FIXED as well

  if(size.width > 0.0f || GetResizePolicy(Dimension::WIDTH) == ResizePolicy::DEFAULT)
  {
    actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::WIDTH);
  }

  if(size.height > 0.0f || GetResizePolicy(Dimension::HEIGHT) == ResizePolicy::DEFAULT)
  {
    actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::HEIGHT);
  }

  actor.mRelayoutData->preferredSize = size;

  actor.mUseAnimatedSize = AnimatedSizeFlag::CLEAR;

  actor.RelayoutRequest();
}

float Actor::Relayouter::ClampDimension(const Internal::Actor& actor, float size, Dimension::Type dimension)
{
  const float minSize = actor.GetMinimumSize(dimension);
  const float maxSize = actor.GetMaximumSize(dimension);

  return std::max(minSize, std::min(size, maxSize));
}

void Actor::Relayouter::SetNegotiatedDimension(float negotiatedDimension, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      negotiatedDimensions[i] = negotiatedDimension;
    }
  }
}

float Actor::Relayouter::GetNegotiatedDimension(Dimension::Type dimension)
{
  // If more than one dimension is requested, just return the first one found
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if((dimension & (1 << i)))
    {
      return negotiatedDimensions[i];
    }
  }

  return 0.0f; // Default
}

float Actor::Relayouter::NegotiateDimensionFromParent(Actor& actor, Dimension::Type dimension)
{
  Actor* parent = actor.GetParent();
  if(parent)
  {
    Vector2 padding(actor.GetPadding(dimension));
    Vector2 parentPadding(parent->GetPadding(dimension));

    // Need to use actor API here to allow deriving actors to layout their children
    return parent->CalculateChildSize(Dali::Actor(&actor), dimension) - parentPadding.x - parentPadding.y - padding.x - padding.y;
  }

  return 0.0f;
}

float Actor::Relayouter::NegotiateDimensionFromChildren(Actor& actor, Dimension::Type dimension)
{
  float maxDimensionPoint = 0.0f;

  for(uint32_t i = 0, count = actor.GetChildCount(); i < count; ++i)
  {
    ActorPtr child = actor.GetChildAt(i);

    if(!child->RelayoutDependentOnParent(dimension))
    {
      // Calculate the min and max points that the children range across
      float childPosition = GetDimensionValue(child->GetTargetPosition(), dimension);
      float dimensionSize = child->GetRelayoutSize(dimension);
      maxDimensionPoint   = std::max(maxDimensionPoint, childPosition + dimensionSize);
    }
  }

  return maxDimensionPoint;
}

void Actor::Relayouter::NegotiateDimension(Actor& actor, Dimension::Type dimension, const Vector2& allocatedSize, Actor::ActorDimensionStack& recursionStack)
{
  // Check if it needs to be negotiated
  if(actor.IsLayoutDirty(dimension) && !actor.IsLayoutNegotiated(dimension))
  {
    // Check that we havn't gotten into an infinite loop
    Actor::ActorDimensionPair searchActor    = Actor::ActorDimensionPair(&actor, dimension);
    bool                      recursionFound = false;
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
      recursionStack.push_back(Actor::ActorDimensionPair(&actor, dimension));

      // Dimension dependency check
      for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
      {
        Dimension::Type dimensionToCheck = static_cast<Dimension::Type>(1 << i);

        if(actor.RelayoutDependentOnDimension(dimension, dimensionToCheck))
        {
          NegotiateDimension(actor, dimensionToCheck, allocatedSize, recursionStack);
        }
      }

      // Parent dependency check
      Actor* parent = actor.GetParent();
      if(parent && actor.RelayoutDependentOnParent(dimension))
      {
        NegotiateDimension(*parent, dimension, allocatedSize, recursionStack);
      }

      // Children dependency check
      if(actor.RelayoutDependentOnChildren(dimension))
      {
        for(uint32_t i = 0, count = actor.GetChildCount(); i < count; ++i)
        {
          ActorPtr child = actor.GetChildAt(i);

          // Only relayout child first if it is not dependent on this actor
          if(!child->RelayoutDependentOnParent(dimension))
          {
            NegotiateDimension(*child, dimension, allocatedSize, recursionStack);
          }
        }
      }

      // For deriving classes
      actor.OnCalculateRelayoutSize(dimension);

      // All dependencies checked, calculate the size and set negotiated flag
      const float newSize = ClampDimension(actor, actor.CalculateSize(dimension, allocatedSize), dimension);

      actor.SetNegotiatedDimension(newSize, dimension);
      actor.SetLayoutNegotiated(true, dimension);

      // For deriving classes
      actor.OnLayoutNegotiated(newSize, dimension);

      // This actor has been successfully processed, pop it off the recursion stack
      recursionStack.pop_back();
    }
    else
    {
      // TODO: Break infinite loop
      actor.SetLayoutNegotiated(true, dimension);
    }
  }
}

void Actor::Relayouter::NegotiateDimensions(Actor& actor, const Vector2& allocatedSize)
{
  // Negotiate all dimensions that require it
  ActorDimensionStack recursionStack;

  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    const Dimension::Type dimension = static_cast<Dimension::Type>(1 << i);

    // Negotiate
    NegotiateDimension(actor, dimension, allocatedSize, recursionStack);
  }
}

void Actor::Relayouter::NegotiateSize(Actor& actor, const Vector2& allocatedSize, RelayoutContainer& container)
{
  // Force a size negotiation for actors that has assigned size during relayout
  // This is required as otherwise the flags that force a relayout will not
  // necessarilly be set. This will occur if the actor has already been laid out.
  // The dirty flags are then cleared. Then if the actor is added back into the
  // relayout container afterwards, the dirty flags would still be clear...
  // causing a relayout to be skipped. Here we force any actors added to the
  // container to be relayed out.
  DALI_LOG_TIMER_START(NegSizeTimer1);

  if(actor.GetUseAssignedSize(Dimension::WIDTH))
  {
    actor.SetLayoutNegotiated(false, Dimension::WIDTH);
  }
  if(actor.GetUseAssignedSize(Dimension::HEIGHT))
  {
    actor.SetLayoutNegotiated(false, Dimension::HEIGHT);
  }

  // Do the negotiation
  NegotiateDimensions(actor, allocatedSize);

  // Set the actor size
  actor.SetNegotiatedSize(container);

  // Negotiate down to children
  for(uint32_t i = 0, count = actor.GetChildCount(); i < count; ++i)
  {
    ActorPtr child = actor.GetChildAt(i);

    // Forces children that have already been laid out to be relayed out
    // if they have assigned size during relayout.
    if(child->GetUseAssignedSize(Dimension::WIDTH))
    {
      child->SetLayoutNegotiated(false, Dimension::WIDTH);
      child->SetLayoutDirty(true, Dimension::WIDTH);
    }

    if(child->GetUseAssignedSize(Dimension::HEIGHT))
    {
      child->SetLayoutNegotiated(false, Dimension::HEIGHT);
      child->SetLayoutDirty(true, Dimension::HEIGHT);
    }

    // Only relayout if required
    if(child->RelayoutRequired())
    {
      container.Add(Dali::Actor(child.Get()), actor.mTargetSize.GetVectorXY());
    }
  }
  DALI_LOG_TIMER_END(NegSizeTimer1, gLogRelayoutFilter, Debug::Concise, "NegotiateSize() took: ");
}

/**
 * @brief Extract a given dimension from a Vector3
 *
 * @param[in] values The values to extract from
 * @param[in] dimension The dimension to extract
 * @return Return the value for the dimension
 */
float Actor::Relayouter::GetDimensionValue(const Vector3& values, const Dimension::Type dimension)
{
  return ::GetDimensionValue(values.GetVectorXY(), dimension);
}

float Actor::Relayouter::CalculateSize(Actor& actor, Dimension::Type dimension, const Vector2& maximumSize)
{
  switch(actor.GetResizePolicy(dimension))
  {
    case ResizePolicy::USE_NATURAL_SIZE:
    {
      return actor.GetNaturalSize(dimension);
    }

    case ResizePolicy::FIXED:
    {
      return ::GetDimensionValue(actor.GetPreferredSize(), dimension);
    }

    case ResizePolicy::USE_ASSIGNED_SIZE:
    {
      return ::GetDimensionValue(maximumSize, dimension);
    }

    case ResizePolicy::FILL_TO_PARENT:
    case ResizePolicy::SIZE_RELATIVE_TO_PARENT:
    case ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT:
    {
      return NegotiateDimensionFromParent(actor, dimension);
    }

    case ResizePolicy::FIT_TO_CHILDREN:
    {
      return NegotiateDimensionFromChildren(actor, dimension);
    }

    case ResizePolicy::DIMENSION_DEPENDENCY:
    {
      const Dimension::Type dimensionDependency = actor.GetDimensionDependency(dimension);

      // Custom rules
      if(dimension == Dimension::WIDTH && dimensionDependency == Dimension::HEIGHT)
      {
        return actor.GetWidthForHeight(actor.GetNegotiatedDimension(Dimension::HEIGHT));
      }

      if(dimension == Dimension::HEIGHT && dimensionDependency == Dimension::WIDTH)
      {
        return actor.GetHeightForWidth(actor.GetNegotiatedDimension(Dimension::WIDTH));
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

float Actor::Relayouter::CalculateChildSize(Actor& actor, const Actor& child, Dimension::Type dimension)
{
  // Fill to parent, taking size mode factor into account
  switch(child.GetResizePolicy(dimension))
  {
    case ResizePolicy::FILL_TO_PARENT:
    {
      return actor.GetLatestSize(dimension);
    }

    case ResizePolicy::SIZE_RELATIVE_TO_PARENT:
    {
      Property::Value value               = child.GetProperty(Dali::Actor::Property::SIZE_MODE_FACTOR);
      Vector3         childSizeModeFactor = value.Get<Vector3>();
      return actor.GetLatestSize(dimension) * GetDimensionValue(childSizeModeFactor, dimension);
    }

    case ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT:
    {
      Property::Value value               = child.GetProperty(Dali::Actor::Property::SIZE_MODE_FACTOR);
      Vector3         childSizeModeFactor = value.Get<Vector3>();
      return actor.GetLatestSize(dimension) + GetDimensionValue(childSizeModeFactor, dimension);
    }

    default:
    {
      return actor.GetLatestSize(dimension);
    }
  }
}

} // namespace Internal

} // namespace Dali
