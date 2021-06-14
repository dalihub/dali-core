/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/custom-actor-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/custom-actor-internal.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
CustomActor CustomActorImpl::Self() const
{
  return CustomActor(mOwner);
}

void CustomActorImpl::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
}

CustomActorImpl::CustomActorImpl(ActorFlags flags)
: mOwner(nullptr),
  mFlags(flags)
{
}

CustomActorImpl::~CustomActorImpl() = default;

void CustomActorImpl::Initialize(Internal::CustomActor& owner)
{
  DALI_ASSERT_DEBUG(mOwner == NULL); // should not already be owned

  mOwner = &owner;
}

Internal::CustomActor* CustomActorImpl::GetOwner() const
{
  return mOwner;
}

bool CustomActorImpl::IsRelayoutEnabled() const
{
  return (mFlags & DISABLE_SIZE_NEGOTIATION) == 0;
}

void CustomActorImpl::RelayoutRequest()
{
  mOwner->RelayoutRequest();
}

float CustomActorImpl::GetHeightForWidthBase(float width)
{
  return mOwner->GetHeightForWidthBase(width);
}

float CustomActorImpl::GetWidthForHeightBase(float height)
{
  return mOwner->GetWidthForHeightBase(height);
}

float CustomActorImpl::CalculateChildSizeBase(const Dali::Actor& child, Dimension::Type dimension)
{
  return mOwner->CalculateChildSizeBase(child, dimension);
}

bool CustomActorImpl::RelayoutDependentOnChildrenBase(Dimension::Type dimension)
{
  return mOwner->RelayoutDependentOnChildrenBase(dimension);
}

} // namespace Dali
