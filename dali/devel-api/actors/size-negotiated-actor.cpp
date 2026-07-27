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

// CLASS HEADER
#include <dali/devel-api/actors/size-negotiated-actor.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/custom-actor-internal.h>
#include <dali/public-api/actors/custom-actor-impl.h>

namespace Dali
{
void SizeNegotiatedActor::RelayoutRequest()
{
  CustomActorImpl* impl = dynamic_cast<CustomActorImpl*>(this);
  if(impl)
  {
    Internal::CustomActor* owner = impl->GetOwner();
    if(owner)
    {
      owner->RelayoutRequest();
    }
  }
}

float SizeNegotiatedActor::GetHeightForWidthBase(float width)
{
  CustomActorImpl* impl = dynamic_cast<CustomActorImpl*>(this);
  if(impl)
  {
    Internal::CustomActor* owner = impl->GetOwner();
    if(owner)
    {
      return owner->GetHeightForWidthBase(width);
    }
  }
  return 0.0f;
}

float SizeNegotiatedActor::GetWidthForHeightBase(float height)
{
  CustomActorImpl* impl = dynamic_cast<CustomActorImpl*>(this);
  if(impl)
  {
    Internal::CustomActor* owner = impl->GetOwner();
    if(owner)
    {
      return owner->GetWidthForHeightBase(height);
    }
  }
  return 0.0f;
}

float SizeNegotiatedActor::CalculateChildSizeBase(const Dali::Actor& child, Dimension::Type dimension)
{
  CustomActorImpl* impl = dynamic_cast<CustomActorImpl*>(this);
  if(impl)
  {
    Internal::CustomActor* owner = impl->GetOwner();
    if(owner)
    {
      return owner->CalculateChildSizeBase(child, dimension);
    }
  }
  return 0.0f;
}

bool SizeNegotiatedActor::RelayoutDependentOnChildrenBase(Dimension::Type dimension)
{
  CustomActorImpl* impl = dynamic_cast<CustomActorImpl*>(this);
  if(impl)
  {
    Internal::CustomActor* owner = impl->GetOwner();
    if(owner)
    {
      return owner->RelayoutDependentOnChildrenBase(dimension);
    }
  }
  return false;
}

} // namespace Dali
