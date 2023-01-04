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

#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogRelayoutFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RELAYOUT_TIMER");
#endif

namespace Dali
{
namespace Internal
{
ActorSizer::Relayouter::Relayouter()
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

ResizePolicy::Type ActorSizer::Relayouter::GetResizePolicy(Dimension::Type dimension) const
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

Vector2 ActorSizer::Relayouter::ApplySizeSetPolicy(Internal::Actor& actor, const Vector2& size)
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

void ActorSizer::Relayouter::SetUseAssignedSize(bool use, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      useAssignedSize[i] = use;
    }
  }
}

bool ActorSizer::Relayouter::GetUseAssignedSize(Dimension::Type dimension) const
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

void ActorSizer::Relayouter::SetMinimumSize(float size, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      minimumSize[i] = size;
    }
  }
}

float ActorSizer::Relayouter::GetMinimumSize(Dimension::Type dimension) const
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

void ActorSizer::Relayouter::SetMaximumSize(float size, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      maximumSize[i] = size;
    }
  }
}

float ActorSizer::Relayouter::GetMaximumSize(Dimension::Type dimension) const
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

void ActorSizer::Relayouter::SetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension, Vector3& targetSize, bool& targetSizeDirtyFlag)
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
      targetSizeDirtyFlag = true;
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
      targetSizeDirtyFlag = true;
    }
  }
}

bool ActorSizer::Relayouter::GetRelayoutDependentOnDimension(Dimension::Type dimension, Dimension::Type dependency)
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

void ActorSizer::Relayouter::SetDimensionDependency(Dimension::Type dimension, Dimension::Type dependency)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      dimensionDependencies[i] = dependency;
    }
  }
}

Dimension::Type ActorSizer::Relayouter::GetDimensionDependency(Dimension::Type dimension) const
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

void ActorSizer::Relayouter::SetLayoutDirty(bool dirty, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      dimensionDirty[i] = dirty;
    }
  }
}

bool ActorSizer::Relayouter::IsLayoutDirty(Dimension::Type dimension) const
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

void ActorSizer::Relayouter::SetNegotiatedDimension(float negotiatedDimension, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      negotiatedDimensions[i] = negotiatedDimension;
    }
  }
}

float ActorSizer::Relayouter::GetNegotiatedDimension(Dimension::Type dimension)
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

void ActorSizer::Relayouter::SetPadding(const Vector2& padding, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      dimensionPadding[i] = padding;
    }
  }
}

Vector2 ActorSizer::Relayouter::GetPadding(Dimension::Type dimension)
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

void ActorSizer::Relayouter::SetLayoutNegotiated(bool negotiated, Dimension::Type dimension)
{
  for(uint32_t i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    if(dimension & (1 << i))
    {
      dimensionNegotiated[i] = negotiated;
    }
  }
}

bool ActorSizer::Relayouter::IsLayoutNegotiated(Dimension::Type dimension) const
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

} // namespace Internal

} // namespace Dali
